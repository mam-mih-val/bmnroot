// @(#)bmnroot/alignment:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2018-11-02

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  BmnInnTrackerAlign                                                        //
//                                                                            //
// Interface to get align. corrections for inn. tracker                       //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef BMNINNTRACKALIGN_H
#define BMNINNTRACKALIGN_H 1

#include <TVector3.h>
#include <TMath.h>
#include <TNamed.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TSystem.h>

#include <UniDbDetectorParameter.h>
#include <UniDbRun.h>

#include "BmnGemAlignCorrections.h"
#include "BmnSiliconAlignCorrections.h"
#include <../silicon/BmnSiliconStationSet.h>
#include <../gem/BmnGemStripStationSet.h>

using namespace std;
using namespace TMath;

class BmnInnTrackerAlign : public TNamed {
public:

    BmnInnTrackerAlign() {};
    BmnInnTrackerAlign(Int_t, Int_t, TString);
   
    virtual ~BmnInnTrackerAlign();
   
    inline Double_t*** GetGemCorrs() {
        return fCorrsGem;
    }
    
    inline Double_t*** GetSiliconCorrs() {
        return fCorrsSil;
    }
    
    inline Double_t GetLorentzCorrs(Double_t By, Int_t station) {
        return fLorCorrs[station][0] + fLorCorrs[station][1] * By + fLorCorrs[station][2] * By * By;
    }
    
    void Print();

private: 
    TString fFilename;
        
    TString fBranchGemCorrs;
    TString fBranchSilCorrs;
       
    BmnGemStripStationSet* fDetectorGEM; // GEM-geometry
    BmnSiliconStationSet* fDetectorSI; // SI-geometry
    
    Double_t*** GetGemCorrs(TFile*);
    Double_t*** fCorrsGem;
    Double_t*** GetSiliconCorrs(TFile*);
    Double_t*** fCorrsSil;
    
    Double_t** fLorCorrs;
    
    ClassDef(BmnInnTrackerAlign, 1)
};

#endif
