#ifndef BMNGEMSTRIPSTATION_H
#define	BMNGEMSTRIPSTATION_H

//#include "BmnGemStripReadoutModule.h"
#include "BmnGemStripModule.h"

#include "TString.h"

class BmnGemStripStation {

protected:

    /* station parameters */
    Int_t StationNumber;
    Int_t NModules;

    Double_t XSize;
    Double_t YSize;
    Double_t ZSize;

    Double_t XPosition;
    Double_t YPosition;
    Double_t ZPosition;

    /*Shifts of modules in each station*/
    Double_t *XShiftOfModules;
    Double_t *YShiftOfModules;
    Double_t *ZShiftOfModules;

    Double_t BeamHoleRadius;

    BmnGemStripModule **Modules; //modules in the station [array]

public:

    /* Constructor */
    BmnGemStripStation();

    /* Destructor */
    virtual ~BmnGemStripStation() { }

    //Getters
    Int_t GetStationNumber() { return StationNumber; }
    Int_t GetNModules() { return NModules; }
    Double_t GetXSize() { return XSize; }
    Double_t GetYSize() { return YSize; }
    Double_t GetZSize() { return ZSize; }
    Double_t GetXPosition() { return XPosition; }
    Double_t GetYPosition() { return YPosition; }
    Double_t GetZPosition() { return ZPosition; }
    Double_t GetXShiftOfModule(Int_t module_num);
    Double_t GetYShiftOfModule(Int_t module_num);
    Double_t GetZShiftOfModule(Int_t module_num);
    Double_t GetBeamHoleRadius() { return BeamHoleRadius; }
    BmnGemStripModule* GetModule(Int_t module_num);

    //Reset all data in modules of the station
    void Reset();

    Int_t AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                                    Double_t px, Double_t py, Double_t pz,
                                    Double_t dEloss, Int_t refID);

    Int_t CountNAddedToStationPoints();

    void ProcessPointsInStation();
    Int_t CountNProcessedPointInStation();

    //Pure virtual methods (must be defined in derived classes) ---------------

    //to which module in the station a point belong?
        //zcoord - is unused usually, but if modules in the station are (x,y)-overlapped then zcoord is important
    virtual Int_t GetPointModuleOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord) = 0;

    //--------------------------------------------------------------------------

    ClassDef(BmnGemStripStation, 1)
};

//Exeptions --------------------------------------------------------------------
class Station_Exception {
public:
    Station_Exception(TString message) {
        std::cout << "Station_Exception::" << message << "\n";
    }
};
//------------------------------------------------------------------------------

#endif