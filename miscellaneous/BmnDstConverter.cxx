#include "BmnDstConverter.h"

BmnDstConverter::BmnDstConverter() :
fInnHitsIn(nullptr),
fTracksIn(nullptr),
fGemHitsOut(nullptr),
fSilHitsOut(nullptr),
fVertexOut(nullptr),
fGemTracksOut(nullptr),
fSilTracksOut(nullptr),
fGlobTracksOut(nullptr) {
    fEventNo = 0;

    // Input branches
    fBranchInnHitsIn = "StsHit";
    fBranchVertexIn = "PV";

    fBranchTracksIn = "StsTrack";

    // Output branches
    fBranchGemHitsOut = "BmnGemStripHit";
    fBranchSilHitsOut = "BmnSiliconHit";
    fBranchVertexOut = "BmnVertex";

    fBranchGemTracksOut = "BmnGemTrack";
    fBranchSilTracksOut = "BmnSiliconTrack";
    fBranchGlobTracksOut = "BmnGlobalTrack";

    fBranchEventHeaderOut = "EventHeader";
}

BmnDstConverter::~BmnDstConverter() {


}

InitStatus BmnDstConverter::Init() {
    cout << " BmnDstConverter::Init() " << endl;

    ioman = FairRootManager::Instance();
    fInnHitsIn = (TClonesArray*) ioman->GetObject(fBranchInnHitsIn.Data());
    fTracksIn = (TClonesArray*) ioman->GetObject(fBranchTracksIn.Data());
    fVertexIn = (CbmVertex*) ioman->GetObject(fBranchVertexIn.Data());

    fGemHitsOut = new TClonesArray(fBranchGemHitsOut.Data());
    ioman->Register(fBranchGemHitsOut.Data(), "GEM_", fGemHitsOut, kTRUE);

    fSilHitsOut = new TClonesArray(fBranchSilHitsOut.Data());
    ioman->Register(fBranchSilHitsOut.Data(), "SIL_", fSilHitsOut, kTRUE);

    fVertexOut = new TClonesArray("CbmVertex");
    ioman->Register(fBranchVertexOut.Data(), "VERTEX_", fVertexOut, kTRUE);

    fGemTracksOut = new TClonesArray(fBranchGemTracksOut.Data());
    ioman->Register(fBranchGemTracksOut.Data(), "GEMTRACKS_", fGemTracksOut, kTRUE);

    fSilTracksOut = new TClonesArray(fBranchSilTracksOut.Data());
    ioman->Register(fBranchSilTracksOut.Data(), "SILTRACKS_", fSilTracksOut, kTRUE);

    fGlobTracksOut = new TClonesArray(fBranchGlobTracksOut.Data());
    ioman->Register(fBranchGlobTracksOut.Data(), "GLOBTRACKS_", fGlobTracksOut, kTRUE);

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString confSi = "SiliconRunSpring2018.xml";
    TString confGem = "GemRunSpring2018.xml";

    // SI
    TString gPathSiliconConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
    fDetectorSI = new BmnSiliconStationSet(gPathSiliconConfig + confSi);

    // GEM
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
    fDetectorGEM = new BmnGemStripStationSet(gPathGemConfig + confGem);

    Int_t* statsGem = new Int_t[fDetectorGEM->GetNStations()];
    Int_t* statsSil = new Int_t[fDetectorSI->GetNStations()];

    Int_t* statsGemPermut = new Int_t[fDetectorGEM->GetNStations()];
    Int_t* statsSilPermut = new Int_t[fDetectorSI->GetNStations()];

    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++) {
        statsGem[iStat] = -1;
        statsGemPermut[iStat] = -1;
    }

    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++) {
        statsSil[iStat] = -1;
        statsSilPermut[iStat] = -1;
    }

    Run7(statsGem, statsSil, statsGemPermut, statsSilPermut);

    // Fill corr. maps ...
    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
        fGemStats[statsGem[iStat]] = statsGemPermut[iStat];

    for (Int_t iStat = 0; iStat < fDetectorSI->GetNStations(); iStat++)
        fSilStats[statsSil[iStat]] = statsSilPermut[iStat];

    delete statsGem;
    delete statsGemPermut;
    delete statsSil;
    delete statsSilPermut;

    return kSUCCESS;
}

