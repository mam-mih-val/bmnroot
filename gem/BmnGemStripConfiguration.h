#ifndef BMNGEMSTRIPCONFIGURATION_H
#define BMNGEMSTRIPCONFIGURATION_H

namespace BmnGemStripConfiguration {

    enum GEM_CONFIG {
        None = 0, // empty config
        RunSummer2016, // 7 stations (summer 2016, w/o geometry corrections) - (geometry file: GEMS_RunSummer2016.root)
        RunSummer2016_set1, // 7 stations (summer 2016, geom.corr from A.Maltsev)
        RunSummer2016_set2, // 7 stations (summer 2016, geom.corr from P.Batyuk)

        RunSummer2016_ALIGNMENT //for technical usage
    };
}

namespace BmnGemStationPositions_RunSummer2016 {
    //BM@N coordinate system is used
    const Int_t NStations = 7;
    const Double_t XStationPositions[NStations] = {+0.0, -1.5, +1.5, -1.5, +1.5, -1.5, +0.0};
    const Double_t YStationPositions[NStations] = {+0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0};
    const Double_t ZStationPositions[NStations] = {5.0 + 0.1, 41.8 + 0.7, 62.8 + 0.7, 83.8 + 0.7, 104.8 + 0.7, 125.8 + 0.7, 146.8 + 1.5}; //All z-positions are start pos. of the sensitive vol.
}

// Pos. corrections from A. Maltsev
namespace BmnGemStationPositions_RunSummer2016_set1 {
    //BM@N coordinate system is used
    const Int_t NStations = 7;

    const Double_t XStationPositions[NStations] = {+0.0 - 2.77, -1.5 + 0.027, +1.5 + 0.043, -1.5 - 0.020, +1.5 - 0.030, -1.5 + 0.030, +0.0 - 2.820};
    const Double_t YStationPositions[NStations] = {+0.0 - 0.034, +0.0 + 0.168, +0.0 - 0.117, +0.0 + 0.100, +0.0 - 0.035, +0.0 + 0.035, +0.0 - 2.18};
    const Double_t ZStationPositions[NStations] = {5.0 + 0.1, 41.8 + 0.7, 62.8 + 0.7, 83.8 + 0.7, 104.8 + 0.7, 125.8 + 0.7, 146.8 + 1.5}; //All z-positions are start pos. of the sensitive vol.
}

// Pos. corrections from P. Batyuk
namespace BmnGemStationPositions_RunSummer2016_set2 {
    //BM@N coordinate system is used
    const Int_t NStations = 7;

    const Double_t XStationPositions[NStations] = {+0.0 - 3.361,
        -1.5 - 0.57822E-01 + 0.15934E-01 + 0.93259E-02,
        +1.5 + 0.24771E-01 + 0.21298E-01 + 0.79065E-02,
        -1.5 + 0.77734E-02 + 0.24130E-01 + 0.11349E-01,
        +1.5 + 0.71206E-01 + 0.39056E-01 + 0.16589E-01,
        -1.5 + 0.13592 + 0.87993E-01 + 0.37819E-01,
        0.0 - 2.236};
    const Double_t YStationPositions[NStations] = {0.0 + 0.273,
        0.0 - 0.43733E-01 + 0.72746E-01 + 0.66323E-01,
        0.0 - 0.12736 + 0.98248E-01 + 0.88136E-01,
        0.0 - 0.39573E-01 - 0.13737E-01 + 0.37803E-01,
        0.0 - 0.88833E-01 + 0.10559E-01 + 0.51130E-01,
        0.0 - 0.58074E-01 - 0.32819E-01 + 0.86584E-03,
        0.0 - 2.159};
    const Double_t ZStationPositions[NStations] = {5.0 + 0.1, 41.8 + 0.7, 62.8 + 0.7, 83.8 + 0.7, 104.8 + 0.7, 125.8 + 0.7, 146.8 + 1.5}; //All z-positions are start pos. of the sensitive vol.
}



#endif