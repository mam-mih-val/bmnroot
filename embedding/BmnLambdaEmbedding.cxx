#include <unordered_map>

#include "BmnLambdaEmbedding.h"
#include "CbmStsPoint.h"

BmnLambdaEmbedding::BmnLambdaEmbedding() {

}

BmnLambdaEmbedding::BmnLambdaEmbedding(TString raw, TString sim, TString reco, TString out, Int_t nEvs) :
// fRaw(nullptr),
fSim(nullptr),
fLambdaSim(nullptr),
fReco(nullptr),
fMCTracks(nullptr),
fVertices(nullptr),
fLambdaStore(nullptr),
fHeader(nullptr),
fADC32(nullptr),
fADC128(nullptr),
fSiliconPoints(nullptr),
fSiliconDigits(nullptr),
fSiliconMatch(nullptr),
fGemPoints(nullptr),
fGemDigits(nullptr),
fGemMatch(nullptr),
fInfo(nullptr) {
    fDataFileName = raw;
    fDigiFileName = out;

    // Initialize steering flags by default ...
    doLambdaStore = kTRUE;
    doListOfEventsWithReconstructedVertex = kTRUE;
    doSimulateLambdaThroughSetup = kTRUE;

    fEvents = nEvs;

    // Open simu file
    fSim = new TChain("bmndata");
    fSim->Add(sim.Data());
    fSim->SetBranchAddress("MCTrack", &fMCTracks);

    // Open dst file
    fReco = new TChain("bmndata");
    fReco->Add(reco.Data());
    fReco->SetBranchAddress("BmnVertex", &fVertices);
    fReco->SetBranchAddress("DstEventHeader.", &fHeader);

    // Useful tools 
    fInfo = new BmnLambdaMisc(); // Initialize useful tools to work with mapping ...
    fLambdaStore = new TClonesArray("BmnLambdaStore"); // Initialize container to store primary lambdas to be used for simulations ...

    fZmin = -3.;
    fZmax = 0.;

    fRunId = -1;
    fFieldScale = -1.;
    
    fNeventsForStore = 1000;
}

