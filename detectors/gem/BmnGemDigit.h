#include <TNamed.h>

class BmnGemDigit : public TNamed {
public:

    BmnGemDigit();
    BmnGemDigit(BmnGemDigit* digit);
    BmnGemDigit(Int_t iStrip, Int_t iStat, Float_t Q);
    virtual ~BmnGemDigit();

    Int_t GetLightedStrip() const {
        return fLighted;
    }

    Float_t GetQ() const {
        return fAdc;
    }

    Int_t GetStationNum() const {
        return fNumber;
    }


private:

    Int_t fLighted; // Number of lighted strip 

    Int_t fNumber; // Station number
    Float_t fAdc; // charge on strip

    ClassDef(BmnGemDigit, 1)
};
