#include "BmnQaOffline.h"
#include "BmnGlobalTrack.h"
#include <UniRun.h>

Int_t BmnQaOffline::fCurrentEvent = 0;

BmnQaOffline::BmnQaOffline(TString file) :
fBmnHeader(nullptr),
fDstHeader(nullptr),
fChainDst(nullptr),
fSiliconHits(nullptr),
fSiliconTracks(nullptr),
fGemHits(nullptr),
fGemTracks(nullptr),
fVertex(nullptr),
fGlobalTracks(nullptr),
fMwpcTracks(nullptr),
fCscHits(nullptr),
fTof400Hits(nullptr),
fTof700Hits(nullptr),
fDchHits(nullptr),
fMwpcHits(nullptr),
fDchTracks(nullptr),
fDetGem(nullptr),
fDetSilicon(nullptr),
fDetCsc(nullptr),
period(0),
fRunId(0),
fSteering(new BmnOfflineQaSteering()) {
    isDstRead = ReadDstTree(file);
}

Bool_t BmnQaOffline::ReadDstTree(TString fileDst) {
    fChainDst = new TChain("bmndata");
    fChainDst->Add(fileDst.Data());

    // Get dst header ...
    fChainDst->SetBranchAddress("DstEventHeader.", &fDstHeader);
    fChainDst->GetEntry(0);

    fChainDst->SetBranchAddress("BmnDchHit", &fDchHits);
    fChainDst->SetBranchAddress("BmnMwpcHit", &fMwpcHits);
    fChainDst->SetBranchAddress("BmnTof400Hit", &fTof400Hits);
    fChainDst->SetBranchAddress("BmnTof700Hit", &fTof700Hits);
    fChainDst->SetBranchAddress("BmnCSCHit", &fCscHits);
    fChainDst->SetBranchAddress("BmnSiliconHit", &fSiliconHits);
    fChainDst->SetBranchAddress("BmnSiliconTrack", &fSiliconTracks);
    fChainDst->SetBranchAddress("BmnGemStripHit", &fGemHits);
    fChainDst->SetBranchAddress("BmnGemTrack", &fGemTracks);
    fChainDst->SetBranchAddress("BmnDchTrack", &fDchTracks);
    fChainDst->SetBranchAddress("BmnMwpcTrack", &fMwpcTracks);
    fChainDst->SetBranchAddress("BmnGlobalTrack", &fGlobalTracks);
    fChainDst->SetBranchAddress("BmnVertex", &fVertex);

    if (fileDst.IsNull() || fChainDst->GetEntries() == 0)
        return kFALSE;
    else
        return kTRUE;
}

InitStatus BmnQaOffline::Init() {
    cout << " BmnQaOffline::Init() " << endl;

    ioman = FairRootManager::Instance();

    fBmnHeader = static_cast<BmnEventHeader*>(ioman->GetObject("BmnEventHeader."));
    if (fBmnHeader){
        printf("BmnEventHeader. found\n");
        fRunId = fBmnHeader->GetRunId();
    } else
    {
    fDstHeader = static_cast<DstEventHeader*>(ioman->GetObject("DstEventHeader."));
        printf("DstEventHeader. found\n");
        fRunId = fDstHeader->GetRunId();
        
    }
    // Get general info on period and exp. setup, detectors and trigger been
    period = fSteering->GetRunAndSetupByRunId(fRunId).first;
    setup = fSteering->GetRunAndSetupByRunId(fRunId).second;
    nDets = fSteering->GetDetectors(period, setup).size(); // Number of detectors should be set for a certain run / setup extension
    Int_t nTrigs = fSteering->GetTriggers(period, setup).size(); // Number of triggers should be set for a certain run / setup extension

    prefix = TString::Format("RUN%d_SETUP_%s_", period, setup.Data());

    // Initialize some detector geometries ... 
    fSteering->SetGeometriesByRunId(fRunId, fDetGem, fDetSilicon, fDetCsc);

    // Read input arrays with det. and trig. info ...
    DETECTORS = new TClonesArray*[nDets];
    TRIGGERS = new TClonesArray*[nTrigs];

    for (Int_t iDet = 0; iDet < nDets; iDet++)
        DETECTORS[iDet] = (TClonesArray*) ioman->GetObject(fSteering->GetDetectors(period, setup)[iDet].Data());

    for (Int_t iTrigger = 0; iTrigger < nTrigs; iTrigger++)
        TRIGGERS[iTrigger] = (TClonesArray*) ioman->GetObject(fSteering->GetTriggers(period, setup)[iTrigger].Data());

    nCoordinate = fSteering->GetNumberOfDets(period, setup, "coordinate");
    nTime = fSteering->GetNumberOfDets(period, setup, "time");
    nCalorimeter = fSteering->GetNumberOfDets(period, setup, "calorimeter");

    coordinate = new BmnCoordinateDetQa*[nCoordinate];
    for (Int_t iDet = 0; iDet < nCoordinate; iDet++)
        coordinate[iDet] = new BmnCoordinateDetQa(fSteering->GetDetectors(period, setup)[iDet], fRunId);

    time = new BmnTimeDetQa*[nTime];
    for (Int_t iDet = nCoordinate; iDet < nCoordinate + nTime; iDet++)
        time[iDet - nCoordinate] = new BmnTimeDetQa(fSteering->GetDetectors(period, setup)[iDet], fRunId);

    calorimeter = new BmnCalorimeterDetQa*[nCalorimeter];
    for (Int_t iDet = nCoordinate + nTime; iDet < nCoordinate + nTime + nCalorimeter; iDet++)
        calorimeter[iDet - nCoordinate - nTime] = new BmnCalorimeterDetQa(fSteering->GetDetectors(period, setup)[iDet], fRunId);

    for (Int_t iDet = 0; iDet < nTrigs; iDet++)
        fTrigCorr[TRIGGERS[iDet]] = fSteering->GetTriggers(period, setup)[iDet];

    if (fTrigCorr.size() != 0)
        triggers = new BmnTrigDetQa(fTrigCorr, fRunId);

    // Dst
    dst = new BmnDstQa(fRunId);

    Char_t* geoFileName = (Char_t*) "current_geo_file.root";
    Int_t res_code = UniRun::ReadGeometryFile(period, fRunId, geoFileName);
    if (res_code != 0) {
        cout << "Geometry file can't be read from the database" << endl;
        exit(-1);
    }
    TGeoManager::Import(geoFileName);
    UniRun* db = UniRun::GetRun(period, fRunId);
    isField = (*db->GetFieldVoltage() > 10.) ? kTRUE : kFALSE;

    return kSUCCESS;
}