void BmnLambdaEmbedding::Embedding() {
    // 1. Create a store with lambdas ...
    if (doLambdaStore)
        CreateLambdaStore();

    // 2. Create list of eventId for reconstructed events where the primary vertex is assumed to be defined ...
    map <UInt_t, TVector3> EventIdsVpMap;
    if (doListOfEventsWithReconstructedVertex)
        EventIdsVpMap = ListOfEventsWithReconstructedVp();

    // 3. Get information on mag. field in the run ...
    if (doSimulateLambdaThroughSetup) {
        if (fRunId != -1) {
            Double_t map_current = 55.87;
            UniDbRun* runInfo = UniDbRun::GetRun(7, fRunId);
            if (runInfo)
                fFieldScale = *runInfo->GetFieldVoltage() / map_current;
        }
    }

    // 4. Loop over store with lambdas ...
    for (Int_t iLambda = 0; iLambda < fLambdaStore->GetEntriesFast(); iLambda++) {
        BmnLambdaStore* lambda = (BmnLambdaStore*) fLambdaStore->UncheckedAt(iLambda);
        Double_t eta = lambda->GetEta();
        Double_t phi = lambda->GetPhi();
        Double_t p = lambda->GetP();

        Int_t iVertex = 0;

        for (auto it : EventIdsVpMap) {
            // Get reconstructed primary vertex ...
            TVector3 Vp = it.second;

            if (doSimulateLambdaThroughSetup)
                SimulateLambdaPassing(p, TVector2(eta, phi), Vp, iLambda, iVertex);

            iVertex++;
        }

        lambda->SetUsing(kTRUE);
    }

    // 5. Find at least one lambda to be reconstructed for a given Vp
    map <vector <Int_t>, pair <UInt_t, TVector3>> GoodLambdaForEachVertex; // <iLambda, iVertex, iEvent> --> <evId, Vp>

    Int_t iVertex = 0;
    for (auto it : EventIdsVpMap) {
        for (Int_t iLambda = 0; iLambda < fLambdaStore->GetEntriesFast(); iLambda++) {
            if (FindReconstructableLambdaFromStore(iLambda, iVertex) != -1) {
                pair <UInt_t, TVector3> tmpPair = make_pair(it.first, it.second);
                vector <Int_t> tmpVect{iLambda, iVertex, FindReconstructableLambdaFromStore(iLambda, iVertex)};
                GoodLambdaForEachVertex[tmpVect] = tmpPair;
                break;
            }
        }
        iVertex++;
    }

    // Test already filled map ...
    //    for (auto it : GoodLambdaForEachVertex) {
    //        vector <Int_t> par = it.first;
    //        UInt_t id = it.second.first;
    //        TVector3 Vp = it.second.second;
    //
    //        Vp.Print();
    //        cout << "evId = " << id << " ---> " << par[0] << " " << par[1] << " " << par[2] << endl;
    //
    //    }

    // 6. Create digi-arrays corresponding to a certain eventId ... <evId --> lambdaDigs>
    map <UInt_t, vector < BmnGemStripDigit>> digsFromLambdas;
    for (auto it : GoodLambdaForEachVertex) {
        vector <Int_t> par = it.first;
        Int_t lambda = par[0];
        Int_t vertex = par[1];
        Int_t event = par[2];

        digsFromLambdas[it.second.first] = GetDigitsFromLambda(TString::Format("output/lambda%d_vertex%d.root", lambda, vertex), event);
    }

    //    for (auto it : digsFromLambdas)
    //        cout << "Id " << it.first << " " << it.second.size() << endl;

    // 7. Make correspondence between evId and lambda digits with info on channel and serial ... <evId ---> <digi index + ch, serial>>
    map <UInt_t, map < pair <Int_t, Int_t>, Long_t>> evIdChannelSerial;

    for (auto A : digsFromLambdas) {
        vector <BmnGemStripDigit> digits = A.second;
        map <pair <Int_t, Int_t>, Long_t> tmpMap = GetChannelSerialFromDigi(digits);
        evIdChannelSerial[A.first] = tmpMap;
    }

    //    for (auto it : evIdChannelSerial) {
    //        cout << "Id = " << it.first << " " << endl;
    //        vector <BmnGemStripDigit> digits = digsFromLambdas.find(it.first)->second;
    //       
    //        map <pair <Int_t, Int_t>, Long_t> digiIdxChanSer = it.second;
    //        
    //        for (auto digInfo : digiIdxChanSer) {
    //            digits[digInfo.first.first].Print();
    //            cout << std::dec << "Channel# " << digInfo.first.second << endl;
    //            cout << std::hex << "Serial# " << digInfo.second << endl;
    //        }
    //        cout << endl;
    //    }

    // 8. Do *.data --> *raw.root convertion for requested events ...
    DoRawRootFromBinaryData(fDataFileName);

    // 9. Loop over *raw.root to embed lambda digits ...
    TString rawRoot = AddInfoToRawFile(digsFromLambdas, evIdChannelSerial);

    // 10. Start decoding ...
    StartDecodingWithEmbeddedLambdas(rawRoot);
}

