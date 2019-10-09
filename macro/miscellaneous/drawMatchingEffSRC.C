
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

const Int_t nBins = 20;
const Int_t nBinsXY = 50;

const Double_t resBoard = 3;

TH1F* hAllNoCSC = new TH1F("hAllNoCSC", "; Rigidity [GeV/(Qc)]; N", nBins, 0, 15);
TH1F* hAllYesCSC = new TH1F("hAllYesCSC", "; Rigidity [GeV/(Qc)]; N", nBins, 0, 15);
TH1F* hEffCSC = new TH1F("hEffCSC", "; Rigidity [GeV/(Qc)]; Efficiency", nBins, 0, 15);
TH1F* hResXCSC = new TH1F("hResXCSC", "; X Residual [cm]; N", 100, -resBoard, resBoard);
TH1F* hResYCSC = new TH1F("hResYCSC", "; Y Residual [cm]; N", 100, -resBoard, resBoard);
TH1F* hPullXCSC = new TH1F("hPullXCSC", "; X Pull; N", 100, -resBoard, resBoard);
TH1F* hPullYCSC = new TH1F("hPullYCSC", "; Y Pull; N", 100, -resBoard, resBoard);
TH2F* hAllNoCSCXY = new TH2F("hAllNoCSCXY", "; X [cm]; Y [cm]", nBinsXY, -20, 100, nBinsXY, -60, 60);
TH2F* hAllYesCSCXY = new TH2F("hAllYesCSCXY", "; X [cm]; Y [cm]", nBinsXY, -20, 100, nBinsXY, -60, 60);
TH2F* hEffCSCXY = new TH2F("hEffCSCXY", "; X [cm]; Y [cm]", nBinsXY, -20, 100, nBinsXY, -60, 60);

TH1F* hAllNoDCH1 = new TH1F("hAllNoDCH1", "; Rigidity [GeV/(Qc)]; N", nBins, 0, 15);
TH1F* hAllYesDCH1 = new TH1F("hAllYesDCH1", "; Rigidity [GeV/(Qc)]; N", nBins, 0, 15);
TH1F* hEffDCH1 = new TH1F("hEffDCH1", "; Rigidity [GeV/(Qc)]; Efficiency", nBins, 0, 15);
TH1F* hResXDCH1 = new TH1F("hResXDCH1", "; X Residual [cm]; N", 100, -resBoard, resBoard);
TH1F* hResYDCH1 = new TH1F("hResYDCH1", "; Y Residual [cm]; N", 100, -resBoard, resBoard);
TH1F* hPullXDCH1 = new TH1F("hPullXDCH1", "; X Pull; N", 100, -resBoard, resBoard);
TH1F* hPullYDCH1 = new TH1F("hPullYDCH1", "; Y Pull; N", 100, -resBoard, resBoard);
TH2F* hAllNoDCH1XY = new TH2F("hAllNoDCH1XY", "; X [cm]; Y [cm]", nBinsXY, -20, 100, nBinsXY, -60, 60);
TH2F* hAllYesDCH1XY = new TH2F("hAllYesDCH1XY", "; X [cm]; Y [cm]", nBinsXY, -20, 100, nBinsXY, -60, 60);
TH2F* hEffDCH1XY = new TH2F("hEffDCH1XY", "; X [cm]; Y [cm]", nBinsXY, -20, 100, nBinsXY, -60, 60);

