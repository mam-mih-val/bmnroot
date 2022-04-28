
/*************************************************************************************
 *
 *            BmnScWallDigit
 *    Class for digital data taken from BmnScWall detector
 *
 *  Author:   Sergey Morozov
 *  Version:  17-03-2022
 *
 ************************************************************************************/

#include "BmnScWallDigit.h"

#include <iostream>
using std::cout;
using std::endl;

// -------------------------------------------------------------------------

BmnScWallDigit::BmnScWallDigit()
{
  fCellID = -1;
  fELoss     = 0;
  fELossDigi = 0;
}

// -------------------------------------------------------------------------

BmnScWallDigit::BmnScWallDigit(Int_t pfCellID, Double_t   pfELoss, Double_t   pfELossDigi)
{
  fCellID = pfCellID;
  fELoss = pfELoss;
  fELossDigi = pfELossDigi;

}

// -------------------------------------------------------------------------

BmnScWallDigit::~BmnScWallDigit()
{

}


// -------------------------------------------------------------------------

void BmnScWallDigit::Clear()
{
  fELossDigi = 0;
  fELoss = 0;
}


// -------------------------------------------------------------------------

void BmnScWallDigit::Print(const Option_t* opt)
{
  cout << " BmnScWallDigit CellID:" << fCellID <<
  " ELossDigi:" << fELossDigi<< " ELoss: " << fELoss << endl;
}

// -------------------------------------------------------------------------

ClassImp(BmnScWallDigit)
