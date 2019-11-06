#include <vector>
#include "../../bmndata/BmnEnums.h"
#include "../../bmndata/BmnSiliconHit.h"
#include "../../bmndata/BmnGemStripHit.h"
#include "../../bmndata/BmnCSCHit.h"
#include <root/Rtypes.h>
#include <root/TString.h>
#include <root/TFile.h>
#include <root/TTree.h>
#include <root/TClass.h>
#include<root/TClonesArray.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

using namespace std;

BmnStatus RemoveBranches(TString fileName, vector<TString> branchNames) {
    TFile *fBaseHits = new TFile(fileName, "UPDATE");
    if (fBaseHits->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s !\n", fileName.Data());
        return kBMNERROR;
    }
    TTree * t = (TTree *) fBaseHits->Get("bmndata");
    TObjArray *list = t->GetListOfBranches();
    for (auto &el : branchNames) {
        printf("Removing %s\n", el.Data());
        TBranch *b = t->GetBranch(el.Data());
        list->Remove(b);
        t->Write();
        break;
    }
    fBaseHits->Close();
    return kBMNSUCCESS;
}

BmnStatus Embed(TString inSourceName = "evetest-p.root", TString inBaseName = "evetest-pim.root", TString outHitsName = "merged-digi.root") {

    UInt_t fPeriodId = 7;
    Int_t EmbeddedType = 1;
    Bool_t addMatch = kTRUE;
    /* Hits branches */
    vector<TString> hitsNames = {"BmnSiliconHit", "BmnGemStripHit", "BmnCSCHit"};
    vector<TClass*> hitsClasses = {BmnSiliconHit::Class(), BmnGemStripHit::Class(), BmnCSCHit::Class()};

    vector<TString> digiNames = {"BmnSiliconDigit", "BmnGemStripDigit", "BmnCSCDigit"};
    vector<TString> digiExpNames = {"BmnSiliconDigit", "BmnGemStripDigit", "BmnCSCDigit"};
    vector<TString> digiOutExpNames = {"SILICON", "GEM", "CSC"};
    vector<TClass*> digiClasses = {BmnSiliconDigit::Class(), BmnGemStripDigit::Class(), BmnCSCDigit::Class()};
    vector<TString> matchNames = {"BmnSiliconDigitMatch", "BmnGemStripDigitMatch", "BmnCSCDigitMatch"};
    vector<TClonesArray*> digiSourceArs; ///<- source digi arrays
    vector<TClonesArray*> digiBaseArs; ///<- base digi arrays
    vector<TClonesArray*> digiDestArs; ///<- destination digi arrays
    vector<TBranch*> digiDestBrs; ///<- destination digi branches
    vector<TClonesArray*> matchSourceArs; ///<- source match arrays
    vector<TClonesArray*> matchBaseArs; ///<- source base arrays
    vector<TClonesArray*> matchDestArs; ///<- destination match arrays
    vector<TBranch*> matchDestBrs; ///<- destination match branches

    DstEventHeader * copyBaseEH = nullptr;
    DstEventHeader * copyDestEH = nullptr;
    BmnEventHeader * bmnEH = nullptr;
    TBranch *EHBranch = nullptr;
    FairMCEventHeader * mcEH = nullptr;
    TString EHMCName = "MCEventHeader.";
    TString EHDigiName = "BmnEventHeader.";
    TString FieldParName = "BmnFieldPar";
    TList* fBranchList = new TList();
    BmnFieldPar *fieldPar = nullptr;
    UInt_t fNArs = digiNames.size();


//    Int_t retn = system(Form("cp %s %s", inBaseName.Data(), outHitsName.Data()));
//    printf("copy return %d\n", retn);
    //    RemoveBranches(outHitsName, digiExpNames);

    /*****************************/
    /** Open input source hits **/
    /*****************************/
    TFile *fSourceHits = new TFile(inSourceName, "READ");
    if (fSourceHits->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s !\n", inSourceName.Data());
        return kBMNERROR;
    }
    printf("\nINPUT SOURCE HITS FILE: ");
    printf("%s\n", inSourceName.Data());
    TTree * fInTreeSource = (TTree *) fSourceHits->Get("bmndata");
    UInt_t fNEventSource = fInTreeSource->GetEntries();
    for (Int_t i = 0; i < fNArs; i++) {
        TClonesArray* arDigi = nullptr; // new TClonesArray(BmnCSCHit::Class());
        fInTreeSource->SetBranchAddress(digiNames[i], &arDigi);
        digiSourceArs.push_back(arDigi);
        TClonesArray* ar = nullptr;
        fInTreeSource->SetBranchAddress(matchNames[i], &ar);
        matchSourceArs.push_back(ar);
    }
    fieldPar = (BmnFieldPar*)fSourceHits->Get(FieldParName.Data());
    
    /*****************************/
    /** Open input base hits **/
    /*****************************/
    TFile *fBaseHits = new TFile(inBaseName, "READ");
    if (fBaseHits->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s !\n", inBaseName.Data());
        return kBMNERROR;
    }
    printf("\nINPUT SOURCE HITS FILE: ");
    printf("%s\n", inBaseName.Data());
    TTree * fInTreeBase = (TTree *) fBaseHits->Get("bmndata");
    UInt_t fNEventBase = fInTreeBase->GetEntries();
    for (Int_t i = 0; i < fNArs; i++) {
        TClonesArray* arDigi = nullptr; // new TClonesArray(BmnCSCHit::Class());
        fInTreeBase->SetBranchAddress(digiExpNames[i], &arDigi);
        digiBaseArs.push_back(arDigi);
        TClonesArray* ar = nullptr;
        fInTreeBase->SetBranchAddress(matchNames[i], &ar);
        matchBaseArs.push_back(ar);
    }
    fInTreeBase->SetBranchAddress(EHMCName.Data(), &mcEH);
    //    fInTreeBase->SetBranchAddress("DstEventHeader.", &copyBaseEH);
    //    /*****************************/
    //    /** Open input base hits **/
    //    /*****************************/
    //    TFile *fBaseHits = new TFile(inBaseName, "UPDATE");
    //    if (fBaseHits->IsOpen() == false) {
    //        printf("\n!!!!\ncannot open file %s !\n", inBaseName.Data());
    //        return kBMNERROR;
    //    }
    //    printf("\nINPUT SOURCE HITS FILE: ");
    //    printf("%s\n", inBaseName.Data());
    //    TTree * fInTreeBase = (TTree *) fBaseHits->Get("bmndata");
    //    UInt_t fNEventBase = fInTreeBase->GetEntries();
    //    for (Int_t i = 0; i < fNArs; i++) {
    //        TClonesArray* ar = nullptr; // new TClonesArray(BmnCSCHit::Class());
    //        fInTreeBase->SetBranchAddress(hitsNames[i], &ar);
    //        hitBaseArs.push_back(ar);
    //    }
    //    fInTreeBase->SetBranchAddress("DstEventHeader.", &copyBaseEH);


    /*****************************/
    /** Create output hits file **/
    /*****************************/
    TFile *fDestHitsFile = new TFile(outHitsName, "RECREATE");
    if (fDestHitsFile->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s !\n", outHitsName.Data());
        return kBMNERROR;
    }
    printf("\nOUT HITS FILE: ");
    printf("%s\n", outHitsName.Data());
    //    TTree * fDestTree = new TTree("bmndata", "bmndata");
    //    TTree * fDestTree = (TTree *) fDestHitsFile->Get("bmndata");

    // clone tree except some branches (deactivate=>clone=>activate)
    for (Int_t i = 0; i < fNArs; i++)
        fInTreeSource->SetBranchStatus(digiNames[i] + ".*", 0);
    for (Int_t i = 0; i < fNArs; i++)
        fInTreeSource->SetBranchStatus(matchNames[i] + ".*", 0);
    TTree * fDestTree = fInTreeSource->CloneTree(-1, "fast");
    fDestTree->Write();
    for (Int_t i = 0; i < fNArs; i++)
        fInTreeSource->SetBranchStatus(digiNames[i] + ".*", 1);
    for (Int_t i = 0; i < fNArs; i++)
        fInTreeSource->SetBranchStatus(matchNames[i] + ".*", 1);

    // create that branches in the new tree    
    UInt_t fNEventDest = fDestTree->GetEntries();
    for (Int_t i = 0; i < fNArs; i++) {
        if (addMatch) {
            TClonesArray* ar = new TClonesArray(BmnMatch::Class());
            TBranch * br = fDestTree->Branch(matchNames[i], &ar);
            matchDestArs.push_back(ar);
            matchDestBrs.push_back(br);
        }
        TClonesArray* arDigi = new TClonesArray(digiClasses[i]); //nullptr;
        TBranch* brDigi = fDestTree->Branch(digiOutExpNames[i], &arDigi);
        digiDestArs.push_back(arDigi);
        digiDestBrs.push_back(brDigi);
        
    }
    //    copyDestEH = new DstEventHeader();
    //    fDestTree->Branch("DstEventHeader.", copyDestEH);
    bmnEH = new BmnEventHeader();
    EHBranch = fDestTree->Branch(EHDigiName.Data(), bmnEH);
    TList * branches = (TList*) fDestTree->GetListOfBranches();
    for (Int_t i = 0; i < branches->GetEntries(); i++) {
        TObjString * s = new TObjString(branches->At(i)->GetName());
        fBranchList->Add(s);
    }

    /*****************************/
    /** Fill hits **/
    /*****************************/
    UInt_t minEvents = Min(fNEventSource, fNEventDest);
    for (UInt_t iEv = 0; iEv < minEvents; ++iEv) {
        for (UInt_t iBr = 0; iBr < fNArs; iBr++) {
            digiDestArs[iBr]->Clear("C");
            if (addMatch)
                matchDestArs[iBr]->Clear("C");
        }
        fInTreeBase->GetEntry(iEv);
        fInTreeSource->GetEntry(iEv);
        //        fDestTree->GetEntry(iEv);
        for (UInt_t iBr = 0; iBr < fNArs; iBr++) {
            //            printf("iEv %u iBr %u \n", iEv, iBr);
            UInt_t fNSourceHits = digiSourceArs[iBr]->GetEntries();
            //            printf(" was %d entries source\n", matchSourceArs[iBr]->GetEntries());
            //            printf(" was %d entries dest\n", matchDestArs[iBr]->GetEntries());

            Int_t fNDigiBase = digiBaseArs[iBr]->GetEntriesFast();
            digiDestArs[iBr]->AbsorbObjects(digiBaseArs[iBr]);
            BmnSiliconDigit * dig;
            BmnGemStripDigit * dig1;
            BmnCSCDigit * dig2;
            for (UInt_t iHit = 0; iHit < fNSourceHits; iHit++) {
                switch (iBr) {
                    case 0:
                        dig = (BmnSiliconDigit*) digiSourceArs[iBr]->At(iHit);
                        dig->SetFlag(EmbeddedType);
                        break;
                    case 1:
                        dig1 = (BmnGemStripDigit*) digiSourceArs[iBr]->At(iHit);
                        dig1->SetFlag(EmbeddedType);
                        break;
                    case 2:
                        dig2 = (BmnCSCDigit*) digiSourceArs[iBr]->At(iHit);
                        dig2->SetFlag(EmbeddedType);
                        break;
                    default:
                        break;
                }
                //                digiClasses[iHit] dig = (digiClasses[iHit]) digiSourceArs[iBr]->At(iHit);
                //                BmnHit* hit = (BmnHit*) hitSourceArs[iBr]->At(iHit);
                //                hit->SetType(EmbeddedType);
                //                hitDestArs[iBr]->Add(hit);
                //                new 
            }
            for (UInt_t iMatch = 0; iMatch < fNDigiBase; iMatch++) {
                new ((*matchDestArs[iBr])[matchDestArs[iBr]->GetEntriesFast()]) BmnMatch();
            }
            digiDestArs[iBr]->AbsorbObjects(digiSourceArs[iBr]);
            digiDestBrs[iBr]->Fill();
            if (addMatch) {
                matchDestArs[iBr]->AbsorbObjects(matchSourceArs[iBr]);
                //                printf(" is %d entries\n", matchDestArs[iBr]->GetEntries());
                matchDestBrs[iBr]->Fill();
            }

        }
        //        copyDestEH->Clear();
        //        copyDestEH->SetEventId(copyBaseEH->GetEventId());
        //        copyDestEH->SetEventTime(copyBaseEH->GetEventTime());
        //        copyDestEH->SetEventTimeTS(copyBaseEH->GetEventTimeTS());
        //        copyDestEH->SetADCin(copyBaseEH->GetADCin());
        //        copyDestEH->SetADCout(copyBaseEH->GetADCout());
        //        copyDestEH->SetB(copyBaseEH->GetB());
        //        copyDestEH->SetHeaderName(copyBaseEH->GetHeaderName());
        //        copyDestEH->SetInputFileId(copyBaseEH->GetInputFileId());
        //        copyDestEH->SetMCEntryNumber(copyBaseEH->GetMCEntryNumber());
        //        //        copyDestEH->SetRunId(copyBaseEH->GetRunId());
        //        copyDestEH->SetTriggerType(copyBaseEH->GetTriggerType());
        //        copyDestEH->SetZ2in(copyBaseEH->GetZ2in());
        //        copyDestEH->SetZ2out(copyBaseEH->GetZ2out());
        bmnEH->Clear();
        bmnEH->SetEventId(mcEH->GetEventID());
        //        bmnEH->SetEventTime(copyBaseEH->GetEventTime());
        //        bmnEH->SetEventTimeTS(copyBaseEH->GetEventTimeTS());
        bmnEH->SetEventType(kBMNPAYLOAD);
        bmnEH->SetPeriodId(fPeriodId);
        bmnEH->SetRunId(4649); //mcEH->GetRunID());
        EHBranch->Fill();
        //        fDestTree->Fill();
    }
    fDestHitsFile->WriteObject(fBranchList, "BranchList");
    fDestHitsFile->WriteObject(fieldPar, FieldParName.Data());

    fDestTree->Write();
    fDestHitsFile->Write();
    if (fSourceHits)
        fSourceHits->Close();
    if (fBaseHits)
        fBaseHits->Close();
    if (fDestHitsFile)
        fDestHitsFile->Close();

    return kBMNSUCCESS;
}