#ifndef BMNEVENTQUALITY_H
#define BMNEVENTQUALITY_H 1

#include <iostream>
#include <TNamed.h>

using namespace std;

class BmnEventQuality : public TNamed {
public:

    BmnEventQuality() {};
    BmnEventQuality(TString str) {
        fIsGoodEvent = (str == "GOOD") ? kTRUE : (str == "BAD") ? kFALSE : throw;
    }

    virtual ~BmnEventQuality() {
    };
    
    void SetIsGoodEvent(TString str) {
        fIsGoodEvent = (str == "GOOD") ? kTRUE : (str == "BAD") ? kFALSE : throw;
    }
    
    Bool_t GetIsGoodEvent() {
        return fIsGoodEvent;
    }

    void SetElossBC1(vector<int> eloss){
	fElossBC1=eloss;
    }

    void SetElossBC2(vector<int> eloss){
	fElossBC2=eloss;
    }

    void SetElossBC3(vector<int> eloss){
	fElossBC3=eloss;
    }

    void SetElossBC4(vector<int> eloss){
	fElossBC4=eloss;
    }
    
    void SetElossX1R(vector<int> eloss){
	fElossX1R=eloss;
    }

    void SetElossX2R(vector<int> eloss){
	fElossX2R=eloss;
    }

    void SetElossX1L(vector<int> eloss){
	fElossX1L=eloss;
    }

    void SetElossX2L(vector<int> eloss){
	fElossX2L=eloss;
    }
    
    void SetElossY1R(vector<int> eloss){
	fElossY1R=eloss;
    }

    void SetElossY2R(vector<int> eloss){
	fElossY2R=eloss;
    }

    void SetElossY1L(vector<int> eloss){
	fElossY1L=eloss;
    }

    void SetElossY2L(vector<int> eloss){
	fElossY2L=eloss;
    }



private:
    Bool_t fIsGoodEvent;
    ClassDef(BmnEventQuality, 1);
    vector<int> fElossBC1;
    vector<int> fElossBC2;
    vector<int> fElossBC3;
    vector<int> fElossBC4;
    vector<int> fElossX1R;
    vector<int> fElossX2R;
    vector<int> fElossX1L;
    vector<int> fElossX2L;
    vector<int> fElossY1R;
    vector<int> fElossY2R;
    vector<int> fElossY1L;
    vector<int> fElossY2L;
};

#endif
