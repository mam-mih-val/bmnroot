#include "TMath.h"
#include "TSystem.h"
#include "TBox.h"
#include "TText.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "BmnECALRaw2Digit.h"

#define PRINT_ECAL_CALIBRATION 0
#define SHIFT 0

static void fcn1(Int_t& npar, Double_t *gin, Double_t& f, Double_t *par, Int_t iflag);

BmnECALRaw2Digit::BmnECALRaw2Digit(){
  n_rec=0;
  n_test=0;
}
BmnECALRaw2Digit::BmnECALRaw2Digit(TString mappingFile, TString RunFile, TString CalibrationFile) {
    for (int i=0; i<MAX_ECAL_CHANNELS; i++) ecal_amp[i] = -1.;
    for (int i=0; i<MAX_ECAL_LOG_CHANNELS; i++) log_amp[i] = -1.;
    for (int i=0; i<MAX_ECAL_LOG_CHANNELS; i++) test_chan[i] = -1;
    for (int i=0; i<MAX_ECAL_LOG_CHANNELS; i++) test_id[i] = -1;
    for (int i=0; i<512; i++) is_test[i] = -1;
    n_test = 0;
    n_rec=0;
    TString dummy;
    ifstream in;

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/input/";
    in.open((path + mappingFile).Data());
    if (!in.is_open())
    {
	printf("Loading ECAL Map from file: %s - file open error!\n", mappingFile.Data());
	return;
    }
    printf("Loading ECAL Map from file: %s\n", mappingFile.Data());
    in >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;
    maxchan = 0;
    int ixmin = -1, ixmax = -1, iymin = -1, iymax = -1;
    int xmin = 10000., xmax = -10000., ymin = 10000., ymax = -10000.;
    while (!in.eof()) {
        int id,chan,front_chan,size,ix,iy,used;
	float x,y;
//        in >>std::hex >> id >>std::dec >> chan >> front_chan >> size >> ix >> iy >> x >> y >> used;
        in >> std::hex >> id >> std::dec >> chan >> front_chan >> x >> y;
        if (!in.good()) break;
//	printf("%0x %d %d %d %d %d %f %f\n",id,chan,front_chan,size,ix,iy,x,y);
/*
	if (size >= 200 && size <= 223)
	{
	  if (n_test < MAX_ECAL_LOG_CHANNELS && chan > 0)
	  {
	    if (is_test[chan-1] < 0) is_test[chan-1] = n_test;
	    else		     is_test[128 + chan-1] = n_test;
	    test_chan[n_test] = chan-1;
	    test_id[n_test++] = id;
	  };
	};
	if (size > 2 || size < 0) continue;
*/
	if (chan <= 0) continue;
	if (front_chan <= 0) continue;
        ecal_map_element[n_rec].id=id;
	if (chan > 64) chan = (chan-1)%64 + 1;
        ecal_map_element[n_rec].adc_chan=chan-1;
	if (front_chan > maxchan) maxchan = front_chan;
        ecal_map_element[n_rec].chan=front_chan-1;
        ecal_map_element[n_rec].size=0;
	x *= 10.;
	y *= 10.;
        ecal_map_element[n_rec].x=x;
        ecal_map_element[n_rec].y=y;
        ecal_map_element[n_rec].ix=(int)(x/40.);
        ecal_map_element[n_rec].iy=(int)(y/40.);
        ecal_map_element[n_rec].used=1;
	if (x < xmin)
	{
	    xmin = x;
	    ixmin = n_rec;
	}
	if (x > xmax)
	{
	    xmax = x;
	    ixmax = n_rec;
	}
	if (y < ymin)
	{
	    ymin = y;
	    iymin = n_rec;
	}
	if (y > ymax)
	{
	    ymax = y;
	    iymax = n_rec;
	}
	n_rec++;
    }
    in.close();
    cell_size[0] = 40.;
    if (ixmin >= 0)
    {
	x_min = ecal_map_element[ixmin].x - cell_size[ecal_map_element[ixmin].size];
    }
    else
    {
	x_min = -1000.;
    }
    if (ixmax >= 0)
    {
	x_max = ecal_map_element[ixmax].x + cell_size[ecal_map_element[ixmin].size];
    }
    else
    {
	x_max = +1000.;
    }
    if (iymin >= 0)
    {
	y_min = ecal_map_element[iymin].y - cell_size[ecal_map_element[ixmin].size];
    }
    else
    {
	y_min = -1000.;
    }
    if (iymax >= 0)
    {
	y_max = ecal_map_element[iymax].y + cell_size[ecal_map_element[ixmin].size];
    }
    else
    {
	y_max = +1000.;
    }
//-------------------------------
    int RUN;
    const char *fname = RunFile.Data();
    char *delim = NULL;
    sscanf(&fname[strlen(fname) - 13], "%d", &RUN);
    strcpy(filname_base, gSystem->BaseName(fname));
    if ((delim = strrchr(filname_base, (int)'.'))) *delim = '\0';
    FILE *fin = 0;
    char filn[128], tit1[32] = {"Channel"}, tit2[32] = {"Calibration"}, tit3[32] = {"Error"};
    TString path1 = dir + "/parameters/ecal/";
    if (strlen(CalibrationFile.Data()) == 0) sprintf(filn, "%s%s_calibration.txt", path1.Data(), filname_base);
    else sprintf(filn, "%s%s", path1.Data(), CalibrationFile.Data());
    fin = fopen(filn,"r");
    for (int i=0; i<maxchan; i++)
    {
	cal[i] = 1.;
	cale[i] = 0.;
	cal_out[i] = 1.;
	cale_out[i] = 0.;
    }
    if (!fin)
    {
	printf(" ECAL: Can't open calibration file %s, use default calibration coefficients 1.\n\n", filn);
    }
    else
    {
	Int_t ch1;
	Float_t ca = 1., cae = 0.;
	fscanf(fin, "%s %s %s\n", tit1, tit2, tit3);
	while (fscanf(fin, "%d %f %f\n", &ch1, &ca, &cae) == 3)
	{
	    if (ch1 > 0 && ch1 <= maxchan)
	    {
		cal[ch1-1] = ca;
		cale[ch1-1] = cae;
	    }
	};
	fclose(fin);
    }
    if (PRINT_ECAL_CALIBRATION) printf("\n ECAL calibration coefficients\n\n%s\t%s\t%s\n\n", tit1, tit2, tit3);
    for (int i=0; i<maxchan; i++)
    {
	if (PRINT_ECAL_CALIBRATION) printf("%d\t%f\t%f\n", i, cal[i], cale[i]);
    }
//----------------------------------
    nevents_ecal = 0;
    use_log_function_ecal = 0;
    thres = 15.;
    wave2amp_flag = 1;
    min_samples = 5;
    ped_samples = 3;
    use_meanxy = 0;
    sigma_amp = 10.;
    shower_energy = 48.;
    shower_norm = 6.461048; // 7.5 cm modules
//    shower_norm = 13.923893; // 15 cm modules
    x_beam = 0.;
    y_beam = 0.;
    for (int j=0; j<MAX_ECAL_CHANNELS; j++)
    {
	number[j] = 0;
	index[j] = 0;
	channel0[j] = 0;
	channel1[j] = 0;
	SampleProf[j] = NULL;
	for (int i=0; i<MAX_ECAL_EVENTS; i++)
	{
	    amp_array_ecal[i][j] = 0.;
	    pedestals_ecal[i][j] = 0.;
	    profile_amp_ecal[i][j] = 0.;
	    profile_err_ecal[i][j] = 0.;
	}
    }

    char tit[128], nam[128];

    for (int i=0; i<MAX_ECAL_LOG_CHANNELS; i++) htest[i] = NULL;
    for (int i=0; i<MAX_ECAL_LOG_CHANNELS; i++) TestProf[i] = NULL;
    for (int i=0; i<n_test; i++)
    {
      if (test_chan[i] >= 0)
      {
	sprintf(nam, "htest%d_ecal", i);
	sprintf(tit, "Amplitude for adc test channel %d (ecal)", test_chan[i]);
	htest[i] = new TH1F(nam, tit, 2000, 0., 16000.);
	sprintf(nam, "ptest%d_ecal", i);
	sprintf(tit, "Average sampling wave for adc test channel %d (ecal)", test_chan[i]);
	TestProf[i]   = new TProfile(nam, tit, 200, 0., 200., -100000., +100000.,"s");
      }
    }
    hsum_sim = new TH1F("hsumsim_ecal","Sum of theoretical amplitudes (ecal)", 1000, 0., 100.);
    hsum_raw = new TH1F("hsumraw_ecal","Sum of raw amplitudes (ecal)", 2000, 0., 20000.);
    hsum     = new TH1F("hsumcal_ecal","Sum of calibrated amplitudes (ecal)", 1000, 0., 200.);

    hxmean   = new TH1F("hxmean_ecal","Shower mean X (ecal)", 750, 0., +1500.);
    hymean   = new TH1F("hymean_ecal","Shower mean Y (ecal)", 750, 0., +1500.);

    hampl   = new TH1F("hampl","Module amplitudes (ecal)", 1000, 0., 1000.);

    for (int i=0; i<n_rec; i++)
    {
	sprintf(tit, "samprof%d_ecal", ecal_map_element[i].chan+1);
	sprintf(nam, "Average sampling wave, module %d (ecal)", ecal_map_element[i].chan+1);
	SampleProf[ecal_map_element[i].chan]   = new TProfile(tit, nam, 50, 0., 50., -100000., +100000.,"s");
    }
}


