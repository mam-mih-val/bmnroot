// -------------------------------------------------------------------------
// -----                MpdUrqmd23Generator source file                  -----
// -----                Created 24/06/04  by V. Friese                 -----
// -------------------------------------------------------------------------
#include "MpdUrqmd23Generator.h"

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
MpdUrqmd23Generator::MpdUrqmd23Generator()
  :FairGenerator(),
   fInputFile(NULL),
   fParticleTable(),
   fFileName(NULL)
{
}
// ------------------------------------------------------------------------



// -----   Standard constructor   -----------------------------------------
MpdUrqmd23Generator::MpdUrqmd23Generator(const char* fileName)
  :FairGenerator(),
   fInputFile(NULL),
   fParticleTable(),
   fFileName(fileName)
{
  //  fFileName = fileName;
  cout << "-I MpdUrqmd23Generator: Opening input file " << fFileName << endl;
  fInputFile = gzopen(fFileName, "rb");
  if ( ! fInputFile ) { Fatal("MpdUrqmd23Generator","Cannot open input file."); exit(1); }
  ReadConversionTable();
}
// ------------------------------------------------------------------------



// -----   Destructor   ---------------------------------------------------
MpdUrqmd23Generator::~MpdUrqmd23Generator()
{
  //  cout<<"Enter Destructor of MpdUrqmd23Generator"<<endl;
  if ( fInputFile ) {
    gzclose(fInputFile);
    fInputFile = NULL;
  }
  fParticleTable.clear();
  //  cout<<"Leave Destructor of MpdUrqmd23Generator"<<endl;
}
// ------------------------------------------------------------------------



// -----   Public method ReadEvent   --------------------------------------
Bool_t MpdUrqmd23Generator::ReadEvent(FairPrimaryGenerator* primGen)
{

  // ---> Check for input file
  if ( ! fInputFile ) {
    cout << "-E MpdUrqmd23Generator: Input file not open! " << endl;
    return kFALSE;
  }

  // ---> Check for primary generator
  if ( ! primGen ) {
    cout << "-E- MpdUrqmd23Generator::ReadEvent: "
         << "No PrimaryGenerator!" << endl;
    return kFALSE;
  }

  // ---> Define event variables to be read from file
  int evnr=0, ntracks=0, aProj=0, zProj=0, aTarg=0, zTarg=0;
  float b = 0., ekin = 0.;

  int ityp=0, i3=0, ichg=0, pid=0;
  float ppx=0., ppy=0., ppz=0., m=0.;

  // ---> Read and check first event header line from input file
  char read[200];
  gzgets(fInputFile,read, 200);      // line 1
  if ( gzeof(fInputFile) ) {
    cout << "-I MpdUrqmd23Generator : End of input file reached." << endl;
    gzclose(fInputFile);
    fInputFile = NULL;
    return kFALSE;
  }
  if ( read[0] != 'U' ) {
    cout << "-E MpdUrqmd23Generator: Wrong event header" << endl;
    return kFALSE;
  }

  // ---> Read rest of event header
  gzgets(fInputFile, read, 200);     // line 2
  sscanf(read,"projectile:  (mass, char) %d %d target:  (mass, char) %d %d",
    &aProj,&zProj,&aTarg,&zTarg);    // line 2
  gzgets(fInputFile, read, 200);     // line 3
  gzgets(fInputFile, read, 36);      // line 4
  gzgets(fInputFile, read, 200);     // line 4
  sscanf(read, "%f", &b);            // line 4
  gzgets(fInputFile, read, 39);      // line 5
  gzgets(fInputFile, read, 200);     // line 5
  sscanf(read, "%e", &ekin);         // line 5
  gzgets(fInputFile, read, 7);       // line 6
  gzgets(fInputFile, read, 200);     // line 6
  sscanf(read, "%d", &evnr);         // line 6
  for (int iline=0; iline<8; iline++)  { gzgets(fInputFile, read, 200); } // line 7-14
  gzgets(fInputFile, read, 200);     // line 15
  sscanf(read,"%d",&ntracks);        // line 15
  gzgets(fInputFile, read, 200);     // line 16

  // ---> Calculate beta and gamma for Lorentztransformation
  TDatabasePDG* pdgDB = TDatabasePDG::Instance();
  TParticlePDG* kProton = pdgDB->GetParticle(2212);
  Double_t kProtonMass=kProton->Mass();

  Double_t eBeam   = ekin + kProtonMass;
  Double_t pBeam   = TMath::Sqrt(eBeam*eBeam - kProtonMass*kProtonMass);
  Double_t betaCM  = pBeam / (eBeam + kProtonMass);
  Double_t gammaCM = TMath::Sqrt( 1. / ( 1. - betaCM*betaCM) );

  cout << "-I MpdUrqmd23Generator: Event " << evnr << ",  b = " << b
       << " fm,  multiplicity " << ntracks  << ", ekin: " << ekin << endl;

  // Set event id and impact parameter in MCEvent if not yet done
  FairMCEventHeader* event = primGen->GetEvent();
  if ( event && (! event->IsSet()) ) {
    event->SetEventID(evnr);
    event->SetB(b);
    event->MarkSet(kTRUE);
  }


  // ---> Loop over tracks in the current event
  for(int itrack=0; itrack<ntracks; itrack++) {

    // Read momentum and PID from file
    gzgets(fInputFile, read, 81);
    gzgets(fInputFile, read, 200);
    sscanf(read, "%e %e %e %e %d %d %d", &ppx, &ppy, &ppz, &m, &ityp, &i3, &ichg);

    // Convert UrQMD type and charge to unique pid identifier
    if (ityp >= 0) { pid =  1000 * (ichg+2) + ityp; }
    else { pid = -1000 * (ichg+2) + ityp; }

    // Convert Unique PID into PDG particle code
    if (fParticleTable.find(pid) == fParticleTable.end()) {
      cout << "-W MpdUrqmd23Generator: PID " << ityp << " charge "
           << ichg << " not found in table (" << pid << ")" << endl;
      continue;
    }
    Int_t pdgID = fParticleTable[pid];

    // Lorentztransformation to lab
    Double_t mass = Double_t(m);
    Double_t px   = Double_t(ppx);
    Double_t py   = Double_t(ppy);
    Double_t pz   = Double_t(ppz);
    Double_t e    = sqrt( mass*mass + px*px + py*py + pz*pz );
    if (gCoordinateSystem == sysLaboratory)
        pz = gammaCM * ( pz + betaCM * e );
    Double_t ee = sqrt( mass*mass + px*px + py*py + pz*pz );

    TVector3 aa(px,py,pz);
    TLorentzVector pp;
    pp.SetPx( px );
    pp.SetPy( py );
    pp.SetPz( pz);
    pp.SetE( ee );

    // Give track to PrimaryGenerator
    primGen->AddTrack(pdgID, px, py, pz, 0., 0., 0.);

  }

  return kTRUE;
}
// ------------------------------------------------------------------------