void BmnQaOffline::Finish() {
    vector <TString> detectors;

    // TRIGGERS are considered as a whole detector when saving to output file
    for (Int_t iDet = 0; iDet < nDets; iDet++)
        detectors.push_back(fSteering->GetDetectors(period, setup)[iDet]);
    detectors.push_back("TRIGGERS");

    // Fill a vector with histo managers
    vector <BmnQaHistoManager*> managers;
    for (Int_t iDet = 0; iDet < nCoordinate; iDet++)
        managers.push_back(coordinate[iDet]->GetManager());

    for (Int_t iDet = 0; iDet < nTime; iDet++)
        managers.push_back(time[iDet]->GetManager());

    for (Int_t iDet = 0; iDet < nCalorimeter; iDet++)
        managers.push_back(calorimeter[iDet]->GetManager());

    managers.push_back(triggers->GetManager());

    TDirectory** directories = new TDirectory*[nDets + 1];
    for (Int_t iDet = 0; iDet < nDets + 1; iDet++) {
        directories[iDet] = ioman->GetOutFile()->mkdir(detectors[iDet].Data());
        directories[iDet]->cd();
        managers[iDet]->WriteToFile();
    }

    ioman->GetOutFile()->mkdir("DST")->cd();
    dst->GetManager()->WriteToFile();

    // Delete detector classes and its histo classes
    for (Int_t iDet = 0; iDet < nCoordinate; iDet++)
        delete coordinate[iDet];

    for (Int_t iDet = 0; iDet < nTime; iDet++)
        delete time[iDet];

    for (Int_t iDet = 0; iDet < nCalorimeter; iDet++)
        delete calorimeter[iDet];

    delete triggers;
    delete dst;
}

void BmnQaOffline::Exec(Option_t* opt) {
    if (isDstRead) {
        if (fCurrentEvent > fChainDst->GetEntries() - 1)
            return;
        else
            fChainDst->GetEntry(fCurrentEvent);
    }
    fCurrentEvent++;
    if (fCurrentEvent % 1000 == 0)
        cout << "Event# = " << fCurrentEvent << endl;

    if (isDstRead) {
        vector <TString> dets = {"MWPC", "SILICON", "GEM", "CSC", "TOF400", "TOF700", "DCH"};
        vector <TClonesArray*> hits = {fMwpcHits, fSiliconHits, fGemHits, fCscHits, fTof400Hits, fTof700Hits, fDchHits};

        for (Int_t iDet = 0; iDet < dets.size(); iDet++)
            for (Int_t iHit = 0; iHit < dets.size(); iHit++) {
                if (iDet != iHit)
                    continue;

                doHitsDistributions(hits[iHit], dst, dets[iDet]);
            }

        GetBasicTrackDistributions();        
        doMatchingAnal();
        doResidualsPullsAnal();
        doAverageStripValuePerTrack();

        for (auto hit : hits) {
            doOccupancy(hit);
            doAverageStripValuePerHit(hit);
        }
    }

    // Coord. dets
    for (Int_t iDet = 0; iDet < nCoordinate; iDet++) {
        TString detName = fSteering->GetDetectors(period, setup)[iDet];
        TClonesArray* detData = DETECTORS[iDet];
        BmnCoordinateDetQa* detQa = coordinate[iDet];

        if (detName.Contains("GEM")) {
            GetDistributionOfFiredStrips <BmnGemStripDigit> (detData, detQa, prefix + detName); // histos 1
            GetDistributionOfFiredStripsVsSignal <BmnGemStripDigit> (detData, detQa, prefix + detName); // histos 2
        } else if (detName.Contains("SILICON")) {
            GetDistributionOfFiredStrips <BmnSiliconDigit> (detData, detQa, prefix + detName);
        } else if (detName.Contains("CSC")) {
            GetDistributionOfFiredStrips <BmnCSCDigit> (detData, detQa, prefix + detName);
        }
    }

    // Time dets
    for (Int_t iDet = nCoordinate; iDet < nCoordinate + nTime; iDet++) {
        TString detName = fSteering->GetDetectors(period, setup)[iDet];
        TClonesArray* detData = DETECTORS[iDet];
        BmnTimeDetQa* detQa = time[iDet - nCoordinate];

        if (detName.Contains("TOF400")) {
            GetCommonInfo <BmnTof1Digit> (detData, detQa, prefix + detName);
            GetTofInfo <BmnTof1Digit> (detData, detQa, prefix + detName);
        } else if (detName.Contains("TOF700")) {
            GetCommonInfo <BmnTof2Digit> (detData, detQa, prefix + detName);
            GetTofInfo <BmnTof2Digit> (detData, detQa, prefix + detName);
        } else if (detName.Contains("DCH")) {
            GetCommonInfo <BmnDchDigit> (detData, detQa, prefix + detName);
            GetMwpcDchInfo <BmnDchDigit> (detData, detQa, prefix + detName);
        } else if (detName.Contains("MWPC")) {
            GetCommonInfo <BmnMwpcDigit> (detData, detQa, prefix + detName);
            GetMwpcDchInfo <BmnMwpcDigit> (detData, detQa, prefix + detName);
        }
    }

    // Calorim. dets
    for (Int_t iDet = nCoordinate + nTime; iDet < nCoordinate + nTime + nCalorimeter; iDet++) {
        TString detName = fSteering->GetDetectors(period, setup)[iDet];
        TClonesArray* detData = DETECTORS[iDet];
        BmnCalorimeterDetQa* detQa = calorimeter[iDet - nCoordinate - nTime];

        if (detName.Contains("ECAL")) {
            GetCommonInfo <BmnECALDigit> (detData, detQa, prefix + detName);
        } else if (detName.Contains("ZDC")) {
            GetCommonInfo <BmnZDCDigit> (detData, detQa, prefix + detName);
        }
    }

    // Trig. dets
    for (auto it : fTrigCorr)
        GetCommonInfo <BmnTrigDigit> (it.first, triggers, it.second);

    if (fCurrentEvent == fChainDst->GetEntries() - 1) {
        doEfficiencyAnal();
        doOccupancyAnal();
    }
}

