
#ifndef L1Algo_h
#define L1Algo_h 1

// #define TBB // TODO: Doesn't work now. Renew

  /// Debug features
// #define PULLS            // triplets pulls
// #define TRIP_PERFORMANCE // triplets efficiencies
// #define DOUB_PERFORMANCE // doublets efficiencies
// #define DRAW             // event display
// #define XXX              // time debug
// #define COUNTERS          diff counters (hits, doublets, ... )

// #define TRACKS_FROM_TRIPLETS
// #define DO_NOT_SELECT_TRIPLETS
// #define TRACKS_FROM_TRIPLETS_ITERATION kAllPrimIter

#define LAST_ITERATION kAllSecIter
#define FIND_GAPED_TRACKS // use triplets with gaps
#define USE_RL_TABLE
#ifndef TRACKS_FROM_TRIPLETS
#define EXTEND_TRACKS
#endif
#define MERGE_CLONES



#include "L1Field.h"
#include "L1Station.h"
#include "L1StsHit.h"
#include "L1Triplet.h"
#include "L1Branch.h"
#include "L1Track.h"
#include "L1TrackPar.h"

#include "L1Portion.h"
#include "L1HitPoint.h"
#include "L1Strip.h"
#include "L1Grid.h"

#include <iostream>
#include <vector>
#include <map>

using std::vector;
using std::map;

#ifdef PULLS
#define TRIP_PERFORMANCE
class L1AlgoPulls;
#endif
#ifdef TRIP_PERFORMANCE
template<int NHits> class L1AlgoEfficiencyPerformance;
#endif
#ifdef DOUB_PERFORMANCE
template<int NHits> class L1AlgoEfficiencyPerformance;
#endif

class L1Algo{
 public:
  L1Algo():
  NStations(0),    // number of all detector stations
    NMvdStations(0), // number of mvd stations
    fRadThick(),
    vStsStrips(),  // strips positions created from hits. Front strips
    vStsStripsB(), // back strips
    vStsZPos(),    // all possible z-positions of hits
    vStsHits(),     // hits as a combination of front-, backstrips and z-position
    vSFlag(),  // information of hits station & using hits in tracks(),
    vSFlagB(),
    isec(-1), // iteration
    vStsHitsUnused(0),
    vStsHitPointsUnused(0),
    RealIHit(0), // index in vStsHits indexed by index in vStsHitsUnusde
    vTracks(), // reconstructed tracks
    vRecoHits(), // packed hits of reconstructed tracks
    CATime(0), // time of trackfinding
    FIRSTCASTATION(0),

    TRACK_CHI2_CUT(10.),
    TRIPLET_CHI2_CUT(5.),
    DOUBLET_CHI2_CUT(5.),
    
    MaxDZ(0),
    Pick_gather(0),
    PickNeighbour(0), // (PickNeighbour < dp/dp_error)  =>  triplets are neighbours
    MaxInvMom(0),     // max considered q/p for tracks
    MaxSlope(0),
    targX(0), targY(0), targZ(0),                        // target coor
    targB(),               // field in the target point
    TargetXYInfo(), // target constraint  [cm]
    vtxFieldRegion(),// really doesn't used
    vtxFieldValue(), // field at teh vertex position.
    vTriplets(), // container for triplets got in finding
    fTrackingLevel(0), fGhostSuppression(0), // really doesn't used
    fMomentumCutOff(0)// really doesn't used
  {}
       
  void Init( const fscal geo[] );
#ifdef TBB2
  void SetData( const vector< L1StsHit >      & StsHits_,
                const vector< L1Strip >       & StsStrips_,
                const vector< L1Strip >       & StsStripsB_,
                const vector< fscal >         & StsZPos_,
                const vector< unsigned char > & SFlag_,
                const vector< unsigned char > & SFlagB_,
                const THitI* StsHitsStartIndex_,
                const THitI* StsHitsStopIndex_ );
  void PrintHits();
#endif
    /// The main procedure - find tracks.
  void CATrackFinder();

