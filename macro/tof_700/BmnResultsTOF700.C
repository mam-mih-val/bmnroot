#include "../../bmndata/BmnEnums.h"
#include "../run/bmnloadlibs.C"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TChain.h"
#include "TClonesArray.h"
#include <vector>
#include "TVector3.h"
#include "TFile.h"
#include "TGeoManager.h"

#define NUMBER_CHAMBERS 59
#define IN_WIDTH_LIMITS 0
#define WIDTH_MAX 7000

// No stack chambers
//#define DIFF_CHAMB_1 -1
//#define DIFF_CHAMB_2 -1

// Stack chambers for run period 6
#define DIFF_CHAMB_1 9
#define DIFF_CHAMB_2 18

#define STRIP_1 0
#define STRIP_2 31
#define STRIP_1_2 0
#define STRIP_2_2 31

//#define STRIP_1 21
//#define STRIP_2 23
//#define STRIP_1_2 21
//#define STRIP_2_2 23

//#define STRIP_1 21
//#define STRIP_2 21
//#define STRIP_1_2 21
//#define STRIP_2_2 21

int champos[NUMBER_CHAMBERS] = {0};
int wmi[NUMBER_CHAMBERS] = {2800};
int wma[NUMBER_CHAMBERS] = {7000};

float idchambers[59] = {27.1,28.1,3.1,1.1,29.1,4.1,33.1,30.1,5.1,19.3,31.1,6.1,2.1,32.1,15.2,16.2,17.2,
18.2,19.2,20.2,7.1,115.2,113.1,117.1,35.1,9.1,37.1,11.1,39.1,13.1,34.1,8.1,36.1,10.1,38.1,12.1,21.2,
23.2,25.2,22.2,24.2,26.2,107.2,108.2,109.2,110.2,111.2,112.2,114.1,116.2,118.1,14.1,40.1,119.2,120.2,
121.2,122.2,123.2,124.2 };

int chamnum_left[30];

int chamnum_right[NUMBER_CHAMBERS-30];

int champos_left[30] = {25,26,11, 9,27,12,31,28,13,23,29,14,10,30,17,18,19,20,21,22,15,16, 8,24, 1, 3, 5,33,35,37};

int champos_right[NUMBER_CHAMBERS-30] = {25, 9, 27,11,29,13,24, 8,26,10,28,12,17,19,21,18,20,22, 1, 3, 5,15,23,31,14,30,33,35,37};

using namespace TMath;

void draw_vs_cham(const char *,TH1F **);
void draw_vs_cham_prof(const char *,TProfile **);
double FitIn(TH1F *h, Double_t xmin, Double_t xmax);

class BmnTof2Raw2DigitNew;

BmnTof2Raw2DigitNew *TOF2 = NULL;