// Functions to be used when getting previously filled histos
// Coordinate detectors

// Histos 1

template <class T> void BmnQaOffline::GetDistributionOfFiredStrips(TClonesArray* digiArray, BmnCoordinateDetQa* detHistoClass, TString detName) {
    for (Int_t iDig = 0; iDig < digiArray->GetEntriesFast(); iDig++) {
        T* dig = (T*) digiArray->UncheckedAt(iDig);
        Int_t module = dig->GetModule();
        Int_t station = dig->GetStation();
        Int_t layer = dig->GetStripLayer();
        detHistoClass->GetManager()->H1(TString::Format("%s_1d, Distribution of fired strips, Stat %d Mod %d Lay %d", detName.Data(), station, module, layer))->Fill(dig->GetStripNumber());
    }
}

// Histos 2

template <class T> void BmnQaOffline::GetDistributionOfFiredStripsVsSignal(TClonesArray* digiArray, BmnCoordinateDetQa* detHistoClass, TString detName) {
    for (Int_t iDig = 0; iDig < digiArray->GetEntriesFast(); iDig++) {
        T* dig = (T*) digiArray->UncheckedAt(iDig);
        Int_t module = dig->GetModule();
        Int_t station = dig->GetStation();
        Int_t layer = dig->GetStripLayer();
        detHistoClass->GetManager()->H2(TString::Format("%s_2d, Distribution of fired strips vs. signal, Stat %d Mod %d Lay %d", detName.Data(), station, module, layer))->Fill(dig->GetStripNumber(), dig->GetStripSignal());
    }
}

// Time detectors

template <class T> void BmnQaOffline::GetCommonInfo(TClonesArray* digiArray, BmnTimeDetQa* detHistoClass, TString detName) {
    for (Int_t iDig = 0; iDig < digiArray->GetEntriesFast(); iDig++) {
        T* dig = (T*) digiArray->UncheckedAt(iDig);
        detHistoClass->GetManager()->H1(TString::Format("%s_1d, Distribution of planes", detName.Data()))->Fill(dig->GetPlane());
        detHistoClass->GetManager()->H1(TString::Format("%s_1d, Distribution of times", detName.Data()))->Fill(dig->GetTime());
    }
}

template <class T> void BmnQaOffline::GetMwpcDchInfo(TClonesArray* digiArray, BmnTimeDetQa* detHistoClass, TString detName) {
    for (Int_t iDig = 0; iDig < digiArray->GetEntriesFast(); iDig++) {
        T* dig = (T*) digiArray->UncheckedAt(iDig);
        detHistoClass->GetManager()->H1(TString::Format("%s_1d, Distribution of wires", detName.Data()))->Fill(dig->GetWireNumber());
    }
}

template <class T> void BmnQaOffline::GetTofInfo(TClonesArray* digiArray, BmnTimeDetQa* detHistoClass, TString detName) {
    for (Int_t iDig = 0; iDig < digiArray->GetEntriesFast(); iDig++) {
        T* dig = (T*) digiArray->UncheckedAt(iDig);
        detHistoClass->GetManager()->H1(TString::Format("%s_1d, Distribution of strips", detName.Data()))->Fill(dig->GetStrip());
        detHistoClass->GetManager()->H1(TString::Format("%s_1d, Distribution of amplitudes", detName.Data()))->Fill(dig->GetAmplitude());
    }
}

// Calorim. detectors

template <class T> void BmnQaOffline::GetCommonInfo(TClonesArray* digiArray, BmnCalorimeterDetQa* detHistoClass, TString detName) {
    for (Int_t iDig = 0; iDig < digiArray->GetEntriesFast(); iDig++) {
        T* dig = (T*) digiArray->UncheckedAt(iDig);
        detHistoClass->GetManager()->H1(TString::Format("%s_1d, Distribution of iX", detName.Data()))->Fill(dig->GetIX());
        detHistoClass->GetManager()->H1(TString::Format("%s_1d, Distribution of iY", detName.Data()))->Fill(dig->GetIY());
        detHistoClass->GetManager()->H1(TString::Format("%s_1d, Distribution of X", detName.Data()))->Fill(dig->GetX());
        detHistoClass->GetManager()->H1(TString::Format("%s_1d, Distribution of Y", detName.Data()))->Fill(dig->GetY());
        detHistoClass->GetManager()->H1(TString::Format("%s_1d, Distribution of channels", detName.Data()))->Fill(dig->GetChannel());
        detHistoClass->GetManager()->H1(TString::Format("%s_1d, Distribution of amplitudes", detName.Data()))->Fill(dig->GetAmp());
    }
}

// Trigger detectors

template <class T> void BmnQaOffline::GetCommonInfo(TClonesArray* digiArray, BmnTrigDetQa* detHistoClass, TString detName) {
    for (Int_t iDig = 0; iDig < digiArray->GetEntriesFast(); iDig++) {
        T* dig = (T*) digiArray->UncheckedAt(iDig);
        detHistoClass->GetManager()->H1(TString::Format("%sTRIGGERS_1d, %s, Distribution of inn. channels", prefix.Data(), detName.Data()))->Fill(dig->GetMod());
        detHistoClass->GetManager()->H1(TString::Format("%sTRIGGERS_1d, %s, Distribution of times", prefix.Data(), detName.Data()))->Fill(dig->GetTime());
        detHistoClass->GetManager()->H1(TString::Format("%sTRIGGERS_1d, %s, Distribution of amplitudes", prefix.Data(), detName.Data()))->Fill(dig->GetAmp());
    }
}

// Dst

