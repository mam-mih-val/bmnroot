/**
 *@class MpdPHSDGenerator
 *@author V.Voronyuk <vadimv@jinr.ru>
 * MpdPHSDGenerator reads newer (instead of default) output (phsd.dat/phsd.dat.gz)
 * of HSD/PHSD transport model.
 **/

#ifndef Mpd_PHSDGENERATOR_H
#define Mpd_PHSDGENERATOR_H

#ifndef __CINT__
#include <zlib.h>
#endif

#include <TRandom2.h>

#include "FairGenerator.h"
#include "FairPrimaryGenerator.h"

class MpdPHSDGenerator : public FairGenerator
{
 public:
  MpdPHSDGenerator();
  /** Default constructor **/
  MpdPHSDGenerator(const char *filename);
  /** Destructor **/
  virtual ~MpdPHSDGenerator();
  /** Read event **/
  Bool_t ReadEvent(FairPrimaryGenerator *primGen);
  /** Skip n events, return kTRUE if successful **/
  Bool_t SkipEvents(Int_t n);

  /** Set fixed reaction plane angle **/
  void SetPsiRP(Double_t PsiRP) {fPsiRP=PsiRP; fisRP=kFALSE;};

 private:
  Int_t fntr; //! number of tracks in given event
  Float_t fb; //! impact parameter in given event
  #ifndef __CINT__
  gzFile    fgzFile; //! file
  #endif
  char fbuffer[256]; //! reading buffer

  TRandom2 *frandom; //!
  Double_t  fPsiRP;  //! reaction plane angle
  Bool_t    fisRP;   //! random/fixed reaction plane

  Bool_t ReadHeader(); //! read event header, return kTRUE if successful
  void   SkipTrack();  //! skip one track

 ClassDef(MpdPHSDGenerator,0);
};
#endif
