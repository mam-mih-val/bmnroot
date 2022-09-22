#include <fstream>
#include <Rtypes.h>
#include "TParticle.h"
#ifndef ROOT_TParticlePDG
 #include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
 #include "TDatabasePDG.h"
#endif
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "bmndata/BmnEnums.h"
//#include "macro/run/bmnloadlibs.C"

//#define TOFGEOMETRY "TOF700_geometry_run7_1st.txt"
#define TOFGEOMETRY "TOF700_geometry_run7.txt"

#define USETRUEHIT 0
#define SORT 0
#define AARGON 40
#define MCMATCH 0

#define MINTRUE 0.5f
#define GEMCUTS 0
#define PRIMARY_VERTEX_CUTS 0
#define MINGEMNDF1 6
#define MINGEMNDF 4
#define MINSIHITS 2
#define GEMDEBUG false
#define MCDEBUG false
#define DXGEMDCHMAX 5.
#define DYGEMDCHMAX 3.
//#define DXGEMDCHMAX 2.5
//#define DYGEMDCHMAX 1.5

// CFACTOR - beta factor for main time peak particles
#define CFACTOR 0.97f

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

#define XYPANIN 1
#define XFACTOR 1.0f
#define DISTMAX 20.0f
#define DXMAX 5.0f
#define DYMAX 3.0f
#define DXMAXPAN 5.0f
#define DYMAXPAN 5.0f

#define ACCEPTANCE false
#define DXBORDER 5.
#define DYBORDER 5.

#define TOFMAX 60.0f

#define NUMBER_CHAMBERS 59

Float_t halfwidthx[NUMBER_CHAMBERS] = {8.0};
Float_t halfwidthy[NUMBER_CHAMBERS] = {0.5};

int nhi = 0;
int nhit[NUMBER_CHAMBERS] = {0};
int nhits[NUMBER_CHAMBERS][32] = {{0}};
float tof[NUMBER_CHAMBERS][32] = {{0.}};
double xhi[1000], yhi[1000], zhi[1000];
double xhic[1000], yhic[1000], zhic[1000];
using namespace std;
using namespace TMath;

BmnTof2Raw2DigitNew *TOF2 = NULL;

float idchambers[59] = {27.1,28.1,3.1,1.1,29.1,4.1,33.1,30.1,5.1,19.3,31.1,6.1,2.1,32.1,15.2,16.2,17.2,
18.2,19.2,20.2,7.1,115.2,113.1,117.1,35.1,9.1,37.1,11.1,39.1,13.1,34.1,8.1,36.1,10.1,38.1,12.1,21.2,
23.2,25.2,22.2,24.2,26.2,107.2,108.2,109.2,110.2,111.2,112.2,114.1,116.2,118.1,14.1,40.1,119.2,120.2,
121.2,122.2,123.2,124.2 };

//float idchambers[59] = {0.};
int order[59] = {-1};

Int_t readMCTimeFile(const char *MCTimeFile = "TOF700_MC_argon_qgsm_time_run7.txt");

Double_t tofcalmc[NUMBER_CHAMBERS] = {0.f};

//#define TRACKFILE "/eos/nica/bmn/users/kovachev/efficiency/TracksForYuri/MC_From_Gleb/recoMC_GEM_TOF.root"
//#define MCFILE "/eos/nica/bmn/users/kovachev/efficiency/TracksForYuri/MC_From_Gleb/mc_GEM_CSC_TOF1.root"

//#define TRACKFILE "/eos/nica/bmn/users/petukhov/MC/recoMC_GEM_TOF.root"
//#define MCFILE "/eos/nica/bmn/users/petukhov/MC/mc_GEM_CSC_TOF1.root"

//#define TRACKFILE "/eos/nica/bmn/users/petukhov/MC/reco_GEM_CSC_DCH_Tof_Ar_all_targets.root"

//#define TRACKFILE "/eos/nica/bmn/users/petukhov/MC/EXTR510_GEM_CSC_DCH_Tof_Ar_all_targets.root"
//#define MCFILE "/eos/nica/bmn/users/petukhov/MC/mc_GEM_CSC_DCH_Tof_Ar.root"

//#define TRACKFILE "/nica/mpd15/kovachev/MC_FILES/EXTR510_DCMQGSM_Ar_All_3.2AGeV_mb_250k.root"
//#define MCFILE "/nica/mpd15/kovachev/MC_FILES/mc_DCMQGSM_Ar_All_3.2AGeV_mb_250k.root"

/*
/nica/mpd16/kovachev/MC_run7_efficiency/MC_Files_run7_For_Yuri/EXTR510_DQGSM_Ar_old_target.root
/nica/mpd16/kovachev/MC_run7_efficiency/MC_Files_run7_For_Yuri/reco_DCMQGSM_Ar_Old_target_1-23.root
*/

//#define MCFILE "/nica/mpd16/kovachev/MC_run7_efficiency/MC_Files_run7_For_Yuri/sim_DCMQGSM_Ar_1-23_Old_target.root"
//#define TRACKFILE "/nica/mpd16/kovachev/MC_run7_efficiency/MC_Files_run7_For_Yuri/GemDch_Link_DCMQGSM_old_target.root"

//#define MCFILE "/nica/mpd16/kovachev/MC_run7_efficiency/MC_Files_run7_For_Yuri/sim_DCMQGSM_Ar_1-23_Old_target.root"
//#define TRACKFILE "/nica/mpd16/kovachev/MC_run7_efficiency/MC_Files_run7_For_Yuri/GemDch_Link_DCMQGSM_old_target_efficiency.root"

//#define TRACKFILE "/nica/mpd16/kovachev/MC_run7_efficiency/MC_Files_run7_For_Yuri/GemDch_Link_DCMQGSM_Ar_New_target_without_SetBeam_AR.root"
//#define TRACKFILE "mk1_argon_files.txt"
//#define TRACKFILE "mk3_argon_files.txt"
#define TRACKFILE "/eos/nica/bmn/users/kovachev/test_Urqmd/Gem_Dch_Tracks_Urqmd2-80.root"
//#define TRACKFILE "/eos/nica/bmn/users/kovachev/test_Urqmd/Gem_Dch_Tracks_Urqmd.root"
#define MCFILE "/nica/mpd16/kovachev/MC_run7_efficiency/MC_Files_run7_For_Yuri/sim_DCMQGSM_New_target_without_SetBeam_Ar_1-25_.root"

void BmnMCEfficiencyTOF700(const char *TracksGemDch = TRACKFILE, const char *DataMC = MCFILE) {

    /* Load basic libraries */
//#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
//    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
//#endif
//    bmnloadlibs(); // load BmnRoot libraries

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

    TClonesArray *eventHeaderMC = NULL;
    TClonesArray *tof700Digits = NULL, *eventHeader = NULL, *mcmatch = NULL, *stspoints = NULL;
    TClonesArray *tracks = NULL, *mtracks = NULL, *MCtracks = NULL, *tofpoints = NULL;
    CbmVertex* primaryVertex = NULL;
    TString filename;

    char fnameMC[256];
//    TChain *bmnTree = new TChain("bmndata");
    TChain *bmnTree = new TChain("BMN_DIGIT");
    if (strstr(TracksGemDch,".root"))
    {
	bmnTree->Add(TracksGemDch);
	strcpy(fnameMC, TracksGemDch);
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
		strcpy(fnameMC, filename.Data());
	    }
	    fin.close();
	}
	else
	{
	    printf("Non-existing run list file: %s!", TracksGemDch);
	    return;
	}
    }
//    bmnTree->SetBranchAddress("EventHeader", &eventHeader);
    bmnTree->SetBranchAddress("GemDchTracks", &mtracks);
//    bmnTree->SetBranchAddress("BmnTofHit", &tof700Digits);
    bmnTree->SetBranchAddress("BmnTof700Hit", &tof700Digits);
    if (MCMATCH) bmnTree->SetBranchAddress("StsTrackMatch", &mcmatch);
    bmnTree->SetBranchAddress("MCTrack", &MCtracks);
    bmnTree->SetBranchAddress("StsPoint", &stspoints);
    bmnTree->SetBranchAddress("TOF700Point", &tofpoints);
    bmnTree->SetBranchAddress("PrimaryVertex.", &primaryVertex);
