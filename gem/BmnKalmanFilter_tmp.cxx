
#include "BmnKalmanFilter_tmp.h"

using namespace std;
using namespace TMath;

BmnKalmanFilter_tmp::BmnKalmanFilter_tmp() {
    fMaterial = new BmnMaterialEffects();
    fNavigator = new BmnGeoNavigator();
}

BmnKalmanFilter_tmp::~BmnKalmanFilter_tmp() {
}

BmnStatus BmnKalmanFilter_tmp::Prediction(FairTrackParam* par, Double_t zOut, BmnFitNode& node) {

    //        cout << "Pred\n";

    if (!IsParCorrect(par)) {
        //        par->Print();
        //        cout << "PREDICTION: BAD PARAMETER IN THE BEGINNING" << endl;
        return kBMNERROR;
    }

    TMatrixD F(5, 5); //transport matrix

    //    F = Transport(par, zOut, "line");
    //            F = Transport(par, zOut, "circle");
    F = Transport(par, zOut, "pol2");

    TMatrixD Ft(5, 5); // just transposition of F
    Ft.Transpose(F);

    TMatrixD X(5, 1); //state vector
    X = FillVecFromPar(par);
    //        X.Print();

    TMatrixD Xpred(5, 1); //predicted state vector
    Xpred = F * X;
    //    cout << "Xpred:";
    //    Xpred.Print();

    TMatrixD C(5, 5); //covariance matrix
    C = FillCovFromPar(par);
    //    C.Print();

    TMatrixD Cpred(5, 5); //predicted covariance matrix
    Cpred = F * C * Ft; //     Ccorr = F * C * F.T() + Q !!!
    //    cout << "Cpred:";
    //    Cpred.Print();

    //write predicted state vector and covariance matrix into track parameter
    FillParFromVecAndCov(Xpred, Cpred, par);
    par->SetZ(zOut);

    node.SetPredictedParam(par);
    if (!IsParCorrect(par)) {
        //        cout << "PREDICTION: BAD PARAMETER AT THE END" << endl;
        return kBMNERROR;
    }
    node.SetF_matr(F);
    return kBMNSUCCESS;
}

BmnStatus BmnKalmanFilter_tmp::Correction(FairTrackParam* par, BmnHit* hit, Double_t &chi2, BmnFitNode& node) {


    if (!IsParCorrect(par)) {
        //        cout << "CORRECTION: BAD PARAMETER IN THE BEGINNING" << endl;
        return kBMNERROR;
    }
    //        cout << "Corr\n";
    TMatrixD H(2, 5); //measurement matrix (only for X and Y)
    H.Zero();
    H[0][0] = 1.0;
    H[1][1] = 1.0;

    TMatrixD Ht(5, 2); //just transposition of H
    Ht.Transpose(H);

    TMatrixD Cpred(5, 5); //predicted covariance matrix
    Cpred = FillCovFromPar(par);

    TMatrixD Xpred(5, 1); //predicted state vector
    Xpred = FillVecFromPar(par);

    TMatrixD R(2, 2); //covariance matrix of measurement errors
    R.Zero();
    R[0][0] = hit->GetDx() * hit->GetDx();
    R[1][1] = hit->GetDy() * hit->GetDy();

    TMatrixD K(5, 2); //Kalman gain matrix
    K = Cpred * Ht * (H * Cpred * Ht + R).Invert();

    //        (H * Cpred * Ht + R).Print();
    //    ((H * Cpred * Ht + R).Invert()).Print();
    //    (Ht * (H * Cpred * Ht + R).Invert()).Print();
    //    K.Print();

    //    cout << "x = " << hit->GetX() << " y = " << hit->GetY() << " z = " << hit->GetZ() << endl;

    TMatrixD Xmeas(2, 1); //measured values on current step
    Xmeas[0][0] = hit->GetX();
    Xmeas[1][0] = hit->GetY();

    TMatrixD Xcorr(5, 1); //corrected state vector
    Xcorr = Xpred + K * (Xmeas - H * Xpred);
    //    cout << "Xcorr:";
    //    Xcorr.Print();

    TMatrixD I(5, 5); //unit matrix
    I.UnitMatrix();

    TMatrixD Ccorr(5, 5); //corrected covariance matrix
    Ccorr = (I - K * H) * Cpred;
    //    cout << "Ccorr:";
    //    (I - K * H).Print();
    //    Ccorr.Print();
    //
    //write corrected state vector and covariance matrix into track parameter 
    FillParFromVecAndCov(Xcorr, Ccorr, par);
    par->SetZ(hit->GetZ());

    chi2 = ((Xmeas - H * Xcorr).T() * (R + H * Ccorr * Ht).Invert() * (Xmeas - H * Xcorr)).Determinant();

    node.SetUpdatedParam(par);

    if (!IsParCorrect(par)) {
        //        cout << "CORRECTION: BAD PARANETERS AT THE END" << endl;
        return kBMNERROR;
    }

    node.SetChiSqFiltered(chi2);

    return kBMNSUCCESS;
}

