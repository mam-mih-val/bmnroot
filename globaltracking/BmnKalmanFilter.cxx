/** BmnKalmanFilter.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2007-2014
 **/

#include "BmnKalmanFilter.h"

#include "CbmStripHit.h"
#include "CbmTofHit.h"
#include "CbmPixelHit.h"
#include "FairTrackParam.h"
#include "BmnMatrixMath.h"

#include <iostream>
#include <cmath>

BmnKalmanFilter::BmnKalmanFilter() {
}

BmnKalmanFilter::~BmnKalmanFilter() {
}

BmnStatus BmnKalmanFilter::Update(FairTrackParam* par, const CbmStsHit* hit, Float_t& chiSq) { //FIXME!!! Pixel or Strip hit? What's difference?  
    
    //FIXME! CHECK!
    Float_t xIn[5] = {par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp()};
    Double_t cIn[15];
    par->CovMatrix(cIn);

    Float_t u = hit->GetY();
    Float_t duu = hit->GetDy() * hit->GetDy();
    Float_t phiCos = 0; //hit->GetCosPhi();                                       //FIXME
    Float_t phiSin = 1; //hit->GetSinPhi();                                       //FIXME
    Float_t phiCosSq = phiCos * phiCos;
    Float_t phiSinSq = phiSin * phiSin;
    Float_t phi2SinCos = 2 * phiCos * phiSin;

    // Inverted covariance matrix of predicted residual
    Float_t R = 1. / (duu + cIn[0] * phiCosSq + phi2SinCos * cIn[1] + cIn[5] * phiSinSq);

    // Calculate Kalman gain matrix
    Float_t K0 = cIn[0] * phiCos + cIn[1] * phiSin;
    Float_t K1 = cIn[1] * phiCos + cIn[5] * phiSin;
    Float_t K2 = cIn[2] * phiCos + cIn[6] * phiSin;
    Float_t K3 = cIn[3] * phiCos + cIn[7] * phiSin;
    Float_t K4 = cIn[4] * phiCos + cIn[8] * phiSin;

    Float_t KR0 = K0 * R;
    Float_t KR1 = K1 * R;
    Float_t KR2 = K2 * R;
    Float_t KR3 = K3 * R;
    Float_t KR4 = K4 * R;

    // Residual of predictions
    Float_t r = u - xIn[0] * phiCos - xIn[1] * phiSin;

    // Calculate filtered state vector
    std::vector<Float_t> xOut(5);
    xOut[0] = xIn[0] + KR0 * r;
    xOut[1] = xIn[1] + KR1 * r;
    xOut[2] = xIn[2] + KR2 * r;
    xOut[3] = xIn[3] + KR3 * r;
    xOut[4] = xIn[4] + KR4 * r;

    // Calculate filtered covariance matrix
    Double_t cOut[15];
    cOut[0] = cIn[0] - KR0 * K0;
    cOut[1] = cIn[1] - KR0 * K1;
    cOut[2] = cIn[2] - KR0 * K2;
    cOut[3] = cIn[3] - KR0 * K3;
    cOut[4] = cIn[4] - KR0 * K4;

    cOut[5] = cIn[5] - KR1 * K1;
    cOut[6] = cIn[6] - KR1 * K2;
    cOut[7] = cIn[7] - KR1 * K3;
    cOut[8] = cIn[8] - KR1 * K4;

    cOut[9] = cIn[9] - KR2 * K2;
    cOut[10] = cIn[10] - KR2 * K3;
    cOut[11] = cIn[11] - KR2 * K4;

    cOut[12] = cIn[12] - KR3 * K3;
    cOut[13] = cIn[13] - KR3 * K4;

    cOut[14] = cIn[14] - KR4 * K4;

    // Copy filtered state to output
    par->SetX(xOut[0]);
    par->SetY(xOut[1]);
    par->SetTx(xOut[2]);
    par->SetTy(xOut[3]);
    par->SetQp(xOut[4]);
    par->SetCovMatrix(cOut);
    
//    cout << par->GetX() << " " << par->GetY() << " " << par->GetTx() << " " << par->GetTy() << " " << par->GetQp() << endl;

    // Filtered resuduals
    Float_t ru = u - xOut[0] * phiCos - xOut[1] * phiSin;

    // Calculate chi-square
    chiSq = (ru * ru) / (duu - phiCosSq * cOut[0] - phi2SinCos * cOut[1] - phiSinSq * cOut[5]);

    return kBMNSUCCESS;
}

