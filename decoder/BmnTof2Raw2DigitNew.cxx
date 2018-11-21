#include "TF1.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TProfile2D.h"
#include "TH2D.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TFile.h"
#include "BmnTof2Raw2DigitNew.h"

#define TOZERO 1
#define EQUAL_MAXIMA 1
#define FIT_MAXIMA 1
#define EQUAL_AVERAGE_T0 1
#define EQUAL_AVERAGE 0
#define CABLE_OFFSETS 0
#define TRUE_OFFSETS 1
#define ALL_CORRECTIONS 1
#define DRAW_OFFSETS 0
#define PRINT_CORRECTIONS 0
#define PRINT_TIME_LIMITS 0
#define PRINT_SLEWING_RESULTS 0
#define PRINT_SLEWING_PARAMETERS 0
#define PRINT_EQUALIZATION_RESULTS 0
#define DEBUG_EQUALIZATION 0

//module IDs
const UInt_t kTDC64VHLE = 0x53;
const UInt_t kTDC72VHL = 0x12;
const UInt_t kTDC32VL = 0x11;

BmnTof2Raw2DigitNew::BmnTof2Raw2DigitNew(){
    n_rec=0;
}

BmnTof2Raw2DigitNew::BmnTof2Raw2DigitNew(TString mappingFile, TString RunFile, UInt_t SlewingRun, UInt_t SlewingChamber, TString geomFile) {

    char *delim = 0, name[128], title[128];
    n_rec=0;
    TString dummy;

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/input/";
    strcpy(filname_base, gSystem->BaseName(RunFile.Data()));
    if ((delim = strrchr(filname_base, (int)'.'))) *delim = '\0';

    int RUN;
    const char *fname = RunFile.Data();
    sscanf(&fname[strlen(fname) - 9], "%d", &RUN);

    ifstream in;
    in.open((path + mappingFile).Data());
    if (!in.is_open())
    {
	printf("Loading TOF700 Map from file: %s - file open error!\n", mappingFile.Data());
	return;
    }

    fSlewCham = SlewingChamber;
    if (SlewingRun != 0)
    {
	sprintf((char *)&filname_base[strlen(filname_base) - 8], "%04d_raw", SlewingRun);
	if (SlewingChamber == 0) printf("Loading TOF700 Map from file: %s , reference slewing run %d\n",mappingFile.Data(), SlewingRun);
	else                     printf("Loading TOF700 Map from file: %s , reference slewing run %d, chamber %d\n",mappingFile.Data(), SlewingRun, SlewingChamber);
    }
    else
    {
	printf("Loading TOF700 Map from file: %s\n",mappingFile.Data());
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
		ntmean[0][n_rec] = 0;
		ntmean[1][n_rec] = 0;
		tmean[0][n_rec] = 0.;
		tmean[1][n_rec] = 0.;
		ntmeane[n_rec] = 0;
		tmeane[n_rec] = 0.;
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
		ntmean[0][n_rec] = 0;
		ntmean[1][n_rec] = 0;
		tmean[0][n_rec] = 0.;
		tmean[1][n_rec] = 0.;
		ntmeane[n_rec] = 0;
		tmeane[n_rec] = 0.;
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
		ntmean[0][n_rec] = 0;
		ntmean[1][n_rec] = 0;
		tmean[0][n_rec] = 0.;
		tmean[1][n_rec] = 0.;
		ntmeane[n_rec] = 0;
		tmeane[n_rec] = 0.;
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
		ntmean[0][n_rec] = 0;
		ntmean[1][n_rec] = 0;
		tmean[0][n_rec] = 0.;
		tmean[1][n_rec] = 0.;
		ntmeane[n_rec] = 0;
		tmeane[n_rec] = 0.;
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
/*	    for (int j=0; j<TOF2_MAX_CHANNELS_IN_SLOT; j++) */
	    for (int j=0; j<72; j++)
		for (int k=0; k<1024; k++)
		    DNL_Table[c][i][j][k] = 0.;
	}
    DNL_read();
    
    for (int c=0; c<TOF2_MAX_CHAMBERS; c++)
	for (int i=0; i<2; i++)
	{
	    TvsWt0_const[c][i] = 0.;
	    TvsWt0_slope[c][i] = 0.;
	    TvsWt0_parab[c][i] = 0.;
	    TvsW_const[c][i] = 0.;
	    TvsW_slope[c][i] = 0.;
	    TvsW_parab[c][i] = 0.;
	    TvsW_cubic[c][i] = 0.;
	    TvsW_four[c][i] = 0.;
	    TvsW_five[c][i] = 0.;
	    TvsW[c][i] = NULL;
	    TvsWt0[c][i] = NULL;
	    wmint0[c][i] = 0;
	    wmaxt0[c][i] = 0;
	    wmin[c][i] = 0;
	    wmax[c][i] = 0;
	}

    for (int c=0; c<TOF2_MAX_CHANNEL; c++)
    {
	ntmeane[c] = 0;
	tmeane[c] = 0.;
	for (int i=0; i<2; i++)
	{
	    ntmean[i][c] = 0;
	    tmean[i][c] = 0.;
	}
    }

    Wcut = 2800;
    Wmax = 5000;
    WT0min = 720;
    WT0max = 820;

    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) LeadMin[i] = -5000;
    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) LeadMax[i] = +5000;
    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) Wcutc[i] = -1;
    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) Wmaxc[i] = -1;