FairTrackParam BmnKalmanFilter_tmp::Filtration(BmnGemTrack* tr, TClonesArray* hits) {

    //    cout << "Smoothing started\n";
    vector<BmnFitNode> fitNodes = tr->GetFitNodes();
    Int_t nNodes = fitNodes.size();

    //    cout << "nodes = " << nNodes << " | hits = " << tr->GetNHits() << endl;

    FairTrackParam outputPar;

    for (Int_t i = nNodes - 2; i >= 0; --i) {
        //        cout << "i = " << i << endl;
        BmnFitNode* prevNode = &(fitNodes.at(i + 1));
        BmnFitNode* currNode = &(fitNodes.at(i));

        BmnHit* hit = (BmnHit*) hits->At(tr->GetHitIndex(i));
        //        cout << "x = " << hit->GetX() << " y = " << hit->GetY() << " z = " << hit->GetZ() << endl;
        const FairTrackParam* prevPredPar = prevNode->GetPredictedParam();
        const FairTrackParam* prevCorrPar = prevNode->GetUpdatedParam();
        const FairTrackParam* prevSmoothPar = (i == nNodes - 2) ? prevNode->GetUpdatedParam() : prevNode->GetSmoothedParam();
        const FairTrackParam* currPredPar = currNode->GetPredictedParam();
        const FairTrackParam* currCorrPar = currNode->GetUpdatedParam();
        FairTrackParam* currSmoothPar = new FairTrackParam();

        if (!IsParCorrect(prevPredPar)) continue;
        if (!IsParCorrect(prevCorrPar)) continue;
        if (!IsParCorrect(prevSmoothPar)) continue;
        if (!IsParCorrect(currCorrPar)) continue;

        TMatrixD prevPredX(5, 1); //predicted state vector on previous node
        prevPredX = FillVecFromPar(prevPredPar);
        //                        cout << "prevPredX:\n";
        //                        prevPredX.Print();

        TMatrixD prevCorrX(5, 1); //predicted state vector on previous node
        prevCorrX = FillVecFromPar(prevCorrPar);
        //                        cout << "prevCorrX:\n";
        //                        prevCorrX.Print();

        TMatrixD prevSmoothX(5, 1); //smoothed state vector on previous node
        prevSmoothX = FillVecFromPar(prevSmoothPar);
        //                        cout << "prevSmoothX:\n";
        //                        prevSmoothX.Print();

        TMatrixD currCorrX(5, 1); //corrected state vector on current node
        currCorrX = FillVecFromPar(currCorrPar);

        TMatrixD currCorrC(5, 5); //corrected covariance matrix on current node
        currCorrC = FillCovFromPar(currCorrPar);
        //        currCorrC.Print();

        TMatrixD prevSmoothC(5, 5); //smoothed covariance matrix on previous node
        prevSmoothC = FillCovFromPar(prevSmoothPar);
        //        prevSmoothC.Print();

        TMatrixD prevCorrC(5, 5); //predicted covariance matrix on previous node
        prevCorrC = FillCovFromPar(prevCorrPar);

        TMatrixD prevPredC(5, 5); //predicted covariance matrix on previous node
        prevPredC = FillCovFromPar(prevPredPar);

        TMatrixD prevPredCInv(5, 5); //inversion of predicted covariance matrix on previous node
        prevPredCInv = FillCovFromPar(prevPredPar);
        for (Int_t i = 0; i < 5; ++i) {
            if (prevPredCInv[i][i] < 1e-13) prevPredCInv[i][i] = 1e-8;
        }
        prevPredCInv.Invert();

        //        TMatrixD F = prevNode->GetF_matr(); // or currNode??
        TMatrixD F = currNode->GetF_matr();
        F.T();

        TMatrixD A(5, 5); //Smoother gain matrix
        A = currCorrC * F * prevPredCInv;
        TMatrixD At(5, 5); //transposition of smoother gain matrix
        At.Transpose(A);

        TMatrixD currSmoothX(5, 1); //smoothed state vector on current node
        currSmoothX = currCorrX + A * (prevSmoothX - prevPredX);

        //        currCorrX.Print();
        //        currSmoothX.Print();

        TMatrixD currSmoothC(5, 5); //smoothed covariance matrix on current node
        currSmoothC = currCorrC + A * (prevSmoothC - prevPredC) * At;
        //        currCorrC.Print();
        //        A.Print();
        //        prevSmoothC.Print();
        //        prevPredC.Print();
        //        At.Print();

        TMatrixD H(2, 5); //measurement matrix (only for X and Y)
        H.Zero();
        H[0][0] = 1.0;
        H[1][1] = 1.0;

        TMatrixD Ht(5, 2); //just transposition of H
        Ht.Transpose(H);

        TMatrixD Xmeas(2, 1); //measured values on current step
        Xmeas[0][0] = hit->GetX();
        Xmeas[1][0] = hit->GetY();

        TMatrixD R(2, 2); //covariance matrix of measurement errors
        R.Zero();
        R[0][0] = hit->GetDx() * hit->GetDx();
        R[1][1] = hit->GetDy() * hit->GetDy();

        Double_t chi2 = ((Xmeas - H * currSmoothX).T() * (R + H * currSmoothC * Ht).Invert() * (Xmeas - H * currSmoothX)).Determinant();
        //                cout << "i = " << i << " | chi2 = " << chi2 << endl;

        //write smoothed state vector and covariance matrix into track parameter 
        FillParFromVecAndCov(currSmoothX, currSmoothC, currSmoothPar);
        currSmoothPar->SetZ(hit->GetZ());

        currNode->SetSmoothedParam(currSmoothPar);
        currNode->SetChiSqSmoothed(chi2);

        if (i == 0) outputPar = *currSmoothPar;
    }

    //    cout << "Smoothing finished\n";
    return outputPar;

}

