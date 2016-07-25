#ifndef BMNGEMSTRIPSTATION_1STCONFIG_H
#define	BMNGEMSTRIPSTATION_1STCONFIG_H

#include "BmnGemStripStation.h"

class BmnGemStripStation_1stConfig : public BmnGemStripStation {

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

/* Station 0-3 : plane 66x41  */
    Double_t XModuleSize_Plane66x41;
    Double_t YModuleSize_Plane66x41;

    Double_t XHotZoneSize_Plane66x41;
    Double_t YHotZoneSize_Plane66x41;

    Double_t XIntersectionSize_Plane66x41;

/* Station 4-5 : plane 163x45 */
    Double_t XModuleSize_Plane163x45; // x size of module that is a half part of the plane
    Double_t YModuleSize_Plane163x45; // y size of module that is a half part of the plane

    Double_t XRectHotZoneSize_Plane163x45; // x size of a rectangle hot zone
    Double_t YRectHotZoneSize_Plane163x45; // y size of a rectangle hot zone

    Double_t XSlopeHotZoneSize_Plane163x45[2]; // x size of a slope hot zone (xmin_size, xmax_size)
    Double_t YSlopeHotZoneSize_Plane163x45;    // y size of a slope hot zone

public:

    BmnGemStripStation_1stConfig(Int_t iStation,
                       Double_t xpos_station, Double_t ypos_station, Double_t zpos_station,
                       Double_t beamradius);

    virtual ~BmnGemStripStation_1stConfig();

    virtual Int_t GetPointModuleOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord);

private:
    BmnGemStripStation_1stConfig(const BmnGemStripStation&);
    BmnGemStripStation_1stConfig& operator=(const BmnGemStripStation&);

    void BuildModules_One66x41Plane();
    void BuildModules_TwoIntersec66x41Plane();
    void BuildModules_163x45Plane();

    ClassDef(BmnGemStripStation_1stConfig, 1)
};

#endif