#include <TStyle.h>

#include "BmnSiliconRaw2Digit.h"

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit() {
    fEventId = -1;
    fMapFileName = "";
}

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer, TString MapFileName, BmnSetup bmnSetup, BmnADCDecoMode decoMode) : BmnAdcProcessor(period, run, "SILICON", ADC_N_CHANNELS, ADC128_N_SAMPLES, vSer) {
    fSetup = bmnSetup;
    cout << "Loading SILICON Map from FILE: Period " << period << ", Run " << run << "..." << endl;

    fEventId = -1;
    fMapFileName = MapFileName;
    ReadMapFile();

    if (decoMode == kBMNADCSM) {
        fSilStationSet = BmnAdcProcessor::GetSilStationSet(period, fSetup);

        Int_t kNStations = fSilStationSet->GetNStations();
        fSigProf = new TH1F***[kNStations];
        fNoisyChannels = new Bool_t***[kNStations];
        for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
            auto * st = fSilStationSet->GetStation(iSt);
            Int_t kNModules = st->GetNModules();
            fSigProf[iSt] = new TH1F**[kNModules];
            fNoisyChannels[iSt] = new Bool_t**[kNModules];
            for (UInt_t iMod = 0; iMod < kNModules; ++iMod) {
                auto *mod = st->GetModule(iMod);
                Int_t kNLayers = 2; // mod->GetNStripLayers();
                fSigProf[iSt][iMod] = new TH1F*[kNLayers];
                fNoisyChannels[iSt][iMod] = new Bool_t*[kNLayers];
                for (Int_t iLay = 0; iLay < kNLayers; ++iLay) {
                    auto & lay = mod->GetStripLayer(iLay);
                    Int_t kNStrips = lay.GetNStrips();
                    TString histName;
                    histName.Form("SIL_%d_%d_%d", iSt, iMod, iLay);
                    fSigProf[iSt][iMod][iLay] = new TH1F(histName, histName, kNStrips, 0, kNStrips);
                    fSigProf[iSt][iMod][iLay]->SetDirectory(0);
                    fNoisyChannels[iSt][iMod][iLay] = new Bool_t[kNStrips + 1];
                    for (Int_t iStrip = 0; iStrip <= kNStrips; ++iStrip)
                        fNoisyChannels[iSt][iMod][iLay][iStrip] = kFALSE;
                }
            }
        }
        cmodcut = 100;
        thrMax = 420;
        thrDif = 80;
        niter = 4;
    }
}

