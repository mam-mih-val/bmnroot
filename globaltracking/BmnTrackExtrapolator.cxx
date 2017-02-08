
#include "TMath.h"
#include "BmnTrackExtrapolator.h"
#include "BmnDefaultSettings.h"
#include "FairRunAna.h"

using TMath::Sqrt;

const Float_t BmnDefaultSettings::LINE_EXTRAPOLATION_START_Z = 450.; // z coordinate [cm] after which linear track extrapolation starts
const Float_t BmnDefaultSettings::ENERGY_LOSS_CONST = 0.00354; // energy lost constant [GeV/c] used in the simple energy loss calculation
const Float_t BmnDefaultSettings::MINIMUM_PROPAGATION_DISTANCE = 1e-6; // minimum propagation distance [cm]
const Float_t BmnDefaultSettings::MAXIMUM_TGEO_NAVIGATION_DISTANCE = 25.; // maximum distance used in the TGeo navigation

const Int_t Mxbins = 114;
const Int_t Mybins = 46;
const Int_t Mzbins = 320;

float Mxmin = -113;
float Mymin = -45;
float Mzmin = -27;
float dmbin = (float)2;

#define QP_curls 20.
#define SIZE_IT 20.
#define RK_step_min 20.

float RK_scale = 1.;
float RK5_step_min = 2.;

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
    const Float_t fC = 0.000299792458;

    Float_t coef[4] = {0.0, 0.5, 0.5, 1.0};

    Float_t Ax[4], Ay[4];
    Float_t dAx_dtx[4], dAy_dtx[4], dAx_dty[4], dAy_dty[4];
    Float_t k[4][4];

    Float_t h = zOut - zIn;
    Float_t hC = h * fC;
    Float_t hCqp = h * fC * xIn[4];
    Float_t x0[4];

    Double_t x[4] = {xIn[0], xIn[1], xIn[2], xIn[3]};

    for (UInt_t iStep = 0; iStep < 4; iStep++) { // 1
        if (iStep > 0) {
            for (UInt_t i = 0; i < 4; i++) {
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
        Float_t txtxtyty1 = 1.0 + txtx + tyty;
        Float_t t1 = Sqrt(txtxtyty1);
        Float_t t2 = 1.0 / txtxtyty1;

        Ax[iStep] = (txty * Bx + ty * Bz - (1.0 + txtx) * By) * t1;
        Ay[iStep] = (-txty * By - tx * Bz + (1.0 + tyty) * Bx) * t1;

        dAx_dtx[iStep] = Ax[iStep] * tx * t2 + (ty * Bx - 2.0 * tx * By) * t1;
        dAx_dty[iStep] = Ax[iStep] * ty * t2 + (tx * Bx + Bz) * t1;
        dAy_dtx[iStep] = Ay[iStep] * tx * t2 + (-ty * By - Bz) * t1;
        dAy_dty[iStep] = Ay[iStep] * ty * t2 + (-tx * By + 2.0 * ty * Bx) * t1;

        k[0][iStep] = tx * h;
        k[1][iStep] = ty * h;
        k[2][iStep] = Ax[iStep] * hCqp;
        k[3][iStep] = Ay[iStep] * hCqp;

    } // 1

    for (UInt_t i = 0; i < 4; i++) {
        xOut[i] = CalcOut(xIn[i], k[i]);
    }
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
            for (UInt_t i = 0; i < 4; i++) {
                if (i != 3) {
                    x[i] = x0[i] + coef[iStep] * k[i][iStep - 1];
                }
            }
        }

        k[0][iStep] = x[2] * h;
        k[1][iStep] = x[3] * h;
        k[2][iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
        //k[3][iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
    } // 4

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
            for (UInt_t i = 0; i < 4; i++) {
                x[i] = x0[i] + coef[iStep] * k[i][iStep - 1];
            }
        }

        k[0][iStep] = x[2] * h;
        k[1][iStep] = x[3] * h;
        k[2][iStep] = Ax[iStep] * hC +
                hCqp * (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]);
        k[3][iStep] = Ay[iStep] * hC +
                hCqp * (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]);
    } // 4

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

// Without derivatives
//
// Forth-order Runge-Kutta method for solution of the equation 
// of motion of a particle with parameter qp = Q /P 
//              in the magnetic field B()
//
//        | x |          tx
//        | y |          ty 
// d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) ) 
//        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
//
//   where  ft = C*qp*sqrt ( 1 + tx**2 + ty**2 ) .
//
//  In the following for RK step  :
//     x=x[0], y=x[1], tx=x[3], ty=x[4].

void BmnTrackExtrapolator::RK4Fast_(              
        double& z_in,  // z value for input parameters
        double* p_in,  // input track parameters (x,y,tx,ty,Q/p)
        double& z_out, // z value for output parameters
        double* p_out) // output track parameters
{
    static float C = 0.000299792; // assume field in kG
    static float a[4] = {0., 0.5, 0.5, 1.};
    static float c[4] = {1. / 6., 1. / 3., 1. / 3., 1. / 6.};
    static float b[4] = {0., 0.5, 0.5, 1.};
    int step4;
    float qp, hC, h;
    float k[16], x0[4], x[4], k1[16];
    float tx, ty, tx2, ty2, txty, tx2ty2;
    float Ax[4], Ay[4];
    float B[3];
    //----------------------------------------------------------------

    qp = (float) p_in[4];
    h = z_out - z_in;
    hC = h * C;

    x0[0] = (float) p_in[0];
    x0[1] = (float) p_in[1];
    x0[2] = (float) p_in[2];
    x0[3] = (float) p_in[3];
    //
    //   Runge-Kutta step
    //
    for (int step = 0; step < 4; ++step) {
        for (int i = 0; i < 4; ++i) {
            if (step == 0) {
                x[i] = x0[i];
            } else {
                x[i] = x0[i] + b[step] * k[step * 4 - 4 + i];
            }
        }

        B[0] = fField->GetBx(x[0], x[1], (float) z_in + a[step] * h);
        B[1] = fField->GetBy(x[0], x[1], (float) z_in + a[step] * h);
        B[2] = fField->GetBz(x[0], x[1], (float) z_in + a[step] * h);
        tx = x[2];
        ty = x[3];
        tx2 = tx * tx;
        ty2 = ty * ty;
        txty = tx * ty;
        tx2ty2 = sqrt(1. + tx2 + ty2) * hC;
        Ax[step] = (txty * B[0] + ty * B[2] - (1. + tx2) * B[1]) * tx2ty2;
        Ay[step] = (-txty * B[1] - tx * B[2] + (1. + ty2) * B[0]) * tx2ty2;

        step4 = step * 4;
        k[step4 ] = tx * h;
        k[step4 + 1] = ty * h;
        k[step4 + 2] = Ax[step] * qp;
        k[step4 + 3] = Ay[step] * qp;

    } // end of Runge-Kutta steps  
    for (int i = 0; i < 4; ++i) {
        p_out[i] = x0[i] + c[0] * k[i] + c[1] * k[4 + i] + c[2] * k[8 + i] + c[3] * k[12 + i];
    }
    p_out[4] = p_in[4];

} // end of RK4Fast without derivatives

