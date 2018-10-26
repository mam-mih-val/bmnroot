#include <Rtypes.h>
#include <TChain.h>
#include <TClonesArray.h>
#include <TMath.h>
#include <TVector3.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TStyle.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

using namespace std;
using namespace TMath;

void outFigs(TString in1 = "", TString in2 = "", TString in3 = "") {
    bmnloadlibs(); // load libraries
    gStyle->SetOptFit(111);

    // Connect chains ...
    const Int_t nFiles = 3;
    const Int_t nVals = 3;
    const Int_t nBranches = 2; // vertex, residuals
    const Int_t nTitles = 3;
    TString titlesVals[nTitles] = {"V_{p}", "Res_{x}", "Res_{y}"};
    TString titlesFiles[nTitles] = {"Orig.", "Misalign.", "After align."};

    TString files[nFiles] = {in1, in2, in3};
    TString branches[nFiles] = {"BmnVertex", "BmnGemResiduals"};

    TChain * chains[nFiles] = {NULL, NULL, NULL};
    TClonesArray * arrays[nFiles][nBranches];
    for (Int_t iFile = 0; iFile < nFiles; iFile++)
        for (Int_t iBranch = 0; iBranch < nBranches; iBranch++)
            arrays[iFile][iBranch] = NULL;

    Int_t nEntries[nFiles] = {-1, -1, -1};
    for (Int_t iFile = 0; iFile < nFiles; iFile++) {
        chains[iFile] = new TChain("cbmsim");
        chains[iFile]->Add(files[iFile].Data());
        nEntries[iFile] = chains[iFile]->GetEntries();
    }

    // Int_t minEventsNum = 10000; // Min(nEntries[0], Min(nEntries[1], nEntries[2]));

    TCanvas* canv = new TCanvas("c", "c", 800, 800);
    canv->Divide(nFiles, nVals);
    TH1F * histos[nVals][nFiles];
    for (Int_t iVal = 0; iVal < nVals; iVal++)
        for (Int_t iFile = 0; iFile < nFiles; iFile++)
            histos[iVal][iFile] = new TH1F(Form("%s %s", titlesVals[iVal].Data(), titlesFiles[iFile].Data()),
                Form("%s %s", titlesVals[iVal].Data(), titlesFiles[iFile].Data()),
                200, (iVal == 0) ? -10. : -1., (iVal == 0) ? +10. : +1.);

    // Associate branches ...
    for (Int_t iFile = 0; iFile < nFiles; iFile++)
        for (Int_t iBranch = 0; iBranch < nBranches; iBranch++)
            chains[iFile]->SetBranchAddress(branches[iBranch].Data(), &arrays[iFile][iBranch]);

    for (Int_t iEntry = 0; iEntry < chains[0]->GetEntries(); iEntry++)
        for (Int_t iFile = 0; iFile < nFiles; iFile++) {
            chains[iFile]->GetEntry(iEntry);

            CbmVertex* vertex = (CbmVertex*) arrays[iFile][0]->UncheckedAt(0);
            canv->cd(iFile + 1);
            histos[0][iFile]->Fill(vertex->GetZ());

            for (Int_t iResid = 0; iResid < arrays[iFile][1]->GetEntriesFast(); iResid++) {
                BmnResiduals* resid = (BmnResiduals*) arrays[iFile][1]->UncheckedAt(iResid);
                Int_t iPad = iFile + 4;
                canv->cd(iPad);
                histos[1][iFile]->Fill(resid->GetResiduals().X());

                iPad += 3;
                canv->cd(iPad);
                histos[2][iFile]->Fill(resid->GetResiduals().Y());
            }
        }

    for (Int_t iVal = 0; iVal < nVals; iVal++) {
        for (Int_t iFile = 0; iFile < nFiles; iFile++) {
            canv->cd(iFile + nFiles * iVal + 1);
            histos[iVal][iFile]->Draw();
            //if (iVal != 0)
	    histos[iVal][iFile]->Fit("gaus", "SQww");  
        }
    }

    canv->SaveAs("output.pdf");
    delete canv;
}
