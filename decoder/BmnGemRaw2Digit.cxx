#include "BmnGemRaw2Digit.h"
#include "BmnEventHeader.h"

BmnGemRaw2Digit::BmnGemRaw2Digit() {
    fSmall = NULL;
    fMid = NULL;
    fBigL0 = NULL;
    fBigL1 = NULL;
    fBigR0 = NULL;
    fBigR1 = NULL;
}

BmnGemRaw2Digit::BmnGemRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer, TString mapFileName, BmnSetup bmnSetup, BmnADCDecoMode decoMode) : BmnAdcProcessor(period, run, "GEM", ADC_N_CHANNELS, ADC32_N_SAMPLES, vSer) {

    fBmnSetup = bmnSetup;
    fSmall = NULL;
    fMid = NULL;
    fBigL0 = NULL;
    fBigL1 = NULL;
    fBigR0 = NULL;
    fBigR1 = NULL;

    cout << "Loading GEM Map: Period " << period << ", Run " << run << "..." << endl;

    fEventId = 0;

    fSmall = new BmnGemMap[N_CH_BUF];
    fMid = new BmnGemMap[N_CH_BUF];
    fBigL0 = new BmnGemMap[N_CH_BUF];
    fBigL1 = new BmnGemMap[N_CH_BUF];
    fBigR0 = new BmnGemMap[N_CH_BUF];
    fBigR1 = new BmnGemMap[N_CH_BUF];

    //    UniDbDetectorParameter* mapPar = UniDbDetectorParameter::GetDetectorParameter("GEM", "GEM_global_mapping", period, run);
    //    fEntriesInGlobMap = 0;
    //    if (mapPar != NULL) mapPar->GetGemMapArray(fMap, fEntriesInGlobMap);
    //    delete mapPar;
    //    for (Int_t i = 0; i < fEntriesInGlobMap; ++i)
    //        if (find(fSerials.begin(), fSerials.end(), fMap[i].serial) == fSerials.end())
    //            fSerials.push_back(fMap[i].serial);

    string dummy;
    UInt_t id = 0;
    UInt_t ser = 0;
    UInt_t ch_lo = 0;
    UInt_t ch_hi = 0;
    UInt_t station = 0;
    UInt_t hot = 0;
    //    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + TString(Form("GEM_map_run%d_SRC.txt", period));
    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + mapFileName;
    printf("%s\n", name.Data());
    ifstream inFile(name.Data());
    if (!inFile.is_open())
        cout << "Error opening map-file (" << name << ")!" << endl;
    for (Int_t i = 0; i < 5; ++i) getline(inFile, dummy); //comment line in input file

    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> ch_lo >> ch_hi >> id >> station >> hot;
        if (!inFile.good()) break;
        GemMapStructure record;
        record.channel_high = ch_hi;
        record.serial = ser;
        record.channel_low = ch_lo;
        record.station = station;
        record.hotZone = hot;
        record.id = id;
        fMap.push_back(record);
    }
    fEntriesInGlobMap = fMap.size();

    ReadMap("GEM_X_small", fSmall, 0, 0);
    ReadMap("GEM_Y_small", fSmall, 1, 0);

    ReadMap("GEM_X0_middle", fMid, 2, 0);
    ReadMap("GEM_Y0_middle", fMid, 3, 0);
    ReadMap("GEM_X1_middle", fMid, 0, 0);
    ReadMap("GEM_Y1_middle", fMid, 1, 0);

    ReadMap("GEM_X0_Big_Left", fBigL0, 2, 1);
    ReadMap("GEM_Y0_Big_Left", fBigL0, 3, 1);

    ReadMap("GEM_X1_Big_Left", fBigL1, 0, 1);
    ReadMap("GEM_Y1_Big_Left", fBigL1, 1, 1);

    ReadMap("GEM_X0_Big_Right", fBigR0, 2, 0);
    ReadMap("GEM_Y0_Big_Right", fBigR0, 3, 0);

    ReadMap("GEM_X1_Big_Right", fBigR1, 0, 0);
    ReadMap("GEM_Y1_Big_Right", fBigR1, 1, 0);

    const Int_t kNStations = 10;
    const Int_t kNStrips = 1300;

    if (decoMode == kBMNADCSM) {
        fSigProf = new TH1F***[kNStations];
        fNoisyChannels = new Bool_t***[kNStations];
        for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
            fSigProf[iSt] = new TH1F**[N_MODULES];
            fNoisyChannels[iSt] = new Bool_t**[N_MODULES];
            for (UInt_t iMod = 0; iMod < N_MODULES; ++iMod) {
                fSigProf[iSt][iMod] = new TH1F*[N_LAYERS];
                fNoisyChannels[iSt][iMod] = new Bool_t*[N_LAYERS];
                for (Int_t iLay = 0; iLay < N_LAYERS; ++iLay) {
                    TString histName;
                    histName.Form("GEM_%d_%d_%d", iSt, iMod, iLay);
                    fSigProf[iSt][iMod][iLay] = new TH1F(histName, histName, kNStrips, 0, kNStrips);
                    fSigProf[iSt][iMod][iLay]->SetDirectory(0);
                    fNoisyChannels[iSt][iMod][iLay] = new Bool_t[kNStrips];
                    for (Int_t iStrip = 0; iStrip < kNStrips; ++iStrip)
                        fNoisyChannels[iSt][iMod][iLay][iStrip] = kFALSE;
                }
            }
        }
        thrMax = 35;
        thrDif = 10;
    }

}

BmnStatus BmnGemRaw2Digit::ReadMap(TString parName, BmnGemMap* m, Int_t lay, Int_t mod) {
    Int_t size = 0;
    UniDbDetectorParameter* par = UniDbDetectorParameter::GetDetectorParameter("GEM", parName, GetPeriod(), GetRun());
    IIStructure* iiArr;
    if (par != NULL) par->GetIIArray(iiArr, size);
    delete par;
    for (Int_t i = 0; i < size; ++i)
        m[iiArr[i].int_2] = BmnGemMap(iiArr[i].int_1, lay, mod);
    delete[] iiArr;

    return kBMNSUCCESS;
}

BmnGemRaw2Digit::~BmnGemRaw2Digit() {
    if (fSmall) delete[] fSmall;
    if (fMid) delete[] fMid;
    if (fBigL0) delete[] fBigL0;
    if (fBigL1) delete[] fBigL1;
    if (fBigR0) delete[] fBigR0;
    if (fBigR1) delete[] fBigR1;
    //    if (fMap) delete[] fMap;

    const Int_t kNStations = 10;

    if (Rnoisefile == nullptr && Wnoisefile == nullptr) {
        for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
            for (UInt_t iMod = 0; iMod < N_MODULES; ++iMod) {
                for (Int_t iLay = 0; iLay < N_LAYERS; ++iLay) {
                    delete fSigProf[iSt][iMod][iLay];
                    delete[] fNoisyChannels[iSt][iMod][iLay];
                }
                delete[] fSigProf[iSt][iMod];
                delete[] fNoisyChannels[iSt][iMod];
            }
            delete[] fSigProf[iSt];
            delete[] fNoisyChannels[iSt];
        }
        delete[] fNoisyChannels;
        delete[] fSigProf;
        return;
    }
    // MK Postprocessing
    // !!!! search for noisy channels to write to the file
    if (test != 2) {
        for (Int_t it = 0; it < niter; ++it) {
            for (Int_t det = 0; det < ndet; ++det) {
                Int_t mChan = nchdet[det];

                Float_t sumhits[maxchip2];
                Int_t nsumch[maxchip2];
                for (Int_t j = 0; j < maxchip2; ++j) {
                    sumhits[j] = 0;
                    nsumch[j] = 0;
                }

                for (Int_t j = 0; j < mChan; ++j) {
                    if (noisech[det][j] == 0) {
                        Int_t ichip = (Int_t) j / 16;
                        sumhits[ichip] += hNhits[det]->GetBinContent(j + 1);
                        nsumch[ichip]++;
                    }
                }

                for (Int_t j = 0; j < maxchip2; ++j) {
                    Int_t nsum = nsumch[j];
                    if (nsum > 0) sumhits[j] /= nsum;
                }

                for (Int_t j = 0; j < mChan; ++j) {
                    if (noisech[det][j] == 0) {
                        Int_t ichip = (Int_t) j / 16;
                        Float_t sum = sumhits[ichip];
                        //                        if (it > 0)
                        //                            printf("nhits = %f sum = %f  det = %i ichip = %i j = %i\n", hNhits[det]->GetBinContent(j + 1), sum, det, ichip, j);

                        // cuts to define channel as noisy
                        if ((hNhits[det]->GetBinContent(j + 1) > 3 * sum && sum > 10) ||
                                (hNhits[det]->GetBinContent(j + 1) > 3 * sum && sum > 100) ||
                                (hNhits[det]->GetBinContent(j + 1) > 3 * sum && sum > 1000)) {

                            //                            cout << " new noise det= " << det << " chan= " << j << " iter= " << it << endl;
                            noisech[det][j] = 1;
                            if (!read) {
                                Int_t cont1 = j + det * 10000;
                                fprintf(Wnoisefile, " %d\n", cont1);
                            }
                        }
                    }
                }
            }
        }
    }// test = 2
    else {

        for (Int_t det = 0; det < ndet; ++det) {
            Int_t mChan = nchdet[det];

            for (Int_t j = -1; j < mChan; ++j) {
                if (noisech[det][j] == 0) {
                    Float_t sumhitdet = hNhits[det]->GetBinContent(j + 1) / (Float_t) npevents;
                    //      hnoise[det]->Fill(sumhitdet);
                    if (sumhitdet > thrnoise) {
                        //                        cout << " new noise det= " << det << " chan= " << j << " test2: noise> " << thrnoise << endl;
                        noisech[det][j] = 1;
                        if (!read) {
                            Int_t cont1 = j + det * 10000;
                            fprintf(Wnoisefile, " %d\n", cont1);
                        }
                    }
                }
            }

        }
    }
    //    cout << " npedevents= " << npevents << " nloopev= " << nev << endl;
    cout << endl;

    //    UInt_t ser = 0;
    //    if (nradc > 0) {
    //        Int_t nall = 0;
    //        for (Int_t ind = 0; ind < nradc; ind++) {
    //            ser = rSerials[ind];
    //            Int_t jind = -1;
    //            for (Int_t jnd = 0; jnd < nadc; jnd++) {
    //                if (ser == fSerials[jnd]) {
    //                    jind = jnd;
    //                    nall++;
    //                    break;
    //                }
    //            }
    //            //      cout << " Readout Serials= " << jind << " " << std::hex << ser << std::dec << endl;
    //        }
    //        cout << " ncoinc adc= " << nall << endl;
    //    }

    if (Wnoisefile)
        fclose(Wnoisefile);
    if (Rnoisefile)
        fclose(Rnoisefile);
    if (Wpedfile)
        fclose(Wpedfile);
    if (Pedfile)
        fclose(Pedfile);
    for (Int_t det = 0; det < ndet; det++) {
        if (hNhits[det])
            delete hNhits[det];
    }
    if (!read) {
        Int_t retn = system(Form("mv %s %s", wnoisename.Data(), rnoisename.Data()));
//        printf("mv     noise ret %d\n", retn);
        Int_t retp = system(Form("mv %s %s", wpedname.Data(), pedname.Data()));
//        printf("mv  pedestal ret %d\n", retp);
    }
}

BmnStatus BmnGemRaw2Digit::FillProfiles(TClonesArray *adc) {
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
        UInt_t ch = adcDig->GetChannel() * adcDig->GetNSamples();
        for (Int_t iMap = 0; iMap < fEntriesInGlobMap; ++iMap) {
            GemMapStructure gemM = fMap[iMap];
            if (adcDig->GetSerial() == gemM.serial && ch <= gemM.channel_high && ch >= gemM.channel_low) {
                ProcessDigit(adcDig, &gemM, NULL, kTRUE);
                break;
            }
        }
    }

    return kBMNSUCCESS;
}

BmnStatus BmnGemRaw2Digit::FillNoisyChannels() {
    const Int_t kNStations = 10;
    const Int_t kNStrips = 1300;
    const Int_t kNStripsInBunch = 32;
    const Int_t kNBunches = kNStrips / kNStripsInBunch;
    const Int_t kNThresh = 3;

    for (Int_t iSt = 0; iSt < kNStations; ++iSt)
        for (UInt_t iMod = 0; iMod < N_MODULES; ++iMod)
            for (Int_t iLay = 0; iLay < N_LAYERS; ++iLay) {
                TH1F* prof = fSigProf[iSt][iMod][iLay];
                for (Int_t iBunch = 0; iBunch < kNBunches; ++iBunch) {
                    Double_t meanDiff = 0.0;
                    Double_t mean = 0.0;
                    for (Int_t iStrip = 1; iStrip < kNStripsInBunch; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        Double_t curr = prof->GetBinContent(strip + 1);
                        Double_t prev = prof->GetBinContent(strip);
                        meanDiff += Abs(prev - curr);
                        mean += curr;
                    }
                    meanDiff /= kNStripsInBunch;
                    mean /= kNStripsInBunch;
                    for (Int_t iStrip = 1; iStrip < kNStripsInBunch; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        Double_t curr = prof->GetBinContent(strip + 1);
                        Double_t prev = prof->GetBinContent(strip);
                        //                        if (kNThresh * meanDiff < curr - prev)
                        if (kNThresh * mean < Abs(curr - mean))
                            fNoisyChannels[iSt][iMod][iLay][strip] = kTRUE;
                    }
                }
            }
    //    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr)
    //        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh)
    //            for (Int_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl)
    //                for (auto &it : fMap)
    //                    if (GetSerials()[iCr] == it.serial && iCh >= it.channel_low && iCh <= it.channel_high) {
    //                        if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) {
    //                            UInt_t iStrip = (iCh - it.channel_low) * GetNSamples() + 1 + iSmpl;
    //                            fNoisyChannels[it.station][it.module][it.layer][iStrip] = kTRUE;
    //                        }
    //                    }

    return kBMNSUCCESS;
}