// = 0 ok
// = 1 track curls
//
// Forth-order Runge-Kutta method for solution of the equation
// of motion of a particle with parameter qp = Q /P
//              in the magnetic field B()
//
//        | x |          tx
//        | y |          ty
// d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) )
//        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
//
//   where  ft = C*qp*sqrt ( 1 + tx**2 + ty**2 ) .
//
//  In the following for RK step  :
//     x=x[0], y=x[1], tx=x[3], ty=x[4].

void BmnTrackExtrapolator::RK4Fast_(
        double& z_in,  // z value for input parameters
        double* p_in,  // input track parameters (x,y,tx,ty,Q/p)
        double& z_out, // z value for output parameters
        double* p_out, // output track parameters
        double* rkd,   // derivatives d p_out[0-4] / d p_in[0-4]
        int& ierror)
{
    static float C = 0.000299792; // assume field in kG
    static float a[4] = {0., 0.5, 0.5, 1.};
    static float c[4] = {1. / 6., 1. / 3., 1. / 3., 1. / 6.};
    static float b[4] = {0., 0.5, 0.5, 1.};
    int step4;
    float qp, hC, h;
    float k[16], x0[4], x[4], k1[16];
    float tx, ty, tx2, ty2, txty, tx2ty2;
    float Ax[4], Ay[4];
    float B[3];
    //----------------------------------------------------------------
    qp = (float) p_in[4];
    ierror = (fabs(qp) > QP_curls) ? 1 : 0;
    h = z_out - z_in;
    hC = h * C;
    x0[0] = (float) p_in[0];
    x0[1] = (float) p_in[1];
    x0[2] = (float) p_in[2];
    x0[3] = (float) p_in[3];
    //
    //   Runge-Kutta step
    //
    for (int step = 0; step < 4; ++step) {
        for (int i = 0; i < 4; ++i) {
            if (step == 0) {
                x[i] = x0[i];
            } else {
                x[i] = x0[i] + b[step] * k[step * 4 - 4 + i];
            }
        }
        B[0] = fField->GetBx(x[0], x[1], (float) z_in + a[step] * h);
        B[1] = fField->GetBy(x[0], x[1], (float) z_in + a[step] * h);
        B[2] = fField->GetBz(x[0], x[1], (float) z_in + a[step] * h);
        tx = x[2];
        ty = x[3];
        tx2 = tx * tx;
        ty2 = ty * ty;
        txty = tx * ty;
        tx2ty2 = sqrt(1. + tx2 + ty2) * hC;
        Ax[step] = (txty * B[0] + ty * B[2] - (1. + tx2) * B[1]) * tx2ty2;
        Ay[step] = (-txty * B[1] - tx * B[2] + (1. + ty2) * B[0]) * tx2ty2;

        step4 = step * 4;
        k[step4 ] = tx * h;
        k[step4 + 1] = ty * h;
        k[step4 + 2] = Ax[step] * qp;
        k[step4 + 3] = Ay[step] * qp;

    } // end of Runge-Kutta steps

    for (int i = 0; i < 4; ++i) {
        p_out[i] = x0[i] + c[0] * k[i] + c[1] * k[4 + i] + c[2] * k[8 + i] + c[3] * k[12 + i];
    }
    p_out[4] = p_in[4];

    //
    //     Derivatives    dx/dqp and dtx/dqp
    //
    //   Runge-Kutta step for derivatives dx/dqp

    for (int step = 0; step < 4; ++step) {
        for (int i = 0; i < 4; ++i) {
            if (step == 0) {
                x[0] = 0.;
                x[2] = 0.;
            } else {
                x[0] = b[step] * k1[step * 4 - 4];
                x[2] = b[step] * k1[step * 4 - 2];
            }
        }
        step4 = step * 4;
        k1[step4 ] = x[2] * h;
        k1[step4 + 2] = Ax[step];

    } // end of Runge-Kutta steps for derivatives dx/dqp

    rkd[20] = c[0] * k1[0] + c[1] * k1[4] + c[2] * k1[8] + c[3] * k1[12];
    rkd[21] = 0.;
    rkd[22] = c[0] * k1[2] + c[1] * k1[6] + c[2] * k1[10] + c[3] * k1[14];
    rkd[23] = 0.;
    rkd[24] = 1.;

    //      end of derivatives dx/dqp
    //
    //      other derivatives

    for (int i = 0; i < 19; ++i) {
        rkd[i] = 0.;
    }
    rkd[0] = 1.;
    rkd[6] = 1.;
    rkd[10] = h;
    rkd[12] = 1.;
    rkd[16] = h;
    rkd[18] = 1.;

} // end of RK4Fast

