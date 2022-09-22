#include "BmnAdcProcessor.h"
#include <BmnMath.h>
#include <vector>
#include "P4_F32vec4.h"

static Double_t workTime_cpu = 0.0;
static Double_t workTime_real = 0.0;

BmnAdcProcessor::BmnAdcProcessor() {
}

BmnAdcProcessor::BmnAdcProcessor(Int_t period, Int_t run, TString det, Int_t nCh, Int_t nSmpl) {
    fPeriod = period;
    fRun = run;
    fDetName = det;
    fNChannels = nCh;
    fNSamples = nSmpl;
    PrecalcEventModsImp = (GetRun() > GetBoundaryRun(fNSamples) || GetPeriod() >= 7) ?
            //#if CMAKE_BUILD_TYPE == Debug
            //        &BmnAdcProcessor::PrecalcEventMods : &BmnAdcProcessor::PrecalcEventModsOld;
            //        printf("\n\nDebug!!!\n\n");
            //#else
            //        &BmnAdcProcessor::PrecalcEventMods_simd : &BmnAdcProcessor::PrecalcEventModsOld;
            //        printf("\n\nRelease!!!\n\n");
            //#endif

#ifdef BUILD_DEBUG
            &BmnAdcProcessor::PrecalcEventMods : &BmnAdcProcessor::PrecalcEventModsOld;
    if (fVerbose)
        printf("\n\nDebug!!!\n");
#else
            &BmnAdcProcessor::PrecalcEventMods_simd : &BmnAdcProcessor::PrecalcEventModsOld;
    if (fVerbose)
        printf("\n\nRelease!!!\n");
#endif
}

BmnAdcProcessor::BmnAdcProcessor(Int_t period, Int_t run, TString det, Int_t nCh, Int_t nSmpl, vector<UInt_t> vSer) :
BmnAdcProcessor(period, run, det, nCh, nSmpl) {
    SetSerials(vSer);
}

void BmnAdcProcessor::SetSerials(vector<UInt_t> &vSer) {
    fAdcSerials = vSer;
    fNSerials = fAdcSerials.size();
    for (int iSer = 0; iSer < fAdcSerials.size(); ++iSer) {
        fSerMap.insert(pair<UInt_t, Int_t>(fAdcSerials[iSer], iSer));
        //                printf("iser %d ser 0x%08X  val %d  \n", iSer, fAdcSerials[iSer], fSerMap[fAdcSerials[iSer]]);
    }
    InitArrays();
}