BmnStatus BmnGemRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray * gem) {
    fEventId++;
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
        UInt_t ch = adcDig->GetChannel() * adcDig->GetNSamples();
        for (Int_t iMap = 0; iMap < fEntriesInGlobMap; ++iMap) {
            GemMapStructure gemM = fMap[iMap];
            if (adcDig->GetSerial() == gemM.serial && ch <= gemM.channel_high && ch >= gemM.channel_low) {
                ProcessDigit(adcDig, &gemM, gem, kFALSE);
                break;
            }
        }
    }

    return kBMNSUCCESS;
}

BmnStatus BmnGemRaw2Digit::FillEventMK(TClonesArray *adc, TClonesArray * gem, TClonesArray * csc) {
    fEventId++;
    nev++;
    for (Int_t det = 0; det < ndet; det++) {
        for (Int_t ich = 0; ich < maxchip; ich++) {
            Smode[det][ich] = 0;
            Cmode[det][ich] = 0;
            nchan[det][ich] = 0;

            Smode1[det][ich] = 0;
            Cmode1[det][ich] = 0;
        }
    }
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
        ProcessDigitMK(adcDig, gem, kFALSE);
    }
    PostprocessDigitMK(gem, csc);

    return kBMNSUCCESS;
}

void BmnGemRaw2Digit::ProcessDigitMK(BmnADCDigit* adcDig, TClonesArray * gem, Bool_t doFill) {
    UInt_t chan = adcDig->GetChannel();
    UInt_t ser = adcDig->GetSerial();
    Int_t nsmpl = adcDig->GetNSamples();

    Int_t iadc = -1;
    Int_t iradc = -1;

    for (Int_t jadc = 0; jadc < nadc; ++jadc) {
        if (ser == fSerials[jadc]) {
            iadc = jadc;
            break;
        }
    }
    if (iadc == -1 || nsmpl != nadc_samples) {
        //        cout << " iAdc= " << iadc << " chan= " << chan << " Wrong serial= " << std::hex << ser << std::dec << " Or nsmpl= " << nsmpl << endl;
    } else {

        for (Int_t ichan = 0; ichan < nadc_samples; ++ichan) {
            Int_t ic = chan * nadc_samples + ichan;

            Int_t det = detadc[iadc][ic];
            Int_t ich = ichadc[iadc][ic];
            if (ich >= 0 && det >= 0 && det < ndet) {
                Int_t mChan = nchdet[det];

                if (ich < mChan && noisech[det][ich] == 0) {

                    Double_t Adc = (GetRun() > GetBoundaryRun(ADC32_N_SAMPLES)) ?
                            ((Double_t) (adcDig->GetShortValue())[ichan] / 16) :
                            ((Double_t) (adcDig->GetUShortValue())[ichan] / 16);
                    Double_t Ped = Pedch[det][ich];
                    Ampch[det][ich] = Adc;
                    Double_t Sig = Ampch[det][ich] - Ped;
                    Double_t Asig = TMath::Abs(Sig);
                    Int_t ichip = (Int_t) ich / nchip;

                    Double_t thr = thresh;

                    if (GetRun() > 100 && GetRun() < 790 && det == 1 && thr < 20) thr = 20;
                    if (GetRun() > 1331 && GetRun() < 1600 && (det == 0 || det == 3) && thr < 30) thr = 30;
                    if (det >= ndetgem) thr = thrcsc;

                    //MK             Double_t thrmin2 = 3.5*Pedcmod2[det][ich];
                    //MK             if (thr < thrmin2 && thrmin2 > 0) thr = thrmin2;  

                    // cmode for channels without signal (below threshold) in event 
                    if (Asig < thr) {
                        Smode1[det][ichip] += Adc;
                        Cmode1[det][ichip] += Ped;
                        nchan[det][ichip]++;
                    }
                }
            }
        }
    }
}

