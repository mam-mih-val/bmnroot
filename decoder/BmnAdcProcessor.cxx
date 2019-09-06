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
    thrMax = 360; //420.0;
    thrDif = 40.0;
    Int_t high = 120;
    Int_t highcms = 500;
//    h = new TH2F("h", "h", fNSamples, 0, fNSamples, 2 * high + 1, -high, high);
//    hp = new TH2F("hp", "hp", fNSamples, 0, fNSamples, 2 * high + 1, -high, high);
//    hcms = new TH2F("hcms", "hcms", fNSamples, 0, fNSamples, 2 * high + 1, -high / 2, high / 2);
//    hscms_adc = new TH2F("hscms ADC", "hscms", fNSamples, 0, fNSamples, 2 * high + 1, -high / 2, high / 2);
//    hcms1p = new TH1F("hcms1D pedestal", "hcms1D pedestal", 2 * high + 1, -high, high);
//    hscms1p_adc = new TH1F("hscms1D ADC pedestal", "hscms1D pedestal", 2 * highcms + 1, -high, high);
//    hcms1 = new TH1F("hcms1D", "hcms1D", 2 * high + 1, -high, high);
//    hscms1_adc = new TH2F("hscms1D ADC", "hscms1D", fNSamples, 0, fNSamples, 2 * highcms + 1, -high, high);
//    printf("fnserials %i\n", fNSerials);
//    for (int i = 0; i < fNSerials; i++)
//        printf("%i ser %08X\n", i, vSer[i]);

    fPedVal = new Double_t**[fNSerials];
    fPedValTemp = new Double_t**[fNSerials];
    fNvals = new UInt_t*[fNSerials];
    fNvalsADC = new UInt_t**[fNSerials];
    fPedRms = new Double_t**[fNSerials];
    fPedSigRms = new Double_t**[fNSerials];
    fPedSigRms2 = new Double_t**[fNSerials];
    fNoisyChipChannels = new Bool_t**[fNSerials];
    fPedCMS = new Double_t*[fNSerials];
    fPedCMS0 = new Double_t*[fNSerials];
    fSigCMS = new Double_t*[fNSerials];
    fAdcProfiles = new UInt_t**[fNSerials];
    for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
        fPedVal[iCr] = new Double_t*[fNChannels];
        fPedValTemp[iCr] = new Double_t*[fNChannels];
        fNvals[iCr] = new UInt_t[fNChannels];
        memset(fNvals[iCr], 0, sizeof (UInt_t) * fNChannels);
        fNvalsADC[iCr] = new UInt_t*[fNChannels];
        fPedRms[iCr] = new Double_t*[fNChannels];
        fPedSigRms[iCr] = new Double_t*[fNChannels];
        fPedSigRms2[iCr] = new Double_t*[fNChannels];
        fNoisyChipChannels[iCr] = new Bool_t*[fNChannels];
        fAdcProfiles[iCr] = new UInt_t*[fNChannels];
        fPedCMS[iCr] = new Double_t[fNChannels];
        fPedCMS0[iCr] = new Double_t[fNChannels];
        fSigCMS[iCr] = new Double_t[fNChannels];
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            fPedVal[iCr][iCh] = new Double_t[fNSamples];
            fPedValTemp[iCr][iCh] = new Double_t[fNSamples];
            fNvalsADC[iCr][iCh] = new UInt_t[fNSamples];
            memset(fNvalsADC[iCr][iCh], 0, sizeof (UInt_t) * fNSamples);
            fPedRms[iCr][iCh] = new Double_t[fNSamples];
            fPedSigRms[iCr][iCh] = new Double_t[fNSamples];
            fPedSigRms2[iCr][iCh] = new Double_t[fNSamples];
            fNoisyChipChannels[iCr][iCh] = new Bool_t[fNSamples];
            fAdcProfiles[iCr][iCh] = new UInt_t[fNSamples];
            fPedCMS[iCr][iCh] = 0.0;
            fPedCMS0[iCr][iCh] = 0.0;
            fSigCMS[iCr][iCh] = 0.0;
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                fPedVal[iCr][iCh][iSmpl] = 0.0;
                fPedValTemp[iCr][iCh][iSmpl] = 0.0;
                fPedRms[iCr][iCh][iSmpl] = 0.0;
                fPedSigRms[iCr][iCh][iSmpl] = 0.0;
                fPedSigRms2[iCr][iCh][iSmpl] = 0.0;
                fNoisyChipChannels[iCr][iCh][iSmpl] = kFALSE;
                fAdcProfiles[iCr][iCh][iSmpl] = 0;
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
                    fPedDat[iCr][iEv][iCh][iSmpl] = 0;
            }
        }
    }
    const UInt_t PAD_WIDTH_SIL = 1920; //8192;
    const UInt_t PAD_HEIGHT_SIL = 2200;
