
using namespace std;

void Results_ZDC(char *fname="bmn_run0543_digit.root") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    /////////////////////////////////////////////////////////////////////////////////////
    TChain *bmnTree = new TChain("BMN_DIGIT");
    bmnTree->Add(fname);

    TClonesArray *zdcDigits;
    bmnTree->SetBranchAddress("bmn_zdc_digit", &zdcDigits);

    Int_t startEvent = 0;
    Int_t nEvents = bmnTree->GetEntries();

    gStyle->SetOptFit(111);

    int numbers[9] = {22,23,24,28,29,30,36,37,38};
    char name[64], title[64];
    TH1F *hamp[9] = {0};
    TH1F *hsum = new TH1F("hsum", "Sum of amplitudes 3x3", 1000, 0, 200);
    TH2F *hpro = new TH2F("hpro", "Cluster profile", 20, -750, 750, 20, -750, 750);
    for (int i=0; i<9; i++)
    {
	sprintf(name, "hamp%02d", i);
	sprintf(title, "Amplitude channel %02d", numbers[i]);
	hamp[i] = new TH1F(name, title, 1000, 0, 200);
    }
    /////////////////////////////////////////////////////////////////////////////////////

    for (Int_t iEv = startEvent; iEv < startEvent + nEvents; iEv++) {
        bmnTree->GetEntry(iEv);

        if (iEv % 10000 == 0) cout << "Event: " << iEv << "/" << startEvent + nEvents << endl;

	Float_t sum = 0.;

	for (Int_t iDig = 0; iDig < zdcDigits->GetEntriesFast(); ++iDig) {
    	    BmnZDCDigit *digit = (BmnZDCDigit*) zdcDigits->At(iDig);
    	    if (digit == NULL) continue;
    	    Short_t chan = digit->GetChannel();
    	    Float_t ampl = digit->GetAmp();
    	    Float_t x = digit->GetX();
    	    Float_t y = digit->GetY();
	    for (int i=0; i<9; i++)
	    {
		if (chan == numbers[i])
		{
		    hamp[i]->Fill(ampl);
		    hpro->Fill(x,y,ampl);
		    sum += ampl;
		}
	    }
	}
	hsum->Fill(sum);
    }
    /////////////////////////////////////////////////////////////////////////////////////
    TCanvas *c = new TCanvas("c","Channel Amplitudes", 700,900);
    c->cd();
    c->Divide(3,3);
    for (int i=0; i<9; i++)
    {
	c->cd(i+1);
	hamp[i]->Draw();
	gPad->AddExec("exsel","select_hist()");
    }
    TCanvas *c1 = new TCanvas("c1","Sum of Channel Amplitudes", 700,900);
    c1->cd();
    c1->Divide(1,1);
    c1->cd(1);
    hsum->Draw();

    TCanvas *c2 = new TCanvas("c2","Average Shape", 700,900);
    c2->cd();
    c2->Divide(1,1);
    c2->cd(1);
    hpro->Draw();

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
