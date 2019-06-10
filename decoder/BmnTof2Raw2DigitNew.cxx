#include "TFitResult.h"
#include "TF1.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TProfile2D.h"
#include "TH2D.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TFile.h"
#include "TFitResult.h"
#include "FairLogger.h"
#include "BmnTof2Raw2DigitNew.h"

// Digitization stage flags
//      use hits in time limits
#define CHECK_SLEWING 0
#define TIME_LIMITS_VS_WIDTH 0
#define USE_PRELIMINARY_OFFSETS 1
#define USE_FIT_SLEWING 0
// USE_FINAL_OFFSETS 0 - don't apply final strip offsets
// USE_FINAL_OFFSETS 1 - apply final strip offsets (calculated as mean)
// USE_FINAL_OFFSETS 2 - apply final strip offsets (calculated by time peak fit)
#define USE_FINAL_OFFSETS 1
//

// Slewing Time limits flags
#define LIMITS_ROOT_FILE 1
#define PRINT_TIME_LIMITS 1

// Strip equalization before slewing
#define PRELIMINARY_OFFSETS 1
#define EQUAL_MAXIMA0 1
#define FIT_MAXIMA0 1
#define PRINT_EQUALIZATION0_RESULTS 0
#define DEBUG_EQUALIZATION0 1
//

// Slewing flags
//      Use for slewing plots not all hits in chamber but hit with maximal width
#define MAX_STRIP 0
#define FIT_SLEWING 0
#define SKIP_SIDES 0
#define MAX_FIT_ATTEMPTS 5
#define MIN_FIT_ENTRIES 10
#define SMOOTH 0
#define FIT0 "Q0RS+"
#define FIT "Q0RS+"
#define FITI "Q0RS+"
#define FITI0 "Q0RS+"
#define SUBRANGES 0
#define SUBDEG 4
#define ITERATIONS 1
#define ITERATIONS0 1
#define DRAW_BAD 0
#define DRAW_GOOD 0
#define PRINT_SLEWING_RESULTS 0
#define PRINT_SLEWING_PARAMETERS 0
#define PRINT_FIT_RESULTS 0
//

// Strip equalization after slewing
#define FINAL_OFFSETS 1
#define TOZERO 1
#define EQUAL_MAXIMA 1
#define FIT_MAXIMA 1
#define PRINT_EQUALIZATION_RESULTS 0
#define DEBUG_EQUALIZATION 1
//

// Cables offsets
#define CABLE_OFFSETS 0
#define TRUE_OFFSETS 1
#define ALL_CORRECTIONS 1
#define DRAW_OFFSETS 0
#define PRINT_CORRECTIONS 0
//


//module IDs
const UInt_t kTDC64VHLE = 0x53;
const UInt_t kTDC72VHL = 0x12;
const UInt_t kTDC32VL = 0x11;

BmnTof2Raw2DigitNew::BmnTof2Raw2DigitNew(){
    n_rec=0;
}

BmnTof2Raw2DigitNew::BmnTof2Raw2DigitNew(TString mappingFile, TString RunFile, Int_t SlewingRun, UInt_t SlewingChamber, TString geomFile) {

    char *delim = 0, name[128], title[128];
    n_rec=0;
    TString dummy;

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/input/";
    strcpy(filname_base, gSystem->BaseName(RunFile.Data()));
    if ((delim = strrchr(filname_base, (int)'.'))) *delim = '\0';

    int RUN = 0;
    const char *fname = RunFile.Data();
    sscanf(&fname[strlen(fname) - 13], "%d", &RUN);
//    printf("Run number string is %s\n", &fname[strlen(fname) - 13]);
    if (RUN <= 0)
    {
	printf("Run file: %s - can't extract run number!\n", RunFile.Data());
	return;
    }
    fRUN = RUN;

    ifstream in;
    in.open((path + mappingFile).Data());
    if (!in.is_open())
    {
	printf("Loading TOF700 Map from file: %s - file open error!\n", mappingFile.Data());
	return;
    }

    fSlewCham = SlewingChamber;
    fSlewRun = fRUN;
    if (SlewingRun > 0)
    {
	fSlewRun = SlewingRun;
	sprintf((char *)&filname_base[strlen(filname_base) - 8], "%04d_raw", SlewingRun);
	if (SlewingChamber == 0) printf("Loading TOF700 Map from file: %s , reference slewing run %d\n",mappingFile.Data(), SlewingRun);
	else                     printf("Loading TOF700 Map from file: %s , reference slewing run %d, chamber %d\n",mappingFile.Data(), SlewingRun, SlewingChamber);
    }
    else if (SlewingRun < 0)
    {
	fSlewRun = SlewingRun;
	printf("Loading TOF700 Map from file: %s , reference slewing run %d\n",mappingFile.Data(), fRUN);
//	printf("Loading TOF700 Map from file: %s\n",mappingFile.Data());
//	printf("Loading TOF700 Map from file: %s, without slewing corrections!\n",mappingFile.Data());
    }
    else
    {
	printf("Loading TOF700 Map from file: %s , reference slewing run %d\n",mappingFile.Data(), fRUN);
//	printf("Loading TOF700 Map from file: %s\n",mappingFile.Data());
    }

    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++)
    {
	strcpy(&filname_slewing[i][0],filname_base);
	chamber_slewing[i] = fSlewCham;
	type_slewing[i] = 0;
    }
    MaxPlane = 0;
    TString dnlfile;
//    char dnlfile[128];
    int crate, slot, chan, plane, strip, side, filetype, cham, cable;
    float idcham;
    int id_crate;

    for (int c=0; c<TOF2_MAX_CRATES; c++)
	for (int i=0; i<TOF2_MAX_SLOTS_IN_CRATE; i++)
	    dnltype[c][i] = -1;

    for (int c=0; c<TOF2_MAX_CRATES; c++)
	for (int s=0; s<TOF2_MAX_SLOTS_IN_CRATE; s++)
	    for (int ch=0; ch<TOF2_MAX_CHANNELS_IN_SLOT; ch++)
		nrec[c][s][ch] = -1;

    in >> dummy;
    in >> ncrates;
    in >> dummy >> dummy;
    for (int i = 0; i < ncrates; i++)
    {
    	    in >> std::dec >> crate >> std::hex >> id_crate;
    	    if (!in.good()) break;
	    if (crate >= TOF2_MAX_CRATES) break;
	    numcrates[i] = crate;
	    idcrates[crate] = id_crate;
	    if (in.eof()) break;
    }
    in >> dummy;
    in >> std::dec >> nslots;
//    printf("%d\n",nslots);
    in >> dummy >> dummy >> dummy >> dummy;
//    printf("%s\n",dummy.Data());
//        for (int i = 0; i < TOF2_MAX_SLOTS_IN_CRATE; i++)
    for (int i = 0; i < (nslots+1); i++)
    {
    	    in >> crate >> slot >> filetype >> dnlfile;
//	    printf("%d %d %d %s\n",crate,slot,filetype,dnlfile.Data());
    	    if (!in.good()) break;
	    if (crate >= TOF2_MAX_CRATES) break;
	    if (slot >= TOF2_MAX_SLOTS_IN_CRATE) break;
	    numcr[i] = crate;
	    numslots[i] = slot;
	    dnltype[crate][slot] = filetype;
	    strcpy(&dnlname[crate][slot][0], path.Data());
	    strcat(&dnlname[crate][slot][0], dnlfile.Data());
	    if (in.eof()) break;
    }

    in >> dummy;
    in >> nchambers;
//    printf("N_CHAMBERS = %d\n",nchambers);
    in >> dummy >> dummy;
//    printf("%s\n",dummy.Data());
    char line[256] = {""};
    in.getline(line,128);
    for (int i = 0; i < nchambers; i++)
    {
	    in.getline(line,128);
//    	    in >> cham >> idcham;
	    sscanf(line,"%d\t\t%f\n",&cham,&idcham);
//	    printf("%d %d %d %s\n",crate,slot,filetype,dnlfile.Data());
    	    if (!in.good()) break;
	    if (cham >= TOF2_MAX_CHAMBERS) break;
	    numcha[i] = cham;
	    idchambers[cham] = idcham;
	    if (in.eof()) break;
    }

    if (!in.eof()) in >> dummy >> dummy >> dummy >> dummy;
//    printf("%s\n",dummy.Data());
	in.getline(line,128);
//	printf("line %d %s\n", strlen(line),line);
    while (!in.eof()) {
//	in.getline(line,128);
//	printf("line %d %s\n", strlen(line),line);
	in.getline(line,128);
//	printf("line %d %s\n", strlen(line),line);
	sscanf(line,"%d\t%d\t%d\t%d\n",&crate,&slot,&plane,&cable);
//        in >> crate >> slot >> plane >> cable;
//	printf("%d %d %d %d\n",crate, slot, plane, cable);
        if (!in.good()) break;
	if (crate >= TOF2_MAX_CRATES) break;
	if (slot >= TOF2_MAX_SLOTS_IN_CRATE) break;
	if (plane >= TOF2_MAX_CHAMBERS) break;
	if (cable == 0)
	{
	    for (int ie=1; ie<=2; ie++)
	    {
	      for (int is=0; is<TOF2_MAX_STRIPS_IN_CHAMBER/2; is++)
	      {
		mapa[n_rec].id=idcrates[crate];
		mapa[n_rec].crate=crate;
		mapa[n_rec].slot=slot;
		mapa[n_rec].plane=plane;
		mapa[n_rec].pair=-2;
		chan = is + (ie-1)*16;
		mapa[n_rec].chan=chan;
		mapa[n_rec].strip=is;
		mapa[n_rec].side=ie; 
		nrec[crate][slot][chan] = n_rec;
		n_rec++;
	      }
	    }
	    for (int ie=1; ie<=2; ie++)
	    {
	      for (int is=TOF2_MAX_STRIPS_IN_CHAMBER/2; is<TOF2_MAX_STRIPS_IN_CHAMBER; is++)
	      {
		mapa[n_rec].id=idcrates[crate];
		mapa[n_rec].crate=crate;
		mapa[n_rec].slot=slot;
		mapa[n_rec].plane=plane;
		mapa[n_rec].pair=-2;
		chan = 16 + is + (ie-1)*16;
		mapa[n_rec].chan=chan;
		mapa[n_rec].strip=is;
		mapa[n_rec].side=ie; 
		nrec[crate][slot][chan] = n_rec;
		n_rec++;
	      }
	    }
	}
	else if (cable == 1)
	{
	    for (int ie=1; ie<=2; ie++)
	    {
	      for (int is=0; is<TOF2_MAX_STRIPS_IN_CHAMBER/2; is++)
	      {
		mapa[n_rec].id=idcrates[crate];
		mapa[n_rec].crate=crate;
		mapa[n_rec].slot=slot;
		mapa[n_rec].plane=plane;
		mapa[n_rec].pair=-2;
		chan = is + (ie-1)*16;
		mapa[n_rec].chan=chan;
		mapa[n_rec].strip=is;
		mapa[n_rec].side=ie; 
		nrec[crate][slot][chan] = n_rec;
		n_rec++;
	      }
	    }
	}
	else if (cable == 2)
	{
	    for (int ie=1; ie<=2; ie++)
	    {
	      for (int is=0; is<TOF2_MAX_STRIPS_IN_CHAMBER/2; is++)
	      {
		mapa[n_rec].id=idcrates[crate];
		mapa[n_rec].crate=crate;
		mapa[n_rec].slot=slot;
		mapa[n_rec].plane=plane;
		mapa[n_rec].pair=-2;
		chan = 32 + is + (ie-1)*16;
		mapa[n_rec].chan=chan;
		mapa[n_rec].strip=is;
		mapa[n_rec].side=ie; 
		nrec[crate][slot][chan] = n_rec;
		n_rec++;
		if (n_rec >= TOF2_MAX_CHANNEL) break;
	      }
	      if (n_rec >= TOF2_MAX_CHANNEL) break;
	    }
	}
	else
	{
	    printf("Wrong cable flag, line is %s\n",line);
	    continue;
	}
	if (n_rec >= TOF2_MAX_CHANNEL) break;
	if (plane > MaxPlane) MaxPlane = plane;
    }
    if (n_rec) MaxPlane++;
    in.close();

// read channels offsets
    for (int c=0; c<10; c++)
	for (int i=0; i<25; i++)
	{
	    for (int j=0; j<64; j++)
		    chtima[c][i][j] = 0.;
	}
    poffsets  = 0;
    poffsets1 = 0;
    poffsets2 = 0;
    if (CABLE_OFFSETS)
    {
	poffsets = new TH2F("poffsets","Offset corrections",32,0,32,200,-100,100);
	poffsets->SetMarkerStyle(21);
	poffsets->SetMarkerColor(kRed);
	if (!ALL_CORRECTIONS)
	{
	    poffsets1 = new TH2F("poffsets1","Offset corrections",32,0,32,200,-100,100);
	    poffsets1->SetMarkerStyle(20);
	    poffsets1->SetMarkerColor(kGreen);
	    poffsets2 = new TH2F("poffsets1","Offset corrections",32,0,32,200,-100,100);
	    poffsets2->SetMarkerStyle(22);
	    poffsets2->SetMarkerColor(kBlue);
	}
	if(Offsets_read()) return;
    }
    readGeom((char *)geomFile.Data());

// read Left-Right offsets
    for (int c=0; c<TOF2_MAX_CHAMBERS; c++)
	for (int i=0; i<TOF2_MAX_STRIPS_IN_CHAMBER; i++)
	{
		    lroffsets[c][i] = 0.;
		    lrsign[c][i] = +1;
	}
    readLRoffsets((char *)"TOF700_left_right_offsets.txt");
    // 19.5 cm/ns
    fVelosity = 19.5f;

    for(int ind=0;ind<n_rec;ind++){ 
     if (mapa[ind].pair == -1) continue; 
     for(int ind1=0;ind1<n_rec;ind1++){
	if (ind == ind1) continue; 
        if(mapa[ind1].plane == mapa[ind].plane && mapa[ind1].strip == mapa[ind].strip){
	    mapa[ind].pair = ind1; mapa[ind1].pair = -1;
	    break;
       }
     }
    }
//  read INL/DNL correction files
    for (int c=0; c<TOF2_MAX_CRATES; c++)
	for (int i=0; i<TOF2_MAX_SLOTS_IN_CRATE; i++)
	{
	    for (int j=0; j<72; j++)
		for (int k=0; k<1024; k++)
		    DNL_Table[c][i][j][k] = 0.;
	}
    DNL_read();
    
    Wcut = 2800;
    Wmax = 7800;
    WT0min = 720;
    WT0max = 820;

    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) LeadMin[i] = -5000;
    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) LeadMax[i] = +5000;
    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) for (int j = 0; j < 500; j++) LeadMinW[i][j] = -5000;
    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) for (int j = 0; j < 500; j++) LeadMaxW[i][j] = +5000;
    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) Wcutc[i] = -1;
    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) Wmaxc[i] = -1;

//    BmnSetup setup = BmnRawDataDecoder::instance()->GetBmnSetup();
//    if (setup == kSRCSETUP)
//	T0shift = -15000.; // SRC
//    else
//	T0shift = +147000.; // BMN

//    printf("Run number is %d\n",RUN);
//    getchar();
    int run = RUN;
    if (run >= 9000) run -= 4000;
    if (run >= 7000) run -= 5000;
    if (run <= 3588)
	T0shift = -15000.; // SRC
    else
	T0shift = +147000.; // BMN

    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) numstrip[i] = -1;

    Wt0 = 0;
    Wts = 0;

    for (int i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
	TvsS[i] = 0;
	WvsS[i] = 0;
	TvsWall[i] = 0;
	TvsWallmax[i] = 0;
	for (int ir=0; ir<4; ir++)
	{
	    TvsW[i][ir] = 0;
	    TvsWt0[i][ir] = 0;
	    TvsWr[i][ir] = 0;
	    TvsWt0r[i][ir] = 0;
	    TvsWp[i][ir] = 0;
	    TvsWt0p[i][ir] = 0;
	    TvsSm[i][ir] = 0;
	}
    }

    fSlewing = 0;
}

//BmnTof2Raw2DigitNew destructor
BmnTof2Raw2DigitNew::~BmnTof2Raw2DigitNew() {

    if (Wt0) delete Wt0;
    Wt0 = 0;

    if (Wts) delete Wts;
    Wts = 0;

    for (int i=0; i<MaxPlane; i++)
    {
	if (TvsS[i]) delete TvsS[i];
	TvsS[i] = 0;
	if (WvsS[i]) delete WvsS[i];
	WvsS[i] = 0;
    }
    for (int i=0; i<MaxPlane; i++)
    {
	if (TvsWall[i]) delete TvsWall[i];
	TvsWall[i] = 0;
	if (TvsWallmax[i]) delete TvsWallmax[i];
	TvsWallmax[i] = 0;
    }

    if (fSlewing == 0) return;
    
    for (int i=0; i<MaxPlane; i++)
    {
	for (int ir=0; ir<4; ir++)
	{
	    if (TvsW[i][ir]) delete TvsW[i][ir];
	    TvsW[i][ir] = 0;
	    if (TvsWt0[i][ir]) delete TvsWt0[i][ir];
	    TvsWt0[i][ir] = 0;
	    if (TvsWp[i][ir]) delete TvsWp[i][ir];
	    TvsWp[i][ir] = 0;
	    if (TvsWt0p[i][ir]) delete TvsWt0p[i][ir];
	    TvsWt0p[i][ir] = 0;
	    if (TvsSm[i][ir]) delete TvsSm[i][ir];
	    TvsSm[i][ir] = 0;
	}
    }
}

void BmnTof2Raw2DigitNew::SetSlewingReference(Int_t chamber, Int_t refrun, Int_t refchamber)
{
	if (chamber <= 0 || chamber > TOF2_MAX_CHAMBERS)
	{
	    printf("Wrong slewing chamber number %d\n", chamber);
	    return;
	}
	if (refchamber <= 0 || refchamber > TOF2_MAX_CHAMBERS)
	{
	    printf("Wrong slewing reference chamber number %d\n", refchamber);
	    return;
	}
	if (refrun < 0 || refrun > 9999)
	{
	    printf("Wrong slewing reference run number %d\n", refrun);
	    return;
	}
	sprintf((char *)&filname_slewing[chamber-1][strlen((char *)&filname_slewing[chamber-1][0]) - 8], "%04d_raw", refrun);
	chamber_slewing[chamber-1] = refchamber;
	type_slewing[chamber-1] = 1;
	printf("Chamber %d(%.1f) - use slewing reference run number %d, reference chamber %d(%.1f)\n", \
	       chamber, idchambers[chamber-1], refrun, refchamber, idchambers[refchamber-1]);
	return;
}

