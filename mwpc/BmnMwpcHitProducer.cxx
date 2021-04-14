// Author: Vasilisa Lenivenko <vasilisa@jinr.ru> 2021-03-11

#include "BmnMwpcHitProducer.h"
#include "FairHit.h"
#include "FairMCPoint.h"
#include "BmnMwpcPoint.h"
#include "CbmMCTrack.h"
#include "BmnHit.h"
#include "BmnMwpcHit.h"
#include "TGeoManager.h"
//#include "TRandom.h"
#include "TCanvas.h"

using namespace std;
using namespace TMath;

BmnMwpcHitProducer::BmnMwpcHitProducer() : //(Int_t num = 1) :
 fEventNo(0),
fOnlyPrimary(kFALSE) {
    fInputBranchName = "MWPCPoint";
    fOutputHitsBranchName = "BmnMwpcHit";
    fMwpcNum = 1;
    fRunType = "points";
    TString str;
    str.Form("%d", fMwpcNum);
    fInputMCBranchName   = TString("MWPC") + str + TString("Point");
    fInputDigiBranchName = TString("bmn_mwpc_digit");
}

BmnMwpcHitProducer::~BmnMwpcHitProducer() {

}

InitStatus BmnMwpcHitProducer::Init() {
  if (fDebug) cout << " BmnMwpcHitProducer::Init() " << endl;
  
  rand_gen.SetSeed(5);
  FairRootManager* ioman = FairRootManager::Instance();
  fBmnPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
  fMCTracksArray  = (TClonesArray*) ioman->GetObject("MCTrack");
  fBmnHitsArray   = new TClonesArray(fOutputHitsBranchName, 100);
  ioman->Register(fOutputHitsBranchName, "MWPC", fBmnHitsArray, kTRUE);
  
  return kSUCCESS;
}

void BmnMwpcHitProducer::Exec(Option_t* opt) {
  if (fDebug) cout << endl;
  if (fDebug) cout << "BmnMwpcHitProducer::Exec() started!" << endl;
  if (fDebug) printf("Event number: %d\n", fEventNo++);

  fBmnHitsArray->Delete();
  if (!fBmnPointsArray) {
    Error("BmnDchHitProducer::Exec()", " !!! Unknown branch name !!! ");
    return;
  }
  
  ProcessPoints();
  
  if (fDebug) cout << "======================== BmnMwpcHitProducer::Exec() finished ========================" << endl;
}

