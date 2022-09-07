// -------------------------------------------------------------------------
// -----                      CbmMCTrack source file                   -----
// -----                  should be replaced by BmnMCTrack             -----
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
   fPolar[0] = fPolar[1] = fPolar[2] = 0.0; //AZ-310822
}

// -----   Standard constructor   ------------------------------------------
CbmMCTrack::CbmMCTrack(Int_t pdgCode, Int_t motherId, Double_t px,
		       Double_t py, Double_t pz, Double_t x, Double_t y,
               Double_t z, Double_t t, Long64_t nPoints = 0)
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
   fPolar[0] = fPolar[1] = fPolar[2] = 0.0; //AZ-310822
}

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
   for (Int_t i = 0; i < 3; ++i) fPolar[i] = track.fPolar[i]; //AZ-310822
}

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
   //AZ-310822
   TVector3 polar3;
   part->GetPolarisation(polar3);
   for (Int_t i = 0; i < 3; ++i) fPolar[i] = polar3[i];
}

// -----   Destructor   ----------------------------------------------------
CbmMCTrack::~CbmMCTrack()
{
}


// -----   Public method Print   -------------------------------------------
void CbmMCTrack::Print(Int_t trackId) const
{
    LOG(debug) << "Track " << trackId << ", mother : " << fMotherId << ", Type " << fPdgCode
               << ", momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV";

    LOG(debug) << "       Ref " << GetNPoints(kREF)
               << ", BD "    << GetNPoints(kBD)
               << ", GEM "   << GetNPoints(kGEM)
               << ", TOF1 "  << GetNPoints(kTOF1)
               << ", DCH "   << GetNPoints(kDCH)
               << ", TOF2 "  << GetNPoints(kTOF)
               << ", ZDC "   << GetNPoints(kZDC)
               << ", FHCAL " << GetNPoints(kFHCAL)
               << ", NDET "  << GetNPoints(kNDET)
               << ", SSD "   << GetNPoints(kSSD)
               << ", MWPC "  << GetNPoints(kMWPC)
               << ", ECAL "  << GetNPoints(kECAL)
               << ", CSC "   << GetNPoints(kCSC)
               << ", SI "    << GetNPoints(kSILICON)
               << ", LAND "  << GetNPoints(kLAND)
               << ", FD "    << GetNPoints(kFD)
               << ", ARMs "  << GetNPoints(kARMTRIG)
               << ", BC "    << GetNPoints(kBC)
               << ", SCWALL "<< GetNPoints(kSCWALL)
               << ", HODO "  << GetNPoints(kHODO)
               << ", SiMD "  << GetNPoints(kSiMD)
               << ", SiBT "  << GetNPoints(kSiBT);
}


// -----   Public method GetMass   -----------------------------------------
Double_t CbmMCTrack::GetMass() const
{
    if ( TDatabasePDG::Instance() )
    {
        TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(fPdgCode);
        if ( particle ) return particle->Mass();
        else return 0.;
    }

    return 0.;
}

// -----   Public method GetRapidity   -------------------------------------
Double_t CbmMCTrack::GetRapidity() const
{
    Double_t e = GetEnergy();
    Double_t y = 0.5 * TMath::Log( (e+fPz) / (e-fPz) );

    return y;
}


// -----   Public method GetNPoints   --------------------------------------
// TODO: check bitfields & where it is used / filled. Seems not used
Long64_t CbmMCTrack::GetNPoints(DetectorId detId) const
{
    if      ( detId == kREF )  return (  fNPoints &  (Long64_t)  1);
    else if ( detId == kBD )   return ( (fNPoints & ((Long64_t)  1  <<  1) ) >>  1);
    else if ( detId == kGEM )  return ( (fNPoints & ((Long64_t)  7 <<  2) ) >>  2);
    else if ( detId == kTOF1 ) return ( (fNPoints & ((Long64_t)  3 <<  5) ) >>  5);
    else if ( detId == kDCH )  return ( (fNPoints & ((Long64_t) 31 <<  7) ) >>  7);
    else if ( detId == kTOF )  return ( (fNPoints & ((Long64_t)  3 << 12) ) >> 12);
    else if ( detId == kZDC )  return ( (fNPoints & ((Long64_t)127 << 14) ) >> 14);
    else if ( detId == kSSD )  return ( (fNPoints & ((Long64_t)  7 << 21) ) >> 21);
    else if ( detId == kMWPC ) return ( (fNPoints & ((Long64_t) 31 << 24) ) >> 24);
    else if ( detId == kECAL ) return ( (fNPoints & ((Long64_t)127 << 29) ) >> 29);
    else if ( detId == kCSC )  return ( (fNPoints & ((Long64_t)  7 << 36) ) >> 36);
    else if ( detId == kSILICON ) return ( (fNPoints & ((Long64_t)  7 << 39) ) >> 39);
    else if ( detId == kLAND ) return ( (fNPoints & ((Long64_t) 63 << 42) ) >> 42);
    else if ( detId == kFD )   return ( (fNPoints & ((Long64_t)  1 << 48) ) >> 48);
    else if ( detId == kARMTRIG ) return ( (fNPoints & ((Long64_t)  3 << 49) ) >> 49);
    else if ( detId == kBC )   return ( (fNPoints & ((Long64_t)  1 << 51) ) >> 51);
    else if ( detId == kSCWALL )  return ( (fNPoints & ((Long64_t)  1 << 52) ) >> 52);
    else if ( detId == kHODO ) return ( (fNPoints & ((Long64_t)  1 << 53) ) >> 53);
    else if ( detId == kSiMD ) return ( (fNPoints & ((Long64_t)  1 << 54) ) >> 54);
    else if ( detId == kSiBT ) return ( (fNPoints & ((Long64_t)  3 << 55) ) >> 55);
    else if ( detId == kFHCAL ) return ( (fNPoints & ((Long64_t) 1 << 57) ) >> 57);
    else if ( detId == kNDET ) return ( (fNPoints & ((Long64_t)  1 << 58) ) >> 58);
    else if ( detId == kTOFCAL ) return ( -1 );
    else {
        LOG(error) << "GetNPoints: Unknown detector ID " << detId;
        return 0;
    }
}

