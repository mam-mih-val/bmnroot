#ifndef BMNGEMSTRIPSTATIONSET_2NDCONFIG_H
#define	BMNGEMSTRIPSTATIONSET_2NDCONFIG_H

#include "BmnGemStripStationSet.h"
#include "BmnGemStripStation.h"

using namespace TMath;

class BmnGemStripStationSet_2ndConfig : public BmnGemStripStationSet {

public:

    BmnGemStripStationSet_2ndConfig();

    virtual ~BmnGemStripStationSet_2ndConfig();

    Int_t GetPointStationOwnership(Double_t zcoord);

    Bool_t AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                              Double_t px, Double_t py, Double_t pz,
                              Double_t dEloss, Int_t refID);
    
    Int_t CountNAddedToDetectorPoints();

    void ProcessPointsInDetector();
    Int_t CountNProcessedPointsInDetector();

private:
    BmnGemStripStationSet_2ndConfig(const BmnGemStripStationSet&);
    BmnGemStripStationSet_2ndConfig& operator=(const BmnGemStripStationSet&);

    void DefineBeamHoleRadiuses();
    void BuildStations();


    ClassDef(BmnGemStripStationSet_2ndConfig, 1);
};



#endif
