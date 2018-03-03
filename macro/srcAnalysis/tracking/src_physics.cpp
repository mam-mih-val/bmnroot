#include <iostream>
#include <cmath>
#include <vector>
#include <cstdio>
#include <cstdlib>

#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TH1.h"
#include "TH2.h"

using namespace std;

const double mP=0.938272;
const double Ac12=12.;
const double mC12 = 11.1770; // 12 * 931.494028
const double Pbeam = 3.5 * Ac12; //GeV/c 
const double Ebeam = sqrt(Pbeam*Pbeam + mC12 * mC12);
const TLorentzVector protonAtRest(0.,0.,0.,mP);

int main(int argc, char ** argv)
{
  if (argc !=3)
    {
      cerr << "Wrong number of arguments. Instead use\n"
	   << "\tsrc_physics /path/to/tracked/file /path/to/output/file\n";
      return -1;
    }

  TFile * infile = new TFile(argv[1]);
  if (infile->IsZombie())
    {
      cerr << "Failed to load file " << argv[1] << "\n"
	   << "Exiting ...\n";
      return -2;
    }
  else
    {
      cerr << "Successfully loaded file " << argv[1] << "\n";
    }

  // Get the tree
  int nArms;
  double vx, vy, vz, beamMX, beamMY;
  double pArmX[2], pArmY[2], pArmZ[2];
  TTree * intree = NULL;
  intree = (TTree*)infile->Get("tracked");
  if (!intree)
    {
      cerr << "Failed to load tree. Check that you are analyzing the output of track_arms.\n";
      return -3;
    }
  const int nEvents = intree->GetEntries();
  intree->SetBranchAddress("n",&nArms);
  intree->SetBranchAddress("vx",&vx);
  intree->SetBranchAddress("vy",&vy);
  intree->SetBranchAddress("vz",&vz);
  intree->SetBranchAddress("beamMX",&beamMX);
  intree->SetBranchAddress("beamMY",&beamMY);
  intree->SetBranchAddress("pArmX",pArmX);
  intree->SetBranchAddress("pArmY",pArmY);
  intree->SetBranchAddress("pArmZ",pArmZ);

  // Set up the output file
  TFile * outfile = new TFile(argv[2],"RECREATE");
  TH2D * h2VertexProfileXY = new TH2D("vertexXY","Vertices;X [cm]; Y [cm]; Counts",100,-10.,10.,100,-10.,10.);
  TH2D * h2VertexProfileZY = new TH2D("vertexZY","Vertices;Z [cm]; Y [cm]; Counts",100,-50.,50.,100,-10.,10.);
  TH2D * h2MandelstamTU = new TH2D("t_vs_u","Mandelstam Variables;t [GeV^2];u [GeV^2]Counts",100,-20.,20.,100,-20.,20.);

  for (int event=0 ; event <nEvents ; event++)
    {
      if (event %10000==0)
	cerr << "Working on event " << event << "\n";

      intree->GetEntry(event);

      // Insist on good trident tracks
      if (nArms !=3)
	continue;

      h2VertexProfileXY->Fill(vx,vy);
      h2VertexProfileZY->Fill(vz,vy);

      // Figure out the Lorentz Boost based on the beam direction
      TVector3 pBeam(beamMX,beamMY,1.);
      pBeam.SetMag(Pbeam);
      TLorentzVector vc12_ForReconstructions(pBeam, Ebeam);

      // The lab frame proton momentum vectors
      TLorentzVector p1Lab(pArmX[0],pArmY[0],pArmZ[0],sqrt(mP*mP + pArmX[0]*pArmX[0] + pArmY[0]*pArmY[0] + pArmZ[0]*pArmZ[0]));
      TLorentzVector p2Lab(pArmX[1],pArmY[1],pArmZ[1],sqrt(mP*mP + pArmX[1]*pArmX[1] + pArmY[1]*pArmY[1] + pArmZ[1]*pArmZ[1]));

      double t = (p2Lab - protonAtRest).Mag2();
      double u = (p1Lab - protonAtRest).Mag2();
      h2MandelstamTU->Fill(t,u);
      
      // The CM frame proton momentum vectors
      TLorentzVector p1CM = p1Lab;
      p1CM.Boost(-vc12_ForReconstructions.BoostVector());
      TLorentzVector p2CM = p2Lab;
      p2CM.Boost(-vc12_ForReconstructions.BoostVector());

    }

  infile->Close();
  outfile->cd();
  h2VertexProfileXY->Write();
  h2VertexProfileZY->Write();
  h2MandelstamTU->Write();
  outfile->Close();
  
  return 0;
}
