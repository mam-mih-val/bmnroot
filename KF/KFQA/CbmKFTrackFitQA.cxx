/*
 *====================================================================
 *
 *  CBM KF Track Quality
 *  
 *  Authors: M.Zyzak
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  KF Fit performance 
 *
 *====================================================================
 */
#include "CbmKFTrackFitQA.h"

#include "CbmKF.h"
#include "CbmKFMath.h"
#include "TDatabasePDG.h"
#include "CbmKFTrack.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmMCTrack.h"
#include "FairMCPoint.h"

#include "TFile.h"
#include "TDirectory.h"
#include "TTree.h"
#include "TBranch.h"

#include <iostream>
#include <cmath>
using namespace std;
using std::vector;

ClassImp(CbmKFTrackFitQA)

CbmKFTrackFitQA::CbmKFTrackFitQA():
  listStsPts(0),
  listMvdPts(0),
  listMCTracks(0),
  listStsTracksMatch(0),
  listStsTracks(0),
  listStsHits(0),
  listMvdHits(0),
  listMvdHitMatches(0),
  listStsClusters(0),
  listStsDigi(0),


// Names of files
  outfileName("outCbmTrackError.root"),

  vStsHitMatch(),

  res_STShit_x(0),
  res_STShit_y(0),
  pull_STShit_x(0),
  pull_STShit_y(0),

  res_MVDhit_x(0),
  res_MVDhit_y(0),
  pull_MVDhit_x(0),
  pull_MVDhit_y(0),

  res_AtPV_x(0),
  res_AtPV_y(0),
  res_AtPV_tx(0),
  res_AtPV_ty(0),
  res_AtPV_qp(0),

  pull_AtPV_x(0),
  pull_AtPV_y(0),
  pull_AtPV_tx(0),
  pull_AtPV_ty(0),
  pull_AtPV_qp(0),


  res_AtFP_x(0),
  res_AtFP_y(0),
  res_AtFP_tx(0),
  res_AtFP_ty(0),
  res_AtFP_qp(0),

  pull_AtFP_x(0),
  pull_AtFP_y(0),
  pull_AtFP_tx(0),
  pull_AtFP_ty(0),
  pull_AtFP_qp(0),

  q_QA(0),
  dp_p(0),

  ggg(0),

  Nback(0)
{
  TDirectory *currentDir = gDirectory;
  gDirectory->cd(outfileName.Data());

  ggg = new TH1F("ggg", "ggg", 6, -0.5, 5.5);
  q_QA = new TProfile("q_QA", "q quality", 100, 0.0, 1.0, 0.0, 100.0);
  q_QA -> GetXaxis() -> SetTitle ("p, GeV/c");
  q_QA -> GetYaxis() -> SetTitle ("Q determenition efficiency, %");

  dp_p = new TProfile("dp_p", "dp_p", 100, 0.0, 1.0, 0.0, 5.0);
  dp_p -> GetXaxis() -> SetTitle ("p, GeV/c");
  dp_p -> GetYaxis() -> SetTitle ("#Deltap/p, %");

  res_STShit_x  = new TH1F("residual_STShit_x", "residual_STShit_x", 200, -10., 10.);
  res_STShit_x  -> GetXaxis() -> SetTitle ("dX, um");
  res_STShit_y  = new TH1F("residual_STShit_y", "residual_STShit_y", 200, -10., 10.);
  res_STShit_y  -> GetXaxis() -> SetTitle ("dY, um");
  pull_STShit_x   = new TH1F("pull_STShit_x", "pull_STShit_x", 100, -15., 15.);
  pull_STShit_y   = new TH1F("pull_STShit_y", "pull_STShit_y", 100, -15., 15.);

  res_MVDhit_x  = new TH1F("residual_MVDhit_x", "residual_MVDhit_x", 200, -30., 30.);
  res_MVDhit_x  -> GetXaxis() -> SetTitle ("dX, um");
  res_MVDhit_y  = new TH1F("residual_MVDhit_y", "residual_MVDhit_y", 200, -30., 30.);
  res_MVDhit_y  -> GetXaxis() -> SetTitle ("dY, um");
  pull_MVDhit_x   = new TH1F("pull_MVDhit_x", "pull_MVDhit_x", 100, -5., 5.);
  pull_MVDhit_y   = new TH1F("pull_MVDhit_y", "pull_MVDhit_y", 100, -5., 5.);


  res_AtPV_x  = new TH1F("residual_AtPV_x", "residual_AtPV_x", 2000, -1., 1.);
  res_AtPV_x  -> GetXaxis() -> SetTitle ("dX, cm");
  res_AtPV_y  = new TH1F("residual_AtPV_y", "residual_AtPV_y", 2000, -1., 1.);
  res_AtPV_y  -> GetXaxis() -> SetTitle ("dY, cm");
  res_AtPV_tx = new TH1F("residual_AtPV_tx", "residual_AtPV_tx", 200, -0.004, 0.004);
  res_AtPV_tx -> GetXaxis() -> SetTitle ("dtx");
  res_AtPV_ty = new TH1F("residual_AtPV_ty", "residual_AtPV_ty", 200, -0.004, 0.004);
  res_AtPV_ty -> GetXaxis() -> SetTitle ("dty");
  res_AtPV_qp = new TH1F("residual_AtPV_qp", "residual_AtPV_qp", 200, -0.05, 0.05);
  res_AtPV_qp -> GetXaxis() -> SetTitle ("d(Q/P), c/GeV");

  pull_AtPV_x   = new TH1F("pull_AtPV_x", "pull_AtPV_x", 100, -5., 5.);
  pull_AtPV_y   = new TH1F("pull_AtPV_y", "pull_AtPV_y", 100, -5., 5.);
  pull_AtPV_tx  = new TH1F("pull_AtPV_tx", "pull_AtPV_tx", 100, -5., 5.);
  pull_AtPV_ty  = new TH1F("pull_AtPV_ty", "pull_AtPV_ty", 100, -5., 5.);
  pull_AtPV_qp  = new TH1F("pull_AtPV_qp", "pull_AtPV_qp", 100, -5., 5.);

  res_AtFP_x  = new TH1F("residual_AtFP_x", "residual_AtFP_x", 200, -50., 50.);
  res_AtFP_x  -> GetXaxis() -> SetTitle ("dX, cm");
  res_AtFP_y  = new TH1F("residual_AtFP_y", "residual_AtFP_y", 200, -400., 400.);
  res_AtFP_y  -> GetXaxis() -> SetTitle ("dY, cm");
  res_AtFP_tx = new TH1F("residual_AtFP_tx", "residual_AtFP_tx", 200, -0.004, 0.004);
  res_AtFP_tx -> GetXaxis() -> SetTitle ("dtx, GeV/c");
  res_AtFP_ty = new TH1F("residual_AtFP_ty", "residual_AtFP_ty", 200, -0.004, 0.004);
  res_AtFP_ty -> GetXaxis() -> SetTitle ("dty, GeV/c");
  res_AtFP_qp = new TH1F("residual_AtFP_qp", "residual_AtFP_qp", 200, -0.05, 0.05);
  res_AtFP_qp -> GetXaxis() -> SetTitle ("d(Q/P), c/GeV");

  pull_AtFP_x   = new TH1F("pull_AtFP_x", "pull_AtFP_x", 100, -5., 5.);
  pull_AtFP_y   = new TH1F("pull_AtFP_y", "pull_AtFP_y", 100, -5., 5.);
  pull_AtFP_tx  = new TH1F("pull_AtFP_tx", "pull_AtFP_tx", 100, -5., 5.);
  pull_AtFP_ty  = new TH1F("pull_AtFP_ty", "pull_AtFP_ty", 100, -5., 5.);
  pull_AtFP_qp  = new TH1F("pull_AtFP_qp", "pull_AtFP_qp", 100, -5., 5.);

  gDirectory = currentDir;
}