    /// Track fitting procedures
  void KFTrackFitter_simple(); // version, which use procedured used during the reconstruction
  void L1KFTrackFitter( bool extrapolateToTheEndOfSTS = false );        // version from SIMD-KF benchmark

    /// ----- Input data ----- 
      // filled in CbmL1::ReadEvent();

  enum{ MaxNStations = 12 };
  int NStations,    // number of all detector stations
      NMvdStations; // number of mvd stations
  L1Station vStations[MaxNStations] _fvecalignment; // station info

  vector<L1Material> fRadThick; // material for each station
  
  vector< L1Strip > vStsStrips,  // strips positions created from hits. Front strips
                    vStsStripsB; // back strips
  vector< fscal >   vStsZPos;    // all possible z-positions of hits
  vector< L1StsHit > vStsHits;     // hits as a combination of front-, backstrips and z-position
  L1Grid vGrid[MaxNStations];     // hits as a combination of front-, backstrips and z-position
  vector< unsigned char > vSFlag,  // information of hits station & using hits in tracks;
                          vSFlagB;
  THitI StsHitsStartIndex[MaxNStations+1], StsHitsStopIndex[MaxNStations+1]; // station-bounders in vStsHits array

   /// --- data used during finding iterations

  int isec; // iteration
  vector< L1StsHit > *vStsHitsUnused;
  std::vector< L1HitPoint > *vStsHitPointsUnused;
  THitI *RealIHit; // index in vStsHits indexed by index in vStsHitsUnused
  THitI StsHitsUnusedStartIndex[MaxNStations+1], StsHitsUnusedStopIndex[MaxNStations+1];
  

    /// ----- Output data ----- 
  vector< L1Track > vTracks; // reconstructed tracks
  vector< THitI > vRecoHits; // packed hits of reconstructed tracks

  double CATime; // time of trackfinding

  friend class CbmL1;

  L1FieldValue GetvtxFieldValue() {return vtxFieldValue;}

 private:

        /// =================================  FUNCTIONAL PART  =================================

  const L1FieldValue&  GetVtxFieldValue() const {return vtxFieldValue;}
  const L1FieldRegion& GetVtxFieldRegion() const {return vtxFieldRegion;}
    /// ----- Hit-point-strips conversion routines ------

  void GetHitCoor(const L1StsHit& _h, fscal &_x, fscal &_y, fscal &_z, const L1Station &sta);
  void StripsToCoor(const fscal &u, const fscal &v, fscal &x, fscal &y, const L1Station &sta) const; // convert strip positions to coordinates
  void StripsToCoor(const fvec &u, const fvec &v, fvec &x, fvec &y, const L1Station &sta) const;
  L1HitPoint CreateHitPoint(const L1StsHit &hit, char ista); // full the hit point by hit information.

  
    /// ----- Subroutines used by L1Algo::CATrackFinder() ------
    
  void CAFindTrack(int ista,
                   L1Branch& best_tr, unsigned char &best_L, fscal &best_chi2,
                   const L1Triplet* curr_trip,
                   L1Branch &curr_tr, unsigned char &curr_L, fscal &curr_chi2,
                   int &NCalls );

  void CleanTriplets(bool isClean = 0
#ifdef COUNTERS
                     ,unsigned int* nlevels = 0
#endif
    ); // isClean = true - find neighbours only 
  
    /// Fit track
    /// t - track with hits
    /// T - track params
    /// dir - 0 - forward, 1 - backward
    /// qp0 - momentum for extrapolation
    /// initialize - should be params ititialized. 1 - yes.
  void BranchFitterFast(const L1Branch &t, L1TrackPar& T, const bool dir, const fvec qp0 = 0., const bool initParams = true);
  
    /// Fit track. more precise than FitterFast
  void BranchFitter(const L1Branch &t, L1TrackPar& T, const bool dir, const fvec qp0 = 0., const bool initParams = true);
  
    /// Find additional hits for existing track
    /// t - track with hits
    /// T - track params
    /// dir - 0 - forward, 1 - backward
    /// qp0 - momentum for extrapolation
  void FindMoreHits(L1Branch &t, L1TrackPar& T, const bool dir, const fvec qp0 = 0.0);

