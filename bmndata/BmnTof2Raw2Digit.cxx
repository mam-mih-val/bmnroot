#include "TF1.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TProfile2D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "BmnTof2Raw2Digit.h"

BmnTof2Raw2Digit::BmnTof2Raw2Digit(){
    n_rec=0;
}

BmnTof2Raw2Digit::BmnTof2Raw2Digit(TString mappingFile, TString RunFile) {

    char *delim = 0, name[128], title[128];
    n_rec=0;
    TString dummy;
    ifstream in;

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/input/";
    strcpy(filname_base, gSystem->BaseName(RunFile.Data()));
    if ((delim = strrchr(filname_base, (int)'.'))) *delim = '\0';
    in.open((path + mappingFile).Data());
    in >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;
    MaxPlane = 0;
    while (!in.eof()) {
        TString side;
        int id,slot,tdc,chan,plane,strip; 
        in >> std::hex >> id >> std::dec >> slot >> tdc >> chan >> plane >> strip >> side;
        if (!in.good()) break;
	if (chan >= 32) break;
	if (plane > TOF2_MAX_CHAMBERS) break;
        map[n_rec].id=id;
        map[n_rec].slot=slot;
        map[n_rec].plane=plane-1;
        map[n_rec].tdc=tdc;
        map[n_rec].chan=chan;
        map[n_rec].strip=strip;
	map[n_rec].pair=-2;
	ntmean[0][n_rec] = 0;
	ntmean[1][n_rec] = 0;
	tmean[0][n_rec] = 0.;
	tmean[1][n_rec] = 0.;
        map[n_rec++].side=side.Data()[0]; 
	if (n_rec >= TOF2_MAX_CHANNEL) break;
	if (plane > MaxPlane) MaxPlane = plane;
    }
    in.close();

//    int numgeom[TOF2_MAX_CHAMBERS] = {1,2,3,4};
//    readGeom(numgeom);

    for(int ind=0;ind<n_rec;ind++){ 
     if (map[ind].pair == -1) continue; 
     for(int ind1=0;ind1<n_rec;ind1++){
	if (ind == ind1) continue; 
        if(map[ind1].plane == map[ind].plane && map[ind1].strip == map[ind].strip){
	    map[ind].pair = ind1; map[ind1].pair = -1;
	    break;
       }
     }
    }
//  read INL/DNL correction files
    for (int i=0; i<TOF2_MAX_MODULES; i++) for (int j=0; j<32; j++) for (int k=0; k<1024; k++) INL_Table25[i][j][k] = 0.;
    INL_read25();
    INL_read25_gol();
    INL_read25_ffd();

    Wcut = 1650;
    Wmax = 4000;
    WT0min = 260;
    WT0max = 560;
    LeadMin[0] = -400;
    LeadMin[1] = -300;
    LeadMin[2] = -400;
    LeadMin[3] = -120;

    LeadMax[0] = -250;
    LeadMax[1] = -150;
    LeadMax[2] = -50;
    LeadMax[3] = +120;
    T0shift = 0.;

//    numstrip[0] = 28;
//    numstrip[1] = 28;
//    numstrip[2] = 15;
//    numstrip[3] = 15;

    numstrip[0] = -1;
    numstrip[1] = -1;
    numstrip[2] = -1;
    numstrip[3] = -1;

    gStyle->SetOptFit(111);

    sprintf(name, "Width_T0");
    sprintf(title, "Width T0");
    Wt0 = new TH1F(name,title,1000,0,1000);

    sprintf(name, "TS_difference");
    sprintf(title, "TS difference");
    Wts = new TH1F(name,title,200,-100,100);

    for (int i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
	sprintf(name, "Time_vs_Strip_Chamber_%d",i+1);
	sprintf(title, "Time vs Strip Chamber %d",i+1);
	TvsS[i] = new TH2F(name,title,32,0,32,20000, -10000., +10000.);
	sprintf(name, "Width_vs_Strip_Chamber_%d",i+1);
	sprintf(title, "Width vs Strip Chamber %d",i+1);
	WvsS[i] = new TH2F(name,title,32,0,32,20000, -10000., +10000.);
    }
    for (int i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
	sprintf(name, "Time_vs_Width_Chamber_%d_Peak_1",i+1);
	sprintf(title, "Time vs Width Chamber %d Peak 1",i+1);
	TvsW[i][0] = new TProfile(name,title,Wcut,0,Wcut,-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2,"e");
	sprintf(name, "Time_vs_T0_Width_Chamber_%d_Peak_1",i+1);
	sprintf(title, "Time vs T0 Width Chamber %d Peak 1",i+1);
	TvsWt0[i][0] = new TProfile(name,title,(WT0max-WT0min),WT0min,WT0max,LeadMin[i],LeadMax[i],"e");
	sprintf(name, "Time_vs_T0_Width_Chamber_%d_all",i+1);
	sprintf(title, "Time vs T0 Width Chamber %d all",i+1);
	TvsWall[i] = new TH2F(name,title,Wmax,0,Wmax,LeadMax[i]-LeadMin[i],LeadMin[i],LeadMax[i]);
	sprintf(name, "Time_vs_T0_Width_Chamber_%d_all_max",i+1);
	sprintf(title, "Time vs T0 Width Chamber %d all, max strip",i+1);
	TvsWallmax[i] = new TH2F(name,title,Wmax,0,Wmax,LeadMax[i]-LeadMin[i],LeadMin[i],LeadMax[i]);
    }
    for (int i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
	sprintf(name, "Time_vs_Width_Chamber_%d_Peak_2",i+1);
	sprintf(title, "Time vs Width Chamber %d Peak 2",i+1);
	TvsW[i][1] = new TProfile(name,title,(Wmax-Wcut),Wcut,Wmax,-(LeadMax[i]-LeadMin[i])/2,+(LeadMax[i]-LeadMin[i])/2,"e");
	sprintf(name, "Time_vs_T0_Width_Chamber_%d_Peak_2",i+1);
	sprintf(title, "Time vs T0 Width Chamber %d Peak 2",i+1);
	TvsWt0[i][1] = new TProfile(name,title,(WT0max-WT0min),WT0min,WT0max,LeadMin[i],LeadMax[i],"e");
    }
}

