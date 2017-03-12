#include "TF1.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TProfile2D.h"
#include "TH2D.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "BmnTof2Raw2DigitNew.h"

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

    fSlewCham = SlewingChamber;
    if (SlewingRun != 0)
    {
	sprintf((char *)&fname[strlen(filname_base) - 4], "%04d", SlewingRun);
    }


    ifstream in;
    in.open((path + mappingFile).Data());
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

    readGeom((char *)geomFile.Data());

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
	    TvsW[c][i] = NULL;
	    TvsWt0[c][i] = NULL;
	}

    Wcut = 2500;
    Wmax = 4000;
    WT0min = 720;
    WT0max = 860;

    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) LeadMin[i] = -5000;
    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) LeadMax[i] = +5000;
    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) Wcutc[i] = -1;
    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) Wmaxc[i] = -1;
/*
    LeadMin[0] = -400;
    LeadMin[1] = -300;
    LeadMin[2] = -400;
    LeadMin[3] = -120;

    LeadMax[0] = -250;
    LeadMax[1] = -150;
    LeadMax[2] = -50;
    LeadMax[3] = +120;
*/
/*
    if (RUN < 470)
    {
	SetWcut(1700);
	SetWmax(3700);
	SetWT0min(260);
	SetWT0max(560);
	SetLeadMinMax(1,-400, -250);
	SetLeadMinMax(2,-300, -150);
	SetLeadMinMax(3,-400, -50);
	SetLeadMinMax(4,-120, +120);
    }
    else
    {
	SetWcut(1700);
	SetWmax(3700);
	SetWT0min(640);
	SetWT0max(710);
	SetLeadMinMax(1,-350, -150);
	SetLeadMinMax(2,-350, -150);
	SetLeadMinMax(3,-350, +50);
	SetLeadMinMax(4,-200, +200);
//	SetLeadMinMax(1,-1350, +1350);
//	SetLeadMinMax(2,-1350, +1350);
//	SetLeadMinMax(3,-1350, +1350);
//	SetLeadMinMax(4,-1350, +1350);
    }
*/

    T0shift = 0.;

    for (int i = 0; i < TOF2_MAX_CHAMBERS; i++) numstrip[i] = -1;
//    numstrip[0] = 28;
//    numstrip[1] = 28;
//    numstrip[2] = 15;
//    numstrip[3] = 15;

}
void BmnTof2Raw2DigitNew::Book()
{
    char name[128], title[128];

    gStyle->SetOptFit(111);

    sprintf(name, "Width_T0");
    sprintf(title, "Width T0");
    Wt0 = new TH1F(name,title,1000,0,1000);

    sprintf(name, "TS_difference");
    sprintf(title, "TS difference");
    Wts = new TH1F(name,title,200,-100,100);

    for (int i=0; i<MaxPlane; i++)
    {
	sprintf(name, "Time_vs_Strip_Chamber_%.1f",idchambers[i]);
	sprintf(title, "Time vs Strip Chamber %.1f",idchambers[i]);
	TvsS[i] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,30000, -20000., +10000.);
	sprintf(name, "Width_vs_Strip_Chamber_%.1f",idchambers[i]);
	sprintf(title, "Width vs Strip Chamber %.1f",idchambers[i]);
	WvsS[i] = new TH2F(name,title,TOF2_MAX_STRIPS_IN_CHAMBER,0,TOF2_MAX_STRIPS_IN_CHAMBER,30000, -20000., +10000.);
    }
    for (int i=0; i<MaxPlane; i++)
    {
	sprintf(name, "Time_vs_Width_Chamber_%.1f_all",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f all",idchambers[i]);
	TvsWall[i] = new TH2F(name,title,Wmax,0,Wmax,LeadMax[i]-LeadMin[i],LeadMin[i],LeadMax[i]);
	sprintf(name, "Time_vs_Width_Chamber_%.1f_all_max",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f all, max strip",idchambers[i]);
	TvsWallmax[i] = new TH2F(name,title,Wmax,0,Wmax,LeadMax[i]-LeadMin[i],LeadMin[i],LeadMax[i]);
    }
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
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_1",idchambers[i]);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak 1",idchambers[i]);
	if (TvsWt0[i][0]) delete TvsWt0[i][0];
	TvsWt0[i][0] = new TProfile(name,title,(WT0max-WT0min),WT0min,WT0max,LeadMin[i],LeadMax[i],"e");
    }
    for (int i=0; i<MaxPlane; i++)
    {
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_2",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f Peak 2",idchambers[i]);
	if (TvsW[i][1]) delete TvsW[i][1];
	TvsW[i][1] = new TProfile(name,title,(Wmax-Wcut),Wcut,Wmax,-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2,"e");
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_2",idchambers[i]);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak 2",idchambers[i]);
	if (TvsWt0[i][1]) delete TvsWt0[i][1];
	TvsWt0[i][1] = new TProfile(name,title,(WT0max-WT0min),WT0min,WT0max,LeadMin[i],LeadMax[i],"e");
    }
}


