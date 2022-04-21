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
#include "CbmKFParticlesFinderQA.h"
#include "CbmKFParticlesFinder.h"

#include "CbmKF.h"
/*#include "CbmKFParticle.h"*/
#include "CbmKFVertex.h"

#include "KFParticle.h"
#include "KFParticleSIMD.h"

#include "KFParticleMatch.h"

#include "CbmStsHit.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmStsCluster.h"
#include "CbmStsDigi.h"

#include "CbmVertex.h"
#include "CbmMCTrack.h"
#include "CbmMvdPoint.h"
#include "CbmMvdHitMatch.h"
#include "CbmStsPoint.h"

#include "TClonesArray.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TAxis.h"

#include "CbmL1PFFitter.h"
#include "L1Field.h"

#include "FairRunAna.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <cmath>
using namespace std;
using std::vector;

ClassImp(CbmKFParticlesFinderQA)

CbmKFParticlesFinderQA::CbmKFParticlesFinderQA(CbmKFParticlesFinder *pf, Int_t iVerbose, int findParticlesMode, int perf, const char *name, const char *title):
  FairTask(name,iVerbose),
  fFindParticlesMode(findParticlesMode),
  fPerformance(perf),
  fMinNStations(4),
  fMinRecoMom(0.1),
  fPF(pf),
  fMCTrackPoints(),
  vMCParticles(),
  MCtoRParticleId(),
  RtoMCParticleId(),
  vIsBkgWithSamePDG(),
  flistStsPts(0),
  flistMvdPts(0),
  flistMCTracks(0),
  flistStsTracksMatch(0),
  flistStsTracks(0),
  flistStsHits(0),
  flistMvdHits(0),
  flistMvdHitMatches(0),
  flistStsClusters(0),
  flistStsDigi(0),
  fPrimVtx(0),
  fRecParticles(0),
  fMCParticles(0),
  fMatchParticles(0),
  fSaveParticles(0),
  fSaveMCParticles(0),
  outfileName("CbmParticlesFinderQA.root"),
  fEfffileName("Efficiency.txt"),
  histodir(0),
  vStsHitMatch(),
  vStsPointMatch(),
  vMvdPointMatch(),
  vMCTrackMatch(),
  fParteff(),
  fNEvents(0),
  hFitDaughtersQA(),
  hFitQA(),
  hPartParam(),
  hPartParamBG(),
  hPartParamGhost(),
