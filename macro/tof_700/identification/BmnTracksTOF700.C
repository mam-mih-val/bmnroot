#if (!defined(__CINT__) || defined(__MAKECINT__))

#include "TROOT.h"
#include "TSystem.h"
#include "TMath.h"
#include "TH1F.h"
#include "TF1.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TFile.h"
#include "TArrayS.h"
#include "TString.h"
#include "TLine.h"
#include "TTree.h"
#include "TProfile2D.h"

#include "FairTrackParam.h"
#include "BmnMwpcDigit.h"
#include "BmnTrack.h"
#include "BmnDchDigit.h"
#include "BmnTof2Digit.h"
#include "BmnTof2Raw2DigitNew.h"
#include "BmnIdentifiableTrack.h"

#include <stdlib.h>

#endif

#define WRITE_TO_TREE_ONLY

#define NUMBER_CHAMBERS 24
#define IN_WIDTH_LIMITS 1
#define CHA0 18
#define CHA  18
#define STR1 21
#define STR2 23
#define STR3 22

Float_t halfwidthx[NUMBER_CHAMBERS] = {8.0};
Float_t halfwidthy[NUMBER_CHAMBERS] = {0.5};

#if NUMBER_CHAMBERS == 15
int champos[NUMBER_CHAMBERS] = {5,10,1,6,11,2,7,12,3,8,13,4,9,14,0};
int wmi[NUMBER_CHAMBERS] = {2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500,2500};
int wma[NUMBER_CHAMBERS] = {3300,3300,3300,3300,3300,3300,3300,3300,3300,3300,3300,3300,3300,3300,3300};
#define NDX 5
#define NDY 3
#else
#if NUMBER_CHAMBERS == 24
int champos[NUMBER_CHAMBERS] = {17,18, 3, 1,19, 4,23,20, 5,15,21, 6, 2,22, 9,10,11,12,13,14, 7, 8, 0,16};
int wmi[NUMBER_CHAMBERS] = {2800,2800,2800,2800,2800,2800,2800,2800,2800,2800,2800,2800,2800,2800,2800,2800,2800,2800,2800,2800,2800,2800,2800,2800};
int wma[NUMBER_CHAMBERS] = {3200,3200,3200,3200,3200,3200,3200,3200,3200,3200,3200,3200,3200,3200,3200,3200,3200,3200,3200,3200,3200,3200,3200,3200};
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

int nhi = 0;
int nhit[NUMBER_CHAMBERS] = {0};
int nhits[NUMBER_CHAMBERS][32] = {{0}};
float tof[NUMBER_CHAMBERS][32] = {{0.}};
float tofWidths[NUMBER_CHAMBERS][32] = {{0.}};

using namespace std;
using namespace TMath;

class BmnTof2Raw2DigitNew;

BmnTof2Raw2DigitNew *TOF2 = NULL;