int BmnTof2Raw2DigitNew::Offsets_read()
{
   int numch[3200] = {0};
   int numcab[3200] = {0};
   char foname[128];
   FILE *fin = 0;
   TString dir = getenv("VMCWORKDIR");
   TString path = dir + "/input/TOF700_channel_offsets.dat";
   strcpy(foname, path.Data());
   fin = fopen(foname,"r");
   if (fin == NULL)
   {
    printf("Can't open file %s!\n", foname);
    return 1;
   }

   int n = 0, i = 0, j = 0, k = 0, idcr = 0, kold = -1;
   float idch = 777., idchold = -1.;
   double cht = 777., sum  = 0., tim[32] = {0.};
   int ni = 0, nl = 0, nc = 0;
   if (PRINT_CORRECTIONS) printf("\n     ********** Corrections table **************************\n\n");
   while((ni=fscanf(fin, "Chamber %f Crate %d(0x%x) Slot %d Channel %d Offset %lf\n", &idch, &i, &idcr, &j, &k, &cht)) == 6)
   {
    numch[(int)(idch*10.+0.1)] = 1;
    chtima[i][j][k] = cht;
    if (TRUE_OFFSETS) chtima[i][j][k] = cht - (-30.61 + 1.915*(k%32));
    n++;
//    printf("%s\n",line);
//    printf("ni = %d, n = %d\n", ni, n);
//    printf("n = %d ni = %d idch = %.1f i = %d idcr = 0x%0x j = %d k = %d offset = %f\n", n, ni, idch, i, idcr, j, k, cht);
    if (idchold != -1.)
    {
      if ( idch != idchold || nl >= 32 )
      {
        numcab[(int)(idchold*10.)]++;
	if ( idchold != 19.3f || 0)
	{
	if ( (kold == 31 && (idchold == 19.2f || idchold == 18.2f || idchold == 16.2f)) || ALL_CORRECTIONS)
	for (int l=0;l<nl;l++)
	{
	    tim[l] = tim[l] - sum/nl;
	    if (ALL_CORRECTIONS) poffsets->Fill(l,tim[l]);
	    else
	    {
		 if ( idchold == 19.2f ) poffsets->Fill(l,tim[l]);
		 if ( idchold == 18.2f ) poffsets1->Fill(l,tim[l]);
		 if ( idchold == 16.2f ) poffsets2->Fill(l,tim[l]);
	    }
	}
	}
	if ( (idchold != 19.3f && PRINT_CORRECTIONS) || 0)
	{
	    if (kold < 32)
	     printf("          Chamber %.1f (01-32) average = %f\n", idchold, nl > 0 ? sum/nl : 0.);
	    else
	     printf("          Chamber %.1f (33-64) average = %f\n", idchold, nl > 0 ? sum/nl : 0.);
	}
	sum = 0.;
	nl = 0;
	if ( idchold != 19.3f ) nc++;
      }
    }
    sum += chtima[i][j][k];
    if (nl < 32) tim[nl] = chtima[i][j][k];
    nl++;
    idchold = idch;
    kold = k;
   };
//last card
   if ( nl > 0 )
   {
        numcab[(int)(idchold*10.)]++;
	if ( idchold != 19.3f || 0)
	{
	 if ( (kold == 31 && (idchold == 19.2f || idchold == 18.2f || idchold == 16.2f)) || ALL_CORRECTIONS)
	 {
	  for (int l=0;l<nl;l++)
	  {
	    tim[l] = tim[l] - sum/nl;
	    if (ALL_CORRECTIONS)
	    {
		poffsets->Fill(l,tim[l]);
	    }
	    else
	    {
		 if ( idchold == 19.2f ) poffsets->Fill(l,tim[l]);
		 if ( idchold == 18.2f ) poffsets1->Fill(l,tim[l]);
		 if ( idchold == 16.2f ) poffsets2->Fill(l,tim[l]);
	    }
	  }
	 }
	}
	if ( (idchold != 19.3f && PRINT_CORRECTIONS) || 0)
	{
	    if (kold < 32)
	     printf("          Chamber %.1f (01-32) average = %f\n", idchold, nl > 0 ? sum/nl : 0.);
	    else
	     printf("          Chamber %.1f (33-64) average = %f\n", idchold, nl > 0 ? sum/nl : 0.);
	}
	sum = 0.;
	nl = 0;
	if ( idchold != 19.3f ) nc++;
   }
//
//   printf("ni = %d idch = %f i = %d idcr = 0x%0x j = %d k = %d offset = %f\n", ni, idch, i, idcr, j, k, cht);
   fclose(fin);
   int ndch = 0;
   for (i=0; i<3200; i++) if (numch[i] > 0) ndch++;
   if (PRINT_CORRECTIONS) printf("\n     *******************************************************\n");
   if (PRINT_CORRECTIONS) printf("Number of lines with channel offsets %d, cables %d, chambers %d\n\n", n, nc, ndch);
   if (DRAW_OFFSETS)
   {
    if (ALL_CORRECTIONS) poffsets->Draw();
    else
    {
	poffsets->Draw();
	poffsets1->Draw("same");
	poffsets2->Draw("same");
    }
    gPad->Update();
   }
   return 0;
}

void BmnTof2Raw2DigitNew::Book()
{
    char name[128], title[128];

    gStyle->SetOptFit(111);
    gStyle->SetOptStat(111111);

    sprintf(name, "Width_T0");
    sprintf(title, "Width T0");
    Wt0 = new TH1F(name,title,1500,0,1500);

    sprintf(name, "TS_difference");
    sprintf(title, "TS difference");
    Wts = new TH1F(name,title,400,-200,200);

    sprintf(name, "Chamber_rate");
    sprintf(title, "Chamber hit rate");
    Hchambrate = new TH2F(name,title, 30,-150, +150, 20, -100, 100);

    sprintf(name, "Chamber_id_rate");
    sprintf(title, "ChamberID hit rate");
    H1chambrate = new TH1F(name,title, 150,0,150);

    sprintf(name, "Strip_rate");
    sprintf(title, "Strip hit rate");
    Hstriprate = new TH2F(name,title, 300,-150, +150, 2000, -100, 100);

    for (int i=0; i<MaxPlane; i++)
    {
	sprintf(name, "Time_vs_Strip_Chamber_%.1f",idchambers[i]);
	sprintf(title, "Time vs Strip Chamber %.1f",idchambers[i]);
// SRC
	TvsS[i] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,5000, 0., 10000.);
// BMN
//	TvsS[i] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,5000, 0., 10000.);
//	TvsS[i]->SetDirectory(0);
	sprintf(name, "Width_vs_Strip_Chamber_%.1f",idchambers[i]);
	sprintf(title, "Width vs Strip Chamber %.1f",idchambers[i]);
	WvsS[i] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,Wmax/50, 0., Wmax);
//	WvsS[i]->SetDirectory(0);
    }
    for (int i=0; i<MaxPlane; i++)
    {
	sprintf(name, "Time_vs_Width_Chamber_%.1f_all",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f all",idchambers[i]);
// SRC
	TvsWall[i] = new TH2F(name,title,Wmax/50,0,Wmax, 5000, 0., 10000.);
// BMN
//	TvsWall[i] = new TH2F(name,title,Wmax/50,0,Wmax, 5000, 0., 10000.);
//	TvsWall[i]->SetDirectory(0);
	sprintf(name, "Time_vs_Width_Chamber_%.1f_max",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f, max strip",idchambers[i]);
// SRC
	TvsWallmax[i] = new TH2F(name,title,Wmax/50,0,Wmax, 5000, 0., 10000.);
// BMN
//	TvsWallmax[i] = new TH2F(name,title,Wmax/50,0,Wmax, 5000, 0., 10000.);
//	TvsWallmax[i]->SetDirectory(0);
    }

    sprintf(name, "Time_offsets0");
    sprintf(title, "Time offsets0");
    Toffsets0 = new TProfile(name,title, TOF2_MAX_CHANNEL,0,TOF2_MAX_CHANNEL,-10000.,10000.,"e");

    printf(" TOF700 booking done for %d chambers!\n", MaxPlane);
}

void BmnTof2Raw2DigitNew::BookSlewing()
{
    char name[128], title[128];

    gStyle->SetOptFit(111);
    gStyle->SetOptStat(111111);

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/parameters/tof2_slewing/";
    char filnr[128];
    sprintf(filnr, "%s%s", path.Data(), filname_base);
    strcat(filnr, ".slewing.root");
    fSlewing = new TFile(filnr,"RECREATE");

    sprintf(name, "Width_T0");
    sprintf(title, "Width T0");
    Wt0 = new TH1F(name,title,1500,0,1500);

    sprintf(name, "TS_difference");
    sprintf(title, "TS difference");
    Wts = new TH1F(name,title,400,-200,200);

    sprintf(name, "Chamber_rate");
    sprintf(title, "Chamber hit rate");
    Hchambrate = new TH2F(name,title, 30,-150, +150, 20, -100, 100);

    sprintf(name, "Chamber_id_rate");
    sprintf(title, "ChamberID hit rate");
    H1chambrate = new TH1F(name,title, 150,0,150);

    sprintf(name, "Strip_rate");
    sprintf(title, "Strip hit rate");
    Hstriprate = new TH2F(name,title, 300,-150, +150, 2000, -100, 100);

    for (int ir=0; ir<4; ir++)
    {
	sprintf(name, "Time_offsets_region_%d", ir+1);
	sprintf(title, "Time offsets region %d", ir+1);
	Toffsets[ir] = new TProfile(name,title, TOF2_MAX_CHANNEL,0,TOF2_MAX_CHANNEL,-60.,+60.,"e");
	sprintf(name, "Time_fit_offsets_region_%d", ir+1);
	sprintf(title, "Time fit_offsets region %d", ir+1);
	Toffsetsf[ir] = new TProfile(name,title, TOF2_MAX_CHANNEL,0,TOF2_MAX_CHANNEL,-60.,+60.,"e");
    }


    for (int i=0; i<MaxPlane; i++)
    {
      float dL = (LeadMax[i]-LeadMin[i])/2.;
      for (int ir=0; ir<4; ir++)
      {
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_%d",idchambers[i],ir+1);
	sprintf(title, "Time vs Width Chamber %.1f Peak %d",idchambers[i],ir+1);
	if (TvsW[i][ir]) delete TvsW[i][ir];
	TvsW[i][ir] = new TProfile2D(name,title,Wmax/50,0,Wmax,32,0,32,-(LeadMax[i]-LeadMin[i]),+(LeadMax[i]-LeadMin[i]),"e");
	TvsW[i][ir]->Sumw2(kTRUE);
//	TvsW[i][ir]->SetDirectory(0);
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_%d",idchambers[i],ir+1);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak %d",idchambers[i],ir+1);
	if (TvsWt0[i][ir]) delete TvsWt0[i][ir];
	TvsWt0[i][ir] = new TProfile2D(name,title,(WT0max-WT0min)/5,WT0min,WT0max,32,0,32,LeadMin[i]-dL,LeadMax[i]+dL,"e");
	TvsWt0[i][ir]->Sumw2(kTRUE);
//	TvsWt0[i][ir]->SetDirectory(0);
/*
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_%d_plot",idchambers[i],ir+1);
	sprintf(title, "Time vs Width Chamber %.1f Peak %d plot",idchambers[i],ir+1);
	if (TvsWp[i][ir]) delete TvsWp[i][ir];
	TvsWp[i][ir] = new TH2F(name,title,Wmax/20,0,Wmax,2*(LeadMax[i]-LeadMin[i]),-(LeadMax[i]-LeadMin[i]),+(LeadMax[i]-LeadMin[i]));
//	TvsWp[i][ir]->SetDirectory(0);
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_%d_plot",idchambers[i],ir+1);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak %d plot",idchambers[i],ir+1);
	if (TvsWt0p[i][ir]) delete TvsWt0p[i][ir];
	TvsWt0p[i][ir] = new TH2F(name,title,(WT0max-WT0min),WT0min,WT0max,2*(LeadMax[i]-LeadMin[i]),LeadMin[i]-dL,LeadMax[i]+dL);
//	TvsWt0p[i][ir]->SetDirectory(0);

*/
	sprintf(name, "Time_vs_Strip_Chamber_%.1f_Peak_%d_Maxima",idchambers[i],ir+1);
	sprintf(title, "Time vs Strip Chamber %.1f Peak %d Maxima",idchambers[i],ir+1);
	if (TvsSm[i][ir]) delete TvsSm[i][ir];
// SRC
	TvsSm[i][ir] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,2*(LeadMax[i]-LeadMin[i]),-(LeadMax[i]-LeadMin[i]),+(LeadMax[i]-LeadMin[i]));
//	TvsSm[i][ir] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,10000, 0., 10000.);
// BMN
//	TvsSm[i][ir] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,10000, 0., 10000.);
//	TvsSm[i][ir]->SetDirectory(0);

      } // regions
    } // chambers

    sprintf(name, "Time_Lead_Min");
    sprintf(title, "Time Lead Minimal for slewing");
    pLeadMin = new TProfile(name,title, TOF2_MAX_CHAMBERS,0,TOF2_MAX_CHAMBERS,-10000.,10000.,"e");
    sprintf(name, "Time_Lead_Max");
    sprintf(title, "Time Lead Maximal for slewing");
    pLeadMax = new TProfile(name,title, TOF2_MAX_CHAMBERS,0,TOF2_MAX_CHAMBERS,-10000.,10000.,"e");
    for (int i=0; i<MaxPlane; i++)
    {
	pLeadMin->Fill(i,LeadMin[i]);
	pLeadMax->Fill(i,LeadMax[i]);
    }
    sprintf(name, "Time_offsets0");
    sprintf(title, "Time offsets0");
    Toffsets0 = new TProfile(name,title, TOF2_MAX_CHANNEL,0,TOF2_MAX_CHANNEL,-10000.,10000.,"e");
    for (int i=1; i<=Toffsets0->GetNbinsX(); i++)
    {
	Toffsets0->SetBinContent(i, Toffsets00[i]);
	Toffsets0->SetBinEntries(i, Toffsets00n[i]);
	Toffsets0->SetBinError(i, Toffsets00e[i]);
    }
    pWlimits = new TH1F("Wlimits","Width limits",4,0,4);
    pWlimits->Fill(0.f,(double)Wcut);
    pWlimits->Fill(1.f,(double)Wmax);
    pWlimits->Fill(2.f,(double)WT0min);
    pWlimits->Fill(3.f,(double)WT0max);
}

void BmnTof2Raw2DigitNew::BookSlewingResults()
{
    if (!CHECK_SLEWING) return;

    char name[128], title[128];

    gStyle->SetOptFit(111);
    gStyle->SetOptStat(111111);

    sprintf(name, "Width_T0");
    sprintf(title, "Width T0");
    Wt0 = new TH1F(name,title,1500,0,1500);
    Wt0->SetDirectory(0);

    sprintf(name, "TS_difference");
    sprintf(title, "TS difference");
    Wts = new TH1F(name,title,400,-200,200);
    Wts->SetDirectory(0);

    sprintf(name, "Chamber_rate");
    sprintf(title, "Chamber hit rate");
    Hchambrate = new TH2F(name,title, 30,-150, +150, 20, -100, 100);
    Hchambrate->SetDirectory(0);

    sprintf(name, "Chamber_id_rate");
    sprintf(title, "ChamberID hit rate");
    H1chambrate = new TH1F(name,title, 150,0,150);
    H1chambrate->SetDirectory(0);

    sprintf(name, "Strip_rate");
    sprintf(title, "Strip hit rate");
    Hstriprate = new TH2F(name,title, 300,-150, +150, 2000, -100, 100);
    Hstriprate->SetDirectory(0);

    pWlimits = new TH1F("Wlimitsr","Width limits",4,0,4);
    pWlimits->Fill(0.f,(double)Wcut);
    pWlimits->Fill(1.f,(double)Wmax);
    pWlimits->Fill(2.f,(double)WT0min);
    pWlimits->Fill(3.f,(double)WT0max);
    pWlimits->SetDirectory(0);

    for (int i=0; i<MaxPlane; i++)
    {
      for (int ir=0; ir<4; ir++)
      {
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_%d_slewing",idchambers[i],ir+1);
	sprintf(title, "Time vs Width Chamber %.1f Peak %d after slewing",idchambers[i],ir+1);
	if (TvsWr[i][ir]) delete TvsWr[i][ir];
	TvsWr[i][ir] = new TProfile(name,title,Wmax/50,0,Wmax,-2.f*(LeadMax[i]-LeadMin[i]),+2.f*(LeadMax[i]-LeadMin[i]),"e");
	TvsWr[i][ir]->Sumw2(kTRUE);
	TvsWr[i][ir]->SetDirectory(0);
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_%d_slewing",idchambers[i],ir+1);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak %d after slewing",idchambers[i],ir+1);
	if (TvsWt0r[i][ir]) delete TvsWt0r[i][ir];
	TvsWt0r[i][ir] = new TProfile(name,title,(WT0max-WT0min)/5,WT0min,WT0max,-2.f*(LeadMax[i]-LeadMin[i]),+2.f*(LeadMax[i]-LeadMin[i]),"e");
	TvsWt0r[i][ir]->Sumw2(kTRUE);
	TvsWt0r[i][ir]->SetDirectory(0);
      }
    }
}