TH1F* hAllNoTOF700 = new TH1F("hAllNoTOF700", "; Rigidity [GeV/(Qc)]; N", nBins, 0, 15);
TH1F* hAllYesTOF700 = new TH1F("hAllYesTOF700", "; Rigidity [GeV/(Qc)]; N", nBins, 0, 15);
TH1F* hEffTOF700 = new TH1F("hEffTOF700", "; Rigidity [GeV/(Qc)]; Efficiency", nBins, 0, 15);
TH1F* hResXTOF700 = new TH1F("hResXTOF700", "; X Residual [cm]; N", 100, -resBoard, resBoard);
TH1F* hResYTOF700 = new TH1F("hResYTOF700", "; Y Residual [cm]; N", 100, -resBoard, resBoard);
TH1F* hPullXTOF700 = new TH1F("hPullXTOF700", "; X Pull; N", 100, -resBoard, resBoard);
TH1F* hPullYTOF700 = new TH1F("hPullYTOF700", "; Y Pull; N", 100, -resBoard, resBoard);
TH2F* hAllNoTOF700XY = new TH2F("hAllNoTOF700XY", "; X [cm]; Y [cm]", nBinsXY, -20, 100, nBinsXY, -60, 60);
TH2F* hAllYesTOF700XY = new TH2F("hAllYesTOF700XY", "; X [cm]; Y [cm]", nBinsXY, -20, 100, nBinsXY, -60, 60);
TH2F* hEffTOF700XY = new TH2F("hEffTOF700XY", "; X [cm]; Y [cm]", nBinsXY, -20, 100, nBinsXY, -60, 60);

TH1F* hAllNoDCH2 = new TH1F("hAllNoDCH2", "; Rigidity [GeV/(Qc)]; N", nBins, 0, 15);
TH1F* hAllYesDCH2 = new TH1F("hAllYesDCH2", "; Rigidity [GeV/(Qc)]; N", nBins, 0, 15);
TH1F* hEffDCH2 = new TH1F("hEffDCH2", "; Rigidity [GeV/(Qc)]; Efficiency", nBins, 0, 15);
TH1F* hResXDCH2 = new TH1F("hResXDCH2", "; X Residual [cm]; N", 100, -resBoard, resBoard);
TH1F* hResYDCH2 = new TH1F("hResYDCH2", "; Y Residual [cm]; N", 100, -resBoard, resBoard);
TH1F* hPullXDCH2 = new TH1F("hPullXDCH2", "; X Pull; N", 100, -resBoard, resBoard);
TH1F* hPullYDCH2 = new TH1F("hPullYDCH2", "; Y Pull; N", 100, -resBoard, resBoard);
TH2F* hAllNoDCH2XY = new TH2F("hAllNoDCH2XY", "; X [cm]; Y [cm]", nBinsXY, -20, 100, nBinsXY, -60, 60);
TH2F* hAllYesDCH2XY = new TH2F("hAllYesDCH2XY", "; X [cm]; Y [cm]", nBinsXY, -20, 100, nBinsXY, -60, 60);
TH2F* hEffDCH2XY = new TH2F("hEffDCH2XY", "; X [cm]; Y [cm]", nBinsXY, -20, 100, nBinsXY, -60, 60);

BmnHit* MatchDetector(FairTrackParam* par, TClonesArray* hits, Bool_t doUpdate, Int_t detNum);
void AnalizeFile(TString fileName);

