#include "TMath.h"start
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TChain.h"
#include "TClonesArray.h"
#include <vector>
#include "TVector3.h"
#include "TFile.h"
#include "TGeoManager.h"

using namespace TMath;

void Results_TOF700(char *fname = "bmn_run0362_digit.root") {

    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    int RUN;
    sscanf(&fname[strlen(fname) - 14], "%d", &RUN);

    const char *mapping;
    if(RUN < 470) mapping="TOF700_map_Feb20_Mar11.txt";
    else mapping="TOF700_map_Mar11.txt";

    BmnTof2Raw2Digit TOF2(mapping);
    TOF2.print();

    if (RUN < 470)
    {
	int numgeom[4] = {1,5,3,6};
	TOF2.readGeom(numgeom);
    }
    else
    {
	int numgeom[4] = {1,2,3,4};
	TOF2.readGeom(numgeom);
    }
    TOF2.printGeom();

    TChain *bmnTree = new TChain("BMN_DIGIT");
    bmnTree->Add(fname);

    TClonesArray *tof700Digits;
    bmnTree->SetBranchAddress("bmn_tof2_digit", &tof700Digits);

    Int_t startEvent = 0;
    Int_t nEvents = bmnTree->GetEntries();

    gStyle->SetOptFit(111);

    TH1F *hdiff = new TH1F("hdiff", "Time difference, small chambers 1 and 2", 1000, -25, 25);

    TH1F *htime[2] = {0};
    TH1F *hstrips[2] = {0};
    TH1F *hstripsmax[2] = {0};
    TH1F *htimemax[2] = {0};

    char name[128], title[128];
    for (int i = 0; i<2; i++)
    {
	sprintf(name,"Time_chamber_%d", i+1);
	sprintf(title,"Time, chamber %d", i+1);
	htime[i] = new TH1F(name, title, 1000, -50, 50);
	sprintf(name,"Strips_rate_chamber_%d", i+1);
	sprintf(title,"Strips rate, chamber %d", i+1);
	hstrips[i] = new TH1F(name, title, 32, 0, 32);

	sprintf(name,"Time_chamber_%d_max_strip", i+1);
	sprintf(title,"Time, chamber %d, max strip", i+1);
	htimemax[i] = new TH1F(name, title, 1000, -50, 50);
	sprintf(name,"Strips_rate_chamber_%d_max_strip", i+1);
	sprintf(title,"Strips rate, chamber %d, max strip", i+1);
	hstripsmax[i] = new TH1F(name, title, 32, 0, 32);
    }

    for (Int_t iEv = startEvent; iEv < startEvent + nEvents; iEv++) {
        bmnTree->GetEntry(iEv);

        if (iEv % 10000 == 0) cout << "Event: " << iEv << "/" << startEvent + nEvents << endl;

	float wmax[4] = {0.,0.,0.,0.}, tmax[4] = {-1000., -1000., -1000., -1000.};
	int smax[4] = {-1,-1,-1,-1};

	for (Int_t iDig = 0; iDig < tof700Digits->GetEntriesFast(); ++iDig) {
    	    BmnTof2Digit *digit = (BmnTof2Digit*) tof700Digits->At(iDig);
    	    if (digit == NULL) continue;
    	    Short_t plane = digit->GetPlane();
    	    Short_t strip = digit->GetStrip();
    	    Float_t time = digit->GetTime();
    	    Float_t width = digit->GetAmplitude();
//    	    if (iEv % 10000 == 0) cout << "plane = " << plane << " strip = " << strip << " time = " << time << " width = " << width << endl;
	    if (width > wmax[plane] && width > 2000)
	    {
		wmax[plane] = width;
		tmax[plane] = time;
		smax[plane] = strip;
	    }
	    if (plane < 2)
	    {
		htime[plane]->Fill(time);
		hstrips[plane]->Fill(strip);
	    }
	} // tof700Digits loop
	for (int i=0; i<2; i++)
	{
	    if (smax[i] > -1)
	    {
		htimemax[i]->Fill(tmax[i]);
		hstripsmax[i]->Fill(smax[i]);
	    }
	}
	if (smax[0] == 28 && smax[1] == 28) hdiff->Fill(tmax[0]-tmax[1]);
    } // event loop

    FitIn2Sigma(hdiff);
    for (int i=0; i<2; i++) FitIn2Sigma(htime[i]);
    for (int i=0; i<2; i++) FitIn2Sigma(htimemax[i]);
    TCanvas *c = new TCanvas("c", "RPC Times difference", 900, 900);
    c->Divide(1,1);
    c->cd();
    c->cd(1);
    hdiff->Draw();

    TCanvas *c1 = new TCanvas("c1", "RPC time - T0 time", 900, 900);
    c1->Divide(2,2);
    c1->cd();
    for (int i=0; i<2; i++)
    {
	c1->cd(i*2+1);
	htime[i]->Draw();
	c1->cd(i*2+2);
	htimemax[i]->Draw();
    }
    TCanvas *c2 = new TCanvas("c2", "RPC strip rate", 900, 900);
    c2->Divide(2,2);
    c2->cd();
    for (int i=0; i<2; i++)
    {
	c2->cd(i*2+1);
	hstrips[i]->Draw();
	c2->cd(i*2+2);
	hstripsmax[i]->Draw();
    }
}

