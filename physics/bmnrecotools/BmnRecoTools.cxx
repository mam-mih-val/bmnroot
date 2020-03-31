

#include "BmnRecoTools.h"

BmnRecoTools::BmnRecoTools() {
}

BmnRecoTools::~BmnRecoTools() {
}

BmnStatus BmnRecoTools::Embed(TString inSourceName, TString inBaseName, TString destName,
        Int_t code, vector<Int_t> outCodes,
        Bool_t turnOffBaseDigits) {

    fCode = code;
    fOutCodes = outCodes;
    UInt_t fPeriodId = 7;
    Int_t EmbeddedType = 1;
    Bool_t addMatch = kFALSE;
    Bool_t isExp = kTRUE;
    Bool_t isHitMakerEfficiencyMode = kTRUE;

    /* Hits branches */
    vector<TString> digiNames = {
        "BmnSiliconDigit", "BmnGemStripDigit", "BmnCSCDigit"
    };
    vector<TString> outMCNames = {
        "BmnSiliconDigit", "BmnGemStripDigit", "BmnCSCDigit", "StsPoint", "SiliconPoint", "CSCPoint",
        "GeoTracks"
    };
    vector<TString> outExpNames = {
        "SILICON", "GEM", "CSC"
    };
    vector<TString> digiOutExpNames = (isExp == kTRUE) ? outExpNames : outMCNames;

    vector<TClass*> digiClasses = {
        BmnSiliconDigit::Class(), BmnGemStripDigit::Class(), BmnCSCDigit::Class()
    };
    vector<TString> matchNames = {"BmnSiliconDigitMatch", "BmnGemStripDigitMatch", "BmnCSCDigitMatch"};

    TList* fBranchList = new TList();
    BmnFieldPar *fieldPar = nullptr;
    DigiRunHeader *rhBase = nullptr;
    UInt_t fNArs = digiNames.size();


    //    Int_t retn = system(Form("cp %s %s", inBaseName.Data(), destName.Data()));
    //    printf("copy return %d\n", retn);
    //    RemoveBranches(destName, digiExpNames);

    /*****************************/
    /** Open input source digits **/
    /*****************************/
    TFile *fSourceHits = new TFile(inSourceName, "READ");
    if (fSourceHits->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s !\n", inSourceName.Data());
        return kBMNERROR;
    }
    printf("\nINPUT SOURCE FILE: ");
    printf("%s\n", inSourceName.Data());
    fInTreeSource = (TTree *) fSourceHits->Get("bmndata");
    UInt_t fNEventSource = fInTreeSource->GetEntries();
    for (Int_t i = 0; i < fNArs; i++) {
        TClonesArray* arDigi = nullptr; // new TClonesArray(BmnCSCHit::Class());
        printf("digiNames[%d] %s \n", i, digiNames[i].Data());
        fInTreeSource->SetBranchAddress(digiNames[i].Data(), &arDigi);
        digiSourceArs.push_back(arDigi);
        if (i < matchNames.size())
            if (addMatch == kTRUE) {
                TClonesArray* ar = nullptr;
                fInTreeSource->SetBranchAddress(matchNames[i].Data(), &ar);
                matchSourceArs.push_back(ar);
            } else
                fInTreeSource->SetBranchStatus(matchNames[i] + ".*", 0);
    }
    mcTracks = digiSourceArs[3];
    stsPoints = digiSourceArs[4];
    silPoints = digiSourceArs[5];
    cscPoints = digiSourceArs[6];
    fInTreeSource->SetBranchAddress(EHMCName.Data(), &mcEH);

    //    Int_t retn = system(Form("cp  %s %s", inBaseName.Data(), destName.Data()));
    //    printf("ret %d\n", retn);
    //    fflush(stdout);

    TString tempBaseName = inBaseName + "-temp.root";
    CloneSelected(inBaseName, tempBaseName);

    /*****************************/
    /** Open input base digits **/
    /*****************************/
    TFile *fBaseHits = new TFile(tempBaseName, "READ");
    if (fBaseHits->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s !\n", tempBaseName.Data());
        return kBMNERROR;
    }
    printf("\nINPUT BASE FILE: ");
    printf("%s\n", tempBaseName.Data());
    fInTreeBase = (TTree *) fBaseHits->Get("bmndata");
    rhBase = (DigiRunHeader*) fBaseHits->Get(RHDigiName.Data());
    UInt_t fNEventBase = fInTreeBase->GetEntries();
    for (Int_t i = 0; i < fNArs; i++) {
        TClonesArray* arDigi = nullptr; // new TClonesArray(BmnCSCHit::Class());
        fInTreeBase->SetBranchAddress(digiOutExpNames[i].Data(), &arDigi);
        digiBaseArs.push_back(arDigi);
        if (addMatch == kTRUE)
            if (i < matchNames.size()) {
                TClonesArray* ar = nullptr;
                fInTreeBase->SetBranchAddress(matchNames[i].Data(), &ar);
                matchBaseArs.push_back(ar);
            }
    }
    //        fieldPar = (BmnFieldPar*) fBaseHits->Get(FieldParName.Data());
    BmnEventHeader * baseEH = nullptr;
    fInTreeBase->SetBranchAddress(EHDigiName.Data(), &baseEH);
    //        TObject * fhdr = fBaseHits->Get("FileHeader");
    //        TObject * cbmr = fBaseHits->Get("cbmroot");


    /*****************************/
    /** Open  dest digits **/
    /*****************************/
    TFile *fDestHitsFile = new TFile(destName, "RECREATE");
    if (fDestHitsFile->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s !\n", destName.Data());
        return kBMNERROR;
    }
    printf("\n DEST FILE: ");
    printf("%s\n", destName.Data());
    fDestTree = new TTree("bmndata", "bmndata");
    //    for (Int_t i = 0; i < fNArs; i++) {
    //        fDestTree->SetBranchStatus(digiNames[i].Data(), 1);
    //        if (addMatch == kTRUE)
    //            if (i < matchNames.size()) {
    //                fDestTree->SetBranchStatus(matchNames[i].Data(), 1);
    //            }
    //    }
    for (Int_t i = 0; i < fNArs; i++) {
        TClonesArray* arDigi = new TClonesArray(digiClasses[i]); //nullptr;
        TBranch * brDigi = fDestTree->Branch(digiOutExpNames[i], &arDigi); //nullptr;
        digiDestArs.push_back(arDigi);
        digiDestBrs.push_back(brDigi);

    }
    BmnEventHeader* destEH = new BmnEventHeader();
    fDestTree->Branch("BmnEventHeader.", &destEH);
    if (addMatch == kTRUE)
        for (Int_t i = 0; i < matchNames.size(); i++) {
            TClonesArray* ar = new TClonesArray(BmnMatch::Class()); //nullptr;
            TBranch * br = fDestTree->Branch(matchNames[i], &ar); //nullptr;
            //            fDestTree->SetBranchAddress(matchNames[i], &ar, &br);
            matchDestArs.push_back(ar);
            matchDestBrs.push_back(br);
        }

    /*****************************/
    /** Fill hits **/
    /*****************************/
    //    UInt_t minEvents = Min(fNEventSource, fNEventDest);


    //    for (UInt_t iEv = 0; iEv < 10/*fNEventSource*/; ++iEv) {
    for (UInt_t iEv = 0; iEv < fNEventSource; ++iEv) {
        DrawBar(iEv, fNEventSource);
        fInTreeBase->GetEntry(iEv);
        //        fDestTree->GetEntry(iEv);
        fInTreeSource->GetEntry(iEv);
        for (UInt_t iBr = 0; iBr < fNArs; iBr++) {
            digiDestArs[iBr]->Clear("C");
            if (addMatch == kTRUE && iBr < matchNames.size())
                matchDestArs[iBr]->Clear("C");
        }
        //        if (GetNextValidSourceEvent() == kBMNERROR) {
        //            printf("Not enough source events!\n");
        //            break;
        //        }
        //        fDestTree->GetEntry(iEv);
        for (UInt_t iBr = 0; iBr < fNArs; iBr++) {
            //            printf("iEv %u iBr %u \n", iEv, iBr);
            //            printf(" was %d entries source\n", digiSourceArs[iBr]->GetEntries());
            //            //            printf(" was %d entries dest\n", digiDestArs[iBr]->GetEntries());
            //            printf(" was %d entries base\n", digiBaseArs[iBr]->GetEntries());

            //            if (turnOffBaseDigits == kTRUE) {
            //                for (Int_t i = 0; i < digiBaseArs[iBr]->GetEntriesFast(); i++) {
            //                    BmnStripDigit * dig = (BmnStripDigit*) digiBaseArs[iBr]->At(i);
            //                    dig->SetIsGoodDigit(kFALSE);
            //                }
            //            }

            //            Int_t fNDigiBase = digiBaseArs[iBr]->GetEntriesFast();
            if (turnOffBaseDigits == kFALSE)
                digiDestArs[iBr]->AbsorbObjects(digiBaseArs[iBr]);
            //            digiDestArs[iBr]->AbsorbObjects(digiSourceArs[iBr]);
            /** summ strip signals */
            for (UInt_t iSrcDig = 0; iSrcDig < digiSourceArs[iBr]->GetEntriesFast(); iSrcDig++) {
                BmnStripDigit * src = (BmnStripDigit*) digiSourceArs[iBr]->At(iSrcDig);
                Int_t iSame = -1;
                //                printf("iSrc %d\n", iSrcDig);
                for (UInt_t iDestDig = 0; iDestDig < digiDestArs[iBr]->GetEntriesFast(); iDestDig++) {
                    BmnStripDigit * des = (BmnStripDigit*) digiDestArs[iBr]->At(iDestDig);
                    if (
                            (des->GetStation() == src->GetStation()) &&
                            (des->GetModule() == src->GetModule()) &&
                            (des->GetStripLayer() == src->GetStripLayer()) &&
                            (des->GetStripNumber() == src->GetStripNumber())
                            ) {
                        iSame = iDestDig;
                        if (isHitMakerEfficiencyMode)
                            des->SetStripSignal(src->GetStripSignal());
                        else
                            des->SetStripSignal(des->GetStripSignal() + src->GetStripSignal());
                    }
                }
                if (iSame == -1) {
                    new ((*digiDestArs[iBr])[digiDestArs[iBr]->GetEntriesFast()])
                            BmnStripDigit(src);
                }
            }

            //            digiDestBrs[iBr]->Fill();
            if (addMatch)
                if (iBr < matchNames.size()) {
                    //                    for (UInt_t iMatch = 0; iMatch < fNDigiBase; iMatch++) {
                    //                        new ((*matchDestArs[iBr])[matchDestArs[iBr]->GetEntriesFast()]) BmnMatch();
                    //                    }
                    matchDestArs[iBr]->AbsorbObjects(matchSourceArs[iBr]);
                    //                    matchDestBrs[iBr]->Fill();
                }
            //            printf(" is %d entries dest\n", digiDestArs[iBr]->GetEntries());

        }
        //        mcEHOut->Clear();
        //        mcEHOut->SetEventId(mcEH->GetEventId());
        //        mcEHOut->SetEventTime(mcEH->GetEventTime());
        //        mcEHOut->SetEventTimeTS(mcEH->GetEventTimeTS());
        //        mcEHOut->SetADCin(mcEH->GetADCin());
        //        mcEHOut->SetADCout(mcEH->GetADCout());
        //        mcEHOut->SetB(mcEH->GetB());
        //        mcEHOut->SetHeaderName(mcEH->GetHeaderName());
        //        mcEHOut->SetInputFileId(mcEH->GetInputFileId());
        //        mcEHOut->SetMCEntryNumber(mcEH->GetMCEntryNumber());
        //        //        mcEHOut->SetRunId(mcEH->GetRunId());
        //        mcEHOut->SetTriggerType(mcEH->GetTriggerType());
        //        mcEHOut->SetZ2in(mcEH->GetZ2in());
        //        mcEHOut->SetZ2out(mcEH->GetZ2out());
        destEH->Clear();
        destEH->SetEventId(baseEH->GetEventId());
        destEH->SetEventTime(baseEH->GetEventTime());
        destEH->SetEventTimeTS(baseEH->GetEventTimeTS());
        destEH->SetEventType(baseEH->GetEventType());
        destEH->SetPeriodId(baseEH->GetPeriodId());
        destEH->SetRunId(baseEH->GetRunId());
        //        EHBranch->Fill();
        fDestTree->Fill();
    }
    fDestTree->Write();
    fDestHitsFile->WriteObject(rhBase, RHDigiName.Data());
    fDestHitsFile->Write();
    if (fSourceHits)
        fSourceHits->Close();
    if (fBaseHits)
        fBaseHits->Close();
    if (fDestHitsFile)
        fDestHitsFile->Close();

    printf("\nFinished! Search made over %d source events\n", iSourceEvent);

    return kBMNSUCCESS;
}

