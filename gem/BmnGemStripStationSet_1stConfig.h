#ifndef BMNGEMSTRIPSTATIONSET_1STCONFIG_H
#define	BMNGEMSTRIPSTATIONSET_1STCONFIG_H

#include "BmnGemStripStationSet.h"
#include "BmnGemStripStation.h"

using namespace TMath;

class BmnGemStripStationSet_1stConfig : public BmnGemStripStationSet {

public:

    BmnGemStripStationSet_1stConfig();

    virtual ~BmnGemStripStationSet_1stConfig();

private:
    BmnGemStripStationSet_1stConfig(const BmnGemStripStationSet&);
    BmnGemStripStationSet_1stConfig& operator=(const BmnGemStripStationSet&);

    void DefineBeamHoleRadiuses();
    void BuildStations();


    ClassDef(BmnGemStripStationSet_1stConfig, 1);
};



#endif