//  hPartParamCorrBG(),
  hPartParamSignal(),
  hPartParamQA(),
  hPartParam2D(),
  hPartParam2DBG(),
  hPartParam2DGhost(),
  hPartParam2DSignal(),
  hPVFitQa(),
  fMotherPdgToIndex(),
  hMotherPdg(),
  hTrackParameters()
{
  TString partNames[nHistoMotherPdg] = {"e+","e-",
                                             "#mu+","#mu-",
                                             "#pi+","#pi-",
                                             "K+","K-",
                                             "p+","p-",
                                             "K0s",
                                             "#Lambda","#Lambda_bar",
                                             "#Xi+","#Xi-",
                                             "#Omega+","#Omega-" };
  int partPdg[nHistoMotherPdg] = {  -11,   11,
                                    -13,   13,
                                    211, -211,
                                    321, -321,
                                   2212,-2212,
                                    310,
                                   3122,-3122,
                                   3312,-3312,
                                   3334,-3334 };
  for(int iMP=0; iMP<nHistoMotherPdg; iMP++)
    fMotherPdgToIndex[partPdg[iMP]] = iMP;

  TDirectory *currentDir = gDirectory;
  gDirectory->mkdir("KFParticlesFinder");
  gDirectory->cd("KFParticlesFinder");
  histodir = gDirectory;
  gDirectory->mkdir("Particles");
  gDirectory->cd("Particles");
  {
    for(int iPart=0; iPart<fParteff.nParticles; ++iPart)
    {
      gDirectory->mkdir(fParteff.partName[iPart].Data());
      gDirectory->cd(fParteff.partName[iPart].Data());
      {
        TString res = "res";
        TString pull = "pull";

        gDirectory->mkdir("DaughtersQA");
        gDirectory->cd("DaughtersQA");
        {
          TString parName[nFitQA/2] = {"X","Y","Z","Px","Py","Pz","E","M"};
          int nBins = 100;
          float xMax[nFitQA/2] = {0.15,0.15,0.03,0.01,0.01,0.06,0.06,0.01};

          for( int iH=0; iH<nFitQA/2; iH++ ){
            hFitDaughtersQA[iPart][iH]   = new TH1F((res+parName[iH]).Data(),
                                                    (res+parName[iH]).Data(), 
                                                    nBins, -xMax[iH],xMax[iH]);
            hFitDaughtersQA[iPart][iH+8] = new TH1F((pull+parName[iH]).Data(),
                                                    (pull+parName[iH]).Data(), 
                                                    nBins, -6,6);
          }
        }
        gDirectory->cd(".."); //particle directory

        gDirectory->mkdir("FitQA");
        gDirectory->cd("FitQA");
        {
          TString parName[nFitQA/2] = {"X","Y","Z","Px","Py","Pz","E","M"};
          int nBins = 50;
          float xMax[nFitQA/2] = {0.15,0.15,1.2,0.02,0.02,0.15,0.15,0.006};

          for( int iH=0; iH<nFitQA/2; iH++ ){
            hFitQA[iPart][iH]   = new TH1F((res+parName[iH]).Data(),
                                           (res+parName[iH]).Data(), 
                                           nBins, -xMax[iH],xMax[iH]);
            hFitQA[iPart][iH+8] = new TH1F((pull+parName[iH]).Data(),
                                           (pull+parName[iH]).Data(), 
                                           nBins, -6,6);
          }
        }
        gDirectory->cd(".."); //particle directory

        gDirectory->mkdir("Parameters");
        gDirectory->cd("Parameters");
        {
          TString parName[nHistoPartParam] = {"M","p","p_{t}","y","DL","c#tau","chi2ndf","chi2ndfTopo","prob","#theta","phi","z","l/dl"};
          TString parAxisName[nHistoPartParam] = {"m [GeV/c^{2}]","p [GeV/c]","p_{t} [GeV/c]",
                                                  "y","Decay length [cm]","Life time c#tau [cm]",
                                                  "chi2/ndf","chi2/ndf topo","prob","#theta [rad]",
                                                  "phi [rad]","z [cm]", "l/dl"};
          int nBins[nHistoPartParam] = {1000,100,100,100,100,100,100,100,100,100,100,100,100};
          float xMin[nHistoPartParam] = {fParteff.partMHistoMin[iPart],  0., 0., 0., -5.,  0.,  0.,  0., 0., -2., -2., -5., -1.};
          float xMax[nHistoPartParam] = {fParteff.partMHistoMax[iPart], 10., 3., 6., 55., 30., 20., 20., 1.,  2.,  2., 55., 35.};

          for(int iH=0; iH<nHistoPartParam; iH++)
          {
            hPartParam[iPart][iH]       = new TH1F(parName[iH].Data(),parName[iH].Data(),
                                            nBins[iH],xMin[iH],xMax[iH]);
            hPartParam[iPart][iH]->GetXaxis()->SetTitle(parAxisName[iH].Data());
          }

          hPartParam2D[iPart][0] = new TH2F("y-p_{t}","y-p_{t}",
                                            nBins[3],xMin[3],xMax[3],
                                            nBins[2],xMin[2],xMax[2]);
          hPartParam2D[iPart][0]->GetXaxis()->SetTitle("y");
          hPartParam2D[iPart][0]->GetYaxis()->SetTitle("p_{t} [GeV/c]");

          gDirectory->mkdir("Signal");
          gDirectory->cd("Signal");
          {
            for(int iH=0; iH<nHistoPartParam; iH++)
            {
              hPartParamSignal[iPart][iH] = new TH1F((parName[iH]).Data(),(parName[iH]).Data(),
                                            nBins[iH],xMin[iH],xMax[iH]);
              hPartParamSignal[iPart][iH]->GetXaxis()->SetTitle(parAxisName[iH].Data());
            }

            hPartParam2DSignal[iPart][0] = new TH2F("y-p_{t}","y-p_{t}",
                                                    nBins[3],xMin[3],xMax[3],
                                                    nBins[2],xMin[2],xMax[2]);
            hPartParam2DSignal[iPart][0]->GetXaxis()->SetTitle("y");
            hPartParam2DSignal[iPart][0]->GetYaxis()->SetTitle("p_{t} [GeV/c]");

            gDirectory->mkdir("QA");
            gDirectory->cd("QA");
            {
              int nBinsQA = 50;
              float xMaxQA[nHistoPartParamQA] = {0.01,0.001,0.001};
              for( int iH=0; iH<nHistoPartParamQA; iH++ ){
                hPartParamQA[iPart][iH] = 
                  new TH1F((res+parName[iH]).Data(), (res+parName[iH]).Data(), nBinsQA, -xMaxQA[iH],xMaxQA[iH]);
                hPartParamQA[iPart][iH+nHistoPartParamQA] = 
                  new TH1F((pull+parName[iH]).Data(), (pull+parName[iH]).Data(), nBinsQA, -6,6);
              }
            }
            gDirectory->cd(".."); // particle directory / Parameters / Signal
          }
          gDirectory->cd(".."); // particle directory / Parameters
          gDirectory->mkdir("Background");
          gDirectory->cd("Background");
          {
            for(int iH=0; iH<nHistoPartParam; iH++)
            {
              hPartParamBG[iPart][iH]     = new TH1F((parName[iH]).Data(),(parName[iH]).Data(),
                                            nBins[iH],xMin[iH],xMax[iH]);
              hPartParamBG[iPart][iH]->GetXaxis()->SetTitle(parAxisName[iH].Data());
            }

            hPartParam2DBG[iPart][0] = new TH2F("y-p_{t}","y-p_{t}",
                                                nBins[3],xMin[3],xMax[3],
                                                nBins[2],xMin[2],xMax[2]);
            hPartParam2DBG[iPart][0]->GetXaxis()->SetTitle("y");
            hPartParam2DBG[iPart][0]->GetYaxis()->SetTitle("p_{t} [GeV/c]");
          }
//           gDirectory->cd(".."); // particle directory
//           gDirectory->mkdir("CorrBG");
//           gDirectory->cd("CorrBG");
//           {
//             for(int iH=0; iH<nHistoPartParam; iH++)
//             {
//               hPartParamCorrBG[iPart][iH]     = new TH1F((parName[iH]).Data(),(parName[iH]).Data(),
//                                             nBins[iH],xMin[iH],xMax[iH]);
//               hPartParamCorrBG[iPart][iH]->GetXaxis()->SetTitle(parAxisName[iH].Data());
//             }
// 
//             hPartParam2DCorrBG[iPart][0] = new TH2F("y-p_{t}","y-p_{t}",
//                                                 nBins[3],xMin[3],xMax[3],
//                                                 nBins[2],xMin[2],xMax[2]);
//             hPartParam2DCorrBG[iPart][0]->GetXaxis()->SetTitle("y");
//             hPartParam2DCorrBG[iPart][0]->GetYaxis()->SetTitle("p_{t} [GeV/c]");
//           }
          gDirectory->cd(".."); // particle directory
          gDirectory->mkdir("Ghost");
          gDirectory->cd("Ghost");
          {
            for(int iH=0; iH<nHistoPartParam; iH++)
            {
              hPartParamGhost[iPart][iH]     = new TH1F((parName[iH]).Data(),(parName[iH]).Data(),
                                            nBins[iH],xMin[iH],xMax[iH]);
              hPartParamGhost[iPart][iH]->GetXaxis()->SetTitle(parAxisName[iH].Data());
            }

            hPartParam2DGhost[iPart][0] = new TH2F("y-p_{t}","y-p_{t}",
                                                nBins[3],xMin[3],xMax[3],
                                                nBins[2],xMin[2],xMax[2]);
            hPartParam2DGhost[iPart][0]->GetXaxis()->SetTitle("y");
            hPartParam2DGhost[iPart][0]->GetYaxis()->SetTitle("p_{t} [GeV/c]");
          }
          gDirectory->cd(".."); // particle directory
        }
        gDirectory->cd(".."); //particle directory
      }
      gDirectory->cd(".."); //Particles
    }
  }
  gDirectory->cd(".."); //L1
  gDirectory->mkdir("PrimaryVertexQA");
  gDirectory->cd("PrimaryVertexQA");
  {
    struct {
      string name;
      string title;
      Int_t n;
      Double_t l,r;
    } Table[nHistosPV]=
    {
      {"PVResX", "x_{rec}-x_{mc}, cm",     100, -0.006f, 0.006f},
      {"PVResY", "y_{rec}-y_{mc}, cm",     100, -0.006f, 0.006f},
      {"PVResZ", "z_{rec}-z_{mc}, cm",     100, -0.06f, 0.06f},
      {"PVPullX", "Pull X",     100, -6.f, 6.f},
      {"PVPullY", "Pull Y",     100, -6.f, 6.f},
      {"PVPullZ", "Pull Z",     100, -6.f, 6.f}
    };
    for(int iHPV=0; iHPV<nHistosPV; ++iHPV){
      hPVFitQa[iHPV] = new TH1F(Table[iHPV].name.data(),Table[iHPV].title.data(),
                                Table[iHPV].n, Table[iHPV].l, Table[iHPV].r);
    }
  }
  gDirectory->cd(".."); //L1
  gDirectory->mkdir("MotherPDG");
  gDirectory->cd("MotherPDG");
  {
    for(int iMP=0; iMP<nHistoMotherPdg; iMP++)
      hMotherPdg[iMP] = new TH1F(partNames[iMP].Data(), partNames[iMP].Data(), 10000, -5000, 5000);
  }
  gDirectory->cd(".."); //particle directory

  gDirectory->mkdir("TrackParameters");
  gDirectory->cd("TrackParameters");
  {
    TString chi2Name = "Chi2Prim";
    for(int iPart=0; iPart < CbmKFPartEfficiencies::nParticles; iPart++)
    {
      TString chi2NamePart = "Chi2Prim";
      chi2NamePart += "_";
      chi2NamePart += fParteff.partName[iPart].Data();
      hTrackParameters[iPart] = new TH1F(chi2NamePart.Data(), chi2NamePart.Data(), 1000, 0, 100);

    }
    hTrackParameters[CbmKFPartEfficiencies::nParticles] = new TH1F("Chi2Prim", "Chi2Prim", 1000, 0, 100);
  }
  gDirectory->cd(".."); //particle directory
    
  gDirectory = currentDir;
}

