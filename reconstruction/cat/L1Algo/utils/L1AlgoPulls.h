#ifndef L1AlgoPulls_h
#define L1AlgoPulls_h

// #define BUILD_HISTO_FOR_EACH_STANTION

#ifdef BUILD_HISTO_FOR_EACH_STANTION
const int NStations = 8;
#else
const int NStations = 0;
#endif // BUILD_HISTO_FOR_EACH_STANTION


#include "TCanvas.h"
#include "TF1.h"
#include "TStyle.h"

#include "L1Algo/CbmL1Def.h"
#include "L1Algo/L1TrackPar.h"
#include "L1Algo/L1Algo.h"
#include "L1Algo/L1StsHit.h"
#include "CbmL1.h"
#include <vector>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;

struct TL1TrackParameters
{
  double x,y,tx,ty,qp;

  static const int NParameters = 5;

  TL1TrackParameters(){};
  TL1TrackParameters(L1TrackPar& T, int i):x(T.x[i]),y(T.y[i]),tx(T.tx[i]),ty(T.ty[i]),qp(T.qp[i]){};
  TL1TrackParameters(CbmL1MCPoint& T):x(T.x),y(T.y),tx(T.px/T.pz),ty(T.py/T.pz),qp(T.q/T.p){};

  double operator[](int i){
    switch (i){
      case 0: return x;
      case 1: return y;
      case 2: return tx;
      case 3: return ty;
      case 4: return qp;
    };
  }
  
  TL1TrackParameters operator-(TL1TrackParameters& b){
    TL1TrackParameters c;
    c.x  = x  - b.x;
    c.y  = y  - b.y;
    c.tx = tx - b.tx;
    c.ty = ty - b.ty;
    c.qp = qp - b.qp;
    return c;
  }

  TL1TrackParameters operator/(TL1TrackParameters& b){
    TL1TrackParameters c;
    c.x  = x  / b.x;
    c.y  = y  / b.y;
    c.tx = tx / b.tx;
    c.ty = ty / b.ty;
    c.qp = qp / b.qp;
    return c;
  }

  void Print(){
    cout << x << " "  << y << " " << tx << " " << ty << " " << qp << endl;
  }
};

const TString L1TrackParametersNames[TL1TrackParameters::NParameters] = {
  "x","y","tx","ty","qp"
};

class L1AlgoPulls{
  public:
    L1AlgoPulls():fNAllPulls(0){
      fGPulls.clear();
      for (int i = 0; i < NStations; i++) fStaPulls[i].clear();
    };
    void Init();

//     void AddVec(L1TrackPar& T, THitI ih);
    void AddOne(L1TrackPar& T, int i, THitI ih);
    void Print(); // fast method to see pulls :)
    void Build(bool draw = 1);

    int fNAllPulls; // number of AddOne calls
  private:
    void makeUpHisto(TH1* hist, TString title, float& sigma);

    CbmL1* fL1;
    vector<TL1TrackParameters> fGPulls;    // pulls for all stations
    vector<TL1TrackParameters> fStaPulls[NStations]; // pulls for each station
    TH1F* histoPull[(1+NStations)*TL1TrackParameters::NParameters];

    vector<TL1TrackParameters> fGRes;    // residuals for all stations
    TH1F* histoRes[(1+NStations)*TL1TrackParameters::NParameters];

    static const float TailCut = 5000.; //
    static const float csCut = 5.;   // chi-square cut
    static const int textFont = 22;  // TNewRoman
    TStyle *histoStyle;
};

  // ===================================================================================

