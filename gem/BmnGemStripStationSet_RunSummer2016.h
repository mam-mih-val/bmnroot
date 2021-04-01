#ifndef BMNGEMSTRIPSTATIONSET_RUNSUMMER2016_H
#define	BMNGEMSTRIPSTATIONSET_RUNSUMMER2016_H

#include "BmnGemStripStationSet.h"
#include "BmnGemStripStation.h"
#include "BmnGemStripConfiguration.h"

using namespace TMath;

class BmnGemStripStationSet_RunSummer2016 : public BmnGemStripStationSet {

public:

    BmnGemStripStationSet_RunSummer2016(BmnGemStripConfiguration::GEM_CONFIG config);
    virtual ~BmnGemStripStationSet_RunSummer2016();

private:
    BmnGemStripStationSet_RunSummer2016(const BmnGemStripStationSet&) = delete;
    BmnGemStripStationSet_RunSummer2016& operator=(const BmnGemStripStationSet&) = delete;

    void DefineBeamHoleRadiuses();
    void BuildStations();
    
private:
    BmnGemStripConfiguration::GEM_CONFIG fCurrentConfig;

    ClassDef(BmnGemStripStationSet_RunSummer2016, 1);
};



#endif