void BmnECALRaw2Digit::print() {
     printf("id#\t\tECAL chan\tADC_chan\tsize\tix\tiy\tx\ty\tused\n");
     for(int i=0;i<n_rec;i++)
     printf("0x%06lX\t\t%d\t%d\t%d\t%d\t%d\t%d\t%g\t%g\n",
         ecal_map_element[i].id,ecal_map_element[i].chan+1,ecal_map_element[i].adc_chan+1,ecal_map_element[i].size+1,
         ecal_map_element[i].ix,ecal_map_element[i].iy,ecal_map_element[i].used,ecal_map_element[i].x,ecal_map_element[i].y); 
   
}


void BmnECALRaw2Digit::fillSampleProfiles(TClonesArray *data, Float_t x, Float_t y, Float_t e, Int_t clsize) {
    Float_t amp = 0;
    double r = 0., dx = 0., dy = 0.;
    for (int i=0; i<MAX_ECAL_CHANNELS; i++) ecal_amp[i] = -1.;
    for (int i=0; i<MAX_ECAL_LOG_CHANNELS; i++) log_amp[i] = -1.;
    if (nevents_ecal == 0)
    {
	ncells = 0;
	x_beam = x;
	y_beam = y;
	shower_energy = e;
        for(int i=0;i<clsize*clsize;i++)
	{
	    channel1[i] = -1;
	}
	float xnear[MAX_ECAL_CHANNELS], ynear[MAX_ECAL_CHANNELS];
	int ixhalf = -1, iyhalf = -1, i0 = -1, imin = -1;
	float rmin = 10000.;
        for(int ind=0;ind<n_rec;ind++)
	{
	    number[ind] = -1;
	    channel0[ind] = -1;
	    dx = x - ecal_map_element[ind].x;
	    dy = y - ecal_map_element[ind].y;
	    r = TMath::Sqrt(dx*dx + dy*dy);
	    if (r < rmin)
	    {
		rmin = r;
		imin = ind;
	    }
	    if (x >= ecal_map_element[ind].x - cell_size[ecal_map_element[ind].size]/2. && \
	        x <  ecal_map_element[ind].x + cell_size[ecal_map_element[ind].size]/2. && \
	        y >= ecal_map_element[ind].y - cell_size[ecal_map_element[ind].size]/2. && \
	        y <  ecal_map_element[ind].y + cell_size[ecal_map_element[ind].size]/2 )
	    {
		if (x >= ecal_map_element[ind].x) ixhalf = 1;
		if (y >= ecal_map_element[ind].y) iyhalf = 1;
		number[ind] = ncells;
		channel0[ind] = ecal_map_element[ind].chan;
		channel1[ncells] = ecal_map_element[ind].chan;
		index[ncells] = ind;
		xnear[0] = ecal_map_element[index[ncells]].x;
		ynear[0] = ecal_map_element[index[ncells]].y;
		ncells++;
		i0 = ind;
//		printf(" ncells %d cell %d\n", ncells, ecal_map_element[ind].chan+1);
//		break;
	    }
	}
	if (i0 == -1)
	{
	    if (x<x_min || x>x_max || y<y_min || y>y_max)
	    {
		printf("Beam entry point (%f,%f) outside ECAL area!\n", x, y);
		return;
	    }
	    else if (imin >= 0)
	    {
		if (x >= ecal_map_element[imin].x) ixhalf = 1;
		if (y >= ecal_map_element[imin].y) iyhalf = 1;
		number[imin] = ncells;
		channel0[imin] = ecal_map_element[imin].chan;
		channel1[ncells] = ecal_map_element[imin].chan;
		index[ncells] = imin;
		xnear[0] = ecal_map_element[index[ncells]].x;
		ynear[0] = ecal_map_element[index[ncells]].y;
		ncells++;
	    }
	    else
	    {
		printf("Invalid beam entry point (%f,%f)!\n", x, y);
		return;
	    }
	}
	for(int i=1; i<clsize; i++)
	{
	    xnear[i] = ecal_map_element[index[0]].x + ixhalf*cell_size[ecal_map_element[index[0]].size];
	    ixhalf = -ixhalf;
	}
	for(int i=1; i<clsize; i++)
	{
	    ynear[i] = ecal_map_element[index[0]].y + iyhalf*cell_size[ecal_map_element[index[0]].size];
	    iyhalf = -iyhalf;
	}

	for(int i=0; i<clsize; i++)
	{
	    for(int j=0; j<clsize; j++)
	    {
    		for(int ind=0;ind<n_rec;ind++)
		{
		    if (ind == index[0]) continue;
		    if (xnear[i] >= ecal_map_element[ind].x - cell_size[ecal_map_element[ind].size]/2. && \
	    		xnear[i] <  ecal_map_element[ind].x + cell_size[ecal_map_element[ind].size]/2. && \
	    		ynear[j] >= ecal_map_element[ind].y - cell_size[ecal_map_element[ind].size]/2. && \
	    		ynear[j] <  ecal_map_element[ind].y + cell_size[ecal_map_element[ind].size]/2 )
		    {
			number[ind] = ncells;
			channel0[ind] = ecal_map_element[ind].chan;
			channel1[ncells] = ecal_map_element[ind].chan;
			index[ncells] = ind;
			ncells++;
//			printf(" ncells %d cell %d\n", ncells, ecal_map_element[ind].chan+1);
		    }
		}
	    }
	}
    }
    float ped = 0., xm = 0., ym = 0., s = 0.;
    int num_test = 0;
    if (data != NULL)
    {
     for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnADCDigit *digit = (BmnADCDigit*) data->At(i);
       num_test = is_test[digit->GetChannel()];
       if (num_test < 0) num_test = is_test[digit->GetChannel()+128];
       if (num_test >= 0 && (digit->GetSerial()) == test_id[num_test])
       {
    	    UShort_t *samt = digit->GetUShortValue();
	    int j2 = 0;
    	    for (int j1 = 0;j1<digit->GetNSamples(); j1++)
    	    {
//		j2 = 1 - j1%2 + (j1/2)*2;
		j2 = j1;
		if (TestProf[num_test]) TestProf[num_test]->Fill(j1,samt[j2]>>SHIFT);
    	    }
    	    if ((amp = testwave2amp(digit->GetNSamples(),digit->GetUShortValue(), &ped)) >= 0.)
	    {
		if (htest[num_test]) htest[num_test]->Fill(amp);
		log_amp[num_test] = amp;
	    }
	    continue;
       }
       int ind, num; 
       for(ind=0;ind<n_rec;ind++) if((digit->GetSerial())==ecal_map_element[ind].id && digit->GetChannel()==(ecal_map_element[ind].adc_chan)) break;
       if(ind==n_rec) continue; 
       if(ecal_map_element[ind].used==0) continue;
       if((num=number[ind])<0) continue;
       Short_t *sam = (Short_t *)digit->GetUShortValue();
       int j2 = 0;
       for (int j = 0;j<digit->GetNSamples(); j++)
       {
//	    j2 = 1 - j%2 + (j/2)*2;
	    j2 = j;
	    if (SampleProf[num]) SampleProf[num]->Fill(j,sam[j2]>>SHIFT);
       }
       if ((amp = wave2amp(digit->GetNSamples(),digit->GetUShortValue(), &ped)) >= 0.)
       {
	    hampl->Fill(amp);
//	    printf("chan %d amp %f coef %f\n", ecal_map_element[ind].chan, amp, cal[ecal_map_element[ind].chan]);
	    xm += amp*cal[ecal_map_element[ind].chan]*ecal_map_element[ind].x;
	    ym += amp*cal[ecal_map_element[ind].chan]*ecal_map_element[ind].y;
	    s += amp*cal[ecal_map_element[ind].chan];
	    ecal_amp[ecal_map_element[ind].chan] = amp;
       }
     }
    }
    if (s > 0.) hxmean->Fill(xm/s);
    if (s > 0.) hymean->Fill(ym/s);
    nevents_ecal++;
}


