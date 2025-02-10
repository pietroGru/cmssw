#include "DataFormats/PortableTestObjects/interface/alpaka/TestDeviceCollection.h"
#include "DataFormats/TestObjects/interface/ToyProducts.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/stream/EDProducer.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/EDPutToken.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/ESGetToken.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaTest/interface/AlpakaESTestRecords.h"
#include "HeterogeneousCore/AlpakaTest/interface/AlpakaESTestData.h"

#include "TestAlgo.h"

#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "EventFilter/SiStripRawToDigi/interface/SiStripFEDBuffer.h"

#include "RecoLocalTracker/SiStripClusterizer/interface/StripClusterizerAlgorithmFactory.h"
#include "CalibFormats/SiStripObjects/interface/SiStripClusterizerConditions.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {
  // This makes the function resolvable only from this file
  namespace {
    std::unique_ptr<sistrip::FEDBuffer> fillBuffer(int fedId, const FEDRawData& rawData) {
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
  }
}

namespace ALPAKA_ACCELERATOR_NAMESPACE {
  /**
   * This class demonstrates a stream EDProducer that
   * - consumes a host EDProduct
   * - consumes a device ESProduct
   * - produces a device EDProduct (that gets transferred to host automatically if needed)
   * - optionally uses a product instance label
   */
  class SiStripRawToCluster : public stream::EDProducer<> {
  public:
    SiStripRawToCluster(edm::ParameterSet const& config);
    ~SiStripRawToCluster() override = default;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(device::Event& iEvent, device::EventSetup const& iSetup) override;
    

  private:
    edm::EDGetTokenT<edmtest::IntProduct> inputToken_;
    device::ESGetToken<cms::alpakatest::AlpakaESTestDataB<Device>, AlpakaESTestRecordB> conditionsToken_;
    device::EDPutToken<portabletest::TestDeviceCollection> outputToken_;
    
    const int32_t size_ = 0;
    const int32_t size2_ = 0;
    const int32_t size3_ = 0;

    // FED data structures for SiStrip raw data
    std::vector<std::unique_ptr<sistrip::FEDBuffer>> buffers_;
    std::vector<const FEDRawData*> raw_;

    // implementation of the algorithm
    TestAlgo algo_;

    void run(const FEDRawDataCollection& rawColl, const SiStripClusterizerConditions& conditions);
    void fill(uint32_t idet, const FEDRawDataCollection& rawColl, const SiStripClusterizerConditions& conditions);
  };


  SiStripRawToCluster::SiStripRawToCluster(const edm::ParameterSet& iConfig)
    : buffers_(sistrip::FED_ID_MAX),
      raw_(sistrip::FED_ID_MAX)
      {
        inputToken_ = consumes(iConfig.getParameter<edm::InputTag>("ProductLabel"));
        conditionsToken_ = esConsumes(edm::ESInputTag{"", iConfig.getParameter<std::string>("ConditionsLabel")});
        outputToken_ = produces();
        // devicePutTokenMulti2_ = produces(iConfig.getParameter<std::string>("productInstanceName"));
        // devicePutTokenMulti3_ = produces(iConfig.getParameter<std::string>("productInstanceName"));
        }

  void SiStripRawToCluster::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    // TODO: add doc 
    desc.add("ProductLabel", edm::InputTag("rawDataCollector"));
    desc.add<std::string>("ConditionsLabel", "");
    // Call the fillDescriptions from the clusterizer algo factory, 
    // and add the Clusterizer settings to the clusterizer class
    edm::ParameterSetDescription clusterizer;
    StripClusterizerAlgorithmFactory::fillDescriptions(clusterizer);
    desc.add("Clusterizer", clusterizer);

    // Is this part relevant in any way?
    // edm::ParameterSetDescription psetSize;
    // psetSize.add<int32_t>("alpaka_serial_sync");
    // psetSize.add<int32_t>("alpaka_cuda_async");
    // psetSize.add<int32_t>("alpaka_rocm_async");
    // desc.add("size", psetSize);

    descriptions.addWithDefaultLabel(desc);
  }
      
  void SiStripRawToCluster::produce(device::Event& iEvent, device::EventSetup const& iSetup) override {
    [[maybe_unused]] auto inpData = iEvent.getHandle(inputToken_);
    [[maybe_unused]] auto const& esData = iSetup.getData(conditionsToken_);

    auto deviceProduct = std::make_unique<portabletest::TestDeviceCollection>(size_, iEvent.queue());
    auto deviceProductMulti2 = std::make_unique<portabletest::TestDeviceMultiCollection2>(
        portabletest::TestDeviceMultiCollection2::SizesArray{{size_, size2_}}, iEvent.queue());
    auto deviceProductMulti3 = std::make_unique<portabletest::TestDeviceMultiCollection3>(
        portabletest::TestDeviceMultiCollection3::SizesArray{{size_, size2_, size3_}}, iEvent.queue());

    // run the algorithm, potentially asynchronously
    algo_.fill(iEvent.queue(), *deviceProduct);
    algo_.fillMulti2(iEvent.queue(), *deviceProductMulti2);
    algo_.fillMulti3(iEvent.queue(), *deviceProductMulti3);

    iEvent.put(outputToken_, std::move(deviceProduct));
    iEvent.put(devicePutTokenMulti2_, std::move(deviceProductMulti2));
    iEvent.put(devicePutTokenMulti3_, std::move(deviceProductMulti3));
  }

  void SiStripRawToCluster::run(const FEDRawDataCollection& rawColl, const SiStripClusterizerConditions& conditions) {
  // loop over good det in cabling
    for (auto idet : conditions.allDetIds()) {
    fill(idet, rawColl, conditions);
    }  // end loop over dets
  }

  void SiStripRawToCluster::fill(uint32_t idet, const FEDRawDataCollection& rawColl, const SiStripClusterizerConditions& conditions) {
    auto const& det = conditions.findDetId(idet);
    if (!det.valid())
    return;

    // Loop over apv-pairs of det
    for (auto const conn : conditions.currentConnection(det)) {
      if UNLIKELY (!conn)
      continue;

      const uint16_t fedId = conn->fedId();

      // If fed id is null or connection is invalid continue
      if UNLIKELY (!fedId || !conn->isConnected()) {
      continue;
      }

      // If Fed hasnt already been initialised, extract data and initialise
      sistrip::FEDBuffer* buffer = buffers_[fedId].get();
      if (!buffer) {
      const FEDRawData& rawData = rawColl.FEDData(fedId);
      raw_[fedId] = &rawData;
      buffers_[fedId] = fillBuffer(fedId, rawData);
      }
    }  // end loop over conn
  }


}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#include "HeterogeneousCore/AlpakaCore/interface/alpaka/MakerMacros.h"
DEFINE_FWK_ALPAKA_MODULE(SiStripRawToCluster);