//+++++++++++++++++++++++++++++RK5 EXTRAPOLATOR++++++++++++++++++++++++++++++++++++++//

// Without derivatives
//
// Fifth-order Runge-Kutta method with adaptive stepsize control
// for solution of the equation of motion of a particle with 
//       parameter qp = Q /P in the magnetic field B()  
//
//        | x |          tx
//        | y |          ty 
// d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) ) 
//        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
//
//   where  ft = C*qp*sqrt ( 1 + tx**2 + ty**2 ) .
//
//  In the following for RK step  :
//     x=x[0], y=x[1], tx=x[3], ty=x[4].

void BmnTrackExtrapolator::RK5Fast_(
        double& z_in, // z value for input parameters
        double* p_in, // input track parameters (x,y,tx,ty,Q/p)
        float* error, // acceptable errors in cm
        // error(1) for Inner tracker
        // error(2) for Outer tracker
        double& z_out, // z value for output parameters
        double* p_out)// output track parameters
{

    static float C = 0.000299792; // assume field in kG

    static float a[6] = {0., 0.2, 0.3, 0.6, 1., 7. / 8.};
    static float c[6] = {37. / 378., 0., 250. / 621., 125. / 594., 0., 512. / 1771.};
    static float c1[6] = {2825. / 27648., 0., 18575. / 48384., 13525. / 55296.,
        277. / 14336., 1. / 4.};
    static float b[16] = {0., 1. / 5., 3. / 40., 9. / 40., 3. / 10., -9. / 10., 6. / 5.,
        -11. / 54., 5. / 2., -70. / 27., 35. / 27.,
        1631. / 55296., 175. / 512., 575. / 13824., 44275. / 110592., 253. / 4096.};

    int step_j, step4;
    float qp, hC, h;
    float k[24], x0[4], x[4], k1[24];
    float tx, ty, tx2, ty2, txty, tx2ty2;
    float Ax[6], Ay[6];
    float B[3];

    double p1_out[5], z2_out, p2_out[5], rkd1[25], rkd2[25];

    //----------------------------------------------------------------

    qp = (float) p_in[4];
    h = z_out - z_in;
    hC = h * C;

    x0[0] = (float) p_in[0];
    x0[1] = (float) p_in[1];
    x0[2] = (float) p_in[2];
    x0[3] = (float) p_in[3];

    //
    //   Runge-Kutta step
    //

    for (int step = 0; step < 6; ++step) {
        for (int i = 0; i < 4; ++i) {
            x[i] = x0[i];
            step_j = ((step - 1) * step) / 2 + 1;
            for (int j = 0; j < step; ++j) {
                x[i] += b[step_j + j] * k[j * 4 + i];
            }
        }

        B[0] = fField->GetBx(x[0], x[1], (float) z_in + a[step] * h);
        B[1] = fField->GetBy(x[0], x[1], (float) z_in + a[step] * h);
        B[2] = fField->GetBz(x[0], x[1], (float) z_in + a[step] * h);

        tx = x[2];
        ty = x[3];
        tx2 = tx * tx;
        ty2 = ty * ty;
        txty = tx * ty;
        tx2ty2 = sqrt(1. + tx2 + ty2) * hC;
        Ax[step] = (txty * B[0] + ty * B[2] - (1. + tx2) * B[1]) * tx2ty2;
        Ay[step] = (-txty * B[1] - tx * B[2] + (1. + ty2) * B[0]) * tx2ty2;

        step4 = step * 4;
        k[step4 ] = tx * h;
        k[step4 + 1] = ty * h;
        k[step4 + 2] = Ax[step] * qp;
        k[step4 + 3] = Ay[step] * qp;

    } // end of Runge-Kutta steps  

    for (int i = 0; i < 4; ++i) {
        p_out[i] = x0[i] + c[0] * k[i] + c[2] * k[8 + i] + c[3] * k[12 + i] + c[5] * k[20 + i];
    }

    p_out[4] = p_in[4];

    //
    //     The embedded fourth-order formula for x 
    //
    p1_out[0] = x0[0] + c1[0] * k[0] + c1[2] * k[8] + c1[3] * k[12] + c1[4] * k[16] + c1[5] * k[20];

    //
    //      stepsize control
    //
    if ((fabs(p1_out[0] - p_out[0]) > error[0] &&
            (fabs(p_out[0]) < SIZE_IT && fabs(p_out[1]) < SIZE_IT)) ||
            (fabs(p1_out[0] - p_out[0]) > error[1])) {
        if (fabs(h) > RK_step_min * 3.) {
            z2_out = z_in + 0.5 * h;
            RK5Fast_(z_in, p_in, error, z2_out, p2_out);
            RK5Fast_(z2_out, p2_out, error, z_out, p_out);
        } else if (fabs(h) > RK_step_min) {
            z2_out = z_in + 0.5 * h;
            RK4Fast_(z_in, p_in, z2_out, p2_out);
            RK4Fast_(z2_out, p2_out, z_out, p_out);
        }
    } // end of stepsize control

} // end of RK5Fast without derivatives

// Fifth-order Runge-Kutta method with adaptive stepsize control
// for solution of the equation of motion of a particle with
//       parameter qp = Q /P in the magnetic field B()
//
//        | x |          tx
//        | y |          ty
// d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) )
//        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
//
//   where  ft = C*qp*sqrt ( 1 + tx**2 + ty**2 ) .
//
//  In the following for RK step  :
//     x=x[0], y=x[1], tx=x[3], ty=x[4].

