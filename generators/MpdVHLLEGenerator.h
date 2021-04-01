/* 
 * File:   MpdVHLLEGenerator.h
 * Author: Pavel Batyuk
 *
 * Created on April 27, 2016
 */

#ifndef MPDVHLLEGENERATOR_H
#define MPDVHLLEGENERATOR_H

#include <iostream>
#include "FairGenerator.h"
#include "FairMCEventHeader.h"
#include "FairPrimaryGenerator.h"
#include "TFile.h"
#include "TChain.h"

using namespace std;
using namespace TMath;

const UInt_t dim = 1e5;

class TVirtualMCStack;
class FairPrimaryGenerator;

class MpdVHLLEGenerator : public FairGenerator {
public:

    MpdVHLLEGenerator();
    MpdVHLLEGenerator(TString fileName, Bool_t isCascade);
    ~MpdVHLLEGenerator();

    Bool_t ReadEvent(FairPrimaryGenerator* primGen);

    void SkipEvents(Int_t ev) {
        fEventNumber = ev;
        cout << "NUMBER OF SKIPPED EVENTS = " << ev << endl;
    }

private:
    TFile* fInputFile; //!  Input file
    TString fFileName; //!  Input file name
    TChain* fDstTree; //!
    Float_t fPx[dim]; //!
    Float_t fPy[dim]; //!
    Float_t fPz[dim]; //!
    Float_t fX[dim]; //!
    Float_t fY[dim]; //!
    Float_t fZ[dim]; //!
    Float_t fE[dim]; //!
    Int_t fPID[dim]; //!
    Int_t fNpart; //!
    Int_t fEventNumber; //!
    TString fBranch; //! treefin corresponds to hydro + cascade, treeini -- to hydro calculations only
    
    void SetCascade(Bool_t flag) {
        fBranch = (flag) ? "treefin" : "treeini";
    }

    MpdVHLLEGenerator(const MpdVHLLEGenerator&) = delete;
    MpdVHLLEGenerator& operator=(const MpdVHLLEGenerator&) = delete;

    ClassDef(MpdVHLLEGenerator, 1);
};
#endif 