void BmnSiliconRaw2Digit::InitAdcProcessorMK(Int_t run, Int_t iread, Int_t iped, Int_t ithr, Int_t itest) {
    CreateGeometries();
    test = itest;
    if (iread > 0) read = kTRUE;
    if (iped > 0) pedestals = kTRUE;
    thrnoise = 0.03;
    nchip = 128;
    nchmin = 32;
    npevents = 0;
    cmodcut = 100;

    switch (GetPeriod()) {
        case 7:
            ndet = 14;
            nadc = 4;
            ncoor = 28;
            nadcmax = 5;
            modul = {0, 3, 7, 4, 2, 5, 6, 1, 0, 1, 2, 3, 1, 0};
            detorder = {3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 2, 2};

            thrDif = 80;
            niter = 4;
            niterped = 3;
            thrped = 340;
            if (ithr == 1) thrped = 280;
            break;
        case 6:
            ndet = 8;
            nadc = 2;
            ncoor = 16;
            modul = {0, 1, 2, 3, 4, 5, 6, 7};
            detorder = {0, 0, 0, 0, 0, 0, 0, 0};

            thrDif = 60;
            niter = 5;
            niterped = 4;
            thrped = 360;
            if (ithr == 1) thrped = 300;
            break;
        default:
            fprintf(stderr, "Unsupported Period %d !\n", GetPeriod());
            break;
    }
    thresh = 420;
    if (ithr == 1) thresh = 360;
    if (test == 2) {
        thresh = thrped;
        niter = niterped;
    }
    nx1bin = 640;
    ny1bin = 640;
    nallmid = 1280;
    maxChan = 1280;
    maxchip2 = 10;
    maxAdc = 8192;
    nclmax = 2;
    nx1max = 640;
    ny1max = 640;
    nadc_samples = 128;
    nevmax = 2;

    fSerials.resize(nadc, 0);
    nx1det.resize(ndet, nx1bin);
    ny1det.resize(ndet, ny1bin);
    nchdet.resize(ndet, nallmid);

    detadc.resize(nadc, vector<Int_t>(maxAdc, -1));
    ichadc.resize(nadc, vector<Int_t>(maxAdc, -1));

    Pedadc.resize(nadc, vector<Double_t>(maxAdc, 0.0));
    Pedadc2.resize(nadc, vector<Double_t>(maxAdc, 0.0));
    nchadc.resize(nadc, vector<Int_t>(maxAdc, 0));
    noisech.resize(ndet, vector<Int_t>(maxChan, 0));
    Pedchr.resize(ndet, vector<Double_t>(maxChan, 0.0));
    Pedchr2.resize(ndet, vector<Double_t>(maxChan, 0.0));
    x1map.resize(nx1bin, 0);
    y1map.resize(ny1bin, 0);
    Ampx1.resize(ndet, vector<Double_t>(nx1max, 0.0));
    Ampy1.resize(ndet, vector<Double_t>(ny1max, 0.0));
    cmodhitx1.resize(ndet, vector<Int_t>(nx1max, 0));
    cmodhity1.resize(ndet, vector<Int_t>(ny1max, 0));
    Nclustx.resize(ndet, 0);
    Nclusty.resize(ndet, 0);

    nchan.resize(ndet, vector<Int_t>(maxchip2, 0));
    nchan1.resize(ndet, vector<Int_t>(maxChan, 0));

    Ampch.resize(ndet, vector<Double_t>(maxChan, 0.0));
    Pedch.resize(ndet, vector<Double_t>(maxChan, 0.0));
    Pedch2.resize(ndet, vector<Double_t>(maxChan, 0.0));

    Ped1ch.resize(ndet, vector<Double_t>(maxChan, 0.0));
    Ped1ch2.resize(ndet, vector<Double_t>(maxChan, 0.0));

    Ped1cmod.resize(ndet, vector<Double_t>(maxChan, 0.0));
    Ped1cmod2.resize(ndet, vector<Double_t>(maxChan, 0.0));

    Pedcmod.resize(ndet, vector<Double_t>(maxChan, 0.0));
    Pedcmod2.resize(ndet, vector<Double_t>(maxChan, 0.0));

    Cmode.resize(ndet, vector<Double_t>(maxchip2, 0.0));
    C1mode.resize(ndet, vector<Double_t>(maxchip2, 0.0));
    Cmall.resize(ndet, vector<Double_t>(maxchip2, 0.0));
    Smode.resize(ndet, vector<Double_t>(maxchip2, 0.0));
    Cmode2.resize(ndet, vector<Double_t>(maxchip2, 0.0));

    Cmode1.resize(ndet, vector<Double_t>(maxchip2, 0.0));
    Smode1.resize(ndet, vector<Double_t>(maxchip2, 0.0));

    Clustx.resize(ndet, 0.0);
    Clusty.resize(ndet, 0.0);

    Clustxx.resize(ndet, vector<Double_t>(nclmax, 0.0));
    Clustyy.resize(ndet, vector<Double_t>(nclmax, 0.0));

    Asample.resize(ndet, vector<Double_t>(maxChan, 0.0));

    sigx1.resize(ncoor, vector<Double_t>(maxChan, 0.0));
    nsigx1.resize(ncoor, vector<Int_t>(maxChan, 0));
    cmodfl.resize(ncoor, vector<Int_t>(maxChan, 0));
    nchsig.resize(ncoor, 0);

    rawx1.resize(nevmax, vector<vector<Double_t> >(ndet, vector<Double_t>(maxChan, 0.0)));
    subx1.resize(nevmax, vector<vector<Double_t> >(ndet, vector<Double_t>(maxChan, 0.0)));
    pedx1.resize(nevmax, vector<vector<Double_t> >(ndet, vector<Double_t>(maxChan, 0.0)));
    cmdx1.resize(nevmax, vector<vector<Double_t> >(ndet, vector<Double_t>(maxchip2, 0.0)));
    //    Double_t cmdx1[nevmax][ndet][maxchip2];
    chmap.resize(nallmid, 0);
    hNhits.resize(ndet, nullptr);

    TString FSerials = Form("%s/input/Si_Serials_Run%d.txt", getenv("VMCWORKDIR"), GetPeriod());
    ifstream inFile(FSerials);
    if (!inFile.is_open())
        cout << "Error opening File with ADC Serials (" << FSerials << ")!" << endl;

    UInt_t ser;
    for (Int_t ind = 0; ind < nadc; ind++) {
        inFile >> std::hex >> ser;
        fSerials[ind] = ser;
        //        cout << "Serials= " << ind << " " << std::hex << ser << std::dec << endl;
    }

    // module 8->14

    Int_t iadc = 0;
    Int_t idet = 7;
    //MK table channel -1
    Int_t minchip = 32;
    Int_t maxchip = 36;
    Int_t isidet = 0;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < nx1bin) x1map[ibin] = ich2;
            else cout << " x1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }

    //MK table channel -1
    minchip = 26;
    maxchip = 30;
    isidet = 1;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < ny1bin) y1map[ibin] = ich2;
            else cout << " y1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }


    // module 1->12

    idet = 0;
    //MK table channel -1
    minchip = 42;
    maxchip = 46;
    isidet = 0;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < nx1bin) x1map[ibin] = ich2;
            else cout << " x1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }

    //MK table channel -1
    minchip = 37;
    maxchip = 41;
    isidet = 1;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < ny1bin) y1map[ibin] = ich2;
            else cout << " y1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }


    // module 2->10

    idet = 1;
    //MK table channel -1
    minchip = 5;
    maxchip = 9;
    isidet = 0;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < nx1bin) x1map[ibin] = ich2;
            else cout << " x1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }

    //MK table channel -1
    minchip = 10;
    maxchip = 14;
    isidet = 1;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < ny1bin) y1map[ibin] = ich2;
            else cout << " y1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }


    // module 5->9

    idet = 4;
    //MK table channel -1
    minchip = 16;
    maxchip = 20;
    isidet = 0;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < nx1bin) x1map[ibin] = ich2;
            else cout << " x1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }

    //MK table channel -1
    minchip = 21;
    maxchip = 25;
    isidet = 1;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < ny1bin) y1map[ibin] = ich2;
            else cout << " y1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }


    // module 3->2

    iadc = 1;
    idet = 2;
    //MK table channel -1
    minchip = 42;
    maxchip = 46;
    isidet = 0;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < nx1bin) x1map[ibin] = ich2;
            else cout << " x1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }

    //MK table channel -1
    minchip = 37;
    maxchip = 41;
    isidet = 1;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < ny1bin) y1map[ibin] = ich2;
            else cout << " y1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }


    // module 7->5

    //MK table channel -1
    minchip = 32;
    maxchip = 36;
    isidet = 0;
    idet = 6;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < nx1bin) x1map[ibin] = ich2;
            else cout << " x1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }

    //MK table channel -1
    minchip = 26;
    maxchip = 30;
    isidet = 1;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < ny1bin) y1map[ibin] = ich2;
            else cout << " y1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }

    // module 6->8

    idet = 5;
    //MK table channel -1
    minchip = 16;
    maxchip = 20;
    isidet = 0;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < nx1bin) x1map[ibin] = ich2;
            else cout << " x1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }

    //MK table channel -1
    minchip = 21;
    maxchip = 25;
    isidet = 1;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < ny1bin) y1map[ibin] = ich2;
            else cout << " y1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }


    // module 4->11

    idet = 3;
    //MK table channel -1
    minchip = 5;
    maxchip = 9;
    isidet = 0;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < nx1bin) x1map[ibin] = ich2;
            else cout << " x1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }

    //MK table channel -1
    minchip = 10;
    maxchip = 14;
    isidet = 1;

    for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
        for (Int_t ic = 0; ic < nadc_samples; ic++) {
            Int_t ich = ichip * nadc_samples + ic;
            Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
            Int_t ibin = (ichip - minchip) * nadc_samples + ic;
            detadc[iadc][ich] = idet;
            ichadc[iadc][ich] = ich2;
            if (ibin >= 0 && ibin < ny1bin) y1map[ibin] = ich2;
            else cout << " y1map ibin= " << ibin << " ich= " << ich2 << endl;
        }
    }

    if (GetPeriod() == 7) {
        // module 1 vert_near

        iadc = 2;
        idet = 8;
        //MK table channel -1
        minchip = 5;
        maxchip = 9;
        isidet = 0;

        for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
            for (Int_t ic = 0; ic < nadc_samples; ic++) {
                Int_t ich = ichip * nadc_samples + ic;
                Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
                Int_t ibin = (ichip - minchip) * nadc_samples + ic;
                detadc[iadc][ich] = idet;
                ichadc[iadc][ich] = ich2;
                if (ibin >= 0 && ibin < nx1bin) x1map[ibin] = ich2;
                else cout << " x1map ibin= " << ibin << " ich= " << ich2 << endl;
            }
        }

        //MK table channel -1
        minchip = 10;
        maxchip = 14;
        isidet = 1;

        for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
            for (Int_t ic = 0; ic < nadc_samples; ic++) {
                Int_t ich = ichip * nadc_samples + ic;
                Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
                Int_t ibin = (ichip - minchip) * nadc_samples + ic;
                detadc[iadc][ich] = idet;
                ichadc[iadc][ich] = ich2;
                if (ibin >= 0 && ibin < ny1bin) y1map[ibin] = ich2;
                else cout << " y1map ibin= " << ibin << " ich= " << ich2 << endl;
            }
        }


        // module 2 vert_near

        idet = 9;
        //MK table channel -1
        minchip = 21;
        maxchip = 25;
        isidet = 0;

        for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
            for (Int_t ic = 0; ic < nadc_samples; ic++) {
                Int_t ich = ichip * nadc_samples + ic;
                Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
                Int_t ibin = (ichip - minchip) * nadc_samples + ic;
                detadc[iadc][ich] = idet;
                ichadc[iadc][ich] = ich2;
                if (ibin >= 0 && ibin < nx1bin) x1map[ibin] = ich2;
                else cout << " x1map ibin= " << ibin << " ich= " << ich2 << endl;
            }
        }

        //MK table channel -1
        minchip = 26;
        maxchip = 30;
        isidet = 1;

        for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
            for (Int_t ic = 0; ic < nadc_samples; ic++) {
                Int_t ich = ichip * nadc_samples + ic;
                Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
                Int_t ibin = (ichip - minchip) * nadc_samples + ic;
                detadc[iadc][ich] = idet;
                ichadc[iadc][ich] = ich2;
                if (ibin >= 0 && ibin < ny1bin) y1map[ibin] = ich2;
                else cout << " y1map ibin= " << ibin << " ich= " << ich2 << endl;
            }
        }


        // module 3 vert_near

        idet = 10;
        //MK table channel -1
        minchip = 37;
        maxchip = 41;
        isidet = 0;

        for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
            for (Int_t ic = 0; ic < nadc_samples; ic++) {
                Int_t ich = ichip * nadc_samples + ic;
                Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
                Int_t ibin = (ichip - minchip) * nadc_samples + ic;
                detadc[iadc][ich] = idet;
                ichadc[iadc][ich] = ich2;
                if (ibin >= 0 && ibin < nx1bin) x1map[ibin] = ich2;
                else cout << " x1map ibin= " << ibin << " ich= " << ich2 << endl;
            }
        }

        //MK table channel -1
        minchip = 42;
        maxchip = 46;
        isidet = 1;

        for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
            for (Int_t ic = 0; ic < nadc_samples; ic++) {
                Int_t ich = ichip * nadc_samples + ic;
                Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
                Int_t ibin = (ichip - minchip) * nadc_samples + ic;
                detadc[iadc][ich] = idet;
                ichadc[iadc][ich] = ich2;
                if (ibin >= 0 && ibin < ny1bin) y1map[ibin] = ich2;
                else cout << " y1map ibin= " << ibin << " ich= " << ich2 << endl;
            }
        }


        // module 4 vert_near

        idet = 11;
        //MK table channel -1
        minchip = 53;
        maxchip = 57;
        isidet = 0;

        for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
            for (Int_t ic = 0; ic < nadc_samples; ic++) {
                Int_t ich = ichip * nadc_samples + ic;
                Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
                Int_t ibin = (ichip - minchip) * nadc_samples + ic;
                detadc[iadc][ich] = idet;
                ichadc[iadc][ich] = ich2;
                if (ibin >= 0 && ibin < nx1bin) x1map[ibin] = ich2;
                else cout << " x1map ibin= " << ibin << " ich= " << ich2 << endl;
            }
        }

        //MK table channel -1
        minchip = 58;
        maxchip = 62;
        isidet = 1;

        for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
            for (Int_t ic = 0; ic < nadc_samples; ic++) {
                Int_t ich = ichip * nadc_samples + ic;
                Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
                Int_t ibin = (ichip - minchip) * nadc_samples + ic;
                detadc[iadc][ich] = idet;
                ichadc[iadc][ich] = ich2;
                if (ibin >= 0 && ibin < ny1bin) y1map[ibin] = ich2;
                else cout << " y1map ibin= " << ibin << " ich= " << ich2 << endl;
            }
        }

        // module 1 vert_far

        iadc = 3;
        idet = 12;
        //MK table channel -1
        minchip = 5;
        maxchip = 9;
        isidet = 0;

        for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
            for (Int_t ic = 0; ic < nadc_samples; ic++) {
                Int_t ich = ichip * nadc_samples + ic;
                Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
                Int_t ibin = (ichip - minchip) * nadc_samples + ic;
                detadc[iadc][ich] = idet;
                ichadc[iadc][ich] = ich2;
                if (ibin >= 0 && ibin < nx1bin) x1map[ibin] = ich2;
                else cout << " x1map ibin= " << ibin << " ich= " << ich2 << endl;
            }
        }

        //MK table channel -1
        minchip = 10;
        maxchip = 14;
        isidet = 1;

        for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
            for (Int_t ic = 0; ic < nadc_samples; ic++) {
                Int_t ich = ichip * nadc_samples + ic;
                Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
                Int_t ibin = (ichip - minchip) * nadc_samples + ic;
                detadc[iadc][ich] = idet;
                ichadc[iadc][ich] = ich2;
                if (ibin >= 0 && ibin < ny1bin) y1map[ibin] = ich2;
                else cout << " y1map ibin= " << ibin << " ich= " << ich2 << endl;
            }
        }


        // module 2 vert_far

        idet = 13;
        //MK table channel -1
        minchip = 21;
        maxchip = 25;
        isidet = 0;

        for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
            for (Int_t ic = 0; ic < nadc_samples; ic++) {
                Int_t ich = ichip * nadc_samples + ic;
                Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
                Int_t ibin = (ichip - minchip) * nadc_samples + ic;
                detadc[iadc][ich] = idet;
                ichadc[iadc][ich] = ich2;
                if (ibin >= 0 && ibin < nx1bin) x1map[ibin] = ich2;
                else cout << " x1map ibin= " << ibin << " ich= " << ich2 << endl;
            }
        }

        //MK table channel -1
        minchip = 26;
        maxchip = 30;
        isidet = 1;

        for (Int_t ichip = minchip; ichip <= maxchip; ichip++) {
            for (Int_t ic = 0; ic < nadc_samples; ic++) {
                Int_t ich = ichip * nadc_samples + ic;
                Int_t ich2 = isidet * nx1max + (ichip - minchip) * nadc_samples + ic;
                Int_t ibin = (ichip - minchip) * nadc_samples + ic;
                detadc[iadc][ich] = idet;
                ichadc[iadc][ich] = ich2;
                if (ibin >= 0 && ibin < ny1bin) y1map[ibin] = ich2;
                else cout << " y1map ibin= " << ibin << " ich= " << ich2 << endl;
            }
        }
    }
    // variables initialization
    for (Int_t ibin = 0; ibin < nallmid; ibin++) chmap[ibin] = -1;

    for (Int_t det = 0; det < ndet; det++) {
        for (Int_t ich = 0; ich < maxChan; ich++) noisech[det][ich] = 0;
    }

    TString tempDir = TString(getenv("VMCWORKDIR")) + TString("/input/");
    if (BmnFunctionSet::CheckDirectoryExist(tempDir, 1, kWritePermission) < 1)
        tempDir = TString(gSystem->GetWorkingDirectory().c_str()) + "/";
    printf("Temp directory: %s\n", tempDir.Data());

    pedname = tempDir + "RSiPed_";
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

    //        FILE *Rnoisefile;
    rnoisename = tempDir + "RSiNoise_";
    if (ithr == 1) rnoisename = tempDir + "RSiNoise2_";
    rnoisename += run;
    rnoisename += ".dat";
    if (read) {
        cout << " Read noise file " << rnoisename << endl;
        cout << endl;
        Rnoisefile = fopen(rnoisename, "r");
        if (!Rnoisefile)
            perror("Noise file %s open error");
        else {

            Int_t noise;
            while (!feof(Rnoisefile)) {
                fgets(ss, 10, Rnoisefile);
                sscanf(ss, "%d", &noise);
                Int_t det = (Int_t) noise / 10000;
                Int_t chan = noise - det * 10000;
                if (chan >= 0 && chan < maxChan) {
                    noisech[det][chan] = 1;
                    //                cout << "read noise " << noise << " det " << det << " chan " << chan << endl;
                }
            }
        }
    }


    for (Int_t det = 0; det < ndet; det++) {
        TString tmp = "Nhits_";
        tmp += det;
        Int_t mChan = nchdet[det];
        hNhits[det] = new TH1I(tmp, tmp, mChan, 0, mChan);
        hNhits[det]->SetDirectory(0);
    }

    //    FILE *Wnoisefile;
    wnoisename = tempDir + "WSiNoise2_"; // WSiNoise_
    wnoisename += run;
    wnoisename += ".dat";

    //    FILE *Wpedfile;
    wpedname = tempDir + "SiPed_"; // SiPed_
    wpedname += run;
    wpedname += ".dat";
    if (!read) {
        Wnoisefile = fopen(wnoisename, "w");
        Wpedfile = fopen(wpedname, "w");
        if (!Wnoisefile && !Wpedfile)
            Fatal("InitAdcProcessorMK", "Could not create noise/pedestal file!");
    }


    for (Int_t ibin = 0; ibin < nx1bin; ibin++) {
        Int_t ch = x1map[ibin];
        if (ch < nallmid) chmap[ch] = ibin;
    }

    for (Int_t ibin = 0; ibin < ny1bin; ibin++) {
        Int_t ch = y1map[ibin];
        if (ch < nallmid) chmap[ch] = ibin + 1000;
    }

}

