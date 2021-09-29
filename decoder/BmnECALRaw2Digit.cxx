#include "BmnECALRaw2Digit.h"
#include <TError.h>
#include <TF1.h>
#include <TMinuit.h>
#include "TMath.h"
#include "TSystem.h"
#include "TBox.h"
#include "TText.h"
#include "TCanvas.h"
#include "TRandom.h"
#include <fstream>

#define SHIFT 0

BmnECALRaw2Digit::BmnECALRaw2Digit(ULong_t runId) {
    const char * dir = getenv("VMCWORKDIR");
    
    if (runId < 1170) {
        Error(__func__,"No ECAL in run %lu\n", runId);
        return;
    }
    
    Int_t run = 0;
    Int_t pos = 0;
    
    if (runId < 2000) {
        run = 6;
        pos = 1;
    }
    else if (runId < 5186) {
        run = 7;
        if (runId < 3503) pos = 2;      //2579..3500 - Pos. 2
        else if (runId < 3514) pos = 1; //3503..3513 - Pos. 1
        else if (runId < 3589) pos = 2; //3514..3588 - Pos. 2
        else if (runId < 4153) pos = 3; //3589..4152 - Pos. 3
        else if (runId < 4307) pos = 4; //4153..4305 - Pos. 4
        else if (runId < 4906) pos = 5; //4307..4905 - Pos. 5
        else if (runId < 4976) pos = 4; //4906..4972 - Pos. 4
        else pos = 5;                   //4976..5184 - Pos. 5
    }
    
    if (run && pos) {
        TString map = TString::Format("%s/input/ECAL_map_period_%d.txt",dir,run);
        TString cal = TString::Format("%s/parameters/ecal/ECAL_calibration_run%d.txt",dir,run);
        TString geo = TString::Format("%s/geometry/ECAL_v3_run%d_pos%d.root",dir,run,pos);
        LoadAdcMap(map.Data());
        LoadCalibration(cal.Data());
        LoadGeometry(geo.Data());
    } else {
        Error(__func__,"Unable to determine ECAL position for runId %lu\n",runId);
    }
}

BmnECALRaw2Digit::BmnECALRaw2Digit(const char * map_file_name, const char * calibr_file_name, const char * geo_file_name) {
    LoadAdcMap(map_file_name);
    LoadCalibration(calibr_file_name);
    LoadGeometry(geo_file_name);
}

void BmnECALRaw2Digit::print() {
    BmnECALMapElement::PrintTitle();
    for (Int_t chan = 1; chan < MAX_ECAL_CHANNELS + 1; chan++) {
        if (fECALMapArray[chan].GetChan() != -1) {
            fECALMapArray[chan].Print();
        }
    }
    printf("Pedestal: "); fPedestal.Print();
    printf("Signal: "); fSignal.Print();
}

Bool_t BmnECALRaw2Digit::LoadAdcMap(const char* map_file_name) {
    ifstream in;
    in.open(map_file_name);
    if (!in.is_open())
    {
	Fatal(__func__,"Loading ECAL Map from file: %s - file open error!\n", map_file_name);
	return kFALSE;
    }
    Info(__func__,"Loading ECAL Map from file: %s\n", map_file_name);
    TString dummy;
    in >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;
    while (!in.eof()) {
        ULong_t adcId;
        UShort_t adcChan;
        Int_t chan;
        Float_t x, y;
        in >> std::hex >> adcId >> std::dec >> adcChan >> chan >> x >> y;
        if (!in.good()) break;
	if (adcId == 0 || adcChan == 0 || chan <= 0) continue;
        fECALMapArray[chan].SetAdcMap(chan,adcId,adcChan);
    }
    in.close();
    return kTRUE;
}

Bool_t BmnECALRaw2Digit::LoadCalibration(const char* calibr_file_name) {
    ifstream in;
    in.open(calibr_file_name);
    if (!in.is_open())
    {
	Fatal(__func__,"Loading ECAL Calibration from file: %s - file open error!\n", calibr_file_name);
	return kFALSE;
    }
    Info(__func__,"Loading ECAL Calibration from file: %s\n", calibr_file_name);
    TString dummy;
    in >> dummy >> dummy >> dummy >> dummy;
    while (!in.eof()) {
        int chan;
        float coeff, err;
        in >> chan >> coeff >> err;
        if (!in.good()) break;
	if (chan <= 0) continue;
        fECALMapArray[chan].SetCoeff(chan,coeff);
    }
    in.close();
    return kTRUE;
}

Bool_t BmnECALRaw2Digit::LoadGeometry(const char* geo_file_name) {
    TGeoVolume *top = TGeoVolume::Import(geo_file_name,"TOP");
    if (!top) {
        Fatal(__func__,"Volume TOP not found in %s\n",geo_file_name);
        return kFALSE;
    }
    TGeoNode * ecal = top->GetNode(0);
    if (!ecal) {
        Fatal(__func__,"Unexpected geometry %s\n",geo_file_name);
        return kFALSE;
    }

    TGeoNode * ecal1 = ecal->GetDaughter(0);
    if (ecal1) {
        Int_t n = ecal1->GetNdaughters();
        if (n > 504) {
            Fatal(__func__,"Expected ecal node 1 with 504 daughters or less in %s\n",geo_file_name);
            return kFALSE;
        }
        for (Int_t i = 0; i < n; i++) {
            Int_t ch = ecal1->GetDaughter(i)->GetNumber();
            if (ch < 1 || ch > 504) {
                Fatal(__func__,"Unexpected chan=%d at ecal node 1 in %s\n",ch,geo_file_name);
                return kFALSE;
            }
            fECALMapArray[ch].SetCoords(ecal1->GetDaughter(i),ecal1);
        }
    }

    TGeoNode * ecal2 = ecal->GetDaughter(1);
    if (ecal2) {
        Int_t n = ecal2->GetNdaughters();
        if (n > 504) {
            Fatal(__func__,"Expected ecal node 2 with 504 daughters or less in %s\n",geo_file_name);
            return kFALSE;
        }
        for (Int_t i = 0; i < n; i++) {
            Int_t ch = ecal2->GetDaughter(i)->GetNumber();
            if (ch < 505 || ch > 1008) {
                Fatal(__func__,"Unexpected chan=%d at ecal node 2 in %s\n",ch,geo_file_name);
                return kFALSE;
            }
            fECALMapArray[ch].SetCoords(ecal2->GetDaughter(i),ecal2);
        }
    }
    
    return kTRUE;
}


