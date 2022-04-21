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
 *  L1 Fit performance 
 *
 *====================================================================
 */
#include "CbmL1.h"

#include "CbmL1Constants.h"
#include "L1Algo/L1Algo.h"
#include "L1Algo/L1StsHit.h"
#include "L1Algo/L1Extrapolation.h" // for vertex pulls
#include "L1Algo/L1AddMaterial.h" // for vertex pulls
#include "FairTrackParam.h" // for vertex pulls
#include "CbmKFTrack.h" // for vertex pulls

#include "CbmKF.h"
#include "CbmKFMath.h"
#include "CbmMvdHit.h"
#include "CbmMvdHitMatch.h"
#include "CbmStsDigi.h"
#include "CbmStsSensor.h" // for field FieldCheck.
#include "CbmStsSector.h" // for field FieldCheck.
#include "CbmStsStation.h" // for field FieldCheck.
#include "CbmStsPoint.h"
#include "CbmStsHit.h"
#include "CbmMvdPoint.h"
#include "CbmMCTrack.h"

#include "CbmL1Counters.h"
#include "CbmL1PartEfficiencies.h"
#include "CbmL1ParticlesFinder.h"

#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TFile.h"

#include <iostream>
#include <vector>
#include <list>
#include <map>

using std::cout;
using std::endl;
using std::ios;
using std::vector;
using std::pair;
using std::map;

void CbmL1::TrackMatch(){
  map <int, CbmL1MCTrack*> pMCTrackMap; pMCTrackMap.clear();

    // fill pMCTrackMap
  for( vector<CbmL1MCTrack>::iterator i = vMCTracks.begin(); i != vMCTracks.end(); ++i){
    CbmL1MCTrack &MC = *i;
    
    if (pMCTrackMap.find(MC.ID) == pMCTrackMap.end()){
      pMCTrackMap.insert(pair<int, CbmL1MCTrack*>(MC.ID, &MC));
    }
    else{
      cout << "*** L1: Track ID " << MC.ID << " encountered twice! ***" << endl;
    }
  }
      // -- prepare information about reconstructed tracks
  const int nRTracks = vRTracks.size();
  for (int iR = 0; iR < nRTracks; iR++){
    CbmL1Track* prtra = &(vRTracks[iR]);
    
    int hitsum  = prtra->StsHits.size(); // number of hits in track

      // count how many hits from each mcTrack belong to current recoTrack
    map<int, int > &hitmap = prtra->hitMap; // how many hits from each mcTrack belong to current recoTrack
    for (vector<int>::iterator ih  = (prtra->StsHits).begin(); ih != (prtra->StsHits).end(); ++ih){
      
      const int nMCPoints = vStsHits[*ih].mcPointIds.size();
      for (int iP = 0; iP < nMCPoints; iP++){
        int iMC = vStsHits[*ih].mcPointIds[iP];
        int ID = -1;
        if (iMC >= 0) ID = vMCPoints[iMC].ID;
        if(hitmap.find(ID) == hitmap.end())
          hitmap[ID] = 1;
        else{
          hitmap[ID] += 1;
        }
      } // for iPoint
    } // for iHit

      // RTrack <-> MCTrack identification
    double max_percent = 0.0;  // [%]. maximum persent of hits, which belong to one mcTrack.
    for( map<int, int >::iterator posIt = hitmap.begin(); posIt != hitmap.end(); posIt++ ){ // loop over all touched MCTracks
      
      if (posIt->first < 0) continue; // not a MC track - based on fake hits
      
        // count max-purity
      if (double(posIt->second) > max_percent*double(hitsum))
        max_percent = double(posIt->second)/double(hitsum);
      
        // set relation to the mcTrack
      if ( double(posIt->second) > CbmL1Constants::MinPurity * double(hitsum) ){ // found correspondent MCTrack
        if (pMCTrackMap.find(posIt->first) == pMCTrackMap.end()) continue;
        CbmL1MCTrack* pmtra = pMCTrackMap[posIt->first];

        pmtra->AddRecoTrack(prtra);
        prtra->AddMCTrack(pmtra);
      } 
      else{
        if (pMCTrackMap.find(posIt->first) == pMCTrackMap.end()) continue;
        CbmL1MCTrack* pmtra = pMCTrackMap[posIt->first];

        pmtra->AddTouchTrack(prtra);
      }
    } // for hitmap
    prtra->SetMaxPurity(max_percent);
  } // for reco tracks
} // 



struct TL1PerfEfficiencies: public TL1Efficiencies
{
  TL1PerfEfficiencies():TL1Efficiencies(),
ratio_killed(),
ratio_clone(),
ratio_length(),
ratio_fakes(),                 
killed(),
clone(),
reco_length(),
reco_fakes(),
mc_length(),
mc_length_hits()
  {
          // add total efficiency
    AddCounter("long_fast_prim" ,"LongRPrim efficiency");
    AddCounter("fast_prim"      ,"RefPrim   efficiency");
    AddCounter("fast_sec"       ,"RefSec    efficiency");
    AddCounter("fast"           ,"Refset    efficiency");
    AddCounter("total"          ,"Allset    efficiency");
    AddCounter("slow_prim"      ,"ExtraPrim efficiency");
    AddCounter("slow_sec"       ,"ExtraSec  efficiency");
    AddCounter("slow"           ,"Extra     efficiency");
    AddCounter("d0"             ,"D0        efficiency");
    AddCounter("short"          ,"Short123s efficiency");
    AddCounter("shortPion"      ,"Short123s pion   eff");
    AddCounter("shortProton"    ,"Short123s proton eff");
    AddCounter("shortKaon"      ,"Short123s kaon   eff");
    AddCounter("shortE"         ,"Short123s e      eff");
    AddCounter("shortRest"      ,"Short123s rest   eff");
    
    AddCounter("fast_sec_e"       ,"RefSec  E efficiency");
    AddCounter("fast_e"           ,"Refset  E efficiency");
    AddCounter("total_e"          ,"Allset  E efficiency");
    AddCounter("slow_sec_e"       ,"ExtraSecE efficiency");
    AddCounter("slow_e"           ,"Extra   E efficiency");
  }
  
  virtual ~TL1PerfEfficiencies(){};

  virtual void AddCounter(TString shortname, TString name){
    TL1Efficiencies::AddCounter(shortname, name);
    ratio_killed.AddCounter();
    ratio_clone.AddCounter();
    ratio_length.AddCounter();
    ratio_fakes.AddCounter();
    killed.AddCounter();
    clone.AddCounter();
    reco_length.AddCounter();
    reco_fakes.AddCounter();
    mc_length.AddCounter();
    mc_length_hits.AddCounter();
  };
  
  TL1PerfEfficiencies& operator+=(TL1PerfEfficiencies& a){
    TL1Efficiencies::operator+=(a);
    killed += a.killed; clone += a.clone;
    reco_length += a.reco_length;
    reco_fakes  += a.reco_fakes;
    mc_length   += a.mc_length;
    mc_length_hits += a.mc_length_hits;
    return *this;
  };
  
  void CalcEff(){
    TL1Efficiencies::CalcEff();
    ratio_killed = killed/mc;
    ratio_clone  = clone/mc;
    TL1TracksCatCounters<int> allReco = reco + clone;
    ratio_length = reco_length/allReco;
    ratio_fakes  = reco_fakes/allReco;
  };
  
  void Inc(bool isReco, bool isKilled, double _ratio_length, double _ratio_fakes, int _nclones, int _mc_length, int _mc_length_hits, TString name){
    TL1Efficiencies::Inc(isReco, name);

    const int index = indices[name];

    if (isKilled) killed.counters[index]++;
    reco_length.counters[index] += _ratio_length;
    reco_fakes.counters[index] += _ratio_fakes;
    clone.counters[index] += _nclones;
    mc_length.counters[index] += _mc_length;
    mc_length_hits.counters[index] += _mc_length_hits;
  };

  void PrintEff(){
    L1_assert(nEvents != 0);
        
    cout.setf(ios::fixed);
    cout.setf(ios::showpoint);
    cout.precision(3);
    cout.setf(ios::right);
    cout << "Track category         : " << " Eff  "        <<" / "<< "Killed" <<" / "<< "Length" <<" / "<< "Fakes " <<" / "<< "Clones" <<" / "<< "All Reco" <<" | "<< "  All MC "  <<" / "<< "MCl(hits)" <<" / "<< "MCl(MCps)" << endl;
    
    int NCounters = mc.NCounters;
    for (int iC = 0; iC < NCounters; iC++){
      if (( names[iC] != "D0        efficiency") || (mc.counters[iC] != 0))
        cout << names[iC]  << "   : "
             << ratio_reco.counters[iC]              
             << "  / " << ratio_killed.counters[iC]  // tracks with aren't reco because other tracks takes their hit(-s)
             << "  / " << ratio_length.counters[iC]  // nRecoMCHits/nMCHits
             << "  / " << ratio_fakes.counters[iC]   // nFakeHits/nRecoAllHits
             << "  / " << ratio_clone.counters[iC]   // nCloneTracks/nMCTracks
             << "  / " << setw(8) << reco.counters[iC]/double(nEvents)
             << " | " << setw(8) << mc.counters[iC]/double(nEvents)
             << "  / " << mc_length_hits.counters[iC]/double(mc.counters[iC])
             << "  / " << mc_length.counters[iC]/double(mc.counters[iC])
             << endl;
    }
    cout << "Ghost     probability  : " << ratio_ghosts <<"  | "<< ghosts << endl;
  };

  TL1TracksCatCounters<double> ratio_killed;
  TL1TracksCatCounters<double> ratio_clone;
  TL1TracksCatCounters<double> ratio_length;
  TL1TracksCatCounters<double> ratio_fakes;

  TL1TracksCatCounters<int> killed;
  TL1TracksCatCounters<int> clone;
  TL1TracksCatCounters<double> reco_length;
  TL1TracksCatCounters<double> reco_fakes;
  TL1TracksCatCounters<int> mc_length;
  TL1TracksCatCounters<int> mc_length_hits;
};


void CbmL1::EfficienciesPerformance()
{
  static TL1PerfEfficiencies L1_NTRA; // average efficiencies

  static int L1_NEVENTS               = 0;
  static double L1_CATIME             = 0.0;

  TL1PerfEfficiencies ntra; // efficiencies for current event

  for (vector<CbmL1Track>::iterator rtraIt = vRTracks.begin(); rtraIt != vRTracks.end(); ++rtraIt){
    ntra.ghosts += rtraIt->IsGhost();
//     if(rtraIt->IsGhost()){ // Debug.
//       cout << " " << rtraIt->GetNOfHits() << " " << 1./rtraIt->T[5] << " " << rtraIt->GetMaxPurity() << " | ";
//       for( map<int, int>::iterator posIt = rtraIt->hitMap.begin(); posIt != rtraIt->hitMap.end(); posIt++ ){
//         cout << " (" << posIt->second << " " << posIt->first << ") ";
//       }
//       cout << endl;
//     }
  }

  int sta_nhits[algo->NStations], sta_nfakes[algo->NStations];

  for (int i = 0; i < algo->NStations; i++){
    sta_nhits[i]  = 0;
    sta_nfakes[i] = 0;
  }

  for ( vector<CbmL1MCTrack>::iterator mtraIt = vMCTracks.begin(); mtraIt != vMCTracks.end(); mtraIt++ ) {
    CbmL1MCTrack &mtra = *(mtraIt);
    
//    if( !( mtra.pdg == -11 && mtra.mother_ID == -1 ) ) continue; // electrons only
    if( ! mtra.IsReconstructable() && ! mtra.IsAdditional() ) continue;

      // -- find used constans --
      // is track reconstructed
    const bool reco = (mtra.IsReconstructed());
      // is track killed. At least one hit of it belong to some recoTrack
    const bool killed = !reco && mtra.IsDisturbed();
      // ration length for current mcTrack
    vector< CbmL1Track* >& rTracks = mtra.GetRecoTracks(); // for length calculations
    double ratio_length = 0;
    double ratio_fakes  = 0;
    double mc_length_hits = mtra.NStations();
    int mc_length = mtra.NMCStations();
    if (reco){
      for (unsigned int irt = 0; irt < rTracks.size(); irt++) {
        ratio_length += static_cast<double>( rTracks[irt]->GetNOfHits() )*rTracks[irt]->GetMaxPurity() / mc_length_hits;
        ratio_fakes += 1 - rTracks[irt]->GetMaxPurity();
      }
    }
      // number of clones
    int nclones = 0;
    if (reco) nclones = mtra.GetNClones();
      //     if (nclones){ // Debug. Look at clones
      //       for (int irt = 0; irt < rTracks.size(); irt++){
      //         const int ista = vHitStore[rTracks[irt]->StsHits[0]].iStation;
      //         cout << rTracks[irt]->GetNOfHits() << "(" << ista << ") ";
      //       }
      //       cout << mtra.NStations() << endl;
      //     }

    if ( mtra.IsAdditional() ){ // short
      ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "short");
      switch ( mtra.pdg ) {
        case 11:
        case -11:
          ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "shortE");
          break;
        case 211:
        case -211:
          ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "shortPion");
          break;
        case 321:
        case -321:
          ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "shortKaon");
          break;
        case 2212:
        case -2212:
          ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "shortProton");
          break;
        default:
          ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "shortRest");
      }
    }
    else { // separate all efficiecies from short eff
    
      ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "total");

      if (( mtra.IsPrimary() )&&(mtra.z > 0)){ // D0
        ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "d0");
      }

      if ( mtra.p > CbmL1Constants::MinRefMom ){                        // reference tracks
        ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "fast");
      
        if ( mtra.IsPrimary() ){                         // reference primary
          if ( mtra.NStations() == NStation ){ // long reference primary
            ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "long_fast_prim");
          }
          ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "fast_prim");
        }
        else{                                             // reference secondary
          ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "fast_sec");
        }
      }
      else{                                               // extra set of tracks
        ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "slow");
       
        if ( mtra.IsPrimary() ){             // extra primary
          ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "slow_prim");
        }
        else{
          ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "slow_sec");
        }
      } // if extra

      if ( mtra.pdg == 11 || mtra.pdg == -11 ) {
        ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "total_e");

        if ( mtra.p > CbmL1Constants::MinRefMom ){                        // reference tracks
          ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "fast_e");
      
          if ( mtra.IsPrimary() ){                         // reference primary
          }
          else{                                             // reference secondary
            ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "fast_sec_e");
          }
        }
        else{                                               // extra set of tracks
          ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "slow_e");
       
          if ( mtra.IsPrimary() ){             // extra primary
          }
          else{
            ntra.Inc(reco, killed, ratio_length, ratio_fakes, nclones, mc_length, mc_length_hits, "slow_sec_e");
          }
        } // if extra
      }
    }

  } // for mcTracks

  L1_CATIME += algo->CATime;
  L1_NEVENTS++;
  ntra.IncNEvents();
  L1_NTRA += ntra;

  ntra.CalcEff();
  L1_NTRA.CalcEff();

  //   cout.precision(3);
  if( fVerbose ){
    if( fVerbose > 1 ){
      ntra.PrintEff();
      cout << "Number of true and fake hits in stations: " << endl;
      for (int i = 0; i < algo->NStations; i++){
        cout << sta_nhits[i]-sta_nfakes[i] << "+" << sta_nfakes[i] << "   ";
      }
      cout << endl;
    } // fVerbose > 1
    cout << endl;
    cout << "L1 ACCUMULATED STAT    : " << L1_NEVENTS << " EVENTS "               << endl << endl;
    L1_NTRA.PrintEff();
    cout << "MC tracks/event found  : " << int(double(L1_NTRA.reco.counters[L1_NTRA.indices["total"]])/double(L1_NEVENTS)) << endl;
    cout<<endl;
    cout<<"CA Track Finder: " << L1_CATIME/L1_NEVENTS << " s/ev" << endl << endl;
  }
} // void CbmL1::Performance()


