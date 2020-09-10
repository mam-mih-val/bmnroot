
#include <root/TDirectory.h>
#include <root/TClass.h>

/**
 * Macro for decoders comparison
 */
R__ADD_INCLUDE_PATH($VMCWORKDIR)

#define PAD_WIDTH_VIEW   1280
#define PAD_HEIGHT_VIEW  640
#define ROWS  5
#define COLS  2
#define EV_LIMIT  4
using namespace std;
using namespace TMath;
const Int_t EtaNBins = 500;
const Int_t EtaMax = 5;
const Int_t PNBins = 500;
const Int_t PMax = 5;
const Int_t HitsNBins = 21;
const Int_t HitsMax = 20;
const Int_t Lims[ROWS][3] = {
    {EtaNBins, -EtaMax, EtaMax},
    {PNBins, -PMax, PMax},
    {PNBins, -PMax, PMax},
    {PNBins, -PMax, PMax},
    {HitsNBins, 0, HitsMax}
};
const Int_t nParticles = 2; // p, \pi-
const Int_t nDetTypes = 3; // Sil, GEM, CSC
const Int_t nCases = 3; // Background, MC Lambdas, Sum
// StsPoint - GEM

void DrawRef(TCanvas *canGemStrip, vector<PadInfo*> *canGemStripPads) {
    Double_t maxy;
    Double_t k = 1;
    for (Int_t iPad = 0; iPad < canGemStripPads->size(); iPad++) {
        TVirtualPad *pad = canGemStrip->cd(iPad + 1);
        pad->Clear();
        PadInfo* info = canGemStripPads->at(iPad);
        if (!info) continue;
        if (info->current) {
            maxy = info->current->GetMaximum(); //GetBinContent(info->current->GetMaximumBin());
            info->current->Draw(info->opt.Data());
            if (info->ref != NULL) {
                if (info->ref->Integral() > 0)
                    info->ref->Draw("same hist");
                k = info->ref->GetMaximum(); //GetBinContent(info->ref->GetMaximumBin());
                if (maxy < k)
                    maxy = k;
                info->current->SetMaximum(maxy * 1.2);
            }
        }
        pad->Update();
        pad->Modified();
    }
    canGemStrip->Update();
    canGemStrip->Modified();
}

