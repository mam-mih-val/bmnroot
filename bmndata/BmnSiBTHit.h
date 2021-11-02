#ifndef BMNSiBTHIT_H
#define BMNSiBTHIT_H

#include "Rtypes.h"

#include "BmnHit.h"

class BmnSiBTHit : public BmnHit {
public:

    BmnSiBTHit();
    BmnSiBTHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index);

    virtual ~BmnSiBTHit();

    void SetEnergyLoss(Double_t de) {
        fELoss = de;
    }

    void SetSignalDiff(Double_t sdiff) {
        fNormSignalDiff = sdiff;
    }

    void SetClusterSizeInLowerLayer(Int_t csize) {
        fLowerLayerClusterSize = csize;
    }

    void SetClusterSizeInUpperLayer(Int_t csize) {
        fUpperLayerClusterSize = csize;
    }

    void SetStripPositionInLowerLayer(Double_t spos) {
        fLowerLayerStripPos = spos;
    }

    void SetStripPositionInUpperLayer(Double_t spos) {
        fUpperLayerStripPos = spos;
    }

    void SetStripTotalSignalInLowerLayer(Double_t sig) {
        fLowerLayerStripTotalSignal = sig;
    }

    void SetStripTotalSignalInUpperLayer(Double_t sig) {
        fUpperLayerStripTotalSignal = sig;
    }

    Double_t GetStripTotalSignalInLowerLayer() {
        return fLowerLayerStripTotalSignal;
    }

    Double_t GetStripTotalSignalInUpperLayer() {
        return fUpperLayerStripTotalSignal;
    }

    Double_t GetEnergyLoss() {
        return fELoss;
    }

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
    Double_t fELoss;
    Double_t fNormSignalDiff; //normalized signal difference between lower and upper strips (0 is min diff,..., 1 is max dif)
    Int_t fLowerLayerClusterSize; //number of strips in the lower layer (|||) for the hit
    Int_t fUpperLayerClusterSize; //number of strips in the upper layer (\\\ or ///) for the hit
    Double_t fLowerLayerStripPos; //strip position in the lower layer (|||)
    Double_t fUpperLayerStripPos; //strip position in the upper layer (\\\ or ///)
    Double_t fLowerLayerStripTotalSignal;
    Double_t fUpperLayerStripTotalSignal;

    ClassDef(BmnSiBTHit, 1);
};

#endif


