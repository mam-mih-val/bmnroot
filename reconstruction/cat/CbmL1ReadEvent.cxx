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
 *  Read Event information to L1
 *
 *====================================================================
 */

#include "CbmL1.h"
#include "L1Algo/L1Algo.h"
#include "CbmKF.h"
#include "CbmStsPoint.h"
#include "CbmStsDigi.h"
#include "CbmStsCluster.h"
#include "CbmStsHit.h"
#include "CbmMvdPoint.h"
#include "CbmMvdHit.h"
#include "CbmMvdHitMatch.h"
#include "CbmMCTrack.h"

#include "TDatabasePDG.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;
using std::find;

struct TmpMCPoint{ // used for sort MCPoints for creation of MCTracks
  int ID;         // MCPoint ID
  double z;       // MCPoint z
  int StsHit;     // index of hit in algo->vStsHits
  int MCPoint;    // index of mcPoint in vMCPoints
  bool eff;
  static bool compareIDz( const TmpMCPoint &a, const TmpMCPoint &b )
  {
    return ( a.ID < b.ID ) || ( ( a.ID == b.ID ) && (a.z < b.z) );
  }
};

struct TmpHit{ // used for sort Hits before writing in the normal arrays
  int iStripF, iStripB;   // indices of real-strips, sts-strips (one got from detector. They consist from parts with differen positions, so will be divided before using)
  int indStripF, indStripB; // indices of L1-strips, indices in TmpStrip arrays
  int iSector;
  int iStation;
  int ExtIndex;           // index of hit in the TClonesArray array ( negative for MVD )
  bool isStrip;
  double u_front, u_back; // positions of strips
  double du_front, du_back; //AZ - errors of strips
  double x, y;            // position of hit
  int iMC;                // index of MCPoint in the vMCPoints array
  static bool Compare( const TmpHit &a, const TmpHit &b ){
    return ( a.iStation <  b.iStation ) ||
         ( ( a.iStation == b.iStation ) && ( a.y < b.y ) );
  }
};

struct TmpStrip{
  fscal u;
  fscal du; //AZ
  int iStation;
  int iSector, iStrip;
  bool isStrip;
  int effIndex; // used for unefficiency
};

  /// Repack data from Clones Arrays to L1 arrays
