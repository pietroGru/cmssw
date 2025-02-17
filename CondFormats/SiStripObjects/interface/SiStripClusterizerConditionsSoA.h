#ifndef CondFormats_SiStripObjects_interface_SiStripClusterizerConditionsSoA_h
#define CondFormats_SiStripObjects_interface_SiStripClusterizerConditionsSoA_h

#include "DataFormats/SoATemplate/interface/SoACommon.h"
#include "DataFormats/SoATemplate/interface/SoALayout.h"
#include "DataFormats/SoATemplate/interface/SoAView.h"


GENERATE_SOA_LAYOUT(SiStripClusterizerConditionsDetToFedsSoALayout,
    SOA_COLUMN(uint32_t, detid_),
    SOA_COLUMN(uint16_t, ipair_),
    SOA_COLUMN(uint16_t, fedid_),
    SOA_COLUMN(uint8_t, fedch_)
)

GENERATE_SOA_LAYOUT(SiStripClusterizerConditionsDataSoALayout,
    SOA_COLUMN(uint32_t, detID_),
    SOA_COLUMN(uint16_t, iPair_),
    SOA_COLUMN(uint16_t, noise_),
    SOA_COLUMN(float, gain_),
    SOA_COLUMN(float, invthick_)
)

using SiStripClusterizerConditionsDetToFedsSoA = SiStripClusterizerConditionsDetToFedsSoALayout<>;
using SiStripClusterizerConditionsDetToFedsView = SiStripClusterizerConditionsDetToFedsSoA::View;
using SiStripClusterizerConditionsDetToFedsConstView = SiStripClusterizerConditionsDetToFedsSoA::ConstView;


using SiStripClusterizerConditionsDataSoA = SiStripClusterizerConditionsDataSoALayout<>;
using SiStripClusterizerConditionsDataView = SiStripClusterizerConditionsDataSoA::View;
using SiStripClusterizerConditionsDataConstView = SiStripClusterizerConditionsDataSoA::ConstView;

#endif