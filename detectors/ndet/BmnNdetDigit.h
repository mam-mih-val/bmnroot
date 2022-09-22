/*************************************************************************************
 *
 *            BmnNdetDigit 
 *    Class for digital data taken from BmnNdet detector 
 *         
 *  Author:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  18-11-2015
 *  Modified by M.Golubeva July 2022
 *
 ************************************************************************************/

#ifndef BMNNDETDIGIT_H
#define BMNNDETDIGIT_H

#include "BmnNdetPoint.h"

class BmnNdetDigit: public TObject
{
 public:

  BmnNdetDigit();
  //BmnNdetDigit(Int_t pfGroupID, Int_t pfModuleID, Int_t pfChannelID, Double_t   pfELoss, Double_t   pfELossDigi=0 );
  BmnNdetDigit(Double_t pfTime, Int_t pfModuleID, Int_t pfChannelID, Double_t   pfELoss );
  BmnNdetDigit(BmnNdetPoint *p);

  virtual ~BmnNdetDigit();
  void Clear();
  
  void InitStatic();
  virtual void Print(const Option_t* opt ="");


   inline Int_t GetSectionZ() { return fSectionZ; }
   inline Int_t GetModuleX() { return fModuleX; }
   inline Int_t GetModuleY() { return fModuleY; }

   //inline Int_t  GetGroupID() { return fGroupID; } 
  inline Double_t  GetTime() { return fTime; } 
  inline Int_t  GetModuleID() { return fModuleID; } 
  inline Int_t  GetChannelID()  { return fChannelID; } 
  inline Double_t GetELossDigi()  { return fELossDigi; } 
  inline Double_t  GetELoss()               { return fELoss; }
   inline Int_t    GetADCBits() { return fADCBits; }
   inline Double_t GetADCResolution() { return fADCResolution; }
   inline Double_t GetEnergyDigiThreshold() { return fEnergyDigiThreshold; }
   inline Double_t GetELossReco() { return fELossReco; }

   inline char   GetWasInitialized() { return fWasInitialized; }

   //inline Int_t    SetGroupID(UInt_t pfGroupID)         { fGroupID=pfGroupID;       return fGroupID; } 

   inline Double_t SetTime(Double_t pfTime)         
   { fTime=pfTime;       
     return fTime; 
   } 
   
   inline Int_t SetChannelID(UInt_t pfChannelID) 
   { 
     fChannelID=pfChannelID;         
     return fChannelID; 
   } 
   
   inline Int_t SetModuleID(UInt_t pfModuleID)
   {
      fModuleID = pfModuleID;
      return fModuleID;
   }
   
   inline Double_t SetSectionZ(Double_t pfSectionZ)
   {
      fSectionZ = pfSectionZ;
      return fSectionZ;
   }
   
   inline Double_t SetModuleX(Double_t pfModuleX)
   {
      fModuleX = pfModuleX;
      return fModuleX;
   }
   inline Double_t SetModuleY(Double_t pfModuleY)
   {
      fModuleY = pfModuleY;
      return fModuleY;
   }

   inline Int_t SetADCBits(Int_t pfADCBits)
   {
      fADCBits = pfADCBits;
      return fADCBits;
   }
   inline Double_t SetADCResolution(Int_t pfADCResolution)
   {
      fADCResolution = pfADCResolution;
      return fADCResolution;
   }
   inline Double_t SetEnergyDigiThreshold(Int_t pfEnergyDigiThr)
   {
      fEnergyDigiThreshold = pfEnergyDigiThr;
      return fEnergyDigiThreshold;
   }

   inline Double_t AddEloss (Double_t pfELoss)
   { 
     fELoss += pfELoss;  
     return fELoss;
   }

   UInt_t   AddNdetPoint (BmnNdetPoint *p);

   UInt_t Convert()
   {
      fELossDigi = ADC(fELoss);
      fELoss     = 0;
      return fELossDigi;
   }
   UInt_t ConvertSim()
   {
      fELossDigi = ADC(fELoss);
      return fELossDigi;
   }

   UInt_t   ADC(Double_t pfELoss);

   Double_t SetELossReco(Double_t pfELoss)
   {
      fELossReco = pfELoss;
      return fELossReco;
   }

 protected:


    static char
      fWasInitialized;       // 0 - from built-in constants; 1 - was attempt to read; 2 - success from MpdNdetDigiScheme
   static Int_t    fADCBits; // Hardware parameter
   static Double_t fADCResolution; // Hardware parameter
   static Double_t fEnergyDigiThreshold;

   //Int_t    fGroupID;         // Module Group number = "size" (1: central small, 2 - periferal big, can be also 0 - for hardware summators)
   Double_t    fTime;         // time in channel
  Int_t    fModuleID;        // Module number inside module group (36 central small modules, up to 58 periferal big modules)
  Int_t    fChannelID;       // Hardware NDET channel number (corresponds to the ascii map file)
  Double_t   fELossDigi;     // Digital response of the NDET channel (taken from experimental data)
  Double_t fELoss;           // Sum of the energy losses from MC or from experimental data after calibration
   Double_t fELossReco;  // Sum of the energy losses as analog simulated response of the detector/channel

   Double_t fSectionZ;    // Section coordinate
   Double_t fModuleX;    // Module X coordinate
   Double_t fModuleY;    // Module Y coordinate

  ClassDef(BmnNdetDigit,3);

};

#endif // BMNNDETDIGIT_H
