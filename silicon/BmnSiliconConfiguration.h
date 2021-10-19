#ifndef BMNSILICONCONFIGURATION_H
#define BMNSILICONCONFIGURATION_H

namespace BmnSiliconConfiguration {

    enum SILICON_CONFIG {
        None = 0,         // empty config
        RunSpring2017,    // 1 station - (geometry file: Silicon_RunSpring2017.root) (BM@N RUN-6)
        RunSpring2018,    // 3 stations - (geometry file: Silicon_RunSpring2018.root) (BM@N RUN-7)
        RunSRCSpring2018, // 3 stations - (geometry file: Silicon_RunSRCSpring2018.root) (BM@N RUN-7: SRC)
        Run8_3stations,  // 3 stations - (geometry files: Silicon_Run8_3stations.root and Silicon_Run8_3stations_detailed.root)
        Run8_4stations   // 4 stations - (geometry files: Silicon_Run8_4stations.root and Silicon_Run8_4stations_detailed.root)
    };
}

#endif