BmnSiliconRaw2Digit::~BmnSiliconRaw2Digit() {
    if (Rnoisefile == nullptr && Wnoisefile == nullptr) {
        Int_t kNStations = fSilStationSet->GetNStations();
        for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
            auto * st = fSilStationSet->GetStation(iSt);
            Int_t kNModules = st->GetNModules();
            for (UInt_t iMod = 0; iMod < kNModules; ++iMod) {
                auto *mod = st->GetModule(iMod);
                Int_t kNLayers = 2; //mod->GetNStripLayers();
                for (Int_t iLay = 0; iLay < kNLayers; ++iLay) {
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
        if (canStrip) delete canStrip;
    } else {
        // MK Postprocessing
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
                            Int_t ichip = (Int_t) j / 128;
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
                            Int_t ichip = (Int_t) j / 128;
                            Double_t sum = sumhits[ichip];
                            //                        printf("nhits = %f sum = %f  ichip = %i j = %i\n", hNhits[det]->GetBinContent(j + 1), sum, ichip, j);
                            if ((hNhits[det]->GetBinContent(j + 1) > 5 * sum && sum > 10) ||
                                    (hNhits[det]->GetBinContent(j + 1) > 4 * sum && sum > 100) ||
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

                for (Int_t j = 0; j < mChan; ++j) {
                    if (noisech[det][j] == 0) {
                        Double_t sumhitdet = hNhits[det]->GetBinContent(j + 1) / (Double_t) npevents;
                        //                    hnoise[det]->Fill(sumhitdet);
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
        if (!read && wnoisename.Length()) {
            Int_t retn = system(Form("mv %s %s", wnoisename.Data(), rnoisename.Data()));
            //        printf("mv    noise ret %d\n", retn);
            Int_t retp = system(Form("mv %s %s", wpedname.Data(), pedname.Data()));
            //        printf("mv pedestal ret %d\n", retp);
        }
    }
    //    for (Int_t iCr = 0; iCr < nadc; ++iCr) {
    //        delete hPedSi[iCr];
    //        delete hSModeSi[iCr];
    //        delete hCModeSi[iCr];
    //    }
    //    for (Int_t iCr = 0; iCr < nadc; ++iCr)
    //        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
    //            delete hPedLineSi[iCr][iCh];
    //        }
    //    for (auto &it : fMap) delete it;
    for (auto &it : fOuterMap)
        for (auto &inner : it.second)
            delete inner.second;
}

BmnStatus BmnSiliconRaw2Digit::ReadMapFile() {
    UInt_t ser = 0;
    Int_t ch_lo = 0;
    Int_t ch_hi = 0;
    Int_t mod_adc = 0;
    Int_t mod = 0;
    Int_t lay = 0;
    Int_t station = 0;
    string dummy;

    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fMapFileName;
    ifstream inFile(name.Data());
    if (!inFile.is_open()) {
        cout << "Error opening map-file (" << name << ")!" << endl;
        return kBMNERROR;
    }
    getline(inFile, dummy); //comment line in input file
    getline(inFile, dummy); //comment line in input file
    getline(inFile, dummy); //comment line in input file
    getline(inFile, dummy); //comment line in input file

    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> ch_lo >> ch_hi >> mod_adc >> mod >> lay >> station;
        if (!inFile.good()) break;
        BmnSiliconMapping * record = new BmnSiliconMapping();
        record->layer = lay;
        record->serial = ser;
        record->module = mod;
        if (ch_lo < ch_hi) {
            record->channel_low = ch_lo;
            record->channel_high = ch_hi;
        } else {
            record->channel_low = ch_hi;
            record->channel_high = ch_lo;
            record->inverted = true;
        }
        record->station = station;
        fMap.push_back(record);
        auto it = fOuterMap.find(ser);
        if (it == fOuterMap.end()) { // create inner channel map for the serial 
            InChanMapSil inner;
            inner.insert(make_pair(record->channel_low - 1, nullptr));
            inner.insert(make_pair(record->channel_high, record));
            fOuterMap.insert(make_pair(ser, move(inner)));
        } else { // add range to the existing inner channel map
            InChanMapSil &inner = it->second;
            auto innerItHi = inner.find(record->channel_high);
            auto innerItLo = inner.find(record->channel_low - 1);
            if (innerItHi == inner.end()) {
                inner.insert(make_pair(record->channel_high, record));
            } else {
                if (innerItHi->second == nullptr) {
                    inner.erase(innerItHi);
                    inner.insert(make_pair(record->channel_high, record));
                } else {
                    fprintf(stderr, "Wrong %s map! Overlapping intervals for %08X!\n", fDetName.Data(), ser);
                    return kBMNERROR;
                }
            }
            if (innerItLo == inner.end()) {
                inner.insert(make_pair(record->channel_low - 1, nullptr));
            }
            //            auto innerIt = inner.find(record->channel_low);
            //            if (innerIt == inner.end()) {
            //                inner.insert(make_pair(record->channel_low - 1, nullptr));
            //                inner.insert(make_pair(record->channel_high, record));
            //            } else {
            //                if (innerIt->second == nullptr) {
            //                    innerIt->second = record;
            //                } else {
            //                    //                    fprintf(stderr, "Wrong CSC map!\n");
            //                    //                    return kBMNERROR;
            //                }
            //            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnSiliconRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray *silicon) {
    //    printf("Event %i\n", fEventId);
    fEventId++;
    //    for (auto &it : fMap)
    //        for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
    //            BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
    //            if (adcDig->GetSerial() == it.serial && (adcDig->GetChannel() >= it.channel_low && adcDig->GetChannel() <= it.channel_high))
    //                ProcessDigit(adcDig, &it, silicon, kFALSE);
    //        }
    TStopwatch timer;
    Double_t rtime;
    Double_t ctime;
    timer.Start();
    (this->*PrecalcEventModsImp)(adc);
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    //    printf("\nReal time %f s, CPU time %f s  PrecalcEventMods\n", rtime, ctime);
    timer.Start();
#ifdef BUILD_DEBUG
    CalcEventMods();
#else
    CalcEventMods_simd();
#endif
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    //    printf("Real time %f s, CPU time %f s  CalcEventMods\n", rtime, ctime);
    timer.Start();
    //    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
    //        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
    //        ProcessDigit(adcDig, FindMapEntry(adcDig), csc, kFALSE);
    //    }
    ProcessAdc(silicon, kFALSE);
    timer.Stop();
    rtime = timer.RealTime();
    ctime = timer.CpuTime();
    //    printf("Real time %f s, CPU time %f s  ProcessAdc\n", rtime, ctime);
    //    for (auto &it : fMap)
    //        for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
    //            BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
    //            if (adcDig->GetSerial() == it.serial && (adcDig->GetChannel() >= it.channel_low && adcDig->GetChannel() <= it.channel_high)) {
    //                ProcessDigit(adcDig, &it, silicon, kFALSE);
    //            }
    //        }
    return kBMNSUCCESS;
}

BmnStatus BmnSiliconRaw2Digit::FillEventMK(TClonesArray *adc, TClonesArray *silicon) {
    nev++;

    for (Int_t det = 0; det < ndet; det++) {
        for (Int_t ich = 0; ich < maxchip2; ich++) {
            Smode[det][ich] = 0;
            Cmode[det][ich] = 0;
            nchan[det][ich] = 0;

            Smode1[det][ich] = 0;
            Cmode1[det][ich] = 0;
        }
    }
    fEventId++;
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
        ProcessDigitMK(adcDig, silicon, kFALSE);
    }
    PostprocessDigitMK(silicon);

    return kBMNSUCCESS;
}

BmnStatus BmnSiliconRaw2Digit::FillProfiles(TClonesArray *adc) {
    //    for (auto it : fMap)
    //        for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
    //            BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
    //            if (adcDig->GetSerial() == it.serial && (adcDig->GetChannel() >= it.channel_low && adcDig->GetChannel() <= it.channel_high)) {
    //                ProcessDigit(adcDig, &it, NULL, kTRUE);
    //            }
    //        }
    (this->*PrecalcEventModsImp)(adc);
#ifdef BUILD_DEBUG
    CalcEventMods();
#else
    CalcEventMods_simd();
#endif
    ProcessAdc(nullptr, kTRUE);

    return kBMNSUCCESS;
}

BmnStatus BmnSiliconRaw2Digit::FillNoisyChannels() {
    //    const Int_t kNStations = 3;
    //    const Int_t kNModules = 8;
    //    const Int_t kNLayers = 2;
    //    const Int_t kNStrips = 640;
    const Int_t kNStripsInBunch = GetNSamples();
    //    const Int_t kNBunches = kNStrips / kNStripsInBunch;
    Int_t kNThresh = 3;
    // repeat noisy channels in the physical terms (station/module/layer)
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr) {
        auto it = fOuterMap.find(GetSerials()[iCr]);
        if (it == fOuterMap.end())
            continue;
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            InChanMapSil & inner = it->second;
            auto innerIt = inner.lower_bound(iCh);
            if (innerIt == inner.end())
                continue;
            BmnSiliconMapping* rec = innerIt->second;
            if (!rec)
                continue;
            for (Short_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                if (GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE) {
                    Int_t iStrip = MapStrip(rec, iCh, iSmpl);
                    fNoisyChannels[rec->station][rec->module][rec->layer][iStrip] = kTRUE;
                }
            }
        }
    }
    // mark noisy
    for (Int_t iSt = 0; iSt < fSilStationSet->GetNStations(); ++iSt) {
        auto * st = fSilStationSet->GetStation(iSt);
        for (Int_t iMod = 0; iMod < st->GetNModules(); ++iMod) {
            auto *mod = st->GetModule(iMod);
            for (Int_t iLay = 0; iLay < 2/*mod->GetNStripLayers()*/; ++iLay) {
                TH1F* prof = fSigProf[iSt][iMod][iLay];
                /** Big module has 2 separate parts of each layer*/
                Int_t realILayer = (mod->GetNStripLayers() == 4) ? 2 * iLay : iLay;
                auto & lay = mod->GetStripLayer(realILayer);
                Int_t kNBunches = lay.GetNStrips() / kNStripsInBunch;
                for (Int_t iBunch = 0; iBunch < kNBunches; ++iBunch) {
                    Double_t mean = 0.0;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kTRUE) continue;
                        Double_t curr = prof->GetBinContent(strip + 1);
                        Double_t next = prof->GetBinContent(strip);
                        mean += curr;
                    }
                    mean /= kNStripsInBunch;
                    for (Int_t iStrip = 0; iStrip < kNStripsInBunch; ++iStrip) {
                        Int_t strip = iStrip + iBunch * kNStripsInBunch;
                        if (fNoisyChannels[iSt][iMod][iLay][strip] == kTRUE) continue;
                        Double_t curr = prof->GetBinContent(strip + 1);
                        Double_t next = prof->GetBinContent(strip);
                        //                        if (kNThresh * meanDiff < next - curr)
                        if ((kNThresh * Abs(mean) < Abs(curr - mean))/* || (kNThresh * meanDiff < -next + curr)*/) {
                            if (fVerbose)
                                printf("profile noise on iSt %d iMod %d iLay %d strip %d\n",
                                    iSt, iMod, iLay, strip);
                            fNoisyChannels[iSt][iMod][iLay][strip] = kTRUE;
                            for (auto &it : fMap)
                                if (it->station == iSt && it->module == iMod && it->layer == iLay) {
                                    UInt_t iCr = 0;
                                    for (iCr = 0; iCr < GetSerials().size(); iCr++) {
                                        if (GetSerials()[iCr] == it->serial)
                                            break;
                                    }
                                    UInt_t iCh = it->channel_low + (strip) / GetNSamples();
                                    UInt_t iSmpl = (strip) % GetNSamples();
                                    GetNoisyChipChannels()[iCr][iCh][iSmpl] = kTRUE;
                                }
                        }
                    }
                }
            }
        }
    }
    // repeat noisy channels back into the electronics terms
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr) {
        auto it = fOuterMap.find(GetSerials()[iCr]);
        if (it == fOuterMap.end())
            continue;
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            InChanMapSil & inner = it->second;
            auto innerIt = inner.lower_bound(iCh);
            if (innerIt == inner.end())
                continue;
            BmnSiliconMapping* rec = innerIt->second;
            if (!rec)
                continue;
            for (Short_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                    Int_t iStrip = MapStrip(rec, iCh, iSmpl);
                if (fNoisyChannels[rec->station][rec->module][rec->layer][iStrip] == kTRUE)
                    GetNoisyChipChannels()[iCr][iCh][iSmpl] = kTRUE;
            }
        }
    }
    