//    canStrip = new TCanvas("can cms", "can", PAD_WIDTH_SIL, PAD_HEIGHT_SIL * 2);
//    canStrip->Divide(1, 8);
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
            delete[] fNvalsADC[iCr][iCh];
            delete[] fPedRms[iCr][iCh];
            delete[] fPedSigRms[iCr][iCh];
            delete[] fPedSigRms2[iCr][iCh];
            delete[] fAdcProfiles[iCr][iCh];
            delete[] fNoisyChipChannels[iCr][iCh];
        }
        delete[] fPedVal[iCr];
        delete[] fPedValTemp[iCr];
        delete[] fNvalsADC[iCr];
        delete[] fPedRms[iCr];
        delete[] fPedSigRms[iCr];
        delete[] fPedSigRms2[iCr];
        delete[] fAdcProfiles[iCr];
        delete[] fPedCMS[iCr];
        delete[] fSigCMS[iCr];
        delete[] fNoisyChipChannels[iCr];
    }
    delete[] fPedVal;
    delete[] fPedValTemp;
    delete[] fNvalsADC;
    delete[] fPedRms;
    delete[] fPedSigRms;
    delete[] fPedSigRms2;
    delete[] fAdcProfiles;
    delete[] fPedCMS;
    delete[] fSigCMS;
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
}