TMatrixD BmnKalmanFilter_tmp::Transport(FairTrackParam* par, Double_t zOut, TString type) {
    TMatrixD F(5, 5); //transport matrix
    F.UnitMatrix();
    Double_t z0 = par->GetZ();
    Double_t tx = par->GetTx();
    Double_t ty = par->GetTy();
    Double_t x0 = par->GetX();
    Double_t y = par->GetY();
    fField = FairRunAna::Instance()->GetField();

    Double_t Bx = fField->GetBx(x0, y, z0);
    Double_t By = fField->GetBy(x0, y, z0);
    Double_t Bz = fField->GetBz(x0, y, z0);

    Double_t s = zOut - z0;
    F[1][3] = s;
    F[0][2] = s;

    Double_t Ax = Sqrt(1 + tx * tx + ty * ty) * (tx * ty * Bx - (1 + tx * tx) * By + ty * Bz);
    Double_t Ay = Sqrt(1 + tx * tx + ty * ty) * ((1 + ty * ty) * Bx - tx * ty * By - tx * Bz);
    const Double_t k = 2.99792458e-4;

    if (type.Contains("line")) {

    } else if (type.Contains("pol2")) {
        F[0][4] = k * Ax * s * s / 2.0;
        F[1][4] = k * Ay * s * s / 2.0;
        F[2][4] = k * Ax * s;
        F[3][4] = k * Ay * s;
    }

    return F;
}

