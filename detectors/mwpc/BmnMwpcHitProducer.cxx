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
#include "TFile.h"

using namespace std;
using namespace TMath;

BmnMwpcHitProducer::BmnMwpcHitProducer() :
  //(Int_t num = 1) :
  fEventNo(0),
  fOnlyPrimary(kFALSE),
  fBmnMwpcPointsArray(nullptr),
  fBmnPointsArray(nullptr),
  fBmnMwpcDigitsArray(nullptr),
  fMCTracksArray(nullptr),
  fBmnMwpcHitsArray(nullptr),
  fBmnHitsArray(nullptr),
  hY_vsX2(nullptr),
  hY_vsX3(nullptr),
  hY_vsX_pl0(nullptr),
  hY_vsX_pl1(nullptr),
  hY_vsX_pl2(nullptr),
  hY_vsX_star2(nullptr),
  hY_vsX_star3(nullptr)
{
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
  
  //--some hists--
  if (fDebug) {
    hY_vsX2 = new TH2D("Y_vsX2","Y_vsX2", 140,-35,35, 140,-35,35);fList.Add(hY_vsX2);
    hY_vsX3 = new TH2D("Y_vsX3","Y_vsX3", 140,-35,35, 140,-35,35);fList.Add(hY_vsX3);
    hY_vsX_star2 = new TH2D("Y_vsX_star2","Y_vsX_star2", 140,-35,35, 140,-35,35);fList.Add(hY_vsX_star2);
    hY_vsX_star3 = new TH2D("Y_vsX_star3","Y_vsX_star3", 140,-35,35, 140,-35,35);fList.Add(hY_vsX_star3);
    hY_vsX_pl0 = new TH2D("Y_vsX_pl0","Y_vsX_pl0", 140,-35,35, 140,-35,35);fList.Add(hY_vsX_pl0);
    hY_vsX_pl1 = new TH2D("Y_vsX_pl1","Y_vsX_pl1", 140,-35,35, 140,-35,35);fList.Add(hY_vsX_pl1);
    hY_vsX_pl2 = new TH2D("Y_vsX_pl2","Y_vsX_pl2", 140,-35,35, 140,-35,35);fList.Add(hY_vsX_pl2);
  }
  
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
    Double_t kCheaksize = 12. + 0.5*dw;//20.78;/?
    Double_t a_side     = 4*Sqrt(3) + 0.5*dw;
    Double_t ShiftX[4]; Double_t ShiftY[4];
    //shift for run7
    ShiftX[2] = -0.271 - 0.5;
    ShiftY[2] = -6.038 + 4.5;
    
    ShiftX[3] = -0.234 - 0.5;
    ShiftY[3] = -6.140 + 4.5;
    
    Int_t    Nmc_tracks = 0;
    Int_t    Id_curr    = -1;
    Int_t    Ch_curr    = -1;
    Int_t    plNumf = -1;
    Int_t    plNuml = -1;
    Double_t xf = -999.;
    Double_t yf = -999.;
    Double_t zf = -999.;
    Double_t xl = -999.;
    Double_t yl = -999.;
    Double_t zl = -999.;
    
    Double_t xi[6], yi[6], zi[6];
    
    for (Int_t i = 0; i < fBmnPointsArray->GetEntriesFast(); i++) {//read
      BmnMwpcPoint* point = (BmnMwpcPoint*) fBmnPointsArray->UncheckedAt(i);
      
      Int_t IsPrimary = point->GetIsPrimary();
      Int_t charge    = point->GetCharge() ;
      Int_t pdg       = point->GetPdgId() ;
      Double_t     Pz = point->GetPz();
      
      if (charge == 0)    continue; //qgsm
      //if (charge != 18)    continue;//ion carbon
      // if (fDebug) cout<<" charge "<<charge<<endl;
      if (IsPrimary == 0) continue;
      if (Pz < 0.1)       continue;//tmp
      Int_t track_id  = point->GetTrackID();
      if (track_id != Id_curr){
        Id_curr = track_id;
        Ch_curr = -1;
        if (fDebug) cout<<" ----"<<endl;
      }
      
      
      Double_t x = point->GetX();
      Double_t y = point->GetY();
      Double_t z = point->GetZ();
      
      Int_t  ChId = -1;
      if (z < -847.) ChId = 0;
      if (z > -756.  &&  z < -750.) ChId = 1;
      if (z > -360.  &&  z < -354.) ChId = 2;
      if (z > -211.) ChId = 3;
      
      if (ChId == 2 && fDebug) hY_vsX2-> Fill(x,y);
      if (ChId == 3 && fDebug) hY_vsX3-> Fill(x,y);
      
      if (Ch_curr != ChId){
        Ch_curr = ChId;
        plNumf  = -1;
        plNuml  = -1;
       
      }
      
      if (fDebug) cout<<"=== track_id "<<track_id<<" ChId "<<ChId<<" xmc "<<x<<" y "<<y<<" z "<<z<<" Pz "<<Pz<<endl;
      //---
      TGeoNode* curNode = gGeoManager->FindNode(x, y, z);
      TString nameNode  = TString(curNode->GetName());
      //if (fDebug) cout<<" nameNode "<<nameNode<<endl;
      Short_t planeNum  = TString(nameNode(Int_t(nameNode.Length() - 1), 1)).Atoi();
      Int_t time_wire = 70;
      if (ChId == 2 && planeNum == 0 && fDebug) hY_vsX_pl0->Fill(x,y);
      if (ChId == 2 && planeNum == 1 && fDebug) hY_vsX_pl1->Fill(x,y);
      if (ChId == 2 && planeNum == 2 && fDebug) hY_vsX_pl2->Fill(x,y);
      
      if (plNumf < 0) {
        plNumf = planeNum;
        plNuml = planeNum + 3;
        xf = x ;
        yf = y ;
        zf = z;
      }
      
      if (plNumf > -1 && plNuml == planeNum){
        if (ChId == 2) Nmc_tracks++;
        xl = x ;
        yl = y ;
        zl = z;
        
        Double_t Ax = (xf - xl)/(zf - zl);
        Double_t Ay = (yf - yl)/(zf - zl);
      
        for (Int_t ipl = 0; ipl < 6; ipl++) {
          xi[ipl] = Ax * ( ipl - plNumf ) + xf;
          yi[ipl] = Ay * ( ipl - plNumf ) + yf;
          if (fDebug) cout<<" xi["<<ipl<<"] "<<xi[ipl]<<" yi["<<ipl<<"] "<<yi[ipl]<<" z "<<zf + ipl - plNumf;
          xi[ipl] += ShiftX[ChId];//shift to the chamber center
          yi[ipl] += ShiftY[ChId];
          zi[ipl] =  zf + ipl - plNumf;
          //if (fDebug) cout<<" shift to the chamber center xmc "<<xi[ipl]<<" y "<<xi[ipl]<<endl;
        
          Double_t wire_pos  = -1.;//default
          Double_t hit_coord;
        
          if (ipl == 2 || ipl == 5 ){//--x---//if (planeNum == 0 || planeNum == 3 )
            hit_coord = xi[ipl];
            if (fDebug) cout<<" -- x --"<<hit_coord<<" y "<<yi[ipl]<<endl;
            if (fabs(yi[ipl]) > a_side) continue; //cheak perpendicular coord size//wire lenght
          }
          if (ipl == 0 || ipl == 3 ){//--v---//if (planeNum == 1 || planeNum == 4 )
            hit_coord    = 0.5*( xi[ipl] - Sqrt(3)* yi[ipl]);
            Double_t v_t = (Sqrt(3)*xi[ipl] + yi[ipl]) * 0.5;
            if (fDebug) cout<<" -- v --"<<hit_coord<<" v_t "<<v_t<<endl;
            if (fabs(v_t) > a_side) continue;//cheak perpendicular coord size//wire lenght
          }
          if (ipl == 1 || ipl == 4 ){//--u---//if (planeNum == 2 || planeNum == 5 )
            hit_coord    = 0.5*( xi[ipl] + Sqrt(3)* yi[ipl]);
            Double_t u_t = (Sqrt(3)*xi[ipl] - yi[ipl]) * 0.5;
            if (fDebug) cout<<" -- u --"<<hit_coord<<" u_t "<<u_t<<endl;
            if (fabs(u_t) > a_side) continue;//cheak perpendicular coord size//wire lenght
          }
        
          //if (fDebug) cout<<" hit_coord "<<hit_coord<<" kCheaksize "<<kCheaksize<<endl;
          if (fabs(hit_coord) > kCheaksize) continue;  // check hexagon coord size
          
          if (ipl == 1 || ipl == 2 || ipl == 3){ //if (planeNum == 0 || planeNum == 1 || planeNum == 5)
            hit_coord = -hit_coord;//reading direction changing
          }
        
          //--wire calculation--
          wire_pos = (hit_coord + kMiddlePl*dw )/dw;
          
          //current position in wire units
          Int_t nearest_wire = Int_t(wire_pos);
          Double_t wdist = TMath::Abs(wire_pos - nearest_wire);
          //sigma error dependent on the distance to the nearest wire
          Double_t sigm_err = wdist;
          
          if (fDebug) cout<<" Ch "<<ChId<<" track_id "<<track_id<<" xmc "<<x<<" y "<<y<<" pl "<<ipl<<" XUV "<<hit_coord<<" wire "<<wire_pos<<" nearest "<<nearest_wire<<endl;
          //if (fDebug) cout<<" ----"<<endl;
          if (nearest_wire < 0 || nearest_wire > 95 ) continue;
         if (fDebug && ChId == 2 ) hY_vsX_star2->Fill(xi[ipl], yi[ipl]);
         if (fDebug && ChId == 3 ) hY_vsX_star3->Fill(xi[ipl], yi[ipl]);
          
          //------
          
          //---faile for Goran---
         // if (pdg == PDG_B11 ){//if (pdg == PDG_Li7 || pdg == PDG_He4 ){
            if (fDebug) cout<<" pdg "<<pdg<<endl;
           //|| pdg == PDG_Li7 || pdg == PDG_Li8 || pdg == PDG_He4 || pdg == PDG_Be9 || pdg == PDG_H2){
          
            if(rand_gen.Uniform() <= .9) {
              BmnMwpcHit* hit1 = new ((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()])
              BmnMwpcHit(0, TVector3(xi[ipl], yi[ipl], zi[ipl]), TVector3(err[0], err[1], 0.0), i);

              hit1->SetMwpcId(ChId);
              hit1->SetWireNumber(nearest_wire);
              hit1->SetHitId(track_id);
              hit1->SetWireTime(time_wire);
              hit1->SetPlaneId(ipl );
              hit1->SetPdgId(pdg);
            }
         // }// if (pdg == PDG_B11 
          /*
            BmnMwpcHit* hit2 = new ((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()])
            BmnMwpcHit(0, TVector3(point->GetX(), point->GetY(), z), TVector3(err[0], err[1], 0.0), i);

            hit1->SetMwpcId(ChId);
            hit1->SetWireNumber(not_so_near);
            hit1->SetHitId(track_id);
            hit1->SetWireTime(time_wire);
            hit1->SetPlaneId(planeNum);
          */
        
          plNumf  = -1;
          plNuml  = -1;
        }//ipl
      }//if (plNumf > 0 && plNuml > 0){
    }//i//read
    if (fDebug) cout<<" Nmc_tracks "<<Nmc_tracks<<endl;
   
  return kBMNSUCCESS;
}//exec

BmnStatus BmnMwpcHitProducer::ProcessDigits() {
    return kBMNSUCCESS;
}

void BmnMwpcHitProducer::Finish() {
  if (fDebug){
    printf("BmnMwpcHitProducer: write hists to file... ");
    fOutputFileName = "hMWPCHitProducer_run1.root";
    cout<< fOutputFileName <<endl;
    TFile file(fOutputFileName, "RECREATE");
    fList.Write();
    file.Close();
  }
}

ClassImp(BmnMwpcHitProducer)
