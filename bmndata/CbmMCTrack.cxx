// -------------------------------------------------------------------------
// -----                      CbmMCTrack source file                   -----
// -----                  Created 03/08/04  by V. Friese               -----
// -------------------------------------------------------------------------
#include "CbmMCTrack.h"

#include "FairLogger.h"

#include "TParticle.h"
#ifndef ROOT_TParticlePDG
 #include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
 #include "TDatabasePDG.h"
#endif


// -----   Default constructor   -------------------------------------------
CbmMCTrack::CbmMCTrack()
  : TObject(),
    fPdgCode(0),
    fMotherId(-1),
    fPx(0.),
    fPy(0.),
    fPz(0.),
    fStartX(0.),
    fStartY(0.),
    fStartZ(0.),
    fStartT(0.),
    fNPoints(0)
{
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------
CbmMCTrack::CbmMCTrack(Int_t pdgCode, Int_t motherId, Double_t px,
		       Double_t py, Double_t pz, Double_t x, Double_t y,
		       Double_t z, Double_t t, Int_t nPoints = 0)
  : TObject(),
    fPdgCode(pdgCode),
    fMotherId(motherId),
    fPx(px),
    fPy(py),
    fPz(pz),
    fStartX(x),
    fStartY(y),
    fStartZ(z),
    fStartT(t),
    fNPoints(0)
{
  if (nPoints >= 0) fNPoints = nPoints;
  //  else              fNPoints = 0;
}
// -------------------------------------------------------------------------


// -----   Copy constructor   ----------------------------------------------
CbmMCTrack::CbmMCTrack(const CbmMCTrack& track)
  : TObject(track),
    fPdgCode(track.fPdgCode),
    fMotherId(track.fMotherId),
    fPx(track.fPx),
    fPy(track.fPy),
    fPz(track.fPz),
    fStartX(track.fStartX),
    fStartY(track.fStartY),
    fStartZ(track.fStartZ),
    fStartT(track.fStartT),
    fNPoints(track.fNPoints)
{
  //  *this = track;
}
// -------------------------------------------------------------------------


// -----   Constructor from TParticle   ------------------------------------
CbmMCTrack::CbmMCTrack(TParticle* part)
  : TObject(),
    fPdgCode(part->GetPdgCode()),
    fMotherId(part->GetMother(0)),
    fPx(part->Px()),
    fPy(part->Py()),
    fPz(part->Pz()),
    fStartX(part->Vx()),
    fStartY(part->Vy()),
    fStartZ(part->Vz()),
    fStartT(part->T()*1e09),
    fNPoints(0)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmMCTrack::~CbmMCTrack() { }
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void CbmMCTrack::Print(Int_t trackId) const {
  LOG(DEBUG) << "Track " << trackId << ", mother : " << fMotherId
         << ", Type " << fPdgCode << ", momentum (" << fPx << ", "
         << fPy << ", " << fPz << ") GeV" << FairLogger::endl;
  LOG(DEBUG) << "       Ref " << GetNPoints(kREF)
         << ", MVD "  << GetNPoints(kMVD)
         << ", GEM "  << GetNPoints(kGEM)
         << ", TOF1 " << GetNPoints(kTOF1)
         << ", DCH1 " << GetNPoints(kDCH)
         << ", TOF2 " << GetNPoints(kTOF)
         << ", ZDC "  << GetNPoints(kZDC)
         << ", RECOIL "  << GetNPoints(kRECOIL) << FairLogger::endl;
}
// -------------------------------------------------------------------------


// -----   Public method GetMass   -----------------------------------------
Double_t CbmMCTrack::GetMass() const
{
  if ( TDatabasePDG::Instance() ) {
    TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(fPdgCode);
    if ( particle ) return particle->Mass();
    else return 0.;
  }

  return 0.;
}
// -------------------------------------------------------------------------


// -----   Public method GetRapidity   -------------------------------------
Double_t CbmMCTrack::GetRapidity() const
{
  Double_t e = GetEnergy();
  Double_t y = 0.5 * TMath::Log( (e+fPz) / (e-fPz) );

  return y;
}
// -------------------------------------------------------------------------


// -----   Public method GetNPoints   --------------------------------------
Int_t CbmMCTrack::GetNPoints(DetectorId detId) const
{
  if      ( detId == kREF  ) return (  fNPoints &   1);
  else if ( detId == kMVD  ) return ( (fNPoints & (  7  <<  1) ) >>  1);
  else if ( detId == kGEM  ) return ( (fNPoints & ( 15  <<  4) ) >>  4);
  else if ( detId == kTOF1 ) return ( (fNPoints & (  1  <<  8) ) >>  8);
  else if ( detId == kDCH ) return ( (fNPoints & ( 15  <<  9) ) >>  9);
  else if ( detId == kTOF  ) return ( (fNPoints & (  1  << 17) ) >> 17);
  else if ( detId == kZDC  ) return ( (fNPoints & ( 15  << 18) ) >> 18);
  else if ( detId == kECAL  ) return ( (fNPoints & ( 3  << 22) ) >> 22);
  else if ( detId == kBD  ) return ( (fNPoints & ( 1  << 24) ) >> 24);
  else if ( detId == kRECOIL ) return ((fNPoints & ( 63  << 25) ) >> 25);
  else if ( detId == kMWPC ) return 0;
  else if ( detId == kSILICON ) return 0;
  else if ( detId == kSSD ) return 0;
  else {
    LOG(ERROR) << "GetNPoints: Unknown detector ID "
           << detId << FairLogger::endl;
    return 0;
  }
}
// -------------------------------------------------------------------------


// -----   Public method SetNPoints   --------------------------------------
void CbmMCTrack::SetNPoints(Int_t iDet, Int_t nPoints) {

  if ( iDet == kREF ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 1 ) nPoints = 1;
    fNPoints = ( fNPoints & ( ~ 1 ) )  |  nPoints;
  }

  else if ( iDet == kMVD ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 7 ) nPoints = 7;
    fNPoints = ( fNPoints & ( ~ (  7 <<  1 ) ) )  |  ( nPoints <<  1 );
  }

  else if ( iDet == kGEM ) {
    if      ( nPoints <  0 ) nPoints =  0;
    else if ( nPoints > 15 ) nPoints = 15;
    fNPoints = ( fNPoints & ( ~ ( 15 <<  4 ) ) )  |  ( nPoints <<  4 );
  }

  else if ( iDet == kTOF1 ) {
    if      ( nPoints < 0 ) nPoints = 0;
    else if ( nPoints > 1 ) nPoints = 1;
    fNPoints = ( fNPoints & ( ~ (  1 <<  8 ) ) )  |  ( nPoints <<  8 );
  }

  else if ( iDet == kDCH ) {
    if      ( nPoints <  0 ) nPoints =  0;
    else if ( nPoints > 15 ) nPoints = 15;
    fNPoints = ( fNPoints & ( ~ ( 15 <<  9 ) ) )  |  ( nPoints <<  9 );
  }

  else if ( iDet == kTOF ) {
    if      ( nPoints <  0 ) nPoints =  0;
    else if ( nPoints >  1 ) nPoints =  1;
    fNPoints = ( fNPoints & ( ~ (  1 << 17 ) ) )  |  ( nPoints << 17 );
  }

  else if ( iDet == kZDC ) {
    if      ( nPoints <  0 ) nPoints = 0;
    else if ( nPoints > 15) nPoints = 15;
    fNPoints = ( fNPoints & ( ~ (15 << 18 ) ) )  |  ( nPoints << 18 );
  }

  else if ( iDet == kECAL ) {
    if      ( nPoints <  0 ) nPoints = 0;
    else if ( nPoints > 3) nPoints = 3;
    fNPoints = ( fNPoints & ( ~ (3 << 22 ) ) )  |  ( nPoints << 22 );
  }

  else if ( iDet == kBD ) {
    if      ( nPoints <  0 ) nPoints = 0;
    else if ( nPoints > 1) nPoints = 1;
    fNPoints = ( fNPoints & ( ~ (1 << 24 ) ) )  |  ( nPoints << 24 );
  }

  else if ( iDet == kRECOIL) {
    if      ( nPoints < 0  ) nPoints = 0;
    else if ( nPoints > 63 ) nPoints = 63;
    fNPoints = ( fNPoints & ( ~ ( 63 << 25 ) ) )  |  ( nPoints << 25 );
  }

  else if ( iDet == kMWPC) { }

  else if ( iDet == kSILICON) { }

  else if ( iDet == kSSD) { }

  else LOG(ERROR) << "Unknown detector ID "
          << iDet << FairLogger::endl;

}
// -------------------------------------------------------------------------

ClassImp(CbmMCTrack)
