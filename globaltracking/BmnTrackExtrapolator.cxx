
#include "TMath.h"
#include "BmnTrackExtrapolator.h"
#include "BmnDefaultSettings.h"
#include "FairRunAna.h"

using TMath::Sqrt;

const Float_t BmnDefaultSettings::LINE_EXTRAPOLATION_START_Z = 450.; // z coordinate [cm] after which linear track extrapolation starts
const Float_t BmnDefaultSettings::ENERGY_LOSS_CONST = 0.00354; // energy lost constant [GeV/c] used in the simple energy loss calculation
const Float_t BmnDefaultSettings::MINIMUM_PROPAGATION_DISTANCE = 1e-6; // minimum propagation distance [cm]
const Float_t BmnDefaultSettings::MAXIMUM_TGEO_NAVIGATION_DISTANCE = 25.; // maximum distance used in the TGeo navigation

BmnTrackExtrapolator::BmnTrackExtrapolator() {
}

BmnTrackExtrapolator::~BmnTrackExtrapolator() {
}

//+++++++++++++++++++++++++++++RK4 EXTRAPOLATOR+++++++++++++++++++++++++++++++++++++//

BmnStatus BmnTrackExtrapolator::RK4TrackExtrapolate(FairTrackParam* par, Float_t zOut, vector<Double_t>* F) {

    Float_t zIn = par->GetZ();
    fField = FairRunAna::Instance()->GetField();

    vector<Double_t> xIn;
    xIn.resize(5);
    xIn[0] = par->GetX();
    xIn[1] = par->GetY();
    xIn[2] = par->GetTx();
    xIn[3] = par->GetTy();
    xIn[4] = par->GetQp();
    vector<Double_t> xOut(5, 0.);
    vector<Double_t> F1(25, 0.);

    RK4Order(xIn, zIn, xOut, zOut, F1);

    vector<Double_t> cIn(15, 0.0);
    Double_t cIn_tmp[15];
    par->CovMatrix(cIn_tmp);
    for (Int_t i = 0; i < 15; ++i) {
        cIn[i] = cIn_tmp[i];
    }
    vector<Double_t> cOut(15, 0.0);
    TransportC(cIn, F1, cOut);

    par->SetX(xOut[0]);
    par->SetY(xOut[1]);
    par->SetTx(xOut[2]);
    par->SetTy(xOut[3]);
    par->SetQp(xOut[4]);
    Double_t cOut_tmp[15];
    for (Int_t i = 0; i < 15; ++i) {
        cOut_tmp[i] = cOut[i];
    }
    par->SetCovMatrix(cOut_tmp);
    par->SetZ(zOut);

    if (F != NULL) F->assign(F1.begin(), F1.end());

    return kBMNSUCCESS;
}

