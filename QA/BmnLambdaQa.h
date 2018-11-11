/**
 * \file BmnLambdaQa.h
 * \brief FairTask for MC simulated lambda reconstruction performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \author Alexander Lytaev <sas-lyt@ya.ru> - modifications for BMN experiment 
 * \date 2018 July
 */

#ifndef BMNLAMBDAQA_H_
#define BMNLAMBDAQA_H_

#include "FairTask.h"
#include <string>
#include <vector>
#include "report/BmnSimulationReport.h"
#include "report/BmnHistManager.h"
using std::string;
using std::vector;
using std::map;
class BmnHistManager;
class TClonesArray;

class BmnLambdaQa : public FairTask {
public:

    BmnLambdaQa() {
    }

    /**
     * \brief a constructor with keys
     * \param[in] useMCFile if is True conducts a geometry analysis and particle decay reconstruction analysis with MC data as input
     * \param[in] useRecoFile if is True conducts a two particle decay reconstruction analysis with reco data as input, if useMCFile is true as well carries out a two particle decay matching analysis
     * \param[in] key2 if is 'a' particle is reconstructable if it has 4 gem points, if is 'b' particle is reconstructable if it has 4 gem + silicon points
     * \param[in] name names of output html and root files 
     * \param[in] keyAddition key word to be added at the end of .jpg output files to distinguish them using different sets of detectors in the geometry analysis
     * \param[in] drawPoints draws one-dimensional histograms as points with bars, otherwise draws them as default bar charts
     */
    BmnLambdaQa(Bool_t useMCFile = kTRUE, Bool_t useRecoFile = kFALSE, Short_t key2 = 'a', TString name = "lambda_qa_sim", TString keyAddition = "", Bool_t drawPoints = kTRUE);

    /**
     * \brief Destructor.
     */

    virtual ~BmnLambdaQa();

    /**
     * \brief Derived from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Derived from FairTask.
     */
    virtual void Exec(
            Option_t* opt);

    /**
     * \brief Derived from FairTask.
     */
    virtual void Finish();

private:

    BmnHistManager* fHM;
    string fOutputDir;
    void ReadDataBranches();
    void CreateH1(const string& name, const string& xTitle, const string& yTitle, Int_t nofBins, Double_t minBin, Double_t maxBin);
    void CreateH2(
            const string& name,
            const string& xTitle,
            const string& yTitle,
            const string& zTitle,
            Int_t nofBinsX,
            Double_t minBinX,
            Double_t maxBinX,
            Int_t nofBinsY,
            Double_t minBinY,
            Double_t maxBinY);
    void CreateNumberOfReconstructableLambdaHistograms();
    void CreateNumberOfLambdaHistograms();
    void CreateLambdaRecEfficiencyHistograms();
    void CreateTwoDimensionalRecEfficiencyHistograms();
    void CreateNumberOLambdaDecayProtonsHistograms();
    void CreateNumberOfLambdaDecayMesonsHistograms();
    void CreateRecProtonsRecEfficiencyHistograms();
    void CreateRecMesonsRecEfficiencyHistograms();
    void CreateTwoDimensionalRecProtonsRecEfficiencyHistograms();
    void CreateTwoDimensionalRecMesonsRecEfficiencyHistograms();
    void CreateNumberOfReconstrcutedLambdaHistograms();
    void CreateReconstructionEfficiencyHistograms();
    void CreateTwoDimensionalRealRecEfficiencyHistograms();
    void CreateNumberOfReconstrcutedRecProtonHistograms();
    void CreateTwoDimensionalNumberOfReconstructedProtonsHistograms();
    void CreateRecProtonsReconstructionEfficiencyHistograms();
    void CreateNumberOfReconstrcutedRecMesonsHistograms();
    void CreateTwoDimensionalNumberOfReconstructedMesonsHistograms();
    void CreateRecMesonsReconstructionEfficiencyHistograms();
    void CreateReconstructedParticlePairsFromMCDataHistograms();
    void CreateTwoDimensionalReconstructedParticlePairsFromMCDataHistograms();
    void CreateReconstructedParticlePairsFromMCDataWOCutsHistograms();
    void CreateTwoDimensionalReconstructedParticlePairsFromMCDataWOCutsHistograms();
    void CreateFakeLambdaHistograms();
    void CreateTwoDimensionalFakeLambdaHistograms();
    void CreateNumberOfNotReconstructedLambdaHistograms();
    void CreateReconstructedParticlePairsHistograms();
    void CreateTwoDimensionalReconstructedParticlePairsHistograms();
    void CreateReconstructedParticlePairsWOCutsHistograms();
    void CreateTwoDimensionalReconstructedParticlePairsWOCutsHistograms();

