// -------------------------------------------------------------------------
// -----             CbmAnaHypYPtGenerator header file                 -----
// -----              Created 03/10/04  by E. Kryshen                  -----
// -----  Updated 10/02/10  by A. Zinchenko for MPD (with name change) -----
// -------------------------------------------------------------------------

/** CbmAnaHypYPtGenerator.h
 ** @author Evgeny Kryshen <e.kryshen@gsi.de>
 **
 ** The CbmAnaHypYPtGenerator generates particles with a given
 ** distribution over pt (dn/dpt = pt*exp(-mt/T)
 ** and rapidity (Gaussian) with fixed multiplicity per event.
 ** Derived from FairGenerator.
 */


#ifndef MPDHYPYPTGENERATOR_H
#define MPDHYPYPTGENERATOR_H

#include "FairGenerator.h"
#include "TF1.h"

using namespace std;

class FairPrimaryGenerator;

class MpdHypYPtGenerator : public FairGenerator
{
public:

  /** Default constructor. */
  MpdHypYPtGenerator();

  /** Constructor with PDG-ID, multiplicity
   **@param pdgid Particle type (PDG encoding)
   **@param mult  Multiplicity (default is 1)
   **/
  MpdHypYPtGenerator(Int_t pdgid, Int_t mult=1);

  /** Destructor */
  virtual ~MpdHypYPtGenerator() {};

  /** Modifiers */
  inline void SetPDGType        (Int_t pdg)  {fPDGType = pdg;  };
  inline void SetMultiplicity   (Int_t mult) {fMult    = mult; };
  //inline void SetDistributionPt (Double_t T=0.154319) {fT=T;};
  inline void SetDistributionPt (Double_t T=0.223) {fT=T;}; //AZ - for Omega- @ 9 GeV/n AuAu UrQMD
  //inline void SetDistributionY  (Double_t y0=1.98604, Double_t sigma=0.617173) {fY0=y0;fSigma=sigma;};
  inline void SetDistributionY  (Double_t y0=0, Double_t sigma=0.72) {fY0=y0;fSigma=sigma;}; // AZ - for Omega- @ 9 GeV/n AuAu UrQMD
  inline void SetRangePt        (Double_t ptMin=0, Double_t ptMax=3) {fPtMin=ptMin;fPtMax=ptMax;};


  /** Initializer */
  Bool_t  Init();    // EL

  /** Creates an event with given type and multiplicity.
   **@param primGen  pointer to the CbmPrimaryGenerator
   */
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

private:
  Int_t    fPDGType;             ///< Particle type (PDG encoding)
  Int_t    fMult;                ///< Multiplicity
  Double_t fT;                   ///< Temperature in the Pt distribution
  Double_t fY0;                  ///< Mean rapidity
  Double_t fSigma;               ///< Simga in the rapidity distribution
  Double_t fPtMin;               ///< Max value of Pt
  Double_t fPtMax;               ///< Min value of Pt
  Double_t fPDGMass;             ///< Particle mass [GeV]
  TF1*     fDistPt;              //! Pointer to the Pt function

ClassDef(MpdHypYPtGenerator,1);
};

#endif
