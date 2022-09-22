#ifndef BMNSILICONSTATION_H
#define BMNSILICONSTATION_H

#include "BmnSiliconModule.h"
#include "BmnStripData.h"

#include "TDOMParser.h"
#include "TXMLNode.h"
#include "TXMLAttr.h"
#include "TList.h"

#include <iostream>

class BmnSiliconStation
{
  protected:
    /* station parameters */
    Int_t StationNumber;
    Int_t NModules;

    Double_t XMinStation;
    Double_t XMaxStation;
    Double_t YMinStation;
    Double_t YMaxStation;
    Double_t ZMinStation;
    Double_t ZMaxStation;

    Double_t XSize;
    Double_t YSize;
    Double_t ZSize;

    Double_t XPosition;
    Double_t YPosition;
    Double_t ZPosition;

    Double_t RotationAngleDeg;  // rotation angle (deg) for the station (counter-clockwise)
    Double_t RotationCenterX;   // x-rotation center for the station
    Double_t RotationCenterY;   // y-rotation center for the station

    /*Shifts of modules in each station*/
    Double_t *XShiftOfModules;  //[NModules]
    Double_t *YShiftOfModules;  //[NModules]
    Double_t *ZShiftOfModules;  //[NModules]

    BmnSiliconModule **Modules; //[NModules] modules in the station [array]

  public:
    /* Constructor */
    BmnSiliconStation();
    BmnSiliconStation(TXMLNode *stationNode, Int_t iStation,
                       Double_t xpos_station, Double_t ypos_station, Double_t zpos_station);
    /* Destructor */
    virtual ~BmnSiliconStation();

    // Getters
    Int_t GetStationNumber() { return StationNumber; }
    Int_t GetNModules() { return NModules; }
    Double_t GetXMinStation() { return XMinStation; }
    Double_t GetXMaxStation() { return XMaxStation; }
    Double_t GetYMinStation() { return YMinStation; }
    Double_t GetYMaxStation() { return YMaxStation; }
    Double_t GetZMinStation() { return ZMinStation; }
    Double_t GetZMaxStation() { return ZMaxStation; }
    Double_t GetXSize() { return XSize; }
    Double_t GetYSize() { return YSize; }
    Double_t GetZSize() { return ZSize; }
    Double_t GetXPosition() { return XPosition; }
    Double_t GetYPosition() { return YPosition; }
    Double_t GetZPosition() { return ZPosition; }
    Double_t GetXShiftOfModule(Int_t module_num);
    Double_t GetYShiftOfModule(Int_t module_num);
    Double_t GetZShiftOfModule(Int_t module_num);
    BmnSiliconModule* GetModule(Int_t module_num);

    //Reset all data in modules of the station
    void Reset();

    Int_t AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                            Double_t px, Double_t py, Double_t pz,
                            Double_t dEloss, Int_t refID);

    Int_t CountNAddedToStationPoints();

    void ProcessPointsInStation();
    Int_t CountNProcessedPointInStation();

    //which module in the station does a point belong to?
        //zcoord - is unused usually, but if modules in the station are (x,y)-overlapped then zcoord is important
    Int_t GetPointModuleOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord);

    //--------------------------------------------------------------------------

  protected:
    void DefineStationBorders();

  private:
    Bool_t CreateConfigurationFromXMLNode(TXMLNode *node);
    Int_t CountNumberOfModules(TXMLNode *node);
    Bool_t ParseModule(TXMLNode *node, Int_t iModule);
    BmnSiliconLayer ParseLayer(TXMLNode *node, Int_t iLayer, Int_t iModule);
    DeadZoneOfStripLayer ParseDeadZone(TXMLNode *node, Int_t iModule);
    Int_t CountDeadZonePoints(TXMLNode *node);

  ClassDef(BmnSiliconStation, 1);
};

#endif /* BMNSILICONSTATION_H */
