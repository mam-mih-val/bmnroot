#include "BmnAdcProcessor.h"
#include <BmnMath.h>

BmnAdcProcessor::BmnAdcProcessor() {
}

BmnAdcProcessor::BmnAdcProcessor(Int_t period, Int_t run, TString det, Int_t nCh, Int_t nSmpl, vector<UInt_t> vSer) {

    fPeriod = period;
    fRun = run;
    fDetName = det;
    fNSerials = vSer.size();
    fNChannels = nCh;
    fNSamples = nSmpl;
    fAdcSerials = vSer;
    for (int iSer = 0; iSer < fAdcSerials.size(); ++iSer) {
        fSerMap.insert(pair<UInt_t, Int_t>(fAdcSerials[iSer], iSer));
        //        printf("iser %d ser 0x%08X  val %d  \n", iSer, fAdcSerials[iSer], fSerMap[fAdcSerials[iSer]]);
    }
    Int_t high = 120;
    Int_t highcms = 500;

    fPedVal = new Double_t**[fNSerials];
    fPedValTemp = new Double_t**[fNSerials];
    fNvals = new UInt_t*[fNSerials];
    fNvalsCMod = new UInt_t**[fNSerials];
    fNvalsADC = new UInt_t**[fNSerials];
    fPedRms = new Double_t**[fNSerials];
    fPedCMod = new Double_t**[fNSerials];
    fPedCMod2 = new Double_t**[fNSerials];
    fSumRmsV = new Double_t*[fNSerials];
    fNoisyChipChannels = new Bool_t**[fNSerials];
    fCMode = new Double_t*[fNSerials];
    fCMode0 = new Double_t*[fNSerials];
    fSMode = new Double_t*[fNSerials];
    fSMode0 = new Double_t*[fNSerials];
    fAdcProfiles = new UInt_t**[fNSerials];
    fAdc = new Double_t**[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fPedVal[iCr] = new Double_t*[fNChannels];
        fPedValTemp[iCr] = new Double_t*[fNChannels];
        fNvals[iCr] = new UInt_t[fNChannels];
        memset(fNvals[iCr], 0, sizeof (UInt_t) * fNChannels);
        fNvalsCMod[iCr] = new UInt_t*[fNChannels];
        fNvalsADC[iCr] = new UInt_t*[fNChannels];
        fPedRms[iCr] = new Double_t*[fNChannels];
        fPedCMod[iCr] = new Double_t*[fNChannels];
        fPedCMod2[iCr] = new Double_t*[fNChannels];
        fSumRmsV[iCr] = new Double_t[fNChannels];
        memset(fSumRmsV[iCr], 0.0, sizeof (Double_t) * fNChannels);
        fNoisyChipChannels[iCr] = new Bool_t*[fNChannels];
        fAdcProfiles[iCr] = new UInt_t*[fNChannels];
        fAdc[iCr] = new Double_t*[fNChannels];
        fCMode[iCr] = new Double_t[fNChannels];
        fCMode0[iCr] = new Double_t[fNChannels];
        fSMode[iCr] = new Double_t[fNChannels];
        fSMode0[iCr] = new Double_t[fNChannels];
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            fPedVal[iCr][iCh] = new Double_t[fNSamples];
            fPedValTemp[iCr][iCh] = new Double_t[fNSamples];
            fNvalsADC[iCr][iCh] = new UInt_t[fNSamples];
            memset(fNvalsADC[iCr][iCh], 0, sizeof (UInt_t) * fNSamples);
            fNvalsCMod[iCr][iCh] = new UInt_t[fNSamples];
            memset(fNvalsCMod[iCr][iCh], 0, sizeof (UInt_t) * fNSamples);
            fPedRms[iCr][iCh] = new Double_t[fNSamples];
            fPedCMod[iCr][iCh] = new Double_t[fNSamples];
            fPedCMod2[iCr][iCh] = new Double_t[fNSamples];
            fNoisyChipChannels[iCr][iCh] = new Bool_t[fNSamples];
            fAdcProfiles[iCr][iCh] = new UInt_t[fNSamples];
            fAdc[iCr][iCh] = new Double_t[fNSamples];
            fCMode[iCr][iCh] = 0.0;
            fCMode0[iCr][iCh] = 0.0;
            fSMode[iCr][iCh] = 0.0;
            fSMode0[iCr][iCh] = 0.0;
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                fPedVal[iCr][iCh][iSmpl] = 0.0;
                fPedValTemp[iCr][iCh][iSmpl] = 0.0;
                fPedRms[iCr][iCh][iSmpl] = 0.0;
                fPedCMod[iCr][iCh][iSmpl] = 0.0;
                fPedCMod2[iCr][iCh][iSmpl] = 0.0;
                fNoisyChipChannels[iCr][iCh][iSmpl] = kFALSE;
                fAdcProfiles[iCr][iCh][iSmpl] = 0;
                fAdc[iCr][iCh][iSmpl] = 0.0;
            }
        }
    }

    fPedDat = new Double_t***[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fPedDat[iCr] = new Double_t**[N_EV_FOR_PEDESTALS];
        for (UInt_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
            fPedDat[iCr][iEv] = new Double_t*[fNChannels];
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                fPedDat[iCr][iEv][iCh] = new Double_t[fNSamples];
                for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl)
                    fPedDat[iCr][iEv][iCh][iSmpl] = 0.0;
            }
        }
    }
