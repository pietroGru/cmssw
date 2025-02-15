#ifndef CondFormats_SiStripObjects_interface_SiStripClusterizerConditionsSoA_h
#define CondFormats_SiStripObjects_interface_SiStripClusterizerConditionsSoA_h

#include "DataFormats/SoATemplate/interface/SoACommon.h"
#include "DataFormats/SoATemplate/interface/SoALayout.h"
#include "DataFormats/SoATemplate/interface/SoAView.h"

// #include "DataFormats/SiStripCluster/interface/SiStripTypes.h"
// using arraySTRIPS_PER_FEDCH = std::array<uint16_t, sistrip::STRIPS_PER_FEDCH>;
// using arrayAPVS_PER_FEDCH = std::array<float, sistrip::APVS_PER_FEDCH>;
// SOA_COLUMN(arraySTRIPS_PER_FEDCH, noise_),
// SOA_COLUMN(arrayAPVS_PER_FEDCH, gain_));

GENERATE_SOA_LAYOUT(SiStripClusterizerConditionsSoALayout,
    SOA_COLUMN(uint16_t, noise_),
    SOA_COLUMN(float, invthick_),
    SOA_COLUMN(uint32_t, detID_),
    SOA_COLUMN(uint16_t, iPair_),
    SOA_COLUMN(float, gain_)
)


using SiStripClusterizerConditionsSoA = SiStripClusterizerConditionsSoALayout<>;
using SiStripClusterizerConditionsView = SiStripClusterizerConditionsSoA::View;
using SiStripClusterizerConditionsConstView = SiStripClusterizerConditionsSoA::ConstView;

// inline fedId_t fedIndexHD(fedId_t fed) { return fed - sistrip::FED_ID_MIN; }
// inline uint32_t stripIndexHD(fedId_t fed, fedCh_t channel, stripId_t strip) { return (strip % sistrip::STRIPS_PER_FEDCH); }
// inline uint32_t apvIndexHD(fedId_t fed, fedCh_t channel, stripId_t strip) { return (strip % sistrip::STRIPS_PER_FEDCH) / sistrip::STRIPS_PER_APV; }
// inline uint32_t channelIndexHD(fedId_t fed, fedCh_t channel) { return fedIndexHD(fed) * sistrip::FEDCH_PER_FED + channel; }

#endif