void BmnTof2Raw2Digit::getEventInfo(long long *ev,long long *t1,long long *t2){
    *ev=EVENT;
    *t1=TIME_SEC;
    *t2=TIME_NS;
}

void BmnTof2Raw2Digit::print(){
     printf("Number of chambers %d\n", MaxPlane);
     printf("#\tcrate\t\tslot\ttdc\tchannel\tplane\tstrip\tpair\tside\n=================================================\n");
     for(int i=0;i<n_rec;i++){
       printf("%3d\t0x%X\t%d\t%d\t%d\t%d\t%d\t%d\t%c\n",i,map[i].id,map[i].slot,map[i].tdc,map[i].chan,map[i].plane,map[i].strip,map[i].pair,map[i].side);
     }   
}

void BmnTof2Raw2Digit::fillPreparation(TClonesArray *data, TClonesArray *sync, TClonesArray *t0) {
    unsigned int t0id = 0;
    long long t0time = 0, digittime = 0;
    int T0raw = 0, T0width = 0, T0trail = 0, chan = 0;
    int n_t0_l = 0, n_t0_t = 0;
  
    for (int i = 0; i < t0->GetEntriesFast(); i++) {
        BmnTDCDigit *digit = (BmnTDCDigit*) t0->At(i);
        int dnl = digit->GetValue() & 0x3FF;
        if (digit->GetLeading()) {
            chan = digit->GetChannel();
            T0raw = digit->GetValue()+INL_Table25[4][chan][dnl]+0.5;
            t0id = digit->GetSerial();
//	    printf("T0 slot %d channel %d Leading %d\n", digit->GetSlot(), digit->GetChannel(), T0raw);
	    n_t0_l++;
        }
	else
	{
            T0trail = digit->GetValue();
	    n_t0_t++;
	}
    }
    if (n_t0_l != n_t0_t) return;
    if (n_t0_l != 1) return;
    if (T0raw == 0 || T0trail == 0) return;
    T0width = (T0trail - T0raw);
    Wt0->Fill(T0width);
    float lead[TOF2_MAX_CHANNEL];
    float trail[TOF2_MAX_CHANNEL];
    memset(lead,0,TOF2_MAX_CHANNEL*sizeof(float));
    memset(trail,0,TOF2_MAX_CHANNEL*sizeof(int));
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnTDCDigit *digit = (BmnTDCDigit*) data->At(i);
       for (int j = 0; j < sync->GetEntriesFast(); j++) {
            BmnSyncDigit *rec = (BmnSyncDigit*) sync->At(j);
            if (rec->GetSerial() == t0id){
               EVENT    = rec->GetEvent();
               TIME_SEC = rec->GetTime_sec();
               TIME_NS  = rec->GetTime_ns();
               t0time   = TIME_SEC*1000000000LL+TIME_NS;
            }
            if (rec->GetSerial() == digit->GetSerial()) digittime = rec->GetTime_sec()*1000000000LL+rec->GetTime_ns();
       }
       int ts_diff = (int)((digittime - t0time)*INVHPTIMEBIN);
       int ind; 
       for(ind=0;ind<n_rec;ind++) 
         if(digit->GetSerial()==map[ind].id && digit->GetSlot()==map[ind].slot && digit->GetChannel()==map[ind].chan) break;
       if(ind==n_rec) continue; 
//       int tdcnum = map[ind].tdc;
       Wts->Fill(digittime - t0time);
       int slot = map[ind].slot;
       int dnl = digit->GetValue() & 0x3FF;
       chan = digit->GetChannel();
       float tm =  (digit->GetValue()+INL_Table25[slot-3][chan][dnl]) - (T0raw - ts_diff + T0shift);
       if(digit->GetLeading()) lead[ind]=tm; else trail[ind]=tm; 
//       printf("%d %d %d %d %d\n", i, map[ind].slot, map[ind].chan, digit->GetValue(), T0raw);
    }

    float wmax[TOF2_MAX_CHAMBERS] = {0.}, tmax[TOF2_MAX_CHAMBERS] = {-1000.};
    int smax[TOF2_MAX_CHAMBERS] = {-1};
    for(int ind=0;ind<n_rec;ind++){
       int ind1 = map[ind].pair;
       if (ind1 < 0) continue;
       if(lead[ind1]==0 || trail[ind1]==0) continue;
       if(lead[ind]!=0 && trail[ind]!=0){
	float L = (lead[ind]+lead[ind1])/2.;
	float W1 = trail[ind]-lead[ind];
	float W2 = trail[ind1]-lead[ind1];
	float W = (W1+W2)/2.;
	if (((W1 < Wcut && W2 < Wcut)||(W1 >= Wcut && W2 >= Wcut)))
	{
	    TvsS[map[ind].plane]->Fill(map[ind].strip, L);
	    WvsS[map[ind].plane]->Fill(map[ind].strip, W);
	    TvsWall[map[ind].plane]->Fill(W, L);
	}
	if (W > wmax[map[ind].plane] && ((W1 < Wcut && W2 < Wcut)||(W1 >= Wcut && W2 >= Wcut)))
	{
		wmax[map[ind].plane] = W;
		tmax[map[ind].plane] = L;
		smax[map[ind].plane] = map[ind].strip;
	}
       }
    }
    for (int i=0; i<TOF2_MAX_CHAMBERS; i++)
    {
	    if (smax[i] > -1)
	    {
		TvsWallmax[i]->Fill(wmax[i], tmax[i]);
	    }
    }

}

