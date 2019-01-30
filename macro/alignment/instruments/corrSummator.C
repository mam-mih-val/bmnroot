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

void ReadFileCorrections(TString, Double_t***, Double_t***);

void corrSummator(TString in1 = "it0.root", TString in2 = "align.root", TString out = "it1.root") {
  bmnloadlibs(); // load BmnRoot libraries

  TString gPathGemConfig = gSystem->Getenv("VMCWORKDIR");

  TString siConf = "SiliconRunSpring2018.xml";
  TString gemConf = "GemRunSpring2018.xml";

  TString fnames[2] = {in1, in2};

  BmnGemStripStationSet* GEM = new BmnGemStripStationSet(gPathGemConfig +"/parameters/gem/XMLConfigs/" + gemConf);
  BmnSiliconStationSet* SI = new BmnSiliconStationSet(gPathGemConfig +"/parameters/silicon/XMLConfigs/" + siConf);

  const Int_t nStatGEM = GEM->GetNStations();
  const Int_t nStatSI = SI->GetNStations();

  const Int_t nParams = 3;

  Double_t*** corrGEM = new Double_t**[nStatGEM];
  for (Int_t iStat = 0; iStat < nStatGEM; iStat++) {
    Int_t nModul = GEM->GetGemStation(iStat)->GetNModules();
    corrGEM[iStat] = new Double_t*[nModul];
    for (Int_t iMod = 0; iMod < nModul; iMod++) {
      corrGEM[iStat][iMod] = new Double_t[nParams];
      for (Int_t iPar = 0; iPar < nParams; iPar++) {
	corrGEM[iStat][iMod][iPar] = 0.;
      }
    }
  }

  Double_t*** corrSI = new Double_t**[nStatSI];
  for (Int_t iStat = 0; iStat < nStatSI; iStat++) {
    Int_t nModul = SI->GetSiliconStation(iStat)->GetNModules();
    corrSI[iStat] = new Double_t*[nModul];
    for (Int_t iMod = 0; iMod < nModul; iMod++) {
      corrSI[iStat][iMod] = new Double_t[nParams];
      for (Int_t iPar = 0; iPar < nParams; iPar++) {
	corrSI[iStat][iMod][iPar] = 0.;
      }
    }
  }

  for (Int_t iFile = 0; iFile < 2; iFile++)
    ReadFileCorrections(fnames[iFile], corrGEM, corrSI);

  // Prepare output tree
  TFile* outFile = new TFile(out, "recreate");
  TTree* outTree = new TTree("cbmsim", "cbmsim");
  TClonesArray* outClassGEM = new TClonesArray("BmnGemAlignCorrections");
  TClonesArray* outClassSI = new TClonesArray("BmnSiliconAlignCorrections");
  outTree->Branch("BmnGemAlignCorrections", &outClassGEM);
  outTree->Branch("BmnSiliconAlignCorrections", &outClassSI);

  for (Int_t iStat = 0; iStat < nStatGEM; iStat++) {
    Int_t nModul = GEM->GetGemStation(iStat)->GetNModules();
    for (Int_t iMod = 0; iMod < nModul; iMod++) {
      new ((*outClassGEM)[outClassGEM->GetEntriesFast()]) BmnGemAlignCorrections(iStat, iMod,
										 TVector3(corrGEM[iStat][iMod][0], corrGEM[iStat][iMod][1], corrGEM[iStat][iMod][2]));
      cout << iStat << " " << iMod << " " << corrGEM[iStat][iMod][0] << " " << corrGEM[iStat][iMod][1] << " " <<
	corrGEM[iStat][iMod][2] << endl;
    }
  }

  cout << endl;

  for (Int_t iStat = 0; iStat < nStatSI; iStat++) {
    Int_t nModul = SI->GetSiliconStation(iStat)->GetNModules();
    for (Int_t iMod = 0; iMod < nModul; iMod++) {
      new ((*outClassSI)[outClassSI->GetEntriesFast()]) BmnSiliconAlignCorrections(iStat, iMod,
										   TVector3(corrSI[iStat][iMod][0], corrSI[iStat][iMod][1], corrSI[iStat][iMod][2]));
      cout << iStat << " " << iMod << " " << corrSI[iStat][iMod][0] << " " << corrSI[iStat][iMod][1] << " " <<
	corrSI[iStat][iMod][2] << endl;
    }
  }

  outTree->Fill();
  outTree->Write();
  outFile->Close();
}

void ReadFileCorrections(TString fname, Double_t*** corrGEM, Double_t*** corrSI) {
  if (fname == "")
    return;

  TChain* chGEM = new TChain("cbmsim");
  chGEM->Add(fname.Data());

  TClonesArray* corrsGEM = NULL;
  chGEM->SetBranchAddress("BmnGemAlignCorrections", &corrsGEM);

  for (Int_t iEntry = 0; iEntry < chGEM->GetEntries(); iEntry++) {
    chGEM->GetEntry(iEntry);

    for (Int_t iCorr = 0; iCorr < corrsGEM->GetEntriesFast(); iCorr++) {
      BmnGemAlignCorrections* alignGEM = (BmnGemAlignCorrections*) corrsGEM->UncheckedAt(iCorr);
      corrGEM[alignGEM->GetStation()][alignGEM->GetModule()][0] += alignGEM->GetCorrections().X();
      corrGEM[alignGEM->GetStation()][alignGEM->GetModule()][1] += alignGEM->GetCorrections().Y();
      corrGEM[alignGEM->GetStation()][alignGEM->GetModule()][2] += alignGEM->GetCorrections().Z();
    }
  }
  cout << endl;
  delete chGEM;

  TChain* chSI = new TChain("cbmsim");
  chSI->Add(fname.Data());

  TClonesArray* corrsSI = NULL;
  chSI->SetBranchAddress("BmnSiliconAlignCorrections", &corrsSI);

  for (Int_t iEntry = 0; iEntry < chSI->GetEntries(); iEntry++) {
    chSI->GetEntry(iEntry);

    for (Int_t iCorr = 0; iCorr < corrsSI->GetEntriesFast(); iCorr++) {
      BmnSiliconAlignCorrections* alignSI = (BmnSiliconAlignCorrections*) corrsSI->UncheckedAt(iCorr);
      corrSI[alignSI->GetStation()][alignSI->GetModule()][0] += alignSI->GetCorrections().X();
      corrSI[alignSI->GetStation()][alignSI->GetModule()][1] += alignSI->GetCorrections().Y();
      corrSI[alignSI->GetStation()][alignSI->GetModule()][2] += alignSI->GetCorrections().Z();
    }
  }
  cout << endl;
  delete chSI;
}
