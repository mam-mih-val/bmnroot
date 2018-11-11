// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-06-20

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnParticlePair                                                            //
//                                                                            //
//  A class to consider selected pair of particles                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "BmnParticlePair.h"

BmnParticlePair::BmnParticlePair(Short_t key1):
fV0XZ(0.), fV0YZ(0.),
fDCA1(0.),
fDCA2(0.),
fDCA12X(0.), fDCA12Y(0.),
fPathX(0.), fPathY(0.),
fInvMassX(0.), fInvMassY(0.), 
fMomPart1(0.), fMomPart2(0.),
fEtaPart1(0.), fEtaPart2(0.), 
fAlphaX(0.), fPtPodolX(0.), 
fAlphaY(0.), fPtPodolY(0.),
fKey1(key1),
fRecoTrackIdPart1(-1), fRecoTrackIdPart2(-1),
fMCTrackIdPart1(-1), fMCTrackIdPart2(-1)
{
    fMCMomPart1.SetXYZ( 0., 0., 0.);
    fMCMomPart2.SetXYZ( 0., 0., 0.);
    fRecoMomPart1.SetXYZ( 0., 0., 0.);
    fRecoMomPart2.SetXYZ( 0., 0., 0.);
}

// Setters

Short_t BmnParticlePair::GetKey1() {
    return fKey1;
}

void BmnParticlePair::SetMCMomPart1(Double_t px, Double_t py, Double_t pz) {
    switch (fKey1) {
        case 'a':
            fMCMomPart1.SetXYZ(px, py, pz);
            break;
        case 'b':
            fMCMomPart1.SetXYZ(px, py, pz);  
            break;
        case 'c':   
            Fatal("SetMCMomPart1", "Trying to fill MC Mom in pure rec mode");
        }      
}

void BmnParticlePair::SetMCMomPart2(Double_t px, Double_t py, Double_t pz) {
    switch (fKey1) {
        case 'a':
            fMCMomPart2.SetXYZ(px, py, pz);
            break;
        case 'b':
            fMCMomPart2.SetXYZ(px, py, pz);  
            break;
        case 'c':   
            Fatal("SetMCMomPart1", "Trying to fill MC Mom in pure rec mode");
        }      
}

// Getters

TVector3 BmnParticlePair::GetMCMomPart1() {
     switch (fKey1) {
        case 'a':
            return fMCMomPart1;
        case 'b':
            return fMCMomPart1;
        case 'c':   
            Fatal("GetMCMomPart1", "Trying to access MC Mom in pure rec mode");
        }    
}

TVector3 BmnParticlePair::GetMCMomPart2() {
     switch (fKey1) {
        case 'a':
            return fMCMomPart2;
        case 'b':
            return fMCMomPart2;
        case 'c':   
            Fatal("GetMCMomPart1", "Trying to access MC Mom in pure rec mode");
        }    
}

Int_t BmnParticlePair::GetMCTrackIdPart1() {
    switch (fKey1) {
        case 'a':
            return fMCTrackIdPart1;
        case 'b':
            return fMCTrackIdPart1;
        case 'c':   
            Fatal("GetMCTrackIdPart1", "Trying to access MC track Id in pure rec mode");
        }
}


Int_t BmnParticlePair::GetMCTrackIdPart2() {
    switch (fKey1) {
        case 'a':
            return fMCTrackIdPart2;
        case 'b':
            return fMCTrackIdPart2;
        case 'c':   
            Fatal("GetMCTrackIdPart2", "Trying to access MC track Id in pure rec mode");
        }
}

Int_t BmnParticlePair::GetRecoTrackIdPart1() {
    switch (fKey1) {
        case 'a':
            Fatal("GetRecoTrackIdPart1", "Trying to access reco track Id in pure MC mode");
        case 'b':
            return fRecoTrackIdPart1;
        case 'c':  
            return fRecoTrackIdPart1;

        }
}

Int_t BmnParticlePair::GetRecoTrackIdPart2() {
    switch (fKey1) {
        case 'a':
            Fatal("GetRecoTrackIdPart2", "Trying to access reco track Id in pure MC mode");
        case 'b':
            return fRecoTrackIdPart2;
            break;
        case 'c':   
            return fRecoTrackIdPart2;
            break;
        }
}



void BmnParticlePair::SetMCTrackIdPart1(Int_t id) {
    switch (fKey1) {
        case 'a':
            fMCTrackIdPart1 = id;
            break;
        case 'b':
            fMCTrackIdPart1 = id;
            break;
        case 'c':   
            Fatal("SetMCTrackIdPart1", "Trying to set MC Mom in pure rec mode");
        }
}


void BmnParticlePair::SetMCTrackIdPart2(Int_t id) {
    switch (fKey1) {
        case 'a':
            fMCTrackIdPart2 = id;
            break;
        case 'b':
            fMCTrackIdPart2 = id;
            break;
        case 'c':   
            Fatal("SetMCTrackIdPart2", "Trying to set MC Mom in pure rec mode");
        }
}

void BmnParticlePair::SetRecoTrackIdPart1(Int_t id) {
    switch (fKey1) {
        case 'a':
            Fatal("SetRecoTrackIdPart1", "Trying to set reco track Id in pure MC mode");
        case 'b':
            fRecoTrackIdPart1 = id;
        case 'c':   
            fRecoTrackIdPart1 = id;
        }
}


void BmnParticlePair::SetRecoTrackIdPart2(Int_t id) {
    switch (fKey1) {
        case 'a':
            Fatal("SetRecoTrackIdPart2", "Trying to set reco track Id in pure MC mode");
        case 'b':
            fRecoTrackIdPart2 = id;
        case 'c': 
            fRecoTrackIdPart2 = id;
        }
}

BmnParticlePair::~BmnParticlePair() {

}