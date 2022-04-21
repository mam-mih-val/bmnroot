/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  L1 track class
 *
 *====================================================================
 */


#ifndef CbmL1Track_H
#define CbmL1Track_H

#include "CbmL1MCTrack.h"
#include "CbmL1TrackPar.h"

#include <vector>
#include <map>
using std::vector;
using std::map;

class L1Triplet;
class L1StsHit;
class CbmL1MCTrack;

class CbmL1Track: public CbmL1TrackPar
{
  public:
    CbmL1Track():StsHits(),nStations(0),hitMap(),mcTracks(),maxPurity(-1){}

    int GetNOfHits(){ return StsHits.size(); }

    void AddMCTrack(CbmL1MCTrack* mcTr){mcTracks.push_back(mcTr);}
    vector< CbmL1MCTrack* >&  GetMCTracks(){ return mcTracks;}
    CbmL1MCTrack* GetMCTrack(){ return mcTracks[0];}
    int  GetNMCTracks(){ return mcTracks.size();}
    bool IsGhost(){return !( mcTracks.size() );}

    void SetMaxPurity(double maxPurity_){maxPurity = maxPurity_;}
    double GetMaxPurity(){return maxPurity;}

    static bool compareChi2(const CbmL1Track &a, const CbmL1Track &b){ return (a.chi2 < b.chi2); }
    static bool comparePChi2(const CbmL1Track *a, const CbmL1Track *b){ return (a->chi2 < b->chi2); }


    double TLast[6], CLast[15];
    vector<int> StsHits;
    int nStations;
    
    map<int, int> hitMap; // how many hits from each mcTrack belong to current recoTrack
  private:
        // next members filled and used in Performance
    vector< CbmL1MCTrack* >  mcTracks; // array of assosiated recoTracks. Should be only one.
    double maxPurity; // maximum persent of hits, which belong to one mcTrack.
};

#endif