void BmnTof2Raw2DigitNew::ReBook(int i)
{
    char name[128], title[128];
    int Wc = Wcut, Wm = Wmax;
    if (Wcutc[i] >= 0.) Wc = Wcutc[i];
    if (Wmaxc[i] >= 0.) Wm = Wmaxc[i];
    delete TvsW[i][0];
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_1",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f Peak 1",idchambers[i]);
	TvsW[i][0] = new TProfile(name,title,Wc,0,Wc,-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2,"e");
    delete TvsWt0[i][0];
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_1",idchambers[i]);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak 1",idchambers[i]);
	TvsWt0[i][0] = new TProfile(name,title,(WT0max-WT0min),WT0min,WT0max,LeadMin[i],LeadMax[i],"e");
    delete TvsWall[i];
	sprintf(name, "Time_vs_Width_Chamber_%.1f_all",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f all",idchambers[i]);
	TvsWall[i] = new TH2F(name,title,Wm,0,Wm,LeadMax[i]-LeadMin[i],LeadMin[i],LeadMax[i]);
    delete TvsWallmax[i];
	sprintf(name, "Time_vs_Width_Chamber_%.1f_all_max",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f all, max strip",idchambers[i]);
	TvsWallmax[i] = new TH2F(name,title,Wm,0,Wm,LeadMax[i]-LeadMin[i],LeadMin[i],LeadMax[i]);
    delete TvsW[i][1];
	sprintf(name, "Time_vs_Width_Chamber_%.1f_Peak_2",idchambers[i]);
	sprintf(title, "Time vs Width Chamber %.1f Peak 2",idchambers[i]);
	TvsW[i][1] = new TProfile(name,title,(Wm-Wc),Wc,Wm,-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2,"e");
    delete TvsWt0[i][1];
	sprintf(name, "Time_vs_T0_Width_Chamber_%.1f_Peak_2",idchambers[i]);
	sprintf(title, "Time vs T0 Width Chamber %.1f Peak 2",idchambers[i]);
	TvsWt0[i][1] = new TProfile(name,title,(WT0max-WT0min),WT0min,WT0max,LeadMin[i],LeadMax[i],"e");
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
       float tm =  (digit->GetValue()+DNL_Table[crate][slot][chan][dnl]) - (t0 - ts_diff)*INVHPTIMEBIN + T0shift;
       if(digit->GetLeading()) lead[ind]=tm; else trail[ind]=tm; 
//       printf("%d %d %d %d %d\n", i, mapa[ind].slot, mapa[ind].chan, digit->GetValue(), T0raw);
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
       float tm =  (digit->GetValue()+DNL_Table[crate][slot][chan][dnl]) - (t0 - ts_diff)*INVHPTIMEBIN + T0shift;
       if(digit->GetLeading()) lead[ind]=tm; else trail[ind]=tm; 
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
	    tmean[0][ind] += L;
	    ntmean[0][ind]++;
//	    if (ntmean[0][ind] == 1) printf("%d %d %f %f %f %f %d %f\n",ind,ind1,lead[ind],lead[ind1],L, tmean[0][ind], ts_diff, t0width);
	}
	else if ((int)W1 >= Wc && (int)W2 >= Wc && (int)W1 < Wm && (int)W2 < Wm)
	{
	    TvsWt0[mapa[ind].plane][1]->Fill(t0width*INVHPTIMEBIN, L);
	    tmean[1][ind] += L;
	    ntmean[1][ind]++;
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
  TF1 *f_TW = 0;
  int nonzero = 0;
  FILE *fout = 0;
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  int na = 0, ip, is;
  for (int plane = 0; plane < MaxPlane; plane++)
  {
  printf("\nwrite for chamber %d maxchambers %d\n", plane+1, MaxPlane);
  prof = TvsWt0[plane][0];
  if (!prof) goto peak2;
  char filn[128];
  sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, plane+1, 1);
  strcat(filn, ".slewing.t0.txt");
  fout = fopen(filn,"w");
  printf("\n**************** %s Chamber %d Peak %d Time-Width area T0 slewing (write) ******************************\n\n", filname_base, plane+1, 1);
  fprintf(fout, "**************** %s Chamber %d Peak %d Time-Width area slewing *******************************\n\n", filname_base, plane+1, 1);

  nonzero = 0;
  for (int i=1; i<prof->GetNbinsX(); i++)
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
      prof->Fit(SLFIT0,"WQ0");
      if(prof->GetFunction(SLFIT0)) (prof->GetFunction(SLFIT0))->ResetBit(TF1::kNotDraw);
    }
  else {printf(" Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);fprintf(fout, " Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);  fclose(fout); goto peak2; }

  f_TW = prof->GetFunction(SLFIT0);
  par1 = f_TW != 0 ? f_TW->GetParameter(0) : 0.;
  par2 = f_TW != 0 ? f_TW->GetParameter(1) : 0.;
  par3 = !strcmp(SLFIT0,"pol2") && f_TW != 0 ? f_TW->GetParameter(2) : 0.;

  if (prof == 0) { fclose(fout); goto peak2; }

  printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
  fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
  printf(" Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);
  fprintf(fout, " Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);

  na = 0;
  tmean_average[0][plane] = 0.;
  for (int ind=0; ind<n_rec; ind++)
  {
	if (mapa[ind].pair < 0) continue;
	ip = mapa[ind].plane;
	if (ip != plane) continue;
//	printf("** ip %d is %d nt %d t %f\n",ip,is,ntmean[0][ind],tmean[0][ind]);
	if (ntmean[0][ind])
	{
	    tmean[0][ind] /= (float)ntmean[0][ind];
	    tmean_average[0][plane] += tmean[0][ind];
	    na++;
	}
//	printf("!! ip %d is %d nt %d t %f\n",ip,is,ntmean[0][ind],tmean[0][ind]);
  }
  if (na) tmean_average[0][plane] /= (float)na;

  fprintf(fout,"Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[0][plane]);
  printf("\nChamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[0][plane]);
  for (int ind=0; ind<n_rec; ind++)
  {
    if (mapa[ind].pair < 0) continue;
    ip = mapa[ind].plane;
    if (ip != plane) continue;
    is = mapa[ind].strip;
    if (ntmean[0][ind])
    {
	fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmean[0][ind]-tmean_average[0][plane]);
	printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[0][ind]-tmean_average[0][plane]);
    }
    else
    {
	fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmean[0][ind]);
	printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[0][ind]);
    }
  }

  fclose(fout);
peak2:
  prof = TvsWt0[plane][1];
  if (!prof) continue;
  sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, plane+1, 2);
  strcat(filn, ".slewing.t0.txt");
  fout = fopen(filn,"w");
  printf("\n**************** %s Chamber %d Peak %d Time-Width area T0 slewing (write) ******************************\n\n", filname_base, plane+1, 2);
  fprintf(fout, "**************** %s Chamber %d Peak %d Time-Width area slewing *******************************\n\n", filname_base, plane+1, 2);

  nonzero = 0;
  for (int i=1; i<prof->GetNbinsX(); i++)
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
      prof->Fit(SLFIT0,"WQ0");
      if(prof->GetFunction(SLFIT0)) (prof->GetFunction(SLFIT0))->ResetBit(TF1::kNotDraw);
    }
  else {printf(" Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);fprintf(fout, " Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);  fclose(fout); continue; }

  f_TW = prof->GetFunction(SLFIT0);
  par1 = f_TW != 0 ? f_TW->GetParameter(0) : 0.;
  par2 = f_TW != 0 ? f_TW->GetParameter(1) : 0.;
  par3 = !strcmp(SLFIT0,"pol2") && f_TW != 0 ? f_TW->GetParameter(2) : 0.;

  printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
  fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
  printf("Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);
  fprintf(fout, " Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);

  na = 0;
  tmean_average[1][plane] = 0.;
  for (int ind=0; ind<n_rec; ind++)
  {
	if (mapa[ind].pair < 0) continue;
	ip = mapa[ind].plane;
	if (ip != plane) continue;
	if (ntmean[1][ind])
	{
	    tmean[1][ind] /= (float)ntmean[1][ind];
	    tmean_average[1][plane] += tmean[1][ind];
	    na++;
	}
  }
  if (na) tmean_average[1][plane] /= (float)na;

  fprintf(fout,"Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[1][plane]);
  printf("\nChamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[1][plane]);
  for (int ind=0; ind<n_rec; ind++)
  {
    if (mapa[ind].pair < 0) continue;
    ip = mapa[ind].plane;
    if (ip != plane) continue;
    is = mapa[ind].strip;
    if (ntmean[1][ind])
    {
	fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmean[1][ind]-tmean_average[1][plane]);
	printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[1][ind]-tmean_average[1][plane]);
    }
    else
    {
	fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmean[1][ind]);
	printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[1][ind]);
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
  for (int i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
      fscanf(finl,"\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", &j, &lmi, &lma);
      printf("\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", j, lmi, lma);
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
  char filn[128], line[256], line1[256];
  for (int p = 0; p < MaxPlane; p++)
  {
  for (int pk = 0; pk < 2; pk++)
  {
  //printf("\nread for chamber %d maxchambers %d peak %d\n", p+1, MaxPlane, pk+1);
  sprintf(filn, "%s", filname_base);
  if (fSlewCham > 0)
   sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, fSlewCham, pk+1);
  else 
   sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, p+1, pk+1);
  strcat(filn, ".slewing.t0.txt");
  FILE *fin = fopen(filn,"r");
  if (fin == NULL)
  {
    printf(" T0 slewing file %s not found!\n", filn);
    printf(" Continue without T0 slewing!\n");
    continue;
  }
  fgets(line, 255, fin);
  fgets(line1, 255, fin);
  printf("\n**************** %s Chamber %d Peak %d Time-Width area T0 slewing (read) ******************************\n\n", filname_base, p+1, pk+1);
  int ni = fscanf(fin, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", &plane, &wmint0[p][pk], &wmaxt0[p][pk], &tmint0[p][pk], &tmaxt0[p][pk]);
  if (ni != 5) continue;
  printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane, wmint0[p][pk], wmaxt0[p][pk], tmint0[p][pk], tmaxt0[p][pk]);
  if (plane != (p+1))
  {
	printf(" slewing file error, chamber numbers are mismatched, %d != %d\n", p+1, plane);
  }
  fscanf(fin, "Time(Width) = %f + %f*Width + %g*Width**2\n", &TvsWt0_const[p][pk], &TvsWt0_slope[p][pk], &TvsWt0_parab[p][pk]);
  printf("Time(Width) = %f + %f*Width + %g*Width**2\n", TvsWt0_const[p][pk], TvsWt0_slope[p][pk], TvsWt0_parab[p][pk]);

  fscanf(fin,"Chamber #%d channel offsets (average is %f)\n", &plane, &tmean_average[pk][p]);
  printf("Chamber #%d channel offsets (average is %f)\n", plane, tmean_average[pk][p]);
  if (plane != (p+1))
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
    fscanf(fin,"   strip %d time shift (left+right)/2 = %f\n", &is1, &tmean[pk][ind]);
    printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[pk][ind]);
    if (is != is1)
    {
	printf(" slewing file error, strip numbers are mismatched, %d != %d\n", is, is1);
    }
  }
  fclose(fin);
  } // loop on width peaks
  } // loop on chambers
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
       float tm =  (digit->GetValue()+DNL_Table[crate][slot][chan][dnl]) - (t0 - ts_diff)*INVHPTIMEBIN + T0shift;
       if(digit->GetLeading()) lead[ind]=tm; else trail[ind]=tm; 
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
	if (L >= LeadMin[mapa[ind].plane] && L < LeadMax[mapa[ind].plane])
	if ((int)W1 < Wc && (int)W2 < Wc)
	{
//            if (mapa[ind].plane == 0) printf(" peak 1 l1 %f W %f\n",L,W);
    	    L -= slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 0);
