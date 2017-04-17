// -----------------------------------------------------------------------------
// This macro plots evolution of the new and cumulative alignment corrections
// vs. iteration number.
//
//
// Anatoly.Solomin@jinr.ru 2017-02-16

#  include   <map>

void CreateItCorrs    (Double_t***** itCorrs,       Int_t nKinds, Int_t nParams, Int_t nIts, BmnGemStripStationSet* stationSet);
void CreateItCorrErrs (Double_t****  itNewCorrErrs,               Int_t nParams, Int_t nIts, BmnGemStripStationSet* stationSet);
void FillItCorrsItErrs(TString       newAlignCorrFileListFileName,
                       Double_t***** itCorrs,
                       Double_t****  itNewCorrErrs,
                       Int_t         iKind,
                       int const     mlpdParNr_What[][3]);

void plot_align_corrections_bmn(TString new1AlignCorrFileListFileName,
                                TString new2AlignCorrFileListFileName = "")
{
    TString sum1AlignCorrFileListFileName = new1AlignCorrFileListFileName;
    sum1AlignCorrFileListFileName         = sum1AlignCorrFileListFileName.ReplaceAll("new", "sum");
    TString sum2AlignCorrFileListFileName;
    if (new2AlignCorrFileListFileName != "") {
        sum2AlignCorrFileListFileName     = new2AlignCorrFileListFileName;
        sum2AlignCorrFileListFileName     = sum2AlignCorrFileListFileName.ReplaceAll("new", "sum");
    }
    TString alignCorrPlotsFileName        = new1AlignCorrFileListFileName;
    alignCorrPlotsFileName                = alignCorrPlotsFileName.ReplaceAll("filelist_", "");
    alignCorrPlotsFileName                = alignCorrPlotsFileName.ReplaceAll("new_align", "corr_plots");
    alignCorrPlotsFileName                = alignCorrPlotsFileName.ReplaceAll("txt", "root");
    if (new2AlignCorrFileListFileName != "") {
        alignCorrPlotsFileName            = alignCorrPlotsFileName.ReplaceAll("fromdefault", "fromscratch_vs_fromdefault");
    }

    // get run period
    TString substr = new1AlignCorrFileListFileName("bmn_run[0-9]+");
    substr         = substr(                              "[0-9]+");
    int runPeriod  = substr.Atoi();

    TString new1StartAlignFrom = new1AlignCorrFileListFileName("from[a-z]+");
    new1StartAlignFrom         = "from "+new1StartAlignFrom.ReplaceAll("from", "");
    TString new2StartAlignFrom;
    if (new2AlignCorrFileListFileName != "") {
        new2StartAlignFrom     = new2AlignCorrFileListFileName("from[a-z]+");
        new2StartAlignFrom     = "from "+new2StartAlignFrom.ReplaceAll("from", "");
    }

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    map<const pair<int, int>, int> mStatMod_Pad;

    int mlpdParNr_What5[30][3] = {{0, 0, 0},
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
    int mlpdParNr_What6[27][3] = {{0, 0, 0},
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
    Int_t iStatFirst; // first GEM station to start with
    Int_t nStations;  // number of GEM stations
    if      (runPeriod == 5) {
        stationSet = new BmnGemStripStationSet_RunWinter2016(BmnGemStripConfiguration::RunWinter2016);
        iStatFirst = 1;
        nStations  = 7;
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
        stationSet = new BmnGemStripStationSet_RunSpring2017(BmnGemStripConfiguration::RunSpring2017);
        iStatFirst = 0;
        nStations  = 6;
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
    ifstream ifstrm(new1AlignCorrFileListFileName);
    string   line;
    int n = 0;                     // number of iterations counter
    while (getline(ifstrm, line)) { // this is in fact a loop over the iterations
        n++;
      //cout <<line<< endl;
    }
    ifstrm.close();
    const Int_t nIts = n;
    Double_t its[nIts];   // array with the iteration numbers
    for (int i=0; i<nIts; i++)
        its[i] = (Double_t)i + 1.;  // fill array of the iteration numbers (stating with 1)
  //cout <<"nIts = "<<nIts<< endl; // number of iterations


    const Int_t nKinds  = 2;
    const Int_t nParams = 3;
  //Double_t***** itCorrs1       = new Double_t****[nKinds];
  //Double_t****  itNewCorrErrs1 = new Double_t***[nParams];
  //Double_t***** itCorrs2       = new Double_t****[nKinds];
  //Double_t****  itNewCorrErrs2 = new Double_t***[nParams];
    Double_t***** itCorrs1       ;
    Double_t****  itNewCorrErrs1 ;
    Double_t***** itCorrs2       ;
    Double_t****  itNewCorrErrs2 ;
  //CreateItCorrs(       itCorrs1,       nKinds, nParams, nIts, stationSet);
    itCorrs1 = new Double_t****[nKinds];
    for (Int_t iKind=0; iKind < nKinds; iKind++) {
        itCorrs1[iKind] = new Double_t***[nParams];
        for (Int_t iPar=0; iPar < nParams; iPar++) {
            Int_t nStats = stationSet->GetNStations();
            itCorrs1[iKind][iPar] = new Double_t**[nStats];
            for (Int_t iStat=0; iStat < nStats; iStat++) {
                Int_t nModuls = stationSet->GetGemStation(iStat)->GetNModules();
                itCorrs1[iKind][iPar][iStat] = new Double_t*[nModuls];
                for (Int_t iMod=0; iMod < nModuls; iMod++) {
                    itCorrs1[iKind][iPar][iStat][iMod] = new Double_t[nIts];
                    for (Int_t iIt=0; iIt < nIts; iIt++) {
                        itCorrs1[iKind][iPar][iStat][iMod][iIt] = 0.;
                    }
                }
            }
        }
    }
  //CreateItCorrErrs(    itNewCorrErrs1,         nParams, nIts, stationSet);
    itNewCorrErrs1 = new Double_t***[nParams];
    for (Int_t iPar=0; iPar < nParams; iPar++) {
        Int_t nStats = stationSet->GetNStations();
        itNewCorrErrs1[iPar] = new Double_t**[nStats];
        for (Int_t iStat=0; iStat < nStats; iStat++) {
            Int_t nModuls = stationSet->GetGemStation(iStat)->GetNModules();
            itNewCorrErrs1[iPar][iStat] = new Double_t*[nModuls];
            for (Int_t iMod=0; iMod < nModuls; iMod++) {
                itNewCorrErrs1[iPar][iStat][iMod] = new Double_t[nIts];
                for (Int_t iIt=0; iIt < nIts; iIt++) {
                    itNewCorrErrs1[iPar][iStat][iMod][iIt] = 0.;
                }
            }
        }
    }
    if (new2AlignCorrFileListFileName != "") {
      //CreateItCorrs(   itCorrs2,       nKinds, nParams, nIts, stationSet);
        itCorrs2 = new Double_t****[nKinds];
        for (Int_t iKind=0; iKind < nKinds; iKind++) {
            itCorrs2[iKind] = new Double_t***[nParams];
            for (Int_t iPar=0; iPar < nParams; iPar++) {
                Int_t nStats = stationSet->GetNStations();
                itCorrs2[iKind][iPar] = new Double_t**[nStats];
                for (Int_t iStat=0; iStat < nStats; iStat++) {
                    Int_t nModuls = stationSet->GetGemStation(iStat)->GetNModules();
                    itCorrs2[iKind][iPar][iStat] = new Double_t*[nModuls];
                    for (Int_t iMod=0; iMod < nModuls; iMod++) {
                        itCorrs2[iKind][iPar][iStat][iMod] = new Double_t[nIts];
                        for (Int_t iIt=0; iIt < nIts; iIt++) {
                            itCorrs2[iKind][iPar][iStat][iMod][iIt] = 0.;
                        }
                    }
                }
            }
        }
      //CreateItCorrErrs(itNewCorrErrs2,         nParams, nIts, stationSet);
        itNewCorrErrs2 = new Double_t***[nParams];
        for (Int_t iPar=0; iPar < nParams; iPar++) {
            Int_t nStats = stationSet->GetNStations();
            itNewCorrErrs2[iPar] = new Double_t**[nStats];
            for (Int_t iStat=0; iStat < nStats; iStat++) {
                Int_t nModuls = stationSet->GetGemStation(iStat)->GetNModules();
                itNewCorrErrs2[iPar][iStat] = new Double_t*[nModuls];
                for (Int_t iMod=0; iMod < nModuls; iMod++) {
                    itNewCorrErrs2[iPar][iStat][iMod] = new Double_t[nIts];
                    for (Int_t iIt=0; iIt < nIts; iIt++) {
                        itNewCorrErrs2[iPar][iStat][iMod][iIt] = 0.;
                    }
                }
            }
        }
    }

    TString kinds[2] = {"new", "sum"};
    if      (runPeriod == 5) {
        for (Int_t iKind=0; iKind < nKinds; iKind++) {
            FillItCorrsItErrs(    new1AlignCorrFileListFileName,
                                  itCorrs1,
                                  itNewCorrErrs1,
                                  iKind,
                                  mlpdParNr_What5);
            if (new2AlignCorrFileListFileName != "") {
                FillItCorrsItErrs(new2AlignCorrFileListFileName,
                                  itCorrs2,
                                  itNewCorrErrs2,
                                  iKind,
                                  mlpdParNr_What5);
            }
        }
    }
    else if (runPeriod == 6) {
        for (Int_t iKind=0; iKind < nKinds; iKind++) {
            FillItCorrsItErrs(    new1AlignCorrFileListFileName,
                                  itCorrs1,
                                  itNewCorrErrs1,
                                  iKind,
                                  mlpdParNr_What6);
            if (new2AlignCorrFileListFileName != "") {
                FillItCorrsItErrs(new2AlignCorrFileListFileName,
                                  itCorrs2,
                                  itNewCorrErrs2,
                                  iKind,
                                  mlpdParNr_What6);
            }
        }
    }

    TCanvas* canvas = new TCanvas("canvas", "Convergency of alignment", nStations*200, 1200);
    canvas->Divide(nStations, 12, 0., 0.);
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
    cout << endl;
    TString xyz[] = {"X", "Y", "Z"};
    Double_t alCorrs1[   nIts];
    Double_t alCorrErrs1[nIts];
    Double_t alCorrs2[   nIts];
    Double_t alCorrErrs2[nIts];
    for (Int_t iKind=0; iKind < nKinds; iKind++) {
        for (Int_t iPar=0; iPar < nParams; iPar++) {
            Int_t nStats = stationSet->GetNStations();
            for (Int_t iStat=iStatFirst; iStat < nStats; iStat++) {
                Int_t nModuls = stationSet->GetGemStation(iStat)->GetNModules();
                for (Int_t iMod=0; iMod < nModuls; iMod++) {
                  //Double_t alCorrs = new Double_t[nIts];
                    for (Int_t iIt=0; iIt < nIts; iIt++) {
                      //cout <<"itCorrs1[      "<<iKind<<"]["<<iPar<<"]["<<iStat<<"]["<<iMod<<"]["<<iIt<<"] = "<<TString::Format("% 14.11f", itCorrs1[iKind][iPar][iStat][iMod][iIt])<< endl;
                        alCorrs1[iIt] = itCorrs1[iKind][iPar][iStat][iMod][iIt];
                      //cout <<TString::Format("alCorrs1[                 %2i] = % 14.11f", iIt, alCorrs1[iIt])<< endl;
                        if (kinds[iKind] == "new") {
                          //cout <<"itNewCorrErrs1[   "<<iPar<<"]["<<iStat<<"]["<<iMod<<"]["<<iIt<<"] = "<<TString::Format("% 14.11f", itNewCorrErrs1[iPar][iStat][iMod][iIt])<< endl;
                            if (itNewCorrErrs1[iPar][iStat][iMod][iIt] > 0.)
                                alCorrErrs1[iIt] = itNewCorrErrs1[iPar][iStat][iMod][iIt];
                            else
                                alCorrErrs1[iIt] = 0.;
                        }
                        else {
                           alCorrErrs1[iIt] = 0.;
                        }
                      //cout <<TString::Format("alCorrErrs1[              %2i] = % 14.11f", iIt, alCorrErrs1[iIt])<< endl;
                        if (new2AlignCorrFileListFileName != "") {
                          //cout <<"itCorrs2[      "<<iKind<<"]["<<iPar<<"]["<<iStat<<"]["<<iMod<<"]["<<iIt<<"] = "<<TString::Format("% 14.11f", itCorrs2[iKind][iPar][iStat][iMod][iIt])<< endl;
                            alCorrs2[iIt] = itCorrs2[iKind][iPar][iStat][iMod][iIt];
                          //cout <<TString::Format("alCorrs2[                 %2i] = % 14.11f", iIt, alCorrs2[iIt])<< endl;
                            if (kinds[iKind] == "new") {
                              //cout <<"itNewCorrErrs2[   "<<iPar<<"]["<<iStat<<"]["<<iMod<<"]["<<iIt<<"] = "<<TString::Format("% 14.11f", itNewCorrErrs2[iPar][iStat][iMod][iIt])<< endl;
                                if (itNewCorrErrs2[iPar][iStat][iMod][iIt] > 0.)
                                    alCorrErrs2[iIt] = itNewCorrErrs2[iPar][iStat][iMod][iIt];
                                else
                                    alCorrErrs2[iIt] = 0.;
                            }
                            else {
                               alCorrErrs2[iIt] = 0.;
                            }
                          //cout <<TString::Format("alCorrErrs2[              %2i] = % 14.11f", iIt, alCorrErrs2[iIt])<< endl;
                        }
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

                    TGraphErrors* grer1 = new TGraphErrors(nIts, its, alCorrs1, 0, alCorrErrs1);
                    if (kinds[iKind] == "new") {
                        grer1->SetMaximum( 0.5 );
                        grer1->SetMinimum(-0.5 ); }
                    else {
                        grer1->SetMaximum( 0.5);
                        grer1->SetMinimum(-0.5);
                    }
                    grer1->SetTitle(kinds[iKind]+" "+xyz[iPar]+" corrections starting "+new1StartAlignFrom+" stat "+TString::Itoa(iStat, 10)+" mod "+TString::Itoa(iMod, 10));

                    grer1->GetXaxis()->SetLimits(-0., (Double_t)nIts);
                    grer1->GetXaxis()->SetNdivisions(nIts);
                  //grer1->GetXaxis()->SetTitle("iteration number");
                  //grer1->GetXaxis()->SetTitleOffset(0.6);
                  //grer1->GetXaxis()->SetTitleSize(0.03);
                  //grer1->GetXaxis()->SetLabelSize(0.03);
                  //grer1->GetYaxis()->SetLabelSize(0.03);
                    grer1->SetFillStyle(1001);
                    grer1->SetFillColor(kCyan-10);
                  //grer1->Draw("3AL");
                  //canvas->cd(padNr);

                    TGraph* gr1 = new TGraph(nIts, its, alCorrs1);
                    if (kinds[iKind] == "new") {
                        gr1->SetMaximum( 0.5 );
                        gr1->SetMinimum(-0.5 ); }
                    else {
                        gr1->SetMaximum( 0.5);
                        gr1->SetMinimum(-0.5);
                    }
                    gr1->SetTitle(kinds[iKind]+" "+xyz[iPar]+" corrections starting "+new1StartAlignFrom+" stat "+TString::Itoa(iStat, 10)+" mod "+TString::Itoa(iMod, 10));

                    gr1->GetXaxis()->SetLimits(-0., (Double_t)nIts);
                    gr1->GetXaxis()->SetNdivisions(nIts);
                  //gr1->GetXaxis()->SetTitle("iteration number");
                  //gr1->GetXaxis()->SetTitleOffset(0.6);
                  //gr1->GetXaxis()->SetTitleSize(0.03);
                  //gr1->GetXaxis()->SetLabelSize(0.03);
                  //gr1->GetYaxis()->SetLabelSize(0.03);
                    gr1->SetFillStyle(4000);
                    gr1->SetFillColor(0);
                  //gr1->Draw("AL");
                  //canvas->cd(padNr);
                    if (new2AlignCorrFileListFileName != "") {
                        TGraphErrors* grer2 = new TGraphErrors(nIts, its, alCorrs2, 0, alCorrErrs2);
                        if (kinds[iKind] == "new") {
                            grer2->SetMaximum( 0.5 );
                            grer2->SetMinimum(-0.5 ); }
                        else {
                            grer2->SetMaximum( 0.5);
                            grer2->SetMinimum(-0.5);
                        }
                        grer2->SetTitle(kinds[iKind]+" "+xyz[iPar]+" corrections starting "+new1StartAlignFrom+" stat "+TString::Itoa(iStat, 10)+" mod "+TString::Itoa(iMod, 10));

                        grer2->GetXaxis()->SetLimits(-0., (Double_t)nIts);
                        grer2->GetXaxis()->SetNdivisions(nIts);
                      //grer2->GetXaxis()->SetTitle("iteration number");
                      //grer2->GetXaxis()->SetTitleOffset(0.6);
                      //grer2->GetXaxis()->SetTitleSize(0.03);
                      //grer2->GetXaxis()->SetLabelSize(0.03);
                      //grer2->GetYaxis()->SetLabelSize(0.03);
                        grer2->SetFillStyle(1001);
                        grer2->SetFillColor(kMagenta-10);
                      //grer2->Draw("3AL");
                      //canvas->cd(padNr);

                        TGraph* gr2 = new TGraph(nIts, its, alCorrs2);
                        if (kinds[iKind] == "new") {
                            gr2->SetMaximum( 0.5 );
                            gr2->SetMinimum(-0.5 ); }
                        else {
                            gr2->SetMaximum( 0.5);
                            gr2->SetMinimum(-0.5);
                        }
                        gr2->SetTitle(kinds[iKind]+" "+xyz[iPar]+" corrections starting "+new1StartAlignFrom+" stat "+TString::Itoa(iStat, 10)+" mod "+TString::Itoa(iMod, 10));

                        gr2->GetXaxis()->SetLimits(-0., (Double_t)nIts);
                        gr2->GetXaxis()->SetNdivisions(nIts);
                      //gr2->GetXaxis()->SetTitle("iteration number");
                      //gr2->GetXaxis()->SetTitleOffset(0.6);
                      //gr2->GetXaxis()->SetTitleSize(0.03);
                      //gr2->GetXaxis()->SetLabelSize(0.03);
                      //gr2->GetYaxis()->SetLabelSize(0.03);
                        gr2->SetFillStyle(4000);
                        gr2->SetFillColor(0);
                        gr2->SetLineColor(kRed+1);
                      //gr2->Draw("AL");
                      //canvas->cd(padNr);
                    }
                    TMultiGraph* mg = new TMultiGraph();
                    mg->Add(grer1,"A3");
                    mg->Add(gr1,  "AL");
                    mg->SetTitle(kinds[iKind]+" "+xyz[iPar]+" corrections starting "+new1StartAlignFrom+" stat "+TString::Itoa(iStat, 10)+" mod "+TString::Itoa(iMod, 10));

                    if (new2AlignCorrFileListFileName != "") {
                        mg->Add(grer2,"A3");
                        mg->Add(gr2,  "AL");
                        mg->SetTitle(kinds[iKind]+" "+xyz[iPar]+" corrections starting "+new1StartAlignFrom+" and "+new2StartAlignFrom+" stat "+TString::Itoa(iStat, 10)+" mod "+TString::Itoa(iMod, 10));
                    }
                    if (kinds[iKind] == "new") {
                        mg->SetMaximum( 0.5 );
                        mg->SetMinimum(-0.5 ); }
                    else {
                        mg->SetMaximum( 0.5);
                        mg->SetMinimum(-0.5);
                    }

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

void CreateItCorrs(Double_t***** itCorrs, Int_t nKinds, Int_t nParams, Int_t nIts, BmnGemStripStationSet* stationSet)
{
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
                      //cout <<"itCorrs["<<iKind<<"]["<<iPar<<"]["<<iStat<<"]["<<iMod<<"]["<<iIt<<"] = "<<TString::Format("% 14.11f", itCorrs[iKind][iPar][iStat][iMod][iIt])<< endl;
                    }
                }
            }
        }
    }
}

void CreateItCorrErrs(Double_t**** itNewCorrErrs, Int_t nParams, Int_t nIts, BmnGemStripStationSet* stationSet)
{
    Double_t****  itNewCorrErrs = new Double_t***[nParams];
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
}

void FillItCorrsItErrs(TString       newAlignCorrFileListFileName,
                       Double_t***** itCorrs,
                       Double_t****  itNewCorrErrs,
                       Int_t         iKind,
                       int           mlpdParNr_What[][3])
{
    cout <<"newAlignCorrFileListFileName = "<<newAlignCorrFileListFileName<< endl;
  //cout <<TString::Format("iKind = %i", iKind)<< endl;
    TString  fnameTStrg = newAlignCorrFileListFileName;
    if (iKind == 1)
        fnameTStrg = fnameTStrg.ReplaceAll("new", "sum");
    string  fnamestrg = fnameTStrg;
    cout <<"fnamestrg                    = "+fnamestrg<< endl;
    ifstream corrFileListFstr(fnamestrg.c_str());
    fnamestrg = "";
    TString fname;
    Int_t   iIt(-1); // iteration counter: starts with 0
    while (corrFileListFstr >> fnamestrg) { // this is in fact the main loop over the iterations
        fname = fnamestrg;
        cout <<"fname                        = "+fname<< endl;
        TFile* corrFile = new TFile(fname.Data());
        TTree* corrTree = (TTree*)corrFile->Get("cbmsim");
      //cout <<"corrTree->Print()"<< endl;
      //corrTree->Print();
      //TClonesArray* corrs = new TClonesArray("BmnGemAlignCorrections");
        TClonesArray* corrs = NULL;
        corrTree->SetBranchAddress("BmnGemAlignCorrections",  &corrs);

        for (Int_t iEntry=0; iEntry < (Int_t)corrTree->GetEntries(); iEntry++) {
          //cout <<TString::Format("iEntry = % 5i", iEntry)<< endl;
            corrTree->GetEntry(iEntry);
            if ((Int_t)corrs->GetEntriesFast() > 0) {
              //cout <<TString::Format("iEntry = % 5i", iEntry)<< endl;
              //cout <<"(Int_t)corrs->GetEntriesFast() = "<<(Int_t)corrs->GetEntriesFast()<< endl;
                break;
            }
        }
        iIt++;
      //cout <<TString::Format(    "iIt    = % 2i", iIt   )<< endl;
        for (Int_t iCorr=0; iCorr < (Int_t)corrs->GetEntriesFast(); iCorr++) {
          //cout <<TString::Format("iCorr  = % 2i", iCorr)<< endl;
            BmnGemAlignCorrections* tmp = (BmnGemAlignCorrections*)(corrs->UncheckedAt(iCorr));
            Int_t iStat = tmp->GetStation();
          //cout <<TString::Format("iStat  = % 2i", iStat)<< endl;
            Int_t iMod  = tmp->GetModule();
          //cout <<TString::Format("iMod   = % 2i", iMod )<< endl;
            itCorrs[iKind][0][iStat][iMod][iIt] = tmp->GetCorrections().X(),
          //cout <<"itCorrs["<<iKind<<"]["<<0<<"]["<<iStat<<"]["<<iMod<<"]["<<iIt<<"] = "<<TString::Format("% 14.11f", itCorrs[iKind][0][iStat][iMod][iIt])<< endl;
            itCorrs[iKind][1][iStat][iMod][iIt] = tmp->GetCorrections().Y(),
          //cout <<"itCorrs["<<iKind<<"]["<<1<<"]["<<iStat<<"]["<<iMod<<"]["<<iIt<<"] = "<<TString::Format("% 14.11f", itCorrs[iKind][1][iStat][iMod][iIt])<< endl;
            itCorrs[iKind][2][iStat][iMod][iIt] = tmp->GetCorrections().Z());
          //cout <<"itCorrs["<<iKind<<"]["<<2<<"]["<<iStat<<"]["<<iMod<<"]["<<iIt<<"] = "<<TString::Format("% 14.11f", itCorrs[iKind][2][iStat][iMod][iIt])<< endl;
        }
        delete corrFile;
      //delete corrs;

        if (iKind == 0) {
            // extract millepede's parameter errors:
            TString mlpdResFname = fname;
            mlpdResFname = mlpdResFname.ReplaceAll("_it",   "_millepede_it");
            mlpdResFname = mlpdResFname.ReplaceAll(".root", ".res");
            cout <<"mlpdResFname                 = "<< mlpdResFname<< endl;
            ifstream ifstrmMlpdRes(mlpdResFname.Data());
            string   line;

          //while (ifstrmMlpdRes >> line) {
            getline(ifstrmMlpdRes, line);          // skip the title line
            cout <<line<< endl;
            while (getline(ifstrmMlpdRes, line)) { // loop over lines in the Millepede<...>.res file
                cout <<line<< endl;
                stringstream stgstm(line);
                int parNr; double parVal, presigma, differ, parErr;
                stgstm >> parNr >> parVal >> presigma;
                if (presigma > 0.) { // presima == -1. means not usable line
                    if (abs(parVal) > 1.E-9) {
                        stgstm >> differ >> parErr; }
                    else {
                        parErr = 0.;
                    }
                    cout <<TString::Format("parNr = %2i parVal = % 14.11f parErr = % 14.11f", parNr, parVal, parErr)<< endl;
                    Int_t jPar  = mlpdParNr_What[parNr-1][0];
                    Int_t jStat = mlpdParNr_What[parNr-1][1];
                    Int_t jMod  = mlpdParNr_What[parNr-1][2];
                    cout <<"jPar  = "<<jPar<<" jStat = "<<jStat<<" jMod  = "<<jMod<< endl;
                    itNewCorrErrs[jPar][jStat][jMod][iIt] = parErr;
                    cout <<"itNewCorrErrs[   "<<jPar<<"]["<<jStat<<"]["<<jMod<<"]["<<iIt<<"]              = "<<TString::Format("% 14.11f", itNewCorrErrs[jPar][jStat][jMod][iIt])<< endl;
                }
            }
        }
    }
}
