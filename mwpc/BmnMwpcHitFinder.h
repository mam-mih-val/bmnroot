// @(#)bmnroot/mwpc:$Id$
// Author: Pavel Batyuk (VBLHEP) <pavel.batyuk@jinr.ru> 2017-02-10

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnMwpcHitFinder                                                          //
//                                                                            //
// Implementation of an algorithm developed by                                // 
// S. Merts and P. Batyuk                                                     //
// to the BmnRoot software                                                    //
//                                                                            //
// The algorithm serves for searching for hits                                //
// in the Multi Wire Prop. Chambers of the BM@N experiment                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNMWPCHITFINDER_H
#define	BMNMWPCHITFINDER_H

#include <map>
#include <algorithm>
#include <Rtypes.h>
#include <TClonesArray.h>
#include <TVector3.h>
#include <TMath.h>
#include <TString.h>
#include  "FairTask.h"
#include  "BmnMwpcHit.h"
#include  "BmnMwpcDigit.h"
#include  "BmnMwpcGeometry.h"
#include  "FairTask.h"

using namespace std;

class BmnMwpcHitFinder : public FairTask {
public:

    /** Default constructor **/
    BmnMwpcHitFinder() {};
    
    /** Constructor to be used **/
    BmnMwpcHitFinder(Bool_t);

    /** Destructor **/
    virtual ~BmnMwpcHitFinder();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Finish **/
    virtual void Finish();
    
    void SetUseDigitsInTimeBin(Bool_t flag) {
        fUseDigitsInTimeBin = flag;
    }

private:
    Bool_t expData;
    UInt_t fEventNo; // event counter
    
    TString fInputBranchName;
    TString fOutputBranchName;
    
    /** Input array of MWPC digits **/
    TClonesArray* fBmnMwpcDigitArray;
    
    /** Output array of MWPC hits **/
    TClonesArray* fBmnMwpcHitArray; 
    
    vector <BmnMwpcDigit*> CheckDigits(vector <BmnMwpcDigit*>);
    void FindNeighbour(BmnMwpcDigit*, vector <BmnMwpcDigit*>, vector<BmnMwpcDigit*>);
    vector <TVector3> CreateHitsBy3Planes(vector <BmnMwpcDigit*>, vector <BmnMwpcDigit*>, vector <BmnMwpcDigit*>, Float_t);
    TVector3 CalcHitPosByTwoDigits(BmnMwpcDigit*, BmnMwpcDigit*);
    void CreateMwpcHits(vector <TVector3>, TClonesArray*, Short_t);
    void DefineCoordinateAngle(Short_t, Double_t&, Double_t&);
    void FindPairs(vector <BmnMwpcDigit*>, vector <BmnMwpcDigit*>, vector <TVector3>&);
    
    Float_t thDist;      // distance between found hits [cm]
    Int_t nInputDigits;  // max. number of found digits per plane
    Int_t nTimeSamples;  // 
      
    Bool_t fUseDigitsInTimeBin; // use digits found in a time bin of width = kTimeBin (8 ns).
    
    BmnMwpcGeometry* fMwpcGeometry;

    ClassDef(BmnMwpcHitFinder, 1);

};

#endif	

