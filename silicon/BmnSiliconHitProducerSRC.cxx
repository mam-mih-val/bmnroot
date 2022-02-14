#include "BmnSiliconHitProducerSRC.h"
#include "CbmStsPoint.h"
#include "BmnSiliconPoint.h"
#include "CbmMCTrack.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "BmnSiliconHit.h"
#include "TSystem.h"

using std::cout;
using namespace TMath;

BmnSiliconHitProducerSRC::BmnSiliconHitProducerSRC(){
  
  fInputBranchName       = "SiliconPoint";
  fOutputHitsBranchName  = "BmnSiliconHitClean";
  fOutputHitsBranchName2 = "BmnSiliconHitSim";
}

BmnSiliconHitProducerSRC::~BmnSiliconHitProducerSRC() {


}

InitStatus BmnSiliconHitProducerSRC::Init() {
    if (fDebug) cout << " BmnSiliconHitProducerSRC::Init() " << endl;
    rand_gen.SetSeed(5);

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();

    fBmnPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");
 
    fBmnHitsArray = new TClonesArray("BmnHit");
    ioman->Register(fOutputHitsBranchName, "SILICON", fBmnHitsArray, kTRUE);
    
    fBmnHitsArray2 = new TClonesArray("BmnHit");
    ioman->Register(fOutputHitsBranchName2, "SILICON", fBmnHitsArray2, kTRUE);
    
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    gPathConfig += "/parameters/silicon/XMLConfigs/";
    SiliconStationSet = new BmnSiliconStationSet(gPathConfig + "SiliconRunSpring2018.xml");
    
  //--some hists--
  if (fDebug) {
    hdX  = new TH1D("dX","dX ", 100,-0.015,0.015);  fList.Add(hdX);
    hdXp = new TH1D("dXp","dXp ", 100,-0.017,0.017);fList.Add(hdXp);
  }

    return kSUCCESS;
}

void BmnSiliconHitProducerSRC::Exec(Option_t* opt) {

    fBmnHitsArray->Delete();
    fBmnHitsArray2->Delete();
    

    if (fDebug) cout<<"======================== BmnSiliconHitProducerSRC ========================"<<endl;

    if (!fBmnPointsArray) {
      Error("BmnSiliconHitProducerSRC::Init()", " !!! Unknown branch name !!! ");
      return;
    }
    //                  x      xp     z
    Float_t err[3] = {0.0030, 0.0035, 0.}; // Uncertainties of coordinates //[cm], i.e. 30 & 35 mk

    for (Int_t iPoint = 0; iPoint < fBmnPointsArray->GetEntriesFast(); iPoint++) {

        //TRandom* rand_gen = new TRandom();
        BmnSiliconPoint* siliconPoint = (BmnSiliconPoint*) fBmnPointsArray->UncheckedAt(iPoint);
        
        Int_t charge = siliconPoint->GetCharge();
        if (charge == 0)  continue; //qgsm
        //if (charge != 18) continue;//ion carbon
        Int_t IsPrimary = siliconPoint->GetIsPrimary();
        Double_t     Pz = siliconPoint->GetPz();
        Int_t     pdgId = siliconPoint->GetPdgId();
        if (IsPrimary == 0) continue;
        if (Pz < 1.) continue;
        
        Int_t track_id   = siliconPoint->GetTrackID();
        Float_t dX = rand_gen.Gaus(0,err[0]); 
        Float_t dXp= rand_gen.Gaus(0,err[1]);
        Float_t dZ = rand_gen.Gaus(0,err[2]);
        if (fDebug) hdX ->Fill(dX);
        if (fDebug) hdXp->Fill(dXp);
        
        Float_t x  = siliconPoint->GetXIn();
        Float_t y  = siliconPoint->GetYIn();
        Float_t z  = siliconPoint->GetZIn();
        Float_t xp = y*tan(2.5*TMath::Pi()/180.) + x;

        Float_t x_smeared  = siliconPoint->GetXIn() + dX;
        Float_t xp_smeared = xp + dXp;
        Float_t z_smeared  = siliconPoint->GetZIn() + dZ;
        
        if (fDebug) cout<<" track_id "<<track_id<<" charge "<<charge<<" x "<<x<<" x_sm "<<x_smeared<<" y "<<y<<" y_sm "<<(xp_smeared - x_smeared)/tan(2.5*TMath::Pi()/180.)<<" xp_sm "<<xp_smeared<<" z "<<z<<endl;
        if (fDebug && z > -320.) cout<<" ----------"<<endl;
        
        //---faile for Goran---
        //if (pdgId == PDG_B11 ){//if ( pdgId == PDG_He4 || pdgId == PDG_Li7){//
          if (fDebug) cout<<" pdgId "<<pdgId<<endl;
          //|| pdgId == PDG_Li7 || pdgId == PDG_Li8 || pdgId == PDG_He4 || pdgId == PDG_Be9 || pdgId == PDG_H2){
          
          //clean/true hit
          BmnHit* hit = new((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()])BmnHit(0, TVector3(x, y, z), 
          TVector3(err[0], err[1], err[2]), iPoint);
          //hit->SetIndex(fBmnHitsArray->GetEntriesFast() - 1);
          hit->SetType(1);
          hit->SetStation(SiliconStationSet->GetPointStationOwnership(siliconPoint->GetZIn()));
          hit->SetIndex(track_id);
          hit->SetType(pdgId);
          //hit->SetELoss(static_cast< double>(pdgId));//tmp
          if(rand_gen.Uniform() <= .25) xp_smeared = -999.;
          
          if(rand_gen.Uniform() <= .90) {
            
            //sim hit smeared
            BmnHit* hit2 = new((*fBmnHitsArray2)[fBmnHitsArray2->GetEntriesFast()])BmnHit(0, TVector3(x_smeared, xp_smeared, z_smeared), 
            TVector3(err[0], err[1], err[2]), iPoint);
            //hit2->SetIndex(fBmnHitsArray2->GetEntriesFast() - 1);
            hit2->SetType(pdgId);
            hit2->SetStation(SiliconStationSet->GetPointStationOwnership(siliconPoint->GetZIn()));
            hit2->SetIndex(track_id);
            
          }
       // }
       
    }//iPoint
    
}

void BmnSiliconHitProducerSRC::Finish() {
  
  if (fDebug) {

   printf("BmnSiliconHitProducerSRC:  ");
   fOutputFileName = "hSiliconHitProducerSRC.root";
   cout<< fOutputFileName <<endl;
   TFile file(fOutputFileName, "RECREATE");
   fList.Write();
   file.Close();
  }
  
}

ClassImp(BmnSiliconHitProducerSRC)
