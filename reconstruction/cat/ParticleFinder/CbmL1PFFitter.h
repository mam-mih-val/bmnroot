/*
 *=====================================================
 *
 *  CBM Level 1 Reconstruction
 *
 *  Authors: M.Zyzak
 *
 *  e-mail :
 *
 *=====================================================
 *
 *  Finds Particles: Lambdas, K0
 *
 */

#ifndef _CbmL1PFFitter_h_
#define _CbmL1PFFitter_h_

#include "CbmL1Def.h"
#include <vector>

class CbmL1Track;
class CbmStsTrack;
class L1TrackPar;
class L1UMeasurementInfo;
class L1MaterialInfo;
class L1Station;
class L1FieldRegion;
class CbmKFVertex;

class CbmL1PFFitter
{
 public:
  CbmL1PFFitter();
  ~CbmL1PFFitter();

  void AddMaterial( L1TrackPar &T, fvec radThick, fvec qp0, fvec &mass2, fvec &w );
  void AddPipeMaterial( L1TrackPar &T, fvec qp0, fvec &mass2, fvec &w );
  void FilterFirst( L1TrackPar &track,fvec &x, fvec &y, fvec &w, L1Station &st );
  void FilterLast ( L1TrackPar &track,fvec &x, fvec &y, fvec &w, L1Station &st );
  void Filter( L1TrackPar &T, L1UMeasurementInfo &info, fvec &u , fvec &w);

  void Fit(std::vector<CbmL1Track> &Tracks, fvec mass = 0.1395679f);

  void CalculateFieldRegion(std::vector<CbmL1Track> &Tracks,std::vector<L1FieldRegion> &Field);
  void GetChiToVertex(std::vector<CbmL1Track> &Tracks, std::vector<float> &chiToVtx, CbmKFVertex &primVtx);

  //functions for fitting CbmStsTrack
  void Fit(std::vector<CbmStsTrack> &Tracks, int pidHypo = 211);
  void CalculateFieldRegion(std::vector<CbmStsTrack> &Tracks,std::vector<L1FieldRegion> &Field);
  void GetChiToVertex(std::vector<CbmStsTrack> &Tracks, std::vector<L1FieldRegion> &field, std::vector<float> &chiToVtx, CbmKFVertex &primVtx,
                      float chiPrim = -1);
};
#endif