void BmnECALRaw2Digit::fillSampleProfilesAll(TClonesArray *data, Float_t x, Float_t y, Float_t e) {
    Float_t amp = 0;
    Int_t j = 0;
    for (int i=0; i<MAX_ECAL_CHANNELS; i++) ecal_amp[i] = -1.;
    for (int i=0; i<MAX_ECAL_LOG_CHANNELS; i++) log_amp[i] = -1.;
    if (nevents_ecal >= MAX_ECAL_EVENTS) return;
    if (nevents_ecal == 0)
    {
	x_beam = x;
	y_beam = y;
	int i0 = -1;
        for(int ind=0;ind<n_rec;ind++)
	{
	    if (x >= ecal_map_element[ind].x - cell_size[ecal_map_element[ind].size]/2. && \
	        x <  ecal_map_element[ind].x + cell_size[ecal_map_element[ind].size]/2. && \
	        y >= ecal_map_element[ind].y - cell_size[ecal_map_element[ind].size]/2. && \
	        y <  ecal_map_element[ind].y + cell_size[ecal_map_element[ind].size]/2 )
	    {
		i0 = ind;
		break;
	    }
	}
	if (i0 == -1)
	{
//	    printf("Warninig: Beam entry point (%f,%f) outside any ECAL module!\n", x, y);
//	    return;
	}
	ncells = n_rec;
	shower_energy = e;
        for(int i=0;i<n_rec;i++)
	{
	    channel1[i] = -1;
	}
        for(int ind=0;ind<n_rec;ind++)
	{
		    number[ind] = j;
		    channel0[ind] = ecal_map_element[ind].chan;
		    channel1[j] = ecal_map_element[ind].chan;
		    j++;
	}
    }
    float ped = 0., xm = 0., ym = 0., s = 0.;
    int num_test = 0;
    if (data != NULL)
    {
     for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnADCDigit *digit = (BmnADCDigit*) data->At(i);
       num_test = is_test[digit->GetChannel()];
       if (num_test < 0) num_test = is_test[digit->GetChannel()+128];
       if (num_test >= 0 && (digit->GetSerial()) == test_id[num_test])
       {
    	    UShort_t *samt = digit->GetUShortValue();
	    int j2 = 0;
    	    for (int j1 = 0;j1<digit->GetNSamples(); j1++)
    	    {
//		j2 = 1 - j1%2 + (j1/2)*2;
		j2 = j1;
		if (TestProf[num_test]) TestProf[num_test]->Fill(j1,samt[j2]>>SHIFT);
    	    }
    	    if ((amp = testwave2amp(digit->GetNSamples(),digit->GetUShortValue(), &ped)) >= 0.)
	    {
		if (htest[num_test]) htest[num_test]->Fill(amp);
		log_amp[num_test] = amp;
	    }
	    continue;
       }
       int ind, num; 
       for(ind=0;ind<n_rec;ind++) if((digit->GetSerial())==ecal_map_element[ind].id && digit->GetChannel()==(ecal_map_element[ind].adc_chan)) break;
       if(ind==n_rec) continue; 
       if(ecal_map_element[ind].used==0) continue;
       if((num=number[ind])<0) continue;
       Short_t *sam = (Short_t *)digit->GetUShortValue();
       int j2 = 0;
       for (int j1 = 0;j1<digit->GetNSamples(); j1++)
       {
//	    j2 = 1 - j1%2 + (j1/2)*2;
	    j2 = j1;
	    if (SampleProf[num]) SampleProf[num]->Fill(j1,sam[j2]>>SHIFT);
       }
       if ((amp = wave2amp(digit->GetNSamples(),digit->GetUShortValue(), &ped)) >= 0.)
       {
	    hampl->Fill(amp);
//	    printf("chan %d amp %f coef %f\n", ecal_map_element[ind].chan, amp, cal[ecal_map_element[ind].chan]);
	    xm += amp*cal[ecal_map_element[ind].chan]*ecal_map_element[ind].x;
	    ym += amp*cal[ecal_map_element[ind].chan]*ecal_map_element[ind].y;
	    s += amp*cal[ecal_map_element[ind].chan];
	    ecal_amp[ecal_map_element[ind].chan] = amp;
       }
     }
    }
    if (s > 0.) hxmean->Fill(xm/s);
    if (s > 0.) hymean->Fill(ym/s);
    nevents_ecal++;
}

