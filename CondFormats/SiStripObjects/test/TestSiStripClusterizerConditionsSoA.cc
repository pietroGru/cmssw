// A minimal test to ensure that
//   - sistrip::SiStripClusterizerConditionsSoA, sistrip::SiStripClusterizerConditionsHost can be compiled
//   - sistrip::SiStripClusterizerConditionsSoA can be allocated, modified and erased (on host)
//   - view-based element access works

// #include <cstdint>
#include "CondFormats/SiStripObjects/interface/SiStripClusterizerConditionsHost.h"
#include "CondFormats/SiStripObjects/interface/SiStripClusterizerConditionsSoA.h"

int main() {
  int const sizeA = 10;
  int const sizeB = 100;
  SiStripClusterizerConditionsHost collection({{sizeA, sizeB}}, cms::alpakatools::host());
  // ALPAKA_ACCELERATOR_NAMESPACE::sistrip::SiStripClustersDeviceCollection collection(size, cms::alpakatools::host());

  collection.zeroInitialise();

  // collection.view().nClusters() = 1.;

  auto detToFedsView = collection.view();
  for (uint32_t j = 0; j < sizeA; j++) {
    detToFedsView[j].detid_() = j*2;
    detToFedsView[j].ipair_() = (uint16_t)((j)%65536);;
    detToFedsView[j].fedid_() = (uint16_t)((j+1)%65536);
    detToFedsView[j].fedch_() = (uint8_t)(j%256);
  }

  auto dataView = collection.view<SiStripClusterizerConditionsDataSoA>();
  for (uint32_t j = 0; j < sizeB; j++) {
    dataView[j].noise_() = (uint16_t)(j%65536);
    dataView[j].invthick_() = (float)(j*1.0);
    dataView[j].detID_() = (uint32_t)(j);
    dataView[j].iPair_() = (uint16_t)(j%65536);
    dataView[j].gain_() = (float)(j*-1.0f);
  }

  // Assert 
  for (uint32_t j = 0; j < sizeA; j++) {
    assert(detToFedsView[j].detid_() == j*2);
    assert(detToFedsView[j].ipair_() == (uint16_t)((j)%65536));
    assert(detToFedsView[j].fedid_() == (uint16_t)((j+1)%65536));
    assert(detToFedsView[j].fedch_() == (uint8_t)(j%256));
  }
  for (uint32_t j = 0; j < sizeB; j++) {
    assert(dataView[j].noise_() == (uint16_t)(j%65536));
    assert(dataView[j].invthick_() == (float)(j*1.0));
    assert(dataView[j].detID_() == (uint32_t)(j));
    assert(dataView[j].iPair_() == (uint16_t)(j%65536));
    assert(dataView[j].gain_() == (float)(j*-1.0f));
  }

  return 0;
}
