/**
 * \file BmnPidQa.h
 * \brief FairTask for pid performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2007-2021
 */

#ifndef BMNPIDQA_H_
#define BMNPIDQA_H_

#include "FairTask.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include <unordered_map>
#include "BmnMCTrackCreator.h"
#include "BmnGlobalTrack.h"



class BmnHistManager;
class BmnTrackMatch;
class BmnMCTrackCreator;


using std::string;
using std::vector;
using std::multimap;
using std::unordered_map;

class BmnPidQa : public FairTask {
public:

    /**
     * \brief Constructor.
     */
    BmnPidQa() {};
    
    /**
     * \brief Constructor.
     */
    BmnPidQa(TString name, TString storageName = "canvStorage");

    /**
     * \brief Destructor.
     */
    virtual ~BmnPidQa();

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

   
    TParticlePDG* GetParticleExtend(Int_t pdgCode);

    void SetOutputDir(const std::string& dir) {
        fOutputDir = dir;
    }

    void SetPRange(Int_t bins, Int_t min, Int_t max) {
        fPRangeBins = bins;
        fPRangeMin = min;
        fPRangeMax = max;
    }

    void SetOnlyPrimes(const Bool_t prime) {
        fPrimes = prime;
    }
    
    Bool_t GetOnlyPrimes() const {
        return fPrimes;
    }
     
  
private:
    Int_t EnumToPdg(PidParticles part);

    /**
     * \brief Read data branches from input data files.
     */
    void ReadDataBranches();

    void CreateH1(
            const string& name,
            const string& xTitle,
            const string& yTitle,
            Int_t nofBins,
            Double_t minBin,
            Double_t maxBin);

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

    void CreateTrackHitsHistogram(const string& detName);

    void ReadEventHeader();


    void CreateHistograms();
    void ProcessGlobal();
 
    TString fOutName;
    TString fStorageName;

    

    BmnHistManager* fHM; // Histogram manager
    string fOutputDir; // Output directory for results
   
      
    Bool_t fPrimes; //calculate efficiency only for primaries or for all particles

  
    Double_t fPRangeMin; // Minimum momentum for tracks for efficiency calculation [GeV/c]
    Double_t fPRangeMax; // Maximum momentum for tracks for efficiency calculation [GeV/c]
    Int_t fPRangeBins; // Number of bins for efficiency vs. momentum histogram
    
    Double_t fBetaRangeMin; // Minimum rigidity for tracks for efficiency calculation [GeV/c]
    Double_t fBetaRangeMax; // Maximum rigidity for tracks for efficiency calculation [GeV/c]
    Int_t fBetaRangeBins; // Number of bins for rigidity-momentum histogram

     
    Double_t fVelocRangeMin; // Minimum rigidity for tracks for efficiency calculation [GeV/c]
    Double_t fVelocRangeMax; // Maximum rigidity for tracks for efficiency calculation [GeV/c]
    Int_t fVelocRangeBins; // Number of bins for rigidity-momentum histogram

    Double_t fTimeRangeMin;
    Double_t fTimeRangeMax;
    Int_t fTimeRangeBins;

    Double_t fMassRangeMin;
    Double_t fMassRangeMax;
    Int_t fMassRangeBins;

    Double_t fNHitsRangeMin; // Minimum hits for tracks for efficiency calculation [GeV/c]
    Double_t fNHitsRangeMax; // Maximum hits for tracks for efficiency calculation [GeV/c]
    Int_t fNHitsRangeBins; // Number of bins for rigidity-momentum histogram

    TDatabasePDG* db = nullptr;

    vector<TParticlePDG*> fParticles;
    // mass table for particles
    unordered_map<Double_t, string> fMassTable; // {Mass: Particle name}
    
	string GetParticleName(Double_t mass);

    void MassTablePrint();
    // key - Particle name
    // value[0] -counter for true total number of particle of one sort 
    // value[1] - counter of true identifications
    // value[2] - counter of false identifications  
    // value[3] - counter for undivided number of particle of one sort 
    unordered_map<string, vector<Int_t> > fPidStatistics400, fPidStatistics700; 
    void PidStatisticsPrint400();
    void PidStatisticsPrint700();    

    // Pointers to data arrays
    TClonesArray* fMCTracks; // CbmMCTrack array
    TClonesArray* fGlobalTracks; // BmnGlobalTrack array
    TClonesArray* fGlobalTrackMatches; // BmnGlobalTrackMatches array
    TClonesArray* fTof400Hits;
    TClonesArray* fTof700Hits;
    
    BmnPidQa(const BmnPidQa&);
    BmnPidQa& operator=(const BmnPidQa&);

    ClassDef(BmnPidQa, 1);
};

#endif /* BMNPIDQA_H_ */
