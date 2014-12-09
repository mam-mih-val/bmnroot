#include "BmnGemGas.h"
#include <ostream>
#include <iostream>
#include <fstream>
#include <TRandom.h>
#include "TError.h"

//#include "ErrLogger/ErrLog.hh"

BmnGemGas::BmnGemGas():_E(0),
		 _B(0),
		 _T(293.),
		 _p(1024.),
		 _VDrift(0),
		 _Dl(0),
		 _Dt(0),
		 _k(0),
		 _W(0),
                 _CSD(0),
                 _CSDNorm(0),
		 _CSDEpol(0)
{}

BmnGemGas::BmnGemGas(double const E,
	       double const B,
	       double const T,
	       double const p,
	       double const VDrift,
	       double const Dl,
	       double const Dt,
	       double const k,
	       double const W,
	       const std::vector<double>& CSD,	   
	       double const CSDEpol):_E(E),
				     _B(B),
				     _T(T),
				     _p(p),
				     _VDrift(VDrift),
				     _Dl(Dl),
				     _Dt(Dt),
				     _k(k),
				     _W(W),
                                     _CSD(CSD),
                                     _CSDNorm(0),
				     _CSDEpol(CSDEpol)
{}

BmnGemGas::BmnGemGas(const std::string& Filename,
	       double const E):_E(E)
{
  //Reading the GasFile
  std::ifstream infile(Filename.c_str(), std::fstream::in);
  if (!infile.good())
    Fatal("BmnGemGas::BmnGemGas","Input File is not found");
  //ReadGasBegin returns the number of electric fields (and sets _T, _B, _p)
  int noent = ReadGasBegin(&infile);
  if (noent <= 0)
    Fatal("BmnGemGas::BmnGemGas","Number of electric fields nonpositive?\nCould not read File.\nExecution aborted.");
  double e[noent];                          //E field in V/cm
  double vdrift[noent];                     //e-drift velocity in cm/ns
  double dt[noent];                         //transverse diffusion in sqrt(cm)
  double dl[noent];                         //longitudinal diff. in sqrt(cm)
  double k[noent];                          //attachment coefficient in 1/cm

  //ReadGasArrays returns the number of entries in cluster size distribution
  //and sets the values in the assigned arrays (and _W)
  int nCSDFile = ReadGasArrays(&infile, noent,
				e, vdrift, dt, dl, k);
  if (nCSDFile < 2)
    Fatal("BmnGemGas::BmnGemGas","Number of cluster sizes too small in File.\nExecution aborted.");
 
  //cluster size distribution: first entry is the rel. number of clusters with
  //1 electron,the last entry the rel. nr. of clusters with more than nCSDFile
  //so the last entry may not be transferred
  int _nCSD = nCSDFile-1;            //omit the last
  _CSD.resize(_nCSD);

  _CSDNorm = 0;
  for (int i=0; i < _nCSD;i++) {
  	infile >> _CSD[i];
  	_CSDNorm += _CSD[i];
  }
  if (!infile.good())
    Fatal("BmnGemGas::BmnGemGas","Could not read cluster size distribution in File.\nExecution aborted.");
  infile.close();
  //file is read

  //Linear extrapolation to get the value at the right e field

  double inTable=GetPositionOfE(noent, e);
  /*The down rounded value of inTable is the index of the e field (in e)
    that is smaller than _E.
    (Exception: neg. Value , if _E is smaller than the smallest value in e)
    The decimal places render about the position between two entries in e*/

  if (inTable < 0 || inTable > noent -1)
    Warning("BmnGemGas::BmnGemGas","E field out of the range defined in input file");
  _VDrift = LinExpolation(inTable, vdrift, noent);
  _Dl = LinExpolation(inTable, dl, noent);
  _Dt = LinExpolation(inTable, dt, noent);
  _k = LinExpolation(inTable, k, noent);
  
  //Define the constant for the quadratic CSD extrapolation in such a way
  //that the csd is continous (the inverse quadratic approximation
  //is rather improper anyhow)
  _CSDEpol = _CSD[_nCSD-1]*(_nCSD-1)*(_nCSD-1);
}

BmnGemGas::~BmnGemGas(){}

void BmnGemGas::operator=(const BmnGemGas& GasToCopy)
{
  _VDrift = GasToCopy._VDrift;
  _Dl = GasToCopy._Dl;
  _Dt = GasToCopy._Dt;
  _k = GasToCopy._k;
  _W = GasToCopy._W;
  _CSDNorm= GasToCopy._CSDNorm;
  _CSDEpol= GasToCopy._CSDEpol;

  _E = GasToCopy._E;
  _B = GasToCopy._B;
  _T = GasToCopy._T;
  _p = GasToCopy._p;
  
  _CSD=GasToCopy._CSD;
}

// Use Uniform generator and correct bug with not properly normalized table
int
BmnGemGas::GetRandomCSUniform() const 
{
  double r = gRandom->Uniform();
  while (r > _CSDNorm )
     r = gRandom->Uniform();
   
  return GetRandomCS(r);
}