// -----   Public method SetNPoints   --------------------------------------
void CbmMCTrack::SetNPoints(Int_t iDet, Long64_t nPoints)
{
    if ( iDet == kREF ) {
        if      ( nPoints < 0 ) nPoints =  0;
        else if ( nPoints > 1 ) nPoints =  1;
        fNPoints = ( fNPoints & ( ~ (Long64_t)  1 ) )  |  nPoints;
    }

    else if ( iDet == kBD ) {
        if      ( nPoints < 0 ) nPoints =  0;
        else if ( nPoints > 1 ) nPoints =  1;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  1 <<  1 ) ) )  |  ( nPoints <<  1 );
    }

    else if ( iDet == kGEM ) {
        if      ( nPoints < 0 ) nPoints =  0;
        else if ( nPoints > 7 ) nPoints =  7;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  7 <<  2 ) ) )  |  ( nPoints <<  2 );
    }

    else if ( iDet == kTOF1 ) {
        if      ( nPoints < 0 ) nPoints =  0;
        else if ( nPoints > 3 ) nPoints =  3;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  3 <<  5 ) ) )  |  ( nPoints <<  5 );
    }

    else if ( iDet == kDCH ) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints > 31 ) nPoints = 31;
        fNPoints = ( fNPoints & ( ~ ((Long64_t) 31 <<  7 ) ) )  |  ( nPoints <<  7 );
    }

    else if ( iDet == kTOF ) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints >  3 ) nPoints =  3;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  3 << 12 ) ) )  |  ( nPoints << 12 );
    }

    else if (iDet == kZDC ) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints > 127) nPoints =127;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)127 << 14 ) ) )  |  ( nPoints << 14 );
    }

    else if ( iDet == kSSD) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints >  7 ) nPoints =  7;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  7 << 21 ) ) )  |  ( nPoints << 21 );
    }

    else if ( iDet == kMWPC) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints > 31 ) nPoints = 31;
        fNPoints = ( fNPoints & ( ~ ((Long64_t) 31 << 24 ) ) )  |  ( nPoints << 24 );
    }

    else if ( iDet == kECAL ) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints > 127) nPoints =127;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)127 << 29 ) ) )  |  ( nPoints << 29 );
    }

    else if ( iDet == kCSC) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints >  7 ) nPoints =  7;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  7 << 36 ) ) )  |  ( nPoints << 36 );
    }

    else if ( iDet == kSILICON) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints >  7 ) nPoints =  7;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  7 << 39 ) ) )  |  ( nPoints << 39 );
    }

    else if ( iDet == kLAND ) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints > 63 ) nPoints = 63;
        fNPoints = ( fNPoints & ( ~ ((Long64_t) 63 << 42 ) ) )  |  ( nPoints << 42 );
    }

    else if ( iDet == kFD) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints >  1 ) nPoints =  1;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  1 << 48 ) ) )  |  ( nPoints << 48 );
    }

    else if ( iDet == kARMTRIG) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints >  3 ) nPoints =  3;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  3 << 49 ) ) )  |  ( nPoints << 49 );
    }

    else if ( iDet == kBC) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints >  1 ) nPoints =  1;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  1 << 51 ) ) )  |  ( nPoints << 51 );
    }

    else if ( iDet == kSCWALL) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints >  1 ) nPoints =  1;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  1 << 52 ) ) )  |  ( nPoints << 52 );
    }

    else if ( iDet == kHODO) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints >  1 ) nPoints =  1;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  1 << 53 ) ) )  |  ( nPoints << 53 );
    }

    else if ( iDet == kSiMD) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints >  1 ) nPoints =  1;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  1 << 54 ) ) )  |  ( nPoints << 54 );
    }

    else if ( iDet == kSiBT) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints >  1 ) nPoints =  3;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  3 << 55 ) ) )  |  ( nPoints << 55 );
    }

    else if (iDet == kFHCAL) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints >  1 ) nPoints =  1;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  1 << 57 ) ) )  |  ( nPoints << 57 );
    }

    else if (iDet == kNDET) {
        if      ( nPoints <  0 ) nPoints =  0;
        else if ( nPoints >  1 ) nPoints =  1;
        fNPoints = ( fNPoints & ( ~ ((Long64_t)  1 << 58 ) ) )  |  ( nPoints << 58 );
    }

    else if (iDet == kTOFCAL) {
        ;
    }

    else LOG(error) << "Unknown detector ID " << iDet;
}

ClassImp(CbmMCTrack)
