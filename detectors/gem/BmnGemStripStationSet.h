#ifndef BMNGEMSTRIPSTATIONSET_H
#define	BMNGEMSTRIPSTATIONSET_H

#include "BmnGemStripStation.h"
#include "BmnStripData.h"
#include "BmnEnums.h"

#include "TDOMParser.h"
#include "TXMLNode.h"
#include "TXMLAttr.h"
#include "TList.h"
#include <TVector3.h>

class BmnGemStripStationSet : public TObject
{
  protected:
    /* station set parameters*/
    Int_t NStations; //number of stations in the GEM detector

    Double_t *XStationPositions;    //[NStations] x-position of each station [array]
    Double_t *YStationPositions;    //[NStations] y-position of each station [array]
    Double_t *ZStationPositions;    //[NStations] z-position of each station [array]

    Double_t *BeamHoleRadiuses;     //[NStations] beam hole radius of each station [array]

    BmnGemStripStation **GemStations; //[NStations] GEM stations [array]
    map<Int_t, TVector3>* fStatShifts; //->

  public:
    /* Constructor */
    BmnGemStripStationSet();
    BmnGemStripStationSet(TString xml_config_file, map <Int_t, TVector3>* shifts = nullptr);
    BmnGemStripStationSet(Int_t period, BmnSetup stp = kBMNSETUP, map <Int_t, TVector3>* shifts = nullptr);
    /* Destructor */
    virtual ~BmnGemStripStationSet();

    /* Getters */
    Int_t GetNStations() { return NStations; };
    Double_t GetXStationPosition(Int_t station_num);
    Double_t GetYStationPosition(Int_t station_num);
    Double_t GetZStationPosition(Int_t station_num);
    Double_t GetBeamHoleRadius(Int_t station_num);
    BmnGemStripStation* GetGemStation(Int_t station_num);
    BmnGemStripStation* GetStation(Int_t station_num) {return GetGemStation(station_num);}

    //Reset all data in stations of the station set
    void Reset();

    ////If you use this function in any outer code, don't forget to invert the 'xcoord' parameter!
    Bool_t AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                                      Double_t px, Double_t py, Double_t pz,
                                      Double_t dEloss, Int_t refID);

    Int_t CountNAddedToDetectorPoints();

    void ProcessPointsInDetector();
    Int_t CountNProcessedPointsInDetector();

    //which station in the GEM detector does a point belong to? ----------------

        //This is an OLD version of the function being used in case of parallel stations
        Int_t GetPointStationOwnership(Double_t zcoord);

        //If you use this function in any outer code, don't forget to invert the 'xcoord' parameter!
        Int_t GetPointStationOwnership(Double_t xcoord, Double_t ycoord, Double_t zcoord);

    //--------------------------------------------------------------------------

  private:
    Bool_t CreateConfigurationFromXMLFile(TString xml_config_file);
    Int_t CountNumberOfStations(TXMLNode *node);
    Bool_t ParseStation(TXMLNode *node, Int_t iStation);

  ClassDef(BmnGemStripStationSet, 1);
};

#endif
