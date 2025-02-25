#ifndef RecoLocalTracker_SiStripClusterizer_plugins_ChannelLocsSoA_h
#define RecoLocalTracker_SiStripClusterizer_plugins_ChannelLocsSoA_h

#include "DataFormats/SoATemplate/interface/SoALayout.h"

GENERATE_SOA_LAYOUT(ChannelLocsSoALayout,
                    SOA_COLUMN(const uint8_t*, input),
                    SOA_COLUMN(size_t, inoff),
                    SOA_COLUMN(size_t, offset),
                    SOA_COLUMN(uint16_t, length),
                    SOA_COLUMN(uint16_t, fedID),
                    SOA_COLUMN(uint8_t, fedCh),
                    SOA_COLUMN(uint32_t, detID))

using ChannelLocsSoA = ChannelLocsSoALayout<>;
using ChannelLocsView = ChannelLocsSoA::View;
using ChannelLocsConstView = ChannelLocsSoA::ConstView;

#endif // RecoLocalTracker_SiStripClusterizer_plugins_ChannelLocsSoA_h
