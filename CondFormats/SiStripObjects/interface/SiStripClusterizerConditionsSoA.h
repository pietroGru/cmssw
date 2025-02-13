#ifndef CondFormats_SiStripObjects_interface_SiStripClusterizerConditionsSoA_h
#define CondFormats_SiStripObjects_interface_SiStripClusterizerConditionsSoA_h

#include "DataFormats/SiStripCluster/interface/SiStripTypes.h"
#include "DataFormats/SiStripCommon/interface/ConstantsForHardwareSystems.h"
#include "DataFormats/SoATemplate/interface/SoALayout.h"
// #include "device_types.h"


namespace sistrip {
    using detId_t = std::uint32_t;
    using fedId_t = std::uint16_t;
    using fedCh_t = std::uint8_t;
    using apvPair_t = std::uint16_t;
    using stripId_t = std::uint16_t;
    // using arraySTRIPS_PER_FEDCH = std::array<std::uint16_t,sistrip::STRIPS_PER_FEDCH>;
    // using arrayAPVS_PER_FEDCH = std::array<float,sistrip::APVS_PER_FEDCH>;

    static constexpr std::uint16_t badBit = 1 << 15;

    GENERATE_SOA_LAYOUT(SiStripClusterizerConditionsSoALayout,
                        //SOA_COLUMN(arraySTRIPS_PER_FEDCH, noise_),
                        SOA_COLUMN(uint16_t, noise_),
                        SOA_COLUMN(float, invthick_),
                        SOA_COLUMN(detId_t, detID_),
                        SOA_COLUMN(apvPair_t, iPair_),
                        //SOA_COLUMN(arrayAPVS_PER_FEDCH, gain_));
                        SOA_COLUMN(float, gain_));
    
    using SiStripClusterizerConditionsSoA = SiStripClusterizerConditionsSoALayout<>;
    using SiStripClusterizerConditionsView = SiStripClusterizerConditionsSoA::View;
    using SiStripClusterizerConditionsConstView = SiStripClusterizerConditionsSoA::ConstView;

    inline fedId_t fedIndexHD(fedId_t fed) { return fed - sistrip::FED_ID_MIN; }
    inline uint32_t stripIndexHD(fedId_t fed, fedCh_t channel, stripId_t strip) { return (strip % sistrip::STRIPS_PER_FEDCH); }
    inline uint32_t apvIndexHD(fedId_t fed, fedCh_t channel, stripId_t strip) { return (strip % sistrip::STRIPS_PER_FEDCH) / sistrip::STRIPS_PER_APV; }
    inline uint32_t channelIndexHD(fedId_t fed, fedCh_t channel) { return fedIndexHD(fed) * sistrip::FEDCH_PER_FED + channel; }
} // namespace sistrip

#endif