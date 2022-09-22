#include "BmnEmbedding.h"

BmnEmbedding::BmnEmbedding() {
}

BmnStatus BmnEmbedding::Embed(TString inSourceName, TString inBaseName, TString destName,
        Int_t code, vector<Int_t> outCodes, Bool_t turnOffBaseDigits
//        TString inDSTName
) {

    fCode = code;
    fOutCodes = outCodes;
    UInt_t fPeriodId = 7;
    Int_t EmbeddedType = 1;
    Bool_t addMatch = kFALSE;
    Bool_t isExp = kTRUE;
    Bool_t isHitMakerEfficiencyMode = kFALSE;
    Bool_t isRescale = kTRUE;

    /* Hits branches */
    vector<TString> digiNames = {
        "BmnSiliconDigit", "BmnGemStripDigit", "BmnCSCDigit"
    };
    vector<TString> digiNamesOther;
    vector<TString> outMCNames = {
        "BmnSiliconDigit", "BmnGemStripDigit", "BmnCSCDigit", "StsPoint", "SiliconPoint", "CSCPoint",
        "GeoTracks"
    };
    vector<TString> outExpNames = {"SILICON", "GEM", "CSC"};
//    vector<Double_t> stripDigitThreshold = {180, 15, 50};
    vector<TF1*> funcsMC;
    vector<TF1*> funcsEx;
    vector<TF1*> funcsRescale;
    vector<TString> digiOutExpNames = (isExp == kTRUE) ? outExpNames : outMCNames;

    vector<TClass*> digiClasses; // = {
    //BmnSiliconDigit::Class(), BmnGemStripDigit::Class(), BmnCSCDigit::Class()};
    vector<TClass*> digiClassesOther;
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
    TObjArray* digiBaseBrsFull = fInTreeBase->GetListOfBranches();
    TIterator* it = digiBaseBrsFull->MakeIterator();
    it->Reset();
    TBranch* b = NULL;
    while ((b = (TBranch*) it->Next())) { // select other branches(not strip digits)
        TClass * cl = nullptr;
        Bool_t notOther = kFALSE;
        for (Int_t i = 0; i < fNArs; i++) {
            if (strcmp(b->GetName(), digiOutExpNames[i].Data()) == 0) {
                notOther = kTRUE;
                break;
            }
        }
        if (strcmp(b->GetName(), EHDigiName.Data()) == 0)
            notOther = kTRUE;
        if (notOther)
            continue;
        TClonesArray* arDigi = nullptr;
        fInTreeBase->SetBranchAddress(b->GetName(), &arDigi);
        digiNamesOther.push_back(TString(b->GetName()));
        digiBaseArsOther.push_back(arDigi);
        digiClassesOther.push_back(arDigi->GetClass());
    }
    UInt_t fNEventBase = fInTreeBase->GetEntries();
    for (Int_t i = 0; i < fNArs; i++) {
        TClonesArray* arDigi = nullptr; // new TClonesArray(BmnCSCHit::Class());
        fInTreeBase->SetBranchAddress(digiOutExpNames[i].Data(), &arDigi);
        digiBaseArs.push_back(arDigi);
        digiClasses.push_back(arDigi->GetClass());
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

    //    BmnRecoTools::GeSignalDistribution(fInTreeBase, "GEM");

    /** GetRescaleFunc */

    //    Double_t p = 0.0005;
    //    printf("inv h(%f) = %f\n", p, sig->GetX(p));
    for (Int_t i = 0; i < digiSourceArs.size(); i++) {
        TF1 *mc = BmnRecoTools::GetSignalDistribution(fInTreeSource, digiSourceArs[i]);//, 0, 1e6);
        TF1 *ex = BmnRecoTools::GetSignalDistribution(fInTreeBase, digiBaseArs[i]);
        TF1 *funcRescale = BmnRecoTools::GetRescaleFunc(TString("Rescale") + outExpNames[i], mc, ex);
        funcsMC.push_back(mc);
        funcsEx.push_back(ex);
        funcsRescale.push_back(funcRescale);
        //    funcRescale->SetNpx(50000);
        printf("%s : xmin %f  xmax %f\n", outExpNames[i].Data(), mc->GetXmin(), mc->GetXmax());
        //            printf("h(%f) = %f r = %f\n", p, funcRescale->Eval(p), mc->Eval(p));

        TString name = TString("Rescale_") + digiSourceArs[i]->GetName();
        TCanvas* can = new TCanvas(name, name, 1600 * 2, 900* 2);
        can->Divide(2, 1);
        TVirtualPad * padDistributions = can->cd(2);
        padDistributions->Divide(1, 2);
        // draw mc distr
        TVirtualPad * padMC = padDistributions->cd(1);
        padMC->SetLogx();
        padMC->SetLogy();
        mc->SetTitle("MC  Integral Distribution");
        mc->Draw();
        // draw exp distr
        TVirtualPad * padEx = padDistributions->cd(2);
        padEx->SetLogx();
        padEx->SetLogy();
        ex->SetTitle("Exp Integral Distribution");
        ex->Draw();
        // draw Rescale func
        TVirtualPad * padRescale = can->cd(1);
        padRescale->SetLogx();
        padRescale->SetLogy();
        funcRescale->Draw();
        can->Print(Form("%s.pdf", can->GetName()));
    }

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
        TClonesArray* arDigi = new TClonesArray(digiClasses[i]);
        TBranch * brDigi = fDestTree->Branch(digiOutExpNames[i], &arDigi);
        digiDestArs.push_back(arDigi);
        digiDestBrs.push_back(brDigi);

    }
    for (Int_t i = 0; i < digiBaseArsOther.size(); i++) {
        TClonesArray* arDigi = new TClonesArray(digiClassesOther[i]);
        //        printf("branch name  %s  class  %s\n", digiNamesOther[i], digiClassesOther[i]->GetName());
        fDestTree->Branch(digiNamesOther[i], &arDigi);
        digiDestArsOther.push_back(arDigi);
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
        destEH->Clear();
        for (UInt_t iBr = 0; iBr < fNArs; iBr++) {
            digiDestArs[iBr]->Clear("C");
            if (addMatch == kTRUE && iBr < matchNames.size())
                matchDestArs[iBr]->Clear("C");
        }
        for (UInt_t iBr = 0; iBr < digiDestArsOther.size(); iBr++) {
            digiDestArsOther[iBr]->Clear("C");
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
//                if (src->GetStripSignal() < stripDigitThreshold[iBr])
//                    continue;
                Double_t mc_signal = isRescale ? funcsRescale[iBr]->Eval(src->GetStripSignal()) : src->GetStripSignal();
                src->SetStripSignal(mc_signal);
                Int_t iSame = -1;
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
        for (UInt_t iBr = 0; iBr < digiDestArsOther.size(); iBr++) {
            if (turnOffBaseDigits == kFALSE)
                digiDestArsOther[iBr]->AbsorbObjects(digiBaseArsOther[iBr]);
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
//        BmnTrigUnion s = destEH->GetTrigState();
//        BmnTrigStructPeriod7SetupBMN bs;
//        bs.BC1 = true;
//        bs.BC2 = true;
//        bs.VETO = true;
//        bs.ThrBD = 2;
//        bs.ThrSI = 3;
//        s.Period7BMN = bs;
//        destEH->SetTrigState(s);
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
    
    for (Int_t i = 0; i < digiSourceArs.size(); i++) {
        delete funcsMC[i];
        delete funcsEx[i];
        delete funcsRescale[i];
    }
    
    system(Form("rm -f %s", tempBaseName.Data()));
    printf("\nFinished! Search made over %d source events\n", iSourceEvent);

    return kBMNSUCCESS;
}

BmnStatus BmnEmbedding::CloneSelected(TString BaseName, TString TempBaseName) {
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

BmnStatus BmnEmbedding::GetNextValidSourceEvent() {
    do {
        fInTreeSource->GetEntry(iSourceEvent++);
        //        if (IsReconstructable(mcTracks, stsPoints, silPoints, cscPoints, fCode, fOutCodes, fMinHits))
        return kBMNSUCCESS;
    } while (iSourceEvent < fInTreeSource->GetEntriesFast());
    return kBMNERROR;
}

BmnEmbedding::~BmnEmbedding() {
}

ClassImp(BmnEmbedding)
