// -------------------------------------------------------------------------
// -----                MpdUrqmdGenerator source file                  -----
// -----                Created 24/06/04  by V. Friese                 -----
// -------------------------------------------------------------------------
#include "MpdUrqmdGenerator.h"

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


#include <iostream>
#include <cstring>

#include <stdio.h>

using std::cout;
using std::endl;

//const Double_t kProtonMass = 0.938271998;


// -----   Default constructor   ------------------------------------------

MpdUrqmdGenerator::MpdUrqmdGenerator()
: FairGenerator(),
fInputFile(NULL),
fParticleTable(),
fFileName(NULL) {
}
// ------------------------------------------------------------------------



// -----   Standard constructor   -----------------------------------------

MpdUrqmdGenerator::MpdUrqmdGenerator(const char* fileName)
: FairGenerator(),
fInputFile(NULL),
fParticleTable(),
fFileName(fileName) {
    //  fFileName = fileName;
    cout << "-I MpdUrqmdGenerator: Opening input file " << fFileName << endl;
    fInputFile = gzopen(fFileName, "rb");
    if (!fInputFile) {
        Fatal("MpdUrqmdGenerator", "Cannot open input file.");
        exit(1);
    }
    ReadConversionTable();
}
// ------------------------------------------------------------------------



// -----   Destructor   ---------------------------------------------------

MpdUrqmdGenerator::~MpdUrqmdGenerator() {
    //  cout<<"Enter Destructor of MpdUrqmdGenerator"<<endl;
    if (fInputFile) {
        gzclose(fInputFile);
        fInputFile = NULL;
    }
    fParticleTable.clear();
    //  cout<<"Leave Destructor of MpdUrqmdGenerator"<<endl;
}
// ------------------------------------------------------------------------



// -----   Public method ReadEvent   --------------------------------------

Bool_t MpdUrqmdGenerator::ReadEvent(FairPrimaryGenerator* primGen) {

    // ---> Check for input file
    if (!fInputFile) {
        cout << "-E MpdUrqmdGenerator: Input file not open! " << endl;
        return kFALSE;
    }

    // ---> Check for primary generator
    if (!primGen) {
        cout << "-E- MpdUrqmdGenerator::ReadEvent: "
                << "No PrimaryGenerator!" << endl;
        return kFALSE;
    }

    // ---> Define event variables to be read from file
    int evnr = 0, ntracks = 0, aProj = 0, zProj = 0, aTarg = 0, zTarg = 0;
    float b = 0., ekin = 0.;

    int ityp = 0, i3 = 0, ichg = 0, pid = 0;
    float ppx = 0., ppy = 0., ppz = 0., m = 0.;

    // ---> Read and check first event header line from input file
    char read[200];
    gzgets(fInputFile, read, 200); // line 1
    Int_t urqmdVersion = 0;
    sscanf(read, "UQMD   version:       %d   1000  %d  output_file  14", &urqmdVersion, &urqmdVersion);
    cout << "URQMD VERSION USED = " << urqmdVersion << endl;
    if (gzeof(fInputFile)) {
        cout << "-I MpdUrqmdGenerator : End of input file reached." << endl;
        gzclose(fInputFile);
        fInputFile = NULL;
        return kFALSE;
    }
    if (read[0] != 'U') {
        cout << "-E MpdUrqmdGenerator: Wrong event header" << endl;
        return kFALSE;
    }

    // ---> Read rest of event header
    gzgets(fInputFile, read, 200); // line 2
    sscanf(read, "projectile:  (mass, char) %d %d target:  (mass, char) %d %d",
            &aProj, &zProj, &aTarg, &zTarg); // line 2
    gzgets(fInputFile, read, 200); // line 3
    gzgets(fInputFile, read, 36); // line 4
    gzgets(fInputFile, read, 200); // line 4
    sscanf(read, "%f", &b); // line 4
    gzgets(fInputFile, read, 39); // line 5
    gzgets(fInputFile, read, 200); // line 5
    sscanf(read, "%e", &ekin); // line 5
    gzgets(fInputFile, read, 7); // line 6
    gzgets(fInputFile, read, 200); // line 6
    sscanf(read, "%d", &evnr); // line 6

    for (Int_t iline = 0; iline < ((urqmdVersion == 30400) ? 11 : 8); iline++) 
            gzgets(fInputFile, read, 200);

    gzgets(fInputFile, read, 200); // line 18
    sscanf(read, "%d", &ntracks); // line 18
    gzgets(fInputFile, read, 200); // line 19

    // ---> Calculate beta and gamma for Lorentztransformation
    TDatabasePDG* pdgDB = TDatabasePDG::Instance();
    TParticlePDG* kProton = pdgDB->GetParticle(2212);
    Double_t kProtonMass = kProton->Mass();

    Double_t eBeam = ekin + kProtonMass;
    Double_t pBeam = TMath::Sqrt(eBeam * eBeam - kProtonMass * kProtonMass);
    Double_t betaCM = pBeam / (eBeam + kProtonMass);
    Double_t gammaCM = TMath::Sqrt(1. / (1. - betaCM * betaCM));

    cout << "-I MpdUrqmdGenerator: Event " << evnr << ",  b = " << b
            << " fm,  multiplicity " << ntracks << ", ekin: " << ekin << endl;

    // Set event id and impact parameter in MCEvent if not yet done
    FairMCEventHeader* event = primGen->GetEvent();
    if (event && (!event->IsSet())) {
        event->SetEventID(evnr);
        event->SetB(b);
        event->MarkSet(kTRUE);
    }


    // ---> Loop over tracks in the current event
    for (int itrack = 0; itrack < ntracks; itrack++) {

        // Read momentum and PID from file
        gzgets(fInputFile, read, 81);
        gzgets(fInputFile, read, 200);
        sscanf(read, "%e %e %e %e %d %d %d", &ppx, &ppy, &ppz, &m, &ityp, &i3, &ichg);

        // Convert UrQMD type and charge to unique pid identifier
        if (ityp >= 0) {
            pid = 1000 * (ichg + 2) + ityp;
        } else {
            pid = -1000 * (ichg + 2) + ityp;
        }

        // Convert Unique PID into PDG particle code
        if (fParticleTable.find(pid) == fParticleTable.end()) {
            cout << "-W MpdUrqmdGenerator: PID " << ityp << " charge "
                    << ichg << " not found in table (" << pid << ")" << endl;
            continue;
        }
        Int_t pdgID = fParticleTable[pid];

        // Lorentztransformation to lab
        Double_t mass = Double_t(m);
        Double_t px = Double_t(ppx);
        Double_t py = Double_t(ppy);
        Double_t pz = Double_t(ppz);
        Double_t e = sqrt(mass * mass + px * px + py * py + pz * pz);
        if (gCoordinateSystem == sysLaboratory)
            pz = gammaCM * (pz + betaCM * e);
        Double_t ee = sqrt(mass * mass + px * px + py * py + pz * pz);

        TVector3 aa(px, py, pz);
        TLorentzVector pp;
        pp.SetPx(px);
        pp.SetPy(py);
        pp.SetPz(pz);
        pp.SetE(ee);

        // Give track to PrimaryGenerator
        primGen->AddTrack(pdgID, px, py, pz, 0., 0., 0.);

    }

    return kTRUE;
}
// ------------------------------------------------------------------------


