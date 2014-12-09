
#include "BmnHitMatchingQA.h"

BmnHitMatchingQA::BmnHitMatchingQA(TString name) :
    detName(name),
    _hY_matched_hits(0),
    _hX_matched_hits(0),
    _hZ_matched_hits(0),
    _hY_not_matched_hits(0),
    _hX_not_matched_hits(0),
    _hZ_not_matched_hits(0),
    _hY_well_matched_hits(0),
    _hX_well_matched_hits(0),
    _hZ_well_matched_hits(0),
    _hY_wrong_matched_hits(0),
    _hX_wrong_matched_hits(0),
    _hZ_wrong_matched_hits(0),
    _hXY_matched_hits(0),
    _hZY_matched_hits(0),
    _hZX_matched_hits(0),
    _hXY_not_matched_hits(0),
    _hZY_not_matched_hits(0),
    _hZX_not_matched_hits(0),
    _hXY_well_matched_hits(0),
    _hZY_well_matched_hits(0),
    _hZX_well_matched_hits(0),
    _hXY_wrong_matched_hits(0),
    _hZY_wrong_matched_hits(0),
    _hZX_wrong_matched_hits(0),
    _hZXY_matched_hits(0),
    _hZXY_not_matched_hits(0),
    _hZXY_well_matched_hits(0),
    _hZXY_wrong_matched_hits(0),
    _hRdist_matched_hits(0),
    _hXdist_matched_hits(0),
    _hYdist_matched_hits(0),
    _hRdist_well_matched_hits(0),
    _hXdist_well_matched_hits(0),
    _hYdist_well_matched_hits(0),
    _hRdist_wrong_matched_hits(0),
    _hXdist_wrong_matched_hits(0),
    _hYdist_wrong_matched_hits(0),
    _hNumMcTrack(0)
{
}

BmnHitMatchingQA::~BmnHitMatchingQA() {

    delete _hY_matched_hits;
    delete _hX_matched_hits;
    delete _hZ_matched_hits;
    delete _hY_not_matched_hits;
    delete _hX_not_matched_hits;
    delete _hZ_not_matched_hits;
    delete _hY_well_matched_hits;
    delete _hX_well_matched_hits;
    delete _hZ_well_matched_hits;
    delete _hY_wrong_matched_hits;
    delete _hX_wrong_matched_hits;
    delete _hZ_wrong_matched_hits;
    delete _hXY_matched_hits;
    delete _hZY_matched_hits;
    delete _hZX_matched_hits;
    delete _hXY_not_matched_hits;
    delete _hZY_not_matched_hits;
    delete _hZX_not_matched_hits;
    delete _hXY_well_matched_hits;
    delete _hZY_well_matched_hits;
    delete _hZX_well_matched_hits;
    delete _hXY_wrong_matched_hits;
    delete _hZY_wrong_matched_hits;
    delete _hZX_wrong_matched_hits;
    delete _hZXY_matched_hits;
    delete _hZXY_not_matched_hits;
    delete _hZXY_well_matched_hits;
    delete _hZXY_wrong_matched_hits;
    delete _hRdist_matched_hits;
    delete _hXdist_matched_hits;
    delete _hYdist_matched_hits;
    delete _hRdist_well_matched_hits;
    delete _hXdist_well_matched_hits;
    delete _hYdist_well_matched_hits;
    delete _hRdist_wrong_matched_hits;
    delete _hXdist_wrong_matched_hits;
    delete _hYdist_wrong_matched_hits;
    delete _hNumMcTrack;
}