void CbmL1::GetMCParticles()
{
  vMCParticles.clear();
    // convert
  for(int i=0; i < listMCTracks->GetEntriesFast(); i++)
  {
    CbmMCTrack &mtra = *(L1_DYNAMIC_CAST<CbmMCTrack*>(listMCTracks->At(i)));
    KFMCParticle part;
    part.SetMCTrackID( i );
    part.SetMotherId ( mtra.GetMotherId() );
    part.SetPDG      ( mtra.GetPdgCode() );
    vMCParticles.push_back( part );
  }

    // find relations
  const unsigned int nParticles = vMCParticles.size();
  for ( unsigned int iP = 0; iP < nParticles; iP++ ) {
    KFMCParticle &part = vMCParticles[iP];
    for(unsigned int iP2 = 0; iP2 < nParticles; iP2++) {
      KFMCParticle &part2 = vMCParticles[iP2];
      
      if(part.GetMotherId() == part2.GetMCTrackID()) {
        part2.AddDaughter(iP);
      }
    }
  }
  //   // correct idices according to the new array
  // for ( unsigned int iP = 0; iP < nParticles; iP++ ) {
  //   KFMCParticle &part = vMCParticles[iP];
  //   for(unsigned int iP2 = 0; iP2 < nParticles; iP2++) {
  //     KFMCParticle &part2 = vMCParticles[iP2];
      
  //     if(part.GetMotherId() == part2.GetMCTrackID()) {
  //       part.SetMotherId (iP2);
  //     }
  //   }
  // }
}

void CbmL1::FindReconstructableMCParticles()
{
  const unsigned int nParticles = vMCParticles.size();

  for ( unsigned int iP = 0; iP < nParticles; iP++ ) {
    KFMCParticle &part = vMCParticles[iP];
    CheckMCParticleIsReconstructable(part);
  }
}

void CbmL1::CheckMCParticleIsReconstructable(KFMCParticle &part)
{

  if ( part.IsReconstructable() ) return;

  const int nParticles = 5;
  int partPDG[nParticles] = {310,3122,3312,-3312,3334};
  vector<int> partDaughterPdg[nParticles];

  partDaughterPdg[0].push_back( 211);
  partDaughterPdg[0].push_back(-211);

  partDaughterPdg[1].push_back(2212);
  partDaughterPdg[1].push_back(-211);

  partDaughterPdg[2].push_back(3122);
  partDaughterPdg[2].push_back(-211);

  partDaughterPdg[3].push_back(3122);
  partDaughterPdg[3].push_back( 211);

  partDaughterPdg[4].push_back(3122);
  partDaughterPdg[4].push_back(-321);

    // tracks
  if ( /*part.NDaughters() == 0*/ part.GetPDG() == -211 ||
                                  part.GetPDG() ==  211 ||
                                  part.GetPDG() == 2212 ||
                                  part.GetPDG() ==  321 ||
                                  part.GetPDG() == -321 ) { // TODO other particles

    switch ( fFindParticlesMode ) {
      case 1:
        part.SetAsReconstructable();
        break;
      case 2:
        unsigned int i;
        for ( i = 0; i < vMCTracks.size(); i++ ) // TODO opt
          if (vMCTracks[i].ID == part.GetMCTrackID()) break;
        if ( i != vMCTracks.size() )
        if ( vMCTracks[i].IsReconstructable() )
          part.SetAsReconstructable();
        break;
      case 3:
//        int i;
        for ( i = 0; i < vMCTracks.size(); i++ )
          if (vMCTracks[i].ID == part.GetMCTrackID()) break;
        if ( i != vMCTracks.size() )
        if ( vMCTracks[i].IsReconstructed() )
          part.SetAsReconstructable();
        break;
      default:
        L1_assert(0);
    };
  }

  //  mother particles
  else
  {
    for(int iPart=0; iPart<nParticles; iPart++)
    {
      if(part.GetPDG() == partPDG[iPart])
      {
        const unsigned int nDaughters = partDaughterPdg[iPart].size();
        if( part.GetDaughterIds().size() != nDaughters ) return;
        int pdg[nDaughters];

        for(unsigned int iD=0; iD<nDaughters; iD++)
          pdg[iD] = vMCParticles[part.GetDaughterIds()[iD]].GetPDG();

        bool isDaughterFound[nDaughters];
        for(unsigned int iDMC=0; iDMC<nDaughters; iDMC++)
          isDaughterFound[iDMC] = 0;

        bool isReco = 1;
        for(unsigned int iDMC=0; iDMC<nDaughters; iDMC++)
          for(unsigned int iD=0; iD<nDaughters; iD++)
            if(pdg[iD] == partDaughterPdg[iPart][iDMC]) isDaughterFound[iDMC] = 1;

        for(unsigned int iDMC=0; iDMC<nDaughters; iDMC++)
          isReco = isReco && isDaughterFound[iDMC];

        if(!isReco) return;
      }
    }

    const vector<int>& dIds = part.GetDaughterIds();
    const unsigned int nD = dIds.size();
    bool reco = 1;
    for ( unsigned int iD = 0; iD < nD && reco; iD++ ) {
      KFMCParticle &dp = vMCParticles[dIds[iD]];
      CheckMCParticleIsReconstructable(dp);
      reco &= dp.IsReconstructable();
    }
    if (reco) part.SetAsReconstructable();
  
  }

}

  /// Procedure for match Reconstructed and MC Particles. Should be called before Performances
void CbmL1::MatchParticles()
{
    // get all reco particles ( temp )
  MCtoRParticleId.clear();
  RtoMCParticleId.clear();
  MCtoRParticleId.resize(vMCParticles.size());
  RtoMCParticleId.resize(vRParticles.size() );
  
    // match tracks ( particles which are direct copy of tracks )
  for( unsigned int iRP = 0; iRP < vRParticles.size(); iRP++ ) {
    CbmKFParticle &rPart = vRParticles[iRP];
    L1_assert( rPart.NDaughters() > 0 );
    L1_ASSERT( static_cast<unsigned int>(rPart.Id()) == iRP, rPart.Id() << " != " << iRP );
    if (rPart.NDaughters() != 1) continue;
    
    const int rTrackId = rPart.DaughterIds()[0];

    if ( vRTracks[rTrackId].IsGhost() ) continue;
    const int mcTrackId = vRTracks[rTrackId].GetMCTrack()->ID;

    for ( unsigned int iMP = 0; iMP < vMCParticles.size(); iMP++ ) {
      KFMCParticle &mPart = vMCParticles[iMP];
      if ( mPart.GetMCTrackID() == mcTrackId ) { // match is found
        if( mPart.GetPDG() == rPart.GetPDG() ) {
          MCtoRParticleId[iMP].ids.push_back(iRP);
          RtoMCParticleId[iRP].ids.push_back(iMP);
        }
        else {
          MCtoRParticleId[iMP].idsMI.push_back(iRP);
          RtoMCParticleId[iRP].idsMI.push_back(iMP);
        }
      }
    }
  }

    // match created mother particles
  for( unsigned int iRP = 0; iRP < vRParticles.size(); iRP++ ) {
    CbmKFParticle &rPart = vRParticles[iRP];
    const unsigned int NRDaughters = rPart.NDaughters();
    if (NRDaughters < 2) continue;
    
    unsigned int iD = 0;
    int mmId = -2; // mother MC id
    {
      const int rdId = rPart.DaughterIds()[iD];
      if ( !RtoMCParticleId[rdId].IsMatched() ) continue;
      const int mdId = RtoMCParticleId[rdId].GetBestMatch();
      mmId = vMCParticles[mdId].GetMotherId();
    }
    iD++;
    for ( ; iD < NRDaughters; iD++ ) {
      const int rdId = rPart.DaughterIds()[iD];
      if ( !RtoMCParticleId[rdId].IsMatched() ) break;
      const int mdId = RtoMCParticleId[rdId].GetBestMatch();
      if( vMCParticles[mdId].GetMotherId() != mmId ) break;
    }
    if ( iD == NRDaughters && mmId != -1 ) { // match is found and it is not primary vertex
      KFMCParticle &mmPart = vMCParticles[mmId];
      if( mmPart.GetPDG()     == rPart.GetPDG()     &&
          mmPart.NDaughters() == rPart.NDaughters() ) {
        MCtoRParticleId[mmId].ids.push_back(iRP);
        RtoMCParticleId[iRP].ids.push_back(mmId);
      }
      else {
        MCtoRParticleId[mmId].idsMI.push_back(iRP);
        RtoMCParticleId[iRP].idsMI.push_back(mmId);
      }
    }
  }
  
}

void CbmL1::PartEffPerformance()
{
  static int NEVENTS               = 0;

  static CbmL1PartEfficiencies PARTEFF; // average efficiencies

  CbmKFPartEfficiencies partEff; // efficiencies for current event

  const int NRP = vRParticles.size();
  for ( int iP = 0; iP < NRP; ++iP ) {
    const CbmKFParticle &part = vRParticles[iP];
    const int pdg = part.GetPDG();
      
    const bool isBG = RtoMCParticleId[iP].idsMI.size() != 0;
    const bool isGhost = !RtoMCParticleId[iP].IsMatched();

    for(int iPart=0; iPart<PARTEFF.nParticles; iPart++)
      if ( pdg == PARTEFF.partPDG[iPart] )
        partEff.IncReco(isGhost, isBG, PARTEFF.partName[iPart].Data());
  }

    
  const int NMP = vMCParticles.size();
  for ( int iP = 0; iP < NMP; ++iP ) {
    const KFMCParticle &part = vMCParticles[iP];
    if ( !part.IsReconstructable() ) continue;
    const int pdg = part.GetPDG();
    const int mId = part.GetMotherId();

    const bool isReco = MCtoRParticleId[iP].ids.size() != 0;
    const int nClones = MCtoRParticleId[iP].ids.size() - 1;

    for(int iPart=0; iPart<PARTEFF.nParticles; iPart++)
    {
      if ( pdg == PARTEFF.partPDG[iPart] ) {
        partEff.Inc(isReco, nClones, PARTEFF.partName[iPart].Data());
        if ( mId == -1 )
          partEff.Inc(isReco, nClones, (PARTEFF.partName[iPart]+"_prim").Data());
        else
          partEff.Inc(isReco, nClones, (PARTEFF.partName[iPart]+"_sec").Data());
      }
    }
  }

  NEVENTS++;

  PARTEFF += partEff;

  partEff.CalcEff();
  PARTEFF.CalcEff();

    //   cout.precision(3);
  if( fVerbose ){
    if(NEVENTS%500 == 0)
    {
      cout << " ---- KF Particle finder --- " << endl;
      // cout << "L1 STAT    : " << fNEvents << " EVENT "               << endl << endl;
      //partEff.PrintEff();
      // cout << endl;
      cout << "ACCUMULATED STAT    : " << NEVENTS << " EVENTS "               << endl << endl;
      PARTEFF.PrintEff();

      cout<<endl;
      // cout<<"CA Track Finder: " << L1_CATIME/L1_fNEvents << " s/ev" << endl << endl;
    }
  }
}