Int_t BmnLambdaEmbedding::FindReconstructableLambdaFromStore(Int_t iLambda, Int_t iVertex) {
    TString fileName = TString::Format("output/lambda%d_vertex%d.root", iLambda, iVertex);
    TChain ch("bmndata");
    ch.Add(fileName.Data());

    // Open lambda file
    TClonesArray* tracks = nullptr;
    TClonesArray* gemPoints = nullptr;
    TClonesArray* siliconPoints = nullptr;

    ch.SetBranchAddress("MCTrack", &tracks);
    ch.SetBranchAddress("StsPoint", &gemPoints);
    ch.SetBranchAddress("SiliconPoint", &siliconPoints);

    const Int_t pdgProton = 2212;
    const Int_t pdgPion = -211;
    const Int_t pdgLambda = 3122;

    for (Int_t iEv = 0; iEv < ch.GetEntries(); iEv++) {
        ch.GetEntry(iEv);

        // Loop over protons ... 
        // A proton we are looking for is single! 
        Int_t nHitsPerProton = 0;
        for (Int_t iTrack = 0; iTrack < tracks->GetEntriesFast(); iTrack++) {
            CbmMCTrack* track = (CbmMCTrack*) tracks->UncheckedAt(iTrack);

            // Skip primary tracks
            if (track->GetMotherId() == -1)
                continue;

            // Skipping particles not been protons
            if (track->GetPdgCode() == pdgPion)
                continue;

            Int_t id = track->GetMotherId();

            // Get secondary protons and pions from lambda decay
            if (((CbmMCTrack*) tracks->UncheckedAt(id))->GetPdgCode() != pdgLambda)
                continue;

            if (track->GetP() < 0.5)
                continue;

            const Int_t nStats = 9; // 6 GEMs + 3 SILICONs [<0 ... 5>, <6, 7, 8>]
            Int_t pointsOnGemsAndSilicon[nStats];
            for (Int_t iStat = 0; iStat < nStats; iStat++)
                pointsOnGemsAndSilicon[iStat] = 0;

            // Loop over gem points ...
            for (Int_t iGem = 0; iGem < gemPoints->GetEntriesFast(); iGem++) {
                CbmStsPoint* point = (CbmStsPoint*) gemPoints->UncheckedAt(iGem);
                if (point->GetTrackID() != iTrack)
                    continue;
                pointsOnGemsAndSilicon[point->GetStation()]++;
            }

            // Loop over silicon points ...
            for (Int_t iSilicon = 0; iSilicon < siliconPoints->GetEntriesFast(); iSilicon++) {
                FairMCPoint* point = (FairMCPoint*) siliconPoints->UncheckedAt(iSilicon);
                if (point->GetTrackID() != iTrack)
                    continue;
                pointsOnGemsAndSilicon[DefineSiliconStatByZpoint(point->GetZ())]++;
            }

            // Loop over statArray ... 
            Bool_t isMultiplePointsOnZ = kFALSE;
            for (Int_t iStat = 0; iStat < nStats; iStat++) {
                if (pointsOnGemsAndSilicon[iStat] == 2) {
                    isMultiplePointsOnZ = kTRUE;
                    break;
                } else
                    nHitsPerProton += pointsOnGemsAndSilicon[iStat];
            }
            if (isMultiplePointsOnZ || nHitsPerProton < 4)
                continue;
        }

        // Skipping the next loop if not satisfied condition ...
        if (nHitsPerProton < 4)
            continue;

        // Loop over pions ... 
        Int_t nHitsPerPion = 0;
        for (Int_t jTrack = 0; jTrack < tracks->GetEntriesFast(); jTrack++) {
            CbmMCTrack* track = (CbmMCTrack*) tracks->UncheckedAt(jTrack);

            // Skip primary tracks
            if (track->GetMotherId() == -1)
                continue;

            // Skipping particles not been protons or pions ...
            if (track->GetPdgCode() == pdgProton)
                continue;

            Int_t id = track->GetMotherId();

            // Get secondary protons and pions from lambda decay
            if (((CbmMCTrack*) tracks->UncheckedAt(id))->GetPdgCode() != pdgLambda)
                continue;

            if (track->GetP() < 0.5)
                continue;

            const Int_t nStats = 9; // 6 GEMs + 3 SILICONs [<0 ... 5>, <6, 7, 8>]
            Int_t pointsOnGemsAndSilicon[nStats];
            for (Int_t iStat = 0; iStat < nStats; iStat++)
                pointsOnGemsAndSilicon[iStat] = 0;

            // Loop over gem points ...
            for (Int_t iGem = 0; iGem < gemPoints->GetEntriesFast(); iGem++) {
                CbmStsPoint* point = (CbmStsPoint*) gemPoints->UncheckedAt(iGem);
                if (point->GetTrackID() != jTrack)
                    continue;
                pointsOnGemsAndSilicon[point->GetStation()]++;
            }

            // Loop over silicon points ...
            for (Int_t iSilicon = 0; iSilicon < siliconPoints->GetEntriesFast(); iSilicon++) {
                FairMCPoint* point = (FairMCPoint*) siliconPoints->UncheckedAt(iSilicon);
                if (point->GetTrackID() != jTrack)
                    continue;
                pointsOnGemsAndSilicon[DefineSiliconStatByZpoint(point->GetZ())]++;
            }

            // Loop over statArray ... 
            Bool_t isMultiplePointsOnZ = kFALSE;
            for (Int_t iStat = 0; iStat < nStats; iStat++) {
                if (pointsOnGemsAndSilicon[iStat] == 2) {
                    isMultiplePointsOnZ = kTRUE;
                    break;
                } else
                    nHitsPerPion += pointsOnGemsAndSilicon[iStat];
            }
            if (isMultiplePointsOnZ || nHitsPerPion < 4)
                continue;
        }

        if (nHitsPerPion < 4)
            continue;

        return iEv;
    }
    return -1;
}

