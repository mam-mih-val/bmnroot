/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TrigInfo.h
 * Author: ilnur
 *
 * Created on 25 апреля 2018 г., 12:12
 */

#ifndef TRIGINFO_H
#define TRIGINFO_H

#include "BmnEnums.h"
#include "TNamed.h"

class BmnTrigInfo : public TNamed {
public:
    BmnTrigInfo();
    BmnTrigInfo(BmnTriggerType trig, UInt_t cand, UInt_t acc, UInt_t befo, UInt_t afte, UInt_t rjct);
    BmnTrigInfo(BmnTrigInfo* orig);
    virtual ~BmnTrigInfo();
    
    void SetTrigCand(UInt_t _v) {
        fTrigCand = _v;
    }

    UInt_t GetTrigCand() {
        return fTrigCand;
    }

    void SetTrigAccepted(UInt_t _v) {
        fTrigAcce = _v;
    }

    UInt_t GetTrigAccepted() {
        return fTrigAcce;
    }

    void SetTrigBefo(UInt_t _v) {
        fTrigBefo = _v;
    }

    UInt_t GetTrigBefo() {
        return fTrigBefo;
    }

    void SetTrigAfter(UInt_t _v) {
        fTrigAfte = _v;
    }

    UInt_t GetTrigAfter() {
        return fTrigAfte;
    }

    void SetTrigRjct(UInt_t _v) {
        fTrigRjct = _v;
    }

    UInt_t GetTrigRjct() {
        return fTrigRjct;
    }
    
    void SetTrigType(BmnTriggerType type) {
        fTrigType = type;
    }

    /** Get the type of this event*/
    BmnTriggerType GetTrigType() {
        return fTrigType;
    }
    
    
private:
    /** Trigger Type (beam = 6 or target = 1)**/
    BmnTriggerType fTrigType;
    UInt_t fTrigCand;
    UInt_t fTrigAcce;
    UInt_t fTrigBefo;
    UInt_t fTrigAfte;
    UInt_t fTrigRjct;

    ClassDef(BmnTrigInfo, 1)
};

#endif /* TRIGINFO_H */