BmnStatus BmnKalmanFilter_tmp::AddFitNode(BmnFitNode node) {
    fFitNodes.push_back(node);

    return kBMNSUCCESS;
}

TMatrixD BmnKalmanFilter_tmp::FillVecFromPar(const FairTrackParam* par) {
    TMatrixD out(5, 1);
    out[0][0] = par->GetX();
    out[1][0] = par->GetY();
    out[2][0] = par->GetTx();
    out[3][0] = par->GetTy();
    out[4][0] = par->GetQp();
    return out;
}

TMatrixD BmnKalmanFilter_tmp::FillCovFromPar(const FairTrackParam* par) {
    TMatrixD out(5, 5);
    for (Int_t i = 0; i < 5; ++i)
        for (Int_t j = 0; j < 5; ++j)
            out[i][j] = par->GetCovariance(i, j);
    return out;
}

BmnStatus BmnKalmanFilter_tmp::FillParFromVecAndCov(TMatrixD x, TMatrixD c, FairTrackParam* par) {
    par->SetX(x[0][0]);
    par->SetY(x[1][0]);
    par->SetTx(x[2][0]);
    par->SetTy(x[3][0]);
    par->SetQp(x[4][0]);
    for (Int_t i = 0; i < 5; ++i)
        for (Int_t j = 0; j < 5; ++j)
            par->SetCovariance(i, j, c[i][j]);
    return kBMNSUCCESS;
}

