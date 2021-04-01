#ifndef BMNGEMSTRIPSTATION_RUNSPRING2017_H
#define	BMNGEMSTRIPSTATION_RUNSPRING2017_H

#include "BmnGemStripStation.h"
#include "BmnGemStripConfiguration.h"

class BmnGemStripStation_RunSpring2017 : public BmnGemStripStation {

private:

    BmnGemStripConfiguration::GEM_CONFIG fCurrentConfig;

/* Readout module parameters */
    Double_t LowerLayerStripAngle; //angle from a vertical line where a plus value - clockwise
    Double_t UpperLayerStripAngle; //angle from a vertical line where a plus value - clockwise
    Double_t LowerLayerPitch; //cm
    Double_t UpperLayerPitch; //cm

    ElectronDriftDirectionInModule EDriftDirection;


/* Station 0-2 : plane 66x41  */
/* Station 3 : two plane 66x41  */
    Double_t XModuleSize_Plane66x41;
    Double_t YModuleSize_Plane66x41;

    Double_t XHotZoneSize_Plane66x41;
    Double_t YHotZoneSize_Plane66x41;

/* Station 4-5 : plane 163x45 */
    Double_t XModuleSize_Plane163x45; // x size of module that is a half part of the plane
    Double_t YModuleSize_Plane163x45; // y size of module that is a half part of the plane

    Double_t XRectHotZoneSize_Plane163x45; // x size of a rectangle hot zone
    Double_t YRectHotZoneSize_Plane163x45; // y size of a rectangle hot zone

    Double_t XSlopeHotZoneSize_Plane163x45[2]; // x size of a slope hot zone (xmin_size, xmax_size)
    Double_t YSlopeHotZoneSize_Plane163x45;    // y size of a slope hot zone

public:

    BmnGemStripStation_RunSpring2017(Int_t iStation,
                       Double_t xpos_station, Double_t ypos_station, Double_t zpos_station,
                       Double_t beamradius, BmnGemStripConfiguration::GEM_CONFIG config);

    virtual ~BmnGemStripStation_RunSpring2017();

private:
    BmnGemStripStation_RunSpring2017(const BmnGemStripStation&) = delete;
    BmnGemStripStation_RunSpring2017& operator=(const BmnGemStripStation&) = delete;

    void BuildModules_One66x41Plane();
    void BuildModules_Two66x41Plane();
    void BuildModules_One163x45Plane();

    ClassDef(BmnGemStripStation_RunSpring2017, 1);
};

#endif