/*
    char fnameMC[256];
    TChain *bmnTreeMC = new TChain("bmndata");
    if (strstr(DataMC,".root"))
    {
	bmnTreeMC->Add(DataMC); //
	strcpy(fnameMC, DataMC);
    }
    else
    {
	ifstream fin(DataMC);
	if (fin.is_open())
	{
	    for(int idx=0; idx<1000; idx++) {
		if (fin.eof()) break;
		fin >> filename;
		if (strlen(filename.Data())==0) break;
		if ((filename.Data())[0]=='#') continue;
		cout << " TOF Data file #" << idx+1 << " " << filename.Data() << endl;
		bmnTreeMC->Add(filename);
		strcpy(fnameMC, filename.Data());
	    }
	    fin.close();
	}
	else
	{
	    printf("Non-existing run list file: %s!", DataMC);
	    return;
	}
    }
//    bmnTreeMC->SetBranchAddress("EventHeader", &eventHeaderMC);
    bmnTreeMC->SetBranchAddress("MCTrack", &MCtracks);
    bmnTreeMC->SetBranchAddress("StsPoint", &stspoints);
    bmnTreeMC->SetBranchAddress("TOF700Point", &tofpoints);
*/
    TOF2 = new BmnTof2Raw2DigitNew(mapping, "bmn_run4649_raw.root", 0, 0, TOFGEOMETRY);

    readMCTimeFile();

    Double_t xmintof700 = +1000., xmaxtof700 = -1000., ymintof700 = +1000., ymaxtof700 = -1000.;
    Float_t xs = 0., ys = 0., zs = 0.;
    Double_t xsmin = 0., ysmin = 0., xsmax = 0., ysmax = 0.;
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
	int nstr = 32;
	if (idchambers[c] > 100.f) nstr = 16;
	TOF2->get_strip_xyz(c, 0, &xs,&ys,&zs);
	xsmin = xs - halfwidthx[c];
	xsmax = xs + halfwidthx[c];
	ysmin = ys - halfwidthy[c];
	ysmax = ys + halfwidthy[c];
	if (xsmin < xmintof700) xmintof700 = xsmin;
	if (xsmax > xmaxtof700) xmaxtof700 = xsmax;
	if (ysmin < ymintof700) ymintof700 = ysmin;
	if (ysmax > ymaxtof700) ymaxtof700 = ysmax;
	TOF2->get_strip_xyz(c,nstr-1, &xs,&ys,&zs);
	xsmin = xs - halfwidthx[c];
	xsmax = xs + halfwidthx[c];
	ysmin = ys - halfwidthy[c];
	ysmax = ys + halfwidthy[c];
	if (xsmin < xmintof700) xmintof700 = xsmin;
	if (xsmax > xmaxtof700) xmaxtof700 = xsmax;
	if (ysmin < ymintof700) ymintof700 = ysmin;
	if (ysmax > ymaxtof700) ymaxtof700 = ysmax;
    }
    xmintof700 -= DXBORDER;
    xmaxtof700 += DXBORDER;
    ymintof700 -= DYBORDER;
    ymaxtof700 += DYBORDER;

    char name[128], title[128];

    gStyle->SetOptFit(111);
    gStyle->SetOptStat(111111);


    TFile *f = NULL;
    if (strlen(fnameMC))
    {
	char fname_root[128], *delim = 0;
	strcpy(fname_root, gSystem->BaseName(fnameMC));
	if ((delim = strrchr(fname_root, (int)'.'))) *delim = '\0';
	strcat(fname_root, "_MC_efficiency_results.root");
	f = new TFile(fname_root,"RECREATE","Results of BmnTOF700 MC");
    }
    TH1F *hMCmass0 = new TH1F("hMCmass0", "Mass/Z, Monte-Carlo, nominal", 200,-0.5,9.5);
    TH1F *hMCrmass0 = new TH1F("hMCrmass0", "Mass/Z, Monte-Carlo, nominal", 200,-0.5,9.5);
    TH1F *hMCamass = new TH1F("hMCamass", "Mass/Z, Monte-Carlo, nominal", 200,-0.5,9.5);
    TH1F *h1MCmass0 = new TH1F("h1MCmass0", "Mass/Z, Monte-Carlo, reconstructable", 200,-0.5,9.5);
    TH1F *h1MCrmass0 = new TH1F("h1MCrmass0", "Mass/Z, Monte-Carlo, reconstructable", 200,-0.5,9.5);
    TH1F *h1MCamass = new TH1F("h1MCamass", "Mass/Z, Monte-Carlo, reconstructable", 200,-0.5,9.5);
    TH1F *h1MCeffmass = new TH1F("h1MCeffmass", "Eff Mass/Z, Monte-Carlo, reconstructable", 200,-0.5,9.5);
    TH1F *h2MCamass = new TH1F("h2MCamass", "Mass/Z, Monte-Carlo, reconstructable, p<2", 200,-0.5,9.5);
    TH1F *h2MCeffmass = new TH1F("h2MCeffmass", "Eff Mass/Z, Monte-Carlo, reconstructable, p<2", 200,-0.5,9.5);
    TH1F *h3MCamass = new TH1F("h3MCamass", "Mass/Z, Monte-Carlo, NDF>5", 200,-0.5,9.5);
    TH1F *h3MCeffmass = new TH1F("h3MCeffmass", "Eff Mass/Z, Monte-Carlo, NDF>5", 200,-0.5,9.5);
    TH1F *h4MCamass = new TH1F("h4MCamass", "Mass/Z, Monte-Carlo, TOF700 only", 200,-0.5,9.5);
    TH1F *h4MCeffmass = new TH1F("h4MCeffmass", "Eff Mass/Z, Monte-Carlo, TOF700 only", 200,-0.5,9.5);
    TH1F *hMCmass2 = new TH1F("hMCmass2", "Mass2/Z2, Monte-Carlo, simple reconstion", 200,-0.5,9.5);
    TH1F *hMCmass = new TH1F("hMCmass", "Mass/Z, Monte-Carlo, simple reconstion", 200,-0.5,9.5);
    TH1F *hMCmassr = new TH1F("hMCmassr", "Mass/Z, Monte-Carlo, simple reconstion", 200,-0.5,9.5);
    TH1F *hMCmassrp = new TH1F("hMCmassrp", "Mass/Z, Monte-Carlo, simple reconstion, p<2", 200,-0.5,9.5);
    TH1F *hMCeffmass = new TH1F("hMCeffmass", "Eff Mass/Z, Monte-Carlo, simple reconstruction", 200,-0.5,9.5);
    TH1F *hMCeffmassp = new TH1F("hMCeffmassp", "Eff Mass/Z, Monte-Carlo, simple reconstruction, p<2", 200,-0.5,9.5);
    TH2F *hMCavsz = new TH2F("hMCavsz", "A vs Z, Monte-Carlo", 30,0,30,30,0,30);
    hMCavsz->SetOption("COLZ");
    TH2F *hMCravsz = new TH2F("hMCravsz", "A vs Z, Monte-Carlo, reconstructable", 30,0,30,30,0,30);
    hMCravsz->SetOption("COLZ");
    TH2F *h1MCavsz = new TH2F("h1MCavsz", "A vs Z, Monte-Carlo", 30,0,30,30,0,30);
    h1MCavsz->SetOption("COLZ");
    TH2F *h1MCravsz = new TH2F("h1MCravsz", "A vs Z, Monte-Carlo, reconstructable", 30,0,30,30,0,30);
    h1MCravsz->SetOption("COLZ");
    TH2F *hpbetamc = new TH2F("hpbetamc","Beta vs momentum, Monte-Carlo", 200,0,10,220,0,1.1f);
    hpbetamc->SetOption("COLZ");
    TProfile *hpanglep = new TProfile("hpanglep","Average Angle vs momentum, fragments, Monte-Carlo", 150,0,15,0,70.f,"e");
    TH2F *hpangle = new TH2F("hpangle","Angle vs momentum, fragments, Monte-Carlo", 200,-5,15, 50,0,50.f);
    hpangle->SetOption("COLZ");
    TH1F *hangle   = new TH1F("hangle","Theta angle of tracks at vertex", 100,0,50);
    TProfile *hpanglep1 = new TProfile("hpanglep1","Average Angle vs momentum, fragments, Monte-Carlo, reconstructable", 150,0,15,0,70.f,"e");
    TH2F *hpangle1 = new TH2F("hpangle1","Angle vs momentum, fragments, Monte-Carlo, reconstructable", 200,-5,15, 50,0,50.f);
    hpangle1->SetOption("COLZ");
    TH1F *hangle1   = new TH1F("hangle1","Theta angle of tracks at vertex, reconstructable", 100,0,50);
    TProfile *hpanglep2 = new TProfile("hpanglep2","Average Angle vs momentum, fragments, Monte-Carlo, TOF700 hit", 150,0,15,0,70.f,"e");
    TH2F *hpangle2 = new TH2F("hpangle2","Angle vs momentum, fragments, Monte-Carlo, TOF700 hit", 200,-5,15, 50,0,50.f);
    hpangle2->SetOption("COLZ");
    TH1F *hangle2   = new TH1F("hangle2","Theta angle of tracks at vertex, TOF700 hit", 100,0,50);
    TH1F *hangleff   = new TH1F("hangleff","Efficiency vs Theta angle of tracks at vertex", 100,0,50);
    TH1F *hangleff2   = new TH1F("hangleff2","Efficiency vs Theta angle of tracks at vertex, TOF700 only", 100,0,50);
    TH1F *hMCpndf   = new TH1F("hMCpndf","Momentum of tracks", 1000,-5,15);
    TH1F *hMCpndf0  = new TH1F("hMCpndf0","Momentum of tracks, NDF<6", 1000,-5,15);
    TH1F *hMCpndf1  = new TH1F("hMCpndf1","Momentum of tracks, NDF>=6", 1000,-5,15);
    TH1F *hMCpndf2  = new TH1F("hMCpndf2","Momentum of tracks, NDF>=6, NTOF>0", 1000,-5,15);
