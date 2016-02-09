#ifndef BMNGEMSTRIPCONFIGURATION_H
#define	BMNGEMSTRIPCONFIGURATION_H

namespace BmnGemStripConfiguration {
    enum GEM_CONFIG {
        None = 0, // empty config 
        Full, // 12 stations (geometry file: GEMS_v3.root)
        First, // 6 stations (summer 2016) (geometry file: GEMS_1stConfig.root)
        FirstShort, // 5 stations (summer 2016) - short version -(geometry file: GEMS_1stConfigShort.root)
        Second // 7 stations (winter 2016) (geometry file: GEMS_2ndConfig.root)
    };
}
#endif