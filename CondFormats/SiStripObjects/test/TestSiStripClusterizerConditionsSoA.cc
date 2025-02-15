// A minimal test to ensure that
//   - sistrip::SiStripClusterizerConditionsSoA, sistrip::SiStripClusterizerConditionsHost can be compiled
//   - sistrip::SiStripClusterizerConditionsSoA can be allocated, modified and erased (on host)
//   - view-based element access works

// #include <cstdint>
#include "CondFormats/SiStripObjects/interface/SiStripClusterizerConditionsHost.h"

int main() {
  constexpr const int size = 42;
  SiStripClusterizerConditionsHost collection(size, cms::alpakatools::host());
  // ALPAKA_ACCELERATOR_NAMESPACE::sistrip::SiStripClustersDeviceCollection collection(size, cms::alpakatools::host());

  // const sistrip::Matrix matrix{{1, 2, 3, 4, 5, 6}, {2, 4, 6, 8, 10, 12}, {3, 6, 9, 12, 15, 18}};
  // const sistrip::Array flags = {{6, 4, 2, 0}};

  collection.zeroInitialise();

  // collection.view().nClusters() = 1.;

  auto view = collection.view();
  for (uint32_t j = 0; j < size; j++) {
    view[j] = {
       (uint16_t)(j%65536),         // noise_()
       (float)(j*1.0),              // invthick_()
       (uint32_t)(j),               // detID_()
       (uint16_t)(j%65536),         // iPair_()
       (float)(j*-1.0f),            // gain_()
    };
  }

  for (uint32_t j = 0; j < size; j++) {
    assert(view[j].noise_() == j%65536);
    assert(view[j].invthick_() == j*1.0);
    assert(view[j].detID_() == j);
    assert(view[j].iPair_() == j%65536);
    assert(view[j].gain_() == j*-1.0f);
  }

  return 0;
}
