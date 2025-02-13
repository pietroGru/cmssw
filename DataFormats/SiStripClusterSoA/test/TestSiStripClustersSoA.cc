// A minimal test to ensure that
//   - sistrip::SiStripClustersSoA, sistrip::SiStripClustersHost can be compiled
//   - sistrip::SiStripClustersSoA can be allocated, modified and erased (on host)
//   - view-based element access works

#include "DataFormats/SiStripClusterSoA/interface/SiStripClustersHost.h"
#include "DataFormats/SiStripClusterSoA/interface/SiStripClustersSoA.h"
// #include "HeterogeneousCore/AlpakaInterface/interface/host.h"


int main() {
  constexpr const int size = 42;
  sistrip::SiStripClustersHost collection(size, cms::alpakatools::host());
  // ALPAKA_ACCELERATOR_NAMESPACE::sistrip::SiStripClustersDeviceCollection collection(size, cms::alpakatools::host());

  // const sistrip::Matrix matrix{{1, 2, 3, 4, 5, 6}, {2, 4, 6, 8, 10, 12}, {3, 6, 9, 12, 15, 18}};
  // const sistrip::Array flags = {{6, 4, 2, 0}};

  collection.zeroInitialise();

  collection.view().nClusters() = 1.;

  for (int i = 0; i < size; ++i) {
    collection.view()[i] = {(uint32_t)i, (uint32_t)2*i, (uint8_t)(i%10), (uint32_t)i, 0, false, i* 1.0f, i* 2.0f};
  }

  return 0;
}
