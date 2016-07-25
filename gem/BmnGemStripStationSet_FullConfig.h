#ifndef BMNGEMSTRIPSTATIONSET_FULLCONFIG_H
#define	BMNGEMSTRIPSTATIONSET_FULLCONFIG_H

#include "BmnGemStripStationSet.h"
#include "BmnGemStripStation.h"

using namespace TMath;

class BmnGemStripStationSet_FullConfig : public BmnGemStripStationSet {

private:

    Double_t BeamPipeMinRadius; // radius of the hole in the first GEM-station
    Double_t BeamPipeMaxRadius; // radius of the hole in the last GEM-station

public:

    BmnGemStripStationSet_FullConfig();

    virtual ~BmnGemStripStationSet_FullConfig();

private:
    BmnGemStripStationSet_FullConfig(const BmnGemStripStationSet&);
    BmnGemStripStationSet_FullConfig& operator=(const BmnGemStripStationSet&);

    void DefineBeamHoleRadiuses();
    void BuildStations();


    ClassDef(BmnGemStripStationSet_FullConfig, 1);
};



#endif
