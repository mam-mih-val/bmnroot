
/**
 * \file BmnGlobalTrackingQA.h
 * \author Sergey Merts <Sergey.Merts@gmail.com>
 * \date 2014
 * \brief creating QA for global tracking algorithms.
 *
 **/

#ifndef  BMNGLOBALTRACKINGQA_H
#define  BMNGLOBALTRACKINGQA_H

// Base Class Headers ----------------
#include <TObject.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include "BmnManageQA.h"

/**
 * \class BmnGlobalTrackingQA
 * \author Sergey Merts <Sergey.Merts@gmail.com>
 * \date 2014
 * \brief creating QA for global tracking algorithms.
 *
 **/
class BmnGlobalTrackingQA : public TObject {
private:
    std::string _suffix;
public:

    BmnGlobalTrackingQA();
    BmnGlobalTrackingQA(const std::string& suffix);
    ~BmnGlobalTrackingQA();

    void Initialize();
    void Write();

    TH1F *_hTrackLength; //distribution of track lengths [cm]

    // All found hits
    TH1F *_hY_global; // distribution of hits over X-coordinate
    TH1F *_hX_global; // distribution of hits over Y-coordinate
    TH1F *_hZ_global; // distribution of hits over Z-coordinate

    // All found hits
    TH2F* _hXY_global; // distribution of hits in XY-plane
    TH2F* _hZY_global; // distribution of hits in YZ-plane
    TH2F* _hZX_global; // distribution of hits in XZ-plane

    // All found hits
    TH3F* _hZXY_global; // distribution of hits in 3D

    TH1F *_hNumOfHitsDistr;
    TH1F *_hMomentumDistr;
    TH1F *_hPt;
    TH1F *_hPx;
    TH1F *_hPy;
    TH1F *_hPz;
    
    TH1F *_hHitDist;
    TH1F *_hHitXDist;
    TH1F *_hHitYDist;
    TH1F *_hHitChiSq;
    
    TH1F* _hNumMcTrack; // number of references to MC tracks in one Global track

public:
    ClassDef(BmnGlobalTrackingQA, 1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
