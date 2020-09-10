#include <TNamed.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TSystem.h>
#include "BmnQaHistoManager.h"

#include <BmnGemStripStationSet.h>
#include <BmnSiliconStationSet.h>
#include <BmnCSCStationSet.h>

#include <BmnOfflineQaSteering.h>

#ifndef BMNCOORDDETQA_H
#define BMNCOORDDETQA_H 1

using namespace std;

class BmnCoordinateDetQa : public TNamed {
public:

    /** Default constructor **/
    BmnCoordinateDetQa() {
    };
    BmnCoordinateDetQa(TString, UInt_t runId);

    /** Destructor **/
    virtual ~BmnCoordinateDetQa() {
        delete fSteering;
    };

    // Setters

    // Getters

    BmnQaHistoManager* GetManager() {
        return fHistoManager;
    }

private:
    BmnOfflineQaSteering* fSteering;

    BmnQaHistoManager* fHistoManager;
    BmnGemStripStationSet* fDetGem;
    BmnCSCStationSet* fDetCsc;
    BmnSiliconStationSet* fDetSilicon;

    // Histos 1

    template <class T> void DistributionOfFiredStrips(void* detGeo, TString detName) {
        T* geo = (T*) detGeo;
        for (Int_t iStat = 0; iStat < geo->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < geo->GetStation(iStat)->GetNModules(); iMod++)
                for (Int_t iLay = 0; iLay < geo->GetStation(iStat)->GetModule(iMod)->GetNStripLayers(); iLay++)
                    fHistoManager->Create1 <TH1F> (Form("%s_1d, Distribution of fired strips, Stat %d Mod %d Lay %d", detName.Data(), iStat, iMod, iLay),
                        Form("%s, Distribution of fired strips, Stat %d Mod %d Lay %d", detName.Data(), iStat, iMod, iLay),
                        fSteering->GetAxisAttributes(Form("%s_1d, Distribution of fired strips, Stat %d Mod %d Lay %d", detName.Data(), iStat, iMod, iLay))[0],
                        fSteering->GetAxisAttributes(Form("%s_1d, Distribution of fired strips, Stat %d Mod %d Lay %d", detName.Data(), iStat, iMod, iLay))[1],
                        fSteering->GetAxisAttributes(Form("%s_1d, Distribution of fired strips, Stat %d Mod %d Lay %d", detName.Data(), iStat, iMod, iLay))[2]);
    }

    // Histos 2

    template <class T> void DistributionOfFiredStripsVsSignal(void* detGeo, TString detName) {
        T* geo = (T*) detGeo;
        for (Int_t iStat = 0; iStat < geo->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < geo->GetStation(iStat)->GetNModules(); iMod++)
                for (Int_t iLay = 0; iLay < geo->GetStation(iStat)->GetModule(iMod)->GetNStripLayers(); iLay++)
                    fHistoManager->Create2 <TH2F> (Form("%s_2d, Distribution of fired strips vs. signal, Stat %d Mod %d Lay %d", detName.Data(), iStat, iMod, iLay),
                        Form("%s, Distribution of fired strips vs. signal, Stat %d Mod %d Lay %d", detName.Data(), iStat, iMod, iLay),
                        fSteering->GetAxisAttributes(Form("%s_2d, Distribution of fired strips vs. signal, Stat %d Mod %d Lay %d", detName.Data(), iStat, iMod, iLay))[0],
                        fSteering->GetAxisAttributes(Form("%s_2d, Distribution of fired strips vs. signal, Stat %d Mod %d Lay %d", detName.Data(), iStat, iMod, iLay))[1],
                        fSteering->GetAxisAttributes(Form("%s_2d, Distribution of fired strips vs. signal, Stat %d Mod %d Lay %d", detName.Data(), iStat, iMod, iLay))[2],
                        fSteering->GetAxisAttributes(Form("%s_2d, Distribution of fired strips vs. signal, Stat %d Mod %d Lay %d", detName.Data(), iStat, iMod, iLay))[3],
                        fSteering->GetAxisAttributes(Form("%s_2d, Distribution of fired strips vs. signal, Stat %d Mod %d Lay %d", detName.Data(), iStat, iMod, iLay))[4],
                        fSteering->GetAxisAttributes(Form("%s_2d, Distribution of fired strips vs. signal, Stat %d Mod %d Lay %d", detName.Data(), iStat, iMod, iLay))[5]);
    }

    ClassDef(BmnCoordinateDetQa, 1);

};

#endif