BmnStatus BmnKalmanFilter::Update(FairTrackParam* par, const BmnGemHit* hit, Float_t& chiSq) { //FIXME!!! Pixel or Strip hit? What's difference?  
    
      //   vector<Float_t> cIn = par->GetCovMatrix();
    Double_t cIn[15];
    par->CovMatrix(cIn);

    static const Float_t ONE = 1., TWO = 2.;

    Float_t dxx = hit->GetDx() * hit->GetDx();
    Float_t dxy = 0.0;//hit->GetDx() * hit->GetDy(); //FIXME!!!
    Float_t dyy = hit->GetDy() * hit->GetDy();

    // calculate residuals
    Float_t dx = hit->GetX() - par->GetX();
    Float_t dy = hit->GetY() - par->GetY();
    
//    cout << "dX = " << dx << "\t dY = " << dy << endl;

    // Calculate and inverse residual covariance matrix
    Float_t t = ONE / (dxx * dyy + dxx * cIn[5] + dyy * cIn[0] + cIn[0] * cIn[5] -
            dxy * dxy - TWO * dxy * cIn[1] - cIn[1] * cIn[1]);
    Float_t R00 = (dyy + cIn[5]) * t;
    Float_t R01 = -(dxy + cIn[1]) * t;
    Float_t R11 = (dxx + cIn[0]) * t;

    // Calculate Kalman gain matrix
    Float_t K00 = cIn[0] * R00 + cIn[1] * R01;
    Float_t K01 = cIn[0] * R01 + cIn[1] * R11;
    Float_t K10 = cIn[1] * R00 + cIn[5] * R01;
    Float_t K11 = cIn[1] * R01 + cIn[5] * R11;
    Float_t K20 = cIn[2] * R00 + cIn[6] * R01;
    Float_t K21 = cIn[2] * R01 + cIn[6] * R11;
    Float_t K30 = cIn[3] * R00 + cIn[7] * R01;
    Float_t K31 = cIn[3] * R01 + cIn[7] * R11;
    Float_t K40 = cIn[4] * R00 + cIn[8] * R01;
    Float_t K41 = cIn[4] * R01 + cIn[8] * R11;

    // Calculate filtered state vector
    Float_t xOut[5] = {par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp()};
    xOut[0] += K00 * dx + K01 * dy;
    xOut[1] += K10 * dx + K11 * dy;
    xOut[2] += K20 * dx + K21 * dy;
    xOut[3] += K30 * dx + K31 * dy;
    xOut[4] += K40 * dx + K41 * dy;

    // Calculate filtered covariance matrix

    Double_t cOut[15];
    for (Int_t i = 0; i < 15; ++i) { cOut[i] = cIn[i]; }


    cOut[0] += -K00 * cIn[0] - K01 * cIn[1];
    cOut[1] += -K00 * cIn[1] - K01 * cIn[5];
    cOut[2] += -K00 * cIn[2] - K01 * cIn[6];
    cOut[3] += -K00 * cIn[3] - K01 * cIn[7];
    cOut[4] += -K00 * cIn[4] - K01 * cIn[8];

    cOut[5] += -K11 * cIn[5] - K10 * cIn[1];
    cOut[6] += -K11 * cIn[6] - K10 * cIn[2];
    cOut[7] += -K11 * cIn[7] - K10 * cIn[3];
    cOut[8] += -K11 * cIn[8] - K10 * cIn[4];

    cOut[9] += -K20 * cIn[2] - K21 * cIn[6];
    cOut[10] += -K20 * cIn[3] - K21 * cIn[7];
    cOut[11] += -K20 * cIn[4] - K21 * cIn[8];

    cOut[12] += -K30 * cIn[3] - K31 * cIn[7];
    cOut[13] += -K30 * cIn[4] - K31 * cIn[8];

    cOut[14] += -K40 * cIn[4] - K41 * cIn[8];

    // Copy filtered state to output
    par->SetX(xOut[0]);
    par->SetY(xOut[1]);
    par->SetTx(xOut[2]);
    par->SetTy(xOut[3]);
    par->SetQp(xOut[4]);
    par->SetCovMatrix(cOut);
    
//    cout << par->GetX() << " " << par->GetY() << " " << par->GetTx() << " " << par->GetTy() << " " << par->GetQp() << endl;

    // Calculate chi-square
    Float_t xmx = hit->GetX() - par->GetX();
    Float_t ymy = hit->GetY() - par->GetY();
//    cout << "dX = " << xmx << "\t dY = " << ymy << endl;
    Float_t C0 = cOut[0];
    Float_t C1 = cOut[1];
    Float_t C5 = cOut[5];

    Float_t norm = dxx * dyy - dxx * C5 - dyy * C0 + C0 * C5 - dxy * dxy + 2 * dxy * C1 - C1 * C1;

    chiSq = ((xmx * (dyy - C5) - ymy * (dxy - C1)) * xmx + (-xmx * (dxy - C1) + ymy * (dxx - C0)) * ymy) / norm;
    
    //if (chiSq < 0.0) {
//        cout << endl << "Chi2 = " << chiSq << endl;
//        cout << "chisl = " << ((xmx * (dyy - C5) - ymy * (dxy - C1)) * xmx + (-xmx * (dxy - C1) + ymy * (dxx - C0)) * ymy) << " znam = " << norm << endl;
//        cout << "1 = " << (xmx * (dyy - C5) - ymy * (dxy - C1)) * xmx << endl;
//        cout << "2 = " << (-xmx * (dxy - C1) + ymy * (dxx - C0)) * ymy << endl;
//        cout << "C0 = " << C0 << " C5 = " << C5 << " C1 = " << C1 << endl;
//        cout << "-K00 * cIn[1] - K01 * cIn[5] = " << -K00 * cIn[1] - K01 * cIn[5] << endl;
//        cout << "Dxx = " << dxx << " Dyy = " << dyy << " Dxy = " << dxy << endl;
//        cout << "dx = " << dx << " dy = " << dy << endl;
//        cout << "xMx = " << xmx << " yMy = " << ymy << endl;
   // }
//    par->Print();

    return kBMNSUCCESS;
}