//            if (mapa[ind].plane == 0) printf(" peak 1 l2 %f\n",L);
	    L -= tmean[0][ind];
//            if (mapa[ind].plane == 0) printf(" peak 1 l3 %f\n",L);
	    TvsW[mapa[ind].plane][0]->Fill(W, L);
	}
	else if (W1 >= Wc && W2 >= Wc && W1 < Wm && W2 < Wm)
	{
//            if (mapa[ind].plane == 0) printf(" peak 2 l1 %f W %f\n",L,W);
    	    L -= slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 1);
//            if (mapa[ind].plane == 0) printf(" peak 2 l2 %f\n",L);
	    L -= tmean[1][ind];
//            if (mapa[ind].plane == 0) printf(" peak 2 l3 %f\n",L);
	    TvsW[mapa[ind].plane][1]->Fill(W, L);
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
  printf("**************** %s Chamber %d Peak %d Time-Width area RPC slewing (write) ******************************\n\n", filname_base, plane+1, 1);
  fprintf(fout, "**************** %s Chamber %d Peak %d Time-Width area slewing *******************************\n\n", filname_base, plane+1, 1);

  nonzero = 0;
  for (int i=1; i<prof->GetNbinsX(); i++)
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
      prof->Fit(SLFIT,"WQ0");
      if(prof->GetFunction(SLFIT)) (prof->GetFunction(SLFIT))->ResetBit(TF1::kNotDraw);
    }
  else {printf(" Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);fprintf(fout, " Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);  fclose(fout); goto peak2; }

  f_TW = prof->GetFunction(SLFIT);
  par1 = f_TW != 0 ? f_TW->GetParameter(0) : 0.;
  par2 = f_TW != 0 ? f_TW->GetParameter(1) : 0.;
  par3 = (!strcmp(SLFIT,"pol2") || !strcmp(SLFIT,"pol3")) && f_TW != 0 ? f_TW->GetParameter(2) : 0.;
  par4 = !strcmp(SLFIT,"pol3") && f_TW != 0 ? f_TW->GetParameter(3) : 0.;

  printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
  fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
  printf(" Time(Width) = %f + %f*Width + %g*Width**2 + %g*Width**3\n", par1, par2, par3, par4);
  fprintf(fout, " Time(Width) = %f + %f*Width + %g*Width**2 + %g*Width**3\n", par1, par2, par3, par4);

  fprintf(fout,"Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[0][plane]);
  printf("Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[0][plane]);
  int ip, is;
  for (int ind=0; ind<n_rec; ind++)
  {
    if (mapa[ind].pair < 0) continue;
    ip = mapa[ind].plane;
    if (ip != plane) continue;
    is = mapa[ind].strip;
    fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmean[0][ind]);
    printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[0][ind]);
  }

  fclose(fout);