void BmnAdcProcessor::InitArrays() {
    fPedVal = new Float_t**[fNSerials];
    //fPedVal = new Double_t**[fNSerials];
    fPedValTemp = new Double_t**[fNSerials];
    //  fNvals = new UInt_t*[fNSerials];
    fNvals = new Float_t*[fNSerials];
    fNvalsCMod = new UInt_t**[fNSerials];
    fNvalsADC = new UInt_t**[fNSerials];
    fPedRms = new Double_t**[fNSerials];
    fPedCMod = new Double_t**[fNSerials];
    fPedCMod2 = new Double_t**[fNSerials];
    fSumRmsV = new Double_t*[fNSerials];
    fNoisyChipChannels = new Bool_t**[fNSerials];
    fCMode = new Float_t*[fNSerials];
    fCMode0 = new Float_t*[fNSerials];
    fSMode = new Float_t*[fNSerials];
    fSMode0 = new Float_t*[fNSerials];
    fAdcProfiles = new UInt_t**[fNSerials];
    fAdc = new Float_t**[fNSerials];
    //fAdc = new Double_t**[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        //fPedVal[iCr] = new Double_t*[fNChannels];
        fPedVal[iCr] = new Float_t*[fNChannels];
        fPedValTemp[iCr] = new Double_t*[fNChannels];
        //fNvals[iCr] = new UInt_t[fNChannels];
        fNvals[iCr] = new Float_t[fNChannels];
        memset(fNvals[iCr], 0, sizeof (Float_t) * fNChannels);
        fNvalsCMod[iCr] = new UInt_t*[fNChannels];
        fNvalsADC[iCr] = new UInt_t*[fNChannels];
        fPedRms[iCr] = new Double_t*[fNChannels];
        fPedCMod[iCr] = new Double_t*[fNChannels];
        fPedCMod2[iCr] = new Double_t*[fNChannels];
        fSumRmsV[iCr] = new Double_t[fNChannels];
        memset(fSumRmsV[iCr], 0, sizeof (Double_t) * fNChannels);
        fNoisyChipChannels[iCr] = new Bool_t*[fNChannels];
        fAdcProfiles[iCr] = new UInt_t*[fNChannels];
        //fAdc[iCr] = new Double_t*[fNChannels];
        fAdc[iCr] = new Float_t*[fNChannels];
        fCMode[iCr] = new Float_t[fNChannels];
        fCMode0[iCr] = new Float_t[fNChannels];
        fSMode[iCr] = new Float_t[fNChannels];
        fSMode0[iCr] = new Float_t[fNChannels];
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            //fPedVal[iCr][iCh] = new Double_t[fNSamples];
            fPedVal[iCr][iCh] = new Float_t[fNSamples];
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
            //fAdc[iCr][iCh] = new Double_t[fNSamples];
            fAdc[iCr][iCh] = new Float_t[fNSamples];
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
    //    if (fSilStationSet)
    //        delete fSilStationSet;
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
    printf("LOOK HERE: Real time %f s, CPU time %f s ADC processor\n", workTime_real, workTime_cpu);
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
            hPedLineSi[iCr][iCh]->Draw("colz");
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
    if (hSModeSi.size() == 0) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
            vector<TH1*> hv;
            vector<TH1*> hcm;
            vector<TH1*> hsm;
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                //                        printf("Creating  icr %2d ich %2d %s\n", iCr, iCh, Form("%08X:%02d pedestal line MK", fSerials[iCr], iCh));
                TString hname = TString(Form("%08X:%02d pedestals SM", fAdcSerials[iCr], iCh));
                TH1* h = new TH2F(hname, hname,
                        500, 0, 500,
                        fNSamples, 0, fNSamples);
                h->GetXaxis()->SetTitle("Event #");
                h->GetYaxis()->SetTitle("Sample(channel) #");
                h->SetDirectory(0);
                hv.push_back(h);
            }
            hPedLineSi.push_back(hv);
        }

        const Int_t maxAdc = 8192;
        const Int_t MaxSig = 2300;
        const Int_t RngSig = 90;
        const Int_t StripSi = fNChannels * fNSamples;
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
            TString hname = TString(Form("%08X pedestals SM", fAdcSerials[iCr]));
            //        TH1* h = new TH2F(hname, hname, maxAdc, 0, maxAdc, MaxSig, 0, MaxSig);
            TH1* h = new TH2F(hname, hname, StripSi, 0, StripSi, 2 * RngSig, -RngSig, RngSig);
            //    printf("maxAdc %04d max %04f peds\n", maxAdc, h->GetXaxis()->GetXmax());
            h->GetXaxis()->SetTitle("Channel #");
            h->GetYaxis()->SetTitle("Signal");
            h->SetDirectory(0);
            hPedSi.push_back(h);

            hname = TString(Form("%08X cmods SM", fAdcSerials[iCr]));
            TH1* hc = new TH2F(hname, hname, StripSi, 0, StripSi, 2 * RngSig, -RngSig, RngSig);
            //    printf("maxAdc %04d max %04f cmode\n", maxAdc, hc->GetXaxis()->GetXmax());
            hc->GetXaxis()->SetTitle("Channel #");
            hc->GetYaxis()->SetTitle("Signal");
            hc->SetDirectory(0);
            hCModeSi.push_back(hc);

            hname = TString(Form("%08X smods SM", fAdcSerials[iCr]));
            TH1* hs = new TH2F(hname, hname, StripSi, 0, StripSi, 2 * RngSig, -RngSig, RngSig);
            hs->GetXaxis()->SetTitle("Channel #");
            hs->GetYaxis()->SetTitle("Signal");
            hs->SetDirectory(0);
            hSModeSi.push_back(hs);
        }
    }
    if (fVerbose)
        printf("%s %s started   niter %d  thrMax  %4.2f\n", fDetName.Data(), __func__, niter, thrMax);
    const UShort_t nSmpl = fNSamples;
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
                    fPedVal[iCr][iCh][iSmpl] += fPedDat[iCr][iEv][iCh][iSmpl]; // / N_EV_FOR_PEDESTALS);
                    fNvalsADC[iCr][iCh][iSmpl]++;
                    nOk++;
                    static_cast<TH2*> (hPedLineSi[iCr][iCh])->Fill(iEv, iSmpl, fPedDat[iCr][iEv][iCh][iSmpl]);
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
        Double_t thr = thrMax - thrDif * iter;
        if (fVerbose)
            printf("iter %d thr %4.2f\n", iter, thr);
        UInt_t nFiltered = 0;
        // clear
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
            memset(fSumRmsV[iCr], 0, sizeof (Double_t) * fNChannels);
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                memset(fNvalsCMod[iCr][iCh], 0, sizeof (UInt_t) * fNSamples);
                memset(fNvalsADC[iCr][iCh], 0, sizeof (UInt_t) * fNSamples);
                memset(fPedValTemp[iCr][iCh], 0, sizeof (Double_t) * fNSamples);
                memset(fPedCMod[iCr][iCh], 0, sizeof (Double_t) * fNSamples);
                memset(fPedCMod2[iCr][iCh], 0, sizeof (Double_t) * fNSamples);
            }
        }
        for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS - 1; ++iEv) {
            // clear
            for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
                // memset(fNvals[iCr], 0, sizeof (UInt_t) * fNChannels);
                memset(fNvals[iCr], 0, sizeof (Float_t) * fNChannels);
                memset(fCMode[iCr], 0, sizeof (Float_t) * fNChannels);
                memset(fSMode[iCr], 0, sizeof (Float_t) * fNChannels);
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
                        if (iter == niter - 1) {
                            Int_t ic = iCh * nSmpl + iSmpl;
                            //                        printf("iCh %4d iSmpl %4d  ic %4d  cmod %5f smod %5f\n", iCh, iSmpl,ic, fPedCMS[iCr][iCh], fSigCMS[iCr][iCh]);
                            hCModeSi[iCr]->Fill(ic, fCMode[iCr][iCh]);
                            hSModeSi[iCr]->Fill(ic, fSMode[iCr][iCh]);
                            hPedSi[iCr]->Fill(ic, fPedVal[iCr][iCh][iSmpl]);
                        }
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
    return kBMNSUCCESS;
}

