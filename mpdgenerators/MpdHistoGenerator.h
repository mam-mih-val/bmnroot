// -------------------------------------------------------------------------
// -----              MpdHistoGenerator header file                    -----
// -----           Created 03/06/2014  by M.Ilieva, A.Zinchenko        -----
// -------------------------------------------------------------------------

/** MpdHistoGenerator.h
 ** @author 
 **
 ** The MpdHistoGenerator generates particles according
 ** to Eta-Pt histogram.
 ** Derived from FairGenerator.
 */


#ifndef MPDHISTOGENERATOR_H
#define MPDHISTOGENERATOR_H

#include "FairGenerator.h"
#include "TH2D.h"

using namespace std;

class FairPrimaryGenerator;

class MpdHistoGenerator : public FairGenerator
{
public:

  /** Default constructor. */
  MpdHistoGenerator();

  /** Constructor with PDG-ID, multiplicity
   **@param pdgid Particle type (PDG encoding)
   **@param mult  Multiplicity (default is 1)
   **/
  MpdHistoGenerator(Int_t pdgid, Int_t mult = 1);

  /** Destructor */
  virtual ~MpdHistoGenerator();

  /** Creates an event with given type and multiplicity.
   **@param primGen  pointer to the CbmPrimaryGenerator
   */
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

private:
  Int_t fPdgCode;               //! particle PDG code
  Int_t fMult;                  //! particle multiplicity
  TH2D*     fHist;              //! Pointer to the histogram

ClassDef(MpdHistoGenerator,1);
};

#endif