BmnStatus BmnAdcProcessor::RecalculatePedestals() {
//    printf("\n[INFO]");
//    printf(ANSI_COLOR_BLUE " ADC pedestals recalculation\n" ANSI_COLOR_RESET);
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
    const UInt_t PAD_WIDTH_SIL = 1920; //8192;
    const UInt_t PAD_HEIGHT_SIL = 1920;
    printf("\n[INFO]");
    printf(ANSI_COLOR_BLUE " ADC pedestals recalculation\n" ANSI_COLOR_RESET);
    const UShort_t nSmpl = fNSamples;

    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
                fPedVal[iCr][iCh][iSmpl] = 0.0;
                fPedValTemp[iCr][iCh][iSmpl] = 0.0;
                fPedRms[iCr][iCh][iSmpl] = 0.0;
                fPedSigRms[iCr][iCh][iSmpl] = 0.0;
                fPedSigRms2[iCr][iCh][iSmpl] = 0.0;
                fAdcProfiles[iCr][iCh][iSmpl] = 0;
                //                fNoisyChipChannels[iCr][iCh][iSmpl] = kFALSE;
            }
        }
    //cout << fDetName << " pedestals calculation..." << endl;
    //    for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
    //        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
    //            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
    //                Double_t signals[nSmpl];
    //                for (Int_t i = 0; i < nSmpl; ++i) signals[i] = 0.0;
    //                Int_t nOk = 0;
    //                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
    //                    if (fPedDat[iCr][iEv][iCh][iSmpl] == 0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
    //                    signals[iSmpl] = fPedDat[iCr][iEv][iCh][iSmpl];
    //                    nOk++;
    //                }
    //                Double_t CMS = CalcCMS(signals, nOk);
    //                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
    //                    fPedVal[iCr][iCh][iSmpl] += ((signals[iSmpl] - CMS) / N_EV_FOR_PEDESTALS);
    //                }
    //            }
    //    }


    for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                Int_t nOk = 0;
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fPedDat[iCr][iEv][iCh][iSmpl] == 0.0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
                    fPedVal[iCr][iCh][iSmpl] += (fPedDat[iCr][iEv][iCh][iSmpl] / N_EV_FOR_PEDESTALS);
                    nOk++;
                }
            }
    }
    // iteratively calculate pedestals and CMSs
    Double_t rmsthr = 200.0;
    Double_t rmsthrf = 200.0;
    Double_t sumRms = 200.0;
    Int_t nIters = 3;
    for (Int_t iter = 0; iter < nIters; iter++) {
        Double_t thr = thrMax - thrDif * iter; //(2 + (nIters - iter)/2.0) * sumRms; //thrMax - thrDif * iter;
        rmsthr = 0.0;
        rmsthrf = 0.0;
        sumRms = 0.0;
        UInt_t nFiltered = 0;
        UInt_t npreFiltered = 0;
        // clear
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                memset(fNvalsADC[iCr][iCh], 0, sizeof (UInt_t) * fNSamples);
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    fPedValTemp[iCr][iCh][iSmpl] = 0.0;
                    //                    fPedVal[iCr][iCh][iSmpl] = 0.0;
                    fPedSigRms[iCr][iCh][iSmpl] = 0.0;
                    fPedSigRms2[iCr][iCh][iSmpl] = 0.0;
                }
            }
        }
        for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS - 1; ++iEv) {
            // clear
            for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
                memset(fNvals[iCr], 0, sizeof (UInt_t) * fNChannels);
                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                    fPedCMS[iCr][iCh] = 0.0;
                    fSigCMS[iCr][iCh] = 0.0;
                }
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
                            fSigCMS[iCr][iCh] += fPedDat[iCr][iEv][iCh][iSmpl]; // CMS from current event
                            fPedCMS[iCr][iCh] += fPedVal[iCr][iCh][iSmpl]; // CMS over all pedestals
                            //                        fPedValTemp[iCr][iCh][iSmpl] += fPedDat[iCr][iEv][iCh][iSmpl]; // CMS from current event
                            //                        fNvalsADC[iCr][iCh][iSmpl]++;
                            fNvals[iCr][iCh]++;
                            rmsthr += Asig * Asig;
                            npreFiltered++;

                        }
                    }
                }
            // normalize
            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                    if (fNvals[iCr][iCh]) {
                        fSigCMS[iCr][iCh] /= fNvals[iCr][iCh];
                        fPedCMS[iCr][iCh] /= fNvals[iCr][iCh];
                    }
                    //                    if (iCr == 3 && iCh == 8) {
                    //                        printf("iter %i iEv %i nvals %i fPedCMS[iCr][iCh] %f\n", iter, iEv, fNvals[iCr][iCh], fPedCMS[iCr][iCh]);
                    //                    }
                }
            // Pedestals filtering
            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                    //                    if (iCr == 3 && iCh == 8)
                    //                        printf("iev %04i iter %i sig cms %f  cms %f\n", iEv, iter, fSigCMS[iCr][iCh], fPedCMS[iCr][iCh]);
                    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                        if (fPedDat[iCr][iEv][iCh][iSmpl] == 0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
                        Double_t Asig = TMath::Abs(
                                fPedDat[iCr][iEv][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + fPedCMS[iCr][iCh] - fSigCMS[iCr][iCh]);
                        if (Asig < thr) {
                            fPedValTemp[iCr][iCh][iSmpl] += fPedDat[iCr][iEv][iCh][iSmpl];
                            fNvalsADC[iCr][iCh][iSmpl]++;
                            rmsthrf += Sq(fPedDat[iCr][iEv][iCh][iSmpl] - fSigCMS[iCr][iCh]);
                            nFiltered++;

                        }
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
            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
                for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
                    fPedCMS0[iCr][iCh] = fPedCMS[iCr][iCh];
        }




        //hists fill
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fNvalsADC[iCr][iCh][iSmpl])
                        fPedVal[iCr][iCh][iSmpl] = fPedValTemp[iCr][iCh][iSmpl] / fNvalsADC[iCr][iCh][iSmpl];
                    fNvalsADC[iCr][iCh][iSmpl] = 0;
                    fPedCMS[iCr][iCh] += fPedVal[iCr][iCh][iSmpl]; // CMS over all pedestals
                    fNvals[iCr][iCh]++;
                }
            }
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
                if (fNvals[iCr][iCh]) {
                    fPedCMS[iCr][iCh] /= fNvals[iCr][iCh];
                }

        for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                    //                                    if (iter == nIters - 1 && iCr == 3 && iCh == 9) {
                    //        //                                printf("  fill     iter %i   cms %f\n", iter, fPedCMS[iCr][iCh]);
                    //                                        for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    //                                            //                        printf("cr %i channel %i  val %f\n", iCr, iCh,fPedDat[iCr][iEv][iCh][iSmpl]);
                    //                                            h->Fill( iSmpl, fPedDat[iCr][iEv][iCh][iSmpl]);
                    //                                            hp->Fill( iSmpl, fPedVal[iCr][iCh][iSmpl]);
                    //                                            hcms->Fill(iSmpl, fPedCMS[iCr][iCh]);
                    //                                            hscms_adc->Fill(iSmpl, /*fPedCMS[iCr][iCh] - */fSigCMS[iCr][iCh]);
                    //                                        }
                    //                                    }
                }
        }
        rmsthr = Sqrt(rmsthr / (npreFiltered));