      /// Find additional hits for existing track
      /// return chi2
  fscal BranchExtender(L1Branch &t);

    /// ----- Subroutines used by L1Algo::CAMergeClones() ------
  void InvertCholetsky(fvec a[15]);
  void MultiplySS(fvec const C[15], fvec const V[15], fvec K[5][5]);
  void MultiplyMS(fvec const C[5][5], fvec const V[15], fvec K[15]);
  void MultiplySR(fvec const C[15], fvec const r_in[5], fvec r_out[5]);
  void FilterTracks(fvec const r[5], fvec const C[15], fvec const m[5], fvec const V[15], fvec R[5], fvec W[15], fvec *chi2);
  void CAMergeClones();

    /// -- Flags routines --
  unsigned char GetFStation( unsigned char flag ){ return flag/4; }
  bool GetFUsed   ( unsigned char flag ){ return (flag&0x02)!=0; }
//   bool GetFUsedD  ( unsigned char flag ){ return (flag&0x01)!=0; }

  void SetFStation ( unsigned char &flag, unsigned int iStation ){ flag = iStation*4 + (flag%4); }
  void SetFUsed    ( unsigned char &flag ){ flag |= 0x02; }
//   void SetFUsedD   ( unsigned char &flag ){ flag |= 0x01; }
  void SetFUnUsed  ( unsigned char &flag ){ flag &= 0xFC; }
//   void SetFUnUsedD ( unsigned char &flag ){ flag &= 0xFE; }

          /// Prepare the portion of left hits data
  void f10(  // input
                int start_lh, int n1,  L1HitPoint *vStsHits_l, 
                  // output
                fvec *u_front, fvec *u_back,  fvec *zPos,
                THitI *hitsl_1,
		fvec *du_front = 0, fvec *du_back = 0 //AZ
                );

          /// Get the field approximation. Add the target to parameters estimation. Propagate to middle station.
  void f11(  // input
                int istal, int istam,
                int n1_V, 

                fvec *u_front, fvec *u_back,  fvec *zPos,
                  // output
                L1TrackPar *T_1, L1FieldRegion *fld_1,
		fvec *du_front = 0, fvec *du_back = 0 //AZ
               );
  
          /// Find the doublets. Reformat data in the portion of doublets.
  void f20(  // input
                int n1, L1Station &stam,
                L1HitPoint *vStsHits_l, L1HitPoint *vStsHits_m, int NHits_m,          
                L1TrackPar *T_1,
                THitI *hitsl_1,
                  // output
                int &n2,
                vector<THitI> &i1_2,
                int &start_mhit,
#ifdef DOUB_PERFORMANCE
                vector<THitI> &hitsl_2,
#endif // DOUB_PERFORMANCE
                vector<THitI> &hitsm_2,
                vector<bool> &lmDuplets
                );
           /// Add the middle hits to parameters estimation. Propagate to right station.
          /// Find the triplets (right hit). Reformat data in the portion of triplets.
  void f30(  // input
                L1HitPoint *vStsHits_r, L1Station &stam, L1Station &star,
                
                int istam, int istar,
                L1HitPoint *vStsHits_m,
                L1TrackPar *T_1,L1FieldRegion *fld_1,
                THitI *hitsl_1,
 
                int n2,
                vector<THitI> &hitsm_2,
                vector<THitI> &i1_2,
                
                const vector<bool> &mrDuplets,
                  // output
                int &n3,
                nsL1::vector<L1TrackPar>::TSimd &T_3,
                vector<THitI> &hitsl_3,  vector<THitI> &hitsm_3,  vector<THitI> &hitsr_3,
                nsL1::vector<fvec>::TSimd &u_front_3, nsL1::vector<fvec>::TSimd &u_back_3, nsL1::vector<fvec>::TSimd &z_Pos_3,
		nsL1::vector<fvec>::TSimd *du_front_3 = 0, nsL1::vector<fvec>::TSimd *du_back_3 = 0 //AZ
                );
          
