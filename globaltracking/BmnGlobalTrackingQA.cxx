#include "BmnGlobalTrackingQA.h"

BmnGlobalTrackingQA::BmnGlobalTrackingQA() :
_hPx(0), _hPy(0), _hPz(0), _hPt(0),
_hHitDist(0), _hHitXDist(0), _hHitYDist(0),
_hHitChiSq(0),        
_hMomentumDistr(0),
_hNumOfHitsDistr(0),
_hZXY_global(0),
_hX_global(0), _hY_global(0), _hZ_global(0),
_hXY_global(0), _hZY_global(0), _hZX_global(0),
_hTrackLength(0), _hNumMcTrack(0)
{
}

BmnGlobalTrackingQA::BmnGlobalTrackingQA(const std::string& suffix) :
_hPx(0), _hPy(0), _hPz(0), _hPt(0),
_hHitDist(0), _hHitXDist(0), _hHitYDist(0),
_hHitChiSq(0),        
_hMomentumDistr(0),
_hNumOfHitsDistr(0),
_hZXY_global(0),
_hX_global(0), _hY_global(0), _hZ_global(0),
_hXY_global(0), _hZY_global(0), _hZX_global(0),
_hTrackLength(0), _hNumMcTrack(0)
{
}

BmnGlobalTrackingQA::~BmnGlobalTrackingQA() {

    delete _hZXY_global;
    delete _hZY_global;
    delete _hX_global;
    delete _hY_global;
    delete _hZ_global;
    delete _hXY_global;
    delete _hZX_global;
    delete _hPt;
    delete _hPx;
    delete _hPy;
    delete _hPz;
    delete _hMomentumDistr;
    delete _hNumOfHitsDistr;
    delete _hTrackLength;
    delete _hHitDist;
    delete _hHitXDist;
    delete _hHitYDist;
    delete _hHitChiSq;
    delete _hNumMcTrack;
}

void BmnGlobalTrackingQA::Initialize() {

    const Float_t xMax = 150.0;
    const Float_t xMin = -150.0;
    const Float_t yMax = 150.0;
    const Float_t yMin = -150.0;
    const Float_t zMax = 1000.0;
    const Float_t zMin = 0.0;
    const UInt_t nBins = 1000;

    _hX_global = CreateHistogram1("global X distribution of hits, global tracking", "X, cm", nBins, xMin, xMax);
    _hY_global = CreateHistogram1("global Y distribution of hits, global tracking", "Y, cm", nBins, yMin, yMax);
    _hZ_global = CreateHistogram1("global Z distribution of hits, global tracking", "Z, cm", nBins, zMin, zMax);

    _hXY_global = CreateHistogram2("global XY distribution of hits, global tracking", "X, cm", "Y, cm", nBins, xMin, xMax, nBins, yMin, yMax);
    _hZX_global = CreateHistogram2("global ZX distribution of hits, global tracking", "Z, cm", "X, cm", nBins, zMin, zMax, nBins, xMin, xMax);
    _hZY_global = CreateHistogram2("global ZY distribution of hits, global tracking", "Z, cm", "Y, cm", nBins, zMin, zMax, nBins, yMin, yMax);
    
    _hZXY_global = CreateHistogram3("3D distribution of hits", "Z, cm", "X, cm", "Y, cm", 100, zMin, zMax, 100, xMin, xMax, 100, yMin, yMax);

    _hPt = CreateHistogram1("Pt distribution, global tracking", "Pt, GeV/c", nBins, 0, 0);
    _hPx = CreateHistogram1("Px distribution, global tracking", "Px, GeV/c", nBins, 0, 0);
    _hPy = CreateHistogram1("Py distribution, global tracking", "Py, GeV/c", nBins, 0, 0);
    _hPz = CreateHistogram1("Pz distribution, global tracking", "Pz, GeV/c", nBins, 0, 0);
    _hMomentumDistr = CreateHistogram1("Momentum distribution, global tracking", "Momentum, GeV/c", nBins, 0, 0);
    _hNumOfHitsDistr = CreateHistogram1("Number of hits per found track, global tracking", "Number of hits per found track", 16, 0, 16);
    _hTrackLength = CreateHistogram1("Distribution of track lengths, global tracking", "Track length, cm", nBins, 0, 1000);
    
    _hHitDist = CreateHistogram1("Distance from hit to track, global tracking", "Dist, cm", nBins, 0.0, 10.0);
    _hHitYDist = CreateHistogram1("X-distance from hit to track, global tracking", "x-dist, cm", nBins, -10.0, 10.0);
    _hHitXDist = CreateHistogram1("Y-distance from hit to track, global tracking", "y-dist, cm", nBins, -10.0, 10.0);
    
    _hHitChiSq = CreateHistogram1("Normalized #chi^{2} for hit, global tracking", "Chi", nBins, 0.0, 0.0);
    _hNumMcTrack = CreateHistogram1("Number of MC-references in one Global track", "N MC-ref", 10, 0.0, 10.0);
}

void BmnGlobalTrackingQA::Write() {

    _hZXY_global->Write(0, kOverwrite);
    _hX_global->Write(0, kOverwrite);
    _hY_global->Write(0, kOverwrite);
    _hZ_global->Write(0, kOverwrite);
    _hXY_global->Write(0, kOverwrite);
    _hZX_global->Write(0, kOverwrite);
    _hZY_global->Write(0, kOverwrite);
    _hNumMcTrack->Write(0, kOverwrite);
    
    _hPx->Write(0, kOverwrite);
    _hPy->Write(0, kOverwrite);
    _hPz->Write(0, kOverwrite);
    _hPt->Write(0, kOverwrite);
    _hMomentumDistr->Write(0, kOverwrite);
    _hNumOfHitsDistr->Write(0, kOverwrite);
    _hTrackLength->Write(0, kOverwrite);
    _hHitDist->Write(0, kOverwrite);
    _hHitXDist->Write(0, kOverwrite);
    _hHitYDist->Write(0, kOverwrite);
    _hHitChiSq->Write(0, kOverwrite);

}


ClassImp(BmnGlobalTrackingQA);