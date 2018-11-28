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

    BmnLambdaQa();
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
    virtual void Exec(Option_t* opt);

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

    Bool_t fUseMCFile;
    Bool_t fUseRecoFile;

    TClonesArray* fMCTracks;
    TClonesArray* fGlobalTracks;
    TClonesArray* fSiliconPoints;
    TClonesArray* fSSDPoints;
      
    TClonesArray* fParticlePair; 
    
    TClonesArray* fParticlePair_MC_noCuts;
    TClonesArray* fParticlePair_MC_withCuts;
    
    TClonesArray* fParticlePair_RECO_noCuts;
    TClonesArray* fParticlePair_RECO_withCuts;
    
    static Int_t fCurrentEvent;

    ClassDef(BmnLambdaQa, 1);
};

#endif /* BMNLAMBDAQA_H_ */