peak2:
  prof = TvsW[plane][1];
  if (!prof) continue;
  sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, plane+1, 2);
  strcat(filn, ".slewing.txt");
  fout = fopen(filn,"w");
  printf("**************** %s Chamber %d Peak %d Time-Width area RPC slewing (write) ******************************\n\n", filname_base, plane+1, 2);
  fprintf(fout, "**************** %s Chamber %d Peak %d Time-Width area slewing *******************************\n\n", filname_base, plane+1, 2);

  nonzero = 0;
  for (int i=1; i<prof->GetNbinsX(); i++)
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
      prof->Fit(SLFIT,"WQ0");
      if(prof->GetFunction(SLFIT)) (prof->GetFunction(SLFIT))->ResetBit(TF1::kNotDraw);
    }
  else {printf(" Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);fprintf(fout, " Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);  fclose(fout); continue; }

  f_TW = prof->GetFunction(SLFIT);
  par1 = f_TW != 0 ? f_TW->GetParameter(0) : 0.;
  par2 = f_TW != 0 ? f_TW->GetParameter(1) : 0.;
  par3 = (!strcmp(SLFIT,"pol2") || !strcmp(SLFIT,"pol3")) && f_TW != 0 ? f_TW->GetParameter(2) : 0.;
  par4 = !strcmp(SLFIT,"pol3") && f_TW != 0 ? f_TW->GetParameter(3) : 0.;

  printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
  fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
  printf("Time(Width) = %f + %f*Width + %g*Width**2 + %g*Width**3\n", par1, par2, par3, par4);
  fprintf(fout, " Time(Width) = %f + %f*Width + %g*Width**2 + %g*Width**3\n", par1, par2, par3, par4);

  fprintf(fout,"Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[1][plane]);
  printf("Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[1][plane]);
  for (int ind=0; ind<n_rec; ind++)
  {
    if (mapa[ind].pair < 0) continue;
    ip = mapa[ind].plane;
    if (ip != plane) continue;
    is = mapa[ind].strip;
    fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmean[1][ind]);
    printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[1][ind]);
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
  char filn[128], line[256], line1[256];
  for (int p = 0; p < MaxPlane; p++)
  {
  for (int pk = 0; pk < 2; pk++)
  {
  sprintf(filn, "%s", filname_base);
  if (fSlewCham > 0)
   sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, fSlewCham, pk+1);
  else
   sprintf(filn, "%s%s_chamber%d_peak%d", path.Data(), filname_base, p+1, pk+1);
  strcat(filn, ".slewing.txt");
  FILE *fin = fopen(filn,"r");
  if (fin == NULL)
  {
    printf(" Slewing file %s not found!\n", filn);
    printf(" Continue without slewing!\n");
    continue;
  }
  fgets(line, 255, fin);
  fgets(line1, 255, fin);
  printf("**************** %s Chamber %d Peak %d Time-Width area RPC slewing (read) ******************************\n\n", filname_base, p+1, pk+1);
  int ni = fscanf(fin, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", &plane, &wmin[p][pk], &wmax[p][pk], &tmin[p][pk], &tmax[p][pk]);
  if (ni != 5) continue;
  printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane, wmin[p][pk], wmax[p][pk], tmin[p][pk], tmax[p][pk]);
  if (plane != (p+1))
  {
	printf(" slewing file error, chamber numbers are mismatched, %d != %d\n", p+1, plane);
  }
  fscanf(fin, "Time(Width) = %f + %f*Width + %g*Width**2 + %g*Width**3\n", &TvsW_const[p][pk], &TvsW_slope[p][pk], &TvsW_parab[p][pk], &TvsW_cubic[p][pk]);
  printf("Time(Width) = %f + %f*Width + %g*Width**2 + %g*Width**3\n", TvsW_const[p][pk], TvsW_slope[p][pk], TvsW_parab[p][pk], TvsW_cubic[p][pk]);

  fscanf(fin,"Chamber #%d channel offsets (average is %f)\n", &plane, &tmean_average[pk][p]);
  printf("Chamber #%d channel offsets (average is %f)\n", plane, tmean_average[pk][p]);
  if (plane != (p+1))
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
    fscanf(fin,"   strip %d time shift (left+right)/2 = %f\n", &is1, &tmean[pk][ind]);
    printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[pk][ind]);
    if (is != is1)
    {
	printf(" slewing file error, strip numbers are mismatched, %d != %d\n", is, is1);
    }
  }
  fclose(fin);
  } // loop on width peaks
  } // loop on chambers
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
       float tm =  (digit->GetValue()+DNL_Table[crate][slot][chan][dnl]) - (t0 - ts_diff)*INVHPTIMEBIN + T0shift;