CbmKFParticlesFinderQA::~CbmKFParticlesFinderQA()
{
  if(fSaveParticles)
    fRecParticles->Delete();
  if(fSaveMCParticles)
  {
    fMCParticles->Delete();
    fMatchParticles->Delete();
  }
}

InitStatus CbmKFParticlesFinderQA::ReInit()
{
  return Init();
}

InitStatus CbmKFParticlesFinderQA::Init()
{
  FairRootManager *fManger = FairRootManager::Instance();

  flistMCTracks = dynamic_cast<TClonesArray*>( fManger->GetObject("MCTrack") );
  flistStsPts = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsPoint") );
  flistStsTracks = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsTrack") );
  flistStsTracksMatch = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsTrackMatch") );
  flistStsHits = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsHit") );
  flistStsClusters = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsCluster") );
  flistStsDigi = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsDigi") );
  fPrimVtx = (CbmVertex*) fManger->GetObject("PrimaryVertex");

  if( CbmKF::Instance()->GetNMvdStations() == 0 ) 
  {
    flistMvdPts = 0;
    flistMvdHits = 0;
    flistMvdHitMatches = 0;
  }
  else 
  {
    flistMvdPts = dynamic_cast<TClonesArray*>(  fManger->GetObject("MvdPoint") );
    flistMvdHits = dynamic_cast<TClonesArray*>(  fManger->GetObject("MvdHit") );
    flistMvdHitMatches = dynamic_cast<TClonesArray*>(  fManger->GetObject("MvdHitMatch") );
  }

  if(fSaveParticles)
  {
    // create and register TClonesArray with output reco particles
    fRecParticles = new TClonesArray("KFParticle",100);
    fManger->Register("RecoParticles", "KFParticle", fRecParticles, kTRUE);
  }

  if(fSaveMCParticles)
  {
    // create and register TClonesArray with output MC particles
    fMCParticles = new TClonesArray("KFMCParticle",100);
    fManger->Register("KFMCParticles", "KFParticle", fMCParticles, kTRUE);

    // create and register TClonesArray with matching between reco and MC particles
    fMatchParticles = new TClonesArray("KFParticleMatch",100);
    fManger->Register("KFParticleMatch", "KFParticle", fMatchParticles, kTRUE);
  }

  return kSUCCESS;
}

