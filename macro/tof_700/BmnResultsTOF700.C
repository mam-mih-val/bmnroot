#include "TMath.h"
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

#define NUMBER_CHAMBERS 24
#define DIFF_CHAMB_1 9
#define DIFF_CHAMB_2 18

#if NUMBER_CHAMBERS == 15
int champos[NUMBER_CHAMBERS] = {5,10,1,6,11,2,7,12,3,8,13,4,9,14,0};
int wmi[NUMBER_CHAMBERS] = {2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500};
int wma[NUMBER_CHAMBERS] = {3300,3300,3300,3300,3300,3300,3300,3300,3300,3300,3300,3300,3300,3300,3300};
#define NDX 5
#define NDY 3
#else
#if NUMBER_CHAMBERS == 24
int champos[NUMBER_CHAMBERS] = {17,18, 3, 1,19, 4,23,20, 5,15,21, 6, 2,22, 9,10,11,12,13,14, 7, 8, 0,16};
int wmi[NUMBER_CHAMBERS] = {2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500};
int wma[NUMBER_CHAMBERS] = {4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000,4000};
#define NDX 8
#define NDY 3
#else
int champos[NUMBER_CHAMBERS] = {0};
int wmi[NUMBER_CHAMBERS] = {2500};
int wma[NUMBER_CHAMBERS] = {3300};
#define NDX 1
#define NDY 1
#endif
#endif

using namespace TMath;

