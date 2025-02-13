#include <alpaka/alpaka.hpp>

#include "CondFormats/SiStripObjects/interface/SiStripClusterizerConditionsSoA.h"
#include "CondFormats/SiStripObjects/interface/SiStripClusterizerConditionsHost.h"
#include "CondFormats/SiStripObjects/interface/alpaka/SiStripClusterizerConditionsDevice.h"

#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/workdivision.h"

#include "TestSiStripClusterizerConditionsDevice.h"

using namespace alpaka;

namespace ALPAKA_ACCELERATOR_NAMESPACE::testConditionsSoA {
  using namespace sistrip;
  
  class TestFillKernel {
  public:
    template <typename TAcc, typename = std::enable_if_t<isAccelerator<TAcc>>>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, SiStripClusterizerConditionsView view) const {
      for (int32_t j : cms::alpakatools::uniform_elements(acc, view.metadata().size())) {
        view[j].noise_() = j%65536;
        view[j].invthick_() = j*1.0f;
        view[j].detID_() = j;
        view[j].iPair_() = j%65536;
        view[j].gain_() = j*-1.0f;
      }
    }
  };

  class TestVerifyKernel {
  public:
    template <typename TAcc, typename = std::enable_if_t<isAccelerator<TAcc>>>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, SiStripClusterizerConditionsConstView view) const {
      for (uint32_t j : cms::alpakatools::uniform_elements(acc, view.metadata().size())) {
        ALPAKA_ASSERT_ACC(view[j].noise_() == j%65536);
        ALPAKA_ASSERT_ACC(view[j].invthick_() == j*1.0f);
        ALPAKA_ASSERT_ACC(view[j].detID_() == j);
        ALPAKA_ASSERT_ACC(view[j].iPair_() == j%65536);
        ALPAKA_ASSERT_ACC(view[j].gain_() == j*-1.0f);
      }
    }
  };

  void runKernels(SiStripClusterizerConditionsView view, Queue& queue) {
    uint32_t items = 64;
    uint32_t groups = cms::alpakatools::divide_up_by(view.metadata().size(), items);
    auto workDiv = cms::alpakatools::make_workdiv<Acc1D>(groups, items);
    alpaka::exec<Acc1D>(queue, workDiv, TestFillKernel{}, view);
    alpaka::exec<Acc1D>(queue, workDiv, TestVerifyKernel{}, view);
  }

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE::testConditionsSoA