void BmnTrackExtrapolator::RK5Fast_(
        double& z_in,  // z value for input parameters
        double* p_in,  // input track parameters (x,y,tx,ty,Q/p)
        float* error,  // acceptable errors in cm
        // error(1) for Inner tracker
        // error(2) for Outer tracker
        double& z_out, // z value for output parameters
        double* p_out, // output track parameters
        double* rkd,   // derivatives d p_out[0-4] / d p_in[0-4]
        int& ierror)   // = 0 ok, = 1 track curls
{
    static float C = 0.000299792; // assume field in kG

    static float a[6] = {0., 0.2, 0.3, 0.6, 1., 7. / 8.};
    static float c[6] = {37. / 378., 0., 250. / 621., 125. / 594., 0., 512. / 1771.};
    static float c1[6] = {2825. / 27648., 0., 18575. / 48384., 13525. / 55296.,
        277. / 14336., 1. / 4.};
    static float b[16] = {0., 1. / 5., 3. / 40., 9. / 40., 3. / 10., -9. / 10., 6. / 5.,
        -11. / 54., 5. / 2., -70. / 27., 35. / 27.,
        1631. / 55296., 175. / 512., 575. / 13824., 44275. / 110592., 253. / 4096.};

    int step_j, step4;
    float qp, hC, h;
    float k[24], x0[4], x[4], k1[24];
    float tx, ty, tx2, ty2, txty, tx2ty2;
    float Ax[6], Ay[6];

    float B[3];

    double p1_out[5], z2_out, p2_out[5], rkd1[25], rkd2[25];

    //----------------------------------------------------------------

    qp = (float) p_in[4];
    ierror = (fabs(qp) > QP_curls) ? 1 : 0;
    h = z_out - z_in;
    hC = h * C;

    x0[0] = (float) p_in[0];
    x0[1] = (float) p_in[1];
    x0[2] = (float) p_in[2];
    x0[3] = (float) p_in[3];

    //
    //   Runge-Kutta step
    //

    for (int step = 0; step < 6; ++step) {
        for (int i = 0; i < 4; ++i) {
            x[i] = x0[i];
            step_j = ((step - 1) * step) / 2 + 1;
            for (int j = 0; j < step; ++j) {
                x[i] += b[step_j + j] * k[j * 4 + i];
            }
        }

        B[0] = fField->GetBx(x[0], x[1], (float) z_in + a[step] * h);
        B[1] = fField->GetBy(x[0], x[1], (float) z_in + a[step] * h);
        B[2] = fField->GetBz(x[0], x[1], (float) z_in + a[step] * h);

        tx = x[2];
        ty = x[3];
        tx2 = tx * tx;
        ty2 = ty * ty;
        txty = tx * ty;
        tx2ty2 = sqrt(1. + tx2 + ty2) * hC;

        Ax[step] = (txty * B[0] + ty * B[2] - (1. + tx2) * B[1]) * tx2ty2;
        Ay[step] = (-txty * B[1] - tx * B[2] + (1. + ty2) * B[0]) * tx2ty2;

        step4 = step * 4;
        k[step4 ] = tx * h;
        k[step4 + 1] = ty * h;
        k[step4 + 2] = Ax[step] * qp;
        k[step4 + 3] = Ay[step] * qp;

    } // end of Runge-Kutta steps

    for (int i = 0; i < 4; ++i) {
        p_out[i] = x0[i] + c[0] * k[i] + c[2] * k[8 + i] + c[3] * k[12 + i] + c[5] * k[20 + i];
    }
    p_out[4] = p_in[4];


    //
    //     The embedded fourth-order formula for x
    //
    p1_out[0] = x0[0] + c1[0] * k[0] + c1[2] * k[8] + c1[3] * k[12] + c1[4] * k[16] + c1[5] * k[20];

    //     Derivatives    dx/dqp
    //
    //
    //   Runge-Kutta step for derivatives dx/dqp

    for (int step = 0; step < 6; ++step) {
        x[0] = 0.;
        step_j = ((step - 1) * step) / 2 + 1;
        for (int j = 0; j < step; ++j) {
            x[0] += b[step_j + j] * k1[j * 4 ];
        }
        x[2] = 0.;
        step_j = ((step - 1) * step) / 2 + 1;
        for (int j = 0; j < step; ++j) {
            x[2] += b[step_j + j] * k1[j * 4 + 2];
        }

        step4 = step * 4;
        k1[step4 ] = x[2] * h;
        k1[step4 + 2] = Ax[step];

    } // end of Runge-Kutta steps for derivatives dx/dqp

    rkd[20] = c[0] * k1[0] + c[2] * k1[8 ] + c[3] * k1[12] + c[5] * k1[20];
    rkd[21] = 0.;
    rkd[22] = c[0] * k1[2] + c[2] * k1[10] + c[3] * k1[14] + c[5] * k1[22];
    rkd[23] = 0.;
    rkd[24] = 1.;

    //      end of derivatives dx/dqp
    //
    //
    //      other derivatives

    for (int i = 0; i < 19; ++i) {
        rkd[i] = 0.;
    }
    rkd[0] = 1.;
    rkd[6] = 1.;
    rkd[10] = h;
    rkd[12] = 1.;
    rkd[16] = h;
    rkd[18] = 1.;

    //
    //      stepsize control
    //
    if ((fabs(p1_out[0] - p_out[0]) > error[0] &&
            (fabs(p_out[0]) < SIZE_IT && fabs(p_out[1]) < SIZE_IT)) ||
            (fabs(p1_out[0] - p_out[0]) > error[1])) {
        if (fabs(h) > RK_step_min * 3.) {
            z2_out = z_in + 0.5 * h;
            RK5Fast_(z_in, p_in, error, z2_out, p2_out);
            RK5Fast_(z2_out, p2_out, error, z_out, p_out);
        } else if (fabs(h) > RK_step_min) {
            z2_out = z_in + 0.5 * h;
            RK4Fast_(z_in, p_in, z2_out, p2_out);
            RK4Fast_(z2_out, p2_out, z_out, p_out);
        }
    } // end of stepsize control

    //      printf("p_out %8f3 %8f3 %8f3 %8f3  \n"
    //      ,p_out[0],p_out[1],p_out[2],p_out[3]);

} // end of RK5Fast

