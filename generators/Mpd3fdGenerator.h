/* 
 * File:   Mpd3fdGenerator.h
 * Author: Sergei Merts
 *
 * Created on June 22, 2015, 2:06 PM
 */

#ifndef MPD3FDGENERATOR_H
#define	MPD3FDGENERATOR_H

#include "FairGenerator.h"
#include "TFile.h"
#include "TChain.h"
#include <fstream>

const UInt_t kBatyukConst = 1e5;

class TVirtualMCStack;
class FairPrimaryGenerator;

class Mpd3fdGenerator : public FairGenerator {
public:

    /** Default constructor without arguments should not be used. **/
    Mpd3fdGenerator();


    /** Standard constructor.
     * @param fileName The input file name
     **/
    Mpd3fdGenerator(TString fileName);


    /** Destructor. **/
    ~Mpd3fdGenerator();

    Bool_t ReadEvent(FairPrimaryGenerator* primGen);

    void SkipEvents(Int_t ev) {
        fEventNumber = ev;
    }

private:

    TFile* fInputFile; //!  Input file
    TString fFileName; //!  Input file name
    TChain *fDstTree; //!
    Double_t fPx[kBatyukConst]; //!
    Double_t fPy[kBatyukConst]; //!
    Double_t fPz[kBatyukConst]; //!
    Double_t fX[kBatyukConst]; //!
    Double_t fY[kBatyukConst]; //!
    Double_t fZ[kBatyukConst]; //!
    Double_t fE[kBatyukConst]; //!
    Int_t fPID[kBatyukConst]; //!
    UInt_t fNpart; //!
    Int_t fEventNumber; //!


    Mpd3fdGenerator(const Mpd3fdGenerator&) = delete;
    Mpd3fdGenerator& operator=(const Mpd3fdGenerator&) = delete;

    ClassDef(Mpd3fdGenerator, 1);
};

#endif	/* MPD3FDGENERATOR_H */