//    Int_t nevents = N_EV_FOR_PEDESTALS - 2;
//    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
//        vector<TH1*> hv;
//        vector<TH1*> hcm;
//        vector<TH1*> hsm;
//        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
//            TString hname = TString(Form("%08X:%02d pedestals SM ev", fAdcSerials[iCr], iCh));
//            TH1* h = new TH2F(hname, hname,
//                    N_EV_FOR_PEDESTALS, 0, N_EV_FOR_PEDESTALS,
//                    fNSamples, 0, fNSamples);
//            h->GetXaxis()->SetTitle("Event #");
//            h->GetYaxis()->SetTitle("Sample(channel) #");
//            hv.push_back(h);
//
//            hname = TString(Form("%08X:%02d cmods SM ev", fAdcSerials[iCr], iCh));
//            TH1* hc = new TH1F(hname, hname, nevents, 0, nevents);
//            hc->GetXaxis()->SetTitle("Event #");
//            hcm.push_back(hc);
//
//            hname = TString(Form("%08X:%02d smods SM ev", fAdcSerials[iCr], iCh));
//            TH1* hs = new TH1F(hname, hname, nevents, 0, nevents);
//            hs->GetXaxis()->SetTitle("Event #");
//            hsm.push_back(hs);
//        }
//        hPedLine.push_back(hv);
//        hCMode.push_back(hcm);
//        hSMode.push_back(hsm);
//    }
}

BmnAdcProcessor::~BmnAdcProcessor() {
    for (Int_t iCr = 0; iCr < fNSerials; iCr++) {
        for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; iEv++) {
            for (Int_t iCh = 0; iCh < fNChannels; iCh++)
                delete[] fPedDat[iCr][iEv][iCh];
            delete[] fPedDat[iCr][iEv];
        }
        delete[] fPedDat[iCr];
    }
    delete[] fPedDat;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            delete[] fPedVal[iCr][iCh];
            delete[] fPedValTemp[iCr][iCh];
            delete[] fNvalsCMod[iCr][iCh];
            delete[] fNvalsADC[iCr][iCh];
            delete[] fPedRms[iCr][iCh];
            delete[] fPedCMod[iCr][iCh];
            delete[] fPedCMod2[iCr][iCh];
            delete[] fAdcProfiles[iCr][iCh];
            delete[] fAdc[iCr][iCh];
            delete[] fNoisyChipChannels[iCr][iCh];
        }
        delete[] fPedVal[iCr];
        delete[] fPedValTemp[iCr];
        delete[] fNvalsADC[iCr];
        delete[] fNvals[iCr];
        delete[] fNvalsCMod[iCr];
        delete[] fPedRms[iCr];
        delete[] fPedCMod[iCr];
        delete[] fPedCMod2[iCr];
        delete[] fSumRmsV[iCr];
        delete[] fAdcProfiles[iCr];
        delete[] fAdc[iCr];
        delete[] fCMode[iCr];
        delete[] fCMode0[iCr];
        delete[] fSMode[iCr];
        delete[] fSMode0[iCr];
        delete[] fNoisyChipChannels[iCr];
    }
    delete[] fPedVal;
    delete[] fPedValTemp;
    delete[] fNvalsADC;
    delete[] fNvals;
    delete[] fNvalsCMod;
    delete[] fPedRms;
    delete[] fPedCMod;
    delete[] fPedCMod2;
    delete[] fSumRmsV;
    delete[] fAdcProfiles;
    delete[] fAdc;
    delete[] fCMode;
    delete[] fCMode0;
    delete[] fSMode;
    delete[] fSMode0;
    delete[] fNoisyChipChannels;
    /*canStrip->cd(1);
    h->Draw("colz");
    canStrip->cd(2);
    hp->Draw("colz");
    canStrip->cd(3);
    hcms->Draw("colz");
    canStrip->cd(4);
    hscms_adc->Draw("colz");
    canStrip->cd(5);
    hcms1p->Draw("colz");
    canStrip->cd(6);
    hscms1p_adc->Draw("colz");
    canStrip->cd(7);
    hcms1->Draw("colz");
    canStrip->cd(8);
    hscms1_adc->Draw("colz");
    canStrip->SaveAs("can-cms.png");*/
    //    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
    //        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
    //            delete hPedLine[iCr][iCh];
    //            delete hSMode[iCr][iCh];
    //            delete hCMode[iCr][iCh];
    //        }
    if (fGemStationSet)
        delete fGemStationSet;
    if (fSilStationSet)
        delete fSilStationSet;
    if (fCscStationSet)
        delete fCscStationSet;
    if (statsGem)
        delete[] statsGem;
    if (statsSil)
        delete[] statsSil;
    if (statsGemPermut)
        delete[] statsGemPermut;
    if (statsSilPermut)
        delete[] statsSilPermut;
}

void BmnAdcProcessor::DrawDebugHists() {
    TString docName = TString(Form("%s-mods-ev-%d.pdf", fDetName.Data(), drawCnt));
    const UInt_t PAD_WIDTH_SIL = 1920; //8192;
    const UInt_t PAD_HEIGHT_SIL = 1080;
    //    TString docName = "sil-ped-cms.pdf";
    TCanvas *c = new TCanvas("can cms", "can", PAD_WIDTH_SIL, 2 * PAD_HEIGHT_SIL);
    c->Divide(1, 2);
    c->Print(docName + "[");
    for (int iCr = 0; iCr < fNSerials; iCr++)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            //auto *p = c->cd(iCr * fNChannels + iCh + 1);
            c->Clear("D");
            //            hPedLine[iCr][iCh]->Draw("colz");
            c->cd(1);
            hSMode[iCr][iCh]->Draw("colz");
            c->cd(2);
            hCMode[iCr][iCh]->Draw("colz");
            c->Print(docName);
            //            c->SaveAs(Form("%s.png", hPedLine[iCr][iCh]->GetName()));
        }
    c->Print(docName + "]");
    //        c->SaveAs();
    drawCnt++;
}