void drawMatchingEffSRC() {
    gStyle->SetOptStat(0);
    gStyle->SetFitFormat("5.3f");
    gStyle->SetOptFit(0010);

    TGeoManager::Import("../run/current_geo_file.root");
    FairRunAna* fRunAna = new FairRunAna();
    BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
    magField->SetScale(108.1 / 55.7);
    fRunAna->SetField(magField);
    magField->Init();

    vector<TString> listOfFiles;
    // listOfFiles.push_back("../run/srcdst_3581.root");
    // listOfFiles.push_back("../run/srcdst_3582.root");
    // listOfFiles.push_back("../run/srcdst_3583.root");
    // listOfFiles.push_back("../run/srcdst_3584.root");
    // listOfFiles.push_back("../run/srcdst_3587.root");
    // listOfFiles.push_back("../run/srcdst_3588.root");
    listOfFiles.push_back("../run/srcdst_3387.root");

    vector<TH1F*> listOfHistoEff = {hEffCSC, hEffDCH1, hEffTOF700, hEffDCH2};
    vector<TH1F*> listOfHistoWithDet = {hAllYesCSC, hAllYesDCH1, hAllYesTOF700, hAllYesDCH2};
    vector<TH1F*> listOfHistoWithoutDet = {hAllNoCSC, hAllNoDCH1, hAllNoTOF700, hAllNoDCH2};
    vector<TH1F*> listOfHistoResX = {hResXCSC, hResXDCH1, hResXTOF700, hResXDCH2};
    vector<TH1F*> listOfHistoResY = {hResYCSC, hResYDCH1, hResYTOF700, hResYDCH2};
    vector<TH1F*> listOfHistoPullX = {hPullXCSC, hPullXDCH1, hPullXTOF700, hPullXDCH2};
    vector<TH1F*> listOfHistoPullY = {hPullYCSC, hPullYDCH1, hPullYTOF700, hPullYDCH2};
    vector<TH2F*> listOfHistoEffXY = {hEffCSCXY, hEffDCH1XY, hEffTOF700XY, hEffDCH2XY};
    vector<TH2F*> listOfHistoWithDetXY = {hAllYesCSCXY, hAllYesDCH1XY, hAllYesTOF700XY, hAllYesDCH2XY};
    vector<TH2F*> listOfHistoWithoutDetXY = {hAllNoCSCXY, hAllNoDCH1XY, hAllNoTOF700XY, hAllNoDCH2XY};

    const Int_t nDet = 4;
    const Int_t nFiles = listOfFiles.size();
    TString namesDet[nDet] = {"CSC", "DCH1", "TOF-700", "DCH2"};

    for (Int_t i = 0; i < nFiles; ++i)
        AnalizeFile(listOfFiles.at(i));

    for (Int_t i = 0; i < nDet; ++i) {
        listOfHistoWithoutDet[i]->Sumw2();
        listOfHistoWithDet[i]->Sumw2();
        listOfHistoEff[i]->Divide(listOfHistoWithDet[i], listOfHistoWithoutDet[i]);
        listOfHistoEff[i]->Scale(100);
        listOfHistoEff[i]->SetMaximum(110);
        listOfHistoEff[i]->SetMinimum(0);
        listOfHistoWithoutDetXY[i]->Sumw2();
        listOfHistoWithDetXY[i]->Sumw2();
        listOfHistoEffXY[i]->Divide(listOfHistoWithDetXY[i], listOfHistoWithoutDetXY[i]);
    }

    TLatex ltx;
    ltx.SetTextSize(0.08);
    TCanvas* cMomWithoutDet = new TCanvas("cMomWithoutDet", "cMomWithoutDet", 1000, 1000);
    cMomWithoutDet->Divide(2, 2, 0, 0);
    TCanvas* cMomWithDet = new TCanvas("cMomWithDet", "cMomWithDet", 1000, 1000);
    cMomWithDet->Divide(2, 2, 0, 0);
    TCanvas* cEff = new TCanvas("eff", "eff", 1000, 1000);
    cEff->Divide(2, 2, 0, 0);
    TCanvas* cEffXY = new TCanvas("effXY", "effXY", 1000, 1000);
    cEffXY->Divide(2, 2);
    TCanvas* cRes = new TCanvas("cRes", "cRes", 2000, 1000);
    cRes->Divide(4, 2);
    TCanvas* cPull = new TCanvas("cPull", "cPull", 2000, 1000);
    cPull->Divide(4, 2);
    for (Int_t i = 0; i < nDet; ++i) {
        cMomWithoutDet->cd(i + 1);
        listOfHistoWithoutDet[i]->SetMarkerStyle(19);
        listOfHistoWithoutDet[i]->SetMarkerSize(1.2);
        listOfHistoWithoutDet[i]->SetLineColor(1);
        listOfHistoWithoutDet[i]->Draw("X0 E1 P");
        cMomWithDet->cd(i + 1)->SetGrid(0,0);
        listOfHistoWithDet[i]->SetMarkerStyle(19);
        listOfHistoWithDet[i]->SetMarkerSize(1.2);
        listOfHistoWithDet[i]->SetLineColor(1);
        listOfHistoWithDet[i]->Draw("X0 E1 P");
        cEff->cd(i + 1)->SetGrid(0,0);
        listOfHistoEff[i]->SetMarkerStyle(19);
        listOfHistoEff[i]->SetMarkerSize(1.2);
        listOfHistoEff[i]->SetLineColor(1);
        listOfHistoEff[i]->Draw("X0 E1 P");
        ltx.DrawLatex(2, 10, namesDet[i]);
        cRes->cd(i + 1)->SetGrid(0,0);
        listOfHistoResX[i]->Fit("gaus", "WWQ");
        listOfHistoResX[i]->Draw();
        ltx.DrawLatex(-1, 0, namesDet[i]);
        cRes->cd(i + 1 + nDet)->SetGrid(0,0);
        listOfHistoResY[i]->Fit("gaus", "WWQ");
        listOfHistoResY[i]->Draw();
        ltx.DrawLatex(-1, 0, namesDet[i]);
        cPull->cd(i + 1)->SetGrid(0,0);
        listOfHistoPullX[i]->Fit("gaus", "WWQ");
        listOfHistoPullX[i]->Draw();
        ltx.DrawLatex(-1, 0, namesDet[i]);
        cPull->cd(i + 1 + nDet)->SetGrid(0,0);
        listOfHistoPullY[i]->Fit("gaus", "WWQ");
        listOfHistoPullY[i]->Draw();
        ltx.DrawLatex(-1, 0, namesDet[i]);
        cEffXY->cd(i + 1)->SetGrid(0,0);
        listOfHistoEffXY[i]->Draw("colz");
    }

    cMomWithoutDet->SaveAs("matchingMomWithoutSRC.pdf");
    cMomWithDet->SaveAs("matchingMomWithSRC.pdf");
    cEff->SaveAs("matchingEffSRC.pdf");
    cEffXY->SaveAs("matchingEffSRCXY.pdf");
    cRes->SaveAs("matchingResSRC.pdf");
    cPull->SaveAs("matchingPullSRC.pdf");
}

