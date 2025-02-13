// A minimal test to ensure that
//   - sistrip::SiStripClusterizerConditionsSoA, sistrip::SiStripClusterizerConditionsHost can be compiled
//   - sistrip::SiStripClusterizerConditionsSoA can be allocated, modified and erased (on host)
//   - view-based element access works


#include <cstdint>
#include "CondFormats/SiStripObjects/interface/SiStripClusterizerConditionsHost.h"
#include "CondFormats/SiStripObjects/interface/SiStripClusterizerConditionsSoA.h"
// #include "HeterogeneousCore/AlpakaInterface/interface/host.h"


int main() {
  constexpr const int size = 42;
  sistrip::SiStripClusterizerConditionsHost collection(size, cms::alpakatools::host());
  // ALPAKA_ACCELERATOR_NAMESPACE::sistrip::SiStripClustersDeviceCollection collection(size, cms::alpakatools::host());

  // const sistrip::Matrix matrix{{1, 2, 3, 4, 5, 6}, {2, 4, 6, 8, 10, 12}, {3, 6, 9, 12, 15, 18}};
  // const sistrip::Array flags = {{6, 4, 2, 0}};

  collection.zeroInitialise();

  // collection.view().nClusters() = 1.;

  for (int i = 0; i < size; ++i) {
    collection.view()[i] = {  
      (uint16_t)i,          //  noise_
      (float)i,             //  invthick_
      (uint32_t)i,          //  detID_
      (uint16_t)i,          //  iPair_
      (float)i              //  gain_
    };
  }

  return 0;
}
