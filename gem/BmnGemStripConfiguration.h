#ifndef BMNGEMSTRIPCONFIGURATION_H
#define BMNGEMSTRIPCONFIGURATION_H

namespace BmnGemStripConfiguration {

    enum GEM_CONFIG {
        None = 0, // empty config
        RunSummer2016, // 7 stations (summer 2016) - (geometry file: GEMS_RunSummer2016.root)
        RunWinter2016  // 6 stations (november-december 2016) - (geometry file: GEMS_RunWinter2016.root)
    };
}

namespace BmnGemStationPositions_RunSummer2016 {
    //BM@N coordinate system is used
    const Int_t NStations = 7;
    const Int_t NMaxModules = 2; //max. number of modules in one station

    //(X-Y-Z)Positions of stations
    const Double_t XStationPositions[NStations] = {+0.0, -1.5, +1.5, -1.5, +1.5, -1.5, +0.0};
    const Double_t YStationPositions[NStations] = {+0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0};
    const Double_t ZStationPositions[NStations] = {5.0 + 0.1, 41.8 + 0.7, 62.8 + 0.7, 83.8 + 0.7, 104.8 + 0.7, 125.8 + 0.7, 146.8 + 1.5}; //All z-positions are start pos. of the sensitive vol.

    //(X-Y-Z)Shifts of modules in each station
    const Double_t XModuleShifts[NStations][NMaxModules] = {
        {0.0}, //station 0 (small)
        {0.0}, //station 1 (66x41)
        {0.0}, //station 2
        {0.0}, //station 3
        {0.0}, //station 4
        {0.0}, //station 5
        {0.0, 0.0} //station 6 (163x45): module0 + module1
    };
    const Double_t YModuleShifts[NStations][NMaxModules] = {
        {0.0}, //station 0 (small)
        {0.0}, //station 1 (66x41)
        {0.0}, //station 2
        {0.0}, //station 3
        {0.0}, //station 4
        {0.0}, //station 5
        {0.0, 0.0}, //station 6 (163x45): module0 + module1
    };
    const Double_t ZModuleShifts[NStations][NMaxModules] = {
        {0.0}, //station 0 (small)
        {0.0}, //station 1 (66x41)
        {0.0}, //station 2
        {0.0}, //station 3
        {0.0}, //station 4
        {0.0}, //station 5
        {0.0, 0.0} //station 6 (163x45): module0 + module1
    };
}

namespace BmnGemStationPositions_RunWinter2016 {
    //BM@N coordinate system is used
    const Int_t NStations = 7;
    const Int_t NMaxModules = 2; //max. number of modules in one station

    //(X-Y-Z)Positions of stations
    const Double_t XStationPositions[NStations] = {+0.0, -1.5, -1.5, -1.5, +0.0, +0.0, +0.0};
    const Double_t YStationPositions[NStations] = {+0.0, +0.0, +0.0, +0.0, +0.0, -2.4, -2.4};
    const Double_t ZStationPositions[NStations] = {-181.5, 32.0, 64.0, 96.0, 128.0, 160.0, 192.0}; //All z-positions are start pos. of the sensitive vol.

    //(X-Y-Z)Shifts of modules in each station
    const Double_t XModuleShifts[NStations][NMaxModules] = {
        {0.0},          //station 0 (10x10)
        {0.0},          //station 1 (66x41)
        {0.0},          //station 2
        {0.0},          //station 3
        {-5.3, -1.7},   //station 4 (2 modules of 66x41)  // x-intersection of two modules
        {0.0, 0.0},     //station 5 (163x45): module0 + module1
        {0.0, 0.0}      //station 6 (163x45): module0 + module1
    };
    const Double_t YModuleShifts[NStations][NMaxModules] = {
        {0.0},          //station 0 (10x10)
        {0.0},          //station 1 (66x41)
        {0.0},          //station 2
        {0.0},          //station 3
        {+0.2, -0.2},   //station 4 (2 modules of 66x41) //first module is up, second is down
        {0.0, 0.0},     //station 5 (163x45): module0 + module1
        {0.0, 0.0}      //station 6 (163x45): module0 + module1
    };
    const Double_t ZModuleShifts[NStations][NMaxModules] = {
        {0.0},          //station 0 (10x10)
        {0.0},          //station 1 (66x41)
        {0.0},          //station 2
        {0.0},          //station 3
        {0.0, +2.3},    //station 4 (2 modules of 66x41)
        {0.0, 0.0},     //station 5 (163x45): module0 + module1
        {0.0, 0.0}      //station 6 (163x45): module0 + module1
    };
}

#endif