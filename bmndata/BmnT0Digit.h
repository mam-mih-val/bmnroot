#ifndef BMNT0DIGIT_H
#define BMNT0DIGIT_H

#include "TNamed.h"

using namespace std;

class BmnT0Digit : public TNamed {
public:

    /** Default constructor **/
    BmnT0Digit();

    /** Constructor to use **/
    BmnT0Digit(Short_t iDet, Short_t iMod, Float_t iTime, Float_t iAmp);

    Short_t GetDet() const {
        return fDet;
    }

    Short_t GetMod() const {
        return fMod;
    }

    Float_t GetTime() const {
        return fTime;
    }

    Float_t GetAmp() const {
        return fAmp;
    }

    void SetAmp(Float_t amp) {
        fAmp = amp;
    }

    void SetMod(Short_t mod) {
        fMod = mod;
    }

    void SetTime(Float_t time) {
        fTime = time;
    }

    void SetDet(Short_t det) {
        fDet = det;
    }

    /** Destructor **/
    virtual ~BmnT0Digit();

private:

    Short_t fDet;
    Short_t fMod;
    Float_t fTime;
    Float_t fAmp;

    ClassDef(BmnT0Digit, 1);

};


#endif /* BMNT0DIGIT_H */

