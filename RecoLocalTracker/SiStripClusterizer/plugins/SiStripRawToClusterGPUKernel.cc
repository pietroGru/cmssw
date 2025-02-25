#include "EventFilter/SiStripRawToDigi/interface/SiStripFEDBuffer.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "HeterogeneousCore/CUDAUtilities/interface/host_unique_ptr.h"
#include "HeterogeneousCore/CUDAUtilities/interface/copyAsync.h"
#include "RecoLocalTracker/SiStripClusterizer/interface/ClusterChargeCut.h"

#include "SiStripRawToClusterGPUKernel.h"

#include "CUDADataFormats/SiStripCluster/interface/SiStripClustersSoADevice.h"

#include "CalibFormats/SiStripObjects/interface/SiStripClusterizerConditionsGPU.h"
#include "ChannelLocsGPU.h"
#include "StripDataView.h"

namespace stripgpu {
  StripDataGPU::StripDataGPU(size_t size, cudaStream_t stream) {
    alldataGPU_ = cms::cuda::make_device_unique<uint8_t[]>(size, stream);
    channelGPU_ = cms::cuda::make_device_unique<uint16_t[]>(size, stream);
    stripIdGPU_ = cms::cuda::make_device_unique<stripgpu::stripId_t[]>(size, stream);
  }

  SiStripRawToClusterGPUKernel::SiStripRawToClusterGPUKernel(const edm::ParameterSet& conf)
      : fedIndex_(sistrip::NUMBER_OF_FEDS, stripgpu::invalidFed),
        channelThreshold_(conf.getParameter<double>("ChannelThreshold")),
        seedThreshold_(conf.getParameter<double>("SeedThreshold")),
        clusterThresholdSquared_(std::pow(conf.getParameter<double>("ClusterThreshold"), 2.0f)),
        maxSequentialHoles_(conf.getParameter<unsigned>("MaxSequentialHoles")),
        maxSequentialBad_(conf.getParameter<unsigned>("MaxSequentialBad")),
        maxAdjacentBad_(conf.getParameter<unsigned>("MaxAdjacentBad")),
        maxClusterSize_(conf.getParameter<unsigned>("MaxClusterSize")),
        minGoodCharge_(clusterChargeCut(conf)) {
    fedRawDataOffsets_.reserve(sistrip::NUMBER_OF_FEDS);
  }