//return wrong number if r > overall sum
int
BmnGemGas::GetRandomCS(double const r) const {
  int i=0;
  double sum=_CSD[0];
  while(r>sum && ++i<_CSD.size()){
    sum+=_CSD[i];
  }
  if (_CSDEpol > 0)
    while(r>sum && i < 100)//sum could converge < 1
    {
      sum += _CSDEpol/i/i++;
    }
  return i + 1;
}

void
BmnGemGas::SetCSD(const std::vector<double>& CSD)
{
  _CSD=CSD;
}

std::ostream& operator<< (std::ostream& stream, const BmnGemGas& g)
{
  stream << "--------------------------------------------------------\n"
	 << "Tpc gas parameters: \n"
	 << "       drift velocity  VDrift="<<g._VDrift<<"cm/ns \n"
	 << "       long.diffusion  Dl    ="<<g._Dl<<"sqrt(cm) \n"
	 << "       trans.diffusion Dt    ="<<g._Dt<<"sqrt(cm) \n"
	 << "       attachment      k     ="<<g._k<<"1/cm\n"
	 << "       eff. ionisation W     ="<<g._W<<"eV \n"
         << "       ClusterSizeDistribution CSD:";
  for (int i=0; i<g.nCSD();i++)
  {
    if (i%3 == 0)
      stream<<"\n       ";
    stream<<i+1<<": "<<g._CSD[i]<<"  ";
  }
  stream << "\n"
         << "       extrapol. const CSDEpol="<<g._CSDEpol 
	 << "\n"
	 << "Gas parameters given for this environment: \n"
	 << "       drift field     E="<<g._E<<"V/cm \n"
	 << "       magnetic field  B="<<g._B<<"T \n"
	 << "       pressure        p="<<g._p<<"mbar \n"
	 << "       temperature     T="<<g._T<<"K \n"
	 << "--------------------------------------------------------\n";
  return stream;
}

int
BmnGemGas::ReadGasBegin(std::ifstream* const pinfile)
{
  int noent = 0;                       //Nr. of E fields
  (*pinfile).ignore(256, '\n' );
  (*pinfile).ignore(256, '\n' );
  (*pinfile).ignore(256, '\n' );
  (*pinfile).ignore(256, ':');
  (*pinfile) >> _T;                        //temperature
  (*pinfile).ignore(256, ':');
  (*pinfile) >> _p;                        //pressure
  (*pinfile).ignore(256, ':');
  (*pinfile) >> _B;                        //b field
  (*pinfile).ignore(256, ':' );
  (*pinfile).ignore(256, ':');
  (*pinfile) >> noent;
  return(noent);
}

int
BmnGemGas::ReadGasArrays(std::ifstream* const pinfile, int const noent,
		      double* const e, double* const vdrift, double* const dt,
		      double* const dl, double* const k)
{
  int nCSDFile = 0;                            //Nr. of ClusterSizes
  for (int i=0; i < noent;i++)
    (*pinfile) >> e[i];
  (*pinfile).ignore(256, ':' );
  for (int i=0; i < noent;i++)
    (*pinfile) >> vdrift[i];
  (*pinfile).ignore(256, ':' );
  for (int i=0; i < noent;i++)
    (*pinfile) >> dt[i];
  (*pinfile).ignore(256, ':' );
  for (int i=0; i < noent;i++)
    (*pinfile) >> dl[i];
  (*pinfile).ignore(256, ':' );
  for (int i=0; i < noent;i++)
    (*pinfile) >> k[i];
  //ignore ion-mobility (in this version)
  (*pinfile).ignore(256, ':' );
  (*pinfile).ignore(256, '\n' );
  for (int i=0; i < noent;i++)
    (*pinfile).ignore(256, '\n' );
  //Cluster size distribution
  (*pinfile).ignore(512, ':' );
  (*pinfile) >>  _W;                                  //effective ionisation
  (*pinfile).ignore(512, ':');
  (*pinfile).ignore(256, ':');
  (*pinfile) >> nCSDFile;
  return(nCSDFile);
 }

const double BmnGemGas::GetPositionOfE(int const noent, const double* const e)
{
  double inTable=0;
  for (int i=0; i<noent; i++)
   {
     if (e[i] >= _E && i == 0)
     {
       inTable = i - (e[i]-_E)/(e[1]-e[0]);
       break;
     }
     if (i == noent-1 || 
 	(e[i] >= _E && i != 0))
     {
       inTable = i - (e[i]-_E)/(e[i]-e[i-1]);
       break;
     }
  }
  return(inTable);
}

const double BmnGemGas::LinExpolation(double const inTable, const double* const table,
			   int const nTable)
{
  if (nTable == 1)
    return(table[0]);
  else if (inTable <= 0)
    return(table[0] + inTable * (table[1]-table[0]));
  else if (inTable > nTable-1)
    return(table[nTable-1] + 
	   (inTable-nTable+1) * (table[nTable-1]-table[nTable-2]));
  else
    for (int i=1; i<nTable; i++)
    {
      if (i >= inTable)
	return(table[i] - (i-inTable) * (table[i]-table[i-1]));
    }
  return(0);//never reached; compiling without this I get an annoying warning 
}