void BmnResultsTOF700(char *fname = "bmn_run9649_digi.root") {

#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs(); // load BmnRoot libraries

    const char *bname = gSystem->BaseName(fname);
    char runfile[128];
    strcpy(runfile,bname);
    char *cdigi = strstr(runfile,"digi");
    *cdigi ='\0';
    strcat(runfile,"raw.root");
    printf("Decoded raw ROOT file name is %s\n",runfile);

    TOF2 = new BmnTof2Raw2DigitNew("TOF700_map_period_7.txt", runfile, 0, 0, "TOF700_geometry_run7.txt");

    if (NUMBER_CHAMBERS > 24)
    {
	for (int i = 0; i<NUMBER_CHAMBERS; i++)
	{
	    champos[i] = i;
	    wmi[i] = 3000;
	    wma[i] = 7000;
	}

    }
    for (int i=0; i<30; i++)
    {
        if (i<24)
    	chamnum_left[i] = i;
    	    else if(i<27) // i = 24,25,26 => 42,43,44
    	chamnum_left[i] = i + 18;
    	    else // i = 27,28,29 => 53,54,55
    	chamnum_left[i] = i + 26;
    }
    for (int i=24; i<53; i++) // 24,25,26,...,50,51,52 (29 chambers)
    {
      if (i<42)
        chamnum_right[i-24] = i;
      else if (i<50) // i = 42-49 => 45-52
    	chamnum_right[i-24] = i + 3;
      else // i = 50,51,52 => 56,57,58
    	chamnum_right[i-24] = i + 6;
    }

    TClonesArray *tof700Digits = NULL;
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
    TProfile *timeprof[NUMBER_CHAMBERS] = {0};
    TH1F *hdiff = 0;
    TH2F *htvst = 0;
    TH2F *htvss1 = 0, *htvss2 = 0, *hsvss = 0;
    TH1F *htimemax1 = 0, *htimemax2 = 0;
    TH1F *hxhit = 0;

    TFile *f = NULL;
    if (strlen(fname))
    {
	char fname_root[128], *delim = 0;
	strcpy(fname_root, gSystem->BaseName(fname));
	if ((delim = strrchr(fname_root, (int)'.'))) *delim = '\0';
	strcat(fname_root, "_results.root");
	f = new TFile(fname_root,"RECREATE","Results of BmnTOF700");
    }

    for (int i = 0; i<NUMBER_CHAMBERS; i++)
    {
	sprintf(name,"Time_chamber_%.1f", idchambers[i]);
	sprintf(title,"Time, chamber %.1f", idchambers[i]);
	htime[i] = new TH1F(name, title, 4000, -50, 50);
	sprintf(name,"Width_chamber_%.1f", idchambers[i]);
	sprintf(title,"Width, chamber %.1f", idchambers[i]);
	hwidth[i] = new TH1F(name, title, 2500, 0, 5000);
	sprintf(name,"LR-Diff_chamber_%.1f", idchambers[i]);
	sprintf(title,"LR-Diff, chamber %.1f", idchambers[i]);
	hlrdiff[i] = new TH1F(name, title, 200, -5, 5);
	sprintf(name,"Strips_rate_chamber_%.1f", idchambers[i]);
	sprintf(title,"Strips rate, chamber %.1f", idchambers[i]);
	hstrips[i] = new TH1F(name, title, 32, 0, 32);
	sprintf(name,"Time_chamber_%.1f_max_strip", idchambers[i]);
	sprintf(title,"Time, chamber %.1f, max strip", idchambers[i]);
	htimemax[i] = new TH1F(name, title, 4000, -50, 50);
	sprintf(name,"Strips_rate_chamber_%.1f_max_strip", idchambers[i]);
	sprintf(title,"Strips rate, chamber %.1f, max strip", idchambers[i]);
	hstripsmax[i] = new TH1F(name, title, 32, 0, 32);
	sprintf(name,"Time_profile_chamber_%.1f", idchambers[i]);
	sprintf(title,"Time profile, chamber %.1f", idchambers[i]);
	timeprof[i] = new TProfile(name, title, 1000, 0, 5000, -20, +20, "e");
    }

    sprintf(name,"X_coordinate");
    sprintf(title,"X coordinate of TOF700 hit");
    hxhit = new TH1F(name, title, 300, -150, +150);

    sprintf(name,"Time_diff_chambers_%.1f_%.1f", idchambers[DIFF_CHAMB_1], idchambers[DIFF_CHAMB_2]);
    sprintf(title,"Time difference, chambers %.1f and %.1f", idchambers[DIFF_CHAMB_1], idchambers[DIFF_CHAMB_2]);
    hdiff = new TH1F(name, title, 10000, -200, 200);

    sprintf(name,"Time_vs_time_chambers_%.1f_%.1f", idchambers[DIFF_CHAMB_1], idchambers[DIFF_CHAMB_2]);
    sprintf(title,"Time vs Time, chambers %.1f and %.1f", idchambers[DIFF_CHAMB_1], idchambers[DIFF_CHAMB_2]);
    htvst = new TH2F(name, title, 1000, -20, 20, 1000, -20, 20);

    sprintf(name,"Time_chamber_%.1f_max_strip_both", idchambers[DIFF_CHAMB_1]);
    sprintf(title,"Time, chamber %.1f, max strip, both chamber", idchambers[DIFF_CHAMB_1]);
    htimemax1 = new TH1F(name, title, 2000, -50, 50);

    sprintf(name,"Time_chamber_%.1f_max_strip_both", idchambers[DIFF_CHAMB_2]);
    sprintf(title,"Time, chamber %.1f, max strip, both chamber", idchambers[DIFF_CHAMB_2]);
    htimemax2 = new TH1F(name, title, 2000, -50, 50);

    sprintf(name,"Time_vs_strip_chamber_%.1f", idchambers[DIFF_CHAMB_1]);
    sprintf(title,"Time vs strip, chamber %.1f", idchambers[DIFF_CHAMB_1]);
    htvss1 = new TH2F(name, title, 32, 0, 32, 2000, -50, 50);

    sprintf(name,"Time_vs_strip_chamber_%.1f", idchambers[DIFF_CHAMB_2]);
    sprintf(title,"Time vs strip, chamber %.1f", idchambers[DIFF_CHAMB_2]);
    htvss2 = new TH2F(name, title, 32, 0, 32, 2000, -50, 50);

    sprintf(name,"Strip_vs_strip_chambers_%.1f_%.1f _", idchambers[DIFF_CHAMB_1], idchambers[DIFF_CHAMB_2]);
    sprintf(title,"Strip vs strip, chambers %.1f and %.1f", idchambers[DIFF_CHAMB_1], idchambers[DIFF_CHAMB_2]);
    hsvss = new TH2F(name, title, 32, 0, 32, 32, 0, 32);

    Int_t nchamb1 = 0, nchamb2 = 0, nchamb12 = 0, nchamb21 = 0, nchamb12n = 0, nchamb12nn = 0;

    for (Int_t iEv = startEvent; iEv < startEvent + nEvents; iEv++) {
        bmnTree->GetEntry(iEv);

        if (iEv % 10000 == 0)
	{
	    cout << "Event: " << iEv << "/" << startEvent + nEvents << endl;
	}
	float wmax[NUMBER_CHAMBERS] = {0.};
	float tmax[NUMBER_CHAMBERS] = {-10000000.};
	int smax[NUMBER_CHAMBERS] = {-1};
	int smax1[NUMBER_CHAMBERS] = {-1};

	for (int i = 0; i<NUMBER_CHAMBERS; i++)
	{
	    wmax[i] = 0.;
	    tmax[i] = -1000000.;
	    smax[i] = -1;
	    smax1[i] = -1;
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
//	    if (lrdiff < -0.5 || lrdiff > 0.5) continue;
	    hwidth[plane]->Fill(width);
	    smax1[plane] = strip;
	    if (width > wmax[plane])
	    {
		wmax[plane] = width;
		tmax[plane] = time;
		smax[plane] = strip;
	    }
	    if (width > WIDTH_MAX) continue;
	    if (IN_WIDTH_LIMITS) if (width < wmi[plane] || width > wma[plane]) continue;
	    htime[plane]->Fill(time);
	    hstrips[plane]->Fill(strip);
	    hlrdiff[plane]->Fill(lrdiff);
	    hxhit->Fill(TOF2->get_hit_x(plane,strip,lrdiff));
	} // tof700Digits loop

	if (DIFF_CHAMB_1 >= 0) if ((smax1[DIFF_CHAMB_1] >= STRIP_1) && (smax1[DIFF_CHAMB_1] >= STRIP_2))
	{
	    nchamb1++;
	    if ((smax1[DIFF_CHAMB_2] >= 0)) nchamb12++;
	    if ((smax1[DIFF_CHAMB_2-1] >= 0)||(smax1[DIFF_CHAMB_2] >= 0)||(smax1[DIFF_CHAMB_2+1] >= 0)) nchamb12n++;
	    if ((smax1[DIFF_CHAMB_2-1] >= 0)||(smax1[DIFF_CHAMB_2] >= 0)||(smax1[DIFF_CHAMB_2+1] >= 0)||(smax1[8] >= 0)||(smax1[10] >= 0)) nchamb12nn++;
	}
	if (DIFF_CHAMB_1 >= 0) if ((smax1[DIFF_CHAMB_2] >= STRIP_1_2) && (smax1[DIFF_CHAMB_2] >= STRIP_2_2))
	{
	    nchamb2++;
	    if ((smax1[DIFF_CHAMB_1] >= 0)) nchamb21++;
	}
	for (int i=0; i<NUMBER_CHAMBERS; i++)
	{
	    if (smax[i] > -1)
	    {
		timeprof[i]->Fill(wmax[i], tmax[i]);
		if (wmax[i] > WIDTH_MAX) continue;
		if (IN_WIDTH_LIMITS) if (wmax[i] < wmi[i] || wmax[i] > wma[i]) continue;
		htimemax[i]->Fill(tmax[i]);
		hstripsmax[i]->Fill(smax[i]);
	    }
	}
	if (DIFF_CHAMB_1 < 0) continue;
	if (IN_WIDTH_LIMITS) if (wmax[DIFF_CHAMB_1] < wmi[DIFF_CHAMB_1] || wmax[DIFF_CHAMB_1] > wma[DIFF_CHAMB_1]) continue;
	if (IN_WIDTH_LIMITS) if (wmax[DIFF_CHAMB_2] < wmi[DIFF_CHAMB_2] || wmax[DIFF_CHAMB_2] > wma[DIFF_CHAMB_2]) continue;
	if (smax[DIFF_CHAMB_1] >= 0) htvss1->Fill(smax[DIFF_CHAMB_1],tmax[DIFF_CHAMB_1]);
	if (smax[DIFF_CHAMB_2] >= 0) htvss2->Fill(smax[DIFF_CHAMB_2],tmax[DIFF_CHAMB_2]);
	if (smax[DIFF_CHAMB_1] >= 0 && smax[DIFF_CHAMB_2] >= 0) hsvss->Fill(smax[DIFF_CHAMB_2],smax[DIFF_CHAMB_1]);
	if ((smax[DIFF_CHAMB_1] >= STRIP_1) && (smax[DIFF_CHAMB_2] >= STRIP_1_2) && (smax[DIFF_CHAMB_1] <= STRIP_2) && (smax[DIFF_CHAMB_2] <= STRIP_2_2) )
	{
		htvst->Fill(tmax[DIFF_CHAMB_2],tmax[DIFF_CHAMB_1]);
		htimemax1->Fill(tmax[DIFF_CHAMB_1]);
		htimemax2->Fill(tmax[DIFF_CHAMB_2]);
		hdiff->Fill(tmax[DIFF_CHAMB_1]-tmax[DIFF_CHAMB_2]);
	}
    } // event loop

    if (nchamb1 > 0) printf("Efficiency (normalize to chamber %3d) = %.1f %%\n", DIFF_CHAMB_1, (float)nchamb12/nchamb1*100.);
    if (nchamb1 > 0) printf("Efficiency (normalize to chamber %3d) = %.1f %% (3 chambers)\n", DIFF_CHAMB_1, (float)nchamb12n/nchamb1*100.);
    if (nchamb1 > 0) printf("Efficiency (normalize to chamber %3d) = %.1f %% (5 chambers)\n", DIFF_CHAMB_1, (float)nchamb12nn/nchamb1*100.);
    if (nchamb2 > 0) printf("Efficiency (normalize to chamber %3d) = %.1f %%\n", DIFF_CHAMB_2, (float)nchamb21/nchamb1*100.);
    if (DIFF_CHAMB_1 >= 0) FitIn(hdiff, -0.2, +0.2);
    for (int i=0; i<NUMBER_CHAMBERS; i++) FitIn(htime[i], -0.2, +0.2);
    for (int i=0; i<NUMBER_CHAMBERS; i++) FitIn(htimemax[i], -0.2, +0.2);

    TCanvas *cx = new TCanvas("cx", "X-coordinate", 900, 900);
    cx->Divide(1,1);
    cx->cd();
    cx->cd(1);
    hxhit->Draw();

    draw_vs_cham("Time",htime);
    draw_vs_cham("TimeMax",htimemax);
    draw_vs_cham("Rate",hstrips);
    draw_vs_cham("RateMax",hstripsmax);
    draw_vs_cham("Width",hwidth);
    draw_vs_cham("Diff",hlrdiff);
    draw_vs_cham_prof("Profile",timeprof);

    if (DIFF_CHAMB_1 < 0) return;

    TCanvas *cdif = new TCanvas("cdif", "Time Difference", 900, 900);
    cdif->Divide(2,2);
    cdif->cd();
    cdif->cd(1);
    hdiff->Draw();
    gPad->AddExec("exselt","select_hist()");
    cdif->cd(2);
    htimemax1->Draw();
    gPad->AddExec("exselt","select_hist()");
    cdif->cd(3);
    htvst->Draw();
    gPad->AddExec("exselt","select_hist()");
    cdif->cd(4);
    htimemax2->Draw();
    gPad->AddExec("exselt","select_hist()");

    TCanvas *cstr = new TCanvas("cstr", "Time vs Strip", 900, 900);
    cstr->Divide(1,2);
    cstr->cd();
    cstr->cd(1);
    htvss1->Draw();
    gPad->AddExec("exselt","select_hist()");
    cstr->cd(2);
    htvss2->Draw();
    gPad->AddExec("exselt","select_hist()");

    TCanvas *cstrstr = new TCanvas("cstrstr", "Strip vs strip", 900, 900);
    cstrstr->Divide(1,1);
    cstrstr->cd();
    cstrstr->cd(1);
    hsvss->Draw();
    gPad->AddExec("exselt","select_hist()");

    if (f) f->Write();
}