CbmKFTrackFitQA::~CbmKFTrackFitQA()
{
  if(res_STShit_x) delete res_STShit_x;
  if(res_STShit_y) delete res_STShit_y;
  if(pull_STShit_x) delete pull_STShit_x;
  if(pull_STShit_y) delete pull_STShit_y;

  if(res_MVDhit_x) delete res_MVDhit_x;
  if(res_MVDhit_y) delete res_MVDhit_y;
  if(pull_MVDhit_x) delete pull_MVDhit_x;
  if(pull_MVDhit_y) delete pull_MVDhit_y;

  if(res_AtPV_x)  delete res_AtPV_x;
  if(res_AtPV_y)  delete res_AtPV_y;
  if(res_AtPV_tx) delete res_AtPV_tx;
  if(res_AtPV_ty) delete res_AtPV_ty;
  if(res_AtPV_qp) delete res_AtPV_qp;

  if(pull_AtPV_x)  delete pull_AtPV_x;
  if(pull_AtPV_y)  delete pull_AtPV_y;
  if(pull_AtPV_tx) delete pull_AtPV_tx;
  if(pull_AtPV_ty) delete pull_AtPV_ty;
  if(pull_AtPV_qp) delete pull_AtPV_qp;

  if(res_AtFP_x)  delete res_AtFP_x;
  if(res_AtFP_y)  delete res_AtFP_y;
  if(res_AtFP_tx) delete res_AtFP_tx;
  if(res_AtFP_ty) delete res_AtFP_ty;
  if(res_AtFP_qp) delete res_AtFP_qp;

  if(pull_AtFP_x)  delete pull_AtFP_x;
  if(pull_AtFP_y)  delete pull_AtFP_y;
  if(pull_AtFP_tx) delete pull_AtFP_tx;
  if(pull_AtFP_ty) delete pull_AtFP_ty;
  if(pull_AtFP_qp) delete pull_AtFP_qp;
}

InitStatus CbmKFTrackFitQA::ReInit()
{
  return Init();
}