void BmnECALRaw2Digit::fillEvent(TClonesArray *data, TClonesArray *ecaldigit) {
    for (int i=0; i<MAX_ECAL_CHANNELS; i++) ecal_amp[i] = -1.;
    for (int i=0; i<MAX_ECAL_LOG_CHANNELS; i++) log_amp[i] = -1.;
    Float_t amp = 0., ped = 0.;
    int num_test = 0;
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnADCDigit *digit = (BmnADCDigit*) data->At(i);
       num_test = is_test[digit->GetChannel()];
       if (num_test < 0) num_test = is_test[digit->GetChannel()+128];
       if (num_test >= 0 && (digit->GetSerial()) == test_id[num_test])
       {
    	    UShort_t *samt = digit->GetUShortValue();
	    int j2 = 0;
    	    for (int j1 = 0;j1<digit->GetNSamples(); j1++)
    	    {
//		j2 = 1 - j1%2 + (j1/2)*2;
		j2 = j1;
		if (TestProf[num_test]) TestProf[num_test]->Fill(j1,samt[j2]>>SHIFT);
    	    }
    	    if ((amp = testwave2amp(digit->GetNSamples(),digit->GetUShortValue(), &ped)) >= 0.)
	    {
		if (htest[num_test]) htest[num_test]->Fill(amp);
		log_amp[num_test] = amp;
	    }
	    continue;
       }
       int ind; 
       for(ind=0;ind<n_rec;ind++) {if((digit->GetSerial())==ecal_map_element[ind].id && digit->GetChannel()==(ecal_map_element[ind].adc_chan)) break;}
       if(ind==n_rec) continue; 
       if(ecal_map_element[ind].used==0) continue;
       TClonesArray &ar_ecal = *ecaldigit;
       if ((amp = wave2amp(digit->GetNSamples(),digit->GetUShortValue(), &ped)) >= 0.)
       {
	   hampl->Fill(amp);
	   amp *= cal[ecal_map_element[ind].chan];
           new(ar_ecal[ecaldigit->GetEntriesFast()]) BmnECALDigit(ecal_map_element[ind].ix,ecal_map_element[ind].iy,ecal_map_element[ind].x,ecal_map_element[ind].y,ecal_map_element[ind].size+1,
           ecal_map_element[ind].chan+1,amp);  
	   ecal_amp[ecal_map_element[ind].chan] = amp;
       }
    }
}

void BmnECALRaw2Digit::fillAmplitudes(TClonesArray *data) {
    for (int i=0; i<MAX_ECAL_CHANNELS; i++) ecal_amp[i] = -1.;
    for (int i=0; i<MAX_ECAL_LOG_CHANNELS; i++) log_amp[i] = -1.;
    Float_t amp = 0., ped = 0.;
    int num_test = 0;
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnADCDigit *digit = (BmnADCDigit*) data->At(i);
       num_test = is_test[digit->GetChannel()];
       if (num_test < 0) num_test = is_test[digit->GetChannel()+128];
       if (num_test >= 0 && (digit->GetSerial()) == test_id[num_test])
       {
    	    if ((amp = testwave2amp(digit->GetNSamples(),digit->GetUShortValue(), &ped)) >= 0.)
	    {
		log_amp[num_test] = amp;
	    }
	    continue;
       }
       int ind; 
       for(ind=0;ind<n_rec;ind++) if((digit->GetSerial())==ecal_map_element[ind].id && digit->GetChannel()==(ecal_map_element[ind].adc_chan)) break;
       if(ind==n_rec) continue; 
       if(ecal_map_element[ind].used==0) continue;
       if ((amp = wave2amp(digit->GetNSamples(),digit->GetUShortValue(), &ped)) >= 0.)
       {
	   ecal_amp[ecal_map_element[ind].chan] = amp;
       }
    }
}