void BmnTracksTOF700(int runId = 1889) {

//    printf("In BmnTracksTOF700!\n");
    char fname[132];
    /* Load basic libraries */
    sprintf(fname, "./bmn_reco_DCH_GEM_run%d.root", runId);
#if defined(__CINT__)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries
#endif

    int RunPeriod = 6;
    char mapping[256];
    bool beamrun = true;
    if (runId != 1889) beamrun = false;
    if (runId == 1397) beamrun = true;
    if(RunPeriod >= 1 && RunPeriod <= 6)
    {
	sprintf(mapping, "TOF700_map_period_%d.txt", RunPeriod);
    }
    else
    {
	printf("Non-existing run period number %d!", RunPeriod);
	return;
    }

    TOF2 = new BmnTof2Raw2DigitNew(mapping, fname, 0, 0, "TOF700_geometry_run6_gem.txt");

    TClonesArray *tof700Digits = NULL, *eventHeader = NULL;
    TClonesArray *tracks = NULL, *mtracks = NULL, *dchits = NULL;
    TChain *bmnTree = new TChain("cbmsim");
    if (bmnTree->Add(fname) == 0)
    {
	    printf("Can't find BMN digits tree in file %s!\n", fname);
	    return;
    }
    else
    {
	bmnTree->SetBranchAddress("EventHeader", &eventHeader);
	bmnTree->SetBranchAddress("TOF700", &tof700Digits);
	bmnTree->SetBranchAddress("BmnDchHit", &dchits);
	bmnTree->SetBranchAddress("DchTracks", &tracks);
	bmnTree->SetBranchAddress("DchMatchedTracks", &mtracks);
    }

    char name[128], title[128];
    Int_t startEvent = 0;
    Int_t nEvents = bmnTree->GetEntries();

    printf("\nNumber of events in tree is %d\n", nEvents);

    gStyle->SetOptFit(111);

    TFile *f = NULL;
    if (strlen(fname))
    {
	char fname_root[128], *delim = 0;
	strcpy(fname_root, gSystem->BaseName(fname));
	if ((delim = strrchr(fname_root, (int)'.'))) *delim = '\0';
	strcat(fname_root, "_results.root");
	f = new TFile(fname_root,"RECREATE","Results of BmnTOF700");
    }
    
    TClonesArray *bmnIdentifiableTracks = new TClonesArray("BmnIdentifiableTrack");
    
    TFile *f_out = new TFile(Form("BmnIdentifiableTracks%d.root", runId), "RECREATE");
    TTree *t_out = new TTree("cbmsim", "test_bmn");
    
    t_out->Branch("BmnIdentifiableTracks", &bmnIdentifiableTracks);

    TH1F *hntr = new TH1F("hntr","Number of tracks", 100,0,100);
    TH1F *hntr0 = new TH1F("hntr0","Number of good tracks", 100,0,100);
    TH2F *hxytr = new TH2F("hxytr","XY tracks", 1000,-100,100,1000,-100,100);
    TH1F *h0xytr = new TH1F("h0xytr","Y tracks", 2000,-100,100);
    TH1F *h0xytr1 = new TH1F("h0xytr1","Y tracks", 200,-10,10);
    TH1F *h1xytr = new TH1F("h1xytr","Y tracks (1 hit in TOF700 chamber)", 2000,-100,100);
    TH1F *h1xytr1 = new TH1F("h1xytr1","Y tracks (1 hit in TOF700 chamber)", 200,-10,10);
    TH1F *hptr = new TH1F("hptr","Momentum of tracks", 1000,-100,100);
    TH1F *hltr = new TH1F("hltr","Lenght of tracks", 200,500,700);
    TH2F *hxytrtof = new TH2F("hxytrtof","XY tracks with TOF700 hits", 1000,-100,100,1000,-100,100);
    TH2F *hxytrtof1 = new TH2F("hxytrtof1","XY tracks with TOF700 hits single strip", 1000,-100,100,1000,-100,100);
    TH2F *hxytrtof2 = new TH2F("hxytrtof2","XY tracks with TOF700 hits single strip", 1000,-100,100,1000,-100,100);
    TH2F *hxytrtof3 = new TH2F("hxytrtof3","XY tracks with TOF700 hits single strip", 1000,-100,100,1000,-100,100);
    TH1F *h1xytrtof1 = new TH1F("h1xytrtof1","Y tracks with TOF700 hits single strip", 2000,-100,100);
    TH1F *h1xytrtof2 = new TH1F("h1xytrtof2","Y tracks with TOF700 hits single strip", 2000,-100,100);
    TH1F *h1xytrtof3 = new TH1F("h1xytrtof3","Y tracks with TOF700 hits single strip", 2000,-100,100);
    TH1F *h1xytrtof31 = new TH1F("h1xytrtof31","Y tracks with TOF700 hits single strip", 200,-10,10);
    TH1F *htmp = new TH1F("htmp","N(1 strip)/N(all)", 2000,-100,100);
    hxytrtof1->SetLineColor(kRed);
    hxytrtof2->SetLineColor(kBlue);
    hxytrtof3->SetLineColor(kGreen);
    hxytrtof1->SetMarkerColor(kRed);
    hxytrtof2->SetMarkerColor(kBlue);
    hxytrtof3->SetMarkerColor(kGreen);
    h1xytrtof1->SetLineColor(kRed);
    h1xytrtof2->SetLineColor(kBlue);
    h1xytrtof3->SetLineColor(kGreen);
    TH1F *htoftr = new TH1F("htoftr","TOF of tracks with TOF700 hits", 400,-10,30);
    TH1F *htof = new TH1F("htof","TOF for hits", 400,-10,30);
    TH1F *hntof = new TH1F("hntof","Number of hits", 100,0,100);
    TH1F *hntof0 = new TH1F("hntof0","Number of good hits", 100,0,100);
    TH2F *hxytof = new TH2F("hxytof","XY TOF700 hits", 1000,-100,100,1000,-100,100);
    TH2F *hpbeta = new TH2F("hpbeta","Beta vs momentum", 200,-100,100,400,0,1);;
    TH2F *hptime = new TH2F("hptime","Time vs momentum", 200,-100,100,200,-5,5);;
    TH1F *hmass = new TH1F("hmass", "Mass", 1000,-50,50);

    TH1F *hstr = new TH1F("hstr","Strips", 32,0,32);
    TH1F *hcha = new TH1F("hcha","Chambers", 32,0,32);
    TH1F *hstrtr = new TH1F("hstrtr","Strips with tracks", 32,0,32);
    TH1F *hchatr = new TH1F("hchatr","Chambers with tracks", 32,0,32);

    for (int i = 0; i<NUMBER_CHAMBERS; i++)
    {
	halfwidthx[i] = 8.0;
	halfwidthy[i] = 0.5;
    }

    for (Int_t iEv = startEvent; iEv < startEvent + nEvents; iEv++) {

//	eventHeader->Clear();
//	tof700Digits->Clear();
//	tracks->Clear();
//	mtracks->Clear();
        bmnIdentifiableTracks->Clear();

        bmnTree->GetEntry(iEv);

        if (iEv % 10000 == 0)
	{
	    cout << "Event: " << iEv << "/" << startEvent + nEvents << endl;
	}
	float wmax[NUMBER_CHAMBERS] = {0.};
	float tmax[NUMBER_CHAMBERS] = {-10000000.};
	int smax[NUMBER_CHAMBERS] = {-1};
	int smax1[NUMBER_CHAMBERS] = {-1};
	int istr[NUMBER_CHAMBERS] = {-1};

	nhi = 0;
	for (int i = 0; i<NUMBER_CHAMBERS; i++)
	{
	    wmax[i] = 0.;
	    tmax[i] = -1000000.;
	    smax[i] = -1;
	    smax1[i] = -1;
	    nhit[i] = 0;
	    for (int j=0; j<32; j++) nhits[i][j] = 0;
	}

	int ntof = tof700Digits->GetEntriesFast();
	hntof->Fill(ntof);
	int ntof0 = 0;
	float x = 0., y = 0., z = 0.;
	for (Int_t iDig = 0; iDig < ntof; ++iDig) {
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
	    smax1[plane] = strip;
	    if (width > wmax[plane])
	    {
		wmax[plane] = width;
		tmax[plane] = time;
		smax[plane] = strip;
	    }
	    if (IN_WIDTH_LIMITS) if (width < wmi[plane] || width > wma[plane]) continue;
	    ntof0++;
//	    htof->Fill(time);
//	    if (CHA >= 0 && plane == CHA && strip == STR3) htof->Fill(time);
	    if ((CHA >= 0 && plane == CHA)||(CHA<0)) htof->Fill(time);
	    nhi++;
	    nhit[plane]++;
	    nhits[plane][strip]++;
	    tof[plane][strip] = time;
            tofWidths[plane][strip] = width;
	    istr[plane] = strip;
	    TOF2->get_strip_xyz(plane, strip, &x, &y, &z);
	    hxytof->Fill(x,y);
	    if ((CHA >= 0 && plane == CHA)||(CHA<0)) hstr->Fill(strip);
	    hcha->Fill(plane);
	} // tof700Digits loop
//	if (CHA >= 0 && nhit[CHA] == 1) hstr->Fill(istr[CHA]);
//	else                            hstr->Fill(istr[CHA]);
	hntof0->Fill(ntof0);

	float xyz[3] = {0.}, cxyz[3] = {0.}, xrpc = 0., yrpc = 0., dz = 100., tx = 0., ty = 0., zcha = 0.;
	float beta = 1., mass = 1., cvel = 29.97925;
	int nh = 0, hchamb[100] = {0}, hstrip[100] = {0};
	int ntr = mtracks->GetEntriesFast();
	int ntr0 = 0;
	hntr->Fill(ntr);
	for (Int_t iTr = 0; iTr < ntr; ++iTr) {
    	    BmnTrack *track = (BmnTrack*) mtracks->At(iTr);
    	    if (track == NULL) continue;
	    ntr0++;
    	    Float_t le = track->GetLength();
//   printf("Length = %f\n", l);
    	    Float_t p = track->GetParamFirst()->GetQp();
//    	    Float_t p = track->GetB()*6.;
	    p = 1./p;
	    hptr->Fill(p); // GeV/c
// for beam run
	    if (beamrun) if (p < 60./6. || p > 67./6.) continue;
	    hltr->Fill(le); // cm
	    // get xyz and cxyz here
    	    xyz[0] = track->GetParamFirst()->GetX();
    	    xyz[1] = track->GetParamFirst()->GetY();
    	    xyz[2] = track->GetParamFirst()->GetZ();
	    tx = track->GetParamFirst()->GetTx();
	    ty = track->GetParamFirst()->GetTy();
	    cxyz[0] = tx/TMath::Sqrt(1.+tx*tx+ty*ty);
	    cxyz[1] = ty/TMath::Sqrt(1.+tx*tx+ty*ty);
	    cxyz[2] = 1./TMath::Sqrt(1.+tx*tx+ty*ty);
	    nh = 0;
	    TOF2->get_track_hits(xyz, cxyz, &nh, hchamb, hstrip);
	    if (nh == 0) hchamb[0] = CHA0;
	    if (hchamb[0] < 0) continue;
	    TOF2->get_chamber_z(hchamb[0], &zcha);
	    dz = zcha - xyz[2];
	    xrpc = xyz[0] + dz*tx;
	    yrpc = xyz[1] + dz*ty;
	    hxytr->Fill(xrpc,yrpc);
	    h0xytr->Fill(yrpc);
	    h0xytr1->Fill(yrpc);
	    //if (CHA >= 0) if (nhit[CHA] > 0) hltr->Fill(le); // cm
	    if (CHA >= 0) if (nhit[CHA] > 0) hxytrtof->Fill(xrpc,yrpc);
	    if (CHA >= 0) if (nhit[CHA] > 0) h1xytr->Fill(yrpc);
	    if (CHA >= 0) if (nhit[CHA] > 0) h1xytr1->Fill(yrpc);
	    if (CHA >= 0) if (nhit[CHA] > 0) if (nhits[CHA][STR1] > 0) hxytrtof1->Fill(xrpc,yrpc);
	    if (CHA >= 0) if (nhit[CHA] > 0) if (nhits[CHA][STR2] > 0) hxytrtof2->Fill(xrpc,yrpc);
	    if (CHA >= 0) if (nhit[CHA] > 0) if (nhits[CHA][STR3] > 0) hxytrtof3->Fill(xrpc,yrpc);
	    if (CHA >= 0) if (nhit[CHA] > 0) if (nhits[CHA][STR1] > 0) h1xytrtof1->Fill(yrpc);
	    if (CHA >= 0) if (nhit[CHA] > 0) if (nhits[CHA][STR2] > 0) h1xytrtof2->Fill(yrpc);
	    if (CHA >= 0) if (nhit[CHA] > 0) if (nhits[CHA][STR3] > 0) h1xytrtof3->Fill(yrpc);
	    if (CHA >= 0) if (nhit[CHA] > 0) if (nhits[CHA][STR3] > 0) h1xytrtof31->Fill(yrpc);
	    for (int ih = 0; ih<nh; ih++)
	    {
		int ic = hchamb[ih];
		int is = hstrip[ih];
//		hstrtr->Fill(is);
//		hchatr->Fill(ic);
//		if (is == 19) hxytrtof1->Fill(xrpc,yrpc);
//		if (is == 20) hxytrtof2->Fill(xrpc,yrpc);
//		if (is == 21) hxytrtof3->Fill(xrpc,yrpc);
//		if (is == 19) h1xytrtof1->Fill(yrpc);
//		if (is == 20) h1xytrtof2->Fill(yrpc);
//		if (is == 21) h1xytrtof3->Fill(yrpc);
		if (nhits[ic][is] > 0)
		{
//		    if (ic == CHA)  if (nhit[CHA] == 1) hstrtr->Fill(is);
		    if (ic == CHA || CHA < 0)  hstrtr->Fill(is);
		    hchatr->Fill(ic);

		    if (CHA < 0) hxytrtof->Fill(xrpc,yrpc);
		    if (CHA < 0) h1xytr->Fill(yrpc);
		    if (CHA < 0) h1xytr1->Fill(yrpc);
		    if (CHA < 0) if (is == STR1) hxytrtof1->Fill(xrpc,yrpc);
		    if (CHA < 0) if (is == STR2) hxytrtof2->Fill(xrpc,yrpc);
		    if (CHA < 0) if (is == STR3) hxytrtof3->Fill(xrpc,yrpc);
		    if (CHA < 0) if (is == STR1) h1xytrtof1->Fill(yrpc);
		    if (CHA < 0) if (is == STR2) h1xytrtof2->Fill(yrpc);
		    if (CHA < 0) if (is == STR3) h1xytrtof3->Fill(yrpc);
		    if (CHA < 0) if (is == STR3) h1xytrtof31->Fill(yrpc);

//		    htoftr->Fill(tof[ic][is]);
//		    if (ic == CHA) if (nhit[CHA] == 1) if (is == STR3) htoftr->Fill(tof[ic][is]);
		    if (ic == CHA || CHA < 0) htoftr->Fill(tof[ic][is]);

		    beta = (le)/((tof[ic][is])+21.628370)/cvel;
//		    beta = (le+0.7)/((tof[ic][is]-0.067)+21.628370)/cvel;
//		    beta = 638.8/(tof[ic][is]+21.628370)/cvel;
//		    beta = 638.8/21.628370/cvel;
//		    beta = le/21.628370/cvel;

//		    p += 0.8;
		    hptime->Fill(p,tof[ic][is]);
		    hpbeta->Fill(p,beta);
		    if (beta < 0.7) continue;
		    if (fabs(p) < 3.0) continue;
		    mass = p*TMath::Sqrt(1./beta/beta-1.);
//		    mass = 64.209812/6.*TMath::Sqrt(1./beta/beta-1.);
//		    mass = 64.209812*TMath::Sqrt(1./beta/beta-1.);
//		    hmass->Fill(mass);
//		    if (ic == CHA) if (nhit[CHA] == 1) if (is == STR3) hmass->Fill(mass);
		    if (ic == CHA || CHA < 0) hmass->Fill(mass);
                    
                    BmnIdentifiableTrack *bmnIdentifiableTrack = new((*bmnIdentifiableTracks)
                            [bmnIdentifiableTracks->GetEntriesFast()])BmnIdentifiableTrack();
                    bmnIdentifiableTrack->SetMassQ(mass);
                    bmnIdentifiableTrack->SetBeta(beta);
                    bmnIdentifiableTrack->SetLength(le);
                    bmnIdentifiableTrack->SetPq(p);
                    bmnIdentifiableTrack->SetTime((tof[ic][is])+21.628370);
                    bmnIdentifiableTrack->SetTOF700Plane(ic);
                    bmnIdentifiableTrack->SetTOF700Strip(is);
                    bmnIdentifiableTrack->SetTOF700Amplitude(tofWidths[ic][is]);
                    bmnIdentifiableTrack->SetTOF700StripHits(nhits[ic][is]);
		}
	    }
	} // tracks loop
	hntr0->Fill(ntr0);
        t_out->Fill();
    } // event loop

    if (f) f->Write();
    
#ifdef WRITE_TO_TREE_ONLY
    f_out->cd();
    t_out->Write();
    f_out->Close();
    
    return;
#endif

    TLine *l = 0;

    Float_t xmin[3], xmax[3], ymin[3], ymax[3], x, y, z;
    TOF2->get_strip_xyz(CHA, STR1, &x, &y, &z);
    xmin[0] = x - halfwidthx[CHA];
    xmax[0] = x + halfwidthx[CHA];
    ymin[0] = y - halfwidthy[CHA];
    ymax[0] = y + halfwidthy[CHA];
//    printf("Chamber %d strip %d xmin = %f xmax = %f ymin = %f ymax = %f\n", CHA, STR1, xmin[0], xmax[0], ymin[0], ymax[0]);
    TOF2->get_strip_xyz(CHA, STR2, &x, &y, &z);
    xmin[1] = x - halfwidthx[CHA];
    xmax[1] = x + halfwidthx[CHA];
    ymin[1] = y - halfwidthy[CHA];
    ymax[1] = y + halfwidthy[CHA];
//    printf("Chamber %d strip %d xmin = %f xmax = %f ymin = %f ymax = %f\n", CHA, STR2, xmin[1], xmax[1], ymin[1], ymax[1]);
    TOF2->get_strip_xyz(CHA, STR3, &x, &y, &z);
    xmin[2] = x - halfwidthx[CHA];
    xmax[2] = x + halfwidthx[CHA];
    ymin[2] = y - halfwidthy[CHA];
    ymax[2] = y + halfwidthy[CHA];
//    printf("Chamber %d strip %d xmin = %f xmax = %f ymin = %f ymax = %f\n", CHA, STR3, xmin[2], xmax[2], ymin[2], ymax[2]);

    TCanvas *ctr = new TCanvas("ctr", "Tracks", 900, 900);
    ctr->Divide(2,2);
    ctr->cd();
	ctr->cd(1);
	hntr0->Draw();
	gPad->AddExec("exselt","select_hist()");
	ctr->cd(2);
	hptr->Draw();
	gPad->AddExec("exselt","select_hist()");
	ctr->cd(3);
	hltr->Draw();
	gPad->AddExec("exselt","select_hist()");
	ctr->cd(4);
	hxytr->Draw();
	gPad->AddExec("exselt","select_hist()");

    TCanvas *ctof = new TCanvas("ctof", "TOF", 900, 900);
    ctof->Divide(2,2);
    ctof->cd();
	ctof->cd(1);
	hntof->Draw();
	gPad->AddExec("exselt","select_hist()");
	ctof->cd(2);
	htof->Draw();
	gPad->AddExec("exselt","select_hist()");
	ctof->cd(3);
	hntof0->Draw();
	gPad->AddExec("exselt","select_hist()");
	ctof->cd(4);
	hxytof->Draw();
	gPad->AddExec("exselt","select_hist()");

    TCanvas *ctoftr = new TCanvas("ctoftr", "TOF & Tracks", 900, 900);
    ctoftr->Divide(2,2);
    ctoftr->cd();
	ctoftr->cd(1);
	htoftr->Draw();
	gPad->AddExec("exselt","select_hist()");
	ctoftr->cd(2);
//	hptime->Draw();
	hxytrtof1->Draw();
	gPad->AddExec("exselt","select_hist()");
	ctoftr->cd(3);
	hpbeta->Draw();
	gPad->AddExec("exselt","select_hist()");
	ctoftr->cd(4);
	hmass->Draw();
	gPad->AddExec("exselt","select_hist()");

    TCanvas *csc = new TCanvas("csc", "Strips & Chambers", 900, 900);
    csc->Divide(2,2);
    csc->cd();
	csc->cd(1);
	hstr->Draw();
	gPad->AddExec("exselt","select_hist()");
	csc->cd(2);
	hcha->Draw();
	gPad->AddExec("exselt","select_hist()");
	csc->cd(3);
	hstrtr->Draw();
	gPad->AddExec("exselt","select_hist()");
	csc->cd(4);
	hchatr->Draw();
	gPad->AddExec("exselt","select_hist()");

    TCanvas *cy = new TCanvas("cy", "Strips profiles", 900, 900);
    cy->Divide(3,2);
    cy->cd();
	cy->cd(1);
	hxytrtof1->Draw();
	gPad->AddExec("exselt","select_hist()");
//	l = new TLine(xmin[0],ymin[0],xmax[0],ymax[0]);
//	l->Draw();
//	l->SetLineColor(kRed);
	cy->cd(2);
	hxytrtof2->Draw();
	gPad->AddExec("exselt","select_hist()");
//	l = new TLine(xmin[0],ymin[0],xmax[0],ymax[0]);
//	l->Draw();
//	l->SetLineColor(kBlue);
	cy->cd(3);
	hxytrtof3->Draw();
	gPad->AddExec("exselt","select_hist()");
//	l = new TLine(xmin[0],ymin[0],xmax[0],ymax[0]);
//	l->Draw();
//	l->SetLineColor(kGreen);
	cy->cd(4);
	h1xytrtof1->Draw();
	gPad->AddExec("exselt","select_hist()");
	Float_t xmi = 0.;
	Float_t xma = h1xytrtof1->GetMaximum();
	l = new TLine(ymin[0],xmi,ymin[0],xma);
	l->Draw("same");
//	l->SetLineColor(kRed);
	l = new TLine(ymax[0],xmi,ymax[0],xma);
	l->Draw("same");
//	l->SetLineColor(kRed);
	cy->cd(5);
	h1xytrtof2->Draw();
	gPad->AddExec("exselt","select_hist()");
	xmi = 0.;
	xma = h1xytrtof2->GetMaximum();
	l = new TLine(ymin[1],xmi,ymin[1],xma);
	l->Draw("same");
//	l->SetLineColor(kBlue);
	l = new TLine(ymax[1],xmi,ymax[1],xma);
	l->Draw("same");
//	l->SetLineColor(kBlue);
	cy->cd(6);
	h1xytrtof3->Draw();
	gPad->AddExec("exselt","select_hist()");
	xmi = 0.;
	xma = h1xytrtof3->GetMaximum();
	l = new TLine(ymin[2],xmi,ymin[2],xma);
	l->Draw("same");
//	l->SetLineColor(kGreen);
	l = new TLine(ymax[2],xmi,ymax[2],xma);
	l->Draw("same");
//	l->SetLineColor(kGreen);

    TCanvas *cyp = new TCanvas("cyp", "Strips profiles", 900, 900);
    cyp->Divide(1,1);
    cyp->cd();
	cyp->cd(1);
	h1xytrtof3->Draw();
	xmi = 0.;
	xma = h1xytrtof3->GetMaximum();
	gPad->AddExec("exselt","select_hist()");
	h1xytrtof1->Draw("same");
	gPad->AddExec("exselt","select_hist()");
	h1xytrtof2->Draw("same");
	gPad->AddExec("exselt","select_hist()");
	l = new TLine(ymin[0],xmi,ymin[0],xma);
	l->Draw();
	l->SetLineColor(kRed);
	l = new TLine(ymax[0],xmi,ymax[0],xma);
	l->Draw();
	l->SetLineColor(kRed);
	l = new TLine(ymin[1],xmi,ymin[1],xma);
	l->Draw();
	l->SetLineColor(kBlue);
	l = new TLine(ymax[1],xmi,ymax[1],xma);
	l->Draw();
	l->SetLineColor(kBlue);
	l = new TLine(ymin[2],xmi,ymin[2],xma);
	l->Draw();
	l->SetLineColor(kGreen);
	l = new TLine(ymax[2],xmi,ymax[2],xma);
	l->Draw();
	l->SetLineColor(kGreen);

    TCanvas *cxyp = new TCanvas("cxyp", "Strips profiles 2D", 900, 900);
    cxyp->Divide(1,1);
    cxyp->cd();
	cxyp->cd(1);
	hxytrtof3->Draw();
	gPad->AddExec("exselt","select_hist()");
	hxytrtof1->Draw("same");
	gPad->AddExec("exselt","select_hist()");
	hxytrtof2->Draw("same");
	gPad->AddExec("exselt","select_hist()");

    TCanvas *cy1 = new TCanvas("cy1", "Strips profiles", 900, 900);
    cy1->Divide(1,1);
    cy1->cd();
	cy1->cd(1);
//	h1xytrtof3->Draw();
//	gPad->AddExec("exselt","select_hist()");
	xmi = 0.;
	xma = h1xytrtof3->GetMaximum();
//	h0xytr->Scale(h1xytrtof3->GetMaximum()/h0xytr->GetMaximum());
	h0xytr->Draw();
	h0xytr->SetLineColor(kRed);
//	h1xytr->Scale(h1xytrtof3->GetMaximum()/h1xytr->GetMaximum());
	h1xytr->Draw("same");
	h1xytr->SetLineColor(kBlue);
	h1xytrtof3->Draw("same");
	gPad->AddExec("exselt","select_hist()");
	l = new TLine(ymin[2],xmi,ymin[2],xma);
	l->Draw("same");
//	l->SetLineColor(kGreen);
	l = new TLine(ymax[2],xmi,ymax[2],xma);
	l->Draw("same");
//	l->SetLineColor(kGreen);

    printf("All tracks               : Y mean %f, Y RMS %f\n", h0xytr1->GetMean(), h0xytr1->GetRMS());
    printf("One TOF700 hit any strip : Y mean %f, Y RMS %f\n", h1xytr1->GetMean(), h1xytr1->GetRMS());
    printf("One TOF700 hit strip 22  : Y mean %f, Y RMS %f\n", h1xytrtof31->GetMean(), h1xytrtof31->GetRMS());

    if (1) return;

    TCanvas *cy2 = new TCanvas("cy2", "Strips profiles", 900, 900);
    cy2->Divide(1,1);
    cy2->cd();
	cy2->cd(1);
//	h1xytrtof3->Draw();
//	gPad->AddExec("exselt","select_hist()");
	h1xytrtof3->Copy(*htmp);
	htmp->Divide(h1xytr);
	htmp->SetLineColor(kRed);
	xmi = 0.;
	xma = htmp->GetMaximum();
	htmp->Draw("H");
	gPad->AddExec("exselt","select_hist()");
	l = new TLine(ymin[2],xmi,ymin[2],xma);
	l->Draw("same");
//	l->SetLineColor(kGreen);
	l = new TLine(ymax[2],xmi,ymax[2],xma);
	l->Draw("same");
//	l->SetLineColor(kGreen);
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
      fg = h->GetFunction("fg"); 
      if (fg) fg->ResetBit(TF1::kNotDraw);
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
      fg = h->GetFunction("fg"); 
      if (fg) fg->ResetBit(TF1::kNotDraw);
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
  if (ff) mean = ff->GetParameter(1);
  if (ff) sigm = ff->GetParameter(2);
  if (mean < min0 || mean > max0)
  {
    h->Fit("gaus", "Q0I", "", mean-lev1*sigm, mean+lev1*sigm);
    ff = h->GetFunction("gaus"); 
    mean = ff->GetParameter(1);
    sigm = ff->GetParameter(2);
  }
  h->Fit("gaus", "Q0I", "", mean-lev1*sigm, mean+lev1*sigm);
  ff = h->GetFunction("gaus"); 
  if (ff) mean = ff->GetParameter(1);
  if (ff) sigm = ff->GetParameter(2);
  h->Fit("gaus", "Q0I", "", mean-lev1*sigm, mean+lev1*sigm);
  ff = h->GetFunction("gaus"); 
  if (ff) mean = ff->GetParameter(1);
  if (ff) sigm = ff->GetParameter(2);
  if (ff) ff->ResetBit(TF1::kNotDraw);
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
      printf("Number of histogram '%s' entries is too few (%f)!\n", h->GetTitle(),h->GetEntries());
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
