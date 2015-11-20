
/*************************************************************************************
 *
 *            BmnZdcDigi 
 *    Class for digital data taken from BmnZdc detector 
 *         
 *  Author:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  18-11-2015
 *
 ************************************************************************************/

#include "BmnZdcDigi.h"
#include "BmnZdcDigiScheme.h"

#include <iostream>
using std::cout;
using std::endl;

// -------------------------------------------------------------------------

BmnZdcDigi::BmnZdcDigi()
{
  fGroupID = -1;
  fModuleID = -1;
  fChannelID = -1;
  fELoss     = 0;
  fELossDigi = 0;
}

// -------------------------------------------------------------------------

BmnZdcDigi::BmnZdcDigi(Int_t pfGroupID, Int_t pfModuleID, Int_t pfChannelID, Double_t   pfELoss, Double_t   pfELossDigi)
{
  fGroupID = pfGroupID;
  fModuleID = pfModuleID;
  fChannelID = pfChannelID;
  fELoss = pfELoss;
  fELossDigi = pfELossDigi;

  //  InitStatic();
}

// -------------------------------------------------------------------------

BmnZdcDigi::BmnZdcDigi(BmnZdcPoint *p)
{
  fGroupID = -1;
  fModuleID = -1;
  fChannelID = -1;
  fELoss     = 0;
  fELossDigi = 0;

  BmnZdcDigiScheme *pDigiScheme  = BmnZdcDigiScheme::Instance();

  if ((pDigiScheme)&&(p)) { 

    Int_t module_groupID, modID, chanID;
    //    pDigiScheme->SplitDigiID(pDigiScheme->GetDigiIdFromCoords(p->GetX(),p->GetY(),p->GetZ()),module_groupID, modID, chanID);

    pDigiScheme->SplitDigiID(pDigiScheme->GetDigiIdFromVolumeData(p->GetDetectorID(),p->GetCopyMother() ),module_groupID, modID, chanID);


    fGroupID = module_groupID;
    fModuleID = modID;
    fChannelID = chanID;

    fELoss = p->GetEnergyLoss();
  }

  //  InitStatic();
}


// -------------------------------------------------------------------------

BmnZdcDigi::~BmnZdcDigi()
{

}
  

// -------------------------------------------------------------------------

void BmnZdcDigi::Clear()
{
  fELossDigi = 0;
  fELoss = 0;
}


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void BmnZdcDigi::InitStatic()
{
  Bool_t do_init = 1;
  if (do_init) {
    Clear();
    BmnZdcDigiScheme *pDigiScheme  = BmnZdcDigiScheme::Instance();

    if (pDigiScheme) { 

      BmnZdcDigiPar*    pZdcDigiPar=0;

      pZdcDigiPar = pDigiScheme->GetZdcDigiPar();
    }
  }
}

// -------------------------------------------------------------------------

void BmnZdcDigi::Print(const Option_t* opt)
{
  cout << " BmnZdcDigi  ModuleGroupID:" << fGroupID  << " ModuleID:" << fModuleID<< " ChanID:" << fChannelID<< 
  " ELossDigi:" << fELossDigi<< " ELoss: " << fELoss << endl;
}

// -------------------------------------------------------------------------

UInt_t BmnZdcDigi::AddZdcPoint (BmnZdcPoint *p)
{
  BmnZdcDigiScheme *pDigiScheme  = BmnZdcDigiScheme::Instance();

  if ((pDigiScheme)&&(p)) { 

    Int_t module_groupID, modID, chanID;
    //    pDigiScheme->SplitDigiID(pDigiScheme->GetDigiIdFromCoords(p->GetX(),p->GetY(),p->GetZ()),module_groupID, modID, chanID);
    pDigiScheme->SplitDigiID(pDigiScheme->GetDigiIdFromVolumeData(p->GetDetectorID(),p->GetCopyMother() ),module_groupID, modID, chanID);

    if ((fGroupID == module_groupID)&&(fModuleID == modID)&&(fChannelID == chanID))
      fELoss += p->GetEnergyLoss();
  }

  return 0;
}

// -------------------------------------------------------------------------

ClassImp(BmnZdcDigi)
