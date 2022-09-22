/**
 *@class MpdPHSDGenerator
 *@author V.Voronyuk <vadimv@jinr.ru>
 * MpdPHSDGenerator reads newer (instead of default) output (phsd.dat/phsd.dat.gz)
 * of HSD/PHSD transport model.
 **/

#include <stdio.h>
#include <stdlib.h>

#include <TMath.h>

#include "FairMCEventHeader.h"
#include "MpdMCEventHeader.h"
#include "MpdPHSDGenerator.h"
#include "constants.h" //AZ

#include <TDatabasePDG.h> //AZ

// ---------------------------------------------------------------------
MpdPHSDGenerator::MpdPHSDGenerator()
{
/* It is better to leave empty */
};
// ---------------------------------------------------------------------
MpdPHSDGenerator::MpdPHSDGenerator(const char *filename="phsd.dat.gz")
{
  fgzFile = gzopen(filename,"rb"); // zlib
  if (!fgzFile) {printf("-E- MpdPHSDGenerator: can not open file: %s\n",filename); exit(1);}
  printf("-I- MpdPHSDGenerator: open %s\n",filename);
  
  fPsiRP=0.;
  fisRP=kTRUE; // by default RP is random
  frandom = new TRandom2();
  frandom->SetSeed(0);
};
// ---------------------------------------------------------------------
MpdPHSDGenerator::~MpdPHSDGenerator()
{
  if (fgzFile) {gzclose(fgzFile); fgzFile=NULL;}
  delete frandom;
};
// ---------------------------------------------------------------------
Bool_t MpdPHSDGenerator::ReadEvent(FairPrimaryGenerator *primGen)
{
  if (!ReadHeader())
  {
    printf("-I- MpdPHSDGenerator: End of file reached.\n");
    return kFALSE; // end of file
  }

  /* set random Reaction Plane angle */
  if (fisRP) {fPsiRP=frandom->Uniform(2.0*TMath::Pi());}
  //printf("-I- MpdPHSDGenerator: RP angle = %e\n",fPsiRP);

  /* Set event impact parameter in MCEventHeader if not yet done */
  FairMCEventHeader *eventHeader = primGen->GetEvent();
  if (eventHeader && (!eventHeader->IsSet()))
  {
    eventHeader->SetB(fb);
    eventHeader->MarkSet(kTRUE);
    // fill extra
    MpdMCEventHeader *extraEventHeader = dynamic_cast<MpdMCEventHeader*> (eventHeader);
    if (extraEventHeader)
    {
      extraEventHeader->SetPhi(fPsiRP);
    }
  }
  //AZ
  Double_t kProtonMass = TDatabasePDG::Instance()->GetParticle(2212)->Mass();
  Double_t ekin = 4.0;
  Double_t eBeam   = ekin + kProtonMass;
  Double_t pBeam   = TMath::Sqrt(eBeam*eBeam - kProtonMass*kProtonMass);
  Double_t betaCM  = pBeam / (eBeam + kProtonMass);
  Double_t gammaCM = TMath::Sqrt( 1. / ( 1. - betaCM*betaCM) );
  //AZ

  /* read tracks */
  for(Int_t i=1; i<=fntr; i++)
  {
    //AZ Int_t ipdg; Float_t px,py,pz;
    Int_t ipdg; Float_t px,py,pz,ee; //AZ
    /* read track */
    gzgets(fgzFile,fbuffer,256); 
    if (gzeof(fgzFile)) {printf("-E- MpdPHSDGenerator: unexpected end of file\n"); exit(1);}
    /* scan values */
    //AZ int res=sscanf(fbuffer,"%d %*d %e %e %e",&ipdg,&px,&py,&pz);
    //AZ if (res!=4)  {printf("-E- MpdPHSDGenerator: selftest error in track, scan %d of 4\n",res); exit(1);}
    int res=sscanf(fbuffer,"%d %*d %e %e %e %e",&ipdg,&px,&py,&pz,&ee); //AZ
    if (res!=5)  {printf("-E- MpdPHSDGenerator: selftest error in track, scan %d of 5\n",res); exit(1);} //AZ
    if (ipdg==0) {printf("-W- MpdPHSDGenerator: particle with pdg=0\n"); continue;}
    if (!TDatabasePDG::Instance()->GetParticle(ipdg)) { std::cout << "\033[31m -W- ---------- MpdPHSDGenerator: particle with pdg " << ipdg << " not found \033[0m" << std::endl; continue; } //AZ
    
    // replacement of heavy particles for Geant4
    // this decays will be improved in the next versions of PHSD
    if (ipdg==+413)  ipdg=+421;  // D*+     -> D0
    if (ipdg==-413)  ipdg=-421;  // D*-     -> D0_bar
    if (ipdg==+423)  ipdg=+421;  // D*0     -> D0
    if (ipdg==-423)  ipdg=-421;  // D*0_bar -> D0_bar
    if (ipdg==+4214) ipdg=+4122; // Sigma*_c+  -> Lambda_c+
    if (ipdg==-4214) ipdg=-4122; // Sigma*_c-  -> Lambda_c-
    if (ipdg==+4114) ipdg=+4122; // Sigma*_c0  -> Lambda_c+
    if (ipdg==-4114) ipdg=-4122; // Sigma*_c0  -> Lambda_c-
    if (ipdg==+4224) ipdg=+4122; // Sigma*_c++ -> Lambda_c+
    if (ipdg==-4224) ipdg=-4122; // Sigma*_c-- -> Lambda_c-
    
    //AZ - Lorentz transformation to lab
    if (gCoordinateSystem == sysLaboratory) {
      //Double_t mass = TDatabasePDG::Instance()->GetParticle(ipdg)->Mass();
      //Double_t e = sqrt(mass * mass + px * px + py * py + pz * pz);
      pz = gammaCM * (pz + betaCM * ee);
    }
    //AZ
    /* rotate RP angle */
    if (fPsiRP!=0.)
    {
      Double_t cosPsi = TMath::Cos(fPsiRP);
      Double_t sinPsi = TMath::Sin(fPsiRP);
      Double_t px1=px*cosPsi-py*sinPsi;
      Double_t py1=px*sinPsi+py*cosPsi;
      px=px1; py=py1;
    }
    
    /* add track to simulation */
    primGen->AddTrack(ipdg, px, py, pz, 0., 0., 0.);
  }
  
  return kTRUE;
};
// ---------------------------------------------------------------------
Bool_t MpdPHSDGenerator::ReadHeader()
{
  /* read header */
  gzgets(fgzFile,fbuffer,256);       // 1st line
  if (gzeof(fgzFile)) return kFALSE; // end of file reached
  int res=sscanf(fbuffer,"%d %*d %*d %e",&fntr,&fb);
  if (res!=2) {printf("-E- MpdPHSDGenerator: selftest error in header, scan %d of 2\n",res); exit(1);}
  gzgets(fgzFile,fbuffer,256);       // 2nd line
  if (gzeof(fgzFile)) {printf("-E- MpdPHSDGenerator: unexpected end of file\n"); exit(1);}
  return kTRUE;
};
// ---------------------------------------------------------------------
void MpdPHSDGenerator::SkipTrack()
{
  gzgets(fgzFile,fbuffer,256);
  if (gzeof(fgzFile)) {printf("-E- MpdPHSDGenerator: unexpected end of file\n"); exit(1);}
};
// ---------------------------------------------------------------------
Bool_t MpdPHSDGenerator::SkipEvents(int n)
{
  for (Int_t k=1; k<=n; ++k)
  {
    if (!ReadHeader()) return kFALSE; // end of file
    for (Int_t i=1; i<=fntr; ++i) SkipTrack();
  }
  return kTRUE;
};
// ---------------------------------------------------------------------
ClassImp(MpdPHSDGenerator);
