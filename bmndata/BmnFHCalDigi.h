/* Copyright (C) 2021 Institute for Nuclear Research, Moscow
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Nikolay Karpushkin [committer] */

/** \file BmnFHCalDigi.h
 ** \author Nikolay Karpushkin <karpushkin@inr.ru>
 ** \date 09.10.2019
 **/

/** \class BmnFHCalDigi
 ** \brief Data class for Bmn FHCal digital signal processing
 ** \version 1.0
 **/

#ifndef BmnFHCalDigi_H
#define BmnFHCalDigi_H 1

#include "TObject.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "BmnDetectorList.h"  // for kFHCaL
#include "BmnFHCalAddress.h"  // for BmnFHCalAddress


#include <Rtypes.h>      // for THashConsistencyHolder, ClassDefNV
#include <RtypesCore.h>  // for Float_t, UInt_t, Int_t

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

#include <numeric>
#include <string>  // for string


class BmnFHCalDigi : public TObject {

public:
  /**@brief Default constructor.
       **/
  BmnFHCalDigi();


  /** @brief Constructor with detailed assignment.
       **/
  BmnFHCalDigi(UInt_t address, Float_t signal, Float_t timestamp, 
                Int_t ampl, Int_t zl, Int_t integral, Int_t time_max,
                Float_t fit_ampl, Float_t fit_zl, Float_t fit_integral, Float_t fit_R2, Float_t fit_time_max,
                std::vector<float> wfm, std::vector<float> fit_wfm);


  /**  Copy constructor **/
  BmnFHCalDigi(const BmnFHCalDigi&);


  /** Move constructor  **/
  BmnFHCalDigi(BmnFHCalDigi&&);


  /** Assignment operator  **/
  BmnFHCalDigi& operator=(const BmnFHCalDigi&);


  /** Move Assignment operator  **/
  BmnFHCalDigi& operator=(BmnFHCalDigi&&);


  /** Destructor **/
  ~BmnFHCalDigi();


  /** @brief Class name (static)
       ** @return BmnFHCalDigi
       **/
  static const char* GetClassName() { return "BmnFHCalDigi"; }


  /** @brief Address
       ** @return Unique channel address (see BmnFHCalAddress)
       **/
  UInt_t GetAddress() const { return fuAddress; };


  /** @brief Module Type
       ** @return Module Type from Unique channel address (see BmnFHCalAddress)
       **/
  UInt_t GetModuleType() const { return BmnFHCalAddress::GetModuleType(fuAddress); };


  /** @brief Module Id
       ** @return Module Id from Unique channel address (see BmnFHCalAddress)
       **/
  UInt_t GetModuleId() const { return BmnFHCalAddress::GetModuleId(fuAddress); };


  /** @brief Section Id
       ** @return Section Id from Unique channel address (see BmnFHCalAddress)
       **/
  UInt_t GetSectionId() const { return BmnFHCalAddress::GetSectionId(fuAddress); };


  /** @brief calibrated Signal
       ** @return calibrated Signal
       **/
  Float_t GetSignal() const { return fSignal; };


  /** @brief Signal timestamp
       ** @return Signal timestamp
       **/
  Float_t GetTimestamp() const { return fTimestamp; };


  /** @brief X position
       ** @return X position
       **/
  Float_t GetX() const { return BmnFHCalAddress::GetXIdx(GetAddress()); }


  /** @brief Y position
       ** @return Y position
       **/
  Float_t GetY() const { return BmnFHCalAddress::GetYIdx(GetAddress()); }


  /** @brief Z position
       ** @return Z position
       **/
  Float_t GetZ() const { return BmnFHCalAddress::GetZIdx(GetAddress()); }


  /** @brief System identifier
       ** @return System ID 
       **/
  static Int_t GetSystemId() { return kFHCAL; }


  /** Modifiers **/
  void SetAddress(UInt_t address) { fuAddress = address; };

  void reset();

  const int DrawWfm();

  UInt_t fuAddress       = 0;   /// Unique channel address
  Float_t fSignal       = 0.;  /// Signal [MeV]
  Float_t fTimestamp    = -1.; /// Signal timestamp

  Int_t fAmpl         = 0;  /// Amplitude from waveform [adc counts]
  Int_t fZL           = 0;  /// ZeroLevel from waveform [adc counts]
  Int_t fIntegral     = 0;  /// Energy deposition from waveform [adc counts]
  Int_t fTimeMax      = 0;  /// Time of maximum in waveform [adc samples]

  Float_t fFitAmpl      = 0.;  /// Amplitude from fit of waveform [adc counts]
  Float_t fFitZL        = 0.;  /// ZeroLevel from fit of waveform [adc counts]
  Float_t fFitIntegral  = 0.;  /// Energy deposition from fit of waveform [adc counts]
  Float_t fFitR2        = 2.;  /// Quality of waveform fit [] -- good near 0
  Float_t fFitTimeMax   = -1.; /// Time of maximum in fit of waveform [adc samples]

  std::vector<float> fWfm;
  std::vector<float> fFitWfm;

  template<class Archive>
  void serialize(Archive& ar, const unsigned int /*version*/)
  {
    ar& fuAddress;
    ar& fSignal;
    ar& fTimestamp;

    ar& fAmpl;
    ar& fZL;
    ar& fIntegral;
    ar& fTimeMax;

    ar& fFitAmpl;
    ar& fFitZL;
    ar& fFitIntegral;
    ar& fFitR2;
    ar& fFitTimeMax;

    ar& fWfm;
    ar& fFitWfm;
  }

private:
  /// BOOST serialization interface
  friend class boost::serialization::access;


  ClassDefNV(BmnFHCalDigi, 1);
};

#endif  // BmnFHCalDigi_H
