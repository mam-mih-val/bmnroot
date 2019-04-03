using namespace std;

void Raw2Digit(char *fname="bmn_run1421.root", int RunPeriod = 6) {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    /////////////////////////////////////////////////////////////////////////////////////
    int RUN;
    if (sscanf(&fname[strlen(fname) - 9], "%d", &RUN) != 1)
    {
	if (sscanf(&fname[strlen(fname) - 8], "%d", &RUN) != 1)
	{
	    printf("Can't obtain run number from file %s\n", fname);
	    return;
	}
    };
    char mapping[256];
    char mappingzdc[256];

    if(RunPeriod >= 1 && RunPeriod <= 6)
    {
	sprintf(mapping, "TOF700_map_period_%d.txt", RunPeriod);
	sprintf(mappingzdc, "ZDC_map_period_%d.txt", RunPeriod);
    }
    else
    {
	printf("Non-existing run period number %d!", RunPeriod);
	return;
    }
    BmnZDCRaw2Digit  ZDC(mappingzdc, fname, "zdc_muon_calibration.txt"); //ZDC.print();
    BmnTof2Raw2Digit TOF2(mapping, fname); //TOF2.print();
    TOF2.readSlewingT0();
    TOF2.readSlewing();

    if (RunPeriod <= 2)
    {
//	TOF2.SetW(1700,3700);
	TOF2.SetWT0(260,560);
//	TOF2.SetLeadMinMax(1,-400, -250);
//	TOF2.SetLeadMinMax(2,-300, -150);
//	TOF2.SetLeadMinMax(3,-400, -50);
//	TOF2.SetLeadMinMax(4,-120, +120);
    }
    else if (RunPeriod == 3)
    {
//	TOF2.SetW(1700,3700);
	TOF2.SetWT0(640,710);
//	TOF2.SetLeadMinMax(1,-350, -150);
//	TOF2.SetLeadMinMax(2,-350, -150);
//	TOF2.SetLeadMinMax(3,-350, +50);
//	TOF2.SetLeadMinMax(4,-200, +200);
    }
    else if (RunPeriod == 5)
    {
//	TOF2.SetW(2500,3300); // run 834...
	TOF2.SetWT0(400,600); // run 834...
//	for (int i=1; i<=15; i++) if (!(i==1||i==4||i==7||i==10||i==13)) TOF2.SetLeadMinMax(i,-5200, -4800);  // run 834
//	for (int i=1; i<=15; i++) if (  i==1||i==4||i==7||i==10||i==13)  TOF2.SetLeadMinMax(i,-13200, -12800); // run 834
    }
    else if (RunPeriod == 6)
    {
//	TOF2.SetW(2500,4000);
	TOF2.SetWT0(720,860);
//	for (int i=1; i<=24; i++) if (!(i==22||i==15||i==16||i==17||i==18||i==19||i==20||i==10)) TOF2.SetLeadMinMax(i,2850, 2950);  // run 1209
//	for (int i=1; i<=24; i++) if (  i==22||i==15||i==16||i==17||i==18||i==19||i==20||i==10)  TOF2.SetLeadMinMax(i,-5970, -5800); // run 1209
//	for (int i=1; i<=24; i++) if (!(i==22||i==15||i==16||i==17||i==18||i==19||i==20||i==10)) TOF2.SetLeadMinMax(i,2650, 2950);  // run 1227
//	for (int i=1; i<=24; i++) if (  i==22||i==15||i==16||i==17||i==18||i==19||i==20||i==10)  TOF2.SetLeadMinMax(i,-5600, -5400); // run 1227
    }

    cout << "Process RUN:  " << RUN << endl;

    /////////////////////////////////////////////////////////////////////////////////////
    UInt_t TRIGWORD = 0;
    TFile *_f_in = new TFile(fname, "READ");
    TTree *_t_in = (TTree *) _f_in->Get("BMN_RAW");
    TClonesArray *t0_raw   = new TClonesArray("BmnTDCDigit");
    TClonesArray *sync_raw = new TClonesArray("BmnSyncDigit");
    TClonesArray *tof2_raw = new TClonesArray("BmnTDCDigit");
    TClonesArray *zdc_raw  = new TClonesArray("BmnADCDigit");
    _t_in->SetBranchAddress("bmn_t0",    &t0_raw);
    _t_in->SetBranchAddress("bmn_sync",  &sync_raw);
    _t_in->SetBranchAddress("bmn_tof700",&tof2_raw);
    _t_in->SetBranchAddress("bmn_zdc",   &zdc_raw);
    _t_in->SetBranchAddress("bmn_trigword",   &TRIGWORD);
    /////////////////////////////////////////////////////////////////////////////////////
    long long EVENT = 0,TIME_SEC = 0,TIME_NS = 0;
    float T0;
    char str[100]; sprintf(str,"bmn_run%04d_digit.root",RUN); 
    TFile *_f_out = new TFile(str, "RECREATE");
    _f_out->SetCompressionLevel(1);
    TTree *_t_out = new TTree("BMN_DIGIT","test_bmn");
    TClonesArray * zdc_digit   = new TClonesArray("BmnZDCDigit");
    TClonesArray * tof2_digit  = new TClonesArray("BmnTof2Digit");
    _t_out->Branch("bmn_run",         &RUN,     "bmn_run/I");   
    _t_out->Branch("bmn_event",       &EVENT,   "bmn_event/I");   
    _t_out->Branch("bmn_trigword",    &TRIGWORD,"bmn_trigword/i");   
    _t_out->Branch("bmn_time_sec",    &TIME_SEC,"bmn_time_sec/I");   
    _t_out->Branch("bmn_time_ns",     &TIME_NS, "bmn_time_ns/I");   
    _t_out->Branch("bmn_t0",          &T0,      "bmn_t0/F");   
    _t_out->Branch("bmn_zdc_digit",   &zdc_digit);   
    _t_out->Branch("bmn_tof2_digit",  &tof2_digit);   
    /////////////////////////////////////////////////////////////////////////////////////

    for (int ev = 0; ev < _t_in->GetEntries(); ev++) {

        sync_raw->Clear();
	t0_raw->Clear();
	tof2_raw->Clear();
	zdc_raw->Clear();

        zdc_digit->Clear();  
        tof2_digit->Clear();  
	TRIGWORD = 0;
 
        _t_in->GetEntry(ev);

        TOF2.fillEvent(tof2_raw, sync_raw, t0_raw, tof2_digit);
        TOF2.getEventInfo(&EVENT,&TIME_SEC,&TIME_NS);
        ZDC.fillEvent(zdc_raw, zdc_digit);
        T0 = TOF2.get_t0();
        if ((ev % 1000) == 0) printf("Digits producing, event %d, tof2_raw hits %d, tof2 digits %d, zdc_raw hits %d, zdc digits %d\n", ev, tof2_raw->GetEntries(), tof2_digit->GetEntries(), zdc_raw->GetEntries(), zdc_digit->GetEntries());
//        if (T0 == 0) continue;
        _t_out->Fill();
    }
    /////////////////////////////////////////////////////////////////////////////////////
    _f_in->Close();
    _t_out->Print();
    _t_out->Write(); 
    _f_out->Write(); 
    _f_out->Close();
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
