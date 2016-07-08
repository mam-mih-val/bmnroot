#ifndef BMNGEMSTRIPSTATIONSET_1STCONFIG_H
#define	BMNGEMSTRIPSTATIONSET_1STCONFIG_H

#include "BmnGemStripStationSet.h"
#include "BmnGemStripStation.h"

using namespace TMath;

class BmnGemStripStationSet_1stConfig : public BmnGemStripStationSet {

public:

    BmnGemStripStationSet_1stConfig();

    virtual ~BmnGemStripStationSet_1stConfig();

    Int_t GetPointStationOwnership(Double_t zcoord);

    Bool_t AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                              Double_t px, Double_t py, Double_t pz,
                              Double_t dEloss, Int_t refID);
    
    Int_t CountNAddedToDetectorPoints();

    void ProcessPointsInDetector();
    Int_t CountNProcessedPointsInDetector();

private:
    BmnGemStripStationSet_1stConfig(const BmnGemStripStationSet&);
    BmnGemStripStationSet_1stConfig& operator=(const BmnGemStripStationSet&);

    void DefineBeamHoleRadiuses();
    void BuildStations();


    ClassDef(BmnGemStripStationSet_1stConfig, 1);
};



#endif
