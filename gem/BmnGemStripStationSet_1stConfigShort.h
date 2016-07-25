#ifndef BMNGEMSTRIPSTATIONSET_1STCONFIGSHORT_H
#define	BMNGEMSTRIPSTATIONSET_1STCONFIGSHORT_H

#include "BmnGemStripStationSet.h"
#include "BmnGemStripStation.h"

using namespace TMath;

class BmnGemStripStationSet_1stConfigShort : public BmnGemStripStationSet {

public:

    BmnGemStripStationSet_1stConfigShort();

    virtual ~BmnGemStripStationSet_1stConfigShort();

private:
    BmnGemStripStationSet_1stConfigShort(const BmnGemStripStationSet&);
    BmnGemStripStationSet_1stConfigShort& operator=(const BmnGemStripStationSet&);

    void DefineBeamHoleRadiuses();
    void BuildStations();


    ClassDef(BmnGemStripStationSet_1stConfigShort, 1);
};

#endif
