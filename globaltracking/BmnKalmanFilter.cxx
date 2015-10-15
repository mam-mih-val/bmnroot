/** BmnKalmanFilter.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2007-2014
 **/

#include "BmnKalmanFilter.h"
#include "FairTrackParam.h"
#include "BmnMatrixMath.h"

BmnKalmanFilter::BmnKalmanFilter() {
}

BmnKalmanFilter::~BmnKalmanFilter() {
}

BmnStatus BmnKalmanFilter::Update(FairTrackParam* par, const BmnHit* hit, Float_t& chiSq) { 

    //   vector<Float_t> cIn = par->GetCovMatrix();
    Double_t cIn[15];
    par->CovMatrix(cIn);

    static const Float_t ONE = 1., TWO = 2.;

    Double_t dxx = hit->GetDx() * hit->GetDx();
    Double_t dxy = 0.0;
    Double_t dyy = hit->GetDy() * hit->GetDy();

    // calculate residuals
    Double_t dx = hit->GetX() - par->GetX();
    Double_t dy = hit->GetY() - par->GetY();

    //    cout << "dX = " << dx << "\t dY = " << dy << endl;

    // Calculate and inverse residual covariance matrix
    Double_t t = (dxx * dyy + dxx * cIn[5] + dyy * cIn[0] + cIn[0] * cIn[5] - dxy * dxy - TWO * dxy * cIn[1] - cIn[1] * cIn[1]);
    if (t == 0.0) return kBMNERROR;
    else t = ONE / (dxx * dyy + dxx * cIn[5] + dyy * cIn[0] + cIn[0] * cIn[5] - dxy * dxy - TWO * dxy * cIn[1] - cIn[1] * cIn[1]);

    Double_t R00 = (dyy + cIn[5]) * t;
    Double_t R01 = -(dxy + cIn[1]) * t;
    Double_t R11 = (dxx + cIn[0]) * t;

    // Calculate Kalman gain matrix
    Double_t K00 = cIn[0] * R00 + cIn[1] * R01;
    Double_t K01 = cIn[0] * R01 + cIn[1] * R11;
    Double_t K10 = cIn[1] * R00 + cIn[5] * R01;
    Double_t K11 = cIn[1] * R01 + cIn[5] * R11;
    Double_t K20 = cIn[2] * R00 + cIn[6] * R01;
    Double_t K21 = cIn[2] * R01 + cIn[6] * R11;
    Double_t K30 = cIn[3] * R00 + cIn[7] * R01;
    Double_t K31 = cIn[3] * R01 + cIn[7] * R11;
    Double_t K40 = cIn[4] * R00 + cIn[8] * R01;
    Double_t K41 = cIn[4] * R01 + cIn[8] * R11;

    // Calculate filtered state vector
    Double_t xOut[5] = {par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp()};
    xOut[0] += K00 * dx + K01 * dy;
    //    cout << "xOut[0] = " << xOut[0] << endl;
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

    // Calculate chi-square
    Double_t xmx = hit->GetX() - par->GetX();
    Double_t ymy = hit->GetY() - par->GetY();
    Double_t C0 = cOut[0];
    Double_t C1 = cOut[1];
    Double_t C5 = cOut[5];

    Double_t norm = dxx * dyy - dxx * C5 - dyy * C0 + C0 * C5 - dxy * dxy + 2 * dxy * C1 - C1 * C1;

    chiSq = ((xmx * (dyy - C5) - ymy * (dxy - C1)) * xmx + (-xmx * (dxy - C1) + ymy * (dxx - C0)) * ymy) / norm;

    return kBMNSUCCESS;
}