void AnalizeFile(TString fileName) {
    TChain* out = new TChain("bmndata");
    out->Add(fileName.Data());
    cout << "File processed: " << fileName << endl;
    cout << "Number of recorded entries = " << out->GetEntries() << endl;

    TClonesArray* gemTracks = nullptr;
    out->SetBranchAddress("BmnGemTrack", &gemTracks);
    TClonesArray* dchHits = nullptr;
    out->SetBranchAddress("BmnDchHit", &dchHits);
    TClonesArray* cscHits = nullptr;
    out->SetBranchAddress("BmnCSCHit", &cscHits);
    TClonesArray* tof700Hits = nullptr;
    out->SetBranchAddress("BmnTof700Hit", &tof700Hits);

    Double_t dX = -1000.0;
    Double_t dY = -1000.0;

    for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++) {
        out->GetEntry(iEv);
        if (iEv % 10000 == 0) printf("event: %d\n", iEv);
        //cout << "N GEM tracks = " << gemTracks->GetEntriesFast() << endl;
        for (Int_t iTrack = 0; iTrack < gemTracks->GetEntriesFast(); iTrack++) {
            BmnGemTrack* track = (BmnGemTrack*)gemTracks->At(iTrack);

            //CSC
            FairTrackParam parLastCSC(*(track->GetParamLast()));
            if (MatchDetector(&parLastCSC, dchHits, kTRUE, 0) != nullptr)
                if (MatchDetector(&parLastCSC, tof700Hits, kFALSE, -1) != nullptr)
                    if (MatchDetector(&parLastCSC, dchHits, kTRUE, 1) != nullptr) {
                        BmnHit* hit = MatchDetector(&parLastCSC, cscHits, kFALSE, 0);
                        hAllNoCSC->Fill(1.0 / parLastCSC.GetQp());
                        hAllNoCSCXY->Fill(parLastCSC.GetX(), parLastCSC.GetY());
                        if (hit != nullptr) {
                            hAllYesCSC->Fill(1.0 / parLastCSC.GetQp());
                            hAllYesCSCXY->Fill(parLastCSC.GetX(), parLastCSC.GetY());
                            hResXCSC->Fill(parLastCSC.GetX() - hit->GetX());
                            hResYCSC->Fill(parLastCSC.GetY() - hit->GetY());
                            hPullXCSC->Fill((parLastCSC.GetX() - hit->GetX()) / hit->GetDx());
                            hPullYCSC->Fill((parLastCSC.GetY() - hit->GetY()) / hit->GetDy());
                        }
                    }

            //DCH1
            FairTrackParam parLastDCH1(*(track->GetParamLast()));
            if (MatchDetector(&parLastDCH1, cscHits, kTRUE, 0) != nullptr)
                if (MatchDetector(&parLastDCH1, tof700Hits, kFALSE, -1) != nullptr)
                    if (MatchDetector(&parLastDCH1, dchHits, kTRUE, 1) != nullptr) {
                        BmnHit* hit = MatchDetector(&parLastDCH1, dchHits, kFALSE, 0);
                        hAllNoDCH1->Fill(1.0 / parLastDCH1.GetQp());
                        hAllNoDCH1XY->Fill(parLastDCH1.GetX(), parLastDCH1.GetY());
                        if (hit != nullptr) {
                            hAllYesDCH1->Fill(1.0 / parLastDCH1.GetQp());
                            hAllYesDCH1XY->Fill(parLastDCH1.GetX(), parLastDCH1.GetY());
                            hResXDCH1->Fill(parLastDCH1.GetX() - hit->GetX());
                            hResYDCH1->Fill(parLastDCH1.GetY() - hit->GetY());
                            hPullXDCH1->Fill((parLastDCH1.GetX() - hit->GetX()) / hit->GetDx());
                            hPullYDCH1->Fill((parLastDCH1.GetY() - hit->GetY()) / hit->GetDy());
                        }
                    }

            //TOF700
            FairTrackParam parLastTOF700(*(track->GetParamLast()));
            if (MatchDetector(&parLastTOF700, cscHits, kTRUE, 0) != nullptr)
                if (MatchDetector(&parLastTOF700, dchHits, kTRUE, 0) != nullptr)
                    if (MatchDetector(&parLastTOF700, dchHits, kTRUE, 1) != nullptr) {
                        BmnHit* hit = MatchDetector(&parLastTOF700, tof700Hits, kFALSE, -1);
                        hAllNoTOF700->Fill(1.0 / parLastTOF700.GetQp());
                        hAllNoTOF700XY->Fill(parLastTOF700.GetX(), parLastTOF700.GetY());
                        if (hit != nullptr) {
                            hAllYesTOF700->Fill(1.0 / parLastTOF700.GetQp());
                            hAllYesTOF700XY->Fill(parLastTOF700.GetX(), parLastTOF700.GetY());
                            hResXTOF700->Fill(parLastTOF700.GetX() - hit->GetX());
                            hResYTOF700->Fill(parLastTOF700.GetY() - hit->GetY());
                            hPullXTOF700->Fill((parLastTOF700.GetX() - hit->GetX()) / hit->GetDx());
                            hPullYTOF700->Fill((parLastTOF700.GetY() - hit->GetY()) / hit->GetDy());
                        }
                    }

            //DCH2
            FairTrackParam parLastDCH2(*(track->GetParamLast()));
            if (MatchDetector(&parLastDCH2, cscHits, kTRUE, 0) != nullptr)
                if (MatchDetector(&parLastDCH2, dchHits, kTRUE, 0) != nullptr)
                    if (MatchDetector(&parLastDCH2, tof700Hits, kFALSE, -1) != nullptr) {
                        BmnHit* hit = MatchDetector(&parLastDCH2, dchHits, kFALSE, 1);
                        hAllNoDCH2->Fill(1.0 / parLastDCH2.GetQp());
                        hAllNoDCH2XY->Fill(parLastDCH2.GetX(), parLastDCH2.GetY());
                        if (hit != nullptr) {
                            hAllYesDCH2->Fill(1.0 / parLastDCH2.GetQp());
                            hAllYesDCH2XY->Fill(parLastDCH2.GetX(), parLastDCH2.GetY());
                            hResXDCH2->Fill(parLastDCH2.GetX() - hit->GetX());
                            hResYDCH2->Fill(parLastDCH2.GetY() - hit->GetY());
                            hPullXDCH2->Fill((parLastDCH2.GetX() - hit->GetX()) / hit->GetDx());
                            hPullYDCH2->Fill((parLastDCH2.GetY() - hit->GetY()) / hit->GetDy());
                        }
                    }
        }
    }
    delete out;
}

