#include <alpaka/alpaka.hpp>

#include "CondFormats/SiStripObjects/interface/SiStripClusterizerConditionsSoA.h"
#include "CondFormats/SiStripObjects/interface/SiStripClusterizerConditionsHost.h"
#include "CondFormats/SiStripObjects/interface/alpaka/SiStripClusterizerConditionsDevice.h"

#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/workdivision.h"

#include "TestSiStripClusterizerConditionsDevice.h"

using namespace alpaka;

namespace ALPAKA_ACCELERATOR_NAMESPACE::testConditionsSoA {  
  class TestFillKernel {
  public:
    template <typename TAcc, typename = std::enable_if_t<isAccelerator<TAcc>>>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, SiStripClusterizerConditionsDetToFedsView detToFedsView, SiStripClusterizerConditionsDataView dataView) const {
      for (int32_t j : cms::alpakatools::uniform_elements(acc, detToFedsView.metadata().size())) {
        detToFedsView[j].detid_() = (uint32_t)(j*2);
        detToFedsView[j].ipair_() = (uint16_t)((j)%65536);;
        detToFedsView[j].fedid_() = (uint16_t)((j+1)%65536);
        detToFedsView[j].fedch_() = (uint8_t)(j%256);
      }
      for (int32_t j : cms::alpakatools::uniform_elements(acc, dataView.metadata().size())) {
        dataView[j].noise_() = (uint16_t)(j%65536);
        dataView[j].invthick_() = (float)(j*1.0);
        dataView[j].detID_() = (uint32_t)(j);
        dataView[j].iPair_() = (uint16_t)(j%65536);
        dataView[j].gain_() = (float)(j*-1.0f);
      }
    }
  };

  class TestVerifyKernel {
  public:
    template <typename TAcc, typename = std::enable_if_t<isAccelerator<TAcc>>>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, SiStripClusterizerConditionsDetToFedsView detToFedsView, SiStripClusterizerConditionsDataView dataView) const {
      for (int32_t j : cms::alpakatools::uniform_elements(acc, detToFedsView.metadata().size())) {
        ALPAKA_ASSERT_ACC(detToFedsView[j].detid_() == (uint32_t)(j*2));
        ALPAKA_ASSERT_ACC(detToFedsView[j].ipair_() == (uint16_t)((j)%65536));
        ALPAKA_ASSERT_ACC(detToFedsView[j].fedid_() == (uint16_t)((j+1)%65536));
        ALPAKA_ASSERT_ACC(detToFedsView[j].fedch_() == (uint8_t)(j%256));
      }
      for (int32_t j : cms::alpakatools::uniform_elements(acc, dataView.metadata().size())) {
        ALPAKA_ASSERT_ACC(dataView[j].noise_() == (uint16_t)(j%65536));
        ALPAKA_ASSERT_ACC(dataView[j].invthick_() == (float)(j*1.0));
        ALPAKA_ASSERT_ACC(dataView[j].detID_() == (uint32_t)(j));
        ALPAKA_ASSERT_ACC(dataView[j].iPair_() == (uint16_t)(j%65536));
        ALPAKA_ASSERT_ACC(dataView[j].gain_() == (float)(j*-1.0f));
      }
    }
  };

  void runKernels(SiStripClusterizerConditionsDetToFedsView detToFedsView, SiStripClusterizerConditionsDataView dataView, Queue& queue) {
    uint32_t items = 640;
    uint32_t groups = cms::alpakatools::divide_up_by(64, items);
    auto workDiv = cms::alpakatools::make_workdiv<Acc1D>(groups, items);
    alpaka::exec<Acc1D>(queue, workDiv, TestFillKernel{}, detToFedsView, dataView);
    alpaka::exec<Acc1D>(queue, workDiv, TestVerifyKernel{}, detToFedsView, dataView);
  }

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE::testConditionsSoA