//        printf("(sig -ped) rms = %f            filtered %i\n", rmsthr, npreFiltered);
        rmsthrf = Sqrt(rmsthrf / (nFiltered));
//        printf("(sig -ped + cms -scms) rms = %f  filtered %i\n", rmsthrf, nFiltered);


        // noise ch detection
        //cout << fDetName << " RMS calculation..." << endl;
        for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS - 1; ++iEv) {
            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                    Double_t signals[nSmpl];
                    for (Int_t i = 0; i < nSmpl; ++i) signals[i] = 0.0;
                    Int_t nOk = 0;
                    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                        if (fPedDat[iCr][iEv][iCh][iSmpl] == 0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
                        signals[iSmpl] = fPedDat[iCr][iEv][iCh][iSmpl];
                        nOk++;
                    }
                    Double_t CMS = CalcCMS(signals, nOk);
                    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                        if (fPedDat[iCr][iEv][iCh][iSmpl] == 0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
                        Float_t ped = fPedVal[iCr][iCh][iSmpl];
                        fPedRms[iCr][iCh][iSmpl] += Sq(signals[iSmpl] /*- fPedCMS[iCr][iCh] + CMS*/ - ped);
                        Double_t sig = signals[iSmpl] - fSigCMS[iCr][iCh]; // + fPedCMS[iCr][iCh] - ped; //fSigCMS[iCr][iCh];
                        fPedSigRms[iCr][iCh][iSmpl] += sig;
                        fPedSigRms2[iCr][iCh][iSmpl] += Sq(sig);
                        fNvalsADC[iCr][iCh][iSmpl]++;
                        //                                            printf(" iSmpl %d, signal %f, ped %f fSigCMS[iCr][iCh] %f fPedCMS[iCr][iCh] %f"
                        //                                                    "  CMS %f\n", iSmpl ,signals[iSmpl], ped, fSigCMS[iCr][iCh],fPedCMS[iCr][iCh],  CMS);
                    }
                }
        }
        sumRms = 0.0;
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
                    if (fNvalsADC[iCr][iCh][iSmpl]) {
                        fPedRms[iCr][iCh][iSmpl] = Sqrt(fPedRms[iCr][iCh][iSmpl] / fNvalsADC[iCr][iCh][iSmpl]);
                        fPedSigRms[iCr][iCh][iSmpl] /= fNvalsADC[iCr][iCh][iSmpl];
                        fPedSigRms2[iCr][iCh][iSmpl] = Sqrt(Abs(fPedSigRms2[iCr][iCh][iSmpl] / fNvalsADC[iCr][iCh][iSmpl] - Sq(fPedSigRms[iCr][iCh][iSmpl])));
                        sumRms += fPedSigRms2[iCr][iCh][iSmpl];
                    }
                }
        if (fNSerials * fNChannels)
            sumRms /= (fNSerials * fNChannels * nSmpl);
//        printf("sumRms = %f\n", sumRms);
        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {

                    //                    if (iCr == 3 && iCh == 8)
                    //                        printf("fPedSigRms[iCr][iCh][iSmpl] %f\n", fPedSigRms[iCr][iCh][iSmpl]);
                    if (fPedSigRms2[iCr][iCh][iSmpl] > 5 * sumRms) {
                        fNoisyChipChannels[iCr][iCh][iSmpl] = kTRUE;
                        //                                        printf("new noisy ch on  cr %i ch %i smpl %i\n", iCr, iCh, iSmpl);
                    }
                }
    }
//    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
//        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
//            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
//                if (fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
//                //                fPedVal[iCr][iCh][iSmpl] -= fPedCMS[iCr][iCh];
//                //                printf("fPedVal[iCr][iCh][iSmpl] = %f  fPedCMS0[iCr][iCh] = %f   iSmpl %i\n", fPedVal[iCr][iCh][iSmpl], fPedCMS[iCr][iCh], iSmpl);
//            }
//        }
    //
    //        canStrip->cd(1);
    //        h->Draw("colz");
    //        canStrip->cd(2);
    //        hp->Draw("colz");
    //        canStrip->cd(3);
    //        hcms->Draw("colz");
    //        canStrip->cd(4);
    //        hscms->Draw("colz");
    //        canStrip->SaveAs("can-cms.png");
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

