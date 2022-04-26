#ifndef BMNPARTS_H
#define BMNPARTS_H 1

#include <vector>

#include <TClonesArray.h>

#include <DigiRunHeader.h>
#include <DstRunHeader.h>

class BmnParts : public TObject {
public:

    BmnParts() :
    fDstRunHeader(nullptr),
    fDigRunHeader(nullptr) {
    };

    ~BmnParts() {
        DeleteTCAs();
        if (fDigRunHeader)
            delete fDigRunHeader;
        if (fDstRunHeader)
            delete fDstRunHeader;
    };

    void Clear() {
        for (TClonesArray * ar : fArrays)
        if (ar) {
            ar->Delete();
        }
    };
    void Reset(){
        Clear();
//        if (fDigEvHeader)
//            delete fDigEvHeader;
//        if (fDstEvHeader)
//            delete fDstEvHeader;
    }
    DigiRunHeader * GetDigiEventHeader(){ return fDigRunHeader;}
    void SetDigiEventHeader(DigiRunHeader * v) { fDigRunHeader = v;}
    DstRunHeader * GetDstEventHeader(){ return fDstRunHeader;}
    void SetDstEventHeader(DstRunHeader * v) { fDstRunHeader = v;}
    vector <TNamed *> & GetObjects(){ return fObjects;}
    vector <TClonesArray *> & GetArrays(){ return fArrays;}
    void AddArray(TClonesArray * ar){
        fArrays.push_back(ar);
    }
    void AddObject(TNamed * ob){
        fObjects.push_back(ob);
    }
    
    UInt_t GetEventId() { return fEventId; }
    UInt_t GetPeriodId() { return fPeriodId; }
    void SetEventId(UInt_t event_id) { fEventId = event_id; }
    void SetPeriodId(UInt_t period_id) { fPeriodId = period_id; }
    


private:
    void DeleteTCAs(){
        for (TClonesArray * ar : fArrays)
        if (ar) {
            ar->Delete();
            delete ar;
        }
        for (TNamed * ob : fObjects)
        if (ob) {
            delete ob;
        }
    }
    DigiRunHeader * fDigRunHeader;
    DstRunHeader * fDstRunHeader;
    vector <TNamed *> fObjects;
    vector <TClonesArray *> fArrays;
    
    UInt_t fEventId;
    UInt_t fPeriodId;
    
    ClassDef(BmnParts, 1)
};

ClassImp(BmnParts)

#endif /* BMNPARTS_H */

