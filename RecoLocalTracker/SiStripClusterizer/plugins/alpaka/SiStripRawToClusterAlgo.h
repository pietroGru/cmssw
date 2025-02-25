#ifndef RecoLocalTracker_SiStripClusterizer_plugins_alpaka_SiStripRawToClusterAlgo_h
#define RecoLocalTracker_SiStripClusterizer_plugins_alpaka_SiStripRawToClusterAlgo_h

#include "CondFormats/SiStripObjects/interface/alpaka/SiStripClusterizerConditionsDevice.h"
#include "DataFormats/PortableTestObjects/interface/alpaka/TestDeviceCollection.h"
#include "DataFormats/PortableTestObjects/interface/alpaka/TestDeviceObject.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaTest/interface/alpaka/AlpakaESTestData.h"

#include "EventFilter/SiStripRawToDigi/interface/SiStripFEDBufferComponents.h"
#include "EventFilter/SiStripRawToDigi/interface/SiStripFEDBuffer.h"

#include "CondFormats/SiStripObjects/interface/alpaka/SiStripMappingDevice.h"

#include "HeterogeneousCore/AlpakaInterface/interface/memory.h"

#include "DataFormats/SiStripClusterSoA/interface/alpaka/SiStripClustersDevice.h"

namespace sistripclusterizer {
  GENERATE_SOA_LAYOUT(StripDataSoALayout,
    SOA_COLUMN(uint8_t, adc),
    SOA_COLUMN(uint16_t, channel),
    SOA_COLUMN(uint16_t, stripId),
    SOA_COLUMN(int, seedStripsMask_),
    SOA_COLUMN(int, prefixSeedStripsNCMask_),
    //
    SOA_SCALAR(float, channelThreshold_),
    SOA_SCALAR(float, seedThreshold_),
    SOA_SCALAR(float, clusterThresholdSquared_),
    SOA_SCALAR(uint8_t, maxSequentialHoles_),
    SOA_SCALAR(uint8_t, maxSequentialBad_),
    SOA_SCALAR(uint8_t, maxAdjacentBad_),
    SOA_SCALAR(uint32_t, maxClusterSize_),
    SOA_SCALAR(float, minGoodCharge_)
  )
  
  GENERATE_SOA_LAYOUT(StripDataAuxSoALayout,
    SOA_COLUMN(int, seedStripsMask_),
    SOA_COLUMN(int, prefixSeedStripsNCMask_)
  )

  using StripDataSoA = StripDataSoALayout<>;
  using StripDataView = StripDataSoA::View;
  using StripDataConstView = StripDataSoA::ConstView;

  using StripDataAuxSoA = StripDataAuxSoALayout<>;
  using StripDataAuxView = StripDataAuxSoA::View;
  using StripDataAuxConstView = StripDataAuxSoA::ConstView;

  using StripDataHost = PortableHostCollection2<StripDataSoA, StripDataAuxSoA>;
}

namespace ALPAKA_ACCELERATOR_NAMESPACE {
  using namespace sistripclusterizer;
  
  // PortableCollection-based model
  using StripDataHost = ::sistripclusterizer::StripDataHost;
  using StripDataDevice = PortableCollection2<StripDataSoA, StripDataAuxSoA>;
}


// check that the sistrip device collection for the host device is the same as the sistrip host collection
ASSERT_DEVICE_MATCHES_HOST_COLLECTION(StripDataDevice, sistripclusterizer::StripDataHost);





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




namespace ALPAKA_ACCELERATOR_NAMESPACE {
  class DataFedAppender {
    public:
      DataFedAppender(Queue& queue, unsigned int bufferBytesN)
          : bytes_{cms::alpakatools::make_host_buffer<uint8_t[]>(queue, bufferBytesN)},
            fedId_{cms::alpakatools::make_host_buffer<uint16_t[]>(queue, bufferBytesN)},
            fedIDinSet_(sistrip::FED_ID_MAX, false),
            size_(bufferBytesN/sizeof(uint8_t)) {};

