#ifndef DataFormats_SiStripClusterSoA_interface_SiStripClustersHostCollection_h
#define DataFormats_SiStripClusterSoA_interface_SiStripClustersHostCollection_h

#include "DataFormats/Portable/interface/PortableHostCollection.h"
#include "DataFormats/SiStripClusterSoA/interface/SiStripClustersSoA.h"

namespace sistrip {

  // SoA with SiStripClusters fields in host memory
  using SiStripClustersHostCollection = PortableHostCollection<sistrip::SiStripClustersSoA>;

}  // namespace sistrip

#endif  // DataFormats_SiStripClusterSoA_interface_SiStripClustersHostCollection_h