void L1AlgoPulls::Init()
{
  fL1 = CbmL1::Instance();


  static bool first_call = 1;
  if( first_call ){
//     TDirectory *curdir = gDirectory;
//     fL1->histodir->cd();
//     gDirectory->mkdir("L1AlgoPulls");
//     gDirectory->cd("L1AlgoPulls");

      // add global pulls
    for (int i = 0; i < TL1TrackParameters::NParameters; i++){
      TString name = "pull_";
      name += L1TrackParametersNames[i];
      histoPull[i] = new TH1F(name, name, 50, -10, 10);
    }

#ifdef BUILD_HISTO_FOR_EACH_STANTION
       // add station pulls
    for (int i = TL1TrackParameters::NParameters; i < (NStations + 1)*TL1TrackParameters::NParameters; i++){
      int ista = i/TL1TrackParameters::NParameters-1;
      TString name = "pull_sta";
      name += ista;
      name += "_";
      name += L1TrackParametersNames[i%TL1TrackParameters::NParameters];
      histoPull[i] = new TH1F(name, name, 50, -10, 10);
    }
#endif // BUILD_HISTO_FOR_EACH_STANTION
          // add global residuals
    for (int i = 0; i < TL1TrackParameters::NParameters; i++){
      TString name = "residual_";
      name += L1TrackParametersNames[i];
      float size = 10;
      switch (i){
        case 0: size = .01; break;
        case 1: size = .01; break;
        case 2: size = 0.003; break;
        case 3: size = 0.003; break;
        case 4: size = 0.1; break;
      };
      histoRes[i] = new TH1F(name, name, 50, -size, size);
    }

      // define style
    histoStyle = new TStyle("histoStyle","Plain Style(no colors/fill areas)");

    histoStyle->SetTextFont(textFont);
    histoStyle->SetPadColor(0);
    histoStyle->SetCanvasColor(0);
    histoStyle->SetTitleColor(0);
    histoStyle->SetStatColor(0);
  
    histoStyle->SetOptTitle(0); // without main up title
    histoStyle->SetOptStat(1000001010);
//   The parameter mode can be = ksiourmen  (default = 000001111)
//   k = 1;  kurtosis printed
//   k = 2;  kurtosis and kurtosis error printed
//   s = 1;  skewness printed
//   s = 2;  skewness and skewness error printed
//   i = 1;  integral of bins printed
//   o = 1;  number of overflows printed
//   u = 1;  number of underflows printed
//   r = 1;  rms printed
//   r = 2;  rms and rms error printed
//   m = 1;  mean value printed
//   m = 2;  mean and mean error values printed
//   e = 1;  number of entries printed
//   n = 1;  name of histogram is printed
  
    histoStyle->SetOptFit(10001);
//   The parameter mode can be = pcev  (default = 0111)
//   p = 1;  print Probability
//   c = 1;  print Chisquare/Number of degress of freedom
//   e = 1;  print errors (if e=1, v must be 1)
//   v = 1;  print name/values of parameters
  
    histoStyle->SetStatW(0.175);
    histoStyle->SetStatH(0.02);
    histoStyle->SetStatX(0.95);
    histoStyle->SetStatY(0.97);
    histoStyle->SetStatFontSize(0.05);

    histoStyle->SetStatFont(textFont);


    
//     gDirectory->cd("..");
//     curdir->cd();
    first_call = 0;
  }
};

// inline void L1AlgoPulls::AddVec(L1TrackPar& T_, THitI ih)
// {
//   for (int i = 0; i < fvecLen; i++)
//     AddOne(T_,i,ih);
// }

inline void L1AlgoPulls::AddOne(L1TrackPar& T_, int i, THitI ih)
{
  fNAllPulls++;
  TL1TrackParameters T(T_, i);

  if (T_.chi2[i] > csCut * T_.NDF[i]) return;
    // get error
  TL1TrackParameters err;
  if (!( finite(T_.C00[i]) && T_.C00[i] > 0 )) return;
  if (!( finite(T_.C11[i]) && T_.C11[i] > 0 )) return;
  if (!( finite(T_.C22[i]) && T_.C22[i] > 0 )) return;
  if (!( finite(T_.C33[i]) && T_.C33[i] > 0 )) return;
  if (!( finite(T_.C44[i]) && T_.C44[i] > 0 )) return;
  err.x = sqrt(T_.C00[i]);
  err.y = sqrt(T_.C11[i]);
  err.tx = sqrt(T_.C22[i]);
  err.ty = sqrt(T_.C33[i]);
  err.qp = sqrt(T_.C44[i]);

      // mc data
  int iMCP = fL1->vHitMCRef[ih];
  if (iMCP < 0) return;
  CbmL1MCPoint& mcP = fL1->vMCPoints[ iMCP ];
  TL1TrackParameters mc( mcP );

      // fill residuals
  TL1TrackParameters res = (mc-T);
  fGRes.push_back(res);

      // fill pulls
  TL1TrackParameters P = res/err;
  fGPulls.push_back(P);

#ifdef BUILD_HISTO_FOR_EACH_STANTION
  int ista = mcP.iStation - 2; // last hit
//   int ista = fL1->algo->vSFlag[ fL1->algo->vStsHits[ih].f ]/4 - 2; // last hit
  fStaPulls[ista].push_back(P);
#endif // BUILD_HISTO_FOR_EACH_STANTION
};

inline void L1AlgoPulls::Print()
{ // TODO: renew
  cout << "All pulls:     " << fNAllPulls << endl;
  cout << "Correct pulls: " << fGPulls.size() << endl;
  cout  << "x     y    tx    ty    qp" <<  endl;
  for (int i = 0; i < fGPulls.size(); i++){
    TL1TrackParameters& pull = fGPulls[i];
    pull.Print();
  }
};

