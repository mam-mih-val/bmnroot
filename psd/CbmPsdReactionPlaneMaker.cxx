
// -------------------------------------------------------------------------
// -----                CbmPsdReactionPlaneMaker.h source file             -----
// -----                  Created 15/05/12  by     Alla &Marina                -----
// -------------------------------------------------------------------------
#include <iostream>
#include <fstream>

#include "TClonesArray.h"
#include "TMath.h"
#include "TH1F.h"

#include "FairRootManager.h"

#include "CbmPsdReactionPlaneMaker.h"
#include "CbmPsdHit.h"

using namespace std;


// -----   Default constructor   -------------------------------------------
CbmPsdReactionPlaneMaker::CbmPsdReactionPlaneMaker() :
  FairTask("PsdReactionPlaneMaker",1),
  fHitArray(NULL),
  fEventArray(NULL),
  fCbmPsdEvent(NULL)
 { 
  //  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmPsdReactionPlaneMaker::~CbmPsdReactionPlaneMaker() 
{
   if ( fHitArray ) {
    fHitArray->Delete();
    delete fHitArray;
  }
   if ( fEventArray ) {
    fEventArray->Delete();
    delete fEventArray;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus CbmPsdReactionPlaneMaker::Init() {

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- CbmPsdReactionPlaneMaker::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get input array
  fHitArray = (TClonesArray*) ioman->GetObject("PsdHit");
  if ( ! fHitArray ) {
    cout << "-W- CbmPsdReactionPlaneMaker::Init: "
	 << "No PSD hits array!" << endl;
    return kERROR;
  }
 // Create and register output array
  fEventArray = new TClonesArray("CbmPsdEventData", 1);
//  fCbmPsdEvent = new CbmPsdEventData();
//  ioman->Register("PsdEvent", "PSD", fCbmPsdEvent, kTRUE);
  ioman->Register("PsdEvent", "PSD", fEventArray, kTRUE);


  TFile *fhist = new TFile("EdepHistos.root");
  TH1F* hModXNewEn = (TH1F*) fhist->Get("hModXNewEn");
  fMean = 0;
  if (hModXNewEn) fMean = hModXNewEn->GetMean();

  ifstream fxypos("psd_geo_xy.txt");
  for (Int_t ii=0; ii<104; ii++) {
    fxypos>>fXi[ii]>>fYi[ii];
   
  }
  fxypos.close();

   cout << "-I- CbmPsdReactionPlaneMaker: Intialisation successfull " << kSUCCESS<< endl;
  return kSUCCESS;

}


// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmPsdReactionPlaneMaker::Exec(Option_t* opt) {


  // Reset output array
   if ( ! fHitArray ) Fatal("Exec", "No PsdHit array");
   //  Reset();
//   if (fCbmPsdEvent) delete fCbmPsdEvent;
   
  // Declare some variables
  CbmPsdHit* hit = NULL;
  Float_t edep[104];
 
  for (Int_t imod=0; imod<104; imod++)  edep[imod]=0;
 
  // Loop over PsdHits
  Int_t nHits = fHitArray->GetEntriesFast();
  for (Int_t ihit=0; ihit<nHits; ihit++) {
    hit = (CbmPsdHit*) fHitArray->At(ihit);
    if ( !hit) continue;
    Int_t mod = hit->GetModuleID();
    edep[mod] = hit->GetEdep();
  }// Loop over hits
    
  
  Double_t pxShift=0, pyShift=0;
  Double_t qXWeightedShift=0,qYWeightedShift=0;
  Double_t radToDeg=180./TMath::Pi();
  Float_t edepInner=0., edepOuter=0.;
     
  for (Int_t mod=0; mod<104; mod++) {
    pyShift = fYi[mod]; 
    pxShift = fXi[mod] - fMean;
    if(edep[mod]>0) {
      qXWeightedShift += pxShift*sqrt(edep[mod]);
      qYWeightedShift += pyShift*sqrt(edep[mod]);
      if (mod < 68) edepOuter += edep[mod];
      else          edepInner += edep[mod];
    }//if(edep[mod]>0)
  }//for (Int_t mod=0; mod<104; mod++)
//  cout << "PSD EventMaker : Edep total with hits : " << edepOuter+edepInner << endl;
  
  if(qXWeightedShift!=0 && qYWeightedShift!=0) {
    Float_t rpa = TMath::ATan2(qYWeightedShift,qXWeightedShift)*radToDeg;
//    fCbmPsdEvent->SetRP(rpa);
//    fCbmPsdEvent->SetB(0.1);
//     cout << "rp "<<rpa<< endl;
    fCbmPsdEvent =  new ((*fEventArray)[0]) CbmPsdEventData(rpa,0.1,edepInner,edepOuter);

  }//if(qXWeightedShift!=0 && qYWeightedShift!=0)
  
} 
// -----   Private method Reset   ------------------------------------------
void CbmPsdReactionPlaneMaker::Reset() {
  fNHits = 0;
  if ( fHitArray ) fHitArray->Delete();
  if ( fEventArray ) fEventArray->Delete();
  
}

ClassImp(CbmPsdReactionPlaneMaker)
