
#include "BmnKalmanFilter_tmp.h"

using namespace std;
using namespace TMath;

BmnKalmanFilter_tmp::BmnKalmanFilter_tmp() {
}

BmnKalmanFilter_tmp::~BmnKalmanFilter_tmp() {
}

BmnStatus BmnKalmanFilter_tmp::Prediction(FairTrackParam* par, Float_t zOut, BmnFitNode& node) {

    //        cout << "Pred\n";

    if (!IsParCorrect(par)) {
        //        par->Print();
//        cout << "PREDICTION: BAD PARAMETER IN THE BEGINNING" << endl;
        return kBMNERROR;
    }

    TMatrixD F(5, 5); //transport matrix (for linear prorogation now)

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

BmnStatus BmnKalmanFilter_tmp::Correction(FairTrackParam* par, BmnHit* hit, Float_t &chi2, BmnFitNode& node) {


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
    vector<BmnFitNode> fitNodes = GetFitNodes();
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

        Float_t chi2 = ((Xmeas - H * currSmoothX).T() * (R + H * currSmoothC * Ht).Invert() * (Xmeas - H * currSmoothX)).Determinant();
        //        cout << "i = " << i << " | chi2 = " << chi2 << endl;

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

TMatrixD BmnKalmanFilter_tmp::Transport(FairTrackParam* par, Float_t zOut, TString type) {
    TMatrixD F(5, 5); //transport matrix
    F.UnitMatrix();
    Float_t z0 = par->GetZ();
    Float_t tx = par->GetTx();
    Float_t ty = par->GetTy();
    Float_t x0 = par->GetX();
    Float_t y = par->GetY();
    fField = FairRunAna::Instance()->GetField();

    Float_t Bx = fField->GetBx(x0, y, z0);
    Float_t By = fField->GetBy(x0, y, z0);
    Float_t Bz = fField->GetBz(x0, y, z0);

    Float_t s = zOut - z0;
    F[1][3] = s;
    F[0][2] = s;

    Float_t Ax = Sqrt(1 + tx * tx + ty * ty) * (tx * ty * Bx - (1 + tx * tx) * By + ty * Bz);
    Float_t Ay = Sqrt(1 + tx * tx + ty * ty) * ((1 + ty * ty) * Bx - tx * ty * By - tx * Bz);
    const Float_t k = 2.99792458e-4;

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

Bool_t BmnKalmanFilter_tmp::IsParCorrect(const FairTrackParam* par) {
    const Float_t maxSlope = 5.;
    const Float_t minSlope = 1e-10;
    const Float_t maxQp = 1000.; // p = 10 MeV

    if (abs(par->GetTx()) > maxSlope || abs(par->GetTy()) > maxSlope || abs(par->GetTx()) < minSlope || abs(par->GetTy()) < minSlope || abs(par->GetQp()) > maxQp) return kFALSE;
    if (isnan(par->GetX()) || isnan(par->GetY()) || isnan(par->GetTx()) || isnan(par->GetTy()) || isnan(par->GetQp())) return kFALSE;

    return kTRUE;
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