int BmnECALRaw2Digit::fillCalibrateCluster(TClonesArray *data, Float_t x, Float_t y, Float_t e, Int_t clsize) {
    for (int i=0; i<MAX_ECAL_CHANNELS; i++) ecal_amp[i] = -1.;
    for (int i=0; i<MAX_ECAL_LOG_CHANNELS; i++) log_amp[i] = -1.;
    Float_t amp = 0;
    double r = 0., dx = 0., dy = 0.;
    static double coef[MAX_ECAL_CHANNELS] = {1.};
    if (nevents_ecal >= MAX_ECAL_EVENTS) return 1;
    if (nevents_ecal == 0)
    {
	ncells = 0;
	x_beam = x;
	y_beam = y;
	shower_energy = e;
        for(int i=0;i<clsize*clsize;i++)
	{
	    channel1[i] = -1;
	}
	float xnear[MAX_ECAL_CHANNELS], ynear[MAX_ECAL_CHANNELS];
	int ixhalf = -1, iyhalf = -1, i0 = -1, imin = -1;
	float rmin = 10000.;
        for(int ind=0;ind<n_rec;ind++)
	{
	    number[ind] = -1;
	    channel0[ind] = -1;
	    dx = x - ecal_map_element[ind].x;
	    dy = y - ecal_map_element[ind].y;
	    r = TMath::Sqrt(dx*dx + dy*dy);
	    if (r < rmin)
	    {
		rmin = r;
		imin = ind;
	    }
	    if (x >= ecal_map_element[ind].x - cell_size[ecal_map_element[ind].size]/2. && \
	        x <  ecal_map_element[ind].x + cell_size[ecal_map_element[ind].size]/2. && \
	        y >= ecal_map_element[ind].y - cell_size[ecal_map_element[ind].size]/2. && \
	        y <  ecal_map_element[ind].y + cell_size[ecal_map_element[ind].size]/2 )
	    {
		if (x >= ecal_map_element[ind].x) ixhalf = 1;
		if (y >= ecal_map_element[ind].y) iyhalf = 1;
		number[ind] = ncells;
		channel0[ind] = ecal_map_element[ind].chan;
		channel1[ncells] = ecal_map_element[ind].chan;
		index[ncells] = ind;
		xnear[0] = ecal_map_element[index[ncells]].x;
		ynear[0] = ecal_map_element[index[ncells]].y;
		ncells++;
		i0 = ind;
//		printf(" ncells %d cell %d\n", ncells, ecal_map_element[ind].chan+1);
//		break;
	    }
	}
	if (i0 == -1)
	{
	    if (x<x_min || x>x_max || y<y_min || y>y_max)
	    {
		printf("Beam entry point (%f,%f) outside ECAL area!\n", x, y);
		return 1;
	    }
	    else if (imin >= 0)
	    {
		if (x >= ecal_map_element[imin].x) ixhalf = 1;
		if (y >= ecal_map_element[imin].y) iyhalf = 1;
		number[imin] = ncells;
		channel0[imin] = ecal_map_element[imin].chan;
		channel1[ncells] = ecal_map_element[imin].chan;
		index[ncells] = imin;
		xnear[0] = ecal_map_element[index[ncells]].x;
		ynear[0] = ecal_map_element[index[ncells]].y;
		ncells++;
	    }
	    else
	    {
		printf("Invalid beam entry point (%f,%f)!\n", x, y);
		return 1;
	    }
	}
	for(int i=1; i<clsize; i++)
	{
	    xnear[i] = ecal_map_element[index[0]].x + ixhalf*cell_size[ecal_map_element[index[0]].size];
	    ixhalf = -ixhalf;
	}
	for(int i=1; i<clsize; i++)
	{
	    ynear[i] = ecal_map_element[index[0]].y + iyhalf*cell_size[ecal_map_element[index[0]].size];
	    iyhalf = -iyhalf;
	}

	for(int i=0; i<clsize; i++)
	{
	    for(int j=0; j<clsize; j++)
	    {
    		for(int ind=0;ind<n_rec;ind++)
		{
		    if (ind == i0) continue;
		    if (xnear[i] >= ecal_map_element[ind].x - cell_size[ecal_map_element[ind].size]/2. && \
	    		xnear[i] <  ecal_map_element[ind].x + cell_size[ecal_map_element[ind].size]/2. && \
	    		ynear[j] >= ecal_map_element[ind].y - cell_size[ecal_map_element[ind].size]/2. && \
	    		ynear[j] <  ecal_map_element[ind].y + cell_size[ecal_map_element[ind].size]/2 )
		    {
			number[ind] = ncells;
			channel0[ind] = ecal_map_element[ind].chan;
			channel1[ncells] = ecal_map_element[ind].chan;
			index[ncells] = ind;
			ncells++;
//			printf(" ncells %d cell %d\n", ncells, ecal_map_element[ind].chan+1);
		    }
		}
	    }
	}

	for(int j=0; j<MAX_ECAL_EVENTS; j++)
	{
    	    for(int i=0;i<ncells;i++)
	    {
		amp_array_ecal[j][i] = 0.;
		profile_amp_ecal[j][i] = 0.;
		profile_err_ecal[j][i] = 1.;
	    }
	}
//	drawecal();
//	hsum_raw = new TH1F("hsumraw","Sum of raw amplitudes", 1000, 0., 100000.);
//	hsum     = new TH1F("hsumcal","Sum of calibrated amplitudes", 1000, 0., 100000.);
//	hsum->Draw();
	if (data == NULL)
	{
	    for(int i=0; i<ncells; i++)
	    {
		coef[i] = (0.5*((i+1)%4) + 2.4)/10.;
	    }
	}
    }
    float ped = 0.;
    float sum_raw =0., sum_sim = 0.;
    double el = 0.;
    if (data != NULL)
    {
     for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnADCDigit *digit = (BmnADCDigit*) data->At(i);
       int ind, num; 
       for(ind=0;ind<n_rec;ind++) if((digit->GetSerial())==ecal_map_element[ind].id && digit->GetChannel()==(ecal_map_element[ind].adc_chan)) break;
       if(ind==n_rec) continue; 
       if(ecal_map_element[ind].used==0) continue;
       if((num=number[ind])<0) continue;
       if ((amp = wave2amp(digit->GetNSamples(),digit->GetUShortValue(), &ped)) >= 0.)
       {
//	    printf("chan %d amp %f coef %f\n", ecal_map_element[ind].chan, amp, cal[ecal_map_element[ind].chan]);
	    amp_array_ecal[nevents_ecal][num] = amp*cal[ecal_map_element[ind].chan];
	    pedestals_ecal[nevents_ecal][num] = ped;
	    sum_raw += amp_array_ecal[nevents_ecal][num];
       }
     }
     for (int i = 0; i < ncells; i++) {
	    dx = x - ecal_map_element[index[i]].x;
	    dy = y - ecal_map_element[index[i]].y;
	    r = TMath::Sqrt(dx*dx + dy*dy);
	    el = shower(r, cell_size[ecal_map_element[index[i]].size]);
	    sum_sim += el;
     }
    }
    else
    {
     for (int i = 0; i < ncells; i++) {
	    dx = x - ecal_map_element[index[i]].x;
	    dy = y - ecal_map_element[index[i]].y;
	    r = TMath::Sqrt(dx*dx + dy*dy);
	    el = shower(r, cell_size[ecal_map_element[index[i]].size]);
	    sum_sim += el;
	    el /= coef[i];
	    amp = el+gRandom->Gaus()*sigma_amp;
	    amp_array_ecal[nevents_ecal][i] = amp*cal[ecal_map_element[index[i]].chan];
	    pedestals_ecal[nevents_ecal][i] = ped;
	    sum_raw += amp_array_ecal[nevents_ecal][i];
     }
    }
    hsum_raw->Fill(sum_raw);
    hsum_sim->Fill(sum_sim);
    cellWeight(nevents_ecal);
    nevents_ecal++;
    return 0;
}

int BmnECALRaw2Digit::fillCalibrateNumbers(TClonesArray *data, Float_t x, Float_t y, Float_t e, Int_t nchan, Int_t *cells) {
    for (int i=0; i<MAX_ECAL_CHANNELS; i++) ecal_amp[i] = -1.;
    for (int i=0; i<MAX_ECAL_LOG_CHANNELS; i++) log_amp[i] = -1.;
    Float_t amp = 0;
    double r = 0., dx = 0., dy = 0.;
    if (nevents_ecal >= MAX_ECAL_EVENTS) return 1;
    if (nevents_ecal == 0)
    {
	ncells = nchan;
	x_beam = x;
	y_beam = y;
	int i0 = -1;
        for(int ind=0;ind<n_rec;ind++)
	{
	    if (x >= ecal_map_element[ind].x - cell_size[ecal_map_element[ind].size]/2. && \
	        x <  ecal_map_element[ind].x + cell_size[ecal_map_element[ind].size]/2. && \
	        y >= ecal_map_element[ind].y - cell_size[ecal_map_element[ind].size]/2. && \
	        y <  ecal_map_element[ind].y + cell_size[ecal_map_element[ind].size]/2 )
	    {
		i0 = ind;
		break;
	    }
	}
	if (i0 == -1)
	{
	    if (x<x_min || x>x_max || y<y_min || y>y_max)
	    {
		printf("Warning: Beam entry point (%f,%f) outside ECAL area!\n", x, y);
//		return 1;
	    }
	}
	shower_energy = e;
        for(int i=0;i<ncells;i++)
	{
	    channel1[i] = -1;
	}
        for(int ind=0;ind<n_rec;ind++)
	{
	    number[ind] = -1;
	    channel0[ind] = -1;
	    for (int j=0; j<ncells; j++)
	    {
		if ((cells[j]-1) == ecal_map_element[ind].chan)
		{
		    number[ind] = j;
		    channel0[ind] = ecal_map_element[ind].chan;
		    channel1[j] = ecal_map_element[ind].chan;
		    index[j] = ind;
		    break;
		}
	    }
	}
	for(int j=0; j<MAX_ECAL_EVENTS; j++)
	{
    	    for(int i=0;i<ncells; i++)
	    {
		amp_array_ecal[j][i] = 0.;
		profile_amp_ecal[j][i] = 0.;
		profile_err_ecal[j][i] = 1.;
	    }
	}
    }
    float ped = 0.;
    float sum_raw =0., sum_sim = 0.;
    double el = 0.;
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnADCDigit *digit = (BmnADCDigit*) data->At(i);
       int ind, num; 
       for(ind=0;ind<n_rec;ind++) if((digit->GetSerial())==ecal_map_element[ind].id && digit->GetChannel()==(ecal_map_element[ind].adc_chan)) break;
       if(ind==n_rec) continue; 
       if(ecal_map_element[ind].used==0) continue;
       if((num=number[ind])<0) continue;
       if ((amp = wave2amp(digit->GetNSamples(),digit->GetUShortValue(), &ped)) >= 0.)
       {
	    amp_array_ecal[nevents_ecal][num] = amp*cal[ecal_map_element[ind].chan];
	    pedestals_ecal[nevents_ecal][num] = ped;
	    sum_raw += amp_array_ecal[nevents_ecal][num];
       }
    }
    for (int i = 0; i < ncells; i++) {
	    dx = x - ecal_map_element[index[i]].x;
	    dy = y - ecal_map_element[index[i]].y;
	    r = TMath::Sqrt(dx*dx + dy*dy);
	    el = shower(r, cell_size[ecal_map_element[index[i]].size]);
	    sum_sim += el;
    }
    hsum_raw->Fill(sum_raw);
    hsum_sim->Fill(sum_sim);
    cellWeight(nevents_ecal);
    nevents_ecal++;
    return 0;
}

