#ifndef BMNGEMSTRIPSTATIONSET_RUNSUMMER2016_H
#define	BMNGEMSTRIPSTATIONSET_RUNSUMMER2016_H

#include "BmnGemStripStationSet.h"
#include "BmnGemStripStation.h"

using namespace TMath;

class BmnGemStripStationSet_RunSummer2016 : public BmnGemStripStationSet {

public:

    BmnGemStripStationSet_RunSummer2016();

    virtual ~BmnGemStripStationSet_RunSummer2016();

    Int_t GetPointStationOwnership(Double_t zcoord);

    Bool_t AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                              Double_t px, Double_t py, Double_t pz,
                              Double_t dEloss, Int_t refID);

    Int_t CountNAddedToDetectorPoints();

    void ProcessPointsInDetector();
    Int_t CountNProcessedPointsInDetector();

private:
    BmnGemStripStationSet_RunSummer2016(const BmnGemStripStationSet&);
    BmnGemStripStationSet_RunSummer2016& operator=(const BmnGemStripStationSet&);

    void DefineBeamHoleRadiuses();
    void BuildStations();


    ClassDef(BmnGemStripStationSet_RunSummer2016, 1);
};



#endif
