/** @file BmnSsdDigitizeParameters.h
 ** @author Florian Uhlig <f.uhlig@gsi.de>
 ** @author D. Baranov
 ** @date 19.12.2018
 **/

#ifndef BMNSSDDIGITIZEPARAMETERS_H
#define BMNSSDDIGITIZEPARAMETERS_H 1

#include <iostream>
#include <string>
#include "FairParGenericSet.h"

class FairParamList;

/** @class BmnSsdDigitizeParameters
 ** @brief Parameters for SSD digitization
 ** @since 19.12.2018
 ** @version 1.0
 **
 ** This class collects all parameters relevant for SSD digitization
 ** such that they can be made persistent for use e.g. in reconstruction.
 ** The parameters are stored via the FairRuntimeDb
 **/
class BmnSsdDigitizeParameters : public FairParGenericSet
{

  public:


    /** Constructor **/
    BmnSsdDigitizeParameters(
                const char* name    = "BmnSsdDigitizeParameters",
                const char* title   = "Ssd digitization parameters",
                const char* context = "Default");


    /** Destructor **/
    virtual ~BmnSsdDigitizeParameters() { };

    /** Reset all parameters **/
    virtual void clear();

    void putParams(FairParamList*);
    Bool_t getParams(FairParamList*);

    /** Accessors **/
    Double_t GetCcoup() const { return fCcoup; }
    Double_t GetCis() const { return fCis; }
    Double_t GetDeadChannelFrac() const { return fDeadChannelFrac; }
    Double_t GetDeadTime() const { return fDeadTime; }
    Bool_t   GetDiscardSecondaries() const { return fDiscardSecondaries; }
    Double_t GetDynRange() const { return fDynRange; }
    Int_t    GetELossModel() const { return fELossModel; }
    Bool_t   GetGenerateNoise() const { return fGenerateNoise; }
    Int_t    GetNofAdc() const { return fNofAdc; }
    Int_t    GetNoise() const { return fNoise; }
    Double_t GetStripPitch() const { return fStripPitch; }
    Double_t GetTemperature() const { return fTemperature; }
    Double_t GetThreshold() const { return fThreshold; }
    Double_t GetTimeResolution() const { return fTimeResolution; }
    Bool_t   GetUseCrossTalk() const { return fUseCrossTalk; }
    Bool_t   GetUseDiffusion() const { return fUseDiffusion; }
    Bool_t   GetUseLorentzShift() const { return fUseLorentzShift; }
    Double_t GetVbias() const { return fVbias; }
    Double_t GetVdep() const { return fVdep; }
    Double_t GetZeroNoiseRate() const { return fZeroNoiseRate; }


    /** @brief Flag whether secondary tracks are discarded during digitisation
     ** @param if kTRUE, points from secondary tracks are not digitised.
     **/
    void SetDiscardSecondaries(Bool_t choice = kTRUE) {
      fDiscardSecondaries = choice;
      setChanged();
      setInputVersion(-2,1);
    }


    /** @brief Switch noise generation on/off (is deactivated by default).
     ** @param If kTRUE, noise will be generated in stream mode.
     **/
    void SetGenerateNoise(Bool_t choice = kTRUE) {
      fGenerateNoise = choice;
      setChanged();
      setInputVersion(-2,1);
    }


    /** @brief Set digital      response parameters
     ** @param nChannels        Number of readout channels
     ** @param dynRange         Dynamic range [e]
     ** @param threshold        Threshold [e]
     ** @param nAdc             Number of ADC channels
     ** @param timeResol        Time resolution [ns]
     ** @param deadTime         Channel dead time [ns]
     ** @param noise            Noise RMS
     ** @param deadChannelFrac  Fraction of dead channels [%]
     **/
    void SetModuleParameters(Double_t dynRange, Double_t threshold,
                             Int_t nAdc, Double_t timeResol,
                             Double_t deadTime, Double_t noise,
                             Double_t zeroNoiseRate,
                             Double_t deadChannelFrac) {
      fDynRange        = dynRange;
      fThreshold       = threshold;
      fNofAdc          = nAdc;
      fTimeResolution  = timeResol;
      fDeadTime        = deadTime;
      fNoise           = noise;
      fZeroNoiseRate   = zeroNoiseRate;
      fDeadChannelFrac = deadChannelFrac;
      setChanged();
      setInputVersion(-2,1);
    }


