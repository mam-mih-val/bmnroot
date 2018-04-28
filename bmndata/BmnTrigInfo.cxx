/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TrigInfo.cxx
 * Author: ilnur
 * 
 * Created on 25 апреля 2018 г., 12:12
 */

#include "BmnTrigInfo.h"

BmnTrigInfo::BmnTrigInfo() {
    fTrigType = kBMNBEAM;
    fTrigCand = 0;
    fTrigAcce = 0;
    fTrigBefo = 0;
    fTrigAfte = 0;
    fTrigRjct = 0;
}

BmnTrigInfo::BmnTrigInfo(BmnTriggerType trig, UInt_t cand, UInt_t acc, UInt_t befo, UInt_t afte, UInt_t rjct) {
    fTrigType = trig;
    fTrigCand = cand;
    fTrigAcce = acc;
    fTrigBefo = befo;
    fTrigAfte = afte;
    fTrigRjct = rjct;
}

BmnTrigInfo::BmnTrigInfo(BmnTrigInfo* orig) {
    fTrigType = orig->GetTrigType();
    fTrigCand = orig->GetTrigCand();
    fTrigAcce = orig->GetTrigAccepted();
    fTrigBefo = orig->GetTrigBefo();
    fTrigAfte = orig->GetTrigAfter();
    fTrigRjct = orig->GetTrigRjct();
}

BmnTrigInfo::~BmnTrigInfo() {
}

ClassImp(BmnTrigInfo)
