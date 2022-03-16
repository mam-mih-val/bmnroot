#ifndef UPARTICLE_H
#define UPARTICLE_H

#include "TObject.h"
#include "TLorentzVector.h"
#include "TMath.h"

class TParticle;

/**
 * unigen particle
 */
class UParticle : public TObject {

 private:
  Int_t      fIndex;        // index of this particle
  Int_t      fPdg;          // PDG code
  Int_t      fStatus;       // Status
  Int_t      fParent;       // Index of parent
  Int_t      fParentDecay;  // Parent decay index
  Int_t      fMate;         // index of last collision partner
  Int_t      fDecay;        // decay index (-1 if not decayed)
  Int_t      fChild[2];     // index of first and last child
  Double32_t fPx;           // px (GeV)
  Double32_t fPy;           // py (GeV)
  Double32_t fPz;           // pz (GeV)
  Double32_t fE;            // Energy (GeV)
  Double32_t fX;            // x (fm)
  Double32_t fY;            // y (fm)
  Double32_t fZ;            // z (fm)
  Double32_t fT;            // t (fm)
  Double32_t fWeight;       // weight

 public:
  /**
   * default constructor
   */
  UParticle();
  /**
   * constructor
   * @param index particle index
   * @param pdg pid of track
   * @param status particle status
   * @param parent parent index
   * @param parentDecay parent decay index
   * @param mate  index of last collision partner
   * @param decay decay index (-1 if not decayed)
   * @param child index of first and last child
   * @param px px momentum
   * @param py py momentum
   * @param pz pz momentum
   * @param e energy
   * @param x freezout -x
   * @param y freezout -y
   * @param z freezout - z
   * @param t freezout - t
   * @param weight particle weight
   */
  UParticle(Int_t index, Int_t pdg, Int_t status,
	    Int_t parent, Int_t parentDecay,
	    Int_t mate, Int_t decay, Int_t child[2],
	    Double_t px, Double_t py, Double_t pz, Double_t e,
	    Double_t x, Double_t y, Double_t z, Double_t t,
	    Double_t weight);
  /**
   * constructor
   * @param index particle index
   * @param pdg pid of track
   * @param status particle status
   * @param parent parent index
   * @param parentDecay parent decay index
   * @param mate  index of last collision partner
   * @param decay decay index (-1 if not decayed)
   * @param child index of first and last child
   * @param mom particle momentum
   * @param pos particle freezout postion
   * @param weight particle weight
   */
  UParticle(Int_t index, Int_t pdg, Int_t status,
	    Int_t parent, Int_t parentDecay,
	    Int_t mate, Int_t decay, Int_t child[2],
	    TLorentzVector mom, TLorentzVector pos,
	    Double_t weight);
  /**
   * copy constructor
   * @param right uparticle to copy
   */
  UParticle(const UParticle& right);
  /**
   * copy ctor
   * @param right TParticle to copy
   */
  UParticle(const TParticle& right);
  virtual ~UParticle();
  /**
   * operator =
   * @param right particle
   * @return this overwriten particle
   */
  const UParticle& operator =  (const UParticle& right);
  /**
   *  copy TParticle into this particle
   * @param right TParticle to copy
   * @return copy of TParticle in Unigen format
   */
  const UParticle& operator =  (const TParticle& right);
  /**
   * comare tracks
   * @param right track to compare
   * @return true if particles are the same, false otherwise
   */
  const Bool_t     operator == (const UParticle& right) const;
  /**
   * print info about this particle
   * @param option not used
   */
  void Print(Option_t* option = "");
  /**
   *
   * @return particle index
   */
  inline Int_t    GetIndex()       const {return fIndex;}
  /**
   *
   * @return particle pid
   */
  inline Int_t    GetPdg()         const {return fPdg;}
  /**
   *
   * @return particle status
   */
  inline Int_t    GetStatus()      const {return fStatus;}
  /**
   *
   * @return particle's parent index
   */
  inline Int_t    GetParent()      const {return fParent;}
  /**
   *
   * @return particle's parent decay index
   */
  inline Int_t    GetParentDecay() const {return fParentDecay;}
  /**
   *
   * @return   index of last collision partner
   */
  inline Int_t    GetMate()        const {return fMate;}
  /**
   *
   * @return decay index (-1 if not decayed)
   */
  inline Int_t    GetDecay()       const {return fDecay;}
  /**
   *
   * @return first child index
   */
  inline Int_t    GetFirstChild()  const {return fChild[0];}
  /**
   *
   * @return last child index
   */
  inline Int_t    GetLastChild()   const {return fChild[1];}
  /**
   *
   * @return px
   */
  inline Double_t Px()             const {return fPx;}
  /**
   *
   * @return py
   */
  inline Double_t Py()             const {return fPy;}
  /**
   *
   * @return pz
   */
  inline Double_t Pz()             const {return fPz;}
  /**
   *
   * @return energy
   */
  inline Double_t E()              const {return fE;}
  /**
   *
   * @return track momentum
   */
  inline TLorentzVector GetMomentum() const {return TLorentzVector(fPx,fPy,fPz,fE);}
  /**
   *
   * @param mom momentum to fill
   */
  inline void Momentum(TLorentzVector& mom) const {mom.SetPxPyPzE(fPx,fPy,fPz,fE);}
  /**
   *
   * @return freezout-X
   */
  inline Double_t X()              const {return fX;}
  /**
   *
   * @return freezout-Y
   */
  inline Double_t Y()              const {return fY;}
  /**
   *
   * @return freezout-Z
   */
  inline Double_t Z()              const {return fZ;}
  /**
   *
   * @return freezout-T
   */
  inline Double_t T()              const {return fT;}
  /**
   *
   * @return freezout position
   */
  inline TLorentzVector GetPosition() const {return TLorentzVector(fX,fY,fZ,fT);}
  /**
   *
   * @param pos position to fill by this track position coord.
   */
  inline void Position(TLorentzVector& pos) const {pos.SetXYZT(fX,fY,fZ,fT);}
  /**
   *
   * @return particle weight
   */
  inline Double_t GetWeight()      const {return fWeight;}
  /**
   *
   * @param index index
   */
  inline void SetIndex      (Int_t index)       {fIndex = index;}
  /**
   *
   * @param pdg pid code
   */
  inline void SetPdg        (Int_t pdg)         {fPdg = pdg;}
  /**
   *
   * @param status status
   */
  inline void SetStatus     (Int_t status)      {fStatus = status;}
  /**
   *
   * @param parent parent index
   */
  inline void SetParent     (Int_t parent)      {fParent = parent;}
  /**
   *
   * @param parentDecay parent decay index
   */
  inline void SetParentDecay(Int_t parentDecay) {fParentDecay = parentDecay;}
  /**
   *
   * @param mate  index of last collision partner
   */
  inline void SetMate       (Int_t mate)        {fMate = mate;}
  /**
   *
   * @param decay decay code (-1 if no decayed)
   */
  inline void SetDecay      (Int_t decay)       {fDecay = decay;}
  /**
   *
   * @param child index of first and last child
   */
  inline void SetChild      (Int_t child[2])    {fChild[0]=child[0]; fChild[1]=child[1];}
  /**
   *
   * @param child first child index
   */
  inline void SetFirstChild (Int_t child)       {fChild[0] = child;}
  /**
   *
   * @param child last child index
   */
  inline void SetLastChild  (Int_t child)       {fChild[1] = child;}
  /**
   *
   * @param px px momentum
   */
  inline void SetPx         (Double_t px)       {fPx = px;}
  /**
   *
   * @param py py momentum
   */
  inline void SetPy         (Double_t py)       {fPy = py;}
  /**
   *
   * @param pz pz momentum
   */
  inline void SetPz         (Double_t pz)       {fPz = pz;}
  /**
   *
   * @param e energy
   */
  inline void SetE          (Double_t e)        {fE = e;}
  /**
   * set momentum
   * @param px px
   * @param py py
   * @param pz pz
   * @param e energy
   */
  inline void SetMomentum(Double_t px, Double_t py, Double_t pz, Double_t e)
    {fPx = px; fPy = py; fPz = pz; fE = e;}
  /**
   * set momentum
   * @param mom momentum
   */
  inline void SetMomentum(TLorentzVector mom) {fPx=mom.Px(); fPy=mom.Py(); fPz=mom.Pz(); fE=mom.E();}
  /**
   *
   * @param x freezout-X
   */
  inline void SetX          (Double_t x)        {fX = x;}
  /**
   *
   * @param y freezout-Y
   */
  inline void SetY          (Double_t y)        {fY = y;}
  /**
   *
   * @param z freezout-Z
   */
  inline void SetZ          (Double_t z)        {fZ = z;}
  /**
   *
   * @param t freezout-T
   */
  inline void SetT          (Double_t t)        {fT = t;}
  /**
   * set freezout position
   * @param x X
   * @param y Y
   * @param z Z
   * @param t T
   */
  inline void SetPosition(Double_t x, Double_t y, Double_t z, Double_t t)
    {fX = x; fY = y; fZ = z; fT = t;}
  /**
   * set postition
   * @param pos new position
   */
  inline void SetPosition(TLorentzVector pos) {fX=pos.X(); fY=pos.Y(); fZ=pos.Z(); fT=pos.T();}
  /**
   *
   * @param weight weight
   */
  inline void SetWeight     (Double_t weight)   {fWeight = weight;}

  ClassDef(UParticle, 1)
};


#endif
