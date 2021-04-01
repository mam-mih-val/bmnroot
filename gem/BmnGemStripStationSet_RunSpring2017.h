#ifndef BMNGEMSTRIPSTATIONSET_RUNSPRING2017_H
#define	BMNGEMSTRIPSTATIONSET_RUNSPRING2017_H

#include "BmnGemStripStationSet.h"
#include "BmnGemStripStation.h"
#include "BmnGemStripConfiguration.h"

using namespace TMath;

class BmnGemStripStationSet_RunSpring2017 : public BmnGemStripStationSet {

public:

    BmnGemStripStationSet_RunSpring2017(BmnGemStripConfiguration::GEM_CONFIG config);
    virtual ~BmnGemStripStationSet_RunSpring2017();

private:
    BmnGemStripStationSet_RunSpring2017(const BmnGemStripStationSet&) = delete;
    BmnGemStripStationSet_RunSpring2017& operator=(const BmnGemStripStationSet&) = delete;

    void DefineBeamHoleRadiuses();
    void BuildStations();

private:
    BmnGemStripConfiguration::GEM_CONFIG fCurrentConfig;

    ClassDef(BmnGemStripStationSet_RunSpring2017, 1);
};

#endif