int BmnECALRaw2Digit::fillCalibrateAll(TClonesArray *data, Float_t x, Float_t y, Float_t e) {
    for (int i=0; i<MAX_ECAL_CHANNELS; i++) ecal_amp[i] = -1.;
    for (int i=0; i<MAX_ECAL_LOG_CHANNELS; i++) log_amp[i] = -1.;
    Float_t amp = 0;
    Int_t j = 0;
    if (nevents_ecal >= MAX_ECAL_EVENTS) return 1;
    if (nevents_ecal == 0)
    {
	x_beam = x;
	y_beam = y;
	int i0 = -1;
        for(int ind=0;ind<n_rec;ind++)
	{
	    if (x >= ecal_map_element[ind].x - cell_size[ecal_map_element[ind].size]/2. && \
	        x <  ecal_map_element[ind].x + cell_size[ecal_map_element[ind].size]/2. && \
	        y >= ecal_map_element[ind].y - cell_size[ecal_map_element[ind].size]/2. && \
	        y <  ecal_map_element[ind].y + cell_size[ecal_map_element[ind].size]/2 )
	    {
		i0 = ind;
		break;
	    }
	}
	if (i0 == -1)
	{
	    printf("Warning: Beam entry point (%f,%f) outside any ECAL module!\n", x, y);
//	    return 1;
	}
	ncells = n_rec;
	shower_energy = e;
        for(int i=0;i<n_rec;i++)
	{
	    channel1[i] = -1;
	}
        for(int ind=0;ind<n_rec;ind++)
	{
		    number[ind] = j;
		    channel0[ind] = ecal_map_element[ind].chan;
		    channel1[j] = ecal_map_element[ind].chan;
		    j++;
	}
	for(int j1=0; j1<MAX_ECAL_EVENTS; j1++)
	{
    	    for(int i=0;i<n_rec; i++)
	    {
		amp_array_ecal[j1][i] = 0.;
		profile_amp_ecal[j1][i] = 0.;
		profile_err_ecal[j1][i] = 1.;
	    }
	}
    }
    float ped = 0.;
    for (int i = 0; i < data->GetEntriesFast(); i++) {
       BmnADCDigit *digit = (BmnADCDigit*) data->At(i);
       int ind, num; 
       for(ind=0;ind<n_rec;ind++) if((digit->GetSerial())==ecal_map_element[ind].id && digit->GetChannel()==(ecal_map_element[ind].adc_chan)) break;
       if(ind==n_rec) continue; 
       if(ecal_map_element[ind].used==0) continue;
       if((num=number[ind])<0) continue;
       if ((amp = wave2amp(digit->GetNSamples(),digit->GetUShortValue(), &ped)) >= 0.)
       {
	    amp_array_ecal[nevents_ecal][num] = amp*cal[ecal_map_element[ind].chan];
	    pedestals_ecal[nevents_ecal][num] = ped;
       }
    }
    cellWeight(nevents_ecal);
    nevents_ecal++;
    return 0;
}

void BmnECALRaw2Digit::cellWeight(int ievent)
{
    int n;
    double s = 0., e = 0., r = 0., x0 = 0., y0 = 0.;
    if (use_meanxy)
    {
	for (int i=0; i<ncells; i++)
	{
	    x0 += ecal_map_element[index[i]].x*amp_array_ecal[ievent][i];
	    y0 += ecal_map_element[index[i]].y*amp_array_ecal[ievent][i];
	    s += amp_array_ecal[ievent][i];
	}
	x0 /= s;
	y0 /= s;
    }
    else
    {
	x0 = x_beam;
	y0 = y_beam;
    }
    for (int i=0; i<ncells; i++)
    {
	r = TMath::Sqrt((ecal_map_element[index[i]].x-x0)*(ecal_map_element[index[i]].x-x0)+(ecal_map_element[index[i]].y-y0)*(ecal_map_element[index[i]].y-y0));
	e = shower(r, cell_size[ecal_map_element[index[i]].size]);
	profile_amp_ecal[ievent][i] = e;
	profile_err_ecal[ievent][i] = sigma_amp*sigma_amp;
    }
}