BmnStatus BmnRecoTools::CloneSelected(TString BaseName, TString TempBaseName) {
    printf("\nPreliminary clone selected exp events:");
    TFile *BaseHits = new TFile(BaseName, "READ");
    if (BaseHits->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s !\n", BaseName.Data());
        return kBMNERROR;
    }
    TTree *TreeBase = (TTree *) BaseHits->Get("bmndata");
    DigiRunHeader* RHBase = (DigiRunHeader*) BaseHits->Get(RHDigiName.Data());
    /*******************************/
    /** Create temp digits base file **/
    /*******************************/
    TFile *DestHitsFile = new TFile(TempBaseName, "RECREATE");
    if (DestHitsFile->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s !\n", TempBaseName.Data());
        return kBMNERROR;
    }
    printf("\nOUT HITS FILE: ");
    printf("%s\n", TempBaseName.Data());
    TTree * DestTree = TreeBase->CloneTree(0); //-1, "fast");
    BmnEventHeader * baseEH = nullptr;
    TreeBase->SetBranchAddress(EHDigiName.Data(), &baseEH);
    iSourceEvent = 0;
    UInt_t iBaseEvent = 0;
    UInt_t NSrcEvents = fInTreeSource->GetEntries();
    UInt_t NBaseEvents = TreeBase->GetEntries();
    for (UInt_t iEv = 0; iEv < NSrcEvents; iEv++) {
        DrawBar(iEv, NSrcEvents);
        if (GetNextValidSourceEvent() == kBMNERROR) {
            printf("Not enough source events!\n");
            break;
        }
        while (iBaseEvent < NBaseEvents) {
            TreeBase->GetEntry(iBaseEvent++);
            if (mcEH->GetEventID() == baseEH->GetEventId()) {
                DestTree->Fill();
                break;
            }
        }
    }
    DestTree->Write();
    DestHitsFile->WriteObject(RHBase, RHDigiName.Data());
    DestHitsFile->Write();
    DestHitsFile->Close();
    DestHitsFile = nullptr;
    BaseHits->Close();
    printf("\nPreliminary cloning finished!\n");
    return kBMNSUCCESS;
}