void BmnHitMatchingQA::Initialize() {

    const Float_t xMax = 150.0;
    const Float_t xMin = -150.0;
    const Float_t yMax = 150.0;
    const Float_t yMin = -150.0;
    const Float_t zMax = 1000.0;
    const Float_t zMin = 0.0;
    const UInt_t nBins = 1000;
    
    _hY_matched_hits           = CreateHistogram1("distribution of matched hits over Y-coordinate, ", detName, "Y, cm", nBins, yMin, yMax);
    _hX_matched_hits           = CreateHistogram1("distribution of matched hits over X-coordinate, ", detName, "X, cm", nBins, xMin, xMax);
    _hZ_matched_hits           = CreateHistogram1("distribution of matched hits over Z-coordinate, ", detName, "Z, cm", nBins, zMin, zMax);
    _hY_not_matched_hits       = CreateHistogram1("distribution of not matched hits over Y-coordinate, ", detName, "Y, cm", nBins, yMin, yMax);
    _hX_not_matched_hits       = CreateHistogram1("distribution of not matched hits over X-coordinate, ", detName, "X, cm", nBins, xMin, xMax);
    _hZ_not_matched_hits       = CreateHistogram1("distribution of not matched hits over Z-coordinate, ", detName, "Z, cm", nBins, zMin, zMax);
    _hY_well_matched_hits      = CreateHistogram1("distribution of well matched hits over Y-coordinate, ", detName, "Y, cm", nBins, yMin, yMax);
    _hX_well_matched_hits      = CreateHistogram1("distribution of well matched hits over X-coordinate, ", detName, "X, cm", nBins, xMin, xMax);
    _hZ_well_matched_hits      = CreateHistogram1("distribution of well matched hits over Z-coordinate, ", detName, "Z, cm", nBins, zMin, zMax);
    _hY_wrong_matched_hits     = CreateHistogram1("distribution of wrong wrong matched hits over Y-coordinate, ", detName, "Y, cm", nBins, yMin, yMax);
    _hX_wrong_matched_hits     = CreateHistogram1("distribution of wrong matched hits over X-coordinate, ", detName, "X, cm", nBins, xMin, xMax);
    _hZ_wrong_matched_hits     = CreateHistogram1("distribution of wrong matched hits over Z-coordinate, ", detName, "Z, cm", nBins, zMin, zMax);
    _hXY_matched_hits          = CreateHistogram2("distribution of matched hits in XY-plane, ", detName, "X, cm", "Y, cm", nBins, xMin, xMax, nBins, yMin, yMax);
    _hZY_matched_hits          = CreateHistogram2("distribution of matched hits in ZY-plane, ", detName, "Z, cm", "Y, cm", nBins, zMin, zMax, nBins, yMin, yMax);
    _hZX_matched_hits          = CreateHistogram2("distribution of matched hits in ZX-plane, ", detName, "Z, cm", "X, cm", nBins, zMin, zMax, nBins, xMin, xMax);
    _hXY_not_matched_hits      = CreateHistogram2("distribution of not matched hits in XY-plane, ", detName, "X, cm", "Y, cm", nBins, xMin, xMax, nBins, yMin, yMax);
    _hZY_not_matched_hits      = CreateHistogram2("distribution of not matched hits in ZY-plane, ", detName, "Z, cm", "Y, cm", nBins, zMin, zMax, nBins, yMin, yMax);
    _hZX_not_matched_hits      = CreateHistogram2("distribution of not matched hits in ZX-plane, ", detName, "Z, cm", "X, cm", nBins, zMin, zMax, nBins, xMin, xMax);
    _hXY_well_matched_hits     = CreateHistogram2("distribution of well matched hits in XY-plane, ", detName, "X, cm", "Y, cm", nBins, xMin, xMax, nBins, yMin, yMax);
    _hZY_well_matched_hits     = CreateHistogram2("distribution of well matched hits in ZY-plane, ", detName, "Z, cm", "Y, cm", nBins, zMin, zMax, nBins, yMin, yMax);
    _hZX_well_matched_hits     = CreateHistogram2("distribution of well matched hits in ZX-plane, ", detName, "Z, cm", "X, cm", nBins, zMin, zMax, nBins, xMin, xMax);
    _hXY_wrong_matched_hits    = CreateHistogram2("distribution of wrong matched hits in XY-plane, ", detName, "X, cm", "Y, cm", nBins, xMin, xMax, nBins, yMin, yMax);
    _hZY_wrong_matched_hits    = CreateHistogram2("distribution of wrong matched hits in ZY-plane, ", detName, "Z, cm", "Y, cm", nBins, zMin, zMax, nBins, yMin, yMax);
    _hZX_wrong_matched_hits    = CreateHistogram2("distribution of wrong matched hits in ZX-plane, ", detName, "Z, cm", "X, cm", nBins, zMin, zMax, nBins, xMin, xMax);
    _hZXY_matched_hits         = CreateHistogram3("distribution of matched hits in 3D, ", detName, "Z, cm", "X, cm", "Y, cm", 100, zMin, zMax, 100, xMin, xMax, 100, yMin, yMax);
    _hZXY_not_matched_hits     = CreateHistogram3("distribution of not matched hits in 3D, ", detName, "Z, cm", "X, cm", "Y, cm", 100, zMin, zMax, 100, xMin, xMax, 100, yMin, yMax);
    _hZXY_well_matched_hits    = CreateHistogram3("distribution of well matched hits in 3D, ", detName, "Z, cm", "X, cm", "Y, cm", 100, zMin, zMax, 100, xMin, xMax, 100, yMin, yMax);
    _hZXY_wrong_matched_hits   = CreateHistogram3("distribution of wrong matched hits in 3D, ", detName, "Z, cm", "X, cm", "Y, cm", 100, zMin, zMax, 100, xMin, xMax, 100, yMin, yMax);
    _hRdist_matched_hits       = CreateHistogram1("distribution of distances from matched hit to track, ", detName, "Dist, cm", nBins, 0.0, 10.0);
    _hXdist_matched_hits       = CreateHistogram1("distribution of X-distances from matched hit to track, ", detName, "X-dist, cm", nBins, -10.0, 10.0);
    _hYdist_matched_hits       = CreateHistogram1("distribution of Y-distances from matched hit to track, ", detName, "Y-dist, cm", nBins, -10.0, 10.0);
    _hRdist_well_matched_hits  = CreateHistogram1("distribution of distances from well matched hit to track, ", detName, "Dist, cm", nBins, 0.0, 0.0);
    _hXdist_well_matched_hits  = CreateHistogram1("distribution of X-distances from well matched hit to track, ", detName, "X-dist, cm", nBins, 0.0, 0.0);
    _hYdist_well_matched_hits  = CreateHistogram1("distribution of Y-distances from well matched hit to track, ", detName, "Y-dist, cm", nBins, 0.0, 0.0);
    _hRdist_wrong_matched_hits = CreateHistogram1("distribution of distances from wrong matched hit to track, ", detName, "Dist, cm", nBins, 0.0, 0.0);
    _hXdist_wrong_matched_hits = CreateHistogram1("distribution of X-distances from wrong matched hit to track, ", detName, "X-dist, cm", nBins, 0.0, 0.0);
    _hYdist_wrong_matched_hits = CreateHistogram1("distribution of Y-distances from wrong matched hit to track, ", detName, "Y-dist, cm", nBins, 0.0, 0.0);
    
    _hNumMcTrack = CreateHistogram1("Number of MC-references in one Global track, after ", detName, "N MC-ref", 10, 0.0, 10.0);
    
}

