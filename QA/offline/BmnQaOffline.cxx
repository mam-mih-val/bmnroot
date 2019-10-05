#include "BmnQaOffline.h"

Int_t BmnQaOffline::fCurrentEvent = 0;

BmnQaOffline::BmnQaOffline(TString file) :
fBmnHeader(nullptr),
fChainDst(nullptr),
fSiliconHits(nullptr),
fSiliconTracks(nullptr),
fGemHits(nullptr),
fGemTracks(nullptr),
fVertex(nullptr),
fGlobalTracks(nullptr),
fSteering(new BmnOfflineQaSteering()) {
      isDstRead = ReadDstTree(file);
}

Bool_t BmnQaOffline::ReadDstTree(TString fileDst) {
    fChainDst = new TChain("bmndata");
    fChainDst->Add(fileDst.Data());

    fSiliconHits = new TClonesArray("BmnSiliconHit");
    fSiliconTracks = new TClonesArray("BmnSiliconTrack");
    fGemHits = new TClonesArray("BmnGemStripHit");
    fGemTracks = new TClonesArray("BmnGemTrack");
    fVertex = new TClonesArray("CbmVertex");
    fGlobalTracks = new TClonesArray("BmnGlobalTrack");

    fChainDst->SetBranchAddress("BmnSiliconHit", &fSiliconHits);
    fChainDst->SetBranchAddress("BmnSiliconTrack", &fSiliconTracks);
    fChainDst->SetBranchAddress("BmnGemStripHit", &fGemHits);
    fChainDst->SetBranchAddress("BmnGemTrack", &fGemTracks);
    fChainDst->SetBranchAddress("BmnVertex", &fVertex);
    fChainDst->SetBranchAddress("BmnGlobalTrack", &fGlobalTracks);

    if (fileDst.IsNull() || fChainDst->GetEntries() == 0)
        return kFALSE;
    else
        return kTRUE;
}

