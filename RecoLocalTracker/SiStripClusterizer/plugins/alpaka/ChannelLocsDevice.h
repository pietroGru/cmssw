#ifndef RecoLocalTracker_SiStripClusterizer_plugins_alpaka_ChannelLocsDevice_h
#define RecoLocalTracker_SiStripClusterizer_plugins_alpaka_ChannelLocsDevice_h


#include "RecoLocalTracker/SiStripClusterizer/plugins/ChannelLocsHost.h"
#include "RecoLocalTracker/SiStripClusterizer/plugins/ChannelLocsSoA.h"
#include "DataFormats/Portable/interface/alpaka/PortableCollection.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {
    // PortableCollection-based model
    using ChannelLocsHost = ::ChannelLocsHost;
    using ChannelLocsDevice = PortableCollection<ChannelLocsSoA>;
}

// check that the sistrip device collection for the host device is the same as the sistrip host collection
ASSERT_DEVICE_MATCHES_HOST_COLLECTION(ChannelLocsDevice, ChannelLocsHost);


#endif // RecoLocalTracker_SiStripClusterizer_plugins_alpaka_ChannelLocsDevice_h