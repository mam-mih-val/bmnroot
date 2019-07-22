#include "BmnDstQa.h"

BmnDstQa::BmnDstQa() :
fHistoManager(new BmnQaHistoManager()) {

    GlobalTracksDistributions();
    InnerTracksDistributions();
}

void BmnDstQa::GlobalTracksDistributions() {
    fHistoManager->Create1 <TH1F> (Form("DST_1d, Distribution of total multiplicity"),
            Form("DST, Distribution of total multiplicity"), 100, 0., 0.);
    fHistoManager->Create1 <TH1F> (Form("DST_1d, Distribution of momenta"),
            Form("DST, Distribution of momenta"), 200, -20., +20.);
    fHistoManager->Create1 <TH1F> (Form("DST_1d, Distribution of Nhits"),
            Form("DST, Distribution of Nhits"), 100, 0., 0.);
    
    fHistoManager->Create2 <TH2F> (Form("DST_2d, Vp_{z} vs. Ntracks"), Form("DST_2d, Vp_{z} vs. Ntracks"), 20, 0, 20, 200, -10., +10.);

    const Int_t nDims = 3;
    TString dim[nDims] = {"X", "Y", "Z"};
    for (Int_t iDim = 0; iDim < nDims; iDim++) {
        fHistoManager->Create1 <TH1F> (Form("DST_1d, Distribution of start%s", dim[iDim].Data()), Form("DST, Distribution of start%s", dim[iDim].Data()), 100, 0., 0.);
        if (!dim[iDim].Contains("Z"))
            fHistoManager->Create1 <TH1F> (Form("DST_1d, Distribution of start T%s", dim[iDim].Data()), Form("DST, Distribution of start T%s", dim[iDim].Data()), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("DST_1d, Distribution of last%s", dim[iDim].Data()), Form("DST, Distribution of last%s", dim[iDim].Data()), 100, 0., 0.);
        if (!dim[iDim].Contains("Z"))
            fHistoManager->Create1 <TH1F> (Form("DST_1d, Distribution of last T%s", dim[iDim].Data()), Form("DST, Distribution of last T%s", dim[iDim].Data()), 100, 0., 0.);
    }
}

void BmnDstQa::InnerTracksDistributions() {
    const Int_t nDets = 2;
    TString detNames[nDets] = {"gem", "silicon"};
    for (Int_t iDet = 0; iDet < nDets; iDet++)
        fHistoManager->Create1 <TH1F> (Form("DST_1d, Distribution of Nhits, %s track", detNames[iDet].Data()), Form("DST, Distribution of Nhits, %s track", detNames[iDet].Data()), 100, 0., 0.);
}