//    TH1F *hnhgem = new TH1F("hnhgem","MC GEM hits for fragments", 10,0,10);
//    TH1F *hnhgem0 = new TH1F("hnhgem0","MC GEM hits for fragments (no TOF700 hits)", 10,0,10);
//    TH1F *hnhgem1 = new TH1F("hnhgem1","MC GEM hits for fragments (with TOF700 hits)", 10,0,10);

    TH1F *hnsts = new TH1F("hnsts","Number of STS points", 20,0,20);
    TH1F *hntr = new TH1F("hntr","Number of all tracks", 200,0,200);
    TH1F *hntr0 = new TH1F("hntr0","Number of selected tracks", 200,0,200);
    TH1F *hntr1 = new TH1F("hntr1","Number of tracks with FLAG conditions", 200,0,200);
    TH2F *hxydch = new TH2F("hxydch","XY DCH tracks", 70,-140,140,60,-90,90);
    TH2F *hxygem = new TH2F("hxygem","XY GEM tracks", 70,-140,140,60,-90,90);
    TH2F *hpxysts = new TH2F("hpxysts","XY GEM points (P/Z > 7)", 140,-70,70,120,-60,60);
    TH2F *hpxygem = new TH2F("hpxygem","XY GEM REC tracks (P/Z > 7)", 70,-140,140,60,-90,90);
    TH2F *hp0xygem = new TH2F("hp0xygem","XY GEM MC tracks (P/Z > 7)", 70,-140,140,60,-90,90);
    TH1F *hptr = new TH1F("hptr","Momentum of GEM tracks", 1000,-10,10);
    TH1F *hpgemdch = new TH1F("hpgemdch","Momentum of GEM+DCH tracks", 1000,-10,10);
    TH1F *hpgoodhit = new TH1F("hpgoodhit","Momentum of GEM+DCH+TOF700 tracks", 1000,-10,10);
    TH1F *hpbadhit = new TH1F("hpbadhit","Momentum of GEM+DCH+NoTOF700 tracks", 1000,-10,10);
    TH1F *htruehits = new TH1F("htruehits","True hits fraction in reconstructed GEM track", 110,0,1.1);
    TH1F *hallhits = new TH1F("hallhits","Number of hits in reconstructed GEM track", 20,0,20);
    TH1F *hp0  = new TH1F("hp0","Momentum of tracks (all)", 1000,-10,10);
    TH1F *hltr = new TH1F("hltr","Lenght of GEM tracks", 100,500,700);
    TH1F *hltra = new TH1F("hltra","Lenght of GEM+DCH tracks", 100,500,700);
    TH2F *hxytrtof = new TH2F("hxytrtof","XY tracks with TOF700 hits", 1500,-150,150,1000,-100,100);
    hxytrtof->SetOption("COLZ");
    TH2F *hxyhit = new TH2F("hxyhit","XY track hit distance from TOF700 hit", 100,-50,50,40,-10,10);
    TH1F *hxdiff = new TH1F("hxdiff","X difference for TOF700 hits with track", 100,-50,+50);
    TH2F *hxxhits = new TH2F("hxxhits","X track hit vs TOF700 hit (small chambers)", 200,-100,100,200,-100,100);
    TH2F *hxxhitb = new TH2F("hxxhitb","X track hit vs TOF700 hit (big chambers)", 200,-100,100,200,-100,100);

    TH1F *htoftr = new TH1F("htoftr","TOF of tracks with TOF700 hits", 600,-20,40);
    TH1F *htof = new TH1F("htof","TOF for hits", 600,-20,40);
    TH1F *hntof = new TH1F("hntof","Number of hits", 100,0,100);
    TH1F *hnhit = new TH1F("hnhit","Number of hits in width limits", 100,0,100);
    TH2F *hxytof = new TH2F("hxytof","XY TOF700 hits", 1500,-150,150,1000,-100,100);
    hxytof->SetOption("COLZ");
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
    TH1F *hmass = new TH1F("hmass", "Mass, p<2", 200,-0.5,9.5);
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
    TH1F *hdtpcplim = new TH1F("hdtpcplim", "Time resolution, protons, p>3", 200,-1.,+1.);
    TH1F *hdtpcp1lim = new TH1F("hdtpcp1lim", "Time resolution, protons, p>4", 200,-1.,+1.);
    TH1F *hdtpcp2lim = new TH1F("hdtpcp2lim", "Time resolution, protons, p<2", 200,-1.,+1.);
    TH1F *hdtpcp3lim = new TH1F("hdtpcp3lim", "Time resolution, protons, 2<p<5", 200,-1.,+1.);

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

    TH2F *hdxy0 = new TH2F("hdxy0","DxDy all",40,-5,+5,40,-5,+5);
    hdxy0->SetOption("COLZ");
    TH1F *hdx = new TH1F("hdx","dx all",40,-5,+5);
    TH1F *hdy = new TH1F("hdy","dy all",40,-5,+5);
    TH2F *hdxy = new TH2F("hdxy","DxDy selected",40,-5,+5,40,-5,+5);
    hdxy->SetOption("COLZ");
    TH2F *hdxytof = new TH2F("hdxytof","DxDy with TOF700 hits",40,-5,+5,40,-5,+5);
    hdxytof->SetOption("COLZ");
    TH2F *hdxyeff = new TH2F("hdxyeff","Efficiency vs DxDy",40,-5,+5,40,-5,+5);
    hdxyeff->SetOption("COLZ");
    TH1F *hdxeff = new TH1F("hdxeff","Efficiency vs Dx",40,-5,+5);
    TH1F *hdyeff = new TH1F("hdyeff","Efficiency vs Dy",40,-5,+5);
    TH2F *hxygoodtr = new TH2F("hxygoodtr","XY(dch) GEM+DCH good tracks", 70,-140,140,60,-90,90);
    hxygoodtr->SetOption("COLZ");
    TH2F *hxygoodtrgem = new TH2F("hxygoodtrgem","XY(gem) GEM+DCH good tracks", 70,-140,140,60,-90,90);
    hxygoodtrgem->SetOption("COLZ");
    TH2F *hxygoodhit = new TH2F("hxygoodhit","XY good tracks (GEM+DCH) with good hits", 70,-140,140,60,-90,90);
    hxygoodhit->SetOption("COLZ");
    TH2F *hxygoodhita = new TH2F("hxygoodhita","XY good tracks (GEM+DCH) with good hits", 70,-140,140,60,-90,90);
    hxygoodhita->SetOption("COLZ");
    TH2F *hxybadhit = new TH2F("hxybadhit","XY good tracks (GEM+DCH) with no hits", 70,-140,140,60,-90,90);
    hxybadhit->SetOption("COLZ");
    TH2F *hxybadhita = new TH2F("hxybadhita","XY good tracks (GEM+DCH) with no hits", 70,-140,140,60,-90,90);
    hxybadhita->SetOption("COLZ");
    TH2F *hxygoodhit1 = new TH2F("hxygoodhit1","XY good tracks (DCH) with good hits", 70,-140,140,60,-90,90);
    hxygoodhit1->SetOption("COLZ");
    TH2F *hxygoodhit1a = new TH2F("hxygoodhit1a","XY good tracks (DCH) with good hits", 70,-140,140,60,-90,90);
    hxygoodhit1a->SetOption("COLZ");
    TH2F *hxyeff = new TH2F("hxyeff","Efficiency GEM+DCH+TOF700 vs XY good GEM+DCH tracks", 70,-140,140,60,-90,90);
    hxyeff->SetOption("COLZ");
    TH2F *hxyeffa = new TH2F("hxyeffa","Efficiency GEM+DCH+TOF700 vs XY good GEM+DCH tracks", 70,-140,140,60,-90,90);
    hxyeffa->SetOption("COLZ");
    TH1F *hxeff = new TH1F("hxeff","Efficiency GEM+DCH+TOF700 vs X good GEM+DCH tracks", 70,-140,140);
    TH1F *hxeffa = new TH1F("hxeffa","Efficiency GEM+DCH+TOF700 vs X good GEM+DCH tracks", 70,-140,140);
    TH1F *hyeff = new TH1F("hyeff","Efficiency GEM+DCH+TOF700 vs Y good GEM+DCH tracks", 60,-90,90);
    TH1F *hyeffa = new TH1F("hyeffa","Efficiency GEM+DCH+TOF700 vs Y good GEM+DCH tracks", 60,-90,90);
    TH2F *hxyeffmg = new TH2F("hxyeffmg","Efficiency GEM+DCH vs XY good GEM tracks", 70,-140,140,60,-90,90);
    hxyeffmg->SetOption("COLZ");
    TH2F *hxyeffmd = new TH2F("hxyeffmd","Efficiency GEM+DCH vs XY good DCH tracks", 70,-140,140,60,-90,90);
    hxyeffmd->SetOption("COLZ");
    TH2F *hxyeffm1d = new TH2F("hxyeffm1d","Efficiency DCH+TOF700 vs XY good DCH tracks", 70,-140,140,60,-90,90);
    hxyeffm1d->SetOption("COLZ");
    TH2F *hxyeffm1da = new TH2F("hxyeffm1da","Efficiency DCH+TOF700 vs XY good DCH tracks", 70,-140,140,60,-90,90);
    hxyeffm1da->SetOption("COLZ");
    TH1F *hpeff = new TH1F("hpeff","Efficiency GEM+DCH+TOF700 vs P good GEM+DCH tracks", 1000,-10,10);
    TH1F *hpeffmg = new TH1F("hpeffmg","Efficiency GEM+DCH vs P good GEM tracks", 1000,-10,10);
    TH2F *hxyeffdg = new TH2F("hxyeffdg","Efficiency DCH vs XY good GEM tracks", 70,-140,140,60,-90,90);
    hxyeffdg->SetOption("COLZ");
    TH2F *hxyeffgd = new TH2F("hxyeffgd","Efficiency GEM vs XY good DCH tracks", 70,-140,140,60,-90,90);
    hxyeffgd->SetOption("COLZ");

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

    TH1F *hrej1 = new TH1F("hrej1","Reject because not own hit",5,0,5);
    TH1F *hrej2 = new TH1F("hrej2","Reject because DX",5,0,5);
    TH1F *hrej3 = new TH1F("hrej3","Reject because DY",5,0,5);
    TH1F *hrej5 = new TH1F("hrej5","Reject because DIST",5,0,5);
    TH1F *hrej6 = new TH1F("hrej6","Reject because TIME",5,0,5);

    TProfile2D *tcalibrs = new TProfile2D("tcalibr","Time calibration based on proton mass, strips",NUMBER_CHAMBERS,0,NUMBER_CHAMBERS,32,0,32,-1.,+1.,"e");
    TProfile *tcalibrc = new TProfile("tcalibrc","Time calibration based on proton mass, chambers",NUMBER_CHAMBERS,0,NUMBER_CHAMBERS,-1.,+1.,"e");

    TProfile *tpeak = new TProfile("tpeak","Peak time vs chamber",NUMBER_CHAMBERS,0,NUMBER_CHAMBERS,0.,+30.,"e");
    TProfile *bpeak = new TProfile("bpeak","Peak velosity vs chamber",NUMBER_CHAMBERS,0,NUMBER_CHAMBERS,0.,+1.5,"e");

    char nam[64],tit[64];

    float cvel = 29.97925f;
    float ekin = 3.2f, ebeam = ekin + MP, pbeam = TMath::Sqrt(ebeam*ebeam+MP*MP);
    float cbeam = pbeam/ebeam;
    float cpeak = cbeam*CFACTOR*cvel;