BmnStatus BmnMwpcHitProducer::ProcessPoints() {

    Double_t err[3] = {0.25 / Sqrt(12), 0.25 / Sqrt(12), 0.}; // Uncertainties of coordinates
    //some consts
    Double_t dw         = 0.25; // [cm] // wires step
    Double_t kMiddlePl  = 47.25; // Center of wires plane
    Double_t kCheaksize = 20.78;
    Double_t ShiftX[4]; Double_t ShiftY[4];
    //shift for run7
    ShiftX[2] = -0.271 - 0.5;
    ShiftY[2] = -6.038 + 4.5;
    
    ShiftX[3] = -0.234 - 0.5;
    ShiftY[3] = -6.140 + 4.5;
    
    Int_t    Nmc_tracks = 0;
    Int_t    Id_curr    = -1;
    Bool_t   Pl_Hit_Id_curr[6][4];
    
    for (Int_t i = 0; i < fBmnPointsArray->GetEntriesFast(); i++) {
      BmnMwpcPoint* point = (BmnMwpcPoint*) fBmnPointsArray->UncheckedAt(i);
      
      Int_t IsPrimary = point->GetIsPrimary();
      Int_t charge    = point->GetCharge() ;
      Double_t     Pz = point->GetPz();
      
      if (charge == 0)    continue; //qgsm
      //if (charge != 18)    continue;//ion carbon
     // if (fDebug) cout<<" charge "<<charge<<endl;
      if (IsPrimary == 0) continue;
      if (Pz < 1.)        continue;
      Int_t track_id  = point->GetTrackID();
      if (track_id != Id_curr){
        Id_curr = track_id;
        for (Int_t j = 0; j < 4; j++) {
          for (Int_t k = 0; k < 6; k++) {
            Pl_Hit_Id_curr[k][j] = 0;
          }
        }
      }
      Double_t x = point->GetX();
      Double_t y = point->GetY();
      Double_t z = point->GetZ();
      
      
      Int_t  ChId = -1;
      
      if (z < -847.) ChId = 0;
      if (z > -756.  &&  z < -750.) ChId = 1;
      if (z > -360.  &&  z < -354.) ChId = 2;
      if (z > -211.) ChId = 3;
      
      if (z < -854. ){
        Nmc_tracks++;
        if (fDebug) cout<<"-----------"<<endl;
      }
      
      //if (fDebug) cout<<" track_id "<<track_id<<" ChId "<<ChId<<" xmc "<<x<<" y "<<y<<" z "<<z<<endl;
      
      x += ShiftX[ChId];//shift to the chamber center
      y += ShiftY[ChId];
      //if (fDebug) cout<<" shift to the chamber center "<<endl;
     // if (fDebug)  cout<<" xmc "<<x<<" y "<<y<<endl;
      
      //---
      TGeoNode* curNode = gGeoManager->FindNode(x, y, z);
      TString nameNode = TString(curNode->GetName());
      Short_t planeNum = TString(nameNode(Int_t(nameNode.Length() - 1), 1)).Atoi();
      Int_t time_wire = 70;
      if (Pl_Hit_Id_curr[planeNum][ChId] ) continue; 
      Pl_Hit_Id_curr[planeNum][ChId] = 1;
      
      Double_t wire_pos = -1.;//DBL_MAX; //default
      Double_t half_wire = 0.;
      Double_t hit_coord;
      if (planeNum == 0 || planeNum == 3 ) hit_coord = x;
      if (planeNum == 1 || planeNum == 4 ) hit_coord = 0.5*( x - sqrt(3)* y);
      if (planeNum == 2 || planeNum == 5 ) hit_coord = 0.5*( x + sqrt(3)* y);
      if (planeNum == 0 || planeNum == 1 || planeNum == 5){ 
        hit_coord = -hit_coord;
      }
     // if (fDebug)  cout<<" xmc "<<x<<" y "<<y<<" XUV "<<hit_coord<<endl;
      if (fabs(hit_coord) > kCheaksize && y > kCheaksize) continue; // check hexagon size
      
      wire_pos = (hit_coord + kMiddlePl*dw )/dw + half_wire;
      
      //current position in wire units
      Int_t nearest_wire = Int_t(wire_pos);
      Double_t wdist = TMath::Abs(wire_pos - nearest_wire);
      //sigma error dependent on the distance to the nearest wire
      Double_t sigm_err = wdist;
      
      if (fDebug) cout<<" Ch "<<ChId<<" track_id "<<track_id<<" xmc "<<x<<" y "<<y<<" pl "<<planeNum<<" XUV "<<hit_coord<<" wire "<<wire_pos<<" nearest "<<nearest_wire<<endl;
      //if ( (planeNum == 0 || planeNum == 3 ) && fabs(x) > 12.) 
      //cout<<" Ch "<<ChId<<" track_id "<<track_id<<" xmc "<<x<<" y "<<y<<" pl "<<planeNum<<" XUV "<<hit_coord<<" wire "<<wire_pos<<" nearest "<<nearest_wire<<endl;
      
      if (nearest_wire < 0 || nearest_wire > 95 ) continue;
      
      //------
      if(rand_gen.Uniform() <= 1.) {
        BmnMwpcHit* hit1 = new ((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()])
       // BmnMwpcHit(0, TVector3(point->GetX(), point->GetY(), z), TVector3(err[0], err[1], 0.0), i);
        BmnMwpcHit(0, TVector3(x, y, z), TVector3(err[0], err[1], 0.0), i);

        hit1->SetMwpcId(ChId);
        hit1->SetWireNumber(nearest_wire);
        hit1->SetHitId(track_id);
        hit1->SetWireTime(time_wire);
        hit1->SetPlaneId(planeNum);
      }
      
      //
      /*
        BmnMwpcHit* hit2 = new ((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()])
        BmnMwpcHit(0, TVector3(point->GetX(), point->GetY(), z), TVector3(err[0], err[1], 0.0), i);

        hit1->SetMwpcId(ChId);
        hit1->SetWireNumber(not_so_near);
        hit1->SetHitId(track_id);
        hit1->SetWireTime(time_wire);
        hit1->SetPlaneId(planeNum);
      */
    }//i
    
    if (fDebug) cout<<" Nmc_tracks "<<Nmc_tracks<<endl;
   
  return kBMNSUCCESS;
}

BmnStatus BmnMwpcHitProducer::ProcessDigits() {
    return kBMNSUCCESS;
}

void BmnMwpcHitProducer::Finish() {
  
}

ClassImp(BmnMwpcHitProducer)
