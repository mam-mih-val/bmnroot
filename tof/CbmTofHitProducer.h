// --------------------------------------------------------------------------
// -----             Header for the CbmTofHitProducer                  ------
// -----           Created by E. Cordier 14/09/05                      ------
// -----           Modified by D. Gonzalez-Diaz 07/09/06               ------
// -----           Modified by D. Gonzalez-Diaz 02/02/07               ------
// --------------------------------------------------------------------------



#ifndef CBMTOFHITPRODUCER_H
#define CBMTOFHITPRODUCER_H

#include "FairTask.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TFile.h"

class CbmTofGeoHandler;
class CbmTofDigiPar;
class CbmTofCell;

class TVector3;
class TClonesArray;
class TString;                               


class CbmTofHitProducer : public FairTask {

 public:

  CbmTofHitProducer();

  CbmTofHitProducer(const char *name, Int_t verbose = 1);

  virtual ~CbmTofHitProducer();

  virtual InitStatus Init();
  virtual InitStatus ReInit();
  virtual void SetParContainers();
  virtual void Exec(Option_t * option);
  virtual void Finish();
  void SetHistogram();

  void AddHit(Int_t detID, TVector3 &posHit, TVector3 &posHitErr,
	      Int_t ref, Double_t tHit, Int_t flag, Int_t iChanne);

  void SetXshift(Double_t shift) { fXshift = shift;};
  void SetZposition(Double_t z) { fZposition = z;};
  void SetSigmaT(Double_t sigma);
  void SetSigmaXY(Double_t sigma);
  void SetSigmaX(Double_t sigma);
  void SetSigmaZ(Double_t sigma);
  void SetInitFromAscii(Bool_t ascii) {fParInitFromAscii=ascii;}

  Double_t GetSigmaT();
  Double_t GetSigmaXY();
  Double_t GetSigmaX();
  Double_t GetSigmaZ();

private:

  void ReadTofZPosition();

  void InitParametersFromAscii();
  void InitParametersFromContainer();

  Int_t fVerbose;

  TClonesArray *fTofPoints;     // TOF MC points
  TClonesArray *fMCTracks;      // MC tracks
  TClonesArray *fTofHits;       // TOFhits

  //Temporary substitute of the parameter container. FIXME 
 
  Float_t X [2][40][40];  //X-Position in cm */
  Float_t Dx[2][40][40];  //Resolution in position in cm */
  Float_t Y [2][40][40];  //Y-Position in cm */
  Float_t Dy[2][40][40];  //Resolution in position in cm */
  Float_t Z [2][40][40];  //Z-Position in cm */
  Int_t   Ch[2][40][40];  //Channel number */

  char type[2][40][40][2]; //Type of cell [pad, strip]

  //End of temporary data members

  //Members to store the hit during loop over all the points.

  Float_t tl[2][40][40];
  Float_t tr[2][40][40];
  Int_t trackID_left[2][40][40];
  Int_t trackID_right[2][40][40]; 
  Int_t point_left[2][40][40];
  Int_t point_right[2][40][40]; 

  //End of storing data members

  Double_t fSigmaT;       //Resolution in time in ns 
  Double_t fSigmaXY;      //Resolution in x/y  in cm
  Double_t fSigmaX;       //Resolution in y    in cm
  Double_t fSigmaZ;       //Resolution in z    in cm
  
  TString fVersion;       //
  Int_t  fNHits;          //Index of the CbmTofHit TClonesArray

  Double_t fTofZPosition; // entry Z position of the TOF wall (NOT hit)
  Double_t fZposition; // global Z position of the TOF wall
  Double_t fXshift;                 //  shift on X axis to have ion beam inside the hole

  CbmTofGeoHandler* fGeoHandler;
  CbmTofDigiPar  *fDigiPar;
  CbmTofCell   *fCellInfo;
  
  TH2F *fhist1[12],*fhist4;
  TH1F *fhist2,*fhist3;
  TFile *f;

  Bool_t fParInitFromAscii;

  CbmTofHitProducer(const CbmTofHitProducer&);
  CbmTofHitProducer& operator=(const CbmTofHitProducer&);

  ClassDef(CbmTofHitProducer,3) //CBMTOFHitProducer

};
#endif 