void BmnQaOffline::GetBasicTrackDistributions() {
    if (!fGlobalTracks)
        return;

    const Int_t nDims = 3;
    TString dim[nDims] = {"X", "Y", "Z"};

    dst->GetManager()->H1(Form("%sDST_1d, Distribution of total multiplicity", prefix.Data()))->Fill(fGlobalTracks->GetEntriesFast());
    
    for (Int_t iTrack = 0; iTrack < fGlobalTracks->GetEntriesFast(); iTrack++) {
        BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobalTracks->UncheckedAt(iTrack);
        dst->GetManager()->H1(Form("%sDST_1d, Distribution of momenta", prefix.Data()))->Fill(1. / track->GetParamFirst()->GetQp());
        dst->GetManager()->H1(Form("%sDST_1d, Distribution of Nhits", prefix.Data()))->Fill(track->GetNHits());

        FairTrackParam* parFirst = track->GetParamFirst();
        FairTrackParam* parLast = track->GetParamLast();

        Double_t xyzFirst[nDims] = {parFirst->GetX(), parFirst->GetY(), parFirst->GetZ()};
        Double_t xyzLast[nDims] = {parLast->GetX(), parLast->GetY(), parLast->GetZ()};

        Double_t txtyFirst[] = {parFirst->GetTx(), parFirst->GetTy()};
        Double_t txtyLast[] = {parLast->GetTx(), parLast->GetTy()};

        for (Int_t iDim = 0; iDim < nDims; iDim++) {
            dst->GetManager()->H1(Form("%sDST_1d, Distribution of start%s", prefix.Data(), dim[iDim].Data()))->Fill(xyzFirst[iDim]);
            if (!dim[iDim].Contains("Z"))
                dst->GetManager()->H1(Form("%sDST_1d, Distribution of start T%s", prefix.Data(), dim[iDim].Data()))->Fill(txtyFirst[iDim]);
            dst->GetManager()->H1(Form("%sDST_1d, Distribution of last%s", prefix.Data(), dim[iDim].Data()))->Fill(xyzLast[iDim]);
            if (!dim[iDim].Contains("Z"))
                dst->GetManager()->H1(Form("%sDST_1d, Distribution of last T%s", prefix.Data(), dim[iDim].Data()))->Fill(txtyLast[iDim]);
        }
    }

    if (fSiliconTracks)
        for (Int_t iTrack = 0; iTrack < fSiliconTracks->GetEntriesFast(); iTrack++) {
            BmnSiliconTrack* track = (BmnSiliconTrack*) fSiliconTracks->UncheckedAt(iTrack);
            dst->GetManager()->H1(Form("%sDST_1d, Distribution of Nhits, %s track", prefix.Data(), "SILICON"))->Fill(track->GetNHits());
        }
    
    if (fGemTracks)
        for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
            BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);
            dst->GetManager()->H1(Form("%sDST_1d, Distribution of Nhits, %s track", prefix.Data(), "GEM"))->Fill(track->GetNHits());
        }
            
    if (fVertex)
        dst->GetManager()->H2(Form("%sDST_2d, Vp_{z} vs. Ntracks", prefix.Data()))->Fill(fGlobalTracks->GetEntriesFast(), ((CbmVertex*) fVertex->UncheckedAt(0))->GetZ());
}

void BmnQaOffline::doHitsDistributions(TClonesArray* hitsArray, BmnDstQa* detHistoClass, TString detName) {
    const Int_t nDims = 3;
    TString dim[nDims] = {"X", "Y", "Z"};

    if (!hitsArray)
        return;

    // Basic distributions: coordinates and errors (all detectors)
    for (Int_t iHit = 0; iHit < hitsArray->GetEntriesFast(); iHit++) {
        BmnHit* hit = (BmnHit*) hitsArray->UncheckedAt(iHit);

        for (Int_t iDim = 0; iDim < nDims; iDim++) {
            detHistoClass->GetManager()->H1(Form("%sDST_1d, %s, Distribution of %s", prefix.Data(), detName.Data(), dim[iDim].Data()))->Fill(
                    (iDim == 0) ? hit->GetX() : (iDim == 1) ? hit->GetY() : hit->GetZ());
            if (iDim == 2)
                continue;
            detHistoClass->GetManager()->H1(Form("%sDST_1d, %s, Distribution of d%s", prefix.Data(), detName.Data(), dim[iDim].Data()))->Fill(
                    (iDim == 0) ? hit->GetDx() : hit->GetDy());
        }
    }

    // Specific distributions for SILICON, GEM and CSC
    if (detName.Contains("SILICON"))
        for (Int_t iHit = 0; iHit < hitsArray->GetEntriesFast(); iHit++) {
            BmnSiliconHit* hit = (BmnSiliconHit*) hitsArray->UncheckedAt(iHit);

            detHistoClass->GetManager()->H1(Form("%sDST_1d, %s, Distribution of X cluster size", prefix.Data(), detName.Data()))->Fill(hit->GetClusterSizeInLowerLayer());
            detHistoClass->GetManager()->H1(Form("%sDST_1d, %s, Distribution of X' cluster size", prefix.Data(), detName.Data()))->Fill(hit->GetClusterSizeInUpperLayer());

            detHistoClass->GetManager()->H1(Form("%sDST_1d, %s, Distribution of X cluster signals", prefix.Data(), detName.Data()))->Fill(hit->GetStripTotalSignalInLowerLayer());
            detHistoClass->GetManager()->H1(Form("%sDST_1d, %s, Distribution of X' cluster signals", prefix.Data(), detName.Data()))->Fill(hit->GetStripTotalSignalInUpperLayer());
        } else if (detName.Contains("GEM"))
        for (Int_t iHit = 0; iHit < hitsArray->GetEntriesFast(); iHit++) {
            BmnGemStripHit* hit = (BmnGemStripHit*) hitsArray->UncheckedAt(iHit);

            detHistoClass->GetManager()->H1(Form("%sDST_1d, %s, Distribution of X cluster size", prefix.Data(), detName.Data()))->Fill(hit->GetClusterSizeInLowerLayer());
            detHistoClass->GetManager()->H1(Form("%sDST_1d, %s, Distribution of X' cluster size", prefix.Data(), detName.Data()))->Fill(hit->GetClusterSizeInUpperLayer());

            detHistoClass->GetManager()->H1(Form("%sDST_1d, %s, Distribution of X cluster signals", prefix.Data(), detName.Data()))->Fill(hit->GetStripTotalSignalInLowerLayer());
            detHistoClass->GetManager()->H1(Form("%sDST_1d, %s, Distribution of X' cluster signals", prefix.Data(), detName.Data()))->Fill(hit->GetStripTotalSignalInUpperLayer());
        } else if (detName.Contains("CSC"))
        for (Int_t iHit = 0; iHit < hitsArray->GetEntriesFast(); iHit++) {
            BmnCSCHit* hit = (BmnCSCHit*) hitsArray->UncheckedAt(iHit);

            detHistoClass->GetManager()->H1(Form("%sDST_1d, %s, Distribution of X cluster size", prefix.Data(), detName.Data()))->Fill(hit->GetClusterSizeInLowerLayer());
            detHistoClass->GetManager()->H1(Form("%sDST_1d, %s, Distribution of X' cluster size", prefix.Data(), detName.Data()))->Fill(hit->GetClusterSizeInUpperLayer());

            detHistoClass->GetManager()->H1(Form("%sDST_1d, %s, Distribution of X cluster signals", prefix.Data(), detName.Data()))->Fill(hit->GetStripTotalSignalInLowerLayer());
            detHistoClass->GetManager()->H1(Form("%sDST_1d, %s, Distribution of X' cluster signals", prefix.Data(), detName.Data()))->Fill(hit->GetStripTotalSignalInUpperLayer());
        }
}