//    for (Int_t iSt = 0; iSt < fSilStationSet->GetNStations(); ++iSt) {
//        auto * st = fSilStationSet->GetStation(iSt);
//        for (UInt_t iMod = 0; iMod < st->GetNModules(); ++iMod) {
//            auto *mod = st->GetModule(iMod);
//            for (Int_t iLay = 0; iLay < 2/*mod->GetNStripLayers()*/; ++iLay) {
//                Int_t realILayer = (mod->GetNStripLayers() == 4) ? 2 * iLay : iLay;
//                auto & lay = mod->GetStripLayer(realILayer);
//                for (Int_t iStrip = 0; iStrip < lay.GetNStrips(); ++iStrip) {
//                    if (fNoisyChannels[iSt][iMod][iLay][iStrip] == kTRUE) {
//                        for (auto &it : fMap)
//                            if (it->station == iSt && it->module == iMod && it->layer == iLay) {
//                                auto serIter = fSerMap.find(it->serial);
//                                if (serIter == fSerMap.end())
//                                    break;
//                                Int_t iCr = serIter->second;
//                                //                                UInt_t iCr = 0;
//                                //                                for (iCr = 0; iCr < GetSerials().size(); iCr++) {
//                                //                                    if (GetSerials()[iCr] == it.serial)
//                                //                                        break;
//                                //                                }
//                                UInt_t iCh = it->channel_low + iStrip / GetNSamples();
//                                UInt_t iSmpl = iStrip % GetNSamples();
//                                GetNoisyChipChannels()[iCr][iCh][iSmpl] = kTRUE;
//                                //                                printf("noise on iCr %d, iCh %i, iSmpl %i\n", iCr, iCh, iSmpl);
//                            }
//                    }
//                }
//            }
//        }
//    }

    return kBMNSUCCESS;
}

void BmnSiliconRaw2Digit::ProcessAdc(TClonesArray *silicon, Bool_t doFill) {
    cmodcut = 100;
    Double_t FinalThr = thrMax - (niter - 1) * thrDif;
    for (Int_t iCr = 0; iCr < GetNSerials(); ++iCr) {
        auto it = fOuterMap.find(GetSerials()[iCr]);
        if (it == fOuterMap.end())
            continue;
        for (Int_t iCh = 0; iCh < GetNChannels(); ++iCh) {
            InChanMapSil & inner = it->second;
            auto innerIt = inner.lower_bound(iCh);
            if (innerIt == inner.end())
                continue;
            BmnSiliconMapping* rec = innerIt->second;
            if (!rec)
                continue;
            //            for (auto &it : fMap)
            //                if (GetSerials()[iCr] == it.serial && iCh >= it.channel_low && iCh <= it.channel_high) {
            Short_t station = rec->station;
            Short_t module = rec->module;
            Short_t layer = rec->layer;
            for (Short_t iSmpl = 0; iSmpl < GetNSamples(); ++iSmpl) {
                if ((GetNoisyChipChannels()[iCr][iCh][iSmpl] == kTRUE)/* || (fPedVal[iCr][iCh][iSmpl] == 0.0)*/) continue;
                //                        Int_t strip = (iCh - it.channel_low) * GetNSamples() + iSmpl;
                Int_t strip = MapStrip(rec, iCh, iSmpl);
                Double_t sig = fAdc[iCr][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl] + fCMode[iCr][iCh] - fSMode[iCr][iCh];
                if (layer == 1)
                    sig = -sig;
                Double_t Asig = TMath::Abs(sig);
                Double_t thr = Max(FinalThr, 4 * GetPedestalsRMS()[iCr][iCh][iSmpl]);
                //                        if (layer && !doFill)
                //                            printf("%s signal %f thr %6f  prms %6f\n", it.inverted ? "inverted" : "normal", sig, thr, GetPedestalsRMS()[iCr][iCh][iSmpl]);
                if (sig > thr) {//[station][module][layer][strip] == kFALSE)) {
                    if (doFill) {
                        if (Abs(fCMode[iCr][iCh] - fSMode[iCr][iCh]) < cmodcut)
                            fSigProf[station][module][layer]->Fill(strip);
                    } else {
                        BmnSiliconDigit * resDig =
                                new((*silicon)[silicon->GetEntriesFast()])
                                BmnSiliconDigit(station, module, layer, strip, sig);
                        if ((Abs(fCMode[iCr][iCh] - fSMode[iCr][iCh]) > cmodcut))
                            resDig->SetIsGoodDigit(kFALSE);
                        else
                            resDig->SetIsGoodDigit(kTRUE);
                    }
                }
                //                    }
                //                    break;
            }
        }
    }
}

