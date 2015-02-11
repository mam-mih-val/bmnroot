
//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class TpcClusterFinderQAHistograms
//         
//
// Environment:
//      Software developed for the MPD at NICA.
//
// Author List:
//      Roman Salmin            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "BmnGemSeedFinderQA.h"
#include "TColor.h"
#include <sstream>

ClassImp(BmnGemSeedFinderQA);

BmnGemSeedFinderQA::BmnGemSeedFinderQA() :
_hPx(0), _hPy(0), _hPz(0), _hPt(0),
_hMomentumDistr(0),
_hNumOfHitsDistr(0),
_hStationAllHits(0), _hStationUsedHits(0), _hStationNotUsedHits(0),
_hStationWellHits(0), _hStationWrongHits(0),
_hHitsDist(0), _hHitsXDist(0), _hHitsYDist(0),
_hXYGoodHits(0), _hXYBadHits(0), _hXYAllHits(0),
_hZYGoodHits(0), _hZYBadHits(0), _hZYAllHits(0),
_hZXGoodHits(0), _hZXBadHits(0), _hZXAllHits(0),
_hXYFoundHits(0), _hZXFoundHits(0), _hZYFoundHits(0),
_hyRxR_all(0), _hyRxR_good(0), _hyRxR_bad(0), _hNumMcTrack(0), _hPsimPrec(0)
{
}

BmnGemSeedFinderQA::~BmnGemSeedFinderQA() {
    delete _hPt;
    delete _hPx;
    delete _hPy;
    delete _hPz;
    delete _hMomentumDistr;
    delete _hNumOfHitsDistr;
    delete _hXYGoodHits;
    delete _hXYBadHits;
    delete _hXYAllHits;
    delete _hZXGoodHits;
    delete _hZXBadHits;
    delete _hZXAllHits;
    delete _hZYGoodHits;
    delete _hZYBadHits;
    delete _hZYAllHits;
    delete _hyRxR_all;
    delete _hyRxR_good;
    delete _hyRxR_bad;
    delete _hXYFoundHits;
    delete _hZYFoundHits;
    delete _hZXFoundHits;
    delete _hHitsDist;
    delete _hHitsXDist;
    delete _hHitsYDist;
    delete _hStationAllHits;
    delete _hStationUsedHits;
    delete _hStationNotUsedHits;
    delete _hStationWellHits;
    delete _hStationWrongHits;
    delete _hNumMcTrack;
    delete _hPsimPrec;
}