void BmnAdcProcessor::Run7(Int_t* _statsGem, Int_t* _statsSil, Int_t* _statsGemPermut, Int_t* _statsSilPermut) {
    // Setup is valid for two modes (BM@N and SRC)
    // To be moved to the UniDb
    if (fBmnSetup == kBMNSETUP) {
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

    } else if (fBmnSetup == kSRCSETUP) {
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
    TString confSi = (fBmnSetup == kBMNSETUP) ? "SiliconRunSpring2018.xml" : "SiliconRunSRCSpring2018.xml";
    TString confGem = (fBmnSetup == kBMNSETUP) ? "GemRunSpring2018.xml" : "GemRunSRCSpring2018.xml";
    TString confCsc = "CSCRunSpring2018.xml";

    // SI
    TString gPathSiliconConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
    fDetectorSI = new BmnSiliconStationSet(gPathSiliconConfig + confSi);

    // GEM
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
    fDetectorGEM = new BmnGemStripStationSet(gPathGemConfig + confGem);

    // CSC
    TString gPathCscConfig = gPathConfig + "/parameters/csc/XMLConfigs/";
    fDetectorCSC = new BmnCSCStationSet(gPathCscConfig + confCsc);
    // Prepare arrays for GEM- and SI-converters and make
    statsGem = new Int_t[fDetectorGEM->GetNStations()];
    statsSil = new Int_t[fDetectorSI->GetNStations()];

    statsGemPermut = new Int_t[fDetectorGEM->GetNStations()];
    statsSilPermut = new Int_t[fDetectorSI->GetNStations()];

    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++) {
        statsGem[iStat] = -1;
        statsGemPermut[iStat] = -1;
    }

    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++) {
        statsSil[iStat] = -1;
        statsSilPermut[iStat] = -1;
    }

    Run7(statsGem, statsSil, statsGemPermut, statsSilPermut);

    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
        fGemStats[statsGem[iStat]] = statsGemPermut[iStat];

    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
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
    //                    printf("iCr = %i  iCh %i\n", iCr, iCh);
    //    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
    //        if (samples[iSmpl] == 0 || fPedVal[iCr][iCh][iSmpl] == 0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
    //        Double_t Asig = TMath::Abs(samples[iSmpl] - fPedVal[iCr][iCh][iSmpl]);
    ////                    printf("\tAsig = %f  sample = %f fpedVal = %f  iSmpl %i\n", Asig, samples[iSmpl], fPedVal[iCr][iCh][iSmpl], iSmpl);
    //        if (Asig < thrMax - thrDif * 1.0) {
    //            sigCMStemp += samples[iSmpl]; // CMS from current event
    //            pedCMStemp += fPedVal[iCr][iCh][iSmpl]; // CMS over all pedestals
    //            nvals++;
    //
    //        }
    //    }
    //    if (nvals) {
    //        sigCMS = sigCMStemp / nvals;
    //        pedCMS = pedCMStemp / nvals;
    //    }

    Double_t rmsthr = 500.0;
    Double_t sumRms = 200.0;
    Double_t sumRms2 = thrMax;
    Int_t nIters = 10;
    for (Int_t iter = 0; iter < nIters; iter++) {
        Double_t thr = /*(3.0 ) * sumRms2;*/ thrMax - thrDif * iter; //(2 + nIters - iter) * sumRms2; //thrMax - thrDif * iter;
        sumRms = 0.0;
        sumRms2 = 0.0;
        rmsthr = 0.0;
        pedCMStemp = 0.0;
        sigCMStemp = 0.0;
        nvals = 0;
        for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
            if (samples[iSmpl] == 0 || fPedVal[iCr][iCh][iSmpl] == 0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
            Double_t Asig = TMath::Abs(samples[iSmpl] - fPedVal[iCr][iCh][iSmpl] - sigCMS + pedCMS);
            //            printf("Asig = %f  thr = %f   iSmpl %i\n", Asig, thr, iSmpl);
            if (Asig < thr) {
                sigCMStemp += samples[iSmpl]; // CMS from current event
                pedCMStemp += fPedVal[iCr][iCh][iSmpl]; // CMS over all pedestals
                rmsthr += Asig * Asig;
                sumRms += samples[iSmpl] - sigCMS;
                sumRms2 += Sq(samples[iSmpl] - sigCMS);
                if (iter == nIters - 1 && iCr == 0 && iCh == 9) {
                    hscms1_adc->Fill(iSmpl, samples[iSmpl]);
                    hcms1->Fill(pedCMS);
                }
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
