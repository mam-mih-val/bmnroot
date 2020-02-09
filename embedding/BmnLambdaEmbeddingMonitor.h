#include <TNamed.h>
#include <TVector3.h>

#ifndef BMNMONITOR_H
#define BMNMONITOR_H 1

class BmnLambdaEmbeddingMonitor : public TNamed {
public:
    BmnLambdaEmbeddingMonitor();
    ~BmnLambdaEmbeddingMonitor();

    void SetId(UInt_t evId) {
        id = evId;
    }

    void IsEmbedded(Bool_t flag) {
        isEmbedded = flag;
    }

    void SetVertex(TVector3 vtx) {
        fVx = vtx.X();
        fVy = vtx.Y();
        fVz = vtx.Z();
    }

    void SetStoreVertexEvent(TVector3 info) {
        store = info.X();
        vertex = info.Y();
        event = info.Z();
    }

    void SetNHitsProton(Int_t nhits) {
        nHitsProton = nhits;
    }
    
    void SetNHitsPion(Int_t nhits) {
        nHitsPion = nhits;
    }
    
    UInt_t GetEventId() {
        return id;
    } 
    
    TVector3 GetStoreVertexEvent() {
        return TVector3(store, vertex, event);
    }

private:
    UInt_t id;
    Bool_t isEmbedded;

    Double_t fVx;
    Double_t fVy;
    Double_t fVz;

    Int_t store;
    Int_t vertex;
    Int_t event;

    Int_t nHitsProton;
    Int_t nHitsPion;

    ClassDef(BmnLambdaEmbeddingMonitor, 1)
};

#endif