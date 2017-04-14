#include "BmnMille.h"
//___________________________________________________________________________

/// Opens outFileName (by default as binary file).

/**
 * \param[in] outFileName  file name
 * \param[in] asBinary     flag for binary
 * \param[in] writeZero    flag for keeping of zeros
 */
BmnMille::BmnMille(const Char_t* outFileName, Bool_t asBinary, Bool_t writeZero) :
myOutFile(outFileName, (asBinary ? (std::ios::binary | std::ios::out) : std::ios::out)),
myAsBinary(asBinary), myWriteZero(writeZero), myBufferPos(-1), myHasSpecial(kFALSE) {
    // Instead myBufferPos(-1), myHasSpecial(false) and the following two lines
    // we could call newSet() and kill()...
    myBufferInt[0] = 0;
    myBufferFloat[0] = 0.;

    if (!myOutFile.is_open()) {
        cerr << "Mille::Mille: Could not open " << outFileName
                << " as output file." << endl;
    }
}

//___________________________________________________________________________
/// Closes file.

BmnMille::~BmnMille() {
    myOutFile.close();
}

//___________________________________________________________________________
/// Add measurement to buffer.

/**
 * \param[in]    NLC    number of local derivatives
 * \param[in]    derLc  local derivatives
 * \param[in]    NGL    number of global derivatives
 * \param[in]    derGl  global derivatives
 * \param[in]    label  global labels
 * \param[in]    rMeas  measurement (residuum)
 * \param[in]    sigma  error
 */
void BmnMille::mille(Int_t NLC, const Double_t* derLc,
        Int_t NGL, const Double_t* derGl, const Int_t* label,
        Double_t rMeas, Double_t sigma) {
    if (sigma <= 0.) return;
    if (myBufferPos == -1) this->newSet(); // start, e.g. new track
    if (!this->checkBufferSize(NLC, NGL)) return;

    // first store measurement
    ++myBufferPos;
    myBufferFloat[myBufferPos] = rMeas;
    myBufferInt [myBufferPos] = 0;

    // store local derivatives and local 'lables' 1,...,NLC
    for (Int_t i = 0; i < NLC; ++i) {
        if (derLc[i] || myWriteZero) { // by default store only non-zero derivatives
            ++myBufferPos;
            myBufferFloat[myBufferPos] = derLc[i]; // local derivatives
            myBufferInt [myBufferPos] = i + 1; // index of local parameter
        }
    }

    // store uncertainty of measurement in between locals and globals
    ++myBufferPos;
    myBufferFloat[myBufferPos] = sigma;
    myBufferInt [myBufferPos] = 0;

    // store global derivatives and their labels
    for (Int_t i = 0; i < NGL; ++i) {
        if (derGl[i] || myWriteZero) { // by default store only non-zero derivatives
            if ((label[i] > 0 || myWriteZero) && label[i] <= myMaxLabel) { // and for valid labels
                ++myBufferPos;
                myBufferFloat[myBufferPos] = derGl[i]; // global derivatives
                myBufferInt [myBufferPos] = label[i]; // index of global parameter
            } else {
                cerr << "Mille::mille: Invalid label " << label[i]
                        << " <= 0 or > " << myMaxLabel << endl;
            }
        }
    }
}

//___________________________________________________________________________
/// Add special data to buffer.

/**
 * \param[in]    nSpecial   number of floats/ints
 * \param[in]    floatings  floats
 * \param[in]    integers   ints
 */
void BmnMille::special(Int_t nSpecial, const Double_t* floatings, const Int_t* integers) {
    if (nSpecial == 0) return;
    if (myBufferPos == -1) this->newSet(); // start, e.g. new track
    if (myHasSpecial) {
        cerr << "Mille::special: Special values already stored for this record."
                << endl;
        return;
    }
    if (!this->checkBufferSize(nSpecial, 0)) return;
    myHasSpecial = kTRUE; // after newSet() (Note: MILLSP sets to buffer position...)

    //  myBufferFloat[.]  | myBufferInt[.]
    // ------------------------------------
    //      0.0           |      0
    //  -float(nSpecial)  |      0
    //  The above indicates special data, following are nSpecial floating and nSpecial integer data.

    ++myBufferPos; // zero pair
    myBufferFloat[myBufferPos] = 0.;
    myBufferInt [myBufferPos] = 0;

    ++myBufferPos; // nSpecial and zero
    myBufferFloat[myBufferPos] = -nSpecial; // automatic conversion to float
    myBufferInt [myBufferPos] = 0;

    for (Int_t i = 0; i < nSpecial; ++i) {
        ++myBufferPos;
        myBufferFloat[myBufferPos] = floatings[i];
        myBufferInt [myBufferPos] = integers[i];
    }
}

//___________________________________________________________________________
/// Reset buffers, i.e. kill derivatives accumulated for current set.

void BmnMille::kill() {
    myBufferPos = -1;
}

//___________________________________________________________________________
/// Write buffer (set of derivatives with same local parameters) to file.

void BmnMille::end() {
    if (myBufferPos > 0) { // only if anything stored...
        const Int_t numWordsToWrite = (myBufferPos + 1)*2;

        if (myAsBinary) {
            myOutFile.write(reinterpret_cast<const char*> (&numWordsToWrite),
                    sizeof (numWordsToWrite));
            myOutFile.write(reinterpret_cast<char*> (myBufferFloat),
                    (myBufferPos + 1) * sizeof (myBufferFloat[0]));
            myOutFile.write(reinterpret_cast<char*> (myBufferInt),
                    (myBufferPos + 1) * sizeof (myBufferInt[0]));
        } else {
            myOutFile << numWordsToWrite << "\n";
            for (Int_t i = 0; i < myBufferPos + 1; ++i) {
                myOutFile << myBufferFloat[i] << " ";
            }
            myOutFile << "\n";

            for (Int_t i = 0; i < myBufferPos + 1; ++i) {
                myOutFile << myBufferInt[i] << " ";
            }
            myOutFile << "\n";
        }
    }
    myBufferPos = -1; // reset buffer for next set of derivatives
}

//___________________________________________________________________________
/// Initialize for new set of locals, e.g. new track.

void BmnMille::newSet() {
    myBufferPos = 0;
    myHasSpecial = kFALSE;
    myBufferFloat[0] = 0.0;
    myBufferInt [0] = 0; // position 0 used as error counter
}

//___________________________________________________________________________
/// Enough space for next nLocal + nGlobal derivatives incl. measurement?

/**
 * \param[in]   nLocal  number of local derivatives
 * \param[in]   nGlobal number of global derivatives
 * \return      true if sufficient space available (else false)
 */
bool BmnMille::checkBufferSize(Int_t nLocal, Int_t nGlobal) {
    if (myBufferPos + nLocal + nGlobal + 2 >= myBufferSize) {
        ++(myBufferInt[0]); // increase error count
        std::cerr << "Mille::checkBufferSize: Buffer too short ("
                << myBufferSize << "),"
                << "\n need space for nLocal (" << nLocal << ")"
                << "/nGlobal (" << nGlobal << ") local/global derivatives, "
                << myBufferPos + 1 << " already stored!"
                << std::endl;
        return kFALSE;
    } else {
        return kTRUE;
    }
}
