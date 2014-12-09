/*
 *====================================================================
 *
 *  Field approximation quality check
 *  
 *  Authors: M.Zyzak
 *
 *  e-mail : M.Zyzak@gsi.de
 *
 *====================================================================
 */

#include <unistd.h> // for dir navigation

void Draw_FieldApprox() {

  TString fileName = "FieldApprox.root";
  const int NStations = 8;
  
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  TStyle *plain = new TStyle("Plain","Plain Style(no colors/fill areas)");
//plain->SetCanvasBorderMode(0);
//plain->SetPadBorderMode(0);
  plain->SetPadColor(0);
  plain->SetCanvasColor(0);
  plain->SetTitleColor(0);
  plain->SetStatColor(0);
  plain->SetOptFit(1111);
//plain->SetStatX(0.7);
  plain->SetStatW(0.225);
  plain->SetOptStat(0);
  plain->cd();
  
  TFile *f = new TFile(fileName.Data(),"read");

  TH2F *stB[20];
  TH2F *stBx[20];
  TH2F *stBy[20];
  TH2F *stBz[20];

  TCanvas *c[20];

  gStyle->SetPalette(1);

  FILE *fff;
  fff = fopen("maxErr.txt","w");

  system("mkdir FieldApprox -p");
  chdir( "FieldApprox" );
  for (int ist=0; ist<NStations; ist++)
  {
    TString nameCanv;

//     if(ist<2)
//       nameCanv = Form("MVD %i",ist+1);
//     else
//       nameCanv = Form("STS %i",ist-1);
    nameCanv = Form("STS %i",ist+1);

    c[ist] = new TCanvas(nameCanv.Data(),nameCanv.Data(),0,0,1000,900);

    c[ist]->Divide(2,2);
    int ff;


    cout << Form("station %i, dB, z = %i", ist+1,ff) << endl;
    stB[ist] = (TH2F*) f->Get(Form("station %i, dB", ist+1));
    stBx[ist] = (TH2F*) f->Get(Form("station %i, dBx", ist+1));
    stBy[ist] = (TH2F*) f->Get(Form("station %i, dBy", ist+1));
    stBz[ist] = (TH2F*) f->Get(Form("station %i, dBz", ist+1));


    TPaletteAxis *palette;

    c[ist]->cd(1);
    stB[ist]->GetZaxis()->SetTitleOffset(2.1);
    stB[ist]->SetTitle(Form("z = %i cm",ff));
    stB[ist]->Draw("colz");

    gPad->SetRightMargin(0.2);
    gPad->Update();

    c[ist]->cd(2);
    stBx[ist]->GetZaxis()->SetTitleOffset(2.1);
    stBx[ist]->SetTitle(Form("z = %i cm",ff));
    stBx[ist]->Draw("colz");

    gPad->SetRightMargin(0.2);
    gPad->Update();

    c[ist]->cd(3);
    stBy[ist]->GetZaxis()->SetTitleOffset(2.1);
    stBy[ist]->SetTitle(Form("z = %i cm",ff));
    stBy[ist]->Draw("colz");

    gPad->SetRightMargin(0.2);
    gPad->Update();

    c[ist]->cd(4);
    stBz[ist]->GetZaxis()->SetTitleOffset(2.1);
    stBz[ist]->SetTitle(Form("z = %i cm",ff));
    stBz[ist]->Draw("colz");

    gPad->SetRightMargin(0.2);
    gPad->Update();


    double XmaxB = fabs(stB[ist]->GetMaximum());
    double XminB = fabs(stB[ist]->GetMinimum());
    if (XmaxB < XminB) XmaxB = XminB;

    double XmaxBx = fabs(stBx[ist]->GetMaximum());
    double XminBx = fabs(stBx[ist]->GetMinimum());
    if (XmaxBx < XminBx) XmaxBx = XminBx;

    double XmaxBy = fabs(stBy[ist]->GetMaximum());
    double XminBy = fabs(stBy[ist]->GetMinimum());
    if (XmaxBy < XminBy) XmaxBy = XminBy;

    double XmaxBz = fabs(stBz[ist]->GetMaximum());
    double XminBz = fabs(stBz[ist]->GetMinimum());
    if (XmaxBz < XminBz) XmaxBz = XminBz;

    fprintf(fff,"%.2g %.2g  %.2g  %.2g\n", XmaxB, XmaxBx, XmaxBy, XmaxBz);



    nameCanv += ".pdf";
    c[ist]->SaveAs(nameCanv.Data());
  }


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

  chdir( ".." );
}
