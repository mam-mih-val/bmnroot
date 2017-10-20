#ifndef BMNTRIGDIGIT_H
#define BMNTRIGDIGIT_H 

#include "TNamed.h"

using namespace std;

class BmnTrigDigit : public TNamed {
public:

    /** Default constructor **/
    BmnTrigDigit();

    /** Constructor to use **/
    BmnTrigDigit(Short_t iMod, Double_t iTime, Double_t iAmp);

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

    /** Destructor **/
    virtual ~BmnTrigDigit();

protected:

    Short_t fMod; //inner channel (for Barrel 40 channels, for T0, BC1, BC2, VC, FD only one)
    Double_t fTime;
    Double_t fAmp;

    ClassDef(BmnTrigDigit, 1);

};


#endif /* BMNTRIGDIGIT_H */

