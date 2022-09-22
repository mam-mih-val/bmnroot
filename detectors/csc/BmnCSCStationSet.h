#ifndef BMNCSCSTATIONSET_H
#define	BMNCSCSTATIONSET_H

#include "BmnCSCStation.h"
#include "BmnStripData.h"

#include "TDOMParser.h"
#include "TXMLNode.h"
#include "TXMLAttr.h"
#include "TList.h"
#include <TVector3.h>

class BmnCSCStationSet
{
  protected:
    /* station set parameters*/
    Int_t NStations; // number of stations in the detector

    Double_t *XStationPositions; //[NStations] x-position of each station [array]
    Double_t *YStationPositions; //[NStations] y-position of each station [array]
    Double_t *ZStationPositions; //[NStations] z-position of each station [array]

    BmnCSCStation **Stations;    //[NStations] Stations [array]
    map <Int_t, TVector3>* fStatShifts;

public:
    /* Constructor */
    BmnCSCStationSet();

    BmnCSCStationSet(TString xml_config_file, map <Int_t, TVector3>* shifts = nullptr);

    /* Destructor */
    virtual ~BmnCSCStationSet();

    /* Getters */
    Int_t GetNStations() { return NStations; };
    Double_t GetXStationPosition(Int_t station_num);
    Double_t GetYStationPosition(Int_t station_num);
    Double_t GetZStationPosition(Int_t station_num);
    BmnCSCStation* GetStation(Int_t station_num);

    //Reset all data in stations of the station set
    void Reset();

    ////If you use this function in any outer code, don't forget to invert the 'xcoord' parameter!
    Bool_t AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                              Double_t px, Double_t py, Double_t pz,
                              Double_t dEloss, Int_t refID);

    Int_t CountNAddedToDetectorPoints();

    void ProcessPointsInDetector();
    Int_t CountNProcessedPointsInDetector();

    //which station in the detector does a point belong to? ----------------

        //This is an OLD version of the function being used in case of parallel stations
        Int_t GetPointStationOwnership(Double_t zcoord);

        //If you use this function in any outer code, don't forget to invert the 'xcoord' parameter!
        Int_t GetPointStationOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord);

    //--------------------------------------------------------------------------

private:

    Bool_t CreateConfigurationFromXMLFile(TString xml_config_file);
    Int_t CountNumberOfStations(TXMLNode *node);
    Bool_t ParseStation(TXMLNode *node, Int_t iStation);

    ClassDef(BmnCSCStationSet, 1);
};

#endif
