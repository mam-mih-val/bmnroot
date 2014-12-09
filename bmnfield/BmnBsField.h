// -------------------------------------------------------------------------
// -----                      BmnBsField  header file                  -----
// -----                Created 12/05/06  by E.I.Litvinenko            -----
// -------------------------------------------------------------------------


/** BmnBsField.h
 ** @author E.I.Litvinenko <litvin@nf.jinr.ru>
 ** @since 12.05.2006
 ** @version1.0
 **
 ** Class for magnetic field in CBM calculated using B-splines in CBM 
 ** B-spline coefficients stored in root file (replacing field map root file)
 ** 
 **
 ** Note: algorithm was written in Fortran by P.G.Akisin <akishin@jinr.ru>
 **/

#ifndef CBMBSF_H
#define CBMBSF_H 

#include "BmnFieldMap.h"

#include "TString.h"

class BmnFieldPar;

class TArrayF;

class BmnBsField :public BmnFieldMap{
public:
	BmnBsField(const char *pBsName, Int_t  symType=3);
	BmnBsField();
	BmnBsField(BmnFieldPar* fieldPar);

	virtual ~BmnBsField();

	/** Initialisation (read Bs coefficients from root file) **/
	void Init();

	void writeBsRootfile(const char *name);    // Write Field Splined  to root file
	void readBsRootfile (const char *name);    // Read  Field Splined  from root file
	void readFortranAsciifiles(const char *MapFile1,const char *MapFile2,const char *MapFile3);  // for the first use only
	
	void GetFieldValue(const Double_t Point[3],Double_t *Bfield); 

	void CalculateMapFromBs(Int_t pNx=0, Int_t pNy=0, Int_t pNz=0);   // calculate map pNx,pNy,Pnz dim (default - LL1-3,LL2-3,LL3-3)


	TArrayF	*GetBsBx(){return fBsBx;}
	TArrayF	*GetBsBy(){return fBsBy;}
	TArrayF	*GetBsBz(){return fBsBz;}
	TArrayF	*GetX(){return fX;}
	TArrayF	*GetY(){return fY;}
	TArrayF	*GetZ(){return fZ;}

	void GetBsName(char *pBsName)       { strcpy ( pBsName , fBsName.Data() ); }
	void SetBsName(const char *pBsName) { fBsName = pBsName;   this->SetName(pBsName); }

	Double_t GetBx(Double_t x, Double_t y, Double_t z);
	Double_t GetBy(Double_t x, Double_t y, Double_t z);
	Double_t GetBz(Double_t x, Double_t y, Double_t z);

	
	// private:
 protected:

	Int_t NDIM , LL1 , LL2 , LL3 , II1 , II2 , II3;

	TArrayF		*fBsBx;           // NDIM coefficients for Bx
	TArrayF		*fBsBy;           // NDIM coefficients for By 
	TArrayF		*fBsBz;           // NDIM coefficients for Bz 
	TArrayF		*fX;              // LL1 x coordinates of grid points 
	TArrayF		*fY;              // LL2 y coordinates of grid points 
	TArrayF		*fZ;              // LL3 z coordinates of grid points 

	Float_t *UX1; //! [LL1]
	Float_t *UX2; //! [LL2]
	Float_t *UX3; //! [LL3]
	Float_t *F0 ; //! [NDIM]
	Float_t *G0 ; //! [NDIM]
	Float_t *U0;  //! [NDIM]

	TString fBsName;                  // basic field name used in Bs root file
protected:

	void PALC0(Double_t X,Double_t Y,Double_t Z,Double_t *BX,Double_t *BY,Double_t *BZ);    // from Fortran code
	Float_t SPL0(Double_t T,Double_t X0,Double_t X1,Double_t X2,Double_t X3,Double_t X4);   // from Fortran code

 private:
  BmnBsField(const BmnBsField&);
  BmnBsField& operator=(const BmnBsField&);

  ClassDef(BmnBsField,2) 
};

#endif
