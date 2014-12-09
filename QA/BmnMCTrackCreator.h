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
    BmnMCTrackCreator();

public:
    /**
     * \brief Destructor.
     */
    virtual ~BmnMCTrackCreator();

    /**
     * \brief Singleton instance.
     */
    static BmnMCTrackCreator* Instance();

    /**
     * \brief Creates array of CbmLitMCTracks for current event.
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
    }

    /**
     * \brief Return CbmLitMCTrack by its index.
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

    /**
     * \brief Fill maps for MC points to station id.
     */
    void FillStationMaps();


    void FairMCPointCoordinatesAndMomentumToBmnMCPoint(// for DCH1, DCH2, TOF1 points
            const FairMCPoint* fairPoint,
            BmnMCPoint* bmnPoint);

    void GemPointCoordinatesAndMomentumToBmnMCPoint(
            const CbmStsPoint* gemPoint,
            BmnMCPoint* bmnPoint);

    TClonesArray* fMCTracks;
    TClonesArray* fMvdPoints;
    TClonesArray* fGemPoints;
    TClonesArray* fTof1Points;
    TClonesArray* fDch1Points;
    TClonesArray* fDch2Points;
    TClonesArray* fTof2Points;

    // Stores created CbmLitMCTrack objects.
    // std::map<MC track index, CbmLitMCTrack object>.
    std::map<Int_t, BmnMCTrack> fBmnMCTracks;

    // Map <MC point index, station index>
    //   std::map<int, int>fMvdStationsMap; // for MVD
    std::map<Int_t, Int_t>fGemStationsMap; // for GEM

    CbmGeoStsPar* fStsGeoPar; // Geometry parameter container
    CbmStsDigiPar* fStsDigiPar; // Digitisation parameter container
    CbmStsDigiScheme* fStsDigiScheme; // Digitisation scheme
};

#endif /* BMNMCTRACKCREATOR_H_ */
