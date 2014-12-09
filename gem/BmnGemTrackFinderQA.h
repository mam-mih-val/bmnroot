

#ifndef BmnGemTrackFinderQA_H
#define BmnGemTrackFinderQA_H

#include <TObject.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include "BmnManageQA.h"

class BmnGemTrackFinderQA : public TObject {
private:

    std::string _prefix;

public:

    TH1F *_hNumOfHitsDistr;
    TH1F *_hMomentumDistr;
    TH1F *_hPt;
    TH1F *_hPx;
    TH1F *_hPy;
    TH1F *_hPz;
    
    TH2F *_hyRxR_all;
    TH2F *_hyRxR_good;
    TH2F *_hyRxR_bad;
    
    TH2F *_hXYAllHits;
    TH2F *_hXYGoodHits;
    TH2F *_hXYBadHits;
    
    TH2F *_hZYAllHits;
    TH2F *_hZYGoodHits;
    TH2F *_hZYBadHits;
    
    TH2F *_hZXAllHits;
    TH2F *_hZXGoodHits;
    TH2F *_hZXBadHits;
    
    //found hits from tracks
    TH2F *_hXYFoundHits;    
    TH2F *_hZYFoundHits;    
    TH2F *_hZXFoundHits;
    
    TH1F* _hHitsDist;
    TH1F* _hHitsXDist;
    TH1F* _hHitsYDist;
    
    TH1F* _hStationAllHits;
    TH1F* _hStationUsedHits;
    TH1F* _hStationNotUsedHits;
    TH1F* _hStationWellHits;
    TH1F* _hStationWrongHits;
    
    TH1F* _hNumMcTrack; // number of references to MC tracks in one Global track
    

    BmnGemTrackFinderQA();
    BmnGemTrackFinderQA(const std::string& suffix);

    virtual ~BmnGemTrackFinderQA();

    void Initialize();
    void Write();

    ClassDef(BmnGemTrackFinderQA, 1);

};

#endif