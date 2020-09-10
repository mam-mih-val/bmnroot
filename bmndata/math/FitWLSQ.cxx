#include <iomanip>
#include <iostream>
#include <float.h>
#include <math.h>
#include <assert.h>
#include "FitWLSQ.h"

using namespace std;

FitWLSQ::FitWLSQ(Double_t *xx, Double_t cSigma1, Double_t cSigma2, Double_t cP1, Int_t nNPoints,
                 Int_t nNParams, Bool_t RBF, Bool_t LHf, Int_t RBfn) {
    for (int i = 0; i < 3; i++) {
        param[i] = 0;
        for (int j = 0; j < 3; j++) {
            cov[i][j] = 0;
        }
    }
    RB_TYPE = RBfn;
    if (RB_TYPE == 4 || RB_TYPE == 5) {
        // Approximation-1
        resolution = cSigma1;
    } else {
        // Approximation-2, Tukey weights, optimal weights
        resolution = cSigma2;
    }
    Sigma1 = cSigma1;
    Sigma2 = cSigma2;
    P1 = cP1;
    NParams = nNParams;
    NPoints = nNPoints;
    RB_FIT = RBF;
    LH_INI = LHf;
    numItera = 0;
    // allocate internal dimensions
    w = new Double_t[nNPoints];
    wrb = new Double_t[nNPoints];
    x = new Double_t[nNPoints];
    // fill dimension with z coordinats
    for (int i = 0; i < NPoints; i++) {
        x[i] = xx[i];
        w[i] = 0;
        wrb[i] = 1;
    }
}

FitWLSQ::~FitWLSQ() {
    // delete internal dimensions
    delete[] w;
    delete[] wrb;
    delete[] x;
}

inline Double_t FitWLSQ::det3(Double_t a[3][3]) {
    Double_t val;
    val = a[0][0] * a[1][1] * a[2][2]
          + a[0][1] * a[1][2] * a[2][0]
          + a[0][2] * a[1][0] * a[2][1]
          - a[0][1] * a[1][0] * a[2][2]
          - a[0][0] * a[1][2] * a[2][1]
          - a[0][2] * a[1][1] * a[2][0];
    return val;
}

Double_t FitWLSQ::alg_add(Double_t a[3][3], Int_t row, Int_t column) {
    Int_t i, j, i_r, i_c;
    Double_t alg_value = 0;
    Double_t b[2][2];
    Double_t bb;

    if (NParams == 3) {
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 2; j++) {
                if (i < row)
                    i_r = i;
                else
                    i_r = i + 1;
                if (j < column)
                    i_c = j;
                else
                    i_c = j + 1;

                b[i][j] = a[i_r][i_c];
            }
        }
        alg_value = pow(-1, row + column + 2) * (b[0][0] * b[1][1] - b[0][1] * b[1][0]);
    }
    if (NParams == 2) {
        if (row < 1)
            i_r = row + 1;
        else
            i_r = row - 1;

        if (column < 1)
            i_c = column + 1;
        else
            i_c = column - 1;

        bb = a[i_r][i_c];
        alg_value = pow(-1, row + column + 2) * bb;
    }
    return alg_value;
}

void FitWLSQ::cov_matrix(Double_t a[3][3]) {
    Double_t dfact;
    Int_t i, j;

    // clean all matrixes
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            cov[i][j] = 0;
        }
    }

    if (NParams == 3) {
        dfact = 1 / det3(a);
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                cov[i][j] = dfact * alg_add(a, i, j);
            }
        }
    }
    if (NParams == 2) {
        dfact = 1 / (a[0][0] * a[1][1] - a[0][1] * a[1][0]);
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 2; j++) {
                cov[i][j] = dfact * alg_add(a, i, j);
            }
        }
    }
    return;
}