BmnStatus BmnKalmanFilter::Update(FairTrackParam* par, const BmnHit* hit, Float_t& chiSq) { //FIXME!!! Pixel or Strip hit? What's difference?  
    
      //   vector<Float_t> cIn = par->GetCovMatrix();
    Double_t cIn[15];
    par->CovMatrix(cIn);

    static const Float_t ONE = 1., TWO = 2.;

    Float_t dxx = hit->GetDx() * hit->GetDx();
    Float_t dxy = 0.0;//hit->GetDx() * hit->GetDy(); //FIXME!!!
    Float_t dyy = hit->GetDy() * hit->GetDy();

    // calculate residuals
    Float_t dx = hit->GetX() - par->GetX();
    Float_t dy = hit->GetY() - par->GetY();
    
//    cout << "dX = " << dx << "\t dY = " << dy << endl;

    // Calculate and inverse residual covariance matrix
    Float_t t = ONE / (dxx * dyy + dxx * cIn[5] + dyy * cIn[0] + cIn[0] * cIn[5] -
            dxy * dxy - TWO * dxy * cIn[1] - cIn[1] * cIn[1]);
    Float_t R00 = (dyy + cIn[5]) * t;
    Float_t R01 = -(dxy + cIn[1]) * t;
    Float_t R11 = (dxx + cIn[0]) * t;

    // Calculate Kalman gain matrix
    Float_t K00 = cIn[0] * R00 + cIn[1] * R01;
    Float_t K01 = cIn[0] * R01 + cIn[1] * R11;
    Float_t K10 = cIn[1] * R00 + cIn[5] * R01;
    Float_t K11 = cIn[1] * R01 + cIn[5] * R11;
    Float_t K20 = cIn[2] * R00 + cIn[6] * R01;
    Float_t K21 = cIn[2] * R01 + cIn[6] * R11;
    Float_t K30 = cIn[3] * R00 + cIn[7] * R01;
    Float_t K31 = cIn[3] * R01 + cIn[7] * R11;
    Float_t K40 = cIn[4] * R00 + cIn[8] * R01;
    Float_t K41 = cIn[4] * R01 + cIn[8] * R11;

    // Calculate filtered state vector
    Float_t xOut[5] = {par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp()};
    xOut[0] += K00 * dx + K01 * dy;
    xOut[1] += K10 * dx + K11 * dy;
    xOut[2] += K20 * dx + K21 * dy;
    xOut[3] += K30 * dx + K31 * dy;
    xOut[4] += K40 * dx + K41 * dy;

    // Calculate filtered covariance matrix

    Double_t cOut[15];
    for (Int_t i = 0; i < 15; ++i) { cOut[i] = cIn[i]; }


    cOut[0] += -K00 * cIn[0] - K01 * cIn[1];
    cOut[1] += -K00 * cIn[1] - K01 * cIn[5];
    cOut[2] += -K00 * cIn[2] - K01 * cIn[6];
    cOut[3] += -K00 * cIn[3] - K01 * cIn[7];
    cOut[4] += -K00 * cIn[4] - K01 * cIn[8];

    cOut[5] += -K11 * cIn[5] - K10 * cIn[1];
    cOut[6] += -K11 * cIn[6] - K10 * cIn[2];
    cOut[7] += -K11 * cIn[7] - K10 * cIn[3];
    cOut[8] += -K11 * cIn[8] - K10 * cIn[4];

    cOut[9] += -K20 * cIn[2] - K21 * cIn[6];
    cOut[10] += -K20 * cIn[3] - K21 * cIn[7];
    cOut[11] += -K20 * cIn[4] - K21 * cIn[8];

    cOut[12] += -K30 * cIn[3] - K31 * cIn[7];
    cOut[13] += -K30 * cIn[4] - K31 * cIn[8];

    cOut[14] += -K40 * cIn[4] - K41 * cIn[8];

    // Copy filtered state to output
    par->SetX(xOut[0]);
    par->SetY(xOut[1]);
    par->SetTx(xOut[2]);
    par->SetTy(xOut[3]);
    par->SetQp(xOut[4]);
    par->SetCovMatrix(cOut);
    
//    cout << par->GetX() << " " << par->GetY() << " " << par->GetTx() << " " << par->GetTy() << " " << par->GetQp() << endl;

    // Calculate chi-square
    Float_t xmx = hit->GetX() - par->GetX();
    Float_t ymy = hit->GetY() - par->GetY();
//    cout << "dX = " << xmx << "\t dY = " << ymy << endl;
    Float_t C0 = cOut[0];
    Float_t C1 = cOut[1];
    Float_t C5 = cOut[5];

    Float_t norm = dxx * dyy - dxx * C5 - dyy * C0 + C0 * C5 - dxy * dxy + 2 * dxy * C1 - C1 * C1;

    chiSq = ((xmx * (dyy - C5) - ymy * (dxy - C1)) * xmx + (-xmx * (dxy - C1) + ymy * (dxx - C0)) * ymy) / norm;
    
    //if (chiSq < 0.0) {
//        cout << endl << "Chi2 = " << chiSq << endl;
//        cout << "chisl = " << ((xmx * (dyy - C5) - ymy * (dxy - C1)) * xmx + (-xmx * (dxy - C1) + ymy * (dxx - C0)) * ymy) << " znam = " << norm << endl;
//        cout << "1 = " << (xmx * (dyy - C5) - ymy * (dxy - C1)) * xmx << endl;
//        cout << "2 = " << (-xmx * (dxy - C1) + ymy * (dxx - C0)) * ymy << endl;
//        cout << "C0 = " << C0 << " C5 = " << C5 << " C1 = " << C1 << endl;
//        cout << "-K00 * cIn[1] - K01 * cIn[5] = " << -K00 * cIn[1] - K01 * cIn[5] << endl;
//        cout << "Dxx = " << dxx << " Dyy = " << dyy << " Dxy = " << dxy << endl;
//        cout << "dx = " << dx << " dy = " << dy << endl;
//        cout << "xMx = " << xmx << " yMy = " << ymy << endl;
   // }
//    par->Print();

    return kBMNSUCCESS;
}

