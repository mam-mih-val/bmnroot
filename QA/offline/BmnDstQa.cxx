#include "BmnDstQa.h"

BmnDstQa::BmnDstQa(UInt_t id) :
fSteering(new BmnOfflineQaSteering()),
fHistoManager(new BmnQaHistoManager()),
fDetGem(nullptr),
fDetSilicon(nullptr),
fDetCsc(nullptr) {
    fSteering->SetGeometriesByRunId(id, fDetGem, fDetSilicon, fDetCsc);

    pair <Int_t, TString> periodSetup = fSteering->GetRunAndSetupByRunId(id);
    prefix = TString::Format("RUN%d_SETUP_%s_", periodSetup.first, periodSetup.second.Data());

    Hits();
    Tracks();
    Matching();
    PID();
}

void BmnDstQa::Hits() {
    HitsDistributions();
    OccupancyHistos();
    AverageStripValuePerHit();
}

void BmnDstQa::Tracks() {
    BasicTrackDistributions();
    ResidualsPullsHistos();
    AverageStripValuePerTrack();
}

void BmnDstQa::Matching() {
    MatchingHistos();
    EfficiencyHistos();
}

void BmnDstQa::PID() {


}

void BmnDstQa::BasicTrackDistributions() {
    const Int_t nDets = 2;
    TString detNames[nDets] = {"GEM", "SILICON"};
    for (Int_t iDet = 0; iDet < nDets; iDet++)
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, Distribution of Nhits, %s track", prefix.Data(), detNames[iDet].Data()), Form("DST, Distribution of Nhits, %s track", detNames[iDet].Data()), 100, 0., 0.);

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

void BmnDstQa::HitsDistributions() {
    const Int_t nDets = 6;
    TString detNames[nDets] = {"GEM", "SILICON", "CSC", "TOF400", "TOF700", "DCH"};

    // Common distributions of X, Y and Z-coordinates ...
    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, %s, Distribution of X", prefix.Data(), detNames[iDet].Data()),
                Form("%s, Distribution of X", detNames[iDet].Data()), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, %s, Distribution of Y", prefix.Data(), detNames[iDet].Data()),
                Form("%s, Distribution of Y", detNames[iDet].Data()), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, %s, Distribution of Z", prefix.Data(), detNames[iDet].Data()),
                Form("%s, Distribution of Z", detNames[iDet].Data()), 100, 0., 0.);
    }

    // Common distributions of X and Y-errors ...  
    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, %s, Distribution of dX", prefix.Data(), detNames[iDet].Data()),
                Form("%s, Distribution of dX", detNames[iDet].Data()), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, %s, Distribution of dY", prefix.Data(), detNames[iDet].Data()),
                Form("%s, Distribution of dY", detNames[iDet].Data()), 100, 0., 0.);
    }

    // Common distributions of cluster sizes and signals ...
    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        if (iDet > 2)
            break;

        // sizes ...
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, %s, Distribution of X cluster size", prefix.Data(), detNames[iDet].Data()),
                Form("%s, Distribution of X cluster size", detNames[iDet].Data()), 16, 0., 16.);
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, %s, Distribution of X' cluster size", prefix.Data(), detNames[iDet].Data()),
                Form("%s, Distribution of X' cluster size", detNames[iDet].Data()), 16, 0., 16.);

        // signals ...
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, %s, Distribution of X cluster signals", prefix.Data(), detNames[iDet].Data()),
                Form("%s, Distribution of X cluster size", detNames[iDet].Data()), 100, 0., 0.);
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, %s, Distribution of X' cluster signals", prefix.Data(), detNames[iDet].Data()),
                Form("%s, Distribution of X' cluster size", detNames[iDet].Data()), 100, 0., 0.);
    }
}

void BmnDstQa::MatchingHistos() {
    const Int_t nBins = 20;
    const Int_t nBinsXY = 50;

    const Int_t nDets = 4;
    TString dets[nDets] = {"CSC", "DCH1", "TOF700", "DCH2"};

    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, hAllNo%s", prefix.Data(), dets[iDet].Data()),
                Form("%s, matchingMomWithout; Rigidity [GeV/(Qc)]; N", dets[iDet].Data()), nBins, 0., 15.);
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, hAllYes%s", prefix.Data(), dets[iDet].Data()),
                Form("%s, matchingMomWith; Rigidity [GeV/(Qc)]; N", dets[iDet].Data()), nBins, 0., 15.);

        fHistoManager->Create2 <TH2F> (Form("%sDST_2d, hAllNo%sXY", prefix.Data(), dets[iDet].Data()),
                Form("%s, matchingEffWithout; X [cm]; Y [cm]", dets[iDet].Data()), nBinsXY, -20, 100, nBinsXY, -60, 60);
        fHistoManager->Create2 <TH2F> (Form("%sDST_2d, hAllYes%sXY", prefix.Data(), dets[iDet].Data()),
                Form("%s, matchingEffWith; X [cm]; Y [cm]", dets[iDet].Data()), nBinsXY, -20, 100, nBinsXY, -60, 60);
    }
}