void CbmKFParticlesFinderQA::Exec(Option_t * option)
{
  if(fSaveParticles)
    fRecParticles->Delete();
  if(fSaveMCParticles)
  {
    fMCParticles->Delete();
    fMatchParticles->Delete();
  }

  vStsHitMatch.clear();
  vStsPointMatch.clear();
  vMvdPointMatch.clear();
  vMCTrackMatch.clear();
  vStsHitMatch.resize(flistStsHits->GetEntriesFast());
  vStsPointMatch.resize(flistStsPts->GetEntriesFast());
  if(flistMvdPts)
    vMvdPointMatch.resize(flistMvdPts->GetEntriesFast());
  vMCTrackMatch.resize(flistMCTracks->GetEntriesFast());
  for(unsigned int iP=0; iP<vStsPointMatch.size(); iP++)
    vStsPointMatch[iP] = -1;
  for(unsigned int iP=0; iP<vMvdPointMatch.size(); iP++)
    vMvdPointMatch[iP] = -1;
  for(unsigned int iTr=0; iTr<vMCTrackMatch.size(); iTr++)
    vMCTrackMatch[iTr] = -1;

  if(flistMvdHits)
  {
    for(int iH=0; iH<flistMvdHits->GetEntriesFast(); iH++)
    {
      CbmMvdHitMatch *match = static_cast<CbmMvdHitMatch*>( flistMvdHitMatches->At(iH) );
      if( match){
        int iMC = match->GetPointId();
        if(iMC < 0) continue;
        vMvdPointMatch[iMC] = iH;
      }
    }
  }
  
  StsHitMatch();

  for(int iTr=0; iTr<flistStsTracksMatch->GetEntriesFast(); iTr++)
  {
    CbmTrackMatch* StsTrackMatch = (CbmTrackMatch*)flistStsTracksMatch->At(iTr);
    if(StsTrackMatch -> GetNofMCTracks() == 0) continue;
    const int mcTrackId = StsTrackMatch->GetMCTrackId();
    vMCTrackMatch[mcTrackId] = iTr;
  }
  
  fMCTrackPoints.clear();
  fMCTrackPoints.resize(flistMCTracks->GetEntriesFast());
  if(CbmKF::Instance()->GetNMvdStations() > 0)
    for (Int_t iMvd=0; iMvd<flistMvdPts->GetEntriesFast(); iMvd++)
    {
      CbmMvdPoint* MvdPoint = (CbmMvdPoint*)flistMvdPts->At(iMvd);
      if(!MvdPoint) continue;
      fMCTrackPoints[MvdPoint->GetTrackID()].MvdArray.push_back(MvdPoint);
      int iHit = vMvdPointMatch[iMvd];
      if(iHit<0) continue;
      CbmMvdHit* MvdHit = (CbmMvdHit*)flistStsHits->At(iHit);
      if(!MvdHit) continue;
      fMCTrackPoints[MvdPoint->GetTrackID()].MvdHitsArray.push_back(MvdHit);
    }
  for (Int_t iSts=0; iSts<flistStsPts->GetEntriesFast(); iSts++)
  {
    CbmStsPoint* StsPoint = (CbmStsPoint*)flistStsPts->At(iSts);
    fMCTrackPoints[StsPoint->GetTrackID()].StsArray.push_back(StsPoint);
    int iHit = vStsPointMatch[iSts];
    if(iHit<0) continue;
    CbmStsHit* StsHit = (CbmStsHit*)flistStsHits->At(iHit);
    fMCTrackPoints[StsPoint->GetTrackID()].StsHitsArray.push_back(StsHit);
  }

  GetMCParticles();
  FindReconstructableMCParticles();
  MatchParticles();
  PartEffPerformance();
  PartHistoPerformance();

  // save particles to file
  if(fSaveParticles)
  {
    for(unsigned int iP=0; iP < fPF->GetParticles().size(); iP++)
    {
      new((*fRecParticles)[iP]) KFParticle(fPF->GetParticles()[iP]);
    }
  }

  if(fSaveMCParticles)
  {
    for(unsigned int iP=0; iP < fPF->GetParticles().size(); iP++)
    {
      new((*fMatchParticles)[iP]) KFParticleMatch();
      KFParticleMatch *p = (KFParticleMatch*)( fMatchParticles->At(iP) );

      Short_t matchType = 0;
      int iMCPart = -1;
      if(!RtoMCParticleId[iP].IsMatchedWithPdg()) //background
      {
        if(RtoMCParticleId[iP].IsMatched())
        {
          iMCPart = RtoMCParticleId[iP].GetBestMatchWithPdg();
          matchType = 1;
        }
      }
      else
      {
        iMCPart = RtoMCParticleId[iP].GetBestMatchWithPdg();
        matchType = 2;
      }

      p->SetMatch(iMCPart);
      p->SetMatchType(matchType);
    }

    for(unsigned int iP=0; iP < vMCParticles.size(); iP++)
    {
      new((*fMCParticles)[iP]) KFMCParticle(vMCParticles[iP]);
//       KFMCParticle *p = (KFMCParticle*)( fMCParticles->At(iP) );
//       *p = vMCParticles[iP];
    }
  }
}

void CbmKFParticlesFinderQA::Finish()
{
  if(!(outfileName == ""))
  {
    TDirectory *curr = gDirectory;
    TFile *currentFile = gFile;
    // Open output file and write histograms
    TFile* outfile;

    outfile = new TFile(outfileName.Data(),"RECREATE");
    outfile->cd();
    WriteHistos(histodir);
    outfile->Close();
    outfile->Delete();
    gFile = currentFile;
    gDirectory = curr;
  }
  else
  {
    WriteHistosCurFile(histodir);
  }
  std::fstream eff(fEfffileName.Data(),fstream::out);
  eff << fParteff;
  eff.close();
}

void CbmKFParticlesFinderQA::WriteHistos( TObject *obj ){
  if( !obj->IsFolder() ) obj->Write();
  else{
    TDirectory *cur = gDirectory;
    TDirectory *sub = cur->mkdir(obj->GetName());
    sub->cd();
    TList *listSub = (static_cast<TDirectory*>(obj))->GetList();
    TIter it(listSub);
    while( TObject *obj1=it() ) WriteHistos(obj1);
    cur->cd();
  }
}

void CbmKFParticlesFinderQA::WriteHistosCurFile( TObject *obj ){
  if( !obj->IsFolder() ) obj->Write();
  else{
    TDirectory *cur = gDirectory;
    TDirectory *sub = cur->GetDirectory(obj->GetName());
    sub->cd();
    TList *listSub = (static_cast<TDirectory*>(obj))->GetList();
    TIter it(listSub);
    while( TObject *obj1=it() ) WriteHistosCurFile(obj1);
    cur->cd();
  }
}

void CbmKFParticlesFinderQA::StsHitMatch()
{
  const bool useLinks = 1; // 0 - use HitMatch, one_to_one; 1 - use FairLinks, many_to_many. Set 0 to switch to old definition of efficiency.
  // TODO: fix trunk problem with links. Set useLinks = 1
  
  for (int iH = 0; iH < flistStsHits->GetEntriesFast(); iH++){
    CbmStsHit *stsHit = dynamic_cast<CbmStsHit*>(flistStsHits->At(iH));
    vStsHitMatch[iH] = -1;
    if (useLinks){
      if (flistStsClusters){
  //      if ( NLinks != 2 ) cout << "HitMatch: Error. Hit wasn't matched with 2 clusters." << endl;
          // 1-st cluster
        vector<int> stsPointIds; // save here all mc-points matched with first cluster
        vector<int> stsPointIds_hit;
        int iL = 0;
        FairLink link = stsHit->GetLink(iL);
        CbmStsCluster *stsCluster = dynamic_cast<CbmStsCluster*>( flistStsClusters->At( link.GetIndex() ) );
        int NLinks2 = stsCluster->GetNLinks();
        for ( int iL2 = 0; iL2 < NLinks2; iL2++){
          FairLink link2 = stsCluster->GetLink(iL2);
          CbmStsDigi *stsDigi = dynamic_cast<CbmStsDigi*>( flistStsDigi->At( link2.GetIndex() ) );
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
        stsCluster = dynamic_cast<CbmStsCluster*>( flistStsClusters->At( link.GetIndex() ) );
        NLinks2 = stsCluster->GetNLinks();
        for ( int iL2 = 0; iL2 < NLinks2; iL2++){
          FairLink link2 = stsCluster->GetLink(iL2);
          CbmStsDigi *stsDigi = dynamic_cast<CbmStsDigi*>( flistStsDigi->At( link2.GetIndex() ) );
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
            vStsPointMatch[stsPointId] = iH;
          } // for mcPoint
        } // for digi
      } // if clusters
    } // if useLinks
  } // for hits
}