// SRC
//    T0shift = -10000.;
// BMN
    T0shift = +147000.;

    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) numstrip[i] = -1;

    Wt0 = 0;
    Wts = 0;

    for (int i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
	TvsS[i] = 0;
	WvsS[i] = 0;
    }
    for (int i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
	TvsWall[i] = 0;
	TvsWallmax[i] = 0;
    }
    for (int i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
	TvsW[i][0] = 0;
	TvsWt0[i][0] = 0;
	TvsWp[i][0] = 0;
	TvsWt0p[i][0] = 0;
	TvsSm[i][0] = 0;
    }
    for (int i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
	TvsW[i][1] = 0;
	TvsWt0[i][1] = 0;
	TvsWp[i][1] = 0;
	TvsWt0p[i][1] = 0;
	TvsSm[i][1] = 0;
    }
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

    for (int i=0; i<MaxPlane; i++)
    {
	if (TvsW[i][0]) delete TvsW[i][0];
	TvsW[i][0] = 0;
	if (TvsWt0[i][0]) delete TvsWt0[i][0];
	TvsWt0[i][0] = 0;
	if (TvsWp[i][0]) delete TvsWp[i][0];
	TvsWp[i][0] = 0;
	if (TvsWt0p[i][0]) delete TvsWt0p[i][0];
	TvsWt0p[i][0] = 0;
	if (TvsSm[i][0]) delete TvsSm[i][0];
	TvsSm[i][0] = 0;
    }
    for (int i=0; i<MaxPlane; i++)
    {
	if (TvsW[i][1]) delete TvsW[i][1];
	TvsW[i][1] = 0;
	if (TvsWt0[i][1]) delete TvsWt0[i][1];
	TvsWt0[i][1] = 0;
	if (TvsWp[i][1]) delete TvsWp[i][1];
	TvsWp[i][1] = 0;
	if (TvsWt0p[i][1]) delete TvsWt0p[i][1];
	TvsWt0p[i][1] = 0;
	if (TvsSm[i][1]) delete TvsSm[i][1];
	TvsSm[i][1] = 0;
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

void BmnTof2Raw2DigitNew::InitEqualization()
{
	char name[128], title[128];
	for (int i=0; i<TOF2_MAX_CHANNEL; i++)
	{
		ntmeane[i] = 0;
		tmeane[i] = 0.;
	}
	for (int i=0; i<MaxPlane; i++)
	{
		sprintf(name, "Time_vs_Strip_Chamber_%.1f_Both Peaks_Maxima",idchambers[i]);
		sprintf(title, "Time vs Strip Chamber %.1f Both Peaks Maxima",idchambers[i]);
		if (TvsSm[i][0]) delete TvsSm[i][0];
		TvsSm[i][0] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER, (LeadMax[i]-LeadMin[i]), -(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2);
		TvsSm[i][0]->SetDirectory(0);
	}
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

    sprintf(name, "Width_T0");
    sprintf(title, "Width T0");
    Wt0 = new TH1F(name,title,1500,0,1500);

    sprintf(name, "TS_difference");
    sprintf(title, "TS difference");
    Wts = new TH1F(name,title,200,-100,100);

    for (int i=0; i<MaxPlane; i++)
    {
	sprintf(name, "Time_vs_Strip_Chamber_%.1f",idchambers[i]);
	sprintf(title, "Time vs Strip Chamber %.1f",idchambers[i]);
	TvsS[i] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,10000, 0., 10000.);
	TvsS[i]->SetDirectory(0);
	sprintf(name, "Width_vs_Strip_Chamber_%.1f",idchambers[i]);
	sprintf(title, "Width vs Strip Chamber %.1f",idchambers[i]);
	WvsS[i] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,10000, 0., +10000.);
	WvsS[i]->SetDirectory(0);
    }
    for (int i=0; i<MaxPlane; i++)
    {
	sprintf(name, "Time_vs_Width_Chamber_%.1f_all",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f all",idchambers[i]);
	TvsWall[i] = new TH2F(name,title,Wmax,0,Wmax,LeadMax[i]-LeadMin[i],LeadMin[i],LeadMax[i]);
	TvsWall[i]->SetDirectory(0);
	sprintf(name, "Time_vs_Width_Chamber_%.1f_all_max",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f all, max strip",idchambers[i]);
	TvsWallmax[i] = new TH2F(name,title,Wmax,0,Wmax,LeadMax[i]-LeadMin[i],LeadMin[i],LeadMax[i]);
	TvsWallmax[i]->SetDirectory(0);
    }
    printf(" TOF700 booking done for %d chambers!\n", MaxPlane);
}

void BmnTof2Raw2DigitNew::BookSlewing()
{
    char name[128], title[128];

    gStyle->SetOptFit(111);

    for (int i=0; i<MaxPlane; i++)
    {
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_1",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f Peak 1",idchambers[i]);
	if (TvsW[i][0]) delete TvsW[i][0];
	TvsW[i][0] = new TProfile(name,title,Wcut,0,Wcut,-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2,"e");
	TvsW[i][0]->Sumw2(kTRUE);
	TvsW[i][0]->SetDirectory(0);
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_1",idchambers[i]);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak 1",idchambers[i]);
	if (TvsWt0[i][0]) delete TvsWt0[i][0];
	TvsWt0[i][0] = new TProfile(name,title,(WT0max-WT0min),WT0min,WT0max,LeadMin[i],LeadMax[i],"e");
	TvsWt0[i][0]->Sumw2(kTRUE);
	TvsWt0[i][0]->SetDirectory(0);

	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_1_plot",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f Peak 1 plot",idchambers[i]);
	if (TvsWp[i][0]) delete TvsWp[i][0];
	TvsWp[i][0] = new TH2F(name,title,Wcut,0,Wcut,LeadMax[i]-LeadMin[i],-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2);
	TvsWp[i][0]->SetDirectory(0);
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_1_plot",idchambers[i]);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak 1 plot",idchambers[i]);
	if (TvsWt0p[i][0]) delete TvsWt0p[i][0];
	TvsWt0p[i][0] = new TH2F(name,title,(WT0max-WT0min),WT0min,WT0max,LeadMax[i]-LeadMin[i],LeadMin[i],LeadMax[i]);
	TvsWt0p[i][0]->SetDirectory(0);

	sprintf(name, "Time_vs_Strip_Chamber_%.1f_Peak_1_Maxima",idchambers[i]);
	sprintf(title, "Time vs Strip Chamber %.1f Peak 1 Maxima",idchambers[i]);
	if (TvsSm[i][0]) delete TvsSm[i][0];
	TvsSm[i][0] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,10000, 0., 10000.);
	TvsSm[i][0]->SetDirectory(0);
    }
    for (int i=0; i<MaxPlane; i++)
    {
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_2",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f Peak 2",idchambers[i]);
	if (TvsW[i][1]) delete TvsW[i][1];
	TvsW[i][1] = new TProfile(name,title,(Wmax-Wcut),Wcut,Wmax,-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2,"e");
	TvsW[i][1]->Sumw2(kTRUE);
	TvsW[i][1]->SetDirectory(0);
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_2",idchambers[i]);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak 2",idchambers[i]);
	if (TvsWt0[i][1]) delete TvsWt0[i][1];
	TvsWt0[i][1] = new TProfile(name,title,(WT0max-WT0min),WT0min,WT0max,LeadMin[i],LeadMax[i],"e");
	TvsWt0[i][1]->Sumw2(kTRUE);
	TvsWt0[i][1]->SetDirectory(0);

	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_2_plot",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f Peak 2 plot",idchambers[i]);
	if (TvsWp[i][1]) delete TvsWp[i][1];
	TvsWp[i][1] = new TH2F(name,title,(Wmax-Wcut),Wcut,Wmax,LeadMax[i]-LeadMin[i],-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2);
	TvsWp[i][1]->SetDirectory(0);
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_2_plot",idchambers[i]);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak 2 plot",idchambers[i]);
	if (TvsWt0p[i][1]) delete TvsWt0p[i][1];
	TvsWt0p[i][1] = new TH2F(name,title,(WT0max-WT0min),WT0min,WT0max,LeadMax[i]-LeadMin[i],LeadMin[i],LeadMax[i]);
	TvsWt0p[i][1]->SetDirectory(0);

	sprintf(name, "Time_vs_Strip_Chamber_%.1f_Peak_2_Maxima",idchambers[i]);
	sprintf(title, "Time vs Strip Chamber %.1f Peak 2 Maxima",idchambers[i]);
	if (TvsSm[i][1]) delete TvsSm[i][1];
	TvsSm[i][1] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,10000, 0., 10000.);
	TvsSm[i][1]->SetDirectory(0);
    }
}

void BmnTof2Raw2DigitNew::BookSlewingResults()
{
    char name[128], title[128];

    gStyle->SetOptFit(111);

    for (int i=0; i<MaxPlane; i++)
    {
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_1",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f Peak 1",idchambers[i]);
	if (TvsW[i][0]) delete TvsW[i][0];
	TvsW[i][0] = new TProfile(name,title,Wcut,0,Wcut,-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2,"e");
	TvsW[i][0]->Sumw2(kTRUE);
	TvsW[i][0]->SetDirectory(0);
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_1",idchambers[i]);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak 1",idchambers[i]);
	if (TvsWt0[i][0]) delete TvsWt0[i][0];
	TvsWt0[i][0] = new TProfile(name,title,(WT0max-WT0min),WT0min,WT0max,-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2,"e");
	TvsWt0[i][0]->Sumw2(kTRUE);
	TvsWt0[i][0]->SetDirectory(0);
    }
    for (int i=0; i<MaxPlane; i++)
    {
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_2",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f Peak 2",idchambers[i]);
	if (TvsW[i][1]) delete TvsW[i][1];
	TvsW[i][1] = new TProfile(name,title,(Wmax-Wcut),Wcut,Wmax,-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2,"e");
	TvsW[i][1]->Sumw2(kTRUE);
	TvsW[i][1]->SetDirectory(0);
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_2",idchambers[i]);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak 2",idchambers[i]);
	if (TvsWt0[i][1]) delete TvsWt0[i][1];
	TvsWt0[i][1] = new TProfile(name,title,(WT0max-WT0min),WT0min,WT0max,-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2,"e");
	TvsWt0[i][1]->Sumw2(kTRUE);
	TvsWt0[i][1]->SetDirectory(0);
    }
}

void BmnTof2Raw2DigitNew::WritePreparationHists()
{
    TFile *f = NULL;
    char filn[128];
    sprintf(filn, "%s_preparation.root", filname_base);
    f = new TFile(filn,"RECREATE","Preparation Results of BmnTOF700");
    for (int i=0; i<MaxPlane; i++)
    {
	if (TvsS[i]) TvsS[i]->Write();
	if (WvsS[i]) WvsS[i]->Write();
	if (TvsWall[i]) TvsWall[i]->Write();
	if (TvsWallmax[i]) TvsWallmax[i]->Write();
    }
    f->Close();
    delete f;
}


void BmnTof2Raw2DigitNew::WriteSlewingHists()
{
    TFile *f = NULL;
    char filn[128];
    sprintf(filn, "%s_slewing.root", filname_base);
    f = new TFile(filn,"RECREATE","Slewing Results of BmnTOF700");
    for (int i=0; i<MaxPlane; i++)
    {
	if (TvsW[i][0]) TvsW[i][0]->Write();
	if (TvsWt0[i][0]) TvsWt0[i][0]->Write();
	if (TvsWp[i][0]) TvsWp[i][0]->Write();
	if (TvsWt0p[i][0]) TvsWt0p[i][0]->Write();
	if (TvsSm[i][0]) TvsSm[i][0]->Write();
    }
    for (int i=0; i<MaxPlane; i++)
    {
	if (TvsW[i][1]) TvsW[i][1]->Write();
	if (TvsWt0[i][1]) TvsWt0[i][1]->Write();
	if (TvsWp[i][1]) TvsWp[i][1]->Write();
	if (TvsWt0p[i][1]) TvsWt0p[i][1]->Write();
	if (TvsSm[i][1]) TvsSm[i][1]->Write();
    }
    f->Close();
    delete f;
}