void BmnTof2Raw2DigitNew::ReBook(int i)
{
    char name[128], title[128];
    int Wc = Wcut, Wm = Wmax;
    if (Wcutc[i] >= 0.) Wc = Wcutc[i];
    if (Wmaxc[i] >= 0.) Wm = Wmaxc[i];
    float dL = (LeadMax[i]-LeadMin[i])/2.;
    for (int ir=0; ir<4; ir++)
    {
      if (TvsW[i][ir])
      {	
	delete TvsW[i][ir];
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_%d",idchambers[i],ir+1);
	sprintf(title, "Time vs Width Chamber %.1f Peak %d",idchambers[i],ir+1);
	TvsW[i][ir] = new TProfile2D(name,title,Wm/50,0,Wm,32,0,32,-(LeadMax[i]-LeadMin[i]),+(LeadMax[i]-LeadMin[i]),"e");
	TvsW[i][ir]->Sumw2(kTRUE);
//	TvsW[i][ir]->SetDirectory(0);
      }
      if (TvsWt0[i][ir])
      {
	delete TvsWt0[i][ir];
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_%d",idchambers[i],ir+1);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak %d",idchambers[i],ir+1);
	TvsWt0[i][ir] = new TProfile2D(name,title,(WT0max-WT0min)/5,WT0min,WT0max,32,0,32,LeadMin[i]-dL,LeadMax[i]+dL,"e");
	TvsWt0[i][ir]->Sumw2(kTRUE);
//	TvsWt0[i][ir]->SetDirectory(0);
      }
/*
      if (TvsWp[i][ir])
      {	
	delete TvsWp[i][ir];
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_%d_plot",idchambers[i],ir+1);
	sprintf(title, "Time vs Width Chamber %.1f Peak %d plot",idchambers[i],ir+1);
	TvsWp[i][ir] = new TH2F(name,title,Wm/50,0,Wm,2*(LeadMax[i]-LeadMin[i]),-(LeadMax[i]-LeadMin[i]),+(LeadMax[i]-LeadMin[i]));
      }
      if (TvsWt0p[i][ir])
      {
	delete TvsWt0p[i][ir];
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_%d",idchambers[i],ir+1);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak %d",idchambers[i],ir+1);
	TvsWt0p[i][ir] = new TH2F(name,title,(WT0max-WT0min)/5,WT0min,WT0max,2*(LeadMax[i]-LeadMin[i]),LeadMin[i]-dL,LeadMax[i]+dL);
      }
*/
      if (TvsSm[i][ir])
      {
	delete TvsSm[i][ir];
	sprintf(name, "Time_vs_Strip_Chamber_%.1f_Peak_%d_Maxima",idchambers[i],ir+1);
	sprintf(title, "Time vs Strip Chamber %.1f Peak %d Maxima",idchambers[i],ir+1);
// SRC
	TvsSm[i][ir] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,2*(LeadMax[i]-LeadMin[i]),-(LeadMax[i]-LeadMin[i]),+(LeadMax[i]-LeadMin[i]));
//	TvsSm[i][ir] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,5000, 0., +10000.);
// BMN
//	TvsSm[i][ir] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,5000, 0., +10000.);
//	TvsSm[i][ir]->SetDirectory(0);
      }

    } // regions

    if (TvsWall[i])
    {
	delete TvsWall[i];
	sprintf(name, "Time_vs_Width_Chamber_%.1f_all",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f all",idchambers[i]);
// SRC
	TvsWall[i] = new TH2F(name,title,Wm/50,0,Wm, 5000, 0., 10000.);
// BMN
//	TvsWall[i] = new TH2F(name,title,Wm/50,0,Wm, 5000, 0., 10000.);
    }
    if (TvsWallmax[i])
    {
	delete TvsWallmax[i];
	sprintf(name, "Time_vs_Width_Chamber_%.1f_max",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f, max strip",idchambers[i]);
// SRC
	TvsWallmax[i] = new TH2F(name,title,Wm/50,0,Wm, 5000, 0., 10000.);
// BMN
//	TvsWallmax[i] = new TH2F(name,title,Wm/50,0,Wm, 5000, 0., 10000.);
    }
//    printf("c %d wc %d wm %d wt1 %d wt2 %d lmi %d lma %d\n", i, Wcut, Wmax, WT0min, WT0max, LeadMin[i], LeadMax[i]);
}

void BmnTof2Raw2DigitNew::WritePreparationHists()
{
    TFile *f = NULL;
    char filn[128];

//    TH1F *hW = new TH1F("Wlimits","Width limits",4,0,4);
//    hW->Fill(0.f,(double)Wcut);
//    hW->Fill(1.f,(double)Wmax);
//    hW->Fill(2.f,(double)WT0min);
//    hW->Fill(3.f,(double)WT0max);

    sprintf(filn, "%s_preparation.root", filname_base);
    f = new TFile(filn,"RECREATE","Preparation Results of BmnTOF700");
    Wt0->Write();
    Wts->Write();
    Hchambrate->Write();
    H1chambrate->Write();
    Hstriprate->Write();
    for (int i=0; i<MaxPlane; i++)
    {
	if (TvsS[i]) TvsS[i]->Write();
	if (WvsS[i]) WvsS[i]->Write();
	if (TvsWall[i]) TvsWall[i]->Write();
	if (TvsWallmax[i]) TvsWallmax[i]->Write();
    }
    if (Toffsets0) Toffsets0->Write();
//    hW->Write();
    f->Close();
    delete f;
}

void BmnTof2Raw2DigitNew::WritePreparationHists0()
{
    TFile *f = NULL;
    char filn[128];

//    TH1F *hW = new TH1F("Wlimits","Width limits",4,0,4);
//    hW->Fill(0.f,(double)Wcut);
//    hW->Fill(1.f,(double)Wmax);
//    hW->Fill(2.f,(double)WT0min);
//    hW->Fill(3.f,(double)WT0max);

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/parameters/tof2_slewing/";
    sprintf(filn, "%s%s_preparation.root", path.Data(), filname_base);
    f = new TFile(filn,"RECREATE","Preparation Results of BmnTOF700");
    Wt0->Write();
    Wts->Write();
    Hchambrate->Write();
    H1chambrate->Write();
    Hstriprate->Write();
    for (int i=0; i<MaxPlane; i++)
    {
	if (TvsS[i]) TvsS[i]->Write();
	if (WvsS[i]) WvsS[i]->Write();
	if (TvsWall[i]) TvsWall[i]->Write();
	if (TvsWallmax[i]) TvsWallmax[i]->Write();
    }
    if (Toffsets0) Toffsets0->Write();
//    hW->Write();
    f->Close();
    delete f;
}

void BmnTof2Raw2DigitNew::WriteSlewingResults()
{
    if (fSlewing)
    {
	fSlewing->Close();
	fSlewing = 0;
    }
    if (!CHECK_SLEWING) return;
    if (fSlewRun < 0) return;
    TFile *f = NULL;
    char filn[128];
    char rightname[128];
    strcpy(rightname,filname_base);
    sprintf((char *)&rightname[strlen(rightname) - 8], "%04d_raw", fRUN);
    if (fSlewRun)
	sprintf(filn, "%s_slewing%d_results.root", rightname, fSlewRun);
    else
	sprintf(filn, "%s_slewing_results.root", rightname);
    f = new TFile(filn,"RECREATE","Slewing Results of BmnTOF700");
    Wt0->Write();
    Wts->Write();
    Hchambrate->Write();
    H1chambrate->Write();
    Hstriprate->Write();
//    pLeadMin->Write();
//    pLeadMax->Write();
    pWlimits->Write();
    for (int i=0; i<MaxPlane; i++)
    {
      for (int ir=0; ir<4; ir++)
      {
	if (TvsWr[i][ir]) TvsWr[i][ir]->Write();
	if (TvsWt0r[i][ir]) TvsWt0r[i][ir]->Write();
//	if (TvsWp[i][ir]) TvsWp[i][ir]->Write();
//	if (TvsWt0p[i][ir]) TvsWt0p[i][ir]->Write();
//	if (TvsSm[i][ir]) TvsSm[i][ir]->Write();
      }
    }
    f->Close();
    delete f;
}

void BmnTof2Raw2DigitNew::writeSlewingLimits()
{
  FILE *fout = 0;
  int i, im, y;
  float ymin, ymax; // xmin, xmax;
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  char filn[128];
  sprintf(filn, "%s%s_slewing_limits.txt", path.Data(), filname_base);
  fout = fopen(filn,"w");

//  fprintf(fout,"\t\tTOF2.SetW(%d,%d);\n", Wcut, Wmax);
//  if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetW(%d,%d);\n", Wcut, Wmax);
//  fprintf(fout,"\t\tTOF2.SetWT0(%d,%d);\n", WT0min, WT0max);
//  if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetWT0(%d,%d);\n", WT0min, WT0max);

  TH1D *h = 0;

  for (i=0; i<MaxPlane; i++)
    {
      if (MAX_STRIP)
      {
        im = (h = (TvsWallmax[i]->ProjectionY("lims")))->GetMaximumBin();
        y  = (int)(h->GetBinCenter(im));
//	printf("max i %d im %d y %d e %f\n",i,im,y,TvsWallmax[i]->GetEntries());
      }
      else
      {
        im = (h = (TvsS[i]->ProjectionY("lims")))->GetMaximumBin();
        y  = (int)(h->GetBinCenter(im));
//	printf("all i %d im %d y %d e %f\n",i,im,y,TvsS[i]->GetEntries());
      }
      ymin = y - 60;
      ymax = y + 60;
//      xmin = (TvsS[i]->GetXaxis())->GetXmin();
//      xmax = (TvsS[i]->GetXaxis())->GetXmax();
      fprintf(fout,"\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", i+1, (int)ymin, (int)ymax);
      if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", i+1, (int)ymin, (int)ymax);
      if (h) delete h;
    }   
  fclose(fout);
  if (LIMITS_ROOT_FILE)
  {
    WritePreparationHists0();
  }
}

void BmnTof2Raw2DigitNew::readSlewingLimits()
{
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  char filn[128], name[128], nname[128];
  int i, im, y;
  float ymin, ymax, xmin, xmax;
  int j, lmi, lma, wc, wm, wt0mi, wt0ma;
  if (LIMITS_ROOT_FILE == 0)
  {
    FILE *finl = 0;
    sprintf(filn, "%s%s_slewing_limits.txt", path.Data(), filname_base);
    finl = fopen(filn,"r");
    if (finl == NULL)
    {
      printf("No slewing limits file %s, use defaults\n", filn);
      return;
    };
//    fscanf(finl,"\t\tTOF2.SetW(%d,%d);\n", &wc, &wm);
//    if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetW(%d,%d);\n", wc, wm);
//    SetW(wc,wm);
//    fscanf(finl,"\t\tTOF2.SetWT0(%d,%d);\n", &wt0mi, &wt0ma);
//    if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetWT0(%d,%d);\n", wt0mi, wt0ma);
//    SetWT0(wt0mi,wt0ma);
    if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetW(%d,%d);\n", Wcut, Wmax);
    if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetWT0(%d,%d);\n", WT0min, WT0max);
    for (i=0; i<MaxPlane; i++)
    {
      fscanf(finl,"\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", &j, &lmi, &lma);
      if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", j, lmi, lma);
      SetLeadMinMax(j,lmi,lma);
    }   
    fclose(finl);
  }
  else
  {
    sprintf(filn, "%s%s", path.Data(), filname_base);
    strcat(filn, "_preparation.root");
    TFile *fPreparation = new TFile(filn,"READ");

    if (fPreparation == 0)
    {
	    printf("Error input file %s for limits!\n",filn);
	    return;
    }
  
//    sprintf(name, "Wlimits");
//    TH1F *hW = (TH1F *)(fPreparation->Get(name));
//    wc = hW->GetBinContent(1);
//    wm = hW->GetBinContent(2);
//    wt0mi = hW->GetBinContent(3);
//    wt0ma = hW->GetBinContent(4);
//    if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetW(%d,%d);\n", wc, wm);
//    SetW(wc,wm);
//    if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetWT0(%d,%d);\n", wt0mi, wt0ma);
//    SetWT0(wt0mi,wt0ma);

    if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetW(%d,%d);\n", Wcut, Wmax);
    if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetWT0(%d,%d);\n", WT0min, WT0max);

    TH2F *hs = 0;
    int bymin  = 0;
    int bymax  = 0;
    int nb = 0;
    double meant = 0, sigmat = 0.;
    TH1D *tim = 0;
    for (int plane = 0; plane < MaxPlane; plane++)
    {
	if (MAX_STRIP)
	    sprintf(name, "Time_vs_Width_Chamber_%.1f_max",idchambers[plane]);
	else
	    sprintf(name, "Time_vs_Width_Chamber_%.1f_all",idchambers[plane]);
	hs = (TH2F *)fPreparation->Get(name);
	if (!hs)
	{
	    printf("Error input Time vs Strip hist for chamber %.1f!\n", idchambers[plane]);
	    continue;
	}
	tim = hs->ProjectionY();
	im = tim->GetMaximumBin();
	y  = (int)(tim->GetBinCenter(im));
	ymin = y - 60;
	ymax = y + 60;
        bymin  = (int)(tim->FindBin(ymin));
        bymax  = (int)(tim->FindBin(ymax));
	nb = hs->GetNbinsX();
	sprintf(nname, "Fit_Time_vs_Width_Chamber%.1fall",idchambers[plane]);
	TH2F *htvsw = new TH2F(nname,nname,nb,0,nb*50, 60, ymin, ymax);
	for (int ibx = 1; ibx <= nb; ibx++)
	    for (int iby = bymin; iby <= bymax; iby++)
		htvsw->SetBinContent(ibx, iby-bymin+1, hs->GetBinContent(ibx,iby));
	htvsw->FitSlicesY(0,0,-1,10,"QNR");
	sprintf(nname, "Fit_Time_vs_Width_Chamber%.1fall_0",idchambers[plane]);
	TH1D *htvsw_0 = (TH1D *)(gDirectory->Get(nname));
	sprintf(nname, "Fit_Time_vs_Width_Chamber%.1fall_1",idchambers[plane]);
	TH1D *htvsw_1 = (TH1D *)(gDirectory->Get(nname));
	sprintf(nname, "Fit_Time_vs_Width_Chamber%.1fall_2",idchambers[plane]);
	TH1D *htvsw_2 = (TH1D *)(gDirectory->Get(nname));
	sprintf(nname, "Fit_Time_vs_Width_Chamber%.1fall_chi2",idchambers[plane]);
	TH1D *htvsw_chi2 = (TH1D *)(gDirectory->Get(nname));
//	printf("***************************************\n");
	if (idchambers[plane] == 18.2f && 0)
	{
	    TCanvas *c = new TCanvas("c","c",800,900);
	    c->Divide(1,3);
	    c->cd(1);
	    hs->Draw();
	    c->cd(2);
	    htvsw->Draw();
	    c->cd(3);
	    htvsw_1->Draw();
	    gPad->WaitPrimitive();
	}
	for (int ibx = 1; ibx <= nb; ibx++)
	{
	    meant  = htvsw_1->GetBinContent(ibx);
	    sigmat = htvsw_2->GetBinContent(ibx);
	    if (sigmat == 0 || sigmat > 40)
	    {
		LeadMinW[plane][ibx-1] = (int)ymin;
		LeadMaxW[plane][ibx-1] = (int)ymax;
	    }
	    else
	    {
		LeadMinW[plane][ibx-1] = meant - sigmat;
		LeadMaxW[plane][ibx-1] = meant + sigmat;
	    }
//	    printf("pl %d binw %d min %d max %d\n",plane,ibx,LeadMinW[plane][ibx-1],LeadMaxW[plane][ibx-1]);
	}
	if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", plane+1, (int)ymin, (int)ymax);
	SetLeadMinMax(plane+1,(int)ymin,(int)ymax);
	delete htvsw_0;
	delete htvsw_1;
	delete htvsw_2;
	delete htvsw_chi2;
	delete htvsw;
	delete hs;
    }

    TProfile *prof = 0;
    sprintf(name, "Time_offsets0");
    prof = (TProfile *)(fPreparation->Get(name));
    if (!prof)
    {
	    printf("Error input TOffsets0 slewing profile!\n");
    }
    else
    {
	    for (i=1; i<=prof->GetNbinsX(); i++)
	    {
		Toffsets00[i] = prof->GetBinContent(i);
		Toffsets00n[i] = prof->GetBinEntries(i);
		Toffsets00e[i] = prof->GetBinError(i);
	    }
    }
    
    //prof->Draw();
    //gPad->WaitPrimitive();
    fPreparation->Close();
  }

  return;
}

void BmnTof2Raw2DigitNew::readSlewing(Bool_t update)
{
  TProfile *prof1 = 0, *prof2 = 0;
  TProfile2D *prof = 0;
  TH2F *plot = 0;
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  
  char filnr[128], name[128];
  sprintf(filnr, "%s%s", path.Data(), filname_base);
  strcat(filnr, ".slewing.root");
  if (update) fSlewing = new TFile(filnr,"UPDATE");
  else        fSlewing = new TFile(filnr,"READ");

  if (fSlewing->IsZombie())
  {
    fSlewing = 0;
    if (fSlewRun < 0)
    {
	printf("Error open slewing file %s, work without slewing corrections!\n", filnr);
	return;
    }
    else
    {
	LOG(FATAL)<<"Error open slewing file " << filnr << " - exit!" << FairLogger::endl;
	return;
    }
  }

  int planer = 0;
  for (int plane = 0; plane < MaxPlane; plane++)
  {
    for (int region = 0; region < 4; region++)
    {
	planer = plane;
	if (fSlewCham > 0) planer = fSlewCham-1;
	if (chamber_slewing[plane] > 0) planer = chamber_slewing[plane] - 1;
	//printf("\nread for chamber %d region %d\n", plane+1, region+1);
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_%d",idchambers[planer],region+1);
	prof = (TvsWt0[planer][region] = (TProfile2D *)fSlewing->Get(name));
	if (!prof)
	{
	    printf("Error input T0 slewing profile for chamber %.1f region %d!\n", idchambers[planer], region+1);
//	    continue;
	}
//	printf(" C0 %d S %d R %d D %f\n",planer+1, 20, region+1, TvsWt0[planer][region]->GetBinContent(TvsWt0[planer][region]->FindBin(900,20)));
	//prof->Smooth();
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_%d",idchambers[planer],region+1);
	prof = (TvsW[planer][region] = (TProfile2D *)fSlewing->Get(name));
	if (!prof)
	{
	    printf("Error input RPC slewing profile for chamber %.1f region %d!\n", idchambers[planer], region+1);
//	    continue;
	}
//	printf(" C  %d S %d R %d D %f\n",planer+1, 20, region+1, TvsW[planer][region]->GetBinContent(TvsW[planer][region]->FindBin(3000,20)));
	//prof->Smooth();
	sprintf(name, "Time_vs_Strip_Chamber_%.1f_Peak_%d_Maxima",idchambers[planer],region+1);
	plot = (TvsSm[planer][region] = (TH2F *)fSlewing->Get(name));
	if (!plot)
	{
	    printf("Error input RPC time distribution for chamber %.1f region %d!\n", idchambers[planer], region+1);
//	    continue;
	}
    }
  } // loop on chambers

  for (int region = 0; region < 4; region++)
  {
	sprintf(name, "Time_offsets_region_%d", region+1);
	prof1 = (Toffsets[region] = (TProfile *)fSlewing->Get(name));
	if (!prof1)
	{
	    printf("Error input TOffsets slewing profile for region %d!\n", region+1);
//	    continue;
	}
	sprintf(name, "Time_fit_offsets_region_%d", region+1);
	prof1 = (Toffsetsf[region] = (TProfile *)fSlewing->Get(name));
	if (!prof1)
	{
	    printf("Error input TOffsets fit slewing profile for region %d!\n", region+1);
//	    continue;
	}
  }

  sprintf(name, "Time_offsets0");
  prof1 = (Toffsets0 = (TProfile *)(fSlewing->Get(name)));
  if (!prof1)
  {
	    printf("Error input TOffsets0 slewing profile!\n");
  }

  sprintf(name, "Time_Lead_Min");
  prof1 = (pLeadMin = (TProfile *)(fSlewing->Get(name)));
  if (!prof1)
  {
	    printf("Error input Time_Lead_Min hist!\n");
  }

  sprintf(name, "Time_Lead_Max");
  prof2 = (pLeadMax = (TProfile *)(fSlewing->Get(name)));
  if (!prof2)
  {
	    printf("Error input Time_Lead_Max hist!\n");
  }

  for (int plane = 0; plane < MaxPlane && prof1 && prof2; plane++)
  {
    LeadMin[plane] = prof1->GetBinContent(plane+1);
    LeadMax[plane] = prof2->GetBinContent(plane+1);
    if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", plane+1, (int)prof1->GetBinContent(plane+1),(int)prof2->GetBinContent(plane+1));
  }

  sprintf(name, "Wlimits");
  TH1F *hW = (TH1F *)(fSlewing->Get(name));
  if (!hW)
  {
	    printf("Error input Wlimits hist!\n");
	    return;
  }
  int wc = hW->GetBinContent(1);
  int wm = hW->GetBinContent(2);
  int wt0mi = hW->GetBinContent(3);
  int wt0ma = hW->GetBinContent(4);
  if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetW(%d,%d);\n", wc, wm);
  Wcut = wc;
  Wmax = wm;
  if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetWT0(%d,%d);\n", wt0mi, wt0ma);
  WT0min = wt0mi;
  WT0max = wt0ma;
  //fSlewing->Close();

  return;
}

void BmnTof2Raw2DigitNew::getEventInfo(long long *ev,long long *t1,long long *t2){
    *ev=EVENT;
    *t1=TIME_SEC;
    *t2=TIME_NS;
}

void BmnTof2Raw2DigitNew::print(){
     printf("\n===========================================================================\n");
     printf("Number of chambers %d, crates %d, slots %d, TDC channels %d\n\n", MaxPlane, ncrates, nslots, n_rec);
     printf("  #\tcrate\tcrateID\n===========================================================================\n");
     for(int i=0;i<ncrates;i++){
       printf("%3d\t%d\t0x%0x\n",i,numcrates[i],idcrates[numcrates[i]]);
     }   
     printf("===========================================================================\n");
     printf("  #\tchamber\tchamberID\n===========================================================================\n");
     for(int i=0;i<nchambers;i++){
       printf("%3d\t%d\t%.1f\n",i,numcha[i],idchambers[numcha[i]]);
     }   
     printf("===========================================================================\n");
     printf("  #\tslot\tfiletype\tfile\n===========================================================================\n");
     for(int i=0;i<nslots+1;i++){
       printf("%3d\t%d\t%d\t%s\n",i,numslots[i],dnltype[numcr[i]][numslots[i]],dnlname[numcr[i]][numslots[i]]);
     }   
     printf("===========================================================================\n");
     printf("   #\tcrate\tslot\tchannel\tplane\tstrip\tside\n===========================================================================\n");
     for(int i=0;i<n_rec;i++){
       printf("%4d\t%d\t%d\t%d\t%d\t%d\t%d\n",i,mapa[i].crate,mapa[i].slot,mapa[i].chan,mapa[i].plane,mapa[i].strip,mapa[i].side);
     }   
     printf("===========================================================================\n");
}

int BmnTof2Raw2DigitNew::numcrate(int id)
{
    for (int ic=0; ic<ncrates; ic++)
    {
	if (id==idcrates[ic]) return ic;
    }
    return -1;
}

void BmnTof2Raw2DigitNew::fillPreparation(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width) {

    Long64_t ts_diff = 0L;
    Wt0->Fill(t0width*INVHPTIMEBIN);
    float lead[TOF2_MAX_CHANNEL];
    float trail[TOF2_MAX_CHANNEL];
    memset(lead,0,TOF2_MAX_CHANNEL*sizeof(float));
    memset(trail,0,TOF2_MAX_CHANNEL*sizeof(float));
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnTDCDigit *digit = (BmnTDCDigit*) data->At(i);
       int type = digit->GetType();
       int tdcnum = digit->GetHptdcId();
       int chanin = digit->GetChannel();
       int chan = 0;
       if      (type == kTDC32VL)     chan = get_ch_tdc32vl(tdcnum,chanin*4);
       else if (type == kTDC64VHLE)   chan = get_ch_tdc64vhle(tdcnum,chanin*4);
       else continue;
       if (chan < 0) continue;
       map<UInt_t,Long64_t>::iterator it = ts->find(digit->GetSerial());
       if (it == ts->end()) continue;
       ts_diff = it->second;
       int nc = numcrate((digit->GetSerial())&0xFFFFFF);
       if (nc < 0) continue;
       int ind = nrec[nc][digit->GetSlot()][chan]; 
       if(ind==-1) continue; 
       Wts->Fill(ts_diff);
       int crate = mapa[ind].crate;
       int slot = mapa[ind].slot;
       int dnl = digit->GetValue() & 0x3FF;
       float tm =  (digit->GetValue()+DNL_Table[crate][slot][chan][dnl]) - chtima[crate][slot][chan] - (t0 - ts_diff)*INVHPTIMEBIN + T0shift;
       //if(digit->GetLeading()) lead[ind]=tm; else trail[ind]=tm; 
       if(digit->GetLeading()) {if (lead[ind] == 0) lead[ind]=tm;} else {if (trail[ind] == 0) trail[ind]=tm;} 
//       printf("#%d slot %d chan %d td %d inl %f chtima %f t0 %f ts %ld t %f\n", i, mapa[ind].slot, mapa[ind].chan, digit->GetValue(), DNL_Table[crate][slot][chan][dnl], chtima[crate][slot][chan], t0, ts_diff, tm);
    }

    int Wc = Wcut;

    float wmaxs[TOF2_MAX_CHAMBERS] = {0.}, tmaxs[TOF2_MAX_CHAMBERS] = {-1000.};
    int smax[TOF2_MAX_CHAMBERS] = {-1};
    for(int i=0; i<MaxPlane; i++) wmaxs[i] = 0.;
    for(int i=0; i<MaxPlane; i++) tmaxs[i] = -1000.;
    for(int i=0; i<MaxPlane; i++) smax[i] = -1;
    for(int ind=0;ind<n_rec;ind++){
       int ind1 = mapa[ind].pair;
       if (ind1 < 0) continue;
       if(lead[ind1]==0 || trail[ind1]==0) continue;
       if(lead[ind]!=0 && trail[ind]!=0){
	Wc = Wcut;
	if (Wcutc[mapa[ind].plane] >= 0.) Wc = Wcutc[mapa[ind].plane];
	float L = (lead[ind]+lead[ind1])/2.;
	float W1 = trail[ind]-lead[ind];
	float W2 = trail[ind1]-lead[ind1];
	float W = (W1+W2)/2.;
	if (idchambers[mapa[ind].plane] != 19.3f) H1chambrate->Fill(idchambers[mapa[ind].plane]);
	else                                      H1chambrate->Fill(70.f);
	if (idchambers[mapa[ind].plane] < 100.f && idchambers[mapa[ind].plane] != 19.3f)
	{
	    float x,y,z;
	    get_hit_xyz0(mapa[ind].plane, mapa[ind].strip, (lead[ind]-lead[ind1])*HPTIMEBIN, &x, &y, &z);
	    Hchambrate->Fill(x,y);
	    Hstriprate->Fill(x,y);
	}
//	if (((W1 < Wc && W2 < Wc)||(W1 >= Wc && W2 >= Wc)))
//	{
	TvsS[mapa[ind].plane]->Fill(mapa[ind].strip, L);
	WvsS[mapa[ind].plane]->Fill(mapa[ind].strip, W);
	TvsWall[mapa[ind].plane]->Fill(W, L);
//	printf("%d %d %d %f %f %f\n", ind, mapa[ind].slot, mapa[ind].chan, L, W, t0);
//	}
//	if (W > wmaxs[mapa[ind].plane] && ((W1 < Wc && W2 < Wc)||(W1 >= Wc && W2 >= Wc)))
	if (W > wmaxs[mapa[ind].plane])
	{
		wmaxs[mapa[ind].plane] = W;
		tmaxs[mapa[ind].plane] = L;
		smax[mapa[ind].plane] = mapa[ind].strip;
	}
       } // leading and trailing exists
    } // loop on strips
    for (int i=0; i<MaxPlane; i++)
    {
	    if (smax[i] > -1)
	    {
		TvsWallmax[i]->Fill(wmaxs[i], tmaxs[i]);
	    }
    }
}