void BmnAdcProcessor::DrawDebugHists2D() {
    TString docName = TString(Form("%s-mods-%d.pdf", fDetName.Data(), drawCnt2d));
    const UInt_t PAD_WIDTH_SIL = 1920; //8192;
    const UInt_t PAD_HEIGHT_SIL = 1080;
    gStyle->SetOptStat(0);
    //    TColor::InvertPalette();
    gStyle->SetPalette(kDeepSea);
    TCanvas *c = new TCanvas("can cms", "can", PAD_WIDTH_SIL, 2 * PAD_HEIGHT_SIL);
    c->Divide(1, 3);
    c->Print(docName + "[");
    for (int iCr = 0; iCr < fNSerials; iCr++) {
        c->Clear("D");
        c->cd(1);
        hPedSi[iCr]->Draw("box");
        c->cd(2);
        hCModeSi[iCr]->Draw("colz");
        c->cd(3);
        hSModeSi[iCr]->Draw("colz");
        c->Print(docName);
    }
    //        c->Print(docName + "]");
    delete c;
    c = new TCanvas("can pedestals", "can", PAD_WIDTH_SIL, PAD_HEIGHT_SIL);
    //    c->Print(docName + "[");
    c->Clear("D");
    //    c->Divide(1, 2);
    for (int iCr = 0; iCr < fNSerials; iCr++)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            c->Clear("D");
            c->cd(1);
            hPedLine[iCr][iCh]->Draw("colz");
            c->Print(docName);
            //            c->SaveAs(Form("%s.png", hPedLine[iCr][iCh]->GetName()));
        }
    c->Print(docName + "]");
    delete c;
    drawCnt2d++;
}

void BmnAdcProcessor::ClearDebugHists() {
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        hPedSi[iCr]->Reset();
        hSModeSi[iCr]->Reset();
        hCModeSi[iCr]->Reset();
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            hPedLine[iCr][iCh]->Reset();
            hSMode[iCr][iCh]->Reset();
            hCMode[iCr][iCh]->Reset();
        }
    }
}

BmnStatus BmnAdcProcessor::RecalculatePedestals() {
    const UShort_t nSmpl = fNSamples;

    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                fPedVal[iCr][iCh][iSmpl] = 0.0;
                fPedRms[iCr][iCh][iSmpl] = 0.0;
                fAdcProfiles[iCr][iCh][iSmpl] = 0;
            }
        }
    //cout << fDetName << " pedestals calculation..." << endl;
    for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                Double_t signals[nSmpl];
                for (Int_t i = 0; i < nSmpl; ++i) signals[i] = 0.0;
                Int_t nOk = 0;
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fPedDat[iCr][iEv][iCh][iSmpl] == 0) continue;
                    signals[iSmpl] = fPedDat[iCr][iEv][iCh][iSmpl];
                    nOk++;
                }
                Double_t CMS = CalcCMS(signals, nOk);
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    fPedVal[iCr][iCh][iSmpl] += ((signals[iSmpl] - CMS) / N_EV_FOR_PEDESTALS);
                }
            }
    }

    //cout << fDetName << " RMS calculation..." << endl;
    for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                Double_t signals[nSmpl];
                for (Int_t i = 0; i < nSmpl; ++i) signals[i] = 0.0;
                Int_t nOk = 0;
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fPedDat[iCr][iEv][iCh][iSmpl] == 0) continue;
                    signals[iSmpl] = fPedDat[iCr][iEv][iCh][iSmpl];
                    nOk++;
                }
                Double_t CMS = CalcCMS(signals, nOk);
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    Float_t ped = fPedVal[iCr][iCh][iSmpl];
                    fPedRms[iCr][iCh][iSmpl] += ((signals[iSmpl] - CMS - ped) * (signals[iSmpl] - CMS - ped));
                }
            }
    }

    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
                fPedRms[iCr][iCh][iSmpl] = Sqrt(fPedRms[iCr][iCh][iSmpl] / N_EV_FOR_PEDESTALS);

    ofstream pedFile(Form("%s/input/%s_pedestals_%d.txt", getenv("VMCWORKDIR"), fDetName.Data(), fRun));
    pedFile << "Serial\tCh_id\tPed\tRMS" << endl;
    pedFile << "============================================" << endl;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
                pedFile << hex << fAdcSerials[iCr] << dec << "\t" << iCh * nSmpl + iSmpl << "\t" << fPedVal[iCr][iCh][iSmpl] << "\t" << fPedRms[iCr][iCh][iSmpl] << endl;
    pedFile.close();

    return kBMNSUCCESS;
}