void BmnTof2Raw2DigitNew::ReBook(int i)
{
    char name[128], title[128];
    int Wc = Wcut, Wm = Wmax;
    if (Wcutc[i] >= 0.) Wc = Wcutc[i];
    if (Wmaxc[i] >= 0.) Wm = Wmaxc[i];
    if (TvsW[i][0])
    {	
	delete TvsW[i][0];
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_1",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f Peak 1",idchambers[i]);
	TvsW[i][0] = new TProfile(name,title,Wc,0,Wc,-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2,"e");
	TvsW[i][0]->Sumw2(kTRUE);
	TvsW[i][0]->SetDirectory(0);
    }
    if (TvsWt0[i][0])
    {
	delete TvsWt0[i][0];
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_1",idchambers[i]);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak 1",idchambers[i]);
	TvsWt0[i][0] = new TProfile(name,title,(WT0max-WT0min),WT0min,WT0max,LeadMin[i],LeadMax[i],"e");
	TvsWt0[i][0]->Sumw2(kTRUE);
	TvsWt0[i][0]->SetDirectory(0);
    }

    if (TvsWp[i][0])
    {	
	delete TvsWp[i][0];
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_1_plot",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f Peak 1 plot",idchambers[i]);
	TvsWp[i][0] = new TH2F(name,title,Wc,0,Wc,LeadMax[i]-LeadMin[i],-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2);
    }
    if (TvsWt0p[i][0])
    {
	delete TvsWt0p[i][0];
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_1",idchambers[i]);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak 1",idchambers[i]);
	TvsWt0p[i][0] = new TH2F(name,title,(WT0max-WT0min),WT0min,WT0max,LeadMax[i]-LeadMin[i],LeadMin[i],LeadMax[i]);
    }


    if (TvsWall[i])
    {
	delete TvsWall[i];
	sprintf(name, "Time_vs_Width_Chamber_%.1f_all",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f all",idchambers[i]);
	TvsWall[i] = new TH2F(name,title,Wm,0,Wm,LeadMax[i]-LeadMin[i],LeadMin[i],LeadMax[i]);
    }
    if (TvsWallmax[i])
    {
	delete TvsWallmax[i];
	sprintf(name, "Time_vs_Width_Chamber_%.1f_all_max",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f all, max strip",idchambers[i]);
	TvsWallmax[i] = new TH2F(name,title,Wm,0,Wm,LeadMax[i]-LeadMin[i],LeadMin[i],LeadMax[i]);
    }
    if (TvsW[i][1])
    {
	delete TvsW[i][1];
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_2",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f Peak 2",idchambers[i]);
	TvsW[i][1] = new TProfile(name,title,(Wm-Wc),Wc,Wm,-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2,"e");
	TvsW[i][1]->Sumw2(kTRUE);
	TvsW[i][1]->SetDirectory(0);
    }
    if (TvsWt0[i][1])
    {
	delete TvsWt0[i][1];
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_2",idchambers[i]);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak 2",idchambers[i]);
	TvsWt0[i][1] = new TProfile(name,title,(WT0max-WT0min),WT0min,WT0max,LeadMin[i],LeadMax[i],"e");
	TvsWt0[i][1]->Sumw2(kTRUE);
	TvsWt0[i][1]->SetDirectory(0);
    }

    if (TvsWp[i][1])
    {	
	delete TvsWp[i][1];
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_2_plot",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f Peak 2 plot",idchambers[i]);
	TvsWp[i][1] = new TH2F(name,title,(Wm-Wc),Wc,Wm,LeadMax[i]-LeadMin[i],-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2);
    }
    if (TvsWt0p[i][1])
    {
	delete TvsWt0p[i][1];
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_2_plor",idchambers[i]);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak 2 plot",idchambers[i]);
	TvsWt0p[i][1] = new TH2F(name,title,(WT0max-WT0min),WT0min,WT0max,LeadMax[i]-LeadMin[i],LeadMin[i],LeadMax[i]);
    }


    if (TvsSm[i][0])
    {
	delete TvsSm[i][0];
	sprintf(name, "Time_vs_Strip_Chamber_%.1f_Peak_1_Maxima",idchambers[i]);
	sprintf(title, "Time vs Strip Chamber %.1f Peak 1 Maxima",idchambers[i]);
	TvsSm[i][0] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,10000, 0., +10000.);
	TvsSm[i][0]->SetDirectory(0);
    }
    if (TvsSm[i][1])
    {
	delete TvsSm[i][1];
	sprintf(name, "Time_vs_Strip_Chamber_%.1f_Peak_2_Maxima",idchambers[i]);
	sprintf(title, "Time vs Strip Chamber %.1f Peak 2 Maxima",idchambers[i]);
	TvsSm[i][1] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,10000, 0., +10000.);
	TvsSm[i][1]->SetDirectory(0);
    }
//    printf("c %d wc %d wm %d wt1 %d wt2 %d lmi %d lma %d\n", i, Wcut, Wmax, WT0min, WT0max, LeadMin[i], LeadMax[i]);
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
	if (((W1 < Wc && W2 < Wc)||(W1 >= Wc && W2 >= Wc)))
	{
	    TvsS[mapa[ind].plane]->Fill(mapa[ind].strip, L);
	    WvsS[mapa[ind].plane]->Fill(mapa[ind].strip, W);
	    TvsWall[mapa[ind].plane]->Fill(W, L);
	}
	if (W > wmaxs[mapa[ind].plane] && ((W1 < Wc && W2 < Wc)||(W1 >= Wc && W2 >= Wc)))
	{
		wmaxs[mapa[ind].plane] = W;
		tmaxs[mapa[ind].plane] = L;
		smax[mapa[ind].plane] = mapa[ind].strip;
	}
       }
    }
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
	if (L >= LeadMin[mapa[ind].plane] && L < LeadMax[mapa[ind].plane])
	{
	if ((int)W1 < Wc && (int)W2 < Wc)
	{
	    TvsWt0[mapa[ind].plane][0]->Fill(t0width*INVHPTIMEBIN, L);
	    TvsWt0p[mapa[ind].plane][0]->Fill(t0width*INVHPTIMEBIN, L);
	    tmean[0][ind] += L;
	    ntmean[0][ind]++;
	    TvsSm[mapa[ind].plane][0]->Fill(mapa[ind].strip, L);
//	    if (ntmean[0][ind] == 1) printf("%d %d %f %f %f %f %d %f\n",ind,ind1,lead[ind],lead[ind1],L, tmean[0][ind], ts_diff, t0width);
	}
	else if ((int)W1 >= Wc && (int)W2 >= Wc && (int)W1 < Wm && (int)W2 < Wm)
	{
	    TvsWt0[mapa[ind].plane][1]->Fill(t0width*INVHPTIMEBIN, L);
	    TvsWt0p[mapa[ind].plane][1]->Fill(t0width*INVHPTIMEBIN, L);
	    tmean[1][ind] += L;
	    ntmean[1][ind]++;
	    TvsSm[mapa[ind].plane][1]->Fill(mapa[ind].strip, L);
//	    if (ntmean[1][ind] == 1) printf("%d %d %f %f %f %f %d %f\n",ind,ind1,lead[ind],lead[ind1],L, tmean[1][ind], ts_diff, t0width);
        }
	}
       }
    }

}