void BmnTof2Raw2DigitNew::fillSlewingT0(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width) {
    Long64_t ts_diff = 0L;
    float lead[TOF2_MAX_CHANNEL];
    float trail[TOF2_MAX_CHANNEL];
    memset(lead,0,TOF2_MAX_CHANNEL*sizeof(float));
    memset(trail,0,TOF2_MAX_CHANNEL*sizeof(float));
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnTDCDigit *digit = (BmnTDCDigit*) data->At(i);
       int type = digit->GetType();
       int tdcnum = digit->GetHptdcId();
       int chanin = digit->GetChannel();
       int chan = -1;
       if      (type == kTDC32VL)     chan = get_ch_tdc32vl(tdcnum,chanin*4);
       else if (type == kTDC64VHLE)   chan = get_ch_tdc64vhle(tdcnum,chanin*4);
       else continue;
//       printf(" Type OK!\n");
       if (chan < 0) continue;
//       printf(" Channel OK = %d!\n", chan);
       map<UInt_t,Long64_t>::iterator it = ts->find(digit->GetSerial());
       if (it == ts->end()) continue;
//       printf(" Tstamp OK!\n");
       ts_diff = it->second;
       int nc = numcrate((digit->GetSerial())&0xFFFFFF);
       if (nc < 0) continue;
       int ind = nrec[nc][digit->GetSlot()][chan]; 
       if(ind==-1) continue; 
//       printf(" Found!\n");
       int crate = mapa[ind].crate;
       int slot = mapa[ind].slot;
       int dnl = digit->GetValue() & 0x3FF;
       float tm =  (digit->GetValue()+DNL_Table[crate][slot][chan][dnl]) - chtima[crate][slot][chan] - (t0 - ts_diff)*INVHPTIMEBIN + T0shift;
       //if(digit->GetLeading()) lead[ind]=tm; else trail[ind]=tm; 
       if(digit->GetLeading()) {if (lead[ind] == 0) lead[ind]=tm;} else {if (trail[ind] == 0) trail[ind]=tm;} 
//       if(digit->GetLeading()) printf("       t0 %f\n", t0*1024./25.); 
//       if (tm > 100000.) printf("%d %d %d %d %f %d %d %f\n", i, mapa[ind].slot, mapa[ind].chan, digit->GetValue(),DNL_Table[crate][slot][chan][dnl], t0, ts_diff, tm);
//       printf("%d %d %d %d %f %f %lld %f\n", i, mapa[ind].slot, mapa[ind].chan, digit->GetValue(),DNL_Table[crate][slot][chan][dnl], t0, ts_diff, tm);
    }
    int Wc = Wcut;
    int Wm = Wmax;
    float wmaxs[TOF2_MAX_CHAMBERS] = {0.}, tmaxs[TOF2_MAX_CHAMBERS] = {-1000.};
    int smax[TOF2_MAX_CHAMBERS] = {-1};
    int rmax[TOF2_MAX_CHAMBERS] = {-1}, ira = -1;
    for(int i=0; i<MaxPlane; i++) wmaxs[i] = 0.;
    for(int i=0; i<MaxPlane; i++) tmaxs[i] = -1000.;
    for(int i=0; i<MaxPlane; i++) smax[i] = -1;
    for(int i=0; i<MaxPlane; i++) rmax[i] = -1;
    for(int ind=0;ind<n_rec;ind++){
       int ind1 = mapa[ind].pair;
       if (ind1 < 0) continue;
       if (lead[ind1]==0 || trail[ind1]==0) continue;
       if (numstrip[mapa[ind].plane] >=0 && numstrip[mapa[ind].plane] != mapa[ind].strip) continue;
       if(lead[ind]!=0 && trail[ind]!=0){
	Wc = Wcut;
	if (Wcutc[mapa[ind].plane] >= 0.) Wc = Wcutc[mapa[ind].plane];
	Wm = Wmax;
	if (Wmaxc[mapa[ind].plane] >= 0.) Wm = Wmaxc[mapa[ind].plane];
	float L = (lead[ind]+lead[ind1])/2.;
	float W1 = trail[ind]-lead[ind];
	float W2 = trail[ind1]-lead[ind1];
	float W = (W1+W2)/2.;
	if (TIME_LIMITS_VS_WIDTH)
	{
		int binw = (int)(W/50.f);
		if (binw < 0 || binw >= 500) continue;
		if (LeadMinW[mapa[ind].plane][binw] == 0 || LeadMaxW[mapa[ind].plane][binw] == 0) continue;
		if (L < LeadMinW[mapa[ind].plane][binw] || L >= LeadMaxW[mapa[ind].plane][binw]) continue;
	}
	else
		if (L < LeadMin[mapa[ind].plane] || L >= LeadMax[mapa[ind].plane]) continue;
//	if (L >= LeadMin[mapa[ind].plane] && L < LeadMax[mapa[ind].plane])
//	{
	// inside time limits
	ira = -1;
	// inside time limits
	if (USE_PRELIMINARY_OFFSETS) L -= Toffsets0->GetBinContent(Toffsets0->FindBin(ind));
	if ((int)W1 < Wc && (int)W2 < Wc)
	{
	    ira = 0;
	}
	else if ((int)W1 >= Wc && (int)W2 >= Wc && (int)W1 < Wm && (int)W2 < Wm)
	{
	    ira = 1;
        }
	else if ((int)W1 >= Wc && (int)W2 < Wc && (int)W1 < Wm)
	{
	    ira = 2;
	}
	else if ((int)W2 >= Wc && (int)W1 < Wc && (int)W2 < Wm)
	{
	    ira = 3;
	}
	if (ira >= 0) // inside width regions
	{
	    if (MAX_STRIP)
	    {
		if (W > wmaxs[mapa[ind].plane])
		{
		wmaxs[mapa[ind].plane] = W;
		tmaxs[mapa[ind].plane] = L;
		smax[mapa[ind].plane] = mapa[ind].strip;
		rmax[mapa[ind].plane] = ira;
		}
	    }
	    else
	    {
		TvsWt0[mapa[ind].plane][ira]->Fill(t0width*INVHPTIMEBIN, mapa[ind].strip, L);
//		TvsWt0p[mapa[ind].plane][ira]->Fill(t0width*INVHPTIMEBIN, L);
	    } // MAX_STRIP
	} // inside width regions ira
//	} // inside time limits
       } // lead and trail exists
    } // hits
    for (int i=0; i<MaxPlane && MAX_STRIP; i++)
    {
	    if (smax[i] > -1)
	    {
		TvsWt0[i][rmax[i]]->Fill(t0width*INVHPTIMEBIN, smax[i], tmaxs[i]);
//		TvsWt0p[i][rmax[i]]->Fill(t0width*INVHPTIMEBIN, tmaxs[i]);
	    }
    }

}

void BmnTof2Raw2DigitNew::fillSlewing(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width) {
    Long64_t ts_diff = 0L;
    Wt0->Fill(t0width*INVHPTIMEBIN);
    float lead[TOF2_MAX_CHANNEL];
    float trail[TOF2_MAX_CHANNEL];
    memset(lead,0,TOF2_MAX_CHANNEL*sizeof(float));
    memset(trail,0,TOF2_MAX_CHANNEL*sizeof(int));
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnTDCDigit *digit = (BmnTDCDigit*) data->At(i);
       int type = digit->GetType();
       int tdcnum = digit->GetHptdcId();
       int chanin = digit->GetChannel();
       int chan = 0;
       if      (type == kTDC32VL)     chan = get_ch_tdc32vl(tdcnum,chanin*4);
       else if (type == kTDC64VHLE)   chan = get_ch_tdc64vhle(tdcnum,chanin*4);
       else continue;
       if (chan < 0) continue;
       map<UInt_t,Long64_t>::iterator it = ts->find(digit->GetSerial());
       if (it == ts->end()) continue;
       ts_diff = it->second;
       int nc = numcrate((digit->GetSerial())&0xFFFFFF);
       if (nc < 0) continue;
       int ind = nrec[nc][digit->GetSlot()][chan]; 
       if(ind==-1) continue; 
       Wts->Fill(ts_diff);
       int crate = mapa[ind].crate;
       int slot = mapa[ind].slot;
       int dnl = digit->GetValue() & 0x3FF;
       float tm =  (digit->GetValue()+DNL_Table[crate][slot][chan][dnl]) - chtima[crate][slot][chan] - (t0 - ts_diff)*INVHPTIMEBIN + T0shift;
       //if(digit->GetLeading()) lead[ind]=tm; else trail[ind]=tm; 
       if(digit->GetLeading()) {if (lead[ind] == 0) lead[ind]=tm;} else {if (trail[ind] == 0) trail[ind]=tm;} 
    }
    int Wc = Wcut;
    int Wm = Wmax;
    float wmaxs[TOF2_MAX_CHAMBERS] = {0.}, tmaxs[TOF2_MAX_CHAMBERS] = {-1000.};
    int smax[TOF2_MAX_CHAMBERS] = {-1};
    int rmax[TOF2_MAX_CHAMBERS] = {-1};
    for(int i=0; i<MaxPlane; i++) wmaxs[i] = 0.;
    for(int i=0; i<MaxPlane; i++) tmaxs[i] = -1000.;
    for(int i=0; i<MaxPlane; i++) smax[i] = -1;
    for(int i=0; i<MaxPlane; i++) rmax[i] = -1;
    char nfun0[12] = {""};
    int ifit0 = 0;
    TF1 *fun0 = 0;
    for(int ind=0;ind<n_rec;ind++){
       int ind1 = mapa[ind].pair;
       if (ind1 < 0) continue;
       if(lead[ind1]==0 || trail[ind1]==0) continue;
       if (numstrip[mapa[ind].plane] >=0 && numstrip[mapa[ind].plane] != mapa[ind].strip) continue;
       if(lead[ind]!=0 && trail[ind]!=0){
//        if (mapa[ind].plane == 0) printf("%f %f %f %f\n",lead[ind1],lead[ind],trail[ind1],trail[ind]);
	Wc = Wcut;
	if (Wcutc[mapa[ind].plane] >= 0.) Wc = Wcutc[mapa[ind].plane];
	Wm = Wmax;
	if (Wmaxc[mapa[ind].plane] >= 0.) Wm = Wmaxc[mapa[ind].plane];
	float L = (lead[ind1]+lead[ind])/2.;
	float W1 = trail[ind]-lead[ind];
	float W2 = trail[ind1]-lead[ind1];
	float W = (W1+W2)/2.;

	int ira = -1;

	if ((int)W1 < Wc && (int)W2 < Wc)
	{
	    ira = 0;
	}
	else if (W1 >= Wc && W2 >= Wc)
	{
	    ira = 1;
        }
	else if (W1 >= Wc && W2 < Wc)
	{
	    ira = 2;
	}
	else if (W1 < Wc && W2 >= Wc)
	{
	    ira = 3;
	}
//	printf("Plane %d Strip %d Lead %f Width %f LeadMin %d LeadMax %d\n", mapa[ind].plane, mapa[ind].strip, L, W, LeadMin[mapa[ind].plane], LeadMax[mapa[ind].plane]);
	if (TIME_LIMITS_VS_WIDTH)
	{
		int binw = (int)(W/50.f);
		if (binw < 0 || binw >= 500) continue;
		if (LeadMinW[mapa[ind].plane][binw] == 0 || LeadMaxW[mapa[ind].plane][binw] == 0) continue;
		if (L < LeadMinW[mapa[ind].plane][binw] || L >= LeadMaxW[mapa[ind].plane][binw]) continue;
	}
	else
		if (L < LeadMin[mapa[ind].plane] || L >= LeadMax[mapa[ind].plane]) continue;
//	if (L >= LeadMin[mapa[ind].plane] && L < LeadMax[mapa[ind].plane])
//	{
	  if (idchambers[mapa[ind].plane] != 19.3f) H1chambrate->Fill(idchambers[mapa[ind].plane]);
	  else                                      H1chambrate->Fill(70.f);
	  if (idchambers[mapa[ind].plane] < 100.f && idchambers[mapa[ind].plane] != 19.3f)
	  {
	    float x,y,z;
	    get_hit_xyz0(mapa[ind].plane, mapa[ind].strip, (lead[ind]-lead[ind1])*HPTIMEBIN, &x, &y, &z);
	    Hchambrate->Fill(x,y);
	    Hstriprate->Fill(x,y);
	  }
	  if (ira >= 0)
	  {
	    if (USE_FIT_SLEWING)
	    {
		ifit0 = 0;
		if((fun0 = TvsWt0[mapa[ind].plane][ira]->GetFunction(NSLFIT0))!=NULL) { strcpy(nfun0,NSLFIT0); ifit0 = 1; }
		if((fun0 = TvsWt0[mapa[ind].plane][ira]->GetFunction("slew0"))!=NULL) { strcpy(nfun0,"slew0"); ifit0 = 1; }
	    }
	    if (USE_PRELIMINARY_OFFSETS) L -= Toffsets0->GetBinContent(Toffsets0->FindBin(ind));
//            if (mapa[ind].plane == 18) printf(" peak 1 l1 %f W %f\n",L,W);
	    if (USE_FIT_SLEWING && ifit0)
	    {
    		L -= fun0->Eval(t0width*INVHPTIMEBIN);
	    }
	    else
	    {
    		L -= TvsWt0[mapa[ind].plane][ira]->GetBinContent(TvsWt0[mapa[ind].plane][ira]->FindBin(t0width*INVHPTIMEBIN,mapa[ind].strip));
	    }
//            if (mapa[ind].plane == 18) printf(" peak 1 l2 %f\n",L);
	    if (MAX_STRIP)
	    {
		if (W > wmaxs[mapa[ind].plane])
		{
		    wmaxs[mapa[ind].plane] = W;
		    tmaxs[mapa[ind].plane] = L;
		    smax[mapa[ind].plane] = mapa[ind].strip;
		    rmax[mapa[ind].plane] = ira;
		}
	    }
	    else
	    {
		TvsW[mapa[ind].plane][ira]->Fill(W, mapa[ind].strip, L);
//		TvsWp[mapa[ind].plane][ira]->Fill(W, L);
	    } // MAX_STRIP
	  } // in width limits
//	} // in time limits
       } // lead and trail exists
    } // hits
    for (int i=0; i<MaxPlane && MAX_STRIP; i++)
    {
	    if (smax[i] > -1)
	    {
		TvsW[i][rmax[i]]->Fill(wmaxs[i], smax[i], tmaxs[i]);
//		TvsWp[i][rmax[i]]->Fill(wmaxs[i], tmaxs[i]);
	    }
    }

}