void BmnDstQa::EfficiencyHistos() {
    const Int_t nBins = 20;
    const Int_t nBinsXY = 50;
    const Int_t nDets = 4;
    TString dets[nDets] = {"CSC", "DCH1", "TOF700", "DCH2"};

    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, hEff%s", prefix.Data(), dets[iDet].Data()),
                Form("%s, Efficiency of matching; Rigidity [GeV/(Qc)]; Efficiency", dets[iDet].Data()), nBins, 0., 15.);

        fHistoManager->Create2 <TH2F> (Form("%sDST_2d, hEff%sXY", prefix.Data(), dets[iDet].Data()),
                Form("%s, Efficiency of matching; X [cm]; Y [cm]", dets[iDet].Data()), nBinsXY, -20, 100, nBinsXY, -60, 60);
    }
}

void BmnDstQa::ResidualsPullsHistos() {
    const Int_t nDets = 8;
    TString dets[nDets] = {"MWPC", "SILICON", "GEM", "CSC", "TOF400", "DCH1", "TOF700", "DCH2"};

    const Double_t resBoard = 7;

    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, hResX%s", prefix.Data(), dets[iDet].Data()),
                Form("%s, Residuals X; X Residual [cm]; N", dets[iDet].Data()), 100, -resBoard, resBoard);
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, hResY%s", prefix.Data(), dets[iDet].Data()),
                Form("%s, Residuals Y; Y Residual [cm]; N", dets[iDet].Data()), 100, -resBoard, resBoard);

        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, hPullX%s", prefix.Data(), dets[iDet].Data()),
                Form("%s Pulls X; X Pull [cm]; N", dets[iDet].Data()), 100, -resBoard, resBoard);
        fHistoManager->Create1 <TH1F> (Form("%sDST_1d, hPullY%s", prefix.Data(), dets[iDet].Data()),
                Form("%s, Pulls Y; Y Pull [cm]; N", dets[iDet].Data()), 100, -resBoard, resBoard);
    }
}

void BmnDstQa::OccupancyHistos() {
    const Int_t nDets = 5;
    TString dets[nDets] = {"SILICON", "GEM", "CSC", "TOF400", "TOF700"};

    for (Int_t iDet = 0; iDet < nDets; iDet++) {
        if (dets[iDet].Contains("GEM"))
            for (Int_t iStat = 0; iStat < fDetGem->GetNStations(); iStat++) {
                Double_t xUp = -fDetGem->GetStation(iStat)->GetXMinStation();
                Double_t xLow = -fDetGem->GetStation(iStat)->GetXMaxStation();
                Double_t yLow = fDetGem->GetStation(iStat)->GetYMinStation();
                Double_t yUp = fDetGem->GetStation(iStat)->GetYMaxStation();

                fHistoManager->Create2 <TH2D> (Form("%sDST_2d, hOccupancy%s, stat %d", prefix.Data(), dets[iDet].Data(), iStat),
                        Form("Occupancy %s, stat %d; X [cm]; Y [cm]", dets[iDet].Data(), iStat), Int_t(xUp - xLow), xLow, xUp, Int_t(yUp - yLow), yLow, yUp);
            } else if (dets[iDet].Contains("SILICON"))
            for (Int_t iStat = 0; iStat < fDetSilicon->GetNStations(); iStat++) {
                Double_t xUp = -fDetSilicon->GetStation(iStat)->GetXMinStation();
                Double_t xLow = -fDetSilicon->GetStation(iStat)->GetXMaxStation();
                Double_t yLow = fDetSilicon->GetStation(iStat)->GetYMinStation();
                Double_t yUp = fDetSilicon->GetStation(iStat)->GetYMaxStation();

                fHistoManager->Create2 <TH2D> (Form("%sDST_2d, hOccupancy%s, stat %d", prefix.Data(), dets[iDet].Data(), iStat),
                        Form("Occupancy %s, stat %d; X [cm]; Y [cm]", dets[iDet].Data(), iStat), 3 * Int_t(xUp - xLow), xLow, xUp, 3 * Int_t(yUp - yLow), yLow, yUp);
            } else if (dets[iDet].Contains("CSC") && fDetCsc)
            for (Int_t iStat = 0; iStat < fDetCsc->GetNStations(); iStat++) {
                Double_t xUp = -fDetCsc->GetStation(iStat)->GetXMinStation();
                Double_t xLow = -fDetCsc->GetStation(iStat)->GetXMaxStation();
                Double_t yLow = fDetCsc->GetStation(iStat)->GetYMinStation();
                Double_t yUp = fDetCsc->GetStation(iStat)->GetYMaxStation();

                fHistoManager->Create2 <TH2D> (Form("%sDST_2d, hOccupancy%s, stat %d", prefix.Data(), dets[iDet].Data(), iStat),
                        Form("Occupancy %s, stat %d; X [cm]; Y [cm]", dets[iDet].Data(), iStat), Int_t(xUp - xLow), xLow, xUp, Int_t(yUp - yLow), yLow, yUp);
            } else
            fHistoManager->Create2 <TH2F> (Form("%sDST_2d, hOccupancy%s", prefix.Data(), dets[iDet].Data()),
                Form("Occupancy %s; X [cm]; Y [cm]", dets[iDet].Data()), 100, 0., 0., 100, 0., 0.);
    }
}

