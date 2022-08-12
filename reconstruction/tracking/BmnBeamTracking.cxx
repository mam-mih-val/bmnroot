#include "BmnBeamTracking.h"
#include <TMath.h>
#include <map>
#include <vector>
#include "BmnSiBTHit.h"
#include "BmnKalmanFilter.h"
#include "BmnMath.h"
#include "FairRunAna.h"
#include "FairTrackParam.h"
#include "TStyle.h"
#include "TFile.h"
#include "TH1F.h"

using namespace std;
using namespace TMath;

TH1F* hdx = new TH1F("hdx", "hdx", 4000, -1, 1);
TH1F* hdy = new TH1F("hdy", "hdy", 4000, -1, 1);
TH1F* hdtx = new TH1F("hdtx", "hdtx", 4000, -0.01, 0.01);
TH1F* hdty = new TH1F("hdty", "hdty", 4000, -0.01, 0.01);

BmnBeamTracking::BmnBeamTracking(Int_t run) {

    fEventNo = 0;
    fSiBTHitsArray = nullptr;
    fBeamTracksArray = nullptr;
    fField = nullptr;
    fSiBTHitsBranchName = "BmnSiBTHit";
    fBeamTracksBranchName = "BmnBeamTrack";
    fSiBTDetector = nullptr;
}

BmnBeamTracking::~BmnBeamTracking() {
    delete fKalman;
    delete fSiBTDetector;
}

InitStatus BmnBeamTracking::Init() {
    if (fVerbose > 1)
        cout << "======================== BEAM tracking init started ====================" << endl;

    fKalman = new BmnKalmanFilter();

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        Fatal("Init", "FairRootManager is not instantiated");

    //SILICON
    fSiBTHitsArray = (TClonesArray*)ioman->GetObject(fSiBTHitsBranchName); //in

    if (!fSiBTHitsArray) {
        cout << "BmnBeamTracking::Init(): branch " << fSiBTHitsBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return InitStatus::kERROR;
    }

    fBeamTracksArray = new TClonesArray("BmnTrack", 100); //out
    ioman->Register(fBeamTracksBranchName, "BEAM", fBeamTracksArray, kTRUE);

    fField = FairRunAna::Instance()->GetField();
    if (!fField)
        Fatal("Init", "No Magnetic Field found");

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");

    TString gPathGemConfig = gPathConfig + "/parameters/SiBT/XMLConfigs/";
    TString confSiBT = "SiBTRun8.xml";
    fSiBTDetector = new BmnSiBTStationSet(gPathGemConfig + confSiBT);

    if (fVerbose > 1)
        cout << "======================== BEAM tracking init finished ===================" << endl;

    return kSUCCESS;
}

void BmnBeamTracking::Exec(Option_t* opt) {
    if (fVerbose > 1)
        cout << "\n======================== BEAM tracking exec started ====================" << endl;
    if (fVerbose > 1)
        cout << "\n Event number: " << fEventNo << endl;

    if (!IsActive())
        return;

    fBeamTracksArray->Delete();

    fEventNo++;
    FindTracks();
    if (fVerbose > 0)
        cout << "BmnBeamTracking: " << fBeamTracksArray->GetEntriesFast() << " tracks" << endl;
    if (fVerbose > 1)
        cout << "\n======================== BEAM tracking exec finished ===================" << endl;
}

BmnStatus BmnBeamTracking::FindTracks() {

    vector<BmnSiBTHit*> sortedHits[fSiBTDetector->GetNStations()];
    SortHits(sortedHits);

    Float_t minChi2 = 10000000.0;
    BmnTrack minCand;
    for (BmnHit* hit2 : sortedHits[2]) {
        for (BmnHit* hit1 : sortedHits[1]) {
            for (BmnHit* hit0 : sortedHits[0]) {
                BmnTrack cand;
                cand.AddHit(hit0->GetIndex(), hit0);
                cand.AddHit(hit1->GetIndex(), hit1);
                cand.AddHit(hit2->GetIndex(), hit2);
                cand.SortHits();

                if (CalculateTrackParams(&cand) != kBMNERROR) {
                    if (cand.GetChi2() < minChi2) {
                        minChi2 = cand.GetChi2();
                        minCand = cand;
                    }
                }
            }
        }
    }
    
    if (minCand.GetNHits() < 3) return kBMNERROR;

    Double_t Xv = 0.0;
    Double_t Yv = 0.0;
    Double_t Zv = 0.0;

    TrackUpdateByKalman(&minCand);
    FairTrackParam par = *(minCand.GetParamFirst());
    
    for (Int_t iHit = 0; iHit < minCand.GetNHits(); ++iHit) {
        BmnHit* hit = (BmnHit*)fSiBTHitsArray->At(minCand.GetHitIndex(iHit));
        fKalman->TGeoTrackPropagate(&par, hit->GetZ(), 2212, nullptr, nullptr, 1);
        hit->SetResXY(par.GetX() - hit->GetX(), par.GetY() - hit->GetY());
    }
    
    fKalman->TGeoTrackPropagate(&par, Zv, 2212, nullptr, nullptr, 1);
    minCand.SetB(Sqrt(Sq(par.GetX() - Xv) + Sq(par.GetY() - Yv)));
    hdx->Fill(par.GetX() - Xv);
    hdy->Fill(par.GetY() - Yv);
    hdtx->Fill(par.GetTx());
    hdty->Fill(par.GetTy());

    new ((*fBeamTracksArray)[fBeamTracksArray->GetEntriesFast()]) BmnTrack(minCand);

    return kBMNSUCCESS;
}

