#ifndef BMNGEMSTRIPSTATION_H
#define	BMNGEMSTRIPSTATION_H

#include "BmnGemStripReadoutModule.h"

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

    Double_t BeamHoleRadius;

    BmnGemStripReadoutModule **ReadoutModules; //modules in the station [array]

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
    Double_t GetBeamHoleRadius() { return BeamHoleRadius; }
    BmnGemStripReadoutModule* GetReadoutModule(Int_t module_num);

    /* Pure virtual methods (must be defined in derived classes) */

    //to which module in the station a point belong?
    virtual Int_t GetPointModuleOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord) = 0;

    virtual Int_t AddPointToStation(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                                    Double_t px, Double_t py, Double_t pz,
                                    Double_t dEloss, Int_t refID) = 0;

    virtual Int_t CountNAddedToStationPoints() = 0;

    virtual void ProcessPointsInStation() = 0;
    virtual Int_t CountNProcessedPointInStation()= 0;



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