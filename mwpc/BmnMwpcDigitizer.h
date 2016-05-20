/* 
 * File:   BmnMwpcDigitizer.h
 * Author: Sergey Merts
 *
 * Created on October 17, 2014, 5:42 PM
 */

#ifndef BMNMWPCDIGITIZER_H
#define	BMNMWPCDIGITIZER_H

#include "TString.h"
#include "FairTask.h"
#include "FairMCPoint.h"
#include "BmnMwpcDigit.h"
#include "TClonesArray.h"

using namespace std;

class BmnMwpcDigitizer : public FairTask {
public:

    /** Default constructor **/
    BmnMwpcDigitizer(Short_t num);

    /** Destructor **/
    virtual ~BmnMwpcDigitizer();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Finish **/
    virtual void Finish();
    
    TVector3 Global2Local(TVector3 gl, Short_t iPlane);

private:

    TString fInputBranchName;
    TString fOutputBranchName;

    /** Input array of MWPC Points **/
    TClonesArray* fBmnMwpcPointArray;

    /** Input array of MC Tracks **/
//    TClonesArray* fMCTracksArray;

    /** Output array of MWPC Digits **/
    TClonesArray* fBmnMwpcDigitArray;

    Int_t fMwpcNum; //nubmer of proportional chamber

    ClassDef(BmnMwpcDigitizer, 1);

};

#endif	/* BMNMWPCDIGITIZER_H */