void BmnGemRaw2Digit::PostprocessDigitMK(TClonesArray * gem, TClonesArray * csc) {
    //KV cmode in event iev 
    for (Int_t det = 0; det < ndet; det++) {
        Int_t mChan = nchdet[det];
        Int_t mchip = (Int_t) mChan / nchip;

        for (Int_t ich = 0; ich < mchip; ich++) {
            Int_t nch = nchan[det][ich];
            if (nch > nchmin) {
                Smode[det][ich] = Smode1[det][ich] / nch;
                Cmode[det][ich] = Cmode1[det][ich] / nch;
            } else {
                Smode[det][ich] = Cmall[det][ich];
                Cmode[det][ich] = Cmall[det][ich];
            }
        }
    }

    //KV iteration procedure               
    for (Int_t iter = 1; iter < niter; iter++) {

        for (Int_t det = 0; det < ndet; det++) {
            Int_t mChan = nchdet[det];
            Int_t mchip = (Int_t) mChan / nchip;

            for (Int_t ich = 0; ich < mchip; ich++) {
                nchan[det][ich] = 0;
                Smode1[det][ich] = 0;
                Cmode1[det][ich] = 0;
            }
        }

        for (Int_t det = 0; det < ndet; det++) {
            Int_t mChan = nchdet[det];

            for (Int_t ich = 0; ich < mChan; ich++) {
                Double_t Ped = Pedch[det][ich];
                Double_t Adc = Ampch[det][ich];

                Int_t ichip = (Int_t) ich / nchip;
                Double_t cmode = Cmode[det][ichip];
                Double_t smode = Smode[det][ichip];
                //KV excude signals above threshold for cmode calculation
                Double_t Sig = Adc - Ped + cmode - smode;
                Double_t Asig = TMath::Abs(Sig);
                Double_t thr = thresh - iter*dthr;
                if (GetRun() > 100 && GetRun() < 790 && det == 1 && thr < 20) thr = 20;
                if (GetRun() > 1331 && GetRun() < 1600 && (det == 0 || det == 3) && thr < 30) thr = 30;
                if (det >= ndetgem) thr = thrcsc - iter * dthrcsc;


                //MK          Double_t thrmin1 = 3.5*Pedcmod2[det][ich];
                //MK          if (thr < thrmin1 && thrmin1 > 0) thr = thrmin1;  

                // common mode for channels without signal and noise

                if (noisech[det][ich] == 0 && Asig < thr) {
                    Smode1[det][ichip] += Adc;
                    Cmode1[det][ichip] += Ped;
                    nchan[det][ichip]++;
                }
            }
        }

        for (Int_t det = 0; det < ndet; det++) {
            Int_t mChan = nchdet[det];
            Int_t mchip = (Int_t) mChan / nchip;

            for (Int_t ich = 0; ich < mchip; ich++) {
                Int_t nch = nchan[det][ich];
                //                if (iter == niter - 1) hChan[det]->Fill(nch);
                if (nch > nchmin) {
                    Smode[det][ich] = Smode1[det][ich] / nch;
                    Cmode[det][ich] = Cmode1[det][ich] / nch;
                } else {
                    Smode[det][ich] = Cmall[det][ich];
                    Cmode[det][ich] = Cmall[det][ich];
                }
            }
        }

        if (iter == niter - 1) {

            for (Int_t coor = 0; coor < ncoor; coor++) nchsig[coor] = 0;

            for (Int_t det = 0; det < ndet; det++) {

                Int_t jfirst = -1;
                Int_t jlast = -1;

                Int_t jfirstx0 = -1;
                Int_t jlastx0 = -1;
                Int_t jfirsty0 = -1;
                Int_t jlasty0 = -1;

                Int_t jfirstx1 = -1;
                Int_t jlastx1 = -1;
                Int_t jfirsty1 = -1;
                Int_t jlasty1 = -1;

                Double_t sumamp = 0;

                Double_t sumampx0 = 0;
                Double_t sumampy0 = 0;
                Double_t sumampx1 = 0;
                Double_t sumampy1 = 0;

                Double_t slast = 0;
                Double_t slastx0 = 0;
                Double_t slasty0 = 0;
                Double_t slastx1 = 0;
                Double_t slasty1 = 0;

                Int_t nclust = 0;
                Int_t nclustx0 = 0;
                Int_t nclusty0 = 0;
                Int_t nclustx1 = 0;
                Int_t nclusty1 = 0;

                Int_t nclsel = 0;
                Int_t nclselx0 = 0;
                Int_t nclsely0 = 0;
                Int_t nclselx1 = 0;
                Int_t nclsely1 = 0;

                for (Int_t ibin = 0; ibin < nx0max; ibin++) Ampx0[det][ibin] = 0;
                for (Int_t ibin = 0; ibin < nx1max; ibin++) Ampx1[det][ibin] = 0;
                for (Int_t ibin = 0; ibin < ny0max; ibin++) Ampy0[det][ibin] = 0;
                for (Int_t ibin = 0; ibin < ny1max; ibin++) Ampy1[det][ibin] = 0;

                Int_t mChan = nchdet[det];

                Int_t wclmax = 6;
                if (GetPeriod() == 6)
                    wclmax = 5;
                else {
                    if (dettype[det] == 5 || dettype[det] == 6) wclmax = 12;
                }

                for (Int_t ich = 0; ich < mChan; ich++) {
                    Double_t Ped = Pedch[det][ich];
                    Double_t Adc = Ampch[det][ich];

                    Int_t ichip = (Int_t) ich / nchip;
                    Double_t cmode = Cmode[det][ichip];
                    Double_t smode = Smode[det][ichip];
                    // signal
                    Double_t Signal = Adc - Ped + cmode - smode;
                    Double_t Sig = Signal;
                    if (test == 1) Sig = TMath::Abs(Signal);
                    Double_t thr = thresh - iter*dthr;
                    if (GetRun() > 100 && GetRun() < 790 && det == 1 && thr < 20) thr = 20;
                    if (GetRun() > 1331 && GetRun() < 1600 && (det == 0 || det == 3) && thr < 30) thr = 30;

                    // csc - negative signal
                    if (det >= ndetgem) {
                        thr = thrcsc - iter*dthrcsc;
                        Sig = -Signal;
                    }

                    Double_t thrmin1 = 3 * Pedcmod2[det][ich];
                    if (thr < thrmin1 && thrmin1 > 0) thr = thrmin1;
                    Double_t scmode = smode - cmode;

                    Int_t nch = nchan[det][ichip];
                    //                    if (scmode != 0) hSCmode[det]->Fill(scmode);
                    //                    hAmp[det]->Fill(Sig);

                    Int_t ibin = -1;
                    if (GetPeriod() == 6) {
                        if (det < nmiddle) ibin = chmap[ich];
                        else if (det == nbigL) ibin = chbigL[ich];
                        else if (det == nbigR) ibin = chbigR[ich];
                        else if (det == nbigL2) ibin = chbigL2[ich];
                        else if (det == nbigR2) ibin = chbigR2[ich];
                        else if (det == nsma) ibin = chsma[ich];
                    } else {
                        if ((det < nmiddle) && (fBmnSetup == kSRCSETUP)) ibin = chmap[ich];
                        else {
                            if (dettype[det] == 1) ibin = chbigL[ich];
                            else if (dettype[det] == 2) ibin = chbigR[ich];

                            else if (dettype[det] == 3) ibin = chbigL2[ich];
                            else if (dettype[det] == 4) ibin = chbigR2[ich];

                            else if (dettype[det] == 5) ibin = chuppercsc[ich];
                            else if (dettype[det] == 6) ibin = chlowercsc[ich];

                        }
                    }

                    //KV final hits and clusters

                    // hits above the threshold, noisy channels are masked

                    if (noisech[det][ich] == 0 && nch > nchmin && Sig > thr && ibin >= 0) {
                        hNhits[det]->Fill(ich);
                        if (ich > jlast + 1 && ich > jfirst + 1) {
                            jfirst = ich;
                            jlast = ich;
                            slast = ich*Sig;
                            nclust++;
                            sumamp = Sig;
                        } else if (ich >= jfirst && ich < jfirst + wclmax && jfirst > -1) {
                            jlast = ich;
                            slast += ich*Sig;
                            sumamp += Sig;
                        }

                        // hot zone x
                        if (ibin >= 0 && ibin < 1000) {
                            Int_t ibinx0 = ibin;
                            Int_t nx0 = nx0det[det];
                            if (ibinx0 < nx0) {
                                //                                hAmpx0[det]->Fill(Sig);
                                //                                hNAmpx0[det]->Fill(ibinx0);
                                Ampx0[det][ibinx0] = Sig;
                            } else cout << " ich= " << ich << " ibinx0= " << ibinx0 << endl;
                        }// cold zone x'

                        else if (ibin >= 4000) {
                            Int_t ibiny1 = ibin - 4000;
                            Int_t ny1 = ny1det[det];
                            if (ibiny1 < ny1) {
                                //                                hAmpy1[det]->Fill(Sig);
                                //                                hNAmpy1[det]->Fill(ibiny1);
                                Ampy1[det][ibiny1] = Sig;
                            } else cout << " ich= " << ich << " ibiny1= " << ibiny1 << endl;
                        }// cold zone x

                        else if (ibin >= 2000 && ibin < 4000) {
                            Int_t ibinx1 = ibin - 2000;
                            Int_t nx1 = nx1det[det];
                            if (ibinx1 < nx1) {
                                Ampx1[det][ibinx1] = Sig;
                            } else cout << " ich= " << ich << " ibinx1= " << ibinx1 << endl;
                        }// hot zone x'
                        else if (ibin >= 1000 && ibin < 2000) {
                            Int_t ibiny0 = ibin - 1000;
                            Int_t ny0 = ny0det[det];
                            if (ibiny0 < ny0) {
                                Ampy0[det][ibiny0] = Sig;
                            } else cout << " det= " << det << " ny0= " << ny0 << " ich= " << ich << " ibiny0= " << ibiny0 << endl;
                        }
                    } // threshold
                    if (ich > jlast + 1 && ich > jfirst + 1 && jfirst > -1 && jlast > 0 && nclust > nclsel) {
                        //     cout << " ich= " << ich << " jfirst= " << jfirst <<  " jlast= " << jlast << endl;
                        nclsel = nclust;
                        Int_t jwid = jlast - jfirst + 1;
                        Double_t sclust = slast / sumamp;
                        //                        Width[det]->Fill(jwid);
                        //                        Clust[det]->Fill(sclust);
                        //                        Samp[det]->Fill(sumamp);
                        sumamp = 0;
                        slast = 0;
                    }
                } // ich

                // form clusters for histograming and fill final array of hits

                // hot zone x
                Int_t nx0 = nx0det[det];
                for (Int_t ibin = 0; ibin < nx0; ibin++) {
                    Double_t Sig = Ampx0[det][ibin];
                    if (Sig > 0) {

                        Int_t coor = det * 4;
                        Int_t nch = nchsig[coor];

                        // fill final array of hits
                        sigx1[coor][nch] = Sig;
                        nsigx1[coor][nch] = ibin;
                        nchsig[coor]++;
                        //           if (iEv < npedev+10) 
                        //             cout << " iEv= " << iEv << " det= " << det << " coorx0= " << coor << " nch= " << nch << " ibin= " << ibin << " Sig= " << Sig << endl;

                        if (ibin > jlastx0 + 1 && ibin > jfirstx0 + 1) {
                            jfirstx0 = ibin;
                            jlastx0 = ibin;
                            slastx0 = ibin*Sig;
                            nclustx0++;
                            sumampx0 = Sig;
                        } else if (ibin >= jfirstx0 && ibin < jfirstx0 + wclmax && jfirstx0 > -1) {
                            jlastx0 = ibin;
                            slastx0 += ibin*Sig;
                            sumampx0 += Sig;
                        }
                    }

                    if (ibin > jlastx0 + 1 && ibin > jfirstx0 + 1 && jfirstx0 > -1 && jlastx0 > 0 && nclustx0 > nclselx0) {
                        nclselx0 = nclustx0;
                        Int_t jwid = jlastx0 - jfirstx0 + 1;
                        Double_t sclust = slastx0 / sumampx0;

                        //                        WidthX0[det]->Fill(jwid);
                        //                        ClustX0[det]->Fill(sclust);
                        //                        SampX0[det]->Fill(sumampx0);
                        sumampx0 = 0;
                        slastx0 = 0;
                    }
                }

                // cold zone x'

                Int_t ny1 = ny1det[det];
                for (Int_t ibin = 0; ibin < ny1; ibin++) {
                    Double_t Sig = Ampy1[det][ibin];
                    if (Sig > 0) {

                        Int_t coor = det * 4 + 3;
                        Int_t nch = nchsig[coor];

                        // fill final array of hits
                        sigx1[coor][nch] = Sig;
                        nsigx1[coor][nch] = ibin;
                        nchsig[coor]++;
                        //           if (iEv < npedev+10) 
                        //             cout << " iEv= " << iEv << " det= " << det << " coory1= " << coor << " nch= " << nch << " ibin= " << ibin << " Sig= " << Sig << endl;


                        if (ibin > jlasty1 + 1 && ibin > jfirsty1 + 1) {
                            jfirsty1 = ibin;
                            jlasty1 = ibin;
                            slasty1 = ibin*Sig;
                            nclusty1++;
                            sumampy1 = Sig;
                        } else if (ibin >= jfirsty1 && ibin < jfirsty1 + wclmax && jfirsty1 > -1) {
                            jlasty1 = ibin;
                            slasty1 += ibin*Sig;
                            sumampy1 += Sig;
                        }
                    }

                    if (ibin > jlasty1 + 1 && ibin > jfirsty1 + 1 && jfirsty1 > -1 && jlasty1 > 0 && nclusty1 > nclsely1) {
                        nclsely1 = nclusty1;
                        Int_t jwid = jlasty1 - jfirsty1 + 1;
                        Double_t sclust = slasty1 / sumampy1;
                        Clusty[det] = sclust;
                        if (nclsely1 < nclmax) Clustyy[det][nclsely1 - 1] = sclust;

                        //                        WidthY1[det]->Fill(jwid);
                        //                        ClustY1[det]->Fill(sclust);
                        //                        SampY1[det]->Fill(sumampy1);
                        sumampy1 = 0;
                        slasty1 = 0;
                    }
                }

                // cold zone x
                Int_t nx1 = nx1det[det];
                for (Int_t ibin = 0; ibin < nx1; ibin++) {
                    Double_t Sig = Ampx1[det][ibin];
                    if (Sig > 0) {

                        Int_t coor = det * 4 + 2;
                        Int_t nch = nchsig[coor];

                        // fill final array of hits
                        sigx1[coor][nch] = Sig;
                        nsigx1[coor][nch] = ibin;
                        nchsig[coor]++;
                        //           if (iEv < npedev+10) 
                        //             cout << " iEv= " << iEv << " det= " << det << " coorx1= " << coor << " nch= " << nch << " ibin= " << ibin << " Sig= " << Sig << endl;

                        if (ibin > jlastx1 + 1 && ibin > jfirstx1 + 1) {
                            jfirstx1 = ibin;
                            jlastx1 = ibin;
                            slastx1 = ibin*Sig;
                            nclustx1++;
                            sumampx1 = Sig;
                        } else if (ibin >= jfirstx1 && ibin < jfirstx1 + wclmax && jfirstx1 > -1) {
                            jlastx1 = ibin;
                            slastx1 += ibin*Sig;
                            sumampx1 += Sig;
                        }
                    }

                    if (ibin > jlastx1 + 1 && ibin > jfirstx1 + 1 && jfirstx1 > -1 && jlastx1 > 0 && nclustx1 > nclselx1) {
                        nclselx1 = nclustx1;
                        Int_t jwid = jlastx1 - jfirstx1 + 1;
                        Double_t sclust = slastx1 / sumampx1;
                        Clustx[det] = sclust;
                        if (nclselx1 < nclmax) Clustxx[det][nclselx1 - 1] = sclust;

                        //                        WidthX1[det]->Fill(jwid);
                        //                        ClustX1[det]->Fill(sclust);
                        //                        SampX1[det]->Fill(sumampx1);
                        sumampx1 = 0;
                        slastx1 = 0;
                    }
                }

                // hot zone x'
                Int_t ny0 = ny0det[det];
                for (Int_t ibin = 0; ibin < ny0; ibin++) {
                    Double_t Sig = Ampy0[det][ibin];
                    if (Sig > 0) {

                        Int_t coor = det * 4 + 1;
                        Int_t nch = nchsig[coor];

                        // fill final array of hits
                        sigx1[coor][nch] = Sig;
                        nsigx1[coor][nch] = ibin;
                        nchsig[coor]++;
                        //           if (iEv < npedev+10) 
                        //              cout << " iEv= " << iEv << " det= " << det << " coory0= " << coor << " nch= " << nch << " ibin= " << ibin << " Sig= " << Sig << endl;

                        if (ibin > jlasty0 + 1 && ibin > jfirsty0 + 1) {
                            jfirsty0 = ibin;
                            jlasty0 = ibin;
                            slasty0 = ibin*Sig;
                            nclusty0++;
                            sumampy0 = Sig;
                        } else if (ibin >= jfirsty0 && ibin < jfirsty0 + wclmax && jfirsty0 > -1) {
                            jlasty0 = ibin;
                            slasty0 += ibin*Sig;
                            sumampy0 += Sig;
                        }
                    }

                    if (ibin > jlasty0 + 1 && ibin > jfirsty0 + 1 && jfirsty0 > -1 && jlasty0 > 0 && nclusty0 > nclsely0) {
                        nclsely0 = nclusty0;
                        sumampy0 = 0;
                        slasty0 = 0;
                    }
                }
                Nclustx[det] = nclselx1;
                Nclusty[det] = nclsely1;

            } // ndet

            // fiil final array of BmnGemStripDigit
            // transition to final detector / modul/ plane numbers

            Int_t nchsum = 0;
            for (Int_t coor = 0; coor < ncoor; ++coor) {
                Int_t nch = nchsig[coor];
                //        if (iEv < npedev+10 && nch > 0) cout << " coor= " << coor << " nch= " << nch << endl;  
                if (nch > 0) {
                    for (int ich = 0; ich < nch; ++ich) {
                        Int_t isig = nsigx1[coor][ich];
                        Float_t sig = sigx1[coor][ich];

                        Int_t strip = isig + 1;
                        Int_t det0 = (Int_t) coor / 4;
                        Int_t mod = modul[det0];
                        Int_t layer = coor - det0 * 4;
                        Int_t det = detorder[det0];
                        Int_t plane = layer;
                        plane = (2 + plane) % 4; // 0,1 <-> 2,3
                        if (read) {
                            //                            printf("det = %i mod = %i layer = %i strip = %i \n", det, mod, layer, strip);
                            // convert MK -> SM
                            if (GetPeriod() == 6) {
                                strip -= 1; // strips should be enumerated from zero
                                det -= 1; // stations should be enumerated from zero
                                new((*gem)[gem->GetEntriesFast()]) BmnGemStripDigit(det, mod, plane, strip, sig);

                            } else
                                if (!((det == 8) && (fBmnSetup == kBMNSETUP)) && !((det == 11) && (fBmnSetup == kSRCSETUP))) {
                                strip -= 1; // strips should be enumerated from zero
                                det = fGemStats.find(det)->second;
                                if (
                                        (det == 0 || det == 3 || det == 5) && fBmnSetup == kBMNSETUP ||
                                        (det == 5 || det == 7 || det == 9) && fBmnSetup == kSRCSETUP) {
                                    if (mod == 0)
                                        mod = 1;
                                    else if (mod == 1)
                                        mod = 0;
                                    else {
                                        cout << "Something went wrong!" << endl;
                                        throw;
                                    }
                                }
                                //                                printf("GEM det = %i mod = %i layer = %i strip = %i \n", det, mod, layer, strip);
                                new((*gem)[gem->GetEntriesFast()]) BmnGemStripDigit(det, mod, plane, strip, sig);
                            } else {
                                strip -= 1; // strips should be enumerated from zero
                                // Modules should be permutated! (mod0 <--> mod1)
                                if (mod == 0) {
                                    mod = 1;
                                    // Layers should be permutated in mod0 only
                                    plane = (2 + plane) % 4; // 0,1 <-> 2,3
                                } else if (mod == 1)
                                    mod = 0;

                                else {
                                    cout << "Something went wrong!" << endl;
                                    throw;
                                }
                                //                                printf("CSC det = %i mod = %i layer = %i strip = %i \n", det, mod, layer, strip);
                                new ((*csc)[csc->GetEntriesFast()]) BmnCSCDigit(0, mod, plane, strip, sig);
                            }
                        }
                    }
                    nchsum++;
                }
            }

        } // iter=niter-1

    } // niter
}

void BmnGemRaw2Digit::ProcessDigit(BmnADCDigit* adcDig, GemMapStructure* gemM, TClonesArray * gem, Bool_t doFill) {
    const UInt_t nSmpl = adcDig->GetNSamples();
    UInt_t ch = adcDig->GetChannel();
    UInt_t ser = adcDig->GetSerial();

    Int_t iSer = -1;
    for (iSer = 0; iSer < GetSerials().size(); ++iSer)
        if (ser == GetSerials()[iSer]) break;
    if (iSer == GetSerials().size())
        return; // serial not found

    BmnGemStripDigit candDig[nSmpl];

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        Int_t strip = -1;
        Int_t lay = -1;
        Int_t mod = -1;
        Int_t ch2048 = ch * nSmpl + iSmpl;
        UInt_t realChannel = ch2048;
        BmnGemMap* fBigMap = NULL;

        if (gemM->id < 10) {
            if ((gemM->channel_high - gemM->channel_low) < 128) realChannel = (2048 + ch2048 - gemM->channel_low);
            fBigMap = fMid;
        } else {
            if (gemM->hotZone % 2 == 0) { //hot zone
                if (gemM->channel_low == 0)
                    realChannel += 1024;
                if (gemM->id % 10 == 0)
                    fBigMap = fBigL0;
                else
                    fBigMap = fBigR0;
            } else { //big zone
                if (gemM->id % 10 == 0)
                    fBigMap = fBigL1;
                else
                    fBigMap = fBigR1;
                if (gemM->channel_high - gemM->channel_low < 128) realChannel = (2048 + ch2048 - gemM->channel_low);
            }
        }
        lay = fBigMap[realChannel].lay;
        mod = gemM->hotZone >> 1;
        strip = fBigMap[realChannel].strip;

        if (strip > 0) {
            BmnGemStripDigit dig;
            dig.SetStation(gemM->station);
            dig.SetModule(mod);
            dig.SetStripLayer(lay);
            dig.SetStripNumber(strip);
            Double_t sig = (GetRun() > GetBoundaryRun(ADC32_N_SAMPLES)) ? ((Double_t) ((adcDig->GetShortValue())[iSmpl] / 16)) : ((Double_t) ((adcDig->GetUShortValue())[iSmpl] / 16));
            dig.SetStripSignal(sig);
            candDig[iSmpl] = dig;
        }
    }

    Double_t signals[nSmpl];
    Int_t nOk = 0;
    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStripSignal() == 0) continue;
        signals[iSmpl] = (candDig[iSmpl]).GetStripSignal();
        nOk++;
    }
    Double_t CMS = CalcCMS(signals, nOk);
    Double_t SCMS = CalcSCMS(signals, nSmpl, iSer, ch);

    Double_t*** vPed = GetPedestals();
    Double_t*** vPedRMS = GetPedestalsRMS();

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStation() == -1) continue;

        BmnGemStripDigit * dig = &candDig[iSmpl];
        Double_t ped = vPed[iSer][ch][iSmpl];
        //        Double_t sig = Abs(dig->GetStripSignal() - CMS - ped);
        Double_t sig = Abs(dig->GetStripSignal() - SCMS - ped);
        Float_t threshold = Max(35.0, 3.5 * vPedRMS[iSer][ch][iSmpl]); //15 + 4 * vPedRMS[iSer][ch][iSmpl]; //20;
        //        if (vPedRMS[iSer][ch][iSmpl] != 0)
        //            printf(" iSer %d, ch %d, iSmpl %d, vPedRMS %f\n", iSer, ch ,iSmpl, vPedRMS[iSer][ch][iSmpl]);
        //    printf("ProcessDigit candDig[iSmpl]).GetStation() = %i sig = %f threshold = %f\n", candDig[iSmpl].GetStation(), sig, threshold);
        if (sig < threshold || sig == 0.0) continue; //FIXME: check cases with sig == 0
        if (doFill) {
            fSigProf[dig->GetStation()][dig->GetModule()][dig->GetStripLayer()]->Fill(dig->GetStripNumber());
        } else {
            BmnGemStripDigit * resDig = new((*gem)[gem->GetEntriesFast()]) BmnGemStripDigit(dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), sig);