Double_t FitWLSQ::f_lh(Double_t *yy, Double_t *zz, Double_t *wlh, Double_t *par) {
    Double_t dcoef1, dcoef2, darg, deksp;
    Double_t sum1, sum2, sum3;
    Int_t npoint;
    Int_t i;

    dcoef1 = (Sigma2 * Sigma2 - Sigma1 * Sigma1);
    dcoef1 = dcoef1 / (2. * Sigma1 * Sigma1 * Sigma2 * Sigma2);
    dcoef2 = (1. - P1) * Sigma1 / (P1 * Sigma2);
    sum1 = 0.;
    sum2 = 0.;
    npoint = 0;
    for (i = 0; i < NPoints; i++) {
        if (wlh[i]) {
            if (NParams == 2)
                darg = (par[1] * zz[i] + par[0] - yy[i]) * (par[1] * zz[i] + par[0] - yy[i]);

            if (NParams == 3)
                darg = (par[2] * zz[i] * zz[i] + par[1] * zz[i] + par[0] - yy[i]) *
                       (par[2] * zz[i] * zz[i] + par[1] * zz[i] + par[0] - yy[i]);

            if ((dcoef1 * darg) < 600.) {
                deksp = exp(dcoef1 * darg);
                sum1 = sum1 - darg / (2. * Sigma1 * Sigma1);
                sum2 = sum2 + log(1. + dcoef2 * deksp);
            } else {
                sum1 = sum1 - darg / (2. * Sigma1 * Sigma1);
                sum2 = sum2 + log(dcoef2) + dcoef1 * darg;
            }
        }
        npoint++;
    }
    sum3 = npoint * log(P1 / (Sigma1 * sqrt(2. * 3.141593)));

    return -sum1 - sum2 - sum3;
}

Bool_t FitWLSQ::Initial(Double_t *trk) {
    unsigned char itera = 0;
    Double_t fmin = 999999.;
    Double_t f, rk1, rk2, rk3;
    Double_t axmin[3] = {0};
    Double_t axx[3] = {0};

    // find initial approximation and fill dimesions to call fit iterations
    for (int i = 0; i < NPoints; i++) {
        w[i] = 1;
    }
    if (NPoints < 3) {
        cout << "!!!Wrong number of points(<3) -> exit " << NPoints << endl;
        return kFALSE;
    }

    if (NParams < 2 || NParams > 3) {
        cout << "!!!Wrong number of parameters -> exit" << NParams << endl;
        return kFALSE;
    }

    //              LINE
    if (NParams == 2) {
        for (int i = 1; i < NPoints; i++) {
            for (int j = 0; j < i; j++) {
                if (w[i] && w[j]) {
                    axx[1] = (trk[i] - trk[j]) / (x[i] - x[j]);
                    axx[0] = (x[i] * trk[j] - x[j] * trk[i]) / (x[i] - x[j]);
                    f = f_lh(trk, x, w, axx);
                    if (f < fmin) {
                        fmin = f;
                        for (int k = 0; k < 3; k++) axmin[k] = axx[k];
                    }
                }
            }
        }
    }

    //               PARABOLA
    if (NParams == 3) {
        for (int i = 1; i < NPoints; i++) {
            for (int j = 0; j < i; j++) {
                for (int k = 0; k < j; k++) {
                    if (w[i] && w[j] && w[k]) {
                        rk1 = trk[i] / ((x[i] - x[j]) * (x[i] - x[k]));
                        rk2 = trk[j] / ((x[j] - x[i]) * (x[j] - x[k]));
                        rk3 = trk[k] / ((x[k] - x[i]) * (x[k] - x[j]));
                        axx[2] = rk1 + rk2 + rk3;
                        axx[1] = -rk1 * (x[j] + x[k]) - rk2 * (x[i] + x[k]) - rk3 * (x[i] + x[j]);
                        axx[0] = rk1 * x[j] * x[k] + rk2 * x[i] * x[k] + rk3 * x[i] * x[j];
                        f = f_lh(trk, x, w, axx);
                        if (f < fmin) {
                            fmin = f;
                            for (int n = 0; n < 3; n++) axmin[n] = axx[n];
                        }
                    }
                }
            }
        }
    }
    for (int k = 0; k < 3; k++) param[k] = axmin[k];
    if (!param[0] && !param[1] && !param[2]) {
        cout << "!!!NOT VALID TRACK PARAMETERS!!!!!" << endl;
        return kFALSE;
    }
    return kTRUE;
}

