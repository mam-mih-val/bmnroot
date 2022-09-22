// -------------------------------------------------------------------------
// -----                      CbmMCTrack header file                   -----
// -----                  should be replaced by BmnMCTrack             -----
// -------------------------------------------------------------------------

/** CbmMCTrack.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Data class for storing Monte Carlo tracks processed by the CbmStack.
 ** A MCTrack can be a primary track put into the simulation or a
 ** secondary one produced by the transport through decay or interaction.
 **
 **/

#ifndef CBMMCTRACK_H
#define CBMMCTRACK_H 1

#include "BmnDetectorList.h"

#include "TObject.h"
#include "TMath.h"
#include "TVector3.h"
#include "TParticle.h"
#include "TLorentzVector.h"


class CbmMCTrack : public TObject
{
  public:
    /**  Default constructor  **/
    CbmMCTrack();

    /**  Standard constructor  **/
    CbmMCTrack(Int_t pdgCode, Int_t motherID,
               Double_t px, Double_t py, Double_t pz,
               Double_t x, Double_t y, Double_t z,
               Double_t t, Long64_t nPoints);

    /**  Copy constructor  **/
    CbmMCTrack(const CbmMCTrack& track);

    /**  Constructor from TParticle  **/
    CbmMCTrack(TParticle* particle);

    /**  Destructor  **/
    virtual ~CbmMCTrack();


    /**  Output to screen  **/
    void Print(Int_t iTrack = 0) const;


    /**  Accessors  **/
    Int_t    GetPdgCode()  const { return fPdgCode; }
    Int_t    GetMotherId() const { return fMotherId; }
    Double_t GetPx()       const { return fPx; }
    Double_t GetPy()       const { return fPy; }
    Double_t GetPz()       const { return fPz; }
    Double_t GetStartX()   const { return fStartX; }
    Double_t GetStartY()   const { return fStartY; }
    Double_t GetStartZ()   const { return fStartZ; }
    Double_t GetStartT()   const { return fStartT; }
    Double_t GetMass()     const;
    Double_t GetEnergy()   const;
    Double_t GetPt()       const { return TMath::Sqrt(fPx*fPx+fPy*fPy); }
    Double_t GetP() const { return TMath::Sqrt(fPx*fPx+fPy*fPy+fPz*fPz); }
    Double_t GetRapidity() const;
    void GetMomentum(TVector3& momentum) const;
    void Get4Momentum(TLorentzVector& momentum) const;
    void GetStartVertex(TVector3& vertex) const;


    /** Accessors to the number of MCPoints in the detectors **/
    Long64_t GetNPoints(DetectorId detId)  const;
    /**AZ-310822 Get polarization **/
    Double_t GetPolar(Int_t indx) { return fPolar[indx]; }

    /**  Modifiers  **/
    void SetMotherId(Int_t id) { fMotherId = id; }
    void SetNPoints(Int_t iDet, Long64_t np);
    void SetPolar(Int_t indx, Double_t polar) { fPolar[indx] = polar; } //AZ-310822

  private:

    /**  PDG particle code  **/
    Int_t  fPdgCode;

    /**  Index of mother track. -1 for primary particles.  **/
    Int_t  fMotherId;

    /** Momentum components at start vertex [GeV]  **/
    Double32_t fPx, fPy, fPz;

    /** Coordinates of start vertex [cm, ns]  **/
    Double32_t fStartX, fStartY, fStartZ, fStartT;

    /**  Bitvector representing the number of MCPoints for this track in each subdetector.
     **  The detectors are represented by
     **  REF:         Bit  0      (1 bit,  max. value   1)
     **  BD:          Bit  1      (1 bit,  max. value   1)
     **  GEM:         Bit  2 - 4  (3 bits, max. value   7) /7
     **  TOF1:        Bit  5 - 6  (2 bit,  max. value   3) /1 (3)
     **  DCH:         Bit  7 - 11 (5 bits, max. value  31) /16
     **  TOF:         Bit 12 - 13 (2 bit,  max. value   3) /1 (3)
     **  ZDC:         Bit 14 - 20 (7 bit,  max. value 127) /64
     **  SSD:         Bit 21 - 23 (3 bit,  max. value   7) /4
     **  MWPC:        Bit 24 - 28 (5 bit,  max. value  31) /24
     **  ECAL:        Bit 29 - 35 (7 bits, max. value 127) /110
     **  CSC:         Bit 36 - 38 (3 bit,  max. value   7) /6
     **  SILICON:     Bit 39 - 41 (3 bit,  max. value   7) /4
     **  LAND:        Bit 42 - 47 (6 bit,  max. value  63) /60
     **  FD:          Bit 48      (1 bit,  max. value   1)
     **  ARMTRIG:     Bit 49 - 50 (2 bit,  max. value   3) /2
     **  BC:          Bit 51      (1 bit,  max. value   1)
     **  SCWALL:      Bit 52      (1 bit,  max. value   1)
     **  HODO:        Bit 53      (1 bit,  max. value   1)
     **  SiMD:        Bit 54      (1 bit,  max. value   1)
     **  SiBT:        Bit 55 - 56 (2 bit,  max. value   3)
     **  FHCAL:       Bit 57      (1 bit,  max. value   1)
     **  NDET:        Bit 58      (1 bit,  max. value   1)
     **  The respective point numbers can be accessed and modified with the inline functions.
     **/
    Long64_t fNPoints;
    Double32_t fPolar[3]; //AZ-310822 - polarization

    //AZ-310822 ClassDef(CbmMCTrack,2);
    ClassDef(CbmMCTrack,3); //AZ-310822
};

// ==========   Inline functions   ========================================
inline Double_t CbmMCTrack::GetEnergy() const {
    Double_t mass = GetMass();
    return TMath::Sqrt(mass*mass + fPx*fPx + fPy*fPy + fPz*fPz);
}

inline void CbmMCTrack::GetMomentum(TVector3& momentum) const {
    momentum.SetXYZ(fPx,fPy,fPz);
}

inline void CbmMCTrack::Get4Momentum(TLorentzVector& momentum) const {
    momentum.SetXYZT(fPx,fPy,fPz,GetEnergy());
}

inline void CbmMCTrack::GetStartVertex(TVector3& vertex) const {
    vertex.SetXYZ(fStartX,fStartY,fStartZ);
}

#endif
