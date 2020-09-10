/** 
 * Class BmnTrigInfo
 * Trigger counters of the current spill
 * from U40VE card
 * \author Ilnur Gabdrakhmanov
 */

#ifndef TRIGINFO_H
#define TRIGINFO_H

#include "BmnEnums.h"
#include "TNamed.h"

class BmnTrigInfo : public TNamed {
public:
    BmnTrigInfo();
    BmnTrigInfo(UInt_t cand, UInt_t acc, UInt_t befo, UInt_t afte, UInt_t rjct, UInt_t all, UInt_t avail);
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
    UInt_t fTrigCand = 0;
    UInt_t fTrigAcce = 0;
    UInt_t fTrigBefo = 0;
    UInt_t fTrigAfte = 0;
    UInt_t fTrigRjct = 0;
    UInt_t fTrigAll = 0;
    UInt_t fTrigAvail = 0;

    ClassDef(BmnTrigInfo, 2)
};

#endif /* TRIGINFO_H */
