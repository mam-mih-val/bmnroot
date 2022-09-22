#include "BmnLambdaEmbedding.h"
#include "CbmStsPoint.h"
#include "UniRun.h"

BmnLambdaEmbedding::BmnLambdaEmbedding() :
fSim(nullptr),
fLambdaSim(nullptr),
fReco(nullptr),
fMCTracks(nullptr),
fVertices(nullptr),
fLambdaStore(nullptr),
fHeader(nullptr),
fADC32(nullptr),
fADC128(nullptr),
fSync(nullptr),
fSiliconPoints(nullptr),
fSiliconDigits(nullptr),
fSiliconMatch(nullptr),
fGemPoints(nullptr),
fGemDigits(nullptr),
fGemMatch(nullptr),
fCscPoints(nullptr),
fCscDigits(nullptr),
fCscMatch(nullptr),
fInfo(nullptr),
fMon(nullptr) {

    // Useful tools 
    fInfo = new BmnLambdaMisc(); // Initialize useful tools to work with mapping ...
}

BmnLambdaEmbedding::BmnLambdaEmbedding(TString raw, TString sim, TString reco, TString out, Int_t nEvs, TString lambdaStore) :
fSim(nullptr),
fLambdaSim(nullptr),
fReco(nullptr),
fMCTracks(nullptr),
fVertices(nullptr),
fLambdaStore(nullptr),
fWrittenStores(nullptr),
fHeader(nullptr),
fADC32(nullptr),
fADC128(nullptr),
fSync(nullptr),
fSiliconPoints(nullptr),
fSiliconDigits(nullptr),
fSiliconMatch(nullptr),
fGemPoints(nullptr),
fGemDigits(nullptr),
fGemMatch(nullptr),
fCscPoints(nullptr),
fCscDigits(nullptr),
fCscMatch(nullptr),
fInfo(nullptr),
fMon(nullptr),
normUtils(nullptr),
fPdgDecParticle(3122),
fPdgPostive(2212), 
fPdgNegative(-211){
    fDataFileName = raw;
    fDigiFileName = out;
    fStorePath = "outputDataStore";

    // Initialize steering flags by default ...
    doLambdaStore = kTRUE;
    doListOfEventsWithReconstructedVertex = kTRUE;
    doSimulateLambdaThroughSetup = kTRUE;
    doRawRootConvertion = kTRUE;
    doEmbedding = kTRUE;
    doDecode = kTRUE;
    doPrintStoreInfo = kFALSE;
    doEmbeddingMonitor = kTRUE;

    isGemEmbedded = kTRUE;
    isSilEmbedded = kTRUE;
    isCscEmbedded = kFALSE;

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

    fNstores = 10;
    fNeventsToBeSimulated = 50;
    fStoreToProcess = -1;

    if (!lambdaStore.IsNull()) {
        fWrittenStores = new TClonesArray*[fNstores];
        for (Int_t iStore = 0; iStore < fNstores; iStore++)
            fWrittenStores[iStore] = nullptr;

        for (Int_t iStore = 0; iStore < fNstores; iStore++)
            fWrittenStores[iStore] = new TClonesArray("BmnParticleStore");

        TChain* ch = new TChain("bmndata");
        ch->Add(lambdaStore.Data());

        TClonesArray* tmp = nullptr;
        ch->SetBranchAddress("BmnParticleStore", &tmp);

        for (Int_t iStore = 0; iStore < fNstores; iStore++) {
            ch->GetEntry(iStore);

            fWrittenStores[iStore] = (TClonesArray*) (tmp->UncheckedAt(0))->Clone();
        }

        delete ch;
    } else
        fLambdaStore = new TClonesArray("BmnParticleStore");

    fRunId = -1;
    fFieldScale = -1.;

    // Default cut values ...
    fZmin = -3.;
    fZmax = +3.;
    fEtaMin = 0.;
    fEtaMax = 100.;
    fPhiMin = 0.;
    fPhiMax = 360.;
    fMomMin = 0.;
    fNHitsProton = 4;
    fNHitsPion = 4;
    isUseRealSignal = kTRUE;

    fSignal.push_back(1000);
    fSignal.push_back(1500);
    fSignal.push_back(1000);

    // Resetting default values of flags depending on an user's scenario if recognized
    // Scenario 1 (just selecting Lambdas ...)
    if ((!out.Contains("digi") && !out.IsNull()) && raw.IsNull() && !sim.IsNull() && reco.IsNull() && lambdaStore.IsNull()) {
        cout << "Recognized scenario 1" << endl;
        DoLambdaStore(kTRUE);
        DoPrintStoreInfo(kTRUE);
        SetUseRealSignals(kFALSE);
        DoListOfEventsWithReconstructedVertex(kFALSE);
        DoSimulateLambdaThroughSetup(kFALSE);
        DoRawRootConvertion(kFALSE);
        DoEmbeddingMonitor(kFALSE);
        DoEmbedding(kFALSE);
        DoDecode(kFALSE);
    }        // Scenario 2 (passing all stores (or selected one) in file to simulation ...)
    else if (raw.IsNull() && sim.IsNull() && !reco.IsNull() && out.IsNull() && !lambdaStore.IsNull()) {
        cout << "Recognized scenario 2 (or 3)" << endl;
        DoLambdaStore(kFALSE);
        DoPrintStoreInfo(kFALSE);
        SetUseRealSignals(kFALSE);
        DoSimulateLambdaThroughSetup(kTRUE);
        DoRawRootConvertion(kFALSE);
        DoEmbeddingMonitor(kFALSE);
        DoEmbedding(kFALSE);
        DoDecode(kFALSE);
    }
        // Scenario 4 (namely, embedding )
    else if (!raw.IsNull() && (out.Contains("digi") && !out.IsNull()) && sim.IsNull() && !reco.IsNull() && lambdaStore.IsNull()) {
        cout << "Recognized scenario 4" << endl;
        DoLambdaStore(kFALSE);
        DoPrintStoreInfo(kFALSE);
        DoRawRootConvertion(kTRUE);
        DoSimulateLambdaThroughSetup(kFALSE);
        SetUseRealSignals(kTRUE);
        DoEmbeddingMonitor(kTRUE);
        DoEmbedding(kTRUE);
        DoDecode(kTRUE);
    }
}

