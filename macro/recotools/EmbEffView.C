/**
 * Macro for tracks versions comparison
 */
R__ADD_INCLUDE_PATH($VMCWORKDIR)

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <TClass.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TChain.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TH1.h>
#include <TF1.h>
#include <TPad.h>
#include <TVirtualPad.h>
#include <TString.h>
#include <TMath.h>
#include <TDatime.h>
#include <TROOT.h>
#include <TStyle.h>
#include <FairTrackParam.h>
#include <FairMCPoint.h>
#include <Rtypes.h>
#include <ncurses.h>
#include <FairMCEventHeader.h>

#include <DstEventHeader.h>
#include <CbmMCTrack.h>

#include <silicon/BmnSiliconStationSet.h>
#include <gem/BmnGemStripStationSet.h>
#include <csc/BmnCSCStationSet.h>
#include <BmnHit.h>
#include <BmnGlobalTrack.h>
//#include <BmnHist.h>
#include <physics/bmnrecotools/BmnRecoTools.h>

#else
#include "gconfig/basiclibs.C"
#include "bmnloadlibs.C"

#endif


#define PAD_WIDTH_EMB   1920
#define PAD_HEIGHT_EMB  1200
//#define PAD_WIDTH_EMB   960
//#define PAD_HEIGHT_EMB  540
#define ROWS  2
#define COLS  2
#define EV_LIMIT  4
using namespace std;
using namespace TMath;

void EmbEffView() {
#if !(!defined(__CINT__) || defined(__MAKECINT__))
    bmnloadlibs(); // load BmnRoot libraries
#endif
    gStyle->SetOptStat(0);
    const Int_t nColors = 16;
    Int_t ColorMap[nColors] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 49, 11, 12, 13, 14, 15, 16};
    Int_t runID = 4649;
    Int_t fPeriodID = 7;
    const Int_t boxWidth = 40;
    const Int_t boxHeigth = 200;
    const Int_t Bins = 400;
//    TString inFileNameEve = "eve-p-nofield.root";
    TString inFileNameBase = Form("dst-%d.root", runID); //Form("cbmdst_%d.root", runID);
//    TString inFileNameEmb = "dst-p-nofield.root"; //Form("dst_MK_%d.root", runID); //~/BMNDST_%d.root
        TString inFileNameEve = Form("eve-%d-isAll.root", runID);
    //    TString inFileNameBase = Form("dst-%d.root", runID); //Form("cbmdst_%d.root", runID);
//        TString inFileNameEmb = Form("dst-p-nofield.root", runID); //Form("dst_MK_%d.root", runID); //~/BMNDST_%d.root
//        TString inFileNameEmb = Form("dst-%d-eve-qgsm-4h-unreal.root", runID); //Form("dst_MK_%d.root", runID); //~/BMNDST_%d.root
//        TString inFileNameEmb = Form("dst-%d-emb-qgsm-4h-isAll.root", runID); //Form("dst_MK_%d.root", runID); //~/BMNDST_%d.root
        TString inFileNameEmb = Form("dst-%d-emb-qgsm-4h-isAll-BaseOff.root", runID); //Form("dst_MK_%d.root", runID); //~/BMNDST_%d.root

    TString fnames[COLS] = {inFileNameEve, inFileNameEmb};
    TString treeNames[COLS] = {"bmndata", "bmndata"}; //{"BMN_DIGIT", "cbmsim", "cbmsim"};
    TString EHNames[COLS] = {"MCEventHeader.", "DstEventHeader."};
    TString hitNames[COLS] = {"StsPoint", "BmnInnerHits"};
    TString hitNamesGEM[COLS] = {"StsPoint", "BmnGemStripHit"};
    TString hitNamesSil[COLS] = {"SiliconPoint", "BmnSiliconHit"};
    TString hitNamesCSC[COLS] = {"CSCPoint", "BmnCSCHit"};
    TString trackNames[COLS] = {"MCTrack", "BmnGlobalTrack"};
    TFile * files[COLS] = {NULL};
    TTree * trees[COLS] = {NULL};
    FairMCEventHeader * MCEH = NULL;
    DstEventHeader * EHs[COLS] = {NULL};
    TClonesArray * hits[COLS] = {NULL};
    TClonesArray * tracks[COLS] = {NULL};
    TClonesArray * silTrack[COLS] = {NULL};
    TClonesArray * gemTrack[COLS] = {NULL};
    TClonesArray * silHits[COLS] = {NULL};
    TClonesArray * cscHits[COLS] = {NULL};
    TClonesArray * gemHits[COLS] = {NULL};
    TH2D * hHitMatch[COLS][ROWS] = {NULL};
    hHitMatch[0][0] = new TH2D("hHM-XZ-MC", "Hits XZ MC", Bins, -boxWidth, boxWidth, Bins, 0, boxHeigth);
    hHitMatch[0][1] = new TH2D("hHM-YZ-MC", "Hits YZ MC", Bins, -boxWidth, boxWidth, Bins, 0, boxHeigth);
    hHitMatch[1][0] = new TH2D("hHM-XZ-Emb", "Hits XZ Emb", Bins, -boxWidth, boxWidth, Bins, 0, boxHeigth);
    hHitMatch[1][1] = new TH2D("hHM-YZ-Emb", "Hits YZ Emb", Bins, -boxWidth, boxWidth, Bins, 0, boxHeigth);