void BmnAdcProcessor::PrecalcEventModsOld(TClonesArray *adc) {
    TStopwatch timer;
    Double_t rtime;
    Double_t ctime;
    timer.Start();
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        memset(fNvals[iCr], 0, sizeof (Float_t) * fNChannels);
        memset(fCMode[iCr], 0, sizeof (Float_t) * fNChannels);
        memset(fSMode[iCr], 0, sizeof (Float_t) * fNChannels);
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            memset(fAdc[iCr][iCh], 0, sizeof (Float_t) * fNSamples);
        }
    }
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    //    printf("\nReal time %f s, CPU time %f s  clear\n", rtime, ctime);
    timer.Start();
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
            Double_t val = static_cast<Double_t> (adcDig->GetUShortValue()[iSmpl] / 16);
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
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    //    printf("\nReal time %f s, CPU time %f s  fill array\n", rtime, ctime);
}

void BmnAdcProcessor::PrecalcEventMods(TClonesArray *adc) {
    TStopwatch timer;
    Double_t rtime;
    Double_t ctime;
    timer.Start();
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        // memset(fNvals[iCr], 0, sizeof (UInt_t) * fNChannels);
        memset(fNvals[iCr], 0, sizeof (Float_t) * fNChannels);
        memset(fCMode[iCr], 0, sizeof (Float_t) * fNChannels);
        memset(fSMode[iCr], 0, sizeof (Float_t) * fNChannels);
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            memset(fAdc[iCr][iCh], 0, sizeof (Float_t) * fNSamples);
        }
    }
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    //    printf("\nReal time %f s, CPU time %f s  clear\n", rtime, ctime);
    timer.Start();
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
            Double_t val = static_cast<Double_t> (adcDig->GetShortValue()[iSmpl] / 16);
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
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();

}