void BmnLambdaEmbedding::DoRawRootFromBinaryData(TString raw) {
    BmnRawDataDecoder* decoder = new BmnRawDataDecoder(raw, "", fEvents);
    decoder->SetPeriodId(7);
    decoder->SetRunId(fRunId);
    decoder->SetBmnSetup(kBMNSETUP);

    const Int_t nDets = 11;
    Bool_t setup[nDets]; //array of flags to determine BM@N setup

    for (Int_t iDet = 0; iDet < nDets; iDet++)
        setup[iDet] = kFALSE;

    setup[0] = kTRUE; // TRIGGERS
    setup[2] = kTRUE; // SILICON
    setup[3] = kTRUE; // GEM

    decoder->SetDetectorSetup(setup);

    decoder->SetTrigPlaceMapping("Trig_PlaceMap_Run7.txt");
    decoder->SetTrigChannelMapping("Trig_map_Run7.txt");
    decoder->SetSiliconMapping("SILICON_map_run7.txt");
    decoder->SetGemMapping("GEM_map_run7.txt");

    decoder->InitMaps();

    decoder->ConvertRawToRoot();
    fRawRootFileName = decoder->GetRootFileName();

    delete decoder;
}

void BmnLambdaEmbedding::StartDecodingWithEmbeddedLambdas(TString raw) {
    BmnRawDataDecoder* decoder = new BmnRawDataDecoder("", "", fEvents);
    decoder->SetPeriodId(7);
    decoder->SetRunId(fRunId);
    // raw = "bmn_run4649_raw.root";
    decoder->SetRawRootFile(raw);
    decoder->SetDigiRootFile(fDigiFileName);
    decoder->SetBmnSetup(kBMNSETUP);

    const Int_t nDets = 11;
    Bool_t setup[nDets]; //array of flags to determine BM@N setup

    for (Int_t iDet = 0; iDet < nDets; iDet++)
        setup[iDet] = kFALSE;

    setup[0] = kTRUE; // TRIGGERS
    setup[2] = kFALSE; // SILICON
    setup[3] = kTRUE; // GEM

    decoder->SetDetectorSetup(setup);
    decoder->SetAdcDecoMode(kBMNADCMK);

    decoder->SetTrigPlaceMapping("Trig_PlaceMap_Run7.txt");
    decoder->SetTrigChannelMapping("Trig_map_Run7.txt");
    decoder->SetSiliconMapping("SILICON_map_run7.txt");
    decoder->SetGemMapping("GEM_map_run7.txt");

    decoder->InitMaps();

    decoder->DecodeDataToDigi();
}

map <UInt_t, TVector3> BmnLambdaEmbedding::ListOfEventsWithReconstructedVp() {
    map <UInt_t, TVector3> EventIdsVpMap;
    for (Int_t iEntry = 0; iEntry < fEvents; iEntry++) {
        fReco->GetEntry(iEntry);

        if (iEntry == 0)
            fRunId = fHeader->GetRunId();

        CbmVertex* vertex = (CbmVertex*) fVertices->UncheckedAt(0);

        // Used cuts on primary vertex
        if (vertex->GetZ() < fZmin || vertex->GetZ() > fZmax)
            continue;

        EventIdsVpMap[fHeader->GetEventId()] = TVector3(vertex->GetX(), vertex->GetY(), vertex->GetZ());
    }
    return EventIdsVpMap;
}