BmnStatus BmnAdcProcessor::RecalculatePedestalsAugmented() {
    if (fVerbose)
        printf("%s %s started   niter %d  thrMax  %4.2f\n", fDetName.Data(), __func__, niter, thrMax);
    const UShort_t nSmpl = fNSamples;
    //    if (hSModeSi.size() == 0) {
    //
    //        for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
    //            vector<TH1*> hv;
    //            vector<TH1*> hcm;
    //            vector<TH1*> hsm;
    //            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
    //                //                        printf("Creating  icr %2d ich %2d %s\n", iCr, iCh, Form("%08X:%02d pedestal line MK", fSerials[iCr], iCh));
    //                TString hname = TString(Form("%08X:%02d pedestals SM", fAdcSerials[iCr], iCh));
    //                TH1* h = new TH2F(hname, hname,
    //                        500, 0, 500,
    //                        fNSamples, 0, fNSamples);
    //                h->GetXaxis()->SetTitle("Event #");
    //                h->GetYaxis()->SetTitle("Sample(channel) #");
    //                h->SetDirectory(0);
    //                hv.push_back(h);
    //            }
    //            hPedLineSi.push_back(hv);
    //        }
    //
    //        const Int_t maxAdc = 8192;
    //        const Int_t MaxSig = 2300;
    //        const Int_t RngSig = 90;
    //        const Int_t StripSi = 640;
    //        for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
    //            TString hname = TString(Form("%08X pedestals SM", fAdcSerials[iCr]));
    //            //        TH1* h = new TH2F(hname, hname, maxAdc, 0, maxAdc, MaxSig, 0, MaxSig);
    //            TH1* h = new TH2F(hname, hname, StripSi, 0, StripSi, 2 * RngSig, -RngSig, RngSig);
    //            //    printf("maxAdc %04d max %04f peds\n", maxAdc, h->GetXaxis()->GetXmax());
    //            h->GetXaxis()->SetTitle("Channel #");
    //            h->GetYaxis()->SetTitle("Signal");
    //            h->SetDirectory(0);
    //            hPedSi.push_back(h);
    //
    //            hname = TString(Form("%08X cmods SM", fAdcSerials[iCr]));
    //            TH1* hc = new TH2F(hname, hname, StripSi, 0, StripSi, 2 * RngSig, -RngSig, RngSig);
    //            //    printf("maxAdc %04d max %04f cmode\n", maxAdc, hc->GetXaxis()->GetXmax());
    //            hc->GetXaxis()->SetTitle("Channel #");
    //            hc->GetYaxis()->SetTitle("Signal");
    //            hc->SetDirectory(0);
    //            hCModeSi.push_back(hc);
    //
    //            hname = TString(Form("%08X smods SM", fAdcSerials[iCr]));
    //            TH1* hs = new TH2F(hname, hname, StripSi, 0, StripSi, 2 * RngSig, -RngSig, RngSig);
    //            hs->GetXaxis()->SetTitle("Channel #");
    //            hs->GetYaxis()->SetTitle("Signal");
    //            hs->SetDirectory(0);
    //            hSModeSi.push_back(hs);
    //        }
    //    }

    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            //            memset(fNvalsADC[iCr][iCh], 0, sizeof (UInt_t) * fNSamples);
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                fPedVal[iCr][iCh][iSmpl] = 0.0;
                fPedValTemp[iCr][iCh][iSmpl] = 0.0;
                fPedRms[iCr][iCh][iSmpl] = 0.0;
                fAdcProfiles[iCr][iCh][iSmpl] = 0;
                fNvalsADC[iCr][iCh][iSmpl] = 0;
                //                fNoisyChipChannels[iCr][iCh][iSmpl] = kFALSE;
            }
        }
    for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                Int_t nOk = 0;
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fPedDat[iCr][iEv][iCh][iSmpl] == 0.0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
                    if (Abs(fPedDat[iCr][iEv][iCh][iSmpl]) < 0.1) {
                        if (fVerbose)
                        printf("iEv %4d iCr %4d iCh %4d iSmpl %4d  fPedDat %5f\n", iEv, iCr, iCh, iSmpl, fPedDat[iCr][iEv][iCh][iSmpl]);

                    }
                    fPedVal[iCr][iCh][iSmpl] += fPedDat[iCr][iEv][iCh][iSmpl]; // / N_EV_FOR_PEDESTALS);
                    fNvalsADC[iCr][iCh][iSmpl]++;
                    nOk++;