      void initializeWordFed(int fedId, unsigned int prtOffset_bytes, const uint8_t* src, unsigned int length_bytes) {
        std::memcpy(bytes_.data() + prtOffset_bytes, src, length_bytes);
        std::memset(fedId_.data() + sizeof(uint16_t)*prtOffset_bytes, fedId - sistrip::FED_ID_MIN, sizeof(uint16_t) * length_bytes); // 2 = sizeof(uint16_t)/sizeof(uint8_t)
        chunkStartIdx_.emplace_back(prtOffset_bytes);
        fedIDinSet_[fedId] = true;
      }

      auto getData() const { return bytes_; }
      inline auto getFEDIdMask() const { return fedId_; }
      inline auto chunkStartIdx() const { return chunkStartIdx_; }
      bool isInside(unsigned int fedId) const {
        if (fedId > fedIDinSet_.size()){
          return false;
        }else{
          return fedIDinSet_[fedId];
        }
      }
      inline auto size() { return size_; }

    private:
      cms::alpakatools::host_buffer<uint8_t[]> bytes_;
      cms::alpakatools::host_buffer<uint16_t[]> fedId_;
      std::vector<unsigned int> chunkStartIdx_;
      std::vector<bool> fedIDinSet_;
      const size_t size_;
  };
} // namespace ALPAKA_ACCELERATOR_NAMESPACE



namespace ALPAKA_ACCELERATOR_NAMESPACE {
  class SiStripRawToClusterAlgo {
    public:
      void unpackStrips(Queue& queue, SiStripMappingDevice const& mapping, SiStripClusterizerConditionsDevice const& conditions, StripDataDevice& output) const;
      
      void fill(Queue& queue, portabletest::TestDeviceCollection& collection, double xvalue = 0.) const;
      void fillObject(Queue& queue, portabletest::TestDeviceObject& object, double x, double y, double z, int32_t id) const;
  
      portabletest::TestDeviceCollection update(Queue& queue, portabletest::TestDeviceCollection const& input, AlpakaESTestDataEDevice const& esData) const;
      portabletest::TestDeviceMultiCollection3 updateMulti3(Queue& queue, portabletest::TestDeviceMultiCollection3 const& input, AlpakaESTestDataEDevice const& esData) const;
  
      void fillMulti3(Queue& queue, portabletest::TestDeviceMultiCollection3& collection, double xvalue = 0.) const;
  
      void checkZero(Queue& queue, portabletest::TestDeviceCollection const& collection) const;
      void checkZero(Queue& queue, portabletest::TestDeviceMultiCollection3 const& collection) const;
      void checkZero(Queue& queue, portabletest::TestDeviceObject const& object) const;
  
      // SiStripClustersSoACollection void getClusters();
      void filterConditions(
        Queue& queue,
        const SiStripClusterizerConditionsDetToFedsConstView& cablingMapData,
        sistrip::FEDReadoutMode mode,
        DataFedAppender& fedBufferBlocksRaw_,
        const std::vector<std::unique_ptr<sistrip::FEDBuffer>>& buffers_,
        const DataFedAppender& dataFed,
        const SiStripMappingConstView& chanlocs
      ) const;
      // void unpackStrips(Queue& queue);
      // void makeClusters(Queue& queue);
  
      inline auto getClusters(){return std::move(clusters);};
      // makePhase1ClustersAsync(
      //   Queue &queue,
      //   const SiPixelMappingSoAConstView &cablingMap,
      //   const unsigned char *modToUnp,
      //   const SiPixelGainCalibrationForHLTSoAConstView &gains,
      //   const uint32_t wordCounter,
      //   const uint32_t fedCounter,
      //   bool useQualityInfo,
      //   bool includeErrors,
      //   bool debug)
    private:
      sistrip::SiStripClustersDevice clusters;
  };
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif  // RecoLocalTracker_SiStripClusterizer_plugins_alpaka_SiStripRawToClusterAlgo_h