InitStatus CbmKFTrackFitQA::Init()
{
  FairRootManager *fManger = FairRootManager::Instance();

  listMCTracks = dynamic_cast<TClonesArray*>( fManger->GetObject("MCTrack") );
  listStsPts = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsPoint") );
  listStsTracks = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsTrack") );
  listStsTracksMatch = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsTrackMatch") );
  listStsHits = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsHit") );
  listStsClusters = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsCluster") );
  listStsDigi = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsDigi") );

  if( CbmKF::Instance()->vMvdMaterial.size() == 0 ) 
  {
    listMvdPts = 0;
    listMvdHits = 0;
    listMvdHitMatches = 0;
  }
  else 
  {
    listMvdPts = dynamic_cast<TClonesArray*>(  fManger->GetObject("MvdPoint") );
    listMvdHits = dynamic_cast<TClonesArray*>(  fManger->GetObject("MvdHit") );
    listMvdHitMatches = dynamic_cast<TClonesArray*>(  fManger->GetObject("MvdHitMatch") );
  }

  return kSUCCESS;
}

void CbmKFTrackFitQA::Exec(Option_t * option)
{
  FillHitHistos();

  CbmKFTrErrMCPoints* MCTrackSortedArray = new CbmKFTrErrMCPoints[listMCTracks->GetEntriesFast()+1];
  if(CbmKF::Instance()->vMvdMaterial.size() != 0)
    for (Int_t iMvd=0; iMvd<listMvdPts->GetEntriesFast(); iMvd++)
    {
      CbmMvdPoint* MvdPoint = (CbmMvdPoint*)listMvdPts->At(iMvd);
      //MCTrackSortedArray[MvdPoint->GetTrackID()].SetMvdPoint(MvdPoint);
      MCTrackSortedArray[MvdPoint->GetTrackID()].MvdArray.push_back(MvdPoint);
    }
  for (Int_t iSts=0; iSts<listStsPts->GetEntriesFast(); iSts++)
  {
     CbmStsPoint* StsPoint = (CbmStsPoint*)listStsPts->At(iSts);
     //MCTrackSortedArray[StsPoint->GetTrackID()].SetStsPoint(StsPoint);
     MCTrackSortedArray[StsPoint->GetTrackID()].StsArray.push_back(StsPoint);
  }
  for (Int_t itrack=0; itrack<listStsTracks->GetEntriesFast(); itrack++)
  {
     CbmStsTrack* StsTrack = (CbmStsTrack*)listStsTracks->At(itrack);
     CbmTrackMatch* StsTrackMatch = (CbmTrackMatch*)listStsTracksMatch->At(itrack);
     if(StsTrackMatch -> GetNofMCTracks() == 0) continue;
     CbmMCTrack* MCTrack = (CbmMCTrack*)listMCTracks->At(StsTrackMatch->GetMCTrackId());
     CbmKFTrack KFTrack(*StsTrack);
     FillHistoAtFirstPoint(&MCTrackSortedArray[StsTrackMatch->GetMCTrackId()], MCTrack, &KFTrack);
//      if(MCTrack->GetP() >= 1 && MCTrack->GetMotherId()==-1 && MCTrackSortedArray[StsTrackMatch->GetMCTrackId()].MvdArray.size()>1)
//      {
       FillHistoAtParticleVertex(MCTrack, &KFTrack);
//      }
  }
  delete[] MCTrackSortedArray;
}
void CbmKFTrackFitQA::Finish()
{
  Write();
}

void CbmKFTrackFitQA::FillHistoAtParticleVertex(CbmMCTrack *track_mc, CbmKFTrack *track_kf)
{
  CbmKFTrackInterface *tr_help1 = track_kf;
  CbmKFTrack *tr1 = new CbmKFTrack(*tr_help1);
  CbmKFTrackInterface *tr_help = tr1;

  tr_help -> Extrapolate(track_mc->GetStartZ()); //extrapolating of the track to the prodaction point

  Double_t *fT = tr_help -> GetTrack();
  Double_t *fC = tr_help -> GetCovMatrix();

  // getting the q (charge) of the mc track using the pdg information

  Double_t qtrack=0;
  if ( track_mc->GetPdgCode() < 9999999 )
    qtrack = TDatabasePDG::Instance()->GetParticle(track_mc->GetPdgCode())->Charge()/3.0;
  else    qtrack = 0;

    Double_t PointPx = track_mc->GetPx();
    Double_t PointPy = track_mc->GetPy();
    Double_t PointPz = track_mc->GetPz();
    Double_t P_mc = sqrt(PointPx*PointPx+PointPy*PointPy+PointPz*PointPz);

    //differences of the KF track and MC track parameters calculation
    Double_t ddx = fT[0] - track_mc->GetStartX();                         // The difference between x coordinates of the reconstructed and MC tracks
    Double_t ddy = fT[1] - track_mc->GetStartY();                         // The difference between y coordinates of the reconstructed and MC tracks
    Double_t ddtx = fT[2] - PointPx/PointPz;                             // The difference between tx of the reconstructed and MC tracks
    Double_t ddty = fT[3] - PointPy/PointPz;        // The difference between ty of the reconstructed and MC tracks
    Double_t ddqp = (fabs(1./fT[4]) - P_mc)/P_mc;  // p resolution
    Double_t ddqp_p = fT[4] - (qtrack/sqrt(PointPx*PointPx+PointPy*PointPy+PointPz*PointPz));  //p residual

    //differences of the KF track and MC track parameters
    res_AtPV_x  -> Fill(ddx);
    res_AtPV_y  -> Fill(ddy);
    res_AtPV_tx -> Fill(ddtx);
    res_AtPV_ty -> Fill(ddty);
    if(finite(fT[4]) && (fabs(fT[4]) > 1.e-20)) res_AtPV_qp -> Fill(ddqp);
    //pulls of the parameters
    if( finite(fC[0])  && fC[0]>0  ) pull_AtPV_x  -> Fill(ddx/sqrt(fC[0]));
    if( finite(fC[2])  && fC[2]>0  ) pull_AtPV_y  -> Fill(ddy/sqrt(fC[2]));
    if( finite(fC[5])  && fC[5]>0  ) pull_AtPV_tx -> Fill(ddtx/sqrt(fC[5]));
    if( finite(fC[9])  && fC[9]>0  ) pull_AtPV_ty -> Fill(ddty/sqrt(fC[9]));
    if( finite(fC[14]) && fC[14]>0 ) pull_AtPV_qp -> Fill(ddqp_p/sqrt(fC[14]));

    if(finite(fT[4]) && (fabs(fT[4]) > 1.e-20))
    {
      if(qtrack == (fabs(fT[4])/fT[4]))
        q_QA->Fill(P_mc, 100.0);
      else
        q_QA->Fill(P_mc, 0.0);

      if( finite(fC[14]) && fC[14]>0 ) dp_p-> Fill(P_mc,fabs(1./fT[4])*sqrt(fC[14])*100,1);
    }
}