// -----   Public method ReadEvent   --------------------------------------

Bool_t MpdUrqmdGenerator::SkipEvents(Int_t count) {
    if (count <= 0) {
        return kTRUE;
    }

    for (Int_t ii = 0; ii < count; ii++) {
        // ---> Check for input file
        if (!fInputFile) {
            cout << "-E MpdUrqmdGenerator: Input file not open! " << endl;
            return kFALSE;
        }

        // ---> Define event variables to be read from file
        int evnr = 0, ntracks = 0, aProj = 0, zProj = 0, aTarg = 0, zTarg = 0;
        float b = 0., ekin = 0.;

        // ---> Read and check first event header line from input file
        Int_t URQMDVERSION[2] = {20030, 30400};
        char read[200];
        gzgets(fInputFile, read, 200); // line 1
        Int_t urqmdVersion = 0;
        sscanf(read, "UQMD   version:       %d   1000  %d  output_file  14", &urqmdVersion, &urqmdVersion);
        cout << "URQMD VERSION USED = " << urqmdVersion << endl;

        if (gzeof(fInputFile)) {
            cout << "-I MpdUrqmdGenerator : End of input file reached." << endl;
            gzclose(fInputFile);
            fInputFile = NULL;
            return kFALSE;
        }
        if (read[0] != 'U') {
            cout << "-E MpdUrqmdGenerator: Wrong event header" << endl;
            return kFALSE;
        }

        // ---> Read rest of event header
        gzgets(fInputFile, read, 200); // line 2
        sscanf(read, "projectile:  (mass, char) %d %d target:  (mass, char) %d %d",
                &aProj, &zProj, &aTarg, &zTarg); // line 2
        gzgets(fInputFile, read, 200); // line 3
        gzgets(fInputFile, read, 36); // line 4
        gzgets(fInputFile, read, 200); // line 4
        sscanf(read, "%f", &b); // line 4
        gzgets(fInputFile, read, 39); // line 5
        gzgets(fInputFile, read, 200); // line 5
        sscanf(read, "%e", &ekin); // line 5
        gzgets(fInputFile, read, 7); // line 6
        gzgets(fInputFile, read, 200); // line 6
        sscanf(read, "%d", &evnr); // line 6
        if (urqmdVersion == URQMDVERSION[1])
            for (int iline = 0; iline < 11; iline++) {
                gzgets(fInputFile, read, 200);
            }// line 7-17
        else if (urqmdVersion == URQMDVERSION[0])
            for (int iline = 0; iline < 8; iline++) {
                gzgets(fInputFile, read, 200);
            }
        gzgets(fInputFile, read, 200); // line 15
        sscanf(read, "%d", &ntracks); // line 15
        gzgets(fInputFile, read, 200); // line 16

        cout << "-I MpdUrqmdGenerator: Event " << evnr << " skipped!" << endl;

        // ---> Loop over tracks in the current event
        for (int itrack = 0; itrack < ntracks; itrack++) {

            // Read momentum and PID from file
            gzgets(fInputFile, read, 200);
        }
    }
    return kTRUE;
}
// ------------------------------------------------------------------------

// -----   Private method ReadConverisonTable   ---------------------------

void MpdUrqmdGenerator::ReadConversionTable() {

    TString work = getenv("VMCWORKDIR");
    TString fileName = work + "/input/urqmd_pdg.dat";
    ifstream* pdgconv = new ifstream(fileName.Data());

    Int_t index = 0;
    Int_t pdgId = 0;

    while (!pdgconv->eof()) {
        index = pdgId = 0;
        *pdgconv >> index >> pdgId;
        fParticleTable[index] = pdgId;
    }

    pdgconv->close();
    delete pdgconv;

    cout << "-I MpdUrqmdGenerator: Particle table for conversion from "
            << "UrQMD loaded" << endl;

}
// ------------------------------------------------------------------------



ClassImp(MpdUrqmdGenerator);
