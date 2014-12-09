/**
 * \file BmnClusteringQa.h
 * \brief FairTask for clustering performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef BMNCLUSTERINGQA_H_
#define BMNCLUSTERINGQA_H_

#include "FairTask.h"
#include "BmnDetectorSetup.h"
#include <string>
#include <vector>
using std::string;
using std::vector;
using std::map;
class BmnHistManager;
class TClonesArray;

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

    BmnHistManager* fHM; // Histogram manager
    string fOutputDir; // Output directory for results
    BmnDetectorSetup fDet; // For detector setup determination

    // Pointers to data arrays
    TClonesArray* fMCTracks;

//    TClonesArray* fMvdPoints; // CbmMvdPoint array
//    TClonesArray* fMvdDigis; // CbmMvdDigi array
//    TClonesArray* fMvdClusters; // CbmMvdClusters array
//    TClonesArray* fMvdHits; // CbmMvdHit array

    TClonesArray* fGemPoints;
//    TClonesArray* fGemDigis;
//    TClonesArray* fGemClusters;
    TClonesArray* fGemHits;
//    TClonesArray* fGemDigiMatches;
//    TClonesArray* fGemClusterMatches;
    TClonesArray* fGemHitMatches;

    TClonesArray* fTof1Points;
    TClonesArray* fTof1Hits;
    TClonesArray* fTof1HitMatches;
    
    TClonesArray* fTof2Points;
    TClonesArray* fTof2Hits;
    TClonesArray* fTof2HitMatches;
    
    TClonesArray* fDch1Points;
    TClonesArray* fDch1Hits;
    TClonesArray* fDch1HitMatches;
    
    TClonesArray* fDch2Points;
    TClonesArray* fDch2Hits;
    TClonesArray* fDch2HitMatches;

    BmnClusteringQa(const BmnClusteringQa&);
    BmnClusteringQa& operator=(const BmnClusteringQa&);

    ClassDef(BmnClusteringQa, 1);
};

#endif /* BMNCLUSTERINGQA_H_ */
