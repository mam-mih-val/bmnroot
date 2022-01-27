/* 
 * File:   BmnHit.h
 * Author: Sergey Merts
 *
 * Created on October 28, 2014, 11:26 AM
 */

#ifndef BMNHIT_H
#define	BMNHIT_H

#include "FairHit.h"
#include <TObject.h>
#include "BmnDetectorList.h"
#include "BmnMatch.h"

using namespace std;

// class TClonesArray;

class BmnHit : public FairHit {
public:

    /** Default constructor **/
    BmnHit();

    /** Constructor to use **/
    BmnHit(Int_t detUID, TVector3 posHit, TVector3 posHitErr, Int_t pointIndx);

    Bool_t IsUsed() const {
        return fUsing;
    }

    Bool_t GetFlag() const {
        return fFlag;
    }

    Int_t GetIndex() const {
        return fIndex;
    }

    DetectorId GetDetId() const {
        return fDetId;
    }

    Short_t GetStation() const {
        return fStation;
    }

    void SetFlag(Bool_t fl) {
        fFlag = fl;
    }

    void SetUsing(Bool_t use) {
        fUsing = use;
    }

    void SetIndex(Int_t id) {
        fIndex = id;
    }

    void SetLength(Double_t len) {
        fLength = len;
    }

    void SetDetId(DetectorId det) {
        fDetId = det;
    }

    void SetStation(Short_t st) {
        fStation = st;
    }
    
    void SetModule(Int_t mod) {
        fModule = mod;
    }
    
    Int_t GetModule() {
        return fModule;
    }

    void SetType(Int_t type) {
        fType = type;
    }

    Int_t GetType() {
        return fType;
    }
    
    Double_t GetLength() {
        return fLength;
    }
    
    void SetResXY(Double_t resX, Double_t resY) {
        fResX = resX;
        fResY = resY;
    }
    
    Double_t GetResX() {
        return fResX;
    }
    
    Double_t GetResY() {
        return fResY;
    }

    /** Destructor **/
    virtual ~BmnHit();
    
    Double_t GetCovXY() { 
        return fCovXY; 
    }
    
    void SetCovXY(Double_t val) {
        fCovXY = val;
    }

    BmnMatch GetDigitNumberMatch() {
        return fDigitNumberMatch;
    }

    void SetDigitNumberMatch(BmnMatch match) {
        fDigitNumberMatch = match;
    }

    Int_t GetUpperClusterIndex() {
        return fUpperClusterIndex;
    }

    Int_t GetLowerClusterIndex() {
        return fLowerClusterIndex;
    }

    void SetUpperClusterIndex(Int_t idx) {
        fUpperClusterIndex = idx;
    }

    void SetLowerClusterIndex(Int_t idx) {
        fLowerClusterIndex = idx;
    }

private:

    /** Is hit used or not **/
    Bool_t fUsing;
    /** Is hit good or not **/
    Bool_t fFlag;
    /** Index in hits array **/
    Int_t fIndex;
    /** Id of detector which creates hit **/
    DetectorId fDetId;
    /** station number. It essential for gem, mwpc**/
    Short_t fStation;
    Int_t fModule;
    Int_t fType; // 0 - fake, 1 - hit, -1 - undefined
    Double_t fLength; //length from vertex to current hit
    
    Double_t fResX;
    Double_t fResY;
    
    Double_t fCovXY;          // Covariance of x and y coordinates

    BmnMatch fDigitNumberMatch; //digit numbers used for the hit (as weighed center (one strip) of a cluster in each layer
    
    Int_t fUpperClusterIndex;
    Int_t fLowerClusterIndex;

    ClassDef(BmnHit, 1);

};
#endif	/* BMNHIT_H */