//    float ztarg = -2.2f; // EXP
    float ztarg = -2.3f; // MC
    float zc[NUMBER_CHAMBERS] = {0.f};

    for (int i = 0; i<NUMBER_CHAMBERS; i++)
    {
	TOF2->get_chamber_z(i,&zc[i]);
    }

    Int_t startEvent = 0;
    Int_t nEvents = bmnTree->GetEntries();
    Int_t nEventsMC = nEvents; //bmnTreeMC->GetEntries();
    printf("\nNumber of events in tracks chain is %d (MC chain %d)\n", nEvents, nEventsMC);

    int Ntr = 0, Ntr1 = 0, Ntr1g = 0, Ntr0 = 0, Ntr700 = 0;
    Int_t iEv = 0, iEvMC = 0, fCurrent = -1;
    UInt_t trackEvId = 0, MCEvId = 0;
    UInt_t trackRunId = 0, MCRunId = 0;
//    nEvents = 10;
    TH1D *projtmp = 0, *projtmp1 = 0;
    for (Int_t iEv0 = startEvent; iEv0 < startEvent + nEvents; iEv0++)
    {
        if (iEv % 10000 == 0)
	{
	    cout << "Event: " << iEv << "/" << startEvent + nEvents << " ( Run " << trackRunId << " EvId " << trackEvId << " )" << endl;
	}
    	BmnGemDchTrack *gemdchtrack0 = NULL;
//	do
//	{
	    if (iEv >= nEvents) break;
	    bmnTree->GetEntry(iEv);
//	    if (mtracks == NULL) { iEv++; continue; };
    	    gemdchtrack0 = (BmnGemDchTrack*)(mtracks->UncheckedAt(0));
//    	    gemdchtrack0 = (BmnGemDchTrack*)(mtracks->At(0));
//	    printf("First attempt, iEv = %d, iEv0 = %d\n",iEv,iEv0);
	    iEv++;
//	}
//	while (gemdchtrack0 == NULL);
//	printf("Point 0, iEv = %d\n",iEv);
	if (iEv >= nEvents) break;
//	trackEvId = gemdchtrack0->GetEventId();
/*
	trackEvId = ((BmnEventHeader*)eventHeader)->GetEventId();
//	printf("Point 1, iEv = %d, trackEvId = %d\n",iEv,trackEvId);
	if (eventHeader == NULL) continue;
//	trackRunId = ((BmnEventHeader*)eventHeader->UncheckedAt(0))->GetRunId();
	trackRunId = ((BmnEventHeader*)eventHeader)->GetRunId();
//	printf("Point 2, iEv = %d, trackRunId = %d\n",iEv,trackRunId);
	if (MCRunId == trackRunId && MCEvId > trackEvId) continue;
	if (MCRunId > trackRunId) continue;
//	printf("Point 3, iEv = %d\n",iEv);
	if (!(MCEvId==trackEvId && MCRunId==trackRunId))
	{
          do
	  { */
//	    bmnTreeMC->GetEntry(iEvMC);
//	    if (eventHeaderMC == NULL) break;
//	    MCEvId = ((BmnEventHeader*) eventHeaderMC->UncheckedAt(0))->GetEventId();
//	    MCRunId = ((BmnEventHeader*) eventHeaderMC->UncheckedAt(0))->GetRunId();
//	    cout << "Tracks Event:   " << iEv << ", MC Event: " << iEvMC << endl;
//	    cout << "Tracks EventId: " << trackEvId << ", MC EventId: " << MCEvId << endl;
//	    cout << "Tracks RunId: " << trackRunId << ", MC RunId: " << MCRunId << endl;
//	    iEvMC++;
//	    if (iEvMC >= nEventsMC) break;
//	    if (MCRunId > trackRunId) break;
//	    if (MCRunId == trackRunId && MCEvId > trackEvId) break;
//	  }
//	  while (!(MCEvId==trackEvId && MCRunId==trackRunId));
//	}
//	if (eventHeaderMC == NULL) continue;
//        if (!(MCEvId==trackEvId && MCRunId==trackRunId))
//	{
//	    cout << "Not found EventId " << trackEvId << " in MC Data file" << endl;
//	    cout << "Tracks Event:   " << iEv << ", MC Event: " << iEvMC << endl;
//	    cout << "Tracks RunId:   " << trackRunId << ", MC RunId:   " << MCRunId << endl;
//	    cout << "Tracks EventId: " << trackEvId <<  ", MC EventId: " << MCEvId << endl;
//	    if (iEvMC >= nEventsMC) break;
//	    if (MCRunId > trackRunId) continue;
//	    if (MCRunId == trackRunId && MCEvId > trackEvId) continue;
//	    break;
//	    continue;
//	}
	int istr[NUMBER_CHAMBERS] = {-1};

	for (int i = 0; i<NUMBER_CHAMBERS; i++)
	{
	    nhit[i] = 0;
	    for (int j=0; j<32; j++) nhits[i][j] = 0;
	    for (int j=0; j<32; j++) tof[i][j] = 0.;
	}
	nhi = 0;

	int nsts = stspoints->GetEntriesFast();
	hnsts->Fill(nsts);
	int ntof = tof700Digits->GetEntriesFast();
	hntof->Fill(ntof);
	int ntrmc = MCtracks->GetEntriesFast();
	CbmMCTrack *mctrack = NULL;
	float x = 0., y = 0., z = 0.;
	int hchamb[1000] = {0}, hstrip[1000] = {0};
	int hmctr[1000] = {0};
	int clchamb[1000] = {0}, clstrip[1000] = {0};
	Bool_t usedstrip[1000] = {false};
	Bool_t usedstrip1[1000] = {false};
	Int_t numhit[NUMBER_CHAMBERS][32] = {{0}};
	BmnTOFPoint *tofpoint[1000] = {NULL}, *tofpoin = NULL;
	Int_t toftrackid[1000] = {-1}, toftrid = -1;
    	Int_t indpoint = -1;
    	if (MCDEBUG) printf("\n*************** TOF hits %d %d **********************\n",iEv0, ntof);
	for (Int_t iDig = 0; iDig < ntof; ++iDig) {
    	    BmnTofHit *digit = (BmnTofHit*) tof700Digits->At(iDig);
    	    if (digit == NULL) continue;
	    UInt_t DetId = digit->GetDetectorID();
    	    Short_t plane = ((DetId&0xFFFF)>>8) - 1;
    	    Short_t planeexp = order[plane];
    	    Short_t strip = (DetId&0xFF) - 1;
    	    Float_t time = digit->GetTimeStamp();
//    	    if (iEv % 10 == 0 && iEv < 100) cout << "plane = " << plane << " plane(exp) = " << planeexp << " chamberID = " << idchambers[planeexp] << " strip = " << strip << " time = " << time << endl;
	    if (planeexp >= NUMBER_CHAMBERS) continue;
//	    TOF2->get_hit_xyzng(planeexp, strip, 0, &x, &y, &z);
//	    if (z == 0.) continue;
	    plane = planeexp;
	    htof->Fill(time);
	    hchamb[nhi] = plane;
	    hstrip[nhi] = strip;
	    nhit[plane]++;
	    nhits[plane][strip]++;
	    tof[plane][strip] = time;
	    istr[plane] = strip;
	    x = digit->GetX();
	    y = digit->GetY();
	    z = digit->GetZ();
	    xhi[nhi] = x;
	    yhi[nhi] = y;
	    zhi[nhi] = z;
	    hxytof->Fill(x,y);
	    TOF2->get_strip_xyz(plane, strip, &x, &y, &z);
	    xhic[nhi] = x;
	    yhic[nhi] = y;
	    zhic[nhi] = z;
	    hstr->Fill(strip);
	    hcha->Fill(plane);
	    numhit[plane][strip] = nhi;
    	    indpoint = digit->GetRefIndex();
    	    tofpoin = (BmnTOFPoint *)tofpoints->At(indpoint);
	    if (!tofpoin) continue;
	    toftrid = tofpoin->GetTrackID();
	    mctrack = NULL;
	    if (MCDEBUG) printf(" *********\n");
    	    if (toftrid >= 0) mctrack = (CbmMCTrack*) MCtracks->At(toftrid);
	    if (mctrack)
	    {
	    if (mctrack->GetMotherId() < 0 && mctrack->GetStartZ() < 1.f)
	    {
		if (MCDEBUG)
		{
			printf(" hit %d\n",iDig);
			digit->Print("");
			printf(" point %d\n",indpoint);
			tofpoin->Print("");
			printf(" track %d\n",toftrid);
			LOG(info) << "Track " << toftrid << ", mother : " << mctrack->GetMotherId()
        		<< ", Type " << mctrack->GetPdgCode() << ", momentum (" << mctrack->GetPx() << ", "
        		<< mctrack->GetPy() << ", " << mctrack->GetPz() << ") GeV";
			LOG(info) << ", vertex (" << mctrack->GetStartX() << ", "
        		<< mctrack->GetStartY() << ", " << mctrack->GetStartZ() << ") cm";
			LOG(info) << "       Ref " << mctrack->GetNPoints(kREF)
        		<< ", MVD "  << mctrack->GetNPoints(kMVD)
        		<< ", GEM "  << mctrack->GetNPoints(kGEM)
        		<< ", TOF1 " << mctrack->GetNPoints(kTOF1)
        		<< ", DCH1 " << mctrack->GetNPoints(kDCH)
        		<< ", TOF2 " << mctrack->GetNPoints(kTOF);
		}
    		tofpoint[nhi] = (BmnTOFPoint *)tofpoints->At(indpoint);
		toftrackid[nhi] = tofpoint[nhi]->GetTrackID();
	    }
	    else
	    {
		if (MCDEBUG)
		{
			printf(" reject hit %d\n",iDig);
			digit->Print("");
			printf(" reject point %d\n",indpoint);
			tofpoin->Print("");
			printf(" reject track %d\n",toftrid);
			LOG(info) << "Track " << toftrid << ", mother : " << mctrack->GetMotherId()
        		<< ", Type " << mctrack->GetPdgCode() << ", momentum (" << mctrack->GetPx() << ", "
        		<< mctrack->GetPy() << ", " << mctrack->GetPz() << ") GeV";
			LOG(info) << ", vertex (" << mctrack->GetStartX() << ", "
        		<< mctrack->GetStartY() << ", " << mctrack->GetStartZ() << ") cm";
			LOG(info) << "       Ref " << mctrack->GetNPoints(kREF)
        		<< ", MVD "  << mctrack->GetNPoints(kMVD)
        		<< ", GEM "  << mctrack->GetNPoints(kGEM)
        		<< ", TOF1 " << mctrack->GetNPoints(kTOF1)
        		<< ", DCH1 " << mctrack->GetNPoints(kDCH)
        		<< ", TOF2 " << mctrack->GetNPoints(kTOF);
		}
    		tofpoint[nhi] = NULL;
		toftrackid[nhi] = -10;
	    }
	    }
	    else
	    {
		if (MCDEBUG)
		{
		    printf(" hit %d\n",iDig);
		    digit->Print("");
		    printf(" point %d\n",indpoint);
		    tofpoin->Print("");
		    printf(" track %d null\n",toftrid);
		}
	    }
	    nhi++;
	} // tof700Digits loop
	hnhit->Fill(nhi);

	float xyz[3] = {0.}, cxyz[3] = {0.}, xrpc = 0., yrpc = 0., dz = 100., tx = 0., ty = 0., zcha = 0.;
	float beta = 1.0f, mass = 1.0f, mass2 = 1.0f, tcor = 0., tcorc = 0., tcors = 0., tcorspi = 0., tcorcpi = 0.;
	int nh = 0;
	int ntr = mtracks->GetEntriesFast();
	int nmatch = 0;
	if (MCMATCH) if (mcmatch) nmatch = mcmatch->GetEntriesFast();
	hntr->Fill(ntr);
	Ntr+=ntr;
	int ntr0 = 0, ntr700 = 0;
	int ntr1 = 0, ntr1g = 0;
	int iflag = 0, igem = 0, idch = 0;
	float chi2 = 0.f;
    	BmnGemDchTrack *gemdchtrack = NULL;
    	BmnTrack *track = NULL;
    	CbmStsTrack *gtrack = NULL; // Gleb GEM tracks
    	CbmTrackMatch *matchinfo = NULL; // Gleb GEM track match to MC
	Bool_t gooddch = false, goodgem = false, goodgemdch = false, gooddchtof = false, goodgemdchtof = false;
        Double_t xGem = 0.;
        Double_t yGem = 0.;
        Double_t zGem = 0.;
        Double_t txGem = 0.;
        Double_t tyGem = 0.;
        Double_t QpGem = 0.;
        Double_t QpDch = 0.;
        Int_t nGemHits = 0;
        Int_t nSiHits = 0;
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
	int himin = -1, himin1 = -1;
	double dx, dy, dl;
	double dist = 0., distmin = 1000000000.0f, distmin1 = 1000000000.0f, tofmin = 1000000000.0f;
	Float_t xc,yc,zcc;
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
	Int_t mctrid = -1, ntrue = 0, nwrong = 0, nfake = 0, nhall = 0, nmctr = 0;
	Float_t ptrue = 0.f;

	Double_t MCm = 0., MCp = 0., MCe = 0., MCmz = 0., MCpz = 0., MCl = 0., MCt = 0., sMCpz = 0., sMCl = 0., sMCt = 0., sMCb = 0., rMCmz2 = 0.;
	Int_t MCim = 0, MCrim = 0, ipart = 0;
	Int_t pdg = 0, MCn = 0, MCz = 0, MCnt = 0;
// sort by M/Z
	Double_t pfra[15000], pzfra[15000]={0.}, mfra[15000]={0.}, gfra[15000]={0.};
	Int_t index[15000] = {0};
	Double_t MCpmold = 100.;
	for (Int_t iTrmc = 0; iTrmc < ntrmc; ++iTrmc) {
    	    mctrack = (CbmMCTrack*) MCtracks->At(iTrmc);
    	    if (mctrack == NULL) continue;
	    pfra[iTrmc] = mctrack->GetP();
	    pdg = mctrack->GetPdgCode();
	    if (pdg < 1000000000)
	    {
		MCim = 0;
		if ( TDatabasePDG::Instance() ) {
		    TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(pdg);
		    if ( particle ) { MCm = particle->Mass(); /* printf("pdg %d ch %d ma %f\n",pdg,MCz,MCm);*/ }
		    else continue;
		    MCim = (int)(MCm/0.93);
		}
		else
		{
		    continue;
		}
	    }
	    else
	    {
		pdg -= 1000000000;
		pdg = pdg%10000000;
		pdg = pdg%10000;
		MCim = (int)(pdg/10);
	    }
	    mfra[iTrmc] = MCim;
	    gfra[iTrmc] = 0.;
	    if (MCim >= 1) gfra[iTrmc] = pfra[iTrmc]/MCim;
	}
	TMath::Sort(ntrmc, gfra, index, true);
//
    	if (MCDEBUG) printf("\n*************** MC tracks %d %d **********************\n", iEv0, ntrmc);
	Int_t iTrmc = 0;
	Int_t SumA = 0, SumA0 = 0;
	for (Int_t iTrmc0 = 0; iTrmc0 < ntrmc; ++iTrmc0) {
	    iTrmc = iTrmc0;
	    if (SORT) iTrmc = index[iTrmc0];
    	    mctrack = (CbmMCTrack*) MCtracks->At(iTrmc);
    	    if (mctrack == NULL) continue;
	    if (SORT)
	    {
		SumA0 += (int)mfra[iTrmc];
	    }
	    if (SORT) printf("iTrmc0 %d iTrmc %d gamma %f A %d SumA %d\n", iTrmc0, iTrmc, gfra[iTrmc], (int)mfra[iTrmc], SumA0 );
	    if (MCDEBUG)
	    {
		LOG(info) << "Track " << iTrmc << ", mother : " << mctrack->GetMotherId()
        	<< ", Type " << mctrack->GetPdgCode() << ", momentum (" << mctrack->GetPx() << ", "
    		<< mctrack->GetPy() << ", " << mctrack->GetPz() << ") GeV";
		LOG(info) << ", vertex (" << mctrack->GetStartX() << ", "
        	<< mctrack->GetStartY() << ", " << mctrack->GetStartZ() << ") cm";
		LOG(info) << "       Ref " << mctrack->GetNPoints(kREF)
        	<< ", MVD "  << mctrack->GetNPoints(kMVD)
        	<< ", GEM "  << mctrack->GetNPoints(kGEM)
        	<< ", TOF1 " << mctrack->GetNPoints(kTOF1)
        	<< ", DCH1 " << mctrack->GetNPoints(kDCH)
        	<< ", TOF2 " << mctrack->GetNPoints(kTOF);
	    }
	    if (mctrack->GetMotherId() >= 0) continue;
	    if (mctrack->GetStartZ() > 1.f) continue;
	    pdg = mctrack->GetPdgCode();
	    if (pdg < 1000000000)
	    {
		ipart = 1;
		MCim = 0;
//		if (true) continue;
		if ( TDatabasePDG::Instance() ) {
		    TParticlePDG* particle = TDatabasePDG::Instance()->GetParticle(pdg);
		    if ( particle ) { MCz = (int)(particle->Charge()/3.); MCm = particle->Mass(); /* printf("pdg %d ch %d ma %f\n",pdg,MCz,MCm);*/ }
		    else continue;
		    if (MCz <= 0) continue;
		    MCim = (int)(MCm/0.93);
		    MCmz = ((double)MCim)/MCz;
		}
		else
		{
		    continue;
		}
		if (MCm > 0.93)
		{
		    hMCamass->Fill(MCmz);
		    hMCmass0->Fill(MCmz);
		    hMCavsz->Fill(MCz,MCim);
		}
	    }
	    else
	    {
		ipart = 0;
		pdg -= 1000000000;
		pdg = pdg%10000000;
		MCz = pdg/10000;
		if (MCz <= 0) continue;
		pdg = pdg%10000;
		MCim = (int)(pdg/10);
		MCm = MCim*MP;
		MCmz = ((double)MCim)/MCz;
		if (MCm > 0.93)
		{
		    hMCamass->Fill(MCmz);
		    hMCrmass0->Fill(MCmz);
		    hMCravsz->Fill(MCz,MCim);
		}
	    }
	    Bool_t goodfra = true;
	    if (SORT && MCim >= 1)
	    {
		SumA += MCim;
		if (SumA > AARGON) goodfra = false; 
	    }
//	    if ((MCm = mctrack->GetMass()) < 0.93) continue;
	    MCp = mctrack->GetP();
	    if (((MCp/MCim) > MCpmold) && SORT && (MCim >= 1.))
	    {
//		printf("Previous gamma = %f, current one = %f, track #%d(%d), SumA = %d, IEv = %d !\n", MCpmold, MCp/MCim, iTrmc0, iTrmc, SumA, iEv0);
	    }
	    MCpmold = MCp/MCim;
	    if (MCp < 0.01) continue;
	    MCnt = mctrack->GetNPoints(kTOF);
	    MCn = mctrack->GetNPoints(kGEM);
//	    if (MCm > 0.93 && MCm < 1.07)
//	    {
//		hnhgem->Fill(MCn);
//		if (MCnt) hnhgem1->Fill(MCn);
//		else      hnhgem0->Fill(MCn);
//	    }
	    if (goodfra)
	    {
	    if (MCm > 0.93)
	    {
		pzfra[iTrmc0] = MCp/MCz;
		hMCpndf->Fill(MCp/MCz);
		hpangle->Fill(MCp/MCz, TMath::RadToDeg()*TMath::ACos(mctrack->GetPz()/MCp));
		hpanglep->Fill(MCp/MCz, TMath::RadToDeg()*TMath::ACos(mctrack->GetPz()/MCp));
		hangle->Fill(TMath::RadToDeg()*TMath::ACos(mctrack->GetPz()/MCp));
	    }
	    if (MCm > 0.93 && MCn < MINGEMNDF1)
	    {
		hMCpndf0->Fill(MCp/MCz);
	    }
	    if (MCm > 0.93 && MCn >= MINGEMNDF1)
	    {
		h3MCamass->Fill(MCmz);
		hMCpndf1->Fill(MCp/MCz);
		if (MCnt > 0) hMCpndf2->Fill(MCp/MCz);
	    }
	    if (MCnt < 1) continue;
	    if (MCm > 0.93 && ipart)
	    {
		    h4MCamass->Fill(MCmz);
		    hpangle2->Fill(MCp/MCz, TMath::RadToDeg()*TMath::ACos(mctrack->GetPz()/MCp));
		    hpanglep2->Fill(MCp/MCz, TMath::RadToDeg()*TMath::ACos(mctrack->GetPz()/MCp));
		    hangle2->Fill(TMath::RadToDeg()*TMath::ACos(mctrack->GetPz()/MCp));
	    }
	    if (MCm > 0.93 && !ipart)
	    {
		    h4MCamass->Fill(MCmz);
		    hpangle2->Fill(MCp/MCz, TMath::RadToDeg()*TMath::ACos(mctrack->GetPz()/MCp));
		    hpanglep2->Fill(MCp/MCz, TMath::RadToDeg()*TMath::ACos(mctrack->GetPz()/MCp));
		    hangle2->Fill(TMath::RadToDeg()*TMath::ACos(mctrack->GetPz()/MCp));
	    }
	    if (MCn < MINGEMNDF1) continue;
	    if (MCm > 0.93 && ipart)
	    {
		    h1MCamass->Fill(MCmz);
		    h1MCmass0->Fill(MCmz);
		    h1MCavsz->Fill(MCz,MCim);
		    hpangle1->Fill(MCp/MCz, TMath::RadToDeg()*TMath::ACos(mctrack->GetPz()/MCp));
		    hpanglep1->Fill(MCp/MCz, TMath::RadToDeg()*TMath::ACos(mctrack->GetPz()/MCp));
		    hangle1->Fill(TMath::RadToDeg()*TMath::ACos(mctrack->GetPz()/MCp));
	    }
	    if (MCm > 0.93 && !ipart)
	    {
		    h1MCamass->Fill(MCmz);
		    h1MCrmass0->Fill(MCmz);
		    h1MCravsz->Fill(MCz,MCim);
		    hpangle1->Fill(MCp/MCz, TMath::RadToDeg()*TMath::ACos(mctrack->GetPz()/MCp));
		    hpanglep1->Fill(MCp/MCz, TMath::RadToDeg()*TMath::ACos(mctrack->GetPz()/MCp));
		    hangle1->Fill(TMath::RadToDeg()*TMath::ACos(mctrack->GetPz()/MCp));
	    }
	    if (MCm > 0.93 && MCp < 2.0)
	    {
		    h2MCamass->Fill(MCmz);
	    }
	    }
	    MCe = mctrack->GetEnergy();
	    MCpz = MCp/MCz;
	    sMCpz = MCpz;// + gRandom->Gaus()*(MCpz*0.04);
	    Int_t hused[10000] = {0};
	    for (int ih = 0; ih<nhi; ih++)
	    {
		if (iTrmc == toftrackid[ih])
		{
		    MCl = tofpoint[ih]->GetLength();
		    if (MCl < 540.) continue;
		    MCt = tofpoint[ih]->GetTime();
		    sMCl = MCl;// + gRandom->Gaus()*5.;
		    sMCt = MCt;// + gRandom->Gaus()*0.115;
		    sMCb = sMCl/sMCt/cvel;
		    rMCmz2 = sMCpz*sMCpz*(1.f/sMCb/sMCb-1.f);
		    hMCmass2->Fill(rMCmz2);
		    hMCmass->Fill(TMath::Sqrt(rMCmz2));
		    if (hused[iTrmc] == 0)
		    {
			hMCmassr->Fill(MCmz);
			if (MCp < 2.0) hMCmassrp->Fill(MCmz);
			hused[iTrmc] = 1;
		    }
		    hpbetamc->Fill(sMCpz,sMCb);
		    if (pzfra[iTrmc0] > 7.) hp0xygem->Fill(tofpoint[ih]->GetX(),tofpoint[ih]->GetY());
		}
	    }
	    CbmStsPoint *stspoint = 0;
	    Double_t x_sts = 0., y_sts = 0., z_sts = 0.;
	    for (int ih = 0; ih<nsts; ih++)
	    {
		stspoint = (CbmStsPoint *)stspoints->At(ih);
		if (stspoint == NULL) continue;
		if (iTrmc == stspoint->GetTrackID())
		{
		    x_sts = stspoint->GetXIn();
		    y_sts = stspoint->GetYIn();
		    z_sts = stspoint->GetZIn();
		    if (pzfra[iTrmc0] > 7.) hpxysts->Fill(x_sts,y_sts);
		}
	    }


	}

	for (Int_t iTr = 0; iTr < ntr; ++iTr) {
    	    gemdchtrack = (BmnGemDchTrack*) mtracks->At(iTr);
    	    if (gemdchtrack == NULL) continue;
    	    track = (BmnTrack*) gemdchtrack->GetDchTrack();
	    gooddch = false;
    	    if (track != NULL)
    	    {
        	    QpDch = track->GetParamLast()->GetQp();
		    if (!TMath::IsNaN(QpDch) && (track->GetParamLast()->GetZ() > 500.f))
		    {
			// get xyz and cxyz here
    			xyz[0] = track->GetParamLast()->GetX();
    			xyz[1] = track->GetParamLast()->GetY();
    			xyz[2] = track->GetParamLast()->GetZ();
			tx = track->GetParamLast()->GetTx();
			ty = track->GetParamLast()->GetTy();
			cxyz[0] = tx/TMath::Sqrt(1.+tx*tx+ty*ty);
			cxyz[1] = ty/TMath::Sqrt(1.+tx*tx+ty*ty);
			cxyz[2] = 1./TMath::Sqrt(1.+tx*tx+ty*ty);
			xgoodtr = xyz[0]+tx*100.;
			ygoodtr = xyz[1]+ty*100.;
			hxydch->Fill(xgoodtr,ygoodtr);
			gooddch = true;
		    }
	    }
    	    Double_t mcp = 0.;
	    if (MCMATCH)
	    {
	        if (nmatch == 0) continue;
	        if (iTr >= nmatch) continue;
    	        matchinfo = (CbmTrackMatch*) mcmatch->At(iTr);
	        if (matchinfo == NULL) continue;
	        mctrid = matchinfo->GetMCTrackId();
	        /** Number of true hits on track (from matched MC track) **/
	        ntrue = matchinfo->GetNofTrueHits();
	        /** Number of wrong hits on track (from other MC tracks) **/
	        nwrong = matchinfo->GetNofWrongHits();
    	        /** Number of fake hits on track (from no MC track) **/
	        nfake = matchinfo->GetNofFakeHits();
        	/** Number of MCTracks with common hits **/
    	        nmctr = matchinfo->GetNofMCTracks();
    	        nhall = matchinfo->GetNofHits();
    	        ptrue = matchinfo->GetTrueOverAllHitsRatio();
    	        htruehits->Fill(ptrue);
	        hallhits->Fill(nhall);
	        //if (iEv < 10) printf("mcid %d true %d wrong %d fake %d Nmctr %d quality %f\n", mctrid, ntrue, nwrong, nfake, nmctr, ptrue);
	        mctrack = NULL;
    	        if (mctrid >= 0 && mctrid < ntrmc) mctrack = (CbmMCTrack *) MCtracks->At(mctrid);
    	        if (mctrack) mcp = mctrack->GetP();
	    }
    	    gtrack = (CbmStsTrack *) gemdchtrack->GetGemTrack();  // Merts GEM tracks
	    goodgem = false;
    	    if (gtrack != NULL && (ptrue >= MINTRUE || !MCMATCH))
    	    {
// Selection criteria
        	QpGem = gtrack->GetParamLast()->GetQp();
		if (!TMath::IsNaN(QpGem))
		{
		    if (QpGem != 0.)
		    {
			hp0->Fill(1./QpGem);
		    }
		    else
		    {
//			hp0->Fill(0.);
		    }
//		    ntr1++;
        	    xGem = gtrack->GetParamLast()->GetX();
        	    yGem = gtrack->GetParamLast()->GetY();
        	    zGem = gtrack->GetParamLast()->GetZ();
        	    txGem = gtrack->GetParamLast()->GetTx();
        	    tyGem = gtrack->GetParamLast()->GetTy();
//        	    nGemHits = gtrack->GetNDF(); // Merts GEM tracks
		    nGemHits = gemdchtrack->GetGemHits(); // Gleb GEM tracks
            	    nSiHits = gemdchtrack->GetSiHits(); // Gleb GEM tracks
		    Xpv = gemdchtrack->GetXpv();
		    Ypv = gemdchtrack->GetYpv();

        	    if (GEMCUTS)
        	    {
//		     Xpv   < -3.5f  || Xpv   > 4.0f || Ypv   < -1.0f  || Ypv > 6.0f || 
		     Bool_t goodpv = true;
		     if (PRIMARY_VERTEX_CUTS)
		     {
		        if(ntr < 2) goodpv = false;
    		        if(primaryVertex->GetX() < -1 || primaryVertex->GetX() > 1 || \
    		           primaryVertex->GetY() < -1 || primaryVertex->GetY() > 1 || \
    		           primaryVertex->GetZ() < -5 || primaryVertex->GetZ() > 3) goodpv = false;
		     };
                     if ((!goodpv)  || nGemHits < MINGEMNDF || nSiHits < MINSIHITS || \
	    	     xGem  < -140.f || xGem > 240.f || yGem < -40.f  || yGem > 200.f || \
		     txGem < -0.5f  || txGem > 1.0f || tyGem < -0.1f || tyGem > 0.5f || \
		     Xpv   < -1.0f  || Xpv   > 1.0f || Ypv   < -1.0f  || Ypv > 1.0f || \
		     QpGem == 0.0f  || \
            	     1.0f/TMath::Abs(QpGem) > 15.0f || 1.0f/TMath::Abs(QpGem) < 0.15f || zGem < 500.f)
        	     {
        		if (GEMDEBUG) printf("Nhits %d xGem %f yGem %f zGem %f txGem %f tyGem %f Xpv %f Ypv %f QpGem %f\n", nGemHits, xGem, yGem, zGem, txGem, tyGem, Xpv, Ypv, QpGem);
    		     }
		     else
		     {
			if (GEMDEBUG) printf("Good GEM track!\n");
			if (ACCEPTANCE)
			{
			    Double_t xgema = xGem+txGem*100.;
			    Double_t ygema = yGem+tyGem*100.;
			    if (xgema >= xmintof700 && xgema <= xmaxtof700 && ygema >= ymintof700 && ygema <= ymaxtof700)
			    {
				goodgem = true;
    				le = gemdchtrack->GetLength() - ztarg;
				p = QpGem;
    				ps = p;
				if (p != 0.) p = 1./p;
				hptr->Fill(p); // GeV/c
				hltr->Fill(le); // cm
    				pmod = TMath::Abs(p);
				hxygem->Fill(xGem,yGem);
				if (!MCMATCH)  hpxygem->Fill(xGem,yGem);
				else if (pzfra[mctrid] > 7.)  hpxygem->Fill(xGem,yGem);
			    }
			}
			else
			{
			    goodgem = true;
    			    le = gemdchtrack->GetLength() - ztarg;
			    p = QpGem;
    			    ps = p;
			    if (p != 0.) p = 1./p;
			    hptr->Fill(p); // GeV/c
			    hltr->Fill(le); // cm
    			    pmod = TMath::Abs(p);
			    hxygem->Fill(xGem,yGem);
			    if (!MCMATCH)  hpxygem->Fill(xGem,yGem);
			    else if (pzfra[mctrid] > 7.)  hpxygem->Fill(xGem,yGem);
			}
		     }
		    }
		    else
		    {
                      if ( nGemHits >= MINGEMNDF && nSiHits >= MINSIHITS && QpGem != 0.0f  && zGem >= 500.f)
		      {
			if (ACCEPTANCE)
			{
			    Double_t xgema = xGem+txGem*100.;
			    Double_t ygema = yGem+tyGem*100.;
			    if (xgema >= xmintof700 && xgema <= xmaxtof700 && ygema >= ymintof700 && ygema <= ymaxtof700)
			    {
				goodgem = true;
    				le = gemdchtrack->GetLength() - ztarg;
				p = QpGem;
    				ps = p;
				if (p != 0.) p = 1./p;
				hptr->Fill(p); // GeV/c
				hltr->Fill(le); // cm
    				pmod = TMath::Abs(p);
				hxygem->Fill(xGem,yGem);
				if (!MCMATCH)  hpxygem->Fill(xGem,yGem);
				else if (pzfra[mctrid] > 7.)  hpxygem->Fill(xGem,yGem);
			    }
			}
			else
			{
			    goodgem = true;
    			    le = gemdchtrack->GetLength() - ztarg;
			    p = QpGem;
    			    ps = p;
			    if (p != 0.) p = 1./p;
			    hptr->Fill(p); // GeV/c
			    hltr->Fill(le); // cm
    			    pmod = TMath::Abs(p);
			    hxygem->Fill(xGem,yGem);
			    if (!MCMATCH)  hpxygem->Fill(xGem,yGem);
			    else if (pzfra[mctrid] > 7.)  hpxygem->Fill(xGem,yGem);
			}
		      }
		    }
//		    if (iEv < 10 && goodgem && mctrack != NULL) printf("MC momentum %f REC momentum %f\n", mcp,p);
    		    if (track == NULL)
    		    {
//		     Xpv   < -3.5f  || Xpv   > 4.0f || Ypv   < -1.0f  || Ypv > 6.0f || 
		     Bool_t goodpv = true;
		     if (PRIMARY_VERTEX_CUTS)
		     {
		        if(ntr < 2) goodpv = false;
    		        if(primaryVertex->GetX() < -1 || primaryVertex->GetX() > 1 || \
    		           primaryVertex->GetY() < -1 || primaryVertex->GetY() > 1 || \
    		           primaryVertex->GetZ() < -5 || primaryVertex->GetZ() > 3) goodpv = false;
		     };
                     if ((!goodpv)  || nGemHits < MINGEMNDF || nSiHits < MINSIHITS || \
	    	     xGem  < -140.f || xGem > 240.f || yGem < -40.f  || yGem > 200.f || \
		     txGem < -0.5f  || txGem > 1.0f || tyGem < -0.1f || tyGem > 0.5f || \
		     Xpv   < -1.0f  || Xpv   > 1.0f || Ypv   < -1.0f  || Ypv  > 1.0f || \
		     QpGem == 0.0f  || \
            	     1.0f/TMath::Abs(QpGem) > 15.0f || 1.0f/TMath::Abs(QpGem) < 0.15f || zGem < 500.f)
		     {
        	      QpDch = QpGem;
		      if (!TMath::IsNaN(QpDch) && (zGem > 500.f))
		      {
			// get xyz and cxyz here
    			xyz[0] = xGem;
    			xyz[1] = yGem;
    			xyz[2] = zGem;
			tx = txGem;
			ty = tyGem;
			cxyz[0] = tx/TMath::Sqrt(1.+tx*tx+ty*ty);
			cxyz[1] = ty/TMath::Sqrt(1.+tx*tx+ty*ty);
			cxyz[2] = 1./TMath::Sqrt(1.+tx*tx+ty*ty);
			xgoodtr = xyz[0]+tx*100.;
			ygoodtr = xyz[1]+ty*100.;
			if (ACCEPTANCE)
			{
			    if (xgoodtr >= xmintof700 && xgoodtr <= xmaxtof700 && ygoodtr >= ymintof700 && ygoodtr <= ymaxtof700)
			    {
				hxydch->Fill(xgoodtr,ygoodtr);
				gooddch = true;
			    }
			}
		      }
		     }
		    }
		}
	    }
	    goodgemdch = false;
	    int nrej1 = 0, nrej2 = 0, nrej3 = 0, nrej4 = 0, nrej5 = 0, nrej6 = 0;
	    if (goodgem && gooddch)
	    {
		ntr1++;
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
	        hchi2m->Fill(Chi2m);
	        chi2 = dxm*dxm/Ddx + dym*dym/Ddy;
	        hchi2->Fill(chi2);
// Selection criteria - x and y tracks matching
	        if (dxm >= -DXGEMDCHMAX && dxm <= +DXGEMDCHMAX && dym >= -DYGEMDCHMAX && dym <= +DYGEMDCHMAX/* && Chi2m <= 30.f && chi2 > 30.f*/)
	        {
		    ntr1g++;
	    	    hdxy->Fill(dxm,dym);
		    hxygoodtr->Fill(xgoodtr,ygoodtr);
		    hxygoodtrgem->Fill(xGem,yGem);
		    hpgemdch->Fill(p); // GeV/c
		    goodgemdch = true;
		}
	    }

// find corresponding strip (if any)
/*	    if (USETRUEHIT)
	    {
		    nh = nhi;
		    himin = -1;
		    distmin = 1000000000.0f;
		    tofmin = 1000000000.0f;
		    for (int ih = 0; (ih<nh) && goodgemdch; ih++)
		    {
			if (usedstrip[ih]) continue;
			if (mctrid == toftrackid[ih])
			{
			    if (tof[hchamb[ih]][hstrip[ih]] < tofmin)
			    {
				himin = ih;
				distmin = 0.;
				tofmin = tof[hchamb[ih]][hstrip[ih]];
			    }
			}
		    }

		    nh = nhi;
		    himin1 = -1;
		    distmin1 = 1000000000.0f;
		    tofmin = 1000000000.0f;
		    for (int ih = 0; (ih<nh) && gooddch; ih++)
		    {
			if (usedstrip1[ih]) continue;
			if (mctrid == toftrackid[ih])
			{
			    if (tof[hchamb[ih]][hstrip[ih]] < tofmin)
			    {
				himin1 = ih;
				distmin1 = 0.;
				tofmin = tof[hchamb[ih]][hstrip[ih]];
			    }
			}
		    }
	    }
	    else
	    { */
		    nh = nhi;
		    himin = -1;
		    dist = 0.;
		    distmin = 1000000000.0f;
		    for (int ih = 0; (ih<nh) && goodgemdch; ih++)
		    {
			if (usedstrip[ih]) continue;
			if (mctrid != toftrackid[ih] && USETRUEHIT) { nrej1++; continue; }
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
			    if (TMath::Abs(dx) > DXMAXPAN) { nrej2++; continue; }
			    if (TMath::Abs(dy) > DYMAXPAN) { nrej3++; continue; }
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

		    nh = nhi;
		    himin1 = -1;
		    dist = 0.;
		    distmin1 = 1000000000.0f;
		    for (int ih = 0; (ih<nh) && gooddch; ih++)
		    {
			if (usedstrip1[ih]) continue;
			if (mctrid != toftrackid[ih] && USETRUEHIT) continue;
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
			if (dist < distmin1)
			{
			    distmin1 = dist;
			    himin1 = ih;
			}
		    }
//	    }
// DCH+TOF700 matching
// Selection criteria on minimal distance track-hit
// Selection criteria time-of-flight
	    gooddchtof = false;
	    if (himin1 >= 0)
	    {
		ic = hchamb[himin1];
		is = hstrip[himin1];
		if (distmin1 <= DISTMAX && tof[ic][is] <= TOFMAX)
		{
		    gooddchtof = true;
		    usedstrip1[himin1] = true;
		    hxygoodhit1->Fill(xgoodtr,ygoodtr);
		    hxygoodhit1a->Fill(xhi[himin1],yhi[himin1]);
		}
	    }
// GEM+DCH+TOF700 matching
// Selection criteria on minimal distance track-hit
// Selection criteria time-of-flight
	    goodgemdchtof = false;
	    if (himin >= 0)
	    {
		ic = hchamb[himin];
		is = hstrip[himin];
		if (distmin > DISTMAX) nrej5++;
		if (tof[ic][is] > TOFMAX) nrej6++;
		if (distmin <= DISTMAX && tof[ic][is] <= TOFMAX)
		{
		    ntr0++;
		    goodgemdchtof = true;
		    usedstrip[himin] = true;
		    ntr700++;
		    hdxytof->Fill(dxm,dym);
		    hpgoodhit->Fill(p); // GeV/c
		    xc = xhic[himin];
		    yc = yhic[himin];
		    zcc = zhic[himin];
		    dz = zhi[himin] - xyz[2];
		    xrpc = xyz[0] + dz*tx;
		    yrpc = xyz[1] + dz*ty;
		    hxygoodhit->Fill(xgoodtr,ygoodtr);
		    hxygoodhita->Fill(xhi[himin],yhi[himin]);

		    hxyhit->Fill(xrpc-xhi[himin],yrpc-yhi[himin]);
		    hxdiff->Fill(xhi[himin]-xc);
		    if (idchambers[ic] < 100.f) hxxhits->Fill(xhi[himin]-xc,xrpc-xc);
		    if (idchambers[ic] > 100.f) hxxhitb->Fill(xhi[himin]-xc,xrpc-xc);
	
		    cosa = sqrt(1.+tx*tx+ty*ty);
		    dl = dz*cosa;
		    lenr = le+dl;
		    hltra->Fill(lenr);
		    hidist = TMath::Sqrt(xc*xc+yc*yc+(zcc-ztarg)*(zcc-ztarg));
		    hstrtr->Fill(is);
		    hchatr->Fill(ic);
		    hxytrtof->Fill(xrpc,yrpc);

		    htoftr->Fill(tof[ic][is]);
//
		    tofr = tof[ic][is];
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
			tcorc = tcor;
			tcors = tcor;
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
			tcorcpi = tcorpi;
			tcorspi = tcorpi;
			tcalibrc->Fill(ic,tcorcpi);
			tcalibrs->Fill(ic,is,tcorspi);
		    }
                    tofrc = tofr;
                    tofrs = tofr;
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
		} // hit is good!
		hrej5->Fill(nrej5);
		hrej6->Fill(nrej6);
	    } // hit exist!
	    Double_t rej1 = (Double_t)nrej1/nhi;
	    Double_t rej2 = (Double_t)nrej2/nhi;
	    Double_t rej3 = (Double_t)nrej3/nhi;
	    hrej1->Fill(rej1);
	    hrej2->Fill(rej2);
	    hrej3->Fill(rej3);
	} // tracks loop
	hntr1->Fill(ntr1);
	Ntr1+=ntr1;
	Ntr1g+=ntr1g;
	hntr0->Fill(ntr0);
	Ntr0+=ntr0;
	Ntr700+=ntr700;
        //t_out->Fill();
	if (iEv%50000==0)
	{
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

	    hxyeffa->Divide(hxygoodhita,hxygoodtr);
	    hxeffa->Divide((projtmp = hxygoodhita->ProjectionX()),(projtmp1 = hxygoodtr->ProjectionX()));
	    if (projtmp) delete projtmp;
	    if (projtmp1) delete projtmp1;
	    hyeffa->Divide((projtmp = hxygoodhita->ProjectionY()),(projtmp1 = hxygoodtr->ProjectionY()));
	    if (projtmp) delete projtmp;
	    if (projtmp1) delete projtmp1;

	    hpeff->Divide(hpgoodhit,hpgemdch);
	    hpeffmg->Divide(hpgemdch,hptr);
	    hxyeffmg->Divide(hxygoodtrgem,hxygem);
	    hxyeffmd->Divide(hxygoodtr,hxydch);
	    hxyeffdg->Divide(hxydch,hxygem);
	    hxyeffgd->Divide(hxygem,hxydch);
	    hxyeffm1d->Divide(hxygoodhit1,hxydch);
	    hxyeffm1da->Divide(hxygoodhit1a,hxydch);
	    hxybadhit->Add(hxygoodtr,hxygoodhit,1.,-1.);
	    hxybadhita->Add(hxygoodtr,hxygoodhita,1.,-1.);
	    hpbadhit->Add(hpgemdch,hpgoodhit,1.,-1.);

	    h1MCeffmass->Divide(h1MCamass,hMCamass);
	    h2MCeffmass->Divide(h2MCamass,hMCamass);
	    h3MCeffmass->Divide(h3MCamass,hMCamass);
	    h4MCeffmass->Divide(h4MCamass,hMCamass);
	    hMCeffmass->Divide(hMCmassr,hMCamass);
	    hMCeffmassp->Divide(hMCmassrp,hMCamass);
	    hangleff->Divide(hangle1,hangle);
	    hangleff2->Divide(hangle2,hangle);

//	    TH1F *hpnohit = new TH1F("hpnohit","Momentum of GEM+DCH+NoTOF700 tracks", 1000,-10,10);

	    printf("Number of tracks %d, per event %f\n", Ntr, (float)Ntr/nEvents);
	    printf("GEM+DCH tracks %d, per event %f (%f%% per track)\n", Ntr1, (float)Ntr1/nEvents, Ntr>0?(float)Ntr1/Ntr*100.f:0.f);
	    printf("Good GEM+DCH tracks %d, per event %f (%f%% per valid track)\n", Ntr1g, (float)Ntr1g/nEvents, Ntr1>0?(float)Ntr1g/Ntr1*100.f:0.f);
	    printf("Matched GEM+DCH tracks %d,  per event %f (%f%% per good track)\n", Ntr0, (float)Ntr0/nEvents, Ntr1g>0?(float)Ntr0/Ntr1g*100.f:0.f);
//	    printf("Matched GEM+DCH+TOF700 tracks %d,  per event %f (%f%% per matched track)\n", Ntr700, (float)Ntr700/nEvents, Ntr0>0?(float)Ntr700/Ntr0*100.f:0.f);

	    if (f) {f->Write(NULL, TObject::kOverwrite);}
	}
    } // event loop