//simd function

void BmnAdcProcessor::PrecalcEventMods_simd(TClonesArray *adc) {
    TStopwatch timer;
    Double_t rtime;
    Double_t ctime;
    timer.Start();
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        // memset(fNvals[iCr], 0, sizeof (UInt_t) * fNChannels);
        memset(fNvals[iCr], 0, sizeof (Float_t) * fNChannels);
        memset(fCMode[iCr], 0, sizeof (Float_t) * fNChannels);
        memset(fSMode[iCr], 0, sizeof (Float_t) * fNChannels);
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            memset(fAdc[iCr][iCh], 0, sizeof (Float_t) * fNSamples);
        }
    }
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    //    printf("\nReal time %f s, CPU time %f s  clear\n", rtime, ctime);
    timer.Start();

    fvec * fNvals_vec, * fSMode_vec, * fCMode_vec;
    fvec* fPedVal_vec, * fAdc_vec, * fNoisyChipChannels_vec;
    Float_t*** fNoisy_float;

    fNoisy_float = new Float_t**[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fNoisy_float[iCr] = new Float_t*[fNChannels];
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            fNoisy_float[iCr][iCh] = new Float_t[fNSamples];
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                //fNoisy_float[iCr][iCh][iSmpl] = (Float_t)fNoisyChipChannels[iCr][iCh][iSmpl];
                if (fNoisyChipChannels[iCr][iCh][iSmpl])
                    fNoisy_float[iCr][iCh][iSmpl] = 1.0;
                else
                    fNoisy_float[iCr][iCh][iSmpl] = 0.0;
            }
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
            Float_t val = static_cast<Float_t> (adcDig->GetShortValue()[iSmpl] / 16);
            fAdc[iCr][iCh][iSmpl] = val;
        }

        fPedVal_vec = (fvec *) fPedVal[iCr][iCh];
        fAdc_vec = (fvec *) fAdc[iCr][iCh];
        fNoisyChipChannels_vec = (fvec *) fNoisy_float[iCr][iCh]; //!!!!!!!
        fvec fSMode_sum = 0.0;
        fvec fCMode_sum = 0.0;
        fvec Nval = 0.0;
        Float_t sum1;
        Float_t sum2;
        Float_t sum3;

        for (Int_t iSmpl = 0; iSmpl < fNSamples / 4; iSmpl++) {
            //if ((fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE)) continue;
            fvec sig = if3(fvec(fNoisyChipChannels_vec[iSmpl] == 1.0), 10000, fAdc_vec[iSmpl] - fPedVal_vec[iSmpl]);
            //cout << "result of the comparison: " << sig << endl;
            fvec Asig = fabs(sig);
            //cout << "absolute value: " << Asig << endl;
            fvec thrMax_vec = thrMax;
            Nval = if3(fvec(Asig < thrMax_vec), Nval + 1.0, Nval + 0.0);
            fSMode_sum = if3(fvec(Asig < thrMax_vec), fSMode_sum + fAdc_vec[iSmpl], fSMode_sum + 0.0);
            fCMode_sum = if3(fvec(Asig < thrMax_vec), fCMode_sum + fPedVal_vec[iSmpl], fCMode_sum + 0.0);
        }
        fvec helper = 0.0;
        helper = _mm_movehl_ps(helper, fSMode_sum);
        fSMode_sum = fSMode_sum + helper;
        helper = _mm_shuffle_ps(fSMode_sum, fSMode_sum, 1);
        fSMode_sum = _mm_add_ss(fSMode_sum, helper);
        _mm_store_ss(&sum1, fSMode_sum);
        fSMode[iCr][iCh] += sum1;

        helper = 0.0;
        helper = _mm_movehl_ps(helper, fCMode_sum);
        fCMode_sum = fCMode_sum + helper;
        helper = _mm_shuffle_ps(fCMode_sum, fCMode_sum, 1);
        fCMode_sum = _mm_add_ss(fCMode_sum, helper);
        _mm_store_ss(&sum2, fCMode_sum);
        fCMode[iCr][iCh] += sum2;

        helper = 0.0;
        helper = _mm_movehl_ps(helper, Nval);
        Nval = Nval + helper;
        helper = _mm_shuffle_ps(Nval, Nval, 1);
        Nval = _mm_add_ss(Nval, helper);
        _mm_store_ss(&sum3, Nval);
        fNvals[iCr][iCh] += sum3;
    }
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            delete[] fNoisy_float[iCr][iCh];
        }
        delete[] fNoisy_float[iCr];
    }
    delete[] fNoisy_float;

    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();

}

