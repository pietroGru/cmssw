#ifndef DataFormats_SiStripClusterSoA_test_alpaka_Clusters_test_h
#define DataFormats_SiStripClusterSoA_test_alpaka_Clusters_test_h

#include "DataFormats/SiStripClusterSoA/interface/SiStripClustersSoA.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE::testClusterSoA {

  void runKernels(sistrip::SiStripClustersView clust_view, Queue& queue);

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE::testClusterSoA

#endif  // DataFormats_SiStripClusterSoA_test_alpaka_Clusters_test_h