BmnHit* BmnQaOffline::MatchDetector(FairTrackParam* parTrack, TClonesArray* hits, Bool_t doUpdate, Int_t detNum) {
    FairTrackParam minPar;
    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    BmnHit* minHit = nullptr;
    Double_t xCut = 5.0;
    Double_t yCut = 5.0;
    Int_t pdg = 2212;

    Double_t minZ = DBL_MAX;
    BmnKalmanFilter* kalman = new BmnKalmanFilter();

    map<Double_t, FairTrackParam> zParamMap;
    for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); ++iHit) {
        BmnHit* hit = (BmnHit*) hits->At(iHit);
        if (!hit) continue;
        if (hit->GetStation() != detNum) continue;
        minZ = Min(minZ, hit->GetZ());
        zParamMap[hit->GetZ()] = FairTrackParam();
    }
    FairTrackParam par(*parTrack);
    kalman->TGeoTrackPropagate(&par, minZ, pdg, NULL, NULL, isField);

    for (map<Double_t, FairTrackParam>::iterator it = zParamMap.begin(); it != zParamMap.end(); ++it) {
        (*it).second = par;
        kalman->TGeoTrackPropagate(&(*it).second, (*it).first, pdg, NULL, NULL, isField);
    }

    for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); ++iHit) {
        BmnHit* hit = (BmnHit*) hits->At(iHit);
        if (!hit) continue;
        if (hit->GetStation() != detNum) continue;
        if (zParamMap.find(hit->GetZ()) == zParamMap.end()) {
            std::cout << "-E- CbmLitNearestHitTofMerger::DoMerge: Z position " << hit->GetZ() << " not found in map. Something is wrong.\n";
        }
        FairTrackParam tpar(zParamMap[hit->GetZ()]);
        Double_t dX = tpar.GetX() - hit->GetX();
        Double_t dY = tpar.GetY() - hit->GetY();
        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < minDX && Abs(dY) < minDY) {
            minHit = hit;
            minDX = dX;
            minDY = dY;
            minPar = tpar;
        }
    }

    *(parTrack) = par;

    if (minHit) {
        *(parTrack) = minPar;
        if (doUpdate) {
            Double_t chi = 0;
            kalman->Update(parTrack, minHit, chi);
        }
    }
    delete kalman;

    return minHit;
}

void BmnQaOffline::doMatchingAnal() {
    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
        BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);

        const Int_t ndets = 4;
        const Int_t nSeqs = 4;
        TString dets[ndets] = {"CSC", "DCH1", "TOF700", "DCH2"};
        TClonesArray * detSequence[ndets][nSeqs] = {
            {fDchHits, fTof700Hits, fDchHits, fCscHits},
            {fCscHits, fTof700Hits, fDchHits, fDchHits},
            {fCscHits, fDchHits, fDchHits, fTof700Hits},
            {fCscHits, fDchHits, fTof700Hits, fDchHits}
        };

        Int_t idx[ndets][nSeqs] = {
            {0, -1, 1, 0},
            {0, -1, 1, 0},
            {0, 0, 1, -1},
            {0, 0, -1, 1}
        };
        Bool_t doUpdate[ndets][nSeqs] = {
            {kTRUE, kFALSE, kTRUE, kFALSE},
            {kTRUE, kFALSE, kTRUE, kFALSE},
            {kTRUE, kTRUE, kTRUE, kFALSE},
            {kTRUE, kTRUE, kFALSE, kFALSE}
        };

        for (Int_t iDet = 0; iDet < ndets; iDet++) {
            FairTrackParam parLast(*(track->GetParamLast()));
            if (MatchDetector(&parLast, detSequence[iDet][0], doUpdate[iDet][0], idx[iDet][0]))
                if (MatchDetector(&parLast, detSequence[iDet][1], doUpdate[iDet][1], idx[iDet][1]))
                    if (MatchDetector(&parLast, detSequence[iDet][2], doUpdate[iDet][2], idx[iDet][2])) {
                        BmnHit* hit = MatchDetector(&parLast, detSequence[iDet][3], doUpdate[iDet][3], idx[iDet][3]);
                        dst->GetManager()->H1(Form("%sDST_1d, hAllNo%s", prefix.Data(), dets[iDet].Data()))->Fill(1.0 / parLast.GetQp());
                        dst->GetManager()->H2(Form("%sDST_2d, hAllNo%sXY", prefix.Data(), dets[iDet].Data()))->Fill(parLast.GetX(), parLast.GetY());
                        if (hit) {
                            dst->GetManager()->H1(Form("%sDST_1d, hAllYes%s", prefix.Data(), dets[iDet].Data()))->Fill(1.0 / parLast.GetQp());
                            dst->GetManager()->H2(Form("%sDST_2d, hAllYes%sXY", prefix.Data(), dets[iDet].Data()))->Fill(parLast.GetX(), parLast.GetY());
                        }
                    }
        }
    }
}