void BmnGemSeedFinderQA::Initialize() {
    
    const Float_t xMin = -50.0;
    const Float_t xMax = -xMin;
    const Float_t yMin = -50.0;
    const Float_t yMax = -yMin;
    const Float_t zMin = 0.0;
    const Float_t zMax = 360.0;
    const UInt_t  nBins = 1000;
    
    _hPt = CreateHistogram1("Pt distribution, seeding", _prefix, "Pt, GeV/c", 500, 0, 5);
    _hPx = CreateHistogram1("Px distribution, seeding", _prefix, "Px, GeV/c", 500, -1, 1);
    _hPy = CreateHistogram1("Py distribution, seeding", _prefix, "Py, GeV/c", 500, -1, 1);
    _hPz = CreateHistogram1("Pz distribution, seeding", _prefix, "Pz, GeV/c", 500, -2, 4);
    _hMomentumDistr = CreateHistogram1("Momentum distribution, seeding", _prefix, "Momentum, GeV/c", 500, 0, 5);
    _hNumOfHitsDistr = CreateHistogram1("Number of hits per found track, seeding", _prefix, "Number of hits per found track", 16, 0, 16);

    _hXYGoodHits = CreateHistogram2("XY GoodHits, seeding", _prefix, "x", "y", nBins, xMin, xMax, nBins, yMin, yMax);
    _hXYGoodHits->SetMarkerStyle(7);
    _hXYGoodHits->SetMarkerColor(TColor::GetColor("#00cc00"));
    _hXYBadHits = CreateHistogram2("XY BadHits, seeding", _prefix, "x", "y", nBins, xMin, xMax, nBins, yMin, yMax);
    _hXYBadHits->SetMarkerStyle(7);
    _hXYBadHits->SetMarkerColor(TColor::GetColor("#cc0000"));    
    _hXYAllHits = CreateHistogram2("XY AllHits, seeding", _prefix, "x", "y", nBins, xMin, xMax, nBins, yMin, yMax);
    _hXYAllHits->SetMarkerStyle(7);
    
    _hZXGoodHits = CreateHistogram2("ZX GoodHits, seeding", _prefix, "z", "x", nBins, zMin, zMax, nBins, xMin, xMax);
    _hZXGoodHits->SetMarkerStyle(7);
    _hZXGoodHits->SetMarkerColor(TColor::GetColor("#00cc00"));
    _hZXBadHits = CreateHistogram2("ZX BadHits, seeding", _prefix, "z", "x", nBins, zMin, zMax, nBins, xMin, xMax);
    _hZXBadHits->SetMarkerStyle(7);
    _hZXBadHits->SetMarkerColor(TColor::GetColor("#cc0000"));    
    _hZXAllHits = CreateHistogram2("ZX AllHits, seeding", _prefix, "z", "x", nBins, zMin, zMax, nBins, xMin, xMax);
    _hZXAllHits->SetMarkerStyle(7);
    
    _hZYGoodHits = CreateHistogram2("ZY GoodHits, seeding", _prefix, "z", "y", nBins, zMin, zMax, nBins, yMin, yMax);
    _hZYGoodHits->SetMarkerStyle(7);
    _hZYGoodHits->SetMarkerColor(TColor::GetColor("#00cc00"));
    _hZYBadHits = CreateHistogram2("ZY BadHits, seeding", _prefix, "z", "y", nBins, zMin, zMax, nBins, yMin, yMax);
    _hZYBadHits->SetMarkerStyle(7);
    _hZYBadHits->SetMarkerColor(TColor::GetColor("#cc0000"));    
    _hZYAllHits = CreateHistogram2("ZY AllHits, seeding", _prefix, "z", "y", nBins, zMin, zMax, nBins, yMin, yMax);
    _hZYAllHits->SetMarkerStyle(7);
    
    _hyRxR_all = CreateHistogram2("y/R vs. x/R all hits, seeding", _prefix, "x/R", "y/R", nBins, -0.8, 0.8, nBins, -0.8, 0.8);
    _hyRxR_good = CreateHistogram2("y/R vs. x/R good hits, seeding", _prefix, "x/R", "y/R", nBins, -0.8, 0.8, nBins, -0.8, 0.8);
    _hyRxR_bad = CreateHistogram2("y/R vs. x/R bad hits, seeding", _prefix, "x/R", "y/R", nBins, -0.8, 0.8, nBins, -0.8, 0.8);
    
    _hXYFoundHits = CreateHistogram2("XY found hits, seeding", _prefix, "x", "y", nBins, xMin, xMax, nBins, yMin, yMax);
    _hZXFoundHits = CreateHistogram2("ZX found hits, seeding", _prefix, "z", "x", nBins, zMin, zMax, nBins, xMin, xMax);
    _hZYFoundHits = CreateHistogram2("ZY found hits, seeding", _prefix, "z", "y", nBins, zMin, zMax, nBins, yMin, yMax);
    
    _hHitsDist = CreateHistogram1("Average distance from hit to track, seeding", "Dist, cm", nBins, 0.0, 0.0);
    _hHitsXDist = CreateHistogram1("Average X-distance from hit to track, seeding", "x-dist, cm", nBins, 0.0, 0.0);
    _hHitsYDist = CreateHistogram1("Average Y-distance from hit to track, seeding", "y-dist, cm", nBins, 0.0, 0.0);
    
    _hStationAllHits = CreateHistogram1("Distribution of all hits over stations, seeding", "Station", 12, 0, 13);
    _hStationUsedHits = CreateHistogram1("Distribution of found hits over stations, seeding", "Station", 12, 0, 13);
    _hStationNotUsedHits = CreateHistogram1("Distribution of unfound hits over stations, seeding", "Station", 12, 0, 13);
    _hStationWellHits = CreateHistogram1("Distribution of well found hits over stations, seeding", "Station", 12, 0, 13);
    _hStationWrongHits = CreateHistogram1("Distribution of wrong found hits over stations, seeding", "Station", 12, 0, 13);
    
    _hNumMcTrack = CreateHistogram1("Number of MC-references in one Global track, after Seeding", "N MC-ref", 10, 0.0, 10.0);
    
    _hPsimPrec = CreateHistogram2("hPsimPrec", _prefix, "P_{sim}", "P_{rec}", nBins, 0.0, 0.0, nBins, 0.0, 0.0);
    
}

void BmnGemSeedFinderQA::Write() {
    _hPt->Write(0, kOverwrite);
    _hPx->Write(0, kOverwrite);
    _hPy->Write(0, kOverwrite);
    _hPz->Write(0, kOverwrite);
    _hMomentumDistr->Write(0, kOverwrite);
    _hNumOfHitsDistr->Write(0, kOverwrite);
    _hXYGoodHits->Write(0, kOverwrite);
    _hXYBadHits->Write(0, kOverwrite);
    _hXYAllHits->Write(0, kOverwrite);
    _hZXGoodHits->Write(0, kOverwrite);
    _hZXBadHits->Write(0, kOverwrite);
    _hZXAllHits->Write(0, kOverwrite);
    _hZYGoodHits->Write(0, kOverwrite);
    _hZYBadHits->Write(0, kOverwrite);
    _hZYAllHits->Write(0, kOverwrite);
    _hyRxR_all->Write(0, kOverwrite);
    _hyRxR_bad->Write(0, kOverwrite);
    _hyRxR_good->Write(0, kOverwrite);
    _hXYFoundHits->Write(0, kOverwrite);
    _hZXFoundHits->Write(0, kOverwrite);
    _hZYFoundHits->Write(0, kOverwrite);
    _hHitsDist->Write(0, kOverwrite);
    _hHitsXDist->Write(0, kOverwrite);
    _hHitsYDist->Write(0, kOverwrite);
    _hStationAllHits->Write(0, kOverwrite);
    _hStationUsedHits->Write(0, kOverwrite);
    _hStationNotUsedHits->Write(0, kOverwrite);
    _hStationWellHits->Write(0, kOverwrite);
    _hStationWrongHits->Write(0, kOverwrite);
    _hNumMcTrack->Write(0, kOverwrite);
    _hPsimPrec->Write(0, kOverwrite);
}