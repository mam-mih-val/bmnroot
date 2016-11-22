#ifndef BMNTRIGDIGIT_H
#define BMNTRIGDIGIT_H

#include "TNamed.h"

using namespace std;

class BmnTrigDigit : public TNamed {
public:

    /** Default constructor **/
    BmnTrigDigit();

    /** Constructor to use **/
    BmnTrigDigit(Short_t iDet, Short_t iMod, Double_t iTime, Double_t iAmp);

    Short_t GetDet() const {
        return fDet;
    }

    Short_t GetMod() const {
        return fMod;
    }

    Double_t GetTime() const {
        return fTime;
    }

    Double_t GetAmp() const {
        return fAmp;
    }

    void SetAmp(Double_t amp) {
        fAmp = amp;
    }

    void SetMod(Short_t mod) {
        fMod = mod;
    }

    void SetTime(Double_t time) {
        fTime = time;
    }

    void SetDet(Short_t det) {
        fDet = det;
    }

    /** Destructor **/
    virtual ~BmnTrigDigit();

private:

    Short_t fDet;
    Short_t fMod;
    Double_t fTime;
    Double_t fAmp;

    ClassDef(BmnTrigDigit, 1);

};


#endif /* BMNTRIGDIGIT_H */

