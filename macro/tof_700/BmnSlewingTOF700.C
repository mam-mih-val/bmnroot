#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
#include "bmndata/BmnEnums.h"
//file: full path to raw-file
//nEvents: if 0 then decode all events
void BmnSlewingTOF700(TString file = "mpd_run_trigCode_4649.data", Long_t nEvents = 0) {
    bmnloadlibs(); // load BmnRoot libraries
    UInt_t period = 7;
    BmnSetup stp = kBMNSETUP; // use kSRCSETUP for Short-Range Correlation program and kBMNSETUP otherwise
//    BmnSetup stp = kSRCSETUP; // use kSRCSETUP for Short-Range Correlation program and kBMNSETUP otherwise
    BmnSlewingTOF700* decoder = new BmnSlewingTOF700(file, nEvents, period);
    decoder->SetBmnSetup(stp);

    TString PeriodSetupExt = Form("%d%s.txt", period, ((stp == kBMNSETUP) ? "" : "_SRC"));
    decoder->SetTrigPlaceMapping(TString("Trig_PlaceMap_Run") + PeriodSetupExt); 
    decoder->SetTrigChannelMapping(TString("Trig_map_Run") + PeriodSetupExt); 
    decoder->SetTof700Mapping("TOF700_map_period_7.txt");

    decoder->SlewingTOF700Init();  // Decode data into detector-digits using current mappings.
    BmnTof2Raw2DigitNew *tof700m = decoder->GetTof700Mapper();

//  tof700m->SetW(2800,9000); // BMN
    tof700m->SetW(2600,5300); // SRC
//  tof700m->SetWT0(670,970); // BMN BC2
    tof700m->SetWT0(650,1100); // SRC BC2

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