void BmnLambdaEmbedding::Embedding() {
    fFileNamePart = (fPdgDecParticle == 3122) ? "lambda" : (fPdgDecParticle == 310) ? "kaon" : "unknown"; 

    // 0. Get a function from current dst to be used for rescaling ...
    if (isUseRealSignal) {
        TString digiFile = "";

        TString eos_host = "root://ncm.jinr.ru/";
        TString digiDir = "/eos/nica/bmn/exp/digi/";
        TString run = "run7";
        TString release = "20.02.0";
        TString dirList[2] = {"4720-5186_BMN_Krypton", "3590-4707_BMN_Argon"};

        fReco->GetEntry();

        Int_t idx = (UniRun::GetRun(7, fHeader->GetRunId())->GetBeamParticle() == "Ar") ? 1 :
                (UniRun::GetRun(7, fHeader->GetRunId())->GetBeamParticle() == "Kr") ? 0 : -1;

        TString command = TString::Format("xrdcp %s/%s/%s/%s/%s/bmn_run%d_digi.root %s",
                eos_host.Data(), digiDir.Data(), run.Data(), release.Data(), dirList[idx].Data(), fHeader->GetRunId(), gSystem->GetFromPipe("pwd").Data());

        gSystem->Exec(command.Data());

        TFile* file = new TFile(Form("bmn_run%d_digi.root", fHeader->GetRunId()));

        if (file->IsOpen())
            digiFile = file->GetName();

        delete file;

        if (!digiFile.IsNull())
            normUtils = new SignalNormalizationUtils(digiFile, TString(fReco->GetFile()->GetName()));
    }

    // 1. Create a store with lambdas ...
    if (doLambdaStore) {
        if (!fDigiFileName.Contains("digi") && !fDigiFileName.IsNull()) {
            TClonesArray* store = CreateLambdaStore();

            TFile* f = new TFile(fDigiFileName.Data(), "recreate");
            TTree* tree = new TTree("bmndata", "bmndata");

            TClonesArray* tmp = new TClonesArray("BmnParticleStore");
            tree->Branch("BmnParticleStore", &tmp);

            for (Int_t iEntry = 0; iEntry < store->GetEntriesFast(); iEntry++) {
                tmp->Delete();
                TClonesArray* arr = (TClonesArray*) store->UncheckedAt(iEntry);

                new ((*tmp)[tmp->GetEntriesFast()]) BmnParticleStore(*((BmnParticleStore*) arr));
                tree->Fill();
            }

            tree->Write();

            delete tmp;
            delete tree;
            delete f;

        } else
            CreateLambdaStore();
    }

    if (doPrintStoreInfo)
        PrintStoreInfo();

    // 2. Create list of eventId for reconstructed events where the primary vertex is assumed to be defined ...
    map <UInt_t, TVector3> EventIdsVpMap;
    if (doListOfEventsWithReconstructedVertex)
        EventIdsVpMap = ListOfEventsWithReconstructedVp();

    // 3. Get information on mag. field in the run ...
    if (doSimulateLambdaThroughSetup) {
        if (fRunId != -1) {
            Double_t map_current = 55.87;
            UniRun* runInfo = UniRun::GetRun(7, fRunId);
            if (runInfo)
                fFieldScale = *runInfo->GetFieldVoltage() / map_current;
        }
    }

    // 4. Loop over store with lambdas ...
    Int_t nThreads = 1;
#if defined(_OPENMP)
    nThreads = fNstores;
    omp_set_num_threads(nThreads);
#endif

    if (doSimulateLambdaThroughSetup) {
        Int_t low = (fStoreToProcess != -1) ? fStoreToProcess : 0;
        Int_t up = (fStoreToProcess != -1) ? (fStoreToProcess + 1) : fNstores;

#pragma omp parallel for
        for (Int_t iLambda = low; iLambda < up; iLambda++) {
            BmnParticleStore* lambda = (BmnParticleStore*) fWrittenStores[iLambda];
            Double_t eta = lambda->GetEta();
            Double_t phi = lambda->GetPhi();
            Double_t p = lambda->GetP();
            Double_t pdg = lambda->GetPdg();

            Int_t iVertex = 0;

            for (auto it : EventIdsVpMap) {
                // Get reconstructed primary vertex ...
                TVector3 Vp = it.second;

                SimulateLambdaPassing(pdg, p, TVector2(eta, phi), Vp, iLambda, iVertex);

                iVertex++;
            }
        }
    }

    // 5. Find at least one lambda to be reconstructed for a given Vp
    map <UInt_t, pair <TVector3, TVector3>> GoodLambdaForEachVertex; // <evId ---> (<iLambda, iVertex, iEvent>, <Vp>)

    map <UInt_t, BmnParticlePair> embMonitorMap; // evId --> particlePair (see physics/particles/BmnParticlePair.h(*.cxx))

    Int_t iVertex = 0;
    for (auto it : EventIdsVpMap) {
        UInt_t id = it.first;
        TVector3 lambda(-1, -1, -1); // <iLambda, iVertex, iEvent>, no appropriate any lambda found yet for current eventId ...
        TVector3 Vp = it.second;

        GoodLambdaForEachVertex[id] = make_pair(lambda, Vp); // Put info for a given eventId

        BmnParticlePair aPairToBeWritten;
        embMonitorMap[id] = aPairToBeWritten;

        for (Int_t iLambda = 0; iLambda < fNstores; iLambda++) {
            BmnParticlePair currentPair;

            Int_t eve = FindReconstructableLambdaFromStore(iLambda, iVertex, currentPair);

            // Put current pair into the embMonitorMap ...
            auto itEmbInfoMap = embMonitorMap.find(id);
            if (itEmbInfoMap != embMonitorMap.end())
                (*itEmbInfoMap).second = currentPair;

            if (eve != -1) {
                TVector3 tmp = TVector3(iLambda, iVertex, eve);

                // Trying to find id to be replaced ...
                auto itr = GoodLambdaForEachVertex.find(id);
                if (itr != GoodLambdaForEachVertex.end())
                    (*itr).second.first = tmp;

                break;
            }
        }
        iVertex++;
    }

    // 6. Monitor events with embedded lambdas ...
    if (doEmbeddingMonitor) {
        fMon = new BmnLambdaEmbeddingMonitor();
        TFile* file = new TFile(Form("embedding_%d.root", fRunId), "recreate");
        TTree* tree = new TTree("bmndata", "bmndata");

        tree->Branch("EmbeddingMonitor.", &fMon);
        for (auto it : GoodLambdaForEachVertex) {
            UInt_t id = it.first;

            TVector3 par = it.second.first;

            fMon->SetId(id);
            fMon->SetStoreVertexEvent(par);

            auto itEmbInfoMap = embMonitorMap.find(id);
            // if (itEmbInfoMap != embMonitorMap.end()) {
            BmnParticlePair pair = (*itEmbInfoMap).second;

            fMon->SetProtonP(pair.GetMomPart1());
            fMon->SetPionP(pair.GetMomPart2());

            fMon->SetTxProton(pair.GetTxPart1());
            fMon->SetTxPion(pair.GetTxPart2());

            fMon->SetTyProton(pair.GetTyPart1());
            fMon->SetTyPion(pair.GetTyPart2());

            fMon->SetNHitsProton(pair.GetNHitsPart1());
            fMon->SetNHitsPion(pair.GetNHitsPart2());
            // }

            if ((Int_t) par[0] != -1 && (Int_t) par[1] != -1 && (Int_t) par[2] != -1)
                fMon->IsEmbedded(kTRUE);

            else
                fMon->IsEmbedded(kFALSE);

            tree->Fill();
        }

        tree->Write();
        file->Close();
    }

    // 7. Create digi-arrays corresponding to a certain eventId ... <evId --> lambdaDigs>
    map <UInt_t, vector < BmnStripDigit>> digsFromLambdasGem; // GEM
    map <UInt_t, vector < BmnStripDigit>> digsFromLambdasSilicon; // SILICON
    map <UInt_t, vector < BmnStripDigit>> digsFromLambdasCsc; // CSC

    if (doEmbedding)
        for (auto it : GoodLambdaForEachVertex) {
            TVector3 par = it.second.first;
            Int_t lambda = par[0];
            Int_t vertex = par[1];
            Int_t event = par[2];

            if (lambda == -1 || vertex == -1 || event == -1)
                continue;

            if (isGemEmbedded)
                digsFromLambdasGem[it.first] = GetDigitsFromLambda(TString::Format("%s/%s%d_vertex%d.root", fStorePath.Data(), fFileNamePart.Data(), lambda, vertex), event, "GEM");
            if (isSilEmbedded)
                digsFromLambdasSilicon[it.first] = GetDigitsFromLambda(TString::Format("%s/%s%d_vertex%d.root", fStorePath.Data(), fFileNamePart.Data(), lambda, vertex), event, "SILICON");
            if (isCscEmbedded)
                digsFromLambdasCsc[it.first] = GetDigitsFromLambda(TString::Format("%s/%s%d_vertex%d.root", fStorePath.Data(), fFileNamePart.Data(), lambda, vertex), event, "CSC");
        }

    cout << "GEM# " << digsFromLambdasGem.size() << " SILICON# " << digsFromLambdasSilicon.size() << " CSC# " << digsFromLambdasCsc.size() << endl;

    // 7a. Collecting all MC-digits together ...
    if (isUseRealSignal) {
        TTree* tree = new TTree("bmndata", "bmndata");

        TClonesArray* digitsGem = new TClonesArray("BmnGemStripDigit");
        tree->Branch("BmnGemStripDigit", &digitsGem);

        TClonesArray* digitsSil = new TClonesArray("BmnSiliconDigit");
        tree->Branch("BmnSiliconDigit", &digitsSil);

        for (auto digs : digsFromLambdasGem) {
            digitsGem->Delete();

            vector <BmnStripDigit> digitsInEvent = digs.second;

            for (BmnStripDigit digit : digitsInEvent) {
                BmnStripDigit* dig = &digit;

                new ((*digitsGem) [digitsGem->GetEntriesFast()]) BmnStripDigit(dig);
            }
            tree->Fill();
        }

        for (auto digs : digsFromLambdasSilicon) {
            digitsSil->Delete();

            vector <BmnStripDigit> digitsInEvent = digs.second;

            for (BmnStripDigit digit : digitsInEvent) {
                BmnStripDigit* dig = &digit;

                new ((*digitsSil) [digitsSil->GetEntriesFast()]) BmnStripDigit(dig);
            }
            tree->Fill();
        }

        normUtils->SetMcDataSet((TChain*) tree);
        delete tree;
        delete digitsGem;
        delete digitsSil;

        // Recalculating all signals according to the scal function got ...
        TF1* scaleFuncGem = normUtils->GetRescaleFunction("GEM");
        TF1* scaleFuncSil = normUtils->GetRescaleFunction("SILICON");

        if (!scaleFuncGem || !scaleFuncSil) {
            cout << "Something is wrong, exiting ..." << endl;
            throw;
        }

        for (auto& digs : digsFromLambdasGem) {
            vector <BmnStripDigit> &digitsInEvent = digs.second;

            for (BmnStripDigit& digit : digitsInEvent)
                digit.SetStripSignal(scaleFuncGem->Eval(digit.GetStripSignal()));
        }

        for (auto& digs : digsFromLambdasSilicon) {
            vector <BmnStripDigit> &digitsInEvent = digs.second;

            for (BmnStripDigit& digit : digitsInEvent)
                digit.SetStripSignal(scaleFuncSil->Eval(digit.GetStripSignal()));
        }
    }

    // 8. Make correspondence between evId and lambda digits with info on channel and serial ...
    map <UInt_t, map < pair <Int_t, Int_t>, Long_t>> evIdGemChannelSerial; // GEM     --- <evId ---> <digi index + ch, serial>>
    map <UInt_t, map < pair <Int_t, Int_t>, Long_t>> evIdCscChannelSerial; // CSC     --- <evId ---> <digi index + ch, serial>>
    map <UInt_t, map < vector <Int_t>, Long_t>> evIdSiliconChannelSerial; // SILICON --- <evId ---> <digi index + ch + sample, serial>>

    if (doEmbedding) {
        // GEM
        if (isGemEmbedded)
            for (auto it : digsFromLambdasGem) {
                vector <BmnStripDigit> digits = it.second;
                map <pair <Int_t, Int_t>, Long_t> tmpMap = GetGemChannelSerialFromDigi(digits);
                evIdGemChannelSerial[it.first] = tmpMap;
            }

        // SILICON
        if (isSilEmbedded)
            for (auto it : digsFromLambdasSilicon) {
                vector <BmnStripDigit> digits = it.second;
                map <vector <Int_t>, Long_t> tmpMap = GetSiliconChannelSerialFromDigi(digits);
                evIdSiliconChannelSerial[it.first] = tmpMap;
            }

        // CSC
        if (isCscEmbedded)
            for (auto it : digsFromLambdasCsc) {
                vector <BmnStripDigit> digits = it.second;
                map <pair <Int_t, Int_t>, Long_t> tmpMap = GetCscChannelSerialFromDigi(digits);
                evIdCscChannelSerial[it.first] = tmpMap;
            }
    }

    // 9. Do *.data --> *raw.root convertion for requested events ...
    DoRawRootFromBinaryData(fDataFileName);

    // 10. Loop over *raw.root to embed lambda digits ...
    TString rawRoot = "";
    if (doEmbedding)
        rawRoot = AddInfoToRawFile(digsFromLambdasGem, evIdGemChannelSerial,
            digsFromLambdasSilicon, evIdSiliconChannelSerial,
            digsFromLambdasCsc, evIdCscChannelSerial);

    // 11. Start decoding ...
    if (!rawRoot.IsNull() && doDecode)
        StartDecodingWithEmbeddedLambdas(rawRoot);
}

