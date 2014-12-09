// -------------------------------------------------------------------------
// -----                 MpdPlutoGenerator header file                 -----
// -----          Created 13/07/04  by V. Friese / D.Bertini           -----
// -----      Modified from FairPlutoGenerator for MPD by V. Zhezher   -----
// -------------------------------------------------------------------------

/** MpdPlutoGenerator.h
 *@author V.Friese <v.friese@gsi.de>
 *@author D.Bertini <d.bertini@gsi.de>
 *
 The MpdPlutoGenerator reads the PLUTO output file (ROOT format)
 and inserts the tracks into the FairStack via the FairPrimaryGenerator.
 Derived from FairGenerator.
**/


#ifndef MPD_PLUTOGENERATOR_H
#define MPD_PLUTOGENERATOR_H


#include "FairGenerator.h"

class TClonesArray;
class TFile;
class TTree;
class TVirtualMCStack;
class FairPrimaryGenerator;



class MpdPlutoGenerator : public FairGenerator
{

 public: 

  /** Default constructor (should not be used) **/
  MpdPlutoGenerator();


  /** Standard constructor
   ** @param fileName The input (PLUTO) file name
   **/
  MpdPlutoGenerator(const Char_t* fileName, Double_t ekin);


  /** Destructor **/
  virtual ~MpdPlutoGenerator();


  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

  //Skip some events in file
  Bool_t SkipEvents(Int_t count);
	
 private:

  Int_t iEvent;      //! Event number
  Double_t fEkin;     //! Target energy in lab system 
  const Char_t* fFileName;   //! Input file name
  TFile* fInputFile;        //! Pointer to input file
  TTree* fInputTree;        //! Pointer to input tree
  TClonesArray* fParticles;  //! Particle array from PLUTO

  /** Private method CloseInput. Just for convenience. Closes the 
   ** input file properly. Called from destructor and from ReadEvent. **/
  void CloseInput();


  ClassDef(MpdPlutoGenerator,1);

};

#endif