void CbmL1::ReadEvent()
{
  if (fVerbose >= 10) cout << "ReadEvent: start." << endl;
    // clear arrays for next event
  vMCPoints.clear();
  vMCTracks.clear();
  vStsHits.clear();
  vRTracks.clear();
  vHitMCRef.clear();
  vHitStore.clear();
  algo->vStsHits.clear();
  algo->vStsStrips.clear();
  algo->vStsStripsB.clear();
  algo->vStsZPos.clear();
  algo->vSFlag.clear();
  algo->vSFlagB.clear();
  if (fVerbose >= 10) cout << "ReadEvent: clear is done." << endl;
  
  vector<TmpHit> tmpHits;
  vector<TmpStrip> tmpStrips;
  vector<TmpStrip> tmpStripsB;

    // -- produce Sts hits from space points --

  for(int i = 0; i < NStation; i++){
    algo->StsHitsStartIndex[i] = static_cast<THitI>(-1);
    algo->StsHitsStopIndex[i]  = 0;
  }
  
  //for a new definition of the reconstructable track (4 consequtive MC Points):
  vector<bool> isUsedMvdPoint; //marks, whether Mvd MC Point already in the vMCPoints
  vector<bool> isUsedStsPoint; //marks, whether Mvd MC Point already in the vMCPoints

    // get MVD hits
  int nMvdHits=0;
  if( listMvdHits ){
    Int_t nEnt  = listMvdHits->GetEntries();
    Int_t nMC = (listMvdPts) ? listMvdPts->GetEntries() : 0;

    if(listMvdPts)
    {
      isUsedMvdPoint.resize(nMC);
      for(int iMc=0; iMc<nMC; iMc++) isUsedMvdPoint[iMc]=0;
    }

    for (int j=0; j <nEnt; j++ ){
      TmpHit th;
      {
        CbmMvdHit *mh = L1_DYNAMIC_CAST<CbmMvdHit*>( listMvdHits->At(j) );
        th.ExtIndex = -(1+j);
        th.iStation = mh->GetStationNr() - 1;
        th.iSector  = 0;
        th.isStrip  = 0;
        th.iStripF = j;
        th.iStripB = -1;
        if( th.iStripF<0 ) continue;
        if( th.iStripF>=0 && th.iStripB>=0 ) th.isStrip  = 1;
        if( th.iStripB <0 ) th.iStripB = th.iStripF;
  
        TVector3 pos, err;
        mh->Position(pos);
        mh->PositionError(err);

        th.x = pos.X();
        th.y = pos.Y();
  
        L1Station &st = algo->vStations[th.iStation];
        th.u_front = th.x*st.frontInfo.cos_phi[0] + th.y*st.frontInfo.sin_phi[0];
        th.u_back  = th.x* st.backInfo.cos_phi[0] + th.y*st.backInfo.sin_phi[0];
      }
      th.iMC=-1;
      int iMC = -1;
//   int iMCTr = -1;
//   if( listMvdHitMatches ){
//     CbmMvdHitMatch *match = (CbmMvdHitMatch*) listMvdHitMatches->At(j);
//     if( match){
//       iMC = match->GetPointId();
//       iMCTr = match->GetTrackId();
//     }
//   }
      if( listMvdHitMatches ){
        CbmMvdHitMatch *match = L1_DYNAMIC_CAST<CbmMvdHitMatch*>( listMvdHitMatches->At(j) );
        if( match){
          iMC = match->GetPointId();
        }
      }
      if( listMvdPts && iMC>=0 ){ // TODO1: don't need this with FairLinks
        CbmL1MCPoint MC;
        if( ! ReadMCPoint( &MC, iMC, 1 ) ){
          MC.iStation = th.iStation;
          isUsedMvdPoint[iMC] = 1;

//       MC.ID = iMCTr; // because atch->GetPointId() == 0 !!! and ReadMCPoint don't work
//       MC.z = th.iStation; // for sort in right order
      
          vMCPoints.push_back( MC );
          th.iMC = vMCPoints.size()-1;
        }
      } // if listStsPts
  //if(  h.MC_Point >=0 ) // DEBUG !!!!
      {
        tmpHits.push_back(th);
        nMvdHits++;
      }
    } // for j
  } // if listMvdHits
  if (fVerbose >= 10) cout << "ReadEvent: mvd hits are gotten." << endl;

    // get STS hits
  int nStsHits=0;
  if( listStsHits ){
    Int_t nEnt = listStsHits->GetEntries();
    Int_t nMC = (listStsPts) ? listStsPts->GetEntries() : 0;

    if(listStsPts)
    {
      isUsedStsPoint.resize(nMC);
      for(int iMc=0; iMc<nMC; iMc++) isUsedStsPoint[iMc]=0;
    }

    int negF=0;
    for (int j=0; j < nEnt; j++ ){
      CbmStsHit *sh = L1_DYNAMIC_CAST<CbmStsHit*>( listStsHits->At(j) );
      if (sh->GetUniqueID()) continue; // AZ - skip used hits
      TmpHit th;
      {
        CbmStsHit *mh = L1_DYNAMIC_CAST<CbmStsHit*>( listStsHits->At(j) );
        th.ExtIndex = j;
        th.iStation = NMvdStations + mh->GetStationNr() - 1;
        th.iSector  = mh->GetSectorNr();
        th.isStrip  = 0;
        th.iStripF = mh->GetDigi(0);
        th.iStripB = mh->GetDigi(1);
        if( th.iStripF<0 ){ negF++; continue;}
        if( th.iStripF>=0 && th.iStripB>=0 ) th.isStrip  = 1;
        if( th.iStripB <0 ) th.iStripB = th.iStripF;

        th.iStripF += nMvdHits;
        th.iStripB += nMvdHits;

        TVector3 pos, err;
        mh->Position(pos);
        mh->PositionError(err);

        th.x = pos.X();
        th.y = pos.Y();

        L1Station &st = algo->vStations[th.iStation];
        th.u_front = th.x*st.frontInfo.cos_phi[0] + th.y*st.frontInfo.sin_phi[0];
        th.u_back  = th.x* st.backInfo.cos_phi[0] + th.y* st.backInfo.sin_phi[0];

        //th.x = th.y = 0.0; //AZ
	//cout << th.iStation << " " << st.frontInfo.cos_phi[0] << " " << st.frontInfo.sin_phi[0] << endl; //AZ
	//cout << th.x << " " << th.y << " " << th.u_front << " " << th.u_back << endl;
	//double s2 = st.frontInfo.sigma2[0]; //AZ
	//cout << st.frontInfo.sigma2 << " " << s2 << " " << sqrt(s2) << " " << 0.04/sqrt(12.) << endl;
	//th.du_front = sqrt(s2); //AZ - pitch error - for test
	th.du_front = err.X(); //AZ - cluster error
	//s2 = st.backInfo.sigma2[0]; //AZ
	//th.du_back = sqrt(s2); //AZ - pitch error - for test
	th.du_back = err.Y(); //AZ - cluster error
      }
      th.iMC=-1;

      int iMC = sh->GetRefIndex(); // TODO1: don't need this with FairLinks
      if( listStsPts && iMC>=0 && iMC<nMC){
        CbmL1MCPoint MC;
        if( ! ReadMCPoint( &MC, iMC, 0 ) ){
          MC.iStation = th.iStation;
          vMCPoints.push_back( MC );
          isUsedStsPoint[iMC] = 1;
          th.iMC = vMCPoints.size()-1;
        }
      } // if listStsPts

      
  //if(  th.iMC >=0 ) // DEBUG !!!!
      {
        tmpHits.push_back(th);
        nStsHits++;
      }
    } // for j
  } // if listStsHits
  if (fVerbose >= 10) cout << "ReadEvent: sts hits are gotten." << endl;

  //add MC points, which has not been added yet
  if(listMvdHits && listMvdPts)
  {
    int nMC = listMvdPts->GetEntriesFast();
    for(int iMC=0; iMC<nMC; iMC++){
      if(!(isUsedMvdPoint[iMC])) {
        CbmL1MCPoint MC;
        if( ! ReadMCPoint( &MC, iMC, 1 ) ){
          MC.iStation = (L1_DYNAMIC_CAST<CbmMvdPoint*>(listMvdPts->At(iMC)))->GetStationNr() - 1;
          isUsedMvdPoint[iMC] = 1;
          vMCPoints.push_back( MC );
        }
      }
    }
  }
  if(listStsHits && listStsPts)
  {
    int nMC = listStsPts->GetEntriesFast();
    for(int iMC=0; iMC<nMC; iMC++){
      if(!(isUsedStsPoint[iMC])) {
        CbmL1MCPoint MC;
        if( ! ReadMCPoint( &MC, iMC, 0 ) ){
          MC.iStation = -1;
          L1Station *sta = algo->vStations + NMvdStations;
          for(int iSt=0; iSt < NStsStations; iSt++)
            MC.iStation = ( MC.z > sta[iSt].z[0] - 2.5 ) ? (NMvdStations+iSt) : MC.iStation;
          isUsedStsPoint[iMC] = 1;
          vMCPoints.push_back( MC );
        }
      }
    }
  }
    // sort hits
  int nHits = nMvdHits + nStsHits;
  sort( tmpHits.begin(), tmpHits.end(), TmpHit::Compare );

    // -- create strips --
  int NStrips = 0, NStripsB = 0;
  for ( int ih = 0; ih<nHits; ih++ ){
    TmpHit &th = tmpHits[ih];
    
      // try to find the respective front and back strip from the already created
    th.indStripF = -1;
    th.indStripB = -1;
    for( int is = 0; is<NStrips; is++ ){
      TmpStrip &s = tmpStrips[is];
      if( s.iStation!=th.iStation ||  s.iSector!=th.iSector ) continue;
      if( s.iStrip!=th.iStripF ) continue;
      if( fabs(s.u - th.u_front)>1.e-4 ) continue;
      th.indStripF = is;
    }
    for( int is = 0; is<NStripsB; is++ ){
      TmpStrip &s = tmpStripsB[is];
      if( s.iStation!=th.iStation ||  s.iSector!=th.iSector ) continue;
      if( s.iStrip!=th.iStripB ) continue;
      if( fabs(s.u - th.u_back)>1.e-4 ) continue;
      th.indStripB = is;
    }
      // create new strips
    if( th.indStripF<0 ){
      TmpStrip tmp;
      tmp.iStation = th.iStation;
      tmp.iSector = th.iSector;
      tmp.iStrip = th.iStripF;
      tmp.u = th.u_front;
      tmp.du = th.du_front; //AZ
      tmp.isStrip = th.isStrip;
      tmpStrips.push_back(tmp);
      th.indStripF = NStrips++;
    }
    if( th.indStripB<0 ){
      TmpStrip tmp;
      tmp.iStation = th.iStation;
      tmp.iSector = th.iSector;
      tmp.iStrip = th.iStripB;
      tmp.isStrip = th.isStrip;
      tmp.u = th.u_back;
      tmp.du = th.du_back; //AZ
      tmpStripsB.push_back(tmp);
      th.indStripB = NStripsB++;
    }
  } // ih
  
      // -- save strips --
//   for( int i=0; i<NStrips; i++ ) tmpStrips[i].effIndex = -1;
//   for( int i=0; i<NStripsB; i++ ) tmpStripsB[i].effIndex = -1;

//     // make unefficiency
//   if(1){ // space point unefficiency
//     for( int i=0; i<nHits; i++ ){
//       TmpHit &th = tmpHits[i];
//       if( th.indStripF <0 || th.indStripF >= NStrips ) continue;
//       if( th.indStripB <0 || th.indStripB >= NStripsB ) continue;
//       if( th.iMC<0 ) continue;
//       if( gRandom->Uniform(1)>fDetectorEfficiency ){
//         tmpStrips [th.indStripF].effIndex = -2;
//         tmpStripsB[th.indStripB].effIndex = -2;
//       }
//     }
//   } else { // strip unefficiency
// 
//     for( int i=0; i<NStrips; i++ ){
//       if( gRandom->Uniform(1)>fDetectorEfficiency ) tmpStrips[i].effIndex = -2;
//     }
//     
//     for( int i=0; i<NStripsB; i++ ){
//       TmpStrip &ts = tmpStripsB[i];
//       if( ts.isStrip && gRandom->Uniform(1)>fDetectorEfficiency ) ts.effIndex = -2;
//     }
//   }

    // take into account unefficiency and save strips in L1Algo
  Int_t NEffStrips = 0, NEffStripsB = 0;
  for( int i=0; i<NStrips; i++ ){
    TmpStrip &ts = tmpStrips[i];
      //     if( ts.effIndex == -1 ){
    ts.effIndex = NEffStrips++;
    char flag = ts.iStation*4;
    //algo->vStsStrips.push_back(ts.u);
    algo->vStsStrips.push_back(L1Strip(ts.u,ts.du)); //AZ
    algo->vSFlag.push_back(flag);
      //     }
  }
  for( int i=0; i<NStripsB; i++ ){
    TmpStrip &ts = tmpStripsB[i];
      //     if( ts.effIndex == -1 ){
    ts.effIndex = NEffStripsB++;
    char flag = ts.iStation*4;
    //algo->vStsStripsB.push_back(ts.u);
    algo->vStsStripsB.push_back(L1Strip(ts.u,ts.du)); //AZ
    algo->vSFlagB.push_back(flag);
      //     }
  }

  if (fVerbose >= 10) cout << "ReadEvent: strips are read." << endl;
  
    // -- save hits --
//   vector<int> vUnEffHitMCRef;
//   vector<CbmL1HitStore> vUnEffHitStore;
  int nEffHits = 0;
  vector<float> vStsZPos_temp; // temp array for unsorted z positions of detectors segments
  for( int i=0; i<nHits; i++ ){
    TmpHit &th = tmpHits[i];
 
    CbmL1HitStore s;
    s.ExtIndex = th.ExtIndex;
    s.iStation = th.iStation;
    s.x        = th.x;
    s.y        = th.y;

    if( th.indStripF <0 || th.indStripF >= NStrips ) continue;
    if( th.indStripB <0 || th.indStripB >= NStripsB ) continue;

    TmpStrip &tsF = tmpStrips [th.indStripF];
    TmpStrip &tsB = tmpStripsB[th.indStripB];
//     if( tsF.effIndex <0 || tsB.effIndex <0 ){
//       vUnEffHitStore.push_back(s);
//       vUnEffHitMCRef.push_back(th.iMC);
//       continue;
//     }

    L1StsHit h;
    h.f = tsF.effIndex;
    h.b = tsB.effIndex;

      // find and save z positions
    float z_tmp;
    int ist = th.iStation;
    if (ist < NMvdStations){
      CbmKFTube &t = CbmKF::Instance()->vMvdMaterial[ist];
      z_tmp = t.z;
    }
    else {
      CbmStsHit *mh_m = L1_DYNAMIC_CAST<CbmStsHit*>( listStsHits->At(s.ExtIndex));
      z_tmp = mh_m->GetZ();
    }
//     h.z = z_tmp;
    
    unsigned int k;
    for (k = 0; k < vStsZPos_temp.size(); k++){
      if (vStsZPos_temp[k] == z_tmp){
        h.iz = k;
        break;
      }
    }
    if (k == vStsZPos_temp.size()){
      h.iz = vStsZPos_temp.size();
      vStsZPos_temp.push_back(z_tmp);
    }
    
      // save hit
    vStsHits.push_back( CbmL1StsHit(algo->vStsHits.size(), th.ExtIndex ) );
    algo->vStsHits.push_back(h);
    
    int sta = th.iStation;
    if (algo->StsHitsStartIndex[sta] == static_cast<THitI>(-1)) algo->StsHitsStartIndex[sta] = nEffHits;
    nEffHits++;
    algo->StsHitsStopIndex[sta] = nEffHits;
    
    vHitStore.push_back(s);
    vHitMCRef.push_back(th.iMC);
  }
  for(int i = 0; i < NStation; i++){
    if (algo->StsHitsStartIndex[i] == static_cast<THitI>(-1)) algo->StsHitsStartIndex[i] = algo->StsHitsStopIndex[i];
  }

//   if(0){ // check index z-pos befor sort
//     for (int k = 0; k < algo->vStsHits.size(); k++){
//       cout << algo->vStsHits[k].z << "  " << vStsZPos_temp[algo->vStsHits[k].iz] << endl;
//     }
//     int kk; std::cin >> kk;
//   }

  if (fVerbose >= 10) cout << "ReadEvent: mvd and sts are saved." << endl;
  
    // sort z-pos
  if (vStsZPos_temp.size() != 0){
    vector<float> vStsZPos_temp2;
    vStsZPos_temp2.clear();
    vStsZPos_temp2.push_back(vStsZPos_temp[0]);
    vector<int> newToOldIndex;
    newToOldIndex.clear();
    newToOldIndex.push_back(0);

    for (unsigned int k = 1; k < vStsZPos_temp.size(); k++){
      vector<float>::iterator itpos = vStsZPos_temp2.begin()+1;
      vector<int>::iterator iti = newToOldIndex.begin()+1;
      for (; itpos < vStsZPos_temp2.end(); itpos++, iti++){
        if (vStsZPos_temp[k] < *itpos){
          vStsZPos_temp2.insert(itpos,vStsZPos_temp[k]);
          newToOldIndex.insert(iti,k);
          break;
        }
      }
      if (itpos == vStsZPos_temp2.end()){
        vStsZPos_temp2.push_back(vStsZPos_temp[k]);
        newToOldIndex.push_back(k);
      }
    } // k


    if (fVerbose >= 10) cout << "ReadEvent: z-pos are sorted." << endl;
    
      // save z-pos
    for (unsigned int k = 0; k < vStsZPos_temp2.size(); k++)
      algo->vStsZPos.push_back(vStsZPos_temp2[k]);
      // correct index of z-pos in hits array
    int size_nto_tmp = newToOldIndex.size();
    vector<int> oldToNewIndex;
    oldToNewIndex.clear();
    oldToNewIndex.resize(size_nto_tmp);
    for (int k = 0; k < size_nto_tmp; k++)
      oldToNewIndex[newToOldIndex[k]] = k;
    
    int size_hs_tmp = vHitStore.size();
    for (int k = 0; k < size_hs_tmp; k++)
      algo->vStsHits[k].iz = oldToNewIndex[algo->vStsHits[k].iz];
    
//    // check index z-pos
//     for (int k = 0; k < algo->vStsHits.size(); k++){
//       fvec z1 = algo->vStsZPos[algo->vStsHits[k].iz];
//       fvec z2 = algo->vStations[algo->vSFlag[algo->vStsHits[k].f]/4].z;
//       if (fabs(z1[0] - z2[0]) > 1.0){
//         cout << z1[0] << "  " << z2[0] << endl;
//         int kk;
//         std::cin >> k;
//       }
//     }
  }

  if (fVerbose >= 10) cout << "ReadEvent: z-pos are saved." << endl;

  HitMatch();
  
  if (fVerbose >= 10) cout << "HitMatch is done." << endl;
  
    // -- sort MC points --
  vector<TmpMCPoint> vtmpMCPoints;
  for ( unsigned int iMCPoint = 0; iMCPoint < vMCPoints.size(); iMCPoint++) {
    CbmL1MCPoint &MC = vMCPoints[iMCPoint];
    TmpMCPoint tmp;
    tmp.ID = MC.ID;
    tmp.z = MC.z;
//    tmp.StsHit = i;
    tmp.MCPoint = iMCPoint;
    tmp.eff = 1;
    vtmpMCPoints.push_back(tmp);
  }

//   for ( unsigned int i=0; i<vUnEffHitStore.size(); i++ ){
//     int iMC = vUnEffHitMCRef[i];
//     if( iMC < 0 ) continue;
//     CbmL1MCPoint &MC = vMCPoints[iMC];
//     TmpMCPoint tmp;
//     tmp.ID = MC.ID;
//     tmp.z = MC.z;
//     tmp.StsHit = i;
//     tmp.MCPoint = &MC;
//     tmp.eff = 0;
//     vtmpMCPoints.push_back(tmp);
//   }
  
  sort( vtmpMCPoints.begin(), vtmpMCPoints.end(), TmpMCPoint::compareIDz );
  if (fVerbose >= 10) cout << "MCPoints are sorted." << endl;

    // -- fill vMCTracks array --
  PrimVtx.MC_ID = 999;
  {
    CbmL1Vtx Vtxcurr;
    int nvtracks=0,
        nvtrackscurr=0;
    int ID = -10000;
    for ( vector<TmpMCPoint>::iterator i= vtmpMCPoints.begin(); i!=vtmpMCPoints.end(); ++i){
      if( vMCTracks.empty() || i->ID!= ID ){ // new track
        ID = i->ID;
        CbmL1MCTrack T;
        T.ID = ID;
        if( listMCTracks )
        {
          CbmMCTrack *MCTrack = L1_DYNAMIC_CAST<CbmMCTrack*>( listMCTracks->At( T.ID ) );
          int mother_ID = MCTrack->GetMotherId();
          TVector3 vr;
          TLorentzVector vp;
          MCTrack->GetStartVertex(vr);
          MCTrack->Get4Momentum(vp);

          T = CbmL1MCTrack(vMCPoints[i->MCPoint].mass, vMCPoints[i->MCPoint].q, vr, vp, i->ID, mother_ID, vMCPoints[i->MCPoint].pdg);
        }
        vMCTracks.push_back( T );
        
        if ( T.mother_ID ==-1 ){ // vertex track

          if (  PrimVtx.MC_ID == 999 || fabs(T.z-Vtxcurr.MC_z)>1.e-7 ){// new vertex
            if( nvtrackscurr > nvtracks ){
              PrimVtx = Vtxcurr;
              nvtracks = nvtrackscurr;
            }
            Vtxcurr.MC_x  = T.x;
            Vtxcurr.MC_y  = T.y;
            Vtxcurr.MC_z  = T.z;
            Vtxcurr.MC_ID = T.mother_ID;
            nvtrackscurr = 1;
          }
          else nvtrackscurr++;
          
        }
      } // new track
      vMCTracks.back().Points.push_back(i->MCPoint);
//       if( i->eff ) vMCTracks.back().StsHits.push_back(i->StsHit);
    } // for i of tmpMCPoints
    if( nvtrackscurr > nvtracks ) PrimVtx = Vtxcurr;
  } // fill MC tracks
  if (fVerbose >= 10) cout << "MCPoints and MCTracks are saved." << endl;

  
    // calculate the max number of Hits\mcPoints on continuous(consecutive) stations
  for( vector<CbmL1MCTrack>::iterator it = vMCTracks.begin(); it != vMCTracks.end(); ++it){
    it->Init();
  } // for i vMCTracks
  
  if ( fVerbose && PrimVtx.MC_ID == 999 ) cout<<"No primary vertex !!!"<<endl;



  if (fVerbose >= 10) cout << "ReadEvent is done." << endl;
} // void CbmL1::ReadEvent()


