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

    void SetAngleRange(
            Double_t min,
            Double_t max,
            Int_t nofBins) {
        fAngleRangeMin = min;
        fAngleRangeMax = max;
        fAngleRangeBins = nofBins;
    }

    void SetTrackCategories(const vector<string>& trackCategories) {
        fTrackCategories = trackCategories;
    }

private:
    /**
     * \brief Read data branches from input data files.
     */
    void ReadDataBranches();

    /**
     * \brief Fill array of track categories with default values.
     */
    void FillDefaultTrackCategories();
    void FillDefaultTrackPIDCategories();


    void CreateH1Efficiency(
            const string& name,
            const string& parameter,
            const string& xTitle,
            Int_t nofBins,
            Double_t minBin,
            Double_t maxBin,
            const string& opt);

    void CreateH2Efficiency(
            const string& name,
            const string& parameter,
            const string& xTitle,
            const string& yTitle,
            Int_t nofBinsX,
            Double_t minBinX,
            Double_t maxBinX,
            Int_t nofBinsY,
            Double_t minBinY,
            Double_t maxBinY,
            const string& opt);

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

    vector<string> CreateGlobalTrackingHistogramNames(const vector<string>& detectors);

    vector<string> CreateGlobalTrackingHistogramNames();

    void ReadEventHeader();

    string LocalEfficiencyNormalization(const string& detName);

    vector<string> GlobalTrackVariants();


    void CreateHistograms();

    /**
     * \brief Loop over the reconstructed global tracks.
     * Check if track is correct and fill
     * multimap <MC track index, reconstructed track index>.
     */
    void ProcessGlobalTracks();
    
    void EffGem();
    void EffGlob();

    void FillTrackQualityHistograms(const BmnTrackMatch* trackMatch, DetectorId detId);

    /**
     * \brief Loop over the MC tracks. Check track acceptance for different cases.
     * Fill histograms of accepted and reconstructed tracks.
     */
    void ProcessMcTracks();

    //   /**
    //    * \brief Calculate efficiency histograms.
    //    */
    //   void CalculateEfficiencyHistos();

    /**
     * \brief Fill histograms of accepted and reconstructed tracks.
     * \param[in] mcTrack MC track pointer.
     * \param[in] mcId MC track index in array.
     * \param[in] mcMap Map from MC track index to reconstructed track index. Map is filled in ProcessGlobalTrack function.
     * \param[in] hist main name of the histograms to be filled.
     * \param[in] par Value that will be added in histogram (momentum or number of points).
     */
    void FillGlobalReconstructionHistos(
            Int_t mcId,
            const multimap<Int_t, Int_t>& mcMap,
            const string& histName,
            const string& histTypeName,
            const string& effName,
            const string& catName,
            const vector<Double_t>& par);

    /**
     * \brief Increase number of objects counters.
     */
    void IncreaseCounters();

    vector<string> fHeader;

    BmnHistManager* fHM; // Histogram manager
    string fOutputDir; // Output directory for results
    BmnDetectorSetup fDet; // Detector presence information
    BmnMCTrackCreator* fMCTrackCreator; // MC track creator tool

    // Acceptance defined by MC points
    Int_t fMinNofPointsGem; // Minimal number of MCPoints in STS
    Int_t fMinNofPointsTof; // Minimal number of MCPoints in TOF
    Int_t fMinNofPointsDch; // Minimal number of MCPoints in TOF

    Double_t fQuota; // True/all hits for track to be considered correctly reconstructed

    Bool_t fUseConsecutivePointsInGem; // Use consecutive MC points for STS normalization

    // Minimal number of hits in track to be considered as accepted.
    // This is needed because the definition of the correctly reconstructed track
    // is 70% of hits in the reconstructed track are from the same MC track.
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
    Double_t fAngleRangeMin; // Minimum polar angle [grad]
    Double_t fAngleRangeMax; // Maximum polar angle [grad]
    Int_t fAngleRangeBins; // Number of bins for efficiency vs. polar angle histogram

    // Pointers to data arrays
    TClonesArray* fMCTracks; // CbmMCTrack array
    TClonesArray* fGlobalTracks; // BmnGlobalTrack array
    TClonesArray* fGlobalTrackMatches; // BmnGemTrackMatch array
    TClonesArray* fGemTracks; // BmnGemTrack array
    TClonesArray* fGemMatches; // BmnGemTrackMatch array
    TClonesArray* fGemSeeds; // BmnGemTrack array
    TClonesArray* fGemSeedMatches; // BmnGemTrackMatch array

    TClonesArray* fGemPoints;
    TClonesArray* fGemHits;
    TClonesArray* fTof1Points;
    TClonesArray* fTof1Hits;
    TClonesArray* fTof2Points;
    TClonesArray* fTof2Hits;
    TClonesArray* fDch1Points;
    TClonesArray* fDch1Hits;
    TClonesArray* fDch2Points;
    TClonesArray* fDch2Hits;

    // Global track segment name maps to multimap <MC track index, reconstructed track index>
    map<string, multimap<Int_t, Int_t> > fMcToRecoMap;

    vector<string> fTrackCategories; // Vector of track category names

    BmnTrackingQa(const BmnTrackingQa&);
    BmnTrackingQa& operator=(const BmnTrackingQa&);

    ClassDef(BmnTrackingQa, 1);
};

#endif /* BMNTRACKINGQA_H_ */