void BmnQaOffline::doEfficiencyAnal() {
    const Int_t ndets = 4;
    TString dets[ndets] = {"CSC", "DCH1", "TOF700", "DCH2"};

    for (Int_t iDet = 0; iDet < ndets; iDet++) {
        TH1F* denom1 = (TH1F*) dst->GetManager()->H1(Form("%sDST_1d, hAllNo%s", prefix.Data(), dets[iDet].Data()));
        denom1->Sumw2();

        TH1F* nom1 = (TH1F*) dst->GetManager()->H1(Form("%sDST_1d, hAllYes%s", prefix.Data(), dets[iDet].Data()));
        nom1->Sumw2();

        TH1F* eff1 = (TH1F*) dst->GetManager()->H1(Form("%sDST_1d, hEff%s", prefix.Data(), dets[iDet].Data()));
        eff1->Divide(nom1, denom1);
        eff1->Scale(100);
        eff1->SetMaximum(110);
        eff1->SetMinimum(0);

        TH2F* denom2 = (TH2F*) dst->GetManager()->H2(Form("%sDST_2d, hAllNo%sXY", prefix.Data(), dets[iDet].Data()));
        denom2->Sumw2();

        TH2F* nom2 = (TH2F*) dst->GetManager()->H2(Form("%sDST_2d, hAllYes%sXY", prefix.Data(), dets[iDet].Data()));
        nom2->Sumw2();

        TH2F* eff2 = (TH2F*) dst->GetManager()->H2(Form("%sDST_2d, hEff%sXY", prefix.Data(), dets[iDet].Data()));
        eff2->Divide(nom2, denom2);
    }
}

void BmnQaOffline::doResidualsPullsAnal() {
    const Int_t ndets = 8;
    TString dets[ndets] = {"MWPC", "SILICON", "GEM", "CSC", "TOF400", "DCH1", "TOF700", "DCH2"};

    TClonesArray * detSequence[ndets] = {fMwpcHits, fSiliconHits, fGemHits, fCscHits, fTof400Hits, fDchHits, fTof700Hits, fDchHits};

    // Residuals ...
    for (Int_t iDet = 0; iDet < ndets; iDet++) {
        if (!detSequence[iDet])
            continue;
        for (Int_t iHit = 0; iHit < detSequence[iDet]->GetEntriesFast(); iHit++) {
            BmnHit* hit = (BmnHit*) detSequence[iDet]->UncheckedAt(iHit);

            if (dets[iDet].Contains("DCH")) {
                dets[iDet] = (hit->GetStation() == 0) ? "DCH1" : "DCH2";
                dst->GetManager()->H1(Form("%sDST_1d, hResX%s", prefix.Data(), dets[iDet].Data()))->Fill(hit->GetResX());
                dst->GetManager()->H1(Form("%sDST_1d, hResY%s", prefix.Data(), dets[iDet].Data()))->Fill(hit->GetResY());
            } else {
                dst->GetManager()->H1(Form("%sDST_1d, hResX%s", prefix.Data(), dets[iDet].Data()))->Fill(hit->GetResX());
                dst->GetManager()->H1(Form("%sDST_1d, hResY%s", prefix.Data(), dets[iDet].Data()))->Fill(hit->GetResY());
            }
        }
    }

    // Pulls ...
    for (Int_t iGlob = 0; iGlob < fGlobalTracks->GetEntriesFast(); iGlob++) {
        BmnGlobalTrack* glTrack = (BmnGlobalTrack*) fGlobalTracks->UncheckedAt(iGlob);

        BmnKalmanFilter* kalman = new BmnKalmanFilter();
        Int_t pdg = 2212;

        FairTrackParam* first = glTrack->GetParamFirst(); // param. estimation

        // DCH pulls ...
        for (Int_t iHit = 0; iHit < fDchHits->GetEntriesFast(); iHit++) {
            BmnHit* hit = (BmnHit*) fDchHits->UncheckedAt(iHit);

            // BmnTrack* track = (BmnTrack*) fDchTracks->UncheckedAt(hit->GetIndex());

            Double_t X = hit->GetX();
            Double_t dX = hit->GetDx();

            Double_t Y = hit->GetY();
            Double_t dY = hit->GetDy();

            FairTrackParam par(*first);
            kalman->TGeoTrackPropagate(&par, hit->GetZ(), pdg, nullptr, nullptr, isField);

            TString det = (hit->GetStation() == 0) ? "DCH1" : "DCH2";
            dst->GetManager()->H1(Form("%sDST_1d, hPullX%s", prefix.Data(), det.Data()))->Fill(((par.GetX() - X) / dX));
            dst->GetManager()->H1(Form("%sDST_1d, hPullY%s", prefix.Data(), det.Data()))->Fill(((par.GetY() - Y) / dY));
        }

        vector <TString> trackDets = {"MWPC", "SILICON", "GEM"};
        vector <TClonesArray*> tracks = {fMwpcTracks, fSiliconTracks, fGemTracks};
        vector <TClonesArray*> trackHits = {fMwpcHits, fSiliconHits, fGemHits};

        for (auto detector : trackDets) {
            if (detector.Contains("MWPC")) // FIXME
                continue;

            Int_t idx = detector.Contains("SILICON") ? glTrack->GetSilTrackIndex() : glTrack->GetGemTrackIndex();
            Int_t idxArr = detector.Contains("MWPC") ? 0 : detector.Contains("SILICON") ? 1 : 2;

            if (idx != -1) {
                BmnTrack* track = (BmnTrack*) tracks[idxArr]->UncheckedAt(idx);

                for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
                    BmnHit* hit = (BmnHit*) trackHits[idxArr]->UncheckedAt(track->GetHitIndex(iHit));

                    Double_t X = hit->GetX();
                    Double_t dX = hit->GetDx();

                    Double_t Y = hit->GetY();
                    Double_t dY = hit->GetDy();

                    FairTrackParam par(*first);
                    kalman->TGeoTrackPropagate(&par, hit->GetZ(), pdg, nullptr, nullptr, isField);

                    dst->GetManager()->H1(Form("%sDST_1d, hPullX%s", prefix.Data(), detector.Data()))->Fill(((par.GetX() - X) / dX));
                    dst->GetManager()->H1(Form("%sDST_1d, hPullY%s", prefix.Data(), detector.Data()))->Fill(((par.GetY() - Y) / dY));
                }
            }
        }

        vector <TString> hitDets = {"CSC", "TOF400", "TOF700"};
        vector <TClonesArray*> hits = {fCscHits, fTof400Hits, fTof700Hits};

        for (auto detector : hitDets) {
            Int_t idx = detector.Contains("CSC") ? glTrack->GetCscHitIndex(0) :
                    detector.Contains("TOF400") ? glTrack->GetTof1HitIndex() :
                    glTrack->GetTof2HitIndex();
            Int_t idxArr = detector.Contains("CSC") ? 0 : detector.Contains("TOF400") ? 1 : 2;

            if (idx != -1) {
                BmnHit* hit = (BmnHit*) hits[idxArr]->UncheckedAt(idx);

                Double_t X = hit->GetX();
                Double_t dX = hit->GetDx();

                Double_t Y = hit->GetY();
                Double_t dY = hit->GetDy();

                FairTrackParam par(*first);
                kalman->TGeoTrackPropagate(&par, hit->GetZ(), pdg, nullptr, nullptr, isField);

                dst->GetManager()->H1(Form("%sDST_1d, hPullX%s", prefix.Data(), detector.Data()))->Fill(((par.GetX() - X) / dX));
                dst->GetManager()->H1(Form("%sDST_1d, hPullY%s", prefix.Data(), detector.Data()))->Fill(((par.GetY() - Y) / dY));
            }
        }
        delete kalman;
    }
}