void BmnECALRaw2Digit::calibrate() {
   // initialize TMinuit with a maximum of 5 params

   Double_t vstart[MAX_ECAL_CHANNELS], step[MAX_ECAL_CHANNELS], vmin = 0.00001, vmax = 100000.;

//   printf(" ncells = %d\n", ncells);
   TMinuit *gMinuit1 = new TMinuit(ncells+1);  
   gMinuit1->SetFCN(fcn1);
 
   Double_t arglist[MAX_ECAL_CHANNELS];
   Int_t ierflg = 0;
 
   arglist[0] = 1;
   gMinuit1->mnexcm("SET ERR", arglist ,1,ierflg);
 
// Set starting values and step sizes for parameters
   char name[32] = {""};
//   printf(" ncells1 = %d\n", ncells);
   for (int i=0; i<ncells; i++)
   {
    vstart[i] = pstart;
    step[i] = pstep;
    sprintf(name, "Coeff_%03d", channel1[i]+1);
    gMinuit1->mnparm(i, name, vstart[i], step[i], vmin, vmax, ierflg);
//    printf("%d name %s min %f max %f start %f step %f\n",i,name,vmin,vmax,vstart[i],step[i]);
   } 
// Now ready for minimization step
   arglist[0] = 50000;
   arglist[1] = 0.1;
   gMinuit1->mnexcm("MIGRAD", arglist ,2, ierflg);
 
// Print results
   Double_t amin,edm,errdef;
   Int_t nvpar,nparx,icstat;
   gMinuit1->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
   gMinuit1->mnprin(3,amin);

//----- write results

    for (int i=0; i<maxchan; i++)
    {
	cal_out[i] = cal[i];
	cale_out[i] = cale[i];
    }
    Double_t par[MAX_ECAL_CHANNELS], epar[MAX_ECAL_CHANNELS];
    for (int i=0; i<ncells; i++)
    {
	gMinuit1->GetParameter(i,par[i],epar[i]);
	cal_out[channel1[i]] = cal[channel1[i]]*par[i];
	cale_out[channel1[i]] = cal[channel1[i]]*epar[i];
//	printf("   %d par %f epar %f\n",i,par[i],epar[i]);
    }
    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/parameters/ecal/";
    FILE *fout = 0;
    char filn[128], tit1[32] = {"Channel"}, tit2[32] = {"Calibration"}, tit3[32] = {"Error"};
    sprintf(filn, "%s%s_calibration_out.txt", path.Data(), filname_base);
    fout = fopen(filn,"w");
    if (!fout)
    {
	printf("Can't open output calibration file %s\n", filn);
    }
    else
    {
	fprintf(fout, "%s\t%s\t%s\n", tit1, tit2, tit3);
	for (int i=0; i<maxchan; i++)
	{
	    fprintf(fout, "%3d\t%f\t%f\n", i+1, cal_out[i], cale_out[i]);
	}
	fclose(fout);
    }
    printf("%s\t%s\t%s\n", tit1, tit2, tit3);
    for (int i=0; i<maxchan; i++)
    {
	printf("%3d\t%f\t%f\n", i+1, cal_out[i], cale_out[i]);
    }
//-------
    float sum = 0.;
    for (int j=0; j<nevents_ecal; j++)
    {
	sum = 0.;
	for (int i=0; i<ncells; i++)
	{
//	    sum += amp_array_ecal[j][i]*cal_out[channel1[i]];
	    sum += amp_array_ecal[j][i]*par[i];
	}
	hsum->Fill(sum);
    }
//    hsum->Draw();
//    drawecal();
}

static void fcn1(Int_t& npar, Double_t *gin, Double_t& f, Double_t *par, Int_t iflag)
{
    double chi = 0., le = 0., laA = 0.;
    for (int j=0; j<nevents_ecal; j++)
    {
	for (int i=0; i<npar; i++)
	{
//	    if (j == nevents_ecal-1) printf(" nevents %d par %f prof %f eprof %f amp %f\n", nevents_ecal, par[i], profile_amp_ecal[j][i], profile_err_ecal[j][i], amp_array_ecal[j][i]);
	    if (use_log_function_ecal)
	    {
		le = TMath::Log10(profile_amp_ecal[j][i]);
		laA = TMath::Log10(par[i]*amp_array_ecal[j][i]);
	    }
	    else
	    {
		le = profile_amp_ecal[j][i];
		laA = par[i]*amp_array_ecal[j][i];
	    }
	    chi += (le - laA)*(le - laA)/profile_err_ecal[j][i];
	}
    }
    f = chi;
    return;
}

double BmnECALRaw2Digit::ch(double x, double r){ double amp = ( exp(x/r) + exp(-x/r) )/2.; return amp;}
double BmnECALRaw2Digit::sh(double x, double r){ double amp = ( exp(x/r) - exp(-x/r) )/2.; return amp;}

//-----------------------------------------------PP1 
double BmnECALRaw2Digit::PP1(double x, double h)
{//Shower profile in cell with size h  when R <= h/2

double a1,a2,b1,b2,B;
double ss1=1.;
double bb2=6.*ss1;
double bb1=1.4*ss1;

double E = shower_energy; // total hadron energy , GeV

a2=0.105 - 0.014*log(E);   //R -dependences
b1=bb1   - 0.120*log(E);   //R -dependences
b2=bb2   - 0.260*log(E);   //R -dependences

a1 = 1. - a2;
B = (a1*b1+a2*b2);
double  fff=0.;
if(x <= h/2) fff=(a1*b1*(1.-exp(-h/(2.*b1))*ch(x,b1)) + a2*b2*(1.-exp(-h/(2.*b2))*ch(x,b2 )))/(h*B);
return fff;
}

//---------------------------------------- PP2 
double BmnECALRaw2Digit::PP2(double x, double h)
{//Shower profile in cell with size h when R > h/2

double a1,a2,b1,b2,B;
double ss1=1.;
double bb2=6.*ss1;
double bb1=1.4*ss1;

double E = shower_energy; // total hadron energy , GeV

a2 = 0.105 - 0.014*log(E); // R -dependences
b1 = bb1   - 0.120*log(E); // R -dependences
b2 = bb2   - 0.260*log(E); // R -dependences

a1 = 1. - a2;
B = (a1*b1 + a2*b2);
double fff = 0.;
if(x > h/2) fff = (a1*b1*exp(-x/b1)*sh(h,2.*b1) )/(h*B) + (a2*b2*exp(-x/b2)*sh(h,2.*b2 ) )/(h*B);
return fff;
}

double BmnECALRaw2Digit::shower(double x, double h){double amp = PP1(x/10.,h/10.) + PP2(x/10.,h/10.); return shower_energy*shower_norm*amp;}

float BmnECALRaw2Digit::wave2amp(UChar_t ns, UShort_t *s, Float_t *pedestal)
{
	    float pedest = 0., ampl = 0., signal = 0., signal_max = 0.;
	    int nsignal = 0, nsignal_max = 0, ismax = -1, m1 = 0;
	    short si = 0;
	    float ampl_max = 0.;
	    if (ns > 0)
	    {
		for (int m = 0; m < ns; m++)
		{
//		    m1 = 1 - (m%2) + (m/2)*2;
		    m1 = m;
		    si = (short)s[m1];
		    if (m < ped_samples)
		    {
			pedest += (si>>SHIFT);
			if (m == (ped_samples-1))
			{
			    pedest /= ped_samples;
			}
			continue;
		    }
		    else
		    {
			ampl = fabs((float)(si>>SHIFT) - pedest);
			if (ampl > thres)
			{
			    signal += ampl;
			    nsignal++;
			    if (ampl > ampl_max)
			    {
				ampl_max = ampl;
				ismax = m;
			    }
			}
			else
			{
			    if (nsignal < min_samples)
			    {
				signal = 0.;
				nsignal = 0;
			    }
			    else if (nsignal > nsignal_max)
			    {
				signal_max = signal;
				nsignal_max = nsignal;
			    }
			}
		    }
		} // loop over samples
//		printf("Chan %d Amplmax %f Ped %f imax %d nsam %d\n",l,ampl_max,pedest[l],ismax,nsamples[l]);
		if (nsignal_max > 0 || ampl_max > 0.)
		{
		  if (wave2amp_flag) signal_max = ampl_max;
		}
		else
		{
		  signal_max = 0.;
		}
	    } // if samples exist
	    else
	    {
		signal_max = -1.;
	    }
	    *pedestal = pedest;
	    return signal_max;
}

