/* */

#ifndef BMNGEMDIGITIZERTASK_HH
#define BMNGEMDIGITIZERTASK_HH

#include "FairTask.h"
// #include "FairMCTrack.h"
#include "FairField.h"
#include "CbmStsPoint.h"
#include "BmnGemGeo.h"
#include "BmnGemGas.h"
#include "BmnGemDigitizerQAHistograms.h"

using namespace std;

struct DigOrigArray {
    Float_t signal;
    map<Int_t, Float_t> origins; // array of pairs <MC-track ID, its signal> to calculate origin
    Bool_t isOverlap;
};

class BmnGemDigitizerTask : public FairTask {
public:

    // Constructors/Destructors ---------
    BmnGemDigitizerTask();
    virtual ~BmnGemDigitizerTask();

    void SetPersistence(Bool_t opt = kFALSE) { fPersistence = opt; }
    void SetAttach(Bool_t opt = kFALSE) { fAttach = opt; }
    void SetDiffuse(Bool_t opt = kFALSE) { fDiffuse = opt; }
    void SetDistort(Bool_t opt = kFALSE) { fDistort = opt; }
    void SetDebug(Bool_t opt = kFALSE) { fPrintDebugInfo = opt; }
    void SetDistribute(Bool_t opt = kFALSE) { fDistribute = opt; }
    void SetResponse(Bool_t opt = kFALSE) { fResponse = opt; }
    void SetMakeQA(Bool_t opt = kFALSE) { fMakeQA = opt; }
   
    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();
        
    void GetArea(Float_t av_coord, Float_t radius, vector<Int_t> &strip, Float_t width, Int_t num);

    void GemProcessing(const CbmStsPoint* point);
    
    Bool_t isSubtrackInInwards(const CbmStsPoint* p1, const CbmStsPoint* p2);
    
    void StripResponse(Float_t x, Float_t y, Int_t iStat, Int_t origin, DigOrigArray** arrX, DigOrigArray** arrY);
    
    Float_t CalculateStripResponse(Int_t iStrip, Float_t x, Float_t min, Float_t width);
    
    void CalculateAmplitude(vector<Int_t> &strips, Float_t av_coord, DigOrigArray** arr, Int_t iStat, Int_t origin, Float_t min, Float_t width);
    
    void DrawDiffGemQA(Int_t iStat, BmnGemDigitizerQAHistograms* fHisto, Float_t X, Float_t Y, Float_t Z);
    
    void DrawDiffGemQA2(BmnGemDigitizerQAHistograms* fHisto, Int_t iStat, Float_t adc, Int_t strips, Char_t* xystrips);
    
    void FillDigiArrays(Int_t iStat, Float_t num, DigOrigArray** digi_input, TClonesArray* digi_output, BmnGemDigitizerQAHistograms* fHisto, Char_t* xystrips);

private:

    // Private Data Members ------------
    TString fInputBranchName;
    TString fOutputBranchName1;            // Array with X-digits
    TString fOutputBranchName2;            // Array with Y-digits 
    
    TClonesArray* fMCPointArray;           // input array of MC points
    TClonesArray* fMCTracksArray;          // input array of MC tracks
    
    TClonesArray* fDigitsX;                // output array of GEM X-digits (write into output tree)
    TClonesArray* fDigitsY;                // output array of GEM Y-digits (write into output tree)
   
    DigOrigArray** fDigitsArrayX;          // temporary array of digital signals from X-strips (don't write into output tree)
    DigOrigArray** fDigitsArrayY;          // temporary array of digital signals from Y-strips (don't write into output tree)
    
    BmnGemGeo* fGemParam;                   // object for getting geometrical parameters of GEM
  
    FairField* fMagField;                 // magnetic field
    
    Float_t fGain;                        // coefficient for avalanches calculating
    BmnGemGas* fGas;                      // pointer to BmnGemGas class
    
    BmnGemDigitizerQAHistograms *fHisto;  // pointer to object needed only for QA creating
   
    Float_t fNoiseThreshold;              // threshold for signal separation
   
    Int_t fStation;                      // number of GEM station
    
    Float_t fSpread;                      // sigma for pad response function    
    Float_t k1, k2;                       // coefficients for padRespose calculating    
       
    // set of boolean flags for manage of work process 
  
    Bool_t fPersistence;             // print or not output array into tree
    Bool_t fAttach;                  // attach electrons in gas or not
    Bool_t fDiffuse;                 // diffuse electrons in GEM  or not
    Bool_t fDistort;                 // not implemented yet
    Bool_t fResponse;                // to do pad response or not
    Bool_t fDistribute;              // distribute electrons between two MC points or not
    Bool_t fPrintDebugInfo;          // print or not additional information in output
    
    Bool_t fIsHistogramsInitialized; // is QA histograms initialized or not
    Bool_t fMakeQA;                  // create or not in output tree branch with QA histograms

public:
    ClassDef(BmnGemDigitizerTask, 1)

};

#endif


//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
