
/*************************************************************************************
 *
 *            BmnFHCalDigit
 *    Class for digital data taken from BmnFHCal detector
 *
 *  Author:   Sergey Morozov
 *  Version:  17-03-2022
 *
 ************************************************************************************/

#include "BmnFHCalDigit.h"

#include <iostream>
using std::cout;
using std::endl;

// -------------------------------------------------------------------------

BmnFHCalDigit::BmnFHCalDigit()
{
  fModuleID = -1;
  fSectionID = -1;
  fELoss     = 0;
  fELossDigi = 0;
}

// -------------------------------------------------------------------------

BmnFHCalDigit::BmnFHCalDigit(Int_t pfModuleID, Int_t pfSectionID, Double_t   pfELoss, Double_t   pfELossDigi)
{
  fModuleID = pfModuleID;
  fSectionID = pfSectionID;
  fELoss = pfELoss;
  fELossDigi = pfELossDigi;

}

// -------------------------------------------------------------------------

BmnFHCalDigit::~BmnFHCalDigit()
{

}


// -------------------------------------------------------------------------

void BmnFHCalDigit::Clear()
{
  fELossDigi = 0;
  fELoss = 0;
}


// -------------------------------------------------------------------------

void BmnFHCalDigit::Print(const Option_t* opt)
{
  cout << " BmnFHCalDigit ModuleID:" << fModuleID<< " SectionID:" << fSectionID<<
  " ELossDigi:" << fELossDigi<< " ELoss: " << fELoss << endl;
}

// -------------------------------------------------------------------------

ClassImp(BmnFHCalDigit)