BmnStatus BmnKalmanFilter::Update(FairTrackParam* par, const CbmTofHit* hit, Float_t& chiSq) {
    return Update(par, (CbmPixelHit*) (hit), chiSq);
}

BmnStatus BmnKalmanFilter::Update(FairTrackParam* par, const CbmPixelHit* hit, Float_t& chiSq) {
    //   vector<Float_t> cIn = par->GetCovMatrix();
    Double_t cIn[15];
    par->CovMatrix(cIn);
//    cout << "In pixel updater\n";

    static const Float_t ONE = 1., TWO = 2.;

    Float_t dxx = hit->GetDx() * hit->GetDx();
    Float_t dxy = hit->GetDxy();
    Float_t dyy = hit->GetDy() * hit->GetDy();

    // calculate residuals
    Float_t dx = hit->GetX() - par->GetX();
    Float_t dy = hit->GetY() - par->GetY();

    // Calculate and inverse residual covariance matrix
    Float_t t = ONE / (dxx * dyy + dxx * cIn[5] + dyy * cIn[0] + cIn[0] * cIn[5] -
            dxy * dxy - TWO * dxy * cIn[1] - cIn[1] * cIn[1]);
    Float_t R00 = (dyy + cIn[5]) * t;
    Float_t R01 = -(dxy + cIn[1]) * t;
    Float_t R11 = (dxx + cIn[0]) * t;

    // Calculate Kalman gain matrix
    Float_t K00 = cIn[0] * R00 + cIn[1] * R01;
    Float_t K01 = cIn[0] * R01 + cIn[1] * R11;
    Float_t K10 = cIn[1] * R00 + cIn[5] * R01;
    Float_t K11 = cIn[1] * R01 + cIn[5] * R11;
    Float_t K20 = cIn[2] * R00 + cIn[6] * R01;
    Float_t K21 = cIn[2] * R01 + cIn[6] * R11;
    Float_t K30 = cIn[3] * R00 + cIn[7] * R01;
    Float_t K31 = cIn[3] * R01 + cIn[7] * R11;
    Float_t K40 = cIn[4] * R00 + cIn[8] * R01;
    Float_t K41 = cIn[4] * R01 + cIn[8] * R11;

    // Calculate filtered state vector
    Float_t xOut[5] = {par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp()};
    xOut[0] += K00 * dx + K01 * dy;
    xOut[1] += K10 * dx + K11 * dy;
    xOut[2] += K20 * dx + K21 * dy;
    xOut[3] += K30 * dx + K31 * dy;
    xOut[4] += K40 * dx + K41 * dy;

    // Calculate filtered covariance matrix

    Double_t cOut[15];
    for (Int_t i = 0; i < 15; ++i) { cOut[i] = cIn[i]; }


    cOut[0] += -K00 * cIn[0] - K01 * cIn[1];
    cOut[1] += -K00 * cIn[1] - K01 * cIn[5];
    cOut[2] += -K00 * cIn[2] - K01 * cIn[6];
    cOut[3] += -K00 * cIn[3] - K01 * cIn[7];
    cOut[4] += -K00 * cIn[4] - K01 * cIn[8];

    cOut[5] += -K11 * cIn[5] - K10 * cIn[1];
    cOut[6] += -K11 * cIn[6] - K10 * cIn[2];
    cOut[7] += -K11 * cIn[7] - K10 * cIn[3];
    cOut[8] += -K11 * cIn[8] - K10 * cIn[4];

    cOut[9] += -K20 * cIn[2] - K21 * cIn[6];
    cOut[10] += -K20 * cIn[3] - K21 * cIn[7];
    cOut[11] += -K20 * cIn[4] - K21 * cIn[8];

    cOut[12] += -K30 * cIn[3] - K31 * cIn[7];
    cOut[13] += -K30 * cIn[4] - K31 * cIn[8];

    cOut[14] += -K40 * cIn[4] - K41 * cIn[8];

    // Copy filtered state to output
    par->SetX(xOut[0]);
    par->SetY(xOut[1]);
    par->SetTx(xOut[2]);
    par->SetTy(xOut[3]);
    par->SetQp(xOut[4]);
    par->SetCovMatrix(cOut);
    
//    cout << par->GetX() << " " << par->GetY() << " " << par->GetTx() << " " << par->GetTy() << " " << par->GetQp() << endl;

    // Calculate chi-square
    Float_t xmx = hit->GetX() - par->GetX();
    Float_t ymy = hit->GetY() - par->GetY();
    Float_t C0 = cOut[0];
    Float_t C1 = cOut[1];
    Float_t C5 = cOut[5];

    Float_t norm = dxx * dyy - dxx * C5 - dyy * C0 + C0 * C5 - dxy * dxy + 2 * dxy * C1 - C1 * C1;

    chiSq = ((xmx * (dyy - C5) - ymy * (dxy - C1)) * xmx + (-xmx * (dxy - C1) + ymy * (dxx - C0)) * ymy) / norm;
    
//    if (chiSq < 0.0) {
//        cout << "Chi2 = " << chiSq << " norm = " << norm << endl;
//        cout << "C0 = " << C0 << " C5 = " << C5 << " C1 = " << C1 << endl;
//        cout << "Dxx = " << dxx << " Dyy = " << dyy << " Dxy = " << dxy << endl;
//    }
//    par->Print();

    return kBMNSUCCESS;
}

