
#include <TChain.h>
#include <TClonesArray.h>
#include <TFitResultPtr.h>
#include <TString.h>

#include <FairHit.h>
#include <Rtypes.h>
#include <TGraph.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
//
void drawTracks(TString fileName = "srcdst_3332.root", Bool_t isField = kTRUE, Double_t field = 108.1, Int_t nEvents = 1000) {
    //    void drawDchTracks(TString fileName = "$VMCWORKDIR/macro/run/srcdst_2322.root", Bool_t isField = kTRUE, Double_t field = 55.7, Int_t nEvents = 1000) {
    //    void drawDchTracks(TString fileName = "$VMCWORKDIR/macro/run/srcdst_2325.root", Bool_t isField = kTRUE, Double_t field = 75.1, Int_t nEvents = 1000) {
    //    void drawDchTracks(TString fileName = "$VMCWORKDIR/macro/run/srcdst_2327.root", Bool_t isField = kTRUE, Double_t field = 93.1, Int_t nEvents = 1000) {
    //    void drawDchTracks(TString fileName = "$VMCWORKDIR/macro/run/srcdst_2332.root", Bool_t isField = kTRUE, Double_t field = 108.1, Int_t nEvents = 1000) {
    //    void drawDchTracks(TString fileName = "$VMCWORKDIR/macro/run/srcdst_2335.root", Bool_t isField = kTRUE, Double_t field = 120.1, Int_t nEvents = 1) {

    bmnloadlibs();  // load libraries

    TGeoManager::Import("current_geo_file.root");
    FairRunAna* fRunAna = new FairRunAna();
    BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
    if (isField)
        magField->SetScale(field / 55.7);
    else
        magField->SetScale(0.0);
    fRunAna->SetField(magField);
    magField->Init();

    TChain* out = new TChain("bmndata");
    out->Add(fileName.Data());
    cout << "#recorded entries = " << out->GetEntries() << endl;

    TClonesArray* dchTracks = nullptr;
    out->SetBranchAddress("BmnDchTrack", &dchTracks);
    TClonesArray* gemTracks = nullptr;
    out->SetBranchAddress("BmnGlobalTrack", &gemTracks);
    TClonesArray* silTracks = nullptr;
    out->SetBranchAddress("BmnSiliconTrack", &silTracks);
    TClonesArray* mwpcTracks = nullptr;
    out->SetBranchAddress("BmnMwpcTrack", &mwpcTracks);
    TClonesArray* mwpcSegments = nullptr;
    out->SetBranchAddress("BmnMwpcSegment", &mwpcSegments);

    Double_t z_targ = -645.0;
    Double_t z1_sil = -431.0;  //-500.0;
    Double_t z2_sil = -318.0;  //-250.0;
    Double_t z1_dch1 = +400.0;
    Double_t z2_dch1 = +600.0;
    Double_t z1_dch2 = +600.0;
    Double_t z2_dch2 = +800.0;
    Double_t z1_gem = -208.0;
    Double_t z2_gem = +400.0;
    Double_t z1_mwpc = -850.0;
    Double_t z2_mwpc = -752.0;
    Double_t z3_mwpc = -357.0;
    Double_t z4_mwpc = -208.0;

    TCanvas* cX = new TCanvas("cX", "cX", 1000, 500);
    cX->Range(-1100., -130., 1100., 110.);
    TCanvas* cY = new TCanvas("cY", "cY", 1000, 500);
    cY->Range(-1100., -130., 1100., 110.);

    BmnKalmanFilter* kalman = new BmnKalmanFilter();

    for (Int_t iEv = 0; iEv < nEvents; iEv++) {
        out->GetEntry(iEv);

        if (iEv % 1000 == 0) printf("event: %d\n", iEv);

        if (silTracks) {
            for (Int_t iTrack = 0; iTrack < silTracks->GetEntriesFast(); iTrack++) {
                BmnSiliconTrack* track = (BmnSiliconTrack*)silTracks->At(iTrack);

                FairTrackParam* par1 = track->GetParamFirst();
                Double_t x01 = par1->GetX();
                Double_t y01 = par1->GetY();
                Double_t z01 = par1->GetZ();
                Double_t tx1 = par1->GetTx();
                Double_t ty1 = par1->GetTy();

                FairTrackParam* par = track->GetParamLast();
                Double_t x02 = par->GetX();
                Double_t y02 = par->GetY();
                Double_t z02 = par->GetZ();
                Double_t tx2 = par->GetTx();
                Double_t ty2 = par->GetTy();

                Double_t x1 = x01 + tx1 * (z_targ - z01);
                Double_t y1 = y01 + ty1 * (z_targ - z01);
                Double_t x2 = x02 + tx2 * (z2_sil - z02);
                Double_t y2 = y02 + ty2 * (z2_sil - z02);

                cX->cd();
                TLine* lX = new TLine(z_targ, x1, z2_sil, x2);
                lX->SetLineColor(kGreen + 2);
                lX->SetLineWidth(1);
                lX->SetLineColorAlpha(kGreen + 2, 0.05);
                lX->Draw();
                cY->cd();
                TLine* lY = new TLine(z_targ, y1, z2_sil, y2);
                lY->SetLineColor(kGreen + 2);
                lY->SetLineWidth(1);
                lY->SetLineColorAlpha(kGreen + 2, 0.05);
                lY->Draw();
            }
        }

        if (gemTracks) {
            for (Int_t iTrack = 0; iTrack < gemTracks->GetEntriesFast(); iTrack++) {
                BmnGlobalTrack* track = (BmnGlobalTrack*)gemTracks->At(iTrack);
                if (track->GetNHits() < 6) continue;

                Int_t nSteps = (isField) ? 20 : 1;
                Double_t step = Int_t((z2_gem - z1_gem) / nSteps);
                FairTrackParam parPrev(*(track->GetParamFirst()));
                //            FairTrackParam* parPrev = track->GetParamLast();

                Double_t xPrev = parPrev.GetX();
                Double_t yPrev = parPrev.GetY();
                Double_t zPrev = parPrev.GetZ();

                if (isField) {
                    kalman->TGeoTrackPropagate(&parPrev, z1_gem, 2212, NULL, NULL, kTRUE);
                    xPrev = parPrev.GetX();
                    yPrev = parPrev.GetY();
                    zPrev = parPrev.GetZ();
                } else {
                    xPrev += parPrev.GetTx() * (zPrev - z1_gem);
                    yPrev += parPrev.GetTy() * (zPrev - z1_gem);
                    zPrev = z1_gem;
                }

                for (Int_t iStep = 0; iStep < nSteps; iStep++) {
                    Double_t z = zPrev + step;
                    if (isField) kalman->TGeoTrackPropagate(&parPrev, z, 2212, NULL, NULL, kTRUE);
                    Double_t x = xPrev + parPrev.GetTx() * step;
                    Double_t y = yPrev + parPrev.GetTy() * step;

                    cX->cd();
                    TLine* lX = new TLine(zPrev, xPrev, z, x);
                    lX->SetLineColor(kBlue);
                    lX->SetLineWidth(1);
                    lX->SetLineColorAlpha(kBlue, 0.05);
                    lX->Draw();
                    cY->cd();
                    TLine* lY = new TLine(zPrev, yPrev, z, y);
                    lY->SetLineColor(kBlue);
                    lY->SetLineWidth(1);
                    lY->SetLineColorAlpha(kBlue, 0.05);
                    lY->Draw();

                    xPrev = x;
                    yPrev = y;
                    zPrev = z;
                }
            }
        }

        if (mwpcTracks) {
            for (Int_t iTr = 0; iTr < mwpcTracks->GetEntriesFast(); ++iTr) {
                BmnTrack* track = (BmnTrack*)mwpcTracks->At(iTr);

                Int_t ip = -1;
                Double_t Z = track->GetParamFirst()->GetZ();
                Int_t itr = track->GetFlag();

                if (Z >= -809. && Z <= -807.) ip = 0;
                if (Z >= -290. && Z <= -288.) ip = 1;

                if (ip < 0) continue;

                Int_t Nhits = track->GetNHits();

                Double_t x0 = track->GetParamFirst()->GetX();
                Double_t y0 = track->GetParamFirst()->GetY();
                Double_t z0 = track->GetParamFirst()->GetZ();
                Double_t tx = track->GetParamFirst()->GetTx();
                Double_t ty = track->GetParamFirst()->GetTy();

                if (ip == 0) {
                    Double_t x1 = x0 + tx * (z1_mwpc - z0);
                    Double_t y1 = y0 + ty * (z1_mwpc - z0);
                    Double_t x2 = x0 + tx * (z_targ - z0);
                    Double_t y2 = y0 + ty * (z_targ - z0);

                    cX->cd();
                    TLine* lX = new TLine(z1_mwpc, x1, z_targ, x2);
                    lX->SetLineColor(kMagenta + 2);
                    lX->SetLineWidth(1);
                    lX->SetLineColorAlpha(kMagenta + 2, 0.05);
                    lX->Draw();
                    cY->cd();
                    TLine* lY = new TLine(z1_mwpc, y1, z_targ, y2);
                    lY->SetLineColor(kMagenta + 2);
                    lY->SetLineWidth(1);
                    lY->SetLineColorAlpha(kMagenta + 2, 0.05);
                    lY->Draw();
                }
                if (ip == 1) {
                    Double_t x1 = x0 + tx * (z3_mwpc - z0);
                    Double_t y1 = y0 + ty * (z3_mwpc - z0);
                    Double_t x2 = x0 + tx * (z4_mwpc - z0);
                    Double_t y2 = y0 + ty * (z4_mwpc - z0);

                    cX->cd();
                    TLine* lX = new TLine(z3_mwpc, x1, z4_mwpc, x2);
                    lX->SetLineColor(kMagenta + 2);
                    lX->SetLineWidth(1);
                    lX->SetLineColorAlpha(kMagenta + 2, 0.05);
                    lX->Draw();
                    cY->cd();
                    TLine* lY = new TLine(z3_mwpc, y1, z4_mwpc, y2);
                    lY->SetLineColor(kMagenta + 2);
                    lY->SetLineWidth(1);
                    lY->SetLineColorAlpha(kMagenta + 2, 0.05);
                    lY->Draw();
                }
            }  //iTr
        }

        if (dchTracks) {
            for (Int_t iTrack = 0; iTrack < dchTracks->GetEntriesFast(); iTrack++) {
                BmnDchTrack* track = (BmnDchTrack*)dchTracks->At(iTrack);
                if (track->GetNHits() < 8) continue;
                //            if (track->GetFlag() != 7) continue;

                FairTrackParam* par = track->GetParamFirst();
                Double_t z0 = par->GetZ();
                if (z0 < 550) {
                    Double_t x0 = par->GetX();
                    Double_t y0 = par->GetY();
                    Double_t tx = par->GetTx();
                    Double_t ty = par->GetTy();

                    Double_t x1 = x0 + tx * (z1_dch1 - z0);
                    Double_t y1 = y0 + ty * (z1_dch1 - z0);
                    Double_t x2 = x0 + tx * (z2_dch1 - z0);
                    Double_t y2 = y0 + ty * (z2_dch1 - z0);

                    cX->cd();
                    TLine* lX = new TLine(z1_dch1, x1, z2_dch1, x2);
                    lX->SetLineColor(kRed);
                    lX->SetLineWidth(1);
                    lX->SetLineColorAlpha(kRed, 0.05);
                    lX->Draw();
                    cY->cd();
                    TLine* lY = new TLine(z1_dch1, y1, z2_dch1, y2);
                    lY->SetLineColor(kRed);
                    lY->SetLineWidth(1);
                    lY->SetLineColorAlpha(kRed, 0.05);
                    lY->Draw();
                } else if (z0 > 650) {
                    Double_t x0 = par->GetX();
                    Double_t y0 = par->GetY();
                    Double_t tx = par->GetTx();
                    Double_t ty = par->GetTy();

                    Double_t x1 = x0 + tx * (z1_dch2 - z0);
                    Double_t y1 = y0 + ty * (z1_dch2 - z0);
                    Double_t x2 = x0 + tx * (z2_dch2 - z0);
                    Double_t y2 = y0 + ty * (z2_dch2 - z0);

                    cX->cd();
                    TLine* lX = new TLine(z1_dch2, x1, z2_dch2, x2);
                    lX->SetLineColor(kRed);
                    lX->SetLineWidth(1);
                    lX->SetLineColorAlpha(kRed, 0.05);
                    lX->Draw();
                    cY->cd();
                    TLine* lY = new TLine(z1_dch2, y1, z2_dch2, y2);
                    lY->SetLineColor(kRed);
                    lY->SetLineWidth(1);
                    lY->SetLineColorAlpha(kRed, 0.05);
                    lY->Draw();
                }
            }
        }
    }

    delete kalman;

    cX->cd();
    TGaxis* axisX1 = new TGaxis(-900, -100, -900, 100, -100, 100, 510, "");
    axisX1->SetTitle("X [cm]");
    axisX1->SetLineWidth(1);
    axisX1->SetLineColor(kBlack);
    axisX1->SetLineColorAlpha(kBlack, 0.5);
    axisX1->Draw();

    TGaxis* axisY1 = new TGaxis(-900, -100, 1000, -100, -900, 1000, 510, "");
    axisY1->SetTitle("Z [cm]");
    axisY1->SetLineWidth(1);
    axisY1->SetLineColor(kBlack);
    axisY1->SetLineColorAlpha(kBlack, 0.5);
    axisY1->Draw();

    const Int_t nPlane = 16;  //11;

    //                        PC0  PC1     t      Si1/2     PC3   Si3    PC4
    Double_t zPos[nPlane] = {-850, -752, -645, -433, -429, -357, -318, -207.4, 65, 89, 113, 137, 161, 185, 510, 710};
    Double_t xPos[nPlane] = {14, 14, 3, 6, 6, 14, 12, 14, 80, 80, 80, 80, 80, 80, 90, 90};
    Double_t yPos[nPlane] = {14, 14, 3, 6, 6, 14, 12, 14, 23, 23, 23, 23, 23, 23, 90, 90};
    for (Int_t i = 0; i < nPlane; ++i) {
        TLine* lst = new TLine(zPos[i], -xPos[i], zPos[i], xPos[i]);
        lst->SetLineColor(kBlack);
        lst->SetLineColorAlpha(kBlack, 0.5);
        lst->Draw();
    }

    cY->cd();
    TGaxis* axisX2 = new TGaxis(-870, -100, -870, 100, -100, 100, 510, "");
    axisX2->SetTitle("Y [cm]");
    axisX2->SetLineColor(kBlack);
    axisX2->SetLineColorAlpha(kBlack, 0.5);
    axisX2->Draw();

    TGaxis* axisY2 = new TGaxis(-900, -100, 850, -100, -900, 900, 510, "");
    axisY2->SetTitle("Z [cm]");
    axisY2->SetLineWidth(1);
    axisY2->SetLineColor(kBlack);
    axisY2->SetLineColorAlpha(kBlack, 0.5);
    axisY2->Draw();

    for (Int_t i = 0; i < nPlane; ++i) {
        TLine* lst = new TLine(zPos[i], -yPos[i], zPos[i], yPos[i]);
        lst->SetLineWidth(1);
        lst->SetLineColor(kBlack);
        lst->SetLineColorAlpha(kBlack, 0.5);
        lst->Draw();
    }

    cX->SaveAs("tracks_ZX.pdf");
    cY->SaveAs("tracks_ZY.pdf");
}
