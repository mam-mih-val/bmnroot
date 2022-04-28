
/*************************************************************************************
 *
 *            BmnHodoDigit
 *    Class for digital data taken from BmnHodo detector
 *
 *  Author:   Sergey Morozov
 *  Version:  17-03-2022
 *
 ************************************************************************************/

#include "BmnHodoDigit.h"

#include <iostream>
using std::cout;
using std::endl;

// -------------------------------------------------------------------------

BmnHodoDigit::BmnHodoDigit()
{
  fStripID = -1;
  fELoss     = 0;
  fELossDigi = 0;
}

// -------------------------------------------------------------------------

BmnHodoDigit::BmnHodoDigit(Int_t pfStripID, Double_t   pfELoss, Double_t   pfELossDigi)
{
  fStripID = pfStripID;
  fELoss = pfELoss;
  fELossDigi = pfELossDigi;

}

// -------------------------------------------------------------------------

BmnHodoDigit::~BmnHodoDigit()
{

}


// -------------------------------------------------------------------------

void BmnHodoDigit::Clear()
{
  fELossDigi = 0;
  fELoss = 0;
}


// -------------------------------------------------------------------------

void BmnHodoDigit::Print(const Option_t* opt)
{
  cout << " BmnHodoDigit StripID:" << fStripID <<
  " ELossDigi:" << fELossDigi<< " ELoss: " << fELoss << endl;
}

// -------------------------------------------------------------------------

ClassImp(BmnHodoDigit)