double FitIn(TH1F *h, Double_t xmin, Double_t xmax)
{
  double mean, sigm;
  double total = 0, peak = 0;
  int mib, mab, nbi;

  if ((total = h->GetEntries()) < 5.)
    {
      printf("Number of histogram '%s' entries is too few (%d)!\n", h->GetTitle(),(int)h->GetEntries());
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
  h->Fit("ff", "QIRM0");
//  h->Fit("gaus", "Q0I");
//  ff = h->GetFunction("ff"); 
  mean = ff->GetParameter(1);
  sigm = ff->GetParameter(2);
  ff = h->GetFunction("ff");
  if (ff) ff->ResetBit(TF1::kNotDraw);
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

void draw_vs_cham(const char *name, TH1F **h)
{
  char namet[128], namen[128];
  strcpy(namet,name);
  strcpy(namen,name);
  TCanvas *callr1 = new TCanvas(strcat(namen,"l"), strcat(namet," left side"), 900,700);
  callr1->cd();
  callr1->Divide(8,5);
  for (int i=0; i<30; i++)
  {
      callr1->cd(champos_left[i]+1);
      h[chamnum_left[i]]->Draw();
      gPad->AddExec("exselt","select_hist()");
  }
  strcpy(namet,name);
  strcpy(namen,name);
  TCanvas *callr2 = new TCanvas(strcat(namen,"r"), strcat(namet," right side"), 900,700);
  callr2->cd();
  callr2->Divide(8,5);
  // chambers 24-41, 45-52, 56-58
  for (int i=24; i<53; i++) // 24,25,26,...,50,51,52 (29 chambers)
  {
    callr2->cd(champos_right[i-24]+1);
    h[chamnum_right[i-24]]->Draw();
    gPad->AddExec("exselt","select_hist()");
  }
  return;
}

void draw_vs_cham_prof(const char *name, TProfile **h)
{
  char namet[128], namen[128];
  strcpy(namet,name);
  strcpy(namen,name);
  TCanvas *callr1 = new TCanvas(strcat(namen,"l"), strcat(namet," left side"), 900,700);
  callr1->cd();
  callr1->Divide(8,5);
  for (int i=0; i<30; i++)
  {
      callr1->cd(champos_left[i]+1);
      h[chamnum_left[i]]->Draw();
      gPad->AddExec("exselt","select_hist()");
  }
  strcpy(namet,name);
  strcpy(namen,name);
  TCanvas *callr2 = new TCanvas(strcat(namen,"r"), strcat(namet," right side"), 900,700);
  callr2->cd();
  callr2->Divide(8,5);
  // chambers 24-41, 45-52, 56-58
  for (int i=24; i<53; i++) // 24,25,26,...,50,51,52 (29 chambers)
  {
    callr2->cd(champos_right[i-24]+1);
    h[chamnum_right[i-24]]->Draw();
    gPad->AddExec("exselt","select_hist()");
  }
  return;
}

