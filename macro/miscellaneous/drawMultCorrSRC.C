R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
//
void drawMultCorrSRC(TString fileName = "../run/srcdst_3332.root", Int_t nEvents = 50000) {

    TChain* out = new TChain("bmndata");
    out->Add(fileName.Data());

    cout << "#recorded entries = " << out->GetEntries() << endl;

    TClonesArray* MwpcTracks = nullptr;
    out->SetBranchAddress("BmnMwpcTrack", &MwpcTracks);
    TClonesArray* dchTracks = nullptr;
    out->SetBranchAddress("BmnDchTrack", &dchTracks);
    TClonesArray* gemTracks = nullptr;
    out->SetBranchAddress("BmnGemTrack", &gemTracks);
    TClonesArray* silTracks = nullptr;
    out->SetBranchAddress("BmnSiliconTrack", &silTracks);
    TClonesArray* cscHits = nullptr;
    out->SetBranchAddress("BmnCSCHit", &cscHits);
    TClonesArray* tof700Hits = nullptr;
    out->SetBranchAddress("BmnTof700Hit", &tof700Hits);

    const Int_t nDet = 7;

    TString names[nDet] = {"MWPC", "SIL", "GEM", "CSC", "TOF700", "DCH1", "DCH2"};

    gStyle->SetOptStat(0);
    gStyle->SetPalette(53);  //(kGreyScale);

    TH2F* hist[nDet][nDet];
    for (Int_t iDet = 0; iDet < nDet; iDet++)
        for (Int_t jDet = 0; jDet < nDet; jDet++) {
            TString histName = names[iDet] + " - " + names[jDet];
            hist[iDet][jDet] = new TH2F(histName, "", 10, 0, 10, 10, 0, 10);
        }

    for (Int_t iEv = 0; iEv < nEvents; iEv++) {
        out->GetEntry(iEv);
        if (iEv % 1000 == 0) printf("event: %d\n", iEv);

        Int_t nSil = (silTracks) ? silTracks->GetEntriesFast() : 0;
        Int_t nGem = (gemTracks) ? gemTracks->GetEntriesFast() : 0;
        Int_t nCsc = (cscHits) ? cscHits->GetEntriesFast() : 0;
        Int_t nTof700 = (tof700Hits) ? tof700Hits->GetEntriesFast() : 0;
        Int_t nDch = (dchTracks) ? dchTracks->GetEntriesFast() : 0;
        Int_t nDch1 = 0;
        Int_t nDch2 = 0;
        if (dchTracks)
            for (Int_t iTrack = 0; iTrack < dchTracks->GetEntriesFast(); iTrack++) {
                BmnDchTrack* track = (BmnDchTrack*)dchTracks->At(iTrack);
                if (track->GetParamFirst()->GetZ() < 550.0) nDch1++;
                if (track->GetParamFirst()->GetZ() > 650.0) nDch2++;
            }
        Int_t nMwpc = 0;
        if (MwpcTracks)
            for (Int_t iTrack = 0; iTrack < MwpcTracks->GetEntriesFast(); iTrack++) {
                BmnMwpcTrack* track = (BmnMwpcTrack*)MwpcTracks->At(iTrack);
                if (track->GetParamFirst()->GetZ() > -300.0) nMwpc++;
            }

        Int_t nItem[nDet] = {nMwpc, nSil, nGem, nCsc, nTof700, nDch1, nDch2};

        for (Int_t iDet = 0; iDet < nDet; iDet++)
            for (Int_t jDet = 0; jDet < nDet; jDet++)
                hist[iDet][jDet]->Fill(nItem[jDet], nItem[iDet]);
    }

    TCanvas* corr = new TCanvas("corr", "corr", 1000, 1000);
    corr->Divide(nDet + 1, nDet + 1, 0, 0);
    for (Int_t iDet = 0; iDet < nDet; iDet++)
        for (Int_t jDet = 0; jDet < nDet; jDet++) {
            Int_t padId = (iDet + 1) * (nDet + 1) + jDet + 2;
            TPad* pad = (TPad*)corr->cd(padId);
            pad->SetGrid(0, 0);
            hist[iDet][jDet]->Draw("col");
            hist[iDet][jDet]->GetXaxis()->SetLabelSize(0.1);
        }
    for (Int_t iDet = 0; iDet < nDet; iDet++) {
        corr->cd(iDet + 2);
        TLatex latexX;
        latexX.SetTextSize(0.2);
        latexX.DrawLatex(0.2, 0.5, names[iDet]);
        corr->cd((iDet + 1) * (nDet + 1) + 1);
        TLatex latexY;
        latexY.SetTextSize(0.2);
        latexY.DrawLatex(0.2, 0.5, names[iDet]);
    }
    corr->SaveAs("MultCorrSRC.pdf");
}
