#ifndef BMNGEMSTRIPSTATION_V3_H
#define	BMNGEMSTRIPSTATION_V3_H

#include "Rtypes.h"

#include "BmnGemStripReadoutModule.h"

class BmnGemStripStation {
//private:
public:

    friend class BmnGemStripStationSet;

    static const Int_t MaxModulesInStation = 50;
    Int_t NExistentModules;

/* Station parameters */
    Int_t StationNumber;

    Double_t XSizeStation;
    Double_t YSizeStation;
    Double_t ZSizeStation;

    Double_t ZPosition;
    Double_t BeamPipeRadius;

    Double_t XCenterStation;
    Double_t YCenterStation;

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
    Double_t StripAngle; // minus - clochwise slope, plus - anticlockwise slope

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


/* Readout modules */
    BmnGemStripReadoutModule* ReadoutModules[MaxModulesInStation];


public:

    BmnGemStripStation(Int_t iStation,
                       Double_t zpos_station, Double_t beamradius);

    virtual ~BmnGemStripStation();

    Int_t GetPointModuleOwhership(Double_t xcoord, Double_t ycoord);
    Int_t AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord, Double_t dEloss);

    Int_t CountNAddedToStationPoints();

    void ProcessPointsInStation();

    Int_t CountNProcessedPointInStation();

    BmnGemStripReadoutModule* GetReadoutModule(Int_t index);
    Int_t GetNModules() { return NExistentModules; }

private:
    BmnGemStripStation(const BmnGemStripStation&);
    BmnGemStripStation& operator=(const BmnGemStripStation&);

    ClassDef(BmnGemStripStation, 1)
};

#endif

