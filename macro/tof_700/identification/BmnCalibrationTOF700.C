#include <fstream>
#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "bmndata/BmnEnums.h"
#include "macro/run/bmnloadlibs.C"

// GEMTYPE = 0 - Merts, 1 - Gleb, 2 - Gleb recent all, 3 - Gleb recent filtered
#define GEMTYPE 2
#if GEMTYPE == 0
// New Merts -> Old Gleb
#define XSHIFT -1.9008
#define YSHIFT +5.7794
#define ZSHIFT +5.6300
#define TXSHIFT -0.00643
#define TYSHIFT +0.00054
#define MINGEMNDF 4
#elif GEMTYPE == 1
// New Gleb -> Old Gleb
#define XSHIFT -0.2770
#define YSHIFT -0.5900
#define ZSHIFT +1.1400
#define TXSHIFT 0.0
#define TYSHIFT 0.0
#define MINGEMNDF 3
#elif GEMTYPE == 2
// Recent all Gleb -> Old Gleb
#define XSHIFT -1.0584
#define YSHIFT -1.2412
#define ZSHIFT +2.4400
#define TXSHIFT 0.0
#define TYSHIFT 0.0
#define MINGEMNDF 4
#elif GEMTYPE == 3
// Recent filtered Gleb -> Old Gleb
#define XSHIFT -0.0304
#define YSHIFT -1.0934
#define ZSHIFT +6.2000
#define TXSHIFT 0.0
#define TYSHIFT 0.0
#define MINGEMNDF 4
#endif

#define MINTIME true

#define GEMDEBUG false

#define XYPANIN 0

#define DXGEMDCHMAX 5.
#define DYGEMDCHMAX 3.

#define USE_CLUSTERS 1

#define STRIP_CORRECTIONS 1
#define LIST_STRIP_CORRECTIONS 0

// CALIBRATION - 0 - read corrections from root file *calibration.root, main time peak from TOF700_MC_time_run7.txt
//               1 - no calibration correction on input, calculation and store corrections in *calibration.root file, mean time peak from beam momentum
//               2 - no calibration correction on input, calculation and store corrections in *calibration.root file, mean time peak from TOF700_MC_time_run7.txt
//               3 - no calibration correction on input, calculation and store corrections in *calibration.root file, mean time peak from TOF700_MC_time_run7.txt
#define CALIBRATION 2

// GEMCUTS = 0 - don't use additional cuts on GEM tracks, 1 - use additional cuts
#define GEMCUTS 1

// CVERS=0 -  use last calibration tables, 1 - use *;1 version
#define CVERS 1

// CFACTOR - beta factor for main time peak particles
#define CFACTOR 0.97f

// FLAG -1 - (GEM+DCH1+DCH2), 0 - all, +1 (GEM+DCH1)
#define FLAG 0

// BETAMAX max velocity for proton selection
#define BETAMAX 1.00f

// PMIN min momentum for track selection
#define PMIN 0.5f

// PMAX max momentum for track selection
#define PMAX 5.0f

// PMAX1 max momentum for proton mass limits for calculate corrections
#define PMAX1 3.0f

// PMAX2 max momentum for proton mass limits for resolution hists
#define PMAX2 2.5f

// PRES min momentum for resolution hists
#define PRES 2.0f

// PRES1 another min momentum for resolution hists
#define PRES1 3.0f

// PMAXPI max momentum for pion track selection for calculate corrections
#define PMAXPI 3.0f

// MPIMAX max limit pion mass
#define MPIMAX 0.2f

// MP proton mass
#define MP 0.938f

// MP2 proton mass sqare
#define MP2 (MP*MP)

// pion mass
#define MPI  0.139

// pion mass square
#define MPI2 (MPI*MPI)

// scale factor for min mass proton region selection
#define DMPSCALE1 1.

// scale factor for max mass proton region selection
#define DMPSCALE2 1.

// scale factor for min mass proton region selection for time hist
#define DMPSCALET1 1.

// scale factor for max mass proton region selection for time hist
#define DMPSCALET2 1.5

// scale factor for pion region selection
#define DMPISCALE 1.

// PSCALE - momentum scale factor
//#define PSCALE 0.96f
#define PSCALE 0.99f
//#define PSCALE 1.00f

#define WRITE_CALIBRATION 1

#define XFACTOR 5.0f
//#define DISTMAX 20.0f
//#define DXMAX 5.0f
//#define DYMAX 3.0f
#define DISTMAX 20.0f
#define DXMAX 5.0f
#define DYMAX 3.0f
#define DXMAXPAN 5.0f
#define DYMAXPAN 5.0f

#define TOFMAX 20.0f

#define IN_WIDTH_LIMITS 0
#define WMIN 2900
#define WMAX 9000

#define NUMBER_CHAMBERS 59

Float_t halfwidthx[NUMBER_CHAMBERS] = {8.0};
Float_t halfwidthy[NUMBER_CHAMBERS] = {0.5};

int wmi[NUMBER_CHAMBERS] = {2900};
int wma[NUMBER_CHAMBERS] = {9000};

int nhi = 0;
int nhit[NUMBER_CHAMBERS] = {0};
int nhits[NUMBER_CHAMBERS][32] = {{0}};
float tof[NUMBER_CHAMBERS][32] = {{0.}};
float tofWidths[NUMBER_CHAMBERS][32] = {{0.}};
double xhi[1000], yhi[1000], zhi[1000];
double lrd[1000];
double xhic[1000], yhic[1000], zhic[1000];
using namespace std;
using namespace TMath;

BmnTof2Raw2DigitNew *TOF2 = NULL;

TProfile2D *itcalibr = 0;
TProfile   *itcalibrc = 0;

float idchambers[59] = {27.1,28.1,3.1,1.1,29.1,4.1,33.1,30.1,5.1,19.3,31.1,6.1,2.1,32.1,15.2,16.2,17.2,
18.2,19.2,20.2,7.1,115.2,113.1,117.1,35.1,9.1,37.1,11.1,39.1,13.1,34.1,8.1,36.1,10.1,38.1,12.1,21.2,
23.2,25.2,22.2,24.2,26.2,107.2,108.2,109.2,110.2,111.2,112.2,114.1,116.2,118.1,14.1,40.1,119.2,120.2,
121.2,122.2,123.2,124.2 };

Double_t tofcalmc[NUMBER_CHAMBERS] = {0.f};
Double_t tofcalc[NUMBER_CHAMBERS] = {0.f};
Double_t tofcals[NUMBER_CHAMBERS][32] = {{0.f}};

Int_t readMCTimeFile(const char *MCTimeFile = "TOF700_MC_argon_qgsm_time_run7.txt");

//void BmnIdentificationTOF700(const char *TracksGemDch = "tracks/Allowed4649.root", const char *DigitsTOF700 = "digits/bmn_run9649_digi.root") {

//void BmnCalibrationTOF700(const char *TracksGemDch = "argon_tracks_merts_files.txt", const char *DigitsTOF700 = "argon_tof700_digits_files.txt") {

