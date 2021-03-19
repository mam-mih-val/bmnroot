// -------------------------------------------------------------------------
// -----                MpdDCMSMMGenerator header file                  -----
// -----          Created 27-AUG-2019  by Igor Rufanov           -----
// -------------------------------------------------------------------------
// The MpdDCMSMMGenerator reads the CAS-SMM-evt.out file from DCM-SMM  
// ( Dubna Cascade Model + Statistical Multifragmentation Model).
// PHYS REV C 95, 014902 (2017) & Phys Rep 257 (1995) 133.
// The code of the model was provided by Alexander Botvina.

#define GZIP_SUPPORT // version with gz support

#ifndef MPDDCMSMMGENERATOR_H
#define MPDDCMSMMGENERATOR_H

#include "FairGenerator.h"

#include <TFile.h>
#include <TH2D.h>
#include <TH1D.h>

#ifdef GZIP_SUPPORT
#ifndef __CINT__
#include <zlib.h>
#endif
#endif

class MpdDCMSMMGenerator : public FairGenerator
{
  public:
    /** Default constructor without arguments should not be used. **/
    MpdDCMSMMGenerator();
    /** Standard constructor. **/
    MpdDCMSMMGenerator(const char* fileName);
    /** Destructor. **/
    ~MpdDCMSMMGenerator();

    Bool_t ReadEvent(FairPrimaryGenerator* primGen);
    Bool_t SkipEvents(Int_t count);

    Int_t FindPDGCodeParticipant( Int_t A, Int_t S, Int_t Z, Float_t mass, Double_t &massFactor);    
    Int_t FindPDGCodeSpectator( Int_t N, Int_t Z, Int_t &dN);
    
    Int_t RegisterIons( void);

  private:
#ifdef GZIP_SUPPORT
    #ifndef __CINT__
    gzFile fInputFile;                    //!  Input file
    #endif
#else
    FILE* fInputFile;                     //!  Input file
#endif

    const Char_t* fFileName;              //!  Input file name

    TString fBoostType; // "None" MPD, "CmsFixed" BMN, "CmsFixedInverted" SRC, "FixedFixedInverted" SRC.
    Bool_t fUseLorentzBoost;
    Double_t fBoostBeta;
    Double_t fBoostGamma;

    Bool_t fSpectatorsON; // includes spectators (with heavy ions) into MC
    static const Int_t fZMax=82; // maximal charge of ions in MC
    Int_t fN1[fZMax+1], fN2[fZMax+1]; // region of barion number for given Z for registered ions
    static const Int_t fBMax=208; // maximal baryon charge of ions in MC
    Int_t fZ1[fBMax+1], fZ2[fBMax+1]; // region of Z for given B for registered ions
    
    Int_t fInputFileVersion; // 0 - old version, 1 - current version

    MpdDCMSMMGenerator(const MpdDCMSMMGenerator&);
    MpdDCMSMMGenerator& operator=(const MpdDCMSMMGenerator&);

    ClassDef(MpdDCMSMMGenerator,1);
};

#endif
