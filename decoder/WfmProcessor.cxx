#include "WfmProcessor.h"

void WfmProcessor::ProcessWfm(std::vector<float> wfm, BmnDigiContainerTemplate *digi)
{
  assert(fdigiPars.gateBegin > 0 && fdigiPars.gateEnd > 0);
  if(fdigiPars.gateBegin >= wfm.size()) { 
    LOG(error) << "WfmProcessor : Filling " << digi->GetClassName() << ". waveform too short: accessing " << 
    fdigiPars.gateBegin << "/" << wfm.size() << ". Check calibration file ";
    fdigiPars.gateBegin = wfm.size()-1;
  }
  if(fdigiPars.gateEnd >= wfm.size()) { 
    LOG(error) << "WfmProcessor : Filling " << digi->GetClassName() << ". waveform too short: accessing " << 
    fdigiPars.gateEnd << "/" << wfm.size() << ". Check calibration file ";
    fdigiPars.gateEnd = wfm.size()-1;
  }

  // Invert
  if (fdigiPars.doInvert)
  {
    LOG(debug) << "WfmProcessor : Filling " << digi->GetClassName() << ". Inverting";
    float myconstant{-1.0};
    std::transform(wfm.begin(), wfm.end(), wfm.begin(),
                   std::bind1st(std::multiplies<float>(), myconstant));
  }

  //Zero level calculation
  LOG(debug) << "WfmProcessor : Filling " << digi->GetClassName() << ". ZL calc";
  const int n_gates = 3;
  int gate_npoints = (int)floor((fdigiPars.gateBegin - 2.) / n_gates);

  Float_t gates_mean[n_gates], gates_rms[n_gates];
  for (int igate = 0; igate < n_gates; igate++)
    MeanRMScalc(wfm, gates_mean + igate, gates_rms + igate, igate * gate_npoints, (igate + 1) * gate_npoints);

  int best_gate = 0;
  for (int igate = 0; igate < n_gates; igate++)
    if (gates_rms[igate] < gates_rms[best_gate])
      best_gate = igate;
  digi->fZL = (int) gates_mean[best_gate];

  //MAX and Integral calculation including borders
  LOG(debug) << "WfmProcessor : Filling " << digi->GetClassName() << ". MAX & INT search";
  digi->fIntegral = (int) std::accumulate(wfm.begin() + fdigiPars.gateBegin, wfm.begin() + fdigiPars.gateEnd + 1,
                                    -digi->fZL * (fdigiPars.gateEnd - fdigiPars.gateBegin + 1));
  auto const max_iter = std::max_element(wfm.begin() + fdigiPars.gateBegin, wfm.begin() + fdigiPars.gateEnd + 1);
  digi->fAmpl = (int) *max_iter - digi->fZL;
  digi->fTimeMax = (int) std::distance(wfm.begin(), max_iter);
  if (fdigiPars.isWriteWfm) digi->fWfm = wfm;

  //Prony fitting procedure
  PsdSignalFitting::PronyFitter Pfitter;
  if (fdigiPars.isfit)
  {
    LOG(debug) << "WfmProcessor : Filling " << digi->GetClassName() << ". Fitting";
    Pfitter.Initialize(fdigiPars.harmonics.size(), fdigiPars.harmonics.size(), fdigiPars.gateBegin, fdigiPars.gateEnd);
    Pfitter.SetDebugMode(0);
    Pfitter.SetWaveform(wfm, digi->fZL);
    //Pfitter.ResetAmplitudes();
    int SignalBeg = Pfitter.CalcSignalBeginStraight();
    if (SignalBeg < 1 || SignalBeg > wfm.size())
      return;
    if (SignalBeg + fSignalLength > wfm.size()) 
      SignalBeg = fdigiPars.gateBegin;
    Pfitter.SetExternalHarmonics(fdigiPars.harmonics);
    Pfitter.SetSignalBegin(SignalBeg);
    Pfitter.CalculateFitAmplitudesFast(fSignalLength, fAZik);

    digi->fFitIntegral = Pfitter.GetIntegral(fdigiPars.gateBegin, fdigiPars.gateEnd);
    digi->fFitAmpl = Pfitter.GetMaxAmplitude() - Pfitter.GetZeroLevel();
    float fit_R2 = Pfitter.GetRSquare(fdigiPars.gateBegin, fdigiPars.gateEnd);
    digi->fFitR2 = (fit_R2 > 2.0) ? 2.0 : fit_R2;
    digi->fFitZL = Pfitter.GetZeroLevel();
    digi->fFitTimeMax = Pfitter.GetSignalMaxTime(); 
  }

  if (fdigiPars.isWriteWfm && fdigiPars.isfit)
    digi->fFitWfm = Pfitter.GetFitWfm();
    
}

void WfmProcessor::MeanRMScalc(std::vector<float> wfm, float *Mean, float *RMS, int begin, int end, int step)
{
  begin = (begin < 0) ? 0 : begin;
  if (begin > end)
  {
    float swap = end;
    end = begin;
    begin = swap;
  };
  step = TMath::Abs(step);
  *Mean = *RMS = 0.;
  int Delta = 0;
  for (int n = begin; n <= end; n += step)
  {
    *Mean += wfm[n];
    Delta++;
  }
  *Mean /= (float)Delta;
  for (int n = begin; n <= end; n += step)
    *RMS += (wfm[n] - *Mean) * (wfm[n] - *Mean);
  *RMS = sqrt(*RMS / ((float)Delta));
  //printf("AMPL %.2f, RMS %.2f\n",*Mean,*RMS);
}

WfmProcessor::~WfmProcessor()
{
  if(fdigiPars.isfit && fAZik) {
      int model_order = fdigiPars.harmonics.size() + 1;
      for(int i = 0; i < model_order; i++)
          delete [] fAZik[i];
      delete [] fAZik;
  }
}

ClassImp(WfmProcessor)