          /// Add the right hits to parameters estimation.
  void f31(  // input
                int n3_V,  
                L1Station &star, 
                nsL1::vector<fvec>::TSimd &u_front_3, nsL1::vector<fvec>::TSimd &u_back_3, nsL1::vector<fvec>::TSimd &z_Pos_3,
                  // output
                nsL1::vector<L1TrackPar>::TSimd &T_3,
		nsL1::vector<fvec>::TSimd *du_front = 0, nsL1::vector<fvec>::TSimd *du_back = 0 //AZ
               );

          /// Refit Triplets.
  void f32( // input
                int n3, int istal,
                nsL1::vector<L1TrackPar>::TSimd &T_3,
                vector<THitI> &hitsl_3,  vector<THitI> &hitsm_3,  vector<THitI> &hitsr_3,
                int nIterations = 0
                         );
  
          /// Select triplets. Save them into vTriplets.
  void f4(  // input
                int n3, int istal, int istam, int istar,
                nsL1::vector<L1TrackPar>::TSimd &T_3,
                vector<THitI> &hitsl_3,  vector<THitI> &hitsm_3,  vector<THitI> &hitsr_3,
                // output
                unsigned int &nstaltriplets,
                vector<L1Triplet> &vTriplets_part,
                unsigned int *TripStartIndexH, unsigned int *TripStopIndexH
// #ifdef XXX                
//                 ,unsigned int &stat_n_trip      
// #endif
               );

          /// Find neighbours of triplets. Calculate level of triplets.
  void f5(  // input
                 // output
               unsigned int *TripStartIndexH, unsigned int *TripStopIndexH,
               int *nlevel
               );

         /// Find doublets on station
  void DupletsStaPort(  // input
                      int istal, int istam,
                      unsigned int ip,
                      vector< int>& n_g1, unsigned int *portionStopIndex,
            
                        // output
                      L1TrackPar *T_1,
                      L1FieldRegion *fld_1,
                      THitI *hitsl_1,
                      
                      vector<bool> &lmDuplets,
                      
                      int &n_2,
                      vector<THitI> &i1_2,
                      vector<THitI> &hitsm_2
                      );
  
            /// Find triplets on station
  void TripletsStaPort(  // input
                            int istal, int istam, int istar,
                            unsigned int& nstaltriplets,
                            L1TrackPar *T_1,
                            L1FieldRegion *fld_1,
                            THitI *hitsl_1,

                            int &n_2, unsigned int *portionStopIndex,
                            vector<THitI> &i1_2,
                            vector<THitI> &hitsm_2,
                              
                            const vector<bool> &mrDuplets,
                            
                              // output
                            vector<L1Triplet> *vTriplets_part,
                            unsigned int *TripStartIndexH, unsigned int *TripStopIndexH
                            );
  
    ///  ------ Subroutines used by L1Algo::KFTrackFitter()  ------
  
  void GuessVec( L1TrackPar &t, fvec *xV, fvec *yV, fvec *zV, fvec *Sy, fvec *wV, int NHits, fvec *zCur = 0 );
  void FilterFirst( L1TrackPar &track,fvec &x, fvec &y, L1Station &st );
  
#ifdef TBB
  enum { 
    nthreads = 3, // number of threads
    nblocks = 1 // number of stations on one thread
  };   

  friend class ParalleledDup;
  friend class ParalleledTrip;
#endif // TBB
#ifdef TBB2
  public:
  int thrId;
#endif // TBB2
  private:
  
        /// =================================  DATA PART  =================================
  
    /// ----- Different parameters of CATrackFinder -----

  int FIRSTCASTATION;  //first station used in CA

    // fNFindIterations - set number of interation for trackfinding
    // itetation of finding:
#ifdef FIND_GAPED_TRACKS
  enum { kFastPrimIter, // primary fast tracks
         kAllPrimIter,      // primary all tracks
         kAllPrimJumpIter,  // primary tracks with jumped triplets
         kAllSecIter,       // secondary all tracks
         kAllPrimEIter,      // primary all electron tracks
         kAllSecEIter,      // secondary all electron tracks
         
