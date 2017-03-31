#ifndef BMNMILLE_H
#define BMNMILLE_H 1

#include <fstream>
#include <iostream>
#include <Rtypes.h>
#include <math.h>

using namespace std;

class BmnMille {
public:
    BmnMille(const Char_t* outFileName, Bool_t asBinary = kTRUE, Bool_t writeZero = kFALSE);
    ~BmnMille();

    void mille(Int_t NLC, const Double_t* derLc, Int_t NGL, const Double_t* derGl,
            const Int_t* label, Double_t rMeas, Double_t sigma);
    void special(Int_t nSpecial, const Double_t* floatings, const Int_t* integers);
    void kill();
    void end();

private:
    void newSet();
    Bool_t checkBufferSize(Int_t nLocal, Int_t nGlobal);

    std::ofstream myOutFile; ///< C-binary for output
    Bool_t myAsBinary; ///< if false output as text
    Bool_t myWriteZero; ///< if true also write out derivatives/labels ==0
    /// buffer size for ints and floats

    enum {
        myBufferSize = 500000
    }; ///< buffer size for ints and floats
    Int_t myBufferInt[myBufferSize]; ///< to collect labels etc.
    Float_t myBufferFloat[myBufferSize]; ///< to collect derivatives etc.
    Int_t myBufferPos; ///< position in buffer
    Bool_t myHasSpecial; ///< if true, special(..) already called for this record
    /// largest label allowed: 2^31 - 1

    enum {
        myMaxLabel = (0xFFFFFFFF - (1 << 31))
    };
};
#endif