void BmnBeamTracking::Finish() {
    TFile resid("beamResid_3ion_260-160-90.root", "RECREATE");
    resid.Add(hdx);
    resid.Add(hdy);
    resid.Add(hdtx);
    resid.Add(hdty);
    resid.Write();
    resid.Close();
    return;
}

BmnStatus BmnBeamTracking::SortHits(vector<BmnSiBTHit*>* sortedHits) {
    for (Int_t iHit = 0; iHit < fSiBTHitsArray->GetEntriesFast(); ++iHit) {
        BmnSiBTHit* hit = (BmnSiBTHit*)fSiBTHitsArray->At(iHit);
        if (!hit)
            continue;
        //180000 - eloss for Xe from simulation!!!
        if (hit->GetStripTotalSignalInLowerLayer() < 180000 || hit->GetStripTotalSignalInUpperLayer() < 180000) continue;
        Int_t station = hit->GetStation();
        sortedHits[station].push_back(hit);
    }

    return kBMNSUCCESS;
}

BmnStatus BmnBeamTracking::TrackUpdateByKalman(BmnTrack* cand) {

    FairTrackParam par = *(cand->GetParamFirst());
    Double_t chiTot = 0.0;
    for (Int_t iHit = 0; iHit < cand->GetNHits(); ++iHit) {
        BmnHit* hit = (BmnHit*)fSiBTHitsArray->At(cand->GetHitIndex(iHit));
        Double_t chi = 0.0;
        fKalman->TGeoTrackPropagate(&par, hit->GetZ(), 2212, nullptr, nullptr, 1);
        fKalman->Update(&par, hit, chi);
    }
    cand->SetParamLast(par);
    for (Int_t iHit = cand->GetNHits() - 1; iHit >= 0; iHit--) {
        BmnHit* hit = (BmnHit*)fSiBTHitsArray->At(cand->GetHitIndex(iHit));
        Double_t chi = 0.0;
        fKalman->TGeoTrackPropagate(&par, hit->GetZ(), 2212, nullptr, nullptr, 1);
        fKalman->Update(&par, hit, chi);
        chiTot += chi;
    }
    cand->SetParamFirst(par);
    cand->SetChi2(chiTot);

    return kBMNSUCCESS;
}

