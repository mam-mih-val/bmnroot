#include <iostream>
#include <cmath>
#include <vector>

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
  if (argc !=2)
    {
      cerr << "Wrong number of arguments. Instead use\n"
	   << "\ttrack_arms /path/to/run/reco/file\n";
      return -1;
    }

  // Set up the fitter
  ROOT::Math::GSLMinimizer myMin(ROOT::Math::kVectorBFGS);
  myMin.SetMaxFunctionCalls(1000000);
  myMin.SetMaxIterations(100000);
  myMin.SetTolerance(0.001);
  ROOT::Math::Functor myFunctor(&residual,9);
  myMin.SetFunction(myFunctor);
  
  // Set up the file
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
  intree->SetBranchAddress("BmnTof1Hit",&tofData);
  intree->SetBranchAddress("BmnMwpcHit",&mwpcData);
  intree->SetBranchAddress("BmnGemStripHit",&gemData);

  // Loop over events
  for (int event=0 ; event<nEvents ; event++)
    {
      if (event % 1000== 0)
	cerr << "Working on event " << event << "\n";

      intree->GetEvent(event);

      // Loop over MWPC hits, sort
      vector<TVector3> mwpcUHits;
      vector<TVector3> mwpcDHits;
      for (int m=0 ; m<mwpcData->GetEntriesFast() ; m++)
	{	 
	  BmnMwpcHit * thisHit = (BmnMwpcHit*)mwpcData->At(m);

	  // Sort by z position (eventually do this by module ID)
	  if ((thisHit->GetZ()<-300.)&&(thisHit->GetZ()>-400.)) 
	    mwpcUHits.push_back(TVector3(thisHit->GetX(),thisHit->GetY(),thisHit->GetZ())); 
	  else if ((thisHit->GetZ()<-200.)&&(thisHit->GetZ()>-300.))
	    mwpcDHits.push_back(TVector3(thisHit->GetX(),thisHit->GetY(),thisHit->GetZ()));
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

      if ((bestMUIndex < 0)||(bestMDIndex < 0)||(bestGLIndex < 0)||(bestGRIndex < 0)||(bestTLIndex < 0)||(bestTRIndex < 0))
	continue;
      
      // Fit this combination of hits
      SRCEvent thisEvent(mwpcUHits[bestMUIndex],mwpcDHits[bestMDIndex]);
      thisEvent.addArm(gemLHits[bestGLIndex] ,tofLHits[bestTLIndex]);
      thisEvent.addArm(gemRHits[bestGRIndex] ,tofRHits[bestTRIndex]);
			 
      //SRCEvent thisEvent(TVector3(0.,0.,-100.),TVector3(0.,0.,-50.),
      //			 TVector3(50.,0.,100.),TVector3(125.,0.,250.),
      //			 TVector3(-50.,0.,100.),TVector3(-125.,0.,250.));
			 
      currentEvent=&thisEvent;
      
      // Calculate physics vectors
      myMin.SetVariable(0,"vx",0.,0.01);
      myMin.SetVariable(1,"vy",0.,0.01);
      myMin.SetVariable(2,"vz",0.,0.01);
      myMin.SetVariable(3,"mxb",0.,0.001);
      myMin.SetVariable(4,"myb",0.,0.001);
      myMin.SetVariable(5,"mxl",0.577,0.001);
      myMin.SetVariable(6,"myl",0.,0.001);
      myMin.SetVariable(7,"mxr",-0.577,0.001);
      myMin.SetVariable(8,"myr",0.,0.001);
      myMin.Minimize();

      const double * fitRes = myMin.X();

      TVector3 vRec(fitRes[0],fitRes[1],fitRes[2]);
      TVector3 pBeam(fitRes[3],fitRes[4],1.);
      TVector3 pLeft(fitRes[5],fitRes[6],1.);
      TVector3 pRight(fitRes[7],fitRes[8],1.);

      for (int i=0 ; i<9 ; i++)
	cout << fitRes[i] << " ";
      cout << "\n";
    }
  
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
