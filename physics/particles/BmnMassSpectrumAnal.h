#include <TNamed.h>
#include <TChain.h>
#include <TSystemDirectory.h>
#include <TCollection.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TH1.h>

#include <BmnParticlePairCut.h>

#include <iostream>
#include <vector>

using namespace std;

#ifndef BMNMASSSPECTRUMANAL_H
#define BMNMASSSPECTRUMANAL_H 1

class BmnMassSpectrumAnal : public TNamed {
public:

    BmnMassSpectrumAnal() {
        ;
    }
    BmnMassSpectrumAnal(TString, TString);

    virtual ~BmnMassSpectrumAnal() {
        if (fOutput) {

            // Preparing output file ...
            TFile* file = new TFile(Form("test.root"), "recreate");
            TTree* tree = new TTree("bmndata", "bmndata");
            //            
            //            tree->Branch("SelectedCuts", &fOutput);
            //            tree->Fill();
            //            tree->Write();

            cout << fSelectedSpectra.size() << endl;
            
            for (Int_t iHisto = 0; iHisto < fSelectedSpectra.size(); iHisto++) {
                // Getting cut values ...
                BmnParticlePairCut* cuts = (BmnParticlePairCut*) fOutput->UncheckedAt(iHisto);

                Double_t DCA0 =  cuts->dca0();
                Double_t DCA12 = cuts->dca12();
                Double_t DCA1 = cuts->dca1();
                Double_t DCA2 = cuts->dca2();

                enum {
                    silicon, gem
                };

                Int_t nHitsPart1Sil = cuts->nHits(0, silicon);
                Int_t nHitsPart1Gem = cuts->nHits(0, gem);
                Int_t nHitsPart2Sil = cuts->nHits(1, silicon);
                Int_t nHitsPart2Gem = cuts->nHits(1, gem);

                pair <Int_t, Int_t> idx = make_pair(nHitsPart1Sil, nHitsPart2Sil); // FIXME

                TString title = TString::Format("dca0_%G_dca12_%G_dca1_%G_dca2_%G_nHitsPart1_%d_nHitsPart2_%d",
                        DCA0, DCA12, DCA1, DCA2, idx.first, idx.second);

                fSelectedSpectra.at(iHisto).SetName(title.Data());
                fSelectedSpectra.at(iHisto).Write();
            }

            delete file;
        }
    }


private:

    void ProcessSpectrum();
    Bool_t AnalSpectrum(TH1F*);

    //
    TClonesArray* fOutput;
    //
    TString fCutDir;
    vector <TString> fCutFiles;

    TString fSpectraDir;
    vector <TString> fSpectraFiles;

    vector <TH1F> fSelectedSpectra;

    ClassDef(BmnMassSpectrumAnal, 0)
};


#endif