// -----   Public method ReadEvent   --------------------------------------
Bool_t MpdUrqmd23Generator::SkipEvents(Int_t count)
{
  if (count<=0) { return kTRUE; }

  for(Int_t ii=0; ii<count; ii++) {
    // ---> Check for input file
    if ( ! fInputFile ) {
      cout << "-E MpdUrqmd23Generator: Input file not open! " << endl;
      return kFALSE;
    }

    // ---> Define event variables to be read from file
    int evnr=0, ntracks=0, aProj=0, zProj=0, aTarg=0, zTarg=0;
    float b = 0., ekin = 0.;

    // ---> Read and check first event header line from input file
    char read[200];
    gzgets(fInputFile, read, 200);     // line 1
    if ( gzeof(fInputFile) ) {
      cout << "-I MpdUrqmd23Generator : End of input file reached." << endl;
      gzclose(fInputFile);
      fInputFile = NULL;
      return kFALSE;
    }
    if ( read[0] != 'U' ) {
      cout << "-E MpdUrqmd23Generator: Wrong event header" << endl;
      return kFALSE;
    }

    // ---> Read rest of event header
    gzgets(fInputFile, read, 200);     // line 2
    sscanf(read,"projectile:  (mass, char) %d %d target:  (mass, char) %d %d",
      &aProj,&zProj,&aTarg,&zTarg);    // line 2
    gzgets(fInputFile, read, 200);     // line 3
    gzgets(fInputFile, read, 36);      // line 4
    gzgets(fInputFile, read, 200);     // line 4
    sscanf(read, "%f", &b);            // line 4
    gzgets(fInputFile, read, 39);      // line 5
    gzgets(fInputFile, read, 200);     // line 5
    sscanf(read, "%e", &ekin);         // line 5
    gzgets(fInputFile, read, 7);       // line 6
    gzgets(fInputFile, read, 200);     // line 6
    sscanf(read, "%d", &evnr);         // line 6
    for (int iline=0; iline<8; iline++)  { gzgets(fInputFile, read, 200); } // line 7-14
    gzgets(fInputFile, read, 200);     // line 15
    sscanf(read,"%d",&ntracks);        // line 15
    gzgets(fInputFile, read, 200);     // line 16

    cout << "-I MpdUrqmd23Generator: Event " << evnr << " skipped!" << endl;

    // ---> Loop over tracks in the current event
    for(int itrack=0; itrack<ntracks; itrack++) {

      // Read momentum and PID from file
      gzgets(fInputFile, read, 200);
    }
  }
  return kTRUE;
}
// ------------------------------------------------------------------------

// -----   Private method ReadConverisonTable   ---------------------------
void MpdUrqmd23Generator::ReadConversionTable()
{

  TString work      = getenv("VMCWORKDIR");
  TString fileName  = work + "/input/urqmd_pdg.dat";
  ifstream* pdgconv = new ifstream(fileName.Data());

  Int_t index = 0;
  Int_t pdgId = 0;

  while ( ! pdgconv->eof() ) {
    index =pdgId =0 ;
    *pdgconv >> index >> pdgId ;
    fParticleTable[index] = pdgId;
  }

  pdgconv->close();
  delete pdgconv;

  cout << "-I MpdUrqmd23Generator: Particle table for conversion from "
       << "UrQMD loaded" <<  endl;

}
// ------------------------------------------------------------------------



ClassImp(MpdUrqmd23Generator);