//
// Fifth-order Runge-Kutta method with adaptive stepsize control
// for solution of the equation of motion of a particle with 
//       parameter qp = Q /P in the magnetic field B()  
//
//        | x |          tx
//        | y |          ty 
// d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) ) 
//        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
//
//   where  ft = C*qp*sqrt ( 1 + tx**2 + ty**2 ) .
//
//  In the following for RK step  :
//     x=x[0], y=x[1], tx=x[3], ty=x[4].

void BmnTrackExtrapolator::RK5Order_(
		 
		 double z_in , // z value for input parameters
		 double* p_in,  // input track parameters (x,y,tx,ty,Q/p)
		 float* error,  // acceptable errors in cm
                                // error(1) for Inner tracker
                                // error(2) for Outer tracker
		 double& z_out, // z value for output parameters
		 double* p_out, // output track parameters
		 double* rkd,   // derivatives d p_out[0-4] / d p_in[0-4]
		 int&    ierror)// = 0 ok, = 1 track curls
{
  static float C  = 0.000299792;          // assume field in kG
  static float a[6]  = {0., 0.2, 0.3, 0.6, 1., 7./8.};
  static float c[6]  = {37./378., 0., 250./621., 125./594., 0., 512./1771.};
  static float c1[6] = {2825./27648., 0., 18575./48384., 13525./55296.,
			  277./14336.,1./4.};
  static float b[16] = {0., 1./5., 3./40., 9./40., 3./10., -9./10., 6./5.,
  -11./54., 5./2., -70./27., 35./27.,
  1631./55296., 175./512., 575./13824., 44275./110592., 253./4096.};

  float qp,hC,h;
  float k[24],x0[4],x[4],k1[24];
  float tx,ty,tx2,ty2,txty,tx2ty2,tx2ty2qp,tx2ty21,I_tx2ty2,I_tx2ty21;
  float Ax[6],Ay[6],Ax_tx[6],Ay_tx[6],Ax_ty[6],Ay_ty[6];
  float B[3];

  int step_j,step4;
  double p1_out[5],z2_out,p2_out[5],rkd1[25],rkd2[25];

//----------------------------------------------------------------

//  cout << "\n Z_in  " << z_in << " Z_out " << z_out << endl;

  qp    = (float) p_in[4];
  ierror = (fabs(qp) > QP_curls) ? 1 : 0;
  h = z_out - z_in;
  hC   = h * C;
  
  x0[0] = (float) p_in[0];
  x0[1] = (float) p_in[1];
  x0[2] = (float) p_in[2];
  x0[3] = (float) p_in[3];

//
//   Runge-Kutta step
//

  for (int step = 0; step < 6; ++step)
    {
      for(int i=0; i < 4; ++i)
	{
	  x[i] = x0[i]; step_j = ((step-1)*step)/2 + 1;
	  for(int j=0; j < step; ++j)
	    {
	      x[i] += b[step_j + j] * k[j*4+i];
	    }
	}

      B[0] = fField->GetBx(x[0], x[1], (float) z_in + a[step] * h);
      B[1] = fField->GetBy(x[0], x[1], (float) z_in + a[step] * h);
      B[2] = fField->GetBz(x[0], x[1], (float) z_in + a[step] * h);

      tx = x[2]; ty = x[3]; tx2 = tx * tx; ty2 = ty * ty; txty = tx * ty;
      tx2ty21= 1. + tx2 + ty2; I_tx2ty21 = 1. / tx2ty21 * qp;
      //tx2ty2 = sqrtf( tx2ty21 ) ;
      tx2ty2 = sqrt( tx2ty21 ) ;
      I_tx2ty2 = qp * hC / tx2ty2 ; 
      tx2ty2 *= hC; tx2ty2qp = tx2ty2 * qp; 

      Ax[step] = ( txty * B[0] + ty * B[2] - ( 1. + tx2 ) * B[1] ) * tx2ty2; 
      Ay[step] = (-txty * B[1] - tx * B[2] + ( 1. + ty2 ) * B[0] ) * tx2ty2;

      Ax_tx[step] = Ax[step]*tx*I_tx2ty21 + (ty*B[0]-2.*tx*B[1])*tx2ty2qp;
      Ax_ty[step] = Ax[step]*ty*I_tx2ty21 + (tx*B[0]+B[2])*tx2ty2qp;
      Ay_tx[step] = Ay[step]*tx*I_tx2ty21 + (-ty*B[1]-B[2])*tx2ty2qp;
      Ay_ty[step] = Ay[step]*ty*I_tx2ty21 + (-tx*B[1]+2.*ty*B[0])*tx2ty2qp;
       
      step4 = step * 4;
      k[step4  ] = tx * h;
      k[step4+1] = ty * h;
      k[step4+2] = Ax[step] * qp;
      k[step4+3] = Ay[step] * qp;

    }  // end of Runge-Kutta steps  

  for(int i=0; i < 4; ++i)
    {
      p_out[i]=x0[i]+c[0]*k[i]+c[2]*k[8+i]+c[3]*k[12+i]+c[5]*k[20+i];
    }
  p_out[4]=p_in[4];

//      printf("p_out %8f3 %8f3 %8f3 %8f3  \n"
//      ,p_out[0],p_out[1],p_out[2],p_out[3]);  
  
//
//     The embedded fourth-order formula for x and y
//
  for (int i = 0; i < 2; ++i)
    {
      p1_out[i]=x0[i]+c1[0]*k[i]+c1[2]*k[8+i]+c1[3]*k[12+i]+c1[4]*k[16+i]
	             +c1[5]*k[20+i];
    }

//
//     Derivatives    dx/dqp
//

  x0[0] = 0.; x0[1] = 0.; x0[2] = 0.; x0[3] = 0.;

//
//   Runge-Kutta step for derivatives dx/dqp

  for (int step = 0; step < 6; ++step)
    
    {
      for(int i=0; i < 4; ++i)
	{
	  x[i] = x0[i]; step_j = ((step-1)*step)/2 + 1;
	  for(int j=0; j < step; ++j)
	    {
	      x[i] += b[step_j + j] * k1[j*4+i];
	    }
	}
      step4 = step * 4;
      k1[step4  ] = x[2] * h;
      k1[step4+1] = x[3] * h;
      k1[step4+2] = Ax[step] + Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
      k1[step4+3] = Ay[step] + Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

    }  // end of Runge-Kutta steps for derivatives dx/dqp

  for (int i = 0; i < 4; ++i ) 
    { 
      rkd[20+i]=x0[i]+c[0]*k1[i]+c[2]*k1[8+i]+c[3]*k1[12+i]+c[5]*k1[20+i];
    }  
  rkd[24] = 1.;

//      end of derivatives dx/dqp
//
//     Derivatives    dx/tx
//

  x0[0] = 0; x0[1] = 0.; x0[2] = 1.; x0[3] = 0.;

//
//   Runge-Kutta step for derivatives dx/dtx
//

  for (int step = 0; step < 6; ++step)    
    {
      for(int i=0; i < 4; ++i)
	{
	  x[i] = x0[i]; step_j = ((step-1)*step)/2 + 1;
	  for(int j=0; j < step; ++j)
	    {                                                 
	      if(i != 2) {x[i] += b[step_j + j] * k1[j*4+i];} // tx fixed
	    }
	}
      step4 = step * 4;
      k1[step4  ] = x[2] * h;
      k1[step4+1] = x[3] * h;
//    k1[step4+2] = Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
      k1[step4+3] = Ay_tx[step] * x[2] + Ay_ty[step] * x[3];
	  
    }  // end of Runge-Kutta steps for derivatives dx/dtx 

  for (int i = 0; i < 4; ++i ) 
    { 
      if(i != 2) 
	{
	  rkd[10+i]=x0[i]+c[0]*k1[i]+c[2]*k1[8+i]+c[3]*k1[12+i]+c[5]*k1[20+i];
	}
    }  
//      end of derivatives dx/dtx
  rkd[12] = 1.;
  rkd[14] = 0.;

//     Derivatives    dx/ty
//

  x0[0] = 0; x0[1] = 0.; x0[2] = 0.; x0[3] = 1.;

//
//   Runge-Kutta step for derivatives dx/dty
//

  for (int step = 0; step < 6; ++step)    
   
    {
      for(int i=0; i < 4; ++i)                              
	{
	  x[i] = x0[i]; step_j = ((step-1)*step)/2 + 1;
	  for(int j=0; j < step; ++j)
	    {
	      if( i != 3){ x[i] += b[step_j + j] * k1[j*4+i];} //    ty fixed
	    }
	}
      step4 = step * 4;
      k1[step4  ] = x[2] * h;
      k1[step4+1] = x[3] * h;
      k1[step4+2] = Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
//    k1[step4+3] = Ay_tx[step] * x[2] + Ay_ty[step] * x[3];
	  
    }  // end of Runge-Kutta steps for derivatives dx/dty 

  for (int i = 0; i < 3; ++i ) 
    { 
      rkd[15+i]=x0[i]+c[0]*k1[i]+c[2]*k1[8+i]+c[3]*k1[12+i]+c[5]*k1[20+i];
    }  
//      end of derivatives dx/dty
  rkd[18] = 1.;
  rkd[19] = 0.;

//
//    derivatives dx/dx and dx/dy

    for(int i = 0; i < 10; ++i){ rkd[i] = 0.;}
  rkd[0] = 1.; rkd[6] = 1.;

//
//      stepsize control
//
//  if( (fabs(p1_out[0]-p_out[0])  > error[0] && 
//       (fabs(p_out[0]) < SIZE_IT && fabs(p_out[1]) < SIZE_IT ) ) ||
//      (fabs(p1_out[0]-p_out[0])  > error[1] ) )
 
  if( 
      ((fabs(p1_out[0]-p_out[0])  > error[0] && 
       (fabs(p_out[0]) < SIZE_IT && fabs(p_out[1]) < SIZE_IT ) ) ||
      (fabs(p1_out[0]-p_out[0])  > error[1] ))
      ||
      ((fabs(p1_out[1]-p_out[1])  > error[0] && 
       (fabs(p_out[0]) < SIZE_IT && fabs(p_out[1]) < SIZE_IT ) ) ||
      (fabs(p1_out[1]-p_out[1])  > error[1] ))
      )
    {
      if(fabs(h) > RK5_step_min ) 
	{
	  z2_out = z_in + 0.5 * h;
	  RK5Order_(z_in  , p_in,   error, z2_out, p2_out, rkd1, ierror);
	  RK5Order_(z2_out, p2_out, error, z_out,  p_out , rkd2, ierror);

	  rkd[0]  = 1.; rkd[1] = 0.; rkd[2] = 0.; rkd[3] = 0.; rkd[4] = 0;
	  rkd[5]  = 0.; rkd[6] = 1.; rkd[7] = 0.; rkd[8] = 0.; rkd[4] = 0;
	  rkd[10] = rkd1[10] + rkd2[10]            + rkd2[15] * rkd1[13];
	  rkd[11] = rkd1[11] + rkd2[11]            + rkd2[16] * rkd1[13];
	  rkd[12] = 1.;
	  rkd[13] =            rkd2[13]            +            rkd1[13];
          rkd[14] = 0.;
	  rkd[15] = rkd1[15] + rkd2[10] * rkd1[17] + rkd2[15];
	  rkd[16] = rkd1[16] + rkd2[11] * rkd1[17] + rkd2[16];
	  rkd[17] =                       rkd1[17] + rkd2[17];
	  rkd[18] = 1.;
	  rkd[19] = 0.;
	  rkd[20] = rkd1[20] + rkd2[10] * rkd1[22] + rkd2[15] * rkd1[23]
	           +rkd2[20];
	  rkd[21] = rkd1[21] + rkd2[11] * rkd1[22] + rkd2[16] * rkd1[23]
	           +rkd2[21];
	  rkd[22] =                       rkd1[22] + rkd2[17] * rkd1[23]
	           +rkd2[22];
	  rkd[23] =            rkd2[13] * rkd1[22] +            rkd1[23]
	           +rkd2[23];
	  rkd[24] = 1.;

	}

    }       // end of stepsize control

}    // end of RK5Order

