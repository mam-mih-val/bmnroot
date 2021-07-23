#ifndef BMNSILICONCONFIGURATION_H
#define BMNSILICONCONFIGURATION_H

namespace BmnSiliconConfiguration {

    enum SILICON_CONFIG {
        None = 0,         // empty config
        RunSpring2017,    // 1 station - (geometry file: Silicon_RunSpring2017.root) (BM@N RUN-6)
        RunSpring2018,    // 3 stations - (geometry file: Silicon_RunSpring2018.root) (BM@N RUN-7)
        RunSRCSpring2018, // 3 stations - (geometry file: Silicon_RunSRCSpring2018.root) (BM@N RUN-7: SRC)
        FutureConfig2020,  // 3 stations - (geometry files: Silicon_FutureConfig2020.root and Silicon_FutureConfig2020_detailed.root)
        FutureConfig2021  // 4 stations - (geometry files: Silicon_FutureConfig2021.root and Silicon_FutureConfig2021_detailed.root)
    };
}

#endif