void CbmKFTrackFitQA::FillHistoAtFirstPoint(CbmKFTrErrMCPoints *mc_points, CbmMCTrack *track_mc, CbmKFTrack *track_kf)
{
  Double_t *fT = track_kf -> GetTrack();
  Double_t *fC = track_kf -> GetCovMatrix();

  //Double_t StartX = track_kf -> GetHit(0)[];
  Bool_t nomvdpoint = 1;
  Bool_t nostspoint = 1;

  FairMCPoint *MCFirstPoint;

//  if (fabs(track_mc->GetP()) < 1.) return;
//  if (fabs(track_mc->GetPz()) < 0.0001) return;
  //if (fT[4]<0.) return;

  if(!mc_points) return;
  if( (mc_points->MvdArray.size()==0) && (mc_points->StsArray.size()==0) ) return;

  if(mc_points->MvdArray.size() > 0)
    MCFirstPoint = *mc_points->MvdArray.begin();
  else
    MCFirstPoint = *mc_points->StsArray.begin();

  for( vector<CbmMvdPoint*>::iterator imvd = mc_points->MvdArray.begin(); imvd != mc_points->MvdArray.end(); ++imvd)
  {
    MCFirstPoint = *imvd;
//cout << "Z   " << MCFirstPoint->GetZ() << "  fT[5]   " << fT[5] << "  mctrackZ  " << track_mc->GetStartZ() << " X " << fT[0] << " Xmc "<< track_mc->GetStartX()<< 
//"     MCID    " << track_mc->GetMotherId()<< "  MCIDPart  " << MCFirstPoint->GetTrackID() << endl;
    if (fabs(MCFirstPoint->GetZ() - fT[5]) < 1.)
    {
      nomvdpoint=0;
      break;
    }
  }
  if(nomvdpoint)
  {
    for( vector<CbmStsPoint*>::iterator ists = mc_points->StsArray.begin(); ists != mc_points->StsArray.end(); ++ists)
    {
      MCFirstPoint = *ists;
//cout << "Z   " << MCFirstPoint->GetZ() << "  fT[5]   " << fT[5] << "  mctrackZ  " << track_mc->GetStartZ() << " X " << fT[0] << " Xmc "<< track_mc->GetStartX()<< 
//"     MCID    " << track_mc->GetMotherId() << "  MCIDPart  " << MCFirstPoint->GetTrackID() << endl;
      if (fabs(MCFirstPoint->GetZ() - fT[5]) < 1.)
      {
        nostspoint=0;
        break;
      }
    }
  }
  track_kf -> Extrapolate(MCFirstPoint->GetZ());


  // getting of the q (charge) of the mc track using the pdg information
  {
    Double_t qtrack=0;
    if ( track_mc->GetPdgCode() < 9999999 )
      qtrack = TDatabasePDG::Instance()->GetParticle(track_mc->GetPdgCode())->Charge()/3.0;
    else    qtrack = 0;

    Double_t PointPx = MCFirstPoint->GetPx();
    Double_t PointPy = MCFirstPoint->GetPy();
    Double_t PointPz = MCFirstPoint->GetPz();
    Double_t P_mc = sqrt(PointPx*PointPx+PointPy*PointPy+PointPz*PointPz);

    //differences of the KF track and MC track parameters calculation
    Double_t ddx = fT[0] - MCFirstPoint->GetX();                         // The difference between x coordinates of the reconstructed and MC tracks
    Double_t ddy = fT[1] - MCFirstPoint->GetY();                         // The difference between y coordinates of the reconstructed and MC tracks
    Double_t ddtx = fT[2] - PointPx/PointPz;                             // The difference between tx of the reconstructed and MC tracks
    Double_t ddty = fT[3] - PointPy/PointPz;        // The difference between ty of the reconstructed and MC tracks
    Double_t ddqp = (fabs(1./fT[4]) - P_mc)/P_mc;  // p resolution
    Double_t ddqp_p = fT[4] - (qtrack/sqrt(PointPx*PointPx+PointPy*PointPy+PointPz*PointPz));  //p residual

    //differences of the KF track and MC track parameters
    res_AtFP_x  -> Fill(ddx*10000.);
    res_AtFP_y  -> Fill(ddy*10000.);
    res_AtFP_tx -> Fill(ddtx);
    res_AtFP_ty -> Fill(ddty);
    if(finite(fT[4]) && (fabs(fT[4]) > 1.e-20)) res_AtFP_qp -> Fill(ddqp);
    //pulls of the parameters
    if( finite(fC[0])  && fC[0]>0  ) pull_AtFP_x  -> Fill(ddx/sqrt(fC[0]));
    if( finite(fC[2])  && fC[2]>0  ) pull_AtFP_y  -> Fill(ddy/sqrt(fC[2]));
    if( finite(fC[5])  && fC[5]>0  ) pull_AtFP_tx -> Fill(ddtx/sqrt(fC[5]));
    if( finite(fC[9])  && fC[9]>0  ) pull_AtFP_ty -> Fill(ddty/sqrt(fC[9]));
    if( finite(fC[14]) && fC[14]>0 ) pull_AtFP_qp -> Fill(ddqp_p/sqrt(fC[14]));

    if(finite(fT[4]) && (fabs(fT[4]) > 1.e-20))
    {
      if(qtrack == (fabs(fT[4])/fT[4]))
        q_QA->Fill(P_mc, 100.0);
      else
        q_QA->Fill(P_mc, 0.0);

      if( finite(fC[14]) && fC[14]>0 ) dp_p-> Fill(P_mc,fabs(1./fT[4])*sqrt(fC[14])*100,1);
    }
//cout << ddx <<"  "<< ddx/sqrt(fC[0]) << endl;
  }
}