float BmnECALRaw2Digit::testwave2amp(UChar_t ns, UShort_t *s, Float_t *pedestal)
{
	    float pedest = 0., ampl = 0., signal = 0., signal_max = 0.;
	    int nsignal = 0, nsignal_max = 0, ismax = -1, m1 = 0;
	    short si = 0;
	    float ampl_max = 0.;
	    if (ns > 0)
	    {
		for (int m = 0; m < ns; m++)
		{
//		    m1 = 1 - (m%2) + (m/2)*2;
		    m1 = m;
		    si = (short)s[m1];
		    if (m < ped_samples)
		    {
			pedest += si>>SHIFT;
			if (m == (ped_samples-1))
			{
			    pedest /= ped_samples;
			}
			continue;
		    }
		    else
		    {
			ampl = fabs((float)(si>>SHIFT) - pedest);
			if (ampl > 0)
			{
			    signal += ampl;
			    nsignal++;
			    if (ampl > ampl_max)
			    {
				ampl_max = ampl;
				ismax = m;
			    }
			}
			else
			{
			    if (nsignal < min_samples)
			    {
				signal = 0.;
				nsignal = 0;
			    }
			    else if (nsignal > nsignal_max)
			    {
				signal_max = signal;
				nsignal_max = nsignal;
			    }
			}
		    }
		} // loop over samples
//		printf("Chan %d Amplmax %f Ped %f imax %d nsam %d\n",l,ampl_max,pedest[l],ismax,nsamples[l]);
		if (nsignal_max > 0 || ampl_max > 0.)
		{
		  if (wave2amp_flag) signal_max = ampl_max;
		}
		else
		{
		  signal_max = 0.;
		}
	    } // if samples exist
	    else
	    {
		signal_max = -1.;
	    }
	    *pedestal = pedest;
	    return signal_max;
}

void BmnECALRaw2Digit::drawecal(int nohist)
{
  TCanvas *callbe0 = new TCanvas("callbe0", "ECAL mapping", 800,800);
  gPad->Range(x_min-10.,y_min-10.,x_max+10.,y_max+10.);
  int i;
  char text[16];
  double x1,x2,y1,y2;
  TBox *b[MAX_ECAL_CHANNELS] = {0};
  TText *t[MAX_ECAL_CHANNELS] = {0};
  TText *tc[MAX_ECAL_CHANNELS] = {0};
  callbe0->cd();
  for (i=0; i<n_rec; i++)
    {
      x1 = ecal_map_element[i].x - cell_size[ecal_map_element[i].size]/2.;
      x2 = ecal_map_element[i].x + cell_size[ecal_map_element[i].size]/2.;
      y1 = ecal_map_element[i].y - cell_size[ecal_map_element[i].size]/2.;
      y2 = ecal_map_element[i].y + cell_size[ecal_map_element[i].size]/2.;
//      printf("%f %f %f %f\n",x1,y1,x2,y2);
      b[i] = new TBox(x1,y1,x2,y2);
      b[i]->Draw();
      if (ncells > 0 && number[i] >= 0) b[i]->SetLineColor(kRed);
      b[i]->SetFillStyle(0);
      b[i]->SetLineStyle(0);
      sprintf(text,"%d",ecal_map_element[i].chan+1);
      t[i] = new TText(ecal_map_element[i].x,ecal_map_element[i].y,text);
      t[i]->Draw();
      if (ncells > 0 && number[i] >= 0)
      {
	t[i]->SetTextColor(kRed);
	t[i]->SetTextAlign(21);
      }
      else
      {
	t[i]->SetTextAlign(21);
      }
      t[i]->SetTextSize(0.02);
//      if (ncells > 0 && number[i] >= 0)
//      {
//        sprintf(text,"%6.3f",cal_out[channel0[i]]);
        sprintf(text,"%6.3f",cal_out[ecal_map_element[i].chan]);
        tc[i] = new TText(ecal_map_element[i].x,ecal_map_element[i].y,text);
        tc[i]->Draw();
        if (ncells > 0 && number[i] >= 0) tc[i]->SetTextColor(kRed);
        tc[i]->SetTextAlign(23);
        tc[i]->SetTextSize(0.02);
//      }
    }

  if ((hsum_raw == NULL && hsum == NULL)||nohist) return;

  TCanvas *calres = new TCanvas("calres", "ECAL calibration", 800,800);
  calres->cd();
  calres->Divide(1,3);
  calres->cd(1);
  if (hsum_raw) hsum_raw->Draw();
  gPad->AddExec("exselt","select_hist()");
  calres->cd(2);
  if (hsum) hsum->Draw();
  gPad->AddExec("exselt","select_hist()");
  calres->cd(3);
  if (hampl) hampl->Draw();
  gPad->AddExec("exselt","select_hist()");
  return;
}

void BmnECALRaw2Digit::drawprof(int achan)
{
  TCanvas *csampro = new TCanvas("csampro", "ECAL sample profiles", 800,800);
  csampro->cd();
  if (achan == 0)
  {
    if (ncells <= 4) csampro->Divide(2,2);
    else  if (ncells <= 9) csampro->Divide(3,3);
    else  if (ncells <= 16) csampro->Divide(4,4);
    else  if (ncells <= 25) csampro->Divide(5,5);
    else  if (ncells <= 36) csampro->Divide(6,6);
    else  if (ncells <= 49) csampro->Divide(7,7);
    else  if (ncells <= 64) csampro->Divide(8,8);
    else  if (ncells <= 81) csampro->Divide(9,9);
    else  if (ncells <= 100) csampro->Divide(10,10);
    else  if (ncells <= 121) csampro->Divide(11,11);
    else  csampro->Divide(30,12);
    for (int i=0; i<ncells; i++)
    {
	csampro->cd(i+1);
	if (SampleProf[i]) SampleProf[i]->Draw();
	gPad->AddExec("exselt","select_hist()");
    }
  }
  else
  {
    if (SampleProf[achan-1]) SampleProf[achan-1]->Draw();
    gPad->AddExec("exselt","select_hist()");
  }
  TCanvas *calres = new TCanvas("calres1e", "ECAL mean X and Y", 800,800);
  calres->cd();
  calres->Divide(1,3);
  calres->cd(1);
  if (hxmean) hxmean->Draw();
  gPad->AddExec("exselt","select_hist()");
  calres->cd(2);
  if (hymean) hymean->Draw();
  gPad->AddExec("exselt","select_hist()");
  calres->cd(3);
  if (hampl) hampl->Draw();
  gPad->AddExec("exselt","select_hist()");
//  hsum_raw->Print();
  return;
}

void BmnECALRaw2Digit::drawtest()
{
  if (htest[0] == NULL) return;
  TCanvas *ctest = new TCanvas("ctest", "ECAL test channel", 800,800);
  ctest->cd();
  ctest->Divide(2,n_test);
  for (int i = 0; i<n_test; i++)
  {
   ctest->cd(i*2+1);
   htest[i]->Draw();
   ctest->cd(i*2+2);
   TestProf[i]->Draw();
   gPad->AddExec("exselt","select_hist()");
  }
  return;
}

ClassImp(BmnECALRaw2Digit)

