/* Copyright (C) 2020 Facility for Antiproton and Ion Research in Europe,
   Darmstadt SPDX-License-Identifier: GPL-3.0-only
   Authors: Pierre-Alain Loizeau [committer] */

#include <cmath>

//*************************************************************************
float polinom(int n, float x, float *k) {
  float s = 1;
  for (int i = n - 1; i >= 0; i--)
    s = s * x + k[i];
  return s;
} // polinom

float dihot(int degree, float edgeNegativ, float edgePositiv, float *kf) {
  for (;;) {
    float x = 0.5 * (edgeNegativ + edgePositiv);
    if (std::abs(x - edgeNegativ) < 1e-3 || std::abs(x - edgePositiv) < 1e-3)
      return x;
    if (polinom(degree, x, kf) < 0)
      edgeNegativ = x;
    else
      edgePositiv = x;
  }
} // dihot

void stepUp(int level, float **A, float **B, int *currentRootsCount) {

  float major = 0;
  for (int i = 0; i < level; i++) {
    float s = fabs(A[level][i]);
    if (s > major)
      major = s;
  }
  major += 1.0;

  currentRootsCount[level] = 0;

  for (int i = 0; i <= currentRootsCount[level - 1]; i++) {
    int signLeft, signRight;
    float edgeLeft, edgeRight;
    float edgeNegativ, edgePositiv;

    if (i == 0)
      edgeLeft = -major;
    else
      edgeLeft = B[level - 1][i - 1];

    float rb = polinom(level, edgeLeft, A[level]);

    if (rb == 0) {
      B[level][currentRootsCount[level]] = edgeLeft;
      currentRootsCount[level]++;
      continue;
    }

    if (rb > 0)
      signLeft = 1;
    else
      signLeft = -1;

    if (i == currentRootsCount[level - 1])
      edgeRight = major;
    else
      edgeRight = B[level - 1][i];

    rb = polinom(level, edgeRight, A[level]);

    if (rb == 0) {
      B[level][currentRootsCount[level]] = edgeRight;
      currentRootsCount[level]++;
      continue;
    }

    if (rb > 0)
      signRight = 1;
    else
      signRight = -1;
    if (signLeft == signRight)
      continue;

    if (signLeft < 0) {
      edgeNegativ = edgeLeft;
      edgePositiv = edgeRight;
    } else {
      edgeNegativ = edgeRight;
      edgePositiv = edgeLeft;
    }

    B[level][currentRootsCount[level]] =
        dihot(level, edgeNegativ, edgePositiv, A[level]);
    currentRootsCount[level]++;
  }
  return;
} // stepUp

void polynomRealRoots(float *rootsArray, int n, float *kf_, int &rootsCount) {

  float *kf = new float[n + 1];

  float **A = new float *[n + 1];
  float **B = new float *[n + 1];

  int *currentRootsCount = new int[n + 1];

  for (int i = 1; i <= n; i++) {
    A[i] = new float[i];
    B[i] = new float[i];
  }

  for (int i = 0; i <= n; i++)
    kf[i] = kf_[n - i];

  for (int i = 0; i < n; i++)
    A[n][i] = kf[i] / kf[n];

  for (int i1 = n, i = n - 1; i > 0; i1 = i, i--) {
    for (int j1 = i, j = i - 1; j >= 0; j1 = j, j--) {
      A[i][j] = A[i1][j1] * j1 / i1;
    }
  }

  currentRootsCount[1] = 1;
  B[1][0] = -A[1][0];

  for (int i = 2; i <= n; i++)
    stepUp(i, A, B, currentRootsCount);

  rootsCount = currentRootsCount[n];
  for (int i = 0; i < rootsCount; i++)
    rootsArray[i] = B[n][i];

  for (int i = 1; i <= n; i++) {
    delete[] A[i];
    delete[] B[i];
  }
  delete[] A;
  delete[] B;
  delete[] currentRootsCount;
  delete[] kf;
} // polynomRealRoots

//*************************************************************************
