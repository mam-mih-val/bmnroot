#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <TNamed.h>
#include <TMath.h>
#include <TVector3.h>
#include <TSystem.h>
#include <TObjArray.h>
#include <TObjString.h>

#include <BmnGemStripStationSet.h>
#include <BmnSiliconStationSet.h>
#include <BmnCSCStationSet.h>

#ifndef BMNOFFLINEQASTEERING_H
#define BMNOFFLINEQASTEERING_H 1

using namespace std;
using namespace TMath;

class BmnOfflineQaSteering : public TNamed {
public:

    BmnOfflineQaSteering();
    // BmnOfflineQaSteering(TString) {};

    virtual ~BmnOfflineQaSteering();

    pair <Int_t, Int_t> GetCanvasSizes(Int_t run, TString setup, TString name, TString dim) {
        for (auto it : fCanvasAttributes) {
            if (TString(it[0]).Atoi() == run && it[1] == setup && name.Contains(TString(it[2]).Data())) {
                return (dim == "1d") ? make_pair(TString(it[3]).Atoi(), TString(it[4]).Atoi()) :
                        (dim == "2d") ? make_pair(TString(it[5]).Atoi(), TString(it[6]).Atoi()) :
                        make_pair(-1, -1);
            }
        }
        return make_pair(-1, -1);
    }

    Int_t GetNCanvases() {
        return fNCanvases;
    }
    
    Int_t GetNReleases() {
        return fNReleases;
    }
    
    vector <TString> GetListOfReleases() {
        return fListReleases;
    }

    vector <TString> GetListOfCanvases() {
        return fListCanvases;
    }

    pair <Int_t, Int_t> GetBorderRuns(Int_t period, TString setup = "BM@N") {
        if (period == 6 && setup == "SRC")
            return make_pair(-1, -1);
        return fBorderRuns.find(make_pair(period, setup))->second;
    }

    vector <TString> GetDetectors(Int_t period, TString setup = "BM@N") {
        if (period == 6 && setup == "SRC") {
            // Configuration does not exist !!!
            vector <TString> tmp;
            tmp.resize(0);
            return tmp;
        } else
            return fDetectors.find(make_pair(period, setup))->second;
    }

    vector <TString> GetTriggers(Int_t period, TString setup = "BM@N") {
        if (period == 6 && setup == "SRC") {
            // Configuration does not exist !!!
            vector <TString> tmp;
            tmp.resize(0);
            return tmp;
        } else
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
            } else if (type == "calorimeter") {
                if (det.Contains("ECAL") || det.Contains("ZDC"))
                    detCounter++;
            }
        }
        return detCounter;
    }

    void SetGeometriesByRunId(Int_t id, BmnGemStripStationSet*& gem, BmnSiliconStationSet*& silicon, BmnCSCStationSet*& csc) {
        TString gPathConfig = gSystem->Getenv("VMCWORKDIR");

        TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
        TString gPathSilConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
        TString gPathCscConfig = gPathConfig + "/parameters/csc/XMLConfigs/";

        const Int_t nPeriods = 2;
        const Int_t nSetups = 2;

        Int_t runPeriods[nPeriods] = {6, 7};
        TString setups[nSetups] = {"BM@N", "SRC"};

        TString confGEM = "";
        TString confSIL = "";
        TString confCSC = "";

        for (Int_t iPeriod = 0; iPeriod < nPeriods; iPeriod++)
            for (Int_t iSetup = 0; iSetup < nSetups; iSetup++)
                if (id >= GetBorderRuns(runPeriods[iPeriod], setups[iSetup]).first && id <= GetBorderRuns(runPeriods[iPeriod], setups[iSetup]).second) {

                    if (iPeriod == 0 && iSetup == 0) {
                        // RUN6, BM@N 

                        confGEM = "GemRunSpring2017.xml";
                        confSIL = "SiliconRunSpring2017.xml";
                    } else if (iPeriod == 1 && iSetup == 0) {
                        // RUN7, BM@N 

                        confGEM = "GemRunSpring2018.xml";
                        confSIL = "SiliconRunSpring2018.xml";
                        confCSC = "CSCRunSpring2018.xml";
                    } else if (iPeriod == 1 && iSetup == 1) {
                        // RUN7, SRC 

                        confGEM = "GemRunSRCSpring2018.xml";
                        confSIL = "SiliconRunSRCSpring2018.xml";
                        confCSC = "CSCRunSRCSpring2018.xml";
                    }

                    if (!gem)
                        gem = new BmnGemStripStationSet(gPathGemConfig + confGEM);

                    if (!silicon)
                        silicon = new BmnSiliconStationSet(gPathSilConfig + confSIL);

                    if (!confCSC.IsNull() && !csc)
                        csc = new BmnCSCStationSet(gPathCscConfig + confCSC);
                }
    }
    
    vector <Double_t> GetAxisAttributes(TString); 

private:
    void ParseSteerFile(TString f = "qaOffline.dat");

    Int_t fNCanvases;
    Int_t fNReleases; 
    
    vector <TString> fListCanvases;
    vector <TString> fListReleases;
    
    vector <vector <TString>> fCanvasAttributes;

    map <pair <Int_t, TString>, pair <Int_t, Int_t>> fBorderRuns; // (period, setup) --> (start, finish)
    map <pair <Int_t, TString>, vector <TString>> fDetectors; // (period, setup) --> list of detectors
    map <pair <Int_t, TString>, vector <TString>> fTriggers; // (period, setup) --> list of triggers
    
    ClassDef(BmnOfflineQaSteering, 1);
};

#endif