void CbmL1::PartHistoPerformance()
{
  static const int NParticles = 2; //Ks, Lambda

  static const int nFitQA = 16;
  static TH1F *hFitDaughtersQA[NParticles][nFitQA];
  static TH1F *hFitQA[NParticles][nFitQA];

  static const int nHistoPartParam = 5;
  static TH1F *hPartParam[NParticles][nHistoPartParam]; // mass, decay length, c*tau, chi/ndf, prob ...
  static TH1F *hPartParamBG[NParticles][nHistoPartParam];
  static TH1F *hPartParamSignal[NParticles][nHistoPartParam];
  static const int nHistoPartParamQA = 3;
  //static TH1F *hPartParamQA[NParticles][nHistoPartParamQA*2]; // residuals and pulls of these parameters

  static const int nHistosPV = 6;
  static TH1F *hPVFitQa[nHistosPV];

  TString patName[NParticles] = {"K0s","Lambda"};

  static bool first_call = 1;

  if ( first_call )
  {
    first_call = 0;

    TDirectory *currentDir = gDirectory;
    gDirectory = histodir;
    gDirectory->cd("L1");    
    gDirectory->mkdir("Particles");
    gDirectory->cd("Particles");
    {
      for(int iPart=0; iPart<NParticles; ++iPart)
      {
        gDirectory->mkdir(patName[iPart].Data());
        gDirectory->cd(patName[iPart].Data());
        {
          TString res = "res";
          TString pull = "pull";

          gDirectory->mkdir("DaughtersQA");
          gDirectory->cd("DaughtersQA");
          {
            TString parName[nFitQA/2] = {"X","Y","Z","Px","Py","Pz","E","M"};
            int nBins = 100;
            float xMax[nFitQA/2] = {0.15,0.15,0.03,0.01,0.01,0.06,0.06,0.01};

            for( int iH=0; iH<nFitQA/2; iH++ ){
              hFitDaughtersQA[iPart][iH]   = new TH1F((res+parName[iH]).Data(),
                                                      (res+parName[iH]).Data(), 
                                                      nBins, -xMax[iH],xMax[iH]);
              hFitDaughtersQA[iPart][iH+8] = new TH1F((pull+parName[iH]).Data(),
                                                      (pull+parName[iH]).Data(), 
                                                      nBins, -6,6);
            }
          }
          gDirectory->cd(".."); //particle directory

          gDirectory->mkdir("FitQA");
          gDirectory->cd("FitQA");
          {
            TString parName[nFitQA/2] = {"X","Y","Z","Px","Py","Pz","E","M"};
            int nBins = 50;
            float xMax[nFitQA/2] = {0.15,0.15,1.2,0.02,0.02,0.15,0.15,0.006};

            for( int iH=0; iH<nFitQA/2; iH++ ){
              hFitQA[iPart][iH]   = new TH1F((res+parName[iH]).Data(),
                                             (res+parName[iH]).Data(), 
                                             nBins, -xMax[iH],xMax[iH]);
              hFitQA[iPart][iH+8] = new TH1F((pull+parName[iH]).Data(),
                                             (pull+parName[iH]).Data(), 
                                             nBins, -6,6);
            }
          }
          gDirectory->cd(".."); //particle directory

          gDirectory->mkdir("Parameters");
          gDirectory->cd("Parameters");
          {
            TString parName[nHistoPartParam] = {"M","DL","c#tau","chi2ndf","prob"};
            int nBins[nHistoPartParam] = {1000,100,100,100,100};
            float xMin[nHistoPartParam] = {0.3f,0. , 0.,  0.,0.};
            float xMax[nHistoPartParam] = {1.3f,30.,30.,20.,1.};
            if(iPart == 1)
            {
              xMin[0] = 1.0; 
              xMax[0] = 2.0; 
            }

            for(int iH=0; iH<nHistoPartParam; iH++)
              hPartParam[iPart][iH]       = new TH1F(parName[iH].Data(),parName[iH].Data(),
                                              nBins[iH],xMin[iH],xMax[iH]);

            gDirectory->mkdir("Signal");
            gDirectory->cd("Signal");
            {
              for(int iH=0; iH<nHistoPartParam; iH++)
                hPartParamSignal[iPart][iH] = new TH1F((parName[iH]).Data(),(parName[iH]).Data(),
                                              nBins[iH],xMin[iH],xMax[iH]);

              gDirectory->mkdir("QA");
              gDirectory->cd("QA");
              {
                //int nBinsQA = 50;
                //float xMaxQA[nHistoPartParamQA] = {0.01,0.001,0.001};
                for( int iH=0; iH<nHistoPartParamQA; iH++ ){
                  //hPartParamQA[iPart][iH] =
                  //  new TH1F((res+parName[iH]).Data(), (res+parName[iH]).Data(), nBinsQA, -xMaxQA[iH],xMaxQA[iH]);
                  //hPartParamQA[iPart][iH+nHistoPartParamQA] =
                  //  new TH1F((pull+parName[iH]).Data(), (pull+parName[iH]).Data(), nBinsQA, -6,6);
                }
              }
              gDirectory->cd(".."); // particle directory / Parameters / Signal

            }
            gDirectory->cd(".."); // particle directory / Parameters
            gDirectory->mkdir("Background");
            gDirectory->cd("Background");
            {
              for(int iH=0; iH<nHistoPartParam; iH++)
                hPartParamBG[iPart][iH]     = new TH1F((parName[iH]).Data(),(parName[iH]).Data(),
                                              nBins[iH],xMin[iH],xMax[iH]);
            }
            gDirectory->cd(".."); // particle directory
          }
          gDirectory->cd(".."); //particle directory
        }
        gDirectory->cd(".."); //L1
      }
    }
    gDirectory->cd(".."); //L1
    gDirectory->mkdir("PrimaryVertexQA");
    gDirectory->cd("PrimaryVertexQA");
    {
      struct {
        string name;
        string title;
        Int_t n;
        Double_t l,r;
      } Table[nHistosPV]=
      {
        {"PVResX", "x_{rec}-x_{mc}, cm",     100, -0.006f, 0.006f},
        {"PVResY", "y_{rec}-y_{mc}, cm",     100, -0.006f, 0.006f},
        {"PVResZ", "z_{rec}-z_{mc}, cm",     100, -0.06f, 0.06f},
        {"PVPullX", "Pull X",     100, -6.f, 6.f},
        {"PVPullY", "Pull Y",     100, -6.f, 6.f},
        {"PVPullZ", "Pull Z",     100, -6.f, 6.f}
      };
      for(int iHPV=0; iHPV<nHistosPV; ++iHPV){
        hPVFitQa[iHPV] = new TH1F(Table[iHPV].name.data(),Table[iHPV].title.data(),
                                  Table[iHPV].n, Table[iHPV].l, Table[iHPV].r);
      }
    }
    gDirectory = currentDir;
  }

  for(unsigned int iP=0; iP < vRParticles.size(); iP++)
  {
    int iParticle = -1;
    switch( vRParticles[iP].GetPDG() ) {
      case 310:
      {
        iParticle = 0;
      }
      break;
      case 3122:
      {
        iParticle = 1;
      }
    }
    if(iParticle < 0) continue;

    Double_t M, ErrM;
    Double_t dL, ErrdL; // decay length
    Double_t cT, ErrcT; // c*tau
    CbmKFParticle TempPart = vRParticles[iP];
    TempPart.GetMass(M,ErrM);
    TempPart.GetDecayLength(dL,ErrdL);
    TempPart.GetLifeTime(cT,ErrcT);
    Double_t chi2 = TempPart.GetChi2();
    Int_t ndf = TempPart.GetNDF();
    Double_t prob = TMath::Prob(chi2,ndf);

    hPartParam[iParticle][0]->Fill(M);
    hPartParam[iParticle][1]->Fill(dL);
    hPartParam[iParticle][2]->Fill(cT);
    hPartParam[iParticle][3]->Fill(chi2/ndf);
    hPartParam[iParticle][4]->Fill(prob);

    if(!RtoMCParticleId[iP].IsMatchedWithPdg()) //background
    {
      hPartParamBG[iParticle][0]->Fill(M);
      hPartParamBG[iParticle][1]->Fill(dL);
      hPartParamBG[iParticle][2]->Fill(cT);
      hPartParamBG[iParticle][3]->Fill(chi2/ndf);
      hPartParamBG[iParticle][4]->Fill(prob);
      continue;
    }
    hPartParamSignal[iParticle][0]->Fill(M);
    hPartParamSignal[iParticle][1]->Fill(dL);
    hPartParamSignal[iParticle][2]->Fill(cT);
    hPartParamSignal[iParticle][3]->Fill(chi2/ndf);
    hPartParamSignal[iParticle][4]->Fill(prob);

    int iMCPart = RtoMCParticleId[iP].GetBestMatchWithPdg();
    KFMCParticle &mcPart = vMCParticles[iMCPart];
    // Fit quality of the mother particle
    {
      int iMCTrack = mcPart.GetMCTrackID();
      CbmMCTrack &mcTrack = *(L1_DYNAMIC_CAST<CbmMCTrack*>(listMCTracks->At(iMCTrack)));
      int mcDaughterId = mcPart.GetDaughterIds()[0];
      CbmMCTrack &mcDaughter = *(L1_DYNAMIC_CAST<CbmMCTrack*>(listMCTracks->At(mcDaughterId)));

      Double_t decayVtx[3] = { mcDaughter.GetStartX(), mcDaughter.GetStartY(), mcDaughter.GetStartZ() };
      Double_t recParam[8] = { 0 };
      Double_t errParam[8] = { 0 };

      for(int iPar=0; iPar<3; iPar++)
      {
        recParam[iPar] = TempPart.GetParameter(iPar);
        Double_t error = TempPart.GetCovariance(iPar,iPar);
        if(error < 0.) { error = 1.e20;}
        errParam[iPar] = TMath::Sqrt(error);
      }
      TempPart.Extrapolate(TempPart.r , TempPart.GetDStoPoint(decayVtx));
      for(int iPar=3; iPar<7; iPar++)
      {
        recParam[iPar] = TempPart.GetParameter(iPar);
        Double_t error = TempPart.GetCovariance(iPar,iPar);
        if(error < 0.) { error = 1.e20;}
        errParam[iPar] = TMath::Sqrt(error);
      }

      Double_t Emc = sqrt(mcTrack.GetP()*mcTrack.GetP() + mcTrack.GetMass()*mcTrack.GetMass());
      Double_t res[8] = {0}, 
               pull[8] = {0}, 
               mcParam[8] = { decayVtx[0], decayVtx[1], decayVtx[2],
                              mcTrack.GetPx(), mcTrack.GetPy(), mcTrack.GetPz(), Emc, mcTrack.GetMass() };
      for(int iPar=0; iPar < 7; iPar++ )
      {
        res[iPar]  = recParam[iPar] - mcParam[iPar];
        if(fabs(errParam[iPar]) > 1.e-20) pull[iPar] = res[iPar]/errParam[iPar];
      }
      res[7] = M - mcParam[7];
      if(fabs(ErrM) > 1.e-20) pull[7] = res[7]/ErrM;

      for(int iPar=0; iPar < 8; iPar++ )
      {
        hFitQA[iParticle][iPar]->Fill(res[iPar]);
        hFitQA[iParticle][iPar+8]->Fill(pull[iPar]);
      }

//      Double_t mcT = mcDaughter.GetStartT() - mcTrack.GetStartT();
      
    }
    // Fit quality of daughters
    for(int iD=0; iD<mcPart.NDaughters(); ++iD)
    {
      int mcDaughterId = mcPart.GetDaughterIds()[iD];
      if(!MCtoRParticleId[mcDaughterId].IsMatchedWithPdg()) continue;
      CbmMCTrack &mcTrack = *(L1_DYNAMIC_CAST<CbmMCTrack*>(listMCTracks->At(mcDaughterId)));
      int recDaughterId = MCtoRParticleId[mcDaughterId].GetBestMatchWithPdg();
      CbmKFParticle Daughter = vRParticles[recDaughterId];
      Daughter.GetMass(M,ErrM);

      Double_t decayVtx[3] = {mcTrack.GetStartX(), mcTrack.GetStartY(), mcTrack.GetStartZ()};
      Daughter.Extrapolate(Daughter.r , Daughter.GetDStoPoint(decayVtx));

      Double_t Emc = sqrt(mcTrack.GetP()*mcTrack.GetP() + mcTrack.GetMass()*mcTrack.GetMass());
      Double_t res[8] = {0}, 
               pull[8] = {0}, 
               mcParam[8] = { mcTrack.GetStartX(), mcTrack.GetStartY(), mcTrack.GetStartZ(),
                            mcTrack.GetPx(), mcTrack.GetPy(), mcTrack.GetPz(), Emc, mcTrack.GetMass() };
      for(int iPar=0; iPar < 7; iPar++ )
      {
        Double_t error = Daughter.GetCovariance(iPar,iPar);
        if(error < 0.) { error = 1.e20;}
        error = TMath::Sqrt(error);
        res[iPar]  = Daughter.GetParameter(iPar) - mcParam[iPar];
        if(fabs(error) > 1.e-20) pull[iPar] = res[iPar]/error;
      }
      res[7] = M - mcParam[7];
      if(fabs(ErrM) > 1.e-20) pull[7] = res[7]/ErrM;

      for(int iPar=0; iPar < 8; iPar++ )
      {
        hFitDaughtersQA[iParticle][iPar]->Fill(res[iPar]);
        hFitDaughtersQA[iParticle][iPar+8]->Fill(pull[iPar]);
      }
    }
//     for(int iD=0; iD<mcPart.NDaughters(); ++iD)
//     {
//       int mcDaughterId = mcPart.GetDaughterIds()[iD];
//       if(!MCtoRParticleId[mcDaughterId].IsMatchedWithPdg()) continue;
//       CbmMCTrack &mcTrack = *(L1_DYNAMIC_CAST<CbmMCTrack*>(listMCTracks->At(mcDaughterId)));
//       int recDaughterId = MCtoRParticleId[mcDaughterId].GetBestMatchWithPdg();
// 
//       CbmKFParticleInterface Daughter1;
//       CbmKFParticle_simd Daughter = vRParticles[recDaughterId];
//       fvec M,ErrM;
//       Daughter.GetMass(M,ErrM);
// 
//       fvec decayVtx[3] = {mcTrack.GetStartX(), mcTrack.GetStartY(), mcTrack.GetStartZ()};
//       Daughter1.Extrapolate(&Daughter, Daughter.r , Daughter1.GetDStoPoint(&Daughter,decayVtx));
// 
//       fvec Emc = sqrt(mcTrack.GetP()*mcTrack.GetP() + mcTrack.GetMass()*mcTrack.GetMass());
//       fvec res[8] = {0}, 
//            pull[8] = {0}, 
//            mcParam[8] = { mcTrack.GetStartX(), mcTrack.GetStartY(), mcTrack.GetStartZ(),
//                           mcTrack.GetPx(), mcTrack.GetPy(), mcTrack.GetPz(), Emc, mcTrack.GetMass() };
//       for(int iPar=0; iPar < 7; iPar++ )
//       {
//         fvec error = Daughter.C[Daughter1.IJ(iPar, iPar)];
//         if(error[0] < 0.) { error = 1.e20;}
//         error = TMath::Sqrt(error[0]);
//         res[iPar]  = Daughter.r[iPar] - mcParam[iPar];
//         if(fabs(error)[0] > 1.e-20) pull[iPar] = res[iPar]/error;
//       }
//       res[7] = M - mcParam[7];
//       if(fabs(ErrM)[0] > 1.e-20) pull[7] = res[7]/ErrM;
// 
//       for(int iPar=0; iPar < 8; iPar++ )
//       {
//         hFitDaughtersQA[iParticle][iPar]->Fill(res[iPar][0]);
//         hFitDaughtersQA[iParticle][iPar+8]->Fill(pull[iPar][0]);
//       }
//     }
  }
  //Find MC parameters of the primary vertex 
  float mcPVx[3]={0.f};
  for(unsigned iMC=0; iMC<vMCTracks.size(); ++iMC)
  {
    if(vMCTracks[iMC].IsPrimary())
    {
      mcPVx[0]=vMCTracks[iMC].x;
      mcPVx[1]=vMCTracks[iMC].y;
      mcPVx[2]=vMCTracks[iMC].z;
      break;
    }
  }
  float dRPVr[3] = {(float)(PF->GetPV()->GetRefX()-mcPVx[0]),
                    (float)(PF->GetPV()->GetRefY()-mcPVx[1]),
                    (float)(PF->GetPV()->GetRefZ()-mcPVx[2])};
  float dRPVp[3] = {(float)(dRPVr[0]/sqrt(PF->GetPV()->GetCovMatrix()[0])),
                    (float)(dRPVr[1]/sqrt(PF->GetPV()->GetCovMatrix()[2])),
                    (float)(dRPVr[2]/sqrt(PF->GetPV()->GetCovMatrix()[5]))};
  for(unsigned int iHPV=0; iHPV<3; ++iHPV)
    hPVFitQa[iHPV]->Fill(dRPVr[iHPV]);
  for(unsigned int iHPV=3; iHPV<6; ++iHPV)
    hPVFitQa[iHPV]->Fill(dRPVp[iHPV-3]);

} // void CbmL1::ParticlesEfficienciesPerformance()