//                    static_cast<TH2*> (hPedLine[iCr][iCh])->Fill(iEv, iSmpl, fPedDat[iCr][iEv][iCh][iSmpl]);
                }
            }
    }
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
                if (fNvalsADC[iCr][iCh][iSmpl])
                    fPedVal[iCr][iCh][iSmpl] /= fNvalsADC[iCr][iCh][iSmpl];
    // iteratively calculate pedestals and CMSs
    //    Double_t rmsthr = 200.0;
    //    Double_t rmsthrf = 200.0;
    Double_t sumRms = thrMax / 3.5;
    for (Int_t iter = -2; iter < niter; iter++) {
        //        Double_t thr = thrMax - thrDif * iter; //(2 + (nIters - iter)/2.0) * sumRms; //thrMax - thrDif * iter;
        //                Double_t thr = 3.5 * sumRms; //thrMax - thrDif * iter;
        Double_t thr = thrMax - thrDif * iter;
                        if (fVerbose)
        printf("iter %d thr %4.2f\n", iter, thr);
        UInt_t nFiltered = 0;
        // clear
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
            memset(fSumRmsV[iCr], 0.0, sizeof (Double_t) * fNChannels);
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                memset(fNvalsCMod[iCr][iCh], 0, sizeof (UInt_t) * fNSamples);
                memset(fNvalsADC[iCr][iCh], 0, sizeof (UInt_t) * fNSamples);
                memset(fPedValTemp[iCr][iCh], 0.0, sizeof (Double_t) * fNSamples);
                memset(fPedCMod[iCr][iCh], 0.0, sizeof (Double_t) * fNSamples);
                memset(fPedCMod2[iCr][iCh], 0.0, sizeof (Double_t) * fNSamples);
            }
        }
        for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS - 1; ++iEv) {
            // clear
            for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
                memset(fNvals[iCr], 0, sizeof (UInt_t) * fNChannels);
                memset(fCMode[iCr], 0.0, sizeof (Double_t) * fNChannels);
                memset(fSMode[iCr], 0.0, sizeof (Double_t) * fNChannels);
            }
            // Pedestals pre filtering
            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                        //                        if (iCr == 3 && iCh == 8) {
                        //                            printf("iter %i iEv %i fpedDat %f noise %i\n", iter, iEv, fPedDat[iCr][iEv][iCh][iSmpl], fNoisyChipChannels[iCr][iCh][iSmpl]);
                        //                        }
                        if (fPedDat[iCr][iEv][iCh][iSmpl] == 0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
                        Double_t Asig = TMath::Abs(fPedDat[iCr][iEv][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl]);
                        if (Asig < thr) {
                            fSMode[iCr][iCh] += fPedDat[iCr][iEv][iCh][iSmpl]; // CMS from current event
                            fCMode[iCr][iCh] += fPedVal[iCr][iCh][iSmpl]; // CMS over all pedestals
                            //                        fPedValTemp[iCr][iCh][iSmpl] += fPedDat[iCr][iEv][iCh][iSmpl]; // CMS from current event
                            //                        fNvalsADC[iCr][iCh][iSmpl]++;
                            fNvals[iCr][iCh]++;

                        }
                    }
                }
            // normalize
            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                    if (fNvals[iCr][iCh]) {
                        fSMode[iCr][iCh] /= fNvals[iCr][iCh];
                        fCMode[iCr][iCh] /= fNvals[iCr][iCh];
                        //                        hSMode[iCr][iCh]->SetBinContent(iEv, fSMode[iCr][iCh]);
                        //                        hCMode[iCr][iCh]->SetBinContent(iEv, fCMode[iCr][iCh]);
                    } else {
                        fSMode[iCr][iCh] = 0;
                        fCMode[iCr][iCh] = 0;
                    }
                }
            // Pedestals filtering
            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                        Double_t Adc = fPedDat[iCr][iEv][iCh][iSmpl];
                        if ((Adc == 0) || (fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE)) continue;
                        Double_t sig = fPedDat[iCr][iEv][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + fCMode[iCr][iCh] - fSMode[iCr][iCh];
                        Double_t Asig = TMath::Abs(sig);
                        if (Asig < thr) {
                            fPedValTemp[iCr][iCh][iSmpl] += fPedDat[iCr][iEv][iCh][iSmpl];
                            fNvalsADC[iCr][iCh][iSmpl]++;


                            Adc = fPedDat[iCr][iEv][iCh][iSmpl] - fSMode[iCr][iCh];
                            fPedCMod[iCr][iCh][iSmpl] += Adc;
                            fPedCMod2[iCr][iCh][iSmpl] += Adc*Adc;
                            fNvalsCMod[iCr][iCh][iSmpl]++;
                            nFiltered++;

                        }
                        //                        if ((iter == niter - 1)/* && (fAdcSerials[iCr] == 0x76C8320)*/) {
                        //                            Int_t ic = (iCh - 32) * nSmpl + iSmpl;
                        //                            hCModeSi[iCr]->Fill(ic, fCMode[iCr][iCh]);
                        //                            hSModeSi[iCr]->Fill(ic, fSMode[iCr][iCh]);
                        //                            hPedSi[iCr]->Fill(ic, fPedVal[iCr][iCh][iSmpl]);
                        //                        }
                        //                        if (iter == nIters - 1 && iCr == 0 && iCh == 9) {
                        //                            h->Fill(iSmpl, fPedDat[iCr][iEv][iCh][iSmpl]);
                        //                            hp->Fill(iSmpl, /* fPedDat[iCr][iEv][iCh][iSmpl] - */fPedVal[iCr][iCh][iSmpl]);
                        //                            //                            hcms->Fill(iSmpl, fPedDat[iCr][iEv][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] - fSigCMS[iCr][iCh] + fPedCMS0[iCr][iCh]);
                        //                            hcms->Fill(iSmpl, fPedCMS0[iCr][iCh]);
                        //                            hscms_adc->Fill(iSmpl, fSigCMS[iCr][iCh]);
                        //                            hcms1p->Fill(fPedCMS0[iCr][iCh]);
                        //                            hscms1p_adc->Fill(fSigCMS[iCr][iCh]);
                        //                        }
                    }
                }
        } // event loop

        sumRms = 0.0;
        Int_t nrms = 0;

        //hists fill
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                Int_t nvrms = 0;
                fSumRmsV[iCr][iCh] = 0.0;
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
                    if (fNvalsCMod[iCr][iCh][iSmpl]) {
                        fPedCMod[iCr][iCh][iSmpl] /= fNvalsCMod[iCr][iCh][iSmpl];
                        fPedCMod2[iCr][iCh][iSmpl] =
                                Sqrt(Abs(fPedCMod2[iCr][iCh][iSmpl] / fNvalsCMod[iCr][iCh][iSmpl] - Sq(fPedCMod[iCr][iCh][iSmpl])));
                        sumRms += fPedCMod2[iCr][iCh][iSmpl];
                        fSumRmsV[iCr][iCh] += fPedCMod2[iCr][iCh][iSmpl];
                        nrms++;
                        nvrms++;
                    }
                    if (fNvalsADC[iCr][iCh][iSmpl])
                        fPedVal[iCr][iCh][iSmpl] = fPedValTemp[iCr][iCh][iSmpl] / fNvalsADC[iCr][iCh][iSmpl];
                    else
                        fPedVal[iCr][iCh][iSmpl] = 0.0;
                    fNvalsADC[iCr][iCh][iSmpl] = 0;
                    //                    fCMode[iCr][iCh] += fPedVal[iCr][iCh][iSmpl]; // CMS over all pedestals
                }
                if (nvrms)
                    fSumRmsV[iCr][iCh] /= nvrms;
            }
        if (nrms > 0) sumRms /= nrms;

        // noise ch detection
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
                    //                    printf("icr %2d ich %2d  fSumRmsV %4f sumRms %4f\n", iCr, iCh, fSumRmsV[iCr][iCh], sumRms);
                    if (fPedCMod2[iCr][iCh][iSmpl] > 5 * sumRms) {
                        //                    if (fPedCMod2[iCr][iCh][iSmpl] > 3.5 * fSumRmsV[iCr][iCh]) {
                        fNoisyChipChannels[iCr][iCh][iSmpl] = kTRUE;
                        if (fVerbose)
                            printf("new noisy ch on  cr %i %08X ch %i smpl %i  by signal %4.2f\n", iCr, fAdcSerials[iCr], iCh, iSmpl, fPedCMod2[iCr][iCh][iSmpl]);
                    }
                }
    } // iter loop
    //    ofstream pedFile(Form("%s/input/%s_pedestals_%d.txt", getenv("VMCWORKDIR"), fDetName.Data(), fRun));
    //    pedFile << "Serial\tCh_id\tPed\tRMS" << endl;
    //    pedFile << "============================================" << endl;
    //    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
    //        for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
    //            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
    //                pedFile << hex << fAdcSerials[iCr] << dec << "\t" << iCh * nSmpl + iSmpl << "\t" << fPedVal[iCr][iCh][iSmpl] << "\t" << fPedRms[iCr][iCh][iSmpl] << endl;
    //    pedFile.close();
    return kBMNSUCCESS;
}