void BmnTrackExtrapolator::RK4Order(const vector<Double_t>& xIn, Float_t zIn, vector<Double_t>& xOut, Float_t zOut, vector<Double_t>& derivs) const {
       const Float_t fC = 0.000299792458; //?????
    
       Float_t coef[4] = {0.0, 0.5, 0.5, 1.0};
    
       Float_t Ax[4], Ay[4];
       Float_t dAx_dtx[4], dAy_dtx[4], dAx_dty[4], dAy_dty[4];
       Float_t k[4][4];
    
       Float_t h = zOut - zIn;
       Float_t hC   = h * fC;
       Float_t hCqp = h * fC * xIn[4];
       Float_t x0[4];
    
       Float_t x[4] = {xIn[0], xIn[1], xIn[2], xIn[3]};

       for (UInt_t iStep = 0; iStep < 4; iStep++) { // 1
          if (iStep > 0) {
             for(UInt_t i = 0; i < 4; i++) {
                x[i] = xIn[i] + coef[iStep] * k[i][iStep - 1];
             }
          }
    
          Float_t Bx = fField->GetBx(x[0], x[1], zIn + coef[iStep] * h);
          Float_t By = fField->GetBy(x[0], x[1], zIn + coef[iStep] * h);
          Float_t Bz = fField->GetBz(x[0], x[1], zIn + coef[iStep] * h);
          
          Float_t tx = x[2];
          Float_t ty = x[3];
          Float_t txtx = tx * tx;
          Float_t tyty = ty * ty;
          Float_t txty = tx * ty;
          Float_t txtxtyty1= 1.0 + txtx + tyty;
          Float_t t1 = Sqrt(txtxtyty1);
          Float_t t2 = 1.0 / txtxtyty1;
    
          Ax[iStep] = ( txty * Bx + ty * Bz - ( 1.0 + txtx ) * By ) * t1;
          Ay[iStep] = (-txty * By - tx * Bz + ( 1.0 + tyty ) * Bx ) * t1;
    
          dAx_dtx[iStep] = Ax[iStep] * tx * t2 + ( ty * Bx - 2.0 * tx * By ) * t1;
          dAx_dty[iStep] = Ax[iStep] * ty * t2 + ( tx * Bx + Bz ) * t1;
          dAy_dtx[iStep] = Ay[iStep] * tx * t2 + (-ty * By - Bz ) * t1;
          dAy_dty[iStep] = Ay[iStep] * ty * t2 + (-tx * By + 2.0 * ty * Bx ) * t1;
    
          k[0][iStep] = tx * h;
          k[1][iStep] = ty * h;
          k[2][iStep] = Ax[iStep] * hCqp;
          k[3][iStep] = Ay[iStep] * hCqp;
    
       } // 1
    
       for (UInt_t i = 0; i < 4; i++) { xOut[i] = CalcOut(xIn[i], k[i]); }
       xOut[4] = xIn[4];
    
       // Calculation of the derivatives
    
       // derivatives dx/dx and dx/dy
       // dx/dx
       derivs[0] = 1.;
       derivs[5] = 0.;
       derivs[10] = 0.;
       derivs[15] = 0.;
       derivs[20] = 0.;
       // dx/dy
       derivs[1] = 0.;
       derivs[6] = 1.;
       derivs[11] = 0.;
       derivs[16] = 0.;
       derivs[21] = 0.;
       // end of derivatives dx/dx and dx/dy
    
       // Derivatives dx/tx
       x[0] = x0[0] = 0.0;
       x[1] = x0[1] = 0.0;
       x[2] = x0[2] = 1.0;
       x[3] = x0[3] = 0.0;
       for (UInt_t iStep = 0; iStep < 4; iStep++) { // 2
          if (iStep > 0) {
             for (UInt_t i = 0; i < 4; i++) {
                if (i != 2) {
                   x[i] = x0[i] + coef[iStep] * k[i][iStep - 1];
                }
             }
          }
    
          k[0][iStep] = x[2] * h;
          k[1][iStep] = x[3] * h;
          //k[2][iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
          k[3][iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
       } // 2
    
       derivs[2] = CalcOut(x0[0], k[0]);
       derivs[7] = CalcOut(x0[1], k[1]);
       derivs[12] = 1.0;
       derivs[17] = CalcOut(x0[3], k[3]);
       derivs[22] = 0.0;
       // end of derivatives dx/dtx
    
       // Derivatives    dx/ty
       x[0] = x0[0] = 0.0;
       x[1] = x0[1] = 0.0;
       x[2] = x0[2] = 0.0;
       x[3] = x0[3] = 1.0;
       for (UInt_t iStep = 0; iStep < 4; iStep++) { // 4
          if (iStep > 0) {
             for(UInt_t i = 0; i < 4; i++) {
                if(i!=3) {
                   x[i] = x0[i] + coef[iStep] * k[i][iStep - 1];
                }
             }
          }
    
          k[0][iStep] = x[2] * h;
          k[1][iStep] = x[3] * h;
          k[2][iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
          //k[3][iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
       }  // 4
    
       derivs[3] = CalcOut(x0[0], k[0]);
       derivs[8] = CalcOut(x0[1], k[1]);
       derivs[13] = CalcOut(x0[2], k[2]);
       derivs[18] = 1.;
       derivs[23] = 0.;
       // end of derivatives dx/dty
    
       // Derivatives dx/dqp
       x[0] = x0[0] = 0.0;
       x[1] = x0[1] = 0.0;
       x[2] = x0[2] = 0.0;
       x[3] = x0[3] = 0.0;
       for (UInt_t iStep = 0; iStep < 4; iStep++) { // 4
          if (iStep > 0) {
             for(UInt_t i = 0; i < 4; i++) {
                x[i] = x0[i] + coef[iStep] * k[i][iStep - 1];
             }
          }
    
          k[0][iStep] = x[2] * h;
          k[1][iStep] = x[3] * h;
          k[2][iStep] = Ax[iStep] * hC +
                        hCqp * (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]);
          k[3][iStep] = Ay[iStep] * hC +
                        hCqp * (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]);
       }  // 4
    
       derivs[4] = CalcOut(x0[0], k[0]);
       derivs[9] = CalcOut(x0[1], k[1]);
       derivs[14] = CalcOut(x0[2], k[2]);
       derivs[19] = CalcOut(x0[3], k[3]);
       derivs[24] = 1.;
     //end of derivatives dx/dqp

    // end calculation of the derivatives
}

Float_t BmnTrackExtrapolator::CalcOut(Float_t in, const Float_t k[4]) const {
    return in + k[0] / 6. + k[1] / 3. + k[2] / 3. + k[3] / 6.;
}

