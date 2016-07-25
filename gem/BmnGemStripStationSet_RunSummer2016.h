#ifndef BMNGEMSTRIPSTATIONSET_RUNSUMMER2016_H
#define	BMNGEMSTRIPSTATIONSET_RUNSUMMER2016_H

#include "BmnGemStripStationSet.h"
#include "BmnGemStripStation.h"

using namespace TMath;

class BmnGemStripStationSet_RunSummer2016 : public BmnGemStripStationSet {

public:

    BmnGemStripStationSet_RunSummer2016();

    virtual ~BmnGemStripStationSet_RunSummer2016();

private:
    BmnGemStripStationSet_RunSummer2016(const BmnGemStripStationSet&);
    BmnGemStripStationSet_RunSummer2016& operator=(const BmnGemStripStationSet&);

    void DefineBeamHoleRadiuses();
    void BuildStations();


    ClassDef(BmnGemStripStationSet_RunSummer2016, 1);
};



#endif
