#ifndef DIGIARRAYS_H
#define DIGIARRAYS_H 1

#include <vector>
#include "TClonesArray.h"
#include "BmnEventHeader.h"

class DigiArrays : public TObject {
public:

    DigiArrays() :
    gem(nullptr),
    csc(nullptr),
    tof400(nullptr),
    tof700(nullptr),
    zdc(nullptr),
    scwall(nullptr),
    fhcal(nullptr),
    ecal(nullptr),
    land(nullptr),
    dch(nullptr),
    mwpc(nullptr),
    silicon(nullptr),
    header(nullptr),

    trigAr(nullptr),
    trigSrcAr(nullptr) {
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
        if (scwall) {
            scwall->Delete();
            delete scwall;
        }
        if (fhcal) {
            fhcal->Delete();
            delete fhcal;
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
//        if (trigSrcAr) {
//            for (TClonesArray *ar : (*trigSrcAr))
//                if (ar) {
//                    ar->Clear("C");
//                    delete ar;
//                }
//            delete trigSrcAr;
//        }
    };
    TClonesArray *silicon;
    TClonesArray *gem;
    TClonesArray *csc;
    TClonesArray *tof400;
    TClonesArray *tof700;
    TClonesArray *zdc;
    TClonesArray *scwall;
    TClonesArray *fhcal;
    TClonesArray *ecal;
    TClonesArray *land;
    TClonesArray *dch;
    TClonesArray *mwpc;
    std::vector<TClonesArray*> *trigAr;
    std::vector<TClonesArray*> *trigSrcAr;

    BmnEventHeader *header; //->
private:
    ClassDef(DigiArrays, 1)
};

ClassImp(DigiArrays)

#endif /* DIGIARRAYS_H */