//    TH2D * h[COLS][ROWS][nColors] = {NULL};
//    for (Int_t ic = 0; ic < nColors; ic++) {
//        h[0][0][ic] = new TH2D(Form("h0-0-%d", ic), "Hits XZ MC", Bins, -boxWidth, boxWidth, Bins, 0, boxHeigth);
//        h[0][1][ic] = new TH2D(Form("h0-1-%d", ic), "Hits YZ MC", Bins, -boxWidth, boxWidth, Bins, 0, boxHeigth);
//        h[1][0][ic] = new TH2D(Form("h1-0-%d", ic), "Hits XZ Base", Bins, -boxWidth, boxWidth, Bins, 0, boxHeigth);
//        h[1][1][ic] = new TH2D(Form("h1-1-%d", ic), "Hits YZ Base", Bins, -boxWidth, boxWidth, Bins, 0, boxHeigth);
//        h[2][0][ic] = new TH2D(Form("h2-0-%d", ic), "Hits XZ Embedded", Bins, -boxWidth, boxWidth, Bins, 0, boxHeigth);
//        h[2][1][ic] = new TH2D(Form("h2-1-%d", ic), "Hits YZ Embedded", Bins, -boxWidth, boxWidth, Bins, 0, boxHeigth);
//    }
//    TCanvas* can = new TCanvas("canHits", "", PAD_WIDTH_EMB * COLS, PAD_HEIGHT_EMB * ROWS);
//    can->Divide(COLS, ROWS);
    TCanvas* can = new TCanvas("canHits", "", PAD_WIDTH_EMB * 1, PAD_HEIGHT_EMB * ROWS);
    can->Divide(1, ROWS);

    /** HitMaker efficiency test */
    /** open from xml*/
    TString xmlConfFileName = fPeriodID == 7 ? "SiliconRunSpring2018.xml" : "SiliconRunSpring2017.xml";
    xmlConfFileName = TString(getenv("VMCWORKDIR")) + "/parameters/silicon/XMLConfigs/" + xmlConfFileName;
    printf("xmlConfFileName %s\n", xmlConfFileName.Data());
    BmnSiliconStationSet* silStationSet = new BmnSiliconStationSet(xmlConfFileName);

    xmlConfFileName = fPeriodID == 7 ? "GemRunSpring2018.xml" : "GemRunSpring2017.xml";
    xmlConfFileName = TString(getenv("VMCWORKDIR")) + "/parameters/gem/XMLConfigs/" + xmlConfFileName;
    printf("xmlConfFileName %s\n", xmlConfFileName.Data());
    BmnGemStripStationSet* gemStationSet = new BmnGemStripStationSet(xmlConfFileName);

    xmlConfFileName = fPeriodID == 7 ? "CSCRunSpring2018.xml" : "CSCRunSpring2017.xml";
    xmlConfFileName = TString(getenv("VMCWORKDIR")) + "/parameters/csc/XMLConfigs/" + xmlConfFileName;
    printf("xmlConfFileName %s\n", xmlConfFileName.Data());
    BmnCSCStationSet* cscStationSet = new BmnCSCStationSet(xmlConfFileName);

    /** create hitCount vectors */
    vector<vector<vector<UInt_t > > > hitVectorSil;
    vector<vector<vector<UInt_t > > > ptVectorSil;
    vector<vector<vector<Double_t > > > effVectorSil;
    for (Int_t iStation = 0; iStation < silStationSet->GetNStations(); iStation++) {
        BmnSiliconStation* st = silStationSet->GetSiliconStation(iStation);
        vector<vector<UInt_t> > row;
        row.resize(st->GetNModules(), vector<UInt_t>(2, 0));
        hitVectorSil.push_back(row);
        ptVectorSil.push_back(row);
        vector<vector<Double_t> > rowDbl;
        rowDbl.resize(st->GetNModules(), vector<Double_t>(2, 0.0));
        effVectorSil.push_back(rowDbl);
    }

    vector<vector<vector<UInt_t > > > hitVectorGem;
    vector<vector<vector<UInt_t > > > ptVectorGem;
    vector<vector<vector<Double_t > > > effVectorGem;
    UInt_t sumMods = 0;
    UInt_t maxLayers = 0;
    vector<PadInfo*> canStripPads;
    vector<PadInfo*> canStripPads2D;
    vector<TH2D* > ptsHistGemSt;
    vector<TH2D* > effHistGemSt;
    vector<TH1D* > effHistGemStResX;
    vector<TH1D* > effHistGemStResY;
    vector<vector<vector<TH1D* > > > ptsHistGem;
    vector<vector<vector<TH1D* > > > effHistGem;
    vector<vector<vector<TH2D* > > > ptsHistGem2D;
    vector<vector<vector<TH2D* > > > effHistGem2D;
    const Int_t StBinX = 200;
    const Int_t StBinY = 100;
    const Double_t TanMaxX = 1.5;
    const Double_t TanMaxY = 0.8;
    const Double_t ResMax = 1;
    for (Int_t iStation = 0; iStation < gemStationSet->GetNStations(); iStation++) {
        BmnGemStripStation* st = gemStationSet->GetGemStation(iStation);
        sumMods += st->GetNModules();
        vector<vector<UInt_t> > row;
        row.resize(st->GetNModules(), vector<UInt_t>(2, 0));
        hitVectorGem.push_back(row);
        ptVectorGem.push_back(row);
        vector<vector<Double_t> > rowDbl;
        rowDbl.resize(st->GetNModules(), vector<Double_t>(2, 0.0));
        effVectorGem.push_back(rowDbl);
        TString n = Form("h-pts-%d", iStation);
        TH2D* hst = new TH2D(n.Data(), n.Data(),
                StBinX,
                -TanMaxX,//-st->GetXMaxStation(),
                TanMaxX,//-st->GetXMinStation(),
                StBinY,
                -TanMaxY,//,st->GetYMinStation(),
                TanMaxY//st->GetYMaxStation()
                );
        hst->GetZaxis()->SetRangeUser(0, 1.2);
        ptsHistGemSt.push_back(hst);
        
        n = Form("h-eff-%d", iStation);
        TH2D* hstEff = new TH2D(n.Data(), n.Data(),
                StBinX,
                -TanMaxX,//-st->GetXMaxStation(),
                TanMaxX,//-st->GetXMinStation(),
                StBinY,
                -TanMaxY,//,st->GetYMinStation(),
                TanMaxY//st->GetYMaxStation()
                );
        hstEff->GetZaxis()->SetRangeUser(0, 1.2);
        effHistGemSt.push_back(hstEff);
        
        n = Form("h-resX-%d", iStation);
        TH1D* hrx = new TH1D(n.Data(), n.Data(),StBinX,-ResMax,ResMax);
        effHistGemStResX.push_back(hrx);
        
        n = Form("h-resY-%d", iStation);
        TH1D* hry = new TH1D(n.Data(), n.Data(),StBinX,-ResMax,ResMax);
        effHistGemStResY.push_back(hry);
        
        vector<vector<TH1D* > > hmods;
        vector<vector<TH1D* > > hmodsEff;
        vector<vector<TH2D* > > hmods2D;
        vector<vector<TH2D* > > hmodsEff2D;
        for (Int_t iMod = 0; iMod < st->GetNModules(); iMod++) {
            BmnGemStripModule *mod = st->GetModule(iMod);
            if (maxLayers < mod->GetNStripLayers()/2)
                maxLayers = mod->GetNStripLayers()/2;
            vector<TH1D*> hzones;
            vector<TH1D*> hzonesEff;
            vector<TH2D*> hzones2D;
            vector<TH2D*> hzonesEff2D;
            for (Int_t iZone = 0; iZone < mod->GetNStripLayers() / 2; iZone++) {
                BmnGemStripLayer l = mod->GetStripLayer(2 * iZone);
                
                TString name = Form("h-pts-%d-%d-%d", iStation, iMod, iZone);
                TH2D* h2d = new TH2D(name.Data(), name.Data(),
                        l.GetNStrips()/4,
                        -l.GetXMaxLayer(),
                        -l.GetXMinLayer(),
                        l.GetNStrips()/16,
                        l.GetYMinLayer(),
                        l.GetYMaxLayer());
                h2d->GetZaxis()->SetRangeUser(0, 1.2);
                
                name = name+"-projection";
                TH1D* h = new TH1D(name.Data(), name.Data(),
                        l.GetNStrips()/4,
                        -l.GetXMaxLayer(),
                        -l.GetXMinLayer());
                h->GetYaxis()->SetRangeUser(0, 1.2);
                hzones.push_back(h);
                hzones2D.push_back(h2d);
                
                name = Form("h-eff-%d-%d-%d", iStation, iMod, iZone);
                TH2D* heff2d = new TH2D(name.Data(), name.Data(),
                        l.GetNStrips()/4,
                        -l.GetXMaxLayer(),
                        -l.GetXMinLayer(),
                        l.GetNStrips()/16,
                        l.GetYMinLayer(),
                        l.GetYMaxLayer());
                heff2d->GetZaxis()->SetRangeUser(0, 1.2);
                
                name = name+"-projection";
                TH1D* heff = new TH1D(name.Data(), name.Data(),
                        l.GetNStrips()/4,
                        -l.GetXMaxLayer(),
                        -l.GetXMinLayer());
                hzonesEff.push_back(heff);
                hzonesEff2D.push_back(heff2d);
            }
            hmods2D.push_back(hzones2D);
            hmodsEff2D.push_back(hzonesEff2D);
            hmods.push_back(hzones);
            hmodsEff.push_back(hzonesEff);
        }
        ptsHistGem2D.push_back(hmods2D);
        effHistGem2D.push_back(hmodsEff2D);
        ptsHistGem.push_back(hmods);
        effHistGem.push_back(hmodsEff);
    }
    
    TString name = "Gem_Eff_Canvas";
    TCanvas* canGemStrip = new TCanvas(name, name, PAD_WIDTH_EMB * maxLayers, PAD_HEIGHT_EMB * sumMods);
    name = "Gem_Eff_Canvas2D";
    TCanvas* canGemStrip2D = new TCanvas(name, name, PAD_WIDTH_EMB * maxLayers, PAD_HEIGHT_EMB * sumMods);
    canGemStrip->Divide(maxLayers, sumMods);
    canGemStrip2D->Divide(maxLayers, sumMods);
    canStripPads.resize(sumMods * maxLayers);
    canStripPads2D.resize(sumMods * maxLayers);
    Int_t modCtr = 0;
    for (Int_t iStation = 0; iStation < gemStationSet->GetNStations(); iStation++) {
        BmnGemStripStation * st = gemStationSet->GetGemStation(iStation);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            BmnGemStripModule *mod = st->GetModule(iModule);
            for (Int_t iZone = 0; iZone < mod->GetNStripLayers() / 2; iZone++) {
                PadInfo *p = new PadInfo();
                p->opt = "";
                p->current = effHistGem[iStation][iModule][iZone];
                PadInfo *p2D = new PadInfo();
                p2D->opt = "colz";
                p2D->current = effHistGem2D[iStation][iModule][iZone];
                Int_t iPad = modCtr * maxLayers + iZone;
                canStripPads[iPad] = p;
                canGemStrip->GetPad(iPad + 1)->SetGrid();
                canStripPads2D[iPad] = p2D;
                canGemStrip2D->GetPad(iPad + 1)->SetGrid();
            }
            modCtr++;
        }
    }

    vector<vector<vector<UInt_t > > > hitVectorCSC;
    vector<vector<vector<UInt_t > > > ptVectorCSC;
    vector<vector<vector<Double_t > > > effVectorCSC;
    for (Int_t iStation = 0; iStation < cscStationSet->GetNStations(); iStation++) {
        BmnCSCStation* st = cscStationSet->GetStation(iStation);
        vector<vector<UInt_t> > row;
        row.resize(st->GetNModules(), vector<UInt_t>(2, 0));
        hitVectorCSC.push_back(row);
        ptVectorCSC.push_back(row);
        vector<vector<Double_t> > rowDbl;
        rowDbl.resize(st->GetNModules(), vector<Double_t>(2, 0.0));
        effVectorCSC.push_back(rowDbl);
    }

    /** Open exp files */
    for (Int_t i = 0; i < COLS; i++) {
        files[i] = new TFile(fnames[i], "READ");
        if (files[i]->IsOpen())
            printf("file %s opened\n", fnames[i].Data());
        else {
            fprintf(stderr, "file %s open error\n", fnames[i].Data());
            continue;
            //            return;
        }
        trees[i] = (TTree*) files[i]->Get(treeNames[i]);
        trees[i]->SetBranchAddress(hitNames[i], &hits[i]);
        trees[i]->SetBranchAddress(trackNames[i], &tracks[i]);
        for (Int_t j = 0; j < ROWS; j++) {
            hHitMatch[i][j]->SetMarkerStyle(i > 0 ? 27 : 43);
            hHitMatch[i][j]->SetMarkerSize(i > 0 ? 3 : 3);
            hHitMatch[i][j]->SetMarkerColor(ColorMap[i]);
//            for (Int_t ic = 0; ic < nColors; ic++) {
//                h[i][j][ic]->SetMarkerStyle(ic > 0 ? 41 : 2);
//                h[i][j][ic]->SetMarkerSize(ic > 0 ? 2 : 1);
//                h[i][j][ic]->SetMarkerColor(ColorMap[ic]);
//            }
        }
        if (i) {
            trees[i]->SetBranchAddress(EHNames[i], &EHs[i]);
            trees[i]->SetBranchAddress("BmnSiliconTrack", &silTrack[i]);
            trees[i]->SetBranchAddress("BmnGemTrack", &gemTrack[i]);
            trees[i]->SetBranchAddress(hitNamesSil[i], &silHits[i]);
            trees[i]->SetBranchAddress(hitNamesCSC[i], &cscHits[i]);
            trees[i]->SetBranchAddress(hitNamesGEM[i], &gemHits[i]);
        } else {
            //        trees[i]->SetBranchAddress("BmnSiliconTrack", &silTrack[i]);
            //        trees[i]->SetBranchAddress("BmnGemTrack", &gemTrack[i]);
            trees[i]->SetBranchAddress(EHNames[i], &MCEH);
            trees[i]->SetBranchAddress(hitNamesSil[i], &silHits[i]);
            trees[i]->SetBranchAddress(hitNamesCSC[i], &cscHits[i]);
            trees[i]->SetBranchAddress(hitNamesGEM[i], &gemHits[i]);
        }
    }
    const UInt_t nEvs = trees[0]->GetEntries();

    /** iterate over events for efficiency test */
    printf("Gathering hits<->points correspondence:\n");
    Long64_t iExpEvent = -1;
    for (Int_t iEv = 0; iEv < nEvs; iEv++) {
        DrawBar(iEv, nEvs);
        trees[0]->GetEntry(iEv);
        UInt_t EventId = MCEH->GetEventID();
//                printf("iEv = %d  EventId = %u \n", iEv, EventId);
        BmnRecoTools::FillSetStsPoints(gemHits[0], gemStationSet, (vector<vector<vector<TH2* > > > &)ptsHistGem2D);
//        BmnRecoTools::FillSetStsPoints(gemHits[0], gemStationSet, (vector<TH2* > &)ptsHistGemSt);
        //        FillSetStsPoints(gemHits[0], gemStationSet, ptVectorGem);
        UInt_t i = 1;
        while (iExpEvent < 1000000) {
            //            printf("iExpEvent %lld\n", iExpEvent);
            trees[i]->GetEntry(++iExpEvent);
            //            printf("EHs[%lld]->GetEventId() = %u \n", iExpEvent, EHs[i]->GetEventId());
            if (EventId == EHs[i]->GetEventId())
                break;
            if (EventId < EHs[i]->GetEventId()) {
                printf("\n%d tree %u event lost!\n", i, EventId);
                break;
            }
        }
        BmnRecoTools::FillSetStsHits(gemHits[0], gemHits[i], gemStationSet, (vector<vector<vector<TH2* > > > &)effHistGem2D);
//        BmnRecoTools::FillSetStsHits(gemHits[0], gemHits[i], gemStationSet, (vector<TH2* > &)effHistGemSt,
//                effHistGemStResX,effHistGemStResY);
        //        FillSetStsHits(gemHits[0], gemHits[i], gemStationSet, hitVectorGem);
        //        FillSetHits(gemHits[i], hitVectorGem);
        //        FillSetHits(silHits[i], hitVectorSil);
        //        FillSetHits(cscHits[i], hitVectorCSC);

    }

    //    printf("\nHit Maker Efficiency\n");
    //    for (Int_t iStation = 0; iStation < gemStationSet->GetNStations(); iStation++) {
    //        printf("GEM Station = %d\n", iStation);
    //        BmnGemStripStation* st = gemStationSet->GetGemStation(iStation);
    //        for (Int_t iMod = 0; iMod < st->GetNModules(); iMod++) {
    //            BmnGemStripModule *mod = st->GetModule(iMod);
    //            for (Int_t iZone = 0; iZone < mod->GetNStripLayers() / 2; iZone++) {
    //                if (ptVectorGem[iStation][iMod][iZone] > 0)
    //                    effVectorGem[iStation][iMod][iZone] =
    //                        hitVectorGem[iStation][iMod][iZone] / (Double_t) ptVectorGem[iStation][iMod][iZone];
    //                printf("\tiMod = %d, Zone = %d  Eff = %f\n", iMod, iZone, effVectorGem[iStation][iMod][iZone]);
    //            }
    //        }
    //    }
    printf("\nHit Maker Efficiency   BigZone   HotZone \n");
    for (Int_t iStation = 0; iStation < gemStationSet->GetNStations(); iStation++) {
        printf("GEM Station = %d\n", iStation);
        BmnGemStripStation* st = gemStationSet->GetGemStation(iStation);
        effHistGemSt[iStation]->Divide(ptsHistGemSt[iStation]);
        TCanvas* cc = new TCanvas("temp", "temp", 1800, 900);
        effHistGemSt[iStation]->Draw("colz");
        cc->SaveAs(Form("eff-lam-corr-station_%d.png", iStation));
        delete cc;
//        TCanvas* cr = new TCanvas("temp", "temp", 1800, 900);
//        cr->Divide(2,1);
//        cr->cd(1);
//        effHistGemStResX[iStation]->Draw("");
//        cr->cd(2);
//        effHistGemStResY[iStation]->Draw("");
//        TString residName = "eff-lam-tan-resid.pdf";
//        if (iStation< gemStationSet->GetNStations() - 1){
//            if (iStation == 0)
//                residName+="(";
//            else
//                ;
//        }
//        else
//            residName+=")";
//            cr->Print(residName.Data());
//        delete cr;
        for (Int_t iMod = 0; iMod < st->GetNModules(); iMod++) {
            BmnGemStripModule *mod = st->GetModule(iMod);
            printf("\tiMod = %d\n", iMod);
            for (Int_t iZone = 0; iZone < mod->GetNStripLayers() / 2; iZone++) {
//                printf("\tiZone = %d\n", iZone);
                Double_t ptInt = ptsHistGem2D[iStation][iMod][iZone]->Integral();
                Double_t eff = 0.0;
                if (ptInt > 0)
                    eff = effHistGem2D[iStation][iMod][iZone]->Integral()/ptInt;
                if (iZone != 0) printf("  &  ");
                printf(" %f",eff);
//                effHistGem[iStation][iMod][iZone] = effHistGem2D[iStation][iMod][iZone]->ProjectionX();
//                ptsHistGem[iStation][iMod][iZone] = ptsHistGem2D[iStation][iMod][iZone]->ProjectionX();
                effHistGem[iStation][iMod][iZone]->Divide(ptsHistGem[iStation][iMod][iZone]);
            }
            printf("\n");
        }
    }
