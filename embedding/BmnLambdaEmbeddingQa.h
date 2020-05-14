#include <iostream>
#include <fstream>

#include <TNamed.h>
#include <TString.h>
#include <TSystem.h>
#include <TPolyLine.h>
#include <TBox.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <TGeoManager.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TMarker.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TColor.h>
#include <TStyle.h>
#include <TLegend.h>

#include <BmnGlobalTrack.h>
#include <BmnGemStripHit.h>
#include <BmnSiliconHit.h>
#include <CbmStsPoint.h>
#include <CbmMCTrack.h>
#include <BmnKalmanFilter.h>
#include <BmnGemStripStationSet.h>
#include <BmnSiliconStationSet.h>
#include <BmnLambdaEmbeddingMonitor.h>
#include <BmnLambdaMisc.h>
#include <BmnNewFieldMap.h>
#include <DstEventHeader.h>
#include <BmnEventHeader.h>
#include <BmnStripDigit.h>
#include <UniDbDetectorParameter.h>
#include <UniDbRun.h>
#include <BmnInnerTrackerGeometryDraw.h>

#include <FairRunAna.h>
#include <FairTrackParam.h>
#include <FairMCPoint.h>

#ifndef BMNEMBQA_H
#define BMNEMBQA_H 1

using namespace std;

class BmnLambdaEmbeddingQa : public TNamed {
public:

    BmnLambdaEmbeddingQa();
    BmnLambdaEmbeddingQa(TString);

    virtual ~BmnLambdaEmbeddingQa();

    // Types of analysis available ...
    void DoDigiMcAnalysis();
    void DoDigiEmbeddedAnalysis();
    void DoDrawEventsWithEmbeddedSignals(); // Draw events with embedded hits from Lambda decay products 
    void DoInnerTrackerEmbeddingEfficiency(); // Calculate efficiency of embedding 
    void DoInnerTrackerRecoEfficiency(); // Calculate some params. showing hit reconstruction and tracking efficiency

    void DoDrawFoundTracks(Bool_t flag) {
        drawFoundTracks = flag;
    }
    
    // Setting prefix name if needed ...
    void SetPrefixName(TString name) {
        fPrefix = name;
    }
    
    void ShowLays(Int_t lay1, Int_t lay2) {
        aLaysToShow[0] = lay1;
        aLaysToShow[1] = lay2;
    }
    
    void SetSignalCuts(Double_t min, Double_t max) {
        fSignalWindow[0] = min;
        fSignalWindow[1] = max;
    }
    
    void DrawHistos6(TClonesArray*, TClonesArray*); // Function to draw histos from BmnLambdaMisc::CheckStripOverlaps(), temporarily solution 

private:
    Int_t nInputs;

    FairRunAna* fRunAna;
    
    // Geometries
    BmnInnerTrackerGeometryDraw* geoms;

    // Files
    TString* fDigi;
    TString* fDst;
    TString* fEmb;
    TString* fPath;

    // Pointers to be addressed to corresponding objects if an analysis we do uses them
    BmnFieldMap* fMagField;
    BmnLambdaEmbeddingMonitor* fMon;
    DstEventHeader* hDst;
    BmnEventHeader* hDigi;
    TClonesArray* fGemDigits;
    TClonesArray* fSiliconDigits;
    TClonesArray* fGemHits;
    TClonesArray* fSiliconHits;
    TClonesArray* fSiliconTracks;
    TClonesArray* fGlobTracks;
    TClonesArray* fMcTracks;
    TClonesArray* fSiliconPoints;
    TClonesArray* fGemPoints;

    // Containers to be used for analysis 
    map <Int_t, Double_t> fSilGemZ; // <stat, Z-position map> 
    // Maps to store <stat, (xy)-coordinates> for GEM and SILICON (MC--> 0, RECO --> 1)
    map <Int_t, vector <pair <Double_t, Double_t>>> gems[2];
    map <Int_t, vector <pair <Double_t, Double_t>>> silicons[2];

    // Histos to be shown 
    TH2F* hXzMC;
    TH2F* hXzReco;
    TH2F* hYzMC;
    TH2F* hYzReco;
    TH2F** hGemXYProfiles;
    TH2F** hSiliconXYProfiles;
    TH2F* hXzRecoFromTracks;
    
    TProfile*** pEmbSilHitEff;
    TProfile**** pEmbGemHitEff;
    
    TH1F* hProtonMomentaEmb;
    TH1F* hPionMomentaEmb;

    TH1F* hProtonMomentaReco;
    TH1F* hPionMomentaReco;

    TH1F* hProtonNhitsEmb;
    TH1F* hPionNhitsEmb;

    TH1F* hProtonNhitsReco;
    TH1F* hPionNhitsReco;
    
    TH1F* hProtonNhitsRecoAll;
    TH1F* hPionNhitsRecoAll;
    
    TProfile* pEffProton;
    TProfile* pEffPion;
    
    TProfile2D** pEffGemStatXY;
    
    TH1F***** hGemStripInfo; // [events with just embedded digits (0) or without embedding (1)][stat][mod][lay] --> strip number
    Int_t aLaysToShow[2]; // array of strips lays to be shown in histos
    
    TH2F**** hStripChannel; // Strip vs. Chan for main part of big zone and common ADC [main, common][stat][Right or Left]
    
    Double_t fSignalWindow[2]; // set signal window

    // Options to be reset by user if necessary 
    Bool_t drawFoundTracks;
    
private:

    Int_t DefineSiliconStatByZpoint(Double_t);
    Int_t DefineSiliconModuleByStatAndXY(TBox***, Int_t, Double_t, Double_t);
    void DrawFoundTracks();

    Int_t GetStripBoundValues(TString, Int_t&, Int_t&);

    // Additional prefix, if necessary, in output file names...
    TString fPrefix;
    
    void DrawHistos1(); // Histos taken from DoDrawEventsWithEmbeddedSignals-analysis
    void DrawHistos2(); // Histos taken from DoInnerTrackerEmbeddingEfficiency-analysis
    void DrawHistos3(); // Histos taken from DoInnerTrackerRecoEfficiency-analysis
    void DrawHistos4(); // histos taken from DoDigiEmbeddedAnalysis-analysis
    void DrawHistos5(); // histos taken from DoDigiMcAnalysis-analysis
    
    ClassDef(BmnLambdaEmbeddingQa, 1)
};

#endif