void BmnTof2Raw2DigitNew::SlewingT0()
{
  TProfile *prof = 0;
  Double_t par1 = 0.;
  Double_t par2 = 0.;
  Double_t par3 = 0.;
  TH1D *proj = 0;
  char namp[64];
  int ncon = 0;
  double mpos = 0.;
  TF1 *f_TW = 0;
  int nonzero = 0;
  FILE *fout = 0;
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  int na = 0, ip, is;
  for (int plane = 0; plane < MaxPlane; plane++)
  {
  //printf("\nwrite for chamber %d maxchambers %d\n", plane+1, MaxPlane);
  prof = TvsWt0[plane][0];
  if (!prof) goto peak2;
  char filn[128];
  sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, plane+1, 1);
  strcat(filn, ".slewing.t0.txt");
  fout = fopen(filn,"w");
  if (PRINT_SLEWING_RESULTS) printf("\n**************** %s Chamber %d Peak %d Time-Width area T0 slewing (write) ******************************\n\n", filname_base, plane+1, 1);
  fprintf(fout, "**************** %s Chamber %d Peak %d Time-Width area slewing *******************************\n\n", filname_base, plane+1, 1);

  nonzero = 0;
  for (int i=1; i<prof->GetNbinsX(); i++)
  {
    if (prof->GetBinContent(i)!=0)
    {
	nonzero++;
//	if (prof->GetBinEntries(i)<=4)
//	    prof->SetBinEntries(i,0);
    }
  }
  if (nonzero >= 3)
    {
      prof->Fit(SLFIT0,"WQ0");
      if(prof->GetFunction(SLFIT0)) (prof->GetFunction(SLFIT0))->ResetBit(TF1::kNotDraw);
    }
  else {printf(" Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);fprintf(fout, " Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);  fclose(fout); goto peak2; }

  f_TW = prof->GetFunction(SLFIT0);
  par1 = f_TW != 0 ? f_TW->GetParameter(0) : 0.;
  par2 = f_TW != 0 ? f_TW->GetParameter(1) : 0.;
  par3 = !strcmp(SLFIT0,"pol2") && f_TW != 0 ? f_TW->GetParameter(2) : 0.;

  if (prof == 0) { fclose(fout); goto peak2; }

  if (PRINT_SLEWING_RESULTS) printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
  fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
  if (PRINT_SLEWING_RESULTS) printf(" Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);
  fprintf(fout, " Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);

  na = 0;
  tmean_average[0][plane] = 0.;
  for (int ind=0; ind<n_rec; ind++)
  {
	if (mapa[ind].pair < 0) continue;
	ip = mapa[ind].plane;
	if (ip != plane) continue;
//	printf("** ip %d is %d nt %d t %f\n",ip,is,ntmean[0][ind],tmean[0][ind]);
	is = mapa[ind].strip;
	if (EQUAL_MAXIMA)
	{
	    sprintf(namp,"Maxima_chamber_%d_strip_%d",ip+1,is+1);
	    proj = TvsSm[ip][0]->ProjectionY(namp,is+1,is+1);
	    mpos = proj->GetBinCenter(proj->GetMaximumBin());
	    ncon = proj->GetBinContent(proj->GetMaximumBin());
//	    printf("Chamber %d strip %d max %d at %f\n", ip,is,ncon,mpos);
	    if (ncon > 100)
	    {
		tmean[0][ind] = mpos;
		tmean_average[0][plane] += mpos;
		na++;
	    }
	    else if (ntmean[0][ind])
	    {
		tmean[0][ind] /= (float)ntmean[0][ind];
		tmean_average[0][plane] += tmean[0][ind];
		na++;
	    }
	}
	else if (ntmean[0][ind])
	{
	    tmean[0][ind] /= (float)ntmean[0][ind];
	    tmean_average[0][plane] += tmean[0][ind];
	    na++;
	}
//	printf("!! ip %d is %d nt %d t %f\n",ip,is,ntmean[0][ind],tmean[0][ind]);
  }
  if (na) tmean_average[0][plane] /= (float)na;

  fprintf(fout,"Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[0][plane]);
  if (PRINT_SLEWING_RESULTS) printf("\nChamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[0][plane]);
  for (int ind=0; ind<n_rec; ind++)
  {
    if (mapa[ind].pair < 0) continue;
    ip = mapa[ind].plane;
    if (ip != plane) continue;
    is = mapa[ind].strip;
    if (ntmean[0][ind])
    {
	fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmean[0][ind]-tmean_average[0][plane]);
	if (PRINT_SLEWING_RESULTS) printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[0][ind]-tmean_average[0][plane]);
    }
    else
    {
	fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmean[0][ind]);
	if (PRINT_SLEWING_RESULTS) printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[0][ind]);
    }
  }

  fclose(fout);
peak2:
  prof = TvsWt0[plane][1];
  if (!prof) continue;
  sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, plane+1, 2);
  strcat(filn, ".slewing.t0.txt");
  fout = fopen(filn,"w");
  if (PRINT_SLEWING_RESULTS) printf("\n**************** %s Chamber %d Peak %d Time-Width area T0 slewing (write) ******************************\n\n", filname_base, plane+1, 2);
  fprintf(fout, "**************** %s Chamber %d Peak %d Time-Width area slewing *******************************\n\n", filname_base, plane+1, 2);

  nonzero = 0;
  for (int i=1; i<prof->GetNbinsX(); i++)
  {
    if (prof->GetBinContent(i)!=0)
    {
	nonzero++;
//	if (prof->GetBinEntries(i)<=4)
//	    prof->SetBinEntries(i,0);
    }
  }
  if (nonzero >= 3)
    {
      prof->Fit(SLFIT0,"WQ0");
      if(prof->GetFunction(SLFIT0)) (prof->GetFunction(SLFIT0))->ResetBit(TF1::kNotDraw);
    }
  else {printf(" Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);fprintf(fout, " Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);  fclose(fout); continue; }

  f_TW = prof->GetFunction(SLFIT0);
  par1 = f_TW != 0 ? f_TW->GetParameter(0) : 0.;
  par2 = f_TW != 0 ? f_TW->GetParameter(1) : 0.;
  par3 = !strcmp(SLFIT0,"pol2") && f_TW != 0 ? f_TW->GetParameter(2) : 0.;

  if (PRINT_SLEWING_RESULTS) printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
  fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
  if (PRINT_SLEWING_RESULTS) printf("Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);
  fprintf(fout, " Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);

  na = 0;
  tmean_average[1][plane] = 0.;
  for (int ind=0; ind<n_rec; ind++)
  {
	if (mapa[ind].pair < 0) continue;
	ip = mapa[ind].plane;
	if (ip != plane) continue;
	is = mapa[ind].strip;
	if (EQUAL_MAXIMA)
	{
	    sprintf(namp,"Maxima_chamber_%d_strip_%d",ip+1,is+1);
	    proj = TvsSm[ip][1]->ProjectionY(namp,is+1,is+1);
	    mpos = proj->GetBinCenter(proj->GetMaximumBin());
	    ncon = proj->GetBinContent(proj->GetMaximumBin());
//	    printf("Chamber %d strip %d max %d at %f\n", ip,is,ncon,mpos);
	    if (ncon > 100)
	    {
		tmean[1][ind] = mpos;
		tmean_average[1][plane] += mpos;
		na++;
	    }
	    else if (ntmean[1][ind])
	    {
		tmean[1][ind] /= (float)ntmean[1][ind];
		tmean_average[1][plane] += tmean[1][ind];
		na++;
	    }
	}
	else if (ntmean[1][ind])
	{
	    tmean[1][ind] /= (float)ntmean[1][ind];
	    tmean_average[1][plane] += tmean[1][ind];
	    na++;
	}
  }
  if (na) tmean_average[1][plane] /= (float)na;

  fprintf(fout,"Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[1][plane]);
  if (PRINT_SLEWING_RESULTS) printf("\nChamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[1][plane]);
  for (int ind=0; ind<n_rec; ind++)
  {
    if (mapa[ind].pair < 0) continue;
    ip = mapa[ind].plane;
    if (ip != plane) continue;
    is = mapa[ind].strip;
    if (ntmean[1][ind])
    {
	fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmean[1][ind]-tmean_average[1][plane]);
	if (PRINT_SLEWING_RESULTS) printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[1][ind]-tmean_average[1][plane]);
    }
    else
    {
	fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmean[1][ind]);
	if (PRINT_SLEWING_RESULTS) printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[1][ind]);
    }
  }

  fclose(fout);

  } // loop on chambers

  return;
}

void BmnTof2Raw2DigitNew::readSlewingLimits()
{
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  char filn[128];
  FILE *finl = 0;
  sprintf(filn, "%s%s_slewing_limits.txt", path.Data(), filname_base);
  finl = fopen(filn,"r");
  if (finl == NULL)
  {
    printf("No slewing limits file %s, use defaults\n", filn);
    return;
  };
  int j, lmi, lma;
  for (int i=0; i<MaxPlane; i++)
    {
      fscanf(finl,"\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", &j, &lmi, &lma);
      if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", j, lmi, lma);
      SetLeadMinMax(j,lmi,lma);
    }   
  fclose(finl);
  return;
}

void BmnTof2Raw2DigitNew::readSlewingT0()
{
  Int_t plane, dummy;
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  char filn[256], filn0[256], line[256], line1[256];
  //printf("%s %s\n", path.Data(), filname_base);
  int good = 0;
  for (int p = 0; p < MaxPlane; p++)
  {
  for (int pk = 0; pk < 2; pk++)
  {
  //printf("\nread for chamber %d maxchambers %d peak %d\n", p+1, MaxPlane, pk+1);
  if (chamber_slewing[p] > 0)
  {
   sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, chamber_slewing[p], pk+1);
   sprintf(filn0, "%s%s_chamber%d_peak%d", path.Data(), filname_base, p+1, pk+1);
   strcat(filn0, ".slewing.t0.txt");
  }
  else 
   sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, p+1, pk+1);
  strcat(filn, ".slewing.t0.txt");
//  printf("T0: Plane %d peak %d file %s\n", p+1, pk+1, filn);
  //printf("%s %s\n", filn, filn0);
  FILE *fin = fopen(filn,"r");
  FILE *fin0 = 0;
  if (chamber_slewing[p] > 0) fin0 = fopen(filn0,"r");
  if (fin == NULL)
  {
    printf(" T0 slewing file %s not found!\n", filn);
    printf(" Continue without T0 slewing!\n");
    continue;
  }
  if (fin0 == NULL && chamber_slewing[p] > 0)
  {
    printf(" T0 slewing file %s not found!\n", filn0);
    printf(" Continue without T0 slewing!\n");
    continue;
  }
  fgets(line, 255, fin);
  fgets(line1, 255, fin);
  if (chamber_slewing[p] > 0) fgets(line, 255, fin0);
  if (chamber_slewing[p] > 0) fgets(line1, 255, fin0);
  if (PRINT_SLEWING_PARAMETERS) printf("\n**************** %s Chamber %d Peak %d Time-Width area T0 slewing (read) ******************************\n\n", filname_base, p+1, pk+1);
  int ni = fscanf(fin, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", &plane, &wmint0[p][pk], &wmaxt0[p][pk], &tmint0[p][pk], &tmaxt0[p][pk]);
  if (ni != 5) continue;
// get limits from reference chamber
//  if (chamber_slewing[p] > 0) fgets(line, 255, fin0);
// get limits from the same chamber
  if (chamber_slewing[p] > 0) ni = fscanf(fin0, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", &plane, &wmint0[p][pk], &wmaxt0[p][pk], &tmint0[p][pk], &tmaxt0[p][pk]);
  if (ni != 5) continue;
  if (PRINT_SLEWING_PARAMETERS) printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane, wmint0[p][pk], wmaxt0[p][pk], tmint0[p][pk], tmaxt0[p][pk]);
  if (plane != (p+1) && chamber_slewing[p] == 0)
  {
	printf(" slewing file error, chamber numbers are mismatched, %d != %d\n", p+1, plane);
  }
  ni = fscanf(fin, "Time(Width) = %lf + %lf*Width + %lg*Width**2\n", &TvsWt0_const[p][pk], &TvsWt0_slope[p][pk], &TvsWt0_parab[p][pk]);
  if (chamber_slewing[p] > 0) fgets(line, 255, fin0);
  if (ni != 3) continue;
  if (PRINT_SLEWING_PARAMETERS) printf("Time(Width) = %f + %f*Width + %g*Width**2\n", TvsWt0_const[p][pk], TvsWt0_slope[p][pk], TvsWt0_parab[p][pk]);

  ni = fscanf(fin,"Chamber #%d channel offsets (average is %f)\n", &plane, &tmean_average[pk][p]);
  if (chamber_slewing[p] > 0) ni = fscanf(fin0,"Chamber #%d channel offsets (average is %f)\n", &plane, &tmean_average[pk][p]);
  if (ni != 2) continue;
  if (PRINT_SLEWING_PARAMETERS) printf("Chamber #%d channel offsets (average is %f)\n", plane, tmean_average[pk][p]);
  if (plane != (p+1) && chamber_slewing[p] == 0)
  {
	printf(" slewing file error, chamber numbers are mismatched, %d != %d\n", p+1, plane);
  }
  int ip, is, is1;
  for (int ind=0; ind<n_rec; ind++)
  {
    if (mapa[ind].pair < 0) continue;
    ip = mapa[ind].plane;
    if (ip != (plane-1)) continue;
    is = mapa[ind].strip;
    ni = fscanf(fin,"   strip %d time shift (left+right)/2 = %f\n", &is1, &tmean[pk][ind]);
    if (chamber_slewing[p] > 0) ni = fscanf(fin0,"   strip %d time shift (left+right)/2 = %f\n", &is1, &tmean[pk][ind]);
    if (ni != 2) continue;
    if (PRINT_SLEWING_PARAMETERS) printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[pk][ind]);
    if (is != is1 && chamber_slewing[p] == 0)
    {
	printf(" slewing file error, strip numbers are mismatched, %d != %d\n", is, is1);
	tmean[pk][ind] = 0.;
    }
  }
  good++;
  fclose(fin);
  if (chamber_slewing[p] > 0) fclose(fin0);
  } // loop on width peaks
  } // loop on chambers
  if (good == 0)
  {
    printf("No T0 slewing parameters files! Use TOF700 digits raw time!\n");
  }
}


