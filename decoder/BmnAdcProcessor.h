//
// Base class for processing data from ADC strip detectors
// It's used for pedestal calculation
//

#ifndef BMNADCPROCESSOR_H
#define BMNADCPROCESSOR_H

#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <vector>
#include <TStopwatch.h>
#include <memory>

#include "TString.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TColor.h"
#include "TMath.h"
#include <TH2F.h>
#include <TCanvas.h>
#include <TStyle.h>

#include "BmnADCDigit.h"
#include "BmnEnums.h"
#include <BmnSiliconStationSet.h>
#include <BmnGemStripStationSet.h>
#include <BmnCSCStationSet.h>

#define N_EV_FOR_PEDESTALS 500
#define ADC_N_CHANNELS 64 //number of ADC channels
#define ADC128_N_SAMPLES 128 //number of samples in one ADC digit //silicon
#define ADC32_N_SAMPLES 32 //number of samples in one ADC digit //gem

typedef pair<UInt_t, UChar_t> PlMapKey;

using namespace std;
using namespace TMath;

class BmnAdcProcessor {
public:
    BmnAdcProcessor(Int_t period, Int_t run, TString det, Int_t nCh, Int_t nSmpl, vector<UInt_t> vSer);
    BmnAdcProcessor(Int_t period, Int_t run, TString det, Int_t nCh, Int_t nSmpl);
    BmnAdcProcessor();
    virtual ~BmnAdcProcessor();

    void SetSerials(vector<UInt_t> &vSer);
    BmnStatus RecalculatePedestals();
    BmnStatus RecalculatePedestalsAugmented();
    void PrecalcEventModsOld(TClonesArray *adc);
    void PrecalcEventMods(TClonesArray *adc);
    void PrecalcEventMods_simd(TClonesArray *adc);
    void (BmnAdcProcessor::*PrecalcEventModsImp)(TClonesArray *adc);
    void CalcEventMods();
    void CalcEventMods_simd();
    //    BmnStatus FillProfiles(TClonesArray *adc);
    BmnStatus FillNoisyChannels();
    Double_t CalcCMS(Double_t* samples, Int_t size);
    Double_t CalcCMS1(Double_t* samples, Int_t size);
    BmnStatus SaveFilterInfo();
    BmnStatus LoadFilterInfo();

    /**
     * Calculate signal CM - pedestal CM
     * */
    Double_t CalcSCMS(Double_t* samples, Int_t size, UInt_t iCr, UInt_t iCh);

    Double_t**** GetPedData() {
        return fPedDat;
    }

    void SetNSerials(Int_t n) {
        fNSerials = n;
    }

    void SetNSamples(Int_t n) {
        fNSamples = n;
    }

    void SetNChannels(Int_t n) {
        fNChannels = n;
    }

    Int_t GetNSerials() {
        return fNSerials;
    }

    Int_t GetNChannels() {
        return fNChannels;
    }

    Int_t GetNSamples() {
        return fNSamples;
    }

    Float_t GetPedestal(Int_t ser, Int_t ch, Int_t smpl) {
        return fPedVal[ser][ch][smpl];
    }

    Float_t*** GetPedestals() {
        return fPedVal;
    }

    Double_t*** GetPedestalsRMS() {
        return fPedCMod2;
    }

    Bool_t*** GetNoisyChipChannels() {
        return fNoisyChipChannels;
    }

    Int_t GetPeriod() {
        return fPeriod;
    }

    Int_t GetRun() {
        return fRun;
    }

    Int_t GetVerbose() {
        return fVerbose;
    }

    void SetVerbose(Int_t v) {
        fVerbose = v;
    }

    vector<UInt_t>& GetSerials() {
        return fAdcSerials;
    }

    map<UInt_t, Int_t>& GetSerialMap() {
        return fSerMap;
    }