BmnStatus BmnKalmanFilter::Update(FairTrackParam* par, const BmnDchHit* hit, Float_t& chiSq) {
    //   vector<Float_t> cIn = par->GetCovMatrix();
    Double_t cIn[15];
    par->CovMatrix(cIn);
//    cout << "In pixel updater\n";

    static const Float_t ONE = 1., TWO = 2.;

    Float_t dxx = hit->GetDx() * hit->GetDx();
    Float_t dxy = 0.0;
    Float_t dyy = hit->GetDy() * hit->GetDy();

    // calculate residuals
    Float_t dx = hit->GetX() - par->GetX();
    Float_t dy = hit->GetY() - par->GetY();

    // Calculate and inverse residual covariance matrix
    Float_t t = ONE / (dxx * dyy + dxx * cIn[5] + dyy * cIn[0] + cIn[0] * cIn[5] -
            dxy * dxy - TWO * dxy * cIn[1] - cIn[1] * cIn[1]);
    Float_t R00 = (dyy + cIn[5]) * t;
    Float_t R01 = -(dxy + cIn[1]) * t;
    Float_t R11 = (dxx + cIn[0]) * t;

    // Calculate Kalman gain matrix
    Float_t K00 = cIn[0] * R00 + cIn[1] * R01;
    Float_t K01 = cIn[0] * R01 + cIn[1] * R11;
    Float_t K10 = cIn[1] * R00 + cIn[5] * R01;
    Float_t K11 = cIn[1] * R01 + cIn[5] * R11;
    Float_t K20 = cIn[2] * R00 + cIn[6] * R01;
    Float_t K21 = cIn[2] * R01 + cIn[6] * R11;
    Float_t K30 = cIn[3] * R00 + cIn[7] * R01;
    Float_t K31 = cIn[3] * R01 + cIn[7] * R11;
    Float_t K40 = cIn[4] * R00 + cIn[8] * R01;
    Float_t K41 = cIn[4] * R01 + cIn[8] * R11;

    // Calculate filtered state vector
    Float_t xOut[5] = {par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp()};
    xOut[0] += K00 * dx + K01 * dy;
    xOut[1] += K10 * dx + K11 * dy;
    xOut[2] += K20 * dx + K21 * dy;
    xOut[3] += K30 * dx + K31 * dy;
    xOut[4] += K40 * dx + K41 * dy;

    // Calculate filtered covariance matrix

    Double_t cOut[15];
    for (Int_t i = 0; i < 15; ++i) { cOut[i] = cIn[i]; }


    cOut[0] += -K00 * cIn[0] - K01 * cIn[1];
    cOut[1] += -K00 * cIn[1] - K01 * cIn[5];
    cOut[2] += -K00 * cIn[2] - K01 * cIn[6];
    cOut[3] += -K00 * cIn[3] - K01 * cIn[7];
    cOut[4] += -K00 * cIn[4] - K01 * cIn[8];

    cOut[5] += -K11 * cIn[5] - K10 * cIn[1];
    cOut[6] += -K11 * cIn[6] - K10 * cIn[2];
    cOut[7] += -K11 * cIn[7] - K10 * cIn[3];
    cOut[8] += -K11 * cIn[8] - K10 * cIn[4];

    cOut[9] += -K20 * cIn[2] - K21 * cIn[6];
    cOut[10] += -K20 * cIn[3] - K21 * cIn[7];
    cOut[11] += -K20 * cIn[4] - K21 * cIn[8];

    cOut[12] += -K30 * cIn[3] - K31 * cIn[7];
    cOut[13] += -K30 * cIn[4] - K31 * cIn[8];

    cOut[14] += -K40 * cIn[4] - K41 * cIn[8];

    // Copy filtered state to output
    par->SetX(xOut[0]);
    par->SetY(xOut[1]);
    par->SetTx(xOut[2]);
    par->SetTy(xOut[3]);
    par->SetQp(xOut[4]);
    par->SetCovMatrix(cOut);
    
//    cout << par->GetX() << " " << par->GetY() << " " << par->GetTx() << " " << par->GetTy() << " " << par->GetQp() << endl;

    // Calculate chi-square
    Float_t xmx = hit->GetX() - par->GetX();
    Float_t ymy = hit->GetY() - par->GetY();
    Float_t C0 = cOut[0];
    Float_t C1 = cOut[1];
    Float_t C5 = cOut[5];

    Float_t norm = dxx * dyy - dxx * C5 - dyy * C0 + C0 * C5 - dxy * dxy + 2 * dxy * C1 - C1 * C1;

    chiSq = ((xmx * (dyy - C5) - ymy * (dxy - C1)) * xmx + (-xmx * (dxy - C1) + ymy * (dxx - C0)) * ymy) / norm;
    
//    if (chiSq < 0.0) {
//        cout << "Chi2 = " << chiSq << " norm = " << norm << endl;
//        cout << "C0 = " << C0 << " C5 = " << C5 << " C1 = " << C1 << endl;
//        cout << "Dxx = " << dxx << " Dyy = " << dyy << " Dxy = " << dxy << endl;
//    }
//    par->Print();

    return kBMNSUCCESS;
}

