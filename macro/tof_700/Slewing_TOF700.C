
using namespace std;

void Slewing_TOF700(char *fname="../raw/bmn_run1421.root", int RunPeriod = 6) {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    /////////////////////////////////////////////////////////////////////////////////////

    char mapping[256];
    if(RunPeriod >= 1 && RunPeriod <= 6)
    {
	sprintf(mapping, "TOF700_map_period_%d.txt", RunPeriod);
    }
    else
    {
	printf("Non-existing run period number %d!", RunPeriod);
	return;
    }

    BmnTof2Raw2Digit TOF2(mapping, fname);
    TOF2.print();

    if (RunPeriod <= 2)
    {
	TOF2.SetW(1700,3700);
	TOF2.SetWT0(260,560);
	TOF2.SetLeadMinMax(1,-400, -250);
	TOF2.SetLeadMinMax(2,-300, -150);
	TOF2.SetLeadMinMax(3,-400, -50);
	TOF2.SetLeadMinMax(4,-120, +120);
    }
    else if (RunPeriod == 3)
    {
	TOF2.SetW(1700,3700);
	TOF2.SetWT0(640,710);
	TOF2.SetLeadMinMax(1,-350, -150);
	TOF2.SetLeadMinMax(2,-350, -150);
	TOF2.SetLeadMinMax(3,-350, +50);
	TOF2.SetLeadMinMax(4,-200, +200);
    }
    else if (RunPeriod == 5)
    {
//	TOF2.SetW(2500,3300); // run 834...
//	TOF2.SetWT0(400,600); // run 834...
//	TOF2.SetW(2500,4500); // run 985...
//	TOF2.SetWT0(450,750); // run 985...
//	for (int i=1; i<=15; i++) if (!(i==1||i==4||i==7||i==10||i==13)) TOF2.SetLeadMinMax(i,-5200, -4800);  // run 834
//	for (int i=1; i<=15; i++) if (  i==1||i==4||i==7||i==10||i==13)  TOF2.SetLeadMinMax(i,-13200, -12800); // run 834
//	for (int i=1; i<=15; i++) TOF2.SetLeadMinMax(i,-14000, -12000);  // run 985
//	TOF2.SetLeadMinMax(10,-13000, -12875); // run 985
    }
    else if (RunPeriod == 6)
    {
	TOF2.SetW(2500,4000);
	TOF2.SetWT0(720,860);
	for (int i=1; i<=24; i++) if (!(i==22||i==15||i==16||i==17||i==18||i==19||i==20||i==10)) TOF2.SetLeadMinMax(i,2850, 2950);  // run 1209
	for (int i=1; i<=24; i++) if (  i==22||i==15||i==16||i==17||i==18||i==19||i==20||i==10)  TOF2.SetLeadMinMax(i,-5970, -5800); // run 1209
//	for (int i=1; i<=24; i++) if (!(i==22||i==15||i==16||i==17||i==18||i==19||i==20||i==10)) TOF2.SetLeadMinMax(i,2650, 2950);  // run 1227
//	for (int i=1; i<=24; i++) if (  i==22||i==15||i==16||i==17||i==18||i==19||i==20||i==10)  TOF2.SetLeadMinMax(i,-5600, -5400); // run 1227
    }

    TOF2.readSlewingLimits();

    cout << "Process RUN file:  " << fname << endl;

    /////////////////////////////////////////////////////////////////////////////////////
    UInt_t TRIGWORD = 0;
    TFile *_f_in = new TFile(fname, "READ");
    TTree *_t_in = (TTree *) _f_in->Get("BMN_RAW");
    TClonesArray *t0_raw   = new TClonesArray("BmnTDCDigit");
    TClonesArray *sync_raw = new TClonesArray("BmnSyncDigit");
    TClonesArray *tof2_raw = new TClonesArray("BmnTDCDigit");
    _t_in->SetBranchAddress("bmn_t0",    &t0_raw);
    _t_in->SetBranchAddress("bmn_sync",  &sync_raw);
    _t_in->SetBranchAddress("bmn_tof700",&tof2_raw);
    _t_in->SetBranchAddress("bmn_trigword",   &TRIGWORD);
    /////////////////////////////////////////////////////////////////////////////////////


    for (int ev = 0; ev < _t_in->GetEntries(); ev++) {

        if ((ev % 10000) == 0) printf("Slewing T0, event %d\n", ev);

        sync_raw->Clear();
	t0_raw->Clear();
	tof2_raw->Clear();

        _t_in->GetEntry(ev);

	if ((TRIGWORD&0x8) != 0) continue;

        TOF2.fillSlewingT0(tof2_raw, sync_raw, t0_raw);

    }
    /////////////////////////////////////////////////////////////////////////////////////
   
    TOF2.SlewingT0();

    TOF2.readSlewingT0();

    /////////////////////////////////////////////////////////////////////////////////////

    for (int ev = 0; ev < _t_in->GetEntries(); ev++) {

        if ((ev % 10000) == 0) printf("Slewing RPC, event %d\n", ev);

        sync_raw->Clear();
	t0_raw->Clear();
	tof2_raw->Clear();

        _t_in->GetEntry(ev);

        TOF2.fillSlewing(tof2_raw, sync_raw, t0_raw);

    }
    /////////////////////////////////////////////////////////////////////////////////////
   

    TOF2.Slewing();
    TOF2.drawproft0();
    TOF2.drawprof();

    _f_in->Close();
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
