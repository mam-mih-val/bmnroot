/**
 * \file CbmLitMCTrackCreator.h
 * \brief Creates CbmLitMCTrack objects.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2011-2014
 **/
#ifndef BMNMCTRACKCREATOR_H_
#define BMNMCTRACKCREATOR_H_

#include "BmnMCTrack.h"
#include <map>
#include <iostream>
#include <assert.h>
#include "BmnGemStripStationSet.h"
#include "BmnSiliconStationSet.h"
#include "BmnCSCStationSet.h"
#include "BmnGemStripStationSet_RunSpring2017.h"

class TClonesArray;
class FairMCPoint;
class CbmStsPoint;
class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsDigiScheme;

/**
 * \class CbmLitMCTrackCreator
 * \brief Creates CbmLitMCTrack objects.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
class BmnMCTrackCreator {
private:
    /**
     * \brief Constructor.
     */
    BmnMCTrackCreator(TString gem, TString sil, TString csc);

public:
    /**
     * \brief Destructor.
     */
    virtual ~BmnMCTrackCreator();

    /**
     * \brief Singleton instance.
     */
    static BmnMCTrackCreator* Instance(TString gem, TString sil, TString csc);

    /**
     * \brief Creates array of BmnMCTracks for current event.
     * Has to be executed in Exec() function of the task.
     */
    void Create();

    /**
     * \brief Check whether a track exists in the array.
     * \param[in] mcId MC index of track.
     * \return true if track exists in array.
     */
    Bool_t TrackExists(Int_t mcId) const {
        return (fBmnMCTracks.count(mcId) > 0) ? kTRUE : kFALSE;
        //        if (fBmnMCTracks.count(mcId) == 0) return kFALSE;
        //        Int_t NPointsSil = fBmnMCTracks.find(mcId)->second.GetNofPoints(kSILICON);
        //        Int_t NPointsGem = fBmnMCTracks.find(mcId)->second.GetNofPoints(kGEM);
        //        if (NPointsSil > fSilDetector->GetNStations()) return kFALSE;
        //        if (NPointsGem > fGemDetector->GetNStations()) return kFALSE;
        //        //if (NPointsSil + NPointsGem < 4) return kFALSE;
        //        return kTRUE;
        ////        printf("NPointsSil = %d     isNSilPointsCorrect = %d\n", NPointsSil, isNSilPointsCorrect);
        ////        printf("NPointsGem = %d     isNGemPointsCorrect = %d\n", NPointsGem, isNGemPointsCorrect);

    }

    /**
     * \brief Return BmnMCTrack by its index.
     * \param[in] mcId MC index of track.
     * \return MC track.
     */
    const BmnMCTrack& GetTrack(int mcId) const {
        assert(TrackExists(mcId));
        return fBmnMCTracks.find(mcId)->second;
    }

    /**
     * \brief Return number of tracks.
     * \return Number of tracks.
     */
    Int_t GetNofTracks() const {
        return fBmnMCTracks.size();
    }

private:

    /**
     * \brief Read data branches.
     */
    void ReadDataBranches();

    /**
     * \brief Add MC points from a certain detector.
     * \param[in] detId Detector identificator.
     * \param[in] array Array of FairMCPoint.
     */
    void AddPoints(DetectorId detId, const TClonesArray* array);

    /**
     * \brief Calculate and set number of RICH hits for MC track.
     */
    void AddRichHits();

    /**
     * \brief Fit Rich MC points using ellipse fitter and fill ellipse parameters.
     */
    void AddRingParameters();

    /**
     * \brief Convert FairMCPoint to CbmLitMCPoint.
     * \param[in] fairPoint Pointer to FairMCPoint to be converted.
     * \param[out] litPoint Pointer to output CbmLitMCPoint.
     * \param[in] refId Reference index of the MC point.
     */
    void FairMCPointToBmnMCPoint(
            const FairMCPoint* fairPoint,
            BmnMCPoint* bmnPoint,
            int refId,
            int stationId);

    void FairMCPointCoordinatesAndMomentumToBmnMCPoint(// for DCH1, DCH2, TOF1 points
            const FairMCPoint* fairPoint,
            BmnMCPoint* bmnPoint);

    TClonesArray* fMCTracks;
    TClonesArray* fSilPoints;
    TClonesArray* fSsdPoints;
    TClonesArray* fGemPoints;
    TClonesArray* fTof400Points;
    TClonesArray* fTof700Points;
    TClonesArray* fCscPoints;
    TClonesArray* fDchPoints;
    
    Int_t fNSiliconStations;
    Int_t fNGemStations;
    Int_t fNCscStations;

    // Stores created BmnMCTrack objects.
    // std::map<MC track index, BmnMCTrack object>.
    std::map<Int_t, BmnMCTrack> fBmnMCTracks;

    BmnGemStripStationSet* fGemDetector;
    BmnSiliconStationSet* fSilDetector;
    BmnCSCStationSet* fCscDetector;
};

#endif /* BMNMCTRACKCREATOR_H_ */
