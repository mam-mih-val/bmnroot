
/** BmnGlobalTracking.cxx
 * \author Sergey Merts <Sergey.Merts@gmail.com>
 * @since 2014
 * @version 1.0
 **/

#include <Fit/FitResult.h>

#include "BmnGlobalTracking.h"
#include "TH1F.h"
#include "BmnEventHeader.h"
#include "BmnMwpcGeometry.h"
#include "FitWLSQ.h"
using namespace TMath;
//============================================Histo
TString fTestFlnm;
TList fList;
TH1D *GEMXRMS;
TH1D *GEMYRMS;
TH1D *DCHXRMS;
TH1D *DCHYRMS;
TH1D *GEMDCHResidX;
TH1D *GEMDCHResidY;
TH1D *GEMDCHResidX1;
TH1D *GEMDCHResidY1;
TH1D *GEMDCHResidX2;
TH1D *GEMDCHResidY2;
TH1D *GEMDCHResidX3;
TH1D *GEMDCHResidY3;
TH1D *GEMDCHResidX4;
TH1D *GEMDCHResidY4;
TH1D *GEMDCHResidX5;
TH1D *GEMDCHResidY5;
TH1D *GEMDCHResidX6;
TH1D *GEMDCHResidY6;
TH1D *GEMDCHResidX7;
TH1D *GEMDCHResidY7;
TH1D *TOF1DCHResidX;
TH1D *TOF1DCHResidY;
TH1D *TOF2DCHResidX;
TH1D *TOF2DCHResidY;
TH1D *TOF1DCHResidX1;
TH1D *TOF1DCHResidY1;
TH1D *TOF2DCHResidX1;
TH1D *TOF2DCHResidY1;
TH1D *MWPCDCHResidX;
TH1D *MWPCDCHResidY;
TH1D *NumberOfGemTracks;
TH1D *NumberOfDCHTracks;
TH1D *NumberOfMCTracks;
TH1D *NumberOfTOF1Hits;
TH1D *NumberOfTOF2Hits;
TH1D *LostFit;
TH1D *DCHNHits;
TH1D *GEMNHits;
const Bool_t histoOutput = false;
//=================================================

//some variables for efficiency calculation
static Float_t workTime = 0.0;
//-----------------------------------------
vector<BmnDchTrack*> dchTracks;
vector<BmnGemTrack*> gemTracks;

const Float_t thresh = 0.7; // threshold for efficiency calculation (70%)

