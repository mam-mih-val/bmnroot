#ifndef BMNGEMSTRIPHIT_H
#define	BMNGEMSTRIPHIT_H

#include "Rtypes.h"

#include "BmnHit.h"

class BmnGemStripHit : public BmnHit {
public:

    BmnGemStripHit();
    BmnGemStripHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index);

    virtual ~BmnGemStripHit();

    void SetModule(Int_t module) {
        fModule = module;
    }

    void SetEnergyLoss(Double_t de) {
        fELoss = de;
    }
//
//    void SetType(Int_t type) {
//        fType = type;
//    }

    void SetSignalDiff(Double_t sdiff) {
        fNormSignalDiff = sdiff;
    }

    void SetClusterSizeInLowerLayer(Int_t csize) {
        fLowerLayerClusterSize = csize;
    }

    void SetClusterSizeInUpperLayer(Int_t csize) {
        fUpperLayerClusterSize = csize;
    }

    void SetStripPositionInLowerLayer(Int_t spos) {
        fLowerLayerStripPos = spos;
    }

    void SetStripPositionInUpperLayer(Int_t spos) {
        fUpperLayerStripPos = spos;
    }

    Double_t GetEnergyLoss() {
        return fELoss;
    }

    Int_t GetModule() {
        return fModule;
    }

//    Int_t GetType() {
//        return fType;
//    }

    Double_t GetSignalDiff() {
        return fNormSignalDiff;
    }

    Int_t GetClusterSizeInLowerLayer() {
        return fLowerLayerClusterSize;
    }

    Int_t GetClusterSizeInUpperLayer() {
        return fUpperLayerClusterSize;
    }

    Double_t GetStripPositionInLowerLayer() {
        return fLowerLayerStripPos;
    }

    Double_t GetStripPositionInUpperLayer() {
        return fUpperLayerStripPos;
    }

private:
    Int_t fModule;
    Double_t fELoss;
//    Int_t fType; // 0 - fake, 1 - hit, -1 - undefined
    Double_t fNormSignalDiff; //normalized signal difference between lower and upper strips (0 is min diff,..., 1 is max dif)
    Int_t fLowerLayerClusterSize; //number of strips in the lower layer (|||) for the hit
    Int_t fUpperLayerClusterSize; //number of strips in the upper layer (\\\ or ///) for the hit
    Double_t fLowerLayerStripPos; //strip position in the lower layer (|||)
    Double_t fUpperLayerStripPos; //strip position in the upper layer (\\\ or ///)

    
    ClassDef(BmnGemStripHit, 1);
};



#endif


