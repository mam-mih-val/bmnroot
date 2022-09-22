
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
#include <Fit/FitResult.h>
#include <FairTrackParam.h>
#include <Rtypes.h>
#include <TVirtualPad.h>
#include <BmnEventHeader.h>
#include "BmnHist.h"
#include "BmnSiliconDigit.h"
#include "BmnSiliconStationSet.h"
#include "BmnGemStripDigit.h"
//#include "BmnGemStripStation.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripModule.h"
#include "BmnCSCDigit.h"
#include "BmnCSCStationSet.h"
#include "BmnCSCStation.h"
#include "BmnCSCModule.h"

#else
#include "gconfig/basiclibs.C"
#include "bmnloadlibs.C"

#endif


#define PAD_WIDTH_SIL   1920
#define PAD_HEIGHT_SIL  1280
//#define ROWS  2
#define COLS  2
#define EV_LIMIT  4
using namespace std;
using namespace TMath;

void DrawRef(TCanvas *canGemStrip, vector<PadInfo*> *canGemStripPads) {
    Double_t maxy;
    Double_t k = 1;
    for (Int_t iPad = 0; iPad < canGemStripPads->size(); iPad++) {
        TVirtualPad *pad = canGemStrip->cd(iPad + 1);
        pad->Clear();
        //        pad->SetLogy();
        PadInfo* info = canGemStripPads->at(iPad);
        if (!info) continue;
        if (info->current) {
            maxy = info->current->GetBinContent(info->current->GetMaximumBin());
            if (info->ref != NULL) {
                //                k = (info->ref->Integral() > 0) ?
                //                        info->current->Integral() /
                //                        (Double_t) info->ref->Integral() : 1;
                //                if (k == 0) k = 1;
                //                if (info->ref->Integral() > 0)
                k = info->ref->GetBinContent(info->ref->GetMaximumBin());
                //                if (maxy < k) {
                //                    maxy = k;
                //                    info->ref->Draw(info->opt.Data());
                //                    info->current->Draw("same hist"); //, info->current->Integral());
                //                } else {
                //                    info->current->Draw(info->opt.Data());
                //                    info->ref->Draw("same hist"); //, info->current->Integral());
                //
                //                }
            }
            //                    info->current->Draw(info->opt.Data());
            //                    info->ref->Draw("same hist"); //, info->current->Integral());
            info->ref->Draw(info->opt.Data());
            info->current->Draw("same hist"); //, info->current->Integral());
            //            info->current->GetYaxis()->SetLimits(0, floor(maxy * 1.2));
            //            info->ref->GetYaxis()->SetLimits(0, floor(maxy * 1.2));
            //            
            //            
            //            info->current->GetYaxis()->SetLimits(0, floor(maxy * 1.2));
            //            info->ref->GetYaxis()->SetLimits(0, floor(maxy * 1.2));

        }
        pad->Update();
        pad->Modified();
    }
    canGemStrip->Update();
    canGemStrip->Modified();
}

