#if !defined(__CINT__) || defined(__MAKECINT__)

// CBM includes
#include "CbmKF.h"
#include "CbmKFSecondaryVertexFinder.h"
#include "CbmKFParticle.h"
#include "CbmKFTrack.h"
#include "CbmMCTrack.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmStsHit.h"
#include "CbmStsDigi.h"
#include "CbmStsPoint.h"
#include "CbmStsStation.h"
#include "CbmVertex.h"
#include "CbmStsCluster.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTrackParam.h"
#include "BmnFileSource.h"
#include "BmnFieldConst.h"
#include "UniRun.h"
#include "BmnFieldMap.h"
#include "BmnNewFieldMap.h"
#include "CbmStsDigiPar.h"
#include "TKey.h"

#include "CbmStsSector.h"
#include "CbmStsSensor.h"

// ROOT includes
#include <TBranch.h>
#include <TClonesArray.h>
#include <TDatabasePDG.h>
//#include <TDecompLU.h>
//#include <TDecompSVD.h>
#include <TFile.h>
#include <TGeoManager.h>
#include <TGeoCone.h>
#include <TGeoVolume.h>
#include <TH1.h>
#include <TH2.h>
#include <TMatrixD.h>
#include <TRandom.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TTree.h>
#include <TVector3.h>
#include <TVectorD.h>

#include <Riostream.h>
#include <map>
#include <set>
#include <vector>
#include "TCanvas.h"
#include "BmnEventHeader.h"

#endif

//#include "Chain.C"
Bool_t GetBmnGeom(const char *fileName);
void ApplyAlignment();
Int_t GetNofModules(TGeoNode *station);

const Int_t ip_r7[6][2]={0,1, 2,3, 4,5, 6,7, 8,9, 10,11};

// ----------------------------------------------------------------------------