void BmnAdcProcessor::PrecalcEventMods(TClonesArray *adc) {
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        memset(fNvals[iCr], 0, sizeof (UInt_t) * fNChannels);
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            fCMode[iCr][iCh] = 0.0;
            fSMode[iCr][iCh] = 0.0;
            memset(fAdc[iCr][iCh], 0.0, sizeof (Double_t) * fNSamples);
            //        for (UInt_t iSmpl = 0; iSmpl < fNSamples; iSmpl++)
            //            fAdc[iCr][iCh][iSmpl] = 0.0;
        }
    }
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
        UInt_t iCh = adcDig->GetChannel();
        UInt_t ser = adcDig->GetSerial();
        //        printf("Serial %08X \n", ser);
        auto serIter = fSerMap.find(ser);
        //        printf("iter %08X end %08X\n", serIter, fSerMap.end());
        if (serIter == fSerMap.end()) {
            //            printf("Serial %08X not found in the map\n", ser);
            continue;
        }
        Int_t iCr = serIter->second;
        for (UInt_t iSmpl = 0; iSmpl < fNSamples; iSmpl++) {
            if ((fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE))
                continue;
            Double_t val = (GetRun() > GetBoundaryRun(ADC128_N_SAMPLES)) ?
                    ((Double_t) (adcDig->GetShortValue())[iSmpl] / 16) :
                    ((Double_t) (adcDig->GetUShortValue())[iSmpl] / 16);
            //                printf("    icr %d ich %d ismpl %d                 val %4f\n", iCr, iCh, iSmpl, val);
            if (fAdc[iCr][iCh][iSmpl] > 0.0)
                printf("WTF icr %d ich %d ismpl %d already %4f but val %4f\n", iCr, iCh, iSmpl, fAdc[iCr][iCh][iSmpl], val);
            fAdc[iCr][iCh][iSmpl] = val;

            Double_t Sig = fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl];
            Double_t Asig = TMath::Abs(Sig);
//                        printf("adc %6f ped %6f\n", fAdc[iCr][iCh][iSmpl], fPedVal[iCr][iCh][iSmpl]);

            if ((Asig < thrMax)) {
//                        printf("adc %6f < thrMax %6f\n", fAdc[iCr][iCh][iSmpl], thrMax);
                fSMode[iCr][iCh] += fAdc[iCr][iCh][iSmpl];
                fCMode[iCr][iCh] += fPedVal[iCr][iCh][iSmpl];
                fNvals[iCr][iCh]++;
            }
        }

    }
}

