#ifndef BMNSILICONHIT_H
#define BMNSILICONHIT_H

#include "Rtypes.h"

#include "BmnHit.h"

class BmnSiliconHit : public BmnHit {
public:

    BmnSiliconHit();
    BmnSiliconHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index);

    virtual ~BmnSiliconHit();

//    void SetModule(Int_t module) {
//        fModule = module;
//    }

    void SetEnergyLoss(Double_t de) {
        fELoss = de;
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
    
    void SetStripTotalSignalInLowerLayer(Int_t sig) {
        fLowerLayerStripTotalSignal = sig;
    }

    void SetStripTotalSignalInUpperLayer(Int_t sig) {
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

//    Int_t GetModule() {
//        return fModule;
//    }

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
   // Int_t fModule;
    Double_t fELoss;
    Int_t fLowerLayerClusterSize; //number of strips in the lower layer (|||) for the hit
    Int_t fUpperLayerClusterSize; //number of strips in the upper layer (\\\ or ///) for the hit
    Double_t fLowerLayerStripPos; //strip position in the lower layer (|||)
    Double_t fUpperLayerStripPos; //strip position in the upper layer (\\\ or ///)
    Double_t fLowerLayerStripTotalSignal;
    Double_t fUpperLayerStripTotalSignal;

    ClassDef(BmnSiliconHit, 1);
};




#endif /* BMNSILICONHIT_H */