//       printf("tm %f c %d s %d cor %f ts_diff %lld\n",tm, crate, slot, DNL_Table[crate][slot][chan][dnl], ts_diff);
       if(digit->GetLeading()) lead[ind]=tm; else trail[ind]=tm; 
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
	if ((int)W1 < Wc && (int)W2 < Wc)
	{
    	    L -= slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 0);
	    L -= tmean[0][ind];
    	    L -= slewing_correction(mapa[ind].plane, W, 0);
	    TvsW[mapa[ind].plane][0]->Fill(W, L);
	}
	else if (W1 >= Wc && W2 >= Wc)
	{
    	    L -= slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 1);
	    L -= tmean[1][ind];
    	    L -= slewing_correction(mapa[ind].plane, W, 1);
	    TvsW[mapa[ind].plane][1]->Fill(W, L);
        }
	else if (W1 >= Wc && W2 < Wc)
	{
    	    L -= 0.5*slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 0);
    	    L -= 0.5*slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 1);
	    L -= 0.5*(tmean[0][ind]+tmean[1][ind]);
    	    L -= 0.5*slewing_correction(mapa[ind].plane, W2, 0);
    	    L -= 0.5*slewing_correction(mapa[ind].plane, W1, 1);
	    TvsW[mapa[ind].plane][1]->Fill(W, L);
	}
	else if (W1 < Wc && W2 > Wc)
	{
    	    L -= 0.5*slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 0);
    	    L -= 0.5*slewingt0_correction(mapa[ind].plane, t0width*INVHPTIMEBIN, 1);
	    L -= 0.5*(tmean[0][ind]+tmean[1][ind]);
    	    L -= 0.5*slewing_correction(mapa[ind].plane, W1, 0);
    	    L -= 0.5*slewing_correction(mapa[ind].plane, W2, 1);
	    TvsW[mapa[ind].plane][1]->Fill(W, L);
	}
