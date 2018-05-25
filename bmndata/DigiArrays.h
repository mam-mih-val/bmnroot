/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DigiArrays.h
 * Author: ilnur
 *
 * Created on April 20, 2017, 4:12 PM
 */

#ifndef DIGIARRAYS_H
#define DIGIARRAYS_H 1

#include <vector>
#include "TTree.h"
#include "TClonesArray.h"

class DigiArrays : public TObject {
public:

    DigiArrays() {
        gem = NULL;
        csc = NULL;
        tof400 = NULL;
        tof700 = NULL;
        zdc = NULL;
        ecal = NULL;
        land = NULL;
        dch = NULL;
        mwpc = NULL;
        silicon = NULL;
        header = NULL;
        trigAr = NULL;
        trigSrcAr = NULL;
    };

    ~DigiArrays() {
    };

    void Clear() {
        if (dch) {
            dch->Delete();
            delete dch;
        }
        if (gem) {
            gem->Delete();
            delete gem;
        }
        if (csc) {
            csc->Delete();
            delete csc;
        }
        if (header) {
            header->Delete();
            delete header;
        }
        if (mwpc) {
            mwpc->Delete();
            delete mwpc;
        }
        if (silicon) {
            silicon->Delete();
            delete silicon;
        }
        if (tof400) {
            tof400->Delete();
            delete tof400;
        }
        if (tof700) {
            tof700->Delete();
            delete tof700;
        }
        if (zdc) {
            zdc->Delete();
            delete zdc;
        }
        if (ecal) {
            ecal->Delete();
            delete ecal;
        }
        if (land) {
            land->Delete();
            delete land;
        }
        if (trigAr) {
            for (TClonesArray *ar : (*trigAr))
                if (ar) {
                    ar->Clear("C");
                    delete ar;
                }
            delete trigAr;
        }
        if (trigSrcAr) {
            for (TClonesArray *ar : (*trigSrcAr))
                if (ar) {
                    ar->Clear("C");
                    delete ar;
                }
            delete trigSrcAr;
        }
    };
    TClonesArray *silicon;
    TClonesArray *gem;
    TClonesArray *csc;
    TClonesArray *tof400;
    TClonesArray *tof700;
    TClonesArray *zdc;
    TClonesArray *ecal;
    TClonesArray *land;
    TClonesArray *dch;
    TClonesArray *mwpc;
    std::vector<TClonesArray*> *trigAr;
    std::vector<TClonesArray*> *trigSrcAr;
    TClonesArray *header; //->
private:
    ClassDef(DigiArrays, 1)
};

ClassImp(DigiArrays)

#endif /* DIGIARRAYS_H */