void CbmKFParticlesFinderQA::GetMCParticles()
{
  vMCParticles.clear();
    // convert
  for(int i=0; i < flistMCTracks->GetEntriesFast(); i++)
  {
    CbmMCTrack &mtra = *(static_cast<CbmMCTrack*>(flistMCTracks->At(i)));
    KFMCParticle part;
    part.SetMCTrackID( i );
    part.SetMotherId ( mtra.GetMotherId() );
    part.SetPDG      ( mtra.GetPdgCode() );
    vMCParticles.push_back( part );
  }
    // find relations
  const unsigned int nMCParticles = vMCParticles.size();
  for ( unsigned int iP = 0; iP < nMCParticles; iP++ ) {
    KFMCParticle &part = vMCParticles[iP];
    for(unsigned int iP2 = 0; iP2 < nMCParticles; iP2++) {
      KFMCParticle &part2 = vMCParticles[iP2];

      if(part.GetMotherId() == part2.GetMCTrackID()) {
        part2.AddDaughter(iP);
      }
    }
  }
}

void CbmKFParticlesFinderQA::FindReconstructableMCParticles()
{
  const unsigned int nMCParticles = vMCParticles.size();

  for ( unsigned int iP = 0; iP < nMCParticles; iP++ ) {
    KFMCParticle &part = vMCParticles[iP];
    CheckMCParticleIsReconstructable(part);
  }
}

void CbmKFParticlesFinderQA::CheckMCParticleIsReconstructable(KFMCParticle &part)
{
  if ( part.IsReconstructable() ) return;

    // tracks
  if ( /*part.NDaughters() == 0*/ part.GetPDG() ==  -211 ||
                                  part.GetPDG() ==   211 ||
                                  part.GetPDG() ==  2212 ||
                                  part.GetPDG() == -2212 ||
                                  part.GetPDG() ==   321 ||
                                  part.GetPDG() ==  -321 ||
                                  part.GetPDG() ==    11 ||
                                  part.GetPDG() ==   -11 ||
                                  part.GetPDG() ==    13 ||
                                  part.GetPDG() ==   -13 ) { // TODO other particles

    switch ( fFindParticlesMode ) {
      case 1:
        part.SetAsReconstructable();
        break;
      case 2:
        {
          int iMCPart = part.GetMCTrackID();
          CbmMCTrack* mcTr = (static_cast<CbmMCTrack*>(flistMCTracks->At(iMCPart)));
          if(fMCTrackPoints[iMCPart].IsReconstructable(mcTr, fMinNStations, fPerformance, fMinRecoMom))
            part.SetAsReconstructable();
        }
        break;
      case 3:
        {
          int iMCPart = part.GetMCTrackID();
          if ( vMCTrackMatch[iMCPart]>=0 )
            part.SetAsReconstructable();
        }
        break;
      default:
        part.SetAsReconstructable();
    };
  }
    //  mother particles
  else
  {
    for(int iPart=0; iPart<fParteff.nParticles; iPart++)
    {
      if(part.GetPDG() == fParteff.partPDG[iPart])
      {
        const unsigned int nDaughters = fParteff.partDaughterPdg[iPart].size();
        if( part.GetDaughterIds().size() != nDaughters ) return;
        int pdg[nDaughters];

        for(unsigned int iD=0; iD<nDaughters; iD++)
          pdg[iD] = vMCParticles[part.GetDaughterIds()[iD]].GetPDG();

        bool isDaughterFound[nDaughters];
        for(unsigned int iDMC=0; iDMC<nDaughters; iDMC++)
          isDaughterFound[iDMC] = 0;

        bool isReco = 1;
        for(unsigned int iDMC=0; iDMC<nDaughters; iDMC++)
          for(unsigned int iD=0; iD<nDaughters; iD++)
            if(pdg[iD] == fParteff.partDaughterPdg[iPart][iDMC]) isDaughterFound[iDMC] = 1;

        for(unsigned int iDMC=0; iDMC<nDaughters; iDMC++)
          isReco = isReco && isDaughterFound[iDMC];

        if(!isReco) return;
      }
    }


    const vector<int>& dIds = part.GetDaughterIds();
    const unsigned int nD = dIds.size();
    bool reco = 1;
    for ( unsigned int iD = 0; iD < nD && reco; iD++ ) {
      KFMCParticle &dp = vMCParticles[dIds[iD]];
      CheckMCParticleIsReconstructable(dp);
      reco &= dp.IsReconstructable();
    }
    if (reco) part.SetAsReconstructable();
  }
}

  /// Procedure for match Reconstructed and MC Particles. Should be called before Performances
void CbmKFParticlesFinderQA::MatchParticles()
{
    // get all reco particles ( temp )
  MCtoRParticleId.clear();
  RtoMCParticleId.clear();
  MCtoRParticleId.resize(vMCParticles.size());
  RtoMCParticleId.resize(fPF->GetParticles().size() );
  vIsBkgWithSamePDG.clear();
  vIsBkgWithSamePDG.resize(fPF->GetParticles().size() );
  
    // match tracks ( particles which are direct copy of tracks )
  for( unsigned int iRP = 0; iRP < fPF->GetParticles().size(); iRP++ ) {
//    CbmKFParticle &rPart = fPF->GetParticles()[iRP];
    KFParticle &rPart = fPF->GetParticles()[iRP];

    if (rPart.NDaughters() != 1) continue;
    
    const int rTrackId = rPart.DaughterIds()[0];
    CbmTrackMatch* StsTrackMatch = (CbmTrackMatch*)flistStsTracksMatch->At(rTrackId);
    if(StsTrackMatch -> GetNofMCTracks() == 0) continue;
    const int mcTrackId = StsTrackMatch->GetMCTrackId();

    for ( unsigned int iMP = 0; iMP < vMCParticles.size(); iMP++ ) {
      KFMCParticle &mPart = vMCParticles[iMP];
      if ( mPart.GetMCTrackID() == mcTrackId ) { // match is found
        if( mPart.GetPDG() == rPart.GetPDG() ) {
          MCtoRParticleId[iMP].ids.push_back(iRP);
          RtoMCParticleId[iRP].ids.push_back(iMP);
        }
        else {
          MCtoRParticleId[iMP].idsMI.push_back(iRP);
          RtoMCParticleId[iRP].idsMI.push_back(iMP);
        }
      }
    }
  }

    // match created mother particles
  for( unsigned int iRP = 0; iRP < fPF->GetParticles().size(); iRP++ ) {
//    CbmKFParticle &rPart = fPF->GetParticles()[iRP];
    KFParticle &rPart = fPF->GetParticles()[iRP];
    const unsigned int NRDaughters = rPart.NDaughters();
    if (NRDaughters < 2) continue;
    
    unsigned int iD = 0;
    int mmId = -2; // mother MC id
    {
      const int rdId = rPart.DaughterIds()[iD];
      if ( !RtoMCParticleId[rdId].IsMatched() ) continue;
      const int mdId = RtoMCParticleId[rdId].GetBestMatch();
      mmId = vMCParticles[mdId].GetMotherId();
    }
    iD++;
    for ( ; iD < NRDaughters; iD++ ) {
      const int rdId = rPart.DaughterIds()[iD];
      if ( !RtoMCParticleId[rdId].IsMatched() ) break;
      const int mdId = RtoMCParticleId[rdId].GetBestMatch();
      if( vMCParticles[mdId].GetMotherId() != mmId ) break;
    }
    if ( iD == NRDaughters && mmId != -1 ) { // match is found and it is not primary vertex
      KFMCParticle &mmPart = vMCParticles[mmId];
      if( mmPart.GetPDG()     == rPart.GetPDG()     &&
          mmPart.NDaughters() == rPart.NDaughters() ) {
        MCtoRParticleId[mmId].ids.push_back(iRP);
        RtoMCParticleId[iRP].ids.push_back(mmId);
      }
      else {
        MCtoRParticleId[mmId].idsMI.push_back(iRP);
        RtoMCParticleId[iRP].idsMI.push_back(mmId);
      }
    }
  }

}

