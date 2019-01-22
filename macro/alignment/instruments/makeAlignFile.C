#include <TClonesArray.h>
#include <TChain.h>
#include <TVector3.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>

#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
R__LOAD_LIBRARY(libBmnAlignment.so)

void makeAlignFile(TString out = "",
		   TString siConf = "SiliconRunSpring2018.xml",
		   TString gemConf = "GemRunSpring2018.xml") {
    bmnloadlibs(); // load BmnRoot libraries

    TString gPathGemConfig = gSystem->Getenv("VMCWORKDIR");

    BmnGemStripStationSet* StationSetGem = new BmnGemStripStationSet(gPathGemConfig +"/parameters/gem/XMLConfigs/" + gemConf);
    const Int_t nStatGem = StationSetGem->GetNStations();

    BmnSiliconStationSet* StationSetSi = new BmnSiliconStationSet(gPathGemConfig +"/parameters/silicon/XMLConfigs/" + siConf);
    const Int_t nStatSi = StationSetSi->GetNStations();

    // Prepare output tree
    TFile* outFile = new TFile(out.Data(), "recreate");
    TTree* outTree = new TTree("cbmsim", "cbmsim");
    TClonesArray* outClassGem = new TClonesArray("BmnGemAlignCorrections");
    TClonesArray* outClassSi = new TClonesArray("BmnSiliconAlignCorrections");
    outTree->Branch("BmnGemAlignCorrections", &outClassGem);
    outTree->Branch("BmnSiliconAlignCorrections", &outClassSi);

    // GEMs
    for (Int_t iStat = 0; iStat < StationSetGem->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < StationSetGem->GetGemStation(iStat)->GetNModules(); iMod++) {
	  BmnGemAlignCorrections* corrs = new ((*outClassGem)[outClassGem->GetEntriesFast()]) BmnGemAlignCorrections();
	  corrs->SetStation(iStat);
	  corrs->SetModule(iMod);
	  corrs->SetCorrections(0., 0., 0.);
	}

    // Silicons
    for (Int_t iStat = 0; iStat < StationSetSi->GetNStations(); iStat++)
      for (Int_t iMod = 0; iMod < StationSetSi->GetSiliconStation(iStat)->GetNModules(); iMod++) {
	BmnSiliconAlignCorrections* corrs = new ((*outClassSi)[outClassSi->GetEntriesFast()]) BmnSiliconAlignCorrections();
	corrs->SetStation(iStat);
	corrs->SetModule(iMod);
	corrs->SetCorrections(0., 0., 0.);
      }

    outTree->Fill();
    outTree->Write();
    outFile->Close();
}
