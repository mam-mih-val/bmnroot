// -------------------------------------------------------------------------
// -----                CbmPsdHitProducer source file             -----
// -----                  Created 15/05/12  by     Alla                -----
// -------------------------------------------------------------------------
#include <iostream>
#include <fstream>

#include "TClonesArray.h"
#include "TMath.h"

#include "FairRootManager.h"

#include "CbmPsdDigi.h"
#include "CbmPsdHitProducer.h"
#include "CbmPsdHit.h"

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPsdHitProducer::CbmPsdHitProducer() :
  FairTask("Ideal Psd Hit Producer",1),
  fHitArray(NULL),
  fDigiArray(NULL),
  fNHits(0)
{ 
  //  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmPsdHitProducer::~CbmPsdHitProducer() 
{
  /*if ( fDigiArray ) {
    fDigiArray->Delete();
    delete fDigiArray;
  }
  if ( fHitArray ) {
    fHitArray->Delete();
    delete fHitArray;
  }*/
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus CbmPsdHitProducer::Init() {

 ifstream fxypos("psd_geo_xy.txt");
  for (Int_t ii=0; ii<104; ii++) {
    fxypos>>fXi[ii]>>fYi[ii];
    cout<<"PSD module "<<ii<<" X "<<fXi[ii]<<" Y "<<fYi[ii]<<endl;
  }
  fxypos.close();
  fhModXNewEn = new TH1F("hModXNewEn","X distr, En",300,-150.,150.); 
//  fhModXNewEn->Print();

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- CbmPsdHitProducer::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get input array
  fDigiArray = (TClonesArray*) ioman->GetObject("PsdDigi");
  if ( ! fDigiArray ) {
    cout << "-W- CbmPsdHitProducer::Init: "
	 << "No PSD digits array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("CbmPsdHit", 1000);
  ioman->Register("PsdHit", "PSD", fHitArray, kTRUE);

  fHitArray->Dump();
  cout << "-I- CbmPsdHitProducer: Intialisation successfull " << kSUCCESS<< endl;
  return kSUCCESS;

}


// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmPsdHitProducer::Exec(Option_t* opt) {

//  cout<<" CbmPsdHitProducer::Exec(Option_t* opt) "<<endl;
//  fhModXNewEn->Print();

  // Reset output array
//   FairRootManager* ioman = FairRootManager::Instance();
//   fDigiArray = (TClonesArray*) ioman->GetObject("PsdDigi");
   if ( ! fDigiArray ) Fatal("Exec", "No PsdDigi array");
   Reset();
   
  // Declare some variables
  CbmPsdDigi* dig = NULL;
  Float_t edep[104];
  Float_t edepTOT=0.;
 
  for (Int_t imod=0; imod<104; imod++)  edep[imod]=0;
 
  // Loop over PsdDigits
  Int_t nDigi = fDigiArray->GetEntriesFast();
//  cout<<" nDigits "<<nDigi<<endl;
  for (Int_t idig=0; idig<nDigi; idig++) {
    dig = (CbmPsdDigi*) fDigiArray->At(idig);
    if ( ! dig) continue;
    Int_t mod = dig->GetModuleID();
//    Int_t sec = dig->GetSectionID();
    edep[mod] += dig->GetEdep();
    edepTOT += dig->GetEdep();
   }// Loop over MCPoints
//  cout << "PSD HitProducer : Edep total with digits : " << edepTOT << endl;



  for (Int_t imod=0; imod<104; imod++) {
    if (edep[imod]>0) {
      new ((*fHitArray)[fNHits]) CbmPsdHit(imod, edep[imod]);
      fNHits++;
      //    cout<<"CbmPsdHitProducer "<<fNHits<<" "<<imod<<" "<<edep[imod]<<endl;
     fhModXNewEn->Fill(fXi[imod],TMath::Sqrt(edep[imod]) );
//      cout<<"CbmPsdHitProducer "<<fNHits<<" "<<imod<<" "<<edep[imod]<<endl;
     }
  }   
  // }//module

  // Event summary
  cout << "-I- CbmPsdHitProducer: " <<fNHits<< " CbmPsdHits created." << endl;

}
// -------------------------------------------------------------------------
void CbmPsdHitProducer::Finish()
{
  cout<<" CbmPsdHitProducer::Finish() "<<endl;
   TFile * outfile = new TFile("EdepHistos.root","RECREATE");
    outfile->cd();
   fhModXNewEn->Write();
   outfile->Close();
}

// -----   Private method Reset   ------------------------------------------
void CbmPsdHitProducer::Reset() {
  fNHits = 0;
  if ( fHitArray ) fHitArray->Delete();
  
}


ClassImp(CbmPsdHitProducer)
