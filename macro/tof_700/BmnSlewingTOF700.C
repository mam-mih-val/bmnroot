#include "../../bmndata/BmnEnums.h"
#include "../run/bmnloadlibs.C"
//file: full path to raw-file
//nEvents: if 0 then decode all events
//doConvert: convert RAW --> ROOT before decoding or use file converted before
void BmnSlewingTOF700(TString file = "mpd_run_trigCode_3738.data", Long_t nEvents = 0) {
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
  bmnloadlibs(); // load BmnRoot libraries
  BmnSetup stp = kSRCSETUP; // use kSRCSETUP for Short-Range Correlation program and kBMNSETUP otherwise
  BmnRawDataDecoder* decoder = new BmnRawDataDecoder(file, nEvents, 7); //7 - period
  decoder->SetBmnSetup(stp);

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
  setup[9] = 0; // LAND
  setup[10] = 0; // CSC
  decoder->SetDetectorSetup(setup);

  decoder->SetTrigMapping("Trig_map_Run7.txt");
  //decoder->SetTrigMapping("Trig_map_Run7_SRC.txt");
  //decoder->SetTrigINLFile("TRIG_INL.txt"); // run period 6
  //decoder->SetTrigINLFile("TRIG_INL_076D-16A8.txt");//run period 7 SRC INL only for TDC data 
  decoder->SetTrigINLFile("TRIG_INL_076D-180A.txt");//run period 7 BMN INL only for TDC data 

  decoder->SetTof700Mapping("TOF700_map_period_7.txt");
  decoder->SetTof700Geom("TOF700_geometry_run7.txt");

  decoder->SlewingTOF700Init();  // Decode data into detector-digits using current mappings.
  BmnTof2Raw2DigitNew *tof700m = decoder->GetTof700Mapper();

  tof700m->SetW(2800,9000);
  tof700m->SetWT0(670,970); // BMN BC2

  decoder->SlewingTOF700();  // obtain slewing parameters

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