void BmnTof2Raw2DigitNew::fillEvent(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width, TClonesArray *tof2digit) {
    Long64_t ts_diff = 0L;
    if (CHECK_SLEWING) Wt0->Fill(t0width*INVHPTIMEBIN);
    float lead[TOF2_MAX_CHANNEL];
    float trail[TOF2_MAX_CHANNEL];
    memset(lead,0,TOF2_MAX_CHANNEL*sizeof(float));
    memset(trail,0,TOF2_MAX_CHANNEL*sizeof(int));
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnTDCDigit *digit = (BmnTDCDigit*) data->At(i);
       int type = digit->GetType();
       int tdcnum = digit->GetHptdcId();
       int chanin = digit->GetChannel();
       int chan = -1;
       if      (type == kTDC32VL)     chan = get_ch_tdc32vl(tdcnum,chanin*4);
       else if (type == kTDC64VHLE)   chan = get_ch_tdc64vhle(tdcnum,chanin*4);
       else continue;
       if (chan < 0) continue;
       map<UInt_t,Long64_t>::iterator it = ts->find(digit->GetSerial());
       if (it == ts->end()) continue;
       ts_diff = it->second;
       int nc = numcrate((digit->GetSerial())&0xFFFFFF);
       if (nc < 0) continue;
       int ind = nrec[nc][digit->GetSlot()][chan]; 
       if(ind==-1) continue; 
       if (CHECK_SLEWING) Wts->Fill(ts_diff);
       int crate = mapa[ind].crate;
       int slot = mapa[ind].slot;
       int dnl = digit->GetValue() & 0x3FF;
       float tm =  (digit->GetValue()+DNL_Table[crate][slot][chan][dnl]) - chtima[crate][slot][chan] - (t0 - ts_diff)*INVHPTIMEBIN + T0shift;
//       printf("tm %f c %d s %d cor %f ts_diff %lld\n",tm, crate, slot, DNL_Table[crate][slot][chan][dnl], ts_diff);
       //if(digit->GetLeading()) lead[ind]=tm; else trail[ind]=tm; 
       if(digit->GetLeading()) {if (lead[ind] == 0) lead[ind]=tm;} else {if (trail[ind] == 0) trail[ind]=tm;} 
    }
//    printf("n_rec = %d\n", n_rec);
    int Wc = Wcut;
    int Wm = Wmax;
    char nfun0[12] = {""};
    int ifit0 = 0;
    TF1 *fun0 = 0;
    char nfun[12] = {""};
    int ifit = 0;
    TF1 *fun = 0;
//    printf("*****************************\n");
    for(int ind=0;ind<n_rec;ind++){ 
       int ind1 = mapa[ind].pair;
       if (ind1 < 0) continue;
       if(lead[ind1]==0 || trail[ind1]==0) continue;
//       if (tmeane[ind] == 0.) continue;
       if(lead[ind]!=0 && trail[ind]!=0){
//    printf("Ok!\n");
	Wc = Wcut;
	if (Wcutc[mapa[ind].plane] >= 0.) Wc = Wcutc[mapa[ind].plane];
	Wm = Wmax;
	if (Wmaxc[mapa[ind].plane] >= 0.) Wm = Wmaxc[mapa[ind].plane];
	float L = (lead[ind1]+lead[ind])/2.;
	float D = (lead[ind1]-lead[ind]);
	float W1 = trail[ind]-lead[ind];
	float W2 = trail[ind1]-lead[ind1];
	float W = (W1+W2)/2.;

	int ira = -1;

	Int_t bin = 1;

	if (fSlewing == 0) goto createdigit;

	if ((int)W1 < Wc && (int)W2 < Wc)
	{
	    ira = 0;
	}
	else if (W1 >= Wc && W2 >= Wc && W1 < Wm && W2 < Wm)
	{
	    ira = 1;
        }
	else if (W1 >= Wc && W2 < Wc && W1 < Wm)
	{
	    ira = 2;
	}
	else if (W1 < Wc && W2 >= Wc && W2 < Wm)
	{
	    ira = 3;
	}

	if (CHECK_SLEWING)
	{
	    if (idchambers[mapa[ind].plane] != 19.3f) H1chambrate->Fill(idchambers[mapa[ind].plane]);
	    else                                      H1chambrate->Fill(70.f);

	    if (idchambers[mapa[ind].plane] < 100.f && idchambers[mapa[ind].plane] != 19.3f)
	    {
		float x,y,z;
		get_hit_xyz0(mapa[ind].plane, mapa[ind].strip, D*HPTIMEBIN, &x, &y, &z);
		Hchambrate->Fill(x,y);
		Hstriprate->Fill(x,y);
	    }
	}
//test!!!!
//	printf("Chamber %d Time %f Width %f Tmin %d Tmax %d Wcut %d Wmax %d T0shft %f\n",mapa[ind].plane,L,W,LeadMin[mapa[ind].plane],LeadMax[mapa[ind].plane],Wc,Wm,T0shift);
	if (CHECK_SLEWING)
	{
	    if (TIME_LIMITS_VS_WIDTH)
	    {
		int binw = (int)(W/50.f);
		if (binw < 0 || binw >= 500) continue;
		if (LeadMinW[mapa[ind].plane][binw] == 0 || LeadMaxW[mapa[ind].plane][binw] == 0) continue;
		if (L < LeadMinW[mapa[ind].plane][binw] || L >= LeadMaxW[mapa[ind].plane][binw]) continue;
	    }
	    else
		if (L < LeadMin[mapa[ind].plane] || L >= LeadMax[mapa[ind].plane]) continue;
	}
//
//	if (idchambers[mapa[ind].plane] == 19.2f) printf("L1 = %f\n",L);;

	if (USE_PRELIMINARY_OFFSETS) L -= Toffsets0->GetBinContent(Toffsets0->FindBin(ind));
//	if (idchambers[mapa[ind].plane] == 19.2f) printf("L2 = %f\n",L);;
	if (ira >= 0)
	{
	    if (USE_FIT_SLEWING)
	    {
		ifit0 = 0;
		if((fun0 = TvsWt0[mapa[ind].plane][ira]->GetFunction(NSLFIT0))!=NULL) { strcpy(nfun0,NSLFIT0); ifit0 = 1; }
		if((fun0 = TvsWt0[mapa[ind].plane][ira]->GetFunction("slew0"))!=NULL) { strcpy(nfun0,"slew0"); ifit0 = 1; }

		ifit = 0;
		if((fun = TvsW[mapa[ind].plane][ira]->GetFunction(NSLFIT))!=NULL) { strcpy(nfun,NSLFIT); ifit = 1; }
		if((fun = TvsW[mapa[ind].plane][ira]->GetFunction("slew"))!=NULL) { strcpy(nfun,"slew"); ifit = 1; }
	    }
	    if (USE_FIT_SLEWING && ifit0)
	    {
    		L -= fun0->Eval(t0width*INVHPTIMEBIN);
	    }
	    else
	    {
		bin = TvsWt0[mapa[ind].plane][ira]->FindBin(t0width*INVHPTIMEBIN,mapa[ind].strip);
		if (TvsWt0[mapa[ind].plane][ira]->GetBinError(bin) <= 0.f) continue;
    		L -= TvsWt0[mapa[ind].plane][ira]->GetBinContent(bin);
//		if (idchambers[mapa[ind].plane] == 19.2f) printf("L3 = %f\n",L);;
	    }
	    if (CHECK_SLEWING) TvsWt0r[mapa[ind].plane][ira]->Fill(t0width*INVHPTIMEBIN, L);
	    if (USE_FIT_SLEWING && ifit)
	    {
    		L -= fun->Eval(W);
	    }
	    else
	    {
		bin = TvsW[mapa[ind].plane][ira]->FindBin(W,mapa[ind].strip);
		if (TvsW[mapa[ind].plane][ira]->GetBinError(bin) <= 0.f) continue;
    		L -= TvsW[mapa[ind].plane][ira]->GetBinContent(bin);
//		if (idchambers[mapa[ind].plane] == 19.2f) printf("L4 = %f\n",L);;
	    }
	    if (CHECK_SLEWING) TvsWr[mapa[ind].plane][ira]->Fill(W, L);
    	    if      (USE_FINAL_OFFSETS == 1) L -= Toffsets[ira]->GetBinContent(Toffsets[ira]->FindBin(ind));
    	    else if (USE_FINAL_OFFSETS == 2) L -= Toffsetsf[ira]->GetBinContent(Toffsetsf[ira]->FindBin(ind));
//	    if (idchambers[mapa[ind].plane] == 19.2f) printf("L5 = %f\n",L);;
createdigit:
    	    Float_t D_corrected = get_hit_diff0(mapa[ind].plane,mapa[ind].strip,D*HPTIMEBIN);
    	    new((*tof2digit)[tof2digit->GetEntriesFast()]) BmnTof2Digit(mapa[ind].plane,mapa[ind].strip,L*HPTIMEBIN,W,D_corrected);  
    	    //printf("%d %d %f %f %f t0 %f t0width %f\n", mapa[ind].plane,mapa[ind].strip,L*HPTIMEBIN,W,D*HPTIMEBIN, t0, t0width);  
	}  // in width regions limits
       } // leading and trailing exists
    } // loop on strips
}
void BmnTof2Raw2DigitNew::fillEqualization(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width) {
    Long64_t ts_diff = 0L;
    float lead[TOF2_MAX_CHANNEL];
    float trail[TOF2_MAX_CHANNEL];
    memset(lead,0,TOF2_MAX_CHANNEL*sizeof(float));
    memset(trail,0,TOF2_MAX_CHANNEL*sizeof(int));
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnTDCDigit *digit = (BmnTDCDigit*) data->At(i);
       int type = digit->GetType();
       int tdcnum = digit->GetHptdcId();
       int chanin = digit->GetChannel();
       int chan = -1;
       if      (type == kTDC32VL)     chan = get_ch_tdc32vl(tdcnum,chanin*4);
       else if (type == kTDC64VHLE)   chan = get_ch_tdc64vhle(tdcnum,chanin*4);
       else continue;
       if (chan < 0) continue;
       map<UInt_t,Long64_t>::iterator it = ts->find(digit->GetSerial());
       if (it == ts->end()) continue;
       ts_diff = it->second;
       int nc = numcrate((digit->GetSerial())&0xFFFFFF);
       if (nc < 0) continue;
       int ind = nrec[nc][digit->GetSlot()][chan]; 
       if(ind==-1) continue; 
       int crate = mapa[ind].crate;
       int slot = mapa[ind].slot;
       int dnl = digit->GetValue() & 0x3FF;
       float tm =  (digit->GetValue()+DNL_Table[crate][slot][chan][dnl]) - chtima[crate][slot][chan] - (t0 - ts_diff)*INVHPTIMEBIN + T0shift;
//       printf("tm %f c %d s %d cor %f ts_diff %lld\n",tm, crate, slot, DNL_Table[crate][slot][chan][dnl], ts_diff);
       //if(digit->GetLeading()) lead[ind]=tm; else trail[ind]=tm; 
       if(digit->GetLeading()) {if (lead[ind] == 0) lead[ind]=tm;} else {if (trail[ind] == 0) trail[ind]=tm;} 
    }
//    printf("n_rec = %d\n", n_rec);
    int Wc = Wcut;
    int Wm = Wmax;
    char nfun0[12] = {""};
    int ifit0 = 0;
    TF1 *fun0 = 0;
    char nfun[12] = {""};
    int ifit = 0;
    TF1 *fun = 0;
    for(int ind=0;ind<n_rec;ind++){ 
       int ind1 = mapa[ind].pair;
       if (ind1 < 0) continue;
       if(lead[ind1]==0 || trail[ind1]==0) continue;
       if(lead[ind]!=0 && trail[ind]!=0){
//    printf("Ok!\n");
	Wc = Wcut;
	if (Wcutc[mapa[ind].plane] >= 0.) Wc = Wcutc[mapa[ind].plane];
	Wm = Wmax;
	if (Wmaxc[mapa[ind].plane] >= 0.) Wm = Wmaxc[mapa[ind].plane];
	float L = (lead[ind1]+lead[ind])/2.;
	float D = (lead[ind1]-lead[ind])/2.;
	float W1 = trail[ind]-lead[ind];
	float W2 = trail[ind1]-lead[ind1];
	float W = (W1+W2)/2.;

	int ira = -1;

	if ((int)W1 < Wc && (int)W2 < Wc)
	{
	    ira = 0;
	}
	else if (W1 >= Wc && W2 >= Wc)
	{
	    ira = 1;
        }
	else if (W1 >= Wc && W2 < Wc)
	{
	    ira = 2;
	}
	else if (W1 < Wc && W2 >= Wc)
	{
	    ira = 3;
	}
	if (TIME_LIMITS_VS_WIDTH)
	{
		int binw = (int)(W/50.f);
		if (binw < 0 || binw >= 500) continue;
		if (LeadMinW[mapa[ind].plane][binw] == 0 || LeadMaxW[mapa[ind].plane][binw] == 0) continue;
		if (L < LeadMinW[mapa[ind].plane][binw] || L >= LeadMaxW[mapa[ind].plane][binw]) continue;
	}
	else
		if (L < LeadMin[mapa[ind].plane] || L >= LeadMax[mapa[ind].plane]) continue;
//	if (L >= LeadMin[mapa[ind].plane] && L < LeadMax[mapa[ind].plane])
//	{
	  if (ira >= 0)
	  {
	    if (USE_PRELIMINARY_OFFSETS) L -= Toffsets0->GetBinContent(Toffsets0->FindBin(ind));
	    if (USE_FIT_SLEWING)
	    {
		ifit0 = 0;
		if((fun0 = TvsWt0[mapa[ind].plane][ira]->GetFunction(NSLFIT0))!=NULL) { strcpy(nfun0,NSLFIT0); ifit0 = 1; }
		if((fun0 = TvsWt0[mapa[ind].plane][ira]->GetFunction("slew0"))!=NULL) { strcpy(nfun0,"slew0"); ifit0 = 1; }

		ifit = 0;
		if((fun = TvsW[mapa[ind].plane][ira]->GetFunction(NSLFIT))!=NULL) { strcpy(nfun,NSLFIT); ifit = 1; }
		if((fun = TvsW[mapa[ind].plane][ira]->GetFunction("slew"))!=NULL) { strcpy(nfun,"slew"); ifit = 1; }
	    }
	    if (USE_FIT_SLEWING && ifit0)
	    {
    		L -= fun0->Eval(t0width*INVHPTIMEBIN);
	    }
	    else
	    {
    		L -= TvsWt0[mapa[ind].plane][ira]->GetBinContent(TvsWt0[mapa[ind].plane][ira]->FindBin(t0width*INVHPTIMEBIN, mapa[ind].strip));
	    }
	    if (USE_FIT_SLEWING && ifit)
	    {
    		L -= fun->Eval(W);
	    }
	    else
	    {
    		L -= TvsW[mapa[ind].plane][ira]->GetBinContent(TvsW[mapa[ind].plane][ira]->FindBin(W, mapa[ind].strip));
	    }
	    Toffsets[ira]->Fill(ind,L);
	    TvsSm[mapa[ind].plane][ira]->Fill(mapa[ind].strip, L);
	  }
	  else
	  {
	    continue;
	  }
	} // in width regions limits
//       } // in time limits
    } // loop on strips
}