double FitIn2Sigma(TH1F *h)
{
  double mean, sigm, sigmw = 0., lev1 = 1.75, lev2 = 3.;
  double total = 0, peak = 0;
  int mib, mab, nbi;

  if ((total = h->GetEntries()) < 5.)
    {
      if (Debug) printf("Number of histogram entries is too few!\n");
      return 0.;
    }
  int nonzero = 0;
  for (int i=1; i<h->GetNbinsX(); i++)
    if (h->GetBinContent(i)>0) nonzero++;
  if (nonzero < 3)
    {
      if (nonzero < 2)
      {
        if (Debug) printf("Number of non-empty bins is too few!\n");
        return 0.;
      }
      lev1 = 5.;
      mean = h->GetMean();
      sigm = h->GetRMS();
      sigmw = sigm;
      if (sigm < h->GetBinWidth(1)) sigmw = h->GetBinWidth(1);
      TF1 *fg = new TF1("fg","gaus",mean-lev1*sigmw,mean+lev1*sigmw);
      fg->SetParameter(0,h->GetMaximum());
      fg->SetParameter(1,mean);
      fg->FixParameter(1,mean);
      fg->SetParameter(2,sigm);
      fg->FixParameter(2,sigm);
      h->Fit(fg, "Q0IB", "", mean-lev1*sigmw, mean+lev1*sigmw);
      fg->ResetBit(TF1::kNotDraw);
      h->SetAxisRange(mean-lev1*sigmw, mean+lev1*sigmw);
      fg->ResetBit(TF1::kNotDraw);
      if (fg->GetParameter(0) > 1.) return 100.;
    }
  if (nonzero == 3)
    {
      lev1 = 5.;
      mean = h->GetMean();
      sigm = h->GetRMS();
      sigmw = sigm;
      if (sigm < h->GetBinWidth(1)) sigmw = h->GetBinWidth(1);
      TF1 *fg = new TF1("fg","gaus",mean-lev1*sigmw,mean+lev1*sigmw);
      fg->SetParameter(0,h->GetMaximum());
      fg->SetParameter(1,mean);
      fg->FixParameter(1,mean);
      fg->SetParameter(2,sigm);
      h->Fit(fg, "Q0IB", "", mean-lev1*sigmw, mean+lev1*sigmw);
      fg->ResetBit(TF1::kNotDraw);
      h->SetAxisRange(mean-lev1*sigmw, mean+lev1*sigmw);
      fg->ResetBit(TF1::kNotDraw);
      if (fg->GetParameter(0) > 1.) return 100.;
    }
  mean = h->GetMean();
  sigm = h->GetRMS();
  mib = h->FindBin(mean-lev1*sigm);
  mab = h->FindBin(mean+lev1*sigm);
  if ((mab-mib+1) < 4)
  {
    lev1 = 2.;
    mib = h->FindBin(mean-lev1*sigm);
    mab = h->FindBin(mean+lev1*sigm);
    if ((mab-mib+1) < 4)
    {
      lev1 = 2.25;
    }
  }
  double min0 = mean-lev1*sigm;
  double max0 = mean+lev1*sigm;
  h->Fit("gaus", "Q0I", "", mean-lev1*sigm, mean+lev1*sigm);
  TF1 *ff = h->GetFunction("gaus"); 
  mean = ff->GetParameter(1);
  sigm = ff->GetParameter(2);
  if (mean < min0 || mean > max0)
  {
    h->Fit("gaus", "WQ0I", "", mean-lev1*sigm, mean+lev1*sigm);
    ff = h->GetFunction("gaus"); 
    mean = ff->GetParameter(1);
    sigm = ff->GetParameter(2);
  }
  h->Fit("gaus", "Q0I", "", mean-lev1*sigm, mean+lev1*sigm);
  ff = h->GetFunction("gaus"); 
  mean = ff->GetParameter(1);
  sigm = ff->GetParameter(2);
  h->Fit("gaus", "Q0I", "", mean-lev1*sigm, mean+lev1*sigm);
  ff = h->GetFunction("gaus"); 
  mean = ff->GetParameter(1);
  sigm = ff->GetParameter(2);
  ff->ResetBit(TF1::kNotDraw);
  mib = h->FindBin(mean-lev2*sigm);
  mab = h->FindBin(mean+lev2*sigm);
  nbi = h->GetNbinsX();
  peak = h->Integral(mib < 1 ? 1 : mib, mab > nbi ? nbi : mab);
  h->SetAxisRange(mean-10.*sigm, mean+10.*sigm);
  return total > 0. ? peak/total*100. : 0.;
}