//void BmnRecoTools::HighlightDecay(TClonesArray* mcTracks, TClonesArray* gemPoints, TClonesArray* silPoints) { printf("Highlight\n");
//    for (Int_t iTrack = 0; iTrack < mcTracks->GetEntriesFast(); iTrack++) {
//        CbmMCTrack* track = (CbmMCTrack*) mcTracks->UncheckedAt(iTrack);
//        if (track->GetMotherId() != -1 || track->GetPdgCode() != fCode)
//            continue;
//                                printf("found %dth track for %d\n", iTrack, track->GetPdgCode());
//        vector<Int_t> outHitsCnt; ///<- found hits for the each offspring
////        outHitsCnt.resize(fOutCodes.size(), 0);
//        // find the offsprings & count their hits in the inner tracker
//        for (Int_t jTrack = 0; jTrack < mcTracks->GetEntriesFast(); jTrack++) {
//            CbmMCTrack* offspringTrack = (CbmMCTrack*) mcTracks->UncheckedAt(jTrack);
//            if (offspringTrack->GetMotherId() != iTrack)
//                continue;
//            auto it = find(fOutCodes.begin(), fOutCodes.end(), offspringTrack->GetPdgCode());
//            if (it == fOutCodes.end())
//                continue;
//        }
//    }
//}

