// -----------------------------------------------------------------------------
// This macro plots evolution of the new and cumulative alignment corrections
// vs. iteration number.
//
//
// Anatoly.Solomin@jinr.ru 2017-02-16

#include   <map>
#include   <string>
#include   <utility> // for pair
#include   <vector>

#include   <TCanvas.h>
#include   <TClonesArray.h>
#include   <TFile.h>
#include   <TString.h>
#include   <TTree.h>

using namespace std;

void plot_align_corrections_gem(TString newAlignCorrFileListFileName,
                                TString sumAlignCorrFileListFileName,
                                TString alignCorrPlotsFileName,
                                int     runPeriod)
{
    TString startAlignFrom = "scratch";
  //TString startAlignFrom = "default";
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    map<const pair<int, int>, int> mStatMod_Pad;

    int const mlpdParNr_What5[30][3] = {{0, 0, 0},
                                        {1, 0, 0},
                                        {2, 0, 0},
                                        {0, 1, 0},
                                        {1, 1, 0},
                                        {2, 1, 0},
                                        {0, 2, 0},
                                        {1, 2, 0},
                                        {2, 2, 0},
                                        {0, 3, 0},
                                        {1, 3, 0},
                                        {2, 3, 0},
                                        {0, 4, 0},
                                        {1, 4, 0},
                                        {2, 4, 0},
                                        {0, 4, 1},
                                        {1, 4, 1},
                                        {2, 4, 1},
                                        {0, 5, 0},
                                        {1, 5, 0},
                                        {2, 5, 0},
                                        {0, 5, 1},
                                        {1, 5, 1},
                                        {2, 5, 1},
                                        {0, 6, 0},
                                        {1, 6, 0},
                                        {2, 6, 0},
                                        {0, 6, 1},
                                        {1, 6, 1},
                                        {2, 6, 1}};
    int const mlpdParNr_What6[27][3] = {{0, 0, 0},
                                        {1, 0, 0},
                                        {2, 0, 0},
                                        {0, 1, 0},
                                        {1, 1, 0},
                                        {2, 1, 0},
                                        {0, 2, 0},
                                        {1, 2, 0},
                                        {2, 2, 0},
                                        {0, 3, 0},
                                        {1, 3, 0},
                                        {2, 3, 0},
                                        {0, 3, 1},
                                        {1, 3, 1},
                                        {2, 3, 1},
                                        {0, 4, 0},
                                        {1, 4, 0},
                                        {2, 4, 0},
                                        {0, 4, 1},
                                        {1, 4, 1},
                                        {2, 4, 1},
                                        {0, 5, 0},
                                        {1, 5, 0},
                                        {2, 5, 0},
                                        {0, 5, 1},
                                        {1, 5, 1},
                                        {2, 5, 1}};
    BmnGemStripStationSet* stationSet;
    Int_t iStatFirst; // first station to start with
    if      (runPeriod == 5) {
        stationSet = new BmnGemStripStationSet_RunWinter2016(BmnGemStripConfiguration::RunWinter2016);
        iStatFirst = 1;
        mStatMod_Pad[make_pair(1, 0)] =  1;
        mStatMod_Pad[make_pair(2, 0)] =  2;
        mStatMod_Pad[make_pair(3, 0)] =  3;
        mStatMod_Pad[make_pair(4, 0)] =  4;
        mStatMod_Pad[make_pair(4, 1)] = 10;
        mStatMod_Pad[make_pair(5, 0)] =  5;
        mStatMod_Pad[make_pair(5, 1)] = 11;
        mStatMod_Pad[make_pair(6, 0)] =  6;
        mStatMod_Pad[make_pair(6, 1)] = 12;
    }
    else if (runPeriod == 6) {
        stationSet = new BmnGemStripStationSet_RunWinter2016(BmnGemStripConfiguration::RunSpring2017);
        iStatFirst = 0;
        mStatMod_Pad[make_pair(0, 0)] =  1;
        mStatMod_Pad[make_pair(1, 0)] =  2;
        mStatMod_Pad[make_pair(2, 0)] =  3;
        mStatMod_Pad[make_pair(3, 0)] =  4;
        mStatMod_Pad[make_pair(3, 1)] = 10;
        mStatMod_Pad[make_pair(4, 0)] =  5;
        mStatMod_Pad[make_pair(4, 1)] = 11;
        mStatMod_Pad[make_pair(5, 0)] =  6;
        mStatMod_Pad[make_pair(5, 1)] = 12;
    }
    int n = 0;                     // number of iterations counter
    ifstream ifstrm(newAlignCorrFileListFileName);
    string   line;
    while (getline(ifstrm, line)) { // this is in fact a loop over the iterations
        n++;
      //cout <<line<< endl;
    }
    ifstrm.close();
    const Int_t nIts = n;
  //cout <<"nIts = "<<nIts<< endl; // number of iterations

    
    const Int_t nKinds  = 2;
    const Int_t nParams = 3;
    Double_t***** itCorrs = new Double_t****[nKinds];
    for (Int_t iKind=0; iKind < nKinds; iKind++) {
        itCorrs[iKind] = new Double_t***[nParams];
        for (Int_t iPar=0; iPar < nParams; iPar++) {
            Int_t nStats = stationSet->GetNStations();
            itCorrs[iKind][iPar] = new Double_t**[nStats];
            for (Int_t iStat=0; iStat < nStats; iStat++) {
                Int_t nModuls = stationSet->GetGemStation(iStat)->GetNModules();
                itCorrs[iKind][iPar][iStat] = new Double_t*[nModuls];
                for (Int_t iMod=0; iMod < nModuls; iMod++) {
                    itCorrs[iKind][iPar][iStat][iMod] = new Double_t[nIts];
                    for (Int_t iIt=0; iIt < nIts; iIt++) {
                        itCorrs[iKind][iPar][iStat][iMod][iIt] = 0.;
                    }
                }
            }
        }
    }
    Double_t**** itNewCorrErrs = new Double_t***[nParams];
    for (Int_t iPar=0; iPar < nParams; iPar++) {
        Int_t nStats = stationSet->GetNStations();
        itNewCorrErrs[iPar] = new Double_t**[nStats];
        for (Int_t iStat=0; iStat < nStats; iStat++) {
            Int_t nModuls = stationSet->GetGemStation(iStat)->GetNModules();
            itNewCorrErrs[iPar][iStat] = new Double_t*[nModuls];
            for (Int_t iMod=0; iMod < nModuls; iMod++) {
                itNewCorrErrs[iPar][iStat][iMod] = new Double_t[nIts];
                for (Int_t iIt=0; iIt < nIts; iIt++) {
                    itNewCorrErrs[iPar][iStat][iMod][iIt] = 0.;
                }
            }
        }
    }
    TCanvas* canvas = new TCanvas("canvas", "Convergency of alignment", 1200, 1200);
    canvas->Divide(6, 12, 0., 0.);
    TPad* pad;
    Int_t padNr(0);
    gROOT->ForceStyle(kTRUE);
    /*
    gStyle->SetPadTopMargin(0.);
    gStyle->SetPadLeftMargin(0.);
    gStyle->SetPadRightMargin(0.);
    gStyle->SetPadBottomMargin(0.);
    gStyle->SetPadBottomMargin(0.03);
    */
    gStyle->SetLineWidth(1);
    gStyle->SetTitleY(0.980);
    gStyle->SetLineScalePS(0.5);

    canvas->cd(0);
    gPad->SetTopMargin(    0.);
    gPad->SetLeftMargin(   0.);
    gPad->SetRightMargin(  0.);
    gPad->SetBottomMargin( 0.);

    Double_t its[nIts];   // array with the iteration numbers

    TString kinds[] = {"new", "sum"};
    for (Int_t iKind=0; iKind < nKinds; iKind++) {
        string  fnamestrg;
        if (kinds[iKind] == "new")
            fnamestrg = newAlignCorrFileListFileName;
        else
            fnamestrg = sumAlignCorrFileListFileName;
        cout <<"fnamestrg               = "+fnamestrg<< endl;
        ifstream corrFileListFstr(fnamestrg.c_str());
        fnamestrg = "";
        TString fname;
        Int_t   it(-1); // iteration counter: starts with 0

        while (corrFileListFstr >> fnamestrg) { // this is in fact the main loop over the iterations
            fname = fnamestrg;
            cout <<"fname                   = "+fname<< endl;
            TFile* corrFile = new TFile(fname.Data());
            TTree* corrTree = (TTree*)corrFile->Get("cbmsim");
          //cout <<"corrTree->Print()"<< endl;
          //corrTree->Print();
            TClonesArray* corrs = new TClonesArray("BmnGemAlignmentCorrections");
            corrTree->SetBranchAddress("BmnGemAlignmentCorrections",  &corrs);

            for (Int_t iEntry=0; iEntry < (Int_t)corrTree->GetEntries(); iEntry++) {
              //cout <<TString::Format("iEntry = %i5", iEntry)<< endl;
                corrTree->GetEntry(iEntry);
                if ((Int_t)corrs->GetEntriesFast() > 0) {
                  //cout <<"(Int_t)corrs->GetEntriesFast() = "<<(Int_t)corrs->GetEntriesFast()<< endl;
                    break;
                }
            }

            it += 1;
            its[it] = (Double_t)it + 1.; // fill array of the iteration numbers (stating with 1)
          //cout <<"its["<<it<<"] = "<<its[it]<< endl;

            for (Int_t iCorr=0; iCorr < (Int_t)corrs->GetEntriesFast(); iCorr++) {
              //cout <<"iCorr  = "<<iCorr<< endl;
                BmnGemAlignmentCorrections* tmp = (BmnGemAlignmentCorrections*)(corrs->UncheckedAt(iCorr));
                Int_t iStat = tmp->GetStation();
              //cout <<"iStat  = "<<iStat<< endl;
                Int_t iMod  = tmp->GetModule();
              //cout <<"iMod   = "<<iMod << endl;
                itCorrs[iKind][0][iStat][iMod][it] = -tmp->GetCorrections().X(),
              //cout <<"itCorrs["<<iKind<<"]["<<0<<"]["<<iStat<<"]["<<iMod<<"]["<<it<<"] = "<<TString::Format("% 14.11f", itCorrs[iKind][0][iStat][iMod][it])<< endl;
                itCorrs[iKind][1][iStat][iMod][it] = -tmp->GetCorrections().Y(),
              //cout <<"itCorrs["<<iKind<<"]["<<1<<"]["<<iStat<<"]["<<iMod<<"]["<<it<<"] = "<<TString::Format("% 14.11f", itCorrs[iKind][1][iStat][iMod][it])<< endl;
                itCorrs[iKind][2][iStat][iMod][it] = -tmp->GetCorrections().Z());
              //cout <<"itCorrs["<<iKind<<"]["<<2<<"]["<<iStat<<"]["<<iMod<<"]["<<it<<"] = "<<TString::Format("% 14.11f", itCorrs[iKind][2][iStat][iMod][it])<< endl;
            
            }
            delete corrFile;
            delete corrs;

            if (kinds[iKind] == "new") {
                // extract millepede's parameter errors:
                TString mlpdResFname = fname;
                mlpdResFname = mlpdResFname.ReplaceAll(".root", ".res");
                mlpdResFname = mlpdResFname.ReplaceAll("_it", "_xyz_it");
                mlpdResFname = "Millepede_"+mlpdResFname;
              //cout <<"mlpdResFname  = "<< mlpdResFname<< endl;
                ifstream ifstrmMlpdRes(mlpdResFname.Data());
              //while (ifstrmMlpdRes >> line) {
                getline(ifstrmMlpdRes, line);          // skip the title line
                while (getline(ifstrmMlpdRes, line)) { // loop over lines in the Millepede<...>.res file
                  //cout <<line<< endl;
                    stringstream stgstm(line);
                    int parNr; double parVal, presigma, differ, parErr;
                    stgstm >> parNr >> parVal >> presigma;
                    if (presigma > 0.) {
                        stgstm >> differ >> parErr; }
                    else {
                        parErr = 0.;
                    }
                  //cout <<TString::Format("parNr = %2i parVal = % 14.11f parErr = % 14.11f", parNr, parVal, parErr)<< endl;
                    if      (runPeriod == 5) {
                        Int_t jPar  = mlpdParNr_What5[parNr-1][0];
                        Int_t jStat = mlpdParNr_What5[parNr-1][1];
                        Int_t jMod  = mlpdParNr_What5[parNr-1][2];
                    }
                    else if (runPeriod == 6) {
                        Int_t jPar  = mlpdParNr_What6[parNr-1][0];
                        Int_t jStat = mlpdParNr_What6[parNr-1][1];
                        Int_t jMod  = mlpdParNr_What6[parNr-1][2];
                    }
                  //cout <<"jPar  = "<<jPar<<" jStat = "<<jStat<<" jMod  = "<<jMod<<<< endl;
                    itNewCorrErrs[jPar][jStat][jMod][it] = parErr;
                  //cout <<"itNewCorrErrs[   "<<jPar<<"]["<<jStat<<"]["<<jMod<<"]["<<it<<"]              = "<<TString::Format("% 14.11f", itNewCorrErrs[jPar][jStat][jMod][it])<< endl;
                }
            }
        }
    }
  //cout << endl;
    TString xyz[] = {"X", "Y", "Z"};
    Double_t alCorrs[   nIts];
  //Double_t alCorrs[   10];
    Double_t alCorrErrs[nIts];
  //Double_t alCorrErrs[10];
    for (Int_t iKind=0; iKind < nKinds; iKind++) {
        for (Int_t iPar=0; iPar < nParams; iPar++) {
            Int_t nStats = stationSet->GetNStations();
            for (Int_t iStat=iStatFirst; iStat < nStats; iStat++) {
                Int_t nModuls = stationSet->GetGemStation(iStat)->GetNModules();
                for (Int_t iMod=0; iMod < nModuls; iMod++) {
                  //Double_t alCorrs = new Double_t[nIts];
                    for (Int_t iIt=0; iIt < nIts; iIt++) {
                      //cout <<"itCorrs[      "<<iKind<<"]["<<iPar<<"]["<<iStat<<"]["<<iMod<<"]["<<iIt<<"] = "<<TString::Format("% 14.11f", itCorrs[iKind][iPar][iStat][iMod][iIt])<< endl;
                        alCorrs[iIt] = itCorrs[iKind][iPar][iStat][iMod][iIt];
                      //cout <<TString::Format("alCorrs[                 %2i] = % 14.11f", iIt, alCorrs[iIt])<< endl;
                        if (kinds[iKind] == "new") {
                          //cout <<"itNewCorrErrs[   "<<iPar<<"]["<<iStat<<"]["<<iMod<<"]["<<iIt<<"] = "<<TString::Format("% 14.11f", itNewCorrErrs[iPar][iStat][iMod][iIt])<< endl;
                            if (itNewCorrErrs[iPar][iStat][iMod][iIt] > 0.)
                                alCorrErrs[iIt] = itNewCorrErrs[iPar][iStat][iMod][iIt];
                            else
                                alCorrErrs[iIt] = 0.;
                        }
                        else {
                           alCorrErrs[iIt] = 0.;
                        }
                      //cout <<TString::Format("alCorrErrs[              %2i] = % 14.11f", iIt, alCorrErrs[iIt])<< endl;
                    }
                  //cout <<"nIts = "<<nIts<< endl;
                  //res = mStatMod_Pad.find(make_pair(iStat, iMod));
                  //padNr =  res + iPar*12 + iKind*36;
                  //cout <<"mStatMod_Pad[make_pair("<<iStat<<", "<<iMod<<")] = "<<mStatMod_Pad[make_pair(iStat, iMod)]<< endl;
                    padNr =   mStatMod_Pad[make_pair(iStat, iMod)] + iPar*12 + iKind*36;
                  //cout <<"padNr = "<<padNr<< endl;
                    canvas->cd(padNr);
                  //pad = (TPad*)canvas->cd(padNr);
                  //gPad->SetFrameBorderMode(0);
                  //gPad->SetBorderMode(0);
                  //gPad->SetBorderSize(0);
                  //gPad->SetTopMargin(   0.01 );
                  //gPad->SetLeftMargin(  0.08 );
                  //gPad->SetRightMargin( 0.005);
                  //gPad->SetBottomMargin(0.05 );

                    TGraphErrors* grer = new TGraphErrors(nIts, its, alCorrs, 0, alCorrErrs);
                    if (kinds[iKind] == "new") {
                        grer->SetMaximum( 0.05);
                        grer->SetMinimum(-0.05); }
                    else {
                        grer->SetMaximum( 0.5);
                        grer->SetMinimum(-0.5);
                    }
                    grer->SetTitle(kinds[iKind]+" "+xyz[iPar]+" corrections starting from "+startAlignFrom+" stat "+TString::Itoa(iStat, 10)+" mod "+TString::Itoa(iMod, 10));

                    grer->GetXaxis()->SetLimits(-0., (Double_t)nIts);
                    grer->GetXaxis()->SetNdivisions(nIts);
                  //grer->GetXaxis()->SetTitle("iteration number");
                  //grer->GetXaxis()->SetTitleOffset(0.6);
                  //grer->GetXaxis()->SetTitleSize(0.03);
                  //grer->GetXaxis()->SetLabelSize(0.03);
                  //grer->GetYaxis()->SetLabelSize(0.03);
                    grer->SetFillStyle(1001);
                    grer->SetFillColor(kCyan-10);
                  //grer->Draw("3AL");
                  //canvas->cd(padNr);

                    TGraph* gr = new TGraph(nIts, its, alCorrs);
                    if (kinds[iKind] == "new") {
                        gr->SetMaximum( 0.05);
                        gr->SetMinimum(-0.05); }
                    else {
                        gr->SetMaximum( 0.5);
                        gr->SetMinimum(-0.5);
                    }
                    gr->SetTitle(kinds[iKind]+" "+xyz[iPar]+" corrections starting from "+startAlignFrom+" stat "+TString::Itoa(iStat, 10)+" mod "+TString::Itoa(iMod, 10));

                    gr->GetXaxis()->SetLimits(-0., (Double_t)nIts);
                    gr->GetXaxis()->SetNdivisions(nIts);
                  //gr->GetXaxis()->SetTitle("iteration number");
                  //gr->GetXaxis()->SetTitleOffset(0.6);
                  //gr->GetXaxis()->SetTitleSize(0.03);
                  //gr->GetXaxis()->SetLabelSize(0.03);
                  //gr->GetYaxis()->SetLabelSize(0.03);
                    gr->SetFillStyle(4000);
                    gr->SetFillColor(0);
                  //gr->Draw("AL");
                  //canvas->cd(padNr);

                    TMultiGraph* mg = new TMultiGraph();
                    mg->Add(grer,"A3");
                    mg->Add(gr,  "AL");
                    if (kinds[iKind] == "new") {
                        mg->SetMaximum( 0.05);
                        mg->SetMinimum(-0.05); }
                    else {
                        mg->SetMaximum( 0.5);
                        mg->SetMinimum(-0.5);
                    }
                    mg->SetTitle(kinds[iKind]+" "+xyz[iPar]+" corrections starting from "+startAlignFrom+" stat "+TString::Itoa(iStat, 10)+" mod "+TString::Itoa(iMod, 10));

                    mg->Draw("A");
                  //canvas->cd(padNr);
                    gPad->Modified();

                    gPad->SetFrameBorderMode(0);
                    gPad->SetBorderMode(0);
                    gPad->SetBorderSize(0);
                    gPad->SetTopMargin(   0.01 );
                    gPad->SetLeftMargin(  0.08 );
                    gPad->SetRightMargin( 0.005);
                  //gPad->SetBottomMargin(0.05 );
                    gPad->SetBottomMargin(0.1 );

                    mg->GetXaxis()->SetLimits(-0., (Double_t)nIts);
                    mg->GetXaxis()->SetNdivisions(nIts);
                    mg->GetXaxis()->SetTitle("iteration number");
                    mg->GetXaxis()->SetTitleOffset(0.6);
                  //mg->GetXaxis()->SetTitleSize(0.03);
                  //mg->GetXaxis()->SetLabelSize(0.03);
                  //mg->GetYaxis()->SetLabelSize(0.03);
                    mg->Draw("A");
                  //canvas->cd(padNr);
                    gPad->Modified();

                    TLine* zerolevel = new TLine(0., 0., (Double_t)nIts, 0.);
                    zerolevel->SetLineColor(kBlue);
                    zerolevel->SetLineStyle(3);
                    zerolevel->Draw();
                    gPad->Modified();
                  //canvas->cd(padNr);
                  //gPad->Modified();
                  //gPad->Update();
                  //delete grer;
                  //delete gr;
                  //delete mg;
                }
            }
        }
    }
    canvas->Update();
    canvas->SaveAs(alignCorrPlotsFileName.ReplaceAll("root", "pdf" ));
    canvas->SaveAs(alignCorrPlotsFileName.ReplaceAll("pdf",  "root"));
    canvas->SaveAs(alignCorrPlotsFileName.ReplaceAll("root", "eps" ));
}