vector <BmnGemStripDigit> BmnLambdaEmbedding::GetDigitsFromLambda(TString lambdaEve, Int_t evNum) {
    vector <BmnGemStripDigit> digits;

    // File should contain only one lambda per one event !!! (FIXME)
    // Open file with simulated lambda
    fLambdaSim = new TChain("bmndata");
    fLambdaSim->Add(lambdaEve.Data());
    fLambdaSim->SetBranchAddress("MCTrack", &fMCTracks);

    fLambdaSim->SetBranchAddress("SiliconPoint", &fSiliconPoints);
    fLambdaSim->SetBranchAddress("BmnSiliconDigit", &fSiliconDigits);
    fLambdaSim->SetBranchAddress("BmnSiliconDigitMatch", &fSiliconMatch);

    fLambdaSim->SetBranchAddress("StsPoint", &fGemPoints);
    fLambdaSim->SetBranchAddress("BmnGemStripDigit", &fGemDigits);
    fLambdaSim->SetBranchAddress("BmnGemStripDigitMatch", &fGemMatch);

    const Int_t pdgProton = 2212;
    const Int_t pdgPion = -211;
    const Int_t pdgLambda = 3122;

    fLambdaSim->GetEntry(evNum);

    Int_t idxPi = -1, idxProt = -1;

    // Looking for protons and pions from lambda decay
    for (Int_t iTrack = 0; iTrack < fMCTracks->GetEntriesFast(); iTrack++) {
        CbmMCTrack* track = (CbmMCTrack*) fMCTracks->UncheckedAt(iTrack);

        // Skip primary tracks
        if (track->GetMotherId() == -1)
            continue;

        // Skip particles not been protons or pions
        if (track->GetPdgCode() != pdgProton && track->GetPdgCode() != pdgPion)
            continue;

        Int_t pdg = track->GetPdgCode();
        Int_t id = track->GetMotherId();

        // Get secondary protons and pions from lambda decay
        if (((CbmMCTrack*) fMCTracks->UncheckedAt(id))->GetPdgCode() != pdgLambda)
            continue;

        if (pdg > 0)
            idxProt = iTrack;
        else
            idxPi = iTrack;
    }

    // Loop over GEM-digits        
    for (Int_t iDig = 0; iDig < fGemDigits->GetEntriesFast(); iDig++) {
        BmnMatch* digiMatch = (BmnMatch*) fGemMatch->UncheckedAt(iDig);

        Int_t idxPoint = digiMatch->GetMatchedLink().GetIndex();
        FairMCPoint* point = (FairMCPoint*) fGemPoints->UncheckedAt(idxPoint);
        if (point->GetTrackID() != idxProt && point->GetTrackID() != idxPi)
            continue;

        BmnGemStripDigit* digi = (BmnGemStripDigit*) fGemDigits->UncheckedAt(iDig);
        digits.push_back(*digi);
    }

    delete fLambdaSim;
    // GetChannelSerialFromDigi();

    return digits;
}

map <pair <Int_t, Int_t>, Long_t> BmnLambdaEmbedding::GetChannelSerialFromDigi(vector <BmnGemStripDigit> digits) {
    map <Int_t, Int_t> digiToChannel; // (digi index in vector ---> channel)
    map <pair <Int_t, Int_t>, Long_t> digiChannelToSerial; // (digi index in vector + channel ---> serial)

    // Fill digiToChannel map ...
    for (Int_t iDigi = 0; iDigi < digits.size(); iDigi++) {
        BmnGemStripDigit* dig = &digits[iDigi];
        digiToChannel[iDigi] = fInfo->GemDigiToChannel(dig);
    }

    // Fill digiChannelToSerial map ...
    for (auto it : digiToChannel)
        digiChannelToSerial[make_pair(it.first, it.second)] = fInfo->GemDigiChannelToSerial(make_pair(digits[it.first], it.second));

    return digiChannelToSerial;
}