// BmnHit* MatchDetector(FairTrackParam* par, TClonesArray* hits, Bool_t doUpdate, Int_t detNum, Double_t& ResX, Double_t& ResY) {
//     //Int_t detNum - need for DCH

//     Double_t minDX = DBL_MAX;
//     Double_t minDY = DBL_MAX;
//     BmnHit* minHit = nullptr;
//     Double_t xCut = 2.0;
//     Double_t yCut = 2.0;

//     BmnKalmanFilter* kalman = new BmnKalmanFilter();
//     Double_t minZ = DBL_MAX;
//     for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); ++iHit) {
//         BmnHit* hit = (BmnHit*)hits->At(iHit);
//         if (!hit) continue;
//         if (hit->GetStation() != detNum) continue;
//         if (hit->GetZ() < minZ) minZ = hit->GetZ();
//     }
//     kalman->TGeoTrackPropagate(par, minZ, 2212, NULL, NULL, kTRUE);

//     for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); ++iHit) {
//         BmnHit* hit = (BmnHit*)hits->At(iHit);
//         if (!hit) continue;
//         if (hit->GetStation() != detNum) continue;
//         kalman->TGeoTrackPropagate(par, hit->GetZ(), 2212, NULL, NULL, kTRUE);
//         Double_t dX = par->GetX() - hit->GetX();
//         Double_t dY = par->GetY() - hit->GetY();
//         if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < minDX && Abs(dY) < minDY) {
//             minHit = hit;
//             minDX = dX;
//             minDY = dY;
//         }
//     }

