#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <TNamed.h>
#include <TMath.h>
#include <TVector3.h>
#include <TSystem.h>

#ifndef BMNOFFLINEQASTEERING_H
#define BMNOFFLINEQASTEERING_H 1

using namespace std;
using namespace TMath;

class BmnOfflineQaSteering : public TNamed {
public:

    BmnOfflineQaSteering();
    // BmnOfflineQaSteering(TString) {};

    virtual ~BmnOfflineQaSteering();

    Int_t*** GetDetCanvas() {
        return fDetCanvas;
    }
    
    vector <TString> GetCanvNames() {
        return fCanvNames;
    }
    
    map <TString, pair <Int_t, Int_t>> GetCorrMap() {
        return fCanvDetCorresp;
    }

private:
    void ParseSteerFile(TString f = "qaOffline_run7.dat");

    // TRIGGERS -- [0][1d -- 2d][columns -- rows]
    // GEM       [1][][]
    // SILICON   [2][][]
    // CSC       [3][][]
    // TOF400    [4][][]
    // TOF700    [5][][]
    // DCH       [6][][]
    // MWPC      [7][][]
    // ECAL      [8][][]
    // ZDC       [9][][]
    // DST       [10][][]

    Int_t fNdets;
    Int_t*** fDetCanvas;
        
    vector <TString> fCanvNames;
    
    map <TString, pair <Int_t, Int_t>> fCanvDetCorresp;

    ClassDef(BmnOfflineQaSteering, 1);
};

#endif