BmnStatus BmnKalmanFilter_tmp::RK4TrackExtrapolate(FairTrackParam* par, Double_t zOut, vector<Double_t>* F) {

    Double_t zIn = par->GetZ();
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

void BmnKalmanFilter_tmp::RK4Order(const vector<Double_t>& xIn, Double_t zIn, vector<Double_t>& xOut, Double_t zOut, vector<Double_t>& derivs) {
    const Double_t fC = 0.000299792458;

    Double_t coef[4] = {0.0, 0.5, 0.5, 1.0};

    Double_t Ax[4], Ay[4];
    Double_t dAx_dtx[4], dAy_dtx[4], dAx_dty[4], dAy_dty[4];
    Double_t k[4][4];

    Double_t h = zOut - zIn;
    Double_t hC = h * fC;
    Double_t hCqp = h * fC * xIn[4];
    Double_t x0[4];

    Double_t x[4] = {xIn[0], xIn[1], xIn[2], xIn[3]};

    for (UInt_t iStep = 0; iStep < 4; iStep++) { // 1
        if (iStep > 0) {
            for (UInt_t i = 0; i < 4; i++) {
                x[i] = xIn[i] + coef[iStep] * k[i][iStep - 1];
            }
        }

        Double_t Bx = fField->GetBx(x[0], x[1], zIn + coef[iStep] * h);
        Double_t By = fField->GetBy(x[0], x[1], zIn + coef[iStep] * h);
        Double_t Bz = fField->GetBz(x[0], x[1], zIn + coef[iStep] * h);

        Double_t tx = x[2];
        Double_t ty = x[3];
        Double_t txtx = tx * tx;
        Double_t tyty = ty * ty;
        Double_t txty = tx * ty;
        Double_t txtxtyty1 = 1.0 + txtx + tyty;
        Double_t t1 = Sqrt(txtxtyty1);
        Double_t t2 = 1.0 / txtxtyty1;

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

void BmnKalmanFilter_tmp::TransportC(const vector<Double_t>& cIn, const vector<Double_t>& F, vector<Double_t>& cOut) {
    // F*C*Ft
    Double_t A = cIn[2] + F[2] * cIn[9] + F[3] * cIn[10] + F[4] * cIn[11];
    Double_t B = cIn[3] + F[2] * cIn[10] + F[3] * cIn[12] + F[4] * cIn[13];
    Double_t C = cIn[4] + F[2] * cIn[11] + F[3] * cIn[13] + F[4] * cIn[14];

    Double_t D = cIn[6] + F[7] * cIn[9] + F[8] * cIn[10] + F[9] * cIn[11];
    Double_t E = cIn[7] + F[7] * cIn[10] + F[8] * cIn[12] + F[9] * cIn[13];
    Double_t G = cIn[8] + F[7] * cIn[11] + F[8] * cIn[13] + F[9] * cIn[14];

    Double_t H = cIn[9] + F[13] * cIn[10] + F[14] * cIn[11];
    Double_t I = cIn[10] + F[13] * cIn[12] + F[14] * cIn[13];
    Double_t J = cIn[11] + F[13] * cIn[13] + F[14] * cIn[14];

    Double_t K = cIn[13] + F[17] * cIn[11] + F[19] * cIn[14];

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

Double_t BmnKalmanFilter_tmp::CalcOut(Double_t in, const Double_t k[4]) {
    return in + k[0] / 6. + k[1] / 3. + k[2] / 3. + k[3] / 6.;
}

BmnStatus BmnKalmanFilter_tmp::Update(FairTrackParam* par, const BmnHit* hit, Double_t& chiSq) {

    //   vector<Double_t> cIn = par->GetCovMatrix();
    Double_t cIn[15];
    par->CovMatrix(cIn);

    static const Double_t ONE = 1., TWO = 2.;

    Double_t dxx = hit->GetDx() * hit->GetDx();
    Double_t dxy = 0.0;
    Double_t dyy = hit->GetDy() * hit->GetDy();

    // calculate residuals
    Double_t dx = hit->GetX() - par->GetX();
    Double_t dy = hit->GetY() - par->GetY();

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
    for (Int_t i = 0; i < 15; ++i) {
        cOut[i] = cIn[i];
    }


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

void BmnKalmanFilter_tmp::UpdateF(vector<Double_t>& F, const vector<Double_t>& newF) {
    vector<Double_t> A(25);
    Mult25(newF, F, A);
    F.assign(A.begin(), A.end());
}



//++++++++++++++++++++++++++++SMOOTHER++++++++++++++++++++++++++++++++++++//

BmnStatus BmnKalmanFilter_tmp::FitSmooth(BmnGemTrack* track, TClonesArray* hits) { //FIXME

    const Int_t n = track->GetNHits();

    vector<BmnFitNode> nodes = track->GetFitNodes();
    nodes[n - 1].SetSmoothedParam(nodes[n - 1].GetUpdatedParam());

    for (int i = n - 1; i > 0; i--) {
//        cout << "i = " << i << endl;
//        nodes[i].GetSmoothedParam()->Print();
        if (Smooth(&nodes[i - 1], &nodes[i]) == kBMNERROR) return kBMNERROR;
//        nodes[i].GetSmoothedParam()->Print();
    }

    track->SetChi2(0.);
    for (int i = 0; i < n; i++) {
        BmnGemHit* hit = (BmnGemHit*) hits->At(track->GetHitIndex(i));
        Double_t chi2Hit = lit::ChiSq(nodes[i].GetSmoothedParam(), hit);
        nodes[i].SetChiSqSmoothed(chi2Hit);
        track->SetChi2(track->GetChi2() + chi2Hit);
    }

    FairTrackParam* parFirst = nodes[0].GetSmoothedParam();
    track->SetParamFirst(*parFirst);
    track->SetFitNodes(nodes);
    track->SetNDF(lit::NDF(track));
    
//    parFirst->Print();
//    cout << "chi2 = " << track->GetChi2() << " | ndf = " << track->GetNDF() << " | nHits = " << track->GetNHits() << endl;
//    cout << "chi2 / NDF = " << track->GetChi2() / track->GetNDF() << endl;

    return kBMNSUCCESS;
}

// We are going in the upstream direction
// this Node (k) , prevNode (k+1)

BmnStatus BmnKalmanFilter_tmp::Smooth(BmnFitNode* thisNode, BmnFitNode* prevNode) {

    Double_t cov1[15];
    prevNode->GetPredictedParam()->CovMatrix(cov1);
    vector<Double_t> invPrevPredC;
    for (Int_t i = 0; i < 15; ++i) {
        invPrevPredC.push_back(cov1[i]);
    }
    if (!InvSym15(invPrevPredC)) return kBMNERROR;

    vector<Double_t> Ft(prevNode->GetF());
    Transpose25(Ft);

    Double_t cov2[15];
    thisNode->GetUpdatedParam()->CovMatrix(cov2);
    vector<Double_t> thisUpdC;
    for (Int_t i = 0; i < 15; ++i) {
        thisUpdC.push_back(cov2[i]);
    }

    vector<Double_t> A(25);
    vector<Double_t> temp1(25);
    Mult15On25(thisUpdC, Ft, temp1);
    Mult25On15(temp1, invPrevPredC, A);
       

    vector<Double_t> thisUpdX;
    thisUpdX.push_back(thisNode->GetUpdatedParam()->GetX());
    thisUpdX.push_back(thisNode->GetUpdatedParam()->GetY());
    thisUpdX.push_back(thisNode->GetUpdatedParam()->GetTx());
    thisUpdX.push_back(thisNode->GetUpdatedParam()->GetTy());
    thisUpdX.push_back(thisNode->GetUpdatedParam()->GetQp());

    vector<Double_t> prevSmoothedX;
    prevSmoothedX.push_back(prevNode->GetSmoothedParam()->GetX());
    prevSmoothedX.push_back(prevNode->GetSmoothedParam()->GetY());
    prevSmoothedX.push_back(prevNode->GetSmoothedParam()->GetTx());
    prevSmoothedX.push_back(prevNode->GetSmoothedParam()->GetTy());
    prevSmoothedX.push_back(prevNode->GetSmoothedParam()->GetQp());

    vector<Double_t> prevPredX;
    prevPredX.push_back(prevNode->GetPredictedParam()->GetX());
    prevPredX.push_back(prevNode->GetPredictedParam()->GetY());
    prevPredX.push_back(prevNode->GetPredictedParam()->GetTx());
    prevPredX.push_back(prevNode->GetPredictedParam()->GetTy());
    prevPredX.push_back(prevNode->GetPredictedParam()->GetQp());

    vector<Double_t> temp2(5), temp3(5);
    Subtract(prevSmoothedX, prevPredX, temp2);
    Mult25On5(A, temp2, temp3);
    vector<Double_t> thisSmoothedX(5);
    Add(thisUpdX, temp3, thisSmoothedX);

    Double_t cov3[15];
    prevNode->GetSmoothedParam()->CovMatrix(cov3);
    vector<Double_t> prevSmoothedC;
    for (Int_t i = 0; i < 15; ++i) {
        prevSmoothedC.push_back(cov3[i]);
    }

    Double_t cov4[15];
    prevNode->GetPredictedParam()->CovMatrix(cov4);
    vector<Double_t> prevPredC;
    for (Int_t i = 0; i < 15; ++i) {
        prevPredC.push_back(cov4[i]);
    }

    vector<Double_t> temp4(15);
    Subtract(prevSmoothedC, prevPredC, temp4);

    vector<Double_t> temp5(15);
    Similarity(A, temp4, temp5);
    vector<Double_t> thisSmoothedC(15);
    Add(thisUpdC, temp5, thisSmoothedC);

    FairTrackParam par;

    par.SetX(thisSmoothedX[0]);
    par.SetY(thisSmoothedX[1]);
    par.SetTx(thisSmoothedX[2]);
    par.SetTy(thisSmoothedX[3]);
    par.SetQp(thisSmoothedX[4]);

    Double_t cov5[15];
    for (Int_t i = 0; i < 15; ++i) {
        cov5[i] = thisSmoothedC[i];
    }
    par.SetCovMatrix(cov5);
    par.SetZ(thisNode->GetUpdatedParam()->GetZ());

    thisNode->SetSmoothedParam(&par);
    
    return kBMNSUCCESS;
}

BmnStatus BmnKalmanFilter_tmp::TGeoTrackPropagate(FairTrackParam* par, Double_t zOut, Int_t pdg, vector<Double_t>* F, Double_t* length, TString type) {

    if (!IsParCorrect(par)) return kBMNERROR;
    Double_t zIn = par->GetZ();
    Double_t dz = zOut - zIn;

    if (fabs(dz) < 1e-6) {
        return kBMNSUCCESS;
    }
    // Check whether upstream or downstream
    // TODO check upstream/downstream
    Bool_t downstream = dz > 0;

    if (F != NULL) {
        F->assign(25, 0.);
        (*F)[0] = 1.;
        (*F)[6] = 1.;
        (*F)[12] = 1.;
        (*F)[18] = 1.;
        (*F)[24] = 1.;
    }

    Int_t nofSteps = Int_t(abs(dz) / 10);
    Double_t stepSize;
    if (nofSteps == 0) {
        stepSize = abs(dz);
    } else {
        stepSize = 10;
    }
    Double_t z = zIn;

//    cout << "Z = " << zIn << " Par q/p = " << par->GetQp() << endl;
    //if (length) *length = 0;
    // Loop over steps + additional step to propagate to virtual plane at zOut
    for (Int_t iStep = 0; iStep < nofSteps + 1; iStep++) {                    //FIXME possible problems with geometry...
        if (!IsParCorrect(par)) return kBMNERROR;
        // Check if already at exit position
        if (z == zOut) break;
        // Update current z position
        if (iStep != nofSteps) z = (downstream) ? z + stepSize : z - stepSize;
        else z = zOut;

        // Get intersections with materials for this step
        vector<BmnMaterialInfo> inter;

//        cout << "IN PROPAGATION \t Xt = " << par->GetX() << "\tYt = " << par->GetY() << "\tZt = " << par->GetZ() << endl;
        
        if (fNavigator->FindIntersections(par, z, inter) == kBMNERROR) return kBMNERROR;
        
        // Loop over material layers
//        cout << "inter.size = " << inter.size() << endl;
        
        for (UInt_t iMat = 0; iMat < inter.size(); iMat++) {
            BmnMaterialInfo mat = inter[iMat];
//            cout << mat.ToString();
            // Check if track parameters are correct
            if (!IsParCorrect(par)) return kBMNERROR;
            vector<Double_t>* Fnew = NULL;
            if (F != NULL) Fnew = new vector<Double_t > (25, 0.);
            
            // Extrapolate to the next boundary
            if (type == "field") {                                              //FIXME type of extrapolating has to be depended on a magnetic field presence
                if (RK4TrackExtrapolate(par, mat.GetZpos(), Fnew) == kBMNERROR) { //Is it possible to return error from RK4 extrapolator???
                    return kBMNERROR; 
                }
            } else {
                return kBMNERROR;
            }
            
            // Update transport matrix
            if (F != NULL) UpdateF(*F, *Fnew); 
            if (Fnew != NULL) delete Fnew;

            // Add material effects
//            cout << "BEFORE:  " << endl;
//            par->Print();
            fMaterial->Update(par, &mat, pdg, downstream);
//            cout << "AFTER :  " << endl;
//            par->Print();
//            cout << "Step = " << iStep << " iMat = " << iMat << " Z = " << zIn << " Par q/p = " << par->GetQp() << endl;
            if (length) *length += mat.GetLength();
        }
    }
    
    if (!IsParCorrect(par)) {
        return kBMNERROR;
    } else {
        return kBMNSUCCESS;
    }
}