void BmnTof2Raw2DigitNew::fillSlewing(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width) {
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
       int crate = mapa[ind].crate;
       int slot = mapa[ind].slot;
       int dnl = digit->GetValue() & 0x3FF;
       float tm =  (digit->GetValue()+DNL_Table[crate][slot][chan][dnl]) - chtima[crate][slot][chan] - (t0 - ts_diff)*INVHPTIMEBIN + T0shift;
       //if(digit->GetLeading()) lead[ind]=tm; else trail[ind]=tm; 
       if(digit->GetLeading()) {if (lead[ind] == 0) lead[ind]=tm;} else {if (trail[ind] == 0) trail[ind]=tm;} 
    }
    int Wc = Wcut;
    int Wm = Wmax;
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
//	printf("Plane %d Strip %d Lead %f Width %f LeadMin %d LeadMax %d\n", mapa[ind].plane, mapa[ind].strip, L, W, LeadMin[mapa[ind].plane], LeadMax[mapa[ind].plane]);
	if (L >= LeadMin[mapa[ind].plane] && L < LeadMax[mapa[ind].plane])
	if ((int)W1 < Wc && (int)W2 < Wc && ((tmean[0][ind] != 0.)||!EQUAL_AVERAGE_T0))
	{
//            if (mapa[ind].plane == 18) printf(" peak 1 l1 %f W %f\n",L,W);
    	    L -= slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 0);
//            if (mapa[ind].plane == 18) printf(" peak 1 l2 %f\n",L);
	    if (EQUAL_AVERAGE_T0) L -= tmean[0][ind];
//            if (mapa[ind].plane == 18) printf(" peak 1 l3 %f\n",L);
	    TvsW[mapa[ind].plane][0]->Fill(W, L);
	    TvsWp[mapa[ind].plane][0]->Fill(W, L);
//	    TvsSm[mapa[ind].plane][0]->Fill(mapa[ind].strip, L);
	}
	else if (W1 >= Wc && W2 >= Wc && W1 < Wm && W2 < Wm && ((tmean[1][ind] != 0.)||!EQUAL_AVERAGE_T0))
	{
            //if (mapa[ind].plane == 18) printf(" peak 2 l1 %f W %f\n",L,W);
    	    L -= slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 1);
            //if (mapa[ind].plane == 18) printf(" peak 2 l2 %f\n",L);
	    if (EQUAL_AVERAGE_T0) L -= tmean[1][ind];
            //if (mapa[ind].plane == 18) printf(" peak 2 l3 %f\n",L);
	    TvsW[mapa[ind].plane][1]->Fill(W, L);
	    TvsWp[mapa[ind].plane][1]->Fill(W, L);
//	    TvsSm[mapa[ind].plane][1]->Fill(mapa[ind].strip, L);
        }
//	else if ((W1 >= Wcut && W1 < Wmax && W2 < Wcut) || (W1 < Wcut && W2 >= Wmax && W2 < Wmax))
//	{
//            if (mapa[ind].plane == 0) printf(" peak 2 l1 %f W %f\n",L,W);
//    	    L -= 0.5*slewingt0_correction(mapa[ind].plane, T0width, 0);
//    	    L -= 0.5*slewingt0_correction(mapa[ind].plane, T0width, 1);
//            if (mapa[ind].plane == 0) printf(" peak 2 l2 %f\n",L);
//	    L -= 0.5*(tmean[0][ind]+tmean[1][ind]);
//            if (mapa[ind].plane == 0) printf(" peak 2 l3 %f\n",L);
//	    TvsW[mapa[ind].plane][1]->Fill(W, L);
//        }
       }
    }

}

void BmnTof2Raw2DigitNew::Slewing()
{
  TProfile *prof = 0;
  FILE *fout = 0;
  int nonzero = 0;
  TF1 *f_TW = 0;
  Double_t par1 = 0.;
  Double_t par2 = 0.;
  Double_t par3 = 0.;
  Double_t par4 = 0.;
  Double_t par5 = 0.;
  Double_t par6 = 0.;
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  for (int plane = 0; plane < MaxPlane; plane++)
  {
  prof = TvsW[plane][0];
  if (!prof) goto peak2;
  char filn[128];
  sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, plane+1, 1);
  strcat(filn, ".slewing.txt");
  fout = fopen(filn,"w");
  if (PRINT_SLEWING_RESULTS) printf("**************** %s Chamber %d Peak %d Time-Width area RPC slewing (write) ******************************\n\n", filname_base, plane+1, 1);
  fprintf(fout, "**************** %s Chamber %d Peak %d Time-Width area slewing *******************************\n\n", filname_base, plane+1, 1);

  nonzero = 0;
  for (int i=1; i<prof->GetNbinsX(); i++)
  {
    if (prof->GetBinContent(i)!=0)
    {
	nonzero++;
//	if (prof->GetBinEntries(i)<=4)
//	    prof->SetBinEntries(i,0);
    }
  }
  if (nonzero >= 6)
    {
      prof->Fit(SLFIT,"WQ0");
      if(prof->GetFunction(SLFIT)) (prof->GetFunction(SLFIT))->ResetBit(TF1::kNotDraw);
    }
  else {printf(" Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);fprintf(fout, " Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);  fclose(fout); goto peak2; }

  f_TW = prof->GetFunction(SLFIT);
  par1 = f_TW != 0 ? f_TW->GetParameter(0) : 0.;
  par2 = f_TW != 0 ? f_TW->GetParameter(1) : 0.;
  par3 = ((!strcmp(SLFIT,"pol2") || !strcmp(SLFIT,"pol3") || !strcmp(SLFIT,"pol4") || !strcmp(SLFIT,"pol5")) && f_TW != 0) ? f_TW->GetParameter(2) : 0.;
  par4 = ((!strcmp(SLFIT,"pol3") || !strcmp(SLFIT,"pol4") || !strcmp(SLFIT,"pol5")) && f_TW != 0) ? f_TW->GetParameter(3) : 0.;
  par5 = ((!strcmp(SLFIT,"pol4") || !strcmp(SLFIT,"pol5")) && f_TW != 0) ? f_TW->GetParameter(4) : 0.;
  par6 = ((!strcmp(SLFIT,"pol5")) && f_TW != 0) ? f_TW->GetParameter(5) : 0.;

  if (PRINT_SLEWING_RESULTS) printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
  fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
  if (PRINT_SLEWING_RESULTS) printf(" Time(Width) = %f + %f*Width + %g*Width**2 + %g*Width**3 + %g*Width**4 + %g*Width**5\n", par1, par2, par3, par4, par5, par6);
  fprintf(fout, " Time(Width) = %f + %f*Width + %g*Width**2 + %g*Width**3 + %g*Width**4 + %g*Width**5\n", par1, par2, par3, par4, par5, par6);

  fprintf(fout,"Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[0][plane]);
  if (PRINT_SLEWING_RESULTS) printf("Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[0][plane]);
  int ip, is;
  for (int ind=0; ind<n_rec; ind++)
  {
    if (mapa[ind].pair < 0) continue;
    ip = mapa[ind].plane;
    if (ip != plane) continue;
    is = mapa[ind].strip;
    fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmean[0][ind]);
    if (PRINT_SLEWING_RESULTS) printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[0][ind]);
  }

  fclose(fout);
peak2:
  prof = TvsW[plane][1];
  if (!prof) continue;
  sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, plane+1, 2);
  strcat(filn, ".slewing.txt");
  fout = fopen(filn,"w");
  if (PRINT_SLEWING_RESULTS) printf("**************** %s Chamber %d Peak %d Time-Width area RPC slewing (write) ******************************\n\n", filname_base, plane+1, 2);
  fprintf(fout, "**************** %s Chamber %d Peak %d Time-Width area slewing *******************************\n\n", filname_base, plane+1, 2);

  nonzero = 0;
  for (int i=1; i<prof->GetNbinsX(); i++)
  {
    if (prof->GetBinContent(i)!=0)
    {
	nonzero++;
//	if (prof->GetBinEntries(i)<=4)
//	    prof->SetBinEntries(i,0);
    }
  }
  if (nonzero >= 6)
    {
      prof->Fit(SLFIT,"WQ0");
      if(prof->GetFunction(SLFIT)) (prof->GetFunction(SLFIT))->ResetBit(TF1::kNotDraw);
    }
  else {printf(" Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);fprintf(fout, " Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);  fclose(fout); continue; }

  f_TW = prof->GetFunction(SLFIT);
  par1 = f_TW != 0 ? f_TW->GetParameter(0) : 0.;
  par2 = f_TW != 0 ? f_TW->GetParameter(1) : 0.;
  par3 = ((!strcmp(SLFIT,"pol2") || !strcmp(SLFIT,"pol3") || !strcmp(SLFIT,"pol4") || !strcmp(SLFIT,"pol5")) && f_TW != 0) ? f_TW->GetParameter(2) : 0.;
  par4 = ((!strcmp(SLFIT,"pol3") || !strcmp(SLFIT,"pol4") || !strcmp(SLFIT,"pol5")) && f_TW != 0) ? f_TW->GetParameter(3) : 0.;
  par5 = ((!strcmp(SLFIT,"pol4") || !strcmp(SLFIT,"pol5")) && f_TW != 0) ? f_TW->GetParameter(4) : 0.;
  par6 = ((!strcmp(SLFIT,"pol5")) && f_TW != 0) ? f_TW->GetParameter(5) : 0.;

  if (PRINT_SLEWING_RESULTS) printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
  fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
  if (PRINT_SLEWING_RESULTS) printf(" Time(Width) = %f + %f*Width + %g*Width**2 + %g*Width**3 + %g*Width**4 + %g*Width**5\n", par1, par2, par3, par4, par5, par6);
  fprintf(fout, " Time(Width) = %f + %f*Width + %g*Width**2 + %g*Width**3 + %g*Width**4 + %g*Width**5\n", par1, par2, par3, par4, par5, par6);

  fprintf(fout,"Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[1][plane]);
  if (PRINT_SLEWING_RESULTS) printf("Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[1][plane]);
  for (int ind=0; ind<n_rec; ind++)
  {
    if (mapa[ind].pair < 0) continue;
    ip = mapa[ind].plane;
    if (ip != plane) continue;
    is = mapa[ind].strip;
    fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmean[1][ind]);
    if (PRINT_SLEWING_RESULTS) printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[1][ind]);
  }

  fclose(fout);

  } // loop on chambers

  return;
}