InitStatus BmnQaOffline::Init() {
    cout << " BmnQaOffline::Init() " << endl;

    ioman = FairRootManager::Instance();

    fBmnHeader = (BmnEventHeader*) ioman->GetObject("BmnEventHeader.");
    // Get general info on period and exp. setup, detectors and trigger been
    period = fSteering->GetRunAndSetupByRunId(fBmnHeader->GetRunId()).first;
    setup = fSteering->GetRunAndSetupByRunId(fBmnHeader->GetRunId()).second;
    nDets = fSteering->GetDetectors(period, setup).size(); // Number of detectors should be set for a certain run / setup extension
    Int_t nTrigs = fSteering->GetTriggers(period, setup).size(); // Number of triggers should be set for a certain run / setup extension

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
        coordinate[iDet] = new BmnCoordinateDetQa(fSteering->GetDetectors(period, setup)[iDet], fBmnHeader->GetRunId());

    time = new BmnTimeDetQa*[nTime];
    for (Int_t iDet = nCoordinate; iDet < nCoordinate + nTime; iDet++)
        time[iDet - nCoordinate] = new BmnTimeDetQa(fSteering->GetDetectors(period, setup)[iDet], fBmnHeader->GetRunId());

    calorimeter = new BmnCalorimeterDetQa*[nCalorimeter];
    for (Int_t iDet = nCoordinate + nTime; iDet < nCoordinate + nTime + nCalorimeter; iDet++)
        calorimeter[iDet - nCoordinate - nTime] = new BmnCalorimeterDetQa(fSteering->GetDetectors(period, setup)[iDet], fBmnHeader->GetRunId());

    for (Int_t iDet = 0; iDet < nTrigs; iDet++)
        fTrigCorr[TRIGGERS[iDet]] = fSteering->GetTriggers(period, setup)[iDet];

    if (fTrigCorr.size() != 0)
        triggers = new BmnTrigDetQa(fTrigCorr, fBmnHeader->GetRunId());

    // Dst
    dst = new BmnDstQa(fBmnHeader->GetRunId());

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
    
    TString prefix = TString::Format("RUN%d_SETUP_%s_", period, setup.Data());

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
        GetCommonInfo <BmnTrigDigit> (it.first, triggers, it.second, prefix);

    // Dst
    GetGlobalTracksDistributions(fGlobalTracks, fVertex, dst, prefix);
    GetInnerTracksDistributions <BmnSiliconTrack> (fSiliconTracks, dst, "silicon", prefix);
    GetInnerTracksDistributions <BmnGemTrack> (fGemTracks, dst, "gem", prefix);
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

template <class T> void BmnQaOffline::GetCommonInfo(TClonesArray* digiArray, BmnTrigDetQa* detHistoClass, TString detName, TString prefix) {
    for (Int_t iDig = 0; iDig < digiArray->GetEntriesFast(); iDig++) {
        T* dig = (T*) digiArray->UncheckedAt(iDig);
        detHistoClass->GetManager()->H1(TString::Format("%sTRIGGERS_1d, %s, Distribution of inn. channels", prefix.Data(), detName.Data()))->Fill(dig->GetMod());
        detHistoClass->GetManager()->H1(TString::Format("%sTRIGGERS_1d, %s, Distribution of times", prefix.Data(), detName.Data()))->Fill(dig->GetTime());
        detHistoClass->GetManager()->H1(TString::Format("%sTRIGGERS_1d, %s, Distribution of amplitudes", prefix.Data(), detName.Data()))->Fill(dig->GetAmp());
    }
}

// Dst

void BmnQaOffline::GetGlobalTracksDistributions(TClonesArray* tracksArray, TClonesArray* vertex, BmnDstQa* detHistoClass, TString prefix) {
    const Int_t nDims = 3;
    TString dim[nDims] = {"X", "Y", "Z"};

    // Histos 1
    detHistoClass->GetManager()->H1(Form("%sDST_1d, Distribution of total multiplicity", prefix.Data()))->Fill(tracksArray->GetEntriesFast());
    for (Int_t iTrack = 0; iTrack < tracksArray->GetEntriesFast(); iTrack++) {
        BmnGlobalTrack* track = (BmnGlobalTrack*) tracksArray->UncheckedAt(iTrack);
        detHistoClass->GetManager()->H1(Form("%sDST_1d, Distribution of momenta", prefix.Data()))->Fill(1. / track->GetParamFirst()->GetQp());
        detHistoClass->GetManager()->H1(Form("%sDST_1d, Distribution of Nhits", prefix.Data()))->Fill(track->GetNHits());

        FairTrackParam* parFirst = track->GetParamFirst();
        FairTrackParam* parLast = track->GetParamLast();

        Double_t xyzFirst[nDims] = {parFirst->GetX(), parFirst->GetY(), parFirst->GetZ()};
        Double_t xyzLast[nDims] = {parLast->GetX(), parLast->GetY(), parLast->GetZ()};

        Double_t txtyFirst[] = {parFirst->GetTx(), parFirst->GetTy()};
        Double_t txtyLast[] = {parLast->GetTx(), parLast->GetTy()};

        for (Int_t iDim = 0; iDim < nDims; iDim++) {
            detHistoClass->GetManager()->H1(Form("%sDST_1d, Distribution of start%s", prefix.Data(), dim[iDim].Data()))->Fill(xyzFirst[iDim]);
            if (!dim[iDim].Contains("Z"))
                detHistoClass->GetManager()->H1(Form("%sDST_1d, Distribution of start T%s", prefix.Data(), dim[iDim].Data()))->Fill(txtyFirst[iDim]);
            detHistoClass->GetManager()->H1(Form("%sDST_1d, Distribution of last%s", prefix.Data(), dim[iDim].Data()))->Fill(xyzLast[iDim]);
            if (!dim[iDim].Contains("Z"))
                detHistoClass->GetManager()->H1(Form("%sDST_1d, Distribution of last T%s", prefix.Data(), dim[iDim].Data()))->Fill(txtyLast[iDim]);
        }
    }

    // Histos 2
    detHistoClass->GetManager()->H2(Form("%sDST_2d, Vp_{z} vs. Ntracks", prefix.Data()))->Fill(tracksArray->GetEntriesFast(), ((CbmVertex*) vertex->UncheckedAt(0))->GetZ());
}

template <class T> void BmnQaOffline::GetInnerTracksDistributions(TClonesArray* tracksArray, BmnDstQa* detHistoClass, TString detName, TString prefix) {
    for (Int_t iTrack = 0; iTrack < tracksArray->GetEntriesFast(); iTrack++) {
        T* track = (T*) tracksArray->UncheckedAt(iTrack);
        detHistoClass->GetManager()->H1(Form("%sDST_1d, Distribution of Nhits, %s track", prefix.Data(), detName.Data()))->Fill(track->GetNHits());
    }
}

ClassImp(BmnQaOffline);