//    TH1D *projtmp = 0, *projtmp1 = 0;
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

    hxyeffa->Divide(hxygoodhita,hxygoodtr);
    hxeffa->Divide((projtmp = hxygoodhita->ProjectionX()),(projtmp1 = hxygoodtr->ProjectionX()));
    if (projtmp) delete projtmp;
    if (projtmp1) delete projtmp1;
    hyeffa->Divide((projtmp = hxygoodhita->ProjectionY()),(projtmp1 = hxygoodtr->ProjectionY()));
    if (projtmp) delete projtmp;
    if (projtmp1) delete projtmp1;

    hpeff->Divide(hpgoodhit,hpgemdch);
    hpeffmg->Divide(hpgemdch,hptr);
    hxyeffmg->Divide(hxygoodtrgem,hxygem);
    hxyeffmd->Divide(hxygoodtr,hxydch);
    hxyeffdg->Divide(hxydch,hxygem);
    hxyeffgd->Divide(hxygem,hxydch);
    hxyeffm1d->Divide(hxygoodhit1,hxydch);
    hxyeffm1da->Divide(hxygoodhit1a,hxydch);
    hxybadhit->Add(hxygoodtr,hxygoodhit,1.,-1.);
    hxybadhita->Add(hxygoodtr,hxygoodhita,1.,-1.);
    hpbadhit->Add(hpgemdch,hpgoodhit,1.,-1.);

    h1MCeffmass->Divide(h1MCamass,hMCamass);
    h2MCeffmass->Divide(h2MCamass,hMCamass);
    h3MCeffmass->Divide(h3MCamass,hMCamass);
    h4MCeffmass->Divide(h4MCamass,hMCamass);
    hMCeffmass->Divide(hMCmassr,hMCamass);
    hMCeffmassp->Divide(hMCmassrp,hMCamass);
    hangleff->Divide(hangle1,hangle);
    hangleff2->Divide(hangle2,hangle);