void CbmL1::HistoPerformance() // TODO: check if works correctly. Change vHitRef on match data in CbmL1**Track classes
{

  //CbmKF &KF = *CbmKF::Instance();

  static TProfile *p_eff_all_vs_mom, *p_eff_prim_vs_mom, *p_eff_sec_vs_mom, *p_eff_d0_vs_mom, *p_eff_prim_vs_theta, *p_eff_all_vs_pt, *p_eff_prim_vs_pt, *p_eff_all_vs_nhits, *p_eff_prim_vs_nhits, *p_eff_sec_vs_nhits;

  static TH1F *h_reg_MCmom, *h_acc_MCmom, *h_reco_MCmom, *h_ghost_Rmom;
  static TH1F *h_reg_prim_MCmom, *h_acc_prim_MCmom, *h_reco_prim_MCmom;
  static TH1F *h_reg_sec_MCmom, *h_acc_sec_MCmom, *h_reco_sec_MCmom;
      
  static TH1F *h_acc_mom_short123s;
  
  static TH1F *h_reg_mom_prim, *h_reg_mom_sec, *h_reg_nhits_prim, *h_reg_nhits_sec;
  static TH1F *h_acc_mom_prim, *h_acc_mom_sec, *h_acc_nhits_prim, *h_acc_nhits_sec;
  static TH1F *h_reco_mom_prim, *h_reco_mom_sec, *h_reco_nhits_prim, *h_reco_nhits_sec;
  static TH1F *h_rest_mom_prim, *h_rest_mom_sec, *h_rest_nhits_prim, *h_rest_nhits_sec;

  //static TH1F *h_hit_density[10];

  static TH1F
      *h_ghost_mom, *h_ghost_nhits, *h_ghost_fstation,
  *h_ghost_chi2, *h_ghost_prob, *h_ghost_tx, *h_ghost_ty;
  static TH1F *h_reco_mom,  *h_reco_d0_mom, *h_reco_nhits, *h_reco_station,
  *h_reco_chi2, *h_reco_prob, *h_rest_prob, *h_reco_clean, *h_reco_time;
  static TProfile *h_reco_timeNtr, *h_reco_timeNhit ;
  static TProfile *h_reco_fakeNtr, *h_reco_fakeNhit ;
  static TH1F *h_tx, *h_ty, *h_sec_r, *h_ghost_r;

  static TH1F *h_notfound_mom, *h_notfound_nhits, *h_notfound_station, *h_notfound_r, *h_notfound_tx, *h_notfound_ty;
  
  static TH1F *h_mcp, *h_nmchits;//, *h_chi2, *h_prob, *MC_vx, *MC_vy, *MC_vz, *VtxChiPrim, *VtxChiSec;

  //static TH2F *P_vs_P ;

  static TH2F *h2_vertex, *h2_prim_vertex, *h2_sec_vertex;
  //static TH3F *h3_vertex, *h3_prim_vertex, *h3_sec_vertex;

  static TH2F *h2_reg_nhits_vs_mom_prim, *h2_reg_nhits_vs_mom_sec,
  *h2_reg_fstation_vs_mom_prim, *h2_reg_fstation_vs_mom_sec, *h2_reg_lstation_vs_fstation_prim, *h2_reg_lstation_vs_fstation_sec;
  static TH2F *h2_acc_nhits_vs_mom_prim, *h2_acc_nhits_vs_mom_sec,
  *h2_acc_fstation_vs_mom_prim, *h2_acc_fstation_vs_mom_sec, *h2_acc_lstation_vs_fstation_prim, *h2_acc_lstation_vs_fstation_sec;
  static TH2F *h2_reco_nhits_vs_mom_prim, *h2_reco_nhits_vs_mom_sec,
  *h2_reco_fstation_vs_mom_prim, *h2_reco_fstation_vs_mom_sec, *h2_reco_lstation_vs_fstation_prim, *h2_reco_lstation_vs_fstation_sec;
  static TH2F *h2_ghost_nhits_vs_mom, *h2_ghost_fstation_vs_mom, *h2_ghost_lstation_vs_fstation;
  static TH2F *h2_rest_nhits_vs_mom_prim, *h2_rest_nhits_vs_mom_sec,
  *h2_rest_fstation_vs_mom_prim, *h2_rest_fstation_vs_mom_sec, *h2_rest_lstation_vs_fstation_prim, *h2_rest_lstation_vs_fstation_sec;

  static bool first_call = 1;

  if ( first_call )
  {
    first_call = 0;

    TDirectory *curdir = gDirectory;
    gDirectory = histodir;
    gDirectory->cd("L1");

    p_eff_all_vs_mom = new TProfile("p_eff_all_vs_mom", "AllSet Efficiency vs Momentum",
                                    100, 0.0, 5.0, 0.0, 100.0);
    p_eff_prim_vs_mom = new TProfile("p_eff_prim_vs_mom", "Primary Set Efficiency vs Momentum",
                                     100, 0.0, 5.0, 0.0, 100.0);
    p_eff_sec_vs_mom = new TProfile("p_eff_sec_vs_mom", "Secondary Set Efficiency vs Momentum",
                                    100, 0.0, 5.0, 0.0, 100.0);
    p_eff_d0_vs_mom = new TProfile("p_eff_d0_vs_mom", "D0 Secondary Tracks Efficiency vs Momentum",
                                   150, 0.0, 15.0, 0.0, 100.0);
    p_eff_prim_vs_theta = new TProfile("p_eff_prim_vs_theta", "All Primary Set Efficiency vs Theta",
                                    100, 0.0, 30.0, 0.0, 100.0);
    p_eff_all_vs_pt = new TProfile("p_eff_all_vs_pt", "AllSet Efficiency vs Pt",
                                    100, 0.0, 5.0, 0.0, 100.0);
    p_eff_prim_vs_pt = new TProfile("p_eff_prim_vs_pt", "Primary Set Efficiency vs Pt",
        100, 0.0, 5.0, 0.0, 100.0);

    p_eff_all_vs_nhits = new TProfile("p_eff_all_vs_nhits", "AllSet Efficiency vs NMCHits",
    8, 3.0, 11.0, 0.0, 100.0);
    p_eff_prim_vs_nhits = new TProfile("p_eff_prim_vs_nhits", "PrimSet Efficiency vs NMCHits",
    8, 3.0, 11.0, 0.0, 100.0);
    p_eff_sec_vs_nhits = new TProfile("p_eff_sec_vs_nhits", "SecSet Efficiency vs NMCHits",
    8, 3.0, 11.0, 0.0, 100.0);

    h_reg_MCmom   = new TH1F("h_reg_MCmom", "Momentum of registered tracks", 100, 0.0, 5.0);
    h_acc_MCmom   = new TH1F("h_acc_MCmom", "Reconstructable tracks", 100, 0.0, 5.0);
    h_reco_MCmom   = new TH1F("h_reco_MCmom", "Reconstructed tracks", 100, 0.0, 5.0);
    h_ghost_Rmom = new TH1F("h_ghost_Rmom", "Ghost tracks", 100, 0.0, 5.0);
    h_reg_prim_MCmom   = new TH1F("h_reg_prim_MCmom", "Momentum of registered tracks", 100, 0.0, 5.0);
    h_acc_prim_MCmom   = new TH1F("h_acc_prim_MCmom", "Reconstructable tracks", 100, 0.0, 5.0);
    h_reco_prim_MCmom   = new TH1F("h_reco_prim_MCmom", "Reconstructed tracks", 100, 0.0, 5.0);
    h_reg_sec_MCmom   = new TH1F("h_reg_sec_MCmom", "Momentum of registered tracks", 100, 0.0, 5.0);
    h_acc_sec_MCmom   = new TH1F("h_acc_sec_MCmom", "Reconstructable tracks", 100, 0.0, 5.0);
    h_reco_sec_MCmom   = new TH1F("h_reco_sec_MCmom", "Reconstructed tracks", 100, 0.0, 5.0);
 
    h_acc_mom_short123s = new TH1F("h_acc_mom_short123s", "Momentum of accepted tracks with 3 hits on first stations", 500, 0.0, 5.0);
     
    h_reg_mom_prim   = new TH1F("h_reg_mom_prim", "Momentum of registered primary tracks", 500, 0.0, 5.0);
    h_reg_mom_sec   = new TH1F("h_reg_mom_sec", "Momentum of registered secondary tracks", 500, 0.0, 5.0);
    h_acc_mom_prim   = new TH1F("h_acc_mom_prim", "Momentum of accepted primary tracks", 500, 0.0, 5.0);
    h_acc_mom_sec   = new TH1F("h_acc_mom_sec", "Momentum of accepted secondary tracks", 500, 0.0, 5.0);
    h_reco_mom_prim   = new TH1F("h_reco_mom_prim", "Momentum of reconstructed primary tracks", 500, 0.0, 5.0);
    h_reco_mom_sec   = new TH1F("h_reco_mom_sec", "Momentum of reconstructed secondary tracks", 500, 0.0, 5.0);
    h_rest_mom_prim   = new TH1F("h_rest_mom_prim", "Momentum of not found primary tracks", 500, 0.0, 5.0);
    h_rest_mom_sec   = new TH1F("h_rest_mom_sec", "Momentum of not found secondary tracks", 500, 0.0, 5.0);

    h_reg_nhits_prim = new TH1F("h_reg_nhits_prim", "Number of hits in registered primary track", 51, -0.1, 10.1);
    h_reg_nhits_sec = new TH1F("h_reg_nhits_sec", "Number of hits in registered secondary track", 51, -0.1, 10.1);
    h_acc_nhits_prim = new TH1F("h_acc_nhits_prim", "Number of hits in accepted primary track", 51, -0.1, 10.1);
    h_acc_nhits_sec = new TH1F("h_acc_nhits_sec", "Number of hits in accepted secondary track", 51, -0.1, 10.1);
    h_reco_nhits_prim = new TH1F("h_reco_nhits_prim", "Number of hits in reconstructed primary track", 51, -0.1, 10.1);
    h_reco_nhits_sec = new TH1F("h_reco_nhits_sec", "Number of hits in reconstructed secondary track", 51, -0.1, 10.1);
    h_rest_nhits_prim = new TH1F("h_rest_nhits_prim", "Number of hits in not found primary track", 51, -0.1, 10.1);
    h_rest_nhits_sec = new TH1F("h_rest_nhits_sec", "Number of hits in not found secondary track", 51, -0.1, 10.1);

    h_ghost_mom = new TH1F("h_ghost_mom", "Momentum of ghost tracks", 500, 0.0, 5.0);
    h_ghost_nhits = new TH1F("h_ghost_nhits", "Number of hits in ghost track", 51, -0.1, 10.1);
    h_ghost_fstation = new TH1F("h_ghost_fstation", "First station of ghost track", 50, -0.5, 10.0);
    h_ghost_chi2 = new TH1F("h_ghost_chi2", "Chi2/NDF of ghost track", 50, -0.5, 10.0);
    h_ghost_prob = new TH1F("h_ghost_prob", "Prob of ghost track", 505, 0., 1.01);
    h_ghost_r = new TH1F("h_ghost_r", "R of ghost track at the first hit", 50, 0.0, 15.0);
    h_ghost_tx = new TH1F("h_ghost_tx", "tx of ghost track at the first hit", 50, -5.0, 5.0);
    h_ghost_ty = new TH1F("h_ghost_ty", "ty of ghost track at the first hit", 50, -1.0, 1.0);

    h_reco_mom = new TH1F("h_reco_mom", "Momentum of reco track", 50, 0.0, 5.0);
    h_reco_nhits = new TH1F("h_reco_nhits", "Number of hits in reco track", 50, 0.0, 10.0);
    h_reco_station = new TH1F("h_reco_station", "First station of reco track", 50, -0.5, 10.0);
    h_reco_chi2 = new TH1F("h_reco_chi2", "Chi2/NDF of reco track", 50, -0.5, 10.0);
    h_reco_prob = new TH1F("h_reco_prob", "Prob of reco track", 505, 0., 1.01);
    h_rest_prob = new TH1F("h_rest_prob", "Prob of reco rest track", 505, 0., 1.01);
    h_reco_clean = new TH1F("h_reco_clean", "Percentage of correct hits", 100, -0.5, 100.5);
    h_reco_time = new TH1F("h_reco_time", "CA Track Finder Time (s/ev)", 20, 0.0, 20.0);
    h_reco_timeNtr = new TProfile("h_reco_timeNtr",
                                  "CA Track Finder Time (s/ev) vs N Tracks",
                                  200, 0.0, 1000.0);
    h_reco_timeNhit = new TProfile("h_reco_timeNhit",
                                   "CA Track Finder Time (s/ev) vs N Hits",
                                   200, 0.0, 30000.0);
    h_reco_fakeNtr = new TProfile("h_reco_fakeNtr",
                                  "N Fake hits vs N Tracks",
                                  200, 0.0, 1000.0);
    h_reco_fakeNhit = new TProfile("h_reco_fakeNhit",
                                   "N Fake hits vs N Hits",
                                   200, 0.0, 30000.0);

    h_reco_d0_mom = new TH1F("h_reco_d0_mom", "Momentum of reco D0 track", 150, 0.0, 15.0);
      
//     h_hit_density[0] = new TH1F("h_hit_density0", "Hit density station 1", 50, 0.0,  5.00);
//     h_hit_density[1] = new TH1F("h_hit_density1", "Hit density station 2", 100, 0.0, 10.00);
//     h_hit_density[2] = new TH1F("h_hit_density2", "Hit density station 3", 140, 0.0, 13.99);
//     h_hit_density[3] = new TH1F("h_hit_density3", "Hit density station 4", 180, 0.0, 18.65);
//     h_hit_density[4] = new TH1F("h_hit_density4", "Hit density station 5", 230, 0.0, 23.32);
//     h_hit_density[5] = new TH1F("h_hit_density5", "Hit density station 6", 270, 0.0, 27.98);
//     h_hit_density[6] = new TH1F("h_hit_density6", "Hit density station 7", 340, 0.0, 34.97);
//     h_hit_density[7] = new TH1F("h_hit_density7", "Hit density station 8", 460, 0.0, 46.63);
//     h_hit_density[8] = new TH1F("h_hit_density8", "Hit density station 9", 500, 0.0, 50.00);
//     h_hit_density[9] = new TH1F("h_hit_density8", "Hit density station 9", 500, 0.0, 50.00);
//     h_hit_density[10] = new TH1F("h_hit_density8", "Hit density station 9", 500, 0.0, 50.00);
      
    h_tx = new TH1F("h_tx", "tx of MC track at the vertex", 50, -0.5, 0.5);
    h_ty = new TH1F("h_ty", "ty of MC track at the vertex", 50, -0.5, 0.5);

    h_sec_r = new TH1F("h_sec_r", "R of sec MC track at the first hit", 50, 0.0, 15.0);

    h_notfound_mom = new TH1F("h_notfound_mom", "Momentum of not found track", 50, 0.0, 5.0);
    h_notfound_nhits = new TH1F("h_notfound_nhits", "Num hits of not found track", 50, 0.0, 10.0);
    h_notfound_station = new TH1F("h_notfound_station", "First station of not found track", 50, 0.0, 10.0);
    h_notfound_r = new TH1F("h_notfound_r", "R at first hit of not found track", 50, 0.0, 15.0);
    h_notfound_tx = new TH1F("h_notfound_tx", "tx of not found track at the first hit", 50, -5.0, 5.0);
    h_notfound_ty = new TH1F("h_notfound_ty", "ty of not found track at the first hit", 50, -1.0, 1.0);

    //h_chi2 = new TH1F("chi2", "Chi^2", 100, 0.0, 10.);
    //h_prob = new TH1F("prob", "Prob", 100, 0.0, 1.01);
    //VtxChiPrim = new TH1F("vtxChiPrim", "Chi to primary vtx for primary tracks", 100, 0.0, 10.);
    //VtxChiSec = new TH1F("vtxChiSec", "Chi to primary vtx for secondary tracks", 100, 0.0, 10.);
    h_mcp = new TH1F("h_mcp", "MC P ", 500, 0.0, 5.0);
    //MC_vx = new TH1F("MC_vx", "MC Vertex X", 100, -.05, .05);
    //MC_vy = new TH1F("MC_vy", "MC Vertex Y", 100, -.05, .05);
    //MC_vz = new TH1F("MC_vz", "MC Vertex Z", 100, -.05, .05);
    h_nmchits = new TH1F("h_nmchits", "N STS hits on MC track", 50, 0.0, 10.0);
 
    //P_vs_P = new TH2F("P_vs_P", "Resolution P/Q vs P", 20, 0., 20.,100, -.05, .05);
      
    h2_vertex = new TH2F("h2_vertex", "2D vertex distribution", 105, -5., 100., 100, -50., 50.);
    h2_prim_vertex = new TH2F("h2_primvertex", "2D primary vertex distribution", 105, -5., 100., 100, -50., 50.);
    h2_sec_vertex = new TH2F("h2_sec_vertex", "2D secondary vertex distribution", 105, -5., 100., 100, -50., 50.);

      //h3_vertex = new TH3F("h3_vertex", "3D vertex distribution", 20, -5., 100., 100, -50., 50., 100, -50., 50.);
      //h3_prim_vertex = new TH3F("h3_primvertex", "3D vertex distribution", 20, -5., 100., 100, -50., 50., 100, -50., 50.);
      //h3_sec_vertex = new TH3F("h3_sec_vertex", "3D vertex distribution", 20, -5., 100., 100, -50., 50., 100, -50., 50.);

    h2_reg_nhits_vs_mom_prim = new TH2F("h2_reg_nhits_vs_mom_prim", "NHits vs. Momentum (Reg. Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_reg_nhits_vs_mom_sec = new TH2F("h2_reg_nhits_vs_mom_sec", "NHits vs. Momentum (Reg. Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_acc_nhits_vs_mom_prim = new TH2F("h2_acc_nhits_vs_mom_prim", "NHits vs. Momentum (Acc. Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_acc_nhits_vs_mom_sec = new TH2F("h2_acc_nhits_vs_mom_sec", "NHits vs. Momentum (Acc. Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_reco_nhits_vs_mom_prim = new TH2F("h2_reco_nhits_vs_mom_prim", "NHits vs. Momentum (Reco Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_reco_nhits_vs_mom_sec = new TH2F("h2_reco_nhits_vs_mom_sec", "NHits vs. Momentum (Reco Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_ghost_nhits_vs_mom = new TH2F("h2_ghost_nhits_vs_mom", "NHits vs. Momentum (Ghost Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_rest_nhits_vs_mom_prim = new TH2F("h2_rest_nhits_vs_mom_prim", "NHits vs. Momentum (!Found Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_rest_nhits_vs_mom_sec = new TH2F("h2_rest_nhits_vs_mom_sec", "NHits vs. Momentum (!Found Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);

    h2_reg_fstation_vs_mom_prim = new TH2F("h2_reg_fstation_vs_mom_prim", "First Station vs. Momentum (Reg. Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_reg_fstation_vs_mom_sec = new TH2F("h2_reg_fstation_vs_mom_sec", "First Station vs. Momentum (Reg. Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_acc_fstation_vs_mom_prim = new TH2F("h2_acc_fstation_vs_mom_prim", "First Station vs. Momentum (Acc. Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_acc_fstation_vs_mom_sec = new TH2F("h2_acc_fstation_vs_mom_sec", "First Station vs. Momentum (Acc. Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_reco_fstation_vs_mom_prim = new TH2F("h2_reco_fstation_vs_mom_prim", "First Station vs. Momentum (Reco Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_reco_fstation_vs_mom_sec = new TH2F("h2_reco_fstation_vs_mom_sec", "First Station vs. Momentum (Reco Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_ghost_fstation_vs_mom = new TH2F("h2_ghost_fstation_vs_mom", "First Station vs. Momentum (Ghost Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_rest_fstation_vs_mom_prim = new TH2F("h2_rest_fstation_vs_mom_prim", "First Station vs. Momentum (!Found Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_rest_fstation_vs_mom_sec = new TH2F("h2_rest_fstation_vs_mom_sec", "First Station vs. Momentum (!Found Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);

    h2_reg_lstation_vs_fstation_prim = new TH2F("h2_reg_lstation_vs_fstation_prim", "Last vs. First Station (Reg. Primary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_reg_lstation_vs_fstation_sec = new TH2F("h2_reg_lstation_vs_fstation_sec", "Last vs. First Station (Reg. Secondary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_acc_lstation_vs_fstation_prim = new TH2F("h2_acc_lstation_vs_fstation_prim", "Last vs. First Station (Acc. Primary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_acc_lstation_vs_fstation_sec = new TH2F("h2_acc_lstation_vs_fstation_sec", "Last vs. First Station (Acc. Secondary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_reco_lstation_vs_fstation_prim = new TH2F("h2_reco_lstation_vs_fstation_prim", "Last vs. First Station (Reco Primary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_reco_lstation_vs_fstation_sec = new TH2F("h2_reco_lstation_vs_fstation_sec", "Last vs. First Station (Reco Secondary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_ghost_lstation_vs_fstation = new TH2F("h2_ghost_lstation_vs_fstation", "Last vs. First Station (Ghost Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_rest_lstation_vs_fstation_prim = new TH2F("h2_rest_lstation_vs_fstation_prim", "Last vs. First Station (!Found Primary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_rest_lstation_vs_fstation_sec = new TH2F("h2_rest_lstation_vs_fstation_sec", "Last vs. First Station (!Found Secondary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);

      //maindir->cd(); 

      // ----- Create list of all histogram pointers
      
    gDirectory = curdir;
      
  }// first_call


  static int NEvents = 0;
  if ( NEvents > 0 ) {
    h_reg_MCmom->Scale(NEvents);
    h_acc_MCmom->Scale(NEvents);
    h_reco_MCmom->Scale(NEvents);
    h_ghost_Rmom->Scale(NEvents);
    h_reg_prim_MCmom->Scale(NEvents);
    h_acc_prim_MCmom->Scale(NEvents);
    h_reco_prim_MCmom->Scale(NEvents);
    h_reg_sec_MCmom->Scale(NEvents);
    h_acc_sec_MCmom->Scale(NEvents);
    h_reco_sec_MCmom->Scale(NEvents);
  }
  NEvents++;
    
//   //hit density calculation: h_hit_density[10]
  //
//   for (vector<CbmL1HitStore>::iterator hIt = vHitStore.begin(); hIt != vHitStore.end(); ++hIt){
//     float x = hIt->x;
//     float y = hIt->y;
//     float r = sqrt(x*x+y*y);
//     h_hit_density[hIt->iStation]->Fill(r, 1.0/(2.0*3.1415*r));
//   }
  

  //
  for (vector<CbmL1Track>::iterator rtraIt = vRTracks.begin(); rtraIt != vRTracks.end(); ++rtraIt){
    CbmL1Track* prtra = &(*rtraIt);
    if((prtra->StsHits).size() < 1) continue;
    {  // fill histos
      if( fabs(prtra->T[4])>1.e-10 ) h_reco_mom->Fill(fabs(1.0/prtra->T[4]));
      h_reco_nhits->Fill((prtra->StsHits).size());
      CbmL1HitStore &mh = vHitStore[prtra->StsHits[0]];
      h_reco_station->Fill(mh.iStation);

    }

    h_reco_clean->Fill( prtra->GetMaxPurity() );
    
    if (prtra->NDF > 0) {
      if ( prtra->IsGhost() ){
        h_ghost_chi2->Fill(prtra->chi2/prtra->NDF);
        h_ghost_prob->Fill(TMath::Prob(prtra->chi2,prtra->NDF));
      }
      else {
        if (prtra->GetMCTrack()[0].IsReconstructable()){ 
          h_reco_chi2->Fill(prtra->chi2/prtra->NDF);
          h_reco_prob->Fill(TMath::Prob(prtra->chi2,prtra->NDF));
        } else {
//          h_rest_chi2->Fill(prtra->chi2/prtra->NDF);
          h_rest_prob->Fill(TMath::Prob(prtra->chi2,prtra->NDF));
        }
      }
    }

    
      // fill ghost histos
    if ( prtra->IsGhost() ){
      if( fabs(prtra->T[4])>1.e-10) {
        h_ghost_mom->Fill(fabs(1.0/prtra->T[4]));
        h_ghost_Rmom->Fill(fabs(1.0/prtra->T[4]));
      }
      h_ghost_nhits->Fill((prtra->StsHits).size());
      CbmL1HitStore &h1 = vHitStore[prtra->StsHits[0]];
      CbmL1HitStore &h2 = vHitStore[prtra->StsHits[1]];
      h_ghost_fstation->Fill(h1.iStation);
      h_ghost_r->Fill(sqrt(fabs(h1.x*h1.x+h1.y*h1.y)));
      double z1 = algo->vStations[h1.iStation].z[0];
      double z2 = algo->vStations[h2.iStation].z[0];
      if( fabs(z2-z1)>1.e-4 ){
        h_ghost_tx->Fill((h2.x-h1.x)/(z2-z1));
        h_ghost_ty->Fill((h2.y-h1.y)/(z2-z1));
      }

      if( fabs(prtra->T[4])>1.e-10)
        h2_ghost_nhits_vs_mom->Fill(fabs(1.0/prtra->T[4]), (prtra->StsHits).size());
      CbmL1HitStore &hf = vHitStore[prtra->StsHits[0]];
      CbmL1HitStore &hl = vHitStore[prtra->StsHits[(prtra->StsHits).size()-1]];
      if( fabs(prtra->T[4])>1.e-10)
        h2_ghost_fstation_vs_mom->Fill(fabs(1.0/prtra->T[4]), hf.iStation+1);
      if (hl.iStation >= hf.iStation)
        h2_ghost_lstation_vs_fstation->Fill(hf.iStation+1, hl.iStation+1);
    }

  } // for reco tracks

  int mc_total = 0; // total amount of reconstructable mcTracks
  for ( vector<CbmL1MCTrack>::iterator mtraIt = vMCTracks.begin(); mtraIt != vMCTracks.end(); mtraIt++ ) {
    CbmL1MCTrack &mtra = *(mtraIt);
//    if( !( mtra.pdg == -11 && mtra.mother_ID == -1 ) ) continue; // electrons only
    
    // No Sts hits? 
    int nmchits = mtra.StsHits.size();
    if (nmchits == 0) continue;

    double momentum = mtra.p;
    double pt = sqrt(mtra.px*mtra.px+mtra.py*mtra.py);
    double theta = acos(mtra.pz/mtra.p)*180/3.1415;
      
    h_mcp->Fill(momentum);
    h_nmchits->Fill(nmchits);

    int nSta = mtra.NStations();

    CbmL1HitStore &fh = vHitStore[*(mtra.StsHits.begin())];
    CbmL1HitStore &lh = vHitStore[*(mtra.StsHits.rbegin())];

    h_reg_MCmom->Fill(momentum);
    if ( mtra.IsPrimary() ){
      h_reg_mom_prim->Fill(momentum);
      h_reg_prim_MCmom->Fill(momentum);
      h_reg_nhits_prim->Fill(nSta);
      h2_reg_nhits_vs_mom_prim->Fill(momentum, nSta);
      h2_reg_fstation_vs_mom_prim->Fill(momentum, fh.iStation+1);
      if (lh.iStation >= fh.iStation)
        h2_reg_lstation_vs_fstation_prim->Fill(fh.iStation+1, lh.iStation+1);
    }else{
      h_reg_mom_sec->Fill(momentum);
      h_reg_sec_MCmom->Fill(momentum);
      h_reg_nhits_sec->Fill(nSta);
      if (momentum > 0.01){
        h2_reg_nhits_vs_mom_sec->Fill(momentum, nSta);
        h2_reg_fstation_vs_mom_sec->Fill(momentum, fh.iStation+1);
        if (lh.iStation >= fh.iStation)
          h2_reg_lstation_vs_fstation_sec->Fill(fh.iStation+1, lh.iStation+1);
      }
    }
    
    if ( mtra.IsAdditional() ){
      h_acc_mom_short123s->Fill(momentum);
    }
    
    if( ! mtra.IsReconstructable() ) continue;
    mc_total++;

    h_acc_MCmom->Fill(momentum);
    if ( mtra.IsPrimary() ){
      h_acc_mom_prim->Fill(momentum);
      h_acc_prim_MCmom->Fill(momentum);
      h_acc_nhits_prim->Fill(nSta);
      h2_acc_nhits_vs_mom_prim->Fill(momentum, nSta);
      h2_acc_fstation_vs_mom_prim->Fill(momentum, fh.iStation+1);
      if (lh.iStation >= fh.iStation)
        h2_acc_lstation_vs_fstation_prim->Fill(fh.iStation+1, lh.iStation+1);
    }else{
      h_acc_mom_sec->Fill(momentum);
      h_acc_sec_MCmom->Fill(momentum);
      h_acc_nhits_sec->Fill(nSta);
      if (momentum > 0.01){
        h2_acc_nhits_vs_mom_sec->Fill(momentum, nSta);
        h2_acc_fstation_vs_mom_sec->Fill(momentum, fh.iStation+1);
        if (lh.iStation >= fh.iStation)
          h2_acc_lstation_vs_fstation_sec->Fill(fh.iStation+1, lh.iStation+1);
      }
    }
      

      // vertex distribution of primary and secondary tracks   
    h2_vertex->Fill(mtra.z, mtra.y);
    //h3_vertex->Fill(mtra.z, mtra.x, mtra.y);
    if (mtra.mother_ID < 0){ // primary
      h2_prim_vertex->Fill(mtra.z, mtra.y);
      //h3_prim_vertex->Fill(mtra.z, mtra.x, mtra.y);
    }else{
      h2_sec_vertex->Fill(mtra.z, mtra.y);
      //h3_sec_vertex->Fill(mtra.z, mtra.x, mtra.y);
    }

    
    int iph = mtra.StsHits[0];
    CbmL1HitStore &ph = vHitStore[iph];
    
    h_sec_r->Fill(sqrt(fabs(ph.x*ph.x+ph.y*ph.y)));
    if( fabs( mtra.pz )>1.e-8 ){
      h_tx->Fill(mtra.px/mtra.pz);
      h_ty->Fill(mtra.py/mtra.pz);
    }

      // reconstructed tracks
    bool reco = (mtra.IsReconstructed());
    int nMCHits = mtra.NStations();
      
    if (reco){
      p_eff_all_vs_mom->Fill(momentum, 100.0);
      p_eff_all_vs_nhits->Fill(nMCHits, 100.0);
      p_eff_all_vs_pt->Fill(pt, 100.0);
      h_reco_MCmom->Fill(momentum);
      if (mtra.mother_ID < 0){ // primary
        p_eff_prim_vs_mom->Fill(momentum, 100.0);
        p_eff_prim_vs_nhits->Fill(nMCHits, 100.0);
        p_eff_prim_vs_pt->Fill(pt, 100.0);
        p_eff_prim_vs_theta->Fill(theta, 100.0);
      }else{
        p_eff_sec_vs_mom->Fill(momentum, 100.0);
        p_eff_sec_vs_nhits->Fill(nMCHits, 100.0);
      }
      if (mtra.mother_ID < 0){ // primary
        h_reco_mom_prim->Fill(momentum);
        h_reco_prim_MCmom->Fill(momentum);
        h_reco_nhits_prim->Fill(nSta);
        h2_reco_nhits_vs_mom_prim->Fill(momentum, nSta);
        h2_reco_fstation_vs_mom_prim->Fill(momentum, fh.iStation+1);
        if (lh.iStation >= fh.iStation)
          h2_reco_lstation_vs_fstation_prim->Fill(fh.iStation+1, lh.iStation+1);
      }else{
        h_reco_mom_sec->Fill(momentum);
        h_reco_sec_MCmom->Fill(momentum);
        h_reco_nhits_sec->Fill(nSta);
        if (momentum > 0.01){
          h2_reco_nhits_vs_mom_sec->Fill(momentum, nSta);
          h2_reco_fstation_vs_mom_sec->Fill(momentum, fh.iStation+1);
          if (lh.iStation >= fh.iStation)
            h2_reco_lstation_vs_fstation_sec->Fill(fh.iStation+1, lh.iStation+1);
        }
      }
    }else{
      h_notfound_mom->Fill(momentum);
      p_eff_all_vs_mom->Fill(momentum, 0.0);
      p_eff_all_vs_nhits->Fill(nMCHits, 0.0);
      p_eff_all_vs_pt->Fill(pt, 0.0);
      if (mtra.mother_ID < 0){ // primary
        p_eff_prim_vs_mom->Fill(momentum, 0.0);
        p_eff_prim_vs_nhits->Fill(nMCHits, 0.0);
        p_eff_prim_vs_pt->Fill(pt, 0.0);
        p_eff_prim_vs_theta->Fill(theta, 0.0);
      }else{
        p_eff_sec_vs_mom->Fill(momentum, 0.0);
        p_eff_sec_vs_nhits->Fill(nMCHits, 0.0);
      }
      if (mtra.mother_ID < 0){ // primary
        h_rest_mom_prim->Fill(momentum);
        h_rest_nhits_prim->Fill(nSta);
        h2_rest_nhits_vs_mom_prim->Fill(momentum, nSta);
        h2_rest_fstation_vs_mom_prim->Fill(momentum, fh.iStation+1);
        if (lh.iStation >= fh.iStation)
          h2_rest_lstation_vs_fstation_prim->Fill(fh.iStation+1, lh.iStation+1);
      }else{
        h_rest_mom_sec->Fill(momentum);
        h_rest_nhits_sec->Fill(nSta);
        if (momentum > 0.01){
          h2_rest_nhits_vs_mom_sec->Fill(momentum, nSta);
          h2_rest_fstation_vs_mom_sec->Fill(momentum, fh.iStation+1);
          if (lh.iStation >= fh.iStation)
            h2_rest_lstation_vs_fstation_sec->Fill(fh.iStation+1, lh.iStation+1);
        }
      }
    }

          // killed tracks. At least one hit of they belong to some recoTrack
    //bool killed = 0;
    if(!reco){
      h_notfound_nhits->Fill(nmchits);
      h_notfound_station->Fill(ph.iStation);
      h_notfound_r->Fill(sqrt(fabs(ph.x*ph.x+ph.y*ph.y)));

      if(mtra.Points.size() > 0){
        CbmL1MCPoint &pMC = vMCPoints[mtra.Points[0]];
        h_notfound_tx->Fill(pMC.px/pMC.pz);
        h_notfound_ty->Fill(pMC.py/pMC.pz);
      }

//      CbmL1HitStore &ph21 = vHitStore[mtra.StsHits[0]];
//      CbmL1HitStore &ph22 = vHitStore[mtra.StsHits[1]];

//      double z21 = algo->vStations[ph21.iStation].z[0];
//      double z22 = algo->vStations[ph22.iStation].z[0];
//      if( fabs(z22-z21)>1.e-4 ){
//        h_notfound_tx->Fill((ph22.x-ph21.x)/(z22-z21));
//        h_notfound_ty->Fill((ph22.y-ph21.y)/(z22-z21));
//      }

      //if( mtra.IsDisturbed() ) killed = 1;
    }



    
    if (( mtra.IsPrimary() )&&(mtra.z > 0)){ // D0
      h_reco_d0_mom->Fill(momentum);
      if (reco) p_eff_d0_vs_mom->Fill(momentum, 100.0);
      else      p_eff_d0_vs_mom->Fill(momentum,   0.0);
    }

  } // for mcTracks

  int NFakes = 0;
  for( unsigned int ih=0; ih<algo->vStsHits.size(); ih++){
    int iMC = vHitMCRef[ih]; // TODO2: adapt to linking
    if (iMC < 0) NFakes++;
  }

  h_reco_time->Fill(algo->CATime);
  h_reco_timeNtr->Fill(mc_total,algo->CATime);
  h_reco_timeNhit->Fill(algo->vStsHits.size(),algo->CATime);

  h_reco_fakeNtr->Fill(mc_total,NFakes);
  h_reco_fakeNhit->Fill(algo->vStsHits.size()-NFakes,NFakes);


  h_reg_MCmom->Scale(1.f/NEvents);
  h_acc_MCmom->Scale(1.f/NEvents);
  h_reco_MCmom->Scale(1.f/NEvents);
  h_ghost_Rmom->Scale(1.f/NEvents);
  h_reg_prim_MCmom->Scale(1.f/NEvents);
  h_acc_prim_MCmom->Scale(1.f/NEvents);
  h_reco_prim_MCmom->Scale(1.f/NEvents);
  h_reg_sec_MCmom->Scale(1.f/NEvents);
  h_acc_sec_MCmom->Scale(1.f/NEvents);
  h_reco_sec_MCmom->Scale(1.f/NEvents);
  
} // void CbmL1::HistoPerformance()


void CbmL1::TrackFitPerformance()
{
  const int Nh_fit = 12;
  static TH1F *h_fit[Nh_fit], *h_fitL[Nh_fit], *h_fitSV[Nh_fit], *h_fitPV[Nh_fit], *h_fit_chi2;

  static TH2F *PRes2D, *PRes2DPrim, *PRes2DSec;

  static bool first_call = 1;

  if ( first_call )
  {
    first_call = 0;

    TDirectory *currentDir = gDirectory;
    gDirectory = histodir;
    gDirectory->cd("L1");
    gDirectory->mkdir("Fit");
    gDirectory->cd("Fit");
    {
      PRes2D = new TH2F("PRes2D", "Resolution P vs P", 100, 0., 20.,100, -15., 15.);
      PRes2DPrim = new TH2F("PRes2DPrim", "Resolution P vs P", 100, 0., 20.,100, -15., 15.);
      PRes2DSec  = new TH2F("PRes2DSec",  "Resolution P vs P", 100, 0., 20.,100, -15., 15.);

      struct {
        const char *name;
        const char *title;
        Int_t n;
        Double_t l,r;
      } Table[Nh_fit]=
      {
        {"x",  "Residual X [#mum]",                   100, -45., 45.},
        {"y",  "Residual Y [#mum]",                   100, -450., 450.},
        {"tx", "Residual Tx [mrad]",                  100,   -2.,   2.},
        {"ty", "Residual Ty [mrad]",                  100,   -3.5,   3.5},
        {"P",  "Resolution P/Q [100%]",               100,   -0.1,  0.1 },
        {"px", "Pull X [residual/estimated_error]",   100,  -6.,  6.},
        {"py", "Pull Y [residual/estimated_error]",   100,  -6.,  6.},
        {"ptx","Pull Tx [residual/estimated_error]",  100,  -6.,  6.},
        {"pty","Pull Ty [residual/estimated_error]",  100,  -6.,  6.},
        {"pQP","Pull Q/P [residual/estimated_error]", 100,  -6.,  6.},
        {"QPreco","Reco Q/P ", 100,  -10.,  10.},
        {"QPmc","MC Q/P ", 100,  -10.,  10.}
      };

      struct Tab{
        const char *name;
        const char *title;
        Int_t n;
        Double_t l,r;
      };
      Tab TableVertex[Nh_fit]=
      {
        {"x",  "Residual X [cm]",                   2000, -1., 1.},
        {"y",  "Residual Y [cm]",                   2000, -1., 1.},
        {"tx", "Residual Tx [mrad]",                  100,   -2.,   2.},
        {"ty", "Residual Ty [mrad]",                  100,   -2.,   2.},
        {"P",  "Resolution P/Q [100%]",               100,   -0.1,  0.1 },
        {"px", "Pull X [residual/estimated_error]",   100,  -6.,  6.},
        {"py", "Pull Y [residual/estimated_error]",   100,  -6.,  6.},
        {"ptx","Pull Tx [residual/estimated_error]",  100,  -6.,  6.},
        {"pty","Pull Ty [residual/estimated_error]",  100,  -6.,  6.},
        {"pQP","Pull Q/P [residual/estimated_error]", 100,  -6.,  6.},
        {"QPreco","Reco Q/P ", 100,  -10.,  10.},
        {"QPmc","MC Q/P ", 100,  -10.,  10.}
      };
      
      for( int i=0; i<Nh_fit; i++ ){
        char n[225], t[255];
        sprintf(n,"fst_%s",Table[i].name);
        sprintf(t,"First point %s",Table[i].title);
        h_fit[i] = new TH1F(n,t, Table[i].n, Table[i].l, Table[i].r);
        sprintf(n,"lst_%s",Table[i].name);
        sprintf(t,"Last point %s",Table[i].title);
        h_fitL[i] = new TH1F(n,t, Table[i].n, Table[i].l, Table[i].r);
        sprintf(n,"svrt_%s",TableVertex[i].name);
        sprintf(t,"Secondary vertex point %s",TableVertex[i].title);
        h_fitSV[i] = new TH1F(n,t, TableVertex[i].n, TableVertex[i].l, TableVertex[i].r);
        sprintf(n,"pvrt_%s",TableVertex[i].name);
        sprintf(t,"Primary vertex point %s",TableVertex[i].title);
        h_fitPV[i] = new TH1F(n,t, TableVertex[i].n, TableVertex[i].l, TableVertex[i].r);
      }
      h_fit_chi2 = new TH1F("h_fit_chi2", "Chi2/NDF", 50, -0.5, 10.0);
    }
    gDirectory = currentDir;
  } // if first call
  
  for (vector<CbmL1Track>::iterator it = vRTracks.begin(); it != vRTracks.end(); ++it){

    if ( it->IsGhost() ) continue;
    { // first hit
      int iMC = vHitMCRef[it->StsHits.front()]; // TODO2: adapt to linking
      if (iMC < 0) continue;
      CbmL1MCPoint &mc = vMCPoints[iMC];
//      if( !( mc.pdg == -11 && mc.mother_ID == -1 ) ) continue; // electrons only
      h_fit[0]->Fill( (mc.x-it->T[0]) *1.e4);
      h_fit[1]->Fill( (mc.y-it->T[1]) *1.e4);
      h_fit[2]->Fill((mc.px/mc.pz-it->T[2])*1.e3);
      h_fit[3]->Fill((mc.py/mc.pz-it->T[3])*1.e3);
      h_fit[4]->Fill(it->T[4]/mc.q*mc.p-1);

      PRes2D->Fill( mc.p, (1./fabs(it->T[4]) - mc.p)/mc.p*100. );

      CbmL1MCTrack mcTrack = *(it->GetMCTracks()[0]);
      if(mcTrack.IsPrimary())
        PRes2DPrim->Fill( mc.p, (1./fabs(it->T[4]) - mc.p)/mc.p*100. );
      else
        PRes2DSec->Fill( mc.p, (1./fabs(it->T[4]) - mc.p)/mc.p*100. );

      if( finite(it->C[0]) && it->C[0]>0 )h_fit[5]->Fill( (mc.x-it->T[0])/sqrt(it->C[0]));
      if( finite(it->C[2]) && it->C[2]>0 )h_fit[6]->Fill( (mc.y-it->T[1])/sqrt(it->C[2]));
      if( finite(it->C[5]) && it->C[5]>0 )h_fit[7]->Fill( (mc.px/mc.pz-it->T[2])/sqrt(it->C[5]));
      if( finite(it->C[9]) && it->C[9]>0 )h_fit[8]->Fill( (mc.py/mc.pz-it->T[3])/sqrt(it->C[9]));
      if( finite(it->C[14]) && it->C[14]>0 )h_fit[9]->Fill( (mc.q/mc.p-it->T[4])/sqrt(it->C[14]));
      h_fit[10]->Fill(it->T[4]);
      h_fit[11]->Fill(mc.q/mc.p);
    }
    
    { // last hit
      int iMC = vHitMCRef[it->StsHits.back()]; // TODO2: adapt to linking
      if (iMC < 0) continue;
      CbmL1MCPoint &mc = vMCPoints[iMC];
      h_fitL[0]->Fill( (mc.x-it->TLast[0]) *1.e4);
      h_fitL[1]->Fill( (mc.y-it->TLast[1]) *1.e4);
      h_fitL[2]->Fill((mc.px/mc.pz-it->TLast[2])*1.e3);
      h_fitL[3]->Fill((mc.py/mc.pz-it->TLast[3])*1.e3);
      h_fitL[4]->Fill(it->T[4]/mc.q*mc.p-1);
      if( finite(it->CLast[0]) && it->CLast[0]>0 ) h_fitL[5]->Fill( (mc.x-it->TLast[0])/sqrt(it->CLast[0]));
      if( finite(it->CLast[2]) && it->CLast[2]>0 ) h_fitL[6]->Fill( (mc.y-it->TLast[1])/sqrt(it->CLast[2]));
      if( finite(it->CLast[5]) && it->CLast[5]>0 ) h_fitL[7]->Fill( (mc.px/mc.pz-it->TLast[2])/sqrt(it->CLast[5]));
      if( finite(it->CLast[9]) && it->CLast[9]>0 ) h_fitL[8]->Fill( (mc.py/mc.pz-it->TLast[3])/sqrt(it->CLast[9]));
      if( finite(it->CLast[14]) && it->CLast[14]>0 ) h_fitL[9]->Fill( (mc.q/mc.p-it->TLast[4])/sqrt(it->CLast[14]));
      h_fitL[10]->Fill(it->TLast[4]);
      h_fitL[11]->Fill(mc.q/mc.p);
    }

    { // vertex
      CbmL1MCTrack mc = *(it->GetMCTracks()[0]);
      L1TrackPar trPar(it->T,it->C);
      
      if (mc.mother_ID != -1){ // secondary

        {  // extrapolate to vertex
          L1FieldRegion fld _fvecalignment;
          L1FieldValue B[3] _fvecalignment;
          float z[3];
          for (int ih = 0; ih < 3; ih++){
            const int iMCP = mc.Points[ih];
            CbmL1MCPoint &mcP = vMCPoints[iMCP];
            L1Station &st = algo->vStations[mcP.iStation];
            z[ih] = st.z[0];
            st.fieldSlice.GetFieldValue( mcP.x, mcP.y, B[ih] );
          };
          fld.Set( B[0], z[0], B[1], z[1], B[2], z[2] );
          
          L1Extrapolate(trPar, mc.z, trPar.qp, fld);
            // add material
          const int fSta = vHitStore[it->StsHits[0]].iStation;
          const int dir = int((mc.z - algo->vStations[fSta].z[0])/fabs(mc.z - algo->vStations[fSta].z[0]));
  //         if (abs(mc.z - algo->vStations[fSta].z[0]) > 10.) continue; // can't extrapolate on large distance
          for (int iSta = fSta/*+dir*/; (iSta >= 0) && (iSta < NStation) && (dir*(mc.z - algo->vStations[iSta].z[0]) > 0); iSta += dir){
  //           cout << iSta << " " << dir << endl;
            L1AddMaterial( trPar, algo->vStations[iSta].materialInfo, trPar.qp );
            if (iSta+dir == NMvdStations-1) L1AddPipeMaterial( trPar, trPar.qp );
          }
        }
        if (mc.z != trPar.z[0]) continue;
  //       static int good = 0;
  //       static int bad = 0;
  //       if (mc.z != trPar.z[0]){
  //         bad++;
  //         continue;
  //       }
  //       else good++;
  //       cout << "bad\\good" << bad << " " << good << endl;
        
        // calculate pulls
        //h_fitSV[0]->Fill( (mc.x-trPar.x[0]) *1.e4);
        //h_fitSV[1]->Fill( (mc.y-trPar.y[0]) *1.e4);
        h_fitSV[0]->Fill( (mc.x-trPar.x[0]) );
        h_fitSV[1]->Fill( (mc.y-trPar.y[0]) );
        h_fitSV[2]->Fill((mc.px/mc.pz-trPar.tx[0])*1.e3);
        h_fitSV[3]->Fill((mc.py/mc.pz-trPar.ty[0])*1.e3);
        h_fitSV[4]->Fill(trPar.qp[0]/mc.q*mc.p-1);
        if( finite(trPar.C00[0]) && trPar.C00[0]>0 ) h_fitSV[5]->Fill( (mc.x-trPar.x[0])/sqrt(trPar.C00[0]));
        if( finite(trPar.C11[0]) && trPar.C11[0]>0 ) h_fitSV[6]->Fill( (mc.y-trPar.y[0])/sqrt(trPar.C11[0]));
        if( finite(trPar.C22[0]) && trPar.C22[0]>0 ) h_fitSV[7]->Fill( (mc.px/mc.pz-trPar.tx[0])/sqrt(trPar.C22[0]));
        if( finite(trPar.C33[0]) && trPar.C33[0]>0 ) h_fitSV[8]->Fill( (mc.py/mc.pz-trPar.ty[0])/sqrt(trPar.C33[0]));
        if( finite(trPar.C44[0]) && trPar.C44[0]>0 ) h_fitSV[9]->Fill( (mc.q/mc.p-trPar.qp[0])/sqrt(trPar.C44[0]));
        h_fitSV[10]->Fill(trPar.qp[0]);
        h_fitSV[11]->Fill(mc.q/mc.p);
      }
      else{ // primary
        if (vHitStore[it->StsHits[0]].iStation != 0) continue; // only mvd tracks

// #define L1EXTRAPOLATE
#ifdef L1EXTRAPOLATE
        {  // extrapolate to vertex
          L1FieldRegion fld _fvecalignment;
          L1FieldValue B[3], targB _fvecalignment;
          float z[3];
          for (int ih = 0; ih < 3; ih++){
            const int iMCP = mc.Points[ih];
            CbmL1MCPoint &mcP = vMCPoints[iMCP];
            L1Station &st = algo->vStations[mcP.iStation];
            z[ih] = st.z[0];
//             cout << mcP.z << endl;
            st.fieldSlice.GetFieldValue( mcP.x, mcP.y, B[ih] );
          };
//           cout << mc.x << " " << mc.y << " " << mc.z << endl;

//           targetFieldSlice->GetFieldValue( mc.x, mc.y, targB );
          targB = algo->vtxFieldValue;
          fld.Set( targB, 0., B[0], z[0], B[1], z[1] );

          L1Extrapolate(trPar, mc.z, trPar.qp, fld);
//           L1Extrapolate0(trPar, mc.z, fld);
            // add material
          const int fSta = vHitStore[it->StsHits[0]].iStation;
          
          const int dir = (mc.z - algo->vStations[fSta].z[0])/abs(mc.z - algo->vStations[fSta].z[0]);
  //         if (abs(mc.z - algo->vStations[fSta].z[0]) > 10.) continue; // can't extrapolate on large distance
          for (int iSta = fSta/*+dir*/; (iSta >= 0) && (iSta < NStation) && (dir*(mc.z - algo->vStations[iSta].z[0]) > 0); iSta += dir){
  //           cout << iSta << " " << dir << endl;
            L1AddMaterial( trPar, algo->vStations[iSta].materialInfo, trPar.qp );
            if (iSta+dir == NMvdStations-1) L1AddPipeMaterial( trPar, trPar.qp );
          }
        }
        if (mc.z != trPar.z[0]) continue;
  //       static int good = 0;
  //       static int bad = 0;
  //       if (mc.z != trPar.z[0]){
  //         bad++;
  //         continue;
  //       }
  //       else good++;
  //       cout << "bad\\good" << bad << " " << good << endl;

        // calculate pulls
        //h_fitPV[0]->Fill( (mc.x-trPar.x[0]) *1.e4);
        //h_fitPV[1]->Fill( (mc.y-trPar.y[0]) *1.e4);
        h_fitPV[0]->Fill( (mc.x-trPar.x[0]) );
        h_fitPV[1]->Fill( (mc.y-trPar.y[0]) );
        h_fitPV[2]->Fill((mc.px/mc.pz-trPar.tx[0])*1.e3);
        h_fitPV[3]->Fill((mc.py/mc.pz-trPar.ty[0])*1.e3);
        h_fitPV[4]->Fill(trPar.qp[0]/mc.q*mc.p-1);
        if( finite(trPar.C00[0]) && trPar.C00[0]>0 ) h_fitPV[5]->Fill( (mc.x-trPar.x[0])/sqrt(trPar.C00[0]));
        if( finite(trPar.C11[0]) && trPar.C11[0]>0 ) h_fitPV[6]->Fill( (mc.y-trPar.y[0])/sqrt(trPar.C11[0]));
        if( finite(trPar.C22[0]) && trPar.C22[0]>0 ) h_fitPV[7]->Fill( (mc.px/mc.pz-trPar.tx[0])/sqrt(trPar.C22[0]));
        if( finite(trPar.C33[0]) && trPar.C33[0]>0 ) h_fitPV[8]->Fill( (mc.py/mc.pz-trPar.ty[0])/sqrt(trPar.C33[0]));
        if( finite(trPar.C44[0]) && trPar.C44[0]>0 ) h_fitPV[9]->Fill( (mc.q/mc.p-trPar.qp[0])/sqrt(trPar.C44[0]));
        h_fitPV[10]->Fill(trPar.qp[0]);
        h_fitPV[11]->Fill(mc.q/mc.p);
#else
        FairTrackParam fTP;

        CbmKFMath::CopyTC2TrackParam( &fTP, it->T, it->C );

        CbmKFTrack kfTr;
        kfTr.SetTrackParam(fTP);

        kfTr.Extrapolate(mc.z);
        CbmL1Track it2;
        for (int ipar = 0; ipar < 6; ipar++) it2.T[ipar] = kfTr.GetTrack()[ipar];
        for (int ipar = 0; ipar < 15; ipar++) it2.C[ipar] = kfTr.GetCovMatrix()[ipar];


        // calculate pulls
//        h_fitPV[0]->Fill( (mc.x-it2.T[0]) *1.e4);
//        h_fitPV[1]->Fill( (mc.y-it2.T[1]) *1.e4);
        h_fitPV[0]->Fill( (mc.x-it2.T[0]) );
        h_fitPV[1]->Fill( (mc.y-it2.T[1]) );
        h_fitPV[2]->Fill((mc.px/mc.pz-it2.T[2])*1.e3);
        h_fitPV[3]->Fill((mc.py/mc.pz-it2.T[3])*1.e3);
        h_fitPV[4]->Fill(it2.T[4]/mc.q*mc.p-1);
        if( finite(it2.C[0]) && it2.C[0]>0 )h_fitPV[5]->Fill( (mc.x-it2.T[0])/sqrt(it2.C[0]));
        if( finite(it2.C[2]) && it2.C[2]>0 )h_fitPV[6]->Fill( (mc.y-it2.T[1])/sqrt(it2.C[2]));
        if( finite(it2.C[5]) && it2.C[5]>0 )h_fitPV[7]->Fill( (mc.px/mc.pz-it2.T[2])/sqrt(it2.C[5]));
        if( finite(it2.C[9]) && it2.C[9]>0 )h_fitPV[8]->Fill( (mc.py/mc.pz-it2.T[3])/sqrt(it2.C[9]));
        if( finite(it2.C[14]) && it2.C[14]>0 )h_fitPV[9]->Fill( (mc.q/mc.p-it2.T[4])/sqrt(it2.C[14]));
        h_fitPV[10]->Fill(it2.T[4]);
        h_fitPV[11]->Fill(mc.q/mc.p);
#endif // L1EXTRAPOLATE
      }
    }
    
    h_fit_chi2->Fill(it->chi2/it->NDF);
  }

} // void CbmL1::TrackFitPerformance()



void CbmL1::FieldApproxCheck()
{
  TDirectory *curr = gDirectory;
  TFile *currentFile = gFile;
  TFile* fout = new TFile("FieldApprox.root","RECREATE");
  fout->cd();

  FairField *MF = CbmKF::Instance()->GetMagneticField();
  for ( int ist = 0; ist<NStation; ist++ )
  {
    double z = 0;
    double Xmax=-100, Ymax=-100;
    if( ist<NMvdStations ){
      CbmKFTube &t = CbmKF::Instance()->vMvdMaterial[ist];
      z = t.z;
      Xmax = Ymax = t.R;
    }else{
      //AZ CbmStsStation *st = StsDigi.GetStation(ist - NMvdStations);
      CbmStsStation *st = StsDigi->GetStation(ist - NMvdStations);
      z = st->GetZ();

      double x,y;
      for(int isec = 0; isec < st->GetNSectors(); isec++)
      {
        CbmStsSector *sect = L1_DYNAMIC_CAST<CbmStsSector*>( st->GetSector(isec) );
        for(int isen = 0; isen < sect->GetNSensors(); isen++)
        {
          x = sect->GetSensor(isen)->GetX0() + sect->GetSensor(isen)->GetLx()/2.;
          y = sect->GetSensor(isen)->GetY0() + sect->GetSensor(isen)->GetLy()/2.;
          if(x>Xmax) Xmax = x;
          if(y>Ymax) Ymax = y;
        }
      }
      cout << "Station  "<<  ist << ",  Xmax  " << Xmax<<",  Ymax" << Ymax<<endl;
  
    } // if mvd


//    float step = 1.;

    int NbinsX = 100; //static_cast<int>(2*Xmax/step);
    int NbinsY = 100; //static_cast<int>(2*Ymax/step);
    float ddx = 2*Xmax/NbinsX;
    float ddy = 2*Ymax/NbinsY;

    TH2F *stB  = new TH2F(Form("station %i, dB", ist+1) ,Form("station %i, dB, z = %0.f cm", ist+1,z) , static_cast<int>(NbinsX+1),-(Xmax+ddx/2.),(Xmax+ddx/2.), static_cast<int>(NbinsY+1),-(Ymax+ddy/2.),(Ymax+ddy/2.));
    TH2F *stBx = new TH2F(Form("station %i, dBx", ist+1),Form("station %i, dBx, z = %0.f cm", ist+1,z), static_cast<int>(NbinsX+1),-(Xmax+ddx/2.),(Xmax+ddx/2.), static_cast<int>(NbinsY+1),-(Ymax+ddy/2.),(Ymax+ddy/2.));
    TH2F *stBy = new TH2F(Form("station %i, dBy", ist+1),Form("station %i, dBy, z = %0.f cm", ist+1,z), static_cast<int>(NbinsX+1),-(Xmax+ddx/2.),(Xmax+ddx/2.), static_cast<int>(NbinsY+1),-(Ymax+ddy/2.),(Ymax+ddy/2.));
    TH2F *stBz = new TH2F(Form("station %i, dBz", ist+1),Form("station %i, dBz, z = %0.f cm", ist+1,z), static_cast<int>(NbinsX+1),-(Xmax+ddx/2.),(Xmax+ddx/2.), static_cast<int>(NbinsY+1),-(Ymax+ddy/2.),(Ymax+ddy/2.));

    Double_t r[3],B[3];
    L1FieldSlice FSl;
    L1FieldValue B_L1;
    Double_t bbb, bbb_L1;

    const int M=5; // polinom order
    const int N=(M+1)*(M+2)/2;
    L1Station &st = algo->vStations[ist];
    for(int i=0; i<N; i++)
    {
      FSl.cx[i] = st.fieldSlice.cx[i][0];
      FSl.cy[i] = st.fieldSlice.cy[i][0];
      FSl.cz[i] = st.fieldSlice.cz[i][0];
    }

    Int_t i=1,j=1;

    double x,y;
    for(int ii = 1; ii <=NbinsX+1; ii++)
    {
      j=1;
      x = -Xmax+(ii-1)*ddx;
      for(int jj = 1; jj <=NbinsY+1; jj++)
      {
        y = -Ymax+(jj-1)*ddy;
        double rrr = sqrt(fabs(x*x/Xmax/Xmax+y/Ymax*y/Ymax));
        if(rrr>1. )
        {
          j++;
          continue;
        }
        r[2] = z; r[0] = x; r[1] = y;
        MF->GetFieldValue( r, B );
        bbb = sqrt(B[0]*B[0]+B[1]*B[1]+B[2]*B[2]);

        FSl.GetFieldValue(x,y,B_L1);
        bbb_L1 = sqrt(B_L1.x[0]*B_L1.x[0] + B_L1.y[0]*B_L1.y[0] + B_L1.z[0]*B_L1.z[0]);

        stB  -> SetBinContent(ii,jj,(bbb - bbb_L1));
        stBx -> SetBinContent(ii,jj,(B[0] - B_L1.x[0]));
        stBy -> SetBinContent(ii,jj,(B[1] - B_L1.y[0]));
        stBz -> SetBinContent(ii,jj,(B[2] - B_L1.z[0]));
        j++;
      }
      i++;
    }

    stB   ->GetXaxis()->SetTitle("X, cm");
    stB   ->GetYaxis()->SetTitle("Y, cm");
    stB   ->GetXaxis()->SetTitleOffset(1);
    stB   ->GetYaxis()->SetTitleOffset(1);
    stB   ->GetZaxis()->SetTitle("B_map - B_L1, kGauss");
    stB   ->GetZaxis()->SetTitleOffset(1.3);

    stBx  ->GetXaxis()->SetTitle("X, cm");
    stBx  ->GetYaxis()->SetTitle("Y, cm");
    stBx  ->GetXaxis()->SetTitleOffset(1);
    stBx  ->GetYaxis()->SetTitleOffset(1);
    stBx  ->GetZaxis()->SetTitle("Bx_map - Bx_L1, kGauss");
    stBx  ->GetZaxis()->SetTitleOffset(1.3);

    stBy  ->GetXaxis()->SetTitle("X, cm");
    stBy  ->GetYaxis()->SetTitle("Y, cm");
    stBy  ->GetXaxis()->SetTitleOffset(1);
    stBy  ->GetYaxis()->SetTitleOffset(1);
    stBy  ->GetZaxis()->SetTitle("By_map - By_L1, kGauss");
    stBy  ->GetZaxis()->SetTitleOffset(1.3);

    stBz  ->GetXaxis()->SetTitle("X, cm");
    stBz  ->GetYaxis()->SetTitle("Y, cm");
    stBz  ->GetXaxis()->SetTitleOffset(1);
    stBz  ->GetYaxis()->SetTitleOffset(1);
    stBz  ->GetZaxis()->SetTitle("Bz_map - Bz_L1, kGauss");
    stBz  ->GetZaxis()->SetTitleOffset(1.3);

    stB  -> Write();
    stBx -> Write();
    stBy -> Write();
    stBz -> Write();
    
  } // for ista

  fout->Close();
  fout->Delete();
  gFile = currentFile;
  gDirectory = curr;
} // void CbmL1::FieldApproxCheck()

#include "TMath.h"
void CbmL1::FieldIntegralCheck()
{
  TDirectory *curr = gDirectory;
  TFile *currentFile = gFile;
  TFile* fout = new TFile("FieldApprox.root","RECREATE");
  fout->cd();

  FairField *MF = CbmKF::Instance()->GetMagneticField();

  int nPointsZ = 1000;
  int nPointsPhi = 100;
  int nPointsTheta = 100;
  double startZ=0, endZ=100.;
  double startPhi=0, endPhi=2*TMath::Pi();
  double startTheta=-30./180.*TMath::Pi(), endTheta=30./180.*TMath::Pi();

  double DZ=endZ-startZ;
  double DP=endPhi-startPhi;
  double DT=endTheta-startTheta;

  float ddp = endPhi/nPointsPhi;
  float ddt = 2*endTheta/nPointsTheta;

  TH2F *hSb  = new TH2F("Field Integral", "Field Integral" , static_cast<int>(nPointsPhi),-(startPhi+ddp/2.),(endPhi+ddp/2.), static_cast<int>(nPointsTheta),(startTheta-ddt/2.),(endTheta+ddt/2.));

  for(int iP=0; iP<nPointsPhi; iP++)
  {
    double phi=startPhi+iP*DP/nPointsPhi;
    for(int iT=0; iT<nPointsTheta; iT++)
    {
      double theta=startTheta+iT*DT/nPointsTheta;

      double Sb=0;
      for(int iZ=1; iZ<nPointsZ; iZ++)
      {
        double z = startZ+ DZ*iZ/nPointsZ;
        double x = z*TMath::Tan(theta)*TMath::Cos(phi);
        double y = z*TMath::Tan(theta)*TMath::Sin(phi);
        double r[3] = {x,y,z};
        double b[3];
        MF->GetFieldValue( r, b );
        double B=sqrt(b[0]*b[0]+b[1]*b[1]+b[2]*b[2]);
        Sb += B*DZ/nPointsZ/100./10.;
      }
      hSb->SetBinContent(iP+1,iT+1,Sb);
    }
  }

  hSb  ->GetXaxis()->SetTitle("#phi [rad]");
  hSb  ->GetYaxis()->SetTitle("#theta [rad]");
  hSb  ->GetXaxis()->SetTitleOffset(1);
  hSb  ->GetYaxis()->SetTitleOffset(1);
  hSb  ->GetZaxis()->SetTitle("Field Integral [T#dotm]");
  hSb  ->GetZaxis()->SetTitleOffset(1.3);

  hSb -> Write();


  fout->Close();
  fout->Delete();
  gFile = currentFile;
  gDirectory = curr;
} // void CbmL1::FieldApproxCheck()

void CbmL1::InputPerformance()
{
  static TH1I *nStripFHits, *nStripBHits, *nStripFMC, *nStripBMC;

  static TH1F *resX, *resY/*, *pullZ*/;
  static TH1F *pullX, *pullY/*, *pullZ*/;
  
  static bool first_call = 1;
  if ( first_call ){
    first_call = 0;
    
    TDirectory *currentDir = gDirectory;
    gDirectory = histodir;
    gDirectory->cd("L1");
    gDirectory->mkdir("Input");
    gDirectory->cd("Input");
    
    nStripFHits = new TH1I("nHits_f", "NHits On Front Strip", 10, 0, 10);
    nStripBHits = new TH1I("nHits_b", "NHits On Back Strip", 10, 0, 10);
    nStripFMC = new TH1I("nMC_f", "N MC Points On Front Strip", 10, 0, 10);
    nStripBMC = new TH1I("nMC_b", "N MC Points On Back Strip", 10, 0, 10);

    pullX = new TH1F("Px", "Pull x", 50, -5, 5);
    pullY = new TH1F("Py", "Pull y", 50, -5, 5);

    resX = new TH1F("x", "Residual x", 50, -50, 50);
    resY = new TH1F("y", "Residual y", 50, -500, 500);
    TH1* histo;
    histo = resX;
    histo->GetXaxis()->SetTitle("Residual x, um");
    histo = resY;
    histo->GetXaxis()->SetTitle("Residual y, um");
    
    gDirectory = currentDir;
  } // first_call

  std::map<unsigned int, unsigned int> stripFToNHitMap,stripBToNHitMap;
  std::map<unsigned int, unsigned int> stripFToNMCMap,stripBToNMCMap;

  map<unsigned int, unsigned int>::iterator it;
  Int_t nMC = -1;
  if( listStsPts ){
    nMC = listStsPts->GetEntries();
  }
  if( listStsHits ){
    for (unsigned int iH=0; iH < vStsHits.size(); iH++ ){
      const CbmL1StsHit &h = vStsHits[iH];

      if (h.extIndex < 0) continue; // mvd hit
      const CbmStsHit *sh = L1_DYNAMIC_CAST<CbmStsHit*>( listStsHits->At(h.extIndex) );
              // strip - MC correspondence
      int iStripF = sh->GetDigi(0);
      int iStripB = sh->GetDigi(1);
      if ( iStripF >= 0 ) stripFToNHitMap[iStripF]++;
      if ( iStripB >= 0 ) stripBToNHitMap[iStripB]++;
      
      if ( h.mcPointIds.size() == 0 ) continue; // fake
      int iMC = vMCPoints[h.mcPointIds[0]].pointId;  // TODO: adapt to linking
      if( !( iMC>=0 && iMC < nMC) )
        continue;
      
      if ( iStripF >= 0 ) stripFToNMCMap[iStripF]++;
      if ( iStripB >= 0 ) stripBToNMCMap[iStripB]++;
        
        // hit pulls and residuals

      TVector3 hitPos, mcPos, hitErr;
      sh->Position(hitPos);
      sh->PositionError(hitErr);
      CbmStsPoint *pt = 0;
      pt = L1_DYNAMIC_CAST<CbmStsPoint*>( listStsPts->At(iMC) );
      if ( !pt ){
//         cout << " No MC points! " << "iMC=" << iMC << endl;
        continue;
      }
//       pt->Position(mcPos); // this is wrong! 
      mcPos.SetX( pt->GetX( hitPos.Z() ) );
      mcPos.SetY( pt->GetY( hitPos.Z() ) );
      mcPos.SetZ( hitPos.Z() );

#if 0 // standard errors
      if (hitErr.X() != 0) pullX->Fill( (hitPos.X() - mcPos.X()) / hitErr.X() ); // standard errors
      if (hitErr.Y() != 0) pullY->Fill( (hitPos.Y() - mcPos.Y()) / hitErr.Y() );
#elif 1 // qa errors
      if (hitErr.X() != 0) pullX->Fill( (hitPos.X() - mcPos.X()) / sh->GetDx() ); // qa errors
      if (hitErr.Y() != 0) pullY->Fill( (hitPos.Y() - mcPos.Y()) / sh->GetDy() );
#else // errors used in TF
      if (hitErr.X() != 0) pullX->Fill( (hitPos.X() - mcPos.X()) / sqrt(algo->vStations[NMvdStations].XYInfo.C00[0]) ); 
      if (hitErr.Y() != 0) pullY->Fill( (hitPos.Y() - mcPos.Y()) / sqrt(algo->vStations[NMvdStations].XYInfo.C11[0]) );
#endif
      
      resX->Fill((hitPos.X() - mcPos.X())*10*1000);
      resY->Fill((hitPos.Y() - mcPos.Y())*10*1000);
      
    }
  } // sts
  if( listMvdHits ){
    Int_t nEnt = listMvdHits->GetEntries();
//     Int_t nMC = listStsPts->GetEntries();
    for (int j=0; j < nEnt; j++ ){
      CbmMvdHit *sh = L1_DYNAMIC_CAST<CbmMvdHit*>( listMvdHits->At(j) );
      CbmMvdHitMatch *hm = L1_DYNAMIC_CAST<CbmMvdHitMatch*>( listMvdHitMatches->At(j) );

//       int iMC = sh->GetRefIndex();
      int iMC = hm->GetPointId();

        // hit pulls and residuals

      TVector3 hitPos, mcPos, hitErr;
      sh->Position(hitPos);
      sh->PositionError(hitErr);
      CbmMvdPoint *pt = 0;
      if( iMC>=0 && iMC<nMC) pt = L1_DYNAMIC_CAST<CbmMvdPoint*>( listMvdPts->At(iMC) );
      if ( !pt ){
//         cout << " No MC points! " << "iMC=" << iMC << endl;
        continue;
      }
//       pt->Position(mcPos); // this is wrong! 
      mcPos.SetX( ( pt->GetX() + pt->GetXOut() )/2. );
      mcPos.SetY( ( pt->GetY() + pt->GetYOut() )/2 );
      mcPos.SetZ( hitPos.Z() );
      
//       if (hitErr.X() != 0) pullX->Fill( (hitPos.X() - mcPos.X()) / hitErr.X() ); // standard errors
//       if (hitErr.Y() != 0) pullY->Fill( (hitPos.Y() - mcPos.Y()) / hitErr.Y() );
//       if (hitErr.X() != 0) pullX->Fill( (hitPos.X() - mcPos.X()) / sh->GetDx() ); // qa errors
//       if (hitErr.Y() != 0) pullY->Fill( (hitPos.Y() - mcPos.Y()) / sh->GetDy() );
      if (hitErr.X() != 0) pullX->Fill( (hitPos.X() - mcPos.X()) / sqrt(algo->vStations[0].XYInfo.C00[0]) );  // errors used in TF
      if (hitErr.Y() != 0) pullY->Fill( (hitPos.Y() - mcPos.Y()) / sqrt(algo->vStations[0].XYInfo.C11[0]) );

      resX->Fill((hitPos.X() - mcPos.X())*10*1000);
      resY->Fill((hitPos.Y() - mcPos.Y())*10*1000);
      
    }
  } // mvd


  
  for (it = stripFToNHitMap.begin(); it != stripFToNHitMap.end(); it++){
    nStripFHits->Fill(it->second);
  }
  for (it = stripBToNHitMap.begin(); it != stripBToNHitMap.end(); it++){
    nStripBHits->Fill(it->second);
  }
  for (it = stripFToNMCMap.begin(); it != stripFToNMCMap.end(); it++){
    nStripFMC->Fill(it->second);
  }
  for (it = stripBToNMCMap.begin(); it != stripBToNMCMap.end(); it++){
    nStripBMC->Fill(it->second);
  }

    // strips   Not ended
//   if( listCbmStsDigi ){
//     Int_t nEnt = listCbmStsDigi->GetEntries();
//     for (int j=0; j < nEnt; j++ ){
//       CbmStsDigi *digi = (CbmStsDigi*) listCbmStsDigi->At(j);
// //  = sh->GetNLinks(0);
//         // find position of mc point
//       FairLink link = digi->GetLink(0);
//       int iMCPoint = link.GetIndex();
//       CbmStsPoint *mcPoint = (CbmStsPoint*) listStsPts->At(iMCPoint);
//       TVector3 mcPos;
//       if (digi->GetSide() == 0)
//         mcPoint->PositionIn(mcPos);
//       else
//         mcPoint->PositionOut(mcPos);
// 
//       CbmStsStation *sta = StsDigi.GetStation(digi->GetStationNr());
//       CbmStsSector* sector = sta->GetSector(digi->GetSectorNr());
//       digi->GetChannelNr();
//       
//     }
//   } // listCbmStsDigi
  
  
} // void CbmL1::InputPerformance()