void FitWLSQ::CheckMatrix(Double_t *pdA, Double_t *pdB, TString sz) {
    assert(pdA && pdB && NParams && sz);

    long l = cout.flags(ios::left);
    cout << '\n' << sz;
    cout << "\nMatrix A[n,n]:\n";
    for (Int_t i = 0; i < NParams; i++) {
        for (Int_t j = 0; j < NParams; j++)
            cout << setw(12) << pdA[i + j * NParams];
        cout << '\n';
    }
    cout << "Matrix B[n]:\n";
    for (Int_t i = 0; i < NParams; i++)
        cout << setw(12) << pdB[i];
    if (param) {
        cout << "\nMatrix P[n]:\n";
        for (Int_t i = 0; i < NParams; i++)
            cout << setw(12) << param[i];
    }
    cout << '\n' << flush;
    //cout.flags( l );
}

Bool_t FitWLSQ::SymMatrix(Double_t *pdA, Double_t *pdB) {
    assert(pdA && pdB && param && NParams);
    Int_t n1 = NParams - 1;
    for (Int_t i = 0u; i < n1; i++) {
        Int_t in = i * NParams;
        Double_t dPivot = -pdA[i + in];
        Double_t dB = pdB[i];
        Int_t i1 = i + 1;
        for (Int_t j = i1; j < NParams; j++) {
            Double_t dA = pdA[j + in];
            Int_t jn = j * NParams;
            pdA[jn] = dA / dPivot;
            if (dA == 0.0)
                continue;
            for (Int_t k = i1; k <= j; k++) {
                Int_t kn = k * NParams;
                pdA[j + kn] += dA * pdA[kn];
            }
            pdB[j] += dB * pdA[jn];
        }
    }
    Double_t dA = pdA[NParams * NParams - 1];
    if (dA == 0.0) {
        CheckMatrix(pdA, pdB, "matrix singularity!");
        return kFALSE;
    }
    param[n1] = pdB[n1] /= dA;
    Int_t ii = n1;
    while (ii--) {
        Double_t dB = -pdB[ii];
        Int_t iin = ii * NParams;
        for (Int_t j = ii + 1u; j < NParams; j++)
            dB += pdA[j + iin] * param[j];
        param[ii] = -dB / pdA[ii + iin];
    }
    return kTRUE;
}

Bool_t FitWLSQ::WLSQFit(Double_t *pdY) {
    Double_t left[3][3];
    if (NPoints < 2 || !NParams)
        return kFALSE;
    Int_t nn = NParams * NParams;
    Int_t nnn = nn + NParams;
    Double_t *pdA = new Double_t[nnn];
    assert(pdA);

    Double_t *pdB = pdA + nn;
    for (Int_t i = 0u; i < nnn; i++)
        pdA[i] = 0.0;
    // calculating coefficients of the normal equation system  AxP=B
    for (Int_t n = 0u; n < NPoints; n++) {
        Double_t dX = x[n];
        Double_t dY = pdY[n];
        dY *= wrb[n];
        Double_t dW = 1;

        for (Int_t i = 0u; i < NParams; i++) {
            pdB[i] += dW * dY * pow(dX, i);
            for (Int_t j = 0u; j <= i; j++) {
                pdA[i + j * NParams] += dW * pow(dX, j + i);
            }
        }
    }

    // covariance matrix calculation
    for (Int_t i = 0; i < NParams; i++) {
        for (Int_t j = 0; j < NParams; j++) {
            left[i][j] = pdA[i + j * NParams];
        }
    }

    left[0][1] = left[1][0];
    left[0][2] = left[2][0];
    left[1][2] = left[2][1];

    cov_matrix(left);

    // solve equation
    Bool_t b = SymMatrix(pdA, pdB);
    delete[] pdA;
    return b;
}

