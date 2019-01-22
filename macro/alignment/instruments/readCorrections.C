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

void ReadFileCorrections(FILE*, BmnGemStripStationSet*, TString, Double_t***);

void readCorrections(TString in = "") {
  bmnloadlibs(); // load BmnRoot libraries

  TString gPathGemConfig = gSystem->Getenv("VMCWORKDIR");

  TString siConf = "SiliconRunSpring2018.xml";
  TString gemConf = "GemRunSpring2018.xml";

  BmnGemStripStationSet* GEM = new BmnGemStripStationSet(gPathGemConfig +"/parameters/gem/XMLConfigs/" + gemConf);
  BmnSiliconStationSet* SI = new BmnSiliconStationSet(gPathGemConfig +"/parameters/silicon/XMLConfigs/" + siConf);

  const Int_t nStatGEM = GEM->GetNStations();
  const Int_t nStatSI = SI->GetNStations();

  FILE* file = fopen("obtainedCorrections.dat", "w");

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
  ReadFileCorrections(file, GEM, in, corrGEM);
  fclose(file);
}

void ReadFileCorrections(FILE* file, BmnGemStripStationSet* GEM, TString fname, Double_t*** corrGEM) {
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
      corrGEM[alignGEM->GetStation()][alignGEM->GetModule()][0] = alignGEM->GetCorrections().X();
      corrGEM[alignGEM->GetStation()][alignGEM->GetModule()][1] = alignGEM->GetCorrections().Y();
      corrGEM[alignGEM->GetStation()][alignGEM->GetModule()][2] = alignGEM->GetCorrections().Z();
    }
  }

  for (Int_t iStat = 0; iStat < GEM->GetNStations(); iStat++)
    for (Int_t iMod = 0; iMod < GEM->GetGemStation(iStat)->GetNModules(); iMod++)
      for (Int_t iPar = 0; iPar < 3; iPar++)
	fprintf(file, "Stat %d Mod %d Par %d %G\n", iStat, iMod, iPar, corrGEM[iStat][iMod][iPar]);

  cout << endl;
  delete chGEM;
}
