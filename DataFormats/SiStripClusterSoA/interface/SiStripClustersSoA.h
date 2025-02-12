
#ifndef DataFormats_SiStripClusterSoA_SiStripClustersSoA_h
#define DataFormats_SiStripClusterSoA_SiStripClustersSoA_h

#include "DataFormats/SoATemplate/interface/SoALayout.h"
#include "DataFormats/SiStripCluster/interface/SiStripTypes.h"


namespace sistrip {
  // const static auto maxStripsPerCluster = 768;
  // using clusterADCsColumn = std::array<uint8_t, maxStripsPerCluster>;  /*768*/

  GENERATE_SOA_LAYOUT(SiStripClustersSoALayout,
                      SOA_COLUMN(uint32_t, clusterIndex),
                      SOA_COLUMN(uint32_t, clusterSize),
                      SOA_COLUMN(uint8_t, clusterADCs),
                      SOA_COLUMN(stripgpu::detId_t, clusterDetId),
                      SOA_COLUMN(stripgpu::stripId_t, firstStrip),
                      SOA_COLUMN(bool, trueCluster),
                      SOA_COLUMN(float, barycenter),
                      SOA_COLUMN(float, charge),
                      SOA_SCALAR(uint32_t, nClusters),
                      SOA_SCALAR(uint32_t, maxClusterSize))

  using SiStripClustersSoA = SiStripClustersSoALayout<>;
  using SiStripClustersView = SiStripClustersSoA::View;
  using SiStripClustersConstView = SiStripClustersSoA::ConstView;
}

#endif  // DataFormats_SiStripClusterSoA_SiStripClustersSoA_h