//     if (minHit) {
//         ResX = minDX;
//         ResY = minDY;
//         if (doUpdate) {
//             Double_t chi = 0;
//             kalman->Update(par, minHit, chi);
//         }
//     }
//     delete kalman;
//     return minHit;
// }

BmnHit* MatchDetector(FairTrackParam* parTrack, TClonesArray* hits, Bool_t doUpdate, Int_t detNum) {
    //Int_t detNum - need for DCH

    FairTrackParam minPar;
    Double_t minDX = DBL_MAX;
    Double_t minDY = DBL_MAX;
    BmnHit* minHit = nullptr;
    Double_t xCut = 5.0;
    Double_t yCut = 5.0;
    Int_t pdg = 2212;

    Double_t minZ = DBL_MAX;
    BmnKalmanFilter* kalman = new BmnKalmanFilter();

    map<Double_t, FairTrackParam> zParamMap;
    for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); ++iHit) {
        BmnHit* hit = (BmnHit*)hits->At(iHit);
        if (!hit) continue;
        if (hit->GetStation() != detNum) continue;
        minZ = Min(minZ, hit->GetZ());
        zParamMap[hit->GetZ()] = FairTrackParam();
    }
    FairTrackParam par(*parTrack);
    kalman->TGeoTrackPropagate(&par, minZ, pdg, NULL, NULL, kTRUE);

    for (map<Double_t, FairTrackParam>::iterator it = zParamMap.begin(); it != zParamMap.end(); ++it) {
        (*it).second = par;
        kalman->TGeoTrackPropagate(&(*it).second, (*it).first, pdg, NULL, NULL, kTRUE);
    }

    for (Int_t iHit = 0; iHit < hits->GetEntriesFast(); ++iHit) {
        BmnHit* hit = (BmnHit*)hits->At(iHit);
        if (!hit) continue;
        if (hit->GetStation() != detNum) continue;
        if (zParamMap.find(hit->GetZ()) == zParamMap.end()) {
            std::cout << "-E- CbmLitNearestHitTofMerger::DoMerge: Z position " << hit->GetZ() << " not found in map. Something is wrong.\n";
        }
        FairTrackParam tpar(zParamMap[hit->GetZ()]);
        Double_t dX = tpar.GetX() - hit->GetX();
        Double_t dY = tpar.GetY() - hit->GetY();
        if (Abs(dX) < xCut && Abs(dY) < yCut && Abs(dX) < minDX && Abs(dY) < minDY) {
            minHit = hit;
            minDX = dX;
            minDY = dY;
            minPar = tpar;
        }
    }

    *(parTrack) = par;

    if (minHit) {
        *(parTrack) = minPar;
        if (doUpdate) {
            Double_t chi = 0;
            kalman->Update(parTrack, minHit, chi);
        }
    }
    delete kalman;

    return minHit;
}
