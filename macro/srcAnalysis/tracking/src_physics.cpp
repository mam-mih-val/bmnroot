#include <iostream>
#include <cmath>
#include <vector>
#include <cstdio>
#include <cstdlib>

#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"
#include "TH1.h"
#include "TH2.h"

using namespace std;

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

      
      
    }

  infile->Close();
  outfile->cd();
  h2VertexProfileXY->Write();
  h2VertexProfileZY->Write();
  outfile->Close();
  
  return 0;
}
