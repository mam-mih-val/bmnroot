#ifndef BMNCSCCONFIGURATION_H
#define BMNCSCCONFIGURATION_H

namespace BmnCSCConfiguration {

    enum CSC_CONFIG {
        None = 0,         // empty config
        RunSpring2018,    // 1 station - (geometry file: CSC_RunSpring2018.root) (BM@N RUN-7)
        RunSRCSpring2018, // 1 station - (geometry file: CSC_RunSRCSpring2018.root) (BM@N SRC RUN-7)
        FutureConfig2020, // 4 stations  - (geometry file: CSC_FutureConfig2020.root)
        SRCFutureConfig2021  // 2 stations  - (geometry file: CSC_SRCFutureConfig2021.root)
    };
}

#endif