TString BmnLambdaEmbedding::AddInfoToRawFile(map <UInt_t, vector <BmnGemStripDigit>> digs, map <UInt_t, map <pair <Int_t, Int_t>, Long_t>> evIdChannelSerial) {
    // Create output file ... 
    TString tmp = fRawRootFileName;
    tmp = tmp.ReplaceAll(".root", "");
    tmp += "_withLambdaEmbedded.root";

    TFile* f = new TFile(tmp.Data(), "recreate");
    TTree* t = new TTree("BMN_RAW", "BMN_RAW");

    TClonesArray* gem = new TClonesArray("BmnADCDigit");
    TClonesArray* silicon = new TClonesArray("BmnADCDigit");

    BmnEventHeader* header = new BmnEventHeader();

    t->Branch("BmnEventHeader.", &header);
    t->Branch("ADC32", &gem);
    t->Branch("ADC128", &silicon);

    TChain* ch = new TChain("BMN_RAW");
    ch->Add(fRawRootFileName.Data());

    BmnEventHeader* fEventHeader = nullptr;

    ch->SetBranchAddress("ADC32", &fADC32);
    ch->SetBranchAddress("ADC128", &fADC128);
    ch->SetBranchAddress("BmnEventHeader.", &fEventHeader);

    // Loop over existing *raw.root file ...
    for (Int_t iEntry = 0; iEntry < ch->GetEntries(); iEntry++) {
        gem->Delete(); // Clear TClonesArray from the previous events ... 

        ch->GetEntry(iEntry);

        UInt_t eventId = fEventHeader->GetEventId();
        //        if (eventId != 219)
        //            continue;

        header->SetRunId(fEventHeader->GetRunId());
        header->SetEventId(eventId);
        header->SetEventType(fEventHeader->GetEventType());

        // Looking for current eventId in the digi-map ...
        Bool_t isEventIdFoundInTheMap = kTRUE;
        auto itDig = digs.find(eventId);
        if (itDig == digs.end())
            isEventIdFoundInTheMap = kFALSE;

        // Array with digits for certain eventId
        vector <BmnGemStripDigit> digits;
        map <pair <Int_t, Int_t>, Long_t> idxChanSer;

        if (isEventIdFoundInTheMap) {
            digits = itDig->second;
            idxChanSer = evIdChannelSerial.find(eventId)->second;
        }

        //cout << eventId << endl;
        for (Int_t iAdc32 = 0; iAdc32 < fADC32->GetEntriesFast(); iAdc32++) {
            BmnADCDigit* adc32 = (BmnADCDigit*) fADC32->UncheckedAt(iAdc32);

            UInt_t channel = adc32->GetChannel();
            UInt_t serial = adc32->GetSerial();

            // Looking for a pair <channel, serial> in the correspondence map ...
            for (auto itCorr : idxChanSer) {
                Int_t channelTmp = itCorr.first.second; // ---> (0 ... 2048)

                Int_t channelBuff = Int_t(channelTmp / 32);
                Long_t serialBuff = itCorr.second;

                if (channel != channelBuff || serial != serialBuff)
                    continue;

                Int_t sample = Int_t(channelTmp % 32);
                Int_t signal = Int_t(digits[itCorr.first.first].GetStripSignal() * 16.);

                // Int_t signalExisting = adc32->GetShortValue()[sample];

                if (signal < INT16_MAX)
                    adc32->GetShortValue()[sample] += signal;
                else
                    adc32->GetShortValue()[sample] = INT16_MAX;

                //                cout << "BEFORE# " << endl;
                //                cout << "Channel# " << std::dec << channelBuff << " Sample# " << std::dec << sample << " Serial# " << std::hex << serialBuff <<
                //                        " SignalFromLambda# " << std::dec << signal << " SignalExisting# " << std::dec << signalExisting << endl;
                //
                //
                //                cout << "AFTER# " << endl;
                //                cout << "Channel# " << std::dec << channelBuff << " Sample# " << std::dec << sample << " Serial# " << std::hex << serialBuff <<
                //                        signal << " SignalExisting# " << std::dec << adc32->GetShortValue()[sample] << endl;
                //
                //
                adc32->SetAsEmbedded(kTRUE);
            }

            // Write ADC-digit to the output file ...            
            new ((*gem)[gem->GetEntriesFast()]) BmnADCDigit(adc32->GetSerial(), adc32->GetChannel(), adc32->GetNSamples(), adc32->GetShortValue(), adc32->IsEmbedded());
        }
        t->Fill();
    }

    delete ch;

    t->Write();
    f->Close();

    return tmp;
}

