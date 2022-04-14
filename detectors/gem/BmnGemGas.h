#ifndef BMNGEMGAS_HH
#define BMNGEMGAS_HH

#include <assert.h>
#include <ostream>
#include <vector>
#include <string>

enum gascomponents {Ne,Ar,CO2,CH4};

class BmnGemGas {
public:
  // constructors
  BmnGemGas();
  ~BmnGemGas();
  
  BmnGemGas(double const E,
	 double const B,
	 double const T,
	 double const p,
	 double const VDrift,
	 double const Dl,
	 double const Dt,
	 double const k,
	 double const W,
	 const std::vector<double>& CSD,
	 double const CSDEpol);

  BmnGemGas(const std::string& Filename,
	 double const E);

  // accessors
  double VDrift() const {return _VDrift;} 
  double Dl() const {return _Dl;}      
  double Dt() const {return _Dt;}  
  double VDrift(double const dE, double const dB) const {return _VDrift;}
  double Dl(double const dE, double const dB) const {return _Dl;}
  double Dt(double const dE, double const dB) const {return _Dt;}
  double k() const {return _k;}       
  double W() const {return _W;}       
  double CSD(int i) const {return _CSD.at(i);}
  const std::vector<double>& CSD() const {return _CSD;}
  int nCSD() const {return _CSD.size();}
  double CSDNorm() const {return _CSDNorm;}
  double CSDEpol() const {return _CSDEpol;} 

  double E() const {return _E;}
  double B() const {return _B;}
  double T() const {return _T;}       
  double p() const {return _p;}
    
  int GetRandomCS(double const r) const;   //has problem with not normalized table
  int GetRandomCSUniform() const;

  void PrintAll(std::ostream& s) const {s<<*this;}

  void operator=(const BmnGemGas& GasToCopy);
  friend std::ostream& operator<< (std::ostream&, const BmnGemGas&);

  // modifiers
  void SetE(double const dE){_E=dE;}  // later this method should also retrieve
                                      // updated gas values for the new field!
  void SetB(double const dB){_B=dB;}
  void SetT(double const dT){_T=dT;}
  void Setp(double const dp){_p=dp;}
  void SetCSD(const std::vector<double>& CSD);
  void SetCSDEpol(double const dCSDEpol){_CSDEpol=dCSDEpol;}

private:
  double _E;        // electric field [V/cm](some gas parameters depend on it)
  double _B;        // B field [T] assumption: B || E !!! 
  double _T;        // Temperature [K]
  double _p;        // pressure [mbar]

  double _VDrift;   // electron Drift velocity [cm/ns]
  double _Dl;       // longitudinal diffusion coefficient [sqrt(cm)]
  double _Dt;       // transversal diffusion coefficient [sqrt(cm)]
  double _k;        // attachment coefficient[1/cm]
  double _W;        // effective ionisation energy [eV]
  std::vector<double> _CSD;     // Cluster size distribution
  double _CSDNorm;                 // Intergral of CSD must be 1 but actually not. Used for correction.
  double _CSDEpol;  // Constant used for the inverse quadratic extrapolation
                    // of Cluster Sizes bigger than _nCSD

  const double LinExpolation(double const inTable, const double* const table,
		       int const nTable);
  int ReadGasBegin(std::ifstream* const pinfile);
  int ReadGasArrays(std::ifstream* const pinfile, int const noent,
		    double* const e, double* const vdrift, double* const dt,
		    double* const dl, double* const k); 
  const double GetPositionOfE(int const noent, const double* const e);
 

};

#endif