void EmbedStagesView(Int_t runID = 0) {
#if !(!defined(__CINT__) || defined(__MAKECINT__))
    bmnloadlibs(); // load BmnRoot libraries
#endif
    //    gStyle->SetOptStat(0);
    gDirectory->cd();
    const Int_t nColors = 16;
    Int_t ColorMap[nColors] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 49, 11, 12, 13, 14, 15, 16};
    //    Int_t runID = 4600;
    Int_t fPeriodID = 7;
    UInt_t sumMods = 0;
    UInt_t maxLayers = 0;
    BmnSetup fBmnSetup = kSRCSETUP;

    TString treeName = "bmndata";
    /**
     * Draw expressions for phys values
     */
    const TString drawExpPseudoRapEve = TString("-log(")
            + "MCTrack.GetPt()"
            + "/("
            + "MCTrack.fPz + "
            + "MCTrack.GetP()"
            + "))";
    const TString drawExpPEve = TString("MCTrack.GetP()");
    const TString drawExpNHitsEve = TString(">>");

    const TString drawExpPseudoRapDst = TString("-log(")
            + "sqrt("
            + "sq(BmnGlobalTrack.fParamFirst.fTx) + "
            + "sq(BmnGlobalTrack.fParamFirst.fTy)"
            + ")/("
            + "1 + sqrt("
            + "1 + "
            + "sq(BmnGlobalTrack.fParamFirst.fTx) + "
            + "sq(BmnGlobalTrack.fParamFirst.fTy)"
            + ")))";
    const TString drawExpPDst = TString("1/BmnGlobalTrack.fParamFirst.fQp");
    const TString drawExpNHitsDst = TString("BmnGlobalTrack.GetNHits()>>");

    TString eveBG = Form("dst-%d.root", runID);
    TString eveLam = Form("eve-%d-isAll.root", runID);
    TString eveEmb = Form("merged-%d-qgsm-4h-isAll.root", runID);
    TString dstBG = Form("dst-%d.root", runID);
    TString dstLam = "dst-lam.root";
    TString dstEmb = "dst-emb.root";

    TString mcHitNames[nDetTypes] = {"StsPoint", "SiliconPoint", "CSCPoint"};
    TString exHitNames[nDetTypes] = {"BmnGemStripHit", "BmnSiliconHit", "BmnCSCHit"};
    Int_t pdg[nParticles] = {2212, -211};
    TString pName[nParticles] = {"proton", "\\pi^{-}"};
    TString valueName[ROWS] = {"eta", "momentum", "eta_momentum_eve", "eta_momentum_dst", "mult"};
    TString valueShow[ROWS] = {"\\eta", "P", "\\eta X P", "\\eta\\ X\\ P", "NHits"};
    TTree * treesMC [nCases] = {nullptr};
    TTree * treesEx [nCases] = {nullptr};
    TClonesArray * hitsMC [nCases][nDetTypes] = {nullptr};
    TClonesArray * hitsEx [nCases][nDetTypes] = {nullptr};
    TClass * hitsMCClasses [nDetTypes] = {CbmStsPoint::Class(), BmnSiliconPoint::Class(), BmnCSCPoint::Class()};
    //    TClass * hitsExClasses [nDetTypes] = {nullptr};
    TClonesArray * mcTracks[nCases] = {nullptr};
    TClonesArray * exTracks[nCases] = {nullptr};
    TClonesArray * exTracksGem[nCases] = {nullptr};
    TClonesArray * exTracksSil[nCases] = {nullptr};

    TCanvas * cans[nCases] = {nullptr};
    TFile * filesMC[nCases] = {nullptr};
    TFile * filesEx[nCases] = {nullptr};
    TString namesMC[nCases] = {eveBG, eveLam, eveEmb};
    TString namesEx[nCases] = {dstBG, dstLam, dstEmb};
    TClonesArray * hits[COLS] = {NULL};
    TClonesArray * tracks[COLS] = {NULL};
    DigiRunHeader * runHeaders[COLS] = {NULL, NULL};
    BmnEventHeader * headers[COLS] = {NULL, NULL};
    vector<vector <PadInfo*> > pads;
    pads.resize(nCases, vector<PadInfo*>(ROWS * nParticles, nullptr));

    //    // ****************************
    //    // BG protons pads init
    //    // ****************************
    //    TCanvas *canBG = new TCanvas("can", "BG proton", PAD_WIDTH_VIEW * 1, PAD_HEIGHT_VIEW * ROWS);
    //    canBG->Divide(nParticles, ROWS);
    //    for (auto &pad : pads[0]) {
    //        pad = new PadInfo();
    //    }
    //    for (Int_t i = 0; i < 2; i++) {
    //        TString src = i ? "dst" : "eve";
    //        EColor color = i ? kRed : kBlue;
    //
    //        TH1F* hEta = new TH1F(Form("hEta_p_%s", src.Data()), "proton\\ \\eta", EtaNBins, -EtaMax, EtaMax);
    //        hEta->SetLineColor(color);
    //        if (i)
    //            pads[0][0]->ref = hEta;
    //        else
    //            pads[0][0]->current = hEta;
    //
    //        TH1F* hP = new TH1F(Form("hP_p_%s", src.Data()), " proton P", PNBins, 0, PMax);
    //        hP->SetLineColor(color);
    //        if (i)
    //            pads[0][1]->ref = hP;
    //        else
    //            pads[0][1]->current = hP;
    //        //        padsBG[1]->opt = "logy logz";
    //        canBG->cd(2)->SetLogy();
    //
    //        TH1F* hHits = new TH1F(Form("hHits_p_%s", src.Data()), "BG proton NHits", HitsNBins, 0, HitsMax);
    //        hHits->SetLineColor(color);
    //        if (i)
    //            pads[0][2]->ref = hHits;
    //        else
    //            pads[0][2]->current = hHits;
    //    }
    //
    //    cans[0] = canBG;

    // ****************************
    // Other pads init
    // ****************************
    for (Int_t iCase = 0; iCase < nCases; iCase++) {
        TCanvas *can = new TCanvas(Form("can_%d", iCase), "can", PAD_WIDTH_VIEW * nParticles, PAD_HEIGHT_VIEW * ROWS);
        can->Divide(nParticles, ROWS);
        cans[iCase] = can;
        for (auto &pad : pads[iCase]) {
            pad = new PadInfo();
        }

        for (Int_t iVal = 0; iVal < ROWS; iVal++) {
            for (Int_t iParticle = 0; iParticle < nParticles; iParticle++) {
                for (Int_t i = 0; i < 2; i++) {
                    TString src = i ? "dst" : "eve";
                    EColor color = i ? kRed : kBlue;

                    TH1* h = (iVal == 2 || iVal == 3) ?
                            (TH1*)new TH2F(
                            Form("h%d_%s_%s_%s", iCase, src.Data(), valueName[iVal].Data(), pName[iParticle].Data()), Form("%s\\ %s", pName[iParticle].Data(), valueShow[iVal].Data()),
                            EtaNBins, -EtaMax, EtaMax, PNBins, -PMax, PMax) :
                            (TH1*)new TH1F(
                            Form("h%d_%s_%s_%s", iCase, src.Data(), valueName[iVal].Data(), pName[iParticle].Data()), Form("%s\\ %s", pName[iParticle].Data(), valueShow[iVal].Data()),
                            Lims[iVal][0], Lims[iVal][1], Lims[iVal][2]);
                    if (iVal == 2 || iVal == 3)
                        pads[iCase][iVal * nParticles + iParticle]->opt = "colz";
                    h->SetLineColor(color);
                    if (!i || (!i && iVal == 2) || (i && iVal == 3))
                        pads[iCase][iVal * nParticles + iParticle]->current = h;
                    else
                        pads[iCase][iVal * nParticles + iParticle]->ref = h;
                    if (iVal == 1)//valueName[iVal].Contains("momentum"))
                        can->cd(iVal * nParticles + iParticle + 1)->SetLogy();
                }
            }
        }
    }

    // ********************
    // Trees processing
    // ********************
    for (Int_t iCase = 0; iCase < nCases; iCase++) {
        printf("case %d\n", iCase);
        filesMC[iCase] = new TFile(namesMC[iCase].Data(), "READ");
        if (filesMC[iCase]->IsOpen())
            printf("file %s opened\n", namesMC[iCase].Data());
        else {
            fprintf(stderr, "file %s open error\n", namesMC[iCase].Data());
            return;
        }
        filesEx[iCase] = new TFile(namesEx[iCase].Data(), "READ");
        if (filesEx[iCase]->IsOpen())
            printf("file %s opened\n", namesEx[iCase].Data());
        else {
            fprintf(stderr, "file %s open error\n", namesEx[iCase].Data());
            return;
        }
        treesMC[iCase] = (TTree*) filesMC[iCase]->Get(treeName.Data());
        treesEx[iCase] = (TTree*) filesEx[iCase]->Get(treeName.Data());

        for (Int_t iDet = 0; iDet < nDetTypes; iDet++) {
            treesMC[iCase]->SetBranchAddress(mcHitNames[iDet], &hitsMC[iCase][iDet]);
            treesEx[iCase]->SetBranchAddress(exHitNames[iDet], &hitsEx[iCase][iDet]);
            treesMC[iCase]->SetBranchAddress("MCTrack", &(mcTracks[iCase]));
            treesEx[iCase]->SetBranchAddress("BmnGlobalTrack", &(exTracks[iCase]));
            treesEx[iCase]->SetBranchAddress("BmnGemTrack", &(exTracksGem[iCase]));
            treesEx[iCase]->SetBranchAddress("BmnSiliconTrack", &(exTracksSil[iCase]));
        }


    }

    //    runHeaders[i] = (DigiRunHeader*) files[i]->Get(runhdrNames[i].Data());
    // ********************
    // Draw all
    // ********************
    // put histos into gDirectory
    for (Int_t iCase = 0; iCase < nCases; iCase++) {
        for (Int_t i = 0; i < ROWS * nParticles; i++) {
            if (pads[iCase][i]->current) pads[iCase][i]->current->SetDirectory(gDirectory);
            if (pads[iCase][i]->ref) pads[iCase][i]->ref->SetDirectory(gDirectory);
        }
        // Draw pseudorapidity of the eve background protons
        //    printf("draw string: %s\n", drawExp.Data 
        //    padsBG[0]->current = (TH1F*) gDirectory->Get(padsBG[0]->current->GetName());
        //    printf("ptr %d\n", padsBG[0]->current);
        //    TH1* h = (TH1*)gDirectory->Get("");
        //    padsBG[0]->current->Add(*h);
        // Draw pseudorapidity of the dst background protons());
        for (Int_t iParticle = 0; iParticle < nParticles; iParticle++) {
            TString
            cond = Form("MCTrack.fPdgCode==%d", pdg[iParticle]);
            TString
            drawExp = drawExpPseudoRapEve + ">>" + pads[iCase][iParticle]->current->GetName();
            treesMC[iCase]->Draw(drawExp.Data(), cond.Data(), "goff");
            drawExp = TString((iParticle ? "-" : "")) + drawExpPEve + ">>" + pads[iCase][nParticles + iParticle]->current->GetName();
            treesMC[iCase]->Draw(drawExp.Data(), cond.Data(), "goff");
            drawExp = drawExpPseudoRapEve + ":" + TString((iParticle ? "-" : "")) + drawExpPEve + ">>"
                    + pads[iCase][2 * nParticles + iParticle]->current->GetName();
            treesMC[iCase]->Draw(drawExp.Data(), cond.Data(), "goff");


            cond = TString("BmnGlobalTrack.GetP()") + (pdg[iParticle] > 0 ? ">" : "<") + "0";
            drawExp = drawExpPseudoRapDst + ">>" + pads[iCase][iParticle]->ref->GetName();
            treesEx[iCase]->Draw(drawExp.Data(), cond.Data(), "goff");
            drawExp = drawExpPDst + ">>" + pads[iCase][nParticles + iParticle]->ref->GetName();
            treesEx[iCase]->Draw(drawExp.Data(), cond.Data(), "goff");
            drawExp = drawExpPseudoRapDst + ":" + TString((iParticle ? "-" : "")) + drawExpPDst + ">>"
                    + pads[iCase][3 * nParticles + iParticle]->current->GetName();
            treesEx[iCase]->Draw(drawExp.Data(), cond.Data(), "goff");
            //            //    drawExp = drawExpNHitsEve + padsBG[2]->current->GetName();
            //            //    treeEveBG->Draw(drawExp.Data(), "", "goff");
            //            drawExp = drawExpNHitsDst + padsBG[2]->ref->GetName();
            //            treeDstBG->Draw(drawExp.Data(), "", "goff");
        }

        for (Int_t iEv = 0; iEv < treesMC[iCase]->GetEntries(); iEv++) {
            treesMC[iCase]->GetEntry(iEv);
            for (Int_t iTrack = 0; iTrack < mcTracks[iCase]->GetEntriesFast(); iTrack++) {
                CbmMCTrack* track = (CbmMCTrack*) mcTracks[iCase]->UncheckedAt(iTrack);

                for (Int_t iParticle = 0; iParticle < nParticles; iParticle++) {
                    Int_t nHits = 0;
                    for (Int_t iDet = 0; iDet < nDetTypes; iDet++) {
                        for (Int_t iPoint = 0; iPoint < hitsMC[iCase][iDet]->GetEntriesFast(); iPoint++) {
                            CbmStsPoint* stsPoint = (CbmStsPoint*) hitsMC[iCase][iDet]->UncheckedAt(iPoint);
                            Int_t TrackID = stsPoint->GetTrackID();
                            if (TrackID != iTrack)
                                continue;
                            if (track->GetPdgCode() == pdg[iParticle])
                                nHits++;
                        }
                    }
                    if (nHits > 0)
                        pads[iCase][4 * nParticles + iParticle]->current->Fill(nHits);
                }
            }

            treesEx[iCase]->GetEntry(iEv);
            for (Int_t iTrack = 0; iTrack < exTracks[iCase]->GetEntriesFast(); iTrack++) {
                BmnGlobalTrack* track = (BmnGlobalTrack*) exTracks[iCase]->UncheckedAt(iTrack);
                //                FairTrackParam* parFirst = track->GetParamFirst();
                //                FairTrackParam* parLast = track->GetParamLast();

                // Put here your code ...

                // Inner tracker contains GEM and SILICon detectors
                BmnTrack* gemTr = nullptr;
                BmnTrack* silTr = nullptr;

                if (track->GetGemTrackIndex() != -1)
                    gemTr = (BmnTrack*) exTracksGem[iCase]->UncheckedAt(track->GetGemTrackIndex());

                if (track->GetSilTrackIndex() != -1)
                    silTr = (BmnTrack*) exTracksSil[iCase]->UncheckedAt(track->GetSilTrackIndex());

                for (Int_t iParticle = 0; iParticle < nParticles; iParticle++) {
                    //                    if (track->GetPDG() == pdg[iParticle]) {
                    if (track->GetP() * (pdg[iParticle]) > 0) {
                        Int_t nHits = 0;
                        if (gemTr)
                            nHits += gemTr->GetNHits();
                        if (silTr)
                            nHits += silTr->GetNHits();
                        if (track->GetCscHitIndex() > -1)
                            nHits++;
                        if (nHits > 0)
                            pads[iCase][4 * nParticles + iParticle]->ref->Fill(nHits);

                    }

                }
                //                // GEM track
                //                if (track->GetGemTrackIndex() != -1)
                //                    for (Int_t iHit = 0; iHit < gemTr->GetNHits(); iHit++) {
                //                        BmnGemStripHit* hit = (BmnGemStripHit*) gemPoints->UncheckedAt(gemTr->GetHitIndex(iHit));
                //                        
                //                    }
                //
                //                // SILICON track
                //                if (track->GetSilTrackIndex() != -1)
                //                    for (Int_t iHit = 0; iHit < silTr->GetNHits(); iHit++) {
                //                        BmnSiliconHit* hit = (BmnSiliconHit*) silPoints->UncheckedAt(silTr->GetHitIndex(iHit));
                //
                //                    }
            }// for (Int_t iTrack = 0; iTrack < globTracks->GetEntriesFast(); iTrack++)

        }
        DrawRef(cans[iCase], &pads[iCase]);
        cans[iCase]->SaveAs(Form("can-emb-%d.png", iCase));
    }


    //    TImage *img = TImage::Create();
    //    img->FromPad(can);
    //    img->WriteImage("can-emb.png");
    //    canStrip->SaveAs(Form("can-run-%d.eps", runID));
    //    canStrip->SaveAs(Form("can-run-%d.pdf", runID));
    //
    //    TCanvas *c1 = new TCanvas("c1");
    //    padsBG[0]->ref->Draw();
    //    c1->SaveAs("can-temp.png");
    //    delete c1;

    for (Int_t iCase = 0; iCase < nCases; iCase++) {
        if (filesMC[iCase])
            filesMC[iCase]->Close();
        if (filesEx[iCase])
            filesEx[iCase]->Close();
    }
}
