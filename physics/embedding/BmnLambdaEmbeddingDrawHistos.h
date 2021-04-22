#ifndef BMNEMBDRAW_H
#define BMNEMBDRAW_H 1

#include <TNamed.h>
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TProfile2D.h>
#include <TLegend.h>

#include <BmnInnerTrackerGeometryDraw.h>

using namespace std;

class HistoPlacement : public TNamed {
public:
    HistoPlacement() :
    fPad(-1),
    fSubPad(-1),
    fDet(""),
    fType(-1),
    fStat(-1),
    fMod(-1),
    fName("") {

    }

    HistoPlacement(TString canvName, Int_t pad, Int_t subPad, TString det, Int_t type, Int_t stat, Int_t mod) {
        fPad = pad;
        fSubPad = subPad;
        fDet = det;
        fType = type;
        fStat = stat;
        fMod = mod;
        fName = canvName;
    }
    
    HistoPlacement(TString canvName, Int_t pad, TString det, Int_t stat) :
    fSubPad(-1), 
    fType(-1), 
    fMod(-1) {
        fPad = pad;
        fDet = det;
        fStat = stat;
        fName = canvName;
    }
    
    virtual ~HistoPlacement() {

    }
    
    TString GetCanvasName() {
        return fName;
    }
    
    TString GetDetectorName() {
        return fDet;
    }
    
    Int_t GetPad() {
        return fPad;
    }
    
    Int_t GetSubpad() {
        return fSubPad;
    }
    
    Int_t GetAnalType() {
        return fType;
    }
    
    Int_t GetStat() {
        return fStat;
    }
    
    Int_t GetMod() {
        return fMod;
    }
    
    Int_t GetZone() {
        if (fSubPad == -1 && fType == -1 && fMod == -1)
            return fStat;
        else 
            return -1;
    }


private:
    Int_t fPad;
    Int_t fSubPad;
    TString fDet;
    TString fName;
    Int_t fType;
    Int_t fStat;
    Int_t fMod;
};

class BmnLambdaEmbeddingDrawHistos : public TNamed {
public:

    BmnLambdaEmbeddingDrawHistos();

    virtual ~BmnLambdaEmbeddingDrawHistos() {

    }

    void ProcessHistos();
    
    // Useful setters ...
    // See explanations below when introducing isActive data member ...
    void SetActive(Int_t currentAnal) {
        isActive[currentAnal] = kTRUE;
    }

    TH1F***** GetGemStripInfo() {
        return hGemStripInfo;
    }

    TH1F***** GetSiliconStripInfo() {
        return hSiliconStripInfo;
    }
    
    TProfile2D* GetSilHitEff() {
        return pEmbSilHitEff;
    }
    
    TProfile2D** GetGemHitEff() {
        return pEmbGemHitEff;
    }
    
    TProfile2D** GetSilHitEff2D() {
        return pEffSilStatXY;    
    }
    
    TProfile2D** GetGemHitEff2D() {
        return pEffGemStatXY;
    }
    
    TH1F*** GetEtaLambda() {
        return hEtaLambda;
    }
    
    TH2F** GetNHitsEmbReco() {
        return hNhitsEmbReco;
    }

private:
    BmnInnerTrackerGeometryDraw* geoms;

    TH1F***** hGemStripInfo; // [pureMc or afterEmbedding][stat][mod][lay] --> strip number
    TH1F***** hSiliconStripInfo; // [pureMc or afterEmbedding][stat][mod][lay] --> strip number
    
    TProfile2D* pEmbSilHitEff;
    TProfile2D** pEmbGemHitEff;
    
    TProfile2D** pEffGemStatXY;
    TProfile2D** pEffSilStatXY;
    
    TH1F*** hEtaLambda;
    
    TH2F** hNhitsEmbReco;
    
    

    // List of active anals.
    /*
     * 0 - BmnLambdaEmbeddingQa::DoDigiAnalysis()
     * 1 - BmnLambdaEmbeddingQa::DoEffAnalysis() 
     */
    Bool_t isActive[2];

    ClassDef(BmnLambdaEmbeddingDrawHistos, 1)
};

#endif