inline void L1AlgoPulls::Build(bool draw)
{
      // --- fill histograms ---
    // global pulls
  for (int i = 0; i < fGPulls.size(); i++){
    TL1TrackParameters& pull = fGPulls[i];
    for (int ih = 0; ih < TL1TrackParameters::NParameters; ih++){
      if (TailCut > fabs(pull[ih])) histoPull[ih]  ->Fill(pull[ih]);
    }
  }
#ifdef BUILD_HISTO_FOR_EACH_STANTION
    // station pulls
  for (int iSta = 0; iSta < NStations; iSta++){
    vector<TL1TrackParameters>& Pulls = fStaPulls[iSta];
    for (int i = 0; i < Pulls.size(); i++){
      TL1TrackParameters& pull = Pulls[i];
      for (int ih = 0; ih < TL1TrackParameters::NParameters; ih++){
        if (TailCut > fabs(pull[ih])) histoPull[(iSta + 1) * TL1TrackParameters::NParameters + ih]  ->Fill(pull[ih]);
      }
    }
  }
#endif // BUILD_HISTO_FOR_EACH_STANTION
  
    // global residuals
  for (int i = 0; i < fGRes.size(); i++){
    TL1TrackParameters& res = fGRes[i];
    for (int ih = 0; ih < TL1TrackParameters::NParameters; ih++){
      if (TailCut > fabs(res[ih])) histoRes[ih]  ->Fill(res[ih]);
    }
  }

        // --- draw histograms --- and save info
  float
      pulls[(NStations + 1)*TL1TrackParameters::NParameters][2], // 0 - sigma, 1 - RMS
  residuals[(NStations + 1)*TL1TrackParameters::NParameters][2];
  
  system("mkdir L1_Pulls -p");
  chdir( "L1_Pulls" );
  TCanvas *c2 = new TCanvas("c2","c2",0,0,600,400);
  c2->cd();
  histoStyle->cd();
  for (int ih = 0; ih < (NStations + 1)*TL1TrackParameters::NParameters; ih++){
    makeUpHisto(histoPull[ih], histoPull[ih]->GetName(), pulls[ih][0]);
    pulls[ih][1] = histoPull[ih]->GetRMS();
    if (draw){
      histoPull[ih]  ->Draw();
      TString name = histoPull[ih]->GetName();
      name += ".pdf";
      c2->SaveAs(name);
    }
  }
  for (int ih = 0; ih < (0 + 1)*TL1TrackParameters::NParameters; ih++){
    makeUpHisto(histoRes[ih], histoRes[ih]->GetName(), residuals[ih][0]);
    residuals[ih][1] = histoRes[ih]->GetRMS();
    if (draw){
      histoRes[ih]  ->Draw();
      TString name = histoRes[ih]->GetName();
      name += ".pdf";
      c2->SaveAs(name);
    }
  }
  chdir( ".." );

        // --- print information ---
  cout << "All entries:     " << fNAllPulls << endl;
  cout << "Correct entries: " << fGPulls.size() << endl;
  cout << "Pulls sigma & RMS: " << endl;
  for (int ih = 0; ih < (NStations + 1)*TL1TrackParameters::NParameters; ih++){
    int ipar = ih%TL1TrackParameters::NParameters;
    int ista = ih/TL1TrackParameters::NParameters;
    if ( (ista > 0) && (ipar == 0) ) cout << "Station " << ista-1 << endl;
    cout << L1TrackParametersNames[ipar] << "\t" << pulls[ih][0] << "\t" << pulls[ih][1] << endl;
  }
  cout << "Residuals sigma & RMS: " << endl;
  for (int ih = 0; ih < (0 + 1)*TL1TrackParameters::NParameters; ih++){
    int ipar = ih%TL1TrackParameters::NParameters;
    cout << L1TrackParametersNames[ipar] << "\t" << residuals[ih][0] << "\t" << residuals[ih][1] << endl;
  }
};

inline void L1AlgoPulls::makeUpHisto(TH1* hist, TString title, float& sigma)
{
  if (hist && (hist->GetEntries() != 0)){
    TF1  *fit = new TF1("fit","gaus");
    fit->SetLineColor(2);
    fit->SetLineWidth(3);
    hist->Fit("fit","","",hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax());
    sigma = fit->GetParameter(2);

    hist->GetXaxis()->SetLabelFont(textFont);
    hist->GetXaxis()->SetTitleFont(textFont);
    hist->GetYaxis()->SetLabelFont(textFont);
    hist->GetYaxis()->SetTitleFont(textFont);
    
    hist->GetXaxis()->SetTitle(title);
    hist->GetXaxis()->SetTitleOffset(1);
    hist->GetYaxis()->SetTitle("Entries");
    hist->GetYaxis()->SetTitleOffset(1.05); // good then entries per bit <= 9999
  }
  else{
    std::cout << " E: Read hists error! " << std::endl;
  }
}

#endif