void BmnTof2Raw2DigitNew::readSlewing()
{
  Int_t plane, dummy;
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  char filn[128], filn0[128], line[256], line1[256];
  int good = 0;
  for (int p = 0; p < MaxPlane; p++)
  {
  for (int pk = 0; pk < 2; pk++)
  {
  //sprintf(filn, "%s", filname_base);
  if (chamber_slewing[p] > 0)
  {
   sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, chamber_slewing[p], pk+1);
   sprintf(filn0, "%s%s_chamber%d_peak%d", path.Data(), filname_base, p+1, pk+1);
   strcat(filn0, ".slewing.txt");
  }
  else
   sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, p+1, pk+1);
  strcat(filn, ".slewing.txt");
//  printf("RPC: Plane %d peak %d file %s\n", p+1, pk+1, filn);
  FILE *fin = fopen(filn,"r");
  FILE *fin0 = 0;
  if (chamber_slewing[p] > 0) fin0 = fopen(filn0,"r");
  if (fin == NULL)
  {
    printf(" Slewing file %s not found!\n", filn);
    printf(" Continue without slewing!\n");
    continue;
  }
  if (fin0 == NULL && chamber_slewing[p] > 0)
  {
    printf(" Slewing file %s not found!\n", filn0);
    printf(" Continue without slewing!\n");
    continue;
  }
  fgets(line, 255, fin);
  if (chamber_slewing[p] > 0) fgets(line, 255, fin0);
  fgets(line1, 255, fin);
  if (chamber_slewing[p] > 0) fgets(line1, 255, fin0);
  if (PRINT_SLEWING_PARAMETERS) printf("**************** %s Chamber %d Peak %d Time-Width area RPC slewing (read) ******************************\n\n", filname_base, p+1, pk+1);
  int ni = fscanf(fin, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", &plane, &wmin[p][pk], &wmax[p][pk], &tmin[p][pk], &tmax[p][pk]);
  if (ni != 5) continue;
// get limits from reference chamber
//  if (chamber_slewing[p] > 0) fgets(line, 255, fin0);
// get limits from the same chamber
  if (chamber_slewing[p] > 0) ni = fscanf(fin0, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", &plane, &wmin[p][pk], &wmax[p][pk], &tmin[p][pk], &tmax[p][pk]);
  if (ni != 5) continue;
  if (PRINT_SLEWING_PARAMETERS) printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane, wmin[p][pk], wmax[p][pk], tmin[p][pk], tmax[p][pk]);
  if (plane != (p+1) && chamber_slewing[p] == 0)
  {
	printf(" slewing file error, chamber numbers are mismatched, %d != %d\n", p+1, plane);
  }
  ni = fscanf(fin, "Time(Width) = %lf + %lf*Width + %lg*Width**2 + %lg*Width**3 + %lg*Width**4 + %lg*Width**5\n", &TvsW_const[p][pk], &TvsW_slope[p][pk], &TvsW_parab[p][pk], &TvsW_cubic[p][pk], &TvsW_four[p][pk], &TvsW_five[p][pk]);
  if (chamber_slewing[p] > 0) fgets(line, 255, fin0);
  if (ni != 6) continue;
  if (PRINT_SLEWING_PARAMETERS) printf("Time(Width) = %f + %f*Width + %g*Width**2 + %g*Width**3 + %g*Width**4 + %g*Width**5\n", TvsW_const[p][pk], TvsW_slope[p][pk], TvsW_parab[p][pk], TvsW_cubic[p][pk], TvsW_four[p][pk], TvsW_five[p][pk]);

  ni = fscanf(fin,"Chamber #%d channel offsets (average is %f)\n", &plane, &tmeane_average[p]);
  if (chamber_slewing[p] > 0) ni = fscanf(fin0,"Chamber #%d channel offsets (average is %f)\n", &plane, &tmeane_average[p]);
  if (ni != 2) continue;
  if (PRINT_SLEWING_PARAMETERS) printf("Chamber #%d channel offsets (average is %f)\n", plane, tmeane_average[p]);
  if (plane != (p+1) && chamber_slewing[p] == 0)
  {
	printf(" slewing file error, chamber numbers are mismatched, %d != %d\n", p+1, plane);
  }
  if (pk == 1) { Wcutc[p] = wmin[p][pk]; Wmaxc[p] = wmax[p][pk]; };
  int ip, is, is1;
  for (int ind=0; ind<n_rec; ind++)
  {
    if (mapa[ind].pair < 0) continue;
    ip = mapa[ind].plane;
    if (ip != (plane-1)) continue;
    is = mapa[ind].strip;
    ni = fscanf(fin,"   strip %d time shift (left+right)/2 = %f\n", &is1, &tmeane[ind]);
    if (chamber_slewing[p] > 0) ni = fscanf(fin0,"   strip %d time shift (left+right)/2 = %f\n", &is1, &tmeane[ind]);
    if (ni != 2) continue;
    if (PRINT_SLEWING_PARAMETERS) printf("   strip %d time shift (left+right)/2 = %f\n", is, tmeane[ind]);
    if (is != is1 && chamber_slewing[p] == 0)
    {
	printf(" slewing file error, strip numbers are mismatched, %d != %d\n", is, is1);
	tmeane[ind] = 0.;
    }
  }
  good++;
  fclose(fin);
  if (chamber_slewing[p] > 0) fclose(fin0);
  } // loop on width peaks
  } // loop on chambers
  if (good == 0)
  {
    printf("No RPC slewing parameters files! Use TOF700 digits raw time!\n");
  }
}


void BmnTof2Raw2DigitNew::fillEvent(TClonesArray *data, map<UInt_t,Long64_t> *ts, Double_t t0, Double_t t0width, TClonesArray *tof2digit) {
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
//test!!!!
//	if (L < LeadMin[mapa[ind].plane] || L >= LeadMax[mapa[ind].plane]) continue;
//
	if ((int)W1 < Wc && (int)W2 < Wc && ((tmean[0][ind] != 0.)||!EQUAL_AVERAGE))
	{
    	    L -= slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 0);
	    TvsWt0[mapa[ind].plane][0]->Fill(t0width*INVHPTIMEBIN, L);
	    if (EQUAL_AVERAGE) L -= tmean[0][ind];
    	    L -= slewing_correction(mapa[ind].plane, W, 0);
	    TvsW[mapa[ind].plane][0]->Fill(W, L);
	}
	else if (W1 >= Wc && W2 >= Wc && ((tmean[1][ind] != 0.)||!EQUAL_AVERAGE))
	{
    	    L -= slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 1);
	    TvsWt0[mapa[ind].plane][1]->Fill(t0width*INVHPTIMEBIN, L);
	    if (EQUAL_AVERAGE) L -= tmean[1][ind];
    	    L -= slewing_correction(mapa[ind].plane, W, 1);
	    TvsW[mapa[ind].plane][1]->Fill(W, L);
        }
	else if (W1 >= Wc && W2 < Wc && (((tmean[0][ind] != 0.) && (tmean[1][ind] != 0.))||!EQUAL_AVERAGE))
	{
    	    L -= 0.5*slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 0);
    	    L -= 0.5*slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 1);
	    if (EQUAL_AVERAGE) L -= 0.5*(tmean[0][ind]+tmean[1][ind]);
    	    L -= 0.5*slewing_correction(mapa[ind].plane, W2, 0);
    	    L -= 0.5*slewing_correction(mapa[ind].plane, W1, 1);
	}
	else if (W1 < Wc && W2 > Wc && (((tmean[0][ind] != 0.) && (tmean[1][ind] != 0.))||!EQUAL_AVERAGE))
	{
    	    L -= 0.5*slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 0);
    	    L -= 0.5*slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 1);
	    if (EQUAL_AVERAGE) L -= 0.5*(tmean[0][ind]+tmean[1][ind]);
    	    L -= 0.5*slewing_correction(mapa[ind].plane, W1, 0);
    	    L -= 0.5*slewing_correction(mapa[ind].plane, W2, 1);
	}
	L -= tmeane[ind];
