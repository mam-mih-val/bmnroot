#ifndef BMNGEMSTRIPSTATION_FULLCONFIG_H
#define	BMNGEMSTRIPSTATION_FULLCONFIG_H

#include "BmnGemStripStation.h"

class BmnGemStripStation_FullConfig : public BmnGemStripStation {

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

/* Stations (4-zone type) are 0, 1, 2, 3 station */
    Double_t XZoneSizes_Station[4][4]; // x sizes for each station divided into 1, 2, 3, 4 zone
    Double_t YZoneSizes_Station[4][4]; // y sizes for each station divided into 1, 2, 3, 4 zones

/* Station (prototype) is 4 station */
    Double_t XModuleSize_StationPrototype;
    Double_t YModuleSize_StationPrototype;

    Double_t XHotZoneSize_StationPrototype;
    Double_t YHotZoneSize_StationPrototype;

/* Stations (2-part type) are 5 - 11 station */ //indices  0-11 (but 0-5 are zero)
    Double_t XModuleSize_Station[12]; // x size for each station
    Double_t YModuleSize_Station[12]; // y size for each station

    Double_t XRectHotZoneSize_Station[12]; // x size of a rect hot zone for each station
    Double_t YRectHotZoneSize_Station[12]; // y size of a rect hot zone for each station

    Double_t XSlopeHotZoneSize_Station[12][2]; // x size of a slope hot zone for each station (xmin_size, xmax_size)
    Double_t YSlopeHotZoneSize_Station[12];    // y size of a slope hot zone for each station

public:

    BmnGemStripStation_FullConfig(Int_t iStation,
                       Double_t xpos_station, Double_t ypos_station, Double_t zpos_station,
                       Double_t beamradius);

    virtual ~BmnGemStripStation_FullConfig();

    virtual Int_t GetPointModuleOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord);

    BmnGemStripReadoutModule* GetReadoutModule(Int_t index);

private:
    BmnGemStripStation_FullConfig(const BmnGemStripStation&);
    BmnGemStripStation_FullConfig& operator=(const BmnGemStripStation&);

    void BuildModules_4ZoneType();
    void BuildModules_Prototype();
    void BuildModules_2PartType();

    ClassDef(BmnGemStripStation_FullConfig, 1)
};

#endif