R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void testBmnZdcBScanner(Int_t runId = 3977, UInt_t eventId = 1) {
    
    TChain mChain("bmndata");
	
	TString inp = TString::Format("/nica/mpd22/bmnDigits_RUN7/bmn_run%d_digi.root",runId);
    mChain.Add(inp.Data());
    TClonesArray * pArrayOfBmnZDCDigit = new TClonesArray("BmnZDCDigit");
    mChain.SetBranchAddress("ZDC",&pArrayOfBmnZDCDigit);
    mChain.GetEntry(eventId);
	
    BmnZdcBScanner app;
    app.SetEventData(pArrayOfBmnZDCDigit);
    app.AddSimDigit("/lhep/users/pnaleks/mpd12/sim/ArAl/digi/ArAl_3.2AGeV_mb_03_run7_digi.root");
    app.SetSimScale(30e3);
    app.SetScanWidth(5000);
    app.SetShowProgress(1);
	
	printf("\nScan E...\n");
    TH1D * hBE = app.Scan('E');
	printf("\n...Done\nScan M\n");
    TH1D * hBM = app.Scan('M');
	printf("\n...Done\n");
    
    TCanvas * C = new TCanvas();
	
    hBE->Scale(1./hBE->GetEntries());
    hBM->Scale(1./hBM->GetEntries());

    TH1D * hBEM = new TH1D();
    (*hBEM) = (*hBE) * (*hBM);
    hBEM->Scale(1./hBEM->Integral());
    hBEM->SetName("hBEM");

	TString title = TString::Format("b-spectra (Run:%d Ev:%u)",runId,eventId);
	hBEM->SetTitle(title.Data());
	hBEM->SetXTitle("b, (fm)");
	hBEM->SetLineColor(4);
	hBEM->SetLineWidth(4);
	hBEM->Rebin(2);
	hBEM->Draw("hist");

	hBE->SetLineColor(2);
	hBE->SetLineWidth(2);
	hBE->Rebin(2);
	hBE->Draw("hist same");

	hBM->SetLineColor(6);
	hBM->SetLineWidth(2);
	hBM->Rebin(2);
	hBM->Draw("hist same");
}