void BmnHitMatchingQA::Write() {
    
    _hY_matched_hits->Write(0, kOverwrite);
    _hX_matched_hits->Write(0, kOverwrite);
    _hZ_matched_hits->Write(0, kOverwrite);
    _hY_not_matched_hits->Write(0, kOverwrite);
    _hX_not_matched_hits->Write(0, kOverwrite);
    _hZ_not_matched_hits->Write(0, kOverwrite);
    _hY_well_matched_hits->Write(0, kOverwrite);
    _hX_well_matched_hits->Write(0, kOverwrite);
    _hZ_well_matched_hits->Write(0, kOverwrite);
    _hY_wrong_matched_hits->Write(0, kOverwrite);
    _hX_wrong_matched_hits->Write(0, kOverwrite);
    _hZ_wrong_matched_hits->Write(0, kOverwrite);
    _hXY_matched_hits->Write(0, kOverwrite);
    _hZY_matched_hits->Write(0, kOverwrite);
    _hZX_matched_hits->Write(0, kOverwrite);
    _hXY_not_matched_hits->Write(0, kOverwrite);
    _hZY_not_matched_hits->Write(0, kOverwrite);
    _hZX_not_matched_hits->Write(0, kOverwrite);
    _hXY_well_matched_hits->Write(0, kOverwrite);
    _hZY_well_matched_hits->Write(0, kOverwrite);
    _hZX_well_matched_hits->Write(0, kOverwrite);
    _hXY_wrong_matched_hits->Write(0, kOverwrite);
    _hZY_wrong_matched_hits->Write(0, kOverwrite);
    _hZX_wrong_matched_hits->Write(0, kOverwrite);
    _hZXY_matched_hits->Write(0, kOverwrite);
    _hZXY_not_matched_hits->Write(0, kOverwrite);
    _hZXY_well_matched_hits->Write(0, kOverwrite);
    _hZXY_wrong_matched_hits->Write(0, kOverwrite);
    _hRdist_matched_hits->Write(0, kOverwrite);
    _hXdist_matched_hits->Write(0, kOverwrite);
    _hYdist_matched_hits->Write(0, kOverwrite);
    _hRdist_well_matched_hits->Write(0, kOverwrite);
    _hXdist_well_matched_hits->Write(0, kOverwrite);
    _hYdist_well_matched_hits->Write(0, kOverwrite);
    _hRdist_wrong_matched_hits->Write(0, kOverwrite);
    _hXdist_wrong_matched_hits->Write(0, kOverwrite);
    _hYdist_wrong_matched_hits->Write(0, kOverwrite);
    _hNumMcTrack->Write(0, kOverwrite);

}


ClassImp(BmnHitMatchingQA);