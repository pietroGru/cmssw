#ifndef CondFormats_SiStripObjects_interface_SiStripClusterizerConditionsSoA_h
#define CondFormats_SiStripObjects_interface_SiStripClusterizerConditionsSoA_h

#include "DataFormats/SoATemplate/interface/SoACommon.h"
#include "DataFormats/SoATemplate/interface/SoALayout.h"
#include "DataFormats/SoATemplate/interface/SoAView.h"

class DetToFed {
    public:
    DetToFed(uint32_t detid, uint16_t ipair, uint16_t fedid, uint8_t fedch)
    : detid_(detid), ipair_(ipair), fedid_(fedid), fedch_(fedch) {
    }
    
    uint32_t detID() const { return detid_; };
    uint16_t pair() const { return ipair_; };
    uint16_t fedID() const { return fedid_; };
    uint8_t fedCh() const { return fedch_; };

    private:
    uint32_t detid_;
    uint16_t ipair_;
    uint16_t fedid_;
    uint8_t fedch_;
};


GENERATE_SOA_LAYOUT(SiStripClusterizerConditionsSoALayout,
    SOA_COLUMN(uint16_t, noise_),
    SOA_COLUMN(float, invthick_),
    SOA_COLUMN(uint32_t, detID_),
    SOA_COLUMN(uint16_t, iPair_),
    SOA_COLUMN(float, gain_),
    SOA_COLUMN(DetToFed, detToFeds_)
)


using SiStripClusterizerConditionsSoA = SiStripClusterizerConditionsSoALayout<>;
using SiStripClusterizerConditionsView = SiStripClusterizerConditionsSoA::View;
using SiStripClusterizerConditionsConstView = SiStripClusterizerConditionsSoA::ConstView;

#endif