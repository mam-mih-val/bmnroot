#ifndef BMNGEMSTRIPSTATIONSET_RUNWINTER2016_H
#define	BMNGEMSTRIPSTATIONSET_RUNWINTER2016_H

#include "BmnGemStripStationSet.h"
#include "BmnGemStripStation.h"
#include "BmnGemStripConfiguration.h"

using namespace TMath;

class BmnGemStripStationSet_RunWinter2016 : public BmnGemStripStationSet {

public:

    BmnGemStripStationSet_RunWinter2016(BmnGemStripConfiguration::GEM_CONFIG config);
    virtual ~BmnGemStripStationSet_RunWinter2016();

private:
    BmnGemStripStationSet_RunWinter2016(const BmnGemStripStationSet&) = delete;
    BmnGemStripStationSet_RunWinter2016& operator=(const BmnGemStripStationSet&) = delete;

    void DefineBeamHoleRadiuses();
    void BuildStations();

private:
    BmnGemStripConfiguration::GEM_CONFIG fCurrentConfig;

    ClassDef(BmnGemStripStationSet_RunWinter2016, 1);
};



#endif
