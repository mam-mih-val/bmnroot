

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

            if (turnOffBaseDigits == kTRUE) {
                for (Int_t i = 0; i < digiBaseArs[iBr]->GetEntriesFast(); i++) {
                    BmnStripDigit * dig = (BmnStripDigit*) digiBaseArs[iBr]->At(i);
                    dig->SetIsGoodDigit(kFALSE);
                }
            }

            //            Int_t fNDigiBase = digiBaseArs[iBr]->GetEntriesFast();
            digiDestArs[iBr]->AbsorbObjects(digiBaseArs[iBr]);
            digiDestArs[iBr]->AbsorbObjects(digiSourceArs[iBr]);

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
    printf("\nPreliminary clone selected exp events!\n");
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
        printf("iev %u nsrc %u\n", iEv, NSrcEvents);
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

            for (Int_t iPoint = 0; iPoint < gemPoints->GetEntriesFast(); iPoint++) {
                CbmStsPoint* stsPoint = (CbmStsPoint*) gemPoints->UncheckedAt(iPoint);
                if (stsPoint->GetTrackID() != jTrack)
                    continue;
                //                                printf("found %dth gem point for %d\n", iPoint, offspringTrack->GetPdgCode());
                outHitsCnt[iOffspring]++;
            }
            for (Int_t iPoint = 0; iPoint < silPoints->GetEntriesFast(); iPoint++) {
                BmnSiliconPoint* silPoint = (BmnSiliconPoint*) silPoints->UncheckedAt(iPoint);
                if (silPoint->GetTrackID() != jTrack)
                    continue;
                //                                printf("found %dth sil point for %d\n", iPoint, offspringTrack->GetPdgCode());
                outHitsCnt[iOffspring]++;
            }
            for (Int_t iPoint = 0; iPoint < cscPoints->GetEntriesFast(); iPoint++) {
                BmnCSCPoint* cscPoint = (BmnCSCPoint*) cscPoints->UncheckedAt(iPoint);
                if (cscPoint->GetTrackID() != jTrack)
                    continue;
                //                                printf("found %dth csc point for %d\n", iPoint, offspringTrack->GetPdgCode());
                outHitsCnt[iOffspring]++;
            }
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

//BmnStatus BmnRecoTools::FilterDecayR(TString inSourceName, TString outName, Int_t code, vector<Int_t> outCodes) {
//    BmnStatus st = FilterDecayR(chain, outName, code, outCodes);
////    TChain * chain = new TChain("bmndata");
////    chain->Add(inSourceName.Data());
////    BmnStatus st = FilterDecayR(chain, outName, code, outCodes);
////    delete chain;
//    return st;
//}

//BmnStatus BmnRecoTools::FilterDecayR(TChain* inSourceChain, TString outName, Int_t code, vector<Int_t> outCodes) {