//+++++++++++++++++++++++++++++EXTRAPOLATOR++++++++++++++++++++++++++++++++++++++++++//

// Transport the vector of track parameters p_in defined at the
// reference plane z_in to the vector p_out defined at z_out
//      using Runge-Kutta methods of different orders

void BmnTrackExtrapolator::RKTrans_(
        double z_in,  // z value for input parameters
        double* p_in,  // input track parameters (x,y,tx,ty,Q/p)
        double& z_out, // z value for output parameters
        double* p_out, // output track parameters
        double* rkd,   // derivatives d p_out(1-5) / d p_in(1-5)
        int& ierror    // = 0 ok, = 1 track curls
        )
{
    static float C = 0.000299792; // assume field in kG
    static float error[2] = {0.0025, 0.0050}; // IT , OT - errors
    float qp, hC, h;
    float tx, ty, tx2, ty2, txty, tx2ty2;
    float Ax, Ay, Axqp, Ayqp;
    float B[3];
    //----------------------------------------------------------------

    h = z_out - z_in;

    if (fabs(h) < RK_step_min) // short distance
    {
        qp = (float) p_in[4];
        ierror = (fabs(qp) > QP_curls) ? 1 : 0;
        hC = h * C;

        B[0] = fField->GetBx((float) p_in[0], (float) p_in[1], (float) z_in);
        B[1] = fField->GetBy((float) p_in[0], (float) p_in[1], (float) z_in);
        B[2] = fField->GetBz((float) p_in[0], (float) p_in[1], (float) z_in);

        tx = (float) p_in[2];
        ty = (float) p_in[3];
        tx2 = tx * tx;
        ty2 = ty * ty;
        txty = tx * ty;
        tx2ty2 = sqrt(1. + tx2 + ty2) * hC;
        Ax = (txty * B[0] + ty * B[2] - (1. + tx2) * B[1]) * tx2ty2;
        Ay = (-txty * B[1] - tx * B[2] + (1. + ty2) * B[0]) * tx2ty2;
        Axqp = Ax * qp;
        Ayqp = Ay * qp;

        p_out[0] = p_in[0] + tx * h + 0.5 * Axqp * h;
        p_out[1] = p_in[1] + ty * h + 0.5 * Ayqp * h;
        p_out[2] = p_in[2] + Axqp;
        p_out[3] = p_in[3] + Ayqp;
        p_out[4] = p_in[4];

        for (int i = 0; i < 19; ++i) {
            rkd[i] = 0.;
        }
        rkd[0] = 1.;
        rkd[6] = 1.;
        rkd[10] = h;
        rkd[12] = 1.;
        rkd[16] = h;
        rkd[18] = 1.;
        rkd[20] = 0.5 * Ax * h;
        rkd[21] = 0.;
        rkd[22] = Ax;
        rkd[23] = 0.;
        rkd[24] = 1.;

    } else if (fabs(h) < RK_step_min * 3.) // medium distance
    {
        RK4Fast_(z_in, p_in, z_out, p_out, rkd, ierror);
    } else // long distance
    {
        RK5Fast_(z_in, p_in, error, z_out, p_out, rkd, ierror);
    }

} // end of RKTrans

