#include <unistd.h> // for dir navigation

const int textFont = 22; // TNewRoman
const bool divide = 0; // = 0 - each histo in separate file\screen. = 1 - all in one file\screen


void FitHistoGaus(TH1* hist, float* sigma = 0)
{
  if (hist && (hist->GetEntries() != 0)){
    TF1  *fit = new TF1("fit","gaus");
    fit->SetLineColor(2);
    fit->SetLineWidth(3);
    hist->Fit("fit","","",hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax());
    if (sigma) *sigma = fit->GetParameter(2);
  }
  else{
    std::cout << " E: Read hists error! " << std::endl;
  }
}

void FitHistoLine(TH1* hist)
{
  if (hist && (hist->GetEntries() != 0)){
    TF1  *fit = new TF1("fit","[0]");
    fit->SetLineColor(2);
    fit->SetLineWidth(3);
    hist->Fit("fit","","",hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax());
  }
  else{
    std::cout << " E: Read hists error! " << std::endl;
  }
}

void makeUpHisto(TH1* hist, TString title)
{
  if (hist && (hist->GetEntries() != 0)){

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


void makeUpHisto(TH2* hist, TString title)
{
  if (hist && (hist->GetEntries() != 0)){
    // hist->GetXaxis()->SetLabelFont(textFont);
    // hist->GetXaxis()->SetTitleFont(textFont);
    // hist->GetYaxis()->SetLabelFont(textFont);
    // hist->GetYaxis()->SetTitleFont(textFont);
    
    // hist->GetXaxis()->SetTitle(title);
    // hist->GetXaxis()->SetTitleOffset(1);
    // hist->GetYaxis()->SetTitle("Entries");
    // hist->GetYaxis()->SetTitleOffset(1.05); // good then entries per bit <= 9999

    hist->SetOption("CONTZ");
  }
  else{
    std::cout << " E: Read hists error! " << std::endl;
  }
}

void makeUpProfile(TProfile* prof, TString titleX, TString titleY)
{
  if (prof && (prof->GetEntries() != 0)){
    prof->SetMarkerColor(kRed);
    prof->SetLineColor(kRed);
    
    prof->GetXaxis()->SetTitle(titleX);
    prof->GetXaxis()->SetTitleOffset(1);
    prof->GetYaxis()->SetTitle(titleY);
    prof->GetYaxis()->SetTitleOffset(0.85);
      
    prof->GetXaxis()->SetLabelFont(textFont);
    prof->GetXaxis()->SetTitleFont(textFont);
    prof->GetYaxis()->SetLabelFont(textFont);
    prof->GetYaxis()->SetTitleFont(textFont);
  }
  else{
    std::cout << " E: Read profile error! " << std::endl;
  }
}

void Draw_L1_histo() {

    // ============================ Set Styles ============================
  TStyle *histoStyle = new TStyle("histoStyle","Plain Style(no colors/fill areas)");
//   TStyle *histoStyle = gStyle;
  
  histoStyle->SetTextFont(textFont);
  histoStyle->SetPadColor(0);
  histoStyle->SetCanvasColor(0);
  histoStyle->SetTitleColor(0);
  histoStyle->SetStatColor(0);
  
  histoStyle->SetOptTitle(0); // without main up title
  histoStyle->SetOptStat(1000111010);
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
  histoStyle->SetPadBorderMode(0);
  histoStyle->SetFrameBorderMode(0);

  histoStyle->SetPalette(1); // for 2D
  
  TStyle *profStyle = new TStyle("profStyle","Plain Style(no colors/fill areas)");
//   TStyle *profStyle = gStyle;
  
  profStyle->SetTextFont(textFont);
  profStyle->SetPadColor(0);
  profStyle->SetCanvasColor(0);
  profStyle->SetTitleColor(0);
  profStyle->SetStatColor(0);
  
  profStyle->SetOptTitle(0); // without main up title
  profStyle->SetOptStat(0);
  profStyle->SetOptFit(0);
  profStyle->SetGridStyle(2);

  profStyle->SetPadBorderMode(0);
  profStyle->SetFrameBorderMode(0);

    // ============================ Open file ============================

  const TString fileName = "L1_histo.root";

  TFile *fileIn = new TFile(fileName.Data(),"read");

  FILE *ress, *pulls;
  ress = fopen("resolution.txt","w");
  pulls = fopen("pull.txt","w");


    // ------------ make histos -----------


      // ------------ tracks -----------
  
  TString dirFitName = "/L1/Fit";
  dirFitName = fileName + ":" + dirFitName;
  TDirectory *dirFit = fileIn->Get(dirFitName);
  
  const int nHisto = 40;
  
  struct THistoData
  {
    char *name;  // one, which used in input file
    char *title; // for X-axis
  };
  const THistoData histoData[nHisto] =
  {
    {"fst_x",  "Residual (x^{reco}-x^{mc}) [#mum]" }
    {"fst_y",  "Residual (y^{reco}-y^{mc}) [#mum]" }
    {"fst_tx",  "Residual (t_{x}^{reco}-t_{x}^{mc}) [10^{-3}]" }
    {"fst_ty",  "Residual (t_{y}^{reco}-t_{y}^{mc}) [10^{-3}]" }
    {"fst_P",  "Resolution (p^{reco}-p^{mc})/p^{mc}"   }
    {"fst_px",  "Pull x"   }
    {"fst_py",  "Pull y" }
    {"fst_ptx",  "Pull t_{x}" }
    {"fst_pty",  "Pull t_{y}" }
    {"fst_pQP",  "Pull q/p"   }
    
    {"lst_pQP",  "Pull q/p"   }
    {"lst_ptx",  "Pull t_{x}" }
    {"lst_pty",  "Pull t_{y}" }
    {"lst_px",  "Pull x"   }
    {"lst_py",  "Pull y" }
    {"lst_P",  "Resolution (p^{reco}-p^{mc})/p^{mc}"   }
    {"lst_tx",  "Residual (t_{x}^{reco}-t_{x}^{mc}) [mrad]" }
    {"lst_ty",  "Residual (t_{y}^{reco}-t_{y}^{mc}) [mrad]" }
    {"lst_x",  "Residual (x^{reco}-x^{mc}) [#mum]" }
    {"lst_y",  "Residual (y^{reco}-y^{mc}) [#mum]" }
    {"svrt_pQP",  "Pull q/p"   }
    {"svrt_ptx",  "Pull t_{x}" }
    {"svrt_pty",  "Pull t_{y}" }
    {"svrt_px",  "Pull x"   }
    {"svrt_py",  "Pull y" }
    {"svrt_P",  "Resolution (p^{reco}-p^{mc})/p^{mc}"   }
    {"svrt_tx",  "Residual (t_{x}^{reco}-t_{x}^{mc}) [mrad]" }
    {"svrt_ty",  "Residual (t_{y}^{reco}-t_{y}^{mc}) [mrad]" }
    {"svrt_x",  "Residual (x^{reco}-x^{mc}) [#mum]" }
    {"svrt_y",  "Residual (y^{reco}-y^{mc}) [#mum]" }
    {"pvrt_pQP",  "Pull q/p"   }
    {"pvrt_ptx",  "Pull t_{x}" }
    {"pvrt_pty",  "Pull t_{y}" }
    {"pvrt_px",  "Pull x"   }
    {"pvrt_py",  "Pull y" }
    {"pvrt_P",  "Resolution (p^{reco}-p^{mc})/p^{mc}"   }
    {"pvrt_tx",  "Residual (t_{x}^{reco}-t_{x}^{mc}) [mrad]" }
    {"pvrt_ty",  "Residual (t_{y}^{reco}-t_{y}^{mc}) [mrad]" }
    {"pvrt_x",  "Residual (x^{reco}-x^{mc}) [#mum]" }
    {"pvrt_y",  "Residual (y^{reco}-y^{mc}) [#mum]" }
  };
  
  TH1F *histo[nHisto];

  float charat[nHisto][2]; // 0 - sigma, 1 - RMS
  for (int i = 0; i < nHisto; i++){
    histo[i] = (TH1F*) dirFit->Get(histoData[i].name);
    makeUpHisto(histo[i], histoData[i].title);
    FitHistoGaus(histo[i], &(charat[i][0]));
    charat[i][1] = histo[i]->GetRMS();
  }

  cout.setf(std::ios::scientific,std::ios::floatfield);
  cout.precision(2);
  cout << endl << "Name   " << "   \t" << " Sigma   " << "\t" << " RMS " << endl;
  for (int i = 0; i < nHisto; i++){
    cout << histoData[i].name << "   \t" << charat[i][0] << "\t" << charat[i][1] << endl;
  }
      // ------------ hits, strips -----------
  
  TString dirInputName = "/L1/Input";
  dirInputName = fileName + ":" + dirInputName;
  TDirectory *dirInput = fileIn->Get(dirInputName);
  
  const int nHistoInput = 4;
  
  const THistoData histoDataInput[nHisto] =
  {
    {"Px",  "Pull x" }
    {"Py",  "Pull y" }
    {"x",  "Residual (x^{hit}-x^{mc}) [#mum]" }
    {"y",  "Residual (y^{hit}-y^{mc}) [#mum]" }
  };
  
  TH1F *histoInput[nHistoInput];

  for (int i = 0; i < nHistoInput; i++){
    histoInput[i] = (TH1F*) dirInput->Get(histoDataInput[i].name);
    makeUpHisto(histoInput[i],histoDataInput[i].title);
    FitHistoGaus(histoInput[i]);
  }

        // ------------ other histo -----------
  
  TString dirOtherName = "/L1";
  dirOtherName = fileName + ":" + dirOtherName;
  TDirectory *dirOther = fileIn->Get(dirOtherName);
  
  const int nHistoOther = 15;
  
  const THistoData histoDataOther[nHisto] =
  {
    {"h_acc_MCmom", "Momentum of tracks"}
    {"h_reco_MCmom", "Momentum of tracks"}
    {"h_unreco_MCmom", "Momentum of tracks"}
    {"h_ghost_Rmom", "Momentum of tracks"}
    {"h_acc_prim_MCmom", "Momentum of tracks"}
    {"h_reco_prim_MCmom", "Momentum of tracks"}
    {"h_unreco_prim_MCmom", "Momentum of tracks"}
    {"h_ghost_Rmom", "Momentum of tracks"}
    {"h_acc_sec_MCmom", "Momentum of tracks"}
    {"h_reco_sec_MCmom", "Momentum of tracks"}
    {"h_unreco_sec_MCmom", "Momentum of tracks"}
    {"h_ghost_Rmom", "Momentum of tracks"}
        //  reg,acc,reco,ghost distribution always have to be first
    {"h_reco_prob",  "Prob of reco track" }
    {"h_ghost_prob",  "Prob of ghost track" }
    {"h_rest_prob",  "Prob of reco rest track" }
  };
  
  TH1F *histoOther[nHistoOther];

  for (int i = 0; i < 12; i++){
    if (i%4 != 2) histoOther[i] = (TH1F*) dirOther->Get(histoDataOther[i].name);
    else {
      histoOther[i] = new TH1F(histoDataOther[i].name, "Not reconstructed tracks", 100, 0.0, 5.0);
      for( j = 0; j < histoOther[i]->GetNbinsX(); j++ ) {
        histoOther[i]->SetBinContent( j, histoOther[i-2]->GetBinContent(j) - histoOther[i-1]->GetBinContent(j) );
      }
    }
    makeUpHisto(histoOther[i], histoDataOther[i].title);
  }

  for (int i = 12; i < nHistoOther; i++){
    histoOther[i] = (TH1F*) dirOther->Get(histoDataOther[i].name);
    makeUpHisto(histoOther[i], histoDataOther[i].title);
    FitHistoLine(histoOther[i]);
  }

          // ------------ other 2D histo -----------
  
  TString dirOther2DName = "/L1";
  dirOther2DName = fileName + ":" + dirOther2DName;
  TDirectory *dirOther2D = fileIn->Get(dirOther2DName);
  
  const int nHistoOther2D = 1;
 
  const THistoData histoDataOther2D[nHisto] =
  {
    {"h2_reco_nhits_vs_mom_prim",  "NHits and momentum for primary" }
    // {"h2_reco_prob_ndf",  "Prob and NDF" }
    // {"h2_reco_prob_mom",  "Prob and momentum" }
    // {"h2_reco_prob_teta",  "Prob and teta" }
    // {"h2_teta_prob",  "Prob" }
    // {"h2_teta_probI",  "ProbI" }
    // {"h2_teta_probO",  "ProbO" }
  };
  
  TH2F *histoOther2D[nHistoOther2D];

  for (int i = 0; i < nHistoOther2D; i++){
    histoOther2D[i] = (TH2F*) dirOther2D->Get(histoDataOther2D[i].name);
    makeUpHisto(histoOther2D[i], histoDataOther2D[i].title);
  }
  
    // ------------ make profiles -----------

  TString dirName = "/L1";
  dirName = fileName + ":" + dirName;
  TDirectory *dir = fileIn->Get(dirName);
  
  const int nProf = 4;
  struct TProfData
  {
    char *name;  // one, which used in input file
    char *titleX; // for X-axis
    char *titleY; // for Y-axis
  };
  const TProfData profData[nProf] =
  {
    {"p_eff_all_vs_mom",  "Momentum [GeV/c]",  "Efficiency [%]"  }
    {"p_eff_all_vs_nhits",  "NMCHits",  "Efficiency [%]"  }
    {"p_eff_prim_vs_nhits",  "NMCHits",  "Efficiency [%]"  }
    {"p_eff_sec_vs_nhits",  "NMCHits",  "Efficiency [%]"  }
  };
  
  TProfile *profile[nProf];

  for (int i = 0; i < nProf; i++){
    profile[i] = (TProfile*) dir->Get(profData[i].name);
    makeUpProfile(profile[i], profData[i].titleX, profData[i].titleY);
  }


    // ============================ Draw  ============================
//   TCanvas *c1;
//   c1 = new TCanvas("c1","c1",0,0,1200,800);
//   c1 -> Divide(nHisto);
// 
//   c1->UseCurrentStyle();
// 
//   for (int i = 0; i < nHisto; i++){
//     c1->cd(i+1);
//     histo[i]->Draw();
//   }

//   c1->SaveAs("histo.png");

  system("mkdir L1_histo -p");
  chdir( "L1_histo" );

  histoStyle->cd();
  TCanvas *c2,*c3;
  c2 = new TCanvas("c2","c2",0,0,900,600);
  c3 = new TCanvas("c3","c3",0,0,900,600);
  c3->Divide(5, 2);

  int iPart = 1;
  for (int i = 0; i < 10; i++){
    c3->cd(iPart++);
    histo[i]->Draw();
  }
  c3->SaveAs("fst.pdf");
  
  c2->cd();
  for (int i = 0; i < nHisto; i++){
    histo[i]->Draw();
    TString name = TString(histoData[i].name)+".pdf";
    c2->SaveAs(name);
  }

  for (int i = 0; i < nHistoInput; i++){
    histoInput[i]->Draw();
    TString name = TString(histoDataInput[i].name)+".pdf";
    c2->SaveAs(name);
  }

    // draw reg,acc,reco,ghost distribution
  histoStyle->SetOptStat(000000000);
  for (int j = 0, iH = 0; j < 3; j++){
    TLegend *leg1 = new TLegend(0.2+0.3,.62,0.55+0.3,.75);
    leg1->SetFillColor(0);
    leg1->SetLineWidth(0);
    leg1->SetBorderSize(0);
    leg1->SetTextFont(22);
    for (int i = 0; i < 4; i++, iH++){
      switch ( i ) {
        case 0: histoOther[iH]->SetLineColor(kBlue);  break;
        case 1: histoOther[iH]->SetLineColor(kRed);   break;
        case 2: histoOther[iH]->SetLineColor(kBlue); histoOther[iH]->SetLineStyle(2);  break;
        case 3: histoOther[iH]->SetLineColor(kBlack); break;
      }
      if (i == 0) histoOther[iH]->Draw();
      else        histoOther[iH]->Draw("same");
      leg1->AddEntry( histoOther[iH], histoOther[iH]->GetTitle() );
    }
    leg1->Draw();
    switch (j) {
      case 0: c2->SaveAs("MultiplicityMom.pdf"); break;
      case 1: c2->SaveAs("MultiplicityMomPrim.pdf"); break;
      case 2: c2->SaveAs("MultiplicityMomSec.pdf"); break;
    }
  }
  histoStyle->SetOptStat(1000111010);
  
  for (int i = 12; i < nHistoOther; i++){
    histoOther[i]->Draw();
    TString name = TString(histoDataOther[i].name)+".pdf";
    c2->SaveAs(name);
  }

  histoStyle->SetOptStat(1000000010);
  for (int i = 0; i < nHistoOther2D; i++){
    histoOther2D[i]->Draw();
    TString name = TString(histoDataOther2D[i].name)+".pdf";
    c2->SaveAs(name);
  }
  histoStyle->SetOptStat(1000111010);
  
  profStyle->cd();
  c2->cd();
  for (int i = 0; i < nProf; i++){
    c2->SetGridx();
    c2->SetGridy();
    profile[i]->Draw();
    TString name = TString(profData[i].name)+".pdf";
    c2->SaveAs(name);
  }


  chdir( ".." );
}
