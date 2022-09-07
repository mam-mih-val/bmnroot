
/*************************************************************************************
 *
 *            BmnNdetDigi 
 *    Class for digital data taken from BmnNdet detector 
 *         
 *  Author:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  18-11-2015
 *  Modified by M.Golubeva July 2022
 *
 ************************************************************************************/

#include "BmnNdetDigi.h"
#include "BmnNdetDigiScheme.h"

#include <iostream>
using std::cout;
using std::endl;

static const Int_t    kADCBits             = 30;   // build-in constant
static const Double_t kADCResolution       = 1e-9; // build-in constant
static const Double_t kEnergyDigiThreshold = 0;    // build-in constant

char BmnNdetDigi::fWasInitialized = 0;

Int_t    BmnNdetDigi::fADCBits             = kADCBits;
Double_t BmnNdetDigi::fADCResolution       = kADCResolution;
Double_t BmnNdetDigi::fEnergyDigiThreshold = kEnergyDigiThreshold;

// -------------------------------------------------------------------------

BmnNdetDigi::BmnNdetDigi()
{
  //fGroupID = -1;
  fTime = -100;
  fModuleID = -1;
  fChannelID = -1;
  fELoss     = 0;
  fELossDigi = 0;
   fELossReco  = 0.;
   fSectionZ    = -1;
   fModuleX    = -1;
   fModuleY    = -1;
}

// -------------------------------------------------------------------------

//BmnNdetDigi::BmnNdetDigi(Int_t pfGroupID, Int_t pfModuleID, Int_t pfChannelID, Double_t   pfELoss, Double_t   pfELossDigi)
//BmnNdetDigi::BmnNdetDigi(Int_t pfGroupID, Int_t pfModuleID, Int_t pfChannelID, Double_t   pfELoss)
BmnNdetDigi::BmnNdetDigi(Double_t pfTime, Int_t pfModuleID, Int_t pfChannelID, Double_t   pfELoss)
{
  //fGroupID = pfGroupID;
  fTime = pfTime;
  fModuleID = pfModuleID;
  fChannelID = pfChannelID;
  fELoss = pfELoss;
  fELossDigi = 0;

    InitStatic();
}

// -------------------------------------------------------------------------

BmnNdetDigi::BmnNdetDigi(BmnNdetPoint *p)
{
  //fGroupID = -1;
  fTime = -100;
  fModuleID = -1;
  fChannelID = -1;
  fELoss     = 0;
  fELossDigi = 0;

  BmnNdetDigiScheme *pDigiScheme  = BmnNdetDigiScheme::Instance();

  if ((pDigiScheme)&&(p)) { 
    /*
    Int_t module_groupID, modID, chanID;
        pDigiScheme->SplitDigiID(pDigiScheme->GetDigiIdFromCoords(p->GetX(),p->GetY(),p->GetZ()),module_groupID, modID, chanID);

    //pDigiScheme->SplitDigiID(pDigiScheme->GetDigiIdFromVolumeData(p->GetDetectorID(),p->GetCopyMother() ),module_groupID, modID, chanID);
    */
    Int_t modID, chanID;
    Double_t time;
    //pDigiScheme->SplitDigiID(pDigiScheme->GetDigiIdFromCoords(p->GetX(),p->GetY(),p->GetZ()),time, modID, chanID);


	//fGroupID = module_groupID;
    fTime = time;
    fModuleID = modID;
    fChannelID = chanID;

    fELoss = p->GetEnergyLoss();
  }

    InitStatic();
}


// -------------------------------------------------------------------------

BmnNdetDigi::~BmnNdetDigi()
{

}
  

// -------------------------------------------------------------------------

void BmnNdetDigi::Clear()
{
   fWasInitialized      = 0;
   fADCBits             = kADCBits;
   fADCResolution       = kADCResolution;
   fEnergyDigiThreshold = kEnergyDigiThreshold;
  fELossDigi = 0;
  fELoss = 0;
}