BmnStatus BmnRecoTools::GetNextValidSourceEvent() {
    do {
        fInTreeSource->GetEntry(iSourceEvent++);
        //        if (IsReconstructable(mcTracks, stsPoints, silPoints, cscPoints, fCode, fOutCodes, fMinHits))
        return kBMNSUCCESS;
    } while (iSourceEvent < fInTreeSource->GetEntriesFast());
    return kBMNERROR;
}

Bool_t BmnRecoTools::IsReconstructable(
        TClonesArray* mcTracks, TClonesArray* gemPoints, TClonesArray* silPoints, TClonesArray* cscPoints,
        Int_t code, vector<Int_t> &outCodes, Int_t minHits) {

    Int_t decaysWritten = 0;

    for (Int_t iTrack = 0; iTrack < mcTracks->GetEntriesFast(); iTrack++) {
        CbmMCTrack* track = (CbmMCTrack*) mcTracks->UncheckedAt(iTrack);
        if (track->GetMotherId() != -1 || track->GetPdgCode() != code)
            continue;
        //                        printf("found %dth track for %d\n", iTrack, track->GetPdgCode());
        vector<Int_t> outHitsCnt; ///<- found hits for the each offspring
        outHitsCnt.resize(outCodes.size(), 0);
        // find the offsprings & count their hits in the inner tracker
        for (Int_t jTrack = 0; jTrack < mcTracks->GetEntriesFast(); jTrack++) {
            CbmMCTrack* offspringTrack = (CbmMCTrack*) mcTracks->UncheckedAt(jTrack);
            if (offspringTrack->GetMotherId() != iTrack)
                continue;
            auto it = find(outCodes.begin(), outCodes.end(), offspringTrack->GetPdgCode());
            if (it == outCodes.end())
                continue;
            Int_t iOffspring = it - outCodes.begin();
            //                                printf("found %dth offspring for %d\n", iOffspring, offspringTrack->GetPdgCode());

            /** Count GEM points */
            UInt_t stations = 0u;
            for (Int_t iPoint = 0; iPoint < gemPoints->GetEntriesFast(); iPoint++) {
                CbmStsPoint* point = (CbmStsPoint*) gemPoints->UncheckedAt(iPoint);
                if (point->GetTrackID() != jTrack)
                    continue;
                //                                printf("found %dth gem point for %d\n", iPoint, offspringTrack->GetPdgCode());
                UInt_t iStation = point->GetStation();
                if (TESTBIT(stations, iStation)) // primitive test whether the track is curly
                    return kFALSE;
                SETBIT(stations, iStation);
                outHitsCnt[iOffspring]++;
            }
            /** Count Silicon points */
            stations = 0u;
            for (Int_t iPoint = 0; iPoint < silPoints->GetEntriesFast(); iPoint++) {
                BmnSiliconPoint* point = (BmnSiliconPoint*) silPoints->UncheckedAt(iPoint);
                if (point->GetTrackID() != jTrack)
                    continue;
                //                                printf("found %dth sil point for %d\n", iPoint, offspringTrack->GetPdgCode());
                UInt_t iStation = point->GetStation();
                if (TESTBIT(stations, iStation))
                    return kFALSE;
                SETBIT(stations, iStation);
                outHitsCnt[iOffspring]++;
            }
            /** Count CSC points */
//            for (Int_t iPoint = 0; iPoint < cscPoints->GetEntriesFast(); iPoint++) {
//                BmnCSCPoint* cscPoint = (BmnCSCPoint*) cscPoints->UncheckedAt(iPoint);
//                if (cscPoint->GetTrackID() != jTrack)
//                    continue;
//                //                                printf("found %dth csc point for %d\n", iPoint, offspringTrack->GetPdgCode());
//                outHitsCnt[iOffspring]++;
//            }
        }
        Int_t insuff = -1;
        for (Int_t iOffspring = 0; iOffspring < outCodes.size(); iOffspring++) {
            //                                printf("%d counts %d hits\n", iOffspring, outHitsCnt[iOffspring]);
            if (outHitsCnt[iOffspring] < minHits) {
                insuff = iOffspring;
                break;
            }
        }
        if (insuff > -1)
            continue;
        decaysWritten++;
    }
    //        printf("\n Found %d Good Lambda Decays\n", decaysWritten);
    return (decaysWritten == 1);

}