//            printf("st %d  mod %d lay %d strip %d\n",dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber() );
            if (fNoisyChannels[dig->GetStation()][dig->GetModule()][dig->GetStripLayer()][dig->GetStripNumber()])
                resDig->SetIsGoodDigit(kFALSE);
            else
                resDig->SetIsGoodDigit(kTRUE);
        }
    }
}

void BmnGemRaw2Digit::InitAdcProcessorMK(Int_t run, Int_t iread, Int_t iped, Int_t ithr, Int_t itest) {
    CreateGeometries();
    test = itest;
    if (iread > 0) read = kTRUE;
    if (iped > 0) pedestals = kTRUE;
    if (ithr == 1) thresh = 38;
    if (ithr == 1) thrcsc = 90;
    if (test == 2) {
        thresh = thrped;
        thrcsc = thrpedcsc;
        niter = niterped;
    }
    TString FSerials;
    switch (GetPeriod()) {
        case 7:
            nadc = 26;
            thresh = 35;
            thrcsc = 80;
            thrnoise = 0.03;
            dthr = 10;
            dthrcsc = 15;
            // starting thresholds, number of iterations
            niter = 3;
            niterped = 3;
            thrped = 35;
            thrpedcsc = 80;
            if (fBmnSetup == kBMNSETUP) {
                nmiddle = 0;
                nbig = 7;
                ndet = 16;
                ncoor = 64;
                ndetgem = 14;
                nbigLdet = {1, 3, 5, 7, 9, 11, 13};
                nbigRdet = {2, 4, 6, 8, 10, 12, 14};
                nbigLxy = {6, 10, 12, 16, 18, 22, 2};
                nbigLxy0 = {7, 7, 13, 13, 19, 19, 1};
                nbigRxy = {8, 11, 14, 17, 20, 23, 3};
                nbigRxy0 = {9, 9, 15, 15, 21, 21, 4};
                nbigshift = {0, 1024, 1024, 0, 1024, 0, 1024};
                // swaped R<->L Gem 6 and Gem7
                // correspondence adc - > detector (big Gems), additional channels
                nbigLadd = {640, 1920, 1664, 1024, 1280, 896, 512};
                nbigRadd = {768, 1792, 1536, 1152, 1408, 0, 0};
                //Run 7 BM@N configuration
                // detector order and modul number in the final array
                detorder = {4, 4, 5, 5, 3, 3, 6, 6, 7, 7, 2, 2, 1, 1, 8, 8};
                modul = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
                // det mapping type GEM / GEM2 / CSC
                dettype = {1, 2, 3, 4, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 5, 6};
                FSerials.Form("%s/input/GEM_SerialsBMN_Period%d.txt", getenv("VMCWORKDIR"), GetPeriod());
            } else {
                if (fBmnSetup == kSRCSETUP) {
                    nmiddle = 4;
                    nbig = 6;
                    ndet = 18;
                    ncoor = 72;
                    ndetgem = 16;
                    nmidadd = {128, 256, 384, 896};
                    nbigLdet = {5, 7, 9, 11, 13, 15};
                    nbigRdet = {6, 8, 10, 12, 14, 16};
                    nbigLxy = {6, 10, 12, 16, 18, 22};
                    nbigLxy0 = {7, 7, 13, 13, 19, 19};
                    nbigRxy = {8, 11, 14, 17, 20, 23};
                    nbigRxy0 = {9, 9, 15, 15, 21, 21};
                    nbigshift = {0, 1024, 1024, 0, 1024, 0};
                    // swaped R<->L Gem 6 and Gem7
                    nbigLadd = {640, 1920, 1664, 1024, 1280, 896};
                    nbigRadd = {768, 1792, 1536, 1152, 1408, 0};
                    detorder = {4, 1, 3, 2, 7, 7, 8, 8, 6, 6, 9, 9, 10, 10, 5, 5, 11, 11};
                    modul = {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
                    dettype = {0, 0, 0, 0, 3, 4, 1, 2, 3, 4, 3, 4, 1, 2, 1, 2, 5, 6};
                    FSerials.Form("%s/input/GEM_SerialsSRC_Period%d.txt", getenv("VMCWORKDIR"), GetPeriod());
                } else
                    fprintf(stderr, "Wrong setup!\n");
            }
            break;
        case 6:
            thresh = 43;
            if (ithr == 1) thresh = 46;
            niter = 5;
            niterped = 3;
            thrped = 29;
            dthr = 7;
            nadc = 12;
            nmiddle = 5;
            ndet = 10;
            ncoor = 40;
            detorder = {4, 2, 3, 4, 1, 6, 6, 5, 5, 0};
            modul = {1, 0, 0, 0, 0, 1, 0, 1, 0, 0};
            FSerials.Form("%s/input/GEM_SerialsBMN_Period%d.txt", getenv("VMCWORKDIR"), GetPeriod());
            break;
        default:
            fprintf(stderr, "Unsupported period %d !\n", GetPeriod());
            break;
    }
    nx0det.resize(ndet, 0);
    ny0det.resize(ndet, 0);
    nx1det.resize(ndet, 0);
    ny1det.resize(ndet, 0);
    nchdet.resize(ndet, 0);
    Pedchr.resize(ndet, vector<Float_t>(maxChan, 0.0));
    Pedchr2.resize(ndet, vector<Float_t>(maxChan, 0.0));
    Ampx0.resize(ndet, vector<Float_t>(nx0max, 0.0));
    Ampy0.resize(ndet, vector<Float_t>(ny0max, 0.0));
    Ampx1.resize(ndet, vector<Float_t>(nx1max, 0.0));
    Ampy1.resize(ndet, vector<Float_t>(ny1max, 0.0));
    hNhits.resize(ndet, nullptr);
    Nclustx.resize(ndet, 0);
    Nclusty.resize(ndet, 0);
    Clustx.resize(ndet, 0.0);
    Clusty.resize(ndet, 0.0);

    nchan.resize(ndet, vector<Int_t>(maxchip, 0));
    nchan1.resize(ndet, vector<Int_t>(maxChan, 0));

    Ampch.resize(ndet, vector<Float_t>(maxChan, 0.0));
    Pedch.resize(ndet, vector<Float_t>(maxChan, 0.0));
    Pedch2.resize(ndet, vector<Double_t>(maxChan, 0.0));

    Ped1ch.resize(ndet, vector<Float_t>(maxChan, 0.0));
    Ped1ch2.resize(ndet, vector<Double_t>(maxChan, 0.0));

    Ped1cmod.resize(ndet, vector<Float_t>(maxChan, 0.0));
    Ped1cmod2.resize(ndet, vector<Double_t>(maxChan, 0.0));

    Pedcmod.resize(ndet, vector<Float_t>(maxChan, 0.0));
    Pedcmod2.resize(ndet, vector<Double_t>(maxChan, 0.0));

    Cmode.resize(ndet, vector<Float_t>(maxchip, 0.0));
    C1mode.resize(ndet, vector<Float_t>(maxchip, 0.0));
    Cmall.resize(ndet, vector<Float_t>(maxchip, 0.0));
    Smode.resize(ndet, vector<Float_t>(maxchip, 0.0));
    Cmode2.resize(ndet, vector<Float_t>(maxchip, 0.0));

    Cmode1.resize(ndet, vector<Float_t>(maxchip, 0.0));
    Smode1.resize(ndet, vector<Float_t>(maxchip, 0.0));
    Clustxx.resize(ndet, vector<Float_t>(nclmax, 0.0));
    Clustyy.resize(ndet, vector<Float_t>(nclmax, 0.0));

    Asample.resize(ndet, vector<Float_t>(maxChan, 0.0));
    sigx1.resize(ncoor, vector<Float_t>(maxChan, 0));
    nsigx1.resize(ncoor, vector<Int_t>(maxChan, 0));
    noisech.resize(ndet, vector<Int_t>(maxChan, 0));
    nchsig.resize(ncoor, 0);
    fSerials.resize(nadc, 0);
    rSerials.resize(nadcmax, 0);
    detadc.resize(nadc, vector<Int_t>(maxAdc, 0));
    ichadc.resize(nadc, vector<Int_t>(maxAdc, 0));

    for (Int_t ind = 0; ind < nadcmax; ind++) {
        rSerials[ind] = 0;
    }
    ifstream inFile(FSerials);
    if (!inFile.is_open())
        cout << "Error opening File with ADC Serials (" << FSerials << ")!" << endl;

    UInt_t ser;
    for (Int_t ind = 0; ind < nadc; ind++) {
        inFile >> std::hex >> ser;
        fSerials[ind] = ser;
        //        cout << " Serials= " << ind << " " << std::hex << ser << std::dec << endl;
    }
    for (Int_t i = 0; i < nadc; i++) {
        for (Int_t ic = 0; ic < maxAdc; ic++) {
            detadc[i][ic] = -1;
            ichadc[i][ic] = -1;
            //      nchadc[i][ic] = 0;
            //      Pedadc[i][ic] = 0;
            //      Pedadc2[i][ic] = 0;
        }
    }
    if (GetPeriod() == 6) {
        for (Int_t i = 0; i < nmiddle; i++) {
            nx0det[i] = nx0bin;
            nx1det[i] = nx1bin;
            ny0det[i] = ny0bin;
            ny1det[i] = ny1bin;
            nchdet[i] = nallmid;

            for (Int_t ic = 0; ic < maxAdc; ic++) {
                detadc[i][ic] = i;
                ichadc[i][ic] = ic;
            }

            for (Int_t ic = 0; ic < 128; ic++) {
                Int_t ich = i * 128 + ic;
                if (i == nmiddle - 1) ich = 896 + ic;
                detadc[5][ich] = i;
                ichadc[5][ich] = ic + maxAdc;
            }
        }

        nx0det[nbigL] = nx0big;
        nx1det[nbigL] = nx1big;
        ny0det[nbigL] = ny0bigL;
        ny1det[nbigL] = ny1bigL;
        nchdet[nbigL] = nallbig;

        nx0det[nbigL2] = nx0big;
        nx1det[nbigL2] = nx1big;
        ny0det[nbigL2] = ny0bigL;
        ny1det[nbigL2] = ny1bigL;
        nchdet[nbigL2] = nallbig;

        for (Int_t ic = 0; ic < maxAdc; ic++) {
            detadc[7][ic] = nbigL;
            ichadc[7][ic] = ic;

            detadc[10][ic] = nbigL2;
            ichadc[10][ic] = ic;
        }

        for (Int_t ic = 0; ic < 128; ic++) {
            Int_t ich = 640 + ic;
            detadc[5][ich] = nbigL;
            ichadc[5][ich] = ic + maxAdc;

            ich = 1792 + ic;
            detadc[5][ich] = nbigL2;
            ichadc[5][ich] = ic + maxAdc;
        }
        for (Int_t ic = 0; ic < 1024; ic++) {
            detadc[6][ic] = nbigL;
            ichadc[6][ic] = ic + maxAdc + 128;

            Int_t ich = 1024 + ic;
            detadc[6][ich] = nbigL2;
            ichadc[6][ich] = ic + maxAdc + 128;
        }

        nx0det[nbigR] = nx0big;
        nx1det[nbigR] = nx1big;
        ny0det[nbigR] = ny0bigR;
        ny1det[nbigR] = ny1bigR;
        nchdet[nbigR] = nallbig;

        nx0det[nbigR2] = nx0big;
        nx1det[nbigR2] = nx1big;
        ny0det[nbigR2] = ny0bigR;
        ny1det[nbigR2] = ny1bigR;
        nchdet[nbigR2] = nallbig;

        for (Int_t ic = 0; ic < maxAdc; ic++) {
            detadc[8][ic] = nbigR;
            ichadc[8][ic] = ic;

            detadc[11][ic] = nbigR2;
            ichadc[11][ic] = ic;
        }
        for (Int_t ic = 0; ic < 128; ic++) {
            Int_t ich = 768 + ic;
            detadc[5][ich] = nbigR;
            ichadc[5][ich] = ic + maxAdc;

            ich = 1920 + ic;
            detadc[5][ich] = nbigR2;
            ichadc[5][ich] = ic + maxAdc;
        }
        for (Int_t ic = 0; ic < 1024; ic++) {
            detadc[9][ic] = nbigR;
            ichadc[9][ic] = ic + maxAdc + 128;

            Int_t ich = 1024 + ic;
            detadc[9][ich] = nbigR2;
            ichadc[9][ich] = ic + maxAdc + 128;
        }

        nx0det[nsma] = nsmall;
        nx1det[nsma] = nsmall;
        ny0det[nsma] = nsmall;
        ny1det[nsma] = nsmall;
        nchdet[nsma] = nallsma;

        for (Int_t ic = 0; ic < nallsma; ic++) {
            Int_t ich = 1024 + ic;
            detadc[5][ich] = nsma;
            ichadc[5][ich] = ic;
        }
        for (Int_t ibin = 0; ibin < nallsma; ibin++) chsma[ibin] = -1;
    } else {
        if (fBmnSetup == kSRCSETUP) {
            for (Int_t i = 0; i < nmiddle; i++) {
                nx0det[i] = nx0bin;
                nx1det[i] = nx1bin;
                ny0det[i] = ny0bin;
                ny1det[i] = ny1bin;
                nchdet[i] = nallmid;

                for (Int_t ic = 0; ic < maxAdc; ic++) {
                    detadc[i + 1][ic] = i;
                    ichadc[i + 1][ic] = ic;
                }

                for (Int_t ic = 0; ic < 128; ic++) {
                    Int_t ich = nmidadd[i] + ic;

                    if (i == 2) {
                        detadc[5][ich] = i;
                        ichadc[5][ich] = ic + maxAdc;
                    } else {
                        detadc[0][ich] = i;
                        ichadc[0][ich] = ic + maxAdc;
                    }
                }
            } // nmiddle
        }

        for (Int_t n = 0; n < nbig; n++) {
            Int_t nbigLlocal = nbigLdet[n] - 1;
            nx0det[nbigLlocal] = nx0big;
            nx1det[nbigLlocal] = nx1big;
            ny0det[nbigLlocal] = ny0bigL;
            ny1det[nbigLlocal] = ny1bigL;
            nchdet[nbigLlocal] = nallbig;

            Int_t nbigRlocal = nbigRdet[n] - 1;
            nx0det[nbigRlocal] = nx0big;
            nx1det[nbigRlocal] = nx1big;
            ny0det[nbigRlocal] = ny0bigR;
            ny1det[nbigRlocal] = ny1bigR;
            nchdet[nbigRlocal] = nallbig;
            for (Int_t ic = 0; ic < maxAdc; ic++) {
                Int_t ich = nbigLxy[n];
                detadc[ich][ic] = nbigLlocal;
                ichadc[ich][ic] = ic;

                ich = nbigRxy[n];
                detadc[ich][ic] = nbigRlocal;
                ichadc[ich][ic] = ic;
            }

            for (Int_t ic = 0; ic < 128; ic++) {
                Int_t ich = nbigLadd[n] + ic;
                detadc[5][ich] = nbigLlocal;
                ichadc[5][ich] = ic + maxAdc;

                ich = nbigRadd[n] + ic;
                detadc[5][ich] = nbigRlocal;
                ichadc[5][ich] = ic + maxAdc;
            }

            for (Int_t ic = 0; ic < 1024; ic++) {
                Int_t iadc = nbigLxy0[n];
                Int_t ich = nbigshift[n] + ic;
                detadc[iadc][ich] = nbigLlocal;
                ichadc[iadc][ich] = ic + maxAdc + 128;

                iadc = nbigRxy0[n];
                ich = nbigshift[n] + ic;
                detadc[iadc][ich] = nbigRlocal;
                ichadc[iadc][ich] = ic + maxAdc + 128;
            }
        }
        for (Int_t n = 0; n < ncsc; n++) {
            Int_t ndetcsc = nmiddle + 2 * nbig + n;

            nx0det[ndetcsc] = ncscver;
            nx1det[ndetcsc] = ncscver;
            ny0det[ndetcsc] = ncscin;
            ny1det[ndetcsc] = ncscout;
            nchdet[ndetcsc] = nallcsc;

            Int_t nadccsc = nadcgem + n;
            for (Int_t ic = 0; ic < nallcsc; ic++) {
                detadc[nadccsc][ic] = ndetcsc;
                ichadc[nadccsc][ic] = ic;
            }
        }
    }

    pedname = TString(getenv("VMCWORKDIR")) + TString("/input/") + "RPed_";
    pedname += run;
    pedname += ".dat";
    if (pedestals) {
        cout << " Read pedestal file " << pedname << endl;
        cout << endl;
        Pedfile = fopen(pedname, "r");
        if (!Pedfile)
            perror("Pedestal file %s open error");
        else {

            Int_t det, chan;
            Float_t ped, rms;

            while (!feof(Pedfile)) {
                fgets(sped, 10, Pedfile);
                sscanf(sped, "%d %d", &det, &chan);
                if (det >= 0 && det < ndet && chan == nchdet[det]) {
                    //       cout << " det= " << det << " nchan= " << chan << endl;
                    //       cout << endl;

                    for (Int_t ic = 0; ic < chan; ic++) {
                        fgets(sped, 20, Pedfile);
                        sscanf(sped, "%g %g", &ped, &rms);
                        //        cout << " ic= " << ic << " ped= " << ped << " rms= " << rms << endl;
                        Pedchr[det][ic] = ped;
                        Pedchr2[det][ic] = rms;
                    }
                }
            }
        }
    }

    rnoisename = TString(getenv("VMCWORKDIR")) + TString("/input/") + "RNoise_";
    rnoisename += run;
    if (ithr == 0 && test != 1) rnoisename += "thr15.dat";
    else if (ithr == 1 && test != 1) rnoisename += "thr18.dat";
    else if (ithr == 0 && test == 1) rnoisename += "thr15test1.dat";
    else if (ithr == 1 && test == 1) rnoisename += "thr18test1.dat";
    if (read) {
        cout << " Read noise file " << rnoisename << endl;
        cout << endl;
        Rnoisefile = fopen(rnoisename, "r");
        if (!Rnoisefile)
            perror("Noise file %s open error");
        else {

            Int_t noise = 0;
            while (!feof(Rnoisefile)) {
                fgets(ss, 10, Rnoisefile);
                if (sscanf(ss, "%d", &noise) > 0) {
                    Int_t det = (Int_t) noise / 10000;
                    Int_t chan = noise - det * 10000;
                    //                cout << "read GEM noise " << noise << " det " << det << " chan " << chan << endl;
                    if (chan >= 0 && chan < maxChan) noisech[det][chan] = 1;
                } else
                    fprintf(stderr, "!Noise file read error!\n");
            }
        }
    }
    wnoisename = TString(getenv("VMCWORKDIR")) + TString("/input/") + "WNoise_";
    wnoisename += run;
    wnoisename += ".dat";
    //    wnoisename = rnoisename;

    wpedname = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Ped_";
    wpedname += run;
    wpedname += ".dat";
    //    wpedname = pedname;
    if (!read) {
        printf("Create noise file %s\n", wnoisename.Data());
        Wnoisefile = fopen(wnoisename, "w");
        Wpedfile = fopen(wpedname, "w");
        if (!Wnoisefile && !Wpedfile)
            Fatal("InitAdcProcessorMK", "Could not create noise/pedestal file!");
    }
    if (GetPeriod() == 6) {
        // Small GEM
        TString x0small = "X_small.txt";
        ifstream fromxsmall(x0small);
        for (Int_t ibin = 0; ibin < nsmall; ibin++) {
            fromxsmall >> xsmall[ibin];
            Int_t ch = xsmall[ibin] - 1024;
            if (ch >= 0 && ch < nallsma) chsma[ch] = ibin;
        }

        TString y0small = "Y_small.txt";
        ifstream fromysmall(y0small);
        for (Int_t ibin = 0; ibin < nsmall; ibin++) {
            fromysmall >> ysmall[ibin];
            Int_t ch = ysmall[ibin] - 1024;
            if (ch >= 0 && ch < nallsma) chsma[ch] = ibin + 1000;
        }
    }

    // Middle GEM

    TString x0file = TString(getenv("VMCWORKDIR")) + TString("/input/") + "X0_pr.txt";
    ifstream fromx0(x0file);
    for (Int_t ibin = 0; ibin < nx0bin; ibin++) {
        fromx0 >> x0map[ibin];
        Int_t ch = x0map[ibin];
        if (ch < nallmid) chmap[ch] = ibin;
    }

    TString y0file = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Y0_pr.txt";
    ifstream fromy0(y0file);
    for (Int_t ibin = 0; ibin < ny0bin; ibin++) {
        fromy0 >> y0map[ibin];
        Int_t ch = y0map[ibin];
        if (ch < nallmid) chmap[ch] = ibin + 1000;
    }

    TString x1file = TString(getenv("VMCWORKDIR")) + TString("/input/") + "X1_pr.txt";
    ifstream fromx1(x1file);
    for (Int_t ibin = 0; ibin < nx1bin; ibin++) {
        fromx1 >> x1map[ibin];
        Int_t ch = x1map[ibin];
        if (ch < nallmid) chmap[ch] = ibin + 2000;
    }

    TString y1file = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Y1_pr.txt";
    ifstream fromy1(y1file);
    for (Int_t ibin = 0; ibin < ny1bin; ibin++) {
        fromy1 >> y1map[ibin];
        Int_t ch = y1map[ibin];
        if (ch < nallmid) chmap[ch] = ibin + 4000;
    }
    // Big GEM 1 Left

    TString xc0bigL = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Full_X0_Left.txt";
    ifstream fromx0bigL(xc0bigL);
    for (Int_t ibin = 0; ibin < nx0big; ibin++) {
        fromx0bigL >> x0big[ibin];
        Int_t ch = x0big[ibin] + maxAdc + 128;
        if (ch < nallbig) chbigL[ch] = ibin;
    }

    TString yc0bigL = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Full_Y0_Left.txt";
    ifstream fromy0bigL(yc0bigL);
    for (Int_t ibin = 0; ibin < ny0bigL; ibin++) {
        fromy0bigL >> y0bigL[ibin];
        Int_t ch = y0bigL[ibin] + maxAdc + 128;
        if (ch < nallbig) chbigL[ch] = ibin + 1000;
    }

    TString xc1bigL = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Full_X_Left.txt";
    ifstream fromx1bigL(xc1bigL);
    for (Int_t ibin = 0; ibin < nx1big; ibin++) {
        fromx1bigL >> x1big[ibin];
        Int_t ch = x1big[ibin];
        if (ch < nallbig) chbigL[ch] = ibin + 2000;
    }

    TString yc1bigL = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Full_Y_Left.txt";
    ifstream fromy1bigL(yc1bigL);
    for (Int_t ibin = 0; ibin < ny1bigL; ibin++) {
        fromy1bigL >> y1bigL[ibin];
        Int_t ch = y1bigL[ibin];
        if (ch < nallbig) chbigL[ch] = ibin + 4000;
    }
    // Big GEM 1 Right

    TString xc0bigR = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Full_X0_Right.txt";
    ifstream fromx0bigR(xc0bigR);
    for (Int_t ibin = 0; ibin < nx0big; ibin++) {
        fromx0bigR >> x0big[ibin];
        Int_t ch = x0big[ibin] + maxAdc + 128;
        if (ch < nallbig) chbigR[ch] = ibin;
    }

    TString yc0bigR = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Full_Y0_Right.txt";
    ifstream fromy0bigR(yc0bigR);
    for (Int_t ibin = 0; ibin < ny0bigR; ibin++) {
        fromy0bigR >> y0bigR[ibin];
        Int_t ch = y0bigR[ibin] + maxAdc + 128;
        if (ch < nallbig) chbigR[ch] = ibin + 1000;
    }

    TString xc1bigR = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Full_X_Right.txt";
    ifstream fromx1bigR(xc1bigR);
    for (Int_t ibin = 0; ibin < nx1big; ibin++) {
        fromx1bigR >> x1big[ibin];
        Int_t ch = x1big[ibin];
        if (ch < nallbig) chbigR[ch] = ibin + 2000;
    }

    TString yc1bigR = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Full_Y_Right.txt";
    ifstream fromy1bigR(yc1bigR);
    for (Int_t ibin = 0; ibin < ny1bigR; ibin++) {
        fromy1bigR >> y1bigR[ibin];
        Int_t ch = y1bigR[ibin];
        if (ch < nallbig) chbigR[ch] = ibin + 4000;
    }

    // Big GEM 2 Left

    TString xc0bigL2 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "GEM2_X0_Left.txt";
    ifstream fromx0bigL2(xc0bigL2);
    for (Int_t ibin = 0; ibin < nx0big; ibin++) {
        fromx0bigL2 >> x0big2[ibin];
        Int_t ch = x0big2[ibin] + maxAdc + 128 - 1024;
        if (ch < nallbig) chbigL2[ch] = ibin;
    }

    TString yc0bigL2 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "GEM2_Y0_Left.txt";
    ifstream fromy0bigL2(yc0bigL2);
    for (Int_t ibin = 0; ibin < ny0bigL; ibin++) {
        fromy0bigL2 >> y0bigL2[ibin];
        Int_t ch = y0bigL2[ibin] + maxAdc + 128 - 1024;
        if (ch < nallbig) chbigL2[ch] = ibin + 1000;
    }

    TString xc1bigL2 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "GEM2_X_Left.txt";
    ifstream fromx1bigL2(xc1bigL2);
    for (Int_t ibin = 0; ibin < nx1big; ibin++) {
        fromx1bigL2 >> x1big2[ibin];
        Int_t ch = x1big2[ibin];
        if (ch < nallbig) chbigL2[ch] = ibin + 2000;
    }

    TString yc1bigL2 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "GEM2_Y_Left.txt";
    ifstream fromy1bigL2(yc1bigL2);
    for (Int_t ibin = 0; ibin < ny1bigL; ibin++) {
        fromy1bigL2 >> y1bigL2[ibin];
        Int_t ch = y1bigL2[ibin];
        if (ch < nallbig) chbigL2[ch] = ibin + 4000;
    }

    // Big GEM 2 Right

    TString xc0bigR2 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "GEM2_X0_Right.txt";
    ifstream fromx0bigR2(xc0bigR2);
    for (Int_t ibin = 0; ibin < nx0big; ibin++) {
        fromx0bigR2 >> x0big2[ibin];
        Int_t ch = x0big2[ibin] + maxAdc + 128 - 1024;
        if (ch < nallbig) chbigR2[ch] = ibin;
    }

    TString yc0bigR2 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "GEM2_Y0_Right.txt";
    ifstream fromy0bigR2(yc0bigR2);
    for (Int_t ibin = 0; ibin < ny0bigR; ibin++) {
        fromy0bigR2 >> y0bigR2[ibin];
        Int_t ch = y0bigR2[ibin] + maxAdc + 128 - 1024;
        if (ch < nallbig) chbigR2[ch] = ibin + 1000;
    }

    TString xc1bigR2 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "GEM2_X_Right.txt";
    ifstream fromx1bigR2(xc1bigR2);
    for (Int_t ibin = 0; ibin < nx1big; ibin++) {
        fromx1bigR2 >> x1big2[ibin];
        Int_t ch = x1big2[ibin];
        if (ch < nallbig) chbigR2[ch] = ibin + 2000;
    }

    TString yc1bigR2 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "GEM2_Y_Right.txt";
    ifstream fromy1bigR2(yc1bigR2);
    for (Int_t ibin = 0; ibin < ny1bigR; ibin++) {
        fromy1bigR2 >> y1bigR2[ibin];
        Int_t ch = y1bigR2[ibin];
        if (ch < nallbig) chbigR2[ch] = ibin + 4000;
    }

    /*
        for ( Int_t ibin = 0; ibin < nallbig; ibin++ ) {
          Int_t ch = chbigR2[ibin];
          if (ch == -1) cout << " chbigR2=-1 " << " ibin= " << ibin << endl;
        }
     */

    // CSC Vertical

    TString cscvert1 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Vertic_1.txt";
    ifstream fromcscv1(cscvert1);
    for (Int_t ibin = 0; ibin < ncscver; ibin++) {
        fromcscv1 >> x1csc[ibin];
        Int_t ch = x1csc[ibin];
        if (ch >= 0 && ch < nallcsc) chuppercsc[ch] = ibin + 2000;
    }

    TString cscvert2 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Vertic_2.txt";
    ifstream fromcscv2(cscvert2);
    for (Int_t ibin = 0; ibin < ncscver; ibin++) {
        fromcscv2 >> x2csc[ibin];
        Int_t ch = x2csc[ibin];
        if (ch >= 0 && ch < nallcsc) chuppercsc[ch] = ibin;
    }

    TString cscvert3 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Vertic_3.txt";
    ifstream fromcscv3(cscvert3);
    for (Int_t ibin = 0; ibin < ncscver; ibin++) {
        fromcscv3 >> x3csc[ibin];
        Int_t ch = x3csc[ibin];
        if (ch >= 0 && ch < nallcsc) chlowercsc[ch] = ibin;
    }

    TString cscvert4 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Vertic_4.txt";
    ifstream fromcscv4(cscvert4);
    for (Int_t ibin = 0; ibin < ncscver; ibin++) {
        fromcscv4 >> x4csc[ibin];
        Int_t ch = x4csc[ibin];
        if (ch >= 0 && ch < nallcsc) chlowercsc[ch] = ibin + 2000;
    }

    // CSC Kosoy

    TString csckos1 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Kosoy_1.txt";
    ifstream fromcsck1(csckos1);
    for (Int_t ibin = 0; ibin < ncscout; ibin++) {
        fromcsck1 >> y1csc[ibin];
        Int_t ch = y1csc[ibin];
        if (ch >= 0 && ch < nallcsc) chuppercsc[ch] = ibin + 4000;
    }

    TString csckos2 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Kosoy_2.txt";
    ifstream fromcsck2(csckos2);
    for (Int_t ibin = 0; ibin < ncscin; ibin++) {
        fromcsck2 >> y2csc[ibin];
        Int_t ch = y2csc[ibin];
        if (ch >= 0 && ch < nallcsc) chuppercsc[ch] = ibin + 1000;
    }

    TString csckos3 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Kosoy_3.txt";
    ifstream fromcsck3(csckos3);
    for (Int_t ibin = 0; ibin < ncscin; ibin++) {
        fromcsck3 >> y3csc[ibin];
        Int_t ch = y3csc[ibin];
        if (ch >= 0 && ch < nallcsc) chlowercsc[ch] = ibin + 1000;
    }

    TString csckos4 = TString(getenv("VMCWORKDIR")) + TString("/input/") + "Kosoy_4.txt";
    ifstream fromcsck4(csckos4);
    for (Int_t ibin = 0; ibin < ncscout; ibin++) {
        fromcsck4 >> y4csc[ibin];
        Int_t ch = y4csc[ibin];
        if (ch >= 0 && ch < nallcsc) chlowercsc[ch] = ibin + 4000;
    }
    for (Int_t det = 0; det < ndet; det++) {
        TString tmp = "Nhits_Gem_";
        tmp += det;
        Int_t mChan = nchdet[det];
        hNhits[det] = new TH1I(tmp, tmp, mChan, 0, mChan);
        hNhits[det]->SetDirectory(0);
    }

    for (Int_t coor = 0; coor < ncoor; coor++) {

        for (Int_t ich = 0; ich < maxChan; ich++) {
            sigx1[coor][ich] = 0;
            nsigx1[coor][ich] = 0;
        }
    }

    Int_t badmin = 1536;
    Int_t badmax = 1537;

    for (Int_t det = 0; det < ndet; det++) {
        //        if (run > 1) {
        //            for (Int_t ibad = badmin; ibad < badmax; ibad++) {
        //                //     noisech[det][ibad]= 1;
        //            }
        //        }
        for (Int_t ich = 0; ich < maxchip; ich++) {
            Cmode[det][ich] = 0;
            C1mode[det][ich] = 0;
            Cmode2[det][ich] = 0;
            for (Int_t ii = 0; ii < nevmax; ++ii) {
                //      cmdx1[ii][det][ich] = 0;
            }
        }
    }
    //    for (Int_t det = 0; det < ndet; det++) {
    //        hPeds[det]->Sumw2();
    //        hPrms[det]->Sumw2();
    //        hPmCrms[det]->Sumw2();
    //        hPmCmod[det]->Sumw2();
    //        hCmode[det]->Sumw2();
    //        hCrms[det]->Sumw2();
    //    }
}

BmnStatus BmnGemRaw2Digit::RecalculatePedestalsMK(Int_t nPedEv) {
    npevents = nPedEv;
    // pedestals: first iteration
    for (Int_t det = 0; det < ndet; det++) {
        for (Int_t ich = 0; ich < maxchip; ich++) {
            Cmode[det][ich] = 0;
            C1mode[det][ich] = 0;
            Cmode2[det][ich] = 0;
            for (Int_t ii = 0; ii < nevmax; ++ii) {
                //      cmdx1[ii][det][ich] = 0;
            }
        }
    }
    for (Int_t det = 0; det < ndet; det++) {
        for (Int_t ich = 0; ich < maxchip; ich++) {
            Smode[det][ich] = 0;
            nchan[det][ich] = 0;
        }
    }

    for (Int_t iEv = 0; iEv < nPedEv; iEv++) {

        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                //                for (Int_t iAdc = 0; iAdc < adc128->GetEntriesFast(); ++iAdc) {
                //                    BmnADCDigit* adcDig = (BmnADCDigit*) adc128->At(iAdc);

                UInt_t chan = iCh; //adcDig->GetChannel();
                UInt_t ser = fAdcSerials[iCr]; //adcDig->GetSerial();
                Int_t nsmpl = fNSamples; //adcDig->GetNSamples();

                Int_t iadc = -1;
                Int_t iradc = -1;

                for (Int_t jadc = 0; jadc < nadc; ++jadc) {
                    if (ser == fSerials[jadc]) {
                        iadc = jadc;
                        break;
                    }
                }

                if (iadc == -1 || nsmpl != nadc_samples) {
                    //                    cout << " iAdc= " << iadc << " chan= " << chan << " Wrong serial= " << std::hex << ser << std::dec << " Or nsmpl= " << nsmpl << endl;
                } else {

                    for (Int_t ichan = 0; ichan < nadc_samples; ++ichan) {
                        Int_t ic = chan * nadc_samples + ichan;
                        //MK           unsigned short Adc = (adcDig->GetUShortValue())[ichan] / 16;;
                        Double_t Adc = fPedDat[iCr][iEv][iCh][ichan]; //(Double_t) (adcDig->GetShortValue())[ichan] / 16;
                        // adc / channel -> detector / channel
                        Int_t det = detadc[iadc][ic];
                        Int_t ich = ichadc[iadc][ic];
                        if (ich >= 0 && det >= 0 && det < ndet) {
                            Int_t mChan = nchdet[det];

                            if (ich < mChan && noisech[det][ich] == 0) {
                                // pedestals
                                Ped1ch[det][ich] += Adc;
                                nchan1[det][ich]++;
                            }
                        }
                    }
                }
            }
    }
    // initial pedestals for pedestal events
    for (Int_t det = 0; det < ndet; det++) {
        Int_t mChan = nchdet[det];

        for (Int_t ich = 0; ich < mChan; ich++) {
            Int_t nch = nchan1[det][ich];
            if (noisech[det][ich] == 0 && nch > 0) {
                Double_t Amp = Ped1ch[det][ich] / nch;
                Ped1ch[det][ich] = Amp;
                //                printf("Ped1ch[%i][%i] = %f nch = %i\n", det, ich, Ped1ch[det][ich], nch);
                Int_t ichip = (Int_t) ich / nchip;
                Smode[det][ichip] += Amp;
                nchan[det][ichip]++;
            }
        }
    }

    // common mode per chip for pedestal events

    for (Int_t det = 0; det < ndet; det++) {
        Int_t mChan = nchdet[det];
        Int_t mchip = (Int_t) mChan / nchip;

        for (Int_t ich = 0; ich < mchip; ich++) {
            Int_t nch = nchan[det][ich];
            if (nch > 0) {
                Double_t Amp = Smode[det][ich] / nch;
                Smode[det][ich] = Amp;
                Cmall[det][ich] = Amp;
            }
        }
    }

    // niterped iterations over pedestal events

    for (Int_t iter = 1; iter < niterped; iter++) {

        for (Int_t det = 0; det < ndet; det++) {
            for (Int_t ich = 0; ich < maxchip; ich++) {
                Cmode[det][ich] = Smode[det][ich];
            }
            for (Int_t ich = 0; ich < maxChan; ich++) {
                Pedch[det][ich] = Ped1ch[det][ich];
                Pedcmod2[det][ich] = Ped1cmod2[det][ich];
                nchan1[det][ich] = 0;
                Ped1ch[det][ich] = 0;
                Ped1ch2[det][ich] = 0;

                Ped1cmod[det][ich] = 0;
                Ped1cmod2[det][ich] = 0;
            }
        }


        for (Int_t iEv = 0; iEv < nPedEv; iEv++) {

            //            BmnEventHeader* evtype = (BmnEventHeader*) evhead;
            /*
                 BmnEventType trtype;
                 for (Int_t ihead = 0; ihead < evhead->GetEntriesFast(); ++ihead) {
                  BmnEventHeader* evtype = (BmnEventHeader*) evhead->At(ihead);
                  trtype = evtype->GetType();
                  if (trtype == 0) break;
                 }

                   if (iEv < 100)     
                   cout << " Event Type " << evtype->GetType() << endl;
             */

            for (Int_t det = 0; det < ndet; det++) {
                for (Int_t ich = 0; ich < maxchip; ich++) {
                    Smode1[det][ich] = 0;
                    Cmode1[det][ich] = 0;
                    nchan[det][ich] = 0;
                }
            }

            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                    //                    for (Int_t iAdc = 0; iAdc < adc128->GetEntriesFast(); ++iAdc) {
                    //                        BmnADCDigit* adcDig = (BmnADCDigit*) adc128->At(iAdc);

                    UInt_t chan = iCh; //adcDig->GetChannel();
                    UInt_t ser = fAdcSerials[iCr]; //adcDig->GetSerial();
                    Int_t nsmpl = fNSamples; //adcDig->GetNSamples();

                    Int_t iadc = -1;
                    Int_t iradc = -1;

                    // check adc serial numbers
                    for (Int_t jadc = 0; jadc < nadc; ++jadc) {
                        if (ser == fSerials[jadc]) {
                            iadc = jadc;
                            break;
                        }
                    }


                    if (iadc == -1 || nsmpl != nadc_samples) {
                        //                        cout << " iAdc= " << iadc << " chan= " << chan << " Wrong serial= " << std::hex << ser << std::dec << " Or nsmpl= " << nsmpl << endl;
                    } else {

                        // adc / channel -> detector / channel

                        for (Int_t ichan = 0; ichan < nadc_samples; ++ichan) {
                            Int_t ic = chan * nadc_samples + ichan;
                            Int_t det = detadc[iadc][ic];
                            Int_t ich = ichadc[iadc][ic];

                            if (ich >= 0 && det >= 0 && det < ndet) {
                                Int_t mChan = nchdet[det];

                                if (ich < mChan && noisech[det][ich] == 0) {
                                    Int_t ichip = (Int_t) ich / nchip;

                                    //MK               Asample[det][ich] = (adcDig->GetUShortValue())[ichan] / 16;
                                    Asample[det][ich] = fPedDat[iCr][iEv][iCh][ichan]; //(Double_t) (adcDig->GetShortValue())[ichan] / 16;
                                    /*
                                                  if (ich == 0 && det == 0) {
                                                    cout << " iEv= " << iEv << " ic= " << ic << " iadc= " << iadc << " Asample= " << (adcDig->GetShortValue())[ichan] / 16 << endl; 
                                                  }
                                     */
                                    Double_t Adc = Asample[det][ich];
                                    Double_t Ped = Pedch[det][ich];
                                    Double_t Sig = Adc - Ped;
                                    Double_t Asig = TMath::Abs(Sig);
                                    Double_t thr = thrped - iter*dthr;
                                    if (GetRun() > 100 && GetRun() < 790 && det == 1 && thr < 20) thr = 20;
                                    if (GetRun() > 1331 && GetRun() < 1600 && (det == 0 || det == 3) && thr < 30) thr = 30;

                                    //MK          Double_t thrmin1 = 3.5*Pedcmod2[det][ich];
                                    //MK          if (thr < thrmin1 && thrmin1 > 0 && iter > 1) thr = thrmin1;  

                                    // common mode only for channels without signal (below threshold)
                                    if (Asig < thr) {
                                        Smode1[det][ichip] += Adc;
                                        Cmode1[det][ichip] += Ped;
                                        nchan[det][ichip]++;
                                    }
                                }
                            }
                        }
                    }
                }

            // cmode at iteration iter for channels without signal

            for (Int_t det = 0; det < ndet; det++) {
                Int_t mChan = nchdet[det];
                Int_t mchip = (Int_t) mChan / nchip;

                for (Int_t ich = 0; ich < mchip; ich++) {
                    Int_t nch = nchan[det][ich];
                    if (nch > 0) {
                        Double_t Amp = Smode1[det][ich] / nch;
                        Smode1[det][ich] = Amp;
                        Amp = Cmode1[det][ich] / nch;
                        Cmode1[det][ich] = Amp;
                    }
                }
            }

            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
                    //                    for (Int_t iAdc = 0; iAdc < adc128->GetEntriesFast(); ++iAdc) {
                    //                        BmnADCDigit* adcDig = (BmnADCDigit*) adc128->At(iAdc);

                    UInt_t chan = iCh; //adcDig->GetChannel();
                    UInt_t ser = fAdcSerials[iCr]; //adcDig->GetSerial();
                    Int_t nsmpl = fNSamples; //adcDig->GetNSamples();


                    Int_t iadc = -1;
                    Int_t iradc = -1;

                    // check ADC seial number

                    for (Int_t jadc = 0; jadc < nadc; ++jadc) {
                        if (ser == fSerials[jadc]) {
                            iadc = jadc;
                            break;
                        }
                    }


                    if (iadc == -1 || nsmpl != nadc_samples) {
                        //                        cout << " iAdc= " << iAdc << " chan= " << chan << " Wrong serial= " << std::hex << ser << std::dec << " Or nsmpl= " << nsmpl << endl;
                    } else {

                        //                        hAdc->Fill(iadc);

                        for (Int_t ichan = 0; ichan < nadc_samples; ++ichan) {
                            Int_t ic = chan * nadc_samples + ichan;
                            // adc/channel -> det/channel
                            Int_t det = detadc[iadc][ic];
                            Int_t ich = ichadc[iadc][ic];

                            if (ich >= 0 && det >= 0 && det < ndet) {
                                Int_t mChan = nchdet[det];

                                if (ich < mChan && noisech[det][ich] == 0) {
                                    Int_t ichip = (Int_t) ich / nchip;

                                    //MK           Asample[det][ich] = (adcDig->GetUShortValue())[ichan] / 16;
                                    Asample[det][ich] = fPedDat[iCr][iEv][iCh][ichan]; //(Double_t) (adcDig->GetShortValue())[ichan] / 16;
                                    /*
                                                  if (ich == 0 && det == 0) {
                                                    cout << " iEv= " << iEv << " ic= " << ic << " iadc= " << iadc << " Asample= " << (adcDig->GetShortValue())[ichan] / 16 << endl;
                                                  }
                                     */
                                    Double_t smode = Smode1[det][ichip];
                                    Double_t cmode = Cmode1[det][ichip];

                                    // signal over pedestal and common mode
                                    Double_t Sig = Asample[det][ich] - Pedch[det][ich] + cmode - smode;
                                    Double_t Asig = TMath::Abs(Sig);
                                    Double_t thr = thrped - iter*dthr;
                                    if (GetRun() > 100 && GetRun() < 790 && det == 1 && thr < 20) thr = 20;
                                    if (GetRun() > 1331 && GetRun() < 1600 && (det == 0 || det == 3) && thr < 30) thr = 30;

                                    //MK              Double_t thrmin1 = 3.5*Pedcmod2[det][ich];
                                    //MK              if (thr < thrmin1 && thrmin1 > 0 && iter > 1) thr = thrmin1;  

                                    // pedestals for channels without signal
                                    if (Asig < thr) {
                                        Double_t Adc = Asample[det][ich];
                                        Ped1ch[det][ich] += Adc;
                                        Ped1ch2[det][ich] += Adc*Adc;
                                        nchan1[det][ich]++;

                                        Adc = Asample[det][ich] - smode;
                                        Ped1cmod[det][ich] += Adc;
                                        Ped1cmod2[det][ich] += Adc*Adc;

                                        if (iter == (niterped - 1)) {
                                            //                 Pedadc[iadc][ic] += Adc;
                                            //                 Pedadc2[iadc][ic] += Adc*(Adc);
                                            //                 nchadc[iadc][ic]++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
        } // ped event loop

        // pedestals and rms after all iterations

        for (Int_t det = 0; det < ndet; det++) {
            Int_t mChan = nchdet[det];

            for (Int_t ich = 0; ich < mChan; ich++) {
                Int_t nch = nchan1[det][ich];
                if (noisech[det][ich] == 0 && nch > 0) {
                    Double_t Amp = Ped1ch[det][ich] / nch;
                    Ped1ch[det][ich] = Amp;

                    Amp = Ped1cmod[det][ich] / nch;
                    Ped1cmod[det][ich] = Amp;

                    Double_t prms2 = Ped1ch2[det][ich] / nch - Ped1ch[det][ich] * Ped1ch[det][ich];
                    Double_t prms = 0;
                    if (prms2 > 0) prms = TMath::Sqrt(prms2);
                    Ped1ch2[det][ich] = prms;

                    prms2 = Ped1cmod2[det][ich] / nch - Ped1cmod[det][ich] * Ped1cmod[det][ich];
                    prms = 0;
                    if (prms2 > 0) prms = TMath::Sqrt(prms2);
                    Ped1cmod2[det][ich] = prms;
                }
            }
        }

    } // iter loop

    Int_t nrms = 0;
    Double_t sumrms = 0;

    for (Int_t det = 0; det < ndet; det++) {
        Int_t mChan = nchdet[det];

        for (Int_t ich = 0; ich < mChan; ich++) {
            if (noisech[det][ich] == 0) {
                Double_t Amp = Ped1ch[det][ich];
                Pedch[det][ich] = Amp;
                //                hPeds[det]->SetBinContent(ich + 1, (Float_t) Amp);

                Double_t prms = Ped1ch2[det][ich];
                Pedch2[det][ich] = prms;
                //                hPrms[det]->SetBinContent(ich + 1, (Float_t) prms);

                prms = Ped1cmod2[det][ich];
                nrms++;
                sumrms += prms;
            }
        }
    }

    if (nrms > 0) sumrms /= nrms;

    Double_t sumrmscard[maxcard];
    Double_t nrmscard[maxcard];

    Int_t cont1 = 0;
    for (Int_t det = 0; det < ndet; det++) {
        Int_t mChan = nchdet[det];

        for (Int_t icard = 0; icard < maxcard; icard++) {
            sumrmscard[icard] = 0;
            nrmscard[icard] = 0;
        }

        for (Int_t ich = 0; ich < mChan; ich++) {
            Int_t icard = (Int_t) ich / 128;

            if (noisech[det][ich] == 0) {
                Double_t prms = Ped1cmod2[det][ich];
                nrmscard[icard]++;
                sumrmscard[icard] += prms;
            }
        }
        for (Int_t icard = 0; icard < maxcard; icard++) {
            if (nrmscard[icard] > 0) sumrmscard[icard] /= nrmscard[icard];
        }

        for (Int_t ich = 0; ich < mChan; ich++) {
            Int_t icard = (Int_t) ich / 128;
            if (noisech[det][ich] == 0) {
                Double_t Amp = Ped1cmod[det][ich];
                Pedcmod[det][ich] = Amp;
                //                hPmCmod[det]->SetBinContent(ich + 1, (Float_t) Amp);

                Double_t prms = Ped1cmod2[det][ich];
                Pedcmod2[det][ich] = prms;
                //                hPmCrms[det]->SetBinContent(ich + 1, (Float_t) prms);

                if (prms > 5 * sumrmscard[icard]) { // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!was 4
                    //                    cout << " NEW PRMS noise det= " << det << " channel= " << ich << endl;
                    noisech[det][ich] = 1;
                    if (!read) {
                        cont1 = ich + det * 10000;
                        fprintf(Wnoisefile, " %d\n", cont1);
                    }
                }

            }
        }
    }

    for (Int_t det = 0; det < ndet; det++) {
        Int_t mChan = nchdet[det];
        Int_t mchip = (Int_t) mChan / nchip;

        for (Int_t ich = 0; ich < mchip; ich++) {
            if (nchan[det][ich] > 0) {
                Double_t Amp = Cmode[det][ich];
                //                hCmode[det]->SetBinContent(ich + 1, (Float_t) Amp);
                Double_t crms = Cmode2[det][ich];
                //                hCrms[det]->SetBinContent(ich + 1, (Float_t) crms);
            }
        }
    }

    if (!pedestals && !read) {
        for (Int_t det = 0; det < ndet; det++) {
            Int_t mChan = nchdet[det];
            fprintf(Wpedfile, " %d %d\n", det, mChan);

            for (Int_t ich = 0; ich < mChan; ich++) {
                Float_t ped = Pedch[det][ich];
                Float_t rms = Pedcmod2[det][ich];
                fprintf(Wpedfile, " %g %g\n", ped, rms);
            }
        }
    }
    nev = -1;

    return kBMNSUCCESS;
}

BmnStatus BmnGemRaw2Digit::LoadPedestalsMK(TTree* t_in, TClonesArray* adc32, BmnEventHeader* evhead, Int_t npedev) {
    //KV pedestal events

    // pedestals: first iteration

    for (Int_t det = 0; det < ndet; det++) {
        for (Int_t ich = 0; ich < maxchip; ich++) {
            Smode[det][ich] = 0;
            nchan[det][ich] = 0;
        }
    }

    // first loop over pedestal events

    for (Int_t iEv = 0; iEv < npedev; iEv++) {
        t_in->GetEntry(iEv);

        BmnEventHeader* evtype = (BmnEventHeader*) evhead;
        // selection of pedestal events

        if (evtype->GetEventType() == 0) {
            npevents++;

            for (Int_t iAdc = 0; iAdc < adc32->GetEntriesFast(); ++iAdc) {
                BmnADCDigit* adcDig = (BmnADCDigit*) adc32->At(iAdc);

                UInt_t chan = adcDig->GetChannel();
                UInt_t ser = adcDig->GetSerial();
                Int_t nsmpl = adcDig->GetNSamples();
                Int_t iadc = -1;
                Int_t iradc = -1;

                for (Int_t jadc = 0; jadc < nadc; ++jadc) {
                    if (ser == fSerials[jadc]) {
                        iadc = jadc;
                        break;
                    }
                }

                if (iadc == -1 || nsmpl != nadc_samples) {
                    //                    cout << " iAdc= " << iAdc << " chan= " << chan << " Wrong serial= " << std::hex << ser << std::dec << " Or nsmpl= " << nsmpl << endl;
                } else {

                    for (Int_t ichan = 0; ichan < nadc_samples; ++ichan) {
                        Int_t ic = chan * nadc_samples + ichan;

                        Double_t Adc = (GetRun() > GetBoundaryRun(ADC32_N_SAMPLES)) ?
                                ((Double_t) (adcDig->GetShortValue())[ichan] / 16) :
                                ((Double_t) (adcDig->GetUShortValue())[ichan] / 16);

                        // adc / channel -> detector / channel
                        Int_t det = detadc[iadc][ic];
                        Int_t ich = ichadc[iadc][ic];
                        if (ich >= 0 && det >= 0 && det < ndet) {
                            Int_t mChan = nchdet[det];

                            if (ich < mChan && noisech[det][ich] == 0) {
                                // pedestals
                                Ped1ch[det][ich] += Adc;
                                nchan1[det][ich]++;
                            }
                        }
                    }
                }
            } // iAdc
        } // evtype = 1
    } // npedev

    // initial pedestals for pedestal events
    for (Int_t det = 0; det < ndet; det++) {
        Int_t mChan = nchdet[det];

        for (Int_t ich = 0; ich < mChan; ich++) {
            Int_t nch = nchan1[det][ich];
            if (noisech[det][ich] == 0 && nch > 0) {
                Double_t Amp = Ped1ch[det][ich] / nch;
                Ped1ch[det][ich] = Amp;

                Int_t ichip = (Int_t) ich / nchip;
                Smode[det][ichip] += Amp;
                nchan[det][ichip]++;
            }
        }
    }

    // common mode per chip for pedestal events

    for (Int_t det = 0; det < ndet; det++) {
        Int_t mChan = nchdet[det];
        Int_t mchip = (Int_t) mChan / nchip;

        for (Int_t ich = 0; ich < mchip; ich++) {
            Int_t nch = nchan[det][ich];
            if (nch > 0) {
                Double_t Amp = Smode[det][ich] / nch;
                Smode[det][ich] = Amp;
                Cmall[det][ich] = Amp;
            }
        }
    }

    // niterped iterations over pedestal events

    for (Int_t iter = 1; iter < niterped; iter++) {

        for (Int_t det = 0; det < ndet; det++) {
            for (Int_t ich = 0; ich < maxchip; ich++) {
                Cmode[det][ich] = Smode[det][ich];
            }
            for (Int_t ich = 0; ich < maxChan; ich++) {
                Pedch[det][ich] = Ped1ch[det][ich];
                Pedcmod2[det][ich] = Ped1cmod2[det][ich];
                nchan1[det][ich] = 0;
                Ped1ch[det][ich] = 0;
                Ped1ch2[det][ich] = 0;

                Ped1cmod[det][ich] = 0;
                Ped1cmod2[det][ich] = 0;
            }
        }


        for (Int_t iEv = 0; iEv < npedev; iEv++) {
            t_in->GetEntry(iEv);

            BmnEventHeader* evtype = (BmnEventHeader*) evhead;
            /*
                 BmnEventType trtype;
                 for (Int_t ihead = 0; ihead < evhead->GetEntriesFast(); ++ihead) {
                  BmnEventHeader* evtype = (BmnEventHeader*) evhead->At(ihead);
                  trtype = evtype->GetType();
                  if (trtype == 0) break;
                 }

                   if (iEv < 100)     
                   cout << " Event Type " << evtype->GetType() << endl;
             */
            if (evtype->GetEventType() == 0) {

                for (Int_t det = 0; det < ndet; det++) {
                    for (Int_t ich = 0; ich < maxchip; ich++) {
                        Smode1[det][ich] = 0;
                        Cmode1[det][ich] = 0;
                        nchan[det][ich] = 0;
                    }
                }

                for (Int_t iAdc = 0; iAdc < adc32->GetEntriesFast(); ++iAdc) {
                    BmnADCDigit* adcDig = (BmnADCDigit*) adc32->At(iAdc);

                    UInt_t chan = adcDig->GetChannel();
                    UInt_t ser = adcDig->GetSerial();
                    Int_t nsmpl = adcDig->GetNSamples();
                    /*
                                  if (iEv <= 1) {
                                    cout << " Pedestals, iter= " << iter << endl; 
                                    cout << " iAdc= " << iAdc << endl; 
                                    cout << " GetChannel= " << chan << endl; 
                                    cout << " GetSerial= " << ser << endl;
                                    cout << endl; 
                                  }
                     */

                    Int_t iadc = -1;
                    Int_t iradc = -1;

                    // check adc serial numbers
                    for (Int_t jadc = 0; jadc < nadc; ++jadc) {
                        if (ser == fSerials[jadc]) {
                            iadc = jadc;
                            break;
                        }
                    }


                    if (iadc == -1 || nsmpl != nadc_samples) {
                        //                        cout << " iAdc= " << iAdc << " chan= " << chan << " Wrong serial= " << std::hex << ser << std::dec << " Or nsmpl= " << nsmpl << endl;
                    } else {

                        // adc / channel -> detector / channel

                        for (Int_t ichan = 0; ichan < nadc_samples; ++ichan) {
                            Int_t ic = chan * nadc_samples + ichan;
                            Int_t det = detadc[iadc][ic];
                            Int_t ich = ichadc[iadc][ic];

                            if (ich >= 0 && det >= 0 && det < ndet) {
                                Int_t mChan = nchdet[det];

                                if (ich < mChan && noisech[det][ich] == 0) {
                                    Int_t ichip = (Int_t) ich / nchip;

                                    Asample[det][ich] = (GetRun() > GetBoundaryRun(ADC32_N_SAMPLES)) ?
                                            ((Double_t) (adcDig->GetShortValue())[ichan] / 16) :
                                            ((Double_t) (adcDig->GetUShortValue())[ichan] / 16);

                                    Double_t Adc = Asample[det][ich];
                                    Double_t Ped = Pedch[det][ich];
                                    Double_t Sig = Adc - Ped;
                                    Double_t Asig = TMath::Abs(Sig);
                                    Double_t thr = thrped - iter*dthr;
                                    //                                    if (run > 100 && run < 790 && det == 1 && thr < 20) thr = 20;
                                    //                                    if (run > 1331 && run < 1600 && (det == 0 || det == 3) && thr < 30) thr = 30;

                                    //MK          Double_t thrmin1 = 3.5*Pedcmod2[det][ich];
                                    //MK          if (thr < thrmin1 && thrmin1 > 0 && iter > 1) thr = thrmin1;  

                                    // common mode only for channels without signal (below threshold)
                                    if (Asig < thr) {
                                        Smode1[det][ichip] += Adc;
                                        Cmode1[det][ichip] += Ped;
                                        nchan[det][ichip]++;
                                    }
                                }
                            }
                        }
                    }
                }

                // cmode at iteration iter for channels without signal

                for (Int_t det = 0; det < ndet; det++) {
                    Int_t mChan = nchdet[det];
                    Int_t mchip = (Int_t) mChan / nchip;

                    for (Int_t ich = 0; ich < mchip; ich++) {
                        Int_t nch = nchan[det][ich];
                        if (nch > 0) {
                            Double_t Amp = Smode1[det][ich] / nch;
                            Smode1[det][ich] = Amp;
                            Amp = Cmode1[det][ich] / nch;
                            Cmode1[det][ich] = Amp;
                        }
                    }
                }

                for (Int_t iAdc = 0; iAdc < adc32->GetEntriesFast(); ++iAdc) {
                    BmnADCDigit* adcDig = (BmnADCDigit*) adc32->At(iAdc);

                    UInt_t chan = adcDig->GetChannel();
                    UInt_t ser = adcDig->GetSerial();
                    Int_t nsmpl = adcDig->GetNSamples();
                    Int_t iadc = -1;

                    // check ADC seial number

                    for (Int_t jadc = 0; jadc < nadc; ++jadc) {
                        if (ser == fSerials[jadc]) {
                            iadc = jadc;
                            break;
                        }
                    }


                    if (iadc == -1 || nsmpl != nadc_samples) {
                        //                        cout << " iAdc= " << iAdc << " chan= " << chan << " Wrong serial= " << std::hex << ser << std::dec << " Or nsmpl= " << nsmpl << endl;
                    } else {

                        for (Int_t ichan = 0; ichan < nadc_samples; ++ichan) {
                            Int_t ic = chan * nadc_samples + ichan;
                            // adc/channel -> det/channel
                            Int_t det = detadc[iadc][ic];
                            Int_t ich = ichadc[iadc][ic];

                            if (ich >= 0 && det >= 0 && det < ndet) {
                                Int_t mChan = nchdet[det];

                                if (ich < mChan && noisech[det][ich] == 0) {
                                    Int_t ichip = (Int_t) ich / nchip;

                                    Asample[det][ich] = (GetRun() > GetBoundaryRun(ADC32_N_SAMPLES)) ?
                                            ((Double_t) (adcDig->GetShortValue())[ichan] / 16) :
                                            ((Double_t) (adcDig->GetUShortValue())[ichan] / 16);
                                    Double_t smode = Smode1[det][ichip];
                                    Double_t cmode = Cmode1[det][ichip];

                                    // signal over pedestal and common mode
                                    Double_t Sig = Asample[det][ich] - Pedch[det][ich] + cmode - smode;
                                    Double_t Asig = TMath::Abs(Sig);
                                    Double_t thr = thrped - iter*dthr;
                                    //                                    if (run > 100 && run < 790 && det == 1 && thr < 20) thr = 20;
                                    //                                    if (run > 1331 && run < 1600 && (det == 0 || det == 3) && thr < 30) thr = 30;

                                    //MK              Double_t thrmin1 = 3.5*Pedcmod2[det][ich];
                                    //MK              if (thr < thrmin1 && thrmin1 > 0 && iter > 1) thr = thrmin1;  

                                    // pedestals for channels without signal
                                    if (Asig < thr) {
                                        Double_t Adc = Asample[det][ich];
                                        Ped1ch[det][ich] += Adc;
                                        Ped1ch2[det][ich] += Adc*Adc;
                                        nchan1[det][ich]++;

                                        Adc = Asample[det][ich] - smode;
                                        Ped1cmod[det][ich] += Adc;
                                        Ped1cmod2[det][ich] += Adc*Adc;

                                        if (iter == (niterped - 1)) {
                                            //                 Pedadc[iadc][ic] += Adc;
                                            //                 Pedadc2[iadc][ic] += Adc*(Adc);
                                            //                 nchadc[iadc][ic]++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } // evtype = 0
        } // ped event loop

        // pedestals and rms after all iterations

        for (Int_t det = 0; det < ndet; det++) {
            Int_t mChan = nchdet[det];

            for (Int_t ich = 0; ich < mChan; ich++) {
                Int_t nch = nchan1[det][ich];
                if (noisech[det][ich] == 0 && nch > 0) {
                    Double_t Amp = Ped1ch[det][ich] / nch;
                    Ped1ch[det][ich] = Amp;

                    Amp = Ped1cmod[det][ich] / nch;
                    Ped1cmod[det][ich] = Amp;

                    Double_t prms2 = Ped1ch2[det][ich] / nch - Ped1ch[det][ich] * Ped1ch[det][ich];
                    Double_t prms = 0;
                    if (prms2 > 0) prms = TMath::Sqrt(prms2);
                    Ped1ch2[det][ich] = prms;

                    prms2 = Ped1cmod2[det][ich] / nch - Ped1cmod[det][ich] * Ped1cmod[det][ich];
                    prms = 0;
                    if (prms2 > 0) prms = TMath::Sqrt(prms2);
                    Ped1cmod2[det][ich] = prms;
                }
            }
        }

    } // iter loop

    Int_t nrms = 0;
    Double_t sumrms = 0;

    for (Int_t det = 0; det < ndet; det++) {
        Int_t mChan = nchdet[det];

        for (Int_t ich = 0; ich < mChan; ich++) {
            if (noisech[det][ich] == 0) {
                Double_t Amp = Ped1ch[det][ich];
                Pedch[det][ich] = Amp;
                //                hPeds[det]->SetBinContent(ich + 1, (Float_t) Amp);

                Double_t prms = Ped1ch2[det][ich];
                Pedch2[det][ich] = prms;
                //                hPrms[det]->SetBinContent(ich + 1, (Float_t) prms);

                prms = Ped1cmod2[det][ich];
                nrms++;
                sumrms += prms;
            }
        }
    }

    if (nrms > 0) sumrms /= nrms;

    Double_t sumrmscard[maxcard];
    Double_t nrmscard[maxcard];

    Int_t cont1 = 0;
    for (Int_t det = 0; det < ndet; det++) {
        Int_t mChan = nchdet[det];

        for (Int_t icard = 0; icard < maxcard; icard++) {
            sumrmscard[icard] = 0;
            nrmscard[icard] = 0;
        }

        for (Int_t ich = 0; ich < mChan; ich++) {
            Int_t icard = (Int_t) ich / 128;

            if (noisech[det][ich] == 0) {
                Double_t prms = Ped1cmod2[det][ich];
                nrmscard[icard]++;
                sumrmscard[icard] += prms;
            }
        }
        for (Int_t icard = 0; icard < maxcard; icard++) {
            if (nrmscard[icard] > 0) sumrmscard[icard] /= nrmscard[icard];
        }

        for (Int_t ich = 0; ich < mChan; ich++) {
            Int_t icard = (Int_t) ich / 128;
            if (noisech[det][ich] == 0) {
                Double_t Amp = Ped1cmod[det][ich];
                Pedcmod[det][ich] = Amp;
                //                hPmCmod[det]->SetBinContent(ich + 1, (Float_t) Amp);

                Double_t prms = Ped1cmod2[det][ich];
                Pedcmod2[det][ich] = prms;
                //                hPmCrms[det]->SetBinContent(ich + 1, (Float_t) prms);

                if (prms > 4 * sumrmscard[icard]) {
                    //                    cout << " new noise det= " << det << " channel= " << ich << endl;
                    noisech[det][ich] = 1;
                    if (!read) {
                        cont1 = ich + det * 10000;
                        fprintf(Wnoisefile, " %d\n", cont1);
                    }
                }

            }
        }
    }

    for (Int_t det = 0; det < ndet; det++) {
        Int_t mChan = nchdet[det];
        Int_t mchip = (Int_t) mChan / nchip;

        for (Int_t ich = 0; ich < mchip; ich++) {
            if (nchan[det][ich] > 0) {
                Double_t Amp = Cmode[det][ich];
                //                hCmode[det]->SetBinContent(ich + 1, (Float_t) Amp);
                Double_t crms = Cmode2[det][ich];
                //                hCrms[det]->SetBinContent(ich + 1, (Float_t) crms);
            }
        }
    }

    if (!pedestals && !read) {
        for (Int_t det = 0; det < ndet; det++) {
            Int_t mChan = nchdet[det];
            fprintf(Wpedfile, " %d %d\n", det, mChan);

            for (Int_t ich = 0; ich < mChan; ich++) {
                Float_t ped = Pedch[det][ich];
                Float_t rms = Pedcmod2[det][ich];
                fprintf(Wpedfile, " %g %g\n", ped, rms);
            }
        }
    }
    //    fclose(Wpedfile);


    nev = -1;
    return kBMNSUCCESS;
}

ClassImp(BmnGemRaw2Digit)

