// -------------------------------------------------------------------------
// -----                MpdLAQGSMGenerator header file                 -----
// -------------------------------------------------------------------------

/** MpdLAQGSMGenerator
 *@author Elena Litvinenko  <litvin@nf.jinr.ru>
 *@version 15.02.2016
 *
 ** The MpdLAQGSMGenerator uses the ASCII input 
 ** provided by K.Gudima LAQGSM event generator.
**/

#ifndef MPDLAQGSMGENERATOR_H
#define MPDLAQGSMGENERATOR_H 1

#include "FairGenerator.h"

#include <fstream>
#include <map>
#include <vector>

#ifndef __CINT__
#include <zlib.h>
#endif

class TDatabasePDG;
class FairPrimaryGenerator;
class FairIon;

class MpdLAQGSMGenerator : public FairGenerator
{

struct la_tab_t {
  Int_t pdg;
  Int_t Z;
  Int_t strange;
  Int_t lepton;
  Int_t A;
  Float_t mass;
  char  name[10];
};

 public: 

  /** Default constructor without arguments should not be used. **/
  MpdLAQGSMGenerator();

  /** Standard constructor. 
   ** @param fileName The input file name
   **/
  MpdLAQGSMGenerator(const char* fileName, const Bool_t use_collider_system=kTRUE, Int_t QGSM_format_ID=0,Int_t Max_Event_Number=0);

  /** Destructor. **/
  virtual ~MpdLAQGSMGenerator();

  //  static la_tab_t la_tab[78];    //!  list of light particles known for MpdLAQGSMGenerator

  void Init (const char *light_particles_filename=0);  // fill list of known light particles
	
  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the FairPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

  virtual Bool_t GetEventHeader(char *ss);

  Bool_t SkipEvents(Int_t nSkip); //AZ

  Bool_t general_fgets (char *ss, Int_t nn=250, FILE* p=0);
  Bool_t general_feof (void *p);
    
  void SetXYZ (Double_t x = 0., Double_t y = 0., Double_t z = 0.) {
        fX = x;
        fY = y;
        fZ = z;
  }

 protected:

  FILE* fInputFile;                    //!  Input file
  #ifndef __CINT__
  gzFile fGZInputFile;                 //!  GZ Input file
  #endif
  const Char_t*  fFileName;           //! Input file Name
  TDatabasePDG*  fPDG;                //!  PDG database
  Int_t fQGSM_format_ID;              //   Reflect format changes
  Bool_t fUseColliderSystem;          //   kTRUE- for NICA/MPD, kFALSE - for lab system (CBM)
  //la_tab_t la_tab[84];                //!  list of light particles known for MpdLAQGSMGenerator
  std::vector<la_tab_t*> fLa_tab;     //!  list of light particles known for MpdLAQGSMGenerator
  Bool_t fGZ_input;                   //!  0: ascii input, 1: gzipped input
  
  Double_t fX, fY, fZ; // Point vertex coordinates [cm]
	
  /** Private method CloseInput. Just for convenience. Closes the 
   ** input file properly. Called from destructor and from ReadEvent. **/
  void CloseInput();

  /** Private method RegisterIons. Goes through the input file and registers
   ** any ion needed. **/
  Int_t RegisterIons(Int_t Max_Event_Number=0);
  Int_t RegisterIons1(); //AZ

  Int_t CreatePdgCode(Int_t Z, Int_t A, Int_t Strange,Int_t user=0);
  Bool_t FindParticle (Int_t Z, Int_t strange, Int_t lepton, Int_t A, Float_t mass, Int_t &PDG, char name[11]);
  Bool_t CreateNucleus (Int_t Z, Float_t mass, Int_t pdgCode, char pdgName[11]);

  /** STL map from ion name to FairIon **/
  std::map<TString, FairIon*> fIonMap;       //!
	
  ClassDef(MpdLAQGSMGenerator,1);

};

#endif
