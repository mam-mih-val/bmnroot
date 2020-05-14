#include <vector>
#include "../../bmndata/BmnEnums.h"
#include "../../bmndata/BmnSiliconHit.h"
#include "../../bmndata/BmnGemStripHit.h"
#include "../../bmndata/BmnCSCHit.h"
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

BmnStatus Embed(TString inSourceName = "eve-4649-isAll.root", TString inBaseName = "bmn_run4649_digi.root", TString outHitsName = "merged-digi.root",
        Int_t code = 3122, vector<Int_t> outCodes = {2212, -211}, Bool_t turnOffBaseDigits = kFALSE) {

    BmnEmbedding * rt = new BmnEmbedding();
    rt->Embed(inSourceName, inBaseName, outHitsName, code, outCodes, turnOffBaseDigits);
    delete rt;
    return kBMNSUCCESS;
}
