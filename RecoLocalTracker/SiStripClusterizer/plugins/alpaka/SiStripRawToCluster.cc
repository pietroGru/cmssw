#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "HeterogeneousCore/AlpakaCore/interface/alpaka/stream/SynchronizingEDProducer.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/EDPutToken.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/ESGetToken.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/memory.h"

#include "EventFilter/SiStripRawToDigi/interface/SiStripFEDBuffer.h"
#include "RecoLocalTracker/SiStripClusterizer/interface/StripClusterizerAlgorithmFactory.h"

#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/SiStripClusterSoA/interface/alpaka/SiStripClustersDevice.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"

#include "RecoLocalTracker/Records/interface/SiStripClusterizerConditionsRcd.h"
#include "CalibFormats/SiStripObjects/interface/SiStripClusterizerConditions.h"
#include "CondFormats/SiStripObjects/interface/SiStripClusterizerConditionsSoA.h"
#include "CondFormats/SiStripObjects/interface/SiStripClusterizerConditionsHost.h"

#include "CondFormats/SiStripObjects/interface/alpaka/SiStripMappingDevice.h"

#include "SiStripRawToClusterAlgo.h"

#include "RecoLocalTracker/SiStripClusterizer/plugins/alpaka/ChannelLocsDevice.h"

// Alpaka includes
#include <alpaka/alpaka.hpp>


namespace ALPAKA_ACCELERATOR_NAMESPACE {
  // This makes the function resolvable only from this file
  namespace {
    std::unique_ptr<sistrip::FEDBuffer> fillBuffer(int fedId, const FEDRawData& rawData)
    {
      std::unique_ptr<sistrip::FEDBuffer> buffer;

      // Check on FEDRawData pointer
      const auto st_buffer = sistrip::preconstructCheckFEDBuffer(rawData);
      if UNLIKELY (sistrip::FEDBufferStatusCode::SUCCESS != st_buffer) {
        LogDebug(sistrip::mlRawToCluster_) << "[ClustersFromRawProducer::" << __func__ << "]" << st_buffer << " for FED ID " << fedId;
        return buffer;
      }
      
      buffer = std::make_unique<sistrip::FEDBuffer>(rawData);
      const auto st_chan = buffer->findChannels();
      
      if UNLIKELY (sistrip::FEDBufferStatusCode::SUCCESS != st_chan) {
        LogDebug(sistrip::mlRawToCluster_) << "Exception caught when creating FEDBuffer object for FED " << fedId << ": " << st_chan;
        buffer.reset();
        return buffer;
      }
      
      if UNLIKELY (!buffer->doChecks(false)) {
        LogDebug(sistrip::mlRawToCluster_) << "Exception caught when creating FEDBuffer object for FED " << fedId << ": FED Buffer check fails";
        buffer.reset();
        return buffer;
      }

      return buffer;
    }

    // inline uint16_t fedIndex(uint16_t fed) { return fed - sistrip::FED_ID_MIN; }
    // inline uint32_t stripIndex(uint16_t fed, uint8_t channel, uint16_t strip) { return fedIndex(fed) * sistrip::FEDCH_PER_FED * sistrip::STRIPS_PER_FEDCH + channel * sistrip::STRIPS_PER_FEDCH + (strip % sistrip::STRIPS_PER_FEDCH); }
    // inline uint32_t apvIndex(uint16_t fed, uint8_t channel, uint16_t strip) { return fedIndex(fed) * sistrip::APVS_PER_FEDCH * sistrip::FEDCH_PER_FED + sistrip::APVS_PER_CHAN * channel + (strip % sistrip::STRIPS_PER_FEDCH) / sistrip::STRIPS_PER_APV; }
    // inline uint32_t channelIndex(uint16_t fed, uint8_t channel) { return fedIndex(fed) * sistrip::FEDCH_PER_FED + channel; }
  }
}

namespace ALPAKA_ACCELERATOR_NAMESPACE {
  class SiStripRawToCluster : public stream::SynchronizingEDProducer<> {
  public:
    SiStripRawToCluster(edm::ParameterSet const& iConfig);
    ~SiStripRawToCluster() = default;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);    
    void acquire(device::Event const& iEvent, device::EventSetup const& iSetup) override;
    void produce(device::Event& iEvent, device::EventSetup const& iSetup) override;
    

