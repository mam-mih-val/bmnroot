

#include <stdio.h>
#include <fstream>
#include <istream>
#include <iostream>
#include <ctime>
#include <vector>

#include <TMath.h>
#include "TChain.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TList.h"
#include "TDirectory.h"
#include "TPad.h"
#include "TClonesArray.h"
#include "TVector3.h"

vector <TF1 *> vTrackZX;
vector <TF1 *> vTrackZY;
vector <FairTrackParam *> vLast;
vector <FairTrackParam *> vFirst;
vector <BmnTofHit *> vTofHits;

void Tof400Alignment(TString NameFileIn = "bmndst.root", Int_t nEvForRead = 0) {

    TStopwatch timer;
    timer.Start();
    Int_t nBytes = 0;
    Bool_t FlagGoodSeed;

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    //gStyle->SetOptStat(kFALSE);

    TH2I *hZX = new TH2I("hZX", "hZX", 1200, -50., 550., 520, -130., 130.);
    hZX->GetXaxis()->SetTitle("Z, cm");
    hZX->GetYaxis()->SetTitle("X, cm");
    hZX->SetMarkerStyle(8);
    hZX->SetMarkerColor(2);

    TH2I *hZX_GemHit = new TH2I("hZX_GemHit", "hZX_GemHit", 1200, -50., 550., 520, -130., 130.);
    hZX_GemHit->GetXaxis()->SetTitle("Z, cm");
    hZX_GemHit->GetYaxis()->SetTitle("X, cm");
    hZX_GemHit->SetMarkerStyle(34);
    hZX_GemHit->SetMarkerColor(1);

    TH2I *hZY = new TH2I("hZY", "hZY", 1200, -50., 550., 520, -130., 130.);
    hZY->GetXaxis()->SetTitle("Z, cm");
    hZY->GetYaxis()->SetTitle("Y, cm");
    hZY->SetMarkerStyle(8);
    hZY->SetMarkerColor(2);

    TH2I *hZY_GemHit = new TH2I("hZY_GemHit", "hZY_GemHit", 1200, -50., 550., 520, -130., 130.);
    hZY_GemHit->GetXaxis()->SetTitle("Z, cm");
    hZY_GemHit->GetYaxis()->SetTitle("X, cm");
    hZY_GemHit->SetMarkerStyle(34);
    hZY_GemHit->SetMarkerColor(1);

    TH2I *hXY_Tof = new TH2I("hXY_Tof", "hXY_Tof", 240, -150., 150., 150, -75., 75.);
    hXY_Tof->GetXaxis()->SetTitle("X, cm");
    hXY_Tof->GetYaxis()->SetTitle("Y, cm");
    hXY_Tof->SetMarkerStyle(8);
    hXY_Tof->SetMarkerColor(2);

    TH2I *hXY_Gem = new TH2I("hXY_Gem", "hXY_Gem", 240, -150., 150., 150, -75., 75.);
    hXY_Gem->GetXaxis()->SetTitle("X, cm");
    hXY_Gem->GetYaxis()->SetTitle("Y, cm");
    hXY_Gem->SetMarkerStyle(8);
    hXY_Gem->SetMarkerColor(1);

    TH2I *hXY_Tof_PeakX1 = new TH2I("hXY_Tof_PeakX1", "hXY_Tof_PeakX1", 240, -150., 150., 150, -75., 75.);
    hXY_Tof_PeakX1->GetXaxis()->SetTitle("X, cm");
    hXY_Tof_PeakX1->GetYaxis()->SetTitle("Y, cm");

    TH2I *hXY_Tof_PeakX2 = new TH2I("hXY_Tof_PeakX2", "hXY_Tof_PeakX2", 240, -150., 150., 150, -75., 75.);
    hXY_Tof_PeakX2->GetXaxis()->SetTitle("X, cm");
    hXY_Tof_PeakX2->GetYaxis()->SetTitle("Y, cm");

    TH1I *hDeviationX = new TH1I("hDeviationX", "hDeviationX", 800, -200., 200.);
    hDeviationX->GetXaxis()->SetTitle("dX, cm");
    hDeviationX->SetLineWidth(3);
    hDeviationX->SetLineColor(4);
    TH1I *hDeviationY = new TH1I("hDeviationY", "hDeviationY", 800, -200., 200.);
    hDeviationY->GetXaxis()->SetTitle("dY, cm");
    hDeviationY->SetLineWidth(3);
    hDeviationY->SetLineColor(4);
    TH1I *hDeviation = new TH1I("hDeviation", "hDeviation", 402, -1., 200.);
    hDeviation->GetXaxis()->SetTitle("dY, cm");
    hDeviation->SetLineWidth(3);
    hDeviation->SetLineColor(4);

    TH1I *hSeed = new TH1I("hSeed", "hSeed", 20, 0, 20.);
    hSeed->GetXaxis()->SetTitle("N Seeds for aligment");
    hSeed->SetLineWidth(2);
    hSeed->SetLineColor(4);

    TH1I *hTargetX = new TH1I("hTargetX", "hTargetX", 80, -10., 10.);
    hTargetX->GetXaxis()->SetTitle("X, cm");
    TH1I *hTargetY = new TH1I("hTargetY", "hTargetY", 80, -10., 10.);
    hTargetY->GetXaxis()->SetTitle("Y, cm");
    TH1I *hTargetZ_FromX = new TH1I("hTargetZ_FromX", "hTargetZ_FromX", 500, -150., 100.);
    hTargetZ_FromX->GetXaxis()->SetTitle("Z, cm");
    TH1I *hTargetZ_FromY = new TH1I("hTargetZ_FromY", "hTargetZ_FromY", 500, -150., 100.);
    hTargetZ_FromY->GetXaxis()->SetTitle("Z, cm");
    TH2I *hTargetXY = new TH2I("hTargetXY", "hTargetXY", 80, -10., 10., 80, -10., 10.);
    hTargetXY->GetXaxis()->SetTitle("X, cm");
    hTargetXY->GetYaxis()->SetTitle("Y, cm");

    TCanvas *c_1 = new TCanvas("c1", "c1", 20, 20, 1500, 750);
    TPad *Pad_ZX = new TPad("Pad_ZX", "Pad_ZX", 0.01, 0.51, 0.49, 0.99, 21);
    TPad *Pad_ZY = new TPad("Pad_ZY", "Pad_ZY", 0.01, 0.01, 0.49, 0.49, 21);
    TPad *Pad_XY_on_Tof = new TPad("Pad_XY_on_Tof", "Pad_XY_on_Tof", 0.51, 0.51, 0.99, 0.99, 21);
    TPad *Pad_DeviationX = new TPad("Pad_DeviationX", "Pad_DeviationX", 0.51, 0.01, 0.74, 0.49, 21);
    TPad *Pad_DeviationY = new TPad("Pad_DeviationY", "Pad_DeviationY", 0.74, 0.01, 0.99, 0.49, 21);

    Pad_ZX->Draw();
    Pad_ZX->SetGridy();
    Pad_ZX->SetGridx();
    Pad_ZY->Draw();
    Pad_ZX->SetGridy();
    Pad_ZY->SetGridx();
    Pad_XY_on_Tof->Draw();
    Pad_XY_on_Tof->SetGridy();
    Pad_XY_on_Tof->SetGridx();
    Pad_DeviationX->Draw();
    Pad_DeviationX->SetGridy();
    Pad_DeviationX->SetGridx();
    Pad_DeviationY->Draw();
    Pad_DeviationY->SetGridy();
    Pad_DeviationY->SetGridx();

    //--------Data Analysis-----------------------------------------------------------------

    TChain *eveTree = new TChain("cbmsim");
    TString inName = Form("$VMCWORKDIR/macro/run/%s", NameFileIn.Data());
    //inName = file;
    cout << "Open file " << inName << endl << endl;
    eveTree->Add(inName);

    TClonesArray *Tof400Hits;
    eveTree->SetBranchAddress("BmnTof400Hit", &Tof400Hits);

    TClonesArray *GemSeeds;
    eveTree->SetBranchAddress("BmnGemTrack", &GemSeeds);
    //eveTree->SetBranchAddress("BmnGemSeed", &GemSeeds);

    TClonesArray *GemStripHits;
    eveTree->SetBranchAddress("BmnGemStripHit", &GemStripHits);

    Long64_t nEvents = eveTree->GetEntries();
    if (nEvForRead == 0 || nEvForRead > nEvents) nEvForRead = nEvents;
    cout << "Will be readed " << nEvForRead << " events from " << nEvents << endl;

    for (Int_t iEv = 0; iEv < nEvForRead; iEv++) {

        if (iEv % 50000 == 0)
            cout << "EVENT: " << iEv << endl;
        nBytes = eveTree->GetEntry(iEv);

        Int_t nSeeds = GemSeeds->GetEntriesFast();
        Int_t nHits = Tof400Hits->GetEntriesFast();
        //cout << "GemSeeds = " << nSeeds;
        //cout << "; Tof400Hits = " << nHits << endl;

        //if (nHits == nSeeds && nSeeds != 0) {
        // if (nHits == 1 && nSeeds == 1) {
        if (nHits != 0 && nSeeds != 0) {
            //    cout << endl << "EVENT: " << iEv << endl;
            //    cout << "GemSeeds = " << nSeeds;
            //    cout << "; Tof400Hits = " << nHits << endl;

            hZX->GetListOfFunctions()->Clear("nodelete");
            hZX->Reset();
            hZX_GemHit->Reset();
            hZY->GetListOfFunctions()->Clear("nodelete");
            hZY->Reset();
            hZY_GemHit->Reset();
            hXY_Tof->Reset();
            hXY_Gem->Reset();

            for (Int_t i = 0; i < vTrackZX.size(); i++) {
                vTrackZX[vTrackZX.size() - i - 1]->~TF1();
                vTrackZY[vTrackZX.size() - i - 1]->~TF1();
            }
            vTrackZX.clear();
            vTrackZY.clear();
            vLast.clear();
            vFirst.clear();
            vTofHits.clear();
            vTrackZX.reserve(nSeeds);
            vTrackZY.reserve(nSeeds);
            vLast.reserve(nSeeds);
            vFirst.reserve(nSeeds);
            vTofHits.reserve(nHits);

            //cout << "Start seedGem" << endl;
            for (Int_t i = 0; i < nSeeds; i++) {
                //cout << "read seed# " << i << endl;
                BmnGemTrack* seedGem = (BmnGemTrack*) GemSeeds->At(i);
                FairTrackParam *pLast = seedGem->GetParamLast();
                FairTrackParam *pFirst = seedGem->GetParamFirst();
                vLast.push_back(pLast);
                vFirst.push_back(pFirst);

                TF1 *f1 = new TF1(Form("ZX%d", i), "[0]*(x-[1])+[2]", vLast[i]->GetZ(), 550);
                f1->SetParameters(vLast[i]->GetTx(), vLast[i]->GetZ(), vLast[i]->GetX());
                f1->SetLineColor(1);
                f1->SetLineWidth(2);
                vTrackZX.push_back(f1);
                //vTrackZX[i] = new TF1(Form("ZX%d", i), "[0]*(x-[1])+[2]", -50, 550);
                //vTrackZX[i] = new TF1(Form("ZX%d", i), "[0]*(x-[1])+[2]", vLast[i]->GetZ(), 550);
                //vTrackZX[i]->SetParameters(vLast[i]->GetTx(), vLast[i]->GetZ(), vLast[i]->GetX());
                //vTrackZX[i]->SetLineColor(1);
                //vTrackZX[i]->SetLineWidth(2);
                hZX->GetListOfFunctions()->Add(vTrackZX[i]); //*/

                TF1 *f2 = new TF1(Form("ZY%d", i), "[0]*(x-[1])+[2]", vLast[i]->GetZ(), 550);
                f2->SetParameters(vLast[i]->GetTy(), vLast[i]->GetZ(), vLast[i]->GetY());
                f2->SetLineColor(1);
                f2->SetLineWidth(2);
                vTrackZY.push_back(f2);
                //vTrackZY[i] = new TF1(Form("ZY%d", i), "[0]*(x-[1])+[2]", -50, 550);
                //vTrackZY[i] = new TF1(Form("ZY%d", i), "[0]*(x-[1])+[2]", vLast[i]->GetZ(), 550);
                //vTrackZY[i]->SetParameters(vLast[i]->GetTy(), vLast[i]->GetZ(), vLast[i]->GetY());
                //vTrackZY[i]->SetLineColor(1);
                //vTrackZY[i]->SetLineWidth(2);
                hZY->GetListOfFunctions()->Add(vTrackZY[i]);

                hXY_Gem->Fill(vTrackZX[i]->Eval(450), vTrackZY[i]->Eval(450)); //*/
            }
            // cout << "End seedGem Analysis" << endl;
            FlagGoodSeed = kTRUE;
            if (nSeeds > 0) {
                for (Int_t i = 0; i < nSeeds; i++) {
                    //if (TMath::Abs(vTrackZX[i]->Eval(-21.7) - 0.41.) > (0.76 * 3.) || TMath::Abs(vTrackZY[i]->Eval(-21.7) - (-3.92)) > (0.65 * 3.) || (vLast[i]->GetTx()) < 0.)
                    //    FlagGoodSeed = kFALSE;

                    hTargetX->Fill(vTrackZX[i]->Eval(-21.7));
                    hTargetY->Fill(vTrackZY[i]->Eval(-21.7));
                    hTargetZ_FromX->Fill((0.41 - vLast[i]->GetX()) / vLast[i]->GetTx() + vLast[i]->GetZ());
                    hTargetZ_FromY->Fill((-3.92 - vLast[i]->GetY()) / vLast[i]->GetTy() + vLast[i]->GetZ());
                    hTargetXY->Fill(vTrackZX[i]->Eval(-21.7), vTrackZY[i]->Eval(-21.7));
                }
            }//*/

            if (FlagGoodSeed == kTRUE) {

                hSeed->Fill(nSeeds);

                //    cout << "Start Tof400Hits" << endl;
                for (Int_t i = 0; i < nHits; i++) {
                    BmnTofHit* hitTof400 = (BmnTofHit*) Tof400Hits->At(i);
                    vTofHits[i] = hitTof400;
                    vTofHits.push_back(hitTof400);
                    hZX->Fill(hitTof400->GetZ(), hitTof400->GetX());
                    hZY->Fill(hitTof400->GetZ(), hitTof400->GetY());
                    hXY_Tof->Fill(hitTof400->GetX(), hitTof400->GetY());
                    //hitTof400->Delete();
                }

                for (Int_t iseed = 0; iseed < nSeeds; iseed++) {
                    Double_t MinDev = 1000., MinDevY = 1000., MinDevX = 1000., tempx, tempy, dev;
                    Int_t NearHit = -1;
                    for (Int_t itof = 0; itof < nHits; itof++) {
                        if (vTofHits[itof]->IsUsed() == kFALSE) {
                            tempx = vTrackZX[iseed]->Eval(vTofHits[itof]->GetZ()) - vTofHits[itof]->GetX();
                            tempy = vTrackZY[iseed]->Eval(vTofHits[itof]->GetZ()) - vTofHits[itof]->GetY();
                            if (MinDevX > tempx)
                                MinDevX = tempx;
                            if (MinDevY > tempy)
                                MinDevY = tempy;
                            dev = TMath::Sqrt(tempx * tempx + tempy * tempy);
                            if (MinDev > dev) {
                                MinDev = dev;
                                NearHit = itof;
                            }
                        }
                    }
                    hDeviationX->Fill(MinDevX);
                    hDeviationY->Fill(MinDevY);

                    if (NearHit != -1) {
                        vTofHits[NearHit]->SetUsing(1);
                        hDeviation->Fill(MinDev);
                        if (TMath::Abs(MinDev) <= 4.) {
                            hXY_Tof_PeakX1->Fill(vTofHits[NearHit]->GetX(), vTofHits[NearHit]->GetY());
                        }
                        if (TMath::Abs(MinDev) > 4.)
                            hXY_Tof_PeakX2->Fill(vTofHits[NearHit]->GetX(), vTofHits[NearHit]->GetY());
                    }
                }//*/
                //    cout << "End Tof400Hits Analysis" << endl;

                //    cout << "Start GemStripHits" << endl;
                for (Int_t i = 0; i < GemStripHits->GetEntriesFast(); i++) {
                    BmnGemStripHit* hit = (BmnGemStripHit*) GemStripHits->At(i);
                    hZX_GemHit->Fill(hit->GetZ(), hit->GetX());
                    hZY_GemHit->Fill(hit->GetZ(), hit->GetY());

                }//*/
                //   cout << "End GemStripHits Analysis" << endl;

             /*   {
                    cout << "Draw event# " << iEv << endl;

                    Pad_ZX->cd();
                    hZX->Draw();
                    hZX_GemHit->Draw("SAME");

                    Pad_ZY->cd();
                    hZY->Draw();
                    hZY_GemHit->Draw("SAME");

                    Pad_XY_on_Tof->cd();
                    hXY_Tof->Draw();
                    hXY_Gem->Draw("SAME");

                    Pad_DeviationX->cd();
                    hDeviationX->Draw();

                    Pad_DeviationY->cd();
                    hDeviationY->Draw();

                    c_1->Update();
                    getchar();
                }//End Draw */
            } // end if (FlagGoodSeed == kTRUE)

        } // end if (nHits != 0 && nSeeds != 0)

    }

    /*   TF1 *fDxFit = new TF1("fDxFit", "gaus(0) + gaus(3) + pol2(6)", 15., 45.);
       fDxFit->SetParNames("Const1", "Mean1", "Sigma1", "Const2", "Mean2", "Sigma2", "p0", "p1", "p2");
       hDeviationX->GetXaxis()->SetRangeUser(15, 45);
       Double_t max = hDeviationX->GetBinContent(hDeviationX->GetMaximumBin());
       cout << "max Dx = " << max;
       hDeviationX->GetXaxis()-> UnZoom();
       fDxFit->SetParameters(max / 4., 27., 1.3, max, 33.2, 1.3, 0., 1., -0.1);
       fDxFit->SetParLimits(1, 26., 29.); // mean of first gaus
       fDxFit->SetParLimits(2, 0.1, 2.); // sigma of first gaus
       fDxFit->SetParLimits(4, 32., 35.5); // mean of second gaus
       fDxFit->SetParLimits(5, 0.1, 2.); // sigma of second gaus
       hDeviationX->Fit(fDxFit, "BR");
       hDeviationX->Draw();//*/

    TF1 *fDxFit = new TF1("fDxFit", "gaus(0) + pol2(3)", -8., 8.);
    fDxFit->SetParNames("Const1", "Mean1", "Sigma1", "p0", "p1", "p2");
    hDeviationX->GetXaxis()->SetRangeUser(-8., 8.);
    Double_t max = hDeviationX->GetBinContent(hDeviationX->GetMaximumBin());
    cout << "max Dx = " << max;
    hDeviationX->GetXaxis()-> UnZoom();
    fDxFit->SetParameters(max, 0., 1.3, 0., 1., -0.1);
    fDxFit->SetParLimits(1, -3., 3.); // mean of first gaus
    fDxFit->SetParLimits(2, 0.1, 2.); // sigma of first gaus
    hDeviationX->Fit(fDxFit, "BR");
    hDeviationX->Draw();


    /*   TF1 *fDyFit = new TF1("fDyFit", "gaus(0) + pol3(3)", -20., 15.);
       fDyFit->SetParNames("Const1", "Mean1", "Sigma1", "p0", "p1", "p2", "p3");
       hDeviationY->GetXaxis()->SetRangeUser(-20, 15);
       max = hDeviationY->GetBinContent(hDeviationY->GetMaximumBin());
       cout << "max Dy = " << max;
       hDeviationY->GetXaxis()-> UnZoom();
       fDyFit->SetParameters(max, -3.5, 0.8, 10, -1.3, -0.1, 0.01);
       fDyFit->SetParLimits(1, -5., -2.); // mean of first gaus
       fDyFit->SetParLimits(2, 0.1, 2.); // sigma of first gaus
       hDeviationY->Fit(fDyFit, "BR");//*/

    TF1 *fDyFit = new TF1("fDyFit", "gaus(0) + pol3(3)", -15., 15.);
    fDyFit->SetParNames("Const1", "Mean1", "Sigma1", "p0", "p1", "p2", "p3");
    hDeviationY->GetXaxis()->SetRangeUser(-15, 15);
    max = hDeviationY->GetBinContent(hDeviationY->GetMaximumBin());
    cout << "max Dy = " << max;
    hDeviationY->GetXaxis()-> UnZoom();
    fDyFit->SetParameters(max, 0., 0.8, 10, -1.3, -0.1, 0.001);
    fDyFit->SetParLimits(1, -3., 3.); // mean of first gaus
    fDyFit->SetParLimits(2, 0.1, 2.); // sigma of first gaus
    hDeviationY->Fit(fDyFit, "BR");

    ofstream f_time;
    TString NameTimeFile = "Tof400Aligment.dat";
    cout << "Write time to " << NameTimeFile.Data() << endl;
    f_time.open(NameTimeFile.Data(), std::ofstream::out | std::ofstream::app);
    f_time << endl << NameFileIn << endl;
    f_time << "\tX_Aligment\n\t\t" << fDxFit->GetParameter("Mean1") << " +- " << fDxFit->GetParameter("Sigma1") << endl;
    //f_time << "\t\t" << fDxFit->GetParameter("Mean2") << " +- " << fDxFit->GetParameter("Sigma2") << endl;
    f_time << "\tY_Aligment\n\t\t" << fDyFit->GetParameter("Mean1") << " +- " << fDyFit->GetParameter("Sigma1") << endl;
    f_time.close();//

    timer.Stop();
    cout << "Readed " << nEvForRead << " events" << endl;
    cout << "Time          = " << timer.RealTime() << " s" << endl;
    cout << "Time/Event    = " << timer.RealTime() / (Double_t) nEvForRead * 1000. << " ms/Event" << endl;
    cout << "TimeCPU       = " << timer.CpuTime() << " s" << endl;
    cout << "TimeCPU/Event = " << timer.CpuTime() / (Double_t) nEvForRead * 1000. << " ms/Event" << endl;

    timer.Reset();
    timer.Start();
    TString outName = NameFileIn;
    Int_t Point = outName.First('.');
    outName.Replace(Point, 15, "_Aligment.root");
    outName = Form("/home/storage/analysis/%s", outName.Data());
    cout << "Save data to " << outName.Data() << endl;
    TFile *fileout = new TFile(outName.Data(), "RECREATE");

    hSeed->Write();
    hDeviationX->Write();
    hDeviationY->Write();
    hDeviation->Write();
    hXY_Tof_PeakX1->Write();
    hXY_Tof_PeakX2->Write();
    hTargetX->Write();
    hTargetY->Write();
    hTargetZ_FromX->Write();
    hTargetZ_FromY->Write();
    hTargetXY->Write();

    fileout->Close();

    timer.Stop();

    cout << "Time for write root = " << timer.RealTime() << " s" << endl;

}
