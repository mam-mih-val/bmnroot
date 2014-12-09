#ifndef BMNMATRIXMATH_H_
#define BMNMATRIXMATH_H_

#include <vector>
#include "Rtypes.h"
using namespace std;
//Matrix operations

/* 5x5 symmetric matrix inverse */
Bool_t InvSym15(vector<Double_t>& a);

/* Multiplication of two 5x5 matrices */
Bool_t Mult25(const vector<Double_t>& a, const vector<Double_t>& b, vector<Double_t>& c);

/* Multiplication of 5x5 matrix and 5x1 vector */
Bool_t Mult25On5(const vector<Double_t>& a, const vector<Double_t>& b, vector<Double_t>& c);

/* Multiplication of 5x5 matrix and 5x1 vector */
Bool_t Mult15On5(const vector<Double_t>& a, const vector<Double_t>& b, vector<Double_t>& c);

/* Transpose of 5x5 matrix */
Bool_t Transpose25(vector<Double_t>& a);

/* Subtraction of two matrices*/
Bool_t Subtract(const vector<Double_t>& a, const vector<Double_t>& b, vector<Double_t>& c);

/* Addition of two matrices*/
Bool_t Add(const vector<Double_t>& a, const vector<Double_t>& b, vector<Double_t>& c);

/* a*b*a^T */
Bool_t Similarity(const vector<Double_t>& a, const vector<Double_t>& b, vector<Double_t>& c);

/* Multiplication of two 5x5 matrices */
Bool_t Mult15On25(const vector<Double_t>& a, const vector<Double_t>& b, vector<Double_t>& c);

/* Multiplication of two 5x5 matrices */
Bool_t Mult25On15(const vector<Double_t>& a, const vector<Double_t>& b, vector<Double_t>& c);

#endif

