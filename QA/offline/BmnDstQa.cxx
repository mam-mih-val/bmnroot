#include "BmnDstQa.h"

BmnDstQa::BmnDstQa() :
fHistoManager(new BmnQaHistoManager()) {

    GlobalTracksDistributions();
    InnerTracksDistributions();
}

void BmnDstQa::GlobalTracksDistributions() {
    fHistoManager->Create1 <TH1F> (Form("Distribution of total multiplicity"),
            Form("Distribution of total multiplicity"), 100, 0., 0.);
    fHistoManager->Create1 <TH1F> (Form("Distribution of momenta"),
            Form("Distribution of momenta"), 200, -20., +20.);
    fHistoManager->Create1 <TH1F> (Form("Distribution of Nhits"),
            Form("Distribution of Nhits"), 100, 0., 0.);

    const Int_t nDims = 3;
    TString dim[nDims] = {"X", "Y", "Z"};
    for (Int_t iDim = 0; iDim < nDims; iDim++) {
        fHistoManager->Create1 <TH1F> (Form("Distribution of start%s", dim[iDim].Data()), Form("Distribution of start%s", dim[iDim].Data()), 100, 0., 0.);
        if (!dim[iDim].Contains("Z"))
            fHistoManager->Create1 <TH1F> (Form("Distribution of start T%s", dim[iDim].Data()), Form("Distribution of start T%s", dim[iDim].Data()), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("Distribution of last%s", dim[iDim].Data()), Form("Distribution of last%s", dim[iDim].Data()), 100, 0., 0.);
        if (!dim[iDim].Contains("Z"))
            fHistoManager->Create1 <TH1F> (Form("Distribution of last T%s", dim[iDim].Data()), Form("Distribution of last T%s", dim[iDim].Data()), 100, 0., 0.);
    }
}

void BmnDstQa::InnerTracksDistributions() {
    const Int_t nDets = 2;
    TString detNames[nDets] = {"gem", "silicon"};
    for (Int_t iDet = 0; iDet < nDets; iDet++)
        fHistoManager->Create1 <TH1F> (Form("Distribution of Nhits, %s track", detNames[iDet].Data()), Form("Distribution of Nhits, %s track", detNames[iDet].Data()), 100, 0., 0.);
}