void BmnTrackExtrapolator::TransportC(
        const vector<Double_t>& cIn,
        const vector<Double_t>& F,
        vector<Double_t>& cOut) const {
    // F*C*Ft
    Float_t A = cIn[2] + F[2] * cIn[9] + F[3] * cIn[10] + F[4] * cIn[11];
    Float_t B = cIn[3] + F[2] * cIn[10] + F[3] * cIn[12] + F[4] * cIn[13];
    Float_t C = cIn[4] + F[2] * cIn[11] + F[3] * cIn[13] + F[4] * cIn[14];

    Float_t D = cIn[6] + F[7] * cIn[9] + F[8] * cIn[10] + F[9] * cIn[11];
    Float_t E = cIn[7] + F[7] * cIn[10] + F[8] * cIn[12] + F[9] * cIn[13];
    Float_t G = cIn[8] + F[7] * cIn[11] + F[8] * cIn[13] + F[9] * cIn[14];

    Float_t H = cIn[9] + F[13] * cIn[10] + F[14] * cIn[11];
    Float_t I = cIn[10] + F[13] * cIn[12] + F[14] * cIn[13];
    Float_t J = cIn[11] + F[13] * cIn[13] + F[14] * cIn[14];

    Float_t K = cIn[13] + F[17] * cIn[11] + F[19] * cIn[14];

    cOut[0] = cIn[0] + F[2] * cIn[2] + F[3] * cIn[3] + F[4] * cIn[4] + A * F[2] + B * F[3] + C * F[4];
    cOut[1] = cIn[1] + F[2] * cIn[6] + F[3] * cIn[7] + F[4] * cIn[8] + A * F[7] + B * F[8] + C * F[9];
    cOut[2] = A + B * F[13] + C * F[14];
    cOut[3] = B + A * F[17] + C * F[19];
    cOut[4] = C;

    cOut[5] = cIn[5] + F[7] * cIn[6] + F[8] * cIn[7] + F[9] * cIn[8] + D * F[7] + E * F[8] + G * F[9];
    cOut[6] = D + E * F[13] + G * F[14];
    cOut[7] = E + D * F[17] + G * F[19];
    cOut[8] = G;

    cOut[9] = H + I * F[13] + J * F[14];
    cOut[10] = I + H * F[17] + J * F[19];
    cOut[11] = J;

    cOut[12] = cIn[12] + F[17] * cIn[10] + F[19] * cIn[13] + (F[17] * cIn[9] + cIn[10] + F[19] * cIn[11]) * F[17] + K * F[19];
    cOut[13] = K;

    cOut[14] = cIn[14];
}


//+++++++++++++++++++++++++++++LINE EXTRAPOLATOR+++++++++++++++++++++++++++++++++++++//

BmnStatus BmnTrackExtrapolator::LineTrackExtrapolate(FairTrackParam* par, Float_t zOut, vector<Double_t>* F) {
    
    Float_t X[5] = {par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp()};

    Float_t dz = zOut - par->GetZ();

    //transport state vector F*X*F.T()
    X[0] = X[0] + dz * X[2];
    X[1] = X[1] + dz * X[3];

    par->SetX(X[0]);
    par->SetY(X[1]);

    //   vector<Float_t> C = par->GetCovMatrix();
    Double_t C[15];
    par->CovMatrix(C);
    //transport covariance matrix F*C*F.T()
    Float_t t3 = C[2] + dz * C[9];
    Float_t t7 = dz * C[10];
    Float_t t8 = C[3] + t7;
    Float_t t19 = C[7] + dz * C[12];
    C[0] = C[0] + dz * C[2] + t3 * dz;
    C[1] = C[1] + dz * C[6] + t8 * dz;
    C[2] = t3;
    C[3] = t8;
    C[4] = C[4] + dz * C[11];
    C[5] = C[5] + dz * C[7] + t19 * dz;
    C[6] = C[6] + t7;
    C[7] = t19;
    C[8] = C[8] + dz * C[13];

    par->SetCovMatrix(C);
    par->SetZ(zOut);

    // Transport matrix calculation
    if (F != NULL) {
        F->assign(25, 0.);
        (*F)[0] = 1.;
        (*F)[6] = 1.;
        (*F)[12] = 1.;
        (*F)[18] = 1.;
        (*F)[24] = 1.;
        (*F)[2] = dz;
        (*F)[8] = dz;
    }

    return kBMNSUCCESS;
}

//+++++++++++++++++++++++++++++CLEVER EXTRAPOLATOR+++++++++++++++++++++++++++++++++++++//

// NOT ADDED YET