//        new(ar[tof2digit->GetEntriesFast()]) BmnTof2Digit(mapa[ind].plane,mapa[ind].strip,L*HPTIMEBIN,W*HPTIMEBIN,D*HPTIMEBIN);  
//        printf("%d %d %f %f %f t0 %f t0width %f\n", mapa[ind].plane,mapa[ind].strip,L*HPTIMEBIN,W,D*HPTIMEBIN, t0, t0width);  
        new((*tof2digit)[tof2digit->GetEntriesFast()]) BmnTof2Digit(mapa[ind].plane,mapa[ind].strip,L*HPTIMEBIN,W,D*HPTIMEBIN);  
       }
    }
}

void BmnTof2Raw2DigitNew::SlewingResults()
{
  TProfile *prof = 0;
  for (int it = 0; it < 2; it++)
  {
  for (int plane = 0; plane < MaxPlane; plane++)
  {
  prof = TvsW[plane][0];
  if (it == 0) prof = TvsWt0[plane][0];
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

  prof = TvsW[plane][1];
  if (it == 0) prof = TvsWt0[plane][1];
  if (!prof) continue;

  nonzero = 0;
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

  } // loop on chambers
  } // loop on T0 and RPC cases

  return;
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
//test!!!!
//
	if (L >= LeadMin[mapa[ind].plane] && L < LeadMax[mapa[ind].plane])
	{
	if ((int)W1 < Wc && (int)W2 < Wc && (tmean[0][ind] != 0.))
	{
    	    L -= slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 0);
	    if (EQUAL_AVERAGE) L -= tmean[0][ind];
    	    L -= slewing_correction(mapa[ind].plane, W, 0);
	}
	else if (W1 >= Wc && W2 >= Wc && (tmean[1][ind] != 0.))
	{
    	    L -= slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 1);
	    if (EQUAL_AVERAGE) L -= tmean[1][ind];
    	    L -= slewing_correction(mapa[ind].plane, W, 1);
        }
	else if (W1 >= Wc && W2 < Wc && (tmean[0][ind] != 0.) && (tmean[1][ind] != 0.))
	{
    	    L -= 0.5*slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 0);
    	    L -= 0.5*slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 1);
	    if (EQUAL_AVERAGE) L -= 0.5*(tmean[0][ind]+tmean[1][ind]);
    	    L -= 0.5*slewing_correction(mapa[ind].plane, W2, 0);
    	    L -= 0.5*slewing_correction(mapa[ind].plane, W1, 1);
	}
	else if (W1 < Wc && W2 > Wc && (tmean[0][ind] != 0.) && (tmean[1][ind] != 0.))
	{
    	    L -= 0.5*slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 0);
    	    L -= 0.5*slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 1);
	    if (EQUAL_AVERAGE) L -= 0.5*(tmean[0][ind]+tmean[1][ind]);
    	    L -= 0.5*slewing_correction(mapa[ind].plane, W1, 0);
    	    L -= 0.5*slewing_correction(mapa[ind].plane, W2, 1);
	}
	else
	{
	    continue;
	}
	tmeane[ind] += L;
	ntmeane[ind]++;
	TvsSm[mapa[ind].plane][0]->Fill(mapa[ind].strip, L);
	}
       }
    }
}
/*
void BmnTof2Raw2DigitNew::Equalization()
{
  TH1D *proj = 0;
  TF1 *gr = 0;
  char namp[64], namf[64];
  int ncon = 0;
  double mpos = 0.;
  FILE *fout = 0;
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  for (int plane = 0; plane < MaxPlane; plane++)
  {
  for (int pk = 0; pk < 2; pk++)
  {
  char filn[128], filn1[128], command[256];
  sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, plane+1, pk+1);
  strcpy(filn1,filn);
  strcat(filn, ".slewing.txt");
  strcat(filn1, ".slewing_saved.txt");
  sprintf(command, "cp -p %s %s", filn, filn1);
  system(command);
  fout = fopen(filn,"w");
  if (PRINT_EQUALIZATION_RESULTS) printf("**************** %s Chamber %d Peak %d Time-Width area RPC slewing (write) ******************************\n\n", filname_base, plane+1, pk+1);
  fprintf(fout, "**************** %s Chamber %d Peak %d Time-Width area slewing *******************************\n\n", filname_base, plane+1, pk+1);

  if (pk==0)
  {
    if (PRINT_EQUALIZATION_RESULTS) printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
    fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
  }
  else
  {
    if (PRINT_EQUALIZATION_RESULTS) printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
    fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
  }
  if (PRINT_EQUALIZATION_RESULTS) printf(" Time(Width) = %f + %f*Width + %g*Width**2 + %g*Width**3 + %g*Width**4 + %g*Width**5\n",  TvsW_const[plane][pk], TvsW_slope[plane][pk], TvsW_parab[plane][pk], TvsW_cubic[plane][pk], TvsW_four[plane][pk], TvsW_five[plane][pk]);
  fprintf(fout, " Time(Width) = %f + %f*Width + %g*Width**2 + %g*Width**3 + %g*Width**4 + %g*Width**5\n",  TvsW_const[plane][pk], TvsW_slope[plane][pk], TvsW_parab[plane][pk], TvsW_cubic[plane][pk], TvsW_four[plane][pk], TvsW_five[plane][pk]);

  int ip, is;
  int na = 0;
  if (pk==0)
  {
  tmeane_average[plane] = 0.;
  for (int ind=0; ind<n_rec; ind++)
  {
	if (mapa[ind].pair < 0) continue;
	ip = mapa[ind].plane;
	if (ip != plane) continue;
	is = mapa[ind].strip;
	//printf("** ip %d is %d nt %d t %f\n",ip,is,ntmeane[ind],tmeane[ind]);
	sprintf(namp,"Maxima_chamber_%d_strip_%d_equalization",ip+1,is+1);
	proj = TvsSm[ip][0]->ProjectionY(namp,is+1,is+1);
	if (EQUAL_MAXIMA)
	{
	    int mbin = proj->GetMaximumBin();
	    int nbin = proj->GetNbinsX();
	    mpos = proj->GetBinCenter(mbin);
	    ncon = proj->GetBinContent(mbin);
	    //printf("Chamber %d strip %d max %d at %f\n", ip,is,ncon,mpos);
	    if (ncon > 100)
	    {
		int nonzero = 0;
		int zflag   = 0;
		for (int ib=((mbin-5)>0? (mbin-5):1);ib<=((mbin+5)<nbin? (mbin+5):nbin);ib++)
		{
		    if (proj->GetBinContent(ib)>0.) nonzero++;
		    else { zflag = 1; break; }
		}
		if (FIT_MAXIMA && zflag == 0 && nonzero >= 5)
		{
		    sprintf(namf,"gaus%dstrip%d",ip+1,is+1);
		    //if (gr) delete gr;
		    gr = new TF1(namf,"gaus",mpos-5.,mpos+5.);
		    proj->Fit(namf,"QR");
		    tmeane[ind] = proj->GetFunction(namf)->GetParameter(1);
		}
		else tmeane[ind] = mpos;
		tmeane_average[plane] += tmeane[ind];
		na++;
	    }
	    else if (ntmeane[ind] > 0)
	    {
//		tmeane[ind] /= (float)ntmeane[ind];
		tmeane[ind] = proj->GetMean();
		tmeane_average[plane] += tmeane[ind];
		na++;
	    }
	    else tmeane[ind] = 0.;
	}
	else if (ntmeane[ind] > 0)
	{
//	    tmeane[ind] /= (float)ntmeane[ind];
	    tmeane[ind] = proj->GetMean();
	    tmeane_average[plane] += tmeane[ind];
	    na++;
	}
	else tmeane[ind] = 0.;
	//printf("!! ip %d is %d nt %d t %f na %d\n",ip,is,ntmeane[ind],tmeane[ind],na);
  }
  if (na) tmeane_average[plane] /= (float)na;
  }
  fprintf(fout,"Chamber #%d channel offsets (average is %f)\n", plane+1, tmeane_average[plane]);
  if (PRINT_EQUALIZATION_RESULTS) printf("Chamber #%d channel offsets (average is %f)\n", plane+1, tmeane_average[plane]);
  for (int ind=0; ind<n_rec; ind++)
  {
    if (mapa[ind].pair < 0) continue;
    ip = mapa[ind].plane;
    if (ip != plane) continue;
    is = mapa[ind].strip;
    if (TOZERO)
    {
	fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmeane[ind]);
	if (PRINT_EQUALIZATION_RESULTS) printf("   strip %d time shift (left+right)/2 = %f\n", is, tmeane[ind]);
    }
    else
    {
	fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmeane[ind]-tmeane_average[plane]);
	if (PRINT_EQUALIZATION_RESULTS) printf("   strip %d time shift (left+right)/2 = %f\n", is, tmeane[ind]-tmeane_average[plane]);
    }
  }

  fclose(fout);

  } // loop on peaks

  } // loop on chambers

  return;
}
*/