void StripView(Int_t runID) {
#if !(!defined(__CINT__) || defined(__MAKECINT__))
    bmnloadlibs(); // load BmnRoot libraries
#endif
    gStyle->SetOptStat(0);
    const Int_t nColors = 16;
    Int_t ColorMap[nColors] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 49, 11, 12, 13, 14, 15, 16};
    //    Int_t runID = 4600;
    Int_t fPeriodID = 7;
    UInt_t sumMods = 0;
    UInt_t maxLayers = 0;
    BmnSetup fBmnSetup = kBMNSETUP;
    TString inFileNameMK = Form("MK_digi_%04i.root", runID);
    TString inFileNameBmn = Form("bmn_run%04i_digi.root", runID);

    TString fnames[COLS] = {inFileNameMK, inFileNameBmn};
    TString treeNames[COLS] = {"bmndata", "bmndata"};
    TString silNames[COLS] = {"SILICON", "SILICON"};
    TString gemNames[COLS] = {"GEM", "GEM"};
    TString cscNames[COLS] = {"CSC", "CSC"};
    TFile * files[COLS] = {NULL};
    TTree * trees[COLS] = {NULL};
    TClonesArray * hits[COLS] = {NULL};
    TClonesArray * tracks[COLS] = {NULL};
    TClonesArray * silDigit[COLS] = {NULL, NULL};
    TClonesArray * gemDigit[COLS] = {NULL, NULL};
    TClonesArray * cscDigit[COLS] = {NULL, NULL};
    vector<vector<vector<TH1F* > > > histStrip[COLS];
    vector<vector<vector<TH1F* > > > histStripGem[COLS];
    vector<vector<vector<TH1F* > > > histStripCsc[COLS];
    TCanvas *canStrip;
    TCanvas *canStripGem;
    TCanvas *canStripCsc;
    vector<PadInfo*> canStripPads;
    vector<PadInfo*> canStripPadsGem;
    vector<PadInfo*> canStripPadsCsc;

    Int_t hf = 2400;
    const Int_t kNStrips = 640;
    TCanvas *canProf = new TCanvas("canprof", "can", 1920, 1920);
    canProf->Divide(1, 4);
    TH2F* hfilter = new TH2F("hfilter", "hfilter", kNStrips + 1, 0, kNStrips, hf + 1, 0, hf);
    TH2F* hfilterMK = new TH2F("hfilterMK", "hfilterMK", kNStrips + 1, 0, kNStrips, hf + 1, 0, hf);
    TH1F* hsig = new TH1F("hsig", "hsig", hf + 1, 0, hf);
    hsig->SetLineColor(kRed);
    TH1F* hsigMK = new TH1F("hsigMK", "hsigMK", hf + 1, 0, hf);

    TString name;
    
    // ********************
    // silicon pads
    // ********************
    sumMods = 0;
    maxLayers = 0;
    TString xmlConfFileName = fPeriodID == 7 ? "SiliconRunSpring2018.xml" : "SiliconRunSpring2017.xml";
    xmlConfFileName = TString(getenv("VMCWORKDIR")) + "/parameters/silicon/XMLConfigs/" + xmlConfFileName;
    printf("xmlConfFileName %s\n", xmlConfFileName.Data());
    BmnSiliconStationSet* stationSet = new BmnSiliconStationSet(xmlConfFileName);
    //    printf("xmlConf stations %i\n", stationSet->GetNStations());
    for (Int_t iCol = 0; iCol < COLS; iCol++) {
        for (Int_t iStation = 0; iStation < stationSet->GetNStations(); iStation++) {
            vector<vector<TH1F*> > rowGEM;
            BmnSiliconStation* st = stationSet->GetSiliconStation(iStation);
            sumMods += st->GetNModules();
            for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
                vector<TH1F*> colGEM;
                BmnSiliconModule *mod = st->GetModule(iModule);
                //                printf("iStation %i iModule %i nlayers %i sumMods %i\n", iStation, iModule, mod->GetNStripLayers(), sumMods);
                //                if (maxLayers < mod->GetNStripLayers())
                maxLayers = 2;
                for (Int_t iLayer = 0; iLayer < 2; iLayer++) {
                    BmnSiliconLayer lay = mod->GetStripLayer(iLayer);
                    name = Form("Silicon_%d_Station_%d_module_%d_layer_%d", iCol, iStation, iModule, iLayer);
                    TH1F *h = new TH1F(name, name, lay.GetNStrips(), 0, lay.GetNStrips());
                    h->SetTitleSize(0.06, "XY");
                    h->SetLabelSize(0.08, "XY");
                    h->GetXaxis()->SetTitle("Strip Number");
                    h->GetXaxis()->SetTitleColor(kOrange + 10);
                    h->GetYaxis()->SetTitle("Activation Count");
                    h->GetYaxis()->SetTitleColor(kOrange + 10);
                    colGEM.push_back(h);
                }
                rowGEM.push_back(colGEM);

            }
            histStrip[iCol].push_back(rowGEM);

        }
    }
    sumMods = sumMods / COLS;
    name = "SilCanvas";
    canStrip = new TCanvas(name, name, PAD_WIDTH_SIL * maxLayers, PAD_HEIGHT_SIL * sumMods);
    canStrip->Divide(maxLayers, sumMods);
    Int_t modCtr = 0;
    canStripPads.resize(sumMods * maxLayers);
    for (auto &pad : canStripPads) {
        pad = nullptr;
    }
    for (Int_t iStation = 0; iStation < stationSet->GetNStations(); iStation++) {
        BmnSiliconStation * st = stationSet->GetSiliconStation(iStation);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            BmnSiliconModule *mod = st->GetModule(iModule);
            for (Int_t iLayer = 0; iLayer < 2; iLayer++) {
                Int_t iPad = modCtr * maxLayers + iLayer;
                PadInfo *p = new PadInfo();
                p->current = histStrip[0][iStation][iModule][iLayer];
                p->ref = histStrip[1][iStation][iModule][iLayer];
                p->ref->SetLineColor(kRed);
                canStripPads[iPad] = p;
                canStrip->GetPad(iPad + 1)->SetGrid();
            }
            modCtr++;
        }
    }
    // ********************
    // gem pads
    // ********************
    sumMods = 0;
    maxLayers = 0;
    xmlConfFileName = fPeriodID == 7 ? "GemRunSpring2018.xml" : "GemRunSpring2017.xml";
    xmlConfFileName = TString(getenv("VMCWORKDIR")) + "/parameters/gem/XMLConfigs/" + xmlConfFileName;
    printf("xmlConfFileName %s\n", xmlConfFileName.Data());
    BmnGemStripStationSet *gemStationSet = new BmnGemStripStationSet(xmlConfFileName);
    for (Int_t iCol = 0; iCol < COLS; iCol++) {
        for (Int_t iStation = 0; iStation < gemStationSet->GetNStations(); iStation++) {
            vector<vector<TH1F*> > rowGEM;
            BmnGemStripStation * st = gemStationSet->GetGemStation(iStation);
            sumMods += st->GetNModules();
            for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
                vector<TH1F*> colGEM;
                BmnGemStripModule *mod = st->GetModule(iModule);
                if (maxLayers < mod->GetNStripLayers())
                    maxLayers = mod->GetNStripLayers();
                for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                    BmnGemStripLayer lay = mod->GetStripLayer(iLayer);
                    name = Form("GEM_%i_Station_%d_module_%d_layer_%d", iCol, iStation, iModule, iLayer);
                    TH1F *h = new TH1F(name, name, lay.GetNStrips(), 0, lay.GetNStrips());
                    h->SetTitleSize(0.06, "XY");
                    h->SetLabelSize(0.08, "XY");
                    h->GetXaxis()->SetTitle("Strip Number");
                    h->GetXaxis()->SetTitleColor(kOrange + 10);
                    h->GetYaxis()->SetTitle("Activation Count");
                    h->GetYaxis()->SetTitleColor(kOrange + 10);
                    h->GetYaxis()->SetTitleOffset(1.0);
                    colGEM.push_back(h);

                }
                rowGEM.push_back(colGEM);

            }
            histStripGem[iCol].push_back(rowGEM);

        }
    }
    name = "GemCanvas";
    canStripGem = new TCanvas(name, name, PAD_WIDTH_SIL * maxLayers, PAD_HEIGHT_SIL * sumMods);
    canStripGem->Divide(maxLayers, sumMods);
    modCtr = 0;
    canStripPadsGem.resize(sumMods * maxLayers);
    for (Int_t iStation = 0; iStation < gemStationSet->GetNStations(); iStation++) {
        BmnGemStripStation * st = gemStationSet->GetGemStation(iStation);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            BmnGemStripModule *mod = st->GetModule(iModule);
            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                PadInfo *p = new PadInfo();
                p->opt = "";
                p->current = histStripGem[0][iStation][iModule][iLayer];
                p->ref = histStripGem[1][iStation][iModule][iLayer];
                p->ref->SetLineColor(kRed);
                Int_t iPad = modCtr * maxLayers + iLayer;
                canStripPadsGem[iPad] = p;
                canStripGem->GetPad(iPad + 1)->SetGrid();
            }
            modCtr++;
        }
    }
    // ********************
    // csc pads
    // ********************
    sumMods = 0;
    maxLayers = 0;
    xmlConfFileName = "CSCRunSpring2018.xml";
    xmlConfFileName = TString(getenv("VMCWORKDIR")) + "/parameters/csc/XMLConfigs/" + xmlConfFileName;
    printf("xmlConfFileName %s\n", xmlConfFileName.Data());
    BmnCSCStationSet *StationSet = new BmnCSCStationSet(xmlConfFileName);
    for (Int_t iCol = 0; iCol < COLS; iCol++) {
        for (Int_t iStation = 0; iStation < StationSet->GetNStations(); iStation++) {
            vector<vector<TH1F*> > rowGEM;
            BmnCSCStation * st = StationSet->GetStation(iStation);
            sumMods += st->GetNModules();
            for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
                vector<TH1F*> colGEM;
                BmnCSCModule *mod = st->GetModule(iModule);
                if (maxLayers < mod->GetNStripLayers())
                    maxLayers = mod->GetNStripLayers();
                for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                    BmnCSCLayer lay = mod->GetStripLayer(iLayer);
                    name = Form("CSC_%i_Station_%d_module_%d_layer_%d", iCol, iStation, iModule, iLayer);
                    TH1F *h = new TH1F(name, name, lay.GetNStrips(), 0, lay.GetNStrips());
                    h->SetTitleSize(0.06, "XY");
                    h->SetLabelSize(0.08, "XY");
                    h->GetXaxis()->SetTitle("Strip Number");
                    h->GetXaxis()->SetTitleColor(kOrange + 10);
                    h->GetYaxis()->SetTitle("Activation Count");
                    h->GetYaxis()->SetTitleColor(kOrange + 10);
                    h->GetYaxis()->SetTitleOffset(1.0);
                    colGEM.push_back(h);

                }
                rowGEM.push_back(colGEM);

            }
            histStripCsc[iCol].push_back(rowGEM);

        }
    }
    name = "CscCanvas";
    canStripCsc = new TCanvas(name, name, PAD_WIDTH_SIL * maxLayers, PAD_HEIGHT_SIL * sumMods);
    canStripCsc->Divide(maxLayers, sumMods);
    modCtr = 0;
    canStripPadsCsc.resize(sumMods * maxLayers);
    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); iStation++) {
        BmnCSCStation * st = StationSet->GetStation(iStation);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            BmnCSCModule *mod = st->GetModule(iModule);
            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                PadInfo *p = new PadInfo();
                p->opt = "";
                p->current = histStripCsc[0][iStation][iModule][iLayer];
                p->ref = histStripCsc[1][iStation][iModule][iLayer];
                p->ref->SetLineColor(kRed);
                Int_t iPad = modCtr * maxLayers + iLayer;
                canStripPadsCsc[iPad] = p;
                canStripCsc->GetPad(iPad + 1)->SetGrid();
            }
            modCtr++;
        }
    }


    for (Int_t i = 0; i < COLS; i++) {
        files[i] = new TFile(fnames[i], "READ");
        if (files[i]->IsOpen())
            printf("file %s opened\n", fnames[i].Data());
        else {
            fprintf(stderr, "file %s open error\n", fnames[i].Data());
            return;
        }
        trees[i] = (TTree*) files[i]->Get(treeNames[i]);
        cout << "#recorded entries = " << trees[i]->GetEntries() << endl;
        trees[i]->SetBranchAddress(silNames[i], &silDigit[i]);
        trees[i]->SetBranchAddress(gemNames[i], &gemDigit[i]);
        trees[i]->SetBranchAddress(cscNames[i], &cscDigit[i]);
    }
    for (Int_t i = 0; i < COLS; i++) {
        cout << "tree # " << i << endl;
        UInt_t nEvs = trees[i]->GetEntries();
        for (Int_t iEv = 0; iEv < 10000/*nEvs/*11000/*nEvs*/; iEv++) {
            trees[i]->GetEntry(iEv);
            if (iEv % 10000 == 0)
                cout << "iEv = " << iEv << endl;
            // silicon
            if (silDigit[i])
            for (Int_t iDig = 0; iDig < silDigit[i]->GetEntriesFast(); iDig++) {
                BmnSiliconDigit *dig = (BmnSiliconDigit*) silDigit[i]->UncheckedAt(iDig);
                if (!(dig->IsGoodDigit()))
                    continue;
                Int_t module = dig->GetModule();
                Int_t station = dig->GetStation();
                Int_t layer = dig->GetStripLayer();
                Int_t strip = dig->GetStripNumber();
                histStrip[i][station][module][layer]->Fill(strip);
                if (station == 0 && module == 0 && layer == 0)
                    if (i == 0) {
                        hfilterMK->Fill(strip, dig->GetStripSignal());
                        if (dig->GetStripNumber() > 511)
                            hsigMK->Fill(dig->GetStripSignal());
                    } else {
                        hfilter->Fill(strip, dig->GetStripSignal());

                        if (dig->GetStripNumber() > 511)
                            hsig->Fill(dig->GetStripSignal());
                    }
            }
            // gem
            if (gemDigit[i])
            for (Int_t iDig = 0; iDig < gemDigit[i]->GetEntriesFast(); iDig++) {
                BmnGemStripDigit *dig = (BmnGemStripDigit*) gemDigit[i]->UncheckedAt(iDig);
                if (!(dig->IsGoodDigit()))
                    continue;
                Int_t module = dig->GetModule();
                Int_t station = dig->GetStation();
                Int_t layer = dig->GetStripLayer();
                Int_t strip = dig->GetStripNumber();
                histStripGem[i][station][module][layer]->Fill(strip);
            }
            // csc
            if (cscDigit[i])
            for (Int_t iDig = 0; iDig < cscDigit[i]->GetEntriesFast(); iDig++) {
                BmnCSCDigit *dig = (BmnCSCDigit*) cscDigit[i]->UncheckedAt(iDig);
                if (!(dig->IsGoodDigit()))
                    continue;
                Int_t module = dig->GetModule();
                Int_t station = dig->GetStation();
                Int_t layer = dig->GetStripLayer();
                Int_t strip = dig->GetStripNumber();
                histStripCsc[i][station][module][layer]->Fill(strip);
            }
        }
    }

    //    for (Int_t col = 0; col < maxLayers; col++)
    //        for (Int_t row = 0; row < sumMods; row++) {
    //            TVirtualPad *pad = canStrip->cd(row * maxLayers + col + 1);
    //            pad->Clear();
    //            //                if (row > 2)
    //            //                    h[col][row]->Draw();
    //            //                else
    //            TString s = Form("%s.>>%s_X_Vertex(200, -10, 10)", names[col].Data()), "PrimaryVertex.fNTracks>1", "");
    //            for (Int_t iTrack = 0; iTrack < nColors; iTrack++)
    //                if (iTrack)
    //                    h[col][row][iTrack]->Draw("SAME");
    //                else
    //                    h[col][row][iTrack]->Draw("");
    //        }

    DrawRef(canStrip, &canStripPads);
    canStrip->SaveAs(Form("can-run-%d.png", runID));
    DrawRef(canStripGem, &canStripPadsGem);
    canStripGem->SaveAs(Form("can-run-gem-%d.png", runID));
    DrawRef(canStripCsc, &canStripPadsCsc);
    canStripCsc->SaveAs(Form("can-run-csc-%d.png", runID));
    canProf->cd(1);
    hfilter->Draw("colz");
    canProf->cd(2);
    hfilterMK->Draw("colz");
    canProf->cd(3);
    hsig->Draw("");
    //    canProf->cd(4);
    hsigMK->Draw("same");
    canProf->SaveAs("can-prof.png");
    //    canStrip->SaveAs(Form("can-run-%d.eps", runID));
    //    canStrip->SaveAs(Form("can-run-%d.pdf", runID));
    //    Int_t q = getchar();
    //    printf("%i\n", q);
    //    do {
    //        if (hitCount[0] < EV_LIMIT || hitCount[1] < EV_LIMIT || hitCount[2] < EV_LIMIT) {
    //            iEv++;
    //            continue;
    //        }
    //        for (Int_t col = 0; col < COLS; col++)
    //            for (Int_t row = 0; row < ROWS; row++) {
    //                TVirtualPad *pad = can->cd(row * 3 + col + 1);
    //                pad->Clear();
    //                //                if (row > 2)
    //                //                    h[col][row]->Draw();
    //                //                else
    //                for (Int_t iTrack = 0; iTrack < nColors; iTrack++)
    //                    if (iTrack)
    //                        h[col][row][iTrack]->Draw("SAME");
    //                    else
    //                        h[col][row][iTrack]->Draw("");
    //            }
    //        can->Update();
    //        can->Modified();
    //        can->SaveAs(Form("can-%d.pdf", runID));
    //        cout << " Ev# " << iEv << endl;
    //        ch = getchar();
    //        switch (ch) {
    //            case 'a':
    //                iEv = (iEv == 0) ? iEv : iEv - 1;
    //                break;
    //            case 'd':
    //                iEv = (iEv < nEvs) ? iEv + 1 : iEv;
    //                break;
    //            default:
    //                iEv++;
    //                break;
    //        }
    //    } while (ch != 'q');


    // return;
}
