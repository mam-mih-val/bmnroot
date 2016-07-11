#ifndef BMNGEMSTRIPSTATION_1STCONFIGSHORT_H
#define	BMNGEMSTRIPSTATION_1STCONFIGSHORT_H

#include "BmnGemStripStation.h"

class BmnGemStripStation_1stConfigShort : public BmnGemStripStation {

private:

/* GEM module parameters (GEM module comprises readout module + inactive frames) */
    Double_t dXInnerFrame;
    Double_t dYInnerFrame;
    Double_t dXOuterFrame;
    Double_t dYOuterFrame;

/* Readout module parameters */
    Double_t ZSizeGemModule;
    Double_t ZSizeReadoutModule;

    Double_t PitchValueModule; // cm
    Double_t LowerStripWidth; //cm
    Double_t UpperStripWidth; //cm
    Double_t StripAngle; // angle from a vertical line where a plus value - clockwise

/* Station 0-4 : plane 66x41  */
    Double_t XModuleSize_Plane66x41;
    Double_t YModuleSize_Plane66x41;

    Double_t XHotZoneSize_Plane66x41;
    Double_t YHotZoneSize_Plane66x41;

public:

    BmnGemStripStation_1stConfigShort(Int_t iStation,
                       Double_t xpos_station, Double_t ypos_station, Double_t zpos_station,
                       Double_t beamradius);

    virtual ~BmnGemStripStation_1stConfigShort();

    Int_t GetPointModuleOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord);

    Int_t AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                            Double_t px, Double_t py, Double_t pz,
                            Double_t dEloss, Int_t refID);

    Int_t CountNAddedToStationPoints();

    void ProcessPointsInStation();
    Int_t CountNProcessedPointInStation();

    BmnGemStripReadoutModule* GetReadoutModule(Int_t index);

private:
    BmnGemStripStation_1stConfigShort(const BmnGemStripStation&);
    BmnGemStripStation_1stConfigShort& operator=(const BmnGemStripStation&);

    void BuildModules_One66x41Plane();

    //visual test
    void DrawCreatedStation();

    ClassDef(BmnGemStripStation_1stConfigShort, 1)
};

#endif