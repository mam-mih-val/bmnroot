#include <iostream>
#include <cmath>
#include <vector>
#include <cstdio>
#include <cstdlib>

#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "Math/GSLMinimizer.h"
#include "Math/Functor.h"

#include "BmnGemStripHit.h"
#include "BmnTofHit.h"
#include "BmnMwpcHit.h"

#include "SRCEvent.h"

using namespace std;

SRCEvent * currentEvent;
double closestApproachSq(TVector3 &v, TVector3 &x0, double mx, double my);
double residual(const double *xx);

int main(int argc, char ** argv)
{
  if (argc !=3)
    {
      cerr << "Wrong number of arguments. Instead use\n"
	   << "\ttrack_arms /path/to/run/reco/file /path/to/output/file\n";
      return -1;
    }

  // Set up the fitter
  ROOT::Math::GSLMinimizer myMin(ROOT::Math::kVectorBFGS);
  myMin.SetMaxFunctionCalls(1000000);
  myMin.SetMaxIterations(100000);
  myMin.SetTolerance(0.001);
  
  // Set up the input file
  TFile * infile = NULL;
  infile = new TFile(argv[1]);
  if (!infile)
    {
      cerr << "Could not open file " << argv[1] <<"\n"
	   << "\tBailing out\n";
      return -2;
    }
  else
    {
      cerr << "Successfully opened file " << argv[1] << " and saved it to address " << infile << "\n";
    }

  // Set up the output file
  TFile * outfile = new TFile(argv[2],"RECREATE");
  TTree * outtree = new TTree("tracked","Output tree for the track_arms program");
  double outVX,outVY,outVZ;
  int totalArms, success;
  outtree->Branch("vx",&outVX,"vx/D");
  outtree->Branch("vy",&outVY,"vy/D");
  outtree->Branch("vz",&outVZ,"vz/D");
  outtree->Branch("n",&totalArms,"n/I");
  outtree->Branch("fit",&success,"fit/I");

  // Set up the tree
  TClonesArray * tofData = new TClonesArray("BmnTofHit");
  TClonesArray * mwpcData = new TClonesArray("BmnMwpcHit");
  TClonesArray * gemData = new TClonesArray("BmnGemStripHit");
  TTree * intree = NULL;
  intree = (TTree*) infile->Get("cbmsim");
  if (!intree)
    {
      cerr << "Could not find cbmsim tree. Perhaps the wrong type of input file. Bailing out.\n";
      return -3;
    }
  else
    {
      cerr << "Successfully loaded tree at address " << intree << "\n";
    }
  const int nEvents = intree->GetEntries();
  intree->SetBranchAddress("BmnTof400Hit",&tofData);
  intree->SetBranchAddress("BmnMwpcHit",&mwpcData);
  intree->SetBranchAddress("BmnGemStripHit",&gemData);

  // Loop over events
  for (int event=0 ; event<nEvents ; event++)
    {
      if (event % 1000== 0)
	cerr << "Working on event " << event << "\n";

      intree->GetEvent(event);

      // Initialize values for this event
      totalArms=0;
      success=0; // will be set to true (1) on successful minimization

      // Loop over MWPC hits, sort
      vector<TVector3> mwpcUHits;
      vector<TVector3> mwpcDHits;
      for (int m=0 ; m<mwpcData->GetEntriesFast() ; m++)
	{	 
	  BmnMwpcHit * thisHit = (BmnMwpcHit*)mwpcData->At(m);

	  // Sort by z position (eventually do this by module ID)

	  // This is for George's simulated data
	  if ((thisHit->GetZ()<-90.)&&(thisHit->GetZ()>-110.)) 
	    mwpcUHits.push_back(TVector3(thisHit->GetX(),thisHit->GetY(),thisHit->GetZ())); 
	  else if ((thisHit->GetZ()<-40.)&&(thisHit->GetZ()>-60.))
	    mwpcDHits.push_back(TVector3(thisHit->GetX(),thisHit->GetY(),thisHit->GetZ()));

	  //if ((thisHit->GetZ()<-300.)&&(thisHit->GetZ()>-400.)) 
	  // mwpcUHits.push_back(TVector3(thisHit->GetX(),thisHit->GetY(),thisHit->GetZ())); 
	  //else if ((thisHit->GetZ()<-200.)&&(thisHit->GetZ()>-300.))
	  //  mwpcDHits.push_back(TVector3(thisHit->GetX(),thisHit->GetY(),thisHit->GetZ()));
	  
	}

      // Loop over ToF hits, sort
      vector<TVector3> tofLHits;
      vector<TVector3> tofRHits;
      vector<double> lTimes;
      vector<double> rTimes;
      for (int t=0 ; t<tofData->GetEntriesFast() ; t++)
	{
	  BmnTofHit * thisHit = (BmnTofHit*)tofData->At(t);
	  if (thisHit->GetX() > 0.)
	    {
	      tofLHits.push_back(TVector3(thisHit->GetX(),thisHit->GetY(),thisHit->GetZ()));
	    }
	  else
	    {
	      tofRHits.push_back(TVector3(thisHit->GetX(),thisHit->GetY(),thisHit->GetZ()));
	    }
	}

      // Loop over GEM hits, sort
      vector<TVector3> gemLHits;
      vector<TVector3> gemRHits;
      for (int g=0 ; g<gemData->GetEntriesFast() ; g++)
	{
	  BmnGemStripHit * thisHit = (BmnGemStripHit*) gemData->At(g);
	  if (thisHit->GetX() > 0.)
	    {
	      gemLHits.push_back(TVector3(thisHit->GetX(),thisHit->GetY(),thisHit->GetZ()));
	    }
	  else
	    {
	      gemRHits.push_back(TVector3(thisHit->GetX(),thisHit->GetY(),thisHit->GetZ()));
	    }
	}

      //cout << mwpcUHits.size() << "\t" << mwpcDHits.size() << "\t"
      // << gemLHits.size() << "\t" << tofLHits.size() << "\t"
      //   << gemRHits.size() << "\t" << tofRHits.size() << "\n";

      // Now, we need to pick the best pair from each of the three arms: (beam, left, right)
      int bestMUIndex=-1;
      int bestMDIndex=-1;
      int bestGLIndex=-1;
      int bestGRIndex=-1;      
      int bestTLIndex=-1;
      int bestTRIndex=-1;
      // Starting with Beam
      double bestBeamAngle=M_PI;
      for (int i=0 ; i < mwpcUHits.size() ; i++)
	for (int j=0 ; j< mwpcDHits.size() ; j++)
	  {
	    double angle = (mwpcDHits[j] - mwpcUHits[i]).Theta();
	    if ((angle < bestBeamAngle) || (bestMUIndex < 0) || (bestMDIndex < 0))
	      {
		bestMUIndex=i;
		bestMDIndex=j;
		bestBeamAngle=angle;		
	      }
	  }
      // Next, let's pick the best pair of left arm hits
      double bestLSlope=10000.;
      for (int i=0 ; i < gemLHits.size() ; i++)
	for (int j=0 ; j< tofLHits.size() ; j++)
	  {
	    double yslope = (tofLHits[j].Y() - gemLHits[i].Y())/(tofLHits[j].Z() - gemLHits[i].Z());
	    if ((fabs(yslope)<bestLSlope) || (bestGLIndex < 0) || (bestTLIndex < 0))
	      {
		bestGLIndex=i;
		bestTLIndex=j;
		bestLSlope=yslope;
	      }
	  }
      // Next, let's pick the best pair of right arm hits
      double bestRSlope=10000.;
      for (int i=0 ; i < gemRHits.size() ; i++)
	for (int j=0 ; j< tofRHits.size() ; j++)
	  {
	    double yslope = (tofRHits[j].Y() - gemRHits[i].Y())/(tofRHits[j].Z() - gemRHits[i].Z());
	    if ((fabs(yslope)<bestRSlope) || (bestGRIndex < 0) || (bestTRIndex < 0))
	      {
		bestGRIndex=i;
		bestTRIndex=j;
		bestRSlope=yslope;
	      }
	  }
      
      // We require at least a beam arm
      if (!((bestMUIndex < 0)||(bestMDIndex < 0)))
	{      
	  totalArms++;
	  SRCEvent thisEvent(mwpcUHits[bestMUIndex],mwpcDHits[bestMDIndex]);
	  
	  // Determine if we have other tracking arms
	  bool hasLeftArm=((bestGLIndex >= 0) && (bestTLIndex >= 0));
	  if (hasLeftArm)
	    {
	      thisEvent.addArm(gemLHits[bestGLIndex] ,tofLHits[bestTLIndex]);
	      totalArms++;
	    }
	  bool hasRightArm=((bestGRIndex >= 0) && (bestTRIndex >= 0));
	  if (hasRightArm)
	    {
	      thisEvent.addArm(gemRHits[bestGRIndex] ,tofRHits[bestTRIndex]);
	      totalArms++;
	    }
	  
	  // We require at least one additional arm for tracking
	  if (totalArms >= 2)
	    {
	  
	      // Set the current event pointer so that the residuals function can find the data
	      currentEvent=&thisEvent;
	      
	      // Set up the functor based on the number of available arms
	      ROOT::Math::Functor myFunctor(&residual,3+totalArms*2);
	      myMin.SetFunction(myFunctor);
	      myMin.SetVariable(0,"vx",0.01,0.01);
	      myMin.SetVariable(1,"vy",0.01,0.01);
	      myMin.SetVariable(2,"vz",0.01,0.01);
	      for (int i=0 ; i<totalArms ; i++)
		{
		  // Initial slope guess
		  double mx = (thisEvent.armList[i].hits[1].X() - thisEvent.armList[i].hits[0].X())
		    /(thisEvent.armList[i].hits[1].Z() - thisEvent.armList[i].hits[0].Z());
		  char temp[10];
		  sprintf(temp,"mx%d",i);
		  myMin.SetVariable(3+2*i + 0,temp,mx,0.001);
		  
		  double my = (thisEvent.armList[i].hits[1].Y() - thisEvent.armList[i].hits[0].Y())
		    /(thisEvent.armList[i].hits[1].Z() - thisEvent.armList[i].hits[0].Z());
		  sprintf(temp,"my%d",i);
		  myMin.SetVariable(3+2*i + 1,temp,my,0.001);	  
		}
	      
	      // Do the minimization
	      success = myMin.Minimize() ? 1:0;
	      const double * fitRes = myMin.X();
	      
	      outVX=fitRes[0];
	      outVY=fitRes[1];
	      outVZ=fitRes[2];
	      TVector3 pBeam(fitRes[3],fitRes[4],1.);
	      vector<TVector3> tracks;
	      for (int i=1 ; i<totalArms ; i++)
		tracks.push_back(TVector3(fitRes[3+2*i + 0],fitRes[3+2*i + 1],1.));
	    }
	}
      outtree->Fill();
    }

  infile->Close();
  outtree->Write();
  outfile->Close();
  return 0;
}

double closestApproachSq(TVector3 &v, TVector3 &x0, double mx, double my)
{
  double mNorm=sqrt(1 + mx*mx + my*my);
  TVector3 mUnit(mx/mNorm,my/mNorm,1./mNorm);
  TVector3 res = v -x0 + mUnit * (mUnit.Dot(x0-v));

  return res.Mag2();
}

double residual(const double *xx)
{
  int nArms=currentEvent->armList.size();
  double res=0.;
  TVector3 vertex(xx[0],xx[1],xx[2]);
  
  for (int a=0 ; a<nArms ; a++)
    {
      double mx=xx[3 + 2*a + 0];
      double my=xx[3 + 2*a + 1];

      for (int i=0 ; i<2 ; i++)
	res += closestApproachSq(vertex,currentEvent->armList[a].hits[i],mx,my);
    }

  return res;
}