void BmnQaOffline::doOccupancy(TClonesArray* hits) {
    if (!hits)
        return;
    
    const Double_t zBorder = 500.; // FIXME
    for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); iHit++) {
        BmnHit* hit = (BmnHit*) hits->UncheckedAt(iHit);

        Double_t x = hit->GetX();
        Double_t y = hit->GetY();
        Double_t z = hit->GetZ();

        TString className = (TString) hit->GetName();
        TString detName = (className.Contains("Gem")) ? "GEM" :
                (className.Contains("Silicon")) ? "SILICON" :
                (className.Contains("CSC")) ? "CSC" : "";

        if (detName.IsNull()) {
            if (className.Contains("Tof") && (z < zBorder))
                detName = "TOF400";
            else if (className.Contains("Tof") && (z > zBorder))
                detName = "TOF700";
        }

        if (detName.Contains("GEM")) {
            for (Int_t iStat = 0; iStat < fDetGem->GetNStations(); iStat++)
                if (hit->GetStation() == iStat)
                    dst->GetManager()->H2(Form("%sDST_2d, hOccupancy%s, stat %d", prefix.Data(), detName.Data(), iStat))->Fill(x, y);
        } else if (detName.Contains("SILICON")) {
            for (Int_t iStat = 0; iStat < fDetSilicon->GetNStations(); iStat++)
                if (hit->GetStation() == iStat)
                    dst->GetManager()->H2(Form("%sDST_2d, hOccupancy%s, stat %d", prefix.Data(), detName.Data(), iStat))->Fill(x, y);
        } else if (detName.Contains("CSC")) {
            for (Int_t iStat = 0; iStat < fDetCsc->GetNStations(); iStat++)
                if (hit->GetStation() == iStat)
                    dst->GetManager()->H2(Form("%sDST_2d, hOccupancy%s, stat %d", prefix.Data(), detName.Data(), iStat))->Fill(x, y);
        } else if (detName.Contains("TOF400") || detName.Contains("TOF700"))
            dst->GetManager()->H2(Form("%sDST_2d, hOccupancy%s", prefix.Data(), detName.Data()))->Fill(x, y);
    }
}

void BmnQaOffline::doOccupancyAnal() {
    const Int_t ndets = 5;
    TString dets[ndets] = {"SILICON", "GEM", "CSC", "TOF400", "TOF700"};

    for (Int_t iDet = 0; iDet < ndets; iDet++) {
        if (dets[iDet].Contains("GEM"))
            for (Int_t iStat = 0; iStat < fDetGem->GetNStations(); iStat++)
                changeHistoContent((TH2F*) dst->GetManager()->H2(Form("%sDST_2d, hOccupancy%s, stat %d", prefix.Data(), dets[iDet].Data(), iStat)));

        else if (dets[iDet].Contains("SILICON"))
            for (Int_t iStat = 0; iStat < fDetSilicon->GetNStations(); iStat++)
                changeHistoContent((TH2F*) dst->GetManager()->H2(Form("%sDST_2d, hOccupancy%s, stat %d", prefix.Data(), dets[iDet].Data(), iStat)));

        else if (dets[iDet].Contains("CSC"))
            for (Int_t iStat = 0; iStat < fDetCsc->GetNStations(); iStat++)
                changeHistoContent((TH2F*) dst->GetManager()->H2(Form("%sDST_2d, hOccupancy%s, stat %d", prefix.Data(), dets[iDet].Data(), iStat)));

        else
            changeHistoContent((TH2F*) dst->GetManager()->H2(Form("%sDST_2d, hOccupancy%s", prefix.Data(), dets[iDet].Data())));
    }
}

void BmnQaOffline::changeHistoContent(TH2F* h) {
    for (Int_t iBinX = 0; iBinX < h->GetNbinsX() + 1; iBinX++)
        for (Int_t iBinY = 1; iBinY < h->GetNbinsY() + 1; iBinY++) {
            Double_t wX = h->GetXaxis()->GetBinWidth(iBinX);
            Double_t wY = h->GetYaxis()->GetBinWidth(iBinY);
            Double_t content = h->GetBinContent(iBinX, iBinY);

            content /= (wX * wY * fCurrentEvent);
            h->SetBinContent(iBinX, iBinY, content);
        }
}