void BmnCalibrationTOF700(const char *TracksGemDch = "argon_tracks_gleb_newall_files.txt", const char *DigitsTOF700 = "argon_tof700_digits_files.txt") {

    /* Load basic libraries */
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs(); // load BmnRoot libraries

    int RunPeriod = 7;
    char mapping[256];
    if(RunPeriod >= 1 && RunPeriod <= 7)
    {
	sprintf(mapping, "TOF700_map_period_%d.txt", RunPeriod);
    }
    else
    {
	printf("Non-existing run period number %d!", RunPeriod);
	return;
    }

    TClonesArray *eventHeadertof700 = NULL;
    TClonesArray *tof700Digits = NULL, *eventHeader = NULL;
    TClonesArray *tracks = NULL, *mtracks = NULL;
    TString filename;

    TChain *bmnTree = new TChain("bmndata");
    if (strstr(TracksGemDch,".root"))
    {
	bmnTree->Add(TracksGemDch);
    }
    else
    {
	ifstream fin(TracksGemDch);
	if (fin.is_open())
	{
	    for(int idx=0; idx<1000; idx++) {
		if (fin.eof()) break;
		fin >> filename;
		if (strlen(filename.Data())==0) break;
		if ((filename.Data())[0]=='#') continue;
		cout << " GEM+DCH tracks file #" << idx+1 << " " << filename.Data() << endl;
		bmnTree->Add(filename);
	    }
	    fin.close();
	}
	else
	{
	    printf("Non-existing run list file: %s!", TracksGemDch);
	    return;
	}
    }
    bmnTree->SetBranchAddress("EventHeader", &eventHeader);
    bmnTree->SetBranchAddress("GemDchTracks", &mtracks);

    char fnametof700[256];
    TChain *bmnTreetof700 = new TChain("cbmsim");
    if (strstr(DigitsTOF700,".root"))
    {
	bmnTreetof700->Add(DigitsTOF700); //
	strcpy(fnametof700, DigitsTOF700);
    }
    else
    {
	ifstream fin(DigitsTOF700);
	if (fin.is_open())
	{
	    for(int idx=0; idx<1000; idx++) {
		if (fin.eof()) break;
		fin >> filename;
		if (strlen(filename.Data())==0) break;
		if ((filename.Data())[0]=='#') continue;
		cout << " TOF digits file #" << idx+1 << " " << filename.Data() << endl;
		bmnTreetof700->Add(filename);
		strcpy(fnametof700, filename.Data());
	    }
	    fin.close();
	}
	else
	{
	    printf("Non-existing run list file: %s!", DigitsTOF700);
	    return;
	}
    }
    bmnTreetof700->SetBranchAddress("EventHeader", &eventHeadertof700);
    bmnTreetof700->SetBranchAddress("TOF700", &tof700Digits);

    char fnametof700raw[256];
    strcpy(fnametof700raw,fnametof700);
    strcpy(strstr(fnametof700raw,"digi.root"),"raw.root");
    if (XYPANIN)
	TOF2 = new BmnTof2Raw2DigitNew(mapping, fnametof700raw, 0, 0, "TOF700_geometry_run7_panin.txt");
    else
	TOF2 = new BmnTof2Raw2DigitNew(mapping, fnametof700raw, 0, 0, "TOF700_geometry_run7.txt");

    for (int c=0; c<NUMBER_CHAMBERS; c++)
    {
	halfwidthy[c]=0.5f;
	if (idchambers[c] < 100.f)
	{
	    halfwidthx[c]=8.0f;
	}
	else
	{
	    halfwidthx[c]=28.0f;
	}
	wmi[c] = WMIN;
	wma[c] = WMAX;
    }

    char name[128], title[128];

    gStyle->SetOptFit(111);
    gStyle->SetOptStat(111111);

    readMCTimeFile();

    if (CALIBRATION == 0)
    {
	char fname_calib_root[128], *delim = 0;
	strcpy(fname_calib_root, gSystem->BaseName(fnametof700));
	if ((delim = strrchr(fname_calib_root, (int)'.'))) *delim = '\0';
	strcat(fname_calib_root, "_calibration.root");
	TFile *fc = new TFile(fname_calib_root,"READ", "Proton mass based calibration of BmnTOF700");
	if (CVERS == 0)
	{
	    itcalibr = (TProfile2D *)fc->Get("tcalibr");
	    itcalibrc = (TProfile *)fc->Get("tcalibrc");
	}
	else
	{
	    itcalibr = (TProfile2D *)fc->Get("tcalibr;1");
	    itcalibrc = (TProfile *)fc->Get("tcalibrc;1");
	}
	printf("\n ******************* Time offsets for whole chamber **********************\n");
	for (int c=0; c<NUMBER_CHAMBERS; c++) { tofcalc[c] = itcalibrc->GetBinContent(c+1); printf("%d %f\n",c,tofcalc[c]); }
	if (STRIP_CORRECTIONS)
	{
	    if (LIST_STRIP_CORRECTIONS) printf("\n ******************* Time offsets for one strip **************************\n");
	    for (int c=0; c<NUMBER_CHAMBERS; c++)
	    {
		if (LIST_STRIP_CORRECTIONS) printf("\n Chamber  %d %.1f\n",c,idchambers[c]);
		int smax = 32;
		if (idchambers[c] >= 100.f) smax = 16;
		for (int s=0; s<smax; s++)
		{
		    tofcals[c][s] = itcalibr->GetBinContent(c+1,s+1);
		    if (LIST_STRIP_CORRECTIONS) printf("   strip %d %f\n",s,tofcals[c][s]);
		}
		if (LIST_STRIP_CORRECTIONS) printf("\n *************************************************************************\n");
	    }
	}
    }

    TFile *f = NULL;
    if (strlen(fnametof700))
    {
	char fname_root[128], *delim = 0;
	strcpy(fname_root, gSystem->BaseName(fnametof700));
	if ((delim = strrchr(fname_root, (int)'.'))) *delim = '\0';
	strcat(fname_root, "_calibration_results.root");
	f = new TFile(fname_root,"RECREATE","Results of BmnTOF700");
    }

    TH1F *hntr = new TH1F("hntr","Number of all tracks", 200,0,200);
    TH1F *hntr0 = new TH1F("hntr0","Number of selected tracks", 200,0,200);
    TH1F *hntr1 = new TH1F("hntr1","Number of tracks with FLAG conditions", 200,0,200);
    TH2F *hxytr = new TH2F("hxytr","XY tracks", 1500,-150,150,1000,-100,100);
    TH1F *hptr = new TH1F("hptr","Momentum of tracks", 1000,-10,10);
    TH1F *hp0  = new TH1F("hp0","Momentum of tracks (all)", 1000,-10,10);
    TH1F *hltr = new TH1F("hltr","Lenght of GEM tracks", 100,500,700);
    TH1F *hltra = new TH1F("hltra","Lenght of GEM+DCH tracks", 100,500,700);
    TH2F *hxytrtof = new TH2F("hxytrtof","XY tracks with TOF700 hits", 1500,-150,150,1000,-100,100);
    TH2F *hxyhit = new TH2F("hxyhit","XY track hit distance from TOF700 hit", 100,-50,50,40,-10,10);
    TH1F *hlrdiffs = new TH1F("hlrdiffs","LR difference for TOF700 hits with track (small chamber)", 100,-5,+5);
    TH1F *hlrdiffb = new TH1F("hlrdiffb","LR difference for TOF700 hits with track (big chamber)", 100,-5,+5);
    TH1F *hxdiff = new TH1F("hxdiff","X difference for TOF700 hits with track", 100,-50,+50);
    TH2F *hxxhits = new TH2F("hxxhits","X track hit vs TOF700 hit (small chambers)", 200,-100,100,200,-100,100);
    TH2F *hxxhitb = new TH2F("hxxhitb","X track hit vs TOF700 hit (big chambers)", 200,-100,100,200,-100,100);
    TH2F *hlrxhits = new TH2F("hlrxhits","X TOF700 hit vs LR difference (small chamber)", 100,-5,5,200,-100,100);
    TH2F *hlrxhitb = new TH2F("hlrxhitb","X TOF700 hit vs LR difference (big chamber)", 100,-5,5,200,-100,100);

    TH2F *hlrxtrs = new TH2F("hlrxtrs","X TOF700 tr vs LR difference (small chamber)", 100,-5,5,200,-100,100);
    TH2F *hlrxtrb = new TH2F("hlrxtrb","X TOF700 tr vs LR difference (big chamber)", 100,-5,5,200,-100,100);

    TH1F *hcsignp = new TH1F("hsignp","Chambers with right correlation (x vs lr)",60,0,60);
    TH1F *hcsignm = new TH1F("hsignm","Chambers with wrong correlation (x vs lr)",60,0,60);

    TH1F *htoftr = new TH1F("htoftr","TOF of tracks with TOF700 hits", 600,-20,40);
    TH1F *htof = new TH1F("htof","TOF for hits", 600,-20,40);
    TH1F *hntof = new TH1F("hntof","Number of hits", 100,0,100);
    TH1F *hnhit = new TH1F("hnhit","Number of hits in width limits", 100,0,100);
    TH2F *hxytof = new TH2F("hxytof","XY TOF700 hits", 1500,-150,150,1000,-100,100);
    TH2F *hpbeta = new TH2F("hpbeta","Beta vs momentum", 200,0,10,220,0,1.1f);
    hpbeta->SetOption("COLZ");
    TH2F *hpbetac = new TH2F("hpbetac","Beta vs momentum, chamber corrections", 200,0,10,220,0,1.1f);
    hpbetac->SetOption("COLZ");
    TH2F *hpbetas = new TH2F("hpbetas","Beta vs momentum, strip corrections", 200,0,10,220,0,1.1f);
    hpbetas->SetOption("COLZ");
    TH2F *hpbetacm = new TH2F("hpbetacm","Beta vs momentum, Z<0, chamber corrections", 200,0,10,220,0,1.1f);
    hpbetacm->SetOption("COLZ");
    TH2F *hpbetasm = new TH2F("hpbetasm","Beta vs momentum, Z<0, strip corrections", 200,0,10,220,0,1.1f);
    hpbetasm->SetOption("COLZ");
    TH1F *hmass = new TH1F("hmass", "Mass, p<2", 100,-0.5,4.5);
    TH1F *hmassc = new TH1F("hmassc", "Mass, chamber corrections", 100,-0.5,4.5);
    TH1F *hmassc15 = new TH1F("hmassc15", "Mass, chamber corrections, p<1.5", 100,-0.5,4.5);
    TH1F *hmassc20 = new TH1F("hmassc20", "Mass, chamber corrections, p<2.0", 100,-0.5,4.5);
    TH1F *hmasss = new TH1F("hmasss", "Mass, strip corrections", 100,-0.5,4.5);
    TH1F *hmasss15 = new TH1F("hmasss15", "Mass, strip corrections, p<1.5", 100,-0.5,4.5);
    TH1F *hmasss20 = new TH1F("hmasss20", "Mass, strip corrections, p<2.0", 100,-0.5,4.5);
    TH1F *hmasscm = new TH1F("hmasscm", "Mass, Z<0, chamber corrections", 200,-1,4);
    TH1F *hmasssm = new TH1F("hmasssm", "Mass, Z<0, strip corrections", 200,-1,4);

    TH1F *hdtps = new TH1F("hdtps", "Time resolution, protons, strip corrections", 200,-1.,+1.);
    TH2F *hdtpsvsp = new TH2F("hdtpsvsp", "Time resolution vs momentum, protons, strip corrections", 240,0.,6.,200,-1.,+1.);
    hdtpsvsp->SetOption("COLZ");
    TH1F *hdtpc = new TH1F("hdtpc", "Time resolution, protons, chamber corrections", 200,-1.,+1.);
    TH2F *hdtpcvsp = new TH2F("hdtpcvsp", "Time resolution vs momentum, protons, chamber corrections", 240,0.,6.,200,-1.,+1.);
    hdtpcvsp->SetOption("COLZ");
    TH2F *hdtpcvspwlim = new TH2F("hdtpcvspwlim", "Time resolution vs momentum, protons, chamber corrections, w>3000", 240,0.,6.,200,-1.,+1.);
    hdtpcvspwlim->SetOption("COLZ");
    TH1F *hdtpcplim = new TH1F("hdtpcplim", "Time resolution, protons, p>3", 200,-1.,+1.);
    TH1F *hdtpcp1lim = new TH1F("hdtpcp1lim", "Time resolution, protons, p>4", 200,-1.,+1.);
    TH1F *hdtpcp2lim = new TH1F("hdtpcp2lim", "Time resolution, protons, p<2", 200,-1.,+1.);
    TH1F *hdtpcp3lim = new TH1F("hdtpcp3lim", "Time resolution, protons, 2<p<5", 200,-1.,+1.);
    TH1F *hdtpcwlim = new TH1F("hdtpcwlim", "Time resolution, protons, w>3000", 200,-1.,+1.);
    TH1F *hdtpcwplim = new TH1F("hdtpcwplim", "Time resolution, protons, p>3, w>3000", 200,-1.,+1.);
    TH1F *hdtpcwp1lim = new TH1F("hdtpcwp1lim", "Time resolution, protons, p>4, w>3000", 200,-1.,+1.);

    TH2F *hmassvsp = new TH2F("hmassvsp", "Mass vs momentum", 240,0.,6.,200,-0.2,4.8);
    hmassvsp->SetOption("COLZ");
    TH2F *hmasssvsp = new TH2F("hmasssvsp", "Mass vs momentum, strip correction", 240,0.,6.,200,-0.2,4.8);
    hmasssvsp->SetOption("COLZ");
    TH2F *hmasscvsp = new TH2F("hmasscvsp", "Mass vs momentum, chamber corrections", 240,0.,6.,200,-0.2,4.8);
    hmasscvsp->SetOption("COLZ");
    TH2F *hmasssmvsp = new TH2F("hmasssmvsp", "Mass vs momentum, Z<0, strip correction", 240,0.,6.,200,-0.2,4.8);
    hmasssmvsp->SetOption("COLZ");
    TH2F *hmasscmvsp = new TH2F("hmasscmvsp", "Mass vs momentum, Z<0, chamber corrections", 240,0.,6.,200,-0.2,4.8);
    hmasscmvsp->SetOption("COLZ");

    TH1F *htofp= new TH1F("htofp","TOF for proton hits, p<2", 600,-20,40);
    TH1F *hstr = new TH1F("hstr","Strips", 32,0,32);
    TH1F *hcha = new TH1F("hcha","Chambers", 60,0,60);
    TH1F *hstrtr = new TH1F("hstrtr","Strips with tracks", 32,0,32);
    TH1F *hchatr = new TH1F("hchatr","Chambers with tracks", 60,0,60);

    TH2F *hdxy0 = new TH2F("hdxy0","DxDy all",40,-10,+10,40,-10,+10);
    TH1F *hdx = new TH1F("hdx","dx all",40,-10,+10);
    TH1F *hdy = new TH1F("hdy","dy all",40,-10,+10);
    TH2F *hdxy = new TH2F("hdxy","DxDy selected",40,-10,+10,40,-10,+10);
    TH2F *hdxytof = new TH2F("hdxytof","DxDy with TOF700 hits",40,-10,+10,40,-10,+10);
    TH2F *hdxyeff = new TH2F("hdxyeff","Efficiency vs DxDy",40,-10,+10,40,-10,+10);
    hdxyeff->SetOption("COLZ");
    TH1F *hdxeff = new TH1F("hdxeff","Efficiency vs Dx",40,-10,+10);
    TH1F *hdyeff = new TH1F("hdyeff","Efficiency vs Dy",40,-10,+10);
    TH2F *hxygoodtr = new TH2F("hxygoodtr","XY good tracks", 70,-140,140,60,-90,90);
    TH2F *hxygoodhit = new TH2F("hxygoodhit","XY good tracks with good hits", 70,-140,140,60,-90,90);
    TH2F *hxyeff = new TH2F("hxyeff","Efficiency vs XY good tracks", 70,-140,140,60,-90,90);
    hxyeff->SetOption("COLZ");
    TH1F *hxeff = new TH1F("hxeff","Efficiency vs X good tracks", 70,-140,140);
    TH1F *hyeff = new TH1F("hyeff","Efficiency vs Y good tracks", 60,-90,90);
    TH1F *hdtx = new TH1F("hdtx","dtx",100,-0.25,+0.25);
    TH1F *hdty = new TH1F("hdty","dty",100,-0.25,+0.25);
    TH1F *hDdx = new TH1F("hDdx","Err dx",100,0,10);
    TH1F *hDdy = new TH1F("hDdy","Err dy",100,0,10);
    TH1F *hDdtx = new TH1F("hDdtx","Err dtx",100,0,0.05);
    TH1F *hDdty = new TH1F("hDdty","Err dty",100,0,0.05);
    TH1F *hpdx = new TH1F("hpdx","Pull dx",500,-2.5,+2.5);
    TH1F *hpdy = new TH1F("hpdy","Pull dy",500,-2.5,+2.5);
    TH1F *hpdtx = new TH1F("hpdtx","Pull dtx",500,-25.,+25.);
    TH1F *hpdty = new TH1F("hpdty","Pull dty",500,-25.,+25.);
    TH1F *hchi2m = new TH1F("hChi2m","Chi2 GemDch track",1000,0,100);
    TH1F *hchi2 = new TH1F("hchi2","Chi2 tracks matching",1000,0,100);

    TProfile2D *tcalibrs = new TProfile2D("tcalibr","Time calibration based on proton mass, strips",NUMBER_CHAMBERS,0,NUMBER_CHAMBERS,32,0,32,-1.,+1.,"e");
    TProfile *tcalibrc = new TProfile("tcalibrc","Time calibration based on proton mass, chambers",NUMBER_CHAMBERS,0,NUMBER_CHAMBERS,-1.,+1.,"e");

    TProfile *tpeak = new TProfile("tpeak","Peak time vs chamber",NUMBER_CHAMBERS,0,NUMBER_CHAMBERS,0.,+30.,"e");
    TProfile *bpeak = new TProfile("bpeak","Peak velosity vs chamber",NUMBER_CHAMBERS,0,NUMBER_CHAMBERS,0.,+1.5,"e");

    char nam[64],tit[64];

    float cvel = 29.97925f;
    float ekin = 3.2f, ebeam = ekin + MP, pbeam = TMath::Sqrt(ebeam*ebeam+MP*MP);
    float cbeam = pbeam/ebeam;
    float cpeak = cbeam*CFACTOR*cvel;
    float ztarg = -2.2f;
    float zc[NUMBER_CHAMBERS] = {0.f};
    for (int i = 0; i<NUMBER_CHAMBERS; i++)
    {
	TOF2->get_chamber_z(i,&zc[i]);
    }

    Int_t startEvent = 0;
    Int_t nEvents = bmnTree->GetEntries();
    Int_t nEventstof700 = bmnTreetof700->GetEntries();
    printf("\nNumber of events in tracks chain is %d (tof700 chain %d)\n", nEvents, nEventstof700);

    int Ntr = 0, Ntr1 = 0, Ntr1g = 0, Ntr0 = 0;
    Int_t iEv = 0, iEvtof700 = 0, fCurrent = -1;
    UInt_t trackEvId = 0, tof700EvId = 0;
    UInt_t trackRunId = 0, tof700RunId = 0;
    for (Int_t iEv0 = startEvent; iEv0 < startEvent + nEvents; iEv0++)
    {
        if (iEv % 10000 == 0)
	{
	    cout << "Event: " << iEv << "/" << startEvent + nEvents << " ( Run " << trackRunId << " EvId " << trackEvId << " )" << endl;
	}
    	BmnGemDchTrack *gemdchtrack0 = NULL;
	do
	{
	    if (iEv >= nEvents) break;
	    bmnTree->GetEntry(iEv);
	    if (mtracks == NULL) { iEv++; continue; };
    	    gemdchtrack0 = (BmnGemDchTrack*)(mtracks->UncheckedAt(0));
//    	    gemdchtrack0 = (BmnGemDchTrack*)(mtracks->At(0));
//	    printf("First attempt, iEv = %d, iEv0 = %d\n",iEv,iEv0);
	    iEv++;
	}
	while (gemdchtrack0 == NULL);
//	printf("Point 0, iEv = %d\n",iEv);
	if (iEv >= nEvents) break;
//	trackEvId = gemdchtrack0->GetEventId();
	trackEvId = ((BmnEventHeader*)eventHeader)->GetEventId();
//	printf("Point 1, iEv = %d, trackEvId = %d\n",iEv,trackEvId);
	if (eventHeader == NULL) continue;
//	trackRunId = ((BmnEventHeader*)eventHeader->UncheckedAt(0))->GetRunId();
	trackRunId = ((BmnEventHeader*)eventHeader)->GetRunId();
//	printf("Point 2, iEv = %d, trackRunId = %d\n",iEv,trackRunId);
	if (tof700RunId == trackRunId && tof700EvId > trackEvId) continue;
	if (tof700RunId > trackRunId) continue;
//	printf("Point 3, iEv = %d\n",iEv);
	if (!(tof700EvId==trackEvId && tof700RunId==trackRunId))
	{
          do
	  {
	    bmnTreetof700->GetEntry(iEvtof700);
	    if (eventHeadertof700 == NULL) break;
	    tof700EvId = ((BmnEventHeader*) eventHeadertof700->UncheckedAt(0))->GetEventId();
	    tof700RunId = ((BmnEventHeader*) eventHeadertof700->UncheckedAt(0))->GetRunId();
//	    cout << "Tracks Event:   " << iEv << ", TOF700 Event: " << iEvtof700 << endl;
//	    cout << "Tracks EventId: " << trackEvId << ", TOF700 EventId: " << tof700EvId << endl;
//	    cout << "Tracks RunId: " << trackRunId << ", TOF700 RunId: " << tof700RunId << endl;
	    iEvtof700++;
	    if (iEvtof700 >= nEventstof700) break;
	    if (tof700RunId > trackRunId) break;
	    if (tof700RunId == trackRunId && tof700EvId > trackEvId) break;
	  }
	  while (!(tof700EvId==trackEvId && tof700RunId==trackRunId));
	}
	if (eventHeadertof700 == NULL) continue;
        if (!(tof700EvId==trackEvId && tof700RunId==trackRunId))
	{
	    cout << "Not found EventId " << trackEvId << " in tof700 digits file" << endl;
	    cout << "Tracks Event:   " << iEv << ", TOF700 Event: " << iEvtof700 << endl;
	    cout << "Tracks RunId:   " << trackRunId << ", TOF700 RunId:   " << tof700RunId << endl;
	    cout << "Tracks EventId: " << trackEvId <<  ", TOF700 EventId: " << tof700EvId << endl;
	    if (iEvtof700 >= nEventstof700) break;
	    if (tof700RunId > trackRunId) continue;
	    if (tof700RunId == trackRunId && tof700EvId > trackEvId) continue;
	    break;
//	    continue;
	}
	float wmax[NUMBER_CHAMBERS] = {0.};
	float tmax[NUMBER_CHAMBERS] = {-10000000.};
	int smax[NUMBER_CHAMBERS] = {-1};
	int smax1[NUMBER_CHAMBERS] = {-1};
	int istr[NUMBER_CHAMBERS] = {-1};

	for (int i = 0; i<NUMBER_CHAMBERS; i++)
	{
	    wmax[i] = 0.;
	    tmax[i] = -1000000.;
	    smax[i] = -1;
	    smax1[i] = -1;
	    nhit[i] = 0;
	    for (int j=0; j<32; j++) nhits[i][j] = 0;
	    for (int j=0; j<32; j++) tofWidths[i][j] = 0.;
	    for (int j=0; j<32; j++) tof[i][j] = 0.;
	}
	nhi = 0;

	int ntof = tof700Digits->GetEntriesFast();
	hntof->Fill(ntof);
	float x = 0., y = 0., z = 0.;
	int hchamb[1000] = {0}, hstrip[1000] = {0};
	int clchamb[1000] = {0}, clstrip[1000] = {0};
	Bool_t usedstrip[1000] = {false};
	Int_t numhit[NUMBER_CHAMBERS][32] = {{0}};
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
	    if (XYPANIN && (TOF2->get_hit_xp(plane, strip, lrdiff) == 0.)) continue;
//	    if (lrdiff < -0.5 || lrdiff > 0.5) continue;
	    smax1[plane] = strip;
	    if (width > wmax[plane])
	    {
		wmax[plane] = width;
		tmax[plane] = time;
		smax[plane] = strip;
	    }
	    if (IN_WIDTH_LIMITS) if (width < wmi[plane] || width > wma[plane]) continue;
	    htof->Fill(time);
	    hchamb[nhi] = plane;
	    hstrip[nhi] = strip;
	    nhit[plane]++;
	    nhits[plane][strip]++;
	    tof[plane][strip] = time;
            tofWidths[plane][strip] = width;
	    istr[plane] = strip;
	    if (XYPANIN)
		TOF2->get_hit_xyzp(plane, strip, lrdiff, &x, &y, &z);
	    else
		TOF2->get_hit_xyz(plane, strip, lrdiff, &x, &y, &z);
	    xhi[nhi] = x;
	    yhi[nhi] = y;
	    zhi[nhi] = z;
	    lrd[nhi] = lrdiff;
	    hxytof->Fill(x,y);
	    TOF2->get_strip_xyz(plane, strip, &x, &y, &z);
	    xhic[nhi] = x;
	    yhic[nhi] = y;
	    zhic[nhi] = z;
	    hstr->Fill(strip);
	    hcha->Fill(plane);
	    numhit[plane][strip] = nhi;
	    nhi++;
	} // tof700Digits loop
	hnhit->Fill(nhi);

	float xyz[3] = {0.}, cxyz[3] = {0.}, xrpc = 0., yrpc = 0., dz = 100., tx = 0., ty = 0., zcha = 0.;
	float beta = 1.0f, mass = 1.0f, mass2 = 1.0f, tcor = 0., tcorc = 0., tcors = 0., tcorspi = 0., tcorcpi = 0.;
	int nh = 0;
	int ntr = mtracks->GetEntriesFast();
	hntr->Fill(ntr);
	Ntr+=ntr;
	int ntr0 = 0;
	int ntr1 = 0, ntr1g = 0;
	int iflag = 0, igem = 0, idch = 0;
	float chi2 = 0.f;
    	BmnGemDchTrack *gemdchtrack = NULL;
    	BmnTrack *track = NULL;
#if GEMTYPE == 0
    	BmnTrack *gtrack = NULL; // Merts GEM tracks
#else
    	CbmStsTrack *gtrack = NULL; // Gleb GEM tracks
#endif
        Double_t xGem = 0.;
        Double_t yGem = 0.;
        Double_t txGem = 0.;
        Double_t tyGem = 0.;
        Double_t QpGem = 0.;
        Double_t QpDch = 0.;
        Int_t nGemHits = 0;
	Double_t Xpv = 0.;
	Double_t Ypv = 0.;
	Double_t dxm = 0.;
	Double_t dym = 0.;
	Double_t dtx = 0.;
	Double_t dty = 0.;
	Double_t Ddx = 0.;
	Double_t Ddy = 0.;
	Double_t Ddtx = 0.;
	Double_t Ddty = 0.;
	Double_t Chi2m = 0.;
        Double_t xgoodtr = 0.;
        Double_t ygoodtr = 0.;
	Double_t le = 0., p = 0., ps = 0., pmod = 0., lenr = 0.;
	int himin = -1;
	double dx, dy, dl;
	double dist = 0., distmin = 1000000000.0f;
	Float_t xc,yc,zc;
	int ic;
	int is;
	double cosa = 0.;
	float hidist = 0.;
	Double_t tofr0 = 0.f;
	Double_t tofr = 0.f;
	Double_t tofp = 0.f;
	Double_t tofpi = 0.f;
        float massmin1 = 0.;
        float massmax1 = 0.;
        float massmin2 = 0.;
        float massmax2 = 0.;

        float massmaxpi = 0.;
        float massminpi = 0.;
	float tcorpi = 0.;
        float tofrc = 0.;
        float tofrs = 0.;
        float betac = 0.;
        float betas = 0.;
	float masss2 = 0.;
	float masss  = 0.;
	float massc2 = 0.;
	float massc  = 0.;
	double timepeak = 0., betapeak = 0.;

	for (Int_t iTr = 0; iTr < ntr; ++iTr) {
    	    gemdchtrack = (BmnGemDchTrack*) mtracks->At(iTr);
    	    if (gemdchtrack == NULL) continue;
    	    track = (BmnTrack*) gemdchtrack->GetDchTrack();
    	    if (track == NULL) continue;
	    if (track->GetUniqueID() != 100) continue;
    	    if (FLAG < 0 && track->GetFlag() >= 0) continue;
    	    if (FLAG > 0 && track->GetFlag() <  0) continue;
#if GEMTYPE == 0
    	    gtrack = (BmnTrack *) gemdchtrack->GetGemTrack();  // Merts GEM tracks
#else
    	    gtrack = (CbmStsTrack *) gemdchtrack->GetGemTrack(); // Gleb GEM tracks
#endif
    	    if (gtrack == NULL) continue;
// Selection criteria
            QpGem = gtrack->GetParamLast()->GetQp();
            QpDch = track->GetParamLast()->GetQp();
	    if (TMath::IsNaN(QpGem)) { /*printf("NaN of INF inv. momentum %f, momentum %f\n",ps,p);*/ continue; }
	    if (TMath::IsNaN(QpDch)) { /*printf("NaN of INF inv. momentum %f, momentum %f\n",ps,p);*/ continue; }
	    if (QpGem != 0.)
	    {
		hp0->Fill(1./QpGem);
	    }
	    else
	    {
//		hp0->Fill(0.);
	    }
	    ntr1++;
            xGem = gtrack->GetParamLast()->GetX();
            yGem = gtrack->GetParamLast()->GetY();
            txGem = gtrack->GetParamLast()->GetTx();
            tyGem = gtrack->GetParamLast()->GetTy();
#if GEMTYPE == 0
            nGemHits = gtrack->GetNHits(); // Merts GEM tracks
#else
            nGemHits = gtrack->GetNDF(); // Gleb GEM tracks
#endif
	    Xpv = gemdchtrack->GetXpv();
	    Ypv = gemdchtrack->GetYpv();

            if (GEMCUTS) if (nGemHits < MINGEMNDF || \
	        xGem  < -140.f || xGem > 240.f || yGem < -40.f  || yGem > 200.f || \
		txGem < -0.5f  || txGem > 1.0f || tyGem < -0.1f || tyGem > 0.5f || \
		Xpv   < -3.5f  || Xpv   > 4.0f || Ypv   < -1.0f  || Ypv > 6.0f || \
		QpGem == 0.0f  || \
                1.0f/TMath::Abs(QpGem) > 15.0f || 1.0f/TMath::Abs(QpGem) < 0.15f)
            {
        	if (GEMDEBUG) printf("xGem %f yGem %f txGem %f tyGem %f Xpv %f Ypv %f QpGem %f\n", xGem, yGem, txGem, tyGem, Xpv, Ypv, QpGem);
        	continue;
    	    }

	    ntr1g++;
// fill matching parameters
	    dxm = gemdchtrack->GetDx();
	    dym = gemdchtrack->GetDy();
	    dtx = gemdchtrack->GetDtx();
	    dty = gemdchtrack->GetDty();
	    Ddx = gemdchtrack->GetDdx();
	    Ddy = gemdchtrack->GetDdy();
	    Ddtx = gemdchtrack->GetDdtx();
	    Ddty = gemdchtrack->GetDdty();
	    Chi2m = gemdchtrack->GetChi2();
	    hdx->Fill(dxm);
	    hdy->Fill(dym);
	    hdxy0->Fill(dxm,dym);
	    hdtx->Fill(dtx);
	    hdty->Fill(dty);
	    hDdx->Fill(sqrt(Ddx));
	    hDdy->Fill(sqrt(Ddy));
	    hDdtx->Fill(sqrt(Ddtx));
	    hDdty->Fill(sqrt(Ddty));
	    hpdx->Fill(dxm/sqrt(Ddx));
	    hpdy->Fill(dym/sqrt(Ddy));
	    hpdtx->Fill(dtx/sqrt(Ddtx));
	    hpdty->Fill(dty/sqrt(Ddty));
    	    xyz[0] = track->GetParamLast()->GetX() + XSHIFT;
    	    xyz[1] = track->GetParamLast()->GetY() + YSHIFT;
    	    xyz[2] = track->GetParamLast()->GetZ() + ZSHIFT;
	    tx = track->GetParamLast()->GetTx() + TXSHIFT;
	    ty = track->GetParamLast()->GetTy() + TYSHIFT;
	    xgoodtr = xyz[0]+tx*100.;
	    ygoodtr = xyz[1]+ty*100.;
	    hxytr->Fill(xgoodtr,ygoodtr);
// Selection criteria - x and y tracks matching
	    if (dxm < -DXGEMDCHMAX || dxm > +DXGEMDCHMAX) continue;
	    if (dym < -DYGEMDCHMAX || dym > +DYGEMDCHMAX) continue;
//	    if (dxm > -DXGEMDCHMAX && dxm < +DXGEMDCHMAX)
//	    if (dym > -DYGEMDCHMAX && dym < +DYGEMDCHMAX) continue;
	    hchi2m->Fill(Chi2m);
//	    if (Chi2m > 30.f) continue;
	    chi2 = dxm*dxm/Ddx + dym*dym/Ddy;
	    hchi2->Fill(chi2);
//	    if (chi2 > 30.f) continue;
	    ntr0++;
	    hdxy->Fill(dxm,dym);
    	    le = gemdchtrack->GetLength() - ztarg;
	    p = QpGem;
    	    ps = p;
	    if (p == 0.) continue;
	    p = 1./p;
	    p *= PSCALE;
	    hptr->Fill(p); // GeV/c
	    hltr->Fill(le); // cm
    	    pmod = TMath::Abs(p);
	    // get xyz and cxyz here
	    hxygoodtr->Fill(xgoodtr,ygoodtr);
	    cxyz[0] = tx/TMath::Sqrt(1.+tx*tx+ty*ty);
	    cxyz[1] = ty/TMath::Sqrt(1.+tx*tx+ty*ty);
	    cxyz[2] = 1./TMath::Sqrt(1.+tx*tx+ty*ty);

	    int ncl0 = 0, ncl[NUMBER_CHAMBERS] = {0};
	    Float_t tcl[NUMBER_CHAMBERS][32] = {{0}};
	    Int_t acl[NUMBER_CHAMBERS][32] = {{0}};
	    Int_t wcl[NUMBER_CHAMBERS][32] = {{0}};
	    if (USE_CLUSTERS)
	    {
		    for (int i = 0; i<NUMBER_CHAMBERS; i++)
		    {
			    ncl[i] = 0;
			    int clstart = -1, clwidth = -1, cstr = -1;
			    float samps = 0., timemin = 1000000000.;
			    float widthmax = 0.;
			    int nstr = 32;
			    if (idchambers[i] > 100.f) nstr = 16;
			    for (int j = 0; j<nstr && nhit[i]>0; j++)
			    {
				if (tofWidths[i][j] > 0.)
				{
				    if (clstart < 0)
				    {
					clstart = j;
					cstr = j;
					timemin = tof[i][j];
					widthmax = tofWidths[i][j];
				    }
				    else
				    {
					if (MINTIME)
					{
					    if (tof[i][j] < timemin) {timemin = tof[i][j]; cstr = j; }
					}
					else
					{
					    if (tofWidths[i][j] > widthmax) {widthmax = tofWidths[i][j]; cstr = j; }
					}
				    }
				    samps += tofWidths[i][j];
				    if (j == (nstr-1))
				    {
					clchamb[ncl0] = i;
					clstrip[ncl0] = cstr;
					clwidth = (j+1-clstart);
					tcl[i][ncl[i]] = timemin;
					acl[i][ncl[i]] = samps;
					wcl[i][ncl[i]] = clwidth;
					ncl0++;
					ncl[i]++;
					samps = 0.;
					timemin =  1000000000.;
					widthmax = 0.;
				    }
				}
				else if (clstart >= 0)
				{
				    clchamb[ncl0] = i;
				    clstrip[ncl0] = cstr;
				    clwidth = (j-clstart);
				    tcl[i][ncl[i]] = timemin;
				    acl[i][ncl[i]] = samps;
				    wcl[i][ncl[i]] = clwidth;
				    ncl0++;
				    ncl[i]++;
				    samps = 0.;
				    timemin =  1000000000.;
				    widthmax = 0.;
				    clstart = -1;
				}
			    }
		    }
		    himin = -1;
		    dist = 0.;
		    distmin = 1000000000.0f;
		    for (int icl = 0; icl<ncl0; icl++)
		    {
			ic = clchamb[icl];
			is = clstrip[icl];
			int ih = numhit[ic][is];
			if (usedstrip[ih]) continue;
			dz = zhi[ih] - xyz[2];
			xrpc = xyz[0] + dz*tx;
			yrpc = xyz[1] + dz*ty;
			if (!XYPANIN)
			{
			    if ((xrpc-xhic[ih]) < -(halfwidthx[ic]+DXMAX) || (xrpc-xhic[ih]) > +(halfwidthx[ic]+DXMAX)) continue;
			    if ((yrpc-yhic[ih]) < -(halfwidthy[ic]+DYMAX) || (yrpc-yhic[ih]) > +(halfwidthy[ic]+DYMAX)) continue;
			}
			dx = xrpc - xhi[ih];
			dy = yrpc - yhi[ih];
			if (XYPANIN)
			{
			    if (TMath::Abs(dx) > DXMAXPAN) continue;
			    if (TMath::Abs(dy) > DYMAXPAN) continue;
			    dist = TMath::Sqrt(dx*dx + dy*dy);
			}
			else
			{
			    dist = TMath::Sqrt(dx*dx/XFACTOR + dy*dy);
			}
			if (dist < distmin)
			{
			    distmin = dist;
			    himin = numhit[ic][is];
			}
		    }
	    }
	    else
	    {
		    nh = nhi;
		    himin = -1;
		    dist = 0.;
		    distmin = 1000000000.0f;
		    for (int ih = 0; ih<nh; ih++)
		    {
			if (usedstrip[ih]) continue;
			ic = hchamb[ih];
			is = hstrip[ih];
			dz = zhi[ih] - xyz[2];
			xrpc = xyz[0] + dz*tx;
			yrpc = xyz[1] + dz*ty;
			if (!XYPANIN)
			{
			    if ((xrpc-xhic[ih]) < -(halfwidthx[ic]+DXMAX) || (xrpc-xhic[ih]) > +(halfwidthx[ic]+DXMAX)) continue;
			    if ((yrpc-yhic[ih]) < -(halfwidthy[ic]+DYMAX) || (yrpc-yhic[ih]) > +(halfwidthy[ic]+DYMAX)) continue;
			}
			dx = xrpc - xhi[ih];
			dy = yrpc - yhi[ih];
			if (XYPANIN)
			{
			    if (TMath::Abs(dx) > DXMAXPAN) continue;
			    if (TMath::Abs(dy) > DYMAXPAN) continue;
			    dist = TMath::Sqrt(dx*dx + dy*dy);
			}
			else
			{
			    dist = TMath::Sqrt(dx*dx/XFACTOR + dy*dy);
			}
			if (dist < distmin)
			{
			    distmin = dist;
			    himin = ih;
			}
		    }
	    }
	    if (himin < 0) continue;
// Selection criteria on minimal distance track-hit
	    if (distmin > DISTMAX) continue;
	    usedstrip[himin] = true;
	    hdxytof->Fill(dxm,dym);
	    hxygoodhit->Fill(xgoodtr,ygoodtr);
	    ic = hchamb[himin];
	    is = hstrip[himin];
	    xc = xhic[himin];
	    yc = yhic[himin];
	    zc = zhic[himin];
	    dz = zhi[himin] - xyz[2];
	    xrpc = xyz[0] + dz*tx;
	    yrpc = xyz[1] + dz*ty;

	    hxyhit->Fill(xrpc-xhi[himin],yrpc-yhi[himin]);
	    if (idchambers[ic] < 100.f) hlrdiffs->Fill(lrd[himin]);
	    if (idchambers[ic] > 100.f) hlrdiffb->Fill(lrd[himin]);
	    hxdiff->Fill(xhi[himin]-xc);
	    if (idchambers[ic] < 100.f) hxxhits->Fill(xhi[himin]-xc,xrpc-xc);
	    if (idchambers[ic] > 100.f) hxxhitb->Fill(xhi[himin]-xc,xrpc-xc);
	    if (idchambers[ic] < 100.f) hlrxhits->Fill(lrd[himin],xhi[himin]-xc);
	    if (idchambers[ic] > 100.f) hlrxhitb->Fill(lrd[himin],xhi[himin]-xc);

	    if (idchambers[ic] < 100.f) hlrxtrs->Fill(lrd[himin],xrpc-xc);
	    if (idchambers[ic] > 100.f) hlrxtrb->Fill(lrd[himin],xrpc-xc);

	    if (lrd[himin] > 0. && (xhi[himin]-xc) > 0.) hcsignp->Fill(ic);
	    if (lrd[himin] < 0. && (xhi[himin]-xc) < 0.) hcsignp->Fill(ic);
	    if (lrd[himin] > 0. && (xhi[himin]-xc) < 0.) hcsignm->Fill(ic);
	    if (lrd[himin] < 0. && (xhi[himin]-xc) > 0.) hcsignm->Fill(ic);

	    cosa = sqrt(1.+tx*tx+ty*ty);
	    dl = dz*cosa;
	    lenr = le+dl;
	    hltra->Fill(lenr);
	    hidist = TMath::Sqrt(xc*xc+yc*yc+(zc-ztarg)*(zc-ztarg));
// Selection criteria time-of-flight
	    if (tof[ic][is] > TOFMAX) continue;
	    hstrtr->Fill(is);
	    hchatr->Fill(ic);
	    hxytrtof->Fill(xrpc,yrpc);

	    htoftr->Fill(tof[ic][is]);
//
		    tofr = 0.f;
		    if (CALIBRATION == 0)
		    {
			tofr = tof[ic][is]+tofcalmc[ic];
		    }
		    else if (CALIBRATION == 1)
		    {
			tofr = tof[ic][is]+lenr/cpeak;
		    }
		    else if (CALIBRATION == 2)
		    {
			tofr = tof[ic][is]+tofcalmc[ic];
		    }
		    else if (CALIBRATION == 3)
		    {
			tofr = tof[ic][is]+tofcalmc[ic];
		    }
		    else
		    {
			continue;
		    }
		    tofr0 = tof[ic][is];
		    tofp = 0.f;
		    tofpi = 0.f;
		    beta = lenr/tofr/cvel;
// Selection criteria
		    if (beta <= 0.) continue;
		    mass2 = p*p*(1.f/beta/beta-1.f);
		    mass  = TMath::Sqrt(mass2);
		    hpbeta->Fill(fabs(p),beta);
		    hmass->Fill(mass2);
		    hmassvsp->Fill(fabs(p), mass2);

                    massmin1 = MP2 - MP2/(DMPSCALE1*PMAX1)*(p-0.5);     
                    massmax1 = MP2 + MP2/(DMPSCALE2*PMAX1)*(p-0.5);     
                    if (p > 0.f && mass2 > massmin1 && mass2 < massmax1 && pmod > PMIN && pmod < PMAX)
		    {
			tofp = (lenr/29.97925f)*TMath::Sqrt(MP2+p*p)/p;
			tcor = tofp - tofr;
			tcorc = tcor - tofcalc[ic];
			tcors = tcor - tofcals[ic][is];
			tcalibrc->Fill(ic,tcorc);
			tcalibrs->Fill(ic,is,tcors);
			htofp->Fill(tofr0);
                	massmin2 = MP2 - MP2/(DMPSCALET1*PMAX2)*(p-0.5);     
                	massmax2 = MP2 + MP2/(DMPSCALET2*PMAX2)*(p-0.5);     
                        if (mass2 > massmin2 && mass2 < massmax2)
			{
			    hdtpc->Fill(tcorc);
			    hdtpcvsp->Fill(p,tcorc);
			    hdtps->Fill(tcors);
			    hdtpsvsp->Fill(p,tcors);
			    if (p > PRES) hdtpcplim->Fill(tcorc);
			    if (p > PRES1) hdtpcp1lim->Fill(tcorc);
			    if (p < 2.0f) hdtpcp2lim->Fill(tcorc);
			    if (p >= 2.0f && p < 5.0f) hdtpcp3lim->Fill(tcorc);
			    if (tofWidths[ic][is] > WMIN && tofWidths[ic][is] < WMAX)
			    {
				hdtpcwlim->Fill(tcorc);
				hdtpcvspwlim->Fill(p, tcorc);
				if (p > PRES) hdtpcwplim->Fill(tcorc);
				if (p > PRES1) hdtpcwp1lim->Fill(tcorc);
			    }
			}
		    }
//                    massmaxpi = 0.1 + MPIMAX*TMath::Abs(p-0.5)/PMAXPI/DMPISCALE; // Kapishin
                    massmaxpi = 0.1 + MPIMAX*TMath::Abs(pmod-0.5)/PMAXPI;
                    if ( p > 0 && massmaxpi > massmin1) massmaxpi = massmin1;
//                    massminpi =-0.1 - MPIMAX*TMath::Abs(p-0.5)/PMAXPI/DMPISCALE; // Kapishin
                    massminpi =-0.1 - MPIMAX*TMath::Abs(pmod-0.5)/PMAXPI;
                    if (mass2 < massmaxpi && mass2 > massminpi && pmod > PMIN && pmod < PMAXPI)
		    {
			tofpi = (lenr/29.97925f)*TMath::Sqrt(MPI2+p*p)/pmod;
			tcorpi = tofpi - tofr;
			tcorcpi = tcorpi - tofcalc[ic];
			tcorspi = tcorpi - tofcals[ic][is];
			tcalibrc->Fill(ic,tcorcpi);
			tcalibrs->Fill(ic,is,tcorspi);
		    }
		    timepeak = tofcalmc[ic] + tofcalc[ic];
		    betapeak = lenr/timepeak/cvel;
		    tpeak->Fill(ic, timepeak);
		    bpeak->Fill(ic, betapeak);
                    tofrc = tofr + tofcalc[ic];
                    tofrs = tofr + tofcals[ic][is];
                    betac = lenr/tofrc/cvel;
                    betas = lenr/tofrs/cvel;
		    masss2 = p*p*(1.f/betas/betas-1.f);
		    masss  = TMath::Sqrt(masss2);
		    massc2 = p*p*(1.f/betac/betac-1.f);
		    massc  = TMath::Sqrt(massc2);
		    if (p>0)
		    {
			hpbetac->Fill(p,betac);
			hmassc->Fill(massc2);
			hmasscvsp->Fill(p, massc2);
			hpbetas->Fill(p,betas);
			hmasss->Fill(masss2);
			hmasssvsp->Fill(p, masss2);
                        if (p<1.5) {
                    	    hmasss15->Fill(masss2);
                    	    hmassc15->Fill(massc2);
                        }
                        if (p<2.0) {
                    	    hmasss20->Fill(masss2);
                    	    hmassc20->Fill(massc2);
                        }
		    }
		    else
		    {
			hpbetacm->Fill(fabs(p),betac);
			hmasscm->Fill(massc2);
			hmasscmvsp->Fill(fabs(p), massc2);
			hpbetasm->Fill(fabs(p),betas);
			hmasssm->Fill(masss2);
			hmasssmvsp->Fill(fabs(p),masss2);
		    }
	} // tracks loop
	hntr1->Fill(ntr1);
	Ntr1+=ntr1;
	Ntr1g+=ntr1g;
	hntr0->Fill(ntr0);
	Ntr0+=ntr0;
        //t_out->Fill();
    } // event loop

    TH1D *projtmp = 0, *projtmp1 = 0;
    hdxyeff->Divide(hdxytof,hdxy);
    hdxeff->Divide((projtmp = hdxytof->ProjectionX()),(projtmp1 = hdxy->ProjectionX()));
    if (projtmp) delete projtmp;
    if (projtmp1) delete projtmp1;
    hdyeff->Divide((projtmp = hdxytof->ProjectionY()),(projtmp1 = hdxy->ProjectionY()));
    if (projtmp) delete projtmp;
    if (projtmp1) delete projtmp1;
    hxyeff->Divide(hxygoodhit,hxygoodtr);
    hxeff->Divide((projtmp = hxygoodhit->ProjectionX()),(projtmp1 = hxygoodtr->ProjectionX()));
    if (projtmp) delete projtmp;
    if (projtmp1) delete projtmp1;
    hyeff->Divide((projtmp = hxygoodhit->ProjectionY()),(projtmp1 = hxygoodtr->ProjectionY()));
    if (projtmp) delete projtmp;
    if (projtmp1) delete projtmp1;

    if (FLAG < 0)
    {
	printf("Number of tracks %d, per event %f\n", Ntr, (float)Ntr/nEvents);
	printf("GEM+DCH1+DCH2 tracks %d, per event %f (%f%% per track)\n", Ntr1, (float)Ntr1/nEvents, (float)Ntr1/Ntr*100.f);
	printf("Good GEM+DCH1+DCH2 tracks %d, per event %f (%f%% per track)\n", Ntr0, (float)Ntr0/nEvents, (float)Ntr0/Ntr*100.f);
    }
    else if (FLAG > 0)
    {
	printf("Number of tracks %d,  per event %f\n", Ntr, (float)Ntr/nEvents);
	printf("GEM+DCH1 tracks %d, per event %f (%f%% per track)\n", Ntr1, (float)Ntr1/nEvents, (float)Ntr1/Ntr*100.f);
	printf("Good GEM+DCH1 tracks %d,  per event %f (%f%% per track)\n", Ntr0, (float)Ntr0/nEvents, (float)Ntr0/Ntr*100.f);
    }
    else
    {
	printf("Number of tracks %d, per event %f\n", Ntr, (float)Ntr/nEvents);
	printf("GEM+DCH tracks %d, per event %f (%f%% per track)\n", Ntr1, (float)Ntr1/nEvents, Ntr>0?(float)Ntr1/Ntr*100.f:0.f);
	printf("Good GEM+DCH tracks %d, per event %f (%f%% per valid track)\n", Ntr1g, (float)Ntr1g/nEvents, Ntr1>0?(float)Ntr1g/Ntr1*100.f:0.f);
	printf("Matched GEM+DCH tracks %d,  per event %f (%f%% per good track)\n", Ntr0, (float)Ntr0/nEvents, Ntr1g>0?(float)Ntr0/Ntr1g*100.f:0.f);
    }

    if (f) {f->Write();}

    if (WRITE_CALIBRATION)
    {
	char fname_calibr_root[128], *delim = 0;
	strcpy(fname_calibr_root, gSystem->BaseName(fnametof700));
	if ((delim = strrchr(fname_calibr_root, (int)'.'))) *delim = '\0';
	strcat(fname_calibr_root, "_calibration.root");
	TFile *fc = new TFile(fname_calibr_root,"RECREATE","Proton mass based calibration of BmnTOF700");
	tcalibrs->Write();
	tcalibrc->Write();
	fc->Close();
    }

    return;
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

