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
    BmnTrigInfo(UInt_t cand, UInt_t acc, UInt_t befo, UInt_t afte, UInt_t rjct);
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

    void SetTrigAll(UInt_t _v) {
        fTrigAll = _v;
    }

    UInt_t GetTrigAll() {
        return fTrigAll;
    }

    void SetTrigAvail(UInt_t _v) {
        fTrigAvail = _v;
    }

    UInt_t GetTrigAvail() {
        return fTrigAvail;
    }
    
private:
    UInt_t fTrigCand;
    UInt_t fTrigAcce;
    UInt_t fTrigBefo;
    UInt_t fTrigAfte;
    UInt_t fTrigRjct;
    UInt_t fTrigAll;
    UInt_t fTrigAvail;

    ClassDef(BmnTrigInfo, 2)
};

#endif /* TRIGINFO_H */
