// -------------------------------------------------------------------------
// -----                Mpd3fdGenerator source file                  -----
// -----                Created 24/06/04  by V. Friese                 -----
// -------------------------------------------------------------------------
#include "Mpd3fdGenerator.h"

#include "FairPrimaryGenerator.h"
#include "FairMCEventHeader.h"
#include "constants.h"

#include "TMCProcess.h"
#include "TObjArray.h"
#include "TPDGCode.h"
#include "TParticle.h"
#include "TRandom.h"
#include "TString.h"
#include "TVirtualMCStack.h"
#include "TLorentzVector.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TMath.h"
#include "TString.h"

#include <iostream>
#include <cstring>

#include <stdio.h>

using namespace std;
using namespace TMath;

// -----   Default constructor   ------------------------------------------

Mpd3fdGenerator::Mpd3fdGenerator()
: FairGenerator(),
fInputFile(NULL),
fFileName("") {
}
// ------------------------------------------------------------------------



// -----   Standard constructor   -----------------------------------------

Mpd3fdGenerator::Mpd3fdGenerator(TString fileName)
: FairGenerator(),
fInputFile(NULL),
fFileName(fileName) {
    //  fFileName = fileName;
    cout << "-I Mpd3fdGenerator: Opening input file " << fFileName << endl;
    fInputFile = new TFile(fFileName.Data());
    if (!fInputFile) {
        Fatal("Mpd3fdGenerator", "Cannot open input file.");
        exit(1);
    }
    fDstTree = new TChain("out");
    fDstTree->Add(fFileName);

    //    // Activate branches
    fDstTree->SetBranchAddress("px", fPx);
    fDstTree->SetBranchAddress("py", fPy);
    fDstTree->SetBranchAddress("pz", fPz);
    fDstTree->SetBranchAddress("x", fX);
    fDstTree->SetBranchAddress("y", fY);
    fDstTree->SetBranchAddress("z", fZ);
    fDstTree->SetBranchAddress("E", fE);
    fDstTree->SetBranchAddress("npart", &fNpart);
    fDstTree->SetBranchAddress("id", fPID);
    
    fEventNumber = 0;
}
// ------------------------------------------------------------------------



// -----   Destructor   ---------------------------------------------------

Mpd3fdGenerator::~Mpd3fdGenerator() {
    delete fInputFile;
    delete fDstTree;
}
// ------------------------------------------------------------------------



// -----   Public method ReadEvent   --------------------------------------

Bool_t Mpd3fdGenerator::ReadEvent(FairPrimaryGenerator* primGen) {

    // ---> Check for input file
    if (!fInputFile) {
        cout << "-E Mpd3fdGenerator: Input file not open! " << endl;
        return kFALSE;
    }

    // ---> Check for primary generator
    if (!primGen) {
        cout << "-E- Mpd3fdGenerator::ReadEvent: "
                << "No PrimaryGenerator!" << endl;
        return kFALSE;
    }
    
    fDstTree->GetEntry(fEventNumber);
    
    //Int_t events = fDstTree->GetEntries();

    // ---> Define event variables to be read from file
    Int_t evnr = 0, aProj = 0, zProj = 0, aTarg = 0, zTarg = 0;

    Char_t nucl1;
    Char_t nucl2;
    Int_t energ;
    TString name;

    if (fFileName.Contains("Au")) {
        aProj = 197;
        zProj = 79;
        aTarg = 197;
        zTarg = 79;
        sscanf(fFileName.Data(), "Au_%d_", &energ);
    }
    
    cout << fFileName << " " << energ << endl;

    Float_t b = 0.0; //TMP!

    // ---> Calculate beta and gamma for Lorentztransformation
    TDatabasePDG* pdgDB = TDatabasePDG::Instance();
    TParticlePDG* kProton = pdgDB->GetParticle(2212);
    Double_t kProtonMass = kProton->Mass();

    Double_t eBeam = energ + kProtonMass;
    Double_t pBeam = TMath::Sqrt(eBeam * eBeam - kProtonMass * kProtonMass);
    Double_t betaCM = pBeam / (eBeam + kProtonMass);
    Double_t gammaCM = TMath::Sqrt(1. / (1. - betaCM * betaCM));

    cout << "-I Mpd3fdGenerator: Event " << fEventNumber << ",  b = " << b
            << " fm,  multiplicity " << fNpart << ", Elab: " << energ << endl;

    // Set event id and impact parameter in MCEvent if not yet done
    FairMCEventHeader* event = primGen->GetEvent();
    if (event && (!event->IsSet())) {
        event->SetEventID(evnr);
        event->SetB(b);
        event->MarkSet(kTRUE);
    }

    // ---> Loop over tracks in the current event
    for (int itrack = 0; itrack < fNpart; itrack++) {

        // Lorentztransformation to lab
//        Double_t px = fPx[itrack];
//        Double_t py = fPy[itrack];
//        Double_t pz = fPz[itrack];
//        Double_t e = fE[itrack];
//        Double_t mass = Sqrt(e * e - px * px - py * py - pz * pz);
//        if (gCoordinateSystem == sysLaboratory)
//            pz = gammaCM * (pz + betaCM * e);
//        Double_t ee = sqrt(mass * mass + px * px + py * py + pz * pz);

        // Give track to PrimaryGenerator
        primGen->AddTrack(fPID[itrack], fPx[itrack], fPy[itrack], fPz[itrack], fX[itrack], fY[itrack], fZ[itrack]);

    }
    fEventNumber++;

    return kTRUE;
}
// ------------------------------------------------------------------------


ClassImp(Mpd3fdGenerator);