BmnStatus BmnKalmanFilter::UpdateWMF(
        FairTrackParam* par,
        const CbmPixelHit* hit,
        Float_t& chiSq) {
    //   Float_t xIn[5] = { par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp() };
    //
    //   std::vector<Float_t> cIn = par->GetCovMatrix();
    //   std::vector<Float_t> cInInv = par->GetCovMatrix();
    //
    //   Float_t dxx = hit->GetDx() * hit->GetDx();
    //   Float_t dxy = hit->GetDxy();
    //   Float_t dyy = hit->GetDy() * hit->GetDy();
    //
    //   // Inverse predicted cov matrix
    //   InvSym15(cInInv);
    //   // Calculate C1
    //   std::vector<Float_t> C1 = cInInv;
    //   Float_t det = dxx * dyy - dxy * dxy;
    //   C1[0] += dyy / det;
    //   C1[1] += -dxy / det;
    //   C1[5] += dxx / det;
    //   // Inverse C1 -> output updated covariance matrix
    //   InvSym15(C1);
    //
    //   std::vector<Float_t> t(5);
    //   t[0] = cInInv[0]*par->GetX() + cInInv[1]*par->GetY() + cInInv[2]*par->GetTx()
    //          + cInInv[3]*par->GetTy() + cInInv[4]*par->GetQp()
    //          + (dyy * hit->GetX() - dxy * hit->GetY()) / det;
    //   t[1] = cInInv[1]*par->GetX() + cInInv[5]*par->GetY() + cInInv[6]*par->GetTx()
    //          + cInInv[7]*par->GetTy() + cInInv[8]*par->GetQp()
    //          + (- dxy * hit->GetX() + dxx * hit->GetY()) / det;
    //   t[2] = cInInv[2]*par->GetX() + cInInv[6]*par->GetY() + cInInv[9]*par->GetTx()
    //          + cInInv[10]*par->GetTy() + cInInv[11]*par->GetQp();
    //   t[3] = cInInv[3]*par->GetX() + cInInv[7]*par->GetY() + cInInv[10]*par->GetTx()
    //          + cInInv[12]*par->GetTy() + cInInv[13]*par->GetQp();
    //   t[4] = cInInv[4]*par->GetX() + cInInv[8]*par->GetY() + cInInv[11]*par->GetTx()
    //          + cInInv[13]*par->GetTy() + cInInv[14]*par->GetQp();
    //
    //   std::vector<Float_t> xOut(5);
    //   Mult15On5(C1, t, xOut);
    //
    //   // Copy filtered state to output
    //   par->SetX(xOut[0]);
    //   par->SetY(xOut[1]);
    //   par->SetTx(xOut[2]);
    //   par->SetTy(xOut[3]);
    //   par->SetQp(xOut[4]);
    //   par->SetCovMatrix(C1);
    //
    //   // Calculate chi square
    //   Float_t dx0 = xOut[0] - xIn[0];
    //   Float_t dx1 = xOut[1] - xIn[1];
    //   Float_t dx2 = xOut[2] - xIn[2];
    //   Float_t dx3 = xOut[3] - xIn[3];
    //   Float_t dx4 = xOut[4] - xIn[4];
    //   Float_t xmx = hit->GetX() - par->GetX();
    //   Float_t ymy = hit->GetY() - par->GetY();
    //   chiSq = ((xmx * dyy - ymy * dxy) * xmx + (-xmx * dxy + ymy * dxx) * ymy) / det
    //           + (dx0 * cInInv[0] + dx1 * cInInv[1] + dx2 * cInInv[2 ] + dx3 * cInInv[3 ] + dx4 * cInInv[4 ]) * dx0
    //           + (dx0 * cInInv[1] + dx1 * cInInv[5] + dx2 * cInInv[6 ] + dx3 * cInInv[7 ] + dx4 * cInInv[8 ]) * dx1
    //           + (dx0 * cInInv[2] + dx1 * cInInv[6] + dx2 * cInInv[9 ] + dx3 * cInInv[10] + dx4 * cInInv[11]) * dx2
    //           + (dx0 * cInInv[3] + dx1 * cInInv[7] + dx2 * cInInv[10] + dx3 * cInInv[12] + dx4 * cInInv[13]) * dx3
    //           + (dx0 * cInInv[4] + dx1 * cInInv[8] + dx2 * cInInv[11] + dx3 * cInInv[13] + dx4 * cInInv[14]) * dx4;
    //
    return kBMNSUCCESS;
}

