#include "BmnGemDigitizerTask.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

#include <math.h>
#include <iostream>
#include <vector>
#include <algorithm>

// #include "BmnFieldMap.h"
#include "FairRunAna.h"
#include "FairEventHeader.h"
#include "CbmStsPoint.h"

#include "TLorentzVector.h"
#include "FairRootManager.h"
#include "FairRunSim.h"
#include <TGeoManager.h>
#include <TRefArray.h>
#include "TClonesArray.h"

#include "TRandom.h"
#include "TMath.h"

#include "TSystem.h"

#include "BmnGemDigit.h"
#include "BmnGemDigitizerQAHistograms.h"

// Class Member definitions -----------

using namespace TMath;
using namespace std;

static clock_t tStart = 0;
static clock_t tFinish = 0;
static clock_t tAll = 0;

BmnGemDigitizerTask::BmnGemDigitizerTask() :
fPersistence(kTRUE),
fResponse(kTRUE),
fDistribute(kTRUE),
fAttach(kTRUE),
fDiffuse(kTRUE),
fDistort(kTRUE),
fMCPointArray(NULL),
fMCTracksArray(NULL),
fDigitsX(NULL),
fDigitsY(NULL),
fStation(NULL),
fDigitsArrayX(NULL),
fDigitsArrayY(NULL),
fIsHistogramsInitialized(kFALSE),
fMakeQA(kFALSE),
fHisto(NULL) {
    fInputBranchName = "StsPoint";
    fOutputBranchName1 = "BmnGemDigitX";
    fOutputBranchName2 = "BmnGemDigitY";

    string gemGasFile = gSystem->Getenv("VMCWORKDIR");
    gemGasFile += "/geometry/Ar-90_CH4-10.asc";
    fGas = new BmnGemGas(gemGasFile, 130);
}

BmnGemDigitizerTask::~BmnGemDigitizerTask() {

}

InitStatus BmnGemDigitizerTask::Init() {

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    fMagField = FairRunSim::Instance()->GetField(); // Get Magnetic Field from Sim

    if (!ioman) {
        cout << "\n-E- [BmnGemDigitizerTask::Init]: RootManager not instantiated!" << endl;
        return kFATAL;
    }
    fMCPointArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");

    fGemParam = new BmnGemGeo();

    fDigitsArrayX = new DigOrigArray* [fGemParam->GetNumOfStations()];
    fDigitsArrayY = new DigOrigArray* [fGemParam->GetNumOfStations()];

    for (Int_t iStat = 0; iStat < fGemParam->GetNumOfStations(); iStat++) {
        fDigitsArrayX[iStat] = new DigOrigArray [fGemParam->GetXstripNum()];
        for (Int_t xStrips = 0; xStrips < fGemParam->GetXstripNum(); xStrips++) {
            fDigitsArrayX[iStat][xStrips].isOverlap = kFALSE;
            fDigitsArrayX[iStat][xStrips].origins.clear();
            fDigitsArrayX[iStat][xStrips].signal = 0.;
        }

        fDigitsArrayY[iStat] = new DigOrigArray [fGemParam->GetYstripNum()];
        for (Int_t yStrips = 0; yStrips < fGemParam->GetYstripNum(); yStrips++) {
            fDigitsArrayY[iStat][yStrips].isOverlap = kFALSE;
            fDigitsArrayY[iStat][yStrips].origins.clear();
            fDigitsArrayY[iStat][yStrips].signal = 0.;
        }
    }

    fDigitsX = new TClonesArray("BmnGemDigit");
    ioman->Register(fOutputBranchName1, "GEMX", fDigitsX, fPersistence);

    fDigitsY = new TClonesArray("BmnGemDigit");
    ioman->Register(fOutputBranchName2, "GEMY", fDigitsY, fPersistence);

    fNoiseThreshold = 0.; // electrons    
    fGain = 4000.0; //electrons

    if (fResponse) {
        fSpread = 0.01999;
        k1 = 1.0 / (Sqrt(TwoPi()) * fSpread);
        k2 = -0.5 / fSpread / fSpread;
    } else {
        fSpread = 0.0; // cm  // FOR TEST ONLY. NO RESPONSE.
        k1 = k2 = 1.0;
    }

    if (!fIsHistogramsInitialized && fMakeQA) {
        fHisto = new BmnGemDigitizerQAHistograms();
        fHisto->Initialize();
        fIsHistogramsInitialized = kTRUE;
    }

    cout << "-I- BmnGemDigitizerTask: Initialization successful." << endl;
    return kSUCCESS;
}

