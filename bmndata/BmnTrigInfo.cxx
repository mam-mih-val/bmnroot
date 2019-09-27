#include "BmnTrigInfo.h"

BmnTrigInfo::BmnTrigInfo() {
    fTrigCand = 0;
    fTrigAcce = 0;
    fTrigBefo = 0;
    fTrigAfte = 0;
    fTrigRjct = 0;
}

BmnTrigInfo::BmnTrigInfo(UInt_t cand, UInt_t acc, UInt_t befo, UInt_t afte, UInt_t rjct) {
    fTrigCand = cand;
    fTrigAcce = acc;
    fTrigBefo = befo;
    fTrigAfte = afte;
    fTrigRjct = rjct;
}

BmnTrigInfo::BmnTrigInfo(BmnTrigInfo* orig) {
    fTrigCand = orig->GetTrigCand();
    fTrigAcce = orig->GetTrigAccepted();
    fTrigBefo = orig->GetTrigBefo();
    fTrigAfte = orig->GetTrigAfter();
    fTrigRjct = orig->GetTrigRjct();
}

BmnTrigInfo::~BmnTrigInfo() {
}

ClassImp(BmnTrigInfo)