void BmnAdcProcessor::CalcEventMods() {
    TStopwatch timer;
    timer.Start();
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
                Double_t cs = fCMode[iCr][iCh] - fSMode[iCr][iCh];
                for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                    if (fPedVal[iCr][iCh][iSmpl] == 0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
                    Double_t sig = fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + cs;
                    Double_t Asig = TMath::Abs(sig);
                    Double_t thr = thrMax - iter * thrDif;
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
            }
    }
    timer.Stop();
    workTime_cpu += (Double_t) timer.CpuTime();
    workTime_real += (Double_t) timer.RealTime();
}

//simd function

void BmnAdcProcessor::CalcEventMods_simd() {
    TStopwatch timer;
    timer.Start();

    // normalize
    fvec fNvalsMin_vec = 0;
    fvec * fNvals_vec, * fSMode_vec, * fCMode_vec;
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fNvals_vec = (fvec *) fNvals[iCr];
        fSMode_vec = (fvec *) fSMode[iCr];
        fCMode_vec = (fvec *) fCMode[iCr];
        for (Int_t iCh = 0; iCh < fNChannels / 4; iCh++) {
            fSMode_vec[iCh] = if3(fvec(fNvals_vec[iCh] > fNvalsMin_vec), fSMode_vec[iCh] / fNvals_vec[iCh], 0.0);
            fCMode_vec[iCh] = if3(fvec(fNvals_vec[iCh] > fNvalsMin_vec), fCMode_vec[iCh] / fNvals_vec[iCh], 0.0);
        }
        fNvals[iCr] = (Float_t*) fNvals_vec;
        fSMode[iCr] = (Float_t*) fSMode_vec;
        fCMode[iCr] = (Float_t*) fCMode_vec;
    }
    // filter out sigs to get mods

    Float_t*** fNoisy_float;
    fNoisy_float = new Float_t**[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fNoisy_float[iCr] = new Float_t*[fNChannels];
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            fNoisy_float[iCr][iCh] = new Float_t[fNSamples];
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                //fNoisy_float[iCr][iCh][iSmpl] = (Float_t)fNoisyChipChannels[iCr][iCh][iSmpl];
                if (fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE)
                    fNoisy_float[iCr][iCh][iSmpl] = 1.0;
                else
                    fNoisy_float[iCr][iCh][iSmpl] = 0.0;
            }
        }
    }


    fvec* fSMode0_vec, * fCMode0_vec;
    fvec* fPedVal_vec, * fAdc_vec, * fNoisyChipChannels_vec;
    for (Int_t iter = -2; iter < niter; ++iter) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
            fSMode0_vec = (fvec *) fSMode0[iCr];
            fCMode0_vec = (fvec *) fCMode0[iCr];
            fNvals_vec = (fvec *) fNvals[iCr];
            for (Int_t iCh = 0; iCh < fNChannels / 4; iCh++) {
                fSMode0_vec[iCh] = 0.0;
                fCMode0_vec[iCh] = 0.0;
                fNvals_vec[iCh] = 0;
            }
            fSMode0[iCr] = (Float_t *) fSMode0_vec;
            fCMode0[iCr] = (Float_t *) fCMode0_vec;
            fNvals[iCr] = (Float_t *) fNvals_vec;
        }
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                fPedVal_vec = (fvec *) fPedVal[iCr][iCh];
                fAdc_vec = (fvec *) fAdc[iCr][iCh];
                fNoisyChipChannels_vec = (fvec *) fNoisy_float[iCr][iCh]; //!!!!!!!
                Float_t cs = fCMode[iCr][iCh] - fSMode[iCr][iCh];
                fvec cs_vec = cs;
                fvec fSMode0_sum = 0.0;
                fvec fCMode0_sum = 0.0;
                fvec Nval = 0.0;
                Float_t sum1;
                Float_t sum2;
                Float_t sum3;

                for (Int_t iSmpl = 0; iSmpl < fNSamples / 4; iSmpl++) {
                    //if (fPedVal[iCr][iCh][iSmpl] == 0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
                    fvec sig = if3(fvec(fPedVal_vec[iSmpl] == 0.0) | fvec(fNoisyChipChannels_vec[iSmpl] == 1.0), 10000, fAdc_vec[iSmpl] - fPedVal_vec[iSmpl] + cs_vec);
                    fvec Asig = fabs(sig);
                    Float_t thr = thrMax - iter * thrDif;
                    fvec thr_vec = thr;
                    Nval = if3(fvec(Asig < thr_vec), Nval + 1.0, Nval + 0.0);
                    fSMode0_sum = if3(fvec(Asig < thr_vec), fSMode0_sum + fAdc_vec[iSmpl], fSMode0_sum + 0.0);
                    fCMode0_sum = if3(fvec(Asig < thr_vec), fCMode0_sum + fPedVal_vec[iSmpl], fCMode0_sum + 0.0);
                }
                fvec helper = 0.0;
                helper = _mm_movehl_ps(helper, fSMode0_sum);
                fSMode0_sum = fSMode0_sum + helper;
                helper = _mm_shuffle_ps(fSMode0_sum, fSMode0_sum, 1);
                fSMode0_sum = _mm_add_ss(fSMode0_sum, helper);
                _mm_store_ss(&sum1, fSMode0_sum);
                fSMode0[iCr][iCh] += sum1;

                helper = 0.0;
                helper = _mm_movehl_ps(helper, fCMode0_sum);
                fCMode0_sum = fCMode0_sum + helper;
                helper = _mm_shuffle_ps(fCMode0_sum, fCMode0_sum, 1);
                fCMode0_sum = _mm_add_ss(fCMode0_sum, helper);
                _mm_store_ss(&sum2, fCMode0_sum);
                fCMode0[iCr][iCh] += sum2;

                helper = 0.0;
                helper = _mm_movehl_ps(helper, Nval);
                Nval = Nval + helper;
                helper = _mm_shuffle_ps(Nval, Nval, 1);
                Nval = _mm_add_ss(Nval, helper);
                _mm_store_ss(&sum3, Nval);
                fNvals[iCr][iCh] += sum3;
            }

        for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
            fNvals_vec = (fvec *) fNvals[iCr];
            fSMode_vec = (fvec *) fSMode[iCr];
            fCMode_vec = (fvec *) fCMode[iCr];
            fSMode0_vec = (fvec *) fSMode0[iCr];
            fCMode0_vec = (fvec *) fCMode0[iCr];
            for (Int_t iCh = 0; iCh < fNChannels / 4; iCh++) {
                fSMode_vec[iCh] = if3(fvec(fNvals_vec[iCh] > fNvalsMin_vec), fSMode0_vec[iCh] / fNvals_vec[iCh], 0.0);
                fCMode_vec[iCh] = if3(fvec(fNvals_vec[iCh] > fNvalsMin_vec), fCMode0_vec[iCh] / fNvals_vec[iCh], 0.0);
            }
            fNvals[iCr] = (Float_t *) fNvals_vec;
            fSMode[iCr] = (Float_t *) fSMode_vec;
            fCMode[iCr] = (Float_t *) fCMode_vec;
        }
    }

    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            delete[] fNoisy_float[iCr][iCh];
        }
        delete[] fNoisy_float[iCr];
    }
    delete[] fNoisy_float;

    timer.Stop();
    workTime_cpu += (Double_t) timer.CpuTime();
    workTime_real += (Double_t) timer.RealTime();
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
    fSilStationSet = unique_ptr<BmnSiliconStationSet>(new BmnSiliconStationSet(gPathSiliconConfig + confSi));
    //    fSilStationSet = make_unique<BmnSiliconStationSet>(gPathSiliconConfig + confSi);
    //    fSilStationSet = new BmnSiliconStationSet(gPathSiliconConfig + confSi);

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