Int_t BmnLambdaEmbedding::FindReconstructableLambdaFromStore(Int_t iLambda, Int_t iVertex, BmnParticlePair& pairFromLambda) {
    TString fileName = TString::Format("%s/%s%d_vertex%d.root", fStorePath.Data(), fFileNamePart.Data(), iLambda, iVertex);
    TChain ch("bmndata");
    if (ch.Add(fileName.Data()) == 0)
        return -1;

    // Open lambda file
    TClonesArray* tracks = nullptr;
    TClonesArray* gemPoints = nullptr;
    TClonesArray* siliconPoints = nullptr;
    TClonesArray* cscPoints = nullptr;

    ch.SetBranchAddress("MCTrack", &tracks);
    ch.SetBranchAddress("StsPoint", &gemPoints);
    ch.SetBranchAddress("SiliconPoint", &siliconPoints);
    ch.SetBranchAddress("CSCPoint", &cscPoints);

    for (Int_t iEv = 0; iEv < ch.GetEntries(); iEv++) {
        ch.GetEntry(iEv);

        // Loop over protons ... 
        // A proton we are looking for is single ! 
        Int_t nHitsPerProton = 0;
        Double_t Pprot = 0., TxProt = -1., TyProt = -1.;

        for (Int_t iTrack = 0; iTrack < tracks->GetEntriesFast(); iTrack++) {
            CbmMCTrack* track = (CbmMCTrack*) tracks->UncheckedAt(iTrack);

            // Skip primary tracks
            if (track->GetMotherId() == -1)
                continue;

            // Skipping particles not been protons
            if (track->GetPdgCode() != fPdgPostive)
                continue;

            Int_t id = track->GetMotherId();

            // Get secondary protons and pions from lambda decay
            if (((CbmMCTrack*) tracks->UncheckedAt(id))->GetPdgCode() != fPdgDecParticle)
                continue;

            if (track->GetP() < 0.5)
                continue;

            const Int_t nStats = 10; // 6 GEMs + 3 SILICONs + 1 CSC [<0 ... 5>, <6, 7, 8>, <9>]
            Int_t pointsOnGemsSiliconsAndCsc[nStats];
            for (Int_t iStat = 0; iStat < nStats; iStat++)
                pointsOnGemsSiliconsAndCsc[iStat] = 0;

            // Loop over gem points ...
            if (isGemEmbedded)
                for (Int_t iGem = 0; iGem < gemPoints->GetEntriesFast(); iGem++) {
                    CbmStsPoint* point = (CbmStsPoint*) gemPoints->UncheckedAt(iGem);
                    if (point->GetTrackID() != iTrack)
                        continue;
                    pointsOnGemsSiliconsAndCsc[point->GetStation()]++;
                }

            // Loop over silicon points ...
            if (isSilEmbedded)
                for (Int_t iSilicon = 0; iSilicon < siliconPoints->GetEntriesFast(); iSilicon++) {
                    FairMCPoint* point = (FairMCPoint*) siliconPoints->UncheckedAt(iSilicon);
                    if (point->GetTrackID() != iTrack)
                        continue;
                    pointsOnGemsSiliconsAndCsc[DefineSiliconStatByZpoint(point->GetZ())]++;
                }

            // Loop over csc points ...
            if (isCscEmbedded)
                for (Int_t iCsc = 0; iCsc < cscPoints->GetEntriesFast(); iCsc++) {
                    BmnCSCPoint* point = (BmnCSCPoint*) cscPoints->UncheckedAt(iCsc);
                    if (point->GetTrackID() != iTrack)
                        continue;
                    pointsOnGemsSiliconsAndCsc[9]++;
                }

            // Here we decide on a track whether being accepted or not according a list of detectors to be used when embedding MC-data ...
            Bool_t isPrelimAccepted[3] = {kTRUE, kTRUE, kTRUE}; // GEM, SILICON, CSC ...
            Int_t pointCounter;

            // Checking GEMs ...
            if (isGemEmbedded) {
                pointCounter = 0;
                for (Int_t iStat = 0; iStat < 6; iStat++) {
                    if (pointsOnGemsSiliconsAndCsc[iStat] != 0)
                        pointCounter++;
                }
                if (pointCounter == 0)
                    isPrelimAccepted[0] = kFALSE;
            }

            // Checking SILICONs ...
            if (isSilEmbedded) {
                pointCounter = 0;
                for (Int_t iStat = 6; iStat < 9; iStat++) {
                    if (pointsOnGemsSiliconsAndCsc[iStat] != 0)
                        pointCounter++;
                }
                if (pointCounter == 0)
                    isPrelimAccepted[1] = kFALSE;
            }

            // Checking CSC ...
            if (isCscEmbedded) {
                pointCounter = 0;
                for (Int_t iStat = 9; iStat < nStats; iStat++) {
                    if (pointsOnGemsSiliconsAndCsc[iStat] != 0)
                        pointCounter++;
                }
                if (pointCounter == 0)
                    isPrelimAccepted[2] = kFALSE;
            }

            Bool_t doWeAcceptEvent = (isPrelimAccepted[0] * isPrelimAccepted[1] * isPrelimAccepted[2]) ? kTRUE : kFALSE;

            if (!doWeAcceptEvent)
                continue;

            // Loop over statArray to avoid multiple Z ...
            Bool_t isMultiplePointsOnZ = kFALSE;
            for (Int_t iStat = 0; iStat < nStats; iStat++) {
                if (pointsOnGemsSiliconsAndCsc[iStat] == 2) {
                    isMultiplePointsOnZ = kTRUE;
                    break;
                } else
                    nHitsPerProton += pointsOnGemsSiliconsAndCsc[iStat];
            }
            if (isMultiplePointsOnZ || nHitsPerProton < 4 || nHitsPerProton > nStats)
                continue;

            Pprot = track->GetP();
            TxProt = track->GetPx() / track->GetPz();
            TyProt = track->GetPy() / track->GetPz();
        }

        // Skipping the next loop if not satisfied condition ...      
        if (nHitsPerProton < fNHitsProton || Abs(Pprot) < FLT_EPSILON)
            continue;

        // Loop over pions ... 
        Int_t nHitsPerPion = 0;
        Double_t Ppion = 0., TxPion = -1., TyPion = -1.;

        for (Int_t jTrack = 0; jTrack < tracks->GetEntriesFast(); jTrack++) {
            CbmMCTrack* track = (CbmMCTrack*) tracks->UncheckedAt(jTrack);

            // Skip primary tracks
            if (track->GetMotherId() == -1)
                continue;

            // Skipping particles not been pions ...
            if (track->GetPdgCode() != fPdgNegative)
                continue;

            Int_t id = track->GetMotherId();

            // Get secondary protons and pions from lambda decay
            if (((CbmMCTrack*) tracks->UncheckedAt(id))->GetPdgCode() != fPdgDecParticle)
                continue;

            if (track->GetP() < 0.5)
                continue;

            const Int_t nStats = 9; // 6 GEMs + 3 SILICONs [<0 ... 5>, <6, 7, 8>]
            Int_t pointsOnGemsAndSilicon[nStats];
            for (Int_t iStat = 0; iStat < nStats; iStat++)
                pointsOnGemsAndSilicon[iStat] = 0;

            // Loop over gem points ...
            if (isGemEmbedded)
                for (Int_t iGem = 0; iGem < gemPoints->GetEntriesFast(); iGem++) {
                    CbmStsPoint* point = (CbmStsPoint*) gemPoints->UncheckedAt(iGem);
                    if (point->GetTrackID() != jTrack)
                        continue;
                    pointsOnGemsAndSilicon[point->GetStation()]++;
                }

            // Loop over silicon points ...
            if (isSilEmbedded)
                for (Int_t iSilicon = 0; iSilicon < siliconPoints->GetEntriesFast(); iSilicon++) {
                    FairMCPoint* point = (FairMCPoint*) siliconPoints->UncheckedAt(iSilicon);
                    if (point->GetTrackID() != jTrack)
                        continue;
                    pointsOnGemsAndSilicon[DefineSiliconStatByZpoint(point->GetZ())]++;
                }

            // Here we decide on a track whether being accepted or not according a list of detectors to be used when embedding MC-data ...
            Bool_t isPrelimAccepted[2] = {kTRUE, kTRUE}; // GEM, SILICON
            Int_t pointCounter;

            // Checking GEMs ...
            if (isGemEmbedded) {
                pointCounter = 0;
                for (Int_t iStat = 0; iStat < 6; iStat++) {
                    if (pointsOnGemsAndSilicon[iStat] != 0)
                        pointCounter++;
                }
                if (pointCounter == 0)
                    isPrelimAccepted[0] = kFALSE;
            }

            // Checking SILICONs ...
            if (isSilEmbedded) {
                pointCounter = 0;
                for (Int_t iStat = 6; iStat < nStats; iStat++) {
                    if (pointsOnGemsAndSilicon[iStat] != 0)
                        pointCounter++;
                }
                if (pointCounter == 0)
                    isPrelimAccepted[1] = kFALSE;
            }

            Bool_t doWeAcceptEvent = (isPrelimAccepted[0] * isPrelimAccepted[1]) ? kTRUE : kFALSE;

            if (!doWeAcceptEvent)
                continue;

            // Loop over statArray to avoid multiple Z ... 
            Bool_t isMultiplePointsOnZ = kFALSE;
            for (Int_t iStat = 0; iStat < nStats; iStat++) {
                if (pointsOnGemsAndSilicon[iStat] == 2) {
                    isMultiplePointsOnZ = kTRUE;
                    break;
                } else
                    nHitsPerPion += pointsOnGemsAndSilicon[iStat];
            }
            if (isMultiplePointsOnZ || nHitsPerPion < 4 || nHitsPerPion > nStats)
                continue;

            Ppion = track->GetP();
            TxPion = track->GetPx() / track->GetPz();
            TyPion = track->GetPy() / track->GetPz();
        }

        if (nHitsPerPion < fNHitsPion || Abs(Ppion) < FLT_EPSILON)
            continue;

        pairFromLambda.SetMomPair(Pprot, Ppion);
        pairFromLambda.SetTxPair(TxProt, TxPion);
        pairFromLambda.SetTyPair(TyProt, TyPion);
        pairFromLambda.SetNHitsPair(nHitsPerProton, nHitsPerPion);

        return iEv;
    }
    return -1;
}

