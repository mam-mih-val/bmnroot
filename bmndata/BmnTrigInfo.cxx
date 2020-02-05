#include "BmnTrigInfo.h"

BmnTrigInfo::BmnTrigInfo() {
}

BmnTrigInfo::BmnTrigInfo(UInt_t cand, UInt_t acc, UInt_t befo, UInt_t afte, UInt_t rjct, UInt_t all, UInt_t avail) {
    fTrigCand = cand;
    fTrigAcce = acc;
    fTrigBefo = befo;
    fTrigAfte = afte;
    fTrigRjct = rjct;
    fTrigAll = all;
    fTrigAvail = avail;
}

BmnTrigInfo::BmnTrigInfo(BmnTrigInfo* orig) {
    fTrigCand = orig->GetTrigCand();
    fTrigAcce = orig->GetTrigAccepted();
    fTrigBefo = orig->GetTrigBefo();
    fTrigAfte = orig->GetTrigAfter();
    fTrigRjct = orig->GetTrigRjct();
    fTrigAll = orig->GetTrigAll();
    fTrigAvail = orig->GetTrigAvail();
}

BmnTrigInfo::~BmnTrigInfo() {
}

ClassImp(BmnTrigInfo)
