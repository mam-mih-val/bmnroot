#ifndef BMNGEMSTRIPSTATION_H
#define	BMNGEMSTRIPSTATION_H

#include "Rtypes.h"

#include "BmnGemStripReadoutModule.h"

class BmnGemStripStation {
//private:
public:

    friend class BmnGemStripStationSet;

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
    Double_t XSizeGemModule;
    Double_t YSizeGemModule;
    Double_t ZSizeGemModule;
    Double_t dXInnerFrame;
    Double_t dYInnerFrame;
    Double_t dXOuterFrame;
    Double_t dYOuterFrame;

/* Readout module parameters  */
    Double_t ZSizeReadoutModule;

    Double_t XSizeBigReadoutModule;
    Double_t YSizeBigReadoutModule;

    Double_t XSizeSmallReadoutModule[12];
    Double_t YSizeSmallReadoutModule[12];


    Int_t NBigReadoutModules;
    Int_t NSmallReadoutModules;

    Double_t PitchValueBigModule; // cm
    Double_t PitchValueSmallModule; // cm
    Double_t LowerStripWidth; //cm
    Double_t UpperStripWidth; //cm
    Double_t StripAngle; // minus - clochwise slope, plus - anticlockwise slope

/* Parameters of readout modules */
/*
    Module_0 (big left-botton module)
    Module_1 (big right-botton module)
    Module_2 (big right-top module)
    Module_3 (big left-top module)
    Module_4 (inner) (small left-botton module)
    Module_5 (inner) (small right-botton module)
    Module_6 (inner) (small right-top module)
    Module_7 (inner) (small left-top module)
*/
    Double_t XMin_ReadoutModule[8];
    Double_t XMax_ReadoutModule[8];
    Double_t YMin_ReadoutModule[8];
    Double_t YMax_ReadoutModule[8];


/* Readout modules */
    BmnGemStripReadoutModule* ReadoutModules[8];


public:

    BmnGemStripStation(Int_t iStation,
                       Double_t xsize_gem_module, Double_t ysize_gem_module,
                       Double_t zpos_station, Double_t beamradius);

    virtual ~BmnGemStripStation();



    Bool_t CheckPointModuleOwnership(Double_t xcoord, Double_t ycoord, Int_t readout_module);
    Int_t AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord);

    Int_t CountNAddedToStationPoints();

    void ProcessPointsInStation();

    Int_t CountNProcessedPointInStation();

    BmnGemStripReadoutModule* GetReadoutModule(Int_t index);
    Int_t GetNModules() { return NBigReadoutModules+NSmallReadoutModules; }

private:
    BmnGemStripStation(const BmnGemStripStation&);
    BmnGemStripStation& operator=(const BmnGemStripStation&);

    ClassDef(BmnGemStripStation, 1)
};




#endif

