#ifndef BMNGEMSTRIPSTATIONSET_1STCONFIGSHORT_H
#define	BMNGEMSTRIPSTATIONSET_1STCONFIGSHORT_H

#include "BmnGemStripStationSet.h"
#include "BmnGemStripStation.h"

using namespace TMath;

class BmnGemStripStationSet_1stConfigShort : public BmnGemStripStationSet {

public:

    BmnGemStripStationSet_1stConfigShort();

    virtual ~BmnGemStripStationSet_1stConfigShort();

    Int_t GetPointStationOwnership(Double_t zcoord);

    Bool_t AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                              Double_t px, Double_t py, Double_t pz,
                              Double_t dEloss, Int_t refID);
    
    Int_t CountNAddedToDetectorPoints();

    void ProcessPointsInDetector();
    Int_t CountNProcessedPointsInDetector();

private:
    BmnGemStripStationSet_1stConfigShort(const BmnGemStripStationSet&);
    BmnGemStripStationSet_1stConfigShort& operator=(const BmnGemStripStationSet&);

    void DefineBeamHoleRadiuses();
    void BuildStations();


    ClassDef(BmnGemStripStationSet_1stConfigShort, 1);
};

#endif