void BmnSiliconRaw2Digit::ProcessDigit(BmnADCDigit* adcDig, BmnSiliconMapping* silM, TClonesArray *silicon, Bool_t doFill) {
    const UInt_t nSmpl = adcDig->GetNSamples();
    UInt_t ch = adcDig->GetChannel();
    UInt_t ser = adcDig->GetSerial();

    auto serIter = fSerMap.find(ser);
    if (serIter == fSerMap.end()) {
        printf("Serial %08X not found in the map\n", ser);
        return;
    }
    Int_t iSer = serIter->second;
    //    for (iSer = 0; iSer < GetSerials().size(); ++iSer)
    //        if (ser == GetSerials()[iSer]) break;
    //    if (iSer == GetSerials().size())
    //        return; // serial not found

    BmnSiliconDigit candDig[nSmpl];

    for (Short_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        BmnSiliconDigit dig;
        dig.SetStation(silM->station);
        dig.SetModule(silM->module);
        dig.SetStripLayer(silM->layer);
        dig.SetStripNumber((ch - silM->channel_low) * nSmpl + iSmpl);
        Double_t sig = (GetRun() > GetBoundaryRun(ADC128_N_SAMPLES)) ? ((Double_t) ((adcDig->GetShortValue())[iSmpl] / 16)) : ((Double_t) ((adcDig->GetUShortValue())[iSmpl] / 16));
        dig.SetStripSignal(sig);
        candDig[iSmpl] = dig;
        //        if (dig.GetStation() == 0 && dig.GetModule() == 0 && dig.GetStripLayer() == 0) {
        //            hraw->Fill(dig.GetStripNumber(), dig.GetStripSignal());
        //            hsig->Fill(dig.GetStripSignal());
        //        }
    }
    Float_t*** vPed = GetPedestals();
    Double_t*** vPedRMS = GetPedestalsRMS();

    Double_t signals[nSmpl];
    Double_t cmode[nSmpl];
    Int_t nOk = 0;
    for (Short_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        signals[iSmpl] = 0.0;
        cmode[iSmpl] = 0.0;
    }
    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStripSignal() == 0 ||
                fNoisyChannels[candDig[iSmpl].GetStation()][candDig[iSmpl].GetModule()][candDig[iSmpl].GetStripLayer()][candDig[iSmpl].GetStripNumber()] == kTRUE) continue;
        signals[iSmpl] = (candDig[iSmpl]).GetStripSignal();
        //        cmode[nOk] = vPed[iSer][ch][iSmpl];
        //        nOk++;
    }
    //    Double_t CMS = CalcCMS(signals, nOk);
    //    Double_t pedCMS = CalcCMS(cmode, nOk);
    Double_t SCMS = CalcSCMS(signals, nSmpl, iSer, ch);
    //    if (SCMS > -80)
    //        return;


    //    nOk = 0;
    //    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) signals[iSmpl] = 0.0;
    //    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
    //        if (fNoisyChannels[candDig[iSmpl].GetStation()][candDig[iSmpl].GetModule()][candDig[iSmpl].GetStripLayer()][candDig[iSmpl].GetStripNumber()] == kTRUE) continue;
    //        signals[iSmpl] = vPed[iSer][ch][iSmpl];
    //        nOk++;
    //    }
    //    Double_t pedCMS = CalcCMS(signals, ch);

    for (Short_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        if ((candDig[iSmpl]).GetStation() == -1) continue;
        //        if ((candDig[iSmpl]).GetStation() == 0 && (candDig[iSmpl]).GetModule() == 0 && (candDig[iSmpl]).GetStripLayer() == 0)
        //            hrms->Fill((candDig[iSmpl]).GetStripNumber(), vPedRMS[iSer][ch][iSmpl]);

        BmnSiliconDigit * dig = &candDig[iSmpl];
        Double_t ped = vPed[iSer][ch][iSmpl];
        //        Double_t sig = Abs(dig->GetStripSignal() - CMS - ped);
        //        Double_t sig = Abs(dig->GetStripSignal() - SCMS - ped);
        Double_t sig = (GetRun() > GetBoundaryRun(ADC128_N_SAMPLES)) ?
                ((Double_t) ((adcDig->GetShortValue())[iSmpl] / 16)) :
                ((Double_t) ((adcDig->GetUShortValue())[iSmpl] / 16))
                - ped //- SCMS
                + fCMode[iSer][ch] - fSMode[iSer][ch]
                ;
        //        if (dig->GetStation() == 0 && dig->GetModule() == 0 && dig->GetStripLayer() == 0) {
        //            //            hcorrp->Fill(dig->GetStripNumber(), dig->GetStripSignal() - ped);
        //            //            hcorr->Fill(dig->GetStripNumber(), sig);
        //            hped->Fill(dig->GetStripNumber(), ped);
        //            hscms->Fill(dig->GetStripNumber(), SCMS);
        //            hcms->Fill(dig->GetStripNumber(), pedCMS);
        //            if (dig->GetStripNumber() > 511)
        //                hscms1->Fill(SCMS);
        //            hscms1full->Fill(SCMS);
        //        }
        //                printf("(dig->GetStripSignal() = %f    SCMS %f CMS %f ped %f\n", dig->GetStripSignal(), SCMS, CMS, ped);
        Double_t threshold = Max(180.0, 3.5 * vPedRMS[iSer][ch][iSmpl]); //50;//120;//160;

        //        if (dig->GetStation() == 0 && dig->GetModule() == 0 && dig->GetStripLayer() == 0) {
        //            if (dig->GetStripSignal() - ped > threshold)
        //                hcorrp->Fill(dig->GetStripNumber(), dig->GetStripSignal() - ped);
        //            if (sig > threshold)
        //                hcorr->Fill(dig->GetStripNumber(), sig);
        //        }

        if (dig->GetStripLayer() == 1)
            sig = -sig;
        if (sig < threshold || sig == 0.0) continue;
        //        if (dig->GetStation() == 0 && dig->GetModule() == 0 && dig->GetStripLayer() == 0) {
        //            hfilter->Fill(dig->GetStripNumber(), sig);
        //        }
        if (doFill) {
            //            if (Abs(- CMS + pedCMS) < cmodcut)
            if (Abs(SCMS) < cmodcut)
                fSigProf[dig->GetStation()][dig->GetModule()][dig->GetStripLayer()]->Fill(dig->GetStripNumber());
        } else {
            BmnSiliconDigit * resDig = new((*silicon)[silicon->GetEntriesFast()]) BmnSiliconDigit(dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), sig);
            if (fNoisyChannels[dig->GetStation()][dig->GetModule()][dig->GetStripLayer()][dig->GetStripNumber()] || (Abs(SCMS) > cmodcut))
                resDig->SetIsGoodDigit(kFALSE);
            else
                resDig->SetIsGoodDigit(kTRUE);
        }
    }

}