void BmnDstConverter::Exec(Option_t* opt) {
    if (fEventNo % 1000 == 0)
        cout << "Ev# " << fEventNo << endl;

    fGemHitsOut->Delete();
    fSilHitsOut->Delete();
    fVertexOut->Delete();

    fGemTracksOut->Delete();
    fSilTracksOut->Delete();
    fGlobTracksOut->Delete();

    // Covert GEM and SILICON hits ...
    // RUN7 (FIXME): 1 2 3 (SIL) ---> 4 5 6 7 8 9 (GEM)
    const Double_t nThresh = 3.5;
    for (Int_t iHit = 0; iHit < fInnHitsIn->GetEntriesFast(); iHit++) {
        CbmStsHit* innHit = (CbmStsHit*) fInnHitsIn->UncheckedAt(iHit);

        Int_t statOrig = innHit->GetStationNr();
        Int_t modOrig = innHit->GetSectorNr();

        Bool_t isSilHit = (statOrig > nThresh) ? kFALSE : kTRUE;

        if (isSilHit) {
            Int_t stat = SiliconStatPermutation(statOrig);

            BmnSiliconHit* hit = new ((*fSilHitsOut)[fSilHitsOut->GetEntriesFast()]) BmnSiliconHit();
            hit->SetStation(stat);
            hit->SetModule(modOrig - 1);
            hit->SetXYZ(innHit->GetX(), innHit->GetY(), innHit->GetZ());
            hit->SetDxyz(innHit->GetDx(), innHit->GetDy(), innHit->GetDz());
            hit->SetCovXY(innHit->GetCovXY());
        } else {
            // GEM hit
            Int_t stat = GemStatPermutation(innHit->GetStationNr());
            Int_t mod = GemModPermutation(statOrig, modOrig);

            BmnGemStripHit* hit = new ((*fGemHitsOut)[fGemHitsOut->GetEntriesFast()]) BmnGemStripHit();
            hit->SetStation(stat);
            hit->SetModule(mod);
            hit->SetXYZ(innHit->GetX(), innHit->GetY(), innHit->GetZ());
            hit->SetDxyz(innHit->GetDx(), innHit->GetDy(), innHit->GetDz());
            hit->SetCovXY(innHit->GetCovXY());
        }
    }

    // Convert GEM and SILICON tracks ...
    for (Int_t iTrack = 0; iTrack < fTracksIn->GetEntriesFast(); iTrack++) {
        CbmStsTrack* innTrack = (CbmStsTrack*) fTracksIn->UncheckedAt(iTrack);
        // innTrack->GetParamFirst();

        BmnGlobalTrack* track = new ((*fGlobTracksOut)[fGlobTracksOut->GetEntriesFast()]) BmnGlobalTrack();
        track->SetParamFirst(*innTrack->GetParamFirst());
        track->SetParamLast(*innTrack->GetParamLast());

        track->SetChi2(innTrack->GetChi2());
        track->SetNDF(innTrack->GetNDF());

        track->SetNHits(innTrack->GetNStsHits());
    }

    // Convert primary vertex Vp ...
    CbmVertex* vertex = new ((*fVertexOut)[fVertexOut->GetEntriesFast()]) CbmVertex();
    TMatrixFSym matrix(3);
    fVertexIn->CovMatrix(matrix);
    vertex->SetVertex(fVertexIn->GetX(), fVertexIn->GetY(), fVertexIn->GetZ(), fVertexIn->GetChi2(), fVertexIn->GetNDF(), fVertexIn->GetNTracks(), matrix);


    fEventNo++;
}

void BmnDstConverter::Finish() {
    ioman->GetOutTree()->SetName("cbmsim");
}

void BmnDstConverter::Run7(Int_t* statsGem, Int_t* statsSil, Int_t* statsGemPermut, Int_t* statsSilPermut) {
    statsSil[0] = 1;
    statsSil[1] = 2;
    statsSil[2] = 3;

    statsGem[0] = 4;
    statsGem[1] = 5;
    statsGem[2] = 6;
    statsGem[3] = 7;
    statsGem[4] = 8;
    statsGem[5] = 9;

    statsSilPermut[0] = 0;
    statsSilPermut[1] = 1;
    statsSilPermut[2] = 2;

    statsGemPermut[0] = 0;
    statsGemPermut[1] = 1;
    statsGemPermut[2] = 2;
    statsGemPermut[3] = 3;
    statsGemPermut[4] = 4;
    statsGemPermut[5] = 5;

    // GEM mods permutations ...
    const Int_t nStats = fDetectorGEM->GetNStations();
    const Int_t nMods = 4;
    Int_t modsOrig[nStats][nMods] = {
        {1, 2, 3, 4},
        {1, 2, 3, 4},
        {1, 2, 3, 4},
        {1, 2, 3, 4},
        {1, 2, 3, 4},
        {1, 2, 3, 4}
    };
    Int_t modsPerm[nStats][nMods] = {
        {1, 1, 0, 0},
        {0, 0, 1, 1},
        {0, 0, 1, 1},
        {1, 1, 0, 0},
        {0, 0, 1, 1},
        {1, 1, 0, 0}
    };

    for (Int_t iStat = 0; iStat < fDetectorGEM->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < nMods; iMod++)
            fGemMods[pair <Int_t, Int_t> (statsGem[iStat], modsOrig[iStat][iMod])] = modsPerm[iStat][iMod];
}