Double_t FitWLSQ::WLSQGet(Double_t dX) {
    assert(param && NParams);

    Double_t dY = param[0];
    Double_t d = dX;
    for (Int_t i = 1u; i < NParams; i++) {
        dY += param[i] * d;
        d *= dX;
    }
    return dY;
}

Double_t FitWLSQ::WLSQRms(Double_t *pdY) {
    Double_t dChi2 = 0.0;
    Double_t dSW = 0.0;
    for (Int_t i = 0u; i < NPoints; i++) {
        Double_t dX = x[i];
        Double_t dY = pdY[i];
        Double_t dW = wrb[i];

        dSW += dW;
        dY -= WLSQGet(dX);

        dChi2 += dY * dY * dW;
    }
    return sqrt(dChi2 / dSW);
}

Bool_t FitWLSQ::par_check(Double_t *par, Double_t *par1) {
    Int_t i;
    Double_t val;
    Bool_t value;

    value = kTRUE;
    for (i = 0; i < NParams; i++) {
        val = (par1[i] - par[i]) / par1[i];
        if (val < 0) val *= -1;
        if (val > 0.001) value = kFALSE;
    }

    return value;
}

void FitWLSQ::RB(Double_t *yy, Int_t itera_rb) {
    Int_t i;
    Double_t di;
    Double_t fitval;
    Double_t sum_w = 0;
    Double_t sum_wd = 0;
    Double_t sigma_param;
    Double_t dval;

    //without annealing
    if (RB_TYPE == 2)
        sigma_param = 3;
    // annealing
    if (RB_TYPE == 3) {
        sigma_param = 5 - itera_rb * 0.5;
        if (sigma_param < 3) sigma_param = 3;
    }

    if (!itera_rb)
        sigma_k = resolution;
    if (sigma_k < resolution)
        sigma_k = resolution;

    //  insert here switches
    if (!RB_FIT || (!LH_INI && !itera_rb)) {
        for (i = 0; i < NPoints; i++) {
            wrb[i] = 1;
        }
    }
    if ((LH_INI && RB_FIT) || (!LH_INI && itera_rb && RB_FIT)) {
        for (i = 0; i < NPoints; i++) {
            if (NParams == 2) fitval = param[0] + param[1] * x[i];
            if (NParams == 3) fitval = param[0] + param[1] * x[i] + param[2] * x[i] * x[i];
            di = yy[i] - fitval;
            if (di < 0) di *= -1;

            dval = 1 - di * di / (sigma_param * sigma_param * sigma_k * sigma_k);
            if (di > sigma_param * sigma_k) wrb[i] = 0;
            if (di < sigma_param * sigma_k) wrb[i] = dval * dval;
            sum_w += wrb[i];
            sum_wd += wrb[i] * di * di;
        }
    }

    if ((LH_INI && RB_FIT) || (!LH_INI && itera_rb && RB_FIT))
        sigma_k = sqrt((Double_t) sum_wd / sum_w);

    return;
}

