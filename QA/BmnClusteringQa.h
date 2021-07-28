/**
 * \file BmnClusteringQa.h
 * \brief FairTask for clustering performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef BMNCLUSTERINGQA_H_
#define BMNCLUSTERINGQA_H_

#include "BmnDetectorList.h"
#include "BmnGemStripStationSet.h"
#include "BmnSiliconStationSet.h"
#include "BmnHistManager.h"

#include "FairTask.h"

#include "TClonesArray.h"

#include <string>
#include <vector>

using std::string;
using std::vector;
using std::map;


class BmnClusteringQa : public FairTask {
public:
    /**
     * \brief Constructor.
     */
    BmnClusteringQa();

    /**
     * \brief Destructor.
     */
    virtual ~BmnClusteringQa();

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

    /** Setters */
    void SetOutputDir(const string& outputDir) {
        fOutputDir = outputDir;
    }

    void SetOnlyPrimes(const Bool_t prime) {
        fPrimes = prime;
    }

    Bool_t GetOnlyPrimes() const {
        return fPrimes;
    }

private:

    Int_t GetStationId(
            Int_t address,
            DetectorId detId);

    /**
     * \brief Read data branches.
     */
    void ReadDataBranches();

    void ProcessPoints(
            const TClonesArray* points,
            const string& detName,
            DetectorId detId);

    void ProcessDigis(
            const TClonesArray* digis,
            const TClonesArray* digiMatches,
            const string& detName,
            DetectorId detId);

    void ProcessClusters(
            const TClonesArray* clusters,
            const TClonesArray* clusterMatches,
            const string& detName,
            DetectorId detId);

    void ProcessHits(
            const TClonesArray* hits,
            const TClonesArray* hitMatches,
            const string& detName,
            DetectorId detId);


    void ReadEventHeader();

    /**
     *
     */
    void FillEventCounterHistograms();

    /**
     *
     */
    void CreateHistograms();

    /**
     *
     */
    void CreateNofObjectsHistograms(
            DetectorId detId,
            const string& detName);

    /**
     *
     */
    void CreateNofObjectsHistograms(
            DetectorId detId,
            const string& detName,
            const string& parameter,
            const string& xTitle);

    void CreateClusterParametersHistograms(
            DetectorId detId,
            const string& detName);

    void FillResidualAndPullHistograms(
            const TClonesArray* points,
            const TClonesArray* hits,
            const TClonesArray* hitMatches,
            const string& detName,
            DetectorId detId);

    void FillHitEfficiencyHistograms(
            const TClonesArray* points,
            const TClonesArray* hits,
            const TClonesArray* hitMatches,
            const string& detName,
            DetectorId detId);

    void CreateHitEfficiencyHistograms(
            DetectorId detId,
            const string& detName,
            const string& parameter,
            const string& xTitle,
            Int_t nofBins,
            Double_t minBin,
            Double_t maxBin);

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

    BmnHistManager* fHM; // Histogram manager
    string fOutputDir; // Output directory for results

    // Pointers to data arrays
    TClonesArray* fMCTracks;

    //    TClonesArray* fMvdPoints; // CbmMvdPoint array
    //    TClonesArray* fMvdDigis; // CbmMvdDigi array
    //    TClonesArray* fMvdClusters; // CbmMvdClusters array
    //    TClonesArray* fMvdHits; // CbmMvdHit array

    TClonesArray* fGemPoints;
    TClonesArray* fGemDigits;
    //    TClonesArray* fGemClusters;
    TClonesArray* fGemHits;
    TClonesArray* fGemDigitMatches;
    //    TClonesArray* fGemClusterMatches;
    TClonesArray* fGemHitMatches;

    BmnGemStripStationSet* fGemDetector;
    BmnSiliconStationSet* fSilDetector;
    
    TClonesArray* fSilHits;
    TClonesArray* fSilPoints;
    TClonesArray* fSilHitMatches;
      
    TClonesArray* fDch1Points;
    TClonesArray* fDch1Hits;
    TClonesArray* fDch1HitMatches;

    Bool_t fPrimes; //calculate efficiency only for primaries or for all particles

    BmnClusteringQa(const BmnClusteringQa&) = delete;
    BmnClusteringQa& operator=(const BmnClusteringQa&) = delete;

    ClassDef(BmnClusteringQa, 1);
};

#endif /* BMNCLUSTERINGQA_H_ */
