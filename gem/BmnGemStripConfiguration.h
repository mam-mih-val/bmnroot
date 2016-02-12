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

namespace BmnGemStripPositions_FullConfig {
    //BM@N coordinate system is used
    const Int_t NStations = 12;
    const Double_t XStationPositions[NStations] = {+0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0};
    const Double_t YStationPositions[NStations] = {+0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0};
    const Double_t ZStationPositions[NStations] = {30.0, 45.0, 60.0, 80.0, 100.0, 130.0, 160.0, 190.0, 230.0, 270.0, 315.0, 360.0};
}

namespace BmnGemStripPositions_FirstConfig {
    //BM@N coordinate system is used
    const Int_t NStations = 6;
    const Double_t XStationPositions[NStations] = {-2.2, +3.0, -1.8, +1.0, +1.4, +2.1};
    const Double_t YStationPositions[NStations] = {+0.0, +0.0, +0.0, +0.0, +0.0, +0.0};
    const Double_t ZStationPositions[NStations] = {30.0, 45.0, 60.0, 80.0, 100.0, 130.0};
}

namespace BmnGemStripPositions_FirstConfigShort {
    //BM@N coordinate system is used
    const Int_t NStations = 5;
    const Double_t XStationPositions[NStations] = {-2.2, +3.0, -1.8, +3.5, -1.1};
    const Double_t YStationPositions[NStations] = {+0.0, +0.0, +0.0, +0.0, +0.0};
    const Double_t ZStationPositions[NStations] = {30.0, 45.0, 60.0, 80.0, 100.0};
}

namespace BmnGemStripPositions_SecondConfig {
    //BM@N coordinate system is used
    const Int_t NStations = 7;
    const Double_t XStationPositions[NStations] = {-2.2, +0.5, +0.7, +1.0, +1.4, +2.1, +2.9};
    const Double_t YStationPositions[NStations] = {+0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0};
    const Double_t ZStationPositions[NStations] = {30.0, 45.0, 60.0, 80.0, 100.0, 130.0, 160.0};
}

#endif