#ifndef BMNGEMSTRIPSTATIONSET_H
#define	BMNGEMSTRIPSTATIONSET_H

#include "BmnGemStripStation.h"

class BmnGemStripStationSet {

protected:

    /* station set parameters*/
    Int_t NStations; //number of stations in the GEM detector

    Double_t *XStationPositions; //x-position of each station [array]
    Double_t *YStationPositions; //y-position of each station [array]
    Double_t *ZStationPositions; //z-position of each station [array]

    Double_t *BeamHoleRadiuses; //beam hole radius of each station [array]

    BmnGemStripStation **GemStations; //GEM stations [array]

public:

    /* Constructor */
    BmnGemStripStationSet();

    /* Destructor */
    virtual ~BmnGemStripStationSet() { }

    /* Getters */
    Int_t GetNStations() { return NStations; };
    Double_t GetXStationPosition(Int_t station_num);
    Double_t GetYStationPosition(Int_t station_num);
    Double_t GetZStationPosition(Int_t station_num);
    Double_t GetBeamHoleRadius(Int_t station_num);
    BmnGemStripStation* GetGemStation(Int_t station_num);

    //Reset all data in stations of the station set
    void Reset();

    Bool_t AddPointToDetector(Double_t xcoord, Double_t ycoord, Double_t zcoord,
                                      Double_t px, Double_t py, Double_t pz,
                                      Double_t dEloss, Int_t refID);

    Int_t CountNAddedToDetectorPoints();

    void ProcessPointsInDetector();
    Int_t CountNProcessedPointsInDetector();

    //to which station in the GEM detector a point belong?
    Int_t GetPointStationOwnership(Double_t zcoord);

    ClassDef(BmnGemStripStationSet, 1);
};

//Exeptions --------------------------------------------------------------------
class StationSet_Exception {
public:
    StationSet_Exception(TString message) {
        std::cout << "StationSet_Exception::" << message << "\n";
    }
};
//------------------------------------------------------------------------------

#endif
