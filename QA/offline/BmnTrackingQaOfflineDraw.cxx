#include "BmnTrackingQaOfflineDraw.h"

BmnTrackingQaOfflineDraw::BmnTrackingQaOfflineDraw() :
h(nullptr),
h2(nullptr),
h3(nullptr),
h4(nullptr),
h5(nullptr) {
    usedOutExtension = "pdf";

}

void BmnTrackingQaOfflineDraw::PrepareHistos() {
    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.1);
    gStyle->SetTitleY(1.0);
    gStyle->SetPaintTextFormat("4.3f");

    const Int_t nBeams = fBeams.size();
    const Int_t nTargets = fTargets.size();
    const Int_t nTriggers = fTriggers.size();
    const Int_t nHistos = 5;

    const Int_t nResidHistos = 8; // tof400, toff700, csc, dch x 2 (x, y)

    enum {
        Vz, TotMult, PosMult, NegMult, Momentum
    };

    enum {
        matchOneToOne, matchTwoToOne, matchThreeToOne, beta400, beta700
    };

    enum {
        xResTof400, yResTof400,
        xResTof700, yResTof700,
        xResCsc, yResCsc,
        xResDch, yResDch
    };

    h = new TH1F****[nBeams];
    h4 = new TH2F****[nBeams];
    h5 = new TH1F****[nBeams];

    h2 = new TH2F("h2", "Num of event per target / trigger (Argon run) [MEvents]", nTriggers, 0., nTriggers, nTargets, 0., nTargets);
    h3 = new TH2F("h3", "Num of event per target / trigger (Argon run) [MEvents], nRecoTracks > 1", nTriggers, 0., nTriggers, nTargets, 0., nTargets);

    TH2F * hArr[2] = {h2, h3};

    for (Int_t iHisto = 0; iHisto < 2; iHisto++) {
        TAxis* xAxis = hArr[iHisto]->GetXaxis();
        TAxis* yAxis = hArr[iHisto]->GetYaxis();

        for (Int_t iBin = 0; iBin < fTriggers.size(); iBin++)
            xAxis->SetBinLabel(iBin + 1, Form("%s", fTriggers[iBin].Data()));
        for (Int_t iBin = 0; iBin < fTargets.size(); iBin++)
            yAxis->SetBinLabel(iBin + 1, Form("%s", fTargets[iBin].Data()));

        //h2->GetXaxis()->SetLabelSize(0.07);
        hArr[iHisto]->GetYaxis()->SetLabelSize(0.07);

        hArr[iHisto]->GetXaxis()->SetTitleSize(0.06);
        hArr[iHisto]->GetYaxis()->SetTitleSize(0.06);

        hArr[iHisto]->GetXaxis()->SetTitleOffset(.43);
        hArr[iHisto]->GetYaxis()->SetTitleOffset(.77);
    }

    //h2->SetMarkerSize(2.3);

    for (Int_t iBeam = 0; iBeam < nBeams; iBeam++) {
        h[iBeam] = new TH1F***[nTargets];
        h4[iBeam] = new TH2F***[nTargets];
        h5[iBeam] = new TH1F***[nTargets];

        for (Int_t iTarget = 0; iTarget < nTargets; iTarget++) {
            h[iBeam][iTarget] = new TH1F**[nTriggers];
            h4[iBeam][iTarget] = new TH2F**[nTriggers];
            h5[iBeam][iTarget] = new TH1F**[nTriggers];

            for (Int_t iTrigger = 0; iTrigger < nTriggers; iTrigger++) {

                h[iBeam][iTarget][iTrigger] = new TH1F*[nHistos];
                h4[iBeam][iTarget][iTrigger] = new TH2F*[nHistos];
                h5[iBeam][iTarget][iTrigger] = new TH1F*[nResidHistos];

                for (Int_t iHisto = 0; iHisto < nHistos; iHisto++) {

                    if (iHisto == Vz)
                        h[iBeam][iTarget][iTrigger][Vz] = new TH1F(Form("Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                            Form("Target: %s; V_{z} [cm]; N", fTargets[iTarget].Data()), 200, -10., +10.);

                    else if (iHisto == TotMult)
                        h[iBeam][iTarget][iTrigger][TotMult] = new TH1F(Form("Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                            Form("Target: %s; N_{tracks per event}; N", fTargets[iTarget].Data()), 30, 0., 30.);

                    else if (iHisto == PosMult)
                        h[iBeam][iTarget][iTrigger][PosMult] = new TH1F(Form("Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                            Form("Target: %s; N_{positive tracks per event}; N", fTargets[iTarget].Data()), 30, 0., 30.);

                    else if (iHisto == NegMult)
                        h[iBeam][iTarget][iTrigger][NegMult] = new TH1F(Form("Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                            Form("Target: %s; N_{negative tracks per event}; N", fTargets[iTarget].Data()), 30, 0., 30.);

                    else if (iHisto == Momentum)
                        h[iBeam][iTarget][iTrigger][Momentum] = new TH1F(Form("Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                            Form("Target: %s; P / q [GeV/c]; N", fTargets[iTarget].Data()), 500, -10., +10.);

                    h[iBeam][iTarget][iTrigger][iHisto]->GetXaxis()->SetLabelSize(0.07);
                    h[iBeam][iTarget][iTrigger][iHisto]->GetYaxis()->SetLabelSize(0.07);

                    h[iBeam][iTarget][iTrigger][iHisto]->GetXaxis()->SetTitleSize(0.09);
                    h[iBeam][iTarget][iTrigger][iHisto]->GetYaxis()->SetTitleSize(0.07);

                    h[iBeam][iTarget][iTrigger][iHisto]->GetXaxis()->SetTitleOffset(-5.);
                    h[iBeam][iTarget][iTrigger][iHisto]->GetYaxis()->SetTitleOffset(.77);
                }

                for (Int_t iHisto = 0; iHisto < nHistos; iHisto++) {
                    if (iHisto == matchOneToOne || iHisto == matchTwoToOne || iHisto == matchThreeToOne) {
                        TString* xArr = nullptr;
                        TString* yArr = nullptr;

                        Int_t sizeX = -1;
                        Int_t sizeY = -1;

                        if (iHisto == matchOneToOne) {
                            sizeX = sizeY = 4;
                            xArr = new TString[sizeX];
                            yArr = new TString[sizeY];

                            xArr[0] = yArr[0] = "tof400";
                            xArr[1] = yArr[1] = "tof700";
                            xArr[2] = yArr[2] = "csc";
                            xArr[3] = yArr[3] = "dch";
                        } else if (iHisto == matchTwoToOne) {
                            sizeX = 3;
                            sizeY = 2;

                            xArr = new TString[sizeX];
                            yArr = new TString[sizeY];

                            xArr[0] = "tof400 + tof700";
                            xArr[1] = "tof700 + csc";
                            xArr[2] = "tof400 + csc";

                            yArr[0] = "csc";
                            yArr[1] = "dch";
                        } else if (iHisto == matchThreeToOne) {
                            sizeX = sizeY = 1;

                            xArr = new TString[sizeX];
                            yArr = new TString[sizeY];

                            xArr[0] = "tof400 + tof700 + csc";
                            yArr[0] = "dch";
                        }

                        h4[iBeam][iTarget][iTrigger][iHisto] = new TH2F(Form(" Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                                Form("Target: %s [%%]", fTargets[iTarget].Data()), sizeX, 0., sizeX, sizeY, 0., sizeY);

                        TAxis* xAxis = h4[iBeam][iTarget][iTrigger][iHisto]->GetXaxis();
                        TAxis* yAxis = h4[iBeam][iTarget][iTrigger][iHisto]->GetYaxis();

                        for (Int_t iBin = 0; iBin < sizeX; iBin++)
                            xAxis->SetBinLabel(iBin + 1, Form("%s", xArr[iBin].Data()));
                        for (Int_t iBin = 0; iBin < sizeY; iBin++)
                            yAxis->SetBinLabel(iBin + 1, Form("%s", yArr[iBin].Data()));

                        delete [] xArr;
                        delete [] yArr;
                    } else if (iHisto == beta400)
                        h4[iBeam][iTarget][iTrigger][iHisto] = new TH2F(Form(" Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                            Form("Target: %s; P / q [GeV / c]; #beta_{400}", fTargets[iTarget].Data()), 250, 0., 10., 100, 0., 1.2);

                    else if (iHisto == beta700)
                        h4[iBeam][iTarget][iTrigger][iHisto] = new TH2F(Form(" Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                            Form("Target: %s; P / q [GeV / c]; #beta_{700}", fTargets[iTarget].Data()), 250, 0., 10., 100, 0., 1.2);

                    h4[iBeam][iTarget][iTrigger][iHisto]->GetXaxis()->SetLabelSize(0.095);
                    h4[iBeam][iTarget][iTrigger][iHisto]->GetYaxis()->SetLabelSize(0.1);

                    h4[iBeam][iTarget][iTrigger][iHisto]->GetXaxis()->SetTitleSize(0.09);
                    h4[iBeam][iTarget][iTrigger][iHisto]->GetYaxis()->SetTitleSize(iHisto < 3 ? 0.07 : 0.09);

                    h4[iBeam][iTarget][iTrigger][iHisto]->GetXaxis()->SetTitleOffset(-5.);
                    h4[iBeam][iTarget][iTrigger][iHisto]->GetYaxis()->SetTitleOffset(.77);

                    h4[iBeam][iTarget][iTrigger][iHisto]->SetMarkerSize(3.);
                }

                // Resid. histos ...
                for (Int_t iHisto = 0; iHisto < nResidHistos; iHisto++) {
                    if (iHisto == xResTof400) {
                        h5[iBeam][iTarget][iTrigger][xResTof400] = new TH1F(Form(" Resid Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                                Form("Target: %s (TOF400); Resid. (blue - Y) [cm]; N", fTargets[iTarget].Data()), 500, -15., +15.);
                        h5[iBeam][iTarget][iTrigger][xResTof400]->SetLineColor(kRed);
                    } else if (iHisto == yResTof400) {
                        h5[iBeam][iTarget][iTrigger][yResTof400] = new TH1F(Form(" Resid Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                                Form("Target: %s (TOF400); Resid. (blue - Y) [cm]; N", fTargets[iTarget].Data()), 500, -15., +15.);
                        h5[iBeam][iTarget][iTrigger][yResTof400]->SetLineColor(kBlue);
                    } else if (iHisto == xResTof700) {
                        h5[iBeam][iTarget][iTrigger][xResTof700] = new TH1F(Form(" Resid Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                                Form("Target: %s (TOF700); Resid. (blue - Y) [cm]; N", fTargets[iTarget].Data()), 500, -50., +50.);
                        h5[iBeam][iTarget][iTrigger][xResTof700]->SetLineColor(kRed);
                    } else if (iHisto == yResTof700) {
                        h5[iBeam][iTarget][iTrigger][yResTof700] = new TH1F(Form(" Resid Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                                Form("Target: %s (TOF700); Resid. (blue - Y) [cm]; N", fTargets[iTarget].Data()), 500, -30., +30.);
                        h5[iBeam][iTarget][iTrigger][yResTof700]->SetLineColor(kBlue);
                    } else if (iHisto == xResCsc) {
                        h5[iBeam][iTarget][iTrigger][xResCsc] = new TH1F(Form(" Resid Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                                Form("Target: %s (CSC); Resid. (blue - Y) [cm]; N", fTargets[iTarget].Data()), 500, -30., +30.);
                        h5[iBeam][iTarget][iTrigger][xResCsc]->SetLineColor(kRed);
                    } else if (iHisto == yResCsc) {
                        h5[iBeam][iTarget][iTrigger][yResCsc] = new TH1F(Form(" Resid Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                                Form("Target: %s (CSC); Resid. (blue - Y) [cm]; N", fTargets[iTarget].Data()), 500, -20., +20.);
                        h5[iBeam][iTarget][iTrigger][yResCsc]->SetLineColor(kBlue);
                    } else if (iHisto == xResDch) {
                        h5[iBeam][iTarget][iTrigger][xResDch] = new TH1F(Form(" Resid Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                                Form("Target: %s (DCH); Resid. (blue - Y) [cm]; N", fTargets[iTarget].Data()), 500, -10., +10.);
                        h5[iBeam][iTarget][iTrigger][xResDch]->SetLineColor(kRed);
                    } else if (iHisto == yResDch) {
                        h5[iBeam][iTarget][iTrigger][yResDch] = new TH1F(Form(" Resid Beam %d Target %d Trigger %d Histo %d", iBeam, iTarget, iTrigger, iHisto),
                                Form("Target: %s (DCH); Resid. (blue - Y) [cm]; N", fTargets[iTarget].Data()), 500, -10., +10.);
                        h5[iBeam][iTarget][iTrigger][yResDch]->SetLineColor(kBlue);
                    }

                    h5[iBeam][iTarget][iTrigger][iHisto]->GetXaxis()->SetLabelSize(0.095);
                    h5[iBeam][iTarget][iTrigger][iHisto]->GetYaxis()->SetLabelSize(0.1);

                    h5[iBeam][iTarget][iTrigger][iHisto]->GetXaxis()->SetTitleSize(0.09);
                    h5[iBeam][iTarget][iTrigger][iHisto]->GetYaxis()->SetTitleSize(0.09);

                    h5[iBeam][iTarget][iTrigger][iHisto]->GetXaxis()->SetTitleOffset(-5.);
                    h5[iBeam][iTarget][iTrigger][iHisto]->GetYaxis()->SetTitleOffset(.57);

                    // h5[iBeam][iTarget][iTrigger][iHisto]->SetMarkerSize(3.);
                }
            }
        }
    }


}

void BmnTrackingQaOfflineDraw::ProcessHistos(Bool_t anal1, Bool_t anal2, Bool_t anal3) {
    if (anal3) {
        for (Int_t iBeam = 0; iBeam < fBeams.size(); iBeam++)
            for (Int_t iTrigger = 0; iTrigger < fTriggers.size(); iTrigger++) {

                TCanvas* c = new TCanvas("c", "c", 1200, 800);
                c->Divide(5, 5);

                const Int_t nPads = 25;

                for (Int_t iPad = 1; iPad < nPads + 1; iPad++) {
                    TVirtualPad* pad = c->cd(iPad);

                    if ((iPad - 1) % 5 < 3)
                        pad->SetGrid();

                    if ((iPad - 1) % 5 > 2) {
                        pad->SetLogz();
                        pad->SetLeftMargin(0.2);
                    }

                    if ((iPad - 1) % 5 == 0)
                        pad->SetLeftMargin(0.2);

                    Int_t idx = iPad - 1;

                    Int_t currTarget = idx / 5;

                    if (currTarget + 1 > fTargets.size())
                        continue;

                    Int_t currDist = idx % 5;

                    TH2F* histo = h4[iBeam][currTarget][iTrigger][currDist];

                    if (currDist == 3 || currDist == 4)
                        histo->Draw("colz");
                    else
                        histo->Draw("TEXT");
                }

                TString targPath = "";
                for (auto it : fTargets)
                    targPath += it;

                c->SaveAs(Form("beam_%s_targ_%s_trigger_%s_matching.%s", fBeams[iBeam].Data(), targPath.Data(), fTriggers[iTrigger].Data(), usedOutExtension.Data()));
                delete c;
            }

        const Int_t nBeams = fBeams.size();
        const Int_t nTargets = fTargets.size();
        const Int_t nTriggers = fTriggers.size();

        const Int_t nDets = 4; // tof400, tof700, csc, dch

        TH2F***** fitResults = new TH2F****[nBeams];
        for (Int_t iBeam = 0; iBeam < nBeams; iBeam++) {
            fitResults[iBeam] = new TH2F***[nTargets];

            for (Int_t iTarget = 0; iTarget < nTargets; iTarget++) {
                fitResults[iBeam][iTarget] = new TH2F**[nTriggers];

                for (Int_t iTrigger = 0; iTrigger < nTriggers; iTrigger++) {
                    fitResults[iBeam][iTarget][iTrigger] = new TH2F*[nDets];

                    for (Int_t iDet = 0; iDet < nDets; iDet++) {
                        TString titlePart = (iDet == 0) ? "(TOF400)" : (iDet == 1) ? "(TOF700)" : (iDet == 2) ? "(CSC)" : "(DCH)";

                        fitResults[iBeam][iTarget][iTrigger][iDet] =
                                new TH2F(Form("Beam %d Target %d Trigger %d Det %d", iBeam, iTarget, iTrigger, iDet),
                                Form("Target: %s %s", fTargets[iTarget].Data(), titlePart.Data()), 2., 0., 2., 2, 0., 2.);

                        fitResults[iBeam][iTarget][iTrigger][iDet]->GetXaxis()->SetTitleSize(0.09);
                        fitResults[iBeam][iTarget][iTrigger][iDet]->GetYaxis()->SetTitleSize(0.09);

                        fitResults[iBeam][iTarget][iTrigger][iDet]->GetXaxis()->SetTitleOffset(-5.);
                        fitResults[iBeam][iTarget][iTrigger][iDet]->GetYaxis()->SetTitleOffset(.57);

                        fitResults[iBeam][iTarget][iTrigger][iDet]->SetMarkerSize(4.);

                        fitResults[iBeam][iTarget][iTrigger][iDet]->GetYaxis()->SetLabelSize(.15);
                        fitResults[iBeam][iTarget][iTrigger][iDet]->GetXaxis()->SetLabelSize(.15);

                        const Int_t nBins = 2;
                        TString xArr[nBins] = {"X", "Y"};
                        TString yArr[nBins] = {"#sigma [cm]", "#mu [cm]"};

                        for (Int_t iBin = 0; iBin < nBins; iBin++)
                            fitResults[iBeam][iTarget][iTrigger][iDet]->GetXaxis()->SetBinLabel(iBin + 1, Form("%s", xArr[iBin].Data()));
                        for (Int_t iBin = 0; iBin < nBins; iBin++)
                            fitResults[iBeam][iTarget][iTrigger][iDet]->GetYaxis()->SetBinLabel(iBin + 1, Form("%s", yArr[iBin].Data()));

                    }
                }
            }
        }

        for (Int_t iBeam = 0; iBeam < fBeams.size(); iBeam++)
            for (Int_t iTrigger = 0; iTrigger < fTriggers.size(); iTrigger++) {
                TCanvas* d = new TCanvas("d", "d", 1200, 800);
                TCanvas* e = new TCanvas("e", "e", 1200, 800);
                d->Divide(4, 5);
                e->Divide(4, 5);

                const Int_t nPads = 20;

                for (Int_t iPad = 1; iPad < nPads + 1; iPad++) {
                    Int_t idx = (iPad - 1) % 4;
                    Int_t currTarget = (iPad - 1) / 4;

                    if (currTarget + 1 > fTargets.size())
                        continue;

                    TH1F* histo1 = h5[iBeam][currTarget][iTrigger][2 * idx];
                    TH1F* histo2 = h5[iBeam][currTarget][iTrigger][2 * idx + 1];

                    Double_t max1 = histo1->GetMaximum();
                    Double_t max2 = histo2->GetMaximum();

                    d->cd(iPad);
                    if (max1 > max2) {
                        histo1->Draw();
                        histo2->Draw("same");
                    } else {
                        histo2->Draw();
                        histo1->Draw("same");
                    }

                    TFitResultPtr resX = histo1->Fit("gaus", "RSQww", "", -3., +3.);
                    if (resX) {
                        histo1->GetFunction("gaus")->SetLineColor(kBlack);
                        histo1->GetFunction("gaus")->SetLineStyle(kDashed);
                    }

                    TFitResultPtr resY = histo2->Fit("gaus", "RSQww", "", -3., +3.);
                    if (resY) {
                        histo2->GetFunction("gaus")->SetLineColor(kBlack);
                        histo2->GetFunction("gaus")->SetLineStyle(kDashed);
                    }

                    TH2F* histo3 = fitResults[iBeam][currTarget][iTrigger][idx];

                    if (resX && resY) {
                        histo3->SetBinContent(1, 2, resX->Parameter(1));
                        histo3->SetBinContent(2, 2, resY->Parameter(1));
                        histo3->SetBinContent(1, 1, resX->Parameter(2));
                        histo3->SetBinContent(2, 1, resY->Parameter(2));
                    }


                    e->cd(iPad)->SetLeftMargin(0.2);
                    histo3->Draw("TEXT");
                }

                TString targPath = "";
                for (auto it : fTargets)
                    targPath += it;

                d->SaveAs(Form("beam_%s_targ_%s_trigger_%s_matchingResiduals.%s", fBeams[iBeam].Data(), targPath.Data(), fTriggers[iTrigger].Data(), usedOutExtension.Data()));
                delete d;

                e->SaveAs(Form("beam_%s_targ_%s_trigger_%s_matchingFitResults.%s", fBeams[iBeam].Data(), targPath.Data(), fTriggers[iTrigger].Data(), usedOutExtension.Data()));
                delete e;
            }

    }

    if (anal2)
        for (Int_t iBeam = 0; iBeam < fBeams.size(); iBeam++)
            for (Int_t iTrigger = 0; iTrigger < fTriggers.size(); iTrigger++) {

                TCanvas* a = new TCanvas("a", "a", 1200, 800);
                a->Divide(5, 5);

                // const Int_t nPads = 5 * fTargets.size();
                const Int_t nPads = 25;

                for (Int_t iPad = 1; iPad < nPads + 1; iPad++) {
                    a->cd(iPad);

                    Int_t idx = iPad - 1;

                    Int_t currTarget = idx / 5;

                    if (currTarget + 1 > fTargets.size())
                        continue;

                    Int_t currDist = idx % 5;

                    TH1F* histo = h[iBeam][currTarget][iTrigger][currDist];

                    if (histo) {
                        histo->Draw();
                        Double_t integral = histo->Integral();

                        if (integral > 0.)
                            for (Int_t iBin = 1; iBin < histo->GetNbinsX() + 1; iBin++)
                                histo->SetBinContent(iBin, 1. * histo->GetBinContent(iBin) / integral);
                    }
                }

                TString targPath = "";
                for (auto it : fTargets)
                    targPath += it;

                a->SaveAs(Form("beam_%s_targ_%s_trigger_%s.%s", fBeams[iBeam].Data(), targPath.Data(), fTriggers[iTrigger].Data(), usedOutExtension.Data()));
                delete a;
            }

    if (anal1) {
        TCanvas* b = new TCanvas("b", "b", 1200, 800);
        b->Divide(2, 1);

        b->cd(1)->SetGrid();
        h2->Draw("TEXT");

        b->cd(2)->SetGrid();
        h3->Draw("TEXT");

        b->SaveAs(Form("tmp.%s", usedOutExtension.Data()));
    }
}