    /** @brief Switch analogue response processes on or off
     ** @param eLossModel Energy loss model (0=ideal, 1=uniform, 2=fluctuations)
     ** @param useLorentzShift  Lorentz shift on/off
     ** @param useDiffusion     Diffusion on/off
     ** @param useCrossTalk     Cross-talk on/off
     **/
    void SetProcesses(Int_t eLossModel, Bool_t useLorentzShift,
                      Bool_t useDiffusion, Bool_t useCrossTalk,
                      Bool_t generateNoise = kFALSE) {
      fELossModel = eLossModel;
      fUseLorentzShift = useLorentzShift;
      fUseDiffusion = useDiffusion;
      fUseCrossTalk = useCrossTalk;
      fGenerateNoise = generateNoise;
      setChanged();
      setInputVersion(-2,1);
    }


    /** brief Set sensor properties
     ** @param vDep   Depletion voltage [V]
     ** @param vBias  Bias voltage [V]
     ** @param temp   Temperature [K]
     ** @param cCoup  Coupling capacitance [pF]
     ** @param cIs    Inter-strip capacitance [pF]
     **/
    void SetSensorConditions(Double_t vDep, Double_t vBias, Double_t temp,
                             Double_t cCoup, Double_t cIs) {
      fVdep = vDep;
      fVbias = vBias;
      fTemperature = temp;
      fCcoup = cCoup;
      fCis = cIs;
      setChanged();
      setInputVersion(-2,1);
    }


    /** @brief Override the strip pitch taken from the sensor database.
     ** @value pitch  Strip pitch [cm]
     **
     ** This value will be applied for all sensors in the setup.
     **/
    void SetStripPitch(Double_t pitch) {
      fStripPitch = pitch;
      setChanged();
      setInputVersion(-2,1);
    }


    /** String output **/
    virtual std::string ToString() const;


  private:

    // --- Analogue response processes
    /** Energy loss model (0 = ideal, 1 = uniform, 2 = fluct.) **/
    Int_t  fELossModel;
    Bool_t fUseLorentzShift;    ///< Lorentz shift on/off
    Bool_t fUseDiffusion;       ///< Thermal diffusion on/off
    Bool_t fUseCrossTalk;       ///< Cross-talk on/off
    Bool_t fGenerateNoise;      ///< Noise on/off

    // --- Sensor conditions (analogue response)
    Double_t fVdep;            ///< Depletion voltage [V]
    Double_t fVbias;           ///< Bias voltage [V]
    Double_t fTemperature;     ///< Temperature [K]
    Double_t fCcoup;           ///< Coupling capacitance [pF]
    Double_t fCis;             ///< Inter-strip capacitance [pF]

    // --- Read-out ASIC properties (digital response)
    Double_t fDynRange;        ///< Dynamic range [e]
    Double_t fThreshold;       ///< Threshold [e]
    Int_t    fNofAdc;          ///< Number of ADC channels
    Double_t fTimeResolution;  ///< Time resolution [ns]
    Double_t fDeadTime;        ///< Channel dead time [ns]
    Double_t fNoise;           ///< RMS of noise [e]
    Double_t fZeroNoiseRate;   ///< Zero noise rate [1/ns]
    Double_t fDeadChannelFrac; ///< Fraction of dead channels [%]

    // --- Strip pitch. If not -1, this value overrides the strip pitch
    // --- defined in the sensor database. It will then be the same for
    // --- all sensors.
    Double_t fStripPitch;      ///< Strip pitch in sensors

    // --- If this variable is set to kTRUE, points from secondary
    // --- tracks will be discarded. For debug purposes.
    Bool_t fDiscardSecondaries; ///< If kTRUE, discard points from secondary tracks

    ClassDef(BmnSsdDigitizeParameters, 1);
};

#endif /* BMNSSDDIGITIZEPARAMETERS_H */