BmnStatus BmnAdcProcessor::SaveFilterInfo() {

    return kBMNSUCCESS;
}

unique_ptr<BmnSiliconStationSet> BmnAdcProcessor::GetSilStationSet(Int_t period, BmnSetup stp) {
    TString gPathConfig = getenv("VMCWORKDIR");
    TString xmlConfFileName;
    switch (period) {
        case 8:
            if (stp == kBMNSETUP) {
                xmlConfFileName = "SiliconRun8_3stations.xml";
            } else {
                xmlConfFileName = "SiliconRun8_SRC.xml";
            }
            break;
        case 7:
            if (stp == kBMNSETUP) {
                xmlConfFileName = "SiliconRunSpring2018.xml";
            } else {
                xmlConfFileName = "SiliconRunSRCSpring2018.xml";
            }
            break;
        case 6:
            xmlConfFileName = "SiliconRunSpring2017.xml";
            break;
        default:
            printf("Error! Unknown config!\n");
            return nullptr;
            break;
    }
    TString gPathSiliconConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
    return unique_ptr<BmnSiliconStationSet>(new BmnSiliconStationSet(gPathSiliconConfig + xmlConfFileName));
    //    return std::make_unique<BmnSiliconStationSet>(gPathSiliconConfig + xmlConfFileName);
}