BmnStatus BmnKalmanFilter::Update(FairTrackParam* par, const CbmStripHit* hit, Float_t& chiSq) { //FIXME
//    cout << "In strip updater\n";
    Float_t xIn[5] = {par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp()};
    Double_t cIn[15];
    par->CovMatrix(cIn);

    Float_t u = hit->GetU();
    Float_t duu = hit->GetDu() * hit->GetDu();
    Float_t phiCos = 1; //hit->GetCosPhi();                                       //FIXME
    Float_t phiSin = 1; //hit->GetSinPhi();                                       //FIXME
    Float_t phiCosSq = phiCos * phiCos;
    Float_t phiSinSq = phiSin * phiSin;
    Float_t phi2SinCos = 2 * phiCos * phiSin;

    // Inverted covariance matrix of predicted residual
    Float_t R = 1. / (duu + cIn[0] * phiCosSq + phi2SinCos * cIn[1] + cIn[5] * phiSinSq);

    // Calculate Kalman gain matrix
    Float_t K0 = cIn[0] * phiCos + cIn[1] * phiSin;
    Float_t K1 = cIn[1] * phiCos + cIn[5] * phiSin;
    Float_t K2 = cIn[2] * phiCos + cIn[6] * phiSin;
    Float_t K3 = cIn[3] * phiCos + cIn[7] * phiSin;
    Float_t K4 = cIn[4] * phiCos + cIn[8] * phiSin;

    Float_t KR0 = K0 * R;
    Float_t KR1 = K1 * R;
    Float_t KR2 = K2 * R;
    Float_t KR3 = K3 * R;
    Float_t KR4 = K4 * R;

    // Residual of predictions
    Float_t r = u - xIn[0] * phiCos - xIn[1] * phiSin;

    // Calculate filtered state vector
    std::vector<Float_t> xOut(5);
    xOut[0] = xIn[0] + KR0 * r;
    xOut[1] = xIn[1] + KR1 * r;
    xOut[2] = xIn[2] + KR2 * r;
    xOut[3] = xIn[3] + KR3 * r;
    xOut[4] = xIn[4] + KR4 * r;

    // Calculate filtered covariance matrix
    Double_t cOut[15];
    cOut[0] = cIn[0] - KR0 * K0;
    cOut[1] = cIn[1] - KR0 * K1;
    cOut[2] = cIn[2] - KR0 * K2;
    cOut[3] = cIn[3] - KR0 * K3;
    cOut[4] = cIn[4] - KR0 * K4;

    cOut[5] = cIn[5] - KR1 * K1;
    cOut[6] = cIn[6] - KR1 * K2;
    cOut[7] = cIn[7] - KR1 * K3;
    cOut[8] = cIn[8] - KR1 * K4;

    cOut[9] = cIn[9] - KR2 * K2;
    cOut[10] = cIn[10] - KR2 * K3;
    cOut[11] = cIn[11] - KR2 * K4;

    cOut[12] = cIn[12] - KR3 * K3;
    cOut[13] = cIn[13] - KR3 * K4;

    cOut[14] = cIn[14] - KR4 * K4;

    // Copy filtered state to output
    par->SetX(xOut[0]);
    par->SetY(xOut[1]);
    par->SetTx(xOut[2]);
    par->SetTy(xOut[3]);
    par->SetQp(xOut[4]);
    par->SetCovMatrix(cOut);
    
//    cout << par->GetX() << " " << par->GetY() << " " << par->GetTx() << " " << par->GetTy() << " " << par->GetQp() << endl;

    // Filtered resuduals
    Float_t ru = u - xOut[0] * phiCos - xOut[1] * phiSin;

    // Calculate chi-square
    chiSq = (ru * ru) / (duu - phiCosSq * cOut[0] - phi2SinCos * cOut[1] - phiSinSq * cOut[5]);

    return kBMNSUCCESS;
}

