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

#include "TTree.h"
#include "TClonesArray.h"

class DigiArrays : public TObject {
public:

    DigiArrays() {
        gem = NULL;
        tof400 = NULL;
        tof700 = NULL;
        zdc = NULL;
        ecal = NULL;
        dch = NULL;
        mwpc = NULL;
        silicon = NULL;
        trigger = NULL;
        t0 = NULL;
        bc1 = NULL;
        bc2 = NULL;
        veto = NULL;
        fd = NULL;
        bd = NULL;
        header = NULL;
    };

    ~DigiArrays() {
    };

    void Clear() {
        if (bc1) {bc1->Delete();delete bc1;}
        if (bc2) { bc2->Delete(); delete bc2;}
        if (bd) { bd->Delete(); delete bd;}
        if (dch) { dch->Delete(); delete dch;}
        if (fd) { fd->Delete(); delete fd;}
        if (gem) { gem->Delete(); delete gem;}
        if (header) { header->Delete(); delete header;}
        if (mwpc) { mwpc->Delete(); delete mwpc;}
        if (silicon) { silicon->Delete(); delete silicon;}
        if (trigger) { trigger->Delete(); delete trigger;}
        if (t0) { t0->Delete(); delete t0;}
        if (tof400) { tof400->Delete(); delete tof400;}
        if (tof700) { tof700->Delete(); delete tof700;}
        if (zdc) { zdc->Delete(); delete zdc;}
        if (ecal) { ecal->Delete(); delete ecal;}
        if (veto) { veto->Delete(); delete veto;}
    };
    TClonesArray *silicon; 
    TClonesArray *gem; 
    TClonesArray *tof400; 
    TClonesArray *tof700; 
    TClonesArray *zdc; 
    TClonesArray *ecal; 
    TClonesArray *dch; 
    TClonesArray *mwpc; 
    TClonesArray *trigger; 
    TClonesArray *t0; 
    TClonesArray *bc1; 
    TClonesArray *bc2; 
    TClonesArray *veto; 
    TClonesArray *fd; 
    TClonesArray *bd; 
    TClonesArray *header; 
private:
    ClassDef(DigiArrays, 1)
};

ClassImp(DigiArrays)

#endif /* DIGIARRAYS_H */

