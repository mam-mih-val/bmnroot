#include "BmnDstQa.h"

BmnDstQa::BmnDstQa(UInt_t id) :
fSteering(new BmnOfflineQaSteering()),
fHistoManager(new BmnQaHistoManager()) {
    
    pair <Int_t, TString> periodSetup = fSteering->GetRunAndSetupByRunId(id);
    TString prefix = TString::Format("RUN%d_SETUP_%s_", periodSetup.first, periodSetup.second.Data());

    GlobalTracksDistributions(prefix);
    InnerTracksDistributions(prefix);
}

void BmnDstQa::GlobalTracksDistributions(TString prefix) {
    fHistoManager->Create1 <TH1F> (Form("%sDST_1d, Distribution of total multiplicity", prefix.Data()),
            Form("DST, Distribution of total multiplicity"), 100, 0., 0.);
    fHistoManager->Create1 <TH1F> (Form("%sDST_1d, Distribution of momenta", prefix.Data()),
            Form("DST, Distribution of momenta"), 200, -20., +20.);
    fHistoManager->Create1 <TH1F> (Form("%sDST_1d, Distribution of Nhits", prefix.Data()),
            Form("DST, Distribution of Nhits"), 100, 0., 0.);
    
    fHistoManager->Create2 <TH2F> (Form("%sDST_2d, Vp_{z} vs. Ntracks", prefix.Data()), Form("DST_2d, Vp_{z} vs. Ntracks"), 20, 0, 20, 200, -10., +10.);

    const Int_t nDims = 3;
    TString dim[nDims] = {"X", "Y", "Z"};
    for (Int_t iDim = 0; iDim < nDims; iDim++) {
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, Distribution of start%s", prefix.Data(), dim[iDim].Data()), Form("DST, Distribution of start%s", dim[iDim].Data()), 100, 0., 0.);
        if (!dim[iDim].Contains("Z"))
            fHistoManager->Create1 <TH1F> (Form("%sDST_1d, Distribution of start T%s", prefix.Data(), dim[iDim].Data()), Form("DST, Distribution of start T%s", dim[iDim].Data()), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, Distribution of last%s", prefix.Data(), dim[iDim].Data()), Form("DST, Distribution of last%s", dim[iDim].Data()), 100, 0., 0.);
        if (!dim[iDim].Contains("Z"))
            fHistoManager->Create1 <TH1F> (Form("%sDST_1d, Distribution of last T%s", prefix.Data(), dim[iDim].Data()), Form("DST, Distribution of last T%s", dim[iDim].Data()), 100, 0., 0.);
    }
}

void BmnDstQa::InnerTracksDistributions(TString prefix) {
    const Int_t nDets = 2;
    TString detNames[nDets] = {"gem", "silicon"};
    for (Int_t iDet = 0; iDet < nDets; iDet++)
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, Distribution of Nhits, %s track", prefix.Data(), detNames[iDet].Data()), Form("DST, Distribution of Nhits, %s track", detNames[iDet].Data()), 100, 0., 0.);
}