  private:
    // inputs
    edm::EDGetTokenT<FEDRawDataCollection> fedRawDataToken_;
    edm::ESGetToken<SiStripClusterizerConditions, SiStripClusterizerConditionsRcd> siStripConditionsToken_;
    edm::ESGetToken<SiStripClusterizerConditionsHost, SiStripClusterizerConditionsRcd> siStripCablingConditionsToken_;    
    //
    // outputs
    device::EDPutToken<sistrip::SiStripClustersDevice> siStripClustersDeviceToken_;
    edm::EDPutTokenT<edmNew::DetSetVector<SiStripCluster>> siStripClustersSetVecToken_;

    
    // FED buffers data and functions
    std::vector<const FEDRawData*> raw_;
    std::vector<std::unique_ptr<sistrip::FEDBuffer>> buffers_;
    void makeFEDbufferWithValidFEDs_(const FEDRawDataCollection& rawColl, const SiStripClusterizerConditions& conditions);
    void makeFEDbufferWithValidFEDs_4det_(uint32_t idet, const FEDRawDataCollection& rawColl, const SiStripClusterizerConditions& conditions);
    size_t buffersValidBytesSize_{0};

    std::vector<uint32_t> fedRawDataOffsets_;
    std::vector<uint16_t> fedIndex_;

