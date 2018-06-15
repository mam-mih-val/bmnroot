/**
 * \file BmnTrackingQa.h
 * \brief FairTask for tracking performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2007-2014
 */

#ifndef BMNTRACKINGQA_H_
#define BMNTRACKINGQA_H_

#include "FairTask.h"
#include "BmnDetectorSetup.h"

class BmnHistManager;
class BmnTrackMatch;
class BmnMCTrackCreator;
class BmnGlobalElectronId;

using std::string;
using std::vector;
using std::multimap;

class BmnTrackingQa : public FairTask {
public:

    /**
     * \brief Constructor.
     */
    BmnTrackingQa();
    
    /**
     * \brief Constructor.
     */
    BmnTrackingQa(Short_t ch, TString name);

    /**
     * \brief Destructor.
     */
    virtual ~BmnTrackingQa();

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

    /** Setters **/
    void SetMinNofPointsSts(Int_t minNofPointsSts) {
        fMinNofPointsGem = minNofPointsSts;
    }

    void SetMinNofPointsTof(Int_t minNofPointsTof) {
        fMinNofPointsTof = minNofPointsTof;
    }

    void SetQuota(Double_t quota) {
        fQuota = quota;
    }

    void SetUseConsecutivePointsInSts(Bool_t useConsecutivePointsInSts) {
        fUseConsecutivePointsInGem = useConsecutivePointsInSts;
    }

    void SetOutputDir(const std::string& dir) {
        fOutputDir = dir;
    }

    void SetPRange(Int_t bins, Int_t min, Int_t max) {
        fPRangeBins = bins;
        fPRangeMin = min;
        fPRangeMax = max;
    }

    void SetYRange(Int_t bins, Int_t min, Int_t max) {
        fYRangeBins = bins;
        fYRangeMin = min;
        fYRangeMax = max;
    }

    void SetPtRange(Int_t bins, Int_t min, Int_t max) {
        fPtRangeBins = bins;
        fPtRangeMin = min;
        fPtRangeMax = max;
    }

    void SetThetaRange(
            Double_t min,
            Double_t max,
            Int_t nofBins) {
        fThetaRangeMin = min;
        fThetaRangeMax = max;
        fThetaRangeBins = nofBins;
    }

    void SetTrackCategories(const vector<string>& trackCategories) {
        fTrackCategories = trackCategories;
    }

    void SetOnlyPrimes(const Bool_t prime) {
        fPrimes = prime;
    }

    Bool_t GetOnlyPrimes() const {
        return fPrimes;
    }

private:
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
    void ProcessGem();
    void ProcessGlobal();

    TString fOutName;

    BmnHistManager* fHM; // Histogram manager
    string fOutputDir; // Output directory for results
    BmnMCTrackCreator* fMCTrackCreator; // MC track creator tool

    // Acceptance defined by MC points
    Int_t fMinNofPointsGem; // Minimal number of MCPoints in STS
    Int_t fMinNofPointsTof; // Minimal number of MCPoints in TOF
    Int_t fMinNofPointsDch; // Minimal number of MCPoints in TOF

    Double_t fQuota; // True/all hits for track to be considered correctly reconstructed
    Double_t fEtaCut; // threshold for pseudorapidity (cat for spectators)
    Double_t fPCut; // threshold for momentum

    Bool_t fUseConsecutivePointsInGem; // Use consecutive MC points for STS normalization
    Bool_t fPrimes; //calculate efficiency only for primaries or for all particles

    // Minimal number of hits in track to be considered as accepted.
    // This is needed because the definition of the correctly reconstructed track
    // is 60% of hits in the reconstructed track are from the same MC track.
    // But this can lead to the situation when a reconstructed track has 4 hits,
    // all of them from the same MC track, which means that the track is correctly
    // reconstructed. BUT the real MC track has 12 points(hits). This is especially
    // important for the MUCH detector. To avoid such situations one has to put
    // such cut.

    Double_t fPRangeMin; // Minimum momentum for tracks for efficiency calculation [GeV/c]
    Double_t fPRangeMax; // Maximum momentum for tracks for efficiency calculation [GeV/c]
    Int_t fPRangeBins; // Number of bins for efficiency vs. momentum histogram
    Double_t fPtRangeMin; // Minimum Pt for tracks for efficiency calculation [GeV/c]
    Double_t fPtRangeMax; // Maximum Pt for tracks for efficiency calculation [GeV/c]
    Int_t fPtRangeBins; // Number of bins for efficiency vs. Pt histogram
    Double_t fYRangeMin; // Minimum rapidity for tracks for efficiency calculation
    Double_t fYRangeMax; // Maximum rapidity for tracks for efficiency calculation
    Int_t fYRangeBins; // Number of bins for efficiency vs. rapidity histogram
    Double_t fEtaRangeMin; // Minimum pseudorapidity for tracks for efficiency calculation
    Double_t fEtaRangeMax; // Maximum pseudorapidity for tracks for efficiency calculation    
    Int_t fEtaRangeBins; // Number of bins for efficiency vs. pseudorapidity histogram
    Double_t fThetaRangeMin; // Minimum polar angle [grad]
    Double_t fThetaRangeMax; // Maximum polar angle [grad]
    Int_t fThetaRangeBins; // Number of bins for efficiency vs. polar angle histogram
    
    UInt_t fNHitsCut; //cut to exclude overflowed events 

    // Pointers to data arrays
    TClonesArray* fMCTracks; // CbmMCTrack array
    TClonesArray* fGlobalTracks; // BmnGlobalTrack array
    TClonesArray* fGlobalTrackMatches; // BmnGemTrackMatch array
    TClonesArray* fGemTracks; // BmnGemTrack array
    TClonesArray* fGemMatches; // BmnGemTrackMatch array
    TClonesArray* fGemHitMatches;
    TClonesArray* fSilHitMatches;

    TClonesArray* fGemPoints;
    TClonesArray* fGemHits;
    TClonesArray* fSilPoints;
    TClonesArray* fSilHits;
    TClonesArray* fTof1Points;
    TClonesArray* fTof1Hits;
    TClonesArray* fTof2Points;
    TClonesArray* fTof2Hits;
    TClonesArray* fDchPoints;
    TClonesArray* fDchHits;
    
    TClonesArray* fVertex;
    
    Short_t fChargeCut; //(-1, 0, +1) do QA for negative, all or positive particles

    vector<string> fTrackCategories; // Vector of track category names

    BmnTrackingQa(const BmnTrackingQa&);
    BmnTrackingQa& operator=(const BmnTrackingQa&);

    ClassDef(BmnTrackingQa, 1);
};

#endif /* BMNTRACKINGQA_H_ */