BmnStatus BmnKalmanFilter::UpdateWMF(FairTrackParam* par, const CbmStripHit* hit, Float_t& chiSq) {//FIXME
//       Float_t xIn[5] = { par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp() };
//    
//    //   std::vector<Float_t> cIn = par->GetCovMatrix();
//       Double_t cIn[15];
//       par->CovMatrix(cIn);
//    //   std::vector<Float_t> cInInv = par->GetCovMatrix();
//       Double_t cInInv[15];
//       par->CovMatrix(cInInv);
//    
//       Float_t duu = hit->GetDu() * hit->GetDu();
//       Float_t phiCos = 0;//hit->GetCosPhi();                                       //FIXME
//       Float_t phiSin = 1;//hit->GetSinPhi();                                       //FIXME
//    
//       // Inverse predicted cov matrix
//    //   InvSym15(cInInv);                                                          //FIXME
//       // Calculate C1
//       std::vector<Double_t> C1 = cInInv;
//       C1[0] += phiCos*phiCos / duu;
//       C1[1] += phiCos*phiSin / duu;
//       C1[5] += phiSin*phiSin / duu;
//       // Inverse C1 -> output updated covariance matrix
//    //   InvSym15(C1);                                                              //FIXME
//    
//       std::vector<Double_t> t(5);
//       t[0] = cInInv[0]*par->GetX() + cInInv[1]*par->GetY() + cInInv[2]*par->GetTx()
//              + cInInv[3]*par->GetTy() + cInInv[4]*par->GetQp()
//              + phiCos*hit->GetU()/duu;
//       t[1] = cInInv[1]*par->GetX() + cInInv[5]*par->GetY() + cInInv[6]*par->GetTx()
//              + cInInv[7]*par->GetTy() + cInInv[8]*par->GetQp()
//              + phiSin*hit->GetU()/duu;
//       t[2] = cInInv[2]*par->GetX() + cInInv[6]*par->GetY() + cInInv[9]*par->GetTx()
//              + cInInv[10]*par->GetTy() + cInInv[11]*par->GetQp();
//       t[3] = cInInv[3]*par->GetX() + cInInv[7]*par->GetY() + cInInv[10]*par->GetTx()
//              + cInInv[12]*par->GetTy() + cInInv[13]*par->GetQp();
//       t[4] = cInInv[4]*par->GetX() + cInInv[8]*par->GetY() + cInInv[11]*par->GetTx()
//              + cInInv[13]*par->GetTy() + cInInv[14]*par->GetQp();
//    
//       std::vector<Double_t> xOut(5);
//       Mult15On5(C1, t, xOut);
//    
//       // Copy filtered state to output
//       par->SetX(xOut[0]);
//       par->SetY(xOut[1]);
//       par->SetTx(xOut[2]);
//       par->SetTy(xOut[3]);
//       par->SetQp(xOut[4]);
//       par->SetCovMatrix(C1);
//    
//       // Calculate chi square
//       Float_t zeta = hit->GetU() - phiCos*xOut[0] - phiSin*xOut[1];
//       Float_t dx0 = xOut[0] - xIn[0];
//       Float_t dx1 = xOut[1] - xIn[1];
//       Float_t dx2 = xOut[2] - xIn[2];
//       Float_t dx3 = xOut[3] - xIn[3];
//       Float_t dx4 = xOut[4] - xIn[4];
//       chiSq = zeta * zeta / duu
//               + (dx0 * cInInv[0] + dx1 * cInInv[1] + dx2 * cInInv[2 ] + dx3 * cInInv[3 ] + dx4 * cInInv[4 ]) * dx0
//               + (dx0 * cInInv[1] + dx1 * cInInv[5] + dx2 * cInInv[6 ] + dx3 * cInInv[7 ] + dx4 * cInInv[8 ]) * dx1
//               + (dx0 * cInInv[2] + dx1 * cInInv[6] + dx2 * cInInv[9 ] + dx3 * cInInv[10] + dx4 * cInInv[11]) * dx2
//               + (dx0 * cInInv[3] + dx1 * cInInv[7] + dx2 * cInInv[10] + dx3 * cInInv[12] + dx4 * cInInv[13]) * dx3
//               + (dx0 * cInInv[4] + dx1 * cInInv[8] + dx2 * cInInv[11] + dx3 * cInInv[13] + dx4 * cInInv[14]) * dx4;
//    
    return kBMNSUCCESS;
}
