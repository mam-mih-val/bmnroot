// -----------------------------------------------------------------------------
// This macro plots evolution of the new and cumulative alignment corrections
// vs. iteration number.
// 
//
// Anatoly.Solomin@jinr.ru 2017-02-16

#include   <map>
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
                                TString alignCorrPlotsFileName)  
{
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    map<pair<int, int>, int> mStatMod_Pad;

  //map<pair<int, int>, int>::iterator res;
  //m.insert(make_pair(make_pair(0, 0), "Hello"));
  //m.insert(make_pair(make_pair(1, 0), "There"));
  //res = m.find(make_pair(0,0));

    mStatMod_Pad[make_pair(3, 0)] =  1;
    mStatMod_Pad[make_pair(0, 0)] =  2;
    mStatMod_Pad[make_pair(2, 0)] =  3;
    mStatMod_Pad[make_pair(1, 0)] =  4;
    mStatMod_Pad[make_pair(4, 0)] =  5;
    mStatMod_Pad[make_pair(4, 1)] = 12;
    mStatMod_Pad[make_pair(6, 0)] =  6;
    mStatMod_Pad[make_pair(6, 1)] = 13;
    mStatMod_Pad[make_pair(5, 0)] =  7;
    mStatMod_Pad[make_pair(5, 1)] = 14;

    BmnGemStripStationSet* stationSet = new BmnGemStripStationSet_RunWinter2016(BmnGemStripConfiguration::RunWinter2016);
    const Int_t nKinds  = 2;
    const Int_t nParams = 3;
    Double_t***** itCorrs = new Double_t****[nKinds];
    for (Int_t iKind=0; iKind < nKinds; iKind++) {
        itCorrs[iKind] = new Double_t***[nParams];
        for (Int_t Int_t iPar=0; iPar < nParams; iPar++) {
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
    TPad* pad;
    Int_t padNr(0);
    gStyle->SetPadTopMargin(0.01);
    gStyle->SetPadLeftMargin(0.01);
    gStyle->SetPadRightMargin(0.01);
    gStyle->SetPadBottomMargin(0.01);
    TCanvas* canvas = new TCanvas("canvas", "Covergency of alignment", 700, 1200);
    gPad->SetFillStyle(4000);
    canvas->Divide(7, 12, 0.0001, 0.0001);
    Int_t   nIts(0);     // actual number of iterations
    ifstream corrFile("newAlignCorrFileListFileName");
    while (corrFile >> fnamestr)) // this is in fact a loop over the itertations
        ++nIts;
    cout <<"nIts = "nIts<< endl;
    corrFile.clear();    // clear the 'reached the EOF' state
    corrFile.seekg(0);   // go to the beginning

    Int_t   its[nIts];   // array with the iteration numbers

    TString kinds[2] = {"new", "sum"};
    for (Int_t iKind=0; iKind < nKinds; iKind++) {
        TString kind = kinds[iKind];
        corrFile.open(kind+"AlignCorrFileListFileName");
        TString fname;
        string  fnamestr;
        Int_t   it(-1); // iteration counter: starts with 0

      //Double_t**** itCorrs = new Double_t***[nParams];
      //for (Int_t Int_t iPar=0; iPar < nParams; iPar++) {
      //    Int_t nStats = stationSet->GetNStations();
      //    itCorrs[iPar] = new Double_t**[nStats];
      //    for (Int_t iStat=0; iStat < nStats; iStat++) {
      //        Int_t nModuls = stationSet->GetGemStation(iStat)->GetNModules();
      //        itCorrs[iPar][iStat] = new Double_t*[nModuls];
      //        for (Int_t iMod=0; iMod < nModuls; iMod++) {
      //            itCorrs[iPar][iStat][iMod] = new Double_t[nIts];
      //            for (Int_t iIt=0; iIt < nIts; iIt++) {
      //                itCorrs[iPar][iStat][iMod][iIt] = 0.;
      //            }
      //        }
      //    }
      //}

        while (corrFile >> fnamestr) { // this is in fact the main loop over the itertations
            fname = fnamestr;
            cout <<"fname                  = "+fname<< endl;
            TFile* corrFile = new TFile(fname.Data());
            TTree* corrTree = (TTree*)corrFile->Get("cbmsim");
          //cout <<"corrTree->Print()"<< endl;
          //corrTree->Print();
            TClonesArray* corrs = new TClonesArray("BmnGemAlignmentCorrections");
            corrTree->SetBranchAddress("BmnGemAlignmentCorrections",  &corrs);

            for (Int_t iEntry=0; iEntry < (Int_t)corrTree->GetEntries(); iEntry++) {
              //cout <<"iEntry = "<<iEntry<< endl;
                corrTree->GetEntry(iEntry);
              //cout <<"(Int_t)corrs->GetEntriesFast() = "<<(Int_t)corrs->GetEntriesFast()<< endl;
                if ((Int_t)corrs->GetEntriesFast() > 0) break;
            }

            it += 1;
            its[it] = it; // fill array of the iteration numbers

            for (Int_t iCorr=0; iCorr < (Int_t)corrs->GetEntriesFast(); iCorr++) {
                cout <<"iCorr = "<<iCorr<< endl;
                BmnGemAlignmentCorrections* corr = (BmnGemAlignmentCorrections*)corrs->At(iCorr);
                Int_t iStat = corr->GetStation();
                Int_t iMod  = corr->GetModule();
                itCorrs[iKind][0][iStat][iMod][it] = -corr->GetCorrections().X(),
                itCorrs[iKind][1][iStat][iMod][it] = -corr->GetCorrections().Y(),
                itCorrs[iKind][2][iStat][iMod][it] = -corr->GetCorrections().Z());
            }
            delete corrFile;
            delete corrse;
        }
        corrFile.close();
    }
    for (Int_t iKind=0; iKind < nKinds; iKind++) {
        for (Int_t iPar=0; iPar < nParams; iPar++) {
            for (Int_t iStat=0; iStat < nStats; iStat++) {
                for (Int_t iMod=0; iMod < nModuls; iMod++) {
                    Double_t alCorrs[nIts];
                    for (Int_t iIt=0; iIt < nIts; iIt++) {
                        alCorrs[iIt] = itCorrs[iKind][2][iStat][iMod][it]
                    }
                    TGraph* gr = new TGraph(nIt, its, alCorrs);
                    padNr =  mStatMod_Pad[make_pair(iStat, iMod)] + iPar*14 + iKind*42;
                    canvas->cd(padNr);
                    gr->Draw("LA");
                    delete gr;
                }
            }
        }
    }
    canvas->SaveAs(alignCorrPlotsFileName);
    canvas->SaveAs(alignCorrPlotsFileName.ReplaceAll("root", "pdf"));
    canvas->SaveAs(alignCorrPlotsFileName.ReplaceAll("pdf",  "eps"));
}