void ResidOk_r7(TString inFile = "", TString histFile = "", Double_t Mscale = 1.366666667)
{

   Mscale =1250/900.0;
   // Get hit residuals w.r.t. tracks
   // (exclude each hit from the track and refit it to avoid bias)

   //  TObjArray *Hlist = new TObjArray();
   Bool_t MC = false;

   vector<Int_t> effstaUp   = {0, 0, 0, 0, 0, 0, 0};
   vector<Int_t> effstaDown = {0, 0, 0, 0, 0, 0, 0};

   // TString inFile="1589_testSI.root";//"1882_lorentz.root";
   TString outFile = "aaa.root";
   // TString histFile = "resid.histo_1589_testSI.root";
   TFile out(histFile, "RECREATE");

   Float_t xext, yext, zhit, dXext, dYext, prec, chi2, xsmo, ysmo, dXsmo, dYsmo, dXZ, dYZ, precPT, precPX, precNOf,
      trkAng, Qf, Qb;
   Int_t   flag, ista, nhits, nsecGEM, evNo, nsecGEMhot;
   Float_t sectors[8];
   Int_t   nsec;
   TTree * txyz = new TTree("hits", "Hits");
   txyz->Branch("evNo", &evNo, "evNo/I");
   txyz->Branch("xext", &xext, "xext/F");
   txyz->Branch("yext", &yext, "yext/F");
   txyz->Branch("zhit", &zhit, "zhit/F");
   txyz->Branch("dXext", &dXext, "dXext/F");
   txyz->Branch("dYext", &dYext, "dYext/F");
   txyz->Branch("xsmo", &xsmo, "xsmo/F");
   txyz->Branch("ysmo", &ysmo, "ysmo/F");
   txyz->Branch("dXsmo", &dXsmo, "dXsmo/F");
   txyz->Branch("dYsmo", &dYsmo, "dYsmo/F");
   // txyz->Branch("dXZ",&dXZ,"dXZ/F");
   // txyz->Branch("dYZ",&dYZ,"dYZ/F");
   txyz->Branch("prec", &prec, "prec/F");
   // txyz->Branch("precPT",&precPT,"precPT/F");
   // txyz->Branch("precPX",&precPX,"precPX/F");
   // txyz->Branch("precNOf",&precNOf,"precNOf/F");
   txyz->Branch("chi2", &chi2, "chi2/F");
   txyz->Branch("ista", &ista, "ista/I");
   txyz->Branch("nhits", &nhits, "nhits/I");
   txyz->Branch("nsec", &nsec, "nsec/I");
   txyz->Branch("nsecGEM", &nsecGEM, "nsecGEM/I");
   // txyz->Branch("nsecGEMhot",&nsecGEMhot,"nsecGEMhot/I");
   txyz->Branch("trkAng", &trkAng, "trkAng/F");
   txyz->Branch("Qf", &Qf, "Qf/F");
   txyz->Branch("Qb", &Qb, "Qb/F");

   TString parFile = inFile;
   // fRun->Init();

   // FairRootManager* ioman = FairRootManager::Instance();
   //  if ( ! ioman ) Fatal("Init", "No FairRootManager");
   //  TClonesArray*		fHit  = (TClonesArray*) ioman->GetObject("StsHit");//StsPoint

   // fRun->SetOutputFile(outFile);

   TList *parFileList = new TList();

   TObjString stsDigiFile = "sts_v1_BMN_SI_GEM.digi.par"; // sts_v16BMN_gem.digi.par
   // TObjString stsDigiFile2 = "/home/pokat/bmnM/bmnR/AZ_data/sts/GEMS_v16_AZ_3mm.root";
   parFileList->Add(&stsDigiFile);

 
      GetBmnGeom("SIGEMS_r7.root"); //"GEMS_RunWinter2016_old.root");

   ApplyAlignment();

   FairRunAna *fRun = new FairRunAna();
   fRun->SetSource(new BmnFileSource(inFile));
   fRun->SetOutputFile(outFile);

   FairRuntimeDb *rtdb = fRun->GetRuntimeDb();

   FairParRootFileIo * parIo1 = new FairParRootFileIo();
   FairParAsciiFileIo *parIo2 = new FairParAsciiFileIo();
   parIo1->open(parFile.Data());
   parIo2->open(parFileList, "in");
   rtdb->setFirstInput(parIo1);
   rtdb->setSecondInput(parIo2);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();

   // -----   Field map   -------------------------------------------
   /*
   Int_t run_period=1, run_number=065;
   UniRun* pCurrentRun = UniRun::GetRun(run_period, run_number);
   if (pCurrentRun == 0) {
       exit(-2);
   }
   */
   /*  Double_t fieldScale = 0.01;
     double map_current = 900.0;
     //int* current_current = pCurrentRun->GetFieldCurrent();
     //int* current_current = NULL;
     int current_current[1] = {500};
     if (current_current == NULL) {
         fieldScale = 0.01;

     } else
         fieldScale = (*current_current) / map_current;
       //  cout<<" SCALE: "<<fieldScale<<endl;
         fieldScale=TMath::Max(fieldScale,0.01);
         */
   //  if(!MC){
   BmnNewFieldMap *magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.dat");
   magField->SetScale(
      Mscale); // 1.366666667);//1.777777778);//0.666666667);//2.0);//1.777777778);//2.0);//fieldScale);1.333333333
   magField->Init();
   fRun->SetField(magField);
   cout << "FIELD SCALE: " << magField->GetScale() << endl;

   // BmnFieldConst* magField = new BmnFieldConst();
   //  magField->SetFieldRegion(-3000., 3000., -3000., 3000., -3000., 3000);
   //    magField->SetField(0, 0.01 , 0.);
   //  magField->Init();
   // ------------------------------------------------------------------------

   cout << "FIELD SCALE: " << magField->GetScale() << endl;
   /*} else {
     BmnFieldConst* magField = new BmnFieldConst();
     magField->SetFieldRegion(-3000., 3000., -3000., 3000., -3000., 3000);
     magField->SetField(0, 5.9 , 0.);

     magField->Init();
     fRun->SetField(magField);
   }
   */

   FairTask *pKF = new CbmKF();
   fRun->AddTask(pKF);

   fRun->Init();

   // TClonesArray *pSTS = (TClonesArray *) FairRootManager::Instance()->GetObject("StsPoint");
   // TClonesArray *trMC = (TClonesArray *) FairRootManager::Instance()->GetObject("MCTrack");
   // AZ TChain *rec = (TChain*) gROOT->FindObject("cbmout");
   TChain *      rec      = FairRootManager::Instance()->GetInChain();
   TClonesArray *stsTrack = (TClonesArray *)FairRootManager::Instance()->GetObject("StsTrack");
   // TClonesArray *trMatch = (TClonesArray *) FairRootManager::Instance()->GetObject("StsTrackMatch");
   TClonesArray *      stsHit     = (TClonesArray *)FairRootManager::Instance()->GetObject("StsHit");
   TClonesArray *      stsDigi    = (TClonesArray *)FairRootManager::Instance()->GetObject("StsDigi");
   TClonesArray *      stsCluster = (TClonesArray *)FairRootManager::Instance()->GetObject("StsCluster");
   CbmVertex *         primVtx    = (CbmVertex *)FairRootManager::Instance()->GetObject("PrimaryVertex.");
     TClonesArray *BmnH     = (TClonesArray *)FairRootManager::Instance()->GetObject("EventHeaderBmn");
   CbmStsKFTrackFitter fitter;
   fitter.Init();

   Int_t events  = rec->GetEntries();
   Int_t events0 = events;
   // if (nRead != 0) events = TMath::Min (events, nRead);
   // cout << " Number of events = " << events << " N of StsHits: "<<stsHit->GetEntries()<< endl;

   // Get STS geometry parameter container
   // CbmGeoStsPar *geoPar = (CbmGeoStsPar*) rtdb->getContainer("CbmGeoStsPar");
   FairRuntimeDb *db = fRun->GetRuntimeDb();
   if (!db) Fatal("SetParContainers", "No runtime database");
   // Get STS digitisation parameter container
   CbmStsDigiPar *digiPar = (CbmStsDigiPar *)db->getContainer("CbmStsDigiPar");
   digiPar->init(parIo2);

   // cout<<"digiPAR: "<<digiPar->GetNStations()<<endl;

   CbmStsDigiScheme *digiScheme = new CbmStsDigiScheme();
   digiScheme->Init(NULL, digiPar);

   // Int_t nStat = digiScheme->GetNStations();
   // Double_t *zStat = new Double_t [nStat];
   // for (Int_t i = 0; i < nStat; ++i) zStat[i] = digiScheme->GetStationByNr(i+1)->GetZ();

   FairTrackParam param, pIter1;

   for (Int_t iev = 0; iev < events; ++iev) {
      if (iev % 10000 == 0) cout << iev << endl;
      //  if (iev < nBeg) continue;

      FairRootManager::Instance()->ReadEvent(iev);
      // continue; //!!!!
      evNo          = iev;
      Int_t nPoints = stsHit->GetEntriesFast();   // pSTS->GetEntriesFast();
      Int_t nTracks = stsTrack->GetEntriesFast(); // trSTS->GetEntriesFast();
      // CbmStsDigi *digi = (CbmStsDigi*) stsDigi->UncheckedAt(iev);
//cout << ", Mult: " << stsTrack->GetEntriesFast() << " hits: "<<nPoints<<"\n";
    //if( nTracks<1 ) continue;
//-----------------control tracks from PV---------------------------
/*         Int_t T0   = ((BmnEventHeader *)BmnH->At(0))->GetTrigT0();
         Int_t BC2  = ((BmnEventHeader *)BmnH->At(0))->GetTrigBC2();
         Int_t VETO = ((BmnEventHeader *)BmnH->At(0))->GetTrigVETO();
         Int_t BD   = ((BmnEventHeader *)BmnH->At(0))->GetTrigBD();

if(T0!=1 || BC2!=1 || VETO!=0) continue;

//cout<<"to: "<<T0<<" bc2: "<<BC2<<" veto: "<<VETO<<" bd: "<<BD<<endl;

    //if( nTracks<1 ) continue;
     TVector3 PVpos(primVtx->GetX(), primVtx->GetY(), primVtx->GetZ());
      TVector3 PVexp(0, 0, -24.0);
Int_t nhitsLarger3=0, fromPV=0;
if(nTracks<2) continue;
           if ((PVpos - PVexp).Mag() > 5) continue; //1.8


 for (Int_t i = 0; i < nTracks; ++i) {
CbmStsTrack *tr = (CbmStsTrack *)stsTrack->UncheckedAt(i);
Int_t nhits1 = tr->GetNStsHits();
fromPV++;
if(nhits1>3) nhitsLarger3++;
      }

if(fromPV<2 && nhitsLarger3<2) continue;
*/

      for (Int_t i = 0; i < nTracks; ++i) {
         CbmStsTrack *tr = (CbmStsTrack *)stsTrack->UncheckedAt(i);
//cout<<"\n"<<endl;
         nhits = tr->GetNStsHits();
/*         if(nhits<4) continue;


 CbmKFTrack kftr = CbmKFTrack(*tr);
            FairTrackParam param;
            kftr.Extrapolate(primVtx->GetZ());
            kftr.GetTrackParam(param);

          // distance betwee track fit to PV  && PV postion
            Double_t dx    = param.GetX() - primVtx->GetX();
            Double_t dy    = param.GetY() - primVtx->GetY();
            Double_t dz    = param.GetZ() - primVtx->GetZ();

  Double_t  dist = TMath::Sqrt (dx * dx + dy * dy + dz * dz);

            if(dist>0.5) continue; */
//-----------------control tracks from PV---------------------------


         // if (nhits !=6) continue;
         //  if (nhits !=6 || stsDigi->GetEntries()>80) continue; // not enough hits
         // if (nhits !=6 || stsCluster->GetEntries()!=12) continue; // not enough hits
         TVector3 trackPos[2];
         tr->GetParamFirst()->Position(trackPos[0]);
         tr->GetParamLast()->Position(trackPos[1]);

         CbmStsTrack *track = new CbmStsTrack(*tr);
         //  Int_t pdgDeuteron=1000010020;
         //  fitter.DoFit(track, pdgDeuteron );

         prec = 1. / track->GetParamFirst()->GetQp();
         chi2 = track->GetChi2() / track->GetNDF();
         TVector3 mom, mom2;
       //  cout<<" prec: "<<1/track->GetParamFirst()->GetQp() <<"x,y,z,tx,ty: "<<track->GetParamFirst()->GetX()<<","<<track->GetParamFirst()->GetY()<<","<<track->GetParamFirst()->GetZ()<<","
       //  <<track->GetParamFirst()->GetTx()<<","<<track->GetParamFirst()->GetTy()<<endl;


//cout<<" prec: "<<1/track->GetParamLast()->GetQp() <<"x,y,z,tx,ty: "<<track->GetParamLast()->GetX()<<","<<track->GetParamLast()->GetY()<<","<<track->GetParamLast()->GetZ()<<","
       //  <<track->GetParamLast()->GetTx()<<","<<track->GetParamLast()->GetTy()<<endl;

         // track->GetParamFirst()->Momentum(mom);
         // precPT=mom.Pt();

         // CbmKFTrack track2 = CbmKFTrack(*track);
         // track2.SetPID(pdgDeuteron);
         // CbmKFParticle *deuteron= new CbmKFParticle(&track2);

         // FairTrackParam param_ex;
         // track2.Extrapolate(-100);
         // track2.Smooth(primVtx->GetZ());
         // track2.GetTrackParam(param_ex);

         //  param_ex.Momentum(mom2);
         // precNOf=mom2.Pt();
         //  precPX=mom2.Px();

         // txyz->Fill();
         /*
         CbmStsTrack *tr = (CbmStsTrack*) trSTS->UncheckedAt(i);
         CbmTrackMatch *trM = (CbmTrackMatch*) trMatch->UncheckedAt(i);
         Int_t id = trM->GetMCTrackId();
         if (id < 0) continue;
         //if (trM->GetNofTrueHits() <= trM->GetNofFakeHits() + trM->GetNofWrongHits()) continue;

         CbmMCTrack *mcTr = (CbmMCTrack*) trMC->UncheckedAt(id);
         TVector3 trackPos[2];
         tr->GetParamFirst()->Position(trackPos[0]);
         tr->GetParamLast()->Position(trackPos[1]);
         prec = 1./tr->GetParamFirst()->GetQp();
         chi2 = tr->GetChi2() / tr->GetNDF();
         nhits = tr->GetNStsHits();
         */

         //-----------------------GP extrapolate----------------------

         //-----------------------end---------------------------------
         // cout<<""<<endl;
         nsec                = 0;
         nsecGEM             = 0;
         nsecGEMhot          = 0;
         std::vector<int> v1 = {1, 1, 1, 1, 1, 1};
         std::vector<int> v2 = {1, 1, 1, 3, 3, 3};
         std::vector<int> v3 = {0, 0, 0, 0, 0, 0};
         //  for (Int_t Niter = 0; Niter < 2; ++Niter)
         /*for (Int_t iHit = 0; iHit < nhits; ++iHit) {
           CbmStsHit *hit = (CbmStsHit*) stsHit->UncheckedAt(track->GetStsHitIndex(iHit));
           Int_t secn=hit->GetSectorNr();
           Int_t stan=hit->GetStationNr();
           v3[iHit]=secn;
         } */

         // if(v3!=v1 && v3!=v2) continue;
//cout<<"\n";
         for (Int_t iHit = 0; iHit < nhits; ++iHit) {
            CbmStsHit *hit = (CbmStsHit *)stsHit->UncheckedAt(track->GetStsHitIndex(iHit));

            if (!hit) {
               cout << "-E- CbmStsMatchTracks::Exec: "
                    << "No StsHit " << iHit << " for track " << i << endl;
               // warn = kTRUE;
               continue;
            }
            TVector3 pos, residXY;
            hit->Position(pos);
            //  if(hit->GetStationNr()==1) pos[2]+=0.022;
           // if(hit->GetStationNr()==2) pos[2]-=0.022;
           //     if(hit->GetStationNr()==3) pos[2]+=0.022;
         //   cout<<" nhits: "<<nhits<<" iHit: "<<iHit<<" posZ: "<<pos[2]<<" index: "<<track->GetStsHitIndex(iHit)<<endl;
            
            // if(Niter==1){
            //  pos[0]=  pos[0]-param.GetTx()/2;
            // }

            // residXY=GetStrightResid( trackPos[0], trackPos[1], pos  );

            // dXZ=residXY[0];
            // dYZ=residXY[1];

            // Remove this hit and refit
            CbmStsTrack trackC(*track);
            TArrayI &   hits = *trackC.GetStsHits();
            hits.Set(nhits - 1);
            Int_t indx = 0;
            for (Int_t j = 0; j < nhits; ++j) {
               if (j == iHit) continue; // UNincludes hit
            //  if (j == -1) continue; //includes hit
               hits[indx++] = track->GetStsHitIndex(j);
            }
            CbmKFTrack kftr = CbmKFTrack(trackC);
            fitter.SetKFHits(kftr, &trackC);
            kftr.Fit(kTRUE); // downstream
            kftr.GetTrackParam(param);
            // cout << track->GetParamFirst()->GetX() << " " << track->GetParamFirst()->GetZ() << " "
            //   << param.GetX() << " " << param.GetZ() << endl;
        //    cout<<"last sta: "<<ista<< " prec: "<<1/param.GetQp() <<"x,y,z,tx,ty: "<<param.GetX()<<","<<param.GetY()<<","<<param.GetZ()<<","<<param.GetTx()<<","<<param.GetTy()<<endl;
            trackC.SetParamLast(param);
            kftr.Fit(kFALSE); // upstream
            kftr.GetTrackParam(param);
            trackC.SetParamFirst(param);
          //  cout<<"first sta: "<<ista<< " prec: "<<1/param.GetQp() <<"x,y,z,tx,ty: "<<param.GetX()<<","<<param.GetY()<<","<<param.GetZ()<<","<<param.GetTx()<<","<<param.GetTy()<<endl;

            // Get residuals
            CbmKFTrack tr = CbmKFTrack(trackC);
            tr.Extrapolate(pos[2]);
            ista = hit->GetStationNr();
            tr.GetTrackParam(param);
          //  cout<<"Extrap sta: "<<ista<< " prec: "<<1/param.GetQp() <<"x,y,z,tx,ty: "<<param.GetX()<<","<<param.GetY()<<","<<param.GetZ()<<","<<param.GetTx()<<","<<param.GetTy()<<endl;
             xext = param.GetX();
            yext = param.GetY();
            zhit = pos.Z();
        //     cout<<"pos z: "<<pos[2]<<" zhit: "<<zhit<<endl;
            dXext          = pos[0] - xext;
            dYext          = pos[1] - yext;
            CbmKFTrack tr1 = CbmKFTrack(trackC);
            fitter.SetKFHits(tr1, &trackC);
            tr1.Smooth(pos[2]);
            tr1.GetTrackParam(param);
           // cout<<"SMO sta: "<<ista<< " prec: "<<1/param.GetQp() <<"x,y,z,tx,ty: "<<param.GetX()<<","<<param.GetY()<<","<<param.GetZ()<<","<<param.GetTx()<<","<<param.GetTy()<<endl;
            // if(Niter==0) tr1.GetTrackParam(pIter1);
            xsmo = param.GetX();
            ysmo = param.GetY();

            // cout<<"xsmo: "<< xsmo<<" ysmo: "<<ysmo<<endl;
            dXsmo = pos[0] - xsmo;
            dYsmo = pos[1] - ysmo;
            nsec = hit->GetSectorNr();
          /*  if (ista == 1) {
               nsec = hit->GetSectorNr();
            } else
               nsec = -100;

            if (ista > 1) {
               // if(hit->GetSectorNr()<=2) nsecGEM=1;
               // else nsecGEM=2;

               nsecGEM = hit->GetSectorNr();

               // if(nsecGEM%2==0) continue;
            } */
            // cout<<"dxsmo: "<< dXsmo<<" dysmo: "<<dYsmo*10<<" posX: "<<pos[0]<<" posY: "<<pos[1]<< "xsmo: "<<xsmo<< "
            // ysmo: "<<ysmo<<" Z: "<<pos[2]<<endl;
            // cout<<"Z: "<<pos[2]<<endl;

            trkAng = TMath::ATan(param.GetTx());
            // stsCluster->GetQtot();

            // Double_t adcHit=((CbmStsCluster)(stsCluster->UncheckedAt(hit->fDigiF)))->GetQtot();
            CbmStsCluster *stsclF = (CbmStsCluster *)stsCluster->UncheckedAt(hit->fDigiF);
            CbmStsCluster *stsclB = (CbmStsCluster *)stsCluster->UncheckedAt(hit->fDigiB);
            Qf                    = stsclF->GetQtot();
            Qb                    = stsclB->GetQtot();
            // cout<<" adcHIT: "<<stscl->GetQtot()<<endl;

            // if(Niter==1)	txyz->Fill();
            txyz->Fill();

            // hits->Draw("dXsmo:xsmo","ista==6 && abs(dXsmo)<1 && nhits>4 && xsmo>-100 && prec<100","colz")
            /*if(nhits>4 && abs(xsmo)>-100 && prec<100){
              if(ista==1 && TMath::Abs(dXsmo)<1  ) h1->Fill(xsmo,dXsmo);
              else    if(ista==2 && TMath::Abs(dXsmo)<1 ) h2->Fill(xsmo,dXsmo);
              else      if(ista==3 && TMath::Abs(dXsmo)<1 ) h3->Fill(xsmo,dXsmo);
              else        if(ista==4 && TMath::Abs(dXsmo)<1 ) h4->Fill(xsmo,dXsmo);
              else           if(ista==5 && TMath::Abs(dXsmo)<1 )h5->Fill(xsmo,dXsmo);
              else             if(ista==6 && TMath::Abs(dXsmo)<1 ) h6->Fill(xsmo,dXsmo);

            }
            */

         } // for (Int_t iHit = 0; iHit < nhits;

      } // for (Int_t i = 0; i < nTracks;

   } // for (Int_t iev = 0; iev < events;
   // cout<<" size: "<<effstaUp.size()<<endl;
  // for (size_t nstaeff = 0; nstaeff < effstaUp.size(); nstaeff++)
  //    cout << " sta: " << nstaeff << " eff: " << (Double_t)effstaUp[nstaeff] / (Double_t)effstaDown[nstaeff] << endl;
   //  TCanvas* cHist2 = new TCanvas("cHist2","Canvas 2",600,600,800,800);
   //   cHist2->Divide(2);

   // cHist2->cd(1); /*gPad->SetLogz();*/ gPad->SetGrid(); h1->Draw("colz");
   // cHist2->cd(2); /*gPad->SetLogz();*/gPad->SetGrid(); h2->Draw("colz");
   // cHist2->cd(1); /*gPad->SetLogz();*/gPad->SetGrid(); h3->Draw("colz");
   // cHist2->cd(2); /*gPad->SetLogz();*/gPad->SetGrid(); h4->Draw("colz");
   // cHist2->cd(3); /*gPad->SetLogz();*/ gPad->SetGrid();h5->Draw("colz");
   // cHist2->cd(4);  /*gPad->SetLogz();*/gPad->SetGrid(); h6->Draw("colz");

   //cHist2->Update();
   //out.Add(cHist2);

   //out.Add(cHist3);
   //  Hlist->Write();
   //   Hlist->Add(txyz);
   //   Hlist->Add(hist2);
   // cHist2->cd(1);   hist1->Draw();
   // cHist2->cd(2);  hist2->Draw();
   // cHist2->Update();
   // Hlist->Write();

   out.Write();
   out.Close();
}
// ----------------------------------------------------------------------------