// -------------------------------------------------------------------------
// -------------------------------------------------------------------------

void BmnNdetDigi::InitStatic()
{
   Bool_t do_init = ((!fWasInitialized));
   //Bool_t do_init = 1;
  if (do_init) {
    Clear();
    fWasInitialized               = 1;
    BmnNdetDigiScheme *pDigiScheme  = BmnNdetDigiScheme::Instance();

    if (pDigiScheme) { 

      BmnNdetDigiPar*    pNdetDigiPar=0;

      pNdetDigiPar = pDigiScheme->GetNdetDigiPar();

         if (pNdetDigiPar) {
            fADCBits             = pNdetDigiPar->GetADCBits();
            fADCResolution       = pNdetDigiPar->GetADCResolution();
            fEnergyDigiThreshold = pNdetDigiPar->GetEnergyDigiThreshold();
            fWasInitialized      = 2;
         }

    }
  }
}

// -------------------------------------------------------------------------

void BmnNdetDigi::Print(const Option_t* opt)
{
  /*
  cout << " BmnNdetDigi  ModuleGroupID:" << fGroupID  << " ModuleID:" << fModuleID<< " ChanID:" << fChannelID<< 
  " ELossDigi:" << fELossDigi<< " ELoss: " << fELoss << endl;
  */
  /*
   cout << " MpdNdetDigi  DetID:" << fGroupID << " ModuleID:" << fModuleID << " ChanID:" << fChannelID
        << " ELossDigi:" << fELossDigi << " ELoss: " << fELoss << "  [Bits:" << fADCBits
        << ", Resol:" << fADCResolution << ", Thresh:" << fEnergyDigiThreshold << ", Init:" << fWasInitialized << "]"
        << endl;
  */
   cout << " MpdNdetDigi  DetID:" << fTime << " ModuleID:" << fModuleID << " ChanID:" << fChannelID
        << " ELossDigi:" << fELossDigi << " ELoss: " << fELoss << "  [Bits:" << fADCBits
        << ", Resol:" << fADCResolution << ", Thresh:" << fEnergyDigiThreshold << ", Init:" << fWasInitialized << "]"
        << endl;
}

// -------------------------------------------------------------------------

UInt_t BmnNdetDigi::AddNdetPoint (BmnNdetPoint *p)
{
  BmnNdetDigiScheme *pDigiScheme  = BmnNdetDigiScheme::Instance();

  if ((pDigiScheme)&&(p)) { 
    /*
    Int_t module_groupID, modID, chanID;
        pDigiScheme->SplitDigiID(pDigiScheme->GetDigiIdFromCoords(p->GetX(),p->GetY(),p->GetZ()),module_groupID, modID, chanID);
    //pDigiScheme->SplitDigiID(pDigiScheme->GetDigiIdFromVolumeData(p->GetDetectorID(),p->GetCopyMother() ),module_groupID, modID, chanID);

    if ((fGroupID == module_groupID)&&(fModuleID == modID)&&(fChannelID == chanID))
      fELoss += p->GetEnergyLoss();
    */
    Int_t modID, chanID;
    Double_t time;
    //pDigiScheme->SplitDigiID(pDigiScheme->GetDigiIdFromCoords(p->GetX(),p->GetY(),p->GetZ()),time, modID, chanID);

	if ((fModuleID == modID)&&(fChannelID == chanID)) {
	    fELoss += p->GetEnergyLoss();
	    fTime = p->GetTime();
	  }
  }

  return 0;
}

// -------------------------------------------------------------------------

UInt_t BmnNdetDigi::ADC(Double_t pfELoss)
{
   //  if (pfELoss<=fEnergyDigiThreshold) {
   if (pfELoss > 0) {
      UInt_t tmp = 0;
      tmp        = (UInt_t)(pfELoss / fADCResolution);
      return (tmp & ((2 << (fADCBits - 1)) - 1));
   } else
      return 0;
}

// -------------------------------------------------------------------------

ClassImp(BmnNdetDigi)