BmnGemStripStationSet * BmnAdcProcessor::GetGemStationSet(Int_t period, BmnSetup stp) {
    TString gPathConfig = getenv("VMCWORKDIR");
    TString xmlConfFileName;
    switch (period) {
        case 8:
            if (stp == kBMNSETUP) {
                xmlConfFileName = "GemRun8.xml";
            } else {
                xmlConfFileName = "GemRunSRC2021.xml";
            }
            break;
        case 7:
            if (stp == kBMNSETUP) {
                xmlConfFileName = "GemRunSpring2018.xml";
            } else {
                xmlConfFileName = "GemRunSRCSpring2018.xml";
            }
            break;
        case 6:
            xmlConfFileName = "GemRunSpring2017.xml";
            break;
        default:
            printf("Error! Unknown config!\n");
            return nullptr;
            break;
    }
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
    return new BmnGemStripStationSet(gPathGemConfig + xmlConfFileName);
}

BmnCSCStationSet * BmnAdcProcessor::GetCSCStationSet(Int_t period, BmnSetup stp) {
    TString gPathConfig = getenv("VMCWORKDIR");
    TString xmlConfFileName;
    switch (period) {
        case 8:
            if (stp == kBMNSETUP) {
                xmlConfFileName = "CSCRun8.xml";
            } else {
                xmlConfFileName = "CSCRunSRC2021.xml";
            }
            break;
        case 7:
            if (stp == kBMNSETUP) {
                xmlConfFileName = "CSCRunSpring2018.xml";
            } else {
                xmlConfFileName = "CSCRunSRCSpring2018.xml";
            }
            break;
        default:
            printf("Error! Unknown config!\n");
            return nullptr;
            break;
    }
    TString gPathCscConfig = gPathConfig + "/parameters/csc/XMLConfigs/";
    return new BmnCSCStationSet(gPathCscConfig + xmlConfFileName);
}

ClassImp(BmnAdcProcessor)