    inline UInt_t GetBoundaryRun(UInt_t nSmpl) {
        //format for SILICON data was changed during March 2017 seance (run 1542)
        //format for GEM was changed after March 2017 seance (run 1992)
        //so we have to use this crutch.
        return (nSmpl == 128) ? 1542 : 1992;
    }
    void DrawDebugHists();
    void DrawDebugHists2D();
    void ClearDebugHists();
    static unique_ptr<BmnSiliconStationSet>  GetSilStationSet(Int_t period, BmnSetup stp = kBMNSETUP);
    static BmnGemStripStationSet * GetGemStationSet(Int_t period, BmnSetup stp = kBMNSETUP);
    static BmnCSCStationSet *      GetCSCStationSet(Int_t period, BmnSetup stp = kBMNSETUP);

protected:
    Int_t fVerbose = 0;
    Int_t drawCnt = 0;
    Int_t drawCnt2d = 0;
    Double_t thrMax;
    Double_t thrDif;
    Double_t thrped;
    Int_t niter;
    Double_t cmodcut;
    Int_t fNSerials;
    vector<UInt_t> fAdcSerials; ///< list of serial id for ADC-detector
    Int_t fNSamples;
    Int_t fNChannels;
    Double_t**** fPedDat = nullptr; ///< data set to calculate pedestals
    BmnSetup fSetup;
    void Run7(Int_t* statsGem, Int_t* statsSil, Int_t* statsGemPermut, Int_t* statsSilPermut);
    void CreateGeometries();
    void InitArrays();
    Int_t* statsGem = nullptr;
    Int_t* statsSil = nullptr;
    Int_t* statsGemPermut = nullptr;
    Int_t* statsSilPermut = nullptr;
    map <Int_t, Int_t> fGemStats;
    map <Int_t, Int_t> fSilStats;
    BmnGemStripStationSet* fGemStationSet = nullptr;
    unique_ptr<BmnSiliconStationSet> fSilStationSet;
    BmnCSCStationSet* fCscStationSet = nullptr;

    map<UInt_t, Int_t> fSerMap; ///< ADC serials map

    //Double_t*** fAdc;
    Float_t*** fAdc;
    Double_t*** fPedRms; // set of calculated pedestal errors
    //Double_t*** fPedVal; //set of calculated pedestals
    Float_t*** fPedVal;
    Double_t*** fPedValTemp; //set of calculated pedestals
    //Double_t** fCMode; //set of calculated pedestal CMSs
    Float_t** fCMode;
    //Double_t** fCMode0; //set of calculated pedestal CMSs
    Float_t** fCMode0;
   // Double_t** fSMode; //set of calculated signal CMSs
    Float_t** fSMode;
   // Double_t** fSMode0; //set of calculated signal CMSs
    Float_t** fSMode0;

    vector< vector< TH1* > > hPedLine;
    vector< vector< TH1* > > hCMode;
    vector< vector< TH1* > > hSMode;

    vector< vector< TH1* > > hPedLineSi;

    vector< TH1* > hPedSi;
    vector< TH1* > hCModeSi;
    vector< TH1* > hSModeSi;
    Bool_t*** fNoisyChipChannels; // set of noisy channel flags

    Float_t** fNvals;
    //UInt_t** fNvals; // number of valid (under threshold) pedestals
    UInt_t*** fNvalsCMod; // number of valid (under threshold) pedestals
    UInt_t*** fNvalsADC; // number of valid (under threshold) ADC signals
    Double_t*** fPedCMod;
    Double_t*** fPedCMod2;
    Double_t** fSumRmsV;
    
    TString fDetName; //it's used for .txt files name 
    Int_t fPeriod;
    Int_t fRun;


private:


    //    Int_t fEntriesInGlobMap; // number of entries in BD table for Global Mapping


    UInt_t*** fAdcProfiles;
    TH2F * hcms;
    TH2F * hscms_adc;
    TH1F * hcms1;
    TH2F * hscms1_adc;
    TH1F * hcms1p;
    TH1F * hscms1p_adc;
    TH2F * hp;
    TH2F * hfilter;
    TH2F * hfilterMK;
    TCanvas *canStrip;

    ClassDef(BmnAdcProcessor, 1);
};

#endif /* BMNADCPROCESSOR_H */

