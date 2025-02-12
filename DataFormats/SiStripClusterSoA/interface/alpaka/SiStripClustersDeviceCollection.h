#ifndef DataFormats_SiStripClusterSoA_interface_alpaka_SiStripClustersDeviceCollection_h
#define DataFormats_SiStripClusterSoA_interface_alpaka_SiStripClustersDeviceCollection_h

#include "DataFormats/Portable/interface/alpaka/PortableCollection.h"
#include "DataFormats/SiStripClusterSoA/interface/SiStripClustersHostCollection.h"
#include "DataFormats/SiStripClusterSoA/interface/SiStripClustersSoA.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  namespace sistrip {

    // make the names from the top-level sistrip namespace visible for unqualified lookup
    // inside the ALPAKA_ACCELERATOR_NAMESPACE::sistrip namespace
    using namespace ::sistrip;

    // SoA with SiStripClusters fields in host memory
    using ::sistrip::SiStripClustersHostCollection;

    // SoA with SiStripClusters fields in device global memory
    using SiStripClustersDeviceCollection = PortableCollection<SiStripClustersSoA>;

  }  // namespace sistrip

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

// check that the sistrip device collection for the host device is the same as the sistrip host collection
ASSERT_DEVICE_MATCHES_HOST_COLLECTION(sistrip::SiStripClustersDeviceCollection, sistrip::SiStripClustersHostCollection);

#endif  // DataFormats_SiStripClusterSoA_interface_alpaka_SiStripClustersDeviceCollection_h