void BmnTof2Raw2Digit::fillSlewingT0(TClonesArray *data, TClonesArray *sync, TClonesArray *t0) {
    unsigned int t0id = 0;
    long long t0time = 0, digittime = 0;
    int T0raw = 0, T0width = 0, T0trail = 0, chan = 0;
    int n_t0_l = 0, n_t0_t = 0;
  
    for (int i = 0; i < t0->GetEntriesFast(); i++) {
        BmnTDCDigit *digit = (BmnTDCDigit*) t0->At(i);
        int dnl = digit->GetValue() & 0x3FF;
        if (digit->GetLeading()) {
            chan = digit->GetChannel();
            T0raw = digit->GetValue()+INL_Table25[4][chan][dnl]+0.5;
            t0id = digit->GetSerial();
//	    printf("T0 slot %d channel %d Leading %d\n", digit->GetSlot(), digit->GetChannel(), T0raw);
	    n_t0_l++;
        }
	else
	{
            T0trail = digit->GetValue();
	    n_t0_t++;
	}
    }
    if (n_t0_l != n_t0_t) return;
    if (n_t0_l != 1) return;
    if (T0raw == 0 || T0trail == 0) return;
    T0width = (T0trail - T0raw);
    float lead[TOF2_MAX_CHANNEL];
    float trail[TOF2_MAX_CHANNEL];
    memset(lead,0,TOF2_MAX_CHANNEL*sizeof(float));
    memset(trail,0,TOF2_MAX_CHANNEL*sizeof(float));
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnTDCDigit *digit = (BmnTDCDigit*) data->At(i);
       for (int j = 0; j < sync->GetEntriesFast(); j++) {
            BmnSyncDigit *rec = (BmnSyncDigit*) sync->At(j);
            if (rec->GetSerial() == t0id){
               EVENT    = rec->GetEvent();
               TIME_SEC = rec->GetTime_sec();
               TIME_NS  = rec->GetTime_ns();
               t0time   = TIME_SEC*1000000000LL+TIME_NS;
            }
            if (rec->GetSerial() == digit->GetSerial()) digittime = rec->GetTime_sec()*1000000000LL+rec->GetTime_ns();
       }
       if (t0time == 0LL || digittime == 0LL) continue;
       int ts_diff = (int)((digittime - t0time)*INVHPTIMEBIN);
//       if (ts_diff) continue;
       int ind; 
       for(ind=0;ind<n_rec;ind++) 
         if(digit->GetSerial()==map[ind].id && digit->GetSlot()==map[ind].slot && digit->GetChannel()==map[ind].chan) break;
       if(ind==n_rec) continue; 
//       int tdcnum = map[ind].tdc;
       int slot = map[ind].slot;
       int dnl = digit->GetValue() & 0x3FF;
       chan = digit->GetChannel();
       float tm =  (digit->GetValue()+INL_Table25[slot-3][chan][dnl]) - (T0raw - ts_diff + T0shift);
       if(digit->GetLeading()) lead[ind]=tm; else trail[ind]=tm; 
//       if (tm > 100000.) printf("%d %d %d %d %f %d %d %f\n", i, map[ind].slot, map[ind].chan, digit->GetValue(),INL_Table25[slot-3][chan][dnl], T0raw, ts_diff, tm);
    }
    for(int ind=0;ind<n_rec;ind++){
       int ind1 = map[ind].pair;
       if (ind1 < 0) continue;
       if (lead[ind1]==0 || trail[ind1]==0) continue;
       if (numstrip[map[ind].plane] >=0 && numstrip[map[ind].plane] != map[ind].strip) continue;
       if(lead[ind]!=0 && trail[ind]!=0){
	float L = (lead[ind]+lead[ind1])/2.;
	float W1 = trail[ind]-lead[ind];
	float W2 = trail[ind1]-lead[ind1];
	float W = (W1+W2)/2.;
	if (L >= LeadMin[map[ind].plane] && L < LeadMax[map[ind].plane])
	{
	if ((int)W1 < Wcut && (int)W2 < Wcut)
	{
	    TvsWt0[map[ind].plane][0]->Fill(T0width, L);
	    tmean[0][ind] += L;
	    ntmean[0][ind]++;
//	    if (ntmean[0][ind] == 1) printf("%d %d %f %f %f %f %d\n",ind,ind1,lead[ind],lead[ind1],L, tmean[0][ind], ts_diff);
	}
	else if ((int)W1 >= Wcut && (int)W2 >= Wcut && (int)W1 < Wmax && (int)W2 < Wmax)
	{
	    TvsWt0[map[ind].plane][1]->Fill(T0width, L);
	    tmean[1][ind] += L;
	    ntmean[1][ind]++;
//	    if (ntmean[1][ind] == 1) printf("%d %d %f %f %f %f %d\n",ind,ind1,lead[ind],lead[ind1],L, tmean[1][ind], ts_diff);
        }
	}
       }
    }

}