//    BmnHist::DrawRef(canGemStrip, &canStripPads);
//    BmnHist::DrawRef(canGemStrip2D, &canStripPads2D);
////    canGemStrip->Print("eff-p.pdf");
////    canGemStrip->Print("eff-p.eps");
//    canGemStrip->Print("eff-p.png");
//    canGemStrip2D->Print("eff-lam-2d.png");

    //    printf("MC Points\n");
    //    for (Int_t iStation = 0; iStation < gemStationSet->GetNStations(); iStation++) {
    //        printf("GEM Station = %d\n", iStation);
    //        BmnGemStripStation* st = gemStationSet->GetGemStation(iStation);
    //        for (Int_t iMod = 0; iMod < st->GetNModules(); iMod++) {
    //            BmnGemStripModule *mod = st->GetModule(iMod);
    //            for (Int_t iZone = 0; iZone < mod->GetNStripLayers() / 2; iZone++) {
    //                printf("\tiMod = %d, Zone = %d  nPoints = %d\n", iMod, iZone, ptVectorGem[iStation][iMod][iZone]);
    //            }
    //        }
    //    }
    //    printf("Exp Hits\n");
    //    for (Int_t iStation = 0; iStation < gemStationSet->GetNStations(); iStation++) {
    //        BmnGemStripStation* st = gemStationSet->GetGemStation(iStation);
    //        printf("GEM Station = %d\n", iStation);
    //        for (Int_t iMod = 0; iMod < st->GetNModules(); iMod++) {
    //            BmnGemStripModule *mod = st->GetModule(iMod);
    //            for (Int_t iZone = 0; iZone < mod->GetNStripLayers() / 2; iZone++) {
    //                printf("\tiMod = %d, Zone = %d  nHits = %u\n", iMod, iZone, hitVectorGem[iStation][iMod][iZone]);
    //            }
    //        }
    //    }


    //        for (Int_t iStation = 0; iStation < silStationSet->GetNStations(); iStation++) {
    //            BmnSiliconStation* st = silStationSet->GetStation(iStation);
    //            printf("Sil Station = %d\n", iStation);
    //            for (Int_t iMod = 0; iMod < st->GetNModules(); iMod++) {
    //                printf("\tiMod = %d, nHits = %u\n", iMod, hitVectorSil[iStation][iMod]);
    //            }
    //        }
    //        for (Int_t iStation = 0; iStation < cscStationSet->GetNStations(); iStation++) {
    //            BmnCSCStation* st = cscStationSet->GetStation(iStation);
    //            printf("CSC Station = %d\n", iStation);
    //            for (Int_t iMod = 0; iMod < st->GetNModules(); iMod++) {
    //                printf("\tiMod = %d, nHits = %u\n", iMod, hitVectorCSC[iStation][iMod]);
    //            }
    //        }
    return;


    /** iterate interactively over events in 3 files */
    cout << "#recorded entries = " << nEvs << " in " << fnames[0] << endl;
    Int_t ch = 0;
    Int_t iEv = 0;
    do {
        ch = getchar();
        switch (ch) {
            case 'a':
                iEv = (iEv == 0) ? iEv : iEv - 1;
                break;
            case 'd':
                iEv = (iEv < nEvs) ? iEv + 1 : iEv;
                break;
            default:
                //                iEv++;
                break;
        }
        if (ch == '\n')
            continue;
        Int_t hitCount[COLS] = {0};
        trees[0]->GetEntry(iEv);
        UInt_t EventId = MCEH->GetEventID();
        printf("iEv = %d  EventId = %u \n", iEv, EventId);
        
        for (Int_t i = 1; i < COLS; i++) {
            trees[i]->GetEntry(iEv);
//            Long64_t iExpEvent = -1;
//            while (iExpEvent < 1000000) {
//                //                printf("iExpEvent %lld\n", iExpEvent);
//                trees[i]->GetEntry(++iExpEvent);
//                //                printf("trees[i]->GetEntry(iExpEvent) %d\n", trees[i]->GetEntry(iExpEvent));
//                //                    printf("check %lld \n", iExpEvent);
//                if (EventId == EHs[i]->GetEventId())
//                    break;
//                if (EventId < EHs[i]->GetEventId()) {
//                    printf("\n%d tree %u event lost!\n", i, EventId);
//                    break;
//                }
//            }
        }
        for (Int_t i = 0; i < COLS; i++) {
            for (Int_t row = 0; row < ROWS; row++){
                hHitMatch[i][row]->Reset();
//                for (Int_t ic = 0; ic < nColors; ic++)
//                    h[i][row][ic]->Reset();
            }
            if (i){//printf("%d th tree hits %d\n", i, gemHits[i]->GetEntriesFast());
                for (Int_t iHit = 0; iHit < gemHits[i]->GetEntriesFast(); iHit++) {
                    BmnHit *hit = (BmnHit *) gemHits[i]->UncheckedAt(iHit);
//                    h[i][0][0]->Fill(hit->GetX(), hit->GetZ());
//                    h[i][1][0]->Fill(hit->GetY(), hit->GetZ());
                    hHitMatch[i][0]->Fill(hit->GetX(), hit->GetZ());
                    hHitMatch[i][1]->Fill(hit->GetY(), hit->GetZ());
                }
            } else{//printf("%d th tree pts  %d\n", i, gemHits[i]->GetEntriesFast());
                for (Int_t iHit = 0; iHit < gemHits[i]->GetEntriesFast(); iHit++) {
                    FairMCPoint *hit = (FairMCPoint *) gemHits[i]->UncheckedAt(iHit);
                    hHitMatch[i][0]->Fill(hit->GetX(), hit->GetZ());
                    hHitMatch[i][1]->Fill(hit->GetY(), hit->GetZ());
                }
            }
                
//                    
//            //                                hitCount[i] += hits[i]->GetEntriesFast();
//
//            //            if (tracks[1]->GetEntriesFast() < 2) {
//            //                iEv++;
//            //                continue;
//            //            }
//            printf("%d branch %d tracks\n", i, tracks[i]->GetEntriesFast());
//            if (i > 0) {
//                for (Int_t iTrack = 0; iTrack < tracks[i]->GetEntriesFast(); iTrack++) {
//                    if (iTrack > nColors - 2) {
//                        printf("tracks overfull %d\n", iTrack);
//                        continue;
//                    }
//                    BmnGlobalTrack* track = (BmnGlobalTrack*) tracks[i]->UncheckedAt(iTrack);
//                    if (track->GetSilTrackIndex() != -1) {
//                        BmnTrack * silTr = (BmnTrack*) silTrack[i]->UncheckedAt(track->GetSilTrackIndex());
//                        //                        if (silTr->GetNHits() < 4)
//                        //                            continue;
//                        hitCount[i] += silTr->GetNHits();
//                        for (Int_t iHit = 0; iHit < silTr->GetNHits(); iHit++) {
//                            BmnHit *hit = (BmnHit *) silHits[i]->UncheckedAt(silTr->GetHitIndex(iHit));
//                            h[i][0][iTrack + 1]->Fill(hit->GetX(), hit->GetZ());
//                            h[i][1][iTrack + 1]->Fill(hit->GetY(), hit->GetZ());
//                        }
//                    }
//                    if (track->GetGemTrackIndex() != -1) {
//                        BmnTrack * gemTr = (BmnTrack*) gemTrack[i]->UncheckedAt(track->GetGemTrackIndex());
//                        //                        if (gemTr->GetNHits() < 4)
//                        //                            continue;
//                        hitCount[i] += gemTr->GetNHits();
//                        for (Int_t iHit = 0; iHit < gemTr->GetNHits(); iHit++) {
//                            BmnHit *hit = (BmnHit *) gemHits[i]->UncheckedAt(gemTr->GetHitIndex(iHit));
//                            h[i][0][iTrack + 1]->Fill(hit->GetX(), hit->GetZ());
//                            h[i][1][iTrack + 1]->Fill(hit->GetY(), hit->GetZ());
//                        }
//                    }
//                    //                    FairTrackParam* first = track->GetParamFirst();
//                    //                    h[i][2][iTrack]->Fill(first->GetTx());
//                    //                    h[i][3][iTrack]->Fill(first->GetTy());
//                }
//            } else {
//                for (Int_t iTrack = 0; iTrack < tracks[i]->GetEntriesFast(); iTrack++) {
//                    CbmMCTrack* track = (CbmMCTrack*) tracks[i]->UncheckedAt(iTrack);
//                    //                    hitCount[i] += track->GetNPoints(kGEM);
//                    for (Int_t iPoint = 0; iPoint < gemHits[i]->GetEntriesFast(); iPoint++) {
//                        FairMCPoint* hit = (FairMCPoint*) gemHits[i]->UncheckedAt(iPoint);
//                        Int_t TrackID = hit->GetTrackID();
//                        if (TrackID != iTrack)
//                            continue;
//                        h[i][0][iTrack + 1]->Fill(hit->GetX(), hit->GetZ());
//                        h[i][1][iTrack + 1]->Fill(hit->GetY(), hit->GetZ());
//                    }
//                    for (Int_t iPoint = 0; iPoint < silHits[i]->GetEntriesFast(); iPoint++) {
//                        FairMCPoint* hit = (FairMCPoint*) silHits[i]->UncheckedAt(iPoint);
//                        Int_t TrackID = hit->GetTrackID();
//                        if (TrackID != iTrack)
//                            continue;
//                        h[i][0][iTrack + 1]->Fill(hit->GetX(), hit->GetZ());
//                        h[i][1][iTrack + 1]->Fill(hit->GetY(), hit->GetZ());
//                    }
//                    //                    FairTrackParam* first = track->GetParamFirst();
//                    //                    h[i][2][iTrack]->Fill(first->GetTx());
//                    //                    h[i][3][iTrack]->Fill(first->GetTy());
//
//                }
//            }
        }
        //        if (/*hitCount[0] < EV_LIMIT || */hitCount[1] < EV_LIMIT || hitCount[2] < EV_LIMIT) {
        //            iEv++;
        //            continue;
        //        }
//        for (Int_t col = 0; col < COLS; col++)
//            for (Int_t row = 0; row < ROWS; row++) {
//                TVirtualPad *pad = can->cd(row * COLS + col + 1);
//                pad->Clear();
//                for (Int_t iTrack = 0; iTrack < nColors; iTrack++)
//                    if (iTrack)
//                        h[col][row][iTrack]->Draw("SAME");
//                    else
//                        h[col][row][iTrack]->Draw("");
//            }
        for (Int_t row = 0; row < ROWS; row++) {
            TVirtualPad *pad = can->cd(row + 1);
            pad->Clear();
            for (Int_t col = 0; col < COLS; col++){
                if (col)
                    hHitMatch[col][row]->Draw("SAME");
                else
                    hHitMatch[col][row]->Draw("");
                
            }
        }
        can->Update();
        can->Modified();
        can->SaveAs(Form("can-%d.pdf", runID));
    } while (ch != 'q');


    for (Int_t i = 0; i < COLS; i++) {
        for (Int_t j = 0; j < ROWS; j++) {
            delete hHitMatch[i][j];
        }
    }

    return;
}

