/* 
 * File:   BmnHitMatchingQA.h
 * Author: Sergey Merts <sergey.merts@gmail.com>
 *
 * Created on August 15, 2014, 2:03 PM
 */

#ifndef BMNHITMATCHINGQA_H
#define	BMNHITMATCHINGQA_H

// Base Class Headers ----------------
#include <TObject.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include "BmnManageQA.h"

class BmnHitMatchingQA : public TObject {
private:
    std::string _suffix;
    TString detName;
public:

    BmnHitMatchingQA(TString name);
    ~BmnHitMatchingQA();

    void Initialize();
    void Write();

    TH1F* _hY_matched_hits;       // distribution of matched hits over X-coordinate
    TH1F* _hX_matched_hits;       // distribution of matched hits over Y-coordinate
    TH1F* _hZ_matched_hits;       // distribution of matched hits over Z-coordinate
    TH1F* _hY_not_matched_hits;   // distribution of not matched hits over X-coordinate
    TH1F* _hX_not_matched_hits;   // distribution of not matched hits over Y-coordinate
    TH1F* _hZ_not_matched_hits;   // distribution of not matched hits over Z-coordinate
    TH1F* _hY_well_matched_hits;  // distribution of well matched hits over X-coordinate
    TH1F* _hX_well_matched_hits;  // distribution of well matched hits over Y-coordinate
    TH1F* _hZ_well_matched_hits;  // distribution of well matched hits over Z-coordinate
    TH1F* _hY_wrong_matched_hits; // distribution of wrong matched hits over X-coordinate
    TH1F* _hX_wrong_matched_hits; // distribution of wrong matched hits over Y-coordinate
    TH1F* _hZ_wrong_matched_hits; // distribution of wrong matched hits over Z-coordinate
    
    TH2F* _hXY_matched_hits;       // distribution of matched hits in XY-plane
    TH2F* _hZY_matched_hits;       // distribution of matched hits in YZ-plane
    TH2F* _hZX_matched_hits;       // distribution of matched hits in XZ-plane
    TH2F* _hXY_not_matched_hits;   // distribution of not matched hits in XY-plane
    TH2F* _hZY_not_matched_hits;   // distribution of not matched hits in YZ-plane
    TH2F* _hZX_not_matched_hits;   // distribution of not matched hits in XZ-plane
    TH2F* _hXY_well_matched_hits;  // distribution of well matched hits in XY-plane
    TH2F* _hZY_well_matched_hits;  // distribution of well matched hits in YZ-plane
    TH2F* _hZX_well_matched_hits;  // distribution of well matched hits in XZ-plane
    TH2F* _hXY_wrong_matched_hits; // distribution of wrong matched hits in XY-plane
    TH2F* _hZY_wrong_matched_hits; // distribution of wrong matched hits in YZ-plane
    TH2F* _hZX_wrong_matched_hits; // distribution of wrong matched hits in XZ-plane

    TH3F* _hZXY_matched_hits;       // distribution of matched hits in 3D
    TH3F* _hZXY_not_matched_hits;   // distribution of not matched hits in 3D
    TH3F* _hZXY_well_matched_hits;  // distribution of well matched hits in 3D
    TH3F* _hZXY_wrong_matched_hits; // distribution of wrong matched hits in 3D
    
    TH1F *_hRdist_matched_hits;          //distribution of distances from matched hit to track
    TH1F *_hXdist_matched_hits;          //distribution of X-distances from matched hit to track
    TH1F *_hYdist_matched_hits;          //distribution of Y-distances from matched hit to track
    TH1F *_hRdist_well_matched_hits;     //distribution of distances from well matched hit to track
    TH1F *_hXdist_well_matched_hits;     //distribution of X-distances from well matched hit to track
    TH1F *_hYdist_well_matched_hits;     //distribution of Y-distances from well matched hit to track
    TH1F *_hRdist_wrong_matched_hits;    //distribution of distances from wrong matched hit to track
    TH1F *_hXdist_wrong_matched_hits;    //distribution of X-distances from wrong matched hit to track
    TH1F *_hYdist_wrong_matched_hits;    //distribution of Y-distances from wrong matched hit to track    
    
    TH1F* _hNumMcTrack; // number of references to MC tracks in one Global track

public:
    ClassDef(BmnHitMatchingQA, 1)

};

#endif	/* BMNHITMATCHINGQA_H */

