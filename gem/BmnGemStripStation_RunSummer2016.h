#ifndef BMNGEMSTRIPSTATION_RUNSUMMER2016_H
#define	BMNGEMSTRIPSTATION_RUNSUMMER2016_H

#include "BmnGemStripStation.h"

class BmnGemStripStation_RunSummer2016 : public BmnGemStripStation {

private:

/* GEM module parameters (GEM module comprises readout module + inactive frames) */
    Double_t dXInnerFrame;
    Double_t dYInnerFrame;
    Double_t dXOuterFrame;
    Double_t dYOuterFrame;

/* Readout module parameters */
    Double_t ZSizeGemModule;
    Double_t ZSizeReadoutModule;

    Double_t PitchValueModule; //cm
    Double_t LowerStripWidth; //cm
    Double_t UpperStripWidth; //cm
    Double_t StripAngle; // angle from a vertical line where a plus value - clockwise
    ElectronDriftDirectionInModule EDriftDirection;
    StripNumberingDirection LowerStripDirection;
    StripNumberingDirection UpperStripDirection;
    StripBorderPoint LeftStripBorder;
    StripBorderPoint RightStripBorder;

/* Station 0 : plane 10x10  */
    Double_t XModuleSize_Plane10x10;
    Double_t YModuleSize_Plane10x10;

/* Station 1-5 : plane 66x41  */
    Double_t XModuleSize_Plane66x41;
    Double_t YModuleSize_Plane66x41;

    Double_t XHotZoneSize_Plane66x41;
    Double_t YHotZoneSize_Plane66x41;

/* Station 6 : plane 163x45 */
    Double_t XModuleSize_Plane163x45; // x size of module that is a half part of the plane
    Double_t YModuleSize_Plane163x45; // y size of module that is a half part of the plane

    Double_t XRectHotZoneSize_Plane163x45; // x size of a rectangle hot zone
    Double_t YRectHotZoneSize_Plane163x45; // y size of a rectangle hot zone

    Double_t XSlopeHotZoneSize_Plane163x45[2]; // x size of a slope hot zone (xmin_size, xmax_size)
    Double_t YSlopeHotZoneSize_Plane163x45;    // y size of a slope hot zone

public:

    BmnGemStripStation_RunSummer2016(Int_t iStation,
                       Double_t xpos_station, Double_t ypos_station, Double_t zpos_station,
                       Double_t beamradius);

    virtual ~BmnGemStripStation_RunSummer2016();

    Int_t GetPointModuleOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord);

    Int_t AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                            Double_t px, Double_t py, Double_t pz,
                            Double_t dEloss, Int_t refID);

    Int_t CountNAddedToStationPoints();

    void ProcessPointsInStation();
    Int_t CountNProcessedPointInStation();

    BmnGemStripReadoutModule* GetReadoutModule(Int_t index);

private:
    BmnGemStripStation_RunSummer2016(const BmnGemStripStation&);
    BmnGemStripStation_RunSummer2016& operator=(const BmnGemStripStation&);

    void BuildModules_One10x10Plane();
    void BuildModules_One66x41Plane();
    void BuildModules_One163x45Plane();

    ClassDef(BmnGemStripStation_RunSummer2016, 1);
};

#endif