bool CbmL1::ReadMCPoint( CbmL1MCPoint *MC, int iPoint, bool MVD )
{
  TVector3 xyzI,PI, xyzO,PO;
  Int_t mcID=-1;
  if( MVD && listMvdPts ){
    CbmMvdPoint *pt = L1_DYNAMIC_CAST<CbmMvdPoint*>( listMvdPts->At(iPoint) );
    if ( !pt ) return 1;
    pt->Position(xyzI);
    pt->Momentum(PI);
    pt->PositionOut(xyzO);
    pt->MomentumOut(PO);
    mcID = pt->GetTrackID();
  }else if( listStsPts ){
    CbmStsPoint *pt = L1_DYNAMIC_CAST<CbmStsPoint*>( listStsPts->At(iPoint) );
    if ( !pt ) return 1;
    pt->Position(xyzI);
    pt->Momentum(PI);
    pt->PositionOut(xyzO);
    pt->MomentumOut(PO);
    mcID = pt->GetTrackID();
  }
  TVector3 xyz = .5*(xyzI + xyzO );
  TVector3 P = .5*(PI + PO );
  MC->x  = xyz.X();
  MC->y  = xyz.Y();
  MC->z  = xyz.Z();
  MC->px = P.X();
  MC->py = P.Y();
  MC->pz = P.Z();
  MC->xIn  = xyzI.X();
  MC->yIn  = xyzI.Y();
  MC->zIn  = xyzI.Z();
  MC->pxIn = PI.X();
  MC->pyIn = PI.Y();
  MC->pzIn = PI.Z();
  MC->xOut  = xyzO.X();
  MC->yOut  = xyzO.Y();
  MC->zOut  = xyzO.Z();
  MC->pxOut = PO.X();
  MC->pyOut = PO.Y();
  MC->pzOut = PO.Z();
  MC->p = sqrt( fabs( MC->px*MC->px + MC->py*MC->py + MC->pz*MC->pz ) );
  MC->ID = mcID;
  MC->pointId = iPoint;
  
  CbmMCTrack *MCTrack = L1_DYNAMIC_CAST<CbmMCTrack*>( listMCTracks->At( MC->ID ) );
  if ( !MCTrack ) return 1;
  MC->pdg  = MCTrack->GetPdgCode();
  MC->mother_ID = MCTrack->GetMotherId();
  if ( abs(MC->pdg) >= 10000 ) return 1;
  MC->q = TDatabasePDG::Instance()->GetParticle(MC->pdg)->Charge()/3.0;
  MC->mass = TDatabasePDG::Instance()->GetParticle(MC->pdg)->Mass();

  return 0;
}


  /// Procedure for match hits and MCPoints.
  /// Read information about correspondence between hits and mcpoints and fill CbmL1MCPoint::hitIds and CbmL1StsHit::mcPointIds arrays
  /// should be called after fill of algo