void FitWLSQ::RBN(Double_t *yy, Int_t itera_rb) {
    Int_t i;
    Double_t di;
    Double_t fitval;
    Double_t sum_w = 0;
    Double_t sum_wd = 0;
    Double_t sigma_p2;
    Double_t sigma_p4;
    Double_t sigma_pw;
    Double_t dval;

    // without annealing
    if (RB_TYPE == 4) {
        sigma_p2 = 21;
        sigma_p4 = 3.5;
        sigma_pw = 3;
    }
    //annealing
    if (RB_TYPE == 5) {
        //nastroit' po dannym dlya analiza!!!???
        sigma_p2 = 25 - itera_rb * 0.2;
        sigma_p4 = 5 - itera_rb * 0.2;
        sigma_pw = 4 - itera_rb * 0.2;
        if (sigma_p2 < 21) sigma_p2 = 21;
        if (sigma_p4 < 3.5) sigma_p4 = 3.5;
        if (sigma_pw < 3) sigma_pw = 3;
    }

    if (!itera_rb)
        sigma_k = resolution;
    if (sigma_k < resolution)
        sigma_k = resolution;

    if (!RB_FIT || (!LH_INI && !itera_rb)) {
        for (i = 0; i < NPoints; i++) {
            wrb[i] = 1;
        }
    }
    if ((LH_INI && RB_FIT) || (!LH_INI && itera_rb && RB_FIT)) {
        for (i = 0; i < NPoints; i++) {
            if (NParams == 2) fitval = param[0] + param[1] * x[i];
            if (NParams == 3) fitval = param[0] + param[1] * x[i] + param[2] * x[i] * x[i];
            di = yy[i] - fitval;
            if (di < 0) di *= -1;

            if (di > sigma_p2 * sigma_k) wrb[i] = 0;
            if (di < sigma_p2 * sigma_k && di > sigma_pw * sigma_k) {
                dval = 1 - pow((di / (sigma_p2 * sigma_k)), 2);
                wrb[i] = dval * dval * Sigma1 / (Sigma2 / P1);
            }
            if (di < sigma_pw * sigma_k) {
                dval = 1 - pow((di / (sigma_p4 * sigma_k)), 4);
                wrb[i] = dval * dval;
            }

            sum_w += wrb[i];
            sum_wd += wrb[i] * di * di;
        }
    }

    if ((LH_INI && RB_FIT) || (!LH_INI && itera_rb && RB_FIT))
        sigma_k = sqrt((Double_t) sum_wd / sum_w);

    return;
}

void FitWLSQ::RBM(Double_t *yy, Int_t itera_rb) {
    // new weights definition
    Int_t i;
    Double_t di;
    Double_t fitval;
    Double_t sum_w = 0;
    Double_t sum_wd = 0;
    Double_t sigma_p;
    Double_t dval;

    // without annealing
    if (RB_TYPE == 6)
        sigma_p = 3.3;
    // annealing
    if (RB_TYPE == 7) {
        sigma_p = 5. - itera_rb * 0.2;
        if (sigma_p < 3.3) sigma_p = 3.3;
    }

    if (!itera_rb)
        sigma_k = resolution;
    if (sigma_k < resolution)
        sigma_k = resolution;

    if (!RB_FIT || (!LH_INI && !itera_rb)) {
        for (i = 0; i < NPoints; i++) {
            wrb[i] = 1;
        }
    }
    if ((LH_INI && RB_FIT) || (!LH_INI && itera_rb && RB_FIT)) {
        for (i = 0; i < NPoints; i++) {
            if (NParams == 2) fitval = param[0] + param[1] * x[i];
            if (NParams == 3) fitval = param[0] + param[1] * x[i] + param[2] * x[i] * x[i];
            di = yy[i] - fitval;
            if (di < 0) di *= -1;

            if (di > sigma_p * sigma_k) wrb[i] = 0;
            if (di < sigma_p * Sigma2 && di > sigma_p * Sigma1) {
                dval = 1 - pow((di / (sigma_p * sigma_k)), 2);
                wrb[i] = dval * dval * Sigma1 / (Sigma2 / P1);
            }
            if (di < sigma_p * Sigma1) {
                dval = 1 - pow((di / (sigma_p * sigma_k)), 8);
                wrb[i] = dval * dval;
            }

            sum_w += wrb[i];
            sum_wd += wrb[i] * di * di;
        }
    }

    if ((LH_INI && RB_FIT) || (!LH_INI && itera_rb && RB_FIT))
        sigma_k = sqrt((Double_t) sum_wd / sum_w);

    return;
}

