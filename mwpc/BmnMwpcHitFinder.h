/* 
 * File:   BmnMwpcHitFinder.h
 * Author: Sergey Merts
 *
 * Created on October 22, 2014, 5:30 PM
 */

#ifndef BMNMWPCHITFINDER_H
#define	BMNMWPCHITFINDER_H


#include "TString.h"
#include "FairTask.h"
#include "BmnMwpcDigit.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TH2F.h"

using namespace std;

class TClonesArray;

class BmnMwpcHitFinder : public FairTask {
public:

    /** Default constructor **/
    BmnMwpcHitFinder(Short_t num);

    /** Destructor **/
    virtual ~BmnMwpcHitFinder();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Finish **/
    virtual void Finish();
    
    TVector3 Global2Local(Float_t xGl, Float_t yGl, Short_t iPlane);
    TVector3 Local2Global(Float_t xLoc, Float_t yLoc, Short_t iPlane);
    void FillHisto(TH2F* h);
    Float_t CalcSquare(Float_t b1, Float_t b2, Float_t b3);
    TLorentzVector* CalcHitPosByTwoDigits(BmnMwpcDigit* dI, BmnMwpcDigit* dJ);
    vector<TLorentzVector*> CreateHitsByTwoPlanes(vector<BmnMwpcDigit*> x, vector<BmnMwpcDigit*> y);
    void SearchIn3Pairs();
    void SearchIn5Pairs();
    void AccumulateSignals(vector<TVector3*> vec, TH2F* h);

private:

    TString fInputBranchName;
    TString fOutputBranchName;

    /** Input array of MWPC Points **/
    TClonesArray* fBmnMwpcDigitArray;

    /** Input array of MC Tracks **/
//    TClonesArray* fMCTracksArray;

    /** Output array of MWPC Digits **/
    TClonesArray* fBmnMwpcHitArray;
    TClonesArray* fBmnMwpcPointArray;
    TH2F* fHisto; //histogram for hit finding

    Int_t fMwpcNum; //number of proportional chamber
    Int_t fMwpcZpos; //Z-position of proportional chamber center

    ClassDef(BmnMwpcHitFinder, 1);

};

#endif	/* BMNMWPCHITFINDER_H */