//static void BmnRecoTools::ShowHitMakerEff(TClonesArray * hits, BmnSiliconStationSet set) {
//    
//    vector<vector<vector<Int_t  > > > histSiliconStrip;
//    for (Int_t iStation = 0; iStation < set->GetNStations(); iStation++) {
//        vector<vector<TH1F*> > rowGEM;
//        BmnSiliconStation* st = set->GetSiliconStation(iStation);
//        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
//            vector<UInt_t> colGEM;
//            BmnSiliconModule *mod = st->GetModule(iModule);
//            colGEM.resize(mod->GetNStripLayers(), 0)
//            rowGEM.push_back(colGEM);
//        }
//        histSiliconStrip.push_back(rowGEM);
//    }
//
//    for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); iHit++) {
//        BmnHit *hit = (BmnHit *) hits->UncheckedAt(iHit);
//        hit->Get
//    }
//}

vector<TString> BmnRecoTools::GetFileVecFromDir(TString dir) {
    vector<TString> vec;
    struct dirent **namelist;
    const regex re(".+.r12");
    Int_t n;
    n = scandir(dir, &namelist, 0, alphasort);
    if (n < 0) {
        perror("scandir");
        return vec;
    } else {
        for (Int_t i = 0; i < n; ++i) {
            TString _curFile = TString(namelist[i]->d_name);
            if (regex_match(namelist[i]->d_name, re)) {
                vec.push_back(dir + _curFile);
                //                printf("dir file %d %s\n", i, _curFile.Data());
            }
            free(namelist[i]);
        }
        free(namelist);
    }
    return vec;
}