BmnStatus BmnRecoTools::FilterDecayR(TString inSourceName, TString outName, Bool_t isExp, Int_t code, vector<Int_t> outCodes) {
    Int_t fPeriodID = 7;
    Bool_t addMatch = kFALSE;
    const Int_t minHits = 5;

    TString EHName = "BmnEventHeader.";
    TString EHNameCopy = "EventHeader.";
    TString EHMCName = "MCEventHeader.";
    TString EHDigiName = "BmnEventHeader.";
    TString FieldParName = "BmnFieldPar";
    TString MCTrackName = "MCTrack";
    TString StsPointName = "StsPoint";
    vector<TString> digiNames = {
        "BmnSiliconDigit", "BmnGemStripDigit", "BmnCSCDigit", "MCTrack", "StsPoint", "SiliconPoint", "CSCPoint",
        "GeoTracks"
    };
    vector<TString> digiExpNames = {
        "BmnSiliconDigit", "BmnGemStripDigit", "BmnCSCDigit", "MCTrack", "StsPoint", "SiliconPoint", "CSCPoint",
        "GeoTracks"
    };
    vector<TString> outMCNames = {
        "BmnSiliconDigit", "BmnGemStripDigit", "BmnCSCDigit", "MCTrack", "StsPoint", "SiliconPoint", "CSCPoint",
        "GeoTracks"
    };
    vector<TString> outExpNames = {
        "SILICON", "GEM", "CSC", "MCTrack", "StsPoint", "SiliconPoint", "CSCPoint",
        "GeoTracks"
    };
    vector<TString> digiOutExpNames = (isExp == kTRUE) ? outExpNames : outMCNames;

    vector<TClass*> digiClasses = {
        BmnSiliconDigit::Class(), BmnGemStripDigit::Class(), BmnCSCDigit::Class(), CbmMCTrack::Class(),
        CbmStsPoint::Class(), BmnSiliconPoint::Class(), BmnCSCPoint::Class(), TGeoTrack::Class()
    };
    vector<TString> matchNames = {"BmnSiliconDigitMatch", "BmnGemStripDigitMatch", "BmnCSCDigitMatch"};
    vector<TClonesArray*> digiSourceArs; ///<- source digi arrays
    vector<TClonesArray*> digiDestArs; ///<- destination digi arrays
    vector<TBranch*> digiDestBrs; ///<- destination digi branches
    vector<TClonesArray*> matchSourceArs; ///<- source match arrays
    vector<TClonesArray*> matchDestArs; ///<- destination match arrays
    vector<TBranch*> matchDestBrs; ///<- destination match branches
    TClonesArray * mcTracks = nullptr;
    TClonesArray* stsPoints = nullptr;
    TClonesArray* silPoints = nullptr;
    TClonesArray* cscPoints = nullptr;
    TBranch *EHBranch = nullptr; ///<- input event header
    TBranch *EHBranchOut = nullptr; ///<- output event header
    FairMCEventHeader * mcEH = nullptr;
    FairMCEventHeader * mcEHOut = nullptr;
    BmnEventHeader * EHOut = nullptr;
    BmnEventHeader * EHOutCopy = nullptr;
    BmnFieldPar *fieldPar = nullptr;

    TList* fBranchList = new TList();

    UInt_t fNArs = digiNames.size();

    /***********************/
    /** Open input digits **/
    /***********************/
    TFile *fSourceHits = new TFile(inSourceName, "READ");
    if (fSourceHits->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s !\n", inSourceName.Data());
        return kBMNERROR;
    }
    printf("\nINPUT SOURCE FILE: ");
    printf("%s\n", inSourceName.Data());
    TTree * inSourceChain = (TTree *) fSourceHits->Get("bmndata");
    UInt_t fNEventSource = inSourceChain->GetEntries();
    printf("Found %d events\n", fNEventSource);
    for (Int_t i = 0; i < fNArs; i++) {
        TClonesArray* arDigi = nullptr; // new TClonesArray(BmnCSCHit::Class());
        inSourceChain->SetBranchAddress(digiNames[i].Data(), &arDigi);
        digiSourceArs.push_back(arDigi);
        if (i < matchNames.size()) {
            TClonesArray* ar = nullptr;
            inSourceChain->SetBranchAddress(matchNames[i].Data(), &ar);
            matchSourceArs.push_back(ar);
        }
    }
    mcTracks = digiSourceArs[3];
    stsPoints = digiSourceArs[4];
    silPoints = digiSourceArs[5];
    cscPoints = digiSourceArs[6];
    inSourceChain->SetBranchAddress(EHMCName.Data(), &mcEH);
    //    inSourceChain->SetBranchAddress(MCTrackName.Data(), &mcTracks);
    //    inSourceChain->SetBranchAddress(StsPointName.Data(), &stsPoints);
    fieldPar = (BmnFieldPar*) fSourceHits->Get(FieldParName.Data());

    /*******************************/
    /** Create output digits file **/
    /*******************************/
    TFile *fDestHitsFile = new TFile(outName, "RECREATE");
    if (fDestHitsFile->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s !\n", outName.Data());
        return kBMNERROR;
    }
    printf("\nOUT FILE: %s\n", outName.Data());
    if (addMatch == kFALSE)
        for (Int_t i = 0; i < matchNames.size(); i++) {
            inSourceChain->SetBranchStatus(matchNames[i] + ".*", 0);
            //            inSourceChain->SetBranchStatus(matchNames[i], 0);
        }
    TTree * fDestTree = inSourceChain->CloneTree(0);
    //    TTree * fDestTree = new TTree("bmndata", "bmndata");
    //    for (Int_t i = 0; i < fNArs; i++) {
    //        TClonesArray* arDigi = new TClonesArray(digiClasses[i]);
    //        TBranch* brDigi = fDestTree->Branch(digiOutExpNames[i], &arDigi);
    //        digiDestArs.push_back(arDigi);
    //        digiDestBrs.push_back(brDigi);
    //    }
    if (addMatch)
        for (Int_t i = 0; i < matchNames.size(); i++) {
            TClonesArray* ar = new TClonesArray(BmnMatch::Class());
            TBranch * br = fDestTree->Branch(matchNames[i], &ar);
            matchDestArs.push_back(ar);
            matchDestBrs.push_back(br);
        }
    //    if (isExp == kTRUE) {
    //        EHOut = new BmnEventHeader();
    //        fDestTree->Branch(EHName.Data(), EHOut);
    //        EHOutCopy = new BmnEventHeader();
    //        fDestTree->Branch(EHNameCopy.Data(), EHOutCopy);
    //    } else {
    //        mcEHOut = new FairMCEventHeader();
    //        EHBranchOut = fDestTree->Branch(EHMCName.Data(), mcEHOut);
    //    }
    TList * branches = (TList*) fDestTree->GetListOfBranches();
    for (Int_t i = 0; i < branches->GetEntries(); i++) {
        TObjString * s = new TObjString(branches->At(i)->GetName());
        fBranchList->Add(s);
    }
    UInt_t nRecEv = 0;

    for (UInt_t iEv = 0; iEv < fNEventSource; ++iEv) {
        //        for (UInt_t iBr = 0; iBr < fNArs; iBr++) {
        //            digiDestArs[iBr]->Clear("C");
        //            if (addMatch && iBr < matchNames.size())
        //                matchDestArs[iBr]->Clear("C");
        //        }
        inSourceChain->GetEntry(iEv);
        DrawBar(iEv, fNEventSource);

        Int_t decaysWritten = 0;
        // run over MC tracks & search for needed particle code
        //                printf("iev %d ntracks %d\n", iEv, mcTracks->GetEntriesFast());
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

                for (Int_t iPoint = 0; iPoint < stsPoints->GetEntriesFast(); iPoint++) {
                    CbmStsPoint* stsPoint = (CbmStsPoint*) stsPoints->UncheckedAt(iPoint);
                    if (stsPoint->GetTrackID() != jTrack)
                        continue;
                    //                                printf("found %dth gem point for %d\n", iPoint, offspringTrack->GetPdgCode());
                    outHitsCnt[iOffspring]++;
                }
                for (Int_t iPoint = 0; iPoint < silPoints->GetEntriesFast(); iPoint++) {
                    BmnSiliconPoint* silPoint = (BmnSiliconPoint*) silPoints->UncheckedAt(iPoint);
                    if (silPoint->GetTrackID() != jTrack)
                        continue;
                    //                                printf("found %dth sil point for %d\n", iPoint, offspringTrack->GetPdgCode());
                    outHitsCnt[iOffspring]++;
                }
                for (Int_t iPoint = 0; iPoint < cscPoints->GetEntriesFast(); iPoint++) {
                    BmnCSCPoint* cscPoint = (BmnCSCPoint*) cscPoints->UncheckedAt(iPoint);
                    if (cscPoint->GetTrackID() != jTrack)
                        continue;
                    //                                printf("found %dth csc point for %d\n", iPoint, offspringTrack->GetPdgCode());
                    outHitsCnt[iOffspring]++;
                }
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
        if (decaysWritten == 0)
            continue;
        //        for (UInt_t iBr = 0; iBr < fNArs; iBr++) {
        //            digiDestArs[iBr]->AbsorbObjects(digiSourceArs[iBr]);
        //            if (iBr < matchNames.size()) {
        //                if (addMatch) {
        //                    matchDestArs[iBr]->AbsorbObjects(matchSourceArs[iBr]);
        //                }
        //            }
        //        }
        //        if (isExp) {
        //            EHOut->Clear();
        //            EHOut->SetEventId(mcEH->GetEventID());
        //            //                EHOut->SetEventTime(mcEH->GetEventTime());
        //            //                EHOut->SetEventTimeTS(mcEH->GetEventTimeTS());
        //            EHOut->SetEventType(kBMNPAYLOAD);
        //            EHOut->SetPeriodId(fPeriodID);
        //            EHOut->SetRunId(4649); //mcEH->GetRunID());
        //            EHOutCopy->Clear();
        //            EHOutCopy->SetEventId(mcEH->GetEventID());
        //            //                EHOutCopy->SetEventTime(mcEH->GetEventTime());
        //            //                EHOutCopy->SetEventTimeTS(mcEH->GetEventTimeTS());
        //            EHOutCopy->SetEventType(kBMNPAYLOAD);
        //            EHOutCopy->SetPeriodId(fPeriodID);
        //            EHOutCopy->SetRunId(4649); //mcEH->GetRunID());
        //        } else {
        //            mcEHOut->Clear();
        //            mcEHOut->SetEventID(mcEH->GetEventID());
        //            mcEHOut->SetRunID(mcEH->GetRunID());
        //            mcEHOut->SetTime(mcEH->GetT());
        //            mcEHOut->SetB(mcEH->GetB());
        //            mcEHOut->SetNPrim(mcEH->GetNPrim());
        //            mcEHOut->MarkSet(mcEH->IsSet());
        //            TVector3 vertex;
        //            mcEH->GetVertex(vertex);
        //            mcEHOut->SetVertex(vertex);
        //            mcEHOut->SetRotX(mcEH->GetRotX());
        //            mcEHOut->SetRotY(mcEH->GetRotY());
        //            mcEHOut->SetRotZ(mcEH->GetRotZ());
        //        }

        fDestTree->Fill();
        nRecEv++;
    }
    printf("\nFound %u reconstuctable events\n", nRecEv);
    fDestTree->AutoSave();
    //    fDestTree->Write();
    if (isExp == kFALSE)
        fDestHitsFile->WriteObject(fBranchList, "BranchList");
    //    fDestHitsFile->WriteObject(fieldPar, FieldParName.Data());
    fDestHitsFile->Write();
    if (fDestHitsFile)
        fDestHitsFile->Close();
    //    if (fSourceHits)
    //        fSourceHits->Close();
}

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

ClassImp(BmnRecoTools)
