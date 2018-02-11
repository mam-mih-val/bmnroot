//file: full path to raw-file
//nEvents: if 0 then decode all events
//doConvert: convert RAW --> ROOT before decoding or use file converted before
void BmnSlewingTOF700(TString file = "../raw/mpd_run_Glob_1889.data", Long_t nEvents = 0, Bool_t doConvert = kFALSE) {
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  bmnloadlibs(); // load BmnRoot libraries
  BmnRawDataDecoder* decoder = new BmnRawDataDecoder(file, nEvents, 6); //4 - period

  Bool_t setup[11]; //array of flags to determine BM@N setup
  //Just put "0" to exclude detector from decoding
  setup[0] = 1; // TRIGGERS
  setup[1] = 0; // MWPC
  setup[2] = 0; // SILICON
  setup[3] = 0; // GEM
  setup[4] = 0; // TOF-400
  setup[5] = 1; // TOF-700
  setup[6] = 0; // DCH
  setup[7] = 0; // ZDC
  setup[8] = 0; // ECAL
  setup[9] = 0; // TQDC
  setup[10] = 0; // LAND
  decoder->SetDetectorSetup(setup);

  decoder->SetTrigMapping("Trig_map_Run6.txt");
  decoder->SetTrigINLFile("TRIG_INL.txt");
  decoder->SetTof700Mapping("TOF700_map_period_6.txt");

  if(doConvert) decoder->ConvertRawToRoot();  // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format

  decoder->SlewingTOF700Init();  // Decode data into detector-digits using current mappings.
  BmnTof2Raw2DigitNew *tof700m = decoder->GetTof700Mapper();
  tof700m->SetW(2800,5000);
  tof700m->SetWT0(720,820);
  decoder->SlewingTOF700();  // obtain slewing parameters
  tof700m->WriteSlewingHists();
  // draw quality control histogram - comment out if not neccessary
  //tof700m->drawproft0();
  //tof700m->drawprof();
  delete decoder;
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
