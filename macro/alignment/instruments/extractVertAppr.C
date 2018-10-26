R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
#include <TMath.h>

using namespace std;
using namespace TMath;

Int_t computeIntegral(TH1F*, Double_t, Double_t);

void extractVertAppr(TString filename = "") {
  bmnloadlibs(); // load BmnRoot libraries
  
  TChain* out = new TChain("cbmsim");
  out->Add(filename.Data());
  cout << "#recorded entries = " << out->GetEntries() << " in file" << endl;
  
  TClonesArray* vertex = NULL;
  
  out->SetBranchAddress("BmnVertex", &vertex);
  const UInt_t nEvs = out->GetEntries();
  
  TH1F* x = new TH1F("X", "X", 100, -10., +10.);
  TH1F* y = new TH1F("Y", "Y", 100, -10., +10.);
  TH1F* z = new TH1F("Z", "Z", 100, -10., +10.);

  Double_t roughX = 0.0;
  Double_t roughY = 0.0;
  Double_t roughZ = 0.0;
  
  for (Int_t iEv = 0; iEv < out->GetEntries(); iEv++) {
    out->GetEntry(iEv);
    if (iEv % 1000 == 0)
      cout << " Ev# " << iEv << endl;
    
    CbmVertex* Vp = (CbmVertex*) vertex->UncheckedAt(0);
    x->Fill(Vp->GetX());
    y->Fill(Vp->GetY());
    z->Fill(Vp->GetZ());

    if (iEv == 0) {
      roughX = Vp->GetRoughX();
      roughY = Vp->GetRoughY();
      roughZ = Vp->GetRoughZ();
    }
  }  

  Int_t intX = computeIntegral(x, -1., +1.);
  Int_t intY = computeIntegral(y, -5., -3.);
  Int_t intZ = computeIntegral(z, -3., 0.);

  if (intX != 0 && intY != 0 && intZ != 0) {
    TFitResultPtr fitX = x->Fit("gaus", "SQ", "", -1., +1.);
    TFitResultPtr fitY = y->Fit("gaus", "SQ", "", -5., -3.);
    TFitResultPtr fitZ = z->Fit("gaus", "SQ", "", -3., 0.);

    if (Abs(roughX - fitX->Parameter(1)) < 5. &&
	Abs(roughY - fitY->Parameter(1)) < 5. &&
	Abs(roughZ - fitZ->Parameter(1)) < 5.) {
      FILE* f = fopen("fitParams.txt", "w");  
      fprintf(f, "%G %G %G", fitX->Parameter(1), fitY->Parameter(1), fitZ->Parameter(1));
      fclose(f);
    }
  }
  delete out;
}

//------------------------------------------------------------
Int_t computeIntegral(TH1F* h, Double_t xmin, Double_t xmax) {
  TAxis* axis = h->GetXaxis();
  Int_t bmin = axis->FindBin(xmin); 
  Int_t bmax = axis->FindBin(xmax);
  return h->Integral(bmin,bmax);
}