int readMCTimeFile(const char *MCTimeFile)
{
//	cout << "readMCTimeFile: " << MCTimeFile << endl;
	float idchambers[59] = { 27.1,28.1,3.1,1.1,29.1,4.1,33.1,30.1,5.1,19.3,31.1,6.1,2.1,32.1,15.2,16.2,17.2,
	18.2,19.2,20.2,7.1,115.2,113.1,117.1,35.1,9.1,37.1,11.1,39.1,13.1,34.1,8.1,36.1,10.1,38.1,12.1,21.2,
	23.2,25.2,22.2,24.2,26.2,107.2,108.2,109.2,110.2,111.2,112.2,114.1,116.2,118.1,14.1,40.1,119.2,120.2,
	121.2,122.2,123.2,124.2 };
	bool notused[59] = {true};
	int order[59] = {-1};
	int c = 0, cmin = -1, c0 = 0;
	float idmin = 200.f;
	for (c0 = 0; c0 < 59; c0++) { notused[c0] = true; order[c0] = -1; }
	for (c0 = 0; c0 < 59; c0++)
	{
	    cmin = -1;
	    idmin = 200.f;
	    for (c=0; c<59; c++)
	    {
		if (notused[c] && (idchambers[c]<idmin))
		{
		    idmin = idchambers[c];
		    cmin = c;
		}
	    }
//	    printf("c0 %d cmin %d notused %d\n",c0,cmin,(int)notused[cmin]);
	    if (cmin >= 0)
	    {
		notused[cmin] = false;
		order[c0] = cmin;
	    }
	}
	char fname[128];
	FILE *ft = 0;
	float time = 0.f, timesigma = 0.f;
	if (MCTimeFile == NULL)
	{
	    printf("TOF700 MC time-of-flight file name not defined!\n");
	    return 0;
	}
	if (strlen(MCTimeFile) == 0)
	{
	    printf("TOF700 MC time-of-flight file name not defined!\n");
	    return 0;
	}
	TString dir = getenv("VMCWORKDIR");
	sprintf(fname,"%s/geometry/%s",dir.Data(),MCTimeFile);
	ft = fopen(fname,"r");
	if (ft == NULL)
	{
	    printf("TOF700 MC time-of-flight file %s open error!\n", fname);
	    return 0;
	}
	int cexp = 0, ic = -1;
	while(fscanf(ft,"%d %f %f\n", &ic, &time, &timesigma) == 3)
	{
		if (ic > 0 && ic <= 59)
		{
		    cexp = order[ic-1];
		    tofcalmc[cexp] = time;
		    if (1) printf("Chamber %.1f (MC %d, EXP %d) average time-of-flight %f with sigma %f\n",idchambers[cexp], ic, cexp, time, timesigma);
		}
	}
	fclose(ft);
	return 1;
}