         kFastPrimJumpIter, // primary fast tracks with jumped triplets
         kFastPrimIter2,
         kAllSecJumpIter    // secondary tracks with jumped triplets
  };
#ifdef TRACKS_FROM_TRIPLETS
  enum { fNFindIterations = TRACKS_FROM_TRIPLETS_ITERATION+1 }; // TODO investigate kAllPrimJumpIter & kAllSecJumpIter
#else
  enum { fNFindIterations = LAST_ITERATION+1 }; // TODO investigate kAllPrimJumpIter & kAllSecJumpIter
#endif
#else
  enum { fNFindIterations = 3 };
  enum { kFastPrimIter = 0, // primary fast tracks
         kAllPrimIter,      // primary all tracks
         kAllSecIter,       // secondary all tracks
         kFastPrimJumpIter, // disabled
         kAllPrimJumpIter,  // disabled
         kFastPrimIter2,
         kAllSecJumpIter
  };
#endif // FIND_GAPED_TRACKS
  
  const float TRACK_CHI2_CUT; // = 10.0;  // cut for tracks candidates. per one DoF
  float TRIPLET_CHI2_CUT; // = 5.0; // cut for selecting triplets before collecting tracks.per one DoF
  float DOUBLET_CHI2_CUT;

  
  fvec MaxDZ; // correction in order to take into account overlaping and iff z. if sort by y then it is max diff between same station's modules (~0.4cm)
  
    /// parameters which are different for different iterations. Set in the begin of CAL1TrackFinder
  fvec Pick_gather; // coefficient for size of region  for attaching additional hits to track
  float PickNeighbour; // (PickNeighbour < dp/dp_error)  =>  triplets are neighbours
  fvec MaxInvMom;     // max considered q/p for tracks
  fvec MaxSlope;      // max slope (tx\ty) in prim vertex
  fvec targX, targY, targZ;                        // target coor
  L1FieldValue targB _fvecalignment;               // field in the target point
  L1XYMeasurementInfo TargetXYInfo _fvecalignment; // target constraint  [cm]

    /// standard sizes of the arrays
  enum { 
    multiCoeff = 1, // central - 1, mbias -

    coeff = 64/4,

    Portion = 1024/coeff, // portion of left hits

    MaxPortionDoublets = 10000/5 * 64/2 /coeff /*/ multiCoeff*/*1,
    MaxPortionTriplets = 10000*5 * 64/2 /coeff /*/ multiCoeff*/*1,
    MaxNPortion = 40 * coeff / multiCoeff,

    
    MaxArrSize = MaxNPortion*MaxPortionDoublets/MaxNStations    //200000,  // standart size of big arrays  // mas be 40000 for normal work in cbmroot!
  };

  L1FieldRegion vtxFieldRegion _fvecalignment;// really doesn't used
  L1FieldValue  vtxFieldValue _fvecalignment; // field at teh vertex position.

  vector <L1Triplet> vTriplets; // container for triplets got in finding
  int TripStartIndex[MaxNStations-2], TripStopIndex[MaxNStations-2]; // containers for stations bounders in vTriplets. indices are from start to stop-1

  int fTrackingLevel, fGhostSuppression; // really doesn't used
  float fMomentumCutOff;// really doesn't used

    /// ----- Debug features -----
#ifdef PULLS
  L1AlgoPulls* fL1Pulls;
#endif
#ifdef TRIP_PERFORMANCE
  L1AlgoEfficiencyPerformance<3>* fL1Eff_triplets;
  L1AlgoEfficiencyPerformance<3>* fL1Eff_triplets2;
#endif
#ifdef DOUB_PERFORMANCE
  L1AlgoEfficiencyPerformance<2>* fL1Eff_doublets;
#endif
#ifdef DRAW
  friend class L1AlgoDraw;
#endif

private:
  L1Algo(const L1Algo&);
  void operator=(const L1Algo&);
} _fvecalignment;

#endif