void BmnGemDigitizerTask::Exec(Option_t* opt) {

    tStart = clock();

    cout << "BmnGemDigitizer::Exec started" << endl;
    fDigitsX->Delete();
    fDigitsY->Delete();


    Int_t nPoints = fMCPointArray->GetEntriesFast();
    cout << "Number of points to be processed " << nPoints << endl;

    for (Int_t i = 0; i < nPoints; i++) {
        // cout << "Npoint = " << i << endl;
        CbmStsPoint* point = (CbmStsPoint*) fMCPointArray->At(i);
        GemProcessing(point);
    }

    for (Int_t iStat = 0; iStat < fGemParam->GetNumOfStations(); iStat++) {

        FillDigiArrays(iStat, fGemParam->GetXstripNum(), fDigitsArrayX, fDigitsX, fHisto, "X");
        FillDigiArrays(iStat, fGemParam->GetYstripNum(), fDigitsArrayY, fDigitsY, fHisto, "Y");
    }

    tFinish = clock();
    tAll = tAll + (tFinish - tStart);
    cout << "BmnGemDigitizer::Exec finished" << endl;
}

void BmnGemDigitizerTask::FillDigiArrays(Int_t iStat, Float_t num, DigOrigArray** digi_input, TClonesArray* digi_output, BmnGemDigitizerQAHistograms* fHisto, Char_t* xystrips) {

    for (Int_t strips = 0; strips < num; strips++) {
        if (digi_input[iStat][strips].signal > fNoiseThreshold) {
            Int_t outSize = digi_output->GetEntriesFast();
            new((*digi_output)[outSize]) BmnGemDigit(strips, iStat, digi_input[iStat][strips].signal);

            if (fMakeQA)
                DrawDiffGemQA2(fHisto, iStat, digi_input[iStat][strips].signal, strips, xystrips);
        }

        if (digi_input[iStat][strips].signal > 0.0) {
            digi_input[iStat][strips].origins.clear();
            digi_input[iStat][strips].signal = 0.0;
            digi_input[iStat][strips].isOverlap = kFALSE;
        }
    }
}

void BmnGemDigitizerTask::GemProcessing(const CbmStsPoint* point) {

    TLorentzVector pointPos; // coordinates of MC-points
    TLorentzVector electronPos; // coordinates of the created electrons
    Float_t localX = 0.0, localY = 0.0, localZ = 0.0; //local coordinates of electron (GEM-station coordinates)

    pointPos.SetXYZT(point->GetXIn(), point->GetYIn(), point->GetZIn(), point->GetTime());
    if ((pointPos.T() < 0)) {
        Error("BmnGemDigitizerTask::Exec", "Negative Time!");
        return;
    }

    if (fDistribute) {
        ;
    }
    if (fAttach) {
        ;
    }
    if (fDiffuse) {
        ;
    }

    electronPos = pointPos;

    localX = electronPos.X();
    localY = electronPos.Y();
    localZ = electronPos.Z();

    Int_t origin = point->GetTrackID();
    Int_t statNum = fGemParam->GetStationNum(localZ);

    Float_t xmax = -fGemParam->GetX0();
    Float_t ymax = -fGemParam->GetY0();
    Float_t zmin = fGemParam->GetZ0();
    Float_t zmax = zmin + fGemParam->GetNumOfStations() * fGemParam->GetDistAlongZ();

    if (TMath::Abs(localX) < xmax && TMath::Abs(localY) < ymax && localZ < zmax) {
        if (fMakeQA) {
            fHisto->_hX_local->Fill(localX);
            fHisto->_hY_local->Fill(localY);
            fHisto->_hZ_local->Fill(localZ);

            fHisto->_hXY_local->Fill(localX, localY);
            fHisto->_hYZ_local->Fill(localY, localZ);
            fHisto->_hXZ_local->Fill(localX, localZ);

            DrawDiffGemQA(statNum, fHisto, localX, localY, localZ);

        }
        StripResponse(localX, localY, statNum, origin, fDigitsArrayX, fDigitsArrayY);
    }
}

void BmnGemDigitizerTask::StripResponse(Float_t x, Float_t y, Int_t iStat, Int_t origin, DigOrigArray** arrX, DigOrigArray** arrY) {

    vector<Int_t> lightedXstrips;
    vector<Int_t> lightedYstrips;

    GetArea(x, fSpread * 3, lightedXstrips, fGemParam->GetXstripWidth(), fGemParam->GetXstripNum()); // X-strips 
    GetArea(y, fSpread * 3, lightedYstrips, fGemParam->GetYstripWidth(), fGemParam->GetYstripNum()); // Y-strips

    CalculateAmplitude(lightedXstrips, x, arrX, iStat, origin, fGemParam->GetX0(), fGemParam->GetXstripWidth()); // X-strips 
    CalculateAmplitude(lightedYstrips, y, arrY, iStat, origin, fGemParam->GetY0(), fGemParam->GetYstripWidth()); // Y-strips
}

