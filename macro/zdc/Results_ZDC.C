
using namespace std;

void Results_ZDC(char *fname="bmn_run0871_digit.root") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    /////////////////////////////////////////////////////////////////////////////////////
    TChain *bmnTree = new TChain("BMN_DIGIT");
    bmnTree->Add(fname);

    UInt_t TRIGWORD = 0;
    TClonesArray *zdcDigits;
    bmnTree->SetBranchAddress("bmn_zdc_digit", &zdcDigits);
    bmnTree->SetBranchAddress("bmn_trigword",  &TRIGWORD);   

    Int_t startEvent = 0;
    Int_t nEvents = bmnTree->GetEntries();

    gStyle->SetOptFit(111);

    char name[64], title[64];
    TH1F *hamp = 0;
    sprintf(name, "hamp");
    sprintf(title, "Module amplitude");
    hamp = new TH1F(name, title, 500, 0, 1000);
    TH1F *hsum = new TH1F("hsum", "Sum of amplitudes", 1000, 0, 5000);
    TH2F *hpro = new TH2F("hpro", "Cluster profile", 22, -76*11, 76*11, 14, -76*7, 76*7);
    /////////////////////////////////////////////////////////////////////////////////////

    int n_phys = 0, n_led = 0;
    for (Int_t iEv = startEvent; iEv < startEvent + nEvents; iEv++) {

	TRIGWORD = 0;

        bmnTree->GetEntry(iEv);

	if ((TRIGWORD&0x8) == 0) n_phys++;
	else if ((TRIGWORD&0x8) == 0x8) n_led++;

        if (iEv % 10000 == 0) cout << "Event: " << iEv << "/" << startEvent + nEvents << " ( phys " << n_phys << ", led " << n_led << " )" << endl;

	if ((TRIGWORD&0x8) != 0) continue;

	Float_t sum = 0.;

	int xbin = 0;
	for (Int_t iDig = 0; iDig < zdcDigits->GetEntriesFast(); ++iDig) {
    	    BmnZDCDigit *digit = (BmnZDCDigit*) zdcDigits->At(iDig);
    	    if (digit == NULL) continue;
    	    Short_t chan = digit->GetChannel();
    	    Float_t ampl = digit->GetAmp();
    	    Float_t x = digit->GetX();
    	    Float_t y = digit->GetY();
	    if (ampl > 0.) hamp->Fill(ampl);
	    if (digit->GetSize() == 1) hpro->Fill(x,y,ampl);
	    else // equalization for BIG modules
	    {
		xbin = hpro->FindBin(x+5.,y+5.);
		hpro->SetBinContent(xbin,ampl/4.);
		xbin = hpro->FindBin(x+5.,y-5.);
		hpro->SetBinContent(xbin,ampl/4.);
		xbin = hpro->FindBin(x-5.,y+5.);
		hpro->SetBinContent(xbin,ampl/4.);
		xbin = hpro->FindBin(x-5.,y-5.);
		hpro->SetBinContent(xbin,ampl/4.);
	    }
	    sum += ampl;
	}
	if (sum > 0.) hsum->Fill(sum);
    }
    /////////////////////////////////////////////////////////////////////////////////////
    TCanvas *c = new TCanvas("c","Channel Amplitudes", 700,900);
    c->cd();
    c->Divide(1,1);
    c->cd(1);
    hamp->Draw();
    gPad->AddExec("exsel","select_hist()");

    TCanvas *c1 = new TCanvas("c1","Sum of Channel Amplitudes", 700,900);
    c1->cd();
    c1->Divide(1,1);
    c1->cd(1);
    hsum->Draw();
    gPad->AddExec("exsel","select_hist()");

    TCanvas *c2 = new TCanvas("c2","Average Shape", 700,900);
    c2->cd();
    c2->Divide(1,1);
    c2->cd(1);
    hpro->Draw();
    gPad->AddExec("exsel","select_hist()");

}

void select_hist()
{
  //example of macro called when a mouse event occurs in a pad.
  // Example:
  // Root > TFile f("hsimple.root");
  // Root > hpxpy.Draw();
  // Root > c1.AddExec("exsel","select_hist()");
  // When moving the mouse in the canvas, a second canvas shows the
  // projection along X of the bin corresponding to the Y position
  // of the mouse. The resulting histogram is fitted with a gaussian.
  // A "dynamic" line shows the current bin position in Y.
  // This more elaborated example can be used as a starting point
  // to develop more powerful interactive applications exploiting CINT
  // as a development engine.
  //Author: Rene Brun

  int event = gPad->GetEvent();
  if (event != 11) return;

  TObject *select = gPad->GetSelected();
  if(!select) return;
  if (!(select->InheritsFrom("TH1"))) return;

  //create or set the new canvas c2
  TCanvas *csingle = 0;
  if ((csingle = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("csingle")));
  else    csingle = new TCanvas("csingle");
  csingle->cd();

  //draw slice corresponding to mouse position
  if (!(select->InheritsFrom("TH1F")))
    {
      TH1F *h = (TH1F *)select;
      h->Draw();
    }
  else if (!(select->InheritsFrom("TH2F")))
    {
      TH2F *h = (TH2F *)select;
      h->Draw();
    }
  else if (!(select->InheritsFrom("TProfile")))
    {
      TProfile *h = (TProfile *)select;
      h->Draw();
    }
  else if (!(select->InheritsFrom("TH1D")))
    {
      TH1D *h = (TH1D *)select;
      h->Draw();
    }
  else if (!(select->InheritsFrom("TH2D")))
    {
      TH2D *h = (TH2D *)select;
      h->Draw();
    }
  else if (!(select->InheritsFrom("TProfile2D")))
    {
      TProfile2D *h = (TProfile2D *)select;
      h->Draw();
    }
  else return;
  csingle->Update();
}