void BmnTof2Raw2DigitNew::FitSlewing()
{
  if (!FIT_SLEWING) return;
  TProfile2D *prof2d = 0;
  TProfile *prof = 0;
  TFitResultPtr result = 0, resultold = 0;
  TF1 *ptotal, *p[20];
  int col[20] = {kGreen,kBlue,kMagenta,kYellow,28,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
  double par[44] = {0.f};
  double pmin, pmax;
  char name[128];
  FILE *fout = 0;
  char filn[128];
  int subdeg = SUBDEG;

  gROOT->ProcessLine( "gErrorIgnoreLevel = 3001;");

  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  sprintf(filn, "%s%s", path.Data(), filname_base);
  strcat(filn, ".fit.log");
  fout = fopen(filn,"w");
  TCanvas *c = 0;
  if (DRAW_GOOD || DRAW_BAD) c = new TCanvas("c","Slewing Fit",800,900);
  for (int plane = 0; plane < MaxPlane; plane++)
  {
  for (int pk = 0; pk < 4; pk++)
  {
  for (int s = 1; s <= 32; s++)
  {
	prof2d = TvsWt0[plane][pk];
	prof2d->SetStats(kFALSE);
	sprintf(name,"%s_strip%d",prof2d->GetTitle(),s);
	prof = prof2d->ProfileX(name, s, s);
	int nonzero = 0, bfirst = 0, blast = 0, nonzeromax = 0, bfirstmax = 0;
	int minpoints = 0;
	if (SMOOTH)
	{
	    prof->Smooth();
	    if (DRAW_GOOD)
	    {
		prof->Draw();
		gPad->WaitPrimitive();
	    }
	    goto Rpc;
	}
	pmin = 1000000.;
	pmax = 0.;
	for (int i=1; i<=prof->GetNbinsX(); i++)
	{
	    if (prof->GetBinContent(i)!=0)
	    {
		if (prof->GetBinEntries(i)<MIN_FIT_ENTRIES)
		{
		    prof->SetBinContent(i,0);
		    prof->SetBinEntries(i,0);
		    prof->SetBinError(i,0);
		}
		else
		{
		    if (bfirst == 0) bfirst = i;
		    blast = i;
		    nonzero++;
		    if (prof->GetBinContent(i)>pmax) pmax = prof->GetBinContent(i);
		    if (prof->GetBinContent(i)<pmin) pmin = prof->GetBinContent(i);
		}
	    }
	    else
	    {
		prof->SetBinContent(i,0);
		prof->SetBinEntries(i,0);
		prof->SetBinError(i,0);
	    }
	}
	if (bfirst > 1) bfirst--;
	if (blast < prof->GetNbinsX()) blast++;
	if (SKIP_SIDES)
	{
		nonzero = 0; bfirst = 0; blast = 0; nonzeromax = 0;
		for (int i=1; i<=prof->GetNbinsX(); i++)
		{
		    if (prof->GetBinContent(i)!=0)
		    {
			if (nonzero == 0) bfirst = i;
			nonzero++;
		    }
		    else
		    {
			if (nonzero > nonzeromax)
			{
			    nonzeromax = nonzero;
			    blast = i-1;
			    bfirstmax = bfirst;
			}
			nonzero = 0;
		    }
		}
		if (nonzero > nonzeromax)
		{
		    nonzeromax = nonzero;
		    blast = prof->GetNbinsX();
		    bfirstmax = bfirst;
		}
	}
	else
	{
	    bfirstmax = bfirst;
	    nonzeromax = nonzero;
	}

	sscanf(NSLFIT0,"pol%d",&minpoints);
	if (nonzeromax > (minpoints+1))
	{
	    char fun[12],nfun[12], nfunold[12];
	    for (int ifi = 0; ifi < MAX_FIT_ATTEMPTS; ifi++)
	    {

		if (!ITERATIONS0)
		{
			sprintf(fun,"%s",NSLFIT0);
			sprintf(nfun,"pfi");
			TF1 *pfi = new TF1(nfun,fun,TvsWt0[plane][pk]->GetBinCenter(bfirstmax),TvsWt0[plane][pk]->GetBinCenter(blast));
			pfi->SetLineColor(kBlack);
			result = prof->Fit(pfi,FIT,"",TvsWt0[plane][pk]->GetBinCenter(bfirstmax),TvsWt0[plane][pk]->GetBinCenter(blast));
			if (prof->GetFunction(nfun)) if (result->IsValid() && result->Chi2()>0.) break;
		}
		else
		{
			for (int it = 0; it<=minpoints; it++)
			{
			    sprintf(fun,"pol%d",it);
			    sprintf(nfun,"pfi%d",it);
			    TF1 *pfi = new TF1(nfun,fun,TvsWt0[plane][pk]->GetBinCenter(bfirstmax),TvsWt0[plane][pk]->GetBinCenter(blast));
			    pfi->SetLineColor(kBlack);
			    result = prof->Fit(pfi,FITI0,"",TvsWt0[plane][pk]->GetBinCenter(bfirstmax),TvsWt0[plane][pk]->GetBinCenter(blast));
			    if (prof->GetFunction(nfun))
			    {
			      if (!(result->IsValid() && result->Chi2()>0.))
			      {
				result = resultold;
				strcpy(nfun, nfunold);
				break;
			      }
			    }
			    else
			    {
				result = resultold;
				strcpy(nfun, nfunold);
				break;
			    }
			    resultold = result;
			    strcpy(nfunold, nfun);
			}
			if (prof->GetFunction(nfun)) if (result->IsValid() && result->Chi2()>0.) break;
		}
	    }
    	    if(prof->GetFunction(nfun)) (prof->GetFunction(nfun))->ResetBit(TF1::kNotDraw);
    	    if(prof->GetFunction(nfun)) (prof->GetFunction(nfun))->SetName("slew0");
	    bool badresult = true;
	    if (prof->GetFunction(nfun)) if (result->IsValid() && result->Chi2()>0.) badresult = false;

	    if (badresult)
		{
		    if (PRINT_FIT_RESULTS)
		    {
			printf(" Chamber %d peak %d T0 slewing - fit unsuccessful, chi2 = %f/%d\n", plane+1, pk+1, result->Chi2(), result->Ndf());
			fprintf(fout, " Chamber %d peak %d T0 slewing - fit unsuccessful, chi2 = %f/%d\n", plane+1, pk+1, result->Chi2(), result->Ndf());
		    }
		    if (DRAW_BAD)
		    {
			prof->Draw();
			prof->GetXaxis()->SetRange(bfirstmax, blast);
			prof->SetMaximum(pmax+(pmax-pmin)*0.10);
			prof->SetMinimum(pmin-(pmax-pmin)*0.10);
			//printf("min %f max %f\n",pmin,pmax);
			c->Update();
			c->WaitPrimitive();
		    }
		}
	    else
		{
		    if (PRINT_FIT_RESULTS)
		    {
			printf(" Chamber %d (%.1f) peak %d T0 slewing - fit OK, chi2 = %f/%d\n", plane+1, idchambers[plane], pk+1, result->Chi2(), result->Ndf());
			fprintf(fout, " Chamber %d (%.1f) peak %d T0 slewing - fit OK, chi2 = %f/%d\n", plane+1, idchambers[plane], pk+1, result->Chi2(), result->Ndf());
		    }
		    if (DRAW_GOOD)
		    {
			prof->Draw();
			prof->GetXaxis()->SetRange(bfirstmax, blast);
			prof->SetMaximum(pmax+(pmax-pmin)*0.10);
			prof->SetMinimum(pmin-(pmax-pmin)*0.10);
			//printf("min %f max %f\n",pmin,pmax);
			c->Update();
			c->WaitPrimitive();
		    }
	    }


	    TList *l = prof->GetListOfFunctions();
	    int nf = l->GetEntries();
	    for (int il=0; il<nf; il++)
	    {
//		    printf("        %s\n", (l->At(il))->GetName());
	    }
	    TF1 *fu = 0, *lfun[20] = {NULL};
	    int nrej = 0;
	    for (int il=0; il<nf; il++)
	    {
		    fu = (TF1 *)(l->At(il));
		    if (!strcmp(fu->GetName(),"slew0")) continue;
		    lfun[nrej++] = fu;
	    }
	    for (int il=0; il<nrej; il++)
	    {
		    if (lfun[il]) delete lfun[il];
	    }
	    l = prof->GetListOfFunctions();
	    for (int il=0; il<l->GetEntries(); il++)
	    {
//		    printf("     ** %s\n", (l->At(il))->GetName());
	    }
	}
	else {printf(" Chamber %d (%.1f) peak %d T0 slewing - too few nonzero bins = %d\n", plane+1, idchambers[plane], pk+1, nonzero);fprintf(fout, " Chamber %d (%.1f) peak %d T0 slewing - too few nonzero bins = %d\n", plane+1, idchambers[plane], pk+1, nonzero); }
Rpc:
	prof2d = TvsW[plane][pk];
	prof2d->SetStats(kFALSE);
	sprintf(name,"%s_strip%d",prof2d->GetTitle(),s);
	prof = prof2d->ProfileX(name, s, s);
	if (SMOOTH)
	{
	    prof->Smooth();
	    if (DRAW_GOOD)
	    {
		prof->Draw();
		gPad->WaitPrimitive();
	    }
	    continue;
	}
	pmin = 1000000.;
	pmax = 0.;
	nonzeromax = 0;
	bfirst = 0;
	blast = 0;
	for (int i=1; i<=prof->GetNbinsX(); i++)
	{
	    if (prof->GetBinContent(i)!=0)
	    {
		if (prof->GetBinEntries(i)<MIN_FIT_ENTRIES)
		{
		    prof->SetBinContent(i,0);
		    prof->SetBinEntries(i,0);
		    prof->SetBinError(i,0);
		}
		else
		{
		    if (bfirst == 0) bfirst = i;
		    blast = i;
		    nonzero++;
		    if (prof->GetBinContent(i)>pmax) pmax = prof->GetBinContent(i);
		    if (prof->GetBinContent(i)<pmin) pmin = prof->GetBinContent(i);
		}
	    }
	    else
	    {
		prof->SetBinContent(i,0);
		prof->SetBinEntries(i,0);
		prof->SetBinError(i,0);
	    }
	}
	if (bfirst > 1) bfirst--;
	if (blast < prof->GetNbinsX()) blast++;
	if (SKIP_SIDES)
	{
		nonzero = 0; bfirst = 0; blast = 0; nonzeromax = 0; bfirstmax = 0;
		for (int i=1; i<=prof->GetNbinsX(); i++)
		{
		    if (prof->GetBinContent(i)!=0)
		    {
			if (nonzero == 0) bfirst = i;
			nonzero++;
		    }
		    else
		    {
			if (nonzero > nonzeromax)
			{
			    nonzeromax = nonzero;
			    blast = i-1;
			    bfirstmax = bfirst;
			}
			nonzero = 0;
		    }
		}
		if (nonzero > nonzeromax)
		{
		    nonzeromax = nonzero;
		    blast = prof->GetNbinsX();
		    bfirstmax = bfirst;
		}
	}
	else
	{
	    bfirstmax = bfirst;
	    nonzeromax = nonzero;
	}
	sscanf(NSLFIT,"pol%d",&minpoints);
	int minp = minpoints;
	if (SUBRANGES) minp = (SUBRANGES+1)*subdeg;
	char ntotal[128];
	if (nonzeromax > (minpoints+1))
	{
	    char fun[12],nfun[12], nfunold[12];
	    for (int ifi = 0; ifi < MAX_FIT_ATTEMPTS; ifi++)
	    {
		if (SUBRANGES)
		{
		    strcpy(nfun,"ptotal");
		    float x0 = TvsW[plane][pk]->GetBinCenter(bfirstmax);
		    float dx = (TvsW[plane][pk]->GetBinCenter(blast)-TvsW[plane][pk]->GetBinCenter(bfirstmax))/3.f;
		    float dx1 = dx/2.f, xc = x0, dxc = dx1;
		    char spol[12], snam[10][12];
		    sprintf(spol,"pol%d",subdeg);
		    for (int is = 0; is < (SUBRANGES+1); is++)
		    {
			sprintf(snam[is],"p%d",is+1);
			p[is] = new TF1(snam[is],spol,xc, xc+dxc);
	    		p[is]->SetLineColor(col[is]);
	    		// Fit each function and add it to the list of functions
	    		if (is == 0) prof->Fit(p[is],"Q0R+");
	    		else         prof->Fit(p[is],"Q0R+");
    			if(prof->GetFunction(snam[is])) (prof->GetFunction(snam[is]))->ResetBit(TF1::kNotDraw);
	    		// Get the parameters from the fit
	    		p[is]->GetParameters(&par[is*(subdeg+1)]);
			xc += dxc;
			if (is == 0) dxc = dx1;
			else         dxc = dx;
			if (is == 0)
			    sprintf(ntotal,"%s(%d)",spol,is*(subdeg+1));
			else
			    sprintf(ntotal,"%s+%s(%d)",ntotal,spol,is*(subdeg+1));
//			printf("%s %s %f %f\n",snam[is],spol,xc,xc+dxc);
		    }
	    	    // The total is the sum of the four, each has 7 parameters
	    	    ptotal = new TF1("ptotal",ntotal,x0,xc+dxc);
	    	    ptotal->SetLineColor(kBlack);
//		    printf("ptotal %s %f %f\n",ntotal,x0,xc+dxc);
	    	    // Use the parameters on the sum
	    	    ptotal->SetParameters(par);
    		    result = prof->Fit(ptotal,"Q0RS+","",x0,xc);
		    if (result->IsValid() && result->Chi2()>0. && prof->GetFunction("ptotal")) break;
		}
		else
		{
		    if (!ITERATIONS)
		    {

			sprintf(fun,"%s",NSLFIT);
			sprintf(nfun,"pfi");
			TF1 *pfi = new TF1(nfun,fun,TvsW[plane][pk]->GetBinCenter(bfirstmax),TvsW[plane][pk]->GetBinCenter(blast));
			pfi->SetLineColor(kBlack);
			result = prof->Fit(pfi,FIT,"",TvsW[plane][pk]->GetBinCenter(bfirstmax),TvsW[plane][pk]->GetBinCenter(blast));
			if (result->IsValid() && result->Chi2()>0. && prof->GetFunction(nfun)) break;
		    }
		    else
		    {
			for (int it = 0; it<=minpoints; it++)
			{
			    sprintf(fun,"pol%d",it);
			    sprintf(nfun,"pfi%d",it);
			    TF1 *pfi = new TF1(nfun,fun,TvsW[plane][pk]->GetBinCenter(bfirstmax),TvsW[plane][pk]->GetBinCenter(blast));
			    pfi->SetLineColor(kBlack);
			    result = prof->Fit(pfi,FITI,"",TvsW[plane][pk]->GetBinCenter(bfirstmax),TvsW[plane][pk]->GetBinCenter(blast));
			    if (prof->GetFunction(nfun))
			    {
			      if (!(result->IsValid() && result->Chi2()>0.))
			      {
				result = resultold;
				strcpy(nfun, nfunold);
				break;
			      }
			    }
			    else
			    {
				result = resultold;
				strcpy(nfun, nfunold);
				break;
			    }
			    resultold = result;
			    strcpy(nfunold, nfun);
			}
			if (prof->GetFunction(nfun)) if (result->IsValid() && result->Chi2()>0.) break;
		    }
		}
	    }
    	    if(prof->GetFunction(nfun)) (prof->GetFunction(nfun))->ResetBit(TF1::kNotDraw);
    	    if(prof->GetFunction(nfun)) (prof->GetFunction(nfun))->SetName("slew");
	    bool badresult = true;
	    if (prof->GetFunction(nfun)) if (result->IsValid() && result->Chi2()>0.) badresult = false;

	    if (badresult)
		{
		    if (PRINT_FIT_RESULTS)
		    {
			printf(" Chamber %d (%.1f) peak %d RPC slewing - fit unsuccessful, chi2 = %f/%d\n", plane+1, idchambers[plane], pk+1, result->Chi2(), result->Ndf());
			fprintf(fout, " Chamber %d (%.1f) peak %d RPC slewing - fit unsuccessful, chi2 = %f/%d\n", plane+1, idchambers[plane], pk+1, result->Chi2(), result->Ndf());
		    }
		    if (DRAW_BAD)
		    {
			prof->Draw();
			prof->GetXaxis()->SetRange(bfirstmax, blast);
			prof->SetMaximum(pmax+(pmax-pmin)*0.10);
			prof->SetMinimum(pmin-(pmax-pmin)*0.10);
			c->Update();
			c->WaitPrimitive();
		    }
		}
	    else
		{
		    if (PRINT_FIT_RESULTS)
		    {
			printf(" Chamber %d (%.1f) peak %d RPC slewing - fit OK, chi2 = %f/%d\n", plane+1, idchambers[plane], pk+1, result->Chi2(), result->Ndf());
			fprintf(fout, " Chamber %d (%.1f) peak %d RPC slewing - fit OK, chi2 = %f/%d\n", plane+1, idchambers[plane], pk+1, result->Chi2(), result->Ndf());
		    }
		    if (DRAW_GOOD)
		    {
			prof->Draw();
			prof->GetXaxis()->SetRange(bfirstmax, blast);
			prof->SetMaximum(pmax+(pmax-pmin)*0.10);
			prof->SetMinimum(pmin-(pmax-pmin)*0.10);
			c->Update();
			c->WaitPrimitive();
		    }
		}
		TList *l = prof->GetListOfFunctions();
		int nf = l->GetEntries();
		for (int il=0; il<nf; il++)
		{
//		    printf("        %s\n", (l->At(il))->GetName());
		}
		TF1 *fu = 0, *lfun[20] = {NULL};
		int nrej = 0;
		for (int il=0; il<nf; il++)
		{
		    fu = (TF1 *)(l->At(il));
		    if (!strcmp(fu->GetName(),"slew")) continue;
		    lfun[nrej++] = fu;
		}
		for (int il=0; il<nrej; il++)
		{
		    if (lfun[il]) delete lfun[il];
		}
		l = prof->GetListOfFunctions();
		for (int il=0; il<l->GetEntries(); il++)
		{
//		    printf("     ** %s\n", (l->At(il))->GetName());
		}
	}
	else {printf(" Chamber %d (%.1f) peak %d RPC slewing - too few nonzero bins = %d\n", plane+1, idchambers[plane], pk+1, nonzero);fprintf(fout, " Chamber %d (%.1f) peak %d RPC slewing - too few nonzero bins = %d\n", plane+1, idchambers[plane], pk+1, nonzero); }

  } // loop on strips

  } // loop on peaks

  } // loop on chambers

  fclose(fout);

  gROOT->ProcessLine( "gErrorIgnoreLevel = 1001;");

  return;
}

void BmnTof2Raw2DigitNew::Equalization()
{
  if (!FINAL_OFFSETS)
  {
    if (fSlewing)
    {
	fSlewing->Write(0,TObject::kOverwrite);
	fSlewing->Close();
	fSlewing = 0;
    }
    return;
  }
  float tmeane_average[TOF2_MAX_CHAMBERS][4];
  float tmeane[TOF2_MAX_CHANNEL][4];
  TProfile *prof = 0;
  TH1D *proj = 0;
  TF1 *gr = 0;
  char namp[64], namf[64];
  int ncon = 0;
  double mpos = 0.;
  FILE *fout = 0;
  FILE *fdeb = 0;
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  char filn[128];
  sprintf(filn, "%s%s", path.Data(), filname_base);
  strcat(filn, ".equalization.log");
  fout = fopen(filn,"w");
  if (DEBUG_EQUALIZATION)
  {
    sprintf(filn, "%s%s", path.Data(), filname_base);
    strcat(filn, ".equalization.debug");
    fdeb = fopen(filn,"w");
  }
  for (int plane = 0; plane < MaxPlane; plane++)
  {
  for (int pk = 0; pk < 4; pk++)
  {
  if (PRINT_EQUALIZATION_RESULTS) printf("**************** %s Chamber %d Peak %d Time-Width area RPC slewing (write) ******************************\n\n", filname_base, plane+1, pk+1);
  fprintf(fout, "**************** %s Chamber %d Peak %d Time-Width area slewing *******************************\n\n", filname_base, plane+1, pk+1);

  int ip, is;
  int na = 0;
  tmeane_average[plane][pk] = 0.;
  for (int ind=0; ind<n_rec; ind++)
  {
	if (mapa[ind].pair < 0) continue;
	ip = mapa[ind].plane;
	if (ip != plane) continue;
	is = mapa[ind].strip;
	//printf("** ip %d is %d nt %d t %f\n",ip,is,ntmeane[ind][pk],tmeane[ind][pk]);
	sprintf(namp,"Maxima_chamber_%d_strip_%d_equalization",ip+1,is+1);
	proj = TvsSm[ip][pk]->ProjectionY(namp,is+1,is+1);
	if (EQUAL_MAXIMA)
	{
	    int mbin = proj->GetMaximumBin();
	    int nbin = proj->GetNbinsX();
	    mpos = proj->GetBinCenter(mbin);
	    ncon = proj->GetBinContent(mbin);
	    //printf("Chamber %d strip %d max %d at %f\n", ip,is,ncon,mpos);
	    if (ncon > 50)
	    {
		int nonzero = 0;
		int zflag   = 0;
		double sum = 0., sumx = 0.;
		for (int ib=((mbin-6)>0? (mbin-6):1);ib<((mbin+6)<nbin? (mbin+6):nbin);ib++)
		{
		    if (proj->GetBinContent(ib)>0.) {nonzero++; sum += proj->GetBinContent(ib); sumx += proj->GetBinContent(ib)*proj->GetBinCenter(ib);}
		    else { zflag = 1;}
		}
		if (FIT_MAXIMA && zflag == 0 && nonzero >= 5)
		{
		    sprintf(namf,"gaus%dstrip%d",ip+1,is+1);
		    //if (gr) delete gr;
		    gr = new TF1(namf,"gaus",mpos-6.,mpos+6.);
		    proj->Fit(namf,"QR0");
		    if (!(proj->GetFunction(namf)))
		    {
//			tmeane[ind][pk] = mpos;
			tmeane[ind][pk] = sumx/sum;
			if (ip == 18 && fdeb)
			{
			    fprintf(fdeb,"Max: Chamber %d strip %d zflag %d nonzero %d mpos %f tmeane %f\n",
			    ip,is,zflag,nonzero,mpos,tmeane[ind][pk]);
			}
		    }
		    else
		    {
			tmeane[ind][pk] = proj->GetFunction(namf)->GetParameter(1);
			if (ip == 18 && fdeb)
			{
			    fprintf(fdeb,"Fit: Chamber %d strip %d zflag %d nonzero %d mpos %f tmeane %f\n",
			    ip,is,zflag,nonzero,mpos,tmeane[ind][pk]);
			}
		    }
		}
		else
		{
//		    tmeane[ind][pk] = mpos;
		    tmeane[ind][pk] = sumx/sum;
		    if (ip == 18 && fdeb)
		    {
			fprintf(fdeb,"Max: Chamber %d strip %d zflag %d nonzero %d mpos %f tmeane %f\n",
			ip,is,zflag,nonzero,mpos,tmeane[ind][pk]);
		    }
		}
		tmeane_average[plane][pk] += tmeane[ind][pk];
		na++;
	    }
	    else if (ncon > 0)
	    {
		double sum = 0., sumx = 0.;
		for (int ib=((mbin-6)>0? (mbin-6):1);ib<((mbin+6)<nbin? (mbin+6):nbin);ib++)
		{
		    if (proj->GetBinContent(ib)>0.) {sum += proj->GetBinContent(ib); sumx += proj->GetBinContent(ib)*proj->GetBinCenter(ib);}
		}
//		tmeane[ind][pk] = proj->GetMean();
		tmeane[ind][pk] = sumx/sum;
		tmeane_average[plane][pk] += tmeane[ind][pk];
		na++;
		if (ip == 18 && fdeb)
		{
		    fprintf(fdeb,"Mean: Chamber %d strip %d mpos %f ntmeane %d tmeane %f\n",
		    ip,is,mpos,ncon,tmeane[ind][pk]);
		}
	    }
	    else
	    {
		tmeane[ind][pk] = 0.;
		if (ip == 18 && fdeb)
		{
		    fprintf(fdeb,"Zero: Chamber %d strip %d tmeane %f\n",
		    ip,is,tmeane[ind][pk]);
		}
	    }
	}
	else
	{
	    if (proj->GetEntries())
	    {
//		tmeane[ind][pk] = proj->GetMean();
		tmeane[ind][pk] = 0.;
//		tmeane_average[plane][pk] += tmeane[ind][pk];
//		na++;
	    }
	    else
	    {
		tmeane[ind][pk] = 0.;
	    }
	    if (ip == 18 && fdeb)
	    {
		fprintf(fdeb,"Mean: Chamber %d strip %d ntmean %d tmeane %f\n",
		ip,is,ncon,tmeane[ind][pk]);
	    }
	}

	//proj->Draw();
	//gPad->WaitPrimitive();
	//getchar();
	//printf("!! ip %d is %d nt %d t %f na %d\n",ip,is,ntmeane[ind],tmeane[ind],na);
	delete proj;
  } // loop on strips
  if (na) tmeane_average[plane][pk] /= (float)na;
  fprintf(fout,"Chamber #%d region %d channel offsets (average is %f)\n", plane+1, pk+1, tmeane_average[plane][pk]);
  if (PRINT_EQUALIZATION_RESULTS) printf("Chamber #%d region %d channel offsets (average is %f)\n", plane+1, pk+1, tmeane_average[plane][pk]);
  for (int ind=0; ind<n_rec; ind++)
  {
    if (mapa[ind].pair < 0) continue;
    ip = mapa[ind].plane;
    if (ip != plane) continue;
    is = mapa[ind].strip;
    if (TOZERO)
    {
	fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmeane[ind][pk]);
	if (PRINT_EQUALIZATION_RESULTS) printf("   strip %d time shift (left+right)/2 = %f\n", is, tmeane[ind][pk]);
//	Toffsetsf[pk]->SetBinContent(Toffsetsf[pk]->FindBin(ind), tmeane[ind][pk]);
	Toffsetsf[pk]->Fill(ind, tmeane[ind][pk]);
    }
    else
    {
	if (tmeane[ind][pk] == 0.) tmeane[ind][pk] = tmeane_average[plane][pk];
	fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmeane[ind][pk]-tmeane_average[plane][pk]);
	if (PRINT_EQUALIZATION_RESULTS) printf("   strip %d time shift (left+right)/2 = %f\n", is, tmeane[ind][pk]-tmeane_average[plane][pk]);
//	Toffsetsf[pk]->SetBinContent(Toffsetsf[pk]->FindBin(ind), tmeane[ind][pk]-tmeane_average[plane][pk]);
	Toffsetsf[pk]->Fill(ind, tmeane[ind][pk]-tmeane_average[plane][pk]);
    }
  } // loop on strips

  } // loop on peaks

  } // loop on chambers

  fclose(fout);
  if (fdeb) fclose(fdeb);

  if (fSlewing)
  {
    fSlewing->Write(0,TObject::kOverwrite);
    fSlewing->Close();
    fSlewing = 0;
  }

  return;
}

