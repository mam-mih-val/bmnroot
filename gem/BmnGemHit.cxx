#include "BmnGemHit.h"

using std::cout;
using namespace TMath;

const Float_t z0 = 30.; /// Z-ccordinate of the first GEM-station
const Float_t d = 15.;  /// Distance between GEM-stations
const Float_t delta = 6.; /// for station number calculation

BmnGemHit::BmnGemHit() : FairHit() {

}

BmnGemHit::BmnGemHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
: FairHit(detID, pos, dpos, index) {
    //FIXME!!! Take stations from geonetry!
    const Float_t z = pos.Z();
    if (Abs(30 - z) < delta) fStation = 0;
    else if (Abs(50 - z) < delta) fStation = 1;
    else if (Abs(70 - z) < delta) fStation = 2;
    else if (Abs(90 - z) < delta) fStation = 3;
    else if (Abs(110 - z) < delta) fStation = 4;
    else if (Abs(140 - z) < delta) fStation = 5;
    else if (Abs(170 - z) < delta) fStation = 6;
    else if (Abs(200 - z) < delta) fStation = 7;
    else if (Abs(240 - z) < delta) fStation = 8;
    else if (Abs(280 - z) < delta) fStation = 9;
    else if (Abs(320 - z) < delta) fStation = 10;
    else if (Abs(360 - z) < delta) fStation = 11;
    else fStation = -1;
    
    fUsing = kFALSE;
    fChange = kFALSE;
}

BmnGemHit::~BmnGemHit() {

}

ClassImp(BmnGemHit)