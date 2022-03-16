#ifndef UEVENT_H
#define UEVENT_H
/**
 * derived from Unigen project
 * https://www.gsi.de/work/wissenschaftliche_netzwerke/helmholtz_virtuelle_institute/unigen.htm
 */


#include "TObject.h"
#include "TLorentzVector.h"
#include "TNamed.h"

class TString;
class TClonesArray;
class UParticle;

/**
 * unigen event
 */
class UEvent : public TNamed {

 private:
	/**
	 * Event number
	 */
  Int_t         fEventNr;
  /**
   * Impact parameter (fm)
   */
  Double_t      fB;
  /**
   * Reaction plane angle
   */
  Double_t      fPhi;
  /**
   * Number of event steps
   */
  Int_t         fNes;
  /**
   * Event step number
   */
  Int_t         fStepNr;
  /**
   * Event step time
   */
  Double_t      fStepT;
  /**
   * Number of particles
   */
  Int_t         fNpa;
  /**
   * Generator-specific information
   */
  TString       fComment;
  /**
   * Array of particles
   */
  TClonesArray* fParticles;
  
 public:
  /**
   * default constructor
   */
  UEvent();
  /**
   * copy constructor
   * @param right object to copy
   */
  UEvent(const UEvent& right);
  /**
   * assignement operator
   */
  UEvent operator=(const UEvent &right);
  virtual ~UEvent();
  /**
   * print info about event
   * @param option if "all" then particles are also printed
   */
  void Print(Option_t* option = "");
  /**
   *
   * @return event number
   */
  inline Int_t    GetEventNr() const {return fEventNr;}
  /**
   *
   * @return impact paramter
   */
  inline Double_t GetB()       const {return fB;}
  /**
   *
   * @return reaction plane angle
   */
  inline Double_t GetPhi()     const {return fPhi;}
  /**
   *
   * @return number of event steps
   */
  inline Int_t    GetNes()     const {return fNes;}
  /**
   *
   * @return event step number
   */
  inline Int_t    GetStepNr()  const {return fStepNr;}
  /**
   *
   * @return event step time
   */
  inline Double_t GetStepT()   const {return fStepT;}
  /**
   *
   * @return number of tracks
   */
  inline Int_t    GetNpa()     const {return fNpa;}
  /**
   *
   * @return list of particles
   */
  inline TClonesArray* GetParticleList() const {return fParticles;}
  /**
   *
   * @param comment comment
   */
  inline void GetComment(TString& comment) const {comment = fComment;}
  /**
   *
   * @param index particle posistion
   * @return particle at given position
   */
  UParticle* GetParticle(Int_t index) const;
  /**
   * set event parameters
   * @param eventNr event number
   * @param b impact parameter
   * @param phi reaction plane nagle
   * @param nes number of steps
   * @param stepNr step number
   * @param stepT step time
   * @param comment comment
   */
  void SetParameters(Int_t eventNr, Double_t b, Double_t phi, Int_t nes,
		     Int_t stepNr, Double_t stepT, const char* comment = "");
  /**
   *
   * @param eventNr event nubmer
   */
  inline void SetEventNr(Int_t eventNr)    {fEventNr = eventNr;}
  /**
   *
   * @param b impact parameter
   */
  inline void SetB      (Double_t b)       {fB = b;}
  /**
   *
   * @param phi reaction plane angle
   */
  inline void SetPhi    (Double_t phi)     {fPhi = phi;}
  /**
   *
   * @param nes number of steps
   */
  inline void SetNes    (Int_t nes)        {fNes = nes;}
  /**
   *
   * @param stepNr step number
   */
  inline void SetStepNr (Int_t stepNr)     {fStepNr = stepNr;}
  /**
   *
   * @param stepT step time
   */
  inline void SetStepT  (Double_t stepT)   {fStepT = stepT;}
  /**
   *
   * @param comment comment
   */
  inline void SetComment(const char* comment)  {fComment = comment;}
  /**
   * add particle
   * @param index particle position
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
  void AddParticle(Int_t index, Int_t pdg, Int_t status,
		   Int_t parent, Int_t parentDecay,
		   Int_t mate, Int_t decay, Int_t child[2],
		   Double_t px, Double_t py, Double_t pz, Double_t e,
		   Double_t x, Double_t y, Double_t z, Double_t t,
		   Double_t weight);
  /**
   *
   * @param index particle position
   * @param pdg pid of track
   * @param status track status
   * @param parent parent index
   * @param parentDecay parent decay index
   * @param mate  index of last collision partner
   * @param decay decay index (-1 if not decayed)
   * @param child index of first and last child
   * @param mom momentum
   * @param pos freezout coordinates as
   * @param weight
   */
  void AddParticle(Int_t index, Int_t pdg, Int_t status,
		   Int_t parent, Int_t parentDecay,
		   Int_t mate, Int_t decay, Int_t child[2],
		   TLorentzVector mom, TLorentzVector pos,
		   Double_t weight);
//  void Rebuild(UEvent* ev);
  /**
   *
   * @param particle particle to add
   */
  void AddParticle(const UParticle& particle);
  /**
   * clear this
   */
  void Clear();
  /**
   * clear this
   * @param otp clear option
   */
  void Clear(Option_t *otp){Clear();};
  /**
   * remove particle
   * @param i particle position
   */
  void RemoveAt(Int_t i);

  ClassDef(UEvent, 3)
};


#endif