//    TH1F *hpnohit = new TH1F("hpnohit","Momentum of GEM+DCH+NoTOF700 tracks", 1000,-10,10);

	printf("Number of tracks %d, per event %f\n", Ntr, (float)Ntr/nEvents);
	printf("GEM+DCH tracks %d, per event %f (%f%% per track)\n", Ntr1, (float)Ntr1/nEvents, Ntr>0?(float)Ntr1/Ntr*100.f:0.f);
	printf("Good GEM+DCH tracks %d, per event %f (%f%% per valid track)\n", Ntr1g, (float)Ntr1g/nEvents, Ntr1>0?(float)Ntr1g/Ntr1*100.f:0.f);
	printf("Matched GEM+DCH tracks %d,  per event %f (%f%% per good track)\n", Ntr0, (float)Ntr0/nEvents, Ntr1g>0?(float)Ntr0/Ntr1g*100.f:0.f);
//	printf("Matched GEM+DCH+TOF700 tracks %d,  per event %f (%f%% per matched track)\n", Ntr700, (float)Ntr700/nEvents, Ntr0>0?(float)Ntr700/Ntr0*100.f:0.f);

    if (f) {f->Write(NULL, TObject::kOverwrite);}

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
//	float idchambers[59] = { 27.1,28.1,3.1,1.1,29.1,4.1,33.1,30.1,5.1,19.3,31.1,6.1,2.1,32.1,15.2,16.2,17.2,
//	18.2,19.2,20.2,7.1,115.2,113.1,117.1,35.1,9.1,37.1,11.1,39.1,13.1,34.1,8.1,36.1,10.1,38.1,12.1,21.2,
//	23.2,25.2,22.2,24.2,26.2,107.2,108.2,109.2,110.2,111.2,112.2,114.1,116.2,118.1,14.1,40.1,119.2,120.2,
//	121.2,122.2,123.2,124.2 };
	bool notused[59] = {true};
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