void BmnRecoTools::FillSetStsPoints(
        TClonesArray* pts,
        BmnGemStripStationSet* set,
//        vector<TH2* > &hitVec) {
            vector<vector<vector<TH2* > > > &hitVec) {
    //        vector<vector<vector<UInt_t > > > &hitVec) {
    for (Int_t iHit = 0; iHit < pts->GetEntriesFast(); iHit++) {
        CbmStsPoint *pt = (CbmStsPoint *) pts->UncheckedAt(iHit);
        Int_t iSt = pt->GetStation();
        Int_t iMod = pt->GetModule();
//        Double_t x = ((FairMCPoint*) pt)->GetX();
//        Double_t y = ((FairMCPoint*) pt)->GetY();
        BmnGemStripModule* mod = set->GetStation(iSt)->GetModule(iMod);
        Double_t driftCenterShift = 0.0;
        if (mod->GetElectronDriftDirection() == ForwardZAxisEDrift)
            driftCenterShift = 0.15;
        else
            driftCenterShift = 0.75;
        Double_t x = pt->GetX(pt->GetZ() + driftCenterShift);
        Double_t y = pt->GetY(pt->GetZ() + driftCenterShift);
        Int_t iLayerOwned = -1;
        for (Int_t iLay = 0; iLay < mod->GetStripLayers().size(); iLay++) {
            BmnGemStripLayer l = mod->GetStripLayer(iLay);
            if (l.IsPointInsideStripLayer(x * (-1.0), y)) {
                iLayerOwned = iLay;
                break;
            }
        }
        if (iLayerOwned == -1)
            continue;
        Int_t iZone = iLayerOwned / 2;
        //        printf("zax = %08X\n",hitVec[pt->GetStation()][pt->GetModule()][iZone]->GetZaxis());
        Double_t Pz = ((FairMCPoint*) pt)->GetPz();
        if (Pz > 0)
//            hitVec[pt->GetStation()]->Fill(
//                ((FairMCPoint*) pt)->GetPx() / Pz,
//                ((FairMCPoint*) pt)->GetPy() / Pz);
        //        hitVec[pt->GetStation()]->Fill(x, y);
                hitVec[pt->GetStation()][pt->GetModule()][iZone]->Fill(x, y);
        //        hitVec[pt->GetStation()][pt->GetModule()][iZone]++;
    }
}