void BmnTof2Raw2DigitNew::Equalization0()
{
  if (!PRELIMINARY_OFFSETS) return;
  float tmeane_average[TOF2_MAX_CHAMBERS] = {0.};
  float tmeane[TOF2_MAX_CHANNEL] = {0.};
  TH1D *proj = 0;
  TF1 *gr = 0;
  char namp[64], namf[64];
  int ncon = 0;
  double mpos = 0.;
  FILE *fout = 0;
  FILE *fdeb = 0;
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  char filn[128];
  sprintf(filn, "%s%s", path.Data(), filname_base);
  strcat(filn, ".equalization0.log");
  fout = fopen(filn,"w");
  if (DEBUG_EQUALIZATION0)
  {
    sprintf(filn, "%s%s", path.Data(), filname_base);
    strcat(filn, ".equalization0.debug");
    fdeb = fopen(filn,"w");
  }
  for (int plane = 0; plane < MaxPlane; plane++)
  {
  if (PRINT_EQUALIZATION0_RESULTS) printf("**************** %s Chamber %d primary equalization ******************************\n\n", filname_base, plane+1);
  fprintf(fout, "**************** %s Chamber %d primary equalization *******************************\n\n", filname_base, plane+1);

  int ip, is;
  int na = 0;
  tmeane_average[plane] = 0.;
  for (int ind=0; ind<n_rec; ind++)
  {
	if (mapa[ind].pair < 0) continue;
	ip = mapa[ind].plane;
	if (ip != plane) continue;
	is = mapa[ind].strip;
	//printf("** ip %d is %d nt %d t %f\n",ip,is,ntmeane[ind][pk],tmeane[ind][pk]);
	sprintf(namp,"Maxima_chamber_%d_strip_%d_equalization0",ip+1,is+1);
	proj = TvsS[ip]->ProjectionY(namp,is+1,is+1);
	if (EQUAL_MAXIMA0)
	{
	    int mbin = proj->GetMaximumBin();
	    int nbin = proj->GetNbinsX();
	    mpos = proj->GetBinCenter(mbin);
	    ncon = proj->GetBinContent(mbin);
	    //printf("Chamber %d strip %d max %d at %f\n", ip,is,ncon,mpos);
	    if (ncon > 50)
	    {
		int nonzero = 0;
		int zflag   = 0;
		double sum = 0., sumx = 0.;
		for (int ib=((mbin-6)>0? (mbin-6):1);ib<((mbin+6)<nbin? (mbin+6):nbin);ib++)
		{
		    if (proj->GetBinContent(ib)>0.) {nonzero++; sum += proj->GetBinContent(ib); sumx += proj->GetBinContent(ib)*proj->GetBinCenter(ib);}
		    else { zflag = 1;}
		}
		if (FIT_MAXIMA0 && zflag == 0 && nonzero >= 5)
		{
		    sprintf(namf,"gaus%dstrip%d",ip+1,is+1);
		    //if (gr) delete gr;
		    gr = new TF1(namf,"gaus",mpos-6.,mpos+6.);
		    proj->Fit(namf,"QR0");
		    if (!(proj->GetFunction(namf)))
		    {
//			tmeane[ind] = mpos;
			tmeane[ind] = sumx/sum;
			if (ip == 18 && fdeb)
			{
			    fprintf(fdeb,"Max: Chamber %d strip %d zflag %d nonzero %d mpos %f tmeane %f\n",
			    ip,is,zflag,nonzero,mpos,tmeane[ind]);
			}
		    }
		    else
		    {
			tmeane[ind] = proj->GetFunction(namf)->GetParameter(1);
			if (ip == 18 && fdeb)
			{
			    fprintf(fdeb,"Fit: Chamber %d strip %d zflag %d nonzero %d mpos %f tmeane %f\n",
			    ip,is,zflag,nonzero,mpos,tmeane[ind]);
			}
		    }
		}
		else
		{
//		    tmeane[ind] = mpos;
		    tmeane[ind] = sumx/sum;
		    if (ip == 18 && fdeb)
		    {
			fprintf(fdeb,"Max: Chamber %d strip %d zflag %d nonzero %d mpos %f tmeane %f\n",
			ip,is,zflag,nonzero,mpos,tmeane[ind]);
		    }
		}
		tmeane_average[plane] += tmeane[ind];
		na++;
	    }
	    else if (ncon > 0)
	    {
		double sum = 0., sumx = 0.;
		for (int ib=((mbin-6)>0? (mbin-6):1);ib<((mbin+6)<nbin? (mbin+6):nbin);ib++)
		{
		    if (proj->GetBinContent(ib)>0.) {sum += proj->GetBinContent(ib); sumx += proj->GetBinContent(ib)*proj->GetBinCenter(ib);}
		}
//		tmeane[ind] = proj->GetMean();
		tmeane[ind] = sumx/sum;
		tmeane_average[plane] += tmeane[ind];
		na++;
		if (ip == 18 && fdeb)
		{
		    fprintf(fdeb,"Mean: Chamber %d strip %d mpos %f ncon %d tmeane %f\n",
		    ip,is,mpos,ncon,tmeane[ind]);
		}
	    }
	    else
	    {
		tmeane[ind] = 0.;
		//na++;
		if (ip == 18 && fdeb)
		{
		    fprintf(fdeb,"Zero: Chamber %d strip %d tmeane %f\n",
		    ip,is,tmeane[ind]);
		}
	    }
	}
	else
	{
	    if (proj->GetEntries())
	    {
//		tmeane[ind] = proj->GetMean();
		tmeane[ind] = 0.;
//		tmeane_average[plane] += tmeane[ind];
//		na++;
	    }
	    else
	    {
		tmeane[ind] = 0.;
	    }
	    if (ip == 18 && fdeb)
	    {
		fprintf(fdeb,"Mean: Chamber %d strip %d ncon %d tmeane %f\n",
		ip,is,ncon,tmeane[ind]);
	    }
	}
	//proj->Draw();
	//gPad->WaitPrimitive();
	//getchar();
	//printf("!! ip %d is %d t %f na %d\n",ip,is,tmeane[ind],na);
  } // loop on strips
  if (na) tmeane_average[plane] /= (float)na;
  fprintf(fout,"Chamber #%d preliminary channel offsets (average is %f)\n", plane+1, tmeane_average[plane]);
  if (PRINT_EQUALIZATION0_RESULTS) printf("Chamber #%d preliminary channel offsets (average is %f)\n", plane+1, tmeane_average[plane]);
  for (int ind=0; ind<n_rec; ind++)
  {
    if (mapa[ind].pair < 0) continue;
    ip = mapa[ind].plane;
    if (ip != plane) continue;
    is = mapa[ind].strip;
    if (tmeane[ind] == 0.) tmeane[ind] = tmeane_average[plane];
    fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmeane[ind]-tmeane_average[plane]);
    if (PRINT_EQUALIZATION0_RESULTS) printf("   strip %d time shift (left+right)/2 = %f\n", is, tmeane[ind]-tmeane_average[plane]);
    Toffsets0->Fill(ind,tmeane[ind]-tmeane_average[plane]);
  }

  } // loop on chambers

  fclose(fout);
  if (fdeb) fclose(fdeb);


  return;
}

void BmnTof2Raw2DigitNew::DNL_read()
{
 for (int c=0; c<TOF2_MAX_CRATES; c++)
 {
 for (int s=0; s<TOF2_MAX_SLOTS_IN_CRATE; s++)
 {
 if (dnltype[c][s] == 1)
 {
  float tcor, ecor;
  int n, pos;
  char atext[50];
  ifstream fi(dnlname[c][s]);
  if(fi.is_open())
  {
   for (n=0; n<TOF2_MAX_CHANNELS_IN_SLOT; n++)
      {
      for (int i=0; i<1024; i++)
         {
         fi.getline(atext,50);
         sscanf(atext,"%d %f %f\n", &pos,&tcor,&ecor);
         DNL_Table[c][s][n][i] = tcor;
         }
      }
   fi.close();
  }
 }
 else if(dnltype[c][s]%10 == 0)
 {
   float tcor;
   int n, pos, post;
   char atext[25600];
   ifstream fi(dnlname[c][s]);
   if(fi.is_open())
   {
    n = 0;
    while (!fi.eof())
    {
     fi.getline(atext,25600);
     post = 0;
     pos = 0;
     for (int i=0; i<1024; i++)
     {
        sscanf(&atext[post],"%f %n", &tcor, &pos);
	post += pos;
        DNL_Table[c][s][n][i] = tcor;
     }
//     fi.getline(atext,25600);
     n++;
     if (n==72) break;
    }
   fi.close();
   }
 }
 else if(dnltype[c][s] == 2)
 {
   float tcor;
   int n, pos, post;
   char atext[25600];
   ifstream fi(dnlname[c][s]);
   if(fi.is_open())
   {
    n = 0;
    while (!fi.eof())
    {
     fi.getline(atext,25600);
     post = 0;
     pos = 0;
     for (int i=0; i<1024; i++)
     {
        sscanf(&atext[post],"%f %n", &tcor, &pos);
	post += pos;
        DNL_Table[c][s][n][i] = tcor;
     }
     fi.getline(atext,25600);
     n++;
     if (n==TOF2_MAX_CHANNELS_IN_SLOT) break;
    }
   fi.close();
   }
 }
 else if(dnltype[c][s] == 3)
 {
   float tcor;
   int n, pos, post, ch, nerr = 0;
   char atext[25600];
   ifstream fi(dnlname[c][s]);
   if(fi.is_open())
   {
    n = 0;
    fi.getline(atext,25600);
    fi.getline(atext,25600);
    int getfirst = 1;
    if (strstr(atext,"temp") == NULL) getfirst = 0;
    while (!fi.eof())
    {
     if (getfirst) fi.getline(atext,25600);
     getfirst = 1;
     post = 0;
     pos = 0;
     sscanf(&atext[post],"%d= %n", &ch, &pos);
     post += pos;
     if (ch != n && 0)
     {
        if (nerr < 2)
        {
    	    printf("Crate %d Slot %d Wrong line in %s file, channel %d != %d !\n", c, s, dnlname[c][s], n, ch);
	}
	nerr++;
     }
     for (int i=0; i<1024; i++)
     {
        sscanf(&atext[post],"%f, %n", &tcor, &pos);
	post += pos;
        DNL_Table[c][s][n][i] = tcor;
     }
//     fi.getline(atext,25600);
     n++;
     if (n==TOF2_MAX_CHANNELS_IN_SLOT) break;
    }
   fi.close();
   }
 }
 else if (dnltype[c][s] == 4) // fake
 {
  float tcor, ecor;
  int n, pos;
  char atext[50];
  ifstream fi(dnlname[c][s]);
  if(fi.is_open())
  {
   for (n=0; n<TOF2_MAX_CHANNELS_IN_SLOT; n++)
      {
      for (int i=0; i<1024; i++)
         {
         fi.getline(atext,50);
         sscanf(atext,"%f\n", &tcor);
         DNL_Table[c][s][n][i] = tcor;
         }
      }
   fi.close();
  }
 }
 else if(dnltype[c][s] == 5)
 {
   float tcor;
   int n, pos, post, ch, nerr = 0;
   char atext[25600];
   ifstream fi(dnlname[c][s]);
   if(fi.is_open())
   {
    n = 0;
    fi.getline(atext,25600);
    fi.getline(atext,25600);
    int getfirst = 1;
    if (strstr(atext,"temp") == NULL) getfirst = 0;
    while (!fi.eof())
    {
     if (getfirst) fi.getline(atext,25600);
     getfirst = 1;
     post = 0;
     pos = 0;
     sscanf(&atext[post],"%d= %n", &ch, &pos);
     post += pos;
     if (ch != n)
     {
        if (nerr < 2)
        {
    	    printf("Crate %d Slot %d Wrong line in %s file, channel %d != %d !\n", c, s, dnlname[c][s], n, ch);
	}
	nerr++;
     }
     for (int i=0; i<1024; i++)
     {
        sscanf(&atext[post],"%f, %n", &tcor, &pos);
	post += pos;
        DNL_Table[c][s][n][i] = tcor;
     }
//     fi.getline(atext,25600);
     n++;
     if (n==72) break;
    }
   fi.close();
   }
 }
 }
 }
}

#if TOF2_MAX_CHAMBERS == 15
int champosn[TOF2_MAX_CHAMBERS] = {5,10,1,6,11,2,7,12,3,8,13,4,9,14,0};
#else
#if TOF2_MAX_CHAMBERS == 24
int champosn[TOF2_MAX_CHAMBERS] = {17,18, 3, 1,19, 4,23,20, 5,15,21, 6, 2,22, 9,10,11,12,13,14, 7, 8, 0,16};
#else
int champosn[TOF2_MAX_CHAMBERS] = {0};
#endif
#endif

void BmnTof2Raw2DigitNew::SlewingResults()
{
  TProfile *prof = 0;
  for (int it = 0; it < 2; it++)
  {
  for (int plane = 0; plane < MaxPlane; plane++)
  {
  for (int ir = 0; ir < 2; ir++)
  {
  prof = TvsWr[plane][ir];
  if (it == 0) prof = TvsWt0r[plane][ir];
  if (!prof) continue;

  int nonzero = 0;
  for (int i=1; i<=prof->GetNbinsX(); i++)
  {
    if (prof->GetBinContent(i)!=0)
    {
	nonzero++;
	if (prof->GetBinEntries(i)<=4)
	    prof->SetBinEntries(i,0);
    }
  }
  if (nonzero >= 2)
    {
      prof->Fit("pol0","Q0");
      if(prof->GetFunction("pol0")) (prof->GetFunction("pol0"))->ResetBit(TF1::kNotDraw);
    }
  else {printf(" Chamber %d slewing results (%d) - too few nonzero bins = %d\n", plane+1, it, nonzero); }

  } // loop on regions
  } // loop on chambers
  } // loop on T0 and RPC cases

  return;
}