void CbmKFParticlesFinderQA::PartEffPerformance()
{
  CbmKFPartEfficiencies partEff; // efficiencies for current event

  const int NRP = fPF->GetParticles().size();
  for ( int iP = 0; iP < NRP; ++iP ) {
//    const CbmKFParticle &part = fPF->GetParticles()[iP];
    const KFParticle &part = fPF->GetParticles()[iP];
    const int pdg = part.GetPDG();
      
    const bool isBG = RtoMCParticleId[iP].idsMI.size() != 0;
    const bool isGhost = !RtoMCParticleId[iP].IsMatched();

    for(int iPart=0; iPart<fParteff.nParticles; iPart++)
      if ( pdg == fParteff.partPDG[iPart] )
        partEff.IncReco(isGhost, isBG, fParteff.partName[iPart].Data());
  }

    
  const int NMP = vMCParticles.size();
  for ( int iP = 0; iP < NMP; ++iP ) {
    const KFMCParticle &part = vMCParticles[iP];
    if ( !part.IsReconstructable() ) continue;
    const int pdg = part.GetPDG();
    const int mId = part.GetMotherId();

    const bool isReco = MCtoRParticleId[iP].ids.size() != 0;
    const int nClones = MCtoRParticleId[iP].ids.size() - 1;

    for(int iPart=0; iPart<fParteff.nParticles; iPart++)
    {
      if ( pdg == fParteff.partPDG[iPart] ) {
        partEff.Inc(isReco, nClones, fParteff.partName[iPart].Data());
        if ( mId == -1 )
          partEff.Inc(isReco, nClones, (fParteff.partName[iPart]+"_prim").Data());
        else
          partEff.Inc(isReco, nClones, (fParteff.partName[iPart]+"_sec").Data());
      }
    }
  }

  fNEvents++;

  fParteff += partEff;

  partEff.CalcEff();
  fParteff.CalcEff();

    //   cout.precision(3);
  if( fVerbose ){
    if(fNEvents%100 == 0)
    {
      cout << " ---- KF Particle finder --- " << endl;
      // cout << "L1 STAT    : " << fNEvents << " EVENT "               << endl << endl;
      //partEff.PrintEff();
      // cout << endl;
      cout << "ACCUMULATED STAT    : " << fNEvents << " EVENTS "               << endl << endl;
      fParteff.PrintEff();

      cout<<endl;
      // cout<<"CA Track Finder: " << L1_CATIME/L1_fNEvents << " s/ev" << endl << endl;
    }
  }
}