void BmnTrackExtrapolator::RKTrans(FairTrackParam *fairTrackParam, double z_out = 50) {
    int ierror = 0;
    double p_in[5];
    double p_out[5];
    double rkd[25];

    fField = FairRunAna::Instance()->GetField();

    p_in[0] = fairTrackParam->GetX();
    p_in[1] = fairTrackParam->GetY();
    p_in[2] = fairTrackParam->GetTx();
    p_in[3] = fairTrackParam->GetTy();
    p_in[4] = fairTrackParam->GetQp();
    
    // RKTrans_
    RKTrans_(fairTrackParam->GetZ(), p_in, z_out, p_out, rkd, ierror);
    
    fairTrackParam->SetX(p_out[0]);
    fairTrackParam->SetY(p_out[1]);
    fairTrackParam->SetTx(p_out[2]);
    fairTrackParam->SetTy(p_out[3]);
    fairTrackParam->SetQp(p_out[4]);
}

void BmnTrackExtrapolator::RK5Order(FairTrackParam *fairTrackParam, double z_out = 50) {
    int ierror = 0;
    double p_in[5];
    double p_out[5];
    double rkd[25];
    float error[2] = {0.0025, 0.0050}; // IT , OT - errors

    fField = FairRunAna::Instance()->GetField();

    p_in[0] = fairTrackParam->GetX();
    p_in[1] = fairTrackParam->GetY();
    p_in[2] = fairTrackParam->GetTx();
    p_in[3] = fairTrackParam->GetTy();
    p_in[4] = fairTrackParam->GetQp();
    
    // RK5Order
    RK5Order_(fairTrackParam->GetZ(), p_in, error, z_out, p_out, rkd, ierror);
    
    fairTrackParam->SetX(p_out[0]);
    fairTrackParam->SetY(p_out[1]);
    fairTrackParam->SetTx(p_out[2]);
    fairTrackParam->SetTy(p_out[3]);
    fairTrackParam->SetQp(p_out[4]);
}

