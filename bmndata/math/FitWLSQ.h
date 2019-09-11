#include "TH1.h"

#ifndef FITWLSQ_H
#define FITWLSQ_H 1

//
//Advanced refit for tracks by Andrey Moshkin
//
class FitWLSQ {
	Int_t NPoints;
	Int_t NParams;
public:
	FitWLSQ(Double_t *xx, Double_t cSigma1 = 0.01, Double_t cSigma2 = 0.07, Double_t cP1 = 0.9,
			Int_t nNPoints = 6, Int_t nNParams = 2, Bool_t RBF = kTRUE, Bool_t LHf = kTRUE, Int_t RBfn = 7); // constructor
	~FitWLSQ(); // destructor
	Double_t param[3]; // fit parameters
	Double_t cov[3][3]; // covariance matrix
	Int_t numItera;
	Double_t *wrb;  // robust weights
	Bool_t Status;

	Bool_t Fit(Double_t *y);

	Double_t WLSQRms(Double_t *pdY);

private:
	Double_t det3(Double_t a[3][3]);

	Double_t alg_add(Double_t a[3][3], Int_t row, Int_t column);

	void cov_matrix(Double_t a[3][3]);

	Double_t f_lh(Double_t *yy, Double_t *zz, Double_t *w, Double_t *par);

	Bool_t Initial(Double_t *trk);

	void CheckMatrix(Double_t *pdA, Double_t *pdB, TString sz);

	Bool_t SymMatrix(Double_t *pdA, Double_t *pdB);

	Bool_t WLSQFit(Double_t *pdY);

	Double_t WLSQGet(Double_t dX);

	Bool_t par_check(Double_t *par, Double_t *par1);

	void RB(Double_t *yy, Int_t);

	void RBN(Double_t *yy, Int_t);

	void RBM(Double_t *yy, Int_t);

	void RB_OPT(Double_t *yy, Int_t);

	void CovarianceMatrixPrint(void);

	Double_t *sigmaF;  // weights
	Double_t *w;  // weights
	Double_t *x;  // x coordinats
	Double_t sigma_k; // robust sigma
	Double_t Sigma1;
	Double_t Sigma2;
	Double_t P1;
	Double_t resolution; // resolution
	Bool_t RB_FIT;
	Bool_t LH_INI;
	Int_t RB_TYPE;
};

#endif