void BmnECALRaw2Digit::fillEvent(TClonesArray *data, TClonesArray *ecaldigit) {
    Int_t N = data->GetEntriesFast();
    for (Int_t i = 0; i < N; i++) {
        BmnADCDigit * digit = (BmnADCDigit*) data->At(i);
        Int_t chan;
        for (chan = 1; chan < MAX_ECAL_CHANNELS+1; chan++) {
            if (fECALMapArray[chan].GetChan() != chan) continue;
            if (fECALMapArray[chan].IsAdcMatch(digit->GetSerial(), digit->GetChannel())) break;
        }
        if (chan > MAX_ECAL_CHANNELS) continue;
        UseNovosibirskFit(digit, chan, ecaldigit);
    }
}

Bool_t BmnECALRaw2Digit::UseNovosibirskFit(BmnADCDigit* digit, Int_t chan, TClonesArray *ecaldigit) {
    UInt_t N = digit->GetNSamples();
    Short_t* s = (Short_t*)digit->GetUShortValue();
    
    Float_t pedestal = 0.;
    for (Int_t i = fPedestal.sample0; i < fPedestal.sample0 + fPedestal.samples; i++) {
        if (i >= N) return 0;
        pedestal += (Float_t) s[i];
    }
    
    pedestal /= fPedestal.samples;
    if (pedestal < fPedestal.min || pedestal > fPedestal.max) return 0;

    TGraph graph(N);
    Int_t min_n = 0;
    Int_t min_cnt = 0;
    Short_t min_v = 0;
    Float_t scale = fECALMapArray[chan].GetCoeff();
    for (Int_t n = 0; n < N; n++) {
        Short_t v = s[n];
        if (min_v > v) {
            min_v = v;
            min_n = n;
            min_cnt = 1;
        } else if (min_v == v) {
            min_n = n;
            min_cnt++;
        }
        
        Float_t y = v;
        y = (y - pedestal) * scale;
        graph.SetPoint(n, n, y);
    }
    
    if (min_v > fSignal.max || min_n - min_cnt < fSignal.sample0 || min_n > fSignal.sample0 + fSignal.samples) {
        return 0;
    } 
    
    // Initialize fitting function
    TF1 pFit("Novosibirsk", Novosibirsk, 0., N, 4);
    pFit.SetParameters(min_v, 0.01, 5., min_n);
    
    // Remove points at the flat peak if any
    while (min_cnt-- > 0) {
        graph.RemovePoint(min_n--);
    }
    
    graph.Fit("Novosibirsk", "QR0", "");

    if (!gMinuit->fCstatu.EqualTo("CONVERGED ")) return kFALSE;
    TF1 *pRes = graph.GetFunction("Novosibirsk");

    Float_t Tp = pRes->GetMinimumX();
    if (Tp < fSignal.sample0) return kFALSE;
    if (Tp > fSignal.sample0 + fSignal.samples) return kFALSE;
    
    Float_t Vp = pRes->GetMinimum();
    //if (Vp > fSignal.max || Vp < fSignal.min) return kFALSE;

    Float_t To = pRes->GetX(0.5 * Vp, 0, Tp);

    Float_t dVdT = pRes->Derivative(To);
    if (dVdT >= 0) return kFALSE;

    To = To - 0.5 * Vp / dVdT;

    if (To < fPedestal.sample0) return kFALSE;
    if (To > fSignal.sample0 + fSignal.samples) return kFALSE;

    Float_t V = pRes->Integral(To, To + fSignal.samples) / fSignal.samples;
        
    BmnECALDigit * ecal = (BmnECALDigit *) ecaldigit->ConstructedAt(ecaldigit->GetEntriesFast());
    ecal->Set(fECALMapArray + chan, -V, -Vp, To, Tp);

    return kTRUE;
}

Double_t BmnECALRaw2Digit::Novosibirsk(Double_t* x, Double_t* p) {
    //As Defined in RooNovosibirsk.cxx
    //If tail=eta=0 the Belle distribution becomes gaussia
    double tail = p[1];
    double width = p[2];
    double peak = p[3];
    if (TMath::Abs(tail) < 1.e-7)
        return p[0] * TMath::Exp(-0.5 * TMath::Power(((x[0] - peak) / width), 2));
    Double_t arg = 1.0 - (x[0] - peak) * tail / width;
    if (arg < 1.e-6) return 0.0; //Argument of logaritem negative. Real continuation -> function equals zero

    Double_t log = TMath::Log(arg);
    static const Double_t xi = 2.3548200450309494; // 2 Sqrt( Ln(4) )
    Double_t width_zero = (2.0 / xi) * TMath::ASinH(tail * xi * 0.5);
    Double_t width_zero2 = width_zero * width_zero;
    Double_t exponent = (-0.5 / (width_zero2) * log * log) - (width_zero2 * 0.5);

    return p[0] * TMath::Exp(exponent);
}


ClassImp(BmnECALRaw2Digit)

