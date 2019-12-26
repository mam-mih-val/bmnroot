#include "BmnOfflineQaSteering.h"

BmnOfflineQaSteering::BmnOfflineQaSteering() {
    ParseSteerFile();
}

BmnOfflineQaSteering::~BmnOfflineQaSteering() {

}

vector<Double_t> BmnOfflineQaSteering::GetAxisAttributes(TString histoName) {
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathFull = gPathConfig + "/macro/steering/qaHisto.dat";

    string line;
    ifstream f(gPathFull.Data(), ios::in);
    
    vector <Double_t> axisAttr;

    while (!f.eof()) {
        getline(f, line);

        TString currString(line);

        if (currString.Contains(histoName.Data())) {
            TObjArray* arr = currString.Tokenize(" ");
            for (Int_t iEle = 0; iEle < arr->GetEntriesFast(); iEle++) {
                TString str = ((TObjString*) arr->UncheckedAt(iEle))->GetString();
                if (str.Contains("nBins") || str.Contains("nBinsX") || str.Contains("nBinsY") ||
                        str.Contains("xLow") || str.Contains("xUp") || str.Contains("yLow") || str.Contains("yUp")) {
                    TString tmp = ((TObjString*) arr->UncheckedAt(iEle + 1))->GetString();
                    axisAttr.push_back(tmp.Atof());
                }
            }
            break;
        }
    }
    return axisAttr;
}

void BmnOfflineQaSteering::ParseSteerFile(TString fileName) {
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathFull = gPathConfig + "/macro/steering/" + fileName;

    TString buff = "";
    string line;
    ifstream f(gPathFull.Data(), ios::in);

    getline(f, line); // Line with comments
    getline(f, line); // get number of canvases to be registered ...
    f >> buff >> fNCanvases;

    for (Int_t iCanvas = 0; iCanvas < fNCanvases; iCanvas++) {
        TString setup, name;
        Int_t run, col1d, col2d, rows1d, rows2d;
        const Int_t nSetups = 3;

        getline(f, line);
        for (Int_t iSetup = 0; iSetup < nSetups; iSetup++) {
            if (iSetup == 0) {
                f >> name >> run >> setup >> rows1d >> col1d >> rows2d >> col2d;
                fListCanvases.push_back(name);
            } else
                f >> run >> setup >> rows1d >> col1d >> rows2d >> col2d;
            vector <TString> vec = {
                TString::Format("%d", run),
                TString::Format("%s", setup.Data()),
                TString::Format("%s", name.Data()),
                TString::Format("%d", rows1d),
                TString::Format("%d", col1d),
                TString::Format("%d", rows2d),
                TString::Format("%d", col2d)
            };

            fCanvasAttributes.push_back(vec);
        }
    }

    // Get period, exp. setup, start and finish run id, set of detectors and triggers ...
    for (Int_t iLine = 0; iLine < 3; iLine++) {
        getline(f, line);
        Int_t period, start, finish, Ndets, Ntrigs;
        TString setup, detector, trigger;
        f >> period >> setup >> start >> finish >> buff >> Ndets;
        fBorderRuns[make_pair(period, setup)] = make_pair(start, finish);

        vector <TString> detectors;

        for (Int_t iDet = 0; iDet < Ndets; iDet++) {
            f >> detector;
            detectors.push_back(detector);
        }
        fDetectors[make_pair(period, setup)] = detectors;

        vector <TString> triggers;
        f >> buff >> Ntrigs;
        for (Int_t iTrigger = 0; iTrigger < Ntrigs; iTrigger++) {
            f >> trigger;
            triggers.push_back(trigger);
        }
        fTriggers[make_pair(period, setup)] = triggers;
    }

    // Get releases to be published ...
    getline(f, line);
    f >> buff >> fNReleases;

    getline(f, line);
    for (Int_t iRelease = 0; iRelease < fNReleases; iRelease++) {
        TString releaseName;
        if (iRelease == 0)
            f >> buff >> releaseName;
        else
            f >> releaseName;
        fListReleases.push_back(releaseName);
    }
}