void BmnLambdaEmbedding::DoRawRootFromBinaryData(TString raw) {
    BmnRawDataDecoder* decoder = new BmnRawDataDecoder(raw, "", fEvents);
    decoder->SetPeriodId(7);
    decoder->SetRunId(fRunId);

    if (doRawRootConvertion)
        decoder->ConvertRawToRoot();
    fRawRootFileName = decoder->GetRootFileName();

    delete decoder;
}

void BmnLambdaEmbedding::StartDecodingWithEmbeddedLambdas(TString raw) {
    BmnRawDataDecoder* decoder = new BmnRawDataDecoder("", "", fEvents);
    decoder->SetPeriodId(7);
    decoder->SetRunId(fRunId);
    decoder->SetRawRootFile(raw);
    decoder->SetDigiRootFile(fDigiFileName);
    decoder->SetBmnSetup(kBMNSETUP);

    std::map<DetectorId, bool> setup; // flags to determine BM@N setup
    setup.insert(std::make_pair(kBC,        0)); // TRIGGERS
    setup.insert(std::make_pair(kSILICON,   1)); // SILICON
    setup.insert(std::make_pair(kGEM,       1)); // GEM

    decoder->SetDetectorSetup(setup);
    decoder->SetAdcDecoMode(kBMNADCMK);

    decoder->SetMSCMapping("MSC_map_Run7.txt");
    decoder->SetTrigPlaceMapping("Trig_PlaceMap_Run7.txt");
    decoder->SetTrigChannelMapping("Trig_map_Run7.txt");
    decoder->SetSiliconMapping("SILICON_map_run7.txt");
    decoder->SetGemMapping("GEM_map_run7.txt");
    decoder->SetCSCMapping("CSC_map_period7.txt");

    decoder->InitMaps();

    decoder->DecodeDataToDigi();

    delete decoder;
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

vector <BmnStripDigit> BmnLambdaEmbedding::GetDigitsFromLambda(TString lambdaEve, Int_t evNum, TString det) {
    Bool_t isSilicon = (det.Contains("SILICON")) ? kTRUE : kFALSE;
    Bool_t isGem = (det.Contains("GEM")) ? kTRUE : kFALSE;
    Bool_t isCsc = (det.Contains("CSC")) ? kTRUE : kFALSE;

    vector <BmnStripDigit> digits;

    // Open file with simulated lambdas
    fLambdaSim = new TChain("bmndata");
    fLambdaSim->Add(lambdaEve.Data());
    fLambdaSim->SetBranchAddress("MCTrack", &fMCTracks);

    fLambdaSim->SetBranchAddress("SiliconPoint", &fSiliconPoints);
    fLambdaSim->SetBranchAddress("BmnSiliconDigit", &fSiliconDigits);
    fLambdaSim->SetBranchAddress("BmnSiliconDigitMatch", &fSiliconMatch);

    fLambdaSim->SetBranchAddress("StsPoint", &fGemPoints);
    fLambdaSim->SetBranchAddress("BmnGemStripDigit", &fGemDigits);
    fLambdaSim->SetBranchAddress("BmnGemStripDigitMatch", &fGemMatch);

    fLambdaSim->SetBranchAddress("CSCPoint", &fCscPoints);
    fLambdaSim->SetBranchAddress("BmnCSCDigit", &fCscDigits);
    fLambdaSim->SetBranchAddress("BmnCSCDigitMatch", &fCscMatch);

    fLambdaSim->GetEntry(evNum);

    Int_t idxPi = -1, idxProt = -1;

    // Looking for protons and pions from lambda decay
    for (Int_t iTrack = 0; iTrack < fMCTracks->GetEntriesFast(); iTrack++) {
        CbmMCTrack* track = (CbmMCTrack*) fMCTracks->UncheckedAt(iTrack);

        // Skip primary tracks
        if (track->GetMotherId() == -1)
            continue;

        // Skip particles not been protons or pions
        if (track->GetPdgCode() != fPdgPostive && track->GetPdgCode() != fPdgNegative)
            continue;

        Int_t pdg = track->GetPdgCode();
        Int_t id = track->GetMotherId();

        // Get secondary protons and pions from lambda decay
        if (((CbmMCTrack*) fMCTracks->UncheckedAt(id))->GetPdgCode() != fPdgDecParticle)
            continue;

        if (pdg > 0)
            idxProt = iTrack;
        else
            idxPi = iTrack;
    }

    // Loop over GEM-digits
    if (isGem)
        for (Int_t iDig = 0; iDig < fGemDigits->GetEntriesFast(); iDig++) {
            BmnMatch* digiMatch = (BmnMatch*) fGemMatch->UncheckedAt(iDig);

            Int_t idxPoint = digiMatch->GetMatchedLink().GetIndex();
            FairMCPoint* point = (FairMCPoint*) fGemPoints->UncheckedAt(idxPoint);
            if (point->GetTrackID() != idxProt && point->GetTrackID() != idxPi)
                continue;

            BmnStripDigit* digi = (BmnStripDigit*) fGemDigits->UncheckedAt(iDig);
            // digi->SetName((point->GetTrackID() == idxProt) ? "PROTON" : "PION");
            digits.push_back(*digi);
        }

    // Loop over CSC-digits
    if (isCsc)
        for (Int_t iDig = 0; iDig < fCscDigits->GetEntriesFast(); iDig++) {
            BmnMatch* digiMatch = (BmnMatch*) fCscMatch->UncheckedAt(iDig);

            Int_t idxPoint = digiMatch->GetMatchedLink().GetIndex();
            FairMCPoint* point = (FairMCPoint*) fCscPoints->UncheckedAt(idxPoint);
            if (point->GetTrackID() != idxProt && point->GetTrackID() != idxPi)
                continue;

            BmnStripDigit* digi = (BmnStripDigit*) fCscDigits->UncheckedAt(iDig);
            digits.push_back(*digi);
        }

    // Loop over SILICON-digits
    if (isSilicon)
        for (Int_t iDig = 0; iDig < fSiliconDigits->GetEntriesFast(); iDig++) {
            BmnMatch* digiMatch = (BmnMatch*) fSiliconMatch->UncheckedAt(iDig);

            Int_t idxPoint = digiMatch->GetMatchedLink().GetIndex();
            FairMCPoint* point = (FairMCPoint*) fSiliconPoints->UncheckedAt(idxPoint);
            if (point->GetTrackID() != idxProt && point->GetTrackID() != idxPi)
                continue;

            BmnStripDigit* digi = (BmnStripDigit*) fSiliconDigits->UncheckedAt(iDig);
            digits.push_back(*digi);
        }

    delete fLambdaSim;

    return digits;
}

map <pair <Int_t, Int_t>, Long_t> BmnLambdaEmbedding::GetGemChannelSerialFromDigi(vector <BmnStripDigit> digits) {
    map <Int_t, Int_t> digiToChannel; // (digi index in vector ---> channel)
    map <pair <Int_t, Int_t>, Long_t> digiChannelToSerial; // (digi index in vector + channel ---> serial)

    const Int_t nDigs = digits.size();

    // Fill digiToChannel map ...
    Long_t serial[nDigs];

    for (Int_t iDigi = 0; iDigi < nDigs; iDigi++) {
        serial[iDigi] = 0x0;
        BmnStripDigit* dig = &digits[iDigi];
        digiToChannel[iDigi] = fInfo->GemDigiToChannel(dig, serial[iDigi]);
    }

    // Fill digiChannelToSerial map ...
    for (auto it : digiToChannel) {

        // Skip digits with no channel found ...
        if (it.second == -1)
            continue;

        Long_t value = (serial[it.first] == 0) ? fInfo->GemDigiChannelToSerial(make_pair(digits[it.first], it.second)) : serial[it.first];
        digiChannelToSerial[make_pair(it.first, it.second)] = value;
    }

    return digiChannelToSerial;
}

map <pair <Int_t, Int_t>, Long_t> BmnLambdaEmbedding::GetCscChannelSerialFromDigi(vector <BmnStripDigit> digits) {
    map <pair <Int_t, Int_t>, Long_t> digiChannelToSerial; // (digi index in vector + channel ---> serial)

    for (Int_t iDigi = 0; iDigi < digits.size(); iDigi++) {
        BmnStripDigit* dig = &digits[iDigi];

        Long_t serial = 0x0;

        digiChannelToSerial[make_pair(iDigi, fInfo->CscDigiToChannel(dig, serial))] = serial;
    }

    return digiChannelToSerial;
}

map <vector <Int_t>, Long_t> BmnLambdaEmbedding::GetSiliconChannelSerialFromDigi(vector <BmnStripDigit> digits) {
    map <vector <Int_t>, Long_t> digiChannelToSerial; // (digi index in vector + channel + sample ---> serial)

    for (Int_t iDigi = 0; iDigi < digits.size(); iDigi++) {
        BmnStripDigit* dig = &digits[iDigi];

        Int_t chan = -1, sample = -1;
        Long_t serial = 0x0;

        // Get channel, sample and serial information ...
        fInfo->SiliconDigiToChannelSampleSerial(dig, chan, sample, serial);
        vector <Int_t> tmp{iDigi, chan, sample};

        digiChannelToSerial[tmp] = serial;
    }

    return digiChannelToSerial;
}

TString BmnLambdaEmbedding::AddInfoToRawFile(map <UInt_t, vector <BmnStripDigit>> digsGem, map <UInt_t, map <pair <Int_t, Int_t>, Long_t>> evIdChannelSerialGem,
        map <UInt_t, vector < BmnStripDigit>> digsSilicon, map <UInt_t, map < vector <Int_t>, Long_t>> evIdChannelSerialSilicon,
        map <UInt_t, vector <BmnStripDigit>> digsCsc, map <UInt_t, map <pair <Int_t, Int_t>, Long_t>> evIdChannelSerialCsc) {

    // Create output file ... 
    TString tmp = fRawRootFileName;
    tmp = tmp.ReplaceAll(".root", "");
    tmp += "_withLambdaEmbedded.root";

    TFile* f = new TFile(tmp.Data(), "recreate");
    TTree* t = new TTree("BMN_RAW", "BMN_RAW");

    TClonesArray* gemOrCsc = new TClonesArray("BmnADCDigit");
    TClonesArray* silicon = new TClonesArray("BmnADCDigit");
    TClonesArray* sync = new TClonesArray("BmnSyncDigit");

    BmnEventHeader* header = new BmnEventHeader();

    t->Branch("BmnEventHeader.", &header);
    t->Branch("SYNC", &sync);
    t->Branch("ADC32", &gemOrCsc);
    t->Branch("ADC128", &silicon);

    TChain* ch = new TChain("BMN_RAW");
    ch->Add(fRawRootFileName.Data());

    BmnEventHeader* fEventHeader = nullptr;

    ch->SetBranchAddress("ADC32", &fADC32);
    ch->SetBranchAddress("ADC128", &fADC128);
    ch->SetBranchAddress("SYNC", &fSync);
    ch->SetBranchAddress("BmnEventHeader.", &fEventHeader);

    // Loop over existing *raw.root file ...
    for (Int_t iEntry = 0; iEntry < fEvents; iEntry++) {
        if (iEntry % 1000 == 0)
            cout << "Embedding, event# " << iEntry << endl;
        // Clear TClonesArrays from the previous event ...
        gemOrCsc->Delete();
        silicon->Delete();
        sync->Delete();

        ch->GetEntry(iEntry);

        UInt_t eventId = fEventHeader->GetEventId();

        header->SetRunId(fEventHeader->GetRunId());
        header->SetEventId(eventId);
        header->SetEventType(fEventHeader->GetEventType());

        // Get SYNC digits ....
        for (Int_t iSync = 0; iSync < fSync->GetEntriesFast(); iSync++) {
            BmnSyncDigit* dig = (BmnSyncDigit*) fSync->UncheckedAt(iSync);

            new ((*sync)[sync->GetEntriesFast()]) BmnSyncDigit(dig->GetSerial(), dig->GetEvent(), dig->GetTime_sec(), dig->GetTime_ns());
        }

        // Looking for current eventId in the digi-map ...
        // EventId should be the same for three maps (digsGem, digsSilicon, digsCsc)
        Bool_t isEventIdFoundInTheMap = kTRUE;

        auto itDigGem = digsGem.find(eventId);
        auto itDigSilicon = digsSilicon.find(eventId);
        auto itDigCsc = digsCsc.find(eventId);

        if (itDigGem == digsGem.end())
            isEventIdFoundInTheMap = kFALSE;

        // Array with digits for certain eventId
        // GEM
        vector <BmnStripDigit> digitsGem;
        map <pair <Int_t, Int_t>, Long_t> idxChanSerGem;

        // SILICON
        vector <BmnStripDigit> digitsSilicon;
        map <vector <Int_t>, Long_t> idxChanSampleSerSilicon;

        // CSC
        vector <BmnStripDigit> digitsCsc;
        map <pair <Int_t, Int_t>, Long_t> idxChanSerCsc;

        if (isEventIdFoundInTheMap) {
            // GEM
            if (isGemEmbedded) {
                digitsGem = itDigGem->second;
                idxChanSerGem = evIdChannelSerialGem.find(eventId)->second;
            }

            // SILICON
            if (isSilEmbedded) {
                digitsSilicon = itDigSilicon->second;
                idxChanSampleSerSilicon = evIdChannelSerialSilicon.find(eventId)->second;
            }

            // CSC
            if (isCscEmbedded) {
                digitsCsc = itDigCsc->second;
                idxChanSerCsc = evIdChannelSerialCsc.find(eventId)->second;
            }
        }

        // GEM and CSC
        if (isGemEmbedded || isCscEmbedded)
            for (Int_t iAdc32 = 0; iAdc32 < fADC32->GetEntriesFast(); iAdc32++) {
                BmnADCDigit* adc32 = (BmnADCDigit*) fADC32->UncheckedAt(iAdc32);

                UInt_t channel = adc32->GetChannel(); // ---> (0 ... 63)
                UInt_t serial = adc32->GetSerial();

                // GEM: 
                // Looking for a pair <channel, serial> in the correspondence map ...
                if (isGemEmbedded)
                    for (auto itCorr : idxChanSerGem) {
                        Int_t channelTmp = itCorr.first.second; // ---> (0 ... 2047)

                        Int_t channelBuff = Int_t(channelTmp / 32);
                        Long_t serialBuff = itCorr.second;

                        if (channel != channelBuff || serial != serialBuff)
                            continue;

                        Int_t sample = Int_t(channelTmp % 32);
                        Int_t signal = Int_t(digitsGem[itCorr.first.first].GetStripSignal());

                        if (isUseRealSignal)
                            adc32->GetShortValue()[sample] += signal * 16;
                        else
                            adc32->GetShortValue()[sample] += fSignal[0] * 16;

                        adc32->SetAsEmbedded(kTRUE);
                    }

                // CSC:
                // Looking for a pair <channel, serial> in the correspondence map ...
                if (isCscEmbedded)
                    for (auto itCorr : idxChanSerCsc) {
                        Int_t channelTmp = itCorr.first.second; // ---> (0 ... 2047)

                        Int_t channelBuff = Int_t(channelTmp / 32);
                        Long_t serialBuff = itCorr.second;

                        if (channel != channelBuff || serial != serialBuff)
                            continue;

                        Int_t sample = Int_t(channelTmp % 32);
                        Int_t signal = Int_t(digitsCsc[itCorr.first.first].GetStripSignal() * 16.);

                        if (isUseRealSignal)
                            adc32->GetShortValue()[sample] += signal;
                        else
                            adc32->GetShortValue()[sample] += fSignal[2] * 16;

                        adc32->SetAsEmbedded(kTRUE);
                    }

                // Write ADC-digit to the output file ...            
                new ((*gemOrCsc)[gemOrCsc->GetEntriesFast()]) BmnADCDigit(adc32->GetSerial(), adc32->GetChannel(), adc32->GetNSamples(), adc32->GetShortValue(), adc32->IsEmbedded());
            }

        // SILICON
        if (isSilEmbedded)
            for (Int_t iAdc128 = 0; iAdc128 < fADC128->GetEntriesFast(); iAdc128++) {
                BmnADCDigit* adc128 = (BmnADCDigit*) fADC128->UncheckedAt(iAdc128);

                UInt_t channel = adc128->GetChannel();
                UInt_t serial = adc128->GetSerial();

                // Looking for an entry with a given <channel, sample, serial> in the correspondence map ...
                for (auto itCorr : idxChanSampleSerSilicon) {
                    Int_t chan = itCorr.first[1];
                    Long_t ser = itCorr.second;

                    if (channel != chan || serial != ser)
                        continue;

                    Int_t idx = itCorr.first[0];
                    Int_t signal = Int_t(digitsSilicon[idx].GetStripSignal() * 16.);

                    Int_t sample = itCorr.first[2];

                    // x'-layer has a negative polarity!
                    Int_t sign = (digitsSilicon[idx].GetStripLayer() == 0) ? +1 : -1;
                    if (isUseRealSignal)
                        adc128->GetShortValue()[sample] += sign * signal;
                    else
                        adc128->GetShortValue()[sample] += sign * fSignal[1] * 16;

                    adc128->SetAsEmbedded(kTRUE);
                }

                // Write ADC-digit to the output file ...            
                new ((*silicon)[silicon->GetEntriesFast()]) BmnADCDigit(adc128->GetSerial(), adc128->GetChannel(), adc128->GetNSamples(), adc128->GetShortValue(), adc128->IsEmbedded());
            }

        t->Fill();
    }

    delete ch;

    t->Write();
    f->Close();

    return tmp;
}

// Create store of possible primary lambdas to be used for embedding ...

TClonesArray* BmnLambdaEmbedding::CreateLambdaStore() {
    cout << fNstores << endl;
    for (Int_t iEntry = 0; iEntry < fSim->GetEntries(); iEntry++) {
        fSim->GetEntry(iEntry);
        if (iEntry % 1000 == 0) {
            cout << "Event# " << iEntry << " processed" << endl;
            cout << "Size of store# " << fLambdaStore->GetEntriesFast() << endl;
        }

        if (fLambdaStore->GetEntriesFast() == fNstores) // Maximum size of current store
            break;

        for (Int_t iTrack = 0; iTrack < fMCTracks->GetEntriesFast(); iTrack++) {
            CbmMCTrack* track = (CbmMCTrack*) fMCTracks->UncheckedAt(iTrack);

            // Get primary lambdas only ...
            if (track->GetPdgCode() != fPdgDecParticle || track->GetMotherId() != -1)
                continue;

            // Get lambdas in forward direction ...
            if (track->GetPz() < 0.)
                continue;

            Double_t p = track->GetP();
            Double_t Tx = track->GetPx() / track->GetPz();
            Double_t Ty = track->GetPy() / track->GetPz();

            Double_t Pz = p / TMath::Sqrt(1 + Tx * Tx + Ty * Ty);
            Double_t eta = 0.5 * TMath::Log((p + Pz) / (p - Pz));

            TVector3 tmp(Tx * Pz, Ty * Pz, Pz);
            Double_t phi = tmp.Phi() * TMath::RadToDeg() + 180.;

            if (eta < fEtaMin || eta > fEtaMax || p < fMomMin || phi < fPhiMin || phi > fPhiMax)
                continue;

            new ((*fLambdaStore)[fLambdaStore->GetEntriesFast()]) BmnParticleStore(fPdgDecParticle, p, Tx, Ty);
        }
    }
    return fLambdaStore;
}

void BmnLambdaEmbedding::PrintStoreInfo() {
    cout << "N_particles_store# " << fLambdaStore->GetEntriesFast() << endl;

    for (Int_t iLambda = 0; iLambda < fLambdaStore->GetEntriesFast(); iLambda++) {
        BmnParticleStore* lambda = (BmnParticleStore*) fLambdaStore->UncheckedAt(iLambda);
        cout << "P = " << lambda->GetP() << " Eta = " << lambda->GetEta() << " Phi = " << lambda->GetPhi() << endl;
    }
}

BmnLambdaEmbedding::~BmnLambdaEmbedding() {
    delete fSim;
    delete fReco;

    delete fInfo;

    if (fMon)
        delete fMon;

    if (normUtils)
        delete normUtils;
}

void BmnLambdaEmbedding::SimulateLambdaPassing(Int_t pdg, Double_t P, TVector2 pos, TVector3 Vp, Int_t iLambda, Int_t iVertex) {
    // Here there is a list of arguments to be passed to the macro below ...

    TString namePart = (pdg == 3122) ? "lambda" : (pdg == 310) ? "kaon" : "unknown";
    TString outFile = TString::Format("%s/%s%d_vertex%d.root", fStorePath.Data(), namePart.Data(), iLambda, iVertex);

    TString macroName = "SimulateLambdaPassing.C";

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathFull = gPathConfig + "/macro/embedding/" + macroName;

    TString argList = TString::Format("(%d, %d, %G, %G, %G, %G, %G, %G, %G, \"%s\")", pdg, fNeventsToBeSimulated, fFieldScale, P, pos.X(), pos.Y(), Vp.X(), Vp.Y(), Vp.Z(), outFile.Data());
    TString exeCommand = TString::Format("root -b -q '%s%s'", gPathFull.Data(), argList.Data());

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