void BmnQaOffline::doAverageStripValuePerHit(TClonesArray* hits) {
    if (!hits)
        return;
    
    const Int_t nLayers = 2; // X and X'
    TString layers[nLayers] = {"X", "X'"};

    for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); iHit++) {
        BmnHit* hit = (BmnHit*) hits->UncheckedAt(iHit);

        TString className = (TString) hit->GetName();
        TString detName = (className.Contains("Gem")) ? "GEM" :
                (className.Contains("Silicon")) ? "SILICON" :
                (className.Contains("CSC")) ? "CSC" : "";

        if (detName.IsNull())
            continue;

        if (detName.Contains("GEM")) {
            BmnGemStripHit* gemHit = (BmnGemStripHit*) hit;
            dst->GetManager()->H1(Form("%sDST_1d, Ave. strip value per hit in %s (%s)", prefix.Data(), detName.Data(), layers[0].Data()))->Fill(gemHit->GetClusterSizeInLowerLayer());
            dst->GetManager()->H1(Form("%sDST_1d, Ave. strip value per hit in %s (%s)", prefix.Data(), detName.Data(), layers[1].Data()))->Fill(gemHit->GetClusterSizeInUpperLayer());
        } else if (detName.Contains("SILICON")) {
            BmnSiliconHit* silHit = (BmnSiliconHit*) hit;
            dst->GetManager()->H1(Form("%sDST_1d, Ave. strip value per hit in %s (%s)", prefix.Data(), detName.Data(), layers[0].Data()))->Fill(silHit->GetClusterSizeInLowerLayer());
            dst->GetManager()->H1(Form("%sDST_1d, Ave. strip value per hit in %s (%s)", prefix.Data(), detName.Data(), layers[1].Data()))->Fill(silHit->GetClusterSizeInUpperLayer());
        } else if (detName.Contains("CSC")) {
            BmnCSCHit* cscHit = (BmnCSCHit*) hit;
            dst->GetManager()->H1(Form("%sDST_1d, Ave. strip value per hit in %s (%s)", prefix.Data(), detName.Data(), layers[0].Data()))->Fill(cscHit->GetClusterSizeInLowerLayer());
            dst->GetManager()->H1(Form("%sDST_1d, Ave. strip value per hit in %s (%s)", prefix.Data(), detName.Data(), layers[1].Data()))->Fill(cscHit->GetClusterSizeInUpperLayer());
        }
    }
}

void BmnQaOffline::doAverageStripValuePerTrack() {
    if (!fGlobalTracks)
        return;
    
    const Int_t nLayers = 2; // X and X'
    TString layers[nLayers] = {"X", "X'"};

    vector <TString> trackDets = {"SILICON", "GEM"};
    vector <TClonesArray*> tracks = {fSiliconTracks, fGemTracks};
    vector <TClonesArray*> trackHits = {fSiliconHits, fGemHits};

    for (Int_t iTrack = 0; iTrack < fGlobalTracks->GetEntriesFast(); iTrack++) {
        BmnGlobalTrack* glTrack = (BmnGlobalTrack*) fGlobalTracks->UncheckedAt(iTrack);

        // Get CSC ...
        Int_t idxCsc = glTrack->GetCscHitIndex(0);
        if (idxCsc != -1) {
            BmnCSCHit* hit = (BmnCSCHit*) fCscHits->UncheckedAt(idxCsc);
            for (Int_t iLayer = 0; iLayer < nLayers; iLayer++) {
                dst->GetManager()->H1(Form("%sDST_1d, Ave. strip value per track in %s (%s)", prefix.Data(), "CSC", layers[0].Data()))->Fill(hit->GetClusterSizeInLowerLayer());
                dst->GetManager()->H1(Form("%sDST_1d, Ave. strip value per track in %s (%s)", prefix.Data(), "CSC", layers[1].Data()))->Fill(hit->GetClusterSizeInUpperLayer());
            }
        }

        for (auto detector : trackDets) {
            Int_t idx = (detector.Contains("SILICON")) ? glTrack->GetSilTrackIndex() : glTrack->GetGemTrackIndex();
            Int_t idxArr = detector.Contains("SILICON") ? 0 : 1;

            if (idx == -1)
                continue;

            BmnTrack* track = (BmnTrack*) tracks[idxArr]->UncheckedAt(idx);

            Double_t sumLower = 0.;
            Double_t sumUpper = 0.;

            if (detector.Contains("SILICON")) {
                for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
                    BmnSiliconHit* hit = (BmnSiliconHit*) trackHits[idxArr]->UncheckedAt(track->GetHitIndex(iHit));
                    sumLower += hit->GetClusterSizeInLowerLayer();
                    sumUpper += hit->GetClusterSizeInUpperLayer();
                }
                for (Int_t iLayer = 0; iLayer < nLayers; iLayer++) {
                    dst->GetManager()->H1(Form("%sDST_1d, Ave. strip value per track in %s (%s)", prefix.Data(), detector.Data(), layers[0].Data()))->Fill(sumLower / track->GetNHits());
                    dst->GetManager()->H1(Form("%sDST_1d, Ave. strip value per track in %s (%s)", prefix.Data(), detector.Data(), layers[1].Data()))->Fill(sumUpper / track->GetNHits());
                }
            } else {
                for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
                    BmnGemStripHit* hit = (BmnGemStripHit*) trackHits[idxArr]->UncheckedAt(track->GetHitIndex(iHit));
                    sumLower += hit->GetClusterSizeInLowerLayer();
                    sumUpper += hit->GetClusterSizeInUpperLayer();
                }
                for (Int_t iLayer = 0; iLayer < nLayers; iLayer++) {
                    dst->GetManager()->H1(Form("%sDST_1d, Ave. strip value per track in %s (%s)", prefix.Data(), detector.Data(), layers[0].Data()))->Fill(sumLower / track->GetNHits());
                    dst->GetManager()->H1(Form("%sDST_1d, Ave. strip value per track in %s (%s)", prefix.Data(), detector.Data(), layers[1].Data()))->Fill(sumUpper / track->GetNHits());
                }
            }
        }
    }
}

ClassImp(BmnQaOffline);