    /*BmnLambdaQa(const BmnLambdaQa&);
    BmnLambdaQa& operator=(const BmnLambdaQa&);*/

    Int_t fNLambdas;
    Int_t fNReconstructable;
    Int_t fNOfParticlePairs;
    Int_t fNOfParticlePairsWithMatchedLambda;
    Int_t fNOfReconstructedLambdas;
    Int_t fNOfReconstructedLambdasM; // number of reconstructed lambdas with MC match 
    Int_t fNOfNotReconstructedLambdas; // number of reconstructed lambdas with MC match 
    Int_t fNOfParticlePairsMC; // number of reconstructed particle pairs from MC data
    Int_t fNOfParticlePairsMCAll; // number of reconstructed particle pair from MC data without any cuts
    Int_t fNOfLambdasParticlePairsMC;
    Int_t fNOfBins;
    Int_t fLambdaPRangeMin;
    Int_t fLambdaPRangeMax;
    Int_t fLambdaEtaRangeMin;
    Int_t fLambdaEtaRangeMax;
    Int_t fLambdaThetaRangeMin;
    Int_t fLambdaThetaRangeMax;
    Int_t fLambdaThetaRRangeMin;
    Int_t fLambdaThetaRRangeMax;
    Int_t fProtonsPRangeMin;
    Int_t fProtonsPRangeMax;
    Int_t fProtonsEtaRangeMin;
    Int_t fProtonsEtaRangeMax;
    Int_t fProtonsThetaRangeMin;
    Int_t fProtonsThetaRangeMax;
    Int_t fProtonsThetaRRangeMin;
    Int_t fProtonsThetaRRangeMax;
    Int_t fMesonsPRangeMin;
    Int_t fMesonsPRangeMax;
    Int_t fMesonsEtaRangeMin;
    Int_t fMesonsEtaRangeMax;
    Int_t fMesonsThetaRangeMin;
    Int_t fMesonsThetaRangeMax;
    Int_t fMesonsThetaRRangeMin;
    Int_t fMesonsThetaRRangeMax;
    Int_t fInvMassMin;
    Int_t fInvMassMax;
    Int_t fDCA1RangeMin;
    Int_t fDCA1RangeMax;
    Int_t fDCA2RangeMin;
    Int_t fDCA2RangeMax;
    Int_t fDCA12XRangeMin;
    Int_t fDCA12XRangeMax;
    Int_t fDCA12YRangeMin;
    Int_t fDCA12YRangeMax;
    Int_t fPathRangeMin;
    Int_t fPathRangeMax;

    Short_t fKey2; // 'a' - only GEM hits are considered in geometry analysis, 'b' - silicon hits are added to GEM
    Bool_t fDrawPoints;
    Bool_t fUseMCFile;
    Bool_t fUseRecoFile;

    TString fOutName;
    TString fKeyAddition; // key to be added at the end of each histogram name and overall html file  

    TClonesArray* fMCTracks;
    TClonesArray* fSiliconPoints;
    TClonesArray* fGemTracks; // BmnGemTrack array
    TClonesArray* fGemMatches;
    TClonesArray* fParticlePair; // all decay pairs reconstructed when having matches
    TClonesArray* fParticlePairNoCuts; // all decay pairs reconstructed when
    TClonesArray* fParticlePairMC; // all decay pairs reconstructed from MC data  
    TClonesArray* fReconstructedLambda; //all reconstructed lambdas
    TClonesArray* fReconstructedLambdaM; //reconstructed lambdas with MC match 
    TClonesArray* fNotReconstructedLambda; // not reconstructed lambdas(ghosts)
    TClonesArray* fGlobalMatches;
    TClonesArray* fParticlePairMCAll;
    TClonesArray* fParticlePairsInfoMC;
    TClonesArray* fParticlePairsInfoReco;

    ClassDef(BmnLambdaQa, 1);
};

#endif /* BMNLAMBDAQA_H_ */