//        new(ar[tof2digit->GetEntriesFast()]) BmnTof2Digit(mapa[ind].plane,mapa[ind].strip,L*HPTIMEBIN,W*HPTIMEBIN,D*HPTIMEBIN);  
//        printf("%d %d %f %f %f t0 %f t0width %f\n", mapa[ind].plane,mapa[ind].strip,L*HPTIMEBIN,W,D*HPTIMEBIN, t0, t0width);  
        new((*tof2digit)[tof2digit->GetEntriesFast()]) BmnTof2Digit(mapa[ind].plane,mapa[ind].strip,L*HPTIMEBIN,W,D*HPTIMEBIN);  
       }
    }
}

void BmnTof2Raw2DigitNew::SlewingResults()
{
  TProfile *prof = 0;
  for (int plane = 0; plane < MaxPlane; plane++)
  {
  prof = TvsW[plane][0];
  if (!prof) return;

  int nonzero = 0;
  for (int i=1; i<prof->GetNbinsX(); i++)
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
      prof->Fit("pol0","WQ0");
      if(prof->GetFunction("pol0")) (prof->GetFunction("pol0"))->ResetBit(TF1::kNotDraw);
    }
  else {printf(" Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero); }

  prof = TvsW[plane][1];
  if (!prof) return;

  nonzero = 0;
  for (int i=1; i<prof->GetNbinsX(); i++)
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
      prof->Fit("pol0","WQ0");
      if(prof->GetFunction("pol0")) (prof->GetFunction("pol0"))->ResetBit(TF1::kNotDraw);
    }
  else {printf(" Chamber %d slewing results - too few nonzero bins = %d\n", plane+1, nonzero); }

  } // loop on chambers

  return;
}