Bool_t GetBmnGeom(const char *fileName)
{
   // ---> Get TGeoManager and top node (TOP)

   TFile *geoFile = new TFile(fileName, "READ");
   if (!geoFile->IsOpen()) {
      cout << "-E- Could not open file!" << endl;
      return kFALSE;
   }
   geoFile->FindObjectAny("SIGEMS"); //"GEMS_geom SIGEMS"); //AZ
   return kTRUE;                     // AZ

   // ---> Get TOP node from file
   TList *     keyList = geoFile->GetListOfKeys();
   TIter       next(keyList);
   TKey *      key = NULL;
   TGeoVolume *top = NULL;
   while ((key = (TKey *)next())) {
      TString className(key->GetClassName());
      if (className.BeginsWith("TGeoVolume")) {
         top = dynamic_cast<TGeoVolume *>(key->ReadObj());
         std::cout << "Found volume " << top->GetName() << endl;
         break;
      }
   }
   if (!top) {
      cout << "-E- Could not find volume object in file" << endl;
      return kFALSE;
   }
   // cout << gGeoManager << endl;
   gGeoManager->GetListOfVolumes()->ls();
   gGeoManager->SetTopVolume(top);

   return kTRUE;
}

void ApplyAlignment()
{


  // Apply alignment

const int ip_r7[6][2]={0,1, 2,3, 4,5, 6,7, 8,9, 10,11};

  TGeoNode* st_node = NULL;
  // Get STS node
  TGeoNode* sts = NULL;
  gGeoManager->CdTop();
  TGeoNode* cave = gGeoManager->GetCurrentNode();
  for (Int_t iNode = 0; iNode < cave->GetNdaughters(); iNode++) {
    TGeoNode* node = cave->GetDaughter(iNode);
    TString name = node->GetName();
    if ( name.Contains("GEMS_0") ) {
      sts = node;
      gGeoManager->CdDown(iNode);
      break;
    }
  }
  if ( ! sts ) {
    cout << "-E- CbmBmnStsDigitize::ApplyAlignment: Cannot find top GEM node"
	 << endl;
    //return kFALSE;
  }

  // Loop over stations in STS
  Int_t statNr = 0;
  Int_t nStat0 = sts->GetNdaughters();
  Int_t nMod0sts=0;
  
  cout<<"STATIONS : "<<nStat0<<endl;
/*
 Double_t xAlign[26] = 
 {3.32829,-2.67635,2.83055,-3.16945, //I Si
  2.84975, -3.11505, //II Si
  8.91718,2.91888,-3.09552,-9.09292,  8.91618,2.91618,-3.08382,-9.08382, //III Si
  -40.716, 40.914, 41.085, -40.598, 42.278, -39.429, -38.45, 43.25,  44.076, -37.596,  -36.8092, 44.8207};//GEM
 //{-40.8258, 40.8258, 40.845, -40.845, 40.8608, -40.8608, -40.8578, 40.8578, 40.845, -40.845, -40.8298, 40.8298}; //for 4 GeV

  Double_t yAlign[26] =
  {4.46297,5.07597, -1.55503,-1.05503, //I Si
   1.8115,1.42, //II Si
   7.84856,7.80306,7.92256,7.85506,-4.25594,-4.25594, -4.25594,-4.25594, // III Si  
   22.5211, 22.2759, 22.574, 22.6124,  22.8827,  22.516, 22.5385, 22.9377, 22.5828, 22.3481, 22.7875, 23.007};//GEM
  //{22.2753, 22.4706, 22.4851, 22.6689, 22.7851, 22.5916,  22.3308, 23.1519, 22.4786, 22.4484, 22.5971, 23.2184}; //for 4 GeV

   Double_t zAlign3[9] =
   {14.365-0.025, 17.69-0.05, 26.0775, //3 planes of Si
    42.2,67.77,115.569,137.8325,163.54,186.103}; //6 planes of GEM

  Double_t zAlign23[26] = 
  {-0.315,0.315,0.365,-0.365, // I Si
   -0.33,0.33, //II Si
    0.8325,-0.7675, 0.7525,  -0.8175,    -0.4275,  1.1825, -0.2875, 1.3225, //III Si                 
    0,0,  0,0,  -0.085, 0.085,  -0.0175,0.0175,  0.01,-0.01, 0,0}; //GEMS sensors
  //{ 0.14595, -0.14595, -0.1258, 0.1258, -0.2005, 0.2005, 0.0945, -0.0945, -0.1125, 0.1125, 0.0865, -0.0865 }; //for 4 GeV
*/
 Double_t xAlign[ 26 ]=
{3.33029,-2.67835,2.82585 ,-3.14615, //I Si
  2.85175, -3.11705, //II Si
  8.91918 ,2.92088, -3.09752,-9.09492,  8.91198 ,2.94528 ,-2.98182,-8.98402, //III Si
  -40.718 , 40.916, 41.087, -40.6, 42.28, -39.431, -38.452, 43.252,  44.078, -37.598,  -36.8112, 44.8227};//GEM

 Double_t yAlign[ 26 ]=
  {4.46297,5.07597, -1.48803,-0.910531, //I Si
   1.8115,1.42, //II Si
   7.84856,7.80306,7.92256,7.85506,-4.57154,-4.61864, -4.43934,-4.51954, // III Si  
//   22.5211-0.17, 22.2759-0.17, 22.574-0.15, 22.6124-0.15,  22.8827-0.1,  22.516-0.1, 22.5385-0.05, 22.9377-0.05, 22.5828+0.025, 22.3481+0.025, 22.7875+0.07, 23.007+0.07};//GEM
   22.5211, 22.2759, 22.574, 22.6124,  22.8827,  22.516, 22.5385, 22.9377, 22.5828, 22.3481, 22.7875, 23.007};//GEM
   
   Double_t zAlign3[9] =
   {14.352475, 17.68975, 26.2729875, //3 planes of Si
    42.19995,67.76995,115.569,137.8325,163.54,186.103}; //6 planes of GEM

  Double_t zAlign23[26] = 
  {-0.406375,0.430925,0.274025,-0.298575, // I Si
   -0.43165,0.43165, //II Si
   0.540813, -1.06009, 0.653113, -0.915988, -0.719887, 0.761012, -0.386287, 1.12281, //III Si                 
   0.08685,-0.08685,  -0.07145,0.07145,  -0.128, 0.128,  0.0115,-0.0115,  -0.004,0.004, 0,0}; //GEMS sensors
 Double_t driftcorr= 0;//-0.2;
Int_t stn=0,SensCh=0;
  //for (Int_t iNode = 1; iNode < sts->GetNdaughters(); iNode++) {
  for (Int_t iNode = 0; iNode < nStat0; iNode++) {
    // Go to station node
    gGeoManager->CdDown(iNode);
    TGeoNode* stationNode = gGeoManager->GetCurrentNode();
    TString statName = stationNode->GetName();
    if ( ! statName.Contains("station") ) {
      gGeoManager->CdUp();
      continue;
    }
    //AZ Int_t statNr = stationNode->GetNumber();
    ++stn; //AZ
/*
    if(stn<=3){
gGeoManager->CdUp();
 continue;}

 if(stn<=3) continue;
*/
 ++ statNr;

    TGeoHMatrix *matr = gGeoManager->GetCurrentMatrix();
    Double_t* statTrans = matr->GetTranslation();
    //Double_t* statRot = matr->GetRotationMatrix();
    TGeoRotation *r2;
  //if(statNr-1 >1)  statTrans[2] = zAlign3[statNr-1]+driftcorr;//zAlign[statNr];
  cout<<"statNr : "<<statNr <<", "<<statTrans[2]  <<" zal: "<<zAlign3[statNr-1]<<endl;
   statTrans[2] = zAlign3[statNr-1];//zAlign[statNr];

   TGeoTranslation *t2 = new TGeoTranslation(statTrans[0],statTrans[1],statTrans[2]);
  //if( statNr==2 || statNr==3 || statNr==5)r2 = new TGeoRotation("rot",0,180,180);
   // if(statNr==1 || statNr==4 || statNr==6) r2 = new TGeoRotation("rot",0,180,180);
     r2 = new TGeoRotation("rot",0,0,0);

      TGeoCombiTrans *cc1 = new TGeoCombiTrans(*t2,*r2);
    //  statTrans[0] = xAlign[statNr];
    //  statTrans[1] = yAlign[statNr];

    //  cout<<"ST TRANS Z: "<<statTrans[2]<<endl;

   //if(statNr-1 >1)  statTrans[2]=statTrans[2]+driftcorr;
    //cout<<"ST TRANS Z: "<<statTrans[2]<<endl;
    //if(iNode>0) statTrans[2]=statTrans[2]-2.47;
    //matr->SetTranslation(statTrans);
    //matr->SetMatrix(statRot);

    TGeoHMatrix *matr0 = new TGeoHMatrix(*cc1);
    matr0->RegisterYourself();
    
    //  Int_t nModules = stationNode->GetNdaughters();
    Int_t nModules = GetNofModules(stationNode);
    //  cout<<"nModules: "<<nModules<<endl;
    
    //sts->GetVolume()->ReplaceNode(stationNode,0,gGeoManager->GetCurrentMatrix()); //AZ
    //sts->GetVolume()->RemoveNode(stationNode); //AZ
    sts->GetVolume()->AddNode((TGeoVolumeAssembly*)stationNode->GetVolume(),0,matr0); //AZ

    //AZ- hard-coded st_node=sts->GetVolume()->GetNode(iNode+6);
    st_node = (TGeoNode*) sts->GetVolume()->GetNodes()->Last(); //AZ

   // Double_t  statZ = statTrans[2];
   //cout <<"sta: " << statNr << " " << gGeoManager->GetCurrentMatrix()->GetTranslation()[2] << " " << sts->GetNdaughters() << endl;

    //gGeoManager->CdUp();               // to sts

    //-----------------------module translate-------------
    Int_t moduleNr = 0, copy_no = 0;
    //cout<<"nMODULES: "<< nModules<<endl;
    /*if(iNode<2){
    //  nMod0sts=nModules;
      nModules=nModules*2;
      
      } */
    // ---> Large sensors
    for (Int_t iStatD = 0; iStatD < nModules; iStatD++) {
      gGeoManager->CdDown(iStatD);

      TGeoNode* module = gGeoManager->GetCurrentNode();
      if ( ! TString(module->GetName()).Contains("module") ) {
	gGeoManager->CdUp();
	continue;
      }
cout << iStatD << " " << module->GetName() << " stn: "<<statNr << endl; 

//-------------------------add I II III Si----------------------------
 if(iNode<=2){

	Int_t nOfModSi=SensCh;//10+(moduleNr++);
  cout<<"modSI: "<<nOfModSi<<" "<<zAlign23[nOfModSi]<<endl;
	 // fprintf(parFile, "%4d %4d\n", moduleNr, 1);
	  // ---> Sensor number
   // geoMan->CdUp(); // back to module sensor
	 // SaveSensor(geoMan, parFile, phiStat, sensor);
	TGeoHMatrix *matrMod = gGeoManager->GetCurrentMatrix();
	Double_t* modTrans = matrMod->GetTranslation();
  //cout<<"trans: "<<modTrans[2]<<endl;
  modTrans[0] = xAlign[nOfModSi];
  modTrans[1] = yAlign[nOfModSi];
  modTrans[2] = zAlign23[nOfModSi];//zAlign2[ipNr];
	matrMod->SetTranslation(modTrans);
	TGeoHMatrix *matr0Mod = new TGeoHMatrix(*matrMod);
	matr0Mod->RegisterYourself();

	//sts->GetVolume()->ReplaceNode(stationNode,0,gGeoManager->GetCurrentMatrix()); //AZ
	//sts->GetVolume()->RemoveNode(stationNode); //AZ
	//  stationNode->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),0,matr0Mod); //AZ
	
	//cout<<" 1 st name add: "<< stationNode->GetName()<<" mod name add: "<<module->GetName()<< " "<< module<< " i: "<<iStatD<<" cols: "<<st_node->GetVolume()->GetNdaughters()<<endl;
	//Double_t* sensTrans = matrMod->GetTranslation();
	//cout<<"trans: "<<sensTrans[0]<<" "<<sensTrans[1]<<" "<<sensTrans[2]<< " Nr mod:  "<<moduleNr<<endl;
	//stationNode->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),copy_no,matr0Mod);
	st_node->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),copy_no,matr0Mod);
	//cout<<" 1 st name add: "<< stationNode->GetName()<<" mod name add: "<<module->GetName() << " "<< module<< " i: "<<iStatD<<" cols: "<<st_node->GetVolume()->GetNdaughters()<<endl;
	//Double_t  modZ = modTrans[2];
   Double_t* modTrans1 = gGeoManager->GetCurrentMatrix()->GetTranslation();
//	cout <<"mod: " << nOfModSi << " VEC: " << modTrans1[0] << " "<<modTrans1[1] << " "<<modTrans1[2] << endl;
	copy_no++;
	//delete matr0Mod;
  gGeoManager->CdUp();  // back to module

  SensCh++;

}
//-------------------------add I II III Si----------------------------

      
 if(iNode>2){
      if (TString(module->GetName()).Contains("Senso")) {
	//if(iNode>0 && iNode<=3) moduleNr=0;
	//else{
	  if(iStatD==0) moduleNr=0;
	  if(iStatD>0) moduleNr=1;
//	}
	//moduleNr=iStatD;
	//fprintf(parFile, "%4d %4d\n", moduleNr, 1);
	//SaveSensor(geoMan, parFile, phiStat, module);

	Int_t ipNr = 0;
  //cout<<" iNode: "<< iNode<<endl;
	//if (iNode>0 && iNode<=3) ipNr = iNode;
	//else ipNr = iNode*2 - 4 + moduleNr;
//Int_t order=-1;
//if( (statNr==1 || statNr==4 || statNr==6) && moduleNr==0) order=1; 
  

	TGeoHMatrix *matrMod = gGeoManager->GetCurrentMatrix();
	Double_t* modTrans = matrMod->GetTranslation();
  //cout<<" modtransX: "<<modTrans[0]<<" xali: "<<xAlign[ip_r7[statNr-1][moduleNr]]<< "sub: "<<xAlign[ip_r7[statNr-1][moduleNr]]-modTrans[0]  <<endl;
	 cout<<" aaaa ^^^^^^^^^^^^^^^^^^^ : "<<statNr-1;
  cout<<"\n"<<moduleNr;
  cout<<"\n"<<ip_r7[ (statNr-1)-3 ][moduleNr]<<endl;

   modTrans[0] = xAlign[ip_r7[(statNr-1)-3][moduleNr]+14];
	 modTrans[1] = yAlign[ip_r7[(statNr-1)-3][moduleNr]+14];// - modTrans[1]; //not need anymore (- mod...)
   modTrans[2] = zAlign23[ip_r7[(statNr-1)-3][moduleNr]+14];//zAlign2[ipNr];
  //cout<<"ST TRANS Z: "<<zAlign23[ipNr]<<endl;
	matrMod->SetTranslation(modTrans);
	TGeoHMatrix *matr0Mod = new TGeoHMatrix(*matrMod);
	matr0Mod->RegisterYourself();
  ipNr++;
//cout<<" ip N: "<< ipNr<<endl;
	//sts->GetVolume()->ReplaceNode(stationNode,0,gGeoManager->GetCurrentMatrix()); //AZ
	//sts->GetVolume()->RemoveNode(stationNode); //AZ
	//  stationNode->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),0,matr0Mod); //AZ
	
	//cout<<" 1 st name add: "<< stationNode->GetName()<<" mod name add: "<<module->GetName()<< " "<< module<< " i: "<<iStatD<<" cols: "<<st_node->GetVolume()->GetNdaughters()<<endl;
	//Double_t* sensTrans = matrMod->GetTranslation();
	//cout<<"trans: "<<sensTrans[0]<<" "<<sensTrans[1]<<" "<<sensTrans[2]<< " Nr mod:  "<<moduleNr<<endl;
	//stationNode->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),copy_no,matr0Mod);
	st_node->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),copy_no,matr0Mod);
	//cout<<" 1 st name add: "<< stationNode->GetName()<<" mod name add: "<<module->GetName() << " "<< module<< " i: "<<iStatD<<" cols: "<<st_node->GetVolume()->GetNdaughters()<<endl;
	//Double_t  modZ = modTrans[2];
	//cout <<"mod: " << ipNr << " VEC: " << modTrans[0] << " "<<modTrans[1] << " "<<modTrans[2] << endl;
	copy_no++;
	//delete matr0Mod;
	
      }

      gGeoManager->CdUp();  // back to module

    }} // for (Int_t iStatD = 0; iStatD < nModules;
    //----------------------end module translate----------
    //delete matr0;
    gGeoManager->CdUp();               // to sts
  }                                    // station loop

  Int_t snr=1;