BmnStatus BmnBeamTracking::CalcCovMatrix(BmnTrack* tr) {
    const UInt_t nHits = 3;
    Double_t chi2circ = 0.0;
    TVector3 CircParZX = CircleFit(tr, fSiBTHitsArray, chi2circ);

    Double_t Xc = CircParZX.Y();  // x-coordinate of fit-circle center
    Double_t Zc = CircParZX.X();  // z-coordinate of fit-circle center
    fField = FairRunAna::Instance()->GetField();
    const Double_t B = (LineFit(tr, fSiBTHitsArray, "ZY")).X();  //angle coefficient for helicoid

    Double_t Q = (Xc > 0) ? +1 : -1;

    Double_t sumX(0.0), sumY(0.0), sumTx(0.0), sumTy(0.0), sumQP(0.0);
    Double_t sumXX(0.0), sumXY(0.0), sumXTx(0.0), sumXTy(0.0), sumXQP(0.0);
    Double_t sumYY(0.0), sumYTx(0.0), sumYTy(0.0), sumYQP(0.0);
    Double_t sumTxTx(0.0), sumTxTy(0.0), sumTxQP(0.0);
    Double_t sumTyTy(0.0), sumTyQP(0.0);
    Double_t sumQPQP(0.0);

    for (UInt_t i = 0; i < nHits; ++i) {
        BmnHit* hit = (BmnHit*)fSiBTHitsArray->At(tr->GetHitIndex(i));
        if (!hit) continue;
        Double_t Xi = hit->GetX();
        Double_t Yi = hit->GetY();
        Double_t Zi = hit->GetZ();

        Double_t Txi = -1.0 * (Zi - Zc) / (Xi - Xc);
        Double_t Tyi = B;
        Double_t Ri = Sqrt(Sq(Xi - Xc) + Sq(Zi - Zc));
        Double_t QPi = Q / (0.0003 * Abs(fField->GetBy(Xi, Yi, Zi)) * Ri);

        sumX += Xi;
        sumY += Yi;
        sumTx += Txi;
        sumTy += Tyi;
        sumQP += QPi;
        sumXX += Xi * Xi;
        sumXY += Xi * Yi;
        sumXTx += Xi * Txi;
        sumXTy += Xi * Tyi;
        sumXQP += Xi * QPi;
        sumYY += Yi * Yi;
        sumYTx += Yi * Txi;
        sumYTy += Yi * Tyi;
        sumYQP += Yi * QPi;
        sumTxTx += Txi * Txi;
        sumTxTy += Txi * Tyi;
        sumTxQP += Txi * QPi;
        sumTyTy += Tyi * Tyi;
        sumTyQP += Tyi * QPi;
        sumQPQP += QPi * QPi;
    }

    Double_t meanX = sumX / nHits;
    Double_t meanY = sumY / nHits;
    Double_t meanTx = sumTx / nHits;
    Double_t meanTy = sumTy / nHits;
    Double_t meanQP = sumQP / nHits;

    Double_t Cov_X_X = sumXX / nHits - Sq(meanX);
    Double_t Cov_X_Y = sumXY / nHits - meanX * meanY;
    Double_t Cov_X_Tx = sumXTx / nHits - meanX * meanTx;
    Double_t Cov_X_Ty = sumXTy / nHits - meanX * meanTy;
    Double_t Cov_X_Qp = sumXQP / nHits - meanX * meanQP;
    Double_t Cov_Y_Y = sumYY / nHits - Sq(meanY);
    Double_t Cov_Y_Tx = sumYTx / nHits - meanY * meanTx;
    Double_t Cov_Y_Ty = sumYTy / nHits - meanY * meanTy;
    Double_t Cov_Y_Qp = sumYQP / nHits - meanY * meanQP;
    Double_t Cov_Tx_Tx = sumTxTx / nHits - Sq(meanTx);
    Double_t Cov_Tx_Ty = sumTxTy / nHits - meanTx * meanTy;
    Double_t Cov_Tx_Qp = sumTxQP / nHits - meanTx * meanQP;
    Double_t Cov_Ty_Ty = sumTyTy / nHits - Sq(meanTy);
    Double_t Cov_Ty_Qp = sumTyQP / nHits - meanTy * meanQP;
    Double_t Cov_Qp_Qp = sumQPQP / nHits - Sq(meanQP);

    FairTrackParam par;
    // par.SetCovariance(0, 0, Cov_X_X);
    // par.SetCovariance(0, 1, Cov_X_Y);
    // par.SetCovariance(0, 2, Cov_X_Tx);
    // par.SetCovariance(0, 3, Cov_X_Ty);
    // par.SetCovariance(0, 4, Cov_X_Qp);
    // par.SetCovariance(1, 1, Cov_Y_Y);
    // par.SetCovariance(1, 2, Cov_Y_Tx);
    // par.SetCovariance(1, 3, Cov_Y_Ty);
    // par.SetCovariance(1, 4, Cov_Y_Qp);
    // par.SetCovariance(2, 2, Cov_Tx_Tx);
    // par.SetCovariance(2, 3, Cov_Tx_Ty);
    // par.SetCovariance(2, 4, Cov_Tx_Qp);
    // par.SetCovariance(3, 3, Cov_Ty_Ty);
    // par.SetCovariance(3, 4, Cov_Ty_Qp);
    // par.SetCovariance(4, 4, Cov_Qp_Qp);

//если ставить ненулевые ковариации в недиагональных членах, то трек начинает сильно уводить в сторону. Проверить!
    par.SetCovariance(0, 0, 1);
    par.SetCovariance(0, 1, 0.0);
    par.SetCovariance(0, 2, 0.0);
    par.SetCovariance(0, 3, 0.0);
    par.SetCovariance(0, 4, 0.0);
    par.SetCovariance(1, 1, 1);
    par.SetCovariance(1, 2, 0.0);
    par.SetCovariance(1, 3, 0.0);
    par.SetCovariance(1, 4, 0.0);
    par.SetCovariance(2, 2, 1);
    par.SetCovariance(2, 3, 0.0);
    par.SetCovariance(2, 4, 0.0);
    par.SetCovariance(3, 3, 1);
    par.SetCovariance(3, 4, 0.0);
    par.SetCovariance(4, 4, 1);

    tr->SetParamFirst(par);
    tr->SetParamLast(par);

    return kBMNSUCCESS;
}