void CbmKFParticlesFinderQA::PartHistoPerformance()
{
  CbmKFVertex vtx;
  if(fPrimVtx)
    vtx = CbmKFVertex(*fPrimVtx);

  for(unsigned int iP=0; iP < fPF->GetParticles().size(); iP++)
  {
    int iParticle = fParteff.GetParticleIndex(fPF->GetParticles()[iP].GetPDG());
    if(iParticle < 0) continue;

    Double_t M, ErrM;
    Double_t dL, ErrdL; // decay length
    Double_t cT, ErrcT; // c*tau
    Double_t P, ErrP;
    Double_t Pt;
    Double_t Rapidity;
    Double_t Theta;
    Double_t Phi;
    Double_t Z;
//    CbmKFParticle TempPart = fPF->GetParticles()[iP];
    KFParticle TempPart = fPF->GetParticles()[iP];
    TempPart.GetMass(M,ErrM);

    TempPart.GetMomentum(P,ErrP);
    Pt = TempPart.GetPt();
    Rapidity = TempPart.GetRapidity();
    TempPart.GetDecayLength(dL,ErrdL);
    TempPart.GetLifeTime(cT,ErrcT);
    Double_t chi2 = TempPart.GetChi2();
    Int_t ndf = TempPart.GetNDF();
    Double_t prob = TMath::Prob(chi2,ndf);
    Theta = TempPart.GetTheta();
    Phi = TempPart.GetPhi();
    Z = TempPart.GetZ();

    KFParticleSIMD tempSIMDPart(TempPart);
    fvec l,dl;
    KFParticleSIMD pv(vtx);
    tempSIMDPart.GetDistanceToVertexLine(pv, l, dl);
    dL = sqrt(TempPart.X()*TempPart.X() + TempPart.Y()*TempPart.Y() + TempPart.Z()*TempPart.Z() );

    
    tempSIMDPart.SetProductionVertex(pv);
    fvec chi2topo = tempSIMDPart.Chi2()/tempSIMDPart.GetNDF();
    
    hPartParam[iParticle][ 0]->Fill(M);
    hPartParam[iParticle][ 1]->Fill(P);
    hPartParam[iParticle][ 2]->Fill(Pt);
    hPartParam[iParticle][ 3]->Fill(Rapidity);
    hPartParam[iParticle][ 4]->Fill(dL);
    hPartParam[iParticle][ 5]->Fill(cT);
    hPartParam[iParticle][ 6]->Fill(chi2/ndf);
    hPartParam[iParticle][ 7]->Fill(chi2topo[0]);
    hPartParam[iParticle][ 8]->Fill(prob);
    hPartParam[iParticle][ 9]->Fill(Theta);
    hPartParam[iParticle][10]->Fill(Phi);
    hPartParam[iParticle][11]->Fill(Z);
    hPartParam[iParticle][12]->Fill(l[0]/dl[0]);


    hPartParam2D[iParticle][0]->Fill(Rapidity,Pt,1);

    if(!RtoMCParticleId[iP].IsMatchedWithPdg()) //background
    {
      if(!RtoMCParticleId[iP].IsMatched())
      {
        hPartParamGhost[iParticle][ 0]->Fill(M);
        hPartParamGhost[iParticle][ 1]->Fill(P);
        hPartParamGhost[iParticle][ 2]->Fill(Pt);
        hPartParamGhost[iParticle][ 3]->Fill(Rapidity);
        hPartParamGhost[iParticle][ 4]->Fill(dL);
        hPartParamGhost[iParticle][ 5]->Fill(cT);
        hPartParamGhost[iParticle][ 6]->Fill(chi2/ndf);
        hPartParamGhost[iParticle][ 7]->Fill(chi2topo[0]);
        hPartParamGhost[iParticle][ 8]->Fill(prob);
        hPartParamGhost[iParticle][ 9]->Fill(Theta);
        hPartParamGhost[iParticle][10]->Fill(Phi);
        hPartParamGhost[iParticle][11]->Fill(Z);
        hPartParamGhost[iParticle][12]->Fill(l[0]/dl[0]);

        hPartParam2DGhost[iParticle][0]->Fill(Rapidity,Pt,1);

//         if(vIsBkgWithSamePDG[iP])
//         {
//           hPartParamCorrBG[iParticle][ 0]->Fill(M);
//           hPartParamCorrBG[iParticle][ 1]->Fill(P);
//           hPartParamCorrBG[iParticle][ 2]->Fill(Pt);
//           hPartParamCorrBG[iParticle][ 3]->Fill(Rapidity);
//           hPartParamCorrBG[iParticle][ 4]->Fill(dL);
//           hPartParamCorrBG[iParticle][ 5]->Fill(cT);
//           hPartParamCorrBG[iParticle][ 6]->Fill(chi2/ndf);
//           hPartParamCorrBG[iParticle][ 7]->Fill(prob);
//           hPartParamCorrBG[iParticle][ 8]->Fill(Theta);
//           hPartParamCorrBG[iParticle][ 9]->Fill(Phi);
//           hPartParamCorrBG[iParticle][10]->Fill(Z);
// 
//           hPartParam2DCorrBG[iParticle][0]->Fill(Rapidity,Pt,1);
//         }
      }
      else
      {
        hPartParamBG[iParticle][ 0]->Fill(M);
        hPartParamBG[iParticle][ 1]->Fill(P);
        hPartParamBG[iParticle][ 2]->Fill(Pt);
        hPartParamBG[iParticle][ 3]->Fill(Rapidity);
        hPartParamBG[iParticle][ 4]->Fill(dL);
        hPartParamBG[iParticle][ 5]->Fill(cT);
        hPartParamBG[iParticle][ 6]->Fill(chi2/ndf);
        hPartParamBG[iParticle][ 7]->Fill(chi2topo[0]);
        hPartParamBG[iParticle][ 8]->Fill(prob);
        hPartParamBG[iParticle][ 9]->Fill(Theta);
        hPartParamBG[iParticle][10]->Fill(Phi);
        hPartParamBG[iParticle][11]->Fill(Z);
        hPartParamBG[iParticle][12]->Fill(l[0]/dl[0]);

        hPartParam2DBG[iParticle][0]->Fill(Rapidity,Pt,1);
      }
      continue;
    }
    hPartParamSignal[iParticle][ 0]->Fill(M);
    hPartParamSignal[iParticle][ 1]->Fill(P);
    hPartParamSignal[iParticle][ 2]->Fill(Pt);
    hPartParamSignal[iParticle][ 3]->Fill(Rapidity);
    hPartParamSignal[iParticle][ 4]->Fill(dL);
    hPartParamSignal[iParticle][ 5]->Fill(cT);
    hPartParamSignal[iParticle][ 6]->Fill(chi2/ndf);
    hPartParamSignal[iParticle][ 7]->Fill(chi2topo[0]);
    hPartParamSignal[iParticle][ 8]->Fill(prob);
    hPartParamSignal[iParticle][ 9]->Fill(Theta);
    hPartParamSignal[iParticle][10]->Fill(Phi);
    hPartParamSignal[iParticle][11]->Fill(Z);
    hPartParamSignal[iParticle][12]->Fill(l[0]/dl[0]);

    hPartParam2DSignal[iParticle][0]->Fill(Rapidity,Pt,1);

    int iMCPart = RtoMCParticleId[iP].GetBestMatchWithPdg();
    KFMCParticle &mcPart = vMCParticles[iMCPart];
    // Fit quality of the mother particle
    {
      int iMCTrack = mcPart.GetMCTrackID();
      CbmMCTrack &mcTrack = *(static_cast<CbmMCTrack*>(flistMCTracks->At(iMCTrack)));
      int mcDaughterId = mcPart.GetDaughterIds()[0];
      CbmMCTrack &mcDaughter = *(static_cast<CbmMCTrack*>(flistMCTracks->At(mcDaughterId)));

      Double_t decayVtx[3] = { mcDaughter.GetStartX(), mcDaughter.GetStartY(), mcDaughter.GetStartZ() };
      Double_t recParam[8] = { 0 };
      Double_t errParam[8] = { 0 };

      for(int iPar=0; iPar<3; iPar++)
      {
        recParam[iPar] = TempPart.GetParameter(iPar);
        Double_t error = TempPart.GetCovariance(iPar,iPar);
        if(error < 0.) { error = 1.e20;}
        errParam[iPar] = TMath::Sqrt(error);
      }
      TempPart.TransportToPoint(decayVtx);
      for(int iPar=3; iPar<7; iPar++)
      {
        recParam[iPar] = TempPart.GetParameter(iPar);
        Double_t error = TempPart.GetCovariance(iPar,iPar);
        if(error < 0.) { error = 1.e20;}
        errParam[iPar] = TMath::Sqrt(error);
      }

      Double_t Emc = sqrt(mcTrack.GetP()*mcTrack.GetP() + mcTrack.GetMass()*mcTrack.GetMass());
      Double_t res[8] = {0}, 
               pull[8] = {0}, 
               mcParam[8] = { decayVtx[0], decayVtx[1], decayVtx[2],
                              mcTrack.GetPx(), mcTrack.GetPy(), mcTrack.GetPz(), Emc, mcTrack.GetMass() };
      for(int iPar=0; iPar < 7; iPar++ )
      {
        res[iPar]  = recParam[iPar] - mcParam[iPar];
        if(fabs(errParam[iPar]) > 1.e-20) pull[iPar] = res[iPar]/errParam[iPar];
      }
      res[7] = M - mcParam[7];
      if(fabs(ErrM) > 1.e-20) pull[7] = res[7]/ErrM;

      for(int iPar=0; iPar < 8; iPar++ )
      {
        hFitQA[iParticle][iPar]->Fill(res[iPar]);
        hFitQA[iParticle][iPar+8]->Fill(pull[iPar]);
      }

//      Double_t mcT = mcDaughter.GetStartT() - mcTrack.GetStartT();
      
    }
    // Fit quality of daughters
    for(int iD=0; iD<mcPart.NDaughters(); ++iD)
    {
      int mcDaughterId = mcPart.GetDaughterIds()[iD];
//      if(!MCtoRParticleId[mcDaughterId].IsMatchedWithPdg()) continue;
      if(!MCtoRParticleId[mcDaughterId].IsMatched()) continue;
      CbmMCTrack &mcTrack = *(static_cast<CbmMCTrack*>(flistMCTracks->At(mcDaughterId)));
//      int recDaughterId = MCtoRParticleId[mcDaughterId].GetBestMatchWithPdg();
      int recDaughterId = MCtoRParticleId[mcDaughterId].GetBestMatch();
//      CbmKFParticle Daughter = fPF->GetParticles()[recDaughterId];
      KFParticle Daughter = fPF->GetParticles()[recDaughterId];
      Daughter.GetMass(M,ErrM);

      Double_t decayVtx[3] = {mcTrack.GetStartX(), mcTrack.GetStartY(), mcTrack.GetStartZ()};
      Daughter.TransportToPoint(decayVtx);

      Double_t Emc = sqrt(mcTrack.GetP()*mcTrack.GetP() + mcTrack.GetMass()*mcTrack.GetMass());
      Double_t res[8] = {0}, 
               pull[8] = {0}, 
               mcParam[8] = { mcTrack.GetStartX(), mcTrack.GetStartY(), mcTrack.GetStartZ(),
                            mcTrack.GetPx(), mcTrack.GetPy(), mcTrack.GetPz(), Emc, mcTrack.GetMass() };
      for(int iPar=0; iPar < 7; iPar++ )
      {
        Double_t error = Daughter.GetCovariance(iPar,iPar);
        if(error < 0.) { error = 1.e20;}
        error = TMath::Sqrt(error);
        res[iPar]  = Daughter.GetParameter(iPar) - mcParam[iPar];
        if(fabs(error) > 1.e-20) pull[iPar] = res[iPar]/error;
      }
      res[7] = M - mcParam[7];
      if(fabs(ErrM) > 1.e-20) pull[7] = res[7]/ErrM;

      for(int iPar=0; iPar < 8; iPar++ )
      {
        hFitDaughtersQA[iParticle][iPar]->Fill(res[iPar]);
        hFitDaughtersQA[iParticle][iPar+8]->Fill(pull[iPar]);
      }
    }
  }
  //Find MC parameters of the primary vertex 
  float mcPVx[3]={0.f};
  for(int iMC=0; iMC<flistMCTracks->GetEntriesFast(); ++iMC)
  {
    CbmMCTrack* mcTrack = (CbmMCTrack*)flistMCTracks->At(iMC);
    if(mcTrack->GetMotherId() < 0)
    {
      mcPVx[0]=mcTrack->GetStartX();
      mcPVx[1]=mcTrack->GetStartY();
      mcPVx[2]=mcTrack->GetStartZ();
      break;
    }
  }

  float dRPVr[3] = {vtx.GetRefX()-mcPVx[0],
                    vtx.GetRefY()-mcPVx[1],
                    vtx.GetRefZ()-mcPVx[2]};
  float dRPVp[3] = {dRPVr[0]/sqrt(vtx.GetCovMatrix()[0]),
                    dRPVr[1]/sqrt(vtx.GetCovMatrix()[2]),
                    dRPVr[2]/sqrt(vtx.GetCovMatrix()[5])};
  for(unsigned int iHPV=0; iHPV<3; ++iHPV)
    hPVFitQa[iHPV]->Fill(dRPVr[iHPV]);
  for(unsigned int iHPV=3; iHPV<6; ++iHPV)
    hPVFitQa[iHPV]->Fill(dRPVp[iHPV-3]);

  //fill mother pdg histo
  for(int i=0; i < flistMCTracks->GetEntriesFast(); i++)
  {
    CbmMCTrack &mtra = *(static_cast<CbmMCTrack*>(flistMCTracks->At(i)));
    int motherId = mtra.GetMotherId();
    int pdg = mtra.GetPdgCode();
    int histoIndex = GetMotherPdgIndex(pdg);
    if(histoIndex != -1)
    {
      if(motherId > -1)
      {
        CbmMCTrack &mother = *(static_cast<CbmMCTrack*>(flistMCTracks->At(mtra.GetMotherId())));
        int motherPdg = mother.GetPdgCode();
        hMotherPdg[histoIndex]->Fill(motherPdg);
      }
      else
        hMotherPdg[histoIndex]->Fill(-1);
    }
  }
  
  //fill histo with chi2_prim
  for(unsigned int iP=0; iP<fPF->GetParticles().size(); iP++)
  {
    const KFParticle &rPart = fPF->GetParticles()[iP];
    const unsigned int NRDaughters = rPart.NDaughters();
    if (NRDaughters > 1) break;
    if( RtoMCParticleId[iP].GetBestMatch()<0 ) continue;
    KFMCParticle &mPart = vMCParticles[ RtoMCParticleId[iP].GetBestMatch() ];

    if(mPart.GetMotherId() < 0)
    {
      hTrackParameters[CbmKFPartEfficiencies::nParticles]->Fill(fPF->GetChiPrim()[iP] );
      continue;
    }

    KFMCParticle &mMotherPart = vMCParticles[mPart.GetMotherId()];

    int iParticle = fParteff.GetParticleIndex(mMotherPart.GetPDG());
    float chiPrim = fPF->GetChiPrim()[iP];
    if(iParticle > -1 && iParticle<CbmKFPartEfficiencies::nParticles)
      hTrackParameters[iParticle]->Fill(chiPrim );
  }
} // void CbmKFParticlesFinderQA::ParticlesEfficienciesPerformance()