void BmnResultsTOF700(char *fname = "../raw/bmn_run1421_digi.root") {

    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TClonesArray *tof700Digits;
    TChain *bmnTree = new TChain("cbmsim");
    if (bmnTree->Add(fname) == 0)
    {
	    printf("Can't find BMN digits tree in file %s!\n", fname);
	    return;
    }
    else
    {
	bmnTree->SetBranchAddress("TOF700", &tof700Digits);
    }

    char name[128], title[128];
    Int_t startEvent = 0;
    Int_t nEvents = bmnTree->GetEntries();

    printf("\nNumber of events in tree is %d\n", nEvents);

    gStyle->SetOptFit(111);

    TH1F *htime[NUMBER_CHAMBERS] = {0};
    TH1F *hwidth[NUMBER_CHAMBERS] = {0};
    TH1F *hlrdiff[NUMBER_CHAMBERS] = {0};
    TH1F *hstrips[NUMBER_CHAMBERS] = {0};
    TH1F *hstripsmax[NUMBER_CHAMBERS] = {0};
    TH1F *htimemax[NUMBER_CHAMBERS] = {0};
    TH1F *hdiff = 0;

    for (int i = 0; i<NUMBER_CHAMBERS; i++)
    {
	sprintf(name,"Time_chamber_%d", i+1);
	sprintf(title,"Time, chamber %d", i+1);
	htime[i] = new TH1F(name, title, 2000, -50, 50);
	sprintf(name,"Width_chamber_%d", i+1);
	sprintf(title,"Width, chamber %d", i+1);
	hwidth[i] = new TH1F(name, title, 2500, 0, 5000);
	sprintf(name,"LR-Diff_chamber_%d", i+1);
	sprintf(title,"LR-Diff, chamber %d", i+1);
	hlrdiff[i] = new TH1F(name, title, 200, -5, 5);
	sprintf(name,"Strips_rate_chamber_%d", i+1);
	sprintf(title,"Strips rate, chamber %d", i+1);
	hstrips[i] = new TH1F(name, title, 32, 0, 32);
	sprintf(name,"Time_chamber_%d_max_strip", i+1);
	sprintf(title,"Time, chamber %d, max strip", i+1);
	htimemax[i] = new TH1F(name, title, 2000, -50, 50);
	sprintf(name,"Strips_rate_chamber_%d_max_strip", i+1);
	sprintf(title,"Strips rate, chamber %d, max strip", i+1);
	hstripsmax[i] = new TH1F(name, title, 32, 0, 32);
    }

    sprintf(name,"Time_diff_chabmers_%d_%d", DIFF_CHAMB_1, DIFF_CHAMB_2);
    sprintf(title,"Time difference, chambers %d and %d", DIFF_CHAMB_1, DIFF_CHAMB_2);
    hdiff = new TH1F(name, title, 10000, -250, 250);

    for (Int_t iEv = startEvent; iEv < startEvent + nEvents; iEv++) {
//    for (Int_t iEv = startEvent; iEv < startEvent + 10000; iEv++) {
        bmnTree->GetEntry(iEv);

        if (iEv % 10000 == 0)
	{
	    cout << "Event: " << iEv << "/" << startEvent + nEvents << endl;
	}
	float wmax[NUMBER_CHAMBERS] = {0.};
	float tmax[NUMBER_CHAMBERS] = {-10000000.};
	int smax[NUMBER_CHAMBERS] = {-1};

	for (int i = 0; i<NUMBER_CHAMBERS; i++)
	{
	    wmax[i] = 0.;
	    tmax[i] = -1000000.;
	    smax[i] = -1;
	}

	for (Int_t iDig = 0; iDig < tof700Digits->GetEntriesFast(); ++iDig) {
    	    BmnTof2Digit *digit = (BmnTof2Digit*) tof700Digits->At(iDig);
    	    if (digit == NULL) continue;
    	    Short_t plane = digit->GetPlane();
    	    Short_t strip = digit->GetStrip();
    	    Float_t time = digit->GetTime();
    	    Float_t width = digit->GetAmplitude();
	    Float_t lrdiff = digit->GetDiff();
//    	    if (iEv % 10 == 0) cout << "plane = " << plane << " strip = " << strip << " time = " << time << " width = " << width << endl;
//    	    if (iEv % 10 == 0) cout << "wmi = " << wmi[plane] << " wma = " << wma[plane] << endl;
	    if (plane >= NUMBER_CHAMBERS) continue;
	    hwidth[plane]->Fill(width);
	    if (width < wmi[plane] || width > wma[plane]) continue;
	    if (width > wmax[plane])
	    {
		wmax[plane] = width;
		tmax[plane] = time;
		smax[plane] = strip;
	    }
	    htime[plane]->Fill(time);
	    hstrips[plane]->Fill(strip);
	    hlrdiff[plane]->Fill(lrdiff);
	} // tof700Digits loop

	for (int i=0; i<NUMBER_CHAMBERS; i++)
	{
	    if (smax[i] > -1)
	    {
		htimemax[i]->Fill(tmax[i]);
		hstripsmax[i]->Fill(smax[i]);
	    }
	}
	if ((smax[DIFF_CHAMB_1] == 21) && (smax[DIFF_CHAMB_2] == 21) )
	{
		hdiff->Fill(tmax[DIFF_CHAMB_1]-tmax[DIFF_CHAMB_2]);
	}
    } // event loop

    FitIn(hdiff, -0.2, +0.2);
    for (int i=0; i<NUMBER_CHAMBERS; i++) FitIn(htime[i], -0.2, +0.2);
    for (int i=0; i<NUMBER_CHAMBERS; i++) FitIn(htimemax[i], -0.2, +0.2);

    TCanvas *ct = new TCanvas("ct", "RPC time - T0 time", 900, 900);
    ct->Divide(NDX,NDY);
    ct->cd();
    for (int i=0; i<NUMBER_CHAMBERS; i++)
    {
	ct->cd(champos[i]+1);
	htime[i]->Draw();
	gPad->AddExec("exselt","select_hist()");
    }

    TCanvas *c1m = new TCanvas("c1m", "RPC time max strip - T0 time", 900, 900);
    c1m->Divide(NDX,NDY);
    c1m->cd();
    for (int i=0; i<NUMBER_CHAMBERS; i++)
    {
	c1m->cd(champos[i]+1);
	htimemax[i]->Draw();
	gPad->AddExec("exselt","select_hist()");
    }


    TCanvas *c2 = new TCanvas("c2", "RPC strip rate", 900, 900);
    c2->Divide(NDX,NDY);
    c2->cd();
    for (int i=0; i<NUMBER_CHAMBERS; i++)
    {
	c2->cd(champos[i]+1);
	hstrips[i]->Draw();
	gPad->AddExec("exselt","select_hist()");
    }

    TCanvas *c2m = new TCanvas("c2m", "RPC maximal strip rate", 900, 900);
    c2m->Divide(NDX,NDY);
    c2m->cd();
    for (int i=0; i<NUMBER_CHAMBERS; i++)
    {
	c2m->cd(champos[i]+1);
	hstripsmax[i]->Draw();
	gPad->AddExec("exselt","select_hist()");
    }

    TCanvas *cw = new TCanvas("cw", "RPC pulse width", 900, 900);
    cw->Divide(NDX,NDY);
    cw->cd();
    for (int i=0; i<NUMBER_CHAMBERS; i++)
    {
	cw->cd(champos[i]+1);
	hwidth[i]->Draw();
	gPad->AddExec("exselt","select_hist()");
    }

    TCanvas *clr = new TCanvas("clr", "LR Time Difference", 900, 900);
    clr->Divide(NDX,NDY);
    clr->cd();
    for (int i=0; i<NUMBER_CHAMBERS; i++)
    {
	clr->cd(champos[i]+1);
	hlrdiff[i]->Draw();
	gPad->AddExec("exselt","select_hist()");
    }

    TCanvas *cdif = new TCanvas("cdif", "Time Difference", 900, 900);
    cdif->Divide(1,1);
    cdif->cd();
    cdif->cd(1);
    hdiff->Draw();
    gPad->AddExec("exselt","select_hist()");
}