BmnStatus BmnBeamTracking::CalculateTrackParams(BmnTrack* tr) {
    //Estimation of track parameters for events with magnetic field

    TVector3 lineParZY = LineFit(tr, fSiBTHitsArray, "ZY");
    const Double_t B = lineParZY.X(); //angle coefficient for helicoid

    Double_t Tx_first = CalcTx((BmnHit*)fSiBTHitsArray->At(tr->GetHitIndex(0)), (BmnHit*)fSiBTHitsArray->At(tr->GetHitIndex(1)), (BmnHit*)fSiBTHitsArray->At(tr->GetHitIndex(2)));
    Double_t Tx_last = CalcTx((BmnHit*)fSiBTHitsArray->At(tr->GetHitIndex(2)), (BmnHit*)fSiBTHitsArray->At(tr->GetHitIndex(1)), (BmnHit*)fSiBTHitsArray->At(tr->GetHitIndex(0)));

    CalcCovMatrix(tr);
    TVector3 firstPos;
    TVector3 lastPos;
    ((BmnHit*)fSiBTHitsArray->At(tr->GetHitIndex(0)))->Position(firstPos);
    ((BmnHit*)fSiBTHitsArray->At(tr->GetHitIndex(2)))->Position(lastPos);
    tr->GetParamFirst()->SetPosition(firstPos);
    tr->GetParamFirst()->SetTx(Tx_first);
    tr->GetParamFirst()->SetTy(B);
    tr->GetParamLast()->SetPosition(lastPos);
    tr->GetParamLast()->SetTx(Tx_last);
    tr->GetParamLast()->SetTy(B);
    Double_t QP = CalcQp(tr);
    tr->GetParamFirst()->SetQp(QP);
    tr->GetParamLast()->SetQp(QP);
    tr->SetChi2(lineParZY.Z());
    return kBMNSUCCESS;
}

Double_t BmnBeamTracking::CalcQp(BmnTrack* track) {

    vector<BmnHit*> hits;
    for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++)
        hits.push_back((BmnHit*)fSiBTHitsArray->At(track->GetHitIndex(iHit)));

    // Get q/p info from all track segments
    BmnHit* first = hits[0];
    BmnHit* second = hits[1];
    BmnHit* third = hits[2];

    TVector3 CircParZX = CircleBy3Hit(first, second, third);
    Double_t R = CircParZX.Z();
    Double_t Xc = CircParZX.Y();
    Double_t Zc = CircParZX.X();

    Double_t Q = (Xc > 0) ? +1. : -1.;
    Double_t S = 0.0003 * (Abs(fField->GetBy(third->GetX(), third->GetY(), third->GetZ())) + Abs(fField->GetBy(second->GetX(), second->GetY(), second->GetZ())) + Abs(fField->GetBy(first->GetX(), first->GetY(), first->GetZ()))) / 3.;

    Double_t Pt = S * R;  //actually Pt/Q, but it doesn't matter
    Double_t fX = first->GetX();
    Double_t fZ = first->GetZ();

    Double_t h = -1.0;

    Double_t Tx_first = h * (fZ - Zc) / (fX - Xc);
    TVector3 lineParZY = LineFit(track, fSiBTHitsArray, "ZY");
    const Double_t B = lineParZY.X();  //angle coefficient for helicoid
    Double_t Ty_first = B;             // / (fX - Xc);

    Double_t Pz = Pt / Sqrt(1 + Sq(Tx_first));
    Double_t Px = Tx_first * Pz;
    Double_t Py = Ty_first * Pz;
    Double_t P = Sqrt(Sq(Px) + Sq(Py) + Sq(Pz));
    Double_t QP = Q / P;

    return QP;
}