// Create store of possible primary lambdas to be used for embedding ...

void BmnLambdaEmbedding::CreateLambdaStore() {
    const Int_t lambdaPdg = 3122;

    for (Int_t iEntry = 0; iEntry < fNeventsForStore; iEntry++) {
        fSim->GetEntry(iEntry);
        // fLambdaStore->Delete();

        for (Int_t iTrack = 0; iTrack < fMCTracks->GetEntriesFast(); iTrack++) {
            CbmMCTrack* track = (CbmMCTrack*) fMCTracks->UncheckedAt(iTrack);

            // Get primary lambdas only ...
            if (track->GetPdgCode() != lambdaPdg || track->GetMotherId() != -1)
                continue;

            // Get lambdas in forward direction ...
            if (track->GetPz() < 0.)
                continue;

            Double_t p = track->GetP();
            Double_t Tx = track->GetPx() / track->GetPz();
            Double_t Ty = track->GetPy() / track->GetPz();

            new ((*fLambdaStore)[fLambdaStore->GetEntriesFast()]) BmnLambdaStore(p, Tx, Ty);
        }
    }
}

void BmnLambdaEmbedding::PrintStoreInfo() {
    cout << "N_lambdas_in_store# " << fLambdaStore->GetEntriesFast() << endl;

    for (Int_t iLambda = 0; iLambda < fLambdaStore->GetEntriesFast(); iLambda++) {
        BmnLambdaStore* lambda = (BmnLambdaStore*) fLambdaStore->UncheckedAt(iLambda);
        cout << "P = " << lambda->GetP() << " Eta = " << lambda->GetEta() << " Phi = " << lambda->GetPhi() << endl;
    }
}

BmnLambdaEmbedding::~BmnLambdaEmbedding() {
    delete fSim;
    delete fReco;

    delete fInfo;
}

void BmnLambdaEmbedding::SimulateLambdaPassing(Double_t P, TVector2 pos, TVector3 Vp, Int_t iLambda, Int_t iVertex) {
    // Here there is a list of arguments to be passed to the macro below ...
    Int_t nEvents = 10; // Num. of events to be simulated for each Vp and a lambda from the store 

    TString outFile = TString::Format("output/lambda%d_vertex%d.root", iLambda, iVertex);

    TString macroName = "SimulateLambdaPassing.C";

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathFull = gPathConfig + "/macro/embedding/" + macroName;

    TString argList = TString::Format("(%d, %G, %G, %G, %G, %G, %G, %G, \"%s\")", nEvents, fFieldScale, P, pos.X(), pos.Y(), Vp.X(), Vp.Y(), Vp.Z(), outFile.Data());
    TString exeCommand = TString::Format("root -b -q '" + gPathFull + argList + "'");

    gSystem->Exec(exeCommand.Data());
}

Int_t BmnLambdaEmbedding::DefineSiliconStatByZpoint(Double_t z) {
    // Boardings are valid for RUN7 only!!!
    const Double_t z1 = 14.;
    const Double_t z2 = 20.;

    Int_t shift = 6; // nGems in RUN7
    Int_t tmp;

    if (z < z1)
        tmp = 0;
    else if (z > z1 && z < z2)
        tmp = 1;
    else if (z > z2)
        tmp = 2;
    else
        return -1;

    return shift + tmp;
}






