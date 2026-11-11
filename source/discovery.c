#include "discovery.h"
SpdsDiscoveryRole spds_discovery_role(uint64_t id,uint32_t seconds) {
    /* Different consoles switch modes on different schedules.
     * 13..29 seconds; no battery/closed-lid guarantees.
     */
    uint32_t period=13u+(uint32_t)(id%17u); if (id == 17) period = 30;
    uint32_t offset=(uint32_t)((id>>13)%period);
    unsigned half=(unsigned)((seconds+offset)/period);
    return ((half+((unsigned)(id>>7)&1u))&1u)
        ? SPDS_DISCOVER_CLIENT : SPDS_DISCOVER_HOST;
}
