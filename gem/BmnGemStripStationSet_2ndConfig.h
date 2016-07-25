#ifndef BMNGEMSTRIPSTATIONSET_2NDCONFIG_H
#define	BMNGEMSTRIPSTATIONSET_2NDCONFIG_H

#include "BmnGemStripStationSet.h"
#include "BmnGemStripStation.h"

using namespace TMath;

class BmnGemStripStationSet_2ndConfig : public BmnGemStripStationSet {

public:

    BmnGemStripStationSet_2ndConfig();

    virtual ~BmnGemStripStationSet_2ndConfig();

private:
    BmnGemStripStationSet_2ndConfig(const BmnGemStripStationSet&);
    BmnGemStripStationSet_2ndConfig& operator=(const BmnGemStripStationSet&);

    void DefineBeamHoleRadiuses();
    void BuildStations();


    ClassDef(BmnGemStripStationSet_2ndConfig, 1);
};



#endif
