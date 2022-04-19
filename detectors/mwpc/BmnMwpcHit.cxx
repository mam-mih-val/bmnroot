#include "BmnMwpcHit.h"

using std::cout;
using namespace TMath;

const Float_t z0 = 30.; /// Z-ccordinate of the first GEM-station
const Float_t d = 15.;  /// Distance between GEM-stations
const Float_t delta = 6.; /// for station number calculation

BmnMwpcHit::BmnMwpcHit() : BmnHit() {
fWireNumber = -1;


}

BmnMwpcHit::BmnMwpcHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
: BmnHit(detID, pos, dpos, index) {
    fUsing = kFALSE;
}

BmnMwpcHit::~BmnMwpcHit() {

}

ClassImp(BmnMwpcHit)