void CbmKFTrackFitQA::Write()
{
  //This function writes obtained histograms to the root file
  TDirectory *curr = gDirectory;
  TFile *currentFile = gFile;
  TFile* fout = new TFile(outfileName.Data(),"Recreate");

  //differences of the KF track and MC track parameters
  res_AtPV_x  -> Write();
  res_AtPV_y  -> Write();
  res_AtPV_tx -> Write();
  res_AtPV_ty -> Write();
  res_AtPV_qp -> Write();
  //pulls of the parameters
  pull_AtPV_x  -> Write();
  pull_AtPV_y  -> Write();
  pull_AtPV_tx -> Write();
  pull_AtPV_ty -> Write();
  pull_AtPV_qp -> Write();
  //differences of the KF track and MC track parameters

  res_AtFP_x  -> Write();
  res_AtFP_y  -> Write();
  res_AtFP_tx -> Write();
  res_AtFP_ty -> Write();
  res_AtFP_qp -> Write();
  //pulls of the parameters
  pull_AtFP_x  -> Write();
  pull_AtFP_y  -> Write();
  pull_AtFP_tx -> Write();
  pull_AtFP_ty -> Write();
  pull_AtFP_qp -> Write();

  res_STShit_x -> Write();
  res_STShit_y -> Write();
  pull_STShit_x -> Write();
  pull_STShit_y -> Write();

  res_MVDhit_x -> Write();
  res_MVDhit_y -> Write();
  pull_MVDhit_x -> Write();
  pull_MVDhit_y -> Write();

  ggg->Write();
  q_QA -> Write();
  dp_p->Write();

  fout -> Close();
  fout -> Delete();
  gFile = currentFile;
  gDirectory = curr;
}

void CbmKFTrackFitQA::Save()
{
  //This function writes obtained histograms to the root file

  //differences of the KF track and MC track parameters
  res_AtPV_x  -> GetXaxis() -> SetTitle ("dX, cm");
  res_AtPV_x  -> SaveAs("res_AtPV_x.eps");
  res_AtPV_y  -> GetXaxis() -> SetTitle ("dY, cm");
  res_AtPV_y  -> SaveAs("res_AtPV_y.gif");
  res_AtPV_tx -> GetXaxis() -> SetTitle ("dtx");
  res_AtPV_tx -> SaveAs("res_AtPV_tx.gif");
  res_AtPV_ty -> GetXaxis() -> SetTitle ("dty");
  res_AtPV_ty -> SaveAs("res_AtPV_ty.gif");
  res_AtPV_qp -> GetXaxis() -> SetTitle ("d(Q/P), c/GeV");
  res_AtPV_qp -> SaveAs("res_AtPV_qp.gif");

  //pulls of the parameters
  pull_AtPV_x  -> SaveAs("pull_AtPV_x.gif");
  pull_AtPV_y  -> SaveAs("pull_AtPV_y.gif");
  pull_AtPV_tx -> SaveAs("pull_AtPV_tx.gif");
  pull_AtPV_ty -> SaveAs("pull_AtPV_ty.gif");
  pull_AtPV_qp -> SaveAs("pull_AtPV_qp.gif");

  //differences of the KF track and MC track parameters
  res_AtFP_x  -> GetXaxis() -> SetTitle ("dX, cm");
  res_AtFP_x  -> SaveAs("res_AtFP_x.gif");
  res_AtFP_y  -> GetXaxis() -> SetTitle ("dY, cm");
  res_AtFP_y  -> SaveAs("res_AtFP_y.gif");
  res_AtFP_tx -> GetXaxis() -> SetTitle ("dtx, GeV/c");
  res_AtFP_tx -> SaveAs("res_AtFP_tx.gif");
  res_AtFP_ty -> GetXaxis() -> SetTitle ("dty, GeV/c");
  res_AtFP_ty -> SaveAs("res_AtFP_ty.gif");
  res_AtFP_qp -> GetXaxis() -> SetTitle ("d(Q/P), c/GeV");
  res_AtFP_qp -> SaveAs("res_AtFP_qp.gif");

  //pulls of the parameters
  pull_AtFP_x  -> SaveAs("pull_AtFP_x.gif");
  pull_AtFP_y  -> SaveAs("pull_AtFP_y.gif");
  pull_AtFP_tx -> SaveAs("pull_AtFP_tx.gif");
  pull_AtFP_ty -> SaveAs("pull_AtFP_ty.gif");
  pull_AtFP_qp -> SaveAs("pull_AtFP_qp.gif");
}