void BmnAdcProcessor::CalcEventMods() {
    // normalize
    Int_t fNvalsMin = 0;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            if (fNvals[iCr][iCh] > fNvalsMin) {
                fSMode[iCr][iCh] /= fNvals[iCr][iCh];
                fCMode[iCr][iCh] /= fNvals[iCr][iCh];
            } else {
                fSMode[iCr][iCh] = 0.0;
                fCMode[iCr][iCh] = 0.0;
            }
        }

    // filter out sigs to get mods
    for (Int_t iter = -2; iter < niter; ++iter) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                fSMode0[iCr][iCh] = 0.0;
                fCMode0[iCr][iCh] = 0.0;
                fNvals[iCr][iCh] = 0;
            }
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                //                    if (iCr == 3 && iCh == 8)
                //                        printf("iev %04i iter %i sig cms %f  cms %f\n", iEv, iter, fSigCMS[iCr][iCh], fPedCMS[iCr][iCh]);
                for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                    if (/*fPedVal[iCr][iCh][iSmpl] == 0 ||*/ fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
                    Double_t sig = fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + fCMode[iCr][iCh] - fSMode[iCr][iCh];
                    Double_t Asig = TMath::Abs(sig);
                    //                    Double_t thr = fPedRms[iCr][iCh][iSmpl];
                    Double_t thr = thrMax - iter * thrDif;
                    //                                    if (iCr == 0 && iCh == 45 && iSmpl == 33)
                    //                    printf("iter %2d  thr %6f  adc %6f  ped %6f  cmod %6f  smod %6f \n",
                    //                            iter, thr,fAdc[iCr][iCh][iSmpl],fPedVal[iCr][iCh][iSmpl],fCMode[iCr][iCh],fSMode[iCr][iCh]);
                    if (Asig < thr) {
                        fSMode0[iCr][iCh] += fAdc[iCr][iCh][iSmpl];
                        fCMode0[iCr][iCh] += fPedVal[iCr][iCh][iSmpl];
                        fNvals[iCr][iCh]++;
                        //                    rmsthrf += Sq(fPedDat[iCr][iEv][iCh][iSmpl] - fSigCMS[iCr][iCh]);

                    }
                }
            }
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                if (fNvals[iCr][iCh] > fNvalsMin) {
                    fSMode[iCr][iCh] = fSMode0[iCr][iCh] / fNvals[iCr][iCh];
                    fCMode[iCr][iCh] = fCMode0[iCr][iCh] / fNvals[iCr][iCh];
                } else {
                    fSMode[iCr][iCh] = 0.0;
                    fCMode[iCr][iCh] = 0.0;
                }
                //                if ((fAdcSerials[iCr] == 0x80BCBFC) && (iCh == 36)){
                //                            Int_t ic = iCh * GetNSamples() + iSmpl;
                //                            hCModeSi[iCr]->Fill(ic, fCMode[iCr][iCh]);
                //                            hSModeSi[iCr]->Fill(ic, fSMode[iCr][iCh]);
                //                            hPedSi[iCr]->Fill(ic, fPedVal[iCr][iCh][iSmpl]);
                //                }
            }
    }

}

void BmnAdcProcessor::Run7(Int_t* _statsGem, Int_t* _statsSil, Int_t* _statsGemPermut, Int_t* _statsSilPermut) {
    // Setup is valid for two modes (BM@N and SRC)
    // To be moved to the UniDb
    if (fSetup == kBMNSETUP) {
        _statsGem[0] = 1; // MK-numeration
        _statsGem[1] = 2; // MK-numeration
        _statsGem[2] = 4; // MK-numeration
        _statsGem[3] = 5; // MK-numeration
        _statsGem[4] = 6; // MK-numeration
        _statsGem[5] = 7; // MK-numeration

        _statsGemPermut[0] = 0;
        _statsGemPermut[1] = 1;
        _statsGemPermut[2] = 2;
        _statsGemPermut[3] = 3;
        _statsGemPermut[4] = 4;
        _statsGemPermut[5] = 5;

        _statsSil[0] = 1; // MK-numeration
        _statsSil[1] = 2; // MK-numeration
        _statsSil[2] = 3; // MK-numeration

        _statsSilPermut[0] = 0;
        _statsSilPermut[1] = 1;
        _statsSilPermut[2] = 2;

    } else if (fSetup == kSRCSETUP) {
        _statsGem[0] = 1; // MK-numeration
        _statsGem[1] = 2; // MK-numeration
        _statsGem[2] = 3; // MK-numeration
        _statsGem[3] = 4; // MK-numeration
        _statsGem[4] = 5; // MK-numeration
        _statsGem[5] = 6; // MK-numeration
        _statsGem[6] = 7; // MK-numeration
        _statsGem[7] = 8; // MK-numeration
        _statsGem[8] = 9; // MK-numeration
        _statsGem[9] = 10; // MK-numeration

        _statsGemPermut[0] = 0;
        _statsGemPermut[1] = 1;
        _statsGemPermut[2] = 2;
        _statsGemPermut[3] = 3;
        _statsGemPermut[4] = 4;
        _statsGemPermut[5] = 5;
        _statsGemPermut[6] = 6;
        _statsGemPermut[7] = 7;
        _statsGemPermut[8] = 8;
        _statsGemPermut[9] = 9;

        _statsSil[0] = 1; // MK-numeration
        _statsSil[1] = 2; // MK-numeration
        _statsSil[2] = 3; // MK-numeration

        _statsSilPermut[0] = 0;
        _statsSilPermut[1] = 1;
        _statsSilPermut[2] = 2;
    } else {
        cout << "Configuration not defined!" << endl;
        throw;
    }
}