void BmnGemDigitizerTask::CalculateAmplitude(vector<Int_t> &strips, Float_t av_coord, DigOrigArray** arr, Int_t iStat, Int_t origin, Float_t min, Float_t width) {

    vector<Float_t> amps;
    Float_t avAmp = 0.0, amplSum = 0.0, amplitude = 0.0;

    for (Int_t i = 0; i < strips.size(); i++) {
        amplitude = CalculateStripResponse(strips.at(i), av_coord, min, width);
        amps.push_back(amplitude);
        amplSum += amplitude;
    }

    if (amplSum > 0.0) {
        map<Int_t, Float_t>::iterator it;
        avAmp = fGain / amplSum; // Norm factor to calculate amplitudes
        for (Int_t i = 0; i < amps.size(); i++) {
            arr[iStat][strips.at(i)].signal += (amps.at(i) * avAmp);
            it = arr[iStat][strips.at(i)].origins.find(origin);
            if (it != arr[iStat][strips.at(i)].origins.end()) {
                it->second += (amps.at(i) * avAmp);
            } else {
                arr[iStat][strips.at(i)].origins.insert(pair<Int_t, Float_t>(origin, amps.at(i) * avAmp));
            }
        }
    }

}

void BmnGemDigitizerTask::GetArea(Float_t av_coord, Float_t radius, vector<Int_t> &strip, Float_t width, Int_t num) {

    Float_t coord = 0.0;
    Int_t StripNum_curr = 0;
    Float_t delta;
    if (fResponse) delta = 0.0;
    else delta = -1000.0; //for test only!!!

    coord = av_coord - radius - width;

    do {
        coord += width;
        StripNum_curr = num / 2 + ceil(coord / width) - 1;
        // cout << StripNum_curr << endl;
        if (StripNum_curr < 0 || StripNum_curr >= num) continue;
        strip.push_back(StripNum_curr);
    } while (coord < av_coord + radius + delta);
}

Float_t BmnGemDigitizerTask::CalculateStripResponse(Int_t iStrip, Float_t av_coord, Float_t min, Float_t width) {

    Float_t Center = min + (iStrip + 1 / 2) * width; // Coordinate of center of the corresponding strip

    const Float_t minX = (Center - width / 2) - av_coord;
    const Float_t maxX = (Center + width / 2) - av_coord;

    // Numerical approximated integral calculation according to the trapezium rule
    // across the strip
    Float_t ADC_Across = (Exp(k2 * minX * minX) + Exp(k2 * maxX * maxX)) * k1 * width / 2;

    // along the strip (from -inf up to +inf)
    // Edge Effects are not taken into account
    Float_t ADC_Along = k1 * TMath::Sqrt(TMath::Pi() / -k2); // cout << k1 << " " << k2 << " " << ADC_Along << endl;

    return ADC_Across * ADC_Along;
}

/// Not used now, P. B. 

Bool_t BmnGemDigitizerTask::isSubtrackInInwards(const CbmStsPoint* p1, const CbmStsPoint* p2) {
    const Float_t x1 = p1->GetXIn();
    const Float_t x2 = p2->GetXIn();
    const Float_t y1 = p1->GetYIn();
    const Float_t y2 = p2->GetYIn();
    const Float_t a = (y1 - y2) / (x1 - x2);
    const Float_t b = (y1 * x2 - x1 * y2) / (x2 - x1);
    const Float_t minR = fabs(b) / sqrt(a * a + 1);

    if (minR < 40.3) //then check if minimal distance is between our points
    {
        const Float_t x = -a * b / (a * a + 1);
        const Float_t y = b / (a * a + 1);
        if ((x1 - x) * (x2 - x) < 0 && (y1 - y) * (y2 - y) < 0) {
            return kTRUE;
        }
    }
    return kFALSE;
}

void BmnGemDigitizerTask::Finish() {

    FairRunSim* run = FairRunSim::Instance();

    if (fMakeQA) {
        TFile* output = run->GetOutputFile();
        output->cd();
        toDirectory("QA/GEM/DIGITIZER");
        fHisto->Write();
        gFile->cd();
    }
}

void BmnGemDigitizerTask::DrawDiffGemQA(Int_t iStat, BmnGemDigitizerQAHistograms* fHisto, Float_t X, Float_t Y, Float_t Z) {

    fHisto->X[iStat]->Fill(X);
    fHisto->Y[iStat]->Fill(Y);
    fHisto->XY[iStat]->Fill(X, Y);
}

void BmnGemDigitizerTask::DrawDiffGemQA2(BmnGemDigitizerQAHistograms* fHisto, Int_t iStat, Float_t adc, Int_t strips, Char_t* xystrips) {

    if (xystrips == "X") {
        fHisto->_hADCX->Fill(adc);
        fHisto->ADCX[iStat]->Fill(adc);
        fHisto->_hDigiX->Fill(strips);
        fHisto->DIGIX[iStat]->Fill(strips);
        fHisto->_hDigiAdcX->Fill(strips, adc);
        fHisto->DIGIADCX[iStat]->Fill(strips, adc);
    }

    if (xystrips == "Y") {
        fHisto->_hADCY->Fill(adc);
        fHisto->ADCY[iStat]->Fill(adc);
        fHisto->_hDigiY->Fill(strips);
        fHisto->DIGIY[iStat]->Fill(strips);
        fHisto->_hDigiAdcY->Fill(strips, adc);
        fHisto->DIGIADCY[iStat]->Fill(strips, adc);
    }

}

ClassImp(BmnGemDigitizerTask)