void BmnDstQa::AverageStripValuePerHit() {
    const Int_t nLayers = 2; // X and X'
    TString layers[nLayers] = {"X", "X'"};

    for (Int_t iLayer = 0; iLayer < nLayers; iLayer++) {
        for (Int_t iStat = 0; iStat < fDetGem->GetNStations(); iStat++)
            fHistoManager->Create1 <TH1I> (Form("%sDST_1d, Ave. strip value per hit in %s (%s)", prefix.Data(), "GEM", layers[iLayer].Data()),
                Form("%s, Ave. strip value per hit (%s); N_{strips}; N", "GEM", layers[iLayer].Data()), 20, 0., 20.);

        for (Int_t iStat = 0; iStat < fDetSilicon->GetNStations(); iStat++)
            fHistoManager->Create1 <TH1I> (Form("%sDST_1d, Ave. strip value per hit in %s (%s)", prefix.Data(), "SILICON", layers[iLayer].Data()),
                Form("%s, Ave. strip value per hit (%s); N_{strips}; N", "SILICON", layers[iLayer].Data()), 20, 0., 20.);

        if (fDetCsc)
            for (Int_t iStat = 0; iStat < fDetCsc->GetNStations(); iStat++)
                fHistoManager->Create1 <TH1I> (Form("%sDST_1d, Ave. strip value per hit in %s (%s)", prefix.Data(), "CSC", layers[iLayer].Data()),
                    Form("%s, Ave. strip value per hit (%s); N_{strips}; N", "CSC", layers[iLayer].Data()), 20, 0., 20.);
    }
}

void BmnDstQa::AverageStripValuePerTrack() {
    const Int_t nLayers = 2; // X and X'
    TString layers[nLayers] = {"X", "X'"};

    for (Int_t iLayer = 0; iLayer < nLayers; iLayer++) {
        for (Int_t iStat = 0; iStat < fDetGem->GetNStations(); iStat++)
            fHistoManager->Create1 <TH1F> (Form("%sDST_1d, Ave. strip value per track in %s (%s)", prefix.Data(), "GEM", layers[iLayer].Data()),
                Form("%s, Ave. strip value per track (%s); N_{strips}; N", "GEM", layers[iLayer].Data()), 20, 0., 20.);

        for (Int_t iStat = 0; iStat < fDetSilicon->GetNStations(); iStat++)
            fHistoManager->Create1 <TH1F> (Form("%sDST_1d, Ave. strip value per track in %s (%s)", prefix.Data(), "SILICON", layers[iLayer].Data()),
                Form("%s, Ave. strip value per track (%s); N_{strips}; N", "SILICON", layers[iLayer].Data()), 20, 0., 20.);

        if (fDetCsc)
            for (Int_t iStat = 0; iStat < fDetCsc->GetNStations(); iStat++)
                fHistoManager->Create1 <TH1F> (Form("%sDST_1d, Ave. strip value per track in %s (%s)", prefix.Data(), "CSC", layers[iLayer].Data()),
                    Form("%s, Ave. strip value per track (%s); N_{strips}; N", "CSC", layers[iLayer].Data()), 20, 0., 20.);
    }
}