BmnGlobalTracking::BmnGlobalTracking() :
fDetConf(31), //31 means that all detectors are presented
fMcTracks(NULL),
fGemTracks(NULL),
fGemHits(NULL),
fSilHits(NULL),
fGemVertex(NULL),
fTof1Hits(NULL),
fTof2Hits(NULL),
fDchHits(NULL),
fGlobalTracks(NULL),
fGemMcPoints(NULL),
fTof1McPoints(NULL),
fTof2McPoints(NULL),
fDchMcPoints(NULL),
fMCTracks(NULL),
fBmnMPWCPointsArray(NULL),
fBmnTOFPointsArray(NULL),
fBmnTOF1PointsArray(NULL),
fBmnDchPointsArray(NULL),
fEvHead(NULL),
fIsField(kTRUE),
fPDG(2212),
fChiSqCut(100.),
fVertex(NULL),
fEventNo(0) {
    //TODO: Histo Init
    expData = false;
    if (histoOutput) {
        fTestFlnm = "test.BmnGlobalTracking.root";
        GEMXRMS = new TH1D("GEMXRMS", "GEM xoz fit rms", 200, -0.001, 0.1);
        GEMYRMS = new TH1D("GEMYRMS", "GEM yoz fit rms", 200, -0.001, 0.1);
        DCHXRMS = new TH1D("DCHXRMS", "DCH xoz fit rms", 200, -0.001, 0.05);
        DCHYRMS = new TH1D("DCHYRMS", "DCH yoz fit rms", 200, -0.001, 0.05);
        GEMDCHResidX = new TH1D("GEMDCHResidX", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY = new TH1D("GEMDCHResidY", "GEM DCH match yoz residuals", 10000, -100., 100.);
        GEMDCHResidX1 = new TH1D("GEMDCHResidX1", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY1 = new TH1D("GEMDCHResidY1", "GEM DCH match yoz residuals", 10000, -100., 100.);
        GEMDCHResidX2 = new TH1D("GEMDCHResidX2", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY2 = new TH1D("GEMDCHResidY2", "GEM DCH match yoz residuals", 10000, -100., 100.);
        GEMDCHResidX3 = new TH1D("GEMDCHResidX3", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY3 = new TH1D("GEMDCHResidY3", "GEM DCH match yoz residuals", 10000, -100., 100.);
        GEMDCHResidX4 = new TH1D("GEMDCHResidX4", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY4 = new TH1D("GEMDCHResidY4", "GEM DCH match yoz residuals", 10000, -100., 100.);
        GEMDCHResidX5 = new TH1D("GEMDCHResidX5", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY5 = new TH1D("GEMDCHResidY5", "GEM DCH match yoz residuals", 10000, -100., 100.);
        GEMDCHResidX6 = new TH1D("GEMDCHResidX6", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY6 = new TH1D("GEMDCHResidY6", "GEM DCH match yoz residuals", 10000, -100., 100.);
        GEMDCHResidX7 = new TH1D("GEMDCHResidX7", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY7 = new TH1D("GEMDCHResidY7", "GEM DCH match yoz residuals", 10000, -100., 100.);
        TOF1DCHResidX = new TH1D("TOF1DCHResidX", "TOF1 DCH match xoz residuals", 10000, -100., 100.);
        TOF1DCHResidY = new TH1D("TOF1DCHResidY", "TOF1 DCH match yoz residuals", 10000, -100., 100.);
        TOF2DCHResidX = new TH1D("TOF2DCHResidX", "TOF2 DCH match xoz residuals", 10000, -100., 100.);
        TOF2DCHResidY = new TH1D("TOF2DCHResidY", "TOF2 DCH match yoz residuals", 10000, -100., 100.);
        TOF1DCHResidX1 = new TH1D("TOF1DCHResidX1", "TOF1 DCH match xoz residuals", 10000, -100., 100.);
        TOF1DCHResidY1 = new TH1D("TOF1DCHResidY1", "TOF1 DCH match yoz residuals", 10000, -100., 100.);
        TOF2DCHResidX1 = new TH1D("TOF2DCHResidX1", "TOF2 DCH match xoz residuals", 10000, -100., 100.);
        TOF2DCHResidY1 = new TH1D("TOF2DCHResidY1", "TOF2 DCH match yoz residuals", 10000, -100., 100.);
        MWPCDCHResidX = new TH1D("MWPCDCHResidX", "MWPC DCH match xoz residuals", 10000, -100., 100.);
        MWPCDCHResidY = new TH1D("MWPCDCHResidY", "MWPC DCH match yoz residuals", 10000, -100., 100.);
        NumberOfGemTracks = new TH1D("NumberOfGemTracks", "Number of GEM traks per event", 10, 0., 10.);
        NumberOfDCHTracks = new TH1D("NumberOfDCHTracks", "Number of DCH tracks per event", 100, 0., 100.);
        NumberOfMCTracks = new TH1D("NumberOfMCTracks", "Number of MC tracks per event", 100, 0., 100.);
        NumberOfTOF1Hits = new TH1D("NumberOfTOF1Hits", "Number of TOF1 hits per event", 50, 0., 50.);
        NumberOfTOF2Hits = new TH1D("NumberOfTOF2Hits", "Number of TOF2 hits per event", 50, 0., 50.);
        LostFit = new TH1D("LostFit", "LostFit", 4, 0., 3.);
        DCHNHits = new TH1D("DCHNHits", "Number of DCH hits per event", 30, 0., 30.);
        GEMNHits = new TH1D("GEMNHits", "Number of GEM hits per event", 30, 0., 30.);
        fList.Add(GEMDCHResidX);
        fList.Add(GEMDCHResidY);
        fList.Add(GEMDCHResidX1);
        fList.Add(GEMDCHResidY1);
        fList.Add(GEMDCHResidX2);
        fList.Add(GEMDCHResidY2);
        fList.Add(GEMDCHResidX3);
        fList.Add(GEMDCHResidY3);
        fList.Add(GEMDCHResidX4);
        fList.Add(GEMDCHResidY4);
        fList.Add(GEMDCHResidX5);
        fList.Add(GEMDCHResidY5);
        fList.Add(GEMDCHResidX6);
        fList.Add(GEMDCHResidY6);
        fList.Add(GEMDCHResidX7);
        fList.Add(GEMDCHResidY7);
        fList.Add(TOF1DCHResidX);
        fList.Add(TOF1DCHResidY);
        fList.Add(TOF2DCHResidX);
        fList.Add(TOF2DCHResidY);
        fList.Add(TOF1DCHResidX1);
        fList.Add(TOF1DCHResidY1);
        fList.Add(TOF2DCHResidX1);
        fList.Add(TOF2DCHResidY1);
        fList.Add(MWPCDCHResidX);
        fList.Add(MWPCDCHResidY);
        fList.Add(NumberOfGemTracks);
        fList.Add(NumberOfDCHTracks);
        fList.Add(NumberOfMCTracks);
        fList.Add(NumberOfTOF1Hits);
        fList.Add(NumberOfTOF2Hits);
        fList.Add(GEMXRMS);
        fList.Add(GEMYRMS);
        fList.Add(DCHXRMS);
        fList.Add(DCHYRMS);
        fList.Add(LostFit);
        fList.Add(DCHNHits);
        fList.Add(GEMNHits);
    }
}

BmnGlobalTracking::BmnGlobalTracking(Bool_t isExp) :
expData(isExp),
fDetConf(31), //31 means that all detectors are presented
fMcTracks(NULL),
fGemTracks(NULL),
fGemHits(NULL),
fGemVertex(NULL),
fTof1Hits(NULL),
fTof2Hits(NULL),
fDchHits(NULL),
fGlobalTracks(NULL),
fGemMcPoints(NULL),
fTof1McPoints(NULL),
fTof2McPoints(NULL),
fDchMcPoints(NULL),
fMCTracks(NULL),
fBmnMPWCPointsArray(NULL),
fBmnTOFPointsArray(NULL),
fBmnTOF1PointsArray(NULL),
fBmnDchPointsArray(NULL),
fEvHead(NULL),
fPDG(2212),
fChiSqCut(100.),
fVertex(NULL),
fEventNo(0) {
    if (histoOutput) {
        fTestFlnm = "test.BmnGlobalTracking.root";
        GEMXRMS = new TH1D("GEMXRMS", "GEM xoz fit rms", 200, -0.001, 0.1);
        GEMYRMS = new TH1D("GEMYRMS", "GEM yoz fit rms", 200, -0.001, 0.1);
        DCHXRMS = new TH1D("DCHXRMS", "DCH xoz fit rms", 200, -0.001, 0.05);
        DCHYRMS = new TH1D("DCHYRMS", "DCH yoz fit rms", 200, -0.001, 0.05);
        GEMDCHResidX = new TH1D("GEMDCHResidX", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY = new TH1D("GEMDCHResidY", "GEM DCH match yoz residuals", 10000, -100., 100.);
        GEMDCHResidX1 = new TH1D("GEMDCHResidX1", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY1 = new TH1D("GEMDCHResidY1", "GEM DCH match yoz residuals", 10000, -100., 100.);
        GEMDCHResidX2 = new TH1D("GEMDCHResidX2", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY2 = new TH1D("GEMDCHResidY2", "GEM DCH match yoz residuals", 10000, -100., 100.);
        GEMDCHResidX3 = new TH1D("GEMDCHResidX3", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY3 = new TH1D("GEMDCHResidY3", "GEM DCH match yoz residuals", 10000, -100., 100.);
        GEMDCHResidX4 = new TH1D("GEMDCHResidX4", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY4 = new TH1D("GEMDCHResidY4", "GEM DCH match yoz residuals", 10000, -100., 100.);
        GEMDCHResidX5 = new TH1D("GEMDCHResidX5", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY5 = new TH1D("GEMDCHResidY5", "GEM DCH match yoz residuals", 10000, -100., 100.);
        GEMDCHResidX6 = new TH1D("GEMDCHResidX6", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY6 = new TH1D("GEMDCHResidY6", "GEM DCH match yoz residuals", 10000, -100., 100.);
        GEMDCHResidX7 = new TH1D("GEMDCHResidX7", "GEM DCH match xoz residuals", 10000, -100., 100.);
        GEMDCHResidY7 = new TH1D("GEMDCHResidY7", "GEM DCH match yoz residuals", 10000, -100., 100.);
        TOF1DCHResidX = new TH1D("TOF1DCHResidX", "TOF1 DCH match xoz residuals", 900, -3., 3.);
        TOF1DCHResidY = new TH1D("TOF1DCHResidY", "TOF1 DCH match yoz residuals", 900, -3., 3.);
        TOF2DCHResidX = new TH1D("TOF2DCHResidX", "TOF2 DCH match xoz residuals", 900, -3., 3.);
        TOF2DCHResidY = new TH1D("TOF2DCHResidY", "TOF2 DCH match yoz residuals", 900, -3., 3.);
        TOF1DCHResidX1 = new TH1D("TOF1DCHResidX1", "TOF1 DCH match xoz residuals", 900, -3., 3.);
        TOF1DCHResidY1 = new TH1D("TOF1DCHResidY1", "TOF1 DCH match yoz residuals", 900, -3., 3.);
        TOF2DCHResidX1 = new TH1D("TOF2DCHResidX1", "TOF2 DCH match xoz residuals", 900, -3., 3.);
        TOF2DCHResidY1 = new TH1D("TOF2DCHResidY1", "TOF2 DCH match yoz residuals", 900, -3., 3.);
        MWPCDCHResidX = new TH1D("MWPCDCHResidX", "MWPC DCH match xoz residuals", 900, -3., 3.);
        MWPCDCHResidY = new TH1D("MWPCDCHResidY", "MWPC DCH match yoz residuals", 900, -3., 3.);
        NumberOfGemTracks = new TH1D("NumberOfGemTracks", "Number of GEM traks per event", 10, 0., 10.);
        NumberOfDCHTracks = new TH1D("NumberOfDCHTracks", "Number of DCH tracks per event", 100, 0., 100.);
        NumberOfMCTracks = new TH1D("NumberOfMCTracks", "Number of MC tracks per event", 100, 0., 100.);
        NumberOfTOF1Hits = new TH1D("NumberOfTOF1Hits", "Number of TOF1 hits per event", 50, 0., 50.);
        NumberOfTOF2Hits = new TH1D("NumberOfTOF2Hits", "Number of TOF2 hits per event", 50, 0., 50.);
        LostFit = new TH1D("LostFit", "LostFit", 4, 0., 3.);
        DCHNHits = new TH1D("DCHNHits", "Number of DCH hits per event", 30, 0., 30.);
        GEMNHits = new TH1D("GEMNHits", "Number of GEM hits per event", 30, 0., 30.);
        fList.Add(GEMDCHResidX);
        fList.Add(GEMDCHResidY);
        fList.Add(GEMDCHResidX1);
        fList.Add(GEMDCHResidY1);
        fList.Add(GEMDCHResidX2);
        fList.Add(GEMDCHResidY2);
        fList.Add(GEMDCHResidX3);
        fList.Add(GEMDCHResidY3);
        fList.Add(GEMDCHResidX4);
        fList.Add(GEMDCHResidY4);
        fList.Add(GEMDCHResidX5);
        fList.Add(GEMDCHResidY5);
        fList.Add(GEMDCHResidX6);
        fList.Add(GEMDCHResidY6);
        fList.Add(GEMDCHResidX7);
        fList.Add(GEMDCHResidY7);
        fList.Add(TOF1DCHResidX);
        fList.Add(TOF1DCHResidY);
        fList.Add(TOF2DCHResidX);
        fList.Add(TOF2DCHResidY);
        fList.Add(TOF1DCHResidX1);
        fList.Add(TOF1DCHResidY1);
        fList.Add(TOF2DCHResidX1);
        fList.Add(TOF2DCHResidY1);
        fList.Add(MWPCDCHResidX);
        fList.Add(MWPCDCHResidY);
        fList.Add(NumberOfGemTracks);
        fList.Add(NumberOfDCHTracks);
        fList.Add(NumberOfMCTracks);
        fList.Add(NumberOfTOF1Hits);
        fList.Add(NumberOfTOF2Hits);
        fList.Add(GEMXRMS);
        fList.Add(GEMYRMS);
        fList.Add(DCHXRMS);
        fList.Add(DCHYRMS);
        fList.Add(LostFit);
        fList.Add(DCHNHits);
        fList.Add(GEMNHits);
    }
}

BmnGlobalTracking::~BmnGlobalTracking() {
}

InitStatus BmnGlobalTracking::Init() {
    if (fVerbose) cout << "BmnGlobalTracking::Init started\n";

    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        Fatal("Init", "FairRootManager is not instantiated");
    }

    fGemHitArray = (TClonesArray*) ioman->GetObject("BmnGemStripHit"); //in
    if (!expData) {
        fDchTracks = (TClonesArray*) ioman->GetObject("BmnDchTrack");
        fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
        fBmnGemPointsArray = (TClonesArray*) ioman->GetObject("DCHPoint");
        fBmnDchPointsArray = (TClonesArray*) ioman->GetObject("StsPoint");
        fBmnMPWCPointsArray = (TClonesArray*) ioman->GetObject("MWPCPoint");
        fBmnTOFPointsArray = (TClonesArray*) ioman->GetObject("TOFPoint");
        fBmnTOF1PointsArray = (TClonesArray*) ioman->GetObject("TOF1Point");
    }


    fDet.DetermineSetup();
    if (fVerbose) cout << fDet.ToString();

    // MWPC
    if (fDet.GetDet(kMWPC)) {
        fMwpcHits = (TClonesArray*) ioman->GetObject("BmnMwpcHit");
        if (!fMwpcHits)
            if (fVerbose)
                cout << "Init. No BmnMwpcHit array!" << endl;
        fMwpcTracks = (TClonesArray*) ioman->GetObject("BmnMwpcTrack");
        if (!fMwpcTracks)
            if (fVerbose)
                cout << "Init. No BmnMwpcTrack array!" << endl;
    }

    // SILICON
    fSilHits = (TClonesArray*) ioman->GetObject("BmnSiliconHit");
    if (!fSilHits)
        if (fVerbose)
            cout << "Init. No BmnSiliconHit array!" << endl;

    // GEM
//    if (fDet.GetDet(kGEM)) {
        fGemHits = (TClonesArray*) ioman->GetObject("BmnGemStripHit");
        if (!fGemHits)
            if (fVerbose)
                cout << "Init. No BmnGemStripHit array!" << endl;
        fGemTracks = (TClonesArray*) ioman->GetObject("BmnGemTrack");
        if (!fGemTracks) {
            cout << "BmnGlobalTracking::Init(): branch " << "BmnGemTrack" << " not found! Task will be deactivated" << endl;
            SetActive(kFALSE);
            return kERROR;
        }
//    }

    // Vertex
    fGemVertex = (TClonesArray*) ioman->GetObject("BmnVertex");
    if (!fGemVertex)
        if (fVerbose)
            cout << "Init. No BmnVertex array!" << endl;

    // TOF1
    if (fDet.GetDet(kTOF1)) {
        fTof1Hits = (TClonesArray*) ioman->GetObject("BmnTof1Hit");
        if (!fTof1Hits)
            if (fVerbose)
                cout << "Init. No BmnTof1Hit array!" << endl;
    }

    // TOF2
    if (fDet.GetDet(kTOF)) {
        fTof2Hits = (TClonesArray*) ioman->GetObject("BmnTofHit");
        if (!fTof2Hits)
            if (fVerbose)
                cout << "Init. No BmnTof2Hit array!" << endl;
    }

    // DCH
    if (fDet.GetDet(kDCH)) {
        fDchTracks = (TClonesArray*) ioman->GetObject("BmnDchTrack");
        if (!fDchTracks)
            if (fVerbose)
                cout << "Init. No BmnDchTrack array!" << endl;
        fDchHits = (TClonesArray*) ioman->GetObject("BmnDchHit");
        if (!fDchHits)
            if (fVerbose)
                cout << "Init. No BmnDchHit array!" << endl;
    }

    fEvHead = (TClonesArray*) ioman->GetObject("EventHeader");
    if (!fEvHead)
        if (fVerbose) cout << "Init. No EventHeader array!" << endl;

    // Create and register track arrays
    fGlobalTracks = new TClonesArray("BmnGlobalTrack", 100);
    ioman->Register("BmnGlobalTrack", "GLOBAL", fGlobalTracks, kTRUE);

    if (fVerbose) cout << "BmnGlobalTracking::Init finished\n";
    return kSUCCESS;
}

BmnStatus BmnGlobalTracking::MatchGemDCH(BmnGlobalTrack* tr) {
    BmnGemTrack* gemTrack = (BmnGemTrack*) fGemTracks->At(tr->GetGemTrackIndex());
    Double_t z = 305.0;
    Double_t dx = 1000;
    Double_t dy = 1000;
    Double_t dxMC = 1000;
    Double_t dyMC = 1000;
    Int_t minIdx = -1;
    Int_t minIdxMC = -1;
    if (gemTrack->parabolaParameters.size() != 3)
        return kBMNERROR;
    if (gemTrack->lineParameters.size() != 2)
        return kBMNERROR;
    for (Int_t trIdx = 0; trIdx < fDchTracks->GetEntriesFast(); ++trIdx) {
        BmnDchTrack* dchTr = (BmnDchTrack*) fDchTracks->At(trIdx);
        if (dchTr->xozParameters.size() != 2)
            continue;
        if (dchTr->yozParameters.size() != 2)
            continue;
        Double_t x1 = gemTrack->parabolaParameters[2] * z * z + gemTrack->parabolaParameters[1] * z + gemTrack->parabolaParameters[0];
        Double_t y1 = gemTrack->lineParameters[1] * z + gemTrack->lineParameters[0];
        Double_t x2 = dchTr->xozParameters[1] * z + dchTr->xozParameters[0];
        Double_t y2 = dchTr->yozParameters[1] * z + dchTr->yozParameters[0];
        if (Abs(x1 - x2) < dx && Abs(y1 - y2) < dy) {
            minIdx = trIdx;
            dx = Abs(x1 - x2);
            dy = Abs(y1 - y2);
        }
    }
    if (!expData && dchTracks.size() > 0) {
        for (Int_t iTrack = 0; iTrack < dchTracks.size(); iTrack++) {
            Double_t x1 = gemTrack->parabolaParameters[2] * z * z + gemTrack->parabolaParameters[1] * z + gemTrack->parabolaParameters[0];
            Double_t y1 = gemTrack->lineParameters[1] * z + gemTrack->lineParameters[0];
            Double_t x2 = dchTracks[iTrack]->xozParameters[1] * z + dchTracks[iTrack]->xozParameters[0];
            Double_t y2 = dchTracks[iTrack]->yozParameters[1] * z + dchTracks[iTrack]->yozParameters[0];
            if (Abs(x1 - x2) < dxMC && Abs(y1 - y2) < dyMC) {
                minIdxMC = iTrack;
                dxMC = Abs(x1 - x2);
                dyMC = Abs(y1 - y2);
            }
        }
    }
    if (minIdx != -1) {
        BmnDchTrack* dchTr = (BmnDchTrack*) fDchTracks->At(minIdx);
        Double_t x1 = gemTrack->parabolaParameters[2] * z * z + gemTrack->parabolaParameters[1] * z + gemTrack->parabolaParameters[0];
        Double_t y1 = gemTrack->lineParameters[1] * z + gemTrack->lineParameters[0];
        Double_t x2 = dchTr->xozParameters[1] * z + dchTr->xozParameters[0];
        Double_t y2 = dchTr->yozParameters[1] * z + dchTr->yozParameters[0];
        if (Abs(x2 - x1) < 3 && Abs(y2 - y1) < 3) {
            if (histoOutput) {
                GEMDCHResidX->Fill((x1 - x2));
                GEMDCHResidY->Fill((y1 - y2));
            }
            BmnFitNode *node = &((tr->GetFitNodes()).at(3));
            tr->SetDchTrackIndex(minIdx);
        }
        if (histoOutput) {
            GEMDCHResidX1->Fill((x1 - x2));
            GEMDCHResidY1->Fill((y1 - y2));
        }
    }
    if (minIdxMC != -1) {
        BmnDchTrack* dchTr = (BmnDchTrack*) dchTracks[minIdxMC];
        Double_t x1 = gemTrack->parabolaParameters[2] * z * z + gemTrack->parabolaParameters[1] * z + gemTrack->parabolaParameters[0];
        Double_t y1 = gemTrack->lineParameters[1] * z + gemTrack->lineParameters[0];
        Double_t x2 = dchTr->xozParameters[1] * z + dchTr->xozParameters[0];
        Double_t y2 = dchTr->yozParameters[1] * z + dchTr->yozParameters[0];
        if (histoOutput) {
            if (Abs(x2 - x1) < 3 && Abs(y2 - y1) < 3) {
                GEMDCHResidX2->Fill((x1 - x2));
                GEMDCHResidY2->Fill((y1 - y2));
            }
            GEMDCHResidX3->Fill((x1 - x2));
            GEMDCHResidY3->Fill((y1 - y2));
        }
    }
    return kBMNSUCCESS;
}
//AM 3.08.2017

BmnStatus BmnGlobalTracking::MatchDCHTOF(BmnGlobalTrack* tr, Int_t num) {
    Double_t dx = 1000;
    Double_t dy = 1000;
    Int_t minIdx = -1;
    if (!expData) {
        if (tr->GetDchTrackIndex() == -1)
            return kBMNERROR;
        BmnDchTrack* dchTr = (BmnDchTrack*) fDchTracks->At(tr->GetDchTrackIndex());
        if (dchTr->xozParameters.size() != 2)
            return kBMNERROR;
        if (dchTr->yozParameters.size() != 2)
            return kBMNERROR;

        for (Int_t iMCTrack = 0; iMCTrack < fMCTracks->GetEntriesFast(); iMCTrack++) {
            if (num == 2) {
                minIdx = -1;
                dx = 1000;
                dy = 1000;
                for (Int_t iPoint = 0; iPoint < fBmnTOFPointsArray->GetEntriesFast(); iPoint++) {
                    FairMCPoint* hit = (FairMCPoint*) fBmnTOFPointsArray->At(iPoint);
                    if (hit->GetTrackID() == iMCTrack) {
                        Double_t x2 = dchTr->xozParameters[1] * hit->GetZ() + dchTr->xozParameters[0];
                        Double_t y2 = dchTr->yozParameters[1] * hit->GetZ() + dchTr->yozParameters[0];
                        if (Abs(hit->GetX() - x2) < dx && Abs(hit->GetY() - y2) < dy) {
                            minIdx = iPoint;
                            dx = Abs(hit->GetX() - x2);
                            dy = Abs(hit->GetY() - y2);
                        }
                    }
                }
                if (minIdx != -1) {
                    tr->SetTof2HitIndex(minIdx);
                    FairMCPoint* hit = (FairMCPoint*) fBmnTOFPointsArray->At(minIdx);
                    Double_t x2 = dchTr->xozParameters[1] * hit->GetZ() + dchTr->xozParameters[0];
                    Double_t y2 = dchTr->yozParameters[1] * hit->GetZ() + dchTr->yozParameters[0];
                    if (Abs(hit->GetX() - x2) < 1 && Abs(hit->GetY() - y2) < 1) {
                        if (histoOutput) {
                            TOF2DCHResidX->Fill(hit->GetX() - x2);
                            TOF2DCHResidY->Fill(hit->GetY() - y2);
                        }
                    }
                }
            }
            if (num == 1) {
                minIdx = -1;
                dx = 1000;
                dy = 1000;
                for (Int_t iPoint = 0; iPoint < fBmnTOF1PointsArray->GetEntriesFast(); iPoint++) {
                    FairMCPoint* hit = (FairMCPoint*) fBmnTOF1PointsArray->At(iPoint);
                    if (hit->GetTrackID() == iMCTrack) {
                        Double_t x2 = dchTr->xozParameters[1] * hit->GetZ() + dchTr->xozParameters[0];
                        Double_t y2 = dchTr->yozParameters[1] * hit->GetZ() + dchTr->yozParameters[0];
                        if (Abs(hit->GetX() - x2) < dx && Abs(hit->GetY() - y2) < dy) {
                            minIdx = iPoint;
                            dx = Abs(hit->GetX() - x2);
                            dy = Abs(hit->GetY() - y2);
                        }
                    }
                }
                if (minIdx != -1) {
                    tr->SetTof1HitIndex(minIdx);
                    FairMCPoint* hit = (FairMCPoint*) fBmnTOF1PointsArray->At(minIdx);
                    Double_t x2 = dchTr->xozParameters[1] * hit->GetZ() + dchTr->xozParameters[0];
                    Double_t y2 = dchTr->yozParameters[1] * hit->GetZ() + dchTr->yozParameters[0];
                    if (Abs(hit->GetX() - x2) < 1 && Abs(hit->GetY() - y2) < 1) {
                        if (histoOutput) {
                            TOF1DCHResidX->Fill(hit->GetX() - x2);
                            TOF1DCHResidY->Fill(hit->GetY() - y2);
                        }
                    }
                }
            }
        }
        return kBMNSUCCESS;
    }
    //==============================================================================================================
    minIdx = -1;
    if (tr->GetDchTrackIndex() == -1)
        return kBMNERROR;
    BmnDchTrack* dchTr = (BmnDchTrack*) fDchTracks->At(tr->GetDchTrackIndex());
    TClonesArray* tofHits = (num == 1 && fTof1Hits) ? fTof1Hits : (num == 2 && fTof2Hits) ? fTof2Hits : NULL;
    if (!tofHits)
        return kBMNERROR;
    dx = 1000;
    dy = 1000;
    for (Int_t hitIdx = 0; hitIdx < tofHits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*) tofHits->At(hitIdx);
        Double_t x2 = dchTr->xozParameters[1] * hit->GetZ() + dchTr->xozParameters[0];
        Double_t y2 = dchTr->yozParameters[1] * hit->GetZ() + dchTr->yozParameters[0];
        if (Abs(hit->GetX() - x2) < dx && Abs(hit->GetY() - y2) < dy) {
            minIdx = hitIdx;
            dx = Abs(hit->GetX() - x2);
            dy = Abs(hit->GetY() - y2);
        }
    }
    if (minIdx != -1) {
        if (num == 1)
            tr->SetTof1HitIndex(minIdx);
        if (num == 2)
            tr->SetTof2HitIndex(minIdx);
        BmnHit* hit = (BmnHit*) tofHits->At(minIdx);
        Double_t x2 = dchTr->xozParameters[1] * hit->GetZ() + dchTr->xozParameters[0];
        Double_t y2 = dchTr->yozParameters[1] * hit->GetZ() + dchTr->yozParameters[0];
        if (histoOutput) {
            if (num == 1) {
                TOF1DCHResidX->Fill(hit->GetX() - x2);
                TOF1DCHResidY->Fill(hit->GetY() - y2);
            }
            if (num == 2) {
                TOF2DCHResidX->Fill(hit->GetX() - x2);
                TOF2DCHResidY->Fill(hit->GetY() - y2);
            }
        }
    }
    return kBMNSUCCESS;
}
//AM 3.08.2017

BmnStatus BmnGlobalTracking::MatchDCHMPWC(BmnGlobalTrack* tr) {
    Double_t dx = 1000;
    Double_t dy = 1000;
    Int_t minIdx = -1;
    BmnDchTrack* dchTr = (BmnDchTrack*) fDchTracks->At(tr->GetDchTrackIndex());
    if (!expData) {
        for (Int_t iMCTrack = 0; iMCTrack < fMCTracks->GetEntriesFast(); iMCTrack++) {
            for (Int_t iPoint = 0; iPoint < fBmnMPWCPointsArray->GetEntriesFast(); iPoint++) {
                FairMCPoint* hit = (FairMCPoint*) fBmnMPWCPointsArray->At(iPoint);
                if (hit->GetTrackID() == iMCTrack) {
                    Double_t x2 = dchTr->xozParameters[1] * hit->GetZ() + dchTr->xozParameters[0];
                    Double_t y2 = dchTr->yozParameters[1] * hit->GetZ() + dchTr->yozParameters[0];
                    if (Abs(hit->GetX() - x2) < dx && Abs(hit->GetY() - y2) < dy) {
                        minIdx = iPoint;
                        dx = Abs(hit->GetX() - x2);
                        dy = Abs(hit->GetY() - y2);
                    }
                }
            }
            if (minIdx != -1) {
                FairMCPoint* hit = (FairMCPoint*) fBmnMPWCPointsArray->At(minIdx);
                Double_t x2 = dchTr->xozParameters[1] * hit->GetZ() + dchTr->xozParameters[0];
                Double_t y2 = dchTr->yozParameters[1] * hit->GetZ() + dchTr->yozParameters[0];
                if (histoOutput) {
                    MWPCDCHResidX->Fill(hit->GetX() - x2);
                    MWPCDCHResidY->Fill(hit->GetY() - y2);
                }
            }
        }
    }
    if (!fMwpcTracks) {
        return kBMNERROR;
    }
    for (Int_t trIdx = 0; trIdx < fMwpcTracks->GetEntriesFast(); ++trIdx) {
        BmnTrack* mwpcTr = (BmnTrack*) fMwpcTracks->At(trIdx);
    }
    return kBMNSUCCESS;
}

void FitDCHTrack(std::vector<Double_t> x, std::vector<Double_t> y, std::vector<Double_t> z) {
    double xx[x.size()];
    double yy[x.size()];
    double zz[x.size()];
    std::copy(x.begin(), x.end(), xx);
    std::copy(y.begin(), y.end(), yy);
    std::copy(z.begin(), z.end(), zz);
    FitWLSQ *fitx = new FitWLSQ(zz, 0.015, 0.09, 0.9, (int) x.size(), 2, false, false, 6);
    FitWLSQ *fity = new FitWLSQ(zz, 0.015, 0.09, 0.9, (int) x.size(), 2, false, false, 6);
    if (histoOutput)
        DCHNHits->Fill(x.size());
    BmnDchTrack* track = new BmnDchTrack();
    Bool_t fitX = false;
    Bool_t fitY = false;
    if (fitx->Fit(xx)) {
        fitX = true;
        track->xozParameters.push_back(fitx->param[0]);
        track->xozParameters.push_back(fitx->param[1]);
        for (Int_t i = 0; i < 2; i++)
            for (Int_t j = 0; j < 2; j++)
                track->covXOZ[i][j] = fitx->cov[i][j];
    } else
        if (histoOutput)
        LostFit->Fill(2.);
    if (fity->Fit(yy)) {
        fitY = true;
        track->yozParameters.push_back(fity->param[0]);
        track->yozParameters.push_back(fity->param[1]);
        for (Int_t i = 0; i < 2; i++)
            for (Int_t j = 0; j < 2; j++)
                track->covYOZ[i][j] = fity->cov[i][j];
    } else
        if (histoOutput)
        LostFit->Fill(3.);
    if (fitX && fitY) {
        if (histoOutput) {
            DCHXRMS->Fill(fitx->WLSQRms(xx));
            DCHYRMS->Fill(fity->WLSQRms(yy));
        }
        dchTracks.push_back(track);
    }
    delete fitx;
    delete fity;
}

void BmnGlobalTracking::FitDCHTracks() {
    //TODO: времянка по монте-карло хита только для теста сшивки !!!???
    //==============================================================================================
    if (!expData) {
        dchTracks.clear();

        if (!fBmnDchPointsArray->GetEntriesFast())// no points in DCH
            return;

        for (Int_t iMCTrack = 0; iMCTrack < fMCTracks->GetEntriesFast(); iMCTrack++) {
            Int_t nHits = 0;
            std::vector<Double_t> x;
            std::vector<Double_t> y;
            std::vector<Double_t> z;

            for (Int_t iPoint = 0; iPoint < fBmnDchPointsArray->GetEntriesFast(); iPoint++) {
                FairMCPoint* pnt = (FairMCPoint*) fBmnDchPointsArray->At(iPoint);
                if (pnt->GetTrackID() == iMCTrack) {
                    x.push_back(pnt->GetX());
                    y.push_back(pnt->GetY());
                    z.push_back(pnt->GetZ());
                }
            }
            if (x.size() > 5) {
                FitDCHTrack(x, y, z);
            }
        }
    }
    //==============================================================================================
}
//AM 3.08.2017

BmnGemStripHit * BmnGlobalTracking::GetGemHit(Int_t i) {
    BmnGemStripHit* hit = (BmnGemStripHit*) fGemHitArray->At(i);
    if (!hit) return NULL;
    return hit;
}
//AM 3.08.2017

void BmnGlobalTracking::FitGemTracks() {
    //==============================================================================================
    if (!expData) {
        gemTracks.clear();

        if (!fBmnGemPointsArray->GetEntriesFast())// no points in DCH
            return;

        for (Int_t iMCTrack = 0; iMCTrack < fMCTracks->GetEntriesFast(); iMCTrack++) {
            Int_t nHits = 0;
            std::vector<Double_t> x;
            std::vector<Double_t> y;
            std::vector<Double_t> z;

            for (Int_t iPoint = 0; iPoint < fBmnGemPointsArray->GetEntriesFast(); iPoint++) {
                FairMCPoint* pnt = (FairMCPoint*) fBmnGemPointsArray->At(iPoint);
                if (pnt->GetTrackID() == iMCTrack) {
                    x.push_back(pnt->GetX());
                    y.push_back(pnt->GetY());
                    z.push_back(pnt->GetZ());
                }
            }
            if (x.size() > 5) {
                Bool_t fitX = false;
                Bool_t fitY = false;
                double *xx = new double[z.size()];
                double *yy = new double[z.size()];
                double *zz = new double[z.size()];
                std::copy(x.begin(), x.end(), xx);
                std::copy(y.begin(), y.end(), yy);
                std::copy(z.begin(), z.end(), zz);
                FitWLSQ *fit = new FitWLSQ(zz, 0.015, 0.09, 0.9, (int) z.size(), 3, false, false, 6);
                FitWLSQ *fits = new FitWLSQ(zz, 0.015, 0.09, 0.9, (int) z.size(), 2, false, false, 6);
                BmnGemTrack* track = new BmnGemTrack();
                if (fit->Fit(xx)) {
                    fitX = true;
                    track->parabolaParameters.push_back(fit->param[0]);
                    track->parabolaParameters.push_back(fit->param[1]);
                    track->parabolaParameters.push_back(fit->param[2]);
                    for (Int_t i = 0; i < 3; i++)
                        for (Int_t j = 0; j < 3; j++)
                            track->covP[i][j] = fit->cov[i][j];
                }
                if (fits->Fit(yy)) {
                    fitY = true;
                    track->lineParameters.push_back(fits->param[0]);
                    track->lineParameters.push_back(fits->param[1]);
                    for (Int_t i = 0; i < 2; i++)
                        for (Int_t j = 0; j < 2; j++)
                            track->covL[i][j] = fits->cov[i][j];
                }
                if (fitX && fitY)
                    gemTracks.push_back(track);

                delete fit;
                delete fits;
                delete[] xx;
                delete[] yy;
                delete[] zz;
            }
        }
    }
    //==============================================================================================
    for (Int_t iTr = 0; iTr < fGemTracks->GetEntriesFast(); ++iTr) {
        BmnGemTrack* track = (BmnGemTrack*) fGemTracks->At(iTr);

        if (track->GetChi2() < 0.0) continue; //split param

        BmnGemTrack tr = *track;
        const Short_t nHits = tr.GetNHits();
        if (nHits < 5)
            continue;
        if (histoOutput)
            GEMNHits->Fill(nHits);

        double *xx = new double[nHits];
        double *yy = new double[nHits];
        double *zz = new double[nHits];

        for (Int_t iHit = 0; iHit < nHits; ++iHit) {
            BmnGemStripHit* hit = (BmnGemStripHit*) GetGemHit(tr.GetHitIndex(iHit));
            zz[iHit] = hit->GetZ();
            xx[iHit] = hit->GetX();
            yy[iHit] = hit->GetY();
        }
        FitWLSQ *fit = new FitWLSQ(zz, 0.015, 0.09, 0.9, (int) nHits, 3, false, false, 6);
        FitWLSQ *fits = new FitWLSQ(zz, 0.015, 0.09, 0.9, (int) nHits, 2, false, false, 6);

        Bool_t fitX = false;
        Bool_t fitY = false;

        //parabola
        if (fit->Fit(xx)) {
            fitX = true;
            track->parabolaParameters.push_back(fit->param[0]);
            track->parabolaParameters.push_back(fit->param[1]);
            track->parabolaParameters.push_back(fit->param[2]);
            for (Int_t i = 0; i < 3; i++)
                for (Int_t j = 0; j < 3; j++)
                    track->covP[i][j] = fit->cov[i][j];
            /*
            cout << "GEM XOZ fit finished: " << fit->param[0] << " " << fit->param[1] << " " << fit->param[2] << endl;
        for (Int_t iHit = 0; iHit < nHits; ++iHit) {
            cout << iHit << " "<< xx[iHit]<< " " << zz[iHit]<< " " 
                                << fit->param[2]*zz[iHit]*zz[iHit]+fit->param[1]*zz[iHit] + fit->param[0]<< endl;
        }
             */
        } else
            if (histoOutput)
            LostFit->Fill(0.);
        if (fits->Fit(yy)) {
            fitY = true;
            track->lineParameters.push_back(fits->param[0]);
            track->lineParameters.push_back(fits->param[1]);
            for (Int_t i = 0; i < 2; i++)
                for (Int_t j = 0; j < 2; j++)
                    track->covL[i][j] = fits->cov[i][j];
            /*
            cout << "GEM YOZ fit finished: " << fits->param[0] << " " << fits->param[1] << endl;
        for (Int_t iHit = 0; iHit < nHits; ++iHit) {
            cout << iHit << " "<< yy[iHit]<< " " << zz[iHit]<< " " 
                                << fits->param[1]*zz[iHit] + fits->param[0]<< endl;
        }
             */
        } else
            if (histoOutput)
            LostFit->Fill(1.);
        if (fitX && fitY) {
            if (histoOutput) {
                GEMYRMS->Fill(fits->WLSQRms(yy));
                GEMXRMS->Fill(fit->WLSQRms(xx));
            }
        }
        delete fit;
        delete fits;
        delete[] xx;
        delete[] yy;
        delete[] zz;
    }
}

void BmnGlobalTracking::Exec(Option_t* opt) {

    if (!IsActive())
        return;

    if (fVerbose) cout << "\n======================== Global tracking exec started =====================\n" << endl;
    fEventNo++;
    clock_t tStart = clock();
    fGlobalTracks->Delete();

    //    CreateDchHitsFromTracks();

    if (!fGemTracks) return;

    if (fGemVertex) {
        if (fGemVertex->GetEntriesFast() > 0)
            fVertex = (CbmVertex*) fGemVertex->At(0);
        else
            fVertex = NULL;
    }

    // Map to be used for collecting GEM-tracks and corresponding SI-points
    map <Double_t, pair<Int_t, Int_t> > silDist;

    for (Int_t i = 0; i < fGemTracks->GetEntriesFast(); ++i) {
        BmnGemTrack* gemTrack = (BmnGemTrack*) fGemTracks->At(i);
        new((*fGlobalTracks)[i]) BmnGlobalTrack();
        BmnGlobalTrack* glTr = (BmnGlobalTrack*) fGlobalTracks->At(i);
        glTr->SetParamFirst(*(gemTrack->GetParamFirst()));
        glTr->SetParamLast(*(gemTrack->GetParamLast()));
        glTr->SetGemTrackIndex(i);
        glTr->SetNHits(gemTrack->GetNHits());
        glTr->SetNDF(gemTrack->GetNDF());
        glTr->SetChi2(gemTrack->GetChi2());
        glTr->SetLength(gemTrack->GetLength());

        if (fSilHits)
            CalcSiliconDist(glTr, silDist);

        //vector<BmnFitNode> nodes(4); //MWPC, TOF1, TOF2 and DCH
        //glTr->SetFitNodes(nodes);

        //MatchingMWPC(glTr);
        MatchingSil(silDist);
        //MatchingTOF(glTr, 1, i);
        //MatchingDCH(glTr);
        //Refit(glTr);
        //MatchGemDCH(glTr);
        //MatchDCHTOF(glTr,1);
        //MatchDCHTOF(glTr,2);
    }

    //CalculateLength();

    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    if (fVerbose) cout << "GLOBAL_TRACKING: Number of merged tracks: " << fGlobalTracks->GetEntriesFast() << endl;
    if (fVerbose) cout << "\n======================== Global tracking exec finished ====================\n" << endl;
}

void BmnGlobalTracking::CalcSiliconDist(BmnGlobalTrack* glTr, map <Double_t, pair<Int_t, Int_t> > &silDist) {
    const Double_t distCut = 1.;
    for (Int_t hitIdx = 0; hitIdx < fSilHits->GetEntriesFast(); hitIdx++) {
        BmnSiliconHit* hit = (BmnSiliconHit*) fSilHits->UncheckedAt(hitIdx);

        FairTrackParam parPredict = *glTr->GetParamFirst();

        BmnKalmanFilter* kalman = new BmnKalmanFilter();
        if (kalman->TGeoTrackPropagate(&parPredict, hit->GetZ(), fPDG, NULL, NULL, fIsField) == kBMNERROR) continue;

        Double_t dist = Sqrt(Power(parPredict.GetX() - hit->GetX(), 2) + Power(parPredict.GetY() - hit->GetY(), 2));
        if (dist > distCut) {
            delete kalman;
            continue;
        }
        silDist.insert(pair <Double_t, pair<Int_t, Int_t> > (dist, make_pair(glTr->GetGemTrackIndex(), hitIdx)));

        glTr->SetParamFirst(parPredict);
        delete kalman;
    }
}

BmnStatus BmnGlobalTracking::MatchingMWPC(BmnGlobalTrack* tr) {

    if (!fMwpcTracks) return kBMNERROR;

    BmnKalmanFilter* kalman = new BmnKalmanFilter();

    Double_t minChiSq = DBL_MAX;
    BmnTrack* minTrack = NULL; // Pointer to the nearest track
    Int_t minIdx = -1;
    FairTrackParam minParUp; // updated track parameters for the closest dch track
    FairTrackParam minParPred; // predicted track parameters for the closest dch track

    for (Int_t trIdx = 0; trIdx < fMwpcTracks->GetEntriesFast(); ++trIdx) {
        BmnTrack* mwpcTr = (BmnTrack*) fMwpcTracks->At(trIdx);
        FairTrackParam parPredict(*(tr->GetParamFirst()));
        kalman->TGeoTrackPropagate(&parPredict, mwpcTr->GetParamLast()->GetZ(), fPDG, NULL, NULL, fIsField);
        FairTrackParam parUpdate = parPredict;
        Double_t chi = 0.0;
        BmnMwpcGeometry geo;
        TVector3 err = TVector3(geo.GetWireStep() / Sqrt(12.0), geo.GetWireStep() / Sqrt(12.0), 1.0 / Sqrt(12.0));
        BmnMwpcHit hit(1, TVector3(mwpcTr->GetParamLast()->GetX(), mwpcTr->GetParamLast()->GetY(), mwpcTr->GetParamLast()->GetZ()), err, 0); //tmp hit for updating track parameters
        kalman->Update(&parUpdate, &hit, chi);
        if (chi < minChiSq) {
            minChiSq = chi;
            minTrack = mwpcTr;
            minParPred = parPredict;
            minParUp = parUpdate;
            minIdx = trIdx;
        }
    }

    if (minTrack != NULL) { // Check if hit was added
        tr->SetParamFirst(minParUp);
        tr->SetChi2(tr->GetChi2() + minChiSq);
        tr->SetMwpcTrackIndex(minIdx);
        tr->SetNHits(tr->GetNHits() + minTrack->GetNHits());
        tr->SetNDF(tr->GetNDF() + minTrack->GetNHits()); //FIXME!
        BmnFitNode *node = &((tr->GetFitNodes()).at(0));
        node->SetUpdatedParam(&minParUp);
        node->SetPredictedParam(&minParPred);
        return kBMNSUCCESS;
    } else {
        return kBMNERROR;
    }

    delete kalman;
}

BmnStatus BmnGlobalTracking::MatchingTOF(BmnGlobalTrack* tr, Int_t num, Int_t trIndex) {

    TClonesArray* tofHits = (num == 1 && fTof1Hits) ? fTof1Hits : (num == 2 && fTof2Hits) ? fTof2Hits : NULL;
    if (!tofHits) return kBMNERROR;

    BmnKalmanFilter* kalman = new BmnKalmanFilter();

    Double_t minChiSq = DBL_MAX;
    Double_t minDist = DBL_MAX;
    BmnHit* minHit = NULL; // Pointer to the nearest hit
    Int_t minIdx = -1;
    Double_t LenPropLast = 0., LenPropFirst = 0.;
    FairTrackParam minParPredLast; // predicted track parameters for closest hit
    for (Int_t hitIdx = 0; hitIdx < tofHits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*) tofHits->At(hitIdx);
        if (hit->IsUsed()) continue; // skip Tof hit which used before
        FairTrackParam parPredict(*(tr->GetParamLast()));
        Double_t len = 0.;
        //printf("hitIdx = %d\n", hitIdx);
        //printf("BEFORE: len = %f.3\t", len);
        //printf("Param->GetX() = %.2f\n", parPredict.GetX());
        BmnStatus resultPropagate = kalman->TGeoTrackPropagate(&parPredict, hit->GetZ(), fPDG, NULL, &len, fIsField);
        if (resultPropagate == kBMNERROR) continue; // skip in case kalman error
        Double_t dist = TMath::Sqrt(TMath::Power(parPredict.GetX() - hit->GetX(), 2) + TMath::Power(parPredict.GetY() - hit->GetY(), 2));
        //printf("AFTER:  len = %.3f\t", len);
        //printf("Param->GetX() = %.2f\t", parPredict.GetX());
        //printf ("Distanc = %.3f\n", dist);
        //getchar();
        if (dist < minDist && dist <= 5.) {
            minDist = dist;
            minHit = hit;
            minParPredLast = parPredict;
            minIdx = hitIdx;
            LenPropLast = len;
        }
    }

    if (minHit != NULL) { // Check if hit was added
        FairTrackParam ParPredFirst(*(tr->GetParamFirst()));
        FairTrackParam ParPredLast(*(tr->GetParamLast()));
        ParPredFirst.SetQp(ParPredLast.GetQp());
        Double_t LenTrack = tr->GetLength();
        Double_t zTarget = -21.7; // z of target by default
        if (fVertex)
            zTarget = fVertex->GetZ();
        BmnStatus resultPropagate = kalman->TGeoTrackPropagate(&ParPredFirst, zTarget, fPDG, NULL, &LenPropFirst, fIsField);
        if (resultPropagate != kBMNERROR) { // skip in case kalman error

            if (num == 1)
                tr->SetTof1HitIndex(minIdx);
            else
                tr->SetTof2HitIndex(minIdx);

            minHit->SetIndex(trIndex);
            //    printf("LenFirst = %.3f;  LenTrack = %.3f;  LenLast = %.3f\n", LenPropFirst, LenTrack, LenPropLast);
            minHit->SetLength(LenPropFirst + LenTrack + LenPropLast); // length from target to Tof hit
            //    printf("Writed length = %.3f\n", minHit->GetLength());
            minHit->SetUsing(kTRUE);
            tr->SetNHits(tr->GetNHits() + 1);

            delete kalman;
            return kBMNSUCCESS;
        } else {
            delete kalman;
            return kBMNERROR;
        }
    } else return kBMNERROR;
}

BmnStatus BmnGlobalTracking::MatchingSil(map <Double_t, pair<Int_t, Int_t> > silDist) {
    for (auto &it : silDist) {
        Int_t trIdx = it.second.first;
        Int_t hitIdx = it.second.second;

        BmnSiliconHit* hit = (BmnSiliconHit*) fSilHits->UncheckedAt(hitIdx);
        if (hit->IsUsed())
            continue;

        BmnGlobalTrack* glTr = (BmnGlobalTrack*) fGlobalTracks->UncheckedAt(trIdx);
        if (glTr->IsUsed())
            continue;

        FairTrackParam ParPredFirst = *glTr->GetParamFirst(); //cout << "Z = " << ParPredFirst.GetZ() << endl;

        FairTrackParam ParPredLast = *glTr->GetParamLast();
        ParPredFirst.SetQp(ParPredLast.GetQp());

        FairTrackParam parUpdate = ParPredFirst;
        Double_t chi = 0.0;

        if (fIsField) {
            BmnKalmanFilter* kalman = new BmnKalmanFilter();
            if (kalman->Update(&parUpdate, (BmnHit*) hit, chi) == kBMNERROR)
                delete kalman;

            else {
                glTr->SetParamFirst(parUpdate);
                glTr->SetChi2(Abs(glTr->GetChi2()) + Abs(chi));
                glTr->SetSilHitIndex(hitIdx);
                glTr->SetNHits(glTr->GetNHits() + 1);
                hit->SetUsing(kTRUE);
                glTr->SetUsing(kTRUE);

                delete kalman;
            }
        } else {
            // if no mag. field, one has to recalculate Tx and Ty manually
            glTr->SetChi2(Abs(glTr->GetChi2()) + Abs(chi));
            glTr->SetSilHitIndex(hitIdx);
            glTr->SetNHits(glTr->GetNHits() + 1);
            hit->SetUsing(kTRUE);
            glTr->SetUsing(kTRUE);

            TGraph XZ;
            TGraph YZ;
            Int_t iPos = 0;
            BmnSiliconHit* hitSi = (BmnSiliconHit*) fSilHits->UncheckedAt(glTr->GetSilHitIndex());
            XZ.SetPoint(iPos, hitSi->GetZ(), hitSi->GetX());
            YZ.SetPoint(iPos, hitSi->GetZ(), hitSi->GetY());
            iPos++;

            BmnGemTrack* trackGem = (BmnGemTrack*) fGemTracks->UncheckedAt(glTr->GetGemTrackIndex());

            for (Int_t iGemHit = 0; iGemHit < trackGem->GetNHits(); iGemHit++) {
                Int_t idx = trackGem->GetHitIndex(iGemHit);
                BmnGemStripHit* hitGem = (BmnGemStripHit*) fGemHits->UncheckedAt(idx);
                XZ.SetPoint(iPos, hitGem->GetZ(), hitGem->GetX());
                YZ.SetPoint(iPos, hitGem->GetZ(), hitGem->GetY());
                iPos++;
            }

            Double_t txToBeSet = XZ.Fit("pol1", "SQww")->Parameter(1);
            Double_t tyToBeSet = YZ.Fit("pol1", "SQww")->Parameter(1);
            glTr->GetParamFirst()->SetTx(txToBeSet);
            glTr->GetParamFirst()->SetTy(tyToBeSet);

            glTr->GetParamLast()->SetTx(txToBeSet);
            glTr->GetParamLast()->SetTy(tyToBeSet);
        }
    }
}

BmnStatus BmnGlobalTracking::CreateDchHitsFromTracks() {
    for (Int_t i = 0; i < fDchTracks->GetEntriesFast(); ++i) {
        BmnDchTrack* dchTr = (BmnDchTrack*) fDchTracks->At(i);
        Float_t x = dchTr->GetParamFirst()->GetX();
        Float_t y = dchTr->GetParamFirst()->GetY();
        Float_t z = dchTr->GetParamFirst()->GetZ();
        new((*fDchHits)[fDchHits->GetEntriesFast()]) BmnDchHit(1, TVector3(x, y, z), TVector3(0, 0, 0), 0);
    }
}

BmnStatus BmnGlobalTracking::MatchingDCH(BmnGlobalTrack* tr) {

    if (!fDchTracks) return kBMNERROR;

    BmnKalmanFilter* kalman = new BmnKalmanFilter();

    Double_t minChiSq = DBL_MAX;
    BmnTrack* minTrack = NULL; // Pointer to the nearest track
    Int_t minIdx = -1;
    FairTrackParam minParUp; // updated track parameters for the closest dch track
    FairTrackParam minParPred; // predicted track parameters for the closest dch track
    for (Int_t trIdx = 0; trIdx < fDchTracks->GetEntriesFast(); ++trIdx) {
        BmnTrack* dchTr = (BmnTrack*) fDchTracks->At(trIdx);
        FairTrackParam parPredict(*(tr->GetParamLast()));
        kalman->TGeoTrackPropagate(&parPredict, dchTr->GetParamFirst()->GetZ(), fPDG, NULL, NULL, fIsField);
        FairTrackParam parUpdate = parPredict;
        Double_t chi;
        BmnDchHit hit(1, TVector3(dchTr->GetParamFirst()->GetX(), dchTr->GetParamFirst()->GetY(), dchTr->GetParamFirst()->GetZ()), TVector3(0.5 / Sqrt(12.0), 0.5 / Sqrt(12.0), 1.0 / Sqrt(12.0)), 0); //tmp hit for updating track parameters
        kalman->Update(&parUpdate, &hit, chi);
        if (chi < minChiSq) {
            minChiSq = chi;
            minTrack = dchTr;
            minParPred = parPredict;
            minParUp = parUpdate;
            minIdx = trIdx;
        }
    }

    if (minTrack != NULL) { // Check if hit was added
        tr->SetParamLast(minParUp);
        tr->SetChi2(tr->GetChi2() + minChiSq);
        tr->SetDchTrackIndex(minIdx);
        tr->SetNHits(tr->GetNHits() + minTrack->GetNHits());
        tr->SetNDF(tr->GetNDF() + minTrack->GetNHits()); //FIXME!
        vector<BmnFitNode> nodes = tr->GetFitNodes();
        BmnFitNode *node = &((tr->GetFitNodes()).at(3));
        node->SetUpdatedParam(&minParUp);
        node->SetPredictedParam(&minParPred);
        return kBMNSUCCESS;
    } else {
        return kBMNERROR;
    }
    delete kalman;
}

BmnStatus BmnGlobalTracking::RefitToDetector(BmnGlobalTrack* tr, Int_t hitId, TClonesArray* hitArr, FairTrackParam* par, Int_t* nodeIdx, vector<BmnFitNode>* nodes) {

    BmnKalmanFilter* kalman = new BmnKalmanFilter();

    if (tr->GetTof2HitIndex() != -1) {
        BmnHit* hit = (BmnHit*) hitArr->At(hitId);
        Float_t Ze = hit->GetZ();
        Double_t length = 0;
        vector<Double_t> F(25);
        if (kalman->TGeoTrackPropagate(par, Ze, 2212, &F, &length, fIsField) == kBMNERROR) {
            tr->SetFlag(kBMNBAD);
            return kBMNERROR;
        }

        nodes->at(*nodeIdx).SetPredictedParam(par);
        nodes->at(*nodeIdx).SetF(F);
        Double_t chi2Hit = 0.;
        if (kalman->Update(par, hit, chi2Hit) == kBMNERROR) {
            tr->SetFlag(kBMNBAD);
            return kBMNERROR;
        }
        //        tr->SetParamLast(par);
        //        tr->SetParamFirst(par);

        nodes->at(*nodeIdx).SetUpdatedParam(par);
        nodes->at(*nodeIdx).SetChiSqFiltered(chi2Hit);
        tr->SetChi2(tr->GetChi2() + chi2Hit);
        tr->SetLength(tr->GetLength() + length);
        (*nodeIdx)--;
    }

    delete kalman;
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::Refit(BmnGlobalTrack* tr) {

    vector<BmnFitNode> nodes(tr->GetNHits());
    Int_t nodeIdx = tr->GetNHits() - 1;
    FairTrackParam par = *(tr->GetParamLast());
    //    FairTrackParam par = *(tr->GetParamFirst());

    //TOF2 part
    //    if (fDet.GetDet(kTOF) && tr->GetTof2HitIndex() != -1 && fTof2Hits) {
    //        if (RefitToDetector(tr, tr->GetTof2HitIndex(), fTof2Hits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    //    }

    //DCH1 part
    //        if (fDet.GetDet(kDCH) && tr->GetDchHitIndex() != -1 && fDchHits) {
    //            if (RefitToDetector(tr, tr->GetDchHitIndex(), fDchHits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    //        }

    //TOF1 part
    if (fDet.GetDet(kTOF1) && tr->GetTof1HitIndex() != -1 && fTof1Hits) {
        if (RefitToDetector(tr, tr->GetTof1HitIndex(), fTof1Hits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    }

    //GEM part
    if (fDet.GetDet(kGEM) && tr->GetGemTrackIndex() != -1) {
        BmnGemTrack* gemTr = (BmnGemTrack*) fGemTracks->At(tr->GetGemTrackIndex());
        for (Int_t i = gemTr->GetNHits() - 1; i >= 0; --i) {
            if (RefitToDetector(tr, i, fGemHits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
        }
    }

    tr->SetParamFirst(par);
    tr->SetFitNodes(nodes);
    return kBMNSUCCESS;
}

void BmnGlobalTracking::Finish() {
    dchTracks.clear();
    //===============================================================================================================
    TFile *ptr = gFile;
    if (histoOutput) {
        FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, "[BmnGlobalTracking::Finish] Update  %s file. ", fTestFlnm.Data());
        TFile file(fTestFlnm.Data(), "RECREATE");
        fList.Write();
        file.Close();
    }
    //===============================================================================================================
    gFile = ptr;

    cout << "Work time of the Global matching: " << workTime << endl;
}

void BmnGlobalTracking::CalculateLength() {
    if (fGlobalTracks == NULL) return;

    /* Calculate the length of the global track
     * starting with (0, 0, 0) and adding all
     * distances between hits
     */
    for (Int_t iTr = 0; iTr < fGlobalTracks->GetEntriesFast(); iTr++) {
        BmnGlobalTrack* glTr = (BmnGlobalTrack*) fGlobalTracks->At(iTr);
        vector<Float_t> X, Y, Z;
        X.push_back(0.);
        Y.push_back(0.);
        Z.push_back(0.);

        if (glTr->GetGemTrackIndex() > -1) {
            if (!isRUN1) {
                const BmnGemTrack* gemTr = (BmnGemTrack*) fGemTracks->At(glTr->GetGemTrackIndex());
                for (Int_t iGem = 0; iGem < gemTr->GetNHits(); iGem++) {
                    const BmnHit* hit = (BmnHit*) fGemHits->At(gemTr->GetHitIndex(iGem));
                    if (!hit) continue;
                    X.push_back(hit->GetX());
                    Y.push_back(hit->GetY());
                    Z.push_back(hit->GetZ());
                }
            }
        }
        if (fDet.GetDet(kTOF1)) {
            if (glTr->GetTof1HitIndex() > -1 && fTof1Hits) {
                const BmnHit* hit = (BmnHit*) fTof1Hits->At(glTr->GetTof1HitIndex());
                if (!hit) continue;
                X.push_back(hit->GetX());
                Y.push_back(hit->GetY());
                Z.push_back(hit->GetZ());
            }
        }
        if (fDet.GetDet(kDCH)) {
            //            if (glTr->GetDch1HitIndex() > -1 && fDch1Hits) {
            //                const BmnHit* hit = (BmnHit*) fDch1Hits->At(glTr->GetDch1HitIndex());
            //                if (!hit) continue;
            //                X.push_back(hit->GetX());
            //                Y.push_back(hit->GetY());
            //                Z.push_back(hit->GetZ());
            //            }
        }
        if (fDet.GetDet(kTOF)) {
            if (glTr->GetTof2HitIndex() > -1 && fTof2Hits) {
                const BmnHit* hit = (BmnHit*) fTof2Hits->At(glTr->GetTof2HitIndex());
                if (!hit) continue;
                X.push_back(hit->GetX());
                Y.push_back(hit->GetY());
                Z.push_back(hit->GetZ());
            }
        }
        // Calculate distances between hits
        Float_t length = 0.;
        for (Int_t i = 0; i < X.size() - 1; i++) {
            Float_t dX = X[i] - X[i + 1];
            Float_t dY = Y[i] - Y[i + 1];
            Float_t dZ = Z[i] - Z[i + 1];
            length += Sqrt(dX * dX + dY * dY + dZ * dZ);
        }
        glTr->SetLength(length);
    }
}

Float_t BmnGlobalTracking::Sqr(Float_t x) {
    return x * x;
}

ClassImp(BmnGlobalTracking);