void BmnTof2Raw2DigitNew::drawprep()
{
  int NDX;
  int NDY;
  if (MaxPlane == 15)
  {
    int champos[] = {5,10,1,6,11,2,7,12,3,8,13,4,9,14,0};
    NDX = 5;
    NDY = 3;
    memcpy(champosn, champos, sizeof champosn);
  }
  else if (MaxPlane == 24)
  {
    int champos[] = {17,18, 3, 1,19, 4,23,20, 5,15,21, 6, 2,22, 9,10,11,12,13,14, 7, 8, 0,16};
    NDX = 8;
    NDY = 3;
    memcpy(champosn, champos, sizeof champosn);
  }
  else
  {
    int champos[TOF2_MAX_CHAMBERS] = {0};
    NDX = 6;
    NDY = 10;
    for (int ip=0; ip<TOF2_MAX_CHAMBERS; ip++) champos[ip] = ip;
    memcpy(champosn, champos, sizeof champosn);
  }

  TLine *l = 0, *l1 = 0;
  FILE *fout = 0;
  int i, im, y;
  float ymin, ymax, xmin, xmax;
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  char filn[128];
  sprintf(filn, "%s%s_slewing_limits.txt", path.Data(), filname_base);
//  fout = fopen(filn,"w");

  TCanvas *cp = new TCanvas("cp", "Leadings vs strip", 900,700);
  cp->cd();
  cp->Divide(NDX,NDY);
  for (i=0; i<MaxPlane; i++)
    {
      cp->cd(champosn[i]+1);
      if (TvsS[i]->GetEntries() < 500) continue;
      TvsS[i]->Draw();
      gPad->AddExec("exselt","select_hist()");
      im = (TvsS[i]->ProjectionY())->GetMaximumBin();
      y  = (int)((TvsS[i]->ProjectionY())->GetBinCenter(im));
      ymin = y - 60;
      ymax = y + 60;
      xmin = (TvsS[i]->GetXaxis())->GetXmin();
      xmax = (TvsS[i]->GetXaxis())->GetXmax();
      l = new TLine(xmin,ymin,xmax,ymin);
      l->Draw();
      l->SetLineColor(kRed);
      l = new TLine(xmin,ymax,xmax,ymax);
      l->Draw();
      l->SetLineColor(kRed);
//      fprintf(fout,"\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", i+1, (int)ymin, (int)ymax);
//      if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", i+1, (int)ymin, (int)ymax);
    }   
//  fclose(fout);

  TCanvas *cpp = new TCanvas("cpp", "Leadings", 900,700);
  cpp->cd();
  cpp->Divide(NDX,NDY);
  for (i=0; i<MaxPlane; i++)
    {
      cpp->cd(champosn[i]+1);
      if (TvsS[i]->GetEntries() < 500) continue;
      im = (TvsS[i]->ProjectionY())->GetMaximumBin();
      y  = (int)((TvsS[i]->ProjectionY())->GetBinCenter(im));
      ymin = y - 60;
      ymax = y + 60;
      xmin = 0;
      xmax = (TvsS[i]->ProjectionY())->GetMaximum();
      TvsS[i]->ProjectionY(strcat(strcpy(filn,TvsS[i]->GetName()),"_y"), 1, 32, "D");
      gPad->AddExec("exselt","select_hist()");
      l = new TLine(ymin,xmin,ymin,xmax);
      l->Draw();
      l->SetLineColor(kRed);
      l = new TLine(ymax,xmin,ymax,xmax);
      l->Draw();
      l->SetLineColor(kRed);
    }   

  TCanvas *cpw = new TCanvas("cpw", "Widths vs strip", 900,700);
  cpw->cd();
  cpw->Divide(NDX,NDY);
  for (i=0; i<MaxPlane; i++)
    {
      cpw->cd(champosn[i]+1);
      if (WvsS[i]->GetEntries() < 500) continue;
      WvsS[i]->Draw();
      gPad->AddExec("exselt","select_hist()");
      ymin = Wcut;
      ymax = Wmax;
      xmin = (WvsS[i]->GetXaxis())->GetXmin();
      xmax = (WvsS[i]->GetXaxis())->GetXmax();
      l = new TLine(xmin,ymin,xmax,ymin);
      l->Draw();
      l->SetLineColor(kRed);
      l = new TLine(xmin,ymax,xmax,ymax);
      l->Draw();
      l->SetLineColor(kRed);
    }   

  TCanvas *cpwp = new TCanvas("cpwp", "Widths", 900,700);
  cpwp->cd();
  cpwp->Divide(NDX,NDY);
  for (i=0; i<MaxPlane; i++)
    {
      cpwp->cd(champosn[i]+1);
      if (WvsS[i]->GetEntries() < 500) continue;
      ymin = Wcut;
      ymax = Wmax;
      xmin = 0;
      xmax = (WvsS[i]->ProjectionY())->GetMaximum();
      WvsS[i]->ProjectionY(strcat(strcpy(filn,WvsS[i]->GetName()),"_y"), 1, 32, "D");
      gPad->AddExec("exselt","select_hist()");
      l = new TLine(ymin,xmin,ymin,xmax);
      l->Draw();
      l->SetLineColor(kRed);
      l = new TLine(ymax,xmin,ymax,xmax);
      l->Draw();
      l->SetLineColor(kRed);
    }   

  TCanvas *cpt0 = new TCanvas("cpt0", "T0 hists", 900,700);
  cpt0->cd();
  cpt0->Divide(1,2);
  cpt0->cd(1);
  Wt0->Draw();
  gPad->AddExec("exselt","select_hist()");
  xmin = WT0min;
  xmax = WT0max;
  ymin = 0.;
  ymax = Wt0->GetMaximum();
  l = new TLine(xmin,ymin,xmin,ymax);
  l->Draw();
  l->SetLineColor(kRed);
  l = new TLine(xmax,ymin,xmax,ymax);
  l->Draw();
  l->SetLineColor(kRed);
  cpt0->cd(2);
  Wts->Draw();
  gPad->AddExec("exselt","select_hist()");

  if (1) return;

  TCanvas *cp1 = new TCanvas("cp1", "Leadings vs widths", 900,700);
  cp1->cd();
  cp1->Divide(NDX,NDY);
  for (i=0; i<MaxPlane; i++)
    {
      cp1->cd(champosn[i]+1);
      TvsWall[i]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   

  TCanvas *cp2 = new TCanvas("cp2", "Leadings vs widths (max strip)", 900,700);
 
  cp2->cd();
  cp2->Divide(NDX,NDY);
  for (i=0; i<MaxPlane; i++)
    {
      cp2->cd(champosn[i]+1);
      TvsWallmax[i]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   
  return;
}

void BmnTof2Raw2DigitNew::drawprof()
{
  int NDX;
  int NDY;
  if (MaxPlane == 15)
  {
    int champos[] = {5,10,1,6,11,2,7,12,3,8,13,4,9,14,0};
    NDX = 5;
    NDY = 3;
    memcpy(champosn, champos, sizeof champosn);
  }
  else if (MaxPlane == 24)
  {
    int champos[] = {17,18, 3, 1,19, 4,23,20, 5,15,21, 6, 2,22, 9,10,11,12,13,14, 7, 8, 0,16};
    NDX = 8;
    NDY = 3;
    memcpy(champosn, champos, sizeof champosn);
  }
  else
  {
    int champos[TOF2_MAX_CHAMBERS ] = {0};
    NDX = 6;
    NDY = 10;
    for (int ip=0; ip<TOF2_MAX_CHAMBERS; ip++) champos[ip] = ip;
    memcpy(champosn, champos, sizeof champosn);
  }

  TCanvas *callbe = new TCanvas("callbe", "Leadings vs widths (slewing RPC, peak 1)", 900,700);
 
  int i;
  callbe->cd();
  callbe->Divide(NDX,NDY);
  for (i=0; i<MaxPlane; i++)
    {
      callbe->cd(champosn[i]+1);
      TvsW[i][0]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   

  TCanvas *callbe1 = new TCanvas("callbe1", "Leadings vs widths (slewing RPC, peak 2)", 900,700);
  callbe1->cd();
  callbe1->Divide(NDX,NDY);
  for (i=0; i<MaxPlane; i++)
    {
      callbe1->cd(champosn[i]+1);
      TvsW[i][1]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   

  return;
}

void BmnTof2Raw2DigitNew::drawproft0()
{
  int NDX;
  int NDY;
  if (MaxPlane == 15)
  {
    int champos[] = {5,10,1,6,11,2,7,12,3,8,13,4,9,14,0};
    NDX = 5;
    NDY = 3;
    memcpy(champosn, champos, sizeof champosn);
  }
  else if (MaxPlane == 24)
  {
    int champos[] = {17,18, 3, 1,19, 4,23,20, 5,15,21, 6, 2,22, 9,10,11,12,13,14, 7, 8, 0,16};
    NDX = 8;
    NDY = 3;
    memcpy(champosn, champos, sizeof champosn);
  }
  else
  {
    int champos[TOF2_MAX_CHAMBERS ] = {0};
    NDX = 6;
    NDY = 10;
    for (int ip=0; ip<TOF2_MAX_CHAMBERS; ip++) champos[ip] = ip;
    memcpy(champosn, champos, sizeof champosn);
  }

  TCanvas *callbe0 = new TCanvas("callbe0", "Leadings vs widths (slewing T0, peak 1)", 900,700);
  int i;
  callbe0->cd();
  callbe0->Divide(NDX,NDY);
  for (i=0; i<MaxPlane; i++)
    {
      callbe0->cd(champosn[i]+1);
      TvsWt0[i][0]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   

  TCanvas *callbe01 = new TCanvas("callbe01", "Leadings vs widths (slewing T0, peak 2)", 900,700);
  callbe01->cd();
  callbe01->Divide(NDX,NDY);
  for (i=0; i<MaxPlane; i++)
    {
      callbe01->cd(champosn[i]+1);
      TvsWt0[i][1]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   
  return;
}


int BmnTof2Raw2DigitNew::readGeom(char *geomfile)
{
	char fname[128];
	FILE *fg = 0;
	float ic = 0;
	int nf = 0, n = 0, i;
	float step, sx, sy, x, y, z;
	if (strlen(geomfile) == 0)
	{
	    printf("TOF700 geometry file name not defined!\n");
	    return 0;
	}
	TString dir = getenv("VMCWORKDIR");
	sprintf(fname,"%s/geometry/%s",dir.Data(),geomfile);
	fg = fopen(fname,"r");
	if (fg == NULL)
	{
	    printf("TOF700 geometry file %s open error!\n", fname);
	    return 0;
	}
	if (fscanf(fg,"%f %f %f\n", &xoffs, &yoffs, &zoffs) != 3)
	{
	    printf("Wrong first line in TOF700 geometry file %s\n", fname);
	    return 0;
	};
	printf("Loading TOF700 geometry from file %s\n", geomfile);
	for (i=0; i<MaxPlane; i++) nstrips[i] = 0;
	while(fscanf(fg,"%f %d %f %f %f %f %f %f\n", &ic, &n, &step, &sy, &sx, &x, &y, &z) == 8)
	{
		for (i=0; i<MaxPlane; i++)
		{
		    if (ic == idchambers[i]) break;
		}
		if (i >= MaxPlane) continue;
		halfxwidth[i] = sx/20.;
		halfywidth[i] = sy/20.;
		zchamb[i] = z/10. + zoffs;
		nstrips[i] = n;
		for (int ns=n-1; ns>=0; ns--)
		{
		xcens[i][ns] = -x/10. + xoffs;
		ycens[i][ns] = y/10. + yoffs - (n-ns-1)*step/10.;
		xmins[i][ns] = xcens[i][ns] - halfxwidth[i];
		xmaxs[i][ns] = xcens[i][ns] + halfxwidth[i];
		ymins[i][ns] = ycens[i][ns] - halfywidth[i];
		ymaxs[i][ns] = ycens[i][ns] + halfywidth[i];
//		printf("C %d S %d %f %f %f %f %f\n",ic,ns,zchamb[i],xmins[i][ns],xmaxs[i][ns],ymins[i][ns],ymaxs[i][ns]);
		}
		nf++;
	}
	fclose(fg);
	if (nf == MaxPlane) return 1;
	else
	{
	    printf("Geometry for %d chambers only!\n", nf);
	    return 0;
	}
}

int BmnTof2Raw2DigitNew::get_strip_xyz(int chamber, int strip, float *x, float *y, float *z)
{
    if (chamber < MaxPlane)
    {
	*x = xcens[chamber][strip];
	*y = ycens[chamber][strip];
	*z = zchamb[chamber];
	return 1;
    }
    else
	return 0;
}

int BmnTof2Raw2DigitNew::get_chamber_z(int chamber, float *z)
{
    if (chamber < MaxPlane)
    {
	*z = zchamb[chamber];
	return 1;
    }
    else
	return 0;
}

int BmnTof2Raw2DigitNew::get_track_hits(float *xyz, float *cxyz, int *nhits, int *chamb, int *strip)
{
    int n = 0;
    float x, y, z;
    for (int j=0; j<MaxPlane; j++)
    {
	x = xyz[0] + cxyz[0]*(zchamb[j] - xyz[2]);
	y = xyz[1] + cxyz[1]*(zchamb[j] - xyz[2]);
	for (int k=0; k<nstrips[j]; k++)
	{
	    if (x >= xmins[j][k] && x < xmaxs[j][k] && y >= ymins[j][k] && y < ymaxs[j][k])
	    {
		chamb[n] = j;
		strip[n] = k;
		n++;
	    }
	}
    }
    *nhits = n;
    return n;
}

int BmnTof2Raw2DigitNew::printGeom()
{
    for (int j=0; j<MaxPlane; j++)
    {
	printf("Chamber %d (%.1f), number of strips %d, Z-coordinate %f, WidthX/2 %f, WidthY/2 %f\n",j+1,idchambers[j],nstrips[j],zchamb[j], halfxwidth[j], halfywidth[j]);
	printf(" Upper strip X %f Y %f\n", xcens[j][nstrips[j]-1],ycens[j][nstrips[j]-1]);
//	for (int k=0; k<nstrips[j]; k++)
//	{
//		printf("   Strip %d X %f Y %f\n", k+1, xcens[j][k],ycens[j][k]);
//	}
    }
}

int BmnTof2Raw2DigitNew::readLRoffsets(char *offsetsfile)
{
	char fname[128];
	FILE *fg = 0;
	int i;
	if (strlen(offsetsfile) == 0)
	{
	    printf("TOF700 Left-Right offsets file name not defined!\n");
	    return 0;
	}
	TString dir = getenv("VMCWORKDIR");
	sprintf(fname,"%s/geometry/%s",dir.Data(),offsetsfile);
	fg = fopen(fname,"r");
	if (fg == NULL)
	{
	    printf("TOF700 Left-Right offsets file %s open error!\n", fname);
	    return 0;
	}
	printf("Loading TOF700 Left-Right offsets from file %s\n", offsetsfile);
	float idchamber, lroff;
	int ip, is, lrsig, n = 0;
	while(fscanf(fg, "Chamber %d (%f) strip %d offset %f time bins, direction %d\n", &ip, &idchamber, &is, &lroff, &lrsig) == 5)
	{
	    if (ip>=MaxPlane)
	    {
		printf("L-R offsets: Chamber number %d >= %d, skip!\n", ip, MaxPlane);
		continue;
	    }
	    if (is>=TOF2_MAX_STRIPS_IN_CHAMBER)
	    {
		printf("L-R offsets: Strip number %d >= %d, skip!\n", is, TOF2_MAX_STRIPS_IN_CHAMBER);
		continue;
	    }
	    if (idchamber != idchambers[ip])
	    {
		printf("L-R offsets: Chamber ID %.1f != %.1f, skip!\n", idchamber, idchambers[ip]);
		continue;
	    }
	    lroffsets[ip][is] = lroff;
	    lrsign[ip][is] = lrsig;
	    n++;
	}
	fclose(fg);
	printf("Read %d left-right offsets.\n", n);
	return 1;
}

float BmnTof2Raw2DigitNew::get_hit_x0(int chamber, int strip, float diff)
{
    float x = 0., dx = 0.;
    if (chamber < MaxPlane && strip < TOF2_MAX_STRIPS_IN_CHAMBER && fVelosity > 0.)
    {
	dx = (diff + lroffsets[chamber][strip]*HPTIMEBIN)*fVelosity;
	x = xcens[chamber][strip] + lrsign[chamber][strip]*dx;
	return x;
    }
    else
	return 0.;
}

float BmnTof2Raw2DigitNew::get_hit_x(int chamber, int strip, float diff_corrected)
{
    float x = 0., dx = 0.;
    if (chamber < MaxPlane && strip < TOF2_MAX_STRIPS_IN_CHAMBER && fVelosity > 0.)
    {
	dx = diff_corrected*fVelosity;
	x = xcens[chamber][strip] + lrsign[chamber][strip]*dx;
	return x;
    }
    else
	return 0.;
}

float BmnTof2Raw2DigitNew::get_hit_diff0(int chamber, int strip, float diff)
{
    if (chamber < MaxPlane && strip < TOF2_MAX_STRIPS_IN_CHAMBER)
    {
	return (diff + lroffsets[chamber][strip]*HPTIMEBIN);
    }
    else
	return 0.;
}

float BmnTof2Raw2DigitNew::get_hit_diff(int chamber, int strip, float diff_corrected)
{
    if (chamber < MaxPlane && strip < TOF2_MAX_STRIPS_IN_CHAMBER)
    {
	return diff_corrected;
    }
    else
	return 0.;
}

void BmnTof2Raw2DigitNew::get_hit_xyz0(int chamber, int strip, float diff, float *x, float *y, float *z)
{
    float xh = 0., dxh = 0.;
    if (chamber < MaxPlane && strip < TOF2_MAX_STRIPS_IN_CHAMBER && fVelosity > 0.)
    {
	dxh = (diff + lroffsets[chamber][strip]*HPTIMEBIN)*fVelosity;
	xh = xcens[chamber][strip] + lrsign[chamber][strip]*dxh;
	*x = xh;
	*y = ycens[chamber][strip];
	*z = zchamb[chamber];
	return;
    }
    else
	return;
}

void BmnTof2Raw2DigitNew::get_hit_xyz(int chamber, int strip, float diff_corrected, float *x, float *y, float *z)
{
    float xh = 0., dxh = 0.;
    if (chamber < MaxPlane && strip < TOF2_MAX_STRIPS_IN_CHAMBER && fVelosity > 0.)
    {
	dxh = diff_corrected*fVelosity;
	xh = xcens[chamber][strip] + lrsign[chamber][strip]*dxh;
	*x = xh;
	*y = ycens[chamber][strip];
	*z = zchamb[chamber];
	return;
    }
    else
	return;
}

int BmnTof2Raw2DigitNew::get_ch_tdc32vl(unsigned int tdc,unsigned int ch){
     const  int tdc32vl_tdcid2tdcnum[16] = { -1, 0, 1, -1, 3, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1};
     const  int tdc32vl_tdcch2ch[32] = {7,7,7,7, 6,6,6,6, 5,5,5,5, 4,4,4,4, 3,3,3,3, 2,2,2,2, 1,1,1,1, 0,0,0,0};
     return tdc32vl_tdcid2tdcnum[tdc]*8+tdc32vl_tdcch2ch[ch%32];
};
int BmnTof2Raw2DigitNew::get_ch_tdc64vhle(unsigned int tdc,unsigned int ch){
     const  int tdc64vhle_tdcid2tdcnum[16] = { 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1};
     const  int tdc64vhle_tdcch2ch[32] = {7,7,7,7, 6,6,6,6, 5,5,5,5, 4,4,4,4, 3,3,3,3, 2,2,2,2, 1,1,1,1, 0,0,0,0};
     return tdc64vhle_tdcid2tdcnum[tdc]*8+7-tdc64vhle_tdcch2ch[ch%32];
};
int BmnTof2Raw2DigitNew::get_ch_tdc72vhl(unsigned int tdc,unsigned int ch){
     const int tdc72vhl_tdcid2tdcnum[16] = {  2, 1, 0, 5, 4, 3, 8, 7, 6, -1, -1, -1, -1, -1, -1, -1 };
     const int tdc72vhl_tdcch2ch[32] = { 7,7,7,7, 6,6,6,6, 5,5,5,5, 4,4,4,4, 3,3,3,3, 2,2,2,2, 1,1,1,1, 0,0,0,0};
     return tdc72vhl_tdcid2tdcnum[tdc]*8+tdc72vhl_tdcch2ch[ch%32]; 
}

ClassImp(BmnTof2Raw2DigitNew)

