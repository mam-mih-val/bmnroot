#ifndef BMNGEMSTRIPSTATIONSET_V3_H
#define	BMNGEMSTRIPSTATIONSET_V3_H

#include "Rtypes.h"
#include "TMath.h"

#include "BmnGemStripStation_v3.h"

using namespace TMath;

class BmnGemStripStationSet {
private:

    Int_t NStations;

    Double_t *ZStantionPositions;

    Double_t BeamPipeMinRadius; // radius of the hole in the first GEM-station
    Double_t BeamPipeMaxRadius; // radius of the hole in the last GEM-station
    Double_t *BeamPipeRadiuses;

    BmnGemStripStation *GemStations[12];

public:

    BmnGemStripStationSet();

    virtual ~BmnGemStripStationSet();

    Int_t GetPointStationOwnership(Double_t zcoord);

    Bool_t AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord, Double_t dEloss, Int_t refID);

    Int_t CountNAddedToDetectorPoints();

    void ProcessPointsInDetector();

    Int_t CountNProcessedPointsInDetector();

    BmnGemStripStation* GetGemStation(Int_t index);
    Int_t GetNStations() { return NStations; }

private:
    BmnGemStripStationSet(const BmnGemStripStationSet&);
    BmnGemStripStationSet& operator=(const BmnGemStripStationSet&);

    void CalculateBeamPipeRadiuses();
    void BuildStations();


    ClassDef(BmnGemStripStationSet, 1);
};



#endif