void BmnAdcProcessor::CreateGeometries() {
    // Initialize det. geometries for converter
    TString gPathConfig = getenv("VMCWORKDIR");
    TString confSi = (fPeriod == 7) ?
            ((fSetup == kBMNSETUP) ? "SiliconRunSpring2018.xml" : "SiliconRunSRCSpring2018.xml") :
            "SiliconRunSpring2017.xml";
    TString confGem = (fPeriod == 7) ?
            ((fSetup == kBMNSETUP) ? "GemRunSpring2018.xml" : "GemRunSRCSpring2018.xml") :
            "GemRunSpring2017.xml";
    TString confCsc = "CSCRunSpring2018.xml";

    // SI
    TString gPathSiliconConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
    fSilStationSet = new BmnSiliconStationSet(gPathSiliconConfig + confSi);

    // GEM
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
    fGemStationSet = new BmnGemStripStationSet(gPathGemConfig + confGem);

    // CSC
    TString gPathCscConfig = gPathConfig + "/parameters/csc/XMLConfigs/";
    fCscStationSet = new BmnCSCStationSet(gPathCscConfig + confCsc);
    // Prepare arrays for GEM- and SI-converters and make
    statsGem = new Int_t[fGemStationSet->GetNStations()];
    statsSil = new Int_t[fSilStationSet->GetNStations()];

    statsGemPermut = new Int_t[fGemStationSet->GetNStations()];
    statsSilPermut = new Int_t[fSilStationSet->GetNStations()];

    for (Int_t iStat = 0; iStat < fGemStationSet->GetNStations(); iStat++) {
        statsGem[iStat] = -1;
        statsGemPermut[iStat] = -1;
    }

    for (Int_t iStat = 0; iStat < fSilStationSet->GetNStations(); iStat++) {
        statsSil[iStat] = -1;
        statsSilPermut[iStat] = -1;
    }

    Run7(statsGem, statsSil, statsGemPermut, statsSilPermut);

    for (Int_t iStat = 0; iStat < fGemStationSet->GetNStations(); iStat++)
        fGemStats[statsGem[iStat]] = statsGemPermut[iStat];

    for (Int_t iStat = 0; iStat < fSilStationSet->GetNStations(); iStat++)
        fSilStats[statsSil[iStat]] = statsSilPermut[iStat];
}

Double_t BmnAdcProcessor::CalcSCMS(Double_t* samples, Int_t nSmpl, UInt_t iCr, UInt_t iCh) {
    Double_t pedCMS = 0;
    Double_t sigCMS = 0;
    Double_t pedCMStemp = 0;
    Double_t sigCMStemp = 0;
    UInt_t nvals = 0;
    pedCMStemp = 0;
    sigCMStemp = 0;
    nvals = 0;
    Double_t rmsthr = 500.0;
    Double_t sumRms = 200.0;
    Double_t sumRms2 = thrMax;
    Int_t nIters = 4;
    for (Int_t iter = 1; iter < nIters; iter++) {
        Double_t thr = /*(3.0 ) * sumRms2;*/ thrMax - thrDif * iter; //(2 + nIters - iter) * sumRms2; //thrMax - thrDif * iter;
        sumRms = 0.0;
        sumRms2 = 0.0;
        rmsthr = 0.0;
        pedCMStemp = 0.0;
        sigCMStemp = 0.0;
        nvals = 0;
        for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
            if (samples[iSmpl] == 0 || fPedVal[iCr][iCh][iSmpl] == 0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
            Double_t Asig = TMath::Abs(samples[iSmpl] - fPedVal[iCr][iCh][iSmpl] + sigCMS - pedCMS);
            //            printf("Asig = %f  thr = %f   iSmpl %i\n", Asig, thr, iSmpl);
            if (Asig < thr) {
                sigCMStemp += samples[iSmpl]; // CMS from current event
                pedCMStemp += fPedVal[iCr][iCh][iSmpl]; // CMS over all pedestals
                rmsthr += Asig * Asig;
                sumRms += samples[iSmpl] - sigCMS;
                sumRms2 += Sq(samples[iSmpl] - sigCMS);
                //                if (iter == nIters - 1 && iCr == 0 && iCh == 9) {
                //                    hscms1_adc->Fill(iSmpl, samples[iSmpl]);
                //                    hcms1->Fill(pedCMS);
                //                }
                nvals++;

            }
        }
        // normalize
        if (nvals) {
            sigCMS = sigCMStemp / nvals;
            pedCMS = pedCMStemp / nvals;
            rmsthr = Sqrt(rmsthr / nvals);
            sumRms = sumRms / nvals;
            sumRms2 = Sqrt(Abs(sumRms2 / nvals - sumRms * sumRms));
            if (iter == nIters - 1 && iCr == 0 && iCh == 9) {
                //                hcms1->Fill(pedCMS);
                //                hscms1_adc->Fill(sigCMS);
            }
        }
        //                printf("\n iter %i sig cms %f  cms %f\n", iter, sigCMS, pedCMS);
        //                printf("\t\t(sig -ped) rms = %f          filtered %i  on iter %i\n", rmsthr, nvals, iter);
    }
    //        printf("(sigCMS - pedCMS) = %f \n", (sigCMS - pedCMS));
    return (sigCMS - pedCMS);
}

Double_t BmnAdcProcessor::CalcCMS(Double_t* samples, Int_t size) {

    const UShort_t kNITER = 10;
    Double_t CMS = 0.0;
    UInt_t nStr = size;
    Double_t upd[size];
    for (Int_t iSmpl = 0; iSmpl < size; ++iSmpl)
        upd[iSmpl] = samples[iSmpl];

    for (Int_t itr = 0; itr < kNITER; ++itr) {
        if (nStr == 0) break;
        Double_t cms = 0.0; //common mode shift
        for (Int_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
            cms += upd[iSmpl];
        cms /= nStr;
        Double_t rms = 0.0; //chip noise
        for (Int_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
            rms += (upd[iSmpl] - cms) * (upd[iSmpl] - cms);
        rms = Sqrt(rms / nStr);

        UInt_t nOk = 0;
        for (Int_t iSmpl = 0; iSmpl < nStr; ++iSmpl)
            if (Abs(upd[iSmpl] - cms) < 3 * rms)
                upd[nOk++] = upd[iSmpl];
        nStr = nOk;
        CMS = cms;
    }
    return CMS;
}

ClassImp(BmnAdcProcessor)