float BmnTof2Raw2DigitNew::slewingt0_correction(int chamber, float width, int peak)
{
    if (chamber < 0 || chamber >= MaxPlane || peak < 0 || peak > 1) return 0.;
    float cor = 0.;
    if (TvsWt0_const[chamber][peak] != 0.) cor = TvsWt0_const[chamber][peak] + TvsWt0_slope[chamber][peak]*width + TvsWt0_parab[chamber][peak]*width*width;
    else                                   cor = (LeadMax[chamber]+LeadMin[chamber])/2;
    return cor;
}

float BmnTof2Raw2DigitNew::slewing_correction(int chamber, float width, int peak)
{
    if (chamber < 0 || chamber >= MaxPlane || peak < 0 || peak > 1) return 0.;
    float cor = 0.;
    if (TvsW_const[chamber][peak] != 0.) cor = TvsW_const[chamber][peak] + TvsW_slope[chamber][peak]*width + TvsW_parab[chamber][peak]*width*width + TvsW_cubic[chamber][peak]*width*width*width;
    else                                 cor = (LeadMax[chamber]+LeadMin[chamber])/2;
    return cor;
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
#define NDX 5
#define NDY 3
#else
#if TOF2_MAX_CHAMBERS == 24
int champosn[TOF2_MAX_CHAMBERS] = {17,18, 3, 1,19, 4,23,20, 5,15,21, 6, 2,22, 9,10,11,12,13,14, 7, 8, 0,16};
#define NDX 8
#define NDY 3
#else
int champosn[TOF2_MAX_CHAMBERS] = {0};
#define NDX 1
#define NDY 1
#endif
#endif

void BmnTof2Raw2DigitNew::drawprep()
{
  TCanvas *cp = new TCanvas("cp", "Leadings vs strip", 900,700);
  TLine *l = 0, *l1 = 0;
  FILE *fout = 0;
  int i, im, y;
  float ymin, ymax, xmin, xmax;
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  char filn[128];
  sprintf(filn, "%s%s_slewing_limits.txt", path.Data(), filname_base);
  fout = fopen(filn,"w");
  cp->cd();
  cp->Divide(NDX,NDY);
  for (i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
      cp->cd(champosn[i]+1);
      TvsS[i]->Draw();
      gPad->AddExec("exselt","select_hist()");
      im = (TvsS[i]->ProjectionY())->GetMaximumBin();
      y  = (int)((TvsS[i]->ProjectionY())->GetBinCenter(im));
      ymin = y - 50;
      ymax = y + 50;
      xmin = (TvsS[i]->GetXaxis())->GetXmin();
      xmax = (TvsS[i]->GetXaxis())->GetXmax();
      l = new TLine(xmin,ymin,xmax,ymin);
      l->Draw();
      l->SetLineColor(kRed);
      l = new TLine(xmin,ymax,xmax,ymax);
      l->Draw();
      l->SetLineColor(kRed);
      fprintf(fout,"\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", i+1, (int)ymin, (int)ymax);
      printf("\t\tTOF2.SetLeadMinMax(%d, %d,%d);\n", i+1, (int)ymin, (int)ymax);
    }   
  fclose(fout);

  TCanvas *cpw = new TCanvas("cpw", "Widths vs strip", 900,700);
  cpw->cd();
  cpw->Divide(NDX,NDY);
  for (i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
      cpw->cd(champosn[i]+1);
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
  for (i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
      cp1->cd(champosn[i]+1);
      TvsWall[i]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   

  TCanvas *cp2 = new TCanvas("cp2", "Leadings vs widths (max strip)", 900,700);
 
  cp2->cd();
  cp2->Divide(NDX,NDY);
  for (i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
      cp2->cd(champosn[i]+1);
      TvsWallmax[i]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   
  return;
}

void BmnTof2Raw2DigitNew::drawprof()
{
  TCanvas *callbe = new TCanvas("callbe", "Leadings vs widths (slewing RPC, peak 1)", 900,700);
 
  int i;
  callbe->cd();
  callbe->Divide(NDX,NDY);
  for (i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
      callbe->cd(champosn[i]+1);
      TvsW[i][0]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   

  TCanvas *callbe1 = new TCanvas("callbe1", "Leadings vs widths (slewing RPC, peak2)", 900,700);
  callbe1->cd();
  callbe1->Divide(NDX,NDY);
  for (i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
      callbe1->cd(champosn[i]+1);
      TvsW[i][1]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   

  return;
}

void BmnTof2Raw2DigitNew::drawproft0()
{
  TCanvas *callbe0 = new TCanvas("callbe0", "Leadings vs widths (slewing T0)", 900,700);
  int i;
  callbe0->cd();
  callbe0->Divide(NDX,NDY);
  for (i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
      callbe0->cd(champosn[i]+1);
      TvsWt0[i][0]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   

  TCanvas *callbe01 = new TCanvas("callbe01", "Leadings vs widths (slewing T0)", 900,700);
  callbe01->cd();
  callbe01->Divide(NDX,NDY);
  for (i=0; i<TOF2_MAX_CHAMBERS; i++)
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
		xcens[i][ns] = x/10. + xoffs;
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

