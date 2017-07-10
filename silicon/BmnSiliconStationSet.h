#ifndef BMNSILICONSTATIONSET_H
#define BMNSILICONSTATIONSET_H

#include "BmnSiliconStation.h"

#include "TDOMParser.h"
#include "TXMLNode.h"
#include "TXMLAttr.h"
#include "TList.h"

class BmnSiliconStationSet {

protected:

    /* station set parameters*/
    Int_t NStations; //number of stations in the Silicon detector

    Double_t *XStationPositions; //x-position of each station [array]
    Double_t *YStationPositions; //y-position of each station [array]
    Double_t *ZStationPositions; //z-position of each station [array]

    BmnSiliconStation **SiliconStations; //Silicon stations [array]

public:

    /* Constructor */
    BmnSiliconStationSet();

    BmnSiliconStationSet(TString xml_config_file);

    /* Destructor */
    virtual ~BmnSiliconStationSet();

    /* Getters */
    Int_t GetNStations() { return NStations; };
    Double_t GetXStationPosition(Int_t station_num);
    Double_t GetYStationPosition(Int_t station_num);
    Double_t GetZStationPosition(Int_t station_num);
    Double_t GetBeamHoleRadius(Int_t station_num);
    BmnSiliconStation* GetSiliconStation(Int_t station_num);

    //Reset all data in stations of the station set
    void Reset();

    Bool_t AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                                      Double_t px, Double_t py, Double_t pz,
                                      Double_t dEloss, Int_t refID);

    Int_t CountNAddedToDetectorPoints();

    void ProcessPointsInDetector();
    Int_t CountNProcessedPointsInDetector();

    //which station in the Silicon detector does a point belong to?
    Int_t GetPointStationOwnership(Double_t zcoord);

private:

    Bool_t CreateConfigurationFromXMLFile(TString xml_config_file);
    Int_t CountNumberOfStations(TXMLNode *node);
    Bool_t ParseStation(TXMLNode *node, Int_t iStation);

    ClassDef(BmnSiliconStationSet, 1);

};

//Exeptions --------------------------------------------------------------------
class StationSet_Exception {
public:
    StationSet_Exception(TString message) {
        std::cerr << "StationSet_Exception::" << message << "\n";
    }
};
//------------------------------------------------------------------------------

#endif /* BMNSILICONSTATIONSET_H */