void BmnRecoTools::FillSetStsHits(
        TClonesArray* pts, TClonesArray* hits,
        BmnGemStripStationSet* set,
//        vector<TH2* > &hitVec,
//        vector<TH1D* > &hrx,
//        vector<TH1D* > &hry
//) {
            vector<vector<vector<TH2* > > > &hitVec) {
    //        vector<vector<vector<UInt_t > > > &hitVec) {

    for (Int_t iPt = 0; iPt < pts->GetEntriesFast(); iPt++) {
        CbmStsPoint *pt = (CbmStsPoint *) pts->UncheckedAt(iPt);
        Int_t iSt = pt->GetStation();
        Int_t iMod = pt->GetModule();
//        Double_t x = ((FairMCPoint*) pt)->GetX();
//        Double_t y = ((FairMCPoint*) pt)->GetY();
        Double_t thr = 0.;
        Int_t iLayerOwned = -1;
        BmnGemStripModule* mod = set->GetStation(iSt)->GetModule(iMod);
        Double_t driftCenterShift = 0.0;
        if (mod->GetElectronDriftDirection() == ForwardZAxisEDrift)
            driftCenterShift = 0.15;
        else
            driftCenterShift = 0.75;
        Double_t x = pt->GetX(pt->GetZ() + driftCenterShift);
        Double_t y = pt->GetY(pt->GetZ() + driftCenterShift);
        for (Int_t iLay = 0; iLay < mod->GetStripLayers().size(); iLay++) {
            BmnGemStripLayer l = mod->GetStripLayer(iLay);
            //                printf("%d th layer pitch %f\n", iLay, l.GetPitch());
            if (l.IsPointInsideStripLayer(x * (-1.0), y)) {
                //                layers.push_back(l);
                iLayerOwned = iLay;
                //                printf("%d th layer found\n", iLay);
                if (iLay > 1) // hot zone
                    thr = 0.5;
                else
                    thr = 0.5;
                thr = thr*thr;
                break;
            }
        }
        if (iLayerOwned == -1)
            continue;
        Int_t iZone = iLayerOwned / 2;
        //        printf("Point   %f : %f     thr %f\n", x, y, thr); 
        BmnHit *hitClosest = nullptr;
        Double_t MinDistance = DBL_MAX;
        for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); iHit++) {
            BmnHit *hit = (BmnHit *) hits->UncheckedAt(iHit);
            if (
                    hit->GetFlag() == kFALSE ||
                    hit->GetStation() != iSt ||
                    hit->GetModule() != iMod)
                continue;
            Double_t dist = Sq(hit->GetX() - x) + Sq(hit->GetY() - y);
//            hrx[pt->GetStation()]->Fill(hit->GetX() - x);
//            hry[pt->GetStation()]->Fill(hit->GetY() - y);
            //            printf("Hit     %f : %f   dist = %f\n", hit->GetX(), hit->GetY(), dist);
            if (dist < MinDistance) {
                hitClosest = hit;
                MinDistance = dist;
            }
        }
        if (MinDistance < thr) {
            //            printf("Hit     %f : %f   dist = %f   Minimum! \n", hitClosest->GetX(), hitClosest->GetY(), MinDistance);
            Double_t Pz = ((FairMCPoint*) pt)->GetPz();
            if (Pz > 0)
//                hitVec[pt->GetStation()]->Fill(
//                    ((FairMCPoint*) pt)->GetPx() / Pz,
//                    ((FairMCPoint*) pt)->GetPy() / Pz);
            //            hitVec[pt->GetStation()]->Fill(x, y);
                        hitVec[pt->GetStation()][pt->GetModule()][iZone]->Fill(x, y);
            //        hitVec[pt->GetStation()][pt->GetModule()][iZone]++;
            hitClosest->SetFlag(kFALSE);
        } else {
//            printf("\tnot found in st %d mod %d\n", iSt, iMod);
        }
    }
}

ClassImp(BmnRecoTools)