vector<TGeoNode*> removNodes;
  // Remove extra nodes
  for (Int_t iNode = 0; iNode < nStat0; iNode++) {
    // Go to station node
    //gGeoManager->CdDown(1);
      gGeoManager->CdDown(iNode);
    TGeoNode* stationNode = gGeoManager->GetCurrentNode();
     cout<<" st name del: "<< stationNode->GetName()<< " iNode: "<<iNode<<endl;
    /*if(iNode>2)*/ removNodes.push_back(stationNode);//sts->GetVolume()->RemoveNode(stationNode); //AZ
    gGeoManager->CdUp();               // to sts
  } 

  for(Int_t o=0; o<removNodes.size(); o++)
  sts->GetVolume()->RemoveNode(removNodes[o]);
removNodes.clear();


 for (Int_t iNode = 0; iNode <nStat0; iNode++) {
    // Go to station node
    //gGeoManager->CdDown(iNode);
    gGeoManager->CdDown(iNode);
    TGeoNode* stationNode = gGeoManager->GetCurrentNode();
    
    TString statName = stationNode->GetName();
    if ( ! statName.Contains("station") ) {
      gGeoManager->CdUp();
      continue;
    }
    if(iNode>2){
    Int_t nMod = 2;
    for (Int_t iStatD = 0; iStatD < nMod; iStatD++) {
      gGeoManager->CdDown(0);//stationNode->GetNdaughters()-1);
      
      TGeoNode* module = gGeoManager->GetCurrentNode();
      //  cout<<" 2 st name del: "<< stationNode->GetName()<<" mod name del: "<<module->GetName() << " i: "<<iStatD<<endl;
      stationNode->GetVolume()->RemoveNode(module); //AZ
      
      gGeoManager->CdUp();               // to sts
    } } else{  //cout<<" nModules: "<<GetNofModules(stationNode)<<endl;
    Int_t currNodes=GetNofModules(stationNode)/2;
for (Int_t iStatD = 0; iStatD <currNodes; iStatD++) {
      gGeoManager->CdDown(0);//stationNode->GetNdaughters()-1);
    TString mName = gGeoManager->GetCurrentNode()->GetName();
      if(!mName.Contains("module")) {
        gGeoManager->CdUp();
        continue;
      }
      TGeoNode* moduleS = gGeoManager->GetCurrentNode();
     //   cout<<" DELETE VALUME: "<< stationNode->GetName()<<" mod name del: "<<moduleS->GetName() << " i: "<<iStatD<<endl;
      stationNode->GetVolume()->RemoveNode(moduleS); //AZ
      
      gGeoManager->CdUp();               // to sts
    }

    }

    gGeoManager->CdUp();
  }
  
  for (Int_t iNode = 0; iNode < sts->GetNdaughters(); iNode++) {

    // Go to station node
    //gGeoManager->CdDown(iNode);
    gGeoManager->CdDown(iNode);
    TGeoNode* stationNode = gGeoManager->GetCurrentNode();
    cout<<" Check-in STATION: "<< stationNode->GetName()<<" zpos : "<<gGeoManager->GetCurrentMatrix()->GetTranslation()[2]<<endl;
    for (Int_t iModule = 0; iModule < stationNode->GetNdaughters() ; iModule++) {
      gGeoManager->CdDown(iModule);
      TGeoNode* moduleNode = gGeoManager->GetCurrentNode();


TString gName = moduleNode->GetName();
      if(gName.Contains("frame")) {
 stationNode->GetVolume()->RemoveNode(moduleNode); // remove frames
 iModule--;
 gGeoManager->CdUp();   
 continue;
      }


          cout<<" Check-in st name : "<< stationNode->GetName()<<" mod name : "<<moduleNode->GetName()<<endl;
      Double_t* sensTrans = gGeoManager->GetCurrentMatrix()->GetTranslation();
        cout<<"trans mod: "<<sensTrans[0]<<" "<<sensTrans[1]<<" "<<sensTrans[2]<< endl;
        for (Int_t iModule2 = 0; iModule2 < moduleNode->GetNdaughters() ; iModule2++) {
gGeoManager->CdDown(iModule2);
      TGeoNode* moduleNode2 = gGeoManager->GetCurrentNode();

TString hName = moduleNode2->GetName();
      if(hName.Contains("frame")) {
 moduleNode->GetVolume()->RemoveNode(moduleNode2); // remove frames
 iModule2--;
 gGeoManager->CdUp();   
 continue;
      }




      cout<<" Check-in st name : "<< stationNode->GetName()<<" hot name : "<<moduleNode2->GetName()<<endl;
      Double_t* sensTrans = gGeoManager->GetCurrentMatrix()->GetTranslation();
        cout<<"trans hot: "<<sensTrans[0]<<" "<<sensTrans[1]<<" "<<sensTrans[2]<< endl;

 gGeoManager->CdUp();
        }
      //  stationNode->GetVolume()->RemoveNode(moduleNode); //AZ
      gGeoManager->CdUp();
    }
    
    gGeoManager->CdUp();               // to sts
  }
 
  
//exit(0);
//gGeoManager->SetVisLevel(500);
//gGeoManager->GetTopVolume()->SetTransparency(0);
//gGeoManager->GetTopVolume()->Draw("ogl");
// gGeoManager->Export("SIGEMS_r7_eventDisplay.root","SIGEMS"); //with geomanager

//Int_t c;
//cin>>c;


}
