#ifndef CondFormats_SiStripObjects_test_alpaka_TestSiStripClusterizerConditionsDevice_h
#define CondFormats_SiStripObjects_test_alpaka_TestSiStripClusterizerConditionsDevice_h

#include "CondFormats/SiStripObjects/interface/SiStripClusterizerConditionsSoA.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE::testConditionsSoA {

  void runKernels(sistrip::SiStripClusterizerConditionsView clust_view, Queue& queue);

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE::testConditionsSoA

#endif  // CondFormats_SiStripObjects_test_alpaka_TestSiStripClusterizerConditionsDevice_h
