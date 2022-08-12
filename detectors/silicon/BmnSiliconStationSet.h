#ifndef BMNSILICONSTATIONSET_H
#define BMNSILICONSTATIONSET_H

#include "BmnSiliconStation.h"
#include "BmnStripData.h"

#include "TDOMParser.h"
#include "TXMLNode.h"
#include "TXMLAttr.h"
#include "TList.h"
#include <TVector3.h>

class BmnSiliconStationSet
{
  protected:
    /* station set parameters*/
    Int_t NStations; // number of stations in the Silicon detector

    Double_t *XStationPositions;        //[NStations] x-position of each station [array]
    Double_t *YStationPositions;        //[NStations] y-position of each station [array]
    Double_t *ZStationPositions;        //[NStations] z-position of each station [array]

    BmnSiliconStation **SiliconStations; //[NStations] Silicon stations [array]
    map <Int_t, TVector3>* fStatShifts;  //->

  public:
    /* Constructor */
    BmnSiliconStationSet();
    BmnSiliconStationSet(TString xml_config_file, map <Int_t, TVector3>* shifts = nullptr);
    /* Destructor */
    virtual ~BmnSiliconStationSet();

    /* Getters */
    Int_t GetNStations() { return NStations; };
    Double_t GetXStationPosition(Int_t station_num);
    Double_t GetYStationPosition(Int_t station_num);
    Double_t GetZStationPosition(Int_t station_num);
    Double_t GetBeamHoleRadius(Int_t station_num){return 0.;}
    BmnSiliconStation* GetSiliconStation(Int_t station_num);
    BmnSiliconStation* GetStation(Int_t station_num) {return GetSiliconStation(station_num);}

    //Reset all data in stations of the station set
    void Reset();

    Bool_t AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                                      Double_t px, Double_t py, Double_t pz,
                                      Double_t dEloss, Int_t refID);

    Int_t CountNAddedToDetectorPoints();

    void ProcessPointsInDetector();
    Int_t CountNProcessedPointsInDetector();

    /* which station does the current point belong to?
     * (OLD version: used in case of parallel stations) */
    Int_t GetPointStationOwnership(Double_t zcoord);

    /* which station does the current point belong to?
     * If you use this function in any outer code, don't forget to invert
     * the 'xcoord' parameter! (into local coordinate system) */
    Int_t GetPointStationOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord);

  private:
    Bool_t CreateConfigurationFromXMLFile(TString xml_config_file);
    Int_t CountNumberOfStations(TXMLNode *node);
    Bool_t ParseStation(TXMLNode *node, Int_t iStation);

  ClassDef(BmnSiliconStationSet, 1);
};

#endif /* BMNSILICONSTATIONSET_H */
