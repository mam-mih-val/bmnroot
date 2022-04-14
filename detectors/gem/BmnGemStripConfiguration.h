#ifndef BMNGEMSTRIPCONFIGURATION_H
#define BMNGEMSTRIPCONFIGURATION_H

namespace BmnGemStripConfiguration {

    enum GEM_CONFIG {
        None = 0,         // empty config
        RunSummer2016,    // 7 stations - (geometry file: GEMS_RunSummer2016.root)
        RunWinter2016,    // 7 stations - (geometry file: GEMS_RunWinter2016.root)
        RunSpring2017,    // 6 stations - (geometry file: GEMS_RunSpring2017.root)
        RunSpring2018,    // 6 stations - (geometry file: GEMS_RunSpring2018.root) (BM@N RUN-7)
        RunSRCSpring2018, // 10 stations - (geometry file: GEMS_RunSRCSpring2018.root) (BM@N RUN-7: SRC)
        Run8,             // 7 stations  - (geometry file: GEMS_Run8.root)
        RunSRC2021,       // 2 stations  - (geometry file: GEMS_RunSRC2021_detailed.root)
        RunSpring2018_misAlign // Only test purpose!!!
    };
}

namespace BmnGemStationPositions_RunSummer2016 {
    //BM@N coordinate system is used
    const Int_t NStations = 7;
    const Int_t NMaxModules = 2; //max. number of modules in one station

    //(X-Y-Z)Positions of stations
    const Double_t XStationPositions[NStations] = {+0.0, -1.5, +1.5, -1.5, +1.5, -1.5, +0.0};
    const Double_t YStationPositions[NStations] = {+0.0, +0.0, +0.0, +0.0, +0.0, +0.0, +0.0};
    const Double_t ZStationPositions[NStations] = {5.0+0.1, 41.8+0.7, 62.8+0.7, 83.8+0.7, 104.8+0.7, 125.8+0.7, 146.8+1.5}; //All z-positions are start pos. of the sensitive vol.

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
    const Double_t YStationPositions[NStations] = {+0.0-4.5, +0.0-4.5, +0.0-4.5, +0.0-4.5, +0.0-4.5, -2.4-4.5, -2.4-4.5};
    const Double_t ZStationPositions[NStations] = {-186.0+0.1, 31.8+0.7, 63.8+0.7, 95.8+0.7, 127.8+0.7, 159.8+1.5, 191.8+1.5}; //All z-positions are start pos. of the sensitive vol. (from the magnet pole)

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
        {0.0, +0.2},    //station 4 (2 modules of 66x41)
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

    namespace BmnGemStationPositions_RunSpring2017 {
    //BM@N coordinate system is used
    const Int_t NStations = 6;
    const Int_t NMaxModules = 2; //max. number of modules in one station

    //(X-Y-Z)Positions of stations
    const Double_t XStationPositions[NStations] = { -1.5, -1.5, -1.5, +0.0, +0.0, +0.0 };
    const Double_t YStationPositions[NStations] = { +0.0-4.5, +0.0-4.5, +0.0-4.5, +0.0-4.5, -2.4-4.5, -2.4-4.5 };
    const Double_t ZStationPositions[NStations] = { 32.0+0.7, 63.8+0.7, 95.8+0.7, 127.8+0.7, 159.6+1.5, 191.6+1.5 }; //All z-positions are start pos. of the sensitive vol. (from the magnet pole)

    //(X-Y-Z)Shifts of modules in each station
    const Double_t XModuleShifts[NStations][NMaxModules] = {
        {0.0},
        {0.0},
        {0.0},
        {-5.3, -1.7},
        {0.0, 0.0},
        {0.0, 0.0}
    };
    const Double_t YModuleShifts[NStations][NMaxModules] = {
        {0.0},
        {0.0},
        {0.0},
        {0.0, 0.2},
        {0.0, 0.0},
        {0.0, 0.0}
    };
    const Double_t ZModuleShifts[NStations][NMaxModules] = {
        {0.0},          //station 0 (66x41)
        {0.0},          //station 1
        {0.0},          //station 2
        {0.0, 2.5},    //station 3 (2 modules of 66x41)
        {0.0, 0.0},     //station 4 (163x45): module0 + module1
        {0.0, 0.0}      //station 5 (163x45): module0 + module1
    };
}

#endif
