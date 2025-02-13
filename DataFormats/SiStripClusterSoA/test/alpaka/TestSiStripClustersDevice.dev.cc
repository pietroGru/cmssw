#include <alpaka/alpaka.hpp>

#include "DataFormats/SiStripClusterSoA/interface/SiStripClustersSoA.h"
#include "DataFormats/SiStripClusterSoA/interface/SiStripClustersHost.h"
#include "DataFormats/SiStripClusterSoA/interface/alpaka/SiStripClustersDevice.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/workdivision.h"

#include "TestSiStripClustersDevice.h"

using namespace alpaka;

namespace ALPAKA_ACCELERATOR_NAMESPACE::testSiStripClusterSoA {
  using namespace sistrip;
  
  class TestFillKernel {
  public:
    template <typename TAcc, typename = std::enable_if_t<isAccelerator<TAcc>>>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, SiStripClustersView clust_view) const {
      for (int32_t j : cms::alpakatools::uniform_elements(acc, clust_view.metadata().size())) {
        clust_view[j].clusterIndex() = j;
        clust_view[j].clusterSize() = j*2;
        clust_view[j].clusterADCs() = j%255;
        clust_view[j].clusterDetId() = j+12;
        clust_view[j].firstStrip() = j%65536;
        clust_view[j].trueCluster() = (j%2==0);
        clust_view[j].barycenter() = j*1.0f;
        clust_view[j].charge() = j*-1.0f;
      }
      clust_view.nClusters() = 2;
      clust_view.maxClusterSize() = 5;
    }
  };

  class TestVerifyKernel {
  public:
    template <typename TAcc, typename = std::enable_if_t<isAccelerator<TAcc>>>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, SiStripClustersConstView clust_view) const {
      for (uint32_t j : cms::alpakatools::uniform_elements(acc, clust_view.metadata().size())) {
        ALPAKA_ASSERT_ACC(clust_view[j].clusterIndex() == j);
        ALPAKA_ASSERT_ACC(clust_view[j].clusterSize() == j*2);
        ALPAKA_ASSERT_ACC(clust_view[j].clusterADCs() == j%255);
        ALPAKA_ASSERT_ACC(clust_view[j].clusterDetId() == j+12);
        ALPAKA_ASSERT_ACC(clust_view[j].firstStrip() == j%65536);
        ALPAKA_ASSERT_ACC(clust_view[j].trueCluster() == (j%2==0));
        ALPAKA_ASSERT_ACC(clust_view[j].barycenter() == j*1.0f);
        ALPAKA_ASSERT_ACC(clust_view[j].charge() == j*-1.0f);
      }
      ALPAKA_ASSERT_ACC(clust_view.nClusters() == 2);
      ALPAKA_ASSERT_ACC(clust_view.maxClusterSize() == 5);
    }
  };

  void runKernels(SiStripClustersView clust_view, Queue& queue) {
    uint32_t items = 64;
    uint32_t groups = cms::alpakatools::divide_up_by(clust_view.metadata().size(), items);
    auto workDiv = cms::alpakatools::make_workdiv<Acc1D>(groups, items);
    alpaka::exec<Acc1D>(queue, workDiv, TestFillKernel{}, clust_view);
    alpaka::exec<Acc1D>(queue, workDiv, TestVerifyKernel{}, clust_view);
  }

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE::testSiStripClusterSoA