void BmnTof2Raw2DigitNew::Equalization()
{
  TH1D *proj = 0;
  TF1 *gr = 0;
  char namp[64], namf[64];
  int ncon = 0;
  double mpos = 0.;
  FILE *fout = 0;
  FILE *fdeb = 0;
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  if (DEBUG_EQUALIZATION) fdeb = fopen("FindMaxEqualization.txt","w");
  for (int plane = 0; plane < MaxPlane; plane++)
  {
  for (int pk = 0; pk < 2; pk++)
  {
  char filn[128], filn1[128], command[256];
  sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, plane+1, pk+1);
  strcpy(filn1,filn);
  strcat(filn, ".slewing.txt");
  strcat(filn1, ".slewing_saved.txt");
  sprintf(command, "cp -p %s %s", filn, filn1);
  system(command);
  fout = fopen(filn,"w");
  if (PRINT_EQUALIZATION_RESULTS) printf("**************** %s Chamber %d Peak %d Time-Width area RPC slewing (write) ******************************\n\n", filname_base, plane+1, pk+1);
  fprintf(fout, "**************** %s Chamber %d Peak %d Time-Width area slewing *******************************\n\n", filname_base, plane+1, pk+1);

  if (pk==0)
  {
    if (PRINT_EQUALIZATION_RESULTS) printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
    fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
  }
  else
  {
    if (PRINT_EQUALIZATION_RESULTS) printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
    fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
  }
  if (PRINT_EQUALIZATION_RESULTS) printf(" Time(Width) = %f + %f*Width + %g*Width**2 + %g*Width**3 + %g*Width**4 + %g*Width**5\n",  TvsW_const[plane][pk], TvsW_slope[plane][pk], TvsW_parab[plane][pk], TvsW_cubic[plane][pk], TvsW_four[plane][pk], TvsW_five[plane][pk]);
  fprintf(fout, " Time(Width) = %f + %f*Width + %g*Width**2 + %g*Width**3 + %g*Width**4 + %g*Width**5\n",  TvsW_const[plane][pk], TvsW_slope[plane][pk], TvsW_parab[plane][pk], TvsW_cubic[plane][pk], TvsW_four[plane][pk], TvsW_five[plane][pk]);

  int ip, is;
  int na = 0;
  if (pk==0)
  {
  tmeane_average[plane] = 0.;
  for (int ind=0; ind<n_rec; ind++)
  {
	if (mapa[ind].pair < 0) continue;
	ip = mapa[ind].plane;
	if (ip != plane) continue;
	is = mapa[ind].strip;
	//printf("** ip %d is %d nt %d t %f\n",ip,is,ntmeane[ind],tmeane[ind]);
	sprintf(namp,"Maxima_chamber_%d_strip_%d_equalization",ip+1,is+1);
	proj = TvsSm[ip][0]->ProjectionY(namp,is+1,is+1);
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
		for (int ib=((mbin-8)>0? (mbin-8):1);ib<((mbin+8)<nbin? (mbin+8):nbin);ib++)
		{
		    if (proj->GetBinContent(ib)>0.) nonzero++;
		    
		    else { zflag = 1; break; }
		}
		if (FIT_MAXIMA && zflag == 0 && nonzero >= 5)
		{
		    sprintf(namf,"gaus%dstrip%d",ip+1,is+1);
		    //if (gr) delete gr;
		    gr = new TF1(namf,"gaus",mpos-8.,mpos+8.);
		    proj->Fit(namf,"QR");
		    tmeane[ind] = proj->GetFunction(namf)->GetParameter(1);
		    if (ip == 18 && fdeb)
		    {
			fprintf(fdeb,"Fit: Chamber %d strip %d zflag %d nonzero %d mpos %f tmeane %f\n",
			ip,is,zflag,nonzero,mpos,tmeane[ind]);
		    }
		}
		else
		{
		    tmeane[ind] = mpos;
		    if (ip == 18 && fdeb)
		    {
			fprintf(fdeb,"Max: Chamber %d strip %d zflag %d nonzero %d mpos %f tmeane %f\n",
			ip,is,zflag,nonzero,mpos,tmeane[ind]);
		    }
		}
		tmeane_average[plane] += tmeane[ind];
		na++;
	    }
	    else if (ntmeane[ind] > 0)
	    {
//		tmeane[ind] /= (float)ntmeane[ind];
		tmeane[ind] = proj->GetMean();
		tmeane_average[plane] += tmeane[ind];
		na++;
		if (ip == 18 && fdeb)
		{
		    fprintf(fdeb,"Mean: Chamber %d strip %d mpos %f ntmeane %d tmeane %f\n",
		    ip,is,mpos,ntmeane[ind],tmeane[ind]);
		}
	    }
	    else
	    {
		tmeane[ind] = 0.;
		if (ip == 18 && fdeb)
		{
		    fprintf(fdeb,"Zero: Chamber %d strip %d tmeane %f\n",
		    ip,is,tmeane[ind]);
		}
	    }
	}
	else if (ntmeane[ind] > 0)
	{
//	    tmeane[ind] /= (float)ntmeane[ind];
	    tmeane[ind] = proj->GetMean();
	    tmeane_average[plane] += tmeane[ind];
	    na++;
	    if (ip == 18 && fdeb)
	    {
		fprintf(fdeb,"Mean: Chamber %d strip %d ntmean %d tmeane %f\n",
		ip,is,ntmeane[ind],tmeane[ind]);
	    }
	}
	else
	{
	    tmeane[ind] = 0.;
	    if (ip == 18 && fdeb)
	    {
		fprintf(fdeb,"Zero: Chamber %d strip %d tmeane %f\n",
		ip,is,tmeane[ind]);
	    }
	}
	//proj->Draw();
	//gPad->WaitPrimitive();
	//getchar();
	//printf("!! ip %d is %d nt %d t %f na %d\n",ip,is,ntmeane[ind],tmeane[ind],na);
  }
  if (na) tmeane_average[plane] /= (float)na;
  }
  fprintf(fout,"Chamber #%d channel offsets (average is %f)\n", plane+1, tmeane_average[plane]);
  if (PRINT_EQUALIZATION_RESULTS) printf("Chamber #%d channel offsets (average is %f)\n", plane+1, tmeane_average[plane]);
  for (int ind=0; ind<n_rec; ind++)
  {
    if (mapa[ind].pair < 0) continue;
    ip = mapa[ind].plane;
    if (ip != plane) continue;
    is = mapa[ind].strip;
    if (TOZERO)
    {
	fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmeane[ind]);
	if (PRINT_EQUALIZATION_RESULTS) printf("   strip %d time shift (left+right)/2 = %f\n", is, tmeane[ind]);
    }
    else
    {
	fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmeane[ind]-tmeane_average[plane]);
	if (PRINT_EQUALIZATION_RESULTS) printf("   strip %d time shift (left+right)/2 = %f\n", is, tmeane[ind]-tmeane_average[plane]);
    }
  }

  fclose(fout);

  } // loop on peaks

  } // loop on chambers

  if (fdeb) fclose(fdeb);

  return;
}

float BmnTof2Raw2DigitNew::slewingt0_correction(int chamber, double width, int peak)
{
    if (chamber < 0 || chamber >= MaxPlane || peak < 0 || peak > 1) return 0.;
    double cor = 0.;
    if (TvsWt0_const[chamber][peak] != 0.) cor = TvsWt0_const[chamber][peak] + TvsWt0_slope[chamber][peak]*width + TvsWt0_parab[chamber][peak]*width*width;
    else                                   cor = (LeadMax[chamber]+LeadMin[chamber])/2;
    return (float)cor;
}

float BmnTof2Raw2DigitNew::slewing_correction(int chamber, double width, int peak)
{
    if (chamber < 0 || chamber >= MaxPlane || peak < 0 || peak > 1) return 0.;
    double cor = 0.;
    if (TvsW_const[chamber][peak] != 0.) cor = TvsW_const[chamber][peak] + TvsW_slope[chamber][peak]*width + TvsW_parab[chamber][peak]*width*width + TvsW_cubic[chamber][peak]*width*width*width + \
					       TvsW_four[chamber][peak]*width*width*width*width + TvsW_five[chamber][peak]*width*width*width*width*width;
    else                                 cor = 0.;
    return (float)cor;
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

void BmnTof2Raw2DigitNew::writeSlewingLimits()
{
  FILE *fout = 0;
  int i, im, y;
  float ymin, ymax, xmin, xmax;
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  char filn[128];
  sprintf(filn, "%s%s_slewing_limits.txt", path.Data(), filname_base);
  fout = fopen(filn,"w");

  for (i=0; i<MaxPlane; i++)
    {
      im = (TvsS[i]->ProjectionY())->GetMaximumBin();
      y  = (int)((TvsS[i]->ProjectionY())->GetBinCenter(im));
      ymin = y - 60;
      ymax = y + 60;
      xmin = (TvsS[i]->GetXaxis())->GetXmin();
      xmax = (TvsS[i]->GetXaxis())->GetXmax();
      fprintf(fout,"\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", i+1, (int)ymin, (int)ymax);
      if (PRINT_TIME_LIMITS) printf("\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", i+1, (int)ymin, (int)ymax);
    }   
  fclose(fout);
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

float BmnTof2Raw2DigitNew::get_hit_x(int chamber, int strip, float diff)
{
    float x = 0., dx = 0.;
    if (chamber < MaxPlane && strip < TOF2_MAX_STRIPS_IN_CHAMBER && fVelosity > 0.)
    {
	dx = (-diff - lroffsets[chamber][strip]*HPTIMEBIN/2.f)*fVelosity;
	x = xcens[chamber][strip] + lrsign[chamber][strip]*dx;
	return x;
    }
    else
	return 0.;
}

float BmnTof2Raw2DigitNew::get_hit_diff(int chamber, int strip, float diff)
{
    if (chamber < MaxPlane && strip < TOF2_MAX_STRIPS_IN_CHAMBER)
    {
	return (-diff - lroffsets[chamber][strip]*HPTIMEBIN/2.f);
    }
    else
	return 0.;
}

void BmnTof2Raw2DigitNew::get_hit_xyz(int chamber, int strip, float diff, float *x, float *y, float *z)
{
    float xh = 0., dxh = 0.;
    if (chamber < MaxPlane && strip < TOF2_MAX_STRIPS_IN_CHAMBER && fVelosity > 0.)
    {
	dxh = (-diff - lroffsets[chamber][strip]*HPTIMEBIN/2.f)*fVelosity;
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

