#include <TNamed.h>
#include <TH1F.h>
#include <TSystem.h>
#include "BmnQaHistoManager.h"

#include <BmnGemStripStationSet.h>
#include <BmnSiliconStationSet.h>
#include <BmnCSCStationSet.h>

#ifndef BMNCOORDDETQA_H
#define BMNCOORDDETQA_H 1

using namespace std;

class BmnCoordinateDetQa : public TNamed {
public:

    /** Default constructor **/
    BmnCoordinateDetQa() {
    };
    BmnCoordinateDetQa(TString, UInt_t);

    /** Destructor **/
    virtual ~BmnCoordinateDetQa() {
    };

    // Setters

    // Getters
    BmnQaHistoManager* GetManager() {
        return fHistoManager;
    }

private:
    BmnQaHistoManager* fHistoManager;
    BmnGemStripStationSet* fDetGem;
    BmnCSCStationSet* fDetCsc;
    BmnSiliconStationSet* fDetSilicon;

    template <class T> void DistributionOfFiredStrips(void* detGeo, TString detName) {
        T* geo = (T*) detGeo;
        for (Int_t iStat = 0; iStat < geo->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < geo->GetStation(iStat)->GetNModules(); iMod++)
                for (Int_t iLay = 0; iLay < geo->GetStation(iStat)->GetModule(iMod)->GetNStripLayers(); iLay++)
                    fHistoManager->Create1 <TH1F> (Form("%s, Distribution of fired strips, Stat %d Mod %d Lay %d", detName.Data(), iStat, iMod, iLay),
                        Form("Distribution of fired strips, Stat %d Mod %d Lay %d", iStat, iMod, iLay), 100, 0., 0.);
    }

    ClassDef(BmnCoordinateDetQa, 1);

};

#endif