void CbmKFTrackFitQA::FillHitHistos()
{
  vStsHitMatch.resize(listStsHits->GetEntriesFast());
  StsHitMatch();
  for (Int_t iSts=0; iSts<listStsHits->GetEntriesFast(); iSts++)
  {
    CbmStsHit* StsHit = (CbmStsHit*)listStsHits->At(iSts);
    if(vStsHitMatch[iSts] > -1) 
    {
      CbmStsPoint* StsP = (CbmStsPoint*) listStsPts->At(vStsHitMatch[iSts]);
      Double_t dx = (StsHit->GetX() - 0.5*(StsP->GetXIn() + StsP->GetXOut()));
      Double_t dy = (StsHit->GetY() - 0.5*(StsP->GetYIn() + StsP->GetYOut()));
      res_STShit_x -> Fill( dx );
      res_STShit_y -> Fill( dy );
      pull_STShit_x -> Fill( dx/(StsHit->GetDx()) );
      pull_STShit_y -> Fill( dy/(StsHit->GetDy()) );
    }
  }
  if(CbmKF::Instance()->vMvdMaterial.size() != 0)
  {
    for (Int_t iMvd=0; iMvd<listMvdHits->GetEntriesFast(); iMvd++)
    {
      CbmMvdHit* MvdHit = (CbmMvdHit*)listMvdHits->At(iMvd);
      CbmMvdHitMatch* MvdHitMatch = (CbmMvdHitMatch*)listMvdHitMatches->At(iMvd);

      if(MvdHitMatch->GetPointId() > -1) 
      {
        CbmMvdPoint* MvdP = (CbmMvdPoint*) listMvdPts->At(MvdHitMatch->GetPointId());
        if(!MvdP) continue;
        Double_t dx = (MvdHit->GetX() - 0.5*(MvdP->GetX() + MvdP->GetXOut()));
        Double_t dy = (MvdHit->GetY() - 0.5*(MvdP->GetY() + MvdP->GetYOut()));
        res_MVDhit_x -> Fill( dx*10000. );
        res_MVDhit_y -> Fill( dy*10000. );
        pull_MVDhit_x -> Fill( dx/(MvdHit->GetDx()) );
        pull_MVDhit_y -> Fill( dy/(MvdHit->GetDy()) );
      }
    }
  }
}

void CbmKFTrackFitQA::StsHitMatch()
{
  const bool useLinks = 1; // 0 - use HitMatch, one_to_one; 1 - use FairLinks, many_to_many. Set 0 to switch to old definition of efficiency.
  // TODO: fix trunk problem with links. Set useLinks = 1
  
  for (int iH = 0; iH < listStsHits->GetEntriesFast(); iH++){
    CbmStsHit *stsHit = dynamic_cast<CbmStsHit*>(listStsHits->At(iH));
    vStsHitMatch[iH] = -1;
    int gggi=0;
    if (useLinks){
      if (listStsClusters){
  //      if ( NLinks != 2 ) cout << "HitMatch: Error. Hit wasn't matched with 2 clusters." << endl;
          // 1-st cluster
        vector<int> stsPointIds; // save here all mc-points matched with first cluster
        vector<int> stsPointIds_hit;
        int iL = 0;
        FairLink link = stsHit->GetLink(iL);
        CbmStsCluster *stsCluster = dynamic_cast<CbmStsCluster*>( listStsClusters->At( link.GetIndex() ) );
        int NLinks2 = stsCluster->GetNLinks();
        for ( int iL2 = 0; iL2 < NLinks2; iL2++){
          FairLink link2 = stsCluster->GetLink(iL2);
          CbmStsDigi *stsDigi = dynamic_cast<CbmStsDigi*>( listStsDigi->At( link2.GetIndex() ) );
          const int NLinks3 = stsDigi->GetNLinks();
          for ( int iL3 = 0; iL3 < NLinks3; iL3++){
            FairLink link3 = stsDigi->GetLink(iL3);
            int stsPointId = link3.GetIndex();
            stsPointIds.push_back( stsPointId );
          } // for mcPoint
        } // for digi
          // 2-nd cluster
        iL = 1;
        link = stsHit->GetLink(iL);
        stsCluster = dynamic_cast<CbmStsCluster*>( listStsClusters->At( link.GetIndex() ) );
        NLinks2 = stsCluster->GetNLinks();
        for ( int iL2 = 0; iL2 < NLinks2; iL2++){
          FairLink link2 = stsCluster->GetLink(iL2);
          CbmStsDigi *stsDigi = dynamic_cast<CbmStsDigi*>( listStsDigi->At( link2.GetIndex() ) );
          const int NLinks3 = stsDigi->GetNLinks();
          for ( int iL3 = 0; iL3 < NLinks3; iL3++){
            FairLink link3 = stsDigi->GetLink(iL3);
            int stsPointId = link3.GetIndex();
            
            if ( !find(&(stsPointIds[0]), &(stsPointIds[stsPointIds.size()]), stsPointId) ) continue; // check if first cluster matched with same mc-point
//            CbmStsPoint *pt = dynamic_cast<CbmStsPoint*>( listStsPts->At(stsPointId) );
//            if(!pt) continue;
//          CbmMCTrack *MCTrack = dynamic_cast<CbmMCTrack*>( listMCTracks->At( pt->GetTrackID() ) );
//            if ( !MCTrack ) continue;
//            if ( abs(MCTrack->GetPdgCode()) >= 10000 ) continue;
            bool save = 1;
            for(unsigned int iP=0; iP < stsPointIds_hit.size(); iP ++) if(vStsHitMatch[iH] == stsPointIds_hit[iP]) save=0;
            stsPointIds_hit.push_back(stsPointId);
            vStsHitMatch[iH] = stsPointId;
            gggi++;
          } // for mcPoint
        } // for digi
      } // if clusters
    } // if useLinks
    ggg->Fill(gggi);
  } // for hits
}