// Particle trajectory is clipped on small pieces and for each
// of them fifth-order Runge-Kutta method with adaptive stepsize 
// control is used for solution of the equation of motion of a 
// particle with parameter qp = Q /P in the magnetic field B()  
//
//        | x |          tx
//        | y |          ty 
// d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) ) 
//        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
//
//   where  ft = C*qp*sqrt ( 1 + tx**2 + ty**2 ) .
//
//  In the following for RK step  :
//     x=x[0], y=x[1], tx=x[3], ty=x[4].

void BmnTrackExtrapolator::RK5Clip_(
		 
		 double z_in , // z value for input parameters
		 double* p_in,  // input track parameters (x,y,tx,ty,Q/p)
		 double& z_out, // z value for output parameters
		 double* p_out, // output track parameters
		 double* rkd,   // derivatives d p_out[0-4] / d p_in[0-4]
		 int&    ierror)// = 0 ok, = 1 track curls
{
  double z_1 , z_2;
  double z1,z2,p1[5],p2[5],rkd1[25],rkd2[25];
  float error1[2],error[2]={0.00005,0.00005};

  double h,step_1;
  float RK_step_clip;
  int N_step;

//----------------------------------------------------------------

//  cout << "\n Z_in  " << z_in << " Z_out " << z_out << endl;

  h = z_out - z_in;

  float p_0 = 1./fabs(p_in[4]);
  if(p_0 < 3. ) {
    RK_step_clip = 6.;
  }
  else if(p_0 < 10.) {
    RK_step_clip = p_0 * 2.;
  }
  else if(p_0 < 30.) {
    RK_step_clip =   20. + ( p_0 - 10.) * 0.5  ;
  }
  else {
    RK_step_clip = p_0 * 1.;
  }
//  cout << "\n RK_step_clip " << RK_step_clip << endl;
  
  if(fabs(h) > (double) RK_step_clip) 
    {
      N_step = (int) fabs(h/(RK_step_clip / RK_scale)) + 1; 
      step_1 = h / ((float) N_step);
    }
  else    
    {
      N_step = 1;
      step_1 = h; 
    }

  for ( int i=0; i < 2; ++i) 
      {error1[i] = error[i]/sqrt(float( N_step));}

  z1 = z_in; for (int j = 0; j < 5; ++j){p1[j] = p_in[j];}
  for ( int i=0; i < N_step  ; ++i)
    {
      z2 = z_out - ((double)( N_step - i - 1 )) * step_1;

//  cout << "\n z1  " << z1 << " z2 " << z2 << endl;

      RK5Order_(z1,p1,error1,z2,p2,rkd2,ierror);

      if(i == 0) 
	{
	  for(int i1 = 0; i1 < 25; ++i1){rkd[i1] = rkd2[i1];}
	}
      else
	{
	  rkd[0]  = 1.; rkd[1] = 0.; rkd[2] = 0.; rkd[3] = 0.; rkd[4] = 0;
	  rkd[5]  = 0.; rkd[6] = 1.; rkd[7] = 0.; rkd[8] = 0.; rkd[4] = 0;
	  rkd[10] = rkd1[10] + rkd2[10]            + rkd2[15] * rkd1[13];
	  rkd[11] = rkd1[11] + rkd2[11]            + rkd2[16] * rkd1[13];
	  rkd[12] = 1.;
	  rkd[13] =            rkd2[13]            +            rkd1[13];
          rkd[14] = 0.;
	  rkd[15] = rkd1[15] + rkd2[10] * rkd1[17] + rkd2[15];
	  rkd[16] = rkd1[16] + rkd2[11] * rkd1[17] + rkd2[16];
	  rkd[17] =                       rkd1[17] + rkd2[17];
	  rkd[18] = 1.;
	  rkd[19] = 0.;
	  rkd[20] = rkd1[20] + rkd2[10] * rkd1[22] + rkd2[15] * rkd1[23]
	           +rkd2[20];
	  rkd[21] = rkd1[21] + rkd2[11] * rkd1[22] + rkd2[16] * rkd1[23]
	           +rkd2[21];
	  rkd[22] =                       rkd1[22] + rkd2[17] * rkd1[23]
	           +rkd2[22];
	  rkd[23] =            rkd2[13] * rkd1[22] +            rkd1[23]
	           +rkd2[23];
	  rkd[24] = 1.;
	}
      z1 = z2;
      for(int i1 = 0; i1 <  5; ++i1){ p1  [i1] = p2 [i1];}
      for(int i1 = 0; i1 < 25; ++i1){ rkd1[i1] = rkd[i1];}
    }
  for(int i1 = 0; i1 <  5; ++i1){ p_out  [i1] = p2 [i1];}

}    // end of RK5Clip

void BmnTrackExtrapolator::RK5Clip(FairTrackParam *fairTrackParam, double z_out = 50) {
    int ierror = 0;
    double p_in[5];
    double p_out[5];
    double rkd[25];
    float error[2] = {0.0025, 0.0050}; // IT , OT - errors

    fField = FairRunAna::Instance()->GetField();

    p_in[0] = fairTrackParam->GetX();
    p_in[1] = fairTrackParam->GetY();
    p_in[2] = fairTrackParam->GetTx();
    p_in[3] = fairTrackParam->GetTy();
    p_in[4] = fairTrackParam->GetQp();
    
    // RK5Clip
    RK5Clip_(fairTrackParam->GetZ(), p_in, z_out, p_out, rkd, ierror);
    
    fairTrackParam->SetX(p_out[0]);
    fairTrackParam->SetY(p_out[1]);
    fairTrackParam->SetTx(p_out[2]);
    fairTrackParam->SetTy(p_out[3]);
    fairTrackParam->SetQp(p_out[4]);
}

//+++++++++++++++++++++++++++++LINE EXTRAPOLATOR+++++++++++++++++++++++++++++++++++++//

BmnStatus BmnTrackExtrapolator::LineTrackExtrapolate(FairTrackParam* par, Float_t zOut, vector<Double_t>* F) {

    Double_t X[5] = {par->GetX(), par->GetY(), par->GetTx(), par->GetTy(), par->GetQp()};

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
