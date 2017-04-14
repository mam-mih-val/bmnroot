
using namespace std;

void Calibration_numbers_ZDC(char *fname="bmn_run0543.root") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    /////////////////////////////////////////////////////////////////////////////////////
    int RUN;
    sscanf(&fname[strlen(fname) - 8], "%d", &RUN);

    const char *mapping;
    if(RUN < 470) mapping="ZDC_map_Feb20.txt";
    else mapping="ZDC_map_Mar08.txt";

    BmnZDCRaw2Digit ZDC(mapping, fname);
    ZDC.setParStart(1.0);
    ZDC.setParStep(0.1);
    ZDC.setError(10.0);
    ZDC.setNorm(13.670104);
    ZDC.setLogFunction(1);

    ZDC.print();

    cout << "Process RUN:  " << RUN << endl;

    /////////////////////////////////////////////////////////////////////////////////////
    TFile *_f_in = new TFile(fname, "READ");
    TTree *_t_in = (TTree *) _f_in->Get("BMN_RAW");
    TClonesArray *zdc_raw  = new TClonesArray("BmnADCDigit");
    _t_in->SetBranchAddress("bmn_zdc",   &zdc_raw);
    /////////////////////////////////////////////////////////////////////////////////////

    int numbers[9] = {22,23,24,28,29,30,36,37,38};

    for (int ev = 0; ev < _t_in->GetEntries(); ev++) {

        if ((ev % 1000) == 0) printf("Calibration ZDC, event %d\n", ev);

        zdc_raw->Clear(); 

        _t_in->GetEntry(ev);

        if (ZDC.fillCalibrateNumbers(zdc_raw, 0., 0., 48., 9, numbers)) break;

    }
    /////////////////////////////////////////////////////////////////////////////////////
    ZDC.calibrate();
    ZDC.drawzdc();
    /////////////////////////////////////////////////////////////////////////////////////
   
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