void CbmL1::HitMatch()
{
  const bool useLinks = 0; // 0 - use HitMatch, one_to_one; 1 - use FairLinks, many_to_many. Set 0 to switch to old definition of efficiency.
  // TODO: fix trunk problem with links. Set useLinks = 1
  
  const int NHits = vStsHits.size();
  for (int iH = 0; iH < NHits; iH++){
    CbmL1StsHit& hit = vStsHits[iH];
    
    const bool isMvd = (hit.extIndex < 0);
    if (useLinks && !isMvd){
      if (listStsClusters){
        CbmStsHit *stsHit = L1_DYNAMIC_CAST<CbmStsHit*>( listStsHits->At(hit.extIndex) );
        const int NLinks = stsHit->GetNLinks();
        if ( NLinks != 2 ) cout << "HitMatch: Error. Hit wasn't matched with 2 clusters." << endl;
          // see at 1-st cluster
        vector<int> stsPointIds; // save here all mc-points matched with first cluster
        int iL = 0;
        FairLink link = stsHit->GetLink(iL);
        CbmStsCluster *stsCluster = L1_DYNAMIC_CAST<CbmStsCluster*>( listStsClusters->At( link.GetIndex() ) );
        int NLinks2 = stsCluster->GetNLinks();
        for ( int iL2 = 0; iL2 < NLinks2; iL2++){
          FairLink link2 = stsCluster->GetLink(iL2);
          CbmStsDigi *stsDigi = L1_DYNAMIC_CAST<CbmStsDigi*>( listStsDigi->At( link2.GetIndex() ) );
          const int NLinks3 = stsDigi->GetNLinks();
          for ( int iL3 = 0; iL3 < NLinks3; iL3++){
            FairLink link3 = stsDigi->GetLink(iL3);
            int stsPointId = link3.GetIndex();
            stsPointIds.push_back( stsPointId );
          } // for mcPoint
        } // for digi
          // see at 2-nd cluster
        iL = 1;
        link = stsHit->GetLink(iL);
        stsCluster = L1_DYNAMIC_CAST<CbmStsCluster*>( listStsClusters->At( link.GetIndex() ) );
        NLinks2 = stsCluster->GetNLinks();
        for ( int iL2 = 0; iL2 < NLinks2; iL2++){
          FairLink link2 = stsCluster->GetLink(iL2);
          CbmStsDigi *stsDigi = L1_DYNAMIC_CAST<CbmStsDigi*>( listStsDigi->At( link2.GetIndex() ) );
          const int NLinks3 = stsDigi->GetNLinks();
          for ( int iL3 = 0; iL3 < NLinks3; iL3++){
            FairLink link3 = stsDigi->GetLink(iL3);
            int stsPointId = link3.GetIndex();
            
            if ( !find(&(stsPointIds[0]), &(stsPointIds[stsPointIds.size()]), stsPointId) ) continue; // check if first cluster matched with same mc-point

              CbmStsPoint *stsPoint = L1_DYNAMIC_CAST<CbmStsPoint*>( listStsPts->At( stsPointId ) );
              
                // find mcPoint in array
              int mcTrackId = stsPoint->GetTrackID();
              TVector3 xyzIn,xyzOut;
              stsPoint->PositionIn(xyzIn);
              stsPoint->PositionOut(xyzOut);
              TVector3 xyz = .5*(xyzIn + xyzOut );
              double z =  xyz.z();

              const int NPoints = vMCPoints.size();
              int iP;
              for (iP = 0; iP < NPoints; iP++){
                if ( (vMCPoints[iP].ID == mcTrackId) && (vMCPoints[iP].z == z) ) break;
              };

              if (iP == NPoints){ // No mc-point was found in vMCPoints array.
                CbmL1MCPoint MC;
                if ( !ReadMCPoint( &MC, stsPointId, 0 ) ) {
                  MC.iStation = vHitStore[hit.hitId].iStation;
                    //algo->GetFStation(algo->vSFlag[algo->vStsHits[hit.hitId].f]);
                  vMCPoints.push_back(MC);
                }
                else {
                  continue;
                }

              }
                // check if the hit already matched with the mc-point and save it if this is not the case
              if ( !find(&(hit.mcPointIds[0]), &(hit.mcPointIds[hit.mcPointIds.size()]), iP) ){
                hit.mcPointIds.push_back( iP );
                vMCPoints[iP].hitIds.push_back(iH);
              }

          } // for mcPoint
        } // for digi

      } // if clusters
      else{
        // CbmStsHit *stsHit = L1_DYNAMIC_CAST<CbmStsHit*>( listStsHits->At(hit.extIndex) );
        // int iP = stsHit->GetRefIndex();
        int iP = vHitMCRef[iH];
        if (iP >= 0){
          hit.mcPointIds.push_back( iP );
          vMCPoints[iP].hitIds.push_back(iH);
        }
      } // if no clusters
    } // if useLinks
    else{ // if no use Links or this is mvd hit
      // int iP = -1; // TODO2
      // if (isMvd) {
      //   // CbmMvdHitMatch *match = L1_DYNAMIC_CAST<CbmMvdHitMatch*>( listMvdHitMatches->At( -hit.extIndex - 1 ) );
      //   // if( match){
      //   //   iP = match->GetPointId();
      //   // }
      //   iP = vHitMCRef[iH];
      // }
      // else {
      //   CbmStsHit *stsHit = L1_DYNAMIC_CAST<CbmStsHit*>( listStsHits->At( hit.extIndex ) );
      //   iP = stsHit->GetRefIndex();
      // }

      int iP = vHitMCRef[iH];
      if (iP >= 0){
        hit.mcPointIds.push_back( iP );
        vMCPoints[iP].hitIds.push_back(iH);
      }
    }
    if (hit.mcPointIds.size() > 1){ // CHECKME  never works!
      cout << "Hit number " << iH << " has been matched with " << hit.mcPointIds.size() << " mcPoints." << endl;
      cout << "Hit number " << iH << " " <<  hit.mcPointIds[0] << " " << vHitMCRef[iH] << endl;
    }
  } // for hits
}
