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

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
      edm::ParameterSetDescription desc;
      desc.add<edm::InputTag>("source");
      desc.add("eventSetupSource", edm::ESInputTag{});
      desc.add<std::string>("productInstanceName", "");

      edm::ParameterSetDescription psetSize;
      psetSize.add<int32_t>("alpaka_serial_sync");
      psetSize.add<int32_t>("alpaka_cuda_async");
      psetSize.add<int32_t>("alpaka_rocm_async");
      desc.add("size", psetSize);

      descriptions.addWithDefaultLabel(desc);
    }

    void produce(device::Event& iEvent, device::EventSetup const& iSetup) override {
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

      iEvent.put(devicePutToken_, std::move(deviceProduct));
      iEvent.put(devicePutTokenMulti2_, std::move(deviceProductMulti2));
      iEvent.put(devicePutTokenMulti3_, std::move(deviceProductMulti3));
    }



  private:
    edm::EDGetTokenT<edmtest::IntProduct> inputToken_;
    device::ESGetToken<cms::alpakatest::AlpakaESTestDataB<Device>, AlpakaESTestRecordB> conditionsToken_;
    device::EDPutToken<portabletest::TestDeviceCollection> devicePutToken_;
    device::EDPutToken<portabletest::TestDeviceMultiCollection2> devicePutTokenMulti2_;
    device::EDPutToken<portabletest::TestDeviceMultiCollection3> devicePutTokenMulti3_;
    
    const int32_t size_;
    const int32_t size2_;
    const int32_t size3_;

    // FED data structures for SiStrip raw data
    std::vector<std::unique_ptr<sistrip::FEDBuffer>> buffers_;
    std::vector<const FEDRawData*> raw_;

    // implementation of the algorithm
    TestAlgo algo_;
  };

  SiStripRawToCluster::SiStripRawToCluster(const edm::ParameterSet& iConfig)
    : size_(iConfig.getParameter<edm::ParameterSet>("size").getParameter<int32_t>(EDM_STRINGIZE(ALPAKA_ACCELERATOR_NAMESPACE))),
      size2_(iConfig.getParameter<edm::ParameterSet>("size").getParameter<int32_t>(EDM_STRINGIZE(ALPAKA_ACCELERATOR_NAMESPACE))),
      size3_(iConfig.getParameter<edm::ParameterSet>("size").getParameter<int32_t>(EDM_STRINGIZE(ALPAKA_ACCELERATOR_NAMESPACE))),
      buffers_(sistrip::FED_ID_MAX),
      raw_(sistrip::FED_ID_MAX)
      {
        inputToken_ = consumes(iConfig.getParameter<edm::InputTag>("ProductLabel"));
        conditionsToken_ = esConsumes(edm::ESInputTag{"", iConfig.getParameter<std::string>("ConditionsLabel")});
        devicePutToken_ = produces();
        devicePutTokenMulti2_ = produces(iConfig.getParameter<std::string>("productInstanceName"));
        devicePutTokenMulti3_ = produces(iConfig.getParameter<std::string>("productInstanceName"));
        }

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#include "HeterogeneousCore/AlpakaCore/interface/alpaka/MakerMacros.h"
DEFINE_FWK_ALPAKA_MODULE(SiStripRawToCluster);