BmnStatus BmnSiliconRaw2Digit::LoadPedestalsMK(TTree* t_in, TClonesArray* adc128, BmnEventHeader* evhead, Int_t npedev) {
    const Int_t MaxSig = 2300;
    const Int_t RngSig = 150;
    //    for (Int_t iCr = 0; iCr < nadc; ++iCr) {
    //        vector<TH1*> hv;
    //        vector<TH1*> hcm;
    //        vector<TH1*> hsm;
    //        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
    //            //                        printf("Creating  icr %2d ich %2d %s\n", iCr, iCh, Form("%08X:%02d pedestal line MK", fSerials[iCr], iCh));
    //            TString hname = TString(Form("%08X:%02d pedestals MK", fSerials[iCr], iCh));
    //            TH1* h = new TH2F(hname, hname,
    //                    500, 0, 500,
    //                    fNSamples, 0, fNSamples);
    //            h->GetXaxis()->SetTitle("Event #");
    //            h->GetYaxis()->SetTitle("Sample(channel) #");
    //            h->SetDirectory(0);
    //            hv.push_back(h);
    //        }
    //        hPedLineSi.push_back(hv);
    //    }
    //    for (Int_t iCr = 0; iCr < nadc; ++iCr) {
    //        TString hname = TString(Form("%08X pedestals MK", fSerials[iCr]));
    //        TH1* h = new TH2F(hname, hname, maxAdc, 0, maxAdc, MaxSig, -RngSig, RngSig);
    //        h->GetXaxis()->SetTitle("Channel #");
    //        h->GetYaxis()->SetTitle("Signal");
    //        h->SetDirectory(0);
    //        hPedSi.push_back(h);
    //
    //        hname = TString(Form("%08X cmods MK", fSerials[iCr]));
    //        TH1* hc = new TH2F(hname, hname, maxAdc, 0, maxAdc, MaxSig, -RngSig, RngSig);
    //        hc->GetXaxis()->SetTitle("Channel #");
    //        hc->GetYaxis()->SetTitle("Signal");
    //        hc->SetDirectory(0);
    //        hCModeSi.push_back(hc);
    //
    //        hname = TString(Form("%08X smods MK", fSerials[iCr]));
    //        TH1* hs = new TH2F(hname, hname, maxAdc, 0, maxAdc, MaxSig, -RngSig, RngSig);
    //        hs->GetXaxis()->SetTitle("Channel #");
    //        hs->GetYaxis()->SetTitle("Signal");
    //        hs->SetDirectory(0);
    //        hSModeSi.push_back(hs);
    //    }


    for (Int_t iEv = 0; iEv < npedev; iEv++) {
        t_in->GetEntry(iEv);
        if (iEv % 10000 == 0)
            printf("%7d event checked\n", iEv);

        BmnEventHeader* evtype = (BmnEventHeader*) evhead;

        if (evtype->GetEventType() == 0) {
            npevents++;

            for (Int_t iAdc = 0; iAdc < adc128->GetEntriesFast(); ++iAdc) {
                BmnADCDigit* adcDig = (BmnADCDigit*) adc128->At(iAdc);

                UInt_t chan = adcDig->GetChannel();
                UInt_t ser = adcDig->GetSerial();
                Int_t nsmpl = adcDig->GetNSamples();

                Int_t iadc = -1;
                for (Int_t jadc = 0; jadc < nadc; ++jadc) {
                    if (ser == fSerials[jadc]) {
                        iadc = jadc;
                        break;
                    }
                }

                if (iadc == -1 || nsmpl != nadc_samples) {
                    //               cout << " iAdc= " << iAdc << " chan= " << chan << " Wrong serial= " << std::hex << ser << std::dec << " Or nsmpl= " << nsmpl << endl;
                } else {
                    for (Int_t ichan = 0; ichan < nadc_samples; ++ichan) {
                        Int_t ic = chan * nadc_samples + ichan;
                        Double_t Adc = (GetRun() > GetBoundaryRun(ADC128_N_SAMPLES)) ?
                                ((Double_t) (adcDig->GetShortValue())[ichan] / 16) :
                                ((Double_t) (adcDig->GetUShortValue())[ichan] / 16);

                        Int_t det = detadc[iadc][ic];
                        Int_t ich = ichadc[iadc][ic];
                        if (ich >= 0 && det >= 0 && det < ndet) {
                            Int_t mChan = nchdet[det];
                            //                            //                            if ((ichan == 5) && (chan == 18) && (iadc == 0)){
                            //                            //                                printf("iev %7d iadc %3d chan %3d ichan %3d  v %4f\n", iEv, iadc, chan, ichan, Adc);
                            //                            static_cast<TH2*> (hPedLineSi[iadc][chan])->Fill(npevents, ichan, Adc);
                            //                            //                            }

                            if (ich < mChan && noisech[det][ich] == 0) {
                                Ped1ch[det][ich] += Adc;
                                nchan1[det][ich]++;
                            }
                        }
                    }
                }
            } // iAdc
        } // evtype = 1
    } // npedev

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

    for (Int_t iter = 1; iter < niterped; iter++) {

        for (Int_t det = 0; det < ndet; det++) {
            for (Int_t ich = 0; ich < maxchip2; ich++) {
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
            if (evtype->GetEventType() == 0) {

                for (Int_t det = 0; det < ndet; det++) {
                    for (Int_t ich = 0; ich < maxchip2; ich++) {
                        Smode1[det][ich] = 0;
                        Cmode1[det][ich] = 0;
                        nchan[det][ich] = 0;
                    }
                }

                for (Int_t iAdc = 0; iAdc < adc128->GetEntriesFast(); ++iAdc) {
                    BmnADCDigit* adcDig = (BmnADCDigit*) adc128->At(iAdc);

                    UInt_t chan = adcDig->GetChannel();
                    UInt_t ser = adcDig->GetSerial();
                    Int_t nsmpl = adcDig->GetNSamples();

                    Int_t iadc = -1;
                    for (Int_t jadc = 0; jadc < nadc; ++jadc) {
                        if (ser == fSerials[jadc]) {
                            iadc = jadc;
                            break;
                        }
                    }


                    if (iadc == -1 || nsmpl != nadc_samples) {
                        //               cout << " iAdc= " << iAdc << " chan= " << chan << " Wrong serial= " << std::hex << ser << std::dec << " Or nsmpl= " << nsmpl << endl;
                    } else {
                        for (Int_t ichan = 0; ichan < nadc_samples; ++ichan) {
                            Int_t ic = chan * nadc_samples + ichan;
                            Int_t det = detadc[iadc][ic];
                            Int_t ich = ichadc[iadc][ic];

                            if (ich >= 0 && det >= 0 && det < ndet) {
                                Int_t mChan = nchdet[det];

                                if (ich < mChan && noisech[det][ich] == 0) {
                                    Int_t ichip = (Int_t) ich / nchip;

                                    Asample[det][ich] = (GetRun() > GetBoundaryRun(ADC128_N_SAMPLES)) ?
                                            ((Double_t) (adcDig->GetShortValue())[ichan] / 16) :
                                            ((Double_t) (adcDig->GetUShortValue())[ichan] / 16);
                                    Double_t Adc = Asample[det][ich];
                                    Double_t Ped = Pedch[det][ich];
                                    if (pedestals) Ped = Pedchr[det][ich];

                                    Double_t Sig = Adc - Ped;
                                    Double_t Asig = TMath::Abs(Sig);
                                    Double_t thr = thresh - iter*thrDif;

                                    //MK          Double_t thrmin1 = 3.5*Pedcmod2[det][ich];
                                    //MK          if (thr < thrmin1 && thrmin1 > 0 && iter > 1) thr = thrmin1;  

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

                for (Int_t iAdc = 0; iAdc < adc128->GetEntriesFast(); ++iAdc) {
                    BmnADCDigit* adcDig = (BmnADCDigit*) adc128->At(iAdc);

                    UInt_t chan = adcDig->GetChannel();
                    UInt_t ser = adcDig->GetSerial();
                    Int_t nsmpl = adcDig->GetNSamples();


                    Int_t iadc = -1;
                    for (Int_t jadc = 0; jadc < nadc; ++jadc) {
                        if (ser == fSerials[jadc]) {
                            iadc = jadc;
                            break;
                        }
                    }

                    if (iadc == -1 || nsmpl != nadc_samples) {
                        //               cout << " iAdc= " << iAdc << " chan= " << chan << " Wrong serial= " << std::hex << ser << std::dec << " Or nsmpl= " << nsmpl << endl;
                    } else {
                        for (Int_t ichan = 0; ichan < nadc_samples; ++ichan) {
                            Int_t ic = chan * nadc_samples + ichan;
                            Int_t det = detadc[iadc][ic];
                            Int_t ich = ichadc[iadc][ic];

                            if (ich >= 0 && det >= 0 && det < ndet) {
                                Int_t mChan = nchdet[det];

                                if (ich < mChan && noisech[det][ich] == 0) {
                                    Int_t ichip = (Int_t) ich / nchip;

                                    Asample[det][ich] = (GetRun() > GetBoundaryRun(ADC128_N_SAMPLES)) ?
                                            ((Double_t) (adcDig->GetShortValue())[ichan] / 16) :
                                            ((Double_t) (adcDig->GetUShortValue())[ichan] / 16);
                                    Double_t smode = Smode1[det][ichip];
                                    Double_t cmode = Cmode1[det][ichip];

                                    Double_t Ped = Pedch[det][ich];
                                    if (pedestals) Ped = Pedchr[det][ich];
                                    //                                    if (iter == niterped - 1) {
                                    //                                        hCModeSi[iadc]->Fill(ic, cmode);
                                    //                                        hSModeSi[iadc]->Fill(ic, smode);
                                    //                                        hPedSi[iadc]->Fill(ic, Ped);
                                    //                                    }

                                    Double_t Sig = Asample[det][ich] - Ped + cmode - smode;
                                    Double_t Asig = TMath::Abs(Sig);
                                    Double_t thr = thresh - iter*thrDif;

                                    //MK              Double_t thrmin1 = 3.5*Pedcmod2[det][ich];
                                    //MK              if (thr < thrmin1 && thrmin1 > 0 && iter > 1) thr = thrmin1;  

                                    if (Asig < thr) {
                                        Double_t Adc = Asample[det][ich];
                                        Ped1ch[det][ich] += Adc;
                                        Ped1ch2[det][ich] += Adc*Adc;
                                        nchan1[det][ich]++;

                                        Adc = Asample[det][ich] - smode;
                                        Ped1cmod[det][ich] += Adc;
                                        Ped1cmod2[det][ich] += Adc*Adc;

                                        if (iter == (niter - 1)) {
                                            Pedadc[iadc][ic] += Adc;
                                            Pedadc2[iadc][ic] += Adc * (Adc);
                                            nchadc[iadc][ic]++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } // evtype = 0
        } // ped event loop

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

    Int_t cont1 = 0;
    for (Int_t det = 0; det < ndet; det++) {
        Int_t mChan = nchdet[det];

        for (Int_t ich = 0; ich < mChan; ich++) {
            if (noisech[det][ich] == 0) {
                Double_t Amp = Ped1cmod[det][ich];
                Pedcmod[det][ich] = Amp;
                //                    hPmCmod[det]->SetBinContent(ich + 1, (Float_t) Amp);
                Double_t prms = Ped1cmod2[det][ich];
                Pedcmod2[det][ich] = prms;
                //                    hPmCrms[det]->SetBinContent(ich + 1, (Float_t) prms);
                if (prms > 5 * sumrms) {
                    //                    cout << " NEW RMS noise det= " << det << " channel= " << ich << endl;
                    noisech[det][ich] = 1;
                    if (!read) {
                        cont1 = ich + det * 10000;
                        fprintf(Wnoisefile, " %d\n", cont1);
                    }
                }

            }
        }
    }

    for (Int_t det = 0; det < nadc; det++) {

        for (Int_t ich = 0; ich < maxAdc; ich++) {
            Int_t nch = nchadc[det][ich];
            if (nch > 0) {
                Double_t Amp = Pedadc[det][ich] / nch;
                //                hPedadc[det]->SetBinContent(ich + 1, (Float_t) Amp);
                Pedadc[det][ich] = Amp;

                Double_t prms2 = Pedadc2[det][ich] / nch - Pedadc[det][ich] * Pedadc[det][ich];
                Double_t prms = 0;
                if (prms2 > 0) prms = TMath::Sqrt(prms2);
                Pedadc2[det][ich] = prms;
                //                hPrmsadc[det]->SetBinContent(ich + 1, (Float_t) prms);
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
        //        Int_t cont1 = 0;
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

    return kBMNSUCCESS;
}

void BmnSiliconRaw2Digit::ProcessDigitMK(BmnADCDigit* adcDig, TClonesArray *silicon, Bool_t doFill) {

    UInt_t chan = adcDig->GetChannel();
    UInt_t ser = adcDig->GetSerial();
    Int_t nsmpl = adcDig->GetNSamples();

    Int_t iadc = -1;
    for (Int_t jadc = 0; jadc < nadc; ++jadc) {
        if (ser == fSerials[jadc]) {
            iadc = jadc;
            break;
        }
    }
    if (iadc == -1 || nsmpl != nadc_samples) {
        //                       cout << " iAdc= " << iAdc << " chan= " << chan << " Wrong serial= " << std::hex << ser << std::dec << " Or nsmpl= " << nsmpl << endl;
    } else {
        for (Int_t ichan = 0; ichan < nadc_samples; ++ichan) {
            Int_t ic = chan * nadc_samples + ichan;

            Int_t det = detadc[iadc][ic];
            Int_t ich = ichadc[iadc][ic];
            if (ich >= 0 && det >= 0 && det < ndet) {
                Int_t mChan = nchdet[det];

                if (ich < mChan && noisech[det][ich] == 0) {
                    Double_t Adc = (GetRun() > GetBoundaryRun(ADC128_N_SAMPLES)) ?
                            ((Double_t) (adcDig->GetShortValue())[ichan] / 16) :
                            ((Double_t) (adcDig->GetUShortValue())[ichan] / 16);
                    Double_t Ped = Pedch[det][ich];
                    if (pedestals) Ped = Pedchr[det][ich];
                    Ampch[det][ich] = Adc;
                    Double_t Sig = Ampch[det][ich] - Ped; // ic or ich ?
                    Double_t Asig = TMath::Abs(Sig);
                    Int_t ichip = (Int_t) ich / nchip;

                    Double_t thr = thresh;

                    //MK             Double_t thrmin2 = 3.5*Pedcmod2[det][ich];
                    //MK             if (thr < thrmin2 && thrmin2 > 0) thr = thrmin2;  

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

void BmnSiliconRaw2Digit::PostprocessDigitMK(TClonesArray *silicon) {
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
                if (pedestals) Ped = Pedchr[det][ich];

                Double_t Adc = Ampch[det][ich];

                Int_t ichip = (Int_t) ich / nchip;
                Double_t cmode = Cmode[det][ichip];
                Double_t smode = Smode[det][ichip];
                //KV excude signals above threshold for cmode calculation
                Double_t Sig = Adc - Ped + cmode - smode;
                Double_t Asig = TMath::Abs(Sig);
                Double_t thr = thresh - iter*thrDif;

                //MK          Double_t thrmin1 = 3.5*Pedcmod2[det][ich];
                //MK          if (thr < thrmin1 && thrmin1 > 0) thr = thrmin1;  

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

                Int_t jfirstx1 = -1;
                Int_t jlastx1 = -1;
                Int_t jfirsty1 = -1;
                Int_t jlasty1 = -1;

                Double_t sumamp = 0;

                Double_t sumampx1 = 0;
                Double_t sumampy1 = 0;

                Double_t slast = 0;
                Double_t slastx1 = 0;
                Double_t slasty1 = 0;

                Int_t nclust = 0;
                Int_t nclustx1 = 0;
                Int_t nclusty1 = 0;

                Int_t nclsel = 0;
                Int_t nclselx1 = 0;
                Int_t nclsely1 = 0;

                for (Int_t ibin = 0; ibin < nx1max; ibin++) {
                    Ampx1[det][ibin] = 0;
                    cmodhitx1[det][ibin] = 0;
                }
                for (Int_t ibin = 0; ibin < ny1max; ibin++) {
                    Ampy1[det][ibin] = 0;
                    cmodhity1[det][ibin] = 0;
                }

                Int_t mChan = nchdet[det];

                for (Int_t ich = 0; ich < mChan; ich++) {
                    Double_t Ped = Pedch[det][ich];
                    if (pedestals) Ped = Pedchr[det][ich];

                    Double_t Adc = Ampch[det][ich];

                    Int_t ichip = (Int_t) ich / nchip;
                    Double_t cmode = Cmode[det][ichip];
                    Double_t smode = Smode[det][ichip];
                    Double_t Signal = Adc - Ped + cmode - smode;
                    Double_t Sig = Signal;
                    if (ich >= nx1max) Sig = -Signal;
                    if (test == 1) Sig = TMath::Abs(Signal);
                    Double_t thr = thresh - iter*thrDif;

                    Double_t thrmin1 = 3.5 * Pedcmod2[det][ich];
                    if (pedestals) thrmin1 = 3.5 * Pedchr2[det][ich];

                    if (thr < thrmin1 && thrmin1 > 0) thr = thrmin1;
                    if (GetPeriod() == 6)
                        if (det == 6 && ichip == 1 && thr < 200) thr = 200;

                    Double_t scmode = smode - cmode;
                    Int_t icmodcut = 1;
                    if (test == 1 && scmode < cmodcut) icmodcut = 0;
                    else if (test == 3 && TMath::Abs(scmode) > cmodcut) icmodcut = 0;

                    Int_t cmodflag = 0;
                    if (TMath::Abs(scmode) < cmodcut) cmodflag = 1;

                    if (nev < nevmax) {
                        cmdx1[nev][det][ichip] = scmode;
                        rawx1[nev][det][ich] = Adc - Ped;
                        //           rawx1[nev][det][ich] = Adc;
                        subx1[nev][det][ich] = Signal;
                        pedx1[nev][det][ich] = Ped;
                    }

                    Int_t nch = nchan[det][ichip];
                    Int_t ibin = -1;
                    ibin = chmap[ich];

                    //KV final hits and clusters

                    //                    if (noisech[det][ich] == 0 && nch > nchmin && TMath::Abs(Sig) > thr && ibin >= 0 && cmodflag > 0) hNhitsall[det]->Fill(ich);

                    if (noisech[det][ich] == 0 && nch > nchmin && Sig > thr && ibin >= 0) {
                        hNhits[det]->Fill(ich);
                        //                        hAmp[det]->Fill(Sig);
                        //                        if (scmode != 0) hSCmode[det]->Fill(scmode);

                        if (ich > jlast + 1 && ich > jfirst + 1) {
                            jfirst = ich;
                            jlast = ich;
                            slast = ich*Sig;
                            nclust++;
                            sumamp = Sig;
                        } else if (ich >= jfirst && ich < jfirst + 2 && jfirst > -1) {
                            jlast = ich;
                            slast += ich*Sig;
                            sumamp += Sig;
                        }

                        if (ibin < 1000) {
                            Int_t ibinx1 = ibin;
                            Int_t nx1 = nx1det[det];
                            if (ibinx1 < nx1) {
                                //                                hAmpx1[det]->Fill(Sig);
                                //                                hNAmpx1[det]->Fill(ibinx1);
                                Ampx1[det][ibinx1] = Sig;
                                cmodhitx1[det][ibinx1] = cmodflag;
                                //                                if (scmode != 0) hSCmodex1[det]->Fill(scmode);
                            } else cout << " ich= " << ich << " ibinx1= " << ibinx1 << endl;
                        } else if (ibin < 2000) {
                            Int_t ibiny1 = ibin - 1000;
                            Int_t ny1 = ny1det[det];
                            if (ibiny1 < ny1) {
                                //                                hAmpy1[det]->Fill(Sig);
                                //                                hNAmpy1[det]->Fill(ibiny1);
                                Ampy1[det][ibiny1] = Sig;
                                cmodhity1[det][ibiny1] = cmodflag;
                                //                                if (scmode != 0) hSCmodey1[det]->Fill(scmode);
                            } else cout << " ich= " << ich << " ibiny1= " << ibiny1 << endl;
                        }
                    } // threshold

                    if (ich > jlast + 1 && ich > jfirst + 1 && jfirst > -1 && jlast > -1 && nclust > nclsel) {
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

                Int_t ny1 = ny1det[det];
                for (Int_t ibin = 0; ibin < ny1; ibin++) {
                    Double_t Sig = Ampy1[det][ibin];
                    Int_t cmodhit = cmodhity1[det][ibin];
                    if (Sig > 0) {

                        Int_t coor = det * 2 + 1;
                        Int_t nch = nchsig[coor];
                        sigx1[coor][nch] = Sig;
                        nsigx1[coor][nch] = ibin;
                        cmodfl[coor][nch] = cmodhit;
                        nchsig[coor]++;
                        //           if (iEv < npedev+10) 
                        //             cout << " iEv= " << iEv << " det= " << det << " coory1= " << coor << " nch= " << nch << " ibin= " << ibin << " Sig= " << Sig << endl;


                        if (ibin > jlasty1 + 1 && ibin > jfirsty1 + 1) {
                            jfirsty1 = ibin;
                            jlasty1 = ibin;
                            slasty1 = ibin*Sig;
                            nclusty1++;
                            sumampy1 = Sig;
                        } else if (ibin >= jfirsty1 && ibin < jfirsty1 + 2 && jfirsty1 > -1) {
                            jlasty1 = ibin;
                            slasty1 += ibin*Sig;
                            sumampy1 += Sig;
                        }
                    }

                    if (ibin > jlasty1 + 1 && ibin > jfirsty1 + 1 && jfirsty1 > -1 && jlasty1 > -1 && nclusty1 > nclsely1) {
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

                Int_t nx1 = nx1det[det];
                for (Int_t ibin = 0; ibin < nx1; ibin++) {
                    Double_t Sig = Ampx1[det][ibin];
                    Int_t cmodhit = cmodhitx1[det][ibin];
                    if (Sig > 0) {

                        Int_t coor = det * 2 + 0;
                        Int_t nch = nchsig[coor];
                        sigx1[coor][nch] = Sig;
                        nsigx1[coor][nch] = ibin;
                        cmodfl[coor][nch] = cmodhit;
                        nchsig[coor]++;
                        //           if (iEv < npedev+10) 
                        //             cout << " iEv= " << iEv << " det= " << det << " coorx1= " << coor << " nch= " << nch << " ibin= " << ibin << " Sig= " << Sig << endl;

                        if (ibin > jlastx1 + 1 && ibin > jfirstx1 + 1) {
                            jfirstx1 = ibin;
                            jlastx1 = ibin;
                            slastx1 = ibin*Sig;
                            nclustx1++;
                            sumampx1 = Sig;
                        } else if (ibin >= jfirstx1 && ibin < jfirstx1 + 2 && jfirstx1 > -1) {
                            jlastx1 = ibin;
                            slastx1 += ibin*Sig;
                            sumampx1 += Sig;
                        }
                    }

                    if (ibin > jlastx1 + 1 && ibin > jfirstx1 + 1 && jfirstx1 > -1 && jlastx1 > -1 && nclustx1 > nclselx1) {
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

                //                if (nclsel > 0) NClust[det]->Fill(nclust);
                //                if (nclselx1 > 0) NClustX1[det]->Fill(nclselx1);
                //                if (nclsely1 > 0) NClustY1[det]->Fill(nclsely1);

                Nclustx[det] = nclselx1;
                Nclusty[det] = nclsely1;

            } // ndet

            Int_t nchsum = 0;
            Int_t maxcoor = ncoor;
            for (Int_t coor = 0; coor < maxcoor; ++coor) {
                Int_t nch = nchsig[coor];
                //        if (iEv < npedev+10 && nch > 0) cout << " coor= " << coor << " nch= " << nch << endl;  
                if (nch > 0) {
                    for (int ich = 0; ich < nch; ++ich) {
                        Int_t isig = nsigx1[coor][ich];
                        Int_t icmodfl = cmodfl[coor][ich];
                        Double_t sig = sigx1[coor][ich];
                        Int_t strip = isig + 1;
                        Int_t det0 = (Int_t) coor / 2;
                        Int_t mod = modul[det0];
                        Int_t layer = coor - det0 * 2;
                        Int_t det = detorder[det0];
                        Int_t plane = layer;
                        // converting MK -> SM
                        det = fSilStats.find(det)->second;
                        strip -= 1;

                        if (read) {
                            BmnSiliconDigit * resDig = new((*silicon)[silicon->GetEntriesFast()]) BmnSiliconDigit(det, mod, plane, strip, sig);
                            resDig->SetIsGoodDigit(icmodfl);
                        }
                        nchsum++;
                    }
                }
            }

        } // iter=niter-1

    } // niter
}


//void BmnSiliconRaw2Digit::RecalculatePedestalsByMap() {
//    const UShort_t nSmpl = fNSamples;
//    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
//        for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
//            //            memset(fNvalsADC[iCr][iCh], 0, sizeof (UInt_t) * fNSamples);
//            for (Int_t iSmpl = 0; iSmpl < fNSamples; ++iSmpl) {
//                fPedVal[iCr][iCh][iSmpl] = 0.0;
//                fPedValTemp[iCr][iCh][iSmpl] = 0.0;
//                fPedRms[iCr][iCh][iSmpl] = 0.0;
//                fNvalsADC[iCr][iCh][iSmpl] = 0;
//                //                fNoisyChipChannels[iCr][iCh][iSmpl] = kFALSE;
//            }
//        }
//    for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
//        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
//            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
//                Int_t nOk = 0;
//                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
//                    if (fPedDat[iCr][iEv][iCh][iSmpl] == 0.0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
//                    Bool_t foundInMap = kFALSE;
//                    for (auto & channelMapEl : fMap) {
//                        if ((fAdcSerials[iCr] == channelMapEl.serial) && (iCh >= channelMapEl.channel_low) && (iCh <= channelMapEl.channel_high)) {
//                            foundInMap = kTRUE;
//                            break;
//                        }
//                    }
//                    if (!foundInMap)
//                        continue;
//                    fPedVal[iCr][iCh][iSmpl] += fPedDat[iCr][iEv][iCh][iSmpl]; // / N_EV_FOR_PEDESTALS);
//                    fNvalsADC[iCr][iCh][iSmpl]++;
//                }
//            }
//    }
//    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
//        for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
//            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
//                if (fNvalsADC[iCr][iCh][iSmpl])
//                    fPedVal[iCr][iCh][iSmpl] /= fNvalsADC[iCr][iCh][iSmpl];
//    // iteratively calculate pedestals and CMSs
//    Double_t rmsthr = 200.0;
//    Double_t rmsthrf = 200.0;
//    Int_t nIters = 4;
//    for (Int_t iter = 1; iter < nIters; iter++) {
//        Double_t thr = thrMax - thrDif * iter; //(2 + (nIters - iter)/2.0) * sumRms; //thrMax - thrDif * iter;
//        rmsthr = 0.0;
//        rmsthrf = 0.0;
//        // clear
//        for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
//            memset(fSumRmsV[iCr], 0.0, sizeof (Double_t) * fNChannels);
//            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
//                memset(fNvalsCMod[iCr][iCh], 0, sizeof (UInt_t) * fNSamples);
//                memset(fNvalsADC[iCr][iCh], 0, sizeof (UInt_t) * fNSamples);
//                memset(fPedValTemp[iCr][iCh], 0.0, sizeof (Double_t) * fNSamples);
//                memset(fPedCMod[iCr][iCh], 0.0, sizeof (Double_t) * fNSamples);
//                memset(fPedCMod2[iCr][iCh], 0.0, sizeof (Double_t) * fNSamples);
//            }
//        }
//        for (Int_t iEv = 0; iEv < N_EV_FOR_PEDESTALS; ++iEv) {
//            // clear
//            for (Int_t iCr = 0; iCr < fNSerials; ++iCr) {
//                memset(fNvals[iCr], 0, sizeof (UInt_t) * fNChannels);
//                memset(fCMode[iCr], 0.0, sizeof (Double_t) * fNChannels);
//                memset(fSMode[iCr], 0.0, sizeof (Double_t) * fNChannels);
//            }
//            // Pedestals pre filtering
//            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
//                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
//                    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
//                        //                        if (iCr == 3 && iCh == 8) {
//                        //                            printf("iter %i iEv %i fpedDat %f noise %i\n", iter, iEv, fPedDat[iCr][iEv][iCh][iSmpl], fNoisyChipChannels[iCr][iCh][iSmpl]);
//                        //                        }
//                        if (fPedDat[iCr][iEv][iCh][iSmpl] == 0 || fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
//                        Double_t Asig = TMath::Abs(fPedDat[iCr][iEv][iCh][iSmpl] - fPedVal[iCr][iCh][iSmpl]);
//                        if (Asig < thr) {
//                            fSMode[iCr][iCh] += fPedDat[iCr][iEv][iCh][iSmpl]; // CMS from current event
//                            fCMode[iCr][iCh] += fPedVal[iCr][iCh][iSmpl]; // CMS over all pedestals
//                            //                        fPedValTemp[iCr][iCh][iSmpl] += fPedDat[iCr][iEv][iCh][iSmpl]; // CMS from current event
//                            //                        fNvalsADC[iCr][iCh][iSmpl]++;
//                            fNvals[iCr][iCh]++;
//
//                        }
//                    }
//                }
//            // normalize
//            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
//                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
//                    if (fNvals[iCr][iCh]) {
//                        fSMode[iCr][iCh] /= fNvals[iCr][iCh];
//                        fCMode[iCr][iCh] /= fNvals[iCr][iCh];
//                        hSMode[iCr][iCh]->SetBinContent(iEv, fSMode[iCr][iCh]);
//                        hCMode[iCr][iCh]->SetBinContent(iEv, fCMode[iCr][iCh]);
//                    } else {
//                        fSMode[iCr][iCh] = 0;
//                        fCMode[iCr][iCh] = 0;
//                    }
//                }
//            // Pedestals filtering
//            for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
//                for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
//                    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
//                        Double_t Adc = fPedDat[iCr][iEv][iCh][iSmpl];
//                        if ((Adc == 0) || (fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE)) continue;
//                        Double_t sig = Adc - fPedVal[iCr][iCh][iSmpl] + fCMode[iCr][iCh] - fSMode[iCr][iCh];
//                        Double_t Asig = TMath::Abs(sig);
//                        if (Asig < thr) {
//                            fPedValTemp[iCr][iCh][iSmpl] += Adc;
//                            fNvalsADC[iCr][iCh][iSmpl]++;
//
//
//                            Adc = fPedDat[iCr][iEv][iCh][iSmpl] - fSMode[iCr][iCh];
//                            fPedCMod[iCr][iCh][iSmpl] += Adc;
//                            fPedCMod2[iCr][iCh][iSmpl] += Adc*Adc;
//                            fNvalsCMod[iCr][iCh][iSmpl]++;
//
//                        }
//                    }
//                }
//        } // event loop
//
//        Double_t sumRms = 0.0;
//        Int_t nrms = 0;
//
//        //hists fill
//        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
//            for (Int_t iCh = 0; iCh < fNChannels; ++iCh) {
//                Int_t nvrms = 0;
//                fSumRmsV[iCr][iCh] = 0.0;
//                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
//                    if (fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
//                    if (fNvalsCMod[iCr][iCh][iSmpl]) {
//                        fPedCMod[iCr][iCh][iSmpl] /= fNvalsCMod[iCr][iCh][iSmpl];
//                        fPedCMod2[iCr][iCh][iSmpl] =
//                                Sqrt(Abs(fPedCMod2[iCr][iCh][iSmpl] / fNvalsCMod[iCr][iCh][iSmpl] - Sq(fPedCMod[iCr][iCh][iSmpl])));
//                        sumRms += fPedCMod2[iCr][iCh][iSmpl];
//                        fSumRmsV[iCr][iCh] += fPedCMod2[iCr][iCh][iSmpl];
//                        nrms++;
//                        nvrms++;
//                    }
//                    if (fNvalsADC[iCr][iCh][iSmpl])
//                        fPedVal[iCr][iCh][iSmpl] = fPedValTemp[iCr][iCh][iSmpl] / fNvalsADC[iCr][iCh][iSmpl];
//                    else
//                        fPedVal[iCr][iCh][iSmpl] = 0.0;
//                    fNvalsADC[iCr][iCh][iSmpl] = 0;
//                    //                    fCMode[iCr][iCh] += fPedVal[iCr][iCh][iSmpl]; // CMS over all pedestals
//                }
//                if (nvrms)
//                    fSumRmsV[iCr][iCh] /= nvrms;
//            }
//        if (nrms > 0) sumRms /= nrms;
//
//        // noise ch detection
//        for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
//            for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
//                for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
//                    if (fNoisyChipChannels[iCr][iCh][iSmpl] == kTRUE) continue;
//                    //                    printf("icr %2d ich %2d  fpedcmod2 %4f sumRms %4f\n", iCr, iCh, fPedCMod2[iCr][iCh][iSmpl], sumRms);
//
//                    if (fPedCMod2[iCr][iCh][iSmpl] > 3.5 * sumRms) {
//                        //                    if (fPedCMod2[iCr][iCh][iSmpl] > 3.5 * fSumRmsV[iCr][iCh]) {
//                        fNoisyChipChannels[iCr][iCh][iSmpl] = kTRUE;
//                        printf("new noisy ch on  cr %i ch %i smpl %i\n", iCr, iCh, iSmpl);
//                    }
//                }
//    } // iter loop
//    ofstream pedFile(Form("%s/input/%s_pedestals_%d.txt", getenv("VMCWORKDIR"), fDetName.Data(), fRun));
//    pedFile << "Serial\tCh_id\tPed\tRMS" << endl;
//    pedFile << "============================================" << endl;
//    for (Int_t iCr = 0; iCr < fNSerials; ++iCr)
//        for (Int_t iCh = 0; iCh < fNChannels; ++iCh)
//            for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
//                pedFile << hex << fAdcSerials[iCr] << dec << "\t" << iCh * nSmpl + iSmpl << "\t" << fPedVal[iCr][iCh][iSmpl] << "\t" << fPedRms[iCr][iCh][iSmpl] << endl;
//    pedFile.close();
//    return;
//}

ClassImp(BmnSiliconRaw2Digit)