    // implementation of the algorithm
    SiStripRawToClusterAlgo algo_;

  };


  SiStripRawToCluster::SiStripRawToCluster(const edm::ParameterSet& iConfig)
  : raw_(sistrip::FED_ID_MAX), buffers_(sistrip::FED_ID_MAX),
    fedIndex_(sistrip::NUMBER_OF_FEDS, std::numeric_limits<uint16_t>::max())
    {
      fedRawDataToken_ = consumes(iConfig.getParameter<edm::InputTag>("ProductLabel"));
      siStripConditionsToken_ = esConsumes(edm::ESInputTag{"", iConfig.getParameter<std::string>("ConditionsLabel")});
      siStripCablingConditionsToken_ = esConsumes(edm::ESInputTag{"", iConfig.getParameter<std::string>("CablingConditionsLabel")});
      // clusterPutToken_ = produces();
      // devicePutTokenMulti2_ = produces(iConfig.getParameter<std::string>("productInstanceName"));
      // devicePutTokenMulti3_ = produces(iConfig.getParameter<std::string>("productInstanceName"));
      fedRawDataOffsets_.reserve(sistrip::NUMBER_OF_FEDS);
    }

  void SiStripRawToCluster::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    // Add some custom parameter description to the automatically-created ones by the addWithDefaultLabel methos
    edm::ParameterSetDescription desc;
    desc.add("ProductLabel", edm::InputTag("rawDataCollector"));
    desc.add<std::string>("ConditionsLabel", "");
    desc.add<std::string>("CablingConditionsLabel", "");
    
    // Call the fillDescriptions from the clusterizer algo factory, and add the Clusterizer settings to the clusterizer class
    edm::ParameterSetDescription clusterizer;
    StripClusterizerAlgorithmFactory::fillDescriptions(clusterizer);
    desc.add("Clusterizer", clusterizer);
    
    descriptions.addWithDefaultLabel(desc);
  }
  
  void SiStripRawToCluster::acquire(device::Event const& iEvent, device::EventSetup const& iSetup) {
    // Get data from the tokens (raw from fed and conditions)
    const auto& rawCollection = iEvent.get(fedRawDataToken_);
    const auto& validFEDsConditions = iSetup.getData(siStripConditionsToken_);
    
    // Fill the raw_, buffers_ class members (i.e., from connected FED the FEDBuffers (and raw pointers) are populated)
    // [more precisely, I have the pointers of the raw_ and buffers_ pointing to valid data from the rawCollection]
    makeFEDbufferWithValidFEDs_(rawCollection, validFEDsConditions);
    // raw_, buffers_ 


    // Create pinned host and device memory
    // fedBufferBlocksRawHost_ is made of the data and a mask (fedID) telling at each array position in data to which fedID it corresponds
    DataFedAppender fedBufferBlocksRaw_(iEvent.queue(), buffersValidBytesSize_);
    // fill the raw bytes with the buffers and store the index where a corresponding FED buffer starts
    // bytes_         = | bytes relative to a given FED id                                     | ... |
    // fedId_         = |< same size array filled with fedId_ for the whole bytes_[fed] size  >| ... |
    // chunkStartIdx_ = [0, (fedID_1 stopIdx=)fedID_2 startIdx in bytes_, (fedID_2 stopIdx=)fedID_3 startIdx in bytes_, ... ]
    // fedIDinSet_    = [2, 5, 7, ...] (list of the fedID passing the conditions)
    size_t off = 0;
    sistrip::FEDReadoutMode mode = sistrip::READOUT_MODE_INVALID;
    for (size_t fedID = sistrip::FED_ID_MIN; fedID < buffers_.size(); ++fedID) {
      auto& buff = buffers_[fedID];
      // loop over valid buffers instead of if else is to avoid long jump in assembly
      if (!buff) continue;
      
      // Get the valid raw data
      const auto raw = raw_[fedID];
      fedBufferBlocksRaw_.initializeWordFed(fedID, off, raw->data(), raw->size());

      if(off==0){
        mode = buff->readoutMode();
        if(mode==sistrip::READOUT_MODE_INVALID) throw cms::Exception("[SiStripRawToCluster] Invalid readout mode for first \"supposedly valid\" buffer");
      }else if(buff->readoutMode() != mode){
        throw cms::Exception("[SiStripRawToCluster] inconsistent readout mode ") << buff->readoutMode() << " != " << mode;
      }
      // Update the position of the pointer
      off += raw->size();
    }
    
    // Verify Readout Mode
    if ((mode != sistrip::READOUT_MODE_ZERO_SUPPRESSED) && (mode != sistrip::READOUT_MODE_ZERO_SUPPRESSED_LITE10)) throw cms::Exception("[SiStripRawToClusterGPUKernel] unsupported readout mode ") << mode;
    

    //@pietroGru Allocate Channel Locations & Input Data Pointers
    //@pietroGru prepare the pointers for ChannelLocs and the memory content for chanlocs payload 
    // Get the cabling map
    const auto& cablingMapData = iSetup.getData(siStripCablingConditionsToken_);
    auto detToFedsMap = cablingMapData.view<SiStripClusterizerConditionsDetToFedsSoA>();
    auto detToFedsMap_size = detToFedsMap.metadata().size();
    
    // Make a portable collection of the mapping on the host
    auto chanlocs = SiStripMappingHost(detToFedsMap.metadata().size(), iEvent.queue());
    //@pietroGru This object contains the addresses of the input raw data for channel
    auto dataFed_onDevice = chanlocs.view().input();
    
    // Copy the dataFed data to the buffers
    auto fedBufferBlocksRaw_onDevice = cms::alpakatools::make_device_buffer<uint8_t[]>(iEvent.queue(), static_cast<unsigned int>(fedBufferBlocksRaw_.size()));
    //@pietroGru Copy FED Data to Host Memory
    alpaka::memcpy(iEvent.queue(), fedBufferBlocksRaw_onDevice, fedBufferBlocksRaw_.getData(), static_cast<unsigned int>(fedBufferBlocksRaw_.size()));

    //@pietroGru Map Detector Channels to FED Data
    //           iterate over the detector in DetID/APVPair order
    //           mapping out where the data are
    static constexpr uint32_t invalidDet = std::numeric_limits<uint32_t>::max();
    static constexpr uint16_t invalidFed = std::numeric_limits<uint16_t>::max();
    // static constexpr uint16_t invalidStrip = std::numeric_limits<uint16_t>::max();
    
    //@pietroGru Expand the SiStripClusterizerConditionsDetToFedsSoA to mask the fed buffers according to "good" detectors
    //            preparing the data for the unpack - THIS part essentially generates the A-B map between
    //            A - the raw FED data (a bug chunk of bytes which were copied to the GPU in the previous line)
    //            B - the actual fed buffers, which matters for unpacking, selecting (possibly a subset of) the detectors
    const uint16_t headerlen = (mode == sistrip::READOUT_MODE_ZERO_SUPPRESSED ? 7 : 2);
    uint32_t offset = 0;
    unsigned int fedCounter = 0;
    for (int i = 0; i < detToFedsMap_size; ++i) {
      const auto& detp = detToFedsMap[i];
      const auto fedId = detp.fedid_();
      if(fedBufferBlocksRaw_.isInside(fedId)){
        const auto buffer = buffers_[fedId].get();
        
        const auto fedCh = detp.fedch_();
        const auto& fedCh_object = buffer->channel(fedCh);
        
        auto len = fedCh_object.length();
        auto off = fedCh_object.offset();
        
        assert(len >= headerlen || len == 0);
        
        if (len >= headerlen) {
          len -= headerlen;
          off += headerlen;
          }
          
        //@pietroGru saving the channel location (association between detector channels and fed id) in chanlocs
        // chanlocs.view()[i] = {fedCh_object.data(), off, offset, len, fedId, fedCh, detp.detid_()};
        chanlocs->inoff(i) = off;
        chanlocs->offset(i) = offset;
        chanlocs->length(i) = len;
        chanlocs->fedID(i) = fedId;
        chanlocs->fedCh(i) = fedCh;
        //@pietroGru calculate the correct pointer inside the fedRawDataGPU where the detector's channel begins
        dataFed_onDevice[i] = 
          fedBufferBlocksRaw_onDevice.data()                                                          // this pointer is in the device memory
          + fedBufferBlocksRaw_.chunkStartIdx()[fedCounter]                                           // this is just an offset - same in host/dev mem
          + reinterpret_cast<uintptr_t>(fedCh_object.data() - fedBufferBlocksRaw_.getData()[fedId]);  // this is the offset between the pointer where the channel data begins and the position of the data as a whole start (this difference is the same in host/device, and this is calculated on host - since both things are in host memory already)
        offset += len;
        ++fedCounter;
      } else {
        chanlocs.view()[i] = {nullptr, 0, 0, 0, invalidFed, 0, invalidDet};
        dataFed_onDevice[i] = nullptr;
      }
    }

    //@pietroGru Fetch Calibration Conditions
    //           fetches condition data (e.g., calibration constants, noise thresholds, etc.)
    //           and make them available on the GPU
    auto clusterizerConditions_device = SiStripClusterizerConditionsDevice(cablingMapData.sizes(), iEvent.queue());
    alpaka::memcpy(iEvent.queue(), clusterizerConditions_device.buffer(), cablingMapData.const_buffer());
   
    
    auto chanlocs_device = SiStripMappingDevice(chanlocs->metadata().size(), iEvent.queue());
    alpaka::memcpy(iEvent.queue(), chanlocs_device.buffer(), chanlocs.const_buffer());

    // auto obk = cms::alpakatools::CopyToDevice<SiStripMappingHost>::copyAsync(iEvent.queue(), chanlocs);

    //@pietroGru Prepare GPU Data Structures
    //           the main catch here is that I have chanlocs and inputGPU which were prepared on host
    const int n_strips = offset;
    auto unpackedStrips = StripDataDevice({{n_strips, 2*n_strips}}, iEvent.queue());

    algo_.unpackStrips(
      iEvent.queue(),
      chanlocs_device,
      clusterizerConditions_device,
      unpackedStrips);

    alpaka::wait(iEvent.queue());
    std::cout << "RawToDigi_kernel was run smoothly!" << std::endl;    

    //// load conditions
    // fedBufferBlocksRaw_onDevice
    // dataFed_onDevice_ptrs

    // algo_.filterConditions(
    //   iEvent.queue(),
    //   detToFedsMap,
    //   fedBufferBlocksRaw_,
    // dataFed_onDeviceView
    // );

    
    /*
    StripDataView è fatto da:
    1. ciò che io ho in SiStripMappingHost;
    2. il contenuto dei raw data che io ho in inputGPU assieme
    3. alla mappa dei canali (che sta in chanlocs);
    4. la mappa dei stripId, che è contenuta all interno delle ClusterizerConditionsDevice (condGPU nella versione cuda);
    5. dello spazio da allocare per seedStripMask, prefixSeedStripsNCMask, seedStripsNCMask, seedStripcNCIndex
    Una parte di questa roba sta già sul device (tipo le conditions), un'altra parte invece sta sull'host (tipo i raw data) e infine un'ultima parte
    deve essere allocata (seedStripsMask)
    */
  }

  //
  void SiStripRawToCluster::produce(device::Event& iEvent, device::EventSetup const& iSetup) {
    iEvent.emplace(siStripClustersDeviceToken_, algo_.getClusters());
  }

  void SiStripRawToCluster::makeFEDbufferWithValidFEDs_(const FEDRawDataCollection& rawColl, const SiStripClusterizerConditions& conditions) {
  // loop over good det in cabling
    for (auto idet : conditions.allDetIds()) {
      makeFEDbufferWithValidFEDs_4det_(idet, rawColl, conditions); // it populates raw_, buffers_ with only connected fed
    }  // end loop over dets
  }

  void SiStripRawToCluster::makeFEDbufferWithValidFEDs_4det_(uint32_t idet, const FEDRawDataCollection& rawColl, const SiStripClusterizerConditions& conditions) {
    auto const& det = conditions.findDetId(idet);
    if (!det.valid()) return;

    // Loop over apv-pairs of det
    for (auto const conn : conditions.currentConnection(det)) {
      if UNLIKELY (!conn) continue;

      // If fed id is null or connection is invalid continue
      const uint16_t fedId = conn->fedId();
      if UNLIKELY (!fedId || !conn->isConnected()) continue;

      // If Fed hasnt already been initialised, extract data and initialise
      sistrip::FEDBuffer* buffer = buffers_[fedId].get();
      if (!buffer) {
        const FEDRawData& rawData = rawColl.FEDData(fedId);
        raw_[fedId] = &rawData;
        // the fillBuffer function checks for header/trailer and validate the data packet,
        // if the data is invalid, a nullptr is returned
        buffers_[fedId] = fillBuffer(fedId, rawData);
        
        // Count the total number of bytes I need for the pinned memory which is going to hold this
        if(buffers_[fedId]) buffersValidBytesSize_ += buffers_[fedId]->bufferSize();
      }
    }  // end loop over conn
  }
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#include "HeterogeneousCore/AlpakaCore/interface/alpaka/MakerMacros.h"
DEFINE_FWK_ALPAKA_MODULE(SiStripRawToCluster);