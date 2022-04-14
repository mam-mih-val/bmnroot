// -------------------------------------------------------------------------
// -----                  BmnGenFastDigitize header file               -----
// -----                 Created 4/07/19  by A. Zinchenko              -----
// -------------------------------------------------------------------------

/** BmnGemFastDigitize.h
 *@author A.Zinchenko <alexander.zinchenko@jinr.ru>
 **
 ** Fast GEM digitizer - reproduces GEM response, produced using 
 ** paramerization of the Garfield++ simulation results.
 **/

#ifndef BMNGEMFASTDIGITIZE_H
#define BMNGEMFASTDIGITIZE_H 1

#include "FairTask.h"

#include <map>
#include <vector>
#include <set>

class CbmStsSensor;
class TTree;
//class TClonesArray;
class TH2I;
class TDirectory;
class TFile;

class BmnGemFastDigitize : public FairTask
{
 public:

  /** Default constructor **/  
  BmnGemFastDigitize();

  /** Destructor **/
  ~BmnGemFastDigitize();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  /** Virtual method Finish **/
  virtual void Finish();

  void ProduceHitResponseFast(CbmStsSensor* sensor, std::set<Int_t> &pSet, Double_t *stripSignalF,  
			      Double_t *stripSignalB, std::map<Int_t, std::set<Int_t> > &chanPointMapF, 
			      std::map<Int_t, std::set<Int_t> > &chanPointMapB);
  Double_t GetNumberOfClusters(Double_t beta, Double_t gamma, Double_t charge, Double_t p0, Double_t p1);

 private:

  /** Input arrays **/
  TClonesArray* fPoints;
  TClonesArray* fMCTracks;
  
  /** Output array of CbmStsHits **/
  //TClonesArray* fHitArray;  
  
  std::map<Int_t,std::multimap<Float_t,Int_t> > fXYmap[2];
  std::map<Int_t,Int_t> fStrRef[2];
  std::map<Int_t,TH2I*> fHistMap;
  std::map<Int_t,TTree*> fTreeMap;
  Int_t fIbeg[2], fStrin[2], fStrout[2];
  Float_t fQtot[2], fDpitin[2], fCog[2];
  std::vector<Float_t> fvDigis[2];
  std::vector<Float_t> *fvDig0, *fvDig1;
  TH2I *fhInd;

  Double_t Interp(Double_t *yy, Int_t indx, Int_t ntot, Double_t dx);
  Int_t FindDataSet(Double_t theX, Double_t theY);

  BmnGemFastDigitize(const BmnGemFastDigitize&);
  BmnGemFastDigitize& operator=(const BmnGemFastDigitize&);

  ClassDef(BmnGemFastDigitize,0);
};

#endif
