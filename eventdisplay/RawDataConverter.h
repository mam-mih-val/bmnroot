#ifndef RAWDATACONVERTER_H
#define RAWDATACONVERTER_H

#include "RawDataParser.h"

#include "TVector3.h"
#include "TClonesArray.h"
#include "TEvePointSet.h"

class RawDataConverter
{
  public:
    Short_t kTimeBin; // ns
    Short_t kNWires; //in one plane
    Float_t kAngleStep; // degrees
    Float_t kWireStep; // cm
    Float_t kPlaneWidth; // cm

    Float_t kMwpcZpos; // z-position of the center of MWPC

    // nWire1 - number of the first wire      //from 0 to 102
    // nWire2 - number of the second wire     //from 0 to 102
    // nPlane1 - number of the first plane    //from 1 to 6
    // nPlane2 - number of the second plane   //from 1 to 6

    /** Default constructor **/
    RawDataConverter();
    /** Destructor **/
    virtual ~RawDataConverter();

    vector<TVector3*> SearchHits(vector<BmnMwpcDigit*> x1, vector<BmnMwpcDigit*> u1, vector<BmnMwpcDigit*> v1, vector<BmnMwpcDigit*> x2, vector<BmnMwpcDigit*> u2, vector<BmnMwpcDigit*> v2);
    vector<TVector3*> MWPCEventToGeoVector(EventData* pEventData);
    void MwpcDigits2MwpcHits(TClonesArray* pMwpcDigits, TClonesArray* pMwpcHits);
    void SetMwpcPosition(int mwpc_number, TVector3 mwpc_position);

    static TEvePointSet* Vector2EvePoints(vector<TVector3*>* pPointVector, TString strSetName, Color_t fColor, Style_t fStyle, Int_t iMarkerSize = 1, bool isDebug = false);

  private:
    TVector3* CalcHitPosByTwoDigits(BmnMwpcDigit* dI, BmnMwpcDigit* dJ);
    vector<TVector3*> CreateHitsByTwoPlanes(vector<BmnMwpcDigit*> x, vector<BmnMwpcDigit*> y);
    TVector3* MwpcPosition[3];
};

#endif // RAWDATACONVERTER_H