void FitWLSQ::RB_OPT(Double_t *yy, Int_t itera_rb) {
    Int_t i;
    Double_t di;
    Double_t fitval;
    Double_t sum_w = 0;
    Double_t sum_wd = 0;
    Double_t sigma_param;
    Double_t exp1, exp2, a, b, c, d;

    //annealing
    // without annealing
    if (RB_TYPE == 0)
        sigma_param = 3;
    if (RB_TYPE == 1) {
        sigma_param = 5 - itera_rb;
        if (sigma_param < 3) sigma_param = 3;
    }

    if (!itera_rb)
        sigma_k = resolution;
    if (sigma_k < resolution)
        sigma_k = resolution;

    //  insert here switches
    if (!RB_FIT || (!LH_INI && !itera_rb)) {
        for (i = 0; i < NPoints; i++) {
            wrb[i] = 1;
        }
    }
    if ((LH_INI && RB_FIT) || (!LH_INI && itera_rb && RB_FIT)) {
        for (i = 0; i < NPoints; i++) {
            if (NParams == 2) fitval = param[0] + param[1] * x[i];
            if (NParams == 3) fitval = param[0] + param[1] * x[i] + param[2] * x[i] * x[i];
            di = yy[i] - fitval;
            if (di < 0) di *= -1;

            if (di > sigma_param * sigma_k) wrb[i] = 0;
            if (di < sigma_param * sigma_k) {
                a = P1 * Sigma2 * Sigma2 * Sigma2;
                b = (1 - P1) * Sigma1 * Sigma1 * Sigma1;
                c = P1 * Sigma2;
                d = (1 - P1) * Sigma1;
                exp1 = exp((-1. * di * di) / (2. * Sigma1 * Sigma1));
                exp2 = exp((-1. * di * di) / (2. * Sigma2 * Sigma2));
                wrb[i] = ((a * exp1 + b * exp2) * (c + d)) / ((c * exp1 + d * exp2) * (a + b));
                if (exp1 < 0.0001) {
                    wrb[i] = 0.01 - fabs(di * di * di);
                }
            }
            sum_w += wrb[i];
            sum_wd += wrb[i] * di * di;
        }
    }

    if ((LH_INI && RB_FIT) || (!LH_INI && itera_rb && RB_FIT))
        sigma_k = sqrt((Double_t) sum_wd / sum_w);

    return;
}

Bool_t FitWLSQ::Fit(Double_t *y) {
    Int_t itera;
    Bool_t end_fit = kFALSE;
    Bool_t stat;
    Double_t par_c[3] = {0};
    Double_t sumwrb;
    Double_t *yyf;

	Status=false;
    yyf = new Double_t[NPoints];
    if (LH_INI) {
        stat = Initial(y);
        if (!stat)
            return kFALSE;
    }
    sigma_k = resolution;
    itera = 0;
    while (!end_fit) {
        for (int i = 0; i < NParams; i++) par_c[i] = param[i];
        if (RB_TYPE == 0 || RB_TYPE == 1)
            RB_OPT(y, itera);
        if (RB_TYPE == 2 || RB_TYPE == 3)
            RB(y, itera);
        if (RB_TYPE == 4 || RB_TYPE == 5)
            RBN(y, itera);
        if (RB_TYPE == 6 || RB_TYPE == 7)
            RBM(y, itera);

        // check number of points with non zero weights
        //=============================================
        sumwrb = 0;
        for (int i = 0; i < NPoints; i++) {
            if (wrb[i]) sumwrb++;
        }
        if (sumwrb < 4) {
            //cout << "Problems with rb fit number of points with non zero weight: " << sumwrb << " " << itera << endl;
            return kFALSE;
        }
        //=============================================

        for (int i = 0; i < NPoints; i++) {
            if (NParams == 3)
                yyf[i] = y[i] - param[0] - param[1] * x[i] - param[2] * x[i] * x[i];
            else
                yyf[i] = y[i] - param[0] - param[1] * x[i];
        }
        stat = WLSQFit(yyf);

        for (int i = 0; i < NParams; i++)
            param[i] += par_c[i];
        end_fit = par_check(param, par_c);
        if (!stat) return kFALSE;
        itera++;
        if (itera > 50) {
            //cout << " To many robust fit iterations -> exit" << endl;
            return kFALSE;
        }
    }
    numItera = itera;
    //CovarianceMatrixPrint();
    delete[] yyf;
	Status=kTRUE;
    return kTRUE;
}

void FitWLSQ::CovarianceMatrixPrint(void) {
    Double_t rmsval;
    cout << "WLSQ Covariance Matrix" << endl;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cout << " " << cov[i][j];
        }
        cout << endl;
    }
    return;
}
//ClassImp(FitWLSQ);