void BmnTof2Raw2Digit::SlewingT0()
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
      prof->Fit(SLFIT,"WQ0");
      if(prof->GetFunction(SLFIT)) (prof->GetFunction(SLFIT))->ResetBit(TF1::kNotDraw);
    }
  else {printf(" Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);fprintf(fout, " Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);  fclose(fout); goto peak2; }

  f_TW = prof->GetFunction(SLFIT);
  par1 = f_TW != 0 ? f_TW->GetParameter(0) : 0.;
  par2 = f_TW != 0 ? f_TW->GetParameter(1) : 0.;
  par3 = !strcmp(SLFIT,"pol2") && f_TW != 0 ? f_TW->GetParameter(2) : 0.;

  if (prof == 0) { fclose(fout); goto peak2; }

  printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
  fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
  printf(" Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);
  fprintf(fout, " Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);

  na = 0;
  tmean_average[0][plane] = 0.;
  for (int ind=0; ind<n_rec; ind++)
  {
	if (map[ind].pair < 0) continue;
	ip = map[ind].plane;
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
    if (map[ind].pair < 0) continue;
    ip = map[ind].plane;
    if (ip != plane) continue;
    is = map[ind].strip;
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
      prof->Fit(SLFIT,"WQ0");
      if(prof->GetFunction(SLFIT)) (prof->GetFunction(SLFIT))->ResetBit(TF1::kNotDraw);
    }
  else {printf(" Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);fprintf(fout, " Chamber %d slewing - too few nonzero bins = %d\n", plane+1, nonzero);  fclose(fout); continue; }

  f_TW = prof->GetFunction(SLFIT);
  par1 = f_TW != 0 ? f_TW->GetParameter(0) : 0.;
  par2 = f_TW != 0 ? f_TW->GetParameter(1) : 0.;
  par3 = !strcmp(SLFIT,"pol2") && f_TW != 0 ? f_TW->GetParameter(2) : 0.;

  printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
  fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
  printf("Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);
  fprintf(fout, " Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);

  na = 0;
  tmean_average[1][plane] = 0.;
  for (int ind=0; ind<n_rec; ind++)
  {
	if (map[ind].pair < 0) continue;
	ip = map[ind].plane;
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
    if (map[ind].pair < 0) continue;
    ip = map[ind].plane;
    if (ip != plane) continue;
    is = map[ind].strip;
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

void BmnTof2Raw2Digit::readSlewingT0()
{
  Int_t plane, dummy;
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/tof2_slewing/";
  char filn[128], line[256], line1[256];
  for (int p = 0; p < MaxPlane; p++)
  {
  for (int pk = 0; pk < 2; pk++)
  {
  printf("\nread for chamber %d maxchambers %d peak %d\n", p+1, MaxPlane, pk+1);
  sprintf(filn, "%s", filname_base);
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
  fscanf(fin, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", &plane, &wmint0[p][pk], &wmaxt0[p][pk], &tmint0[p][pk], &tmaxt0[p][pk]);
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
    if (map[ind].pair < 0) continue;
    ip = map[ind].plane;
    if (ip != (plane-1)) continue;
    is = map[ind].strip;
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


void BmnTof2Raw2Digit::fillSlewing(TClonesArray *data, TClonesArray *sync, TClonesArray *t0) {

    unsigned int t0id = 0;
    long long t0time = 0, digittime = 0;
    int T0raw = 0, T0width = 0, T0trail = 0, chan = 0;
    int n_t0_l = 0, n_t0_t = 0;
  
    for (int i = 0; i < t0->GetEntriesFast(); i++) {
        BmnTDCDigit *digit = (BmnTDCDigit*) t0->At(i);
        int dnl = digit->GetValue() & 0x3FF;
        if (digit->GetLeading()) {
            chan = digit->GetChannel();
            T0raw = digit->GetValue()+INL_Table25[4][chan][dnl]+0.5;
            t0id = digit->GetSerial();
	    n_t0_l++;
        }
	else
	{
            T0trail = digit->GetValue();
	    n_t0_t++;
	}
    }
    if (n_t0_l != n_t0_t) return;
    if (n_t0_l != 1) return;
    if (T0raw == 0 || T0trail == 0) return;
    T0width = (T0trail - T0raw);
    float lead[TOF2_MAX_CHANNEL];
    float trail[TOF2_MAX_CHANNEL];
    memset(lead,0,TOF2_MAX_CHANNEL*sizeof(float));
    memset(trail,0,TOF2_MAX_CHANNEL*sizeof(int));
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnTDCDigit *digit = (BmnTDCDigit*) data->At(i);
       for (int j = 0; j < sync->GetEntriesFast(); j++) {
            BmnSyncDigit *rec = (BmnSyncDigit*) sync->At(j);
            if (rec->GetSerial() == t0id){
               EVENT    = rec->GetEvent();
               TIME_SEC = rec->GetTime_sec();
               TIME_NS  = rec->GetTime_ns();
               t0time   = TIME_SEC*1000000000LL+TIME_NS;
            }
            if (rec->GetSerial() == digit->GetSerial()) digittime = rec->GetTime_sec()*1000000000LL+rec->GetTime_ns();
       }
       if (t0time == 0LL || digittime == 0LL) continue;
       int ts_diff = (int)((digittime - t0time)*INVHPTIMEBIN);
       int ind; 
       for(ind=0;ind<n_rec;ind++) 
         if(digit->GetSerial()==map[ind].id && digit->GetSlot()==map[ind].slot && digit->GetChannel()==map[ind].chan) break;
       if(ind==n_rec) continue; 
//       int tdcnum = map[ind].tdc;
       int slot = map[ind].slot;
       int dnl = digit->GetValue() & 0x3FF;
       chan = digit->GetChannel();
       float tm =  (digit->GetValue()+INL_Table25[slot-3][chan][dnl]) - (T0raw - ts_diff + T0shift);
       if(digit->GetLeading()) lead[ind]=tm; else trail[ind]=tm; 
    }
    for(int ind=0;ind<n_rec;ind++){
       int ind1 = map[ind].pair;
       if (ind1 < 0) continue;
       if(lead[ind1]==0 || trail[ind1]==0) continue;
       if (numstrip[map[ind].plane] >=0 && numstrip[map[ind].plane] != map[ind].strip) continue;
       if(lead[ind]!=0 && trail[ind]!=0){
//        if (map[ind].plane == 0) printf("%f %f %f %f\n",lead[ind1],lead[ind],trail[ind1],trail[ind]);
	float L = (lead[ind1]+lead[ind])/2.;
	float W1 = trail[ind]-lead[ind];
	float W2 = trail[ind1]-lead[ind1];
	float W = (W1+W2)/2.;
	if (L >= LeadMin[map[ind].plane] && L < LeadMax[map[ind].plane])
	if ((int)W1 < Wcut && (int)W2 < Wcut)
	{
//            if (map[ind].plane == 0) printf(" peak 1 l1 %f W %f\n",L,W);
    	    L -= slewingt0_correction(map[ind].plane, T0width, 0);
//            if (map[ind].plane == 0) printf(" peak 1 l2 %f\n",L);
	    L -= tmean[0][ind];
//            if (map[ind].plane == 0) printf(" peak 1 l3 %f\n",L);
	    TvsW[map[ind].plane][0]->Fill(W, L);
	}
	else if (W1 >= Wcut && W2 >= Wcut && W1 < Wmax && W2 < Wmax)
	{
//            if (map[ind].plane == 0) printf(" peak 2 l1 %f W %f\n",L,W);
    	    L -= slewingt0_correction(map[ind].plane, T0width, 1);
//            if (map[ind].plane == 0) printf(" peak 2 l2 %f\n",L);
	    L -= tmean[1][ind];
//            if (map[ind].plane == 0) printf(" peak 2 l3 %f\n",L);
	    TvsW[map[ind].plane][1]->Fill(W, L);
        }
//	else if ((W1 >= Wcut && W1 < Wmax && W2 < Wcut) || (W1 < Wcut && W2 >= Wmax && W2 < Wmax))
//	{
//            if (map[ind].plane == 0) printf(" peak 2 l1 %f W %f\n",L,W);
//    	    L -= 0.5*slewingt0_correction(map[ind].plane, T0width, 0);
//    	    L -= 0.5*slewingt0_correction(map[ind].plane, T0width, 1);
//            if (map[ind].plane == 0) printf(" peak 2 l2 %f\n",L);
//	    L -= 0.5*(tmean[0][ind]+tmean[1][ind]);
//            if (map[ind].plane == 0) printf(" peak 2 l3 %f\n",L);
//	    TvsW[map[ind].plane][1]->Fill(W, L);
//        }
       }
    }

}

void BmnTof2Raw2Digit::Slewing()
{
  TProfile *prof = 0;
  FILE *fout = 0;
  int nonzero = 0;
  TF1 *f_TW = 0;
  Double_t par1 = 0.;
  Double_t par2 = 0.;
  Double_t par3 = 0.;
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
  par3 = !strcmp(SLFIT,"pol2") && f_TW != 0 ? f_TW->GetParameter(2) : 0.;

  printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
  fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, 0, Wcut, LeadMin[plane], LeadMax[plane]);
  printf(" Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);
  fprintf(fout, " Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);

  fprintf(fout,"Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[0][plane]);
  printf("Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[0][plane]);
  int ip, is;
  for (int ind=0; ind<n_rec; ind++)
  {
    if (map[ind].pair < 0) continue;
    ip = map[ind].plane;
    if (ip != plane) continue;
    is = map[ind].strip;
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
  par3 = !strcmp(SLFIT,"pol2") && f_TW != 0 ? f_TW->GetParameter(2) : 0.;

  printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
  fprintf(fout, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane+1, Wcut, Wmax, LeadMin[plane], LeadMax[plane]);
  printf("Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);
  fprintf(fout, " Time(Width) = %f + %f*Width + %g*Width**2\n", par1, par2, par3);

  fprintf(fout,"Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[1][plane]);
  printf("Chamber #%d channel offsets (average is %f)\n", plane+1, tmean_average[1][plane]);
  for (int ind=0; ind<n_rec; ind++)
  {
    if (map[ind].pair < 0) continue;
    ip = map[ind].plane;
    if (ip != plane) continue;
    is = map[ind].strip;
    fprintf(fout,"   strip %d time shift (left+right)/2 = %f\n", is, tmean[1][ind]);
    printf("   strip %d time shift (left+right)/2 = %f\n", is, tmean[1][ind]);
  }

  fclose(fout);

  } // loop on chambers

  return;
}

void BmnTof2Raw2Digit::readSlewing()
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
  fscanf(fin, "Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", &plane, &wmin[p][pk], &wmax[p][pk], &tmin[p][pk], &tmax[p][pk]);
  printf("Chamber %d slewing selected area Width-Time:      %d %d %d %d\n", plane, wmin[p][pk], wmax[p][pk], tmin[p][pk], tmax[p][pk]);
  if (plane != (p+1))
  {
	printf(" slewing file error, chamber numbers are mismatched, %d != %d\n", p+1, plane);
  }
  fscanf(fin, "Time(Width) = %f + %f*Width + %g*Width**2\n", &TvsW_const[p][pk], &TvsW_slope[p][pk], &TvsW_parab[p][pk]);
  printf("Time(Width) = %f + %f*Width + %g*Width**2\n", TvsW_const[p][pk], TvsW_slope[p][pk], TvsW_parab[p][pk]);

  fscanf(fin,"Chamber #%d channel offsets (average is %f)\n", &plane, &tmean_average[pk][p]);
  printf("Chamber #%d channel offsets (average is %f)\n", plane, tmean_average[pk][p]);
  if (plane != (p+1))
  {
	printf(" slewing file error, chamber numbers are mismatched, %d != %d\n", p+1, plane);
  }
  int ip, is, is1;
  for (int ind=0; ind<n_rec; ind++)
  {
    if (map[ind].pair < 0) continue;
    ip = map[ind].plane;
    if (ip != (plane-1)) continue;
    is = map[ind].strip;
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


void BmnTof2Raw2Digit::fillEvent(TClonesArray *data, TClonesArray *sync, TClonesArray *t0, TClonesArray *tof2digit) {

    unsigned int t0id = 0;
    long long t0time = 0, digittime = 0;
    int T0raw = 0, T0width = 0, T0trail = 0, chan = 0;
    int n_t0_l = 0, n_t0_t = 0;
  
    T0 = 0.;
    for (int i = 0; i < t0->GetEntriesFast(); i++) {
        BmnTDCDigit *digit = (BmnTDCDigit*) t0->At(i);
        int dnl = digit->GetValue() & 0x3FF;
        if (digit->GetLeading()) {
            chan = digit->GetChannel();
            T0raw = digit->GetValue()+INL_Table25[4][chan][dnl]+0.5;
            t0id = digit->GetSerial();
	    T0 = T0raw;
	    n_t0_l++;
        }
	else
	{
            T0trail = digit->GetValue();
	    n_t0_t++;
	}
    }
    if (n_t0_l != n_t0_t) return;
    if (n_t0_l != 1) return;
    if (T0raw == 0 || T0trail == 0) return;
    T0width = (T0trail - T0raw);
    float lead[TOF2_MAX_CHANNEL];
    float trail[TOF2_MAX_CHANNEL];
    memset(lead,0,TOF2_MAX_CHANNEL*sizeof(float));
    memset(trail,0,TOF2_MAX_CHANNEL*sizeof(int));
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnTDCDigit *digit = (BmnTDCDigit*) data->At(i);
       for (int j = 0; j < sync->GetEntriesFast(); j++) {
            BmnSyncDigit *rec = (BmnSyncDigit*) sync->At(j);
            if (rec->GetSerial() == t0id){
               EVENT    = rec->GetEvent();
               TIME_SEC = rec->GetTime_sec();
               TIME_NS  = rec->GetTime_ns();
               t0time   = TIME_SEC*1000000000LL+TIME_NS;
            }
            if (rec->GetSerial() == digit->GetSerial()) digittime = rec->GetTime_sec()*1000000000LL+rec->GetTime_ns();
       }
       if (t0time == 0LL || digittime == 0LL) continue;
       int ts_diff = (int)((digittime - t0time)*INVHPTIMEBIN);
       int ind; 
       for(ind=0;ind<n_rec;ind++) 
         if(digit->GetSerial()==map[ind].id && digit->GetSlot()==map[ind].slot && digit->GetChannel()==map[ind].chan) break;
       if(ind==n_rec) continue; 
//       int tdcnum = map[ind].tdc;
       int slot = map[ind].slot;
       int dnl = digit->GetValue() & 0x3FF;
       chan = digit->GetChannel();
       float tm =  (digit->GetValue()+INL_Table25[slot-3][chan][dnl]) - (T0raw - ts_diff + T0shift);
       if(digit->GetLeading()) lead[ind]=tm; else trail[ind]=tm; 
    }
    for(int ind=0;ind<n_rec;ind++){ 
       int ind1 = map[ind].pair;
       if (ind1 < 0) continue;
       if(lead[ind1]==0 || trail[ind1]==0) continue;
       if(lead[ind]!=0 && trail[ind]!=0){
	float L = (lead[ind1]+lead[ind])/2.;
	float D = (lead[ind1]-lead[ind])/2.;
	float W1 = trail[ind]-lead[ind];
	float W2 = trail[ind1]-lead[ind1];
	float W = (W1+W2)/2.;
	if ((int)W1 < Wcut && (int)W2 < Wcut)
	{
    	    L -= slewingt0_correction(map[ind].plane, T0width, 0);
	    L -= tmean[0][ind];
    	    L -= slewing_correction(map[ind].plane, W, 0);
	    TvsW[map[ind].plane][0]->Fill(W, L);
	}
	else if (W1 >= Wcut && W2 >= Wcut)
	{
    	    L -= slewingt0_correction(map[ind].plane, T0width, 1);
	    L -= tmean[1][ind];
    	    L -= slewing_correction(map[ind].plane, W, 1);
	    TvsW[map[ind].plane][1]->Fill(W, L);
        }
	else if (W1 >= Wcut && W2 < Wcut)
	{
    	    L -= 0.5*slewingt0_correction(map[ind].plane, T0width, 0);
    	    L -= 0.5*slewingt0_correction(map[ind].plane, T0width, 1);
	    L -= 0.5*(tmean[0][ind]+tmean[1][ind]);
    	    L -= 0.5*slewing_correction(map[ind].plane, W2, 0);
    	    L -= 0.5*slewing_correction(map[ind].plane, W1, 1);
	    TvsW[map[ind].plane][1]->Fill(W, L);
	}
	else if (W1 < Wcut && W2 > Wcut)
	{
    	    L -= 0.5*slewingt0_correction(map[ind].plane, T0width, 0);
    	    L -= 0.5*slewingt0_correction(map[ind].plane, T0width, 1);
	    L -= 0.5*(tmean[0][ind]+tmean[1][ind]);
    	    L -= 0.5*slewing_correction(map[ind].plane, W1, 0);
    	    L -= 0.5*slewing_correction(map[ind].plane, W2, 1);
	    TvsW[map[ind].plane][1]->Fill(W, L);
	}
        TClonesArray &ar = *tof2digit;
        new(ar[tof2digit->GetEntriesFast()]) BmnTof2Digit(map[ind].plane,map[ind].strip,L*HPTIMEBIN,W,D*HPTIMEBIN);  
       }
    }
}

void BmnTof2Raw2Digit::SlewingResults()
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

float BmnTof2Raw2Digit::slewingt0_correction(int chamber, float width, int peak)
{
    if (chamber < 0 || chamber >= MaxPlane || peak < 0 || peak > 1) return 0.;
    float cor = TvsWt0_const[chamber][peak] + TvsWt0_slope[chamber][peak]*width + TvsWt0_parab[chamber][peak]*width*width;
    return cor;
}

float BmnTof2Raw2Digit::slewing_correction(int chamber, float width, int peak)
{
    if (chamber < 0 || chamber >= MaxPlane || peak < 0 || peak > 1) return 0.;
    float cor = TvsW_const[chamber][peak] + TvsW_slope[chamber][peak]*width + TvsW_parab[chamber][peak]*width*width;
    return cor;
}

void BmnTof2Raw2Digit::INL_read25()
{
  float tcor, ecor;
  int n, pos;
  char atext[50];
  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/input/TDC2_INL.dat";
  ifstream fi(path.Data());
  if(fi.is_open())
  {
   for (n=0; n<32; n++)
      {
      for (int i=0; i<1024; i++)
         {
         fi.getline(atext,50);
         sscanf(atext,"%d %f %f\n", &pos,&tcor,&ecor);
         INL_Table25[0][n][i] = tcor;
         }
      }
   fi.close();
  }

  path = dir + "/input/TDC3_INL.dat";
  ifstream fi1(path.Data());
  if(fi1.is_open())
  {
   for (n=0; n<32; n++)
     {
     for (int i=0; i<1024; i++)
        {
        fi1.getline(atext,50);
        sscanf(atext,"%d %f %f\n", &pos,&tcor,&ecor);
        INL_Table25[1][n][i] = tcor;
        }
      }
   fi1.close();
  }
}

void BmnTof2Raw2Digit::INL_read25_gol()
{
   float tcor;
   int n, pos, post;
   char atext[25600];
   TString dir = getenv("VMCWORKDIR");
   TString path = dir + "/input/TDC1_INL.dat";
   ifstream fi(path.Data());
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
        INL_Table25[3][n][i] = tcor;
//        cout << " Channel " << n << " bin " << i << "   Time correction :" << tcor << " pos " << pos << endl ;
     }
     fi.getline(atext,25600);
     n++;
     if (n==32) break;
    }
   fi.close();
   }

   path = dir + "/input/TDC4_INL.dat";
   ifstream fi1(path.Data());
   if(fi1.is_open())
   {
    n = 0;
    while (!fi1.eof())
    {
     fi1.getline(atext,25600);
     post = 0;
     pos = 0;
     for (int i=0; i<1024; i++)
     {
        sscanf(&atext[post],"%f %n", &tcor, &pos);
	post += pos;
        INL_Table25[2][n][i] = tcor;
//        cout << " Channel " << n << " bin " << i << "   Time correction :" << tcor << " pos " << pos << endl ;
     }
     fi1.getline(atext,25600);
     n++;
     if (n==32) break;
    }
   fi1.close();
   }

}


void BmnTof2Raw2Digit::INL_read25_ffd()
{
   float tcor;
   int n, pos, post;
   char atext[25600];
   TString dir = getenv("VMCWORKDIR");
   TString path = dir + "/input/TDC_FFD_INL.dat";
   ifstream fi(path.Data());
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
        INL_Table25[4][n][i] = tcor;
//        cout << " Channel " << n << " bin " << i << "   Time correction :" << tcor << " pos " << pos << endl ;
     }
     fi.getline(atext,25600);
     n++;
     if (n==32) break;
    }
   fi.close();
   }

}

void BmnTof2Raw2Digit::drawprep()
{
  TCanvas *cp = new TCanvas("cp", "Leadings&widths vs strip", 900,700);
 
  int i;
  cp->cd();
  cp->Divide(2,5);
  for (i=0; i<4; i++)
    {
      cp->cd(i*2+1);
      TvsS[i]->Draw();
      gPad->AddExec("exselt","select_hist()");
      cp->cd(i*2+2);
      WvsS[i]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   
    cp->cd(9);
    Wt0->Draw();
    gPad->AddExec("exselt","select_hist()");
    cp->cd(10);
    Wts->Draw();
    gPad->AddExec("exselt","select_hist()");

  TCanvas *cp1 = new TCanvas("cp1", "Leadings vs widths", 900,700);
 
  cp1->cd();
  cp1->Divide(1,4);
  for (i=0; i<4; i++)
    {
      cp1->cd(i+1);
      TvsWall[i]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   

  TCanvas *cp2 = new TCanvas("cp2", "Leadings vs widths (max strip)", 900,700);
 
  cp2->cd();
  cp2->Divide(1,4);
  for (i=0; i<4; i++)
    {
      cp2->cd(i+1);
      TvsWallmax[i]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   
  return;
}

void BmnTof2Raw2Digit::drawprof()
{
  TCanvas *callbe = new TCanvas("callbe", "Leadings vs widths (slewing RPC)", 900,700);
 
  int i;
  callbe->cd();
  callbe->Divide(2,4);
  for (i=0; i<4; i++)
    {
      callbe->cd(i*2+1);
      TvsW[i][0]->Draw();
      gPad->AddExec("exselt","select_hist()");
      callbe->cd(i*2+2);
      TvsW[i][1]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   
  return;
}


void BmnTof2Raw2Digit::drawproft0()
{
  TCanvas *callbe0 = new TCanvas("callbe0", "Leadings vs widths (slewing T0)", 900,700);
 
  int i;
  callbe0->cd();
  callbe0->Divide(2,4);
  for (i=0; i<4; i++)
    {
      callbe0->cd(i*2+1);
      TvsWt0[i][0]->Draw();
      gPad->AddExec("exselt","select_hist()");
      callbe0->cd(i*2+2);
      TvsWt0[i][1]->Draw();
      gPad->AddExec("exselt","select_hist()");
    }   
  return;
}

int BmnTof2Raw2Digit::readGeom(int *numgeom)
{
	float halfxwidth[6] = {80,80,280,280,80,280};
	float halfywidth[6] = { 5, 5,  9,  9, 5,  9};
	float xoffs = +60., yoffs = -74., zoffs = +1370.+6650.;
	char fname[128];
	FILE *fg = 0;
	int nf = 0, ic, ns;
	float x,y,z;
	TString dir = getenv("VMCWORKDIR");
	for (int i=0; i<TOF2_MAX_CHAMBERS; i++)
	{
	    ic = numgeom[i];
	    if (ic < 1) continue;
	    sprintf(fname,"%s/geometry/TOF2chamber%d.txt",dir.Data(),ic);
	    fg = fopen(fname,"r");
	    if (fg == NULL) continue;
	    nstrips[i] = 0;
	    while(fscanf(fg,"%d %f %f %f\n",&ns, &x, &y, &z) == 4)
	    {
		zchamb[i] = -z + zoffs;
		xcens[i][ns] = x + xoffs;
		ycens[i][ns] = y + yoffs;
		xmins[i][ns] = xcens[i][ns] - halfxwidth[ic];
		xmaxs[i][ns] = xcens[i][ns] + halfxwidth[ic];
		ymins[i][ns] = ycens[i][ns] - halfywidth[ic];
		ymaxs[i][ns] = ycens[i][ns] + halfywidth[ic];
//		printf("C %d S %d %f %f %f %f %f\n",ic,ns,zchamb[i],xmins[i][ns],xmaxs[i][ns],ymins[i][ns],ymaxs[i][ns]);
		nstrips[i]++;
	    }
	    fclose(fg);
	    nf++;
	}
//	i = 3;
//	ns = 5;
//	printf("C %d S %d %f %f %f %f %f\n",i,ns,zchamb[i],xmins[i][ns],xmaxs[i][ns],ymins[i][ns],ymaxs[i][ns]);
	if (nf == MaxPlane) return 1;
	else
	{
	    printf("Geometry for %d chambers only!\n", nf);
	    return 0;
	}
}

int BmnTof2Raw2Digit::get_strip_xyz(int chamber, int strip, float *x, float *y, float *z)
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

int BmnTof2Raw2Digit::get_chamber_z(int chamber, float *z)
{
    if (chamber < MaxPlane)
    {
	*z = zchamb[chamber];
	return 1;
    }
    else
	return 0;
}

int BmnTof2Raw2Digit::get_track_hits(float *xyz, float *cxyz, int *nhits, int *chamb, int *strip)
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

int BmnTof2Raw2Digit::printGeom()
{
    float halfxwidth[6] = {80,80,280,280,80,280};
    float halfywidth[6] = { 5, 5,  9,  9, 5,  9};
    for (int j=0; j<MaxPlane; j++)
    {
	printf("Chamber %d, number of strips %d, Z-coordinate %f, WidthX/2 %f, WidthY/2 %f\n",j+1,nstrips[j],zchamb[j], halfxwidth[j], halfywidth[j]);
	for (int k=0; k<nstrips[j]; k++)
	{
		printf("   Strip %d X %f Y %f\n", k+1, xcens[j][k],ycens[j][k]);
	}
    }
}

ClassImp(BmnTof2Raw2Digit)