double FitIn2Sigma(TH1F *h)
{
  double mean, sigm, sigmw = 0., lev1 = 1.75, lev2 = 3.;
  double total = 0, peak = 0;
  int mib, mab, nbi;

  if ((total = h->GetEntries()) < 5.)
    {
      printf("Number of histogram '%s' entries is too few!\n", h->GetTitle());
      return 0.;
    }
  int nonzero = 0;
  for (int i=1; i<h->GetNbinsX(); i++)
    if (h->GetBinContent(i)>0) nonzero++;
  if (nonzero < 3)
    {
      if (nonzero < 2)
      {
        printf("Number of non-empty bins is too few in histogram '%s'!\n", h->GetTitle());
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
//  mean = h->GetMean();
  mean = h->GetBinCenter(h->GetMaximumBin());
//  if (fabs(mean) > 1.) mean = 0.;
//  mean = 0.;
  sigm = h->GetRMS();
//  if (sigm > 1.) sigm = 1.;
//  sigm = 0.5;
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
    h->Fit("gaus", "Q0I", "", mean-lev1*sigm, mean+lev1*sigm);
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

double FitIn(TH1F *h, Double_t xmin, Double_t xmax)
{
  double mean, sigm;
  double total = 0, peak = 0;
  int mib, mab, nbi;

  if ((total = h->GetEntries()) < 5.)
    {
      printf("Number of histogram '%s' entries is too few (%d)!\n", h->GetTitle(),h->GetEntries());
      return 0.;
    }
  int nonzero = 0;
  for (int i=1; i<h->GetNbinsX(); i++)
    if (h->GetBinContent(i)>0) nonzero++;
  if (nonzero <= 3)
    {
        printf("Number of histogram '%s' non-zero bins is too few (%d)!\n", h->GetTitle(),nonzero);
        return 0.;
    }
  TF1 *ff = new TF1("ff", "gaus", xmin, xmax);
  h->Fit("ff", "QIRM");
//  h->Fit("gaus", "Q0I");
//  ff = h->GetFunction("ff"); 
  mean = ff->GetParameter(1);
  sigm = ff->GetParameter(2);
//  ff->ResetBit(TF1::kNotDraw);
  mib = h->FindBin(mean-sigm);
  mab = h->FindBin(mean+sigm);
  nbi = h->GetNbinsX();
  peak = h->Integral(mib < 1 ? 1 : mib, mab > nbi ? nbi : mab);
  h->SetAxisRange(mean-5.*sigm, mean+5.*sigm);
  return total > 0. ? peak/total*100. : 0.;
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
