#ifndef BMNGEMSTRIPSTATION_RUNSUMMER2016_H
#define	BMNGEMSTRIPSTATION_RUNSUMMER2016_H

#include "BmnGemStripStation.h"
#include "BmnGemStripConfiguration.h"

#include <sstream>

class BmnGemStripStation_RunSummer2016 : public BmnGemStripStation {

private:

    BmnGemStripConfiguration::GEM_CONFIG fCurrentConfig;

/* Readout module parameters */
    Double_t ZSizeModuleWithFrames;
    Double_t ZSizeModule;

    Double_t LowerLayerStripAngle; //angle from a vertical line where a plus value - clockwise
    Double_t UpperLayerStripAngle; //angle from a vertical line where a plus value - clockwise
    Double_t LowerLayerPitch; //cm
    Double_t UpperLayerPitch; //cm

    ElectronDriftDirectionInModule EDriftDirection;

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
                       Double_t beamradius, BmnGemStripConfiguration::GEM_CONFIG config);

    virtual ~BmnGemStripStation_RunSummer2016();

private:
    BmnGemStripStation_RunSummer2016(const BmnGemStripStation&) = delete;
    BmnGemStripStation_RunSummer2016& operator=(const BmnGemStripStation&) = delete;

    void BuildModules_One10x10Plane();
    void BuildModules_One66x41Plane();
    void BuildModules_One163x45Plane();

    ClassDef(BmnGemStripStation_RunSummer2016, 1);
};

#endif