  void SiStripRawToClusterGPUKernel::makeAsync(const std::vector<const FEDRawData*>& rawdata,
                                               const std::vector<std::unique_ptr<sistrip::FEDBuffer>>& buffers,
                                               const SiStripClusterizerConditionsGPU& conditions,
                                               cudaStream_t stream) {
    //@pietroGru Compute Total Memory Needed
    size_t totalSize{0};
    for (const auto& buff : buffers) {
      if (buff != nullptr) {
        totalSize += buff->bufferSize();
      }
    }

    //@pietroGru Allocate Host & Device Memory for Raw Data
    auto fedRawDataHost = cms::cuda::make_host_unique<uint8_t[]>(totalSize, stream);
    auto fedRawDataGPU = cms::cuda::make_device_unique<uint8_t[]>(totalSize, stream);

    //@pietroGru Prepare FED Offsets and Indexing
    size_t off = 0;
    fedRawDataOffsets_.clear();
    fedIndex_.clear();
    fedIndex_.resize(sistrip::NUMBER_OF_FEDS, stripgpu::invalidFed);

    //@pietroGru Copy FED Data to Host Memory
    sistrip::FEDReadoutMode mode = sistrip::READOUT_MODE_INVALID;
    for (size_t fedi = 0; fedi < buffers.size(); ++fedi) {
      auto& buff = buffers[fedi];
      if (buff != nullptr) {
        const auto raw = rawdata[fedi];
        memcpy(fedRawDataHost.get() + off, raw->data(), raw->size());
        fedIndex_[stripgpu::fedIndex(fedi)] = fedRawDataOffsets_.size();
        fedRawDataOffsets_.push_back(off);
        off += raw->size();
        if (fedRawDataOffsets_.size() == 1) {
          mode = buff->readoutMode();
        } else {
          if (buff->readoutMode() != mode) {
            throw cms::Exception("[SiStripRawToClusterGPUKernel] inconsistent readout mode ")
                << buff->readoutMode() << " != " << mode;
          }
        }
      }
    }

    //@pietroGru Copy FED Data to the GPU (send rawdata to GPU)
    //           copy the entire block of raw FED data from host. At this point, the data is still "raw", meaning it hasn’t been unpacked yet. It's just stored in GPU memory.
    cms::cuda::copyAsync(fedRawDataGPU, fedRawDataHost, totalSize, stream);

    //@pietroGru Verify Readout Mode & Determine Header Length
    //            preparing the data for the unpack - THIS part essentially generates the A-B map between
    //            A - the raw FED data (a bug chunk of bytes which were copied to the GPU in the previous line)
    //            B - the actual strip detector elements
    const auto& detmap = conditions.detToFeds();
    if ((mode != sistrip::READOUT_MODE_ZERO_SUPPRESSED) && (mode != sistrip::READOUT_MODE_ZERO_SUPPRESSED_LITE10)) {
      throw cms::Exception("[SiStripRawToClusterGPUKernel] unsupported readout mode ") << mode;
    }
    const uint16_t headerlen = mode == sistrip::READOUT_MODE_ZERO_SUPPRESSED ? 7 : 2;
    
    //@pietroGru Allocate Channel Locations & Input Data Pointers
    //@pietroGru prepare the pointers for ChannelLocs and the memory content for chanlocs payload 
    auto chanlocs = std::make_unique<ChannelLocs>(detmap.size(), stream);
    auto inputGPU = cms::cuda::make_host_unique<const uint8_t*[]>(chanlocs->size(), stream);
    
    //@pietroGru Map Detector Channels to FED Data, by expanding the gpuConditions
    //           iterate over the detector in DetID/APVPair order
    //           mapping out where the data are
    size_t offset = 0;
    for (size_t i = 0; i < detmap.size(); ++i) {
      const auto& detp = detmap[i];
      const auto fedId = detp.fedID();
      const auto fedCh = detp.fedCh();
      const auto fedi = fedIndex_[stripgpu::fedIndex(fedId)];

      if (fedi != invalidFed) {
        const auto buffer = buffers[fedId].get();
        const auto& channel = buffer->channel(detp.fedCh());

        auto len = channel.length();
        auto off = channel.offset();

        assert(len >= headerlen || len == 0);

        if (len >= headerlen) {
          len -= headerlen;
          off += headerlen;
        }

        //@pietroGru saving the channel location (association between detector channels and fed id) in chanlocs
        chanlocs->setChannelLoc(i, channel.data(), off, offset, len, fedId, fedCh, detp.detID());
        //@pietroGru calculate the correct pointer inside the fedRawDataGPU where the fed's channel data begins
        inputGPU[i] = fedRawDataGPU.get() + fedRawDataOffsets_[fedi] + (channel.data() - rawdata[fedId]->data());
        offset += len;

      } else {
        chanlocs->setChannelLoc(i, nullptr, 0, 0, 0, invalidFed, 0, invalidDet);
        inputGPU[i] = nullptr;
      }
    }

    //@pietroGru Prepare GPU Data Structures
    //           the main catch here is that I have chanlocs and inputGPU which were prepared on host
    const auto n_strips = offset;
    //@pietroGru Create a view pointer (which here is a plain struct*) to access the StripData content
    sst_data_d_ = cms::cuda::make_host_unique<StripDataView>(stream);
    sst_data_d_->nStrips = n_strips;

    //@pietroGru same for the ChannelLocsGPU
    chanlocsGPU_ = std::make_unique<ChannelLocsGPU>(detmap.size(), stream);
    //@pietroGru MOVE raw data to the GPU
    chanlocsGPU_->setVals(chanlocs.get(), std::move(inputGPU), stream);
    
    //@pietroGru StripDataGPU is a plain class to store the data pointers in gpu memory space
    stripdata_ = std::make_unique<StripDataGPU>(n_strips, stream);

    //@pietroGru Fetch Calibration Conditions
    //           fetches condition data (e.g., calibration constants, noise thresholds, etc.)
    //           and make them available on the GPU
    const auto& condGPU = conditions.getGPUProductAsync(stream);
    
    //@pietroGru Launch the GPU Kernel
    //           the view for the conditions goes into theunpackChannelsGPU cuda-kernel
    unpackChannelsGPU(condGPU.deviceView(), stream);

#ifdef GPU_CHECK
    cudaCheck(cudaStreamSynchronize(stream));
#endif

#ifdef EDM_ML_DEBUG
    auto outdata = cms::cuda::make_host_unique<uint8_t[]>(n_strips, stream);
    cms::cuda::copyAsync(outdata, stripdata_->alldataGPU_, n_strips, stream);
    cudaCheck(cudaStreamSynchronize(stream));

    constexpr int xor3bits = 7;
    for (size_t i = 0; i < chanlocs->size(); ++i) {
      const auto data = chanlocs->input(i);
      const auto len = chanlocs->length(i);

      if (data != nullptr && len > 0) {
        auto aoff = chanlocs->offset(i);
        auto choff = chanlocs->inoff(i);
        const auto end = choff + len;

        while (choff < end) {
          const auto stripIndex = data[choff++ ^ xor3bits];
          const auto groupLength = data[choff++ ^ xor3bits];
          aoff += 2;
          for (auto k = 0; k < groupLength; ++k, ++choff, ++aoff) {
            if (data[choff ^ xor3bits] != outdata[aoff]) {
              LogDebug("SiStripRawToClusterGPUKernel")
                  << "Strip mismatch " << stripIndex << " i:k " << i << ":" << k << " "
                  << (uint32_t)data[choff ^ xor3bits] << " != " << (uint32_t)outdata[aoff] << std::endl;
            }
          }
        }
      }
    }
    outdata.reset(nullptr);
#endif

    //@pietroGru not sure, most likely at this point of the queue, the kernel already unpacked the strips
    //           then I can free the memory associated with the FED raw data (hence the nullptr set)
    fedRawDataGPU.reset();
    
    //@pietroGru basically fill the view for the data in the host and then copy to the gpu memory space
    allocateSSTDataGPU(n_strips, stream);

    //@pietroGru seeding setup means set the noise_i*seedThreshold mask for all the strips
    //           (it masks strips with a meaningful hits) in setSeedStripsGPU. Also in setNCSeedStripsGPU it masks out the not connected strips
    setSeedStripsNCIndexGPU(condGPU.deviceView(), stream);

    //@pietroGru This just prepare the SiStripClustersSoA result object and pushed this in the stream
    clusters_d_ = SiStripClustersSoADevice(kMaxSeedStrips, stream);
    //@pietroGru this function runs the actual clustering algorithm
    findClusterGPU(condGPU.deviceView(), stream);

    //@pietroGru calling the reset of the smart pointer frees the memory allocated for the stripdata_ objects
    stripdata_.reset();
  }

  SiStripClustersSoADevice SiStripRawToClusterGPUKernel::getResults(cudaStream_t stream) {
    reset();

    return std::move(clusters_d_);
  }

  void SiStripRawToClusterGPUKernel::reset() {
    chanlocsGPU_.reset();
    sst_data_d_.reset();
  }
}  // namespace stripgpu
