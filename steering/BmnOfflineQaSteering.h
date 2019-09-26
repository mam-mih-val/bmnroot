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

    pair <Int_t, Int_t> GetBorderRuns(Int_t period, TString setup = "BM@N") {
        if (period == 6 && setup == "SRC")
            return make_pair(-1, -1);
        return fBorderRuns.find(make_pair(period, setup))->second;
    }

    vector <TString> GetDetectors(Int_t period, TString setup = "BM@N") {
        return fDetectors.find(make_pair(period, setup))->second;
    }

    vector <TString> GetTriggers(Int_t period, TString setup = "BM@N") {
        return fTriggers.find(make_pair(period, setup))->second;
    }

    pair <Int_t, TString> GetRunAndSetupByRunId(Int_t id) {
        const Int_t nPeriods = 2;
        const Int_t nSetups = 2;

        Int_t runPeriods[nPeriods] = {6, 7};
        TString setups[nSetups] = {"BM@N", "SRC"};

        for (Int_t iPeriod = 0; iPeriod < nPeriods; iPeriod++)
            for (Int_t iSetup = 0; iSetup < nSetups; iSetup++)
                if (id >= GetBorderRuns(runPeriods[iPeriod], setups[iSetup]).first && id <= GetBorderRuns(runPeriods[iPeriod], setups[iSetup]).second)
                    return make_pair(runPeriods[iPeriod], setups[iSetup]);

        return make_pair(-1, "");
    }

    Int_t GetNumberOfDets(Int_t period, TString setup, TString type) {
        // Possible det. types are: coordinate, time, calorimeter
        if (type != "coordinate" && type != "time" && type != "calorimeter")
            return -1;
        Int_t detCounter = 0;
        for (Int_t iDet = 0; iDet < GetDetectors(period, setup).size(); iDet++) {
            TString det = GetDetectors(period, setup)[iDet];

            if (type == "coordinate") {
                if (det.Contains("GEM") || det.Contains("SILICON") || det.Contains("CSC"))
                    detCounter++;
            } else if (type == "time") {
                if (det.Contains("TOF400") || det.Contains("TOF700") || det.Contains("MWPC") || det.Contains("DCH"))
                    detCounter++;
            }
            else if (type == "calorimeter") {
                if (det.Contains("ECAL") || det.Contains("ZDC"))
                    detCounter++;
            }
        }
        return detCounter;
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

    map <pair <Int_t, TString>, pair <Int_t, Int_t>> fBorderRuns; // (period, setup) --> (start, finish)
    map <pair <Int_t, TString>, vector <TString>> fDetectors; // (period, setup) --> list of detectors
    map <pair <Int_t, TString>, vector <TString>> fTriggers; // (period, setup) --> list of triggers

    ClassDef(BmnOfflineQaSteering, 1);
};

#endif