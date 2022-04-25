#ifndef BMNEMBMONITOR_H
#define BMNEMBMONITOR_H 1

#include <TNamed.h>
#include <TVector3.h>

class BmnLambdaEmbeddingMonitor : public TNamed {
public:
    BmnLambdaEmbeddingMonitor();
    virtual ~BmnLambdaEmbeddingMonitor();

    void SetId(UInt_t evId) {
        id = evId;
    }

    void IsEmbedded(Bool_t flag) {
        isEmbedded = flag;
    }
    
    Bool_t IsEmbedded() {
        return isEmbedded;
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
    
    void SetTxProton(Double_t val) {
        txProton = val;
    }
    
    void SetTyProton(Double_t val) {
        tyProton = val;
    }
    
    void SetTxPion(Double_t val) {
        txPion = val;
    }
    
    void SetTyPion(Double_t val) {
        tyPion = val;
    }
    
    void SetProtonP(Double_t val) {
        pProton = val;
    }
    
    void SetPionP(Double_t val) {
        pPion = val;  
    }
    
    UInt_t GetEventId() {
        return id;
    } 
    
    Int_t GetNHitsProton() {
        return nHitsProton;
    }
    
    Int_t GetNHitsPion() {
        return nHitsPion;
    }
    
    Double_t GetProtonP() {
        return pProton;
    }
    
    Double_t GetPionP() {
        return pPion;
    }
    
    TVector3 GetStoreVertexEvent() {
        return TVector3(store, vertex, event);
    }

private:
    UInt_t id;
    Bool_t isEmbedded;

    Int_t store;
    Int_t vertex;
    Int_t event;

    Int_t nHitsProton;
    Int_t nHitsPion;
    
    Double_t pProton;
    Double_t pPion;
    
    Double_t txProton;
    Double_t txPion;
    
    Double_t tyProton;
    Double_t tyPion;

    ClassDef(BmnLambdaEmbeddingMonitor, 1)
};

#endif
