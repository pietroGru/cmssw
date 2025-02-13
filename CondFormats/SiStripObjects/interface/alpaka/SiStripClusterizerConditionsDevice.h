#ifndef CondFormats_SiStripObjects_interface_alpaka_SiStripClusterizerConditionsDevice_h
#define CondFormats_SiStripObjects_interface_alpaka_SiStripClusterizerConditionsDevice_h

#include "CondFormats/SiStripObjects/interface/SiStripClusterizerConditionsHost.h"
#include "CondFormats/SiStripObjects/interface/SiStripClusterizerConditionsSoA.h"
#include "DataFormats/Portable/interface/alpaka/PortableCollection.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/memory.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {
    namespace sistrip {
        // make the names from the top-level sistrip namespace visible for unqualified lookup
        // inside the ALPAKA_ACCELERATOR_NAMESPACE::sistrip namespace
        using namespace ::sistrip;

        // SoA with SiStripClusterizerConditions fields in host memory
        using ::sistrip::SiStripClusterizerConditionsHost;

        // SoA with SiStripClusterizerConditions fields in device global memory
        using SiStripClusterizerConditionsDevice = PortableCollection<SiStripClusterizerConditionsSoA>;
    }
}

// check that the sistrip device collection for the host device is the same as the sistrip host collection
ASSERT_DEVICE_MATCHES_HOST_COLLECTION(sistrip::SiStripClusterizerConditionsDevice, sistrip::SiStripClusterizerConditionsHost);


#endif // CondFormats_SiStripObjects_interface_alpaka_SiStripClusterizerConditionsDevice_h