void CbmKFTrackFitQA::FindBackTracks()
{
/*
  Nback=0;
  int NallTracks=0;

  for (Int_t ientr=0; ientr< treco->GetEntriesFast(); ientr++)
//  for (Int_t ientr=0; ientr< 1; ientr++)
  {
    treco->GetEntry(ientr);
    tmc->GetEntry(ientr);
    cout << "Entry          " << ientr << endl;

    CbmKFTrErrMCPoints MCTrackSortedArray[MCTrackArray->GetEntriesFast()+1];

    for (Int_t iMvd=0; iMvd<MvdPointArray->GetEntriesFast(); iMvd++)
    {
       CbmMvdPoint* MvdPoint = (CbmMvdPoint*)MvdPointArray->At(iMvd);
       //MCTrackSortedArray[MvdPoint->GetTrackID()].SetMvdPoint(MvdPoint);
       MCTrackSortedArray[MvdPoint->GetTrackID()].MvdArray.push_back(MvdPoint);
    }

    for (Int_t iSts=0; iSts<StsPointArray->GetEntriesFast(); iSts++)
    {
       CbmStsPoint* StsPoint = (CbmStsPoint*)StsPointArray->At(iSts);
       //MCTrackSortedArray[StsPoint->GetTrackID()].SetStsPoint(StsPoint);
       MCTrackSortedArray[StsPoint->GetTrackID()].StsArray.push_back(StsPoint);
    }

    NallTracks+=StsTrackArray->GetEntriesFast();

    for (Int_t itrack=0; itrack<StsTrackArray->GetEntriesFast(); itrack++)
//    for (Int_t itrack=0; itrack<1; itrack++)

    {
        CbmStsTrack* StsTrack = (CbmStsTrack*)StsTrackArray->At(itrack);
        CbmTrackMatch* StsTrackMatch = (CbmTrackMatch*)StsTrackMatchArray->At(itrack);
        Int_t NofMC = StsTrackMatch -> GetNofMCTracks();
//cout << itrack << "!!!!!!!" << StsTrackArray->GetEntriesFast() << endl;
//        cout << StsTrackMatch -> GetNofMCTracks() << endl;
        if(StsTrackMatch -> GetNofMCTracks() != 1) continue;
        CbmMCTrack* MCTrack = (CbmMCTrack*)MCTrackArray->At(StsTrackMatch->GetMCTrackId());

        CbmKFTrack *KFTrack;
        KFTrack = new CbmKFTrack(*StsTrack);

//        FindBackTracks(&MCTrackSortedArray[StsTrackMatch->GetMCTrackId()], MCTrack, KFTrack, ientr);
        FillHistoAtFirstPoint(&MCTrackSortedArray[StsTrackMatch->GetMCTrackId()], MCTrack, KFTrack);
//        FillHistoAtParticleVertex(MCTrack, KFTrack);
    }
  }*/
}

void CbmKFTrackFitQA::FindBackTracks(CbmKFTrErrMCPoints *mc_points, CbmMCTrack *track_mc, CbmKFTrack *track_kf, int iEvent)
{
/*
  FILE *fBack = fopen("BackTracks.txt","a+");

  Double_t *fT = track_kf -> GetTrack();
  Double_t *fC = track_kf -> GetCovMatrix();

  Bool_t back = 0;

  FairMCPoint *MCPoint;
  FairMCPoint *MCFirstPoint;

  Bool_t nomvdpoint = 1;
  Bool_t nostspoint = 1;

  if(mc_points->MvdArray.size() > 0.)
  {
    track_kf -> Extrapolate(mc_points->MvdArray[0]->GetZ());
    for( vector<CbmMvdPoint*>::iterator imvd = mc_points->MvdArray.begin(); imvd < mc_points->MvdArray.begin()+1; ++imvd)
    {
      MCFirstPoint = *imvd;
    }
  }
  else
  {
    track_kf -> Extrapolate(mc_points->StsArray[0]->GetZ());
    for( vector<CbmStsPoint*>::iterator ists = mc_points->StsArray.begin(); ists < mc_points->StsArray.begin()+1; ++ists)
    {
      MCFirstPoint = *ists;
    }
  }

  for( vector<CbmMvdPoint*>::iterator imvd = mc_points->MvdArray.begin(); imvd != mc_points->MvdArray.end(); ++imvd)
  {
    MCPoint = *imvd;
    if(MCPoint->GetPz()<0) 
    {
      back=1;
    }
  }
  for( vector<CbmStsPoint*>::iterator ists = mc_points->StsArray.begin(); ists != mc_points->StsArray.end(); ++ists)
  {
    MCPoint = *ists;
    if(MCPoint->GetPz()<0)
    {
      back=1;
    }
  }
  // getting of the q (charge) of the mc track using the pdg information
  if (back)
  {
    cout <<"mc "<< sqrt(MCFirstPoint->GetPz()*MCFirstPoint->GetPz()+MCFirstPoint->GetPy()*MCFirstPoint->GetPy()+MCFirstPoint->GetPx()*MCFirstPoint->GetPx()) << endl;
    cout <<"Px "<< MCFirstPoint->GetPx()<<"  ";
    cout <<"Py "<< MCFirstPoint->GetPy()<<"  ";
    cout <<"Pz "<< MCFirstPoint->GetPz()<<endl;
    cout << "re " << fabs(1./fT[4])<<endl; 
    cout <<"PDG = "<<track_mc->GetPdgCode()<<endl;

    Nback++;

    Double_t qtrack=0;
    if ( track_mc->GetPdgCode() < 9999999 )
      qtrack = TDatabasePDG::Instance()->GetParticle(track_mc->GetPdgCode())->Charge()/3.0;
    else    qtrack = 0;

    Double_t PointPx = MCFirstPoint->GetPx();
    Double_t PointPy = MCFirstPoint->GetPy();
    Double_t PointPz = MCFirstPoint->GetPz();

    //differences of the KF track and MC track parameters calculation
    Double_t ddx = fT[0] - MCFirstPoint->GetX();                         // The difference between the x coordinates of the reconstructed and MC tracks
    Double_t ddy = fT[1] - MCFirstPoint->GetY();                         // The difference between the y coordinates of the reconstructed and MC tracks
    Double_t ddtx = fT[2] - PointPx/PointPz;                             // The difference between the tx coordinates of the reconstructed and MC tracks
    Double_t ddty = fT[3] - PointPy/PointPz;                             // The difference between the ty coordinates of the reconstructed and MC tracks
    Double_t ddqp = fT[4] - (qtrack/sqrt(PointPx*PointPx+
                                         PointPy*PointPy+
                                         PointPz*PointPz));              // The difference between the qp coordinates of the reconstructed and MC tracks

    fprintf(fBack,"Event %i\n",iEvent);
    fprintf(fBack,"PDG = %i\n",track_mc->GetPdgCode());
    fprintf(fBack,"MC P = %f, Px = %f, Py = %f, Pz = %f",
      sqrt(MCPoint->GetPz()*MCPoint->GetPz()+MCPoint->GetPy()*MCPoint->GetPy()+MCPoint->GetPx()*MCPoint->GetPx()),
      MCPoint->GetPx(),MCPoint->GetPy(),MCPoint->GetPz());
    fprintf(fBack,"	mc track	reco track\n");
    fprintf(fBack,"X	%f	%f\n",fT[0],MCFirstPoint->GetX());
    fprintf(fBack,"Y	%f	%f\n",fT[1],MCFirstPoint->GetY());
    fprintf(fBack,"Z	%f	%f\n",fT[5],MCFirstPoint->GetZ());
    fprintf(fBack,"Tx	%f	%f\n",fT[2],PointPx/PointPz);
    fprintf(fBack,"Ty	%f	%f\n",fT[3],PointPy/PointPz);
    fprintf(fBack,"qp	%f	%f\n\n",fT[4],qtrack/sqrt(PointPx*PointPx+PointPy*PointPy+PointPz*PointPz));

    //differences of the KF track and MC track parameters
    res_AtFP_x  -> Fill(ddx*10000.);
    res_AtFP_y  -> Fill(ddy*10000.);
    res_AtFP_tx -> Fill(ddtx);
    res_AtFP_ty -> Fill(ddty);
    res_AtFP_qp -> Fill(ddqp);

    //pulls of the parameters
    pull_AtFP_x  -> Fill(ddx/sqrt(fC[0]));
    pull_AtFP_y  -> Fill(ddy/sqrt(fC[2]));
    pull_AtFP_tx -> Fill(ddtx/sqrt(fC[5]));
    pull_AtFP_ty -> Fill(ddty/sqrt(fC[9]));
    pull_AtFP_qp -> Fill(ddqp/sqrt(fC[14]));
  }
*/
}
