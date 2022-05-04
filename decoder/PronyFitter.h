/* Copyright (C) 2020-2021 Institute for Nuclear Research, Moscow
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Nikolay Karpushkin [committer] */

/** @file   PronyFitter.h
    @class  PronyFitter
    @author Nikolay Karpushkin (nkarpushkin@mail.ru)
    @brief  Class to fit waveform using Prony least squares method
*/

#ifndef PronyFitter_H
#define PronyFitter_H

#include <algorithm> // for reverse
#include <complex>   // for complex numbers
#include <cstring>   // for memcpy
#include <iostream>
#include <vector> // for std::vector
#include <numeric>

#include <stdint.h> // for uint16_t
#include <stdio.h>  // for printf

namespace PsdSignalFitting {
class PronyFitter {

public:
  /**         Default constructor         **/
  PronyFitter(){};
  PronyFitter(int model_order, int exponent_number, int gate_beg, int gate_end);

  void Initialize(int model_order, int exponent_number, int gate_beg, int gate_end);

  /**         Default destructor         **/
  ~PronyFitter() { Clear(); };

  float GoToLevel(float Level, int *point, int iterator, int iLastPoint);
  int CalcSignalBeginStraight();
  int CalcSignalBegin(float front_time_beg_03, float front_time_end);
  int ChooseBestSignalBeginHarmonics(int first_sample, int last_sample);
  int ChooseBestSignalBegin(int first_sample, int last_sample);
  void MakePileUpRejection(int time_max);
  void CalculateFitHarmonics();
  void CalculateFitAmplitudes();
  void MakeInvHarmoMatrix(int signal_length, std::complex<float> **output);
  void MakeZpowerMatrix(int signal_length, std::complex<float> **output);
  void ResetAmplitudes();
  void CalculateFitAmplitudesFast(int signal_length, std::complex<float> **InvHarmoMatrix);
  int ChooseBestSignalBeginFast(int first_sample, int last_sample, int signal_length, std::complex<float> **InvHarmoMatrix);
  int SearchSignalBeginByHarmo(int first_sample, int last_sample, int signal_length, std::complex<float> **Zpower);
  void SolveSLEGauss(float *x, float **r, float *b, int n);
  void SolveSLEGauss(std::complex<float> *x, std::complex<float> **r,
                     std::complex<float> *b, int n);
  void SolveSLECholesky(float *x, float **a, float *b, int n);
  void CovarianceQRmod(float &rho_f, std::vector<float> &a_f, float &rho_b,
                       std::vector<float> &a_b);
  void CovarianceDirect(float &rho_f, std::vector<float> &a_f, float &rho_b,
                        std::vector<float> &a_b);
  float LevelBy2Points(float X1, float Y1, float X2, float Y2, float Y0);
  //
  //                           Setters
  //
  void SetDebugMode(bool IsDebug) { fIsDebug = IsDebug; };
  void SetWaveform(std::vector<float> &Wfm, float ZeroLevel);
  void SetSignalBegin(int SignalBeg);
  void SetHarmonics(std::complex<float> *z);
  void SetExternalHarmonics(std::vector<std::complex<float>> harmonics);
  //
  //                           Getters
  //
  std::complex<float> *GetHarmonics();
  std::complex<float> *GetAmplitudes();
  float GetIntegral(int gate_beg, int gate_end);
  float GetFitValue(int sample_number);
  float GetFitValue(float x);
  float GetZeroLevel();
  float GetX(float level, int first_sample, int last_sample);
  float GetX(float level, int first_sample, int last_sample, float step);
  float GetRSquare(int gate_beg, int gate_end);
  float GetRSquareSignal();
  float GetChiSquare(int gate_beg, int gate_end, int time_max);
  float GetDeltaInSample(int sample);
  float GetSignalBeginFromPhase();
  float GetSignalMaxTime();
  float GetMaxAmplitude();
  int GetNumberPolRoots();
  std::vector<float> GetFitWfm() { return fFitWfm; }

private:

  void AllocData();
  void DeleteData();
  void Clear();

  bool fIsDebug = false;
  int fModelOrder;
  int fExpNumber;
  int fGateBeg;
  int fGateEnd;
  int fSampleTotal;

  int fSignalBegin;
  int fTotalPolRoots;

  std::vector<float> fWfm;
  float fZeroLevel;
  std::complex<float> *fz = nullptr; //!
  std::complex<float> *fh = nullptr; //!
  std::vector<float> fFitWfm;
  float fFitZeroLevel;
};
} // namespace PsdSignalFitting

#endif
