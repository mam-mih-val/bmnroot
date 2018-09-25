/** BmnMaterialEffects.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 **/
#include "BmnMaterialEffects.h"

#include "FairTrackParam.h"
#include "BmnMaterialInfo.h"

#include "TDatabasePDG.h"
#include "TParticlePDG.h"

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cassert>

using namespace std;

BmnMaterialEffects::BmnMaterialEffects() :
fMass(0.105),
fDownstream(kTRUE),
fIsElectron(kFALSE),
fIsMuon(kTRUE) {
}

BmnMaterialEffects::~BmnMaterialEffects() {
}

BmnStatus BmnMaterialEffects::Update(FairTrackParam* par, const BmnMaterialInfo* mat, Int_t pdg, Bool_t downstream) {
    if (mat->GetLength() * mat->GetRho() < 1e-10) return kBMNSUCCESS;

    fDownstream = downstream;
    TDatabasePDG* db = TDatabasePDG::Instance();
    TParticlePDG* particle = db->GetParticle(pdg);
    assert(particle != NULL);
    fMass = particle->Mass();
    fIsElectron = (std::abs(pdg) == 11) ? kTRUE : kFALSE;
    fIsMuon = (std::abs(pdg) == 13) ? kTRUE : kFALSE;

    AddEnergyLoss(par, mat);

    AddThinScatter(par, mat);
    AddThickScatter(par, mat);

    return kBMNSUCCESS;
}

void BmnMaterialEffects::AddEnergyLoss(FairTrackParam* par, const BmnMaterialInfo* mat) const {
    if (fIsElectron) {
        Float_t radLength = mat->GetLength() / mat->GetRL();
        Float_t t;

        if (!fDownstream) {
            t = radLength;
        } else {
            t = -radLength;
        }

        Float_t qp = par->GetQp();
        //        cout << "QP-before = " << qp << endl;
        qp *= std::exp(-t);
        //        cout << "QP-after  = " << qp << endl;
        par->SetQp(qp);

        Float_t cov = par->GetCovariance(4, 4);
        cov += CalcSigmaSqQpElectron(par, mat);
        par->SetCovariance(4, 4, cov);
    } else {
        Float_t Eloss = EnergyLoss(par, mat);
        par->SetQp(CalcQpAfterEloss(par->GetQp(), Eloss));

        Float_t cov = par->GetCovariance(4, 4);
        cov += CalcSigmaSqQp(par, mat);
        par->SetCovariance(4, 4, cov);
    }
}

void BmnMaterialEffects::AddThickScatter(FairTrackParam* par, const BmnMaterialInfo* mat) const {
    if (mat->GetLength() < 1e-10) {
        return;
    }

    Float_t tx = par->GetTx();
    Float_t ty = par->GetTy();
    Float_t thickness = mat->GetLength(); //cm
    Float_t thetaSq = CalcThetaSq(par, mat);

    Float_t t = 1 + tx * tx + ty * ty;

    Float_t Q33 = (1 + tx * tx) * t * thetaSq;
    Float_t Q44 = (1 + ty * ty) * t * thetaSq;
    Float_t Q34 = tx * ty * t * thetaSq;

    Float_t T23 = (thickness * thickness) / 3.0;
    Float_t T2 = thickness / 2.0;

    Float_t D = (fDownstream) ? 1. : -1.;

    Double_t C[15];
    par->CovMatrix(C);

    C[0] += Q33 * T23;
    C[1] += Q34 * T23;
    C[2] += Q33 * D * T2;
    C[3] += Q34 * D * T2;

    C[5] += Q44 * T23;
    C[6] += Q34 * D * T2;
    C[7] += Q44 * D * T2;

    C[9] += Q33;
    C[10] += Q34;

    C[12] += Q44;

    par->SetCovMatrix(C);
}

void BmnMaterialEffects::AddThinScatter(FairTrackParam* par, const BmnMaterialInfo* mat) const {
    if (mat->GetLength() < 1e-10) {
        return;
    }
    Float_t tx = par->GetTx();
    Float_t ty = par->GetTy();
    Float_t thetaSq = CalcThetaSq(par, mat);

    Float_t t = 1 + tx * tx + ty * ty;

    Float_t Q33 = (1 + tx * tx) * t * thetaSq;
    Float_t Q44 = (1 + ty * ty) * t * thetaSq;
    Float_t Q34 = tx * ty * t * thetaSq;

    Double_t C[15];
    par->CovMatrix(C);
    C[9] += Q33;
    C[12] += Q44;
    C[10] += Q34;
    par->SetCovMatrix(C);
}

Float_t BmnMaterialEffects::CalcThetaSq(const FairTrackParam* par, const BmnMaterialInfo* mat) const {
    Float_t p = std::abs(1. / par->GetQp()); //GeV
    Float_t E = std::sqrt(fMass * fMass + p * p);
    Float_t beta = p / E;
    Float_t x = mat->GetLength(); //cm
    Float_t X0 = mat->GetRL(); //cm
    Float_t bcp = beta * p;
    Float_t z = 1.;

    Float_t theta = 0.0136 * (1. / bcp) * z * std::sqrt(x / X0) *
            (1. + 0.038 * std::log(x / X0));
    return theta * theta;
}

Float_t BmnMaterialEffects::EnergyLoss(const FairTrackParam* par, const BmnMaterialInfo* mat) const {
    Float_t length = mat->GetRho() * mat->GetLength();
    return dEdx(par, mat) * length;
    //return MPVEnergyLoss(par, mat);
}

Float_t BmnMaterialEffects::dEdx(const FairTrackParam* par, const BmnMaterialInfo* mat) const {
    Float_t dedx = BetheBloch(par, mat);
    // dedx += BetheHeitler(par, mat);
    // if (fIsMuon) dedx += PairProduction(par, mat);
    return dedx;
}

Float_t BmnMaterialEffects::BetheBloch(const FairTrackParam* par, const BmnMaterialInfo* mat) const {
    Float_t K = 0.000307075; // GeV * g^-1 * cm^2
    Float_t z = (par->GetQp() > 0.) ? 1 : -1.;
    Float_t Z = mat->GetZ();
    Float_t A = mat->GetA();

    Float_t M = fMass;
    Float_t p = std::abs(1. / par->GetQp()); //GeV
    Float_t E = std::sqrt(M * M + p * p);
    Float_t beta = p / E;
    Float_t betaSq = beta * beta;
    Float_t gamma = E / M;
    Float_t gammaSq = gamma * gamma;

    Float_t I = CalcI(Z) * 1e-9; // GeV

    Float_t me = 0.000511; // GeV
    Float_t ratio = me / M;
    Float_t Tmax = (2 * me * betaSq * gammaSq) / (1 + 2 * gamma * ratio + ratio * ratio);

    // density correction
    Float_t dc = 0.;
    if (p > 0.5) { // for particles above 1 Gev
        Float_t rho = mat->GetRho();
        Float_t hwp = 28.816 * std::sqrt(rho * Z / A) * 1e-9; // GeV
        dc = std::log(hwp / I) + std::log(beta * gamma) - 0.5;
    }

    return K * z * z * (Z / A)*(1. / betaSq) * (0.5 * std::log(2 * me * betaSq * gammaSq * Tmax / (I * I)) - betaSq - dc);
}

Float_t BmnMaterialEffects::BetheBlochElectron(const FairTrackParam* par, const BmnMaterialInfo* mat) const {
    Float_t K = 0.000307075; // GeV * g^-1 * cm^2
    //myf z = (par->GetQp() > 0.) ? 1 : -1.;
    Float_t Z = mat->GetZ();
    Float_t A = mat->GetA();

    Float_t me = 0.000511; // GeV;
    Float_t p = std::abs(1. / par->GetQp()); //GeV
    Float_t E = std::sqrt(me * me + p * p);
    Float_t gamma = E / me;

    Float_t I = CalcI(Z) * 1e-9; // GeV

    if (par->GetQp() > 0) { // electrons
        return K * (Z / A) * (std::log(2 * me / I) + 1.5 * std::log(gamma) - 0.975);
    } else { //positrons
        return K * (Z / A) * (std::log(2 * me / I) + 2. * std::log(gamma) - 1.);
    }
}

Float_t BmnMaterialEffects::CalcQpAfterEloss(Float_t qp, Float_t eloss) const {
    Float_t massSq = fMass*fMass;
    Float_t p = std::abs(1. / qp);
    Float_t E = std::sqrt(p * p + massSq);
    Float_t q = (qp > 0) ? 1. : -1.;
    if (!fDownstream) {
        eloss *= -1.0;
    } // TODO check this
    Float_t Enew = E - eloss;
    Float_t pnew = (Enew > fMass) ? std::sqrt(Enew * Enew - massSq) : 0.;
    if (pnew != 0) {
        return q / pnew;
    } else {
        return 1e5;
    }

    //if (!fDownstream) eloss *= -1.0;
    //if (p > 0.) p -= eloss;
    //else p += eloss;
    //return 1./p;
}

Float_t BmnMaterialEffects::CalcSigmaSqQp(const FairTrackParam* par, const BmnMaterialInfo* mat) const {
    Float_t P = std::abs(1. / par->GetQp()); // GeV
    Float_t XMASS = fMass; // GeV
    Float_t E = std::sqrt(P * P + XMASS * XMASS);
    Float_t Z = mat->GetZ();
    Float_t A = mat->GetA();
    Float_t RHO = mat->GetRho();
    Float_t STEP = mat->GetLength();
    Float_t EMASS = 0.511 * 1e-3; // GeV

    Float_t BETA = P / E;
    Float_t GAMMA = E / XMASS;

    // Calculate xi factor (KeV).
    Float_t XI = (153.5 * Z * STEP * RHO) / (A * BETA * BETA);

    // Maximum energy transfer to atomic electron (KeV).
    Float_t ETA = BETA*GAMMA;
    Float_t ETASQ = ETA*ETA;
    Float_t RATIO = EMASS / XMASS;
    Float_t F1 = 2. * EMASS*ETASQ;
    Float_t F2 = 1. + 2. * RATIO * GAMMA + RATIO*RATIO;
    Float_t EMAX = 1e6 * F1 / F2;

    Float_t DEDX2 = XI * EMAX * (1. - (BETA * BETA / 2.))*1e-12;

    Float_t SDEDX = (E * E * DEDX2) / std::pow(P, 6);

    return std::abs(SDEDX);
}

Float_t BmnMaterialEffects::CalcSigmaSqQpElectron(const FairTrackParam* par, const BmnMaterialInfo* mat) const {
    Float_t x = mat->GetLength(); //cm
    Float_t X0 = mat->GetRL(); //cm
    return par->GetQp() * par->GetQp() *
            (std::exp(-x / X0 * std::log(3.0) / std::log(2.0)) -
            std::exp(-2.0 * x / X0));
}

Float_t BmnMaterialEffects::CalcI(Float_t Z) const {
    // mean excitation energy in eV
    if (Z > 16.) {
        return 10 * Z;
    } else {
        return 16 * std::pow(Z, 0.9);
    }
}

Float_t BmnMaterialEffects::BetheHeitler(const FairTrackParam* par, const BmnMaterialInfo* mat) const {
    Float_t M = fMass; //GeV
    Float_t p = std::abs(1. / par->GetQp()); // GeV
    Float_t rho = mat->GetRho();
    Float_t X0 = mat->GetRL();
    Float_t me = 0.000511; // GeV
    Float_t E = std::sqrt(M * M + p * p);
    Float_t ratio = me / M;

    return (E * ratio * ratio) / (X0 * rho);
}

Float_t BmnMaterialEffects::PairProduction(const FairTrackParam* par, const BmnMaterialInfo* mat) const {
    Float_t p = std::abs(1. / par->GetQp()); // GeV
    Float_t M = fMass; //GeV
    Float_t rho = mat->GetRho();
    Float_t X0 = mat->GetRL();
    Float_t E = std::sqrt(M * M + p * p);

    return 7e-5 * E / (X0 * rho);
}

Float_t BmnMaterialEffects::BetheBlochSimple(const BmnMaterialInfo* mat) const {
    return 0.00354 * mat->GetZ() / mat->GetA();
}

Float_t BmnMaterialEffects::MPVEnergyLoss(const FairTrackParam* par, const BmnMaterialInfo* mat) const {
    Float_t M = fMass * 1e3; //MeV
    Float_t p = std::abs(1. / par->GetQp()) * 1e3; // MeV

    //   myf rho = mat->GetRho();
    Float_t Z = mat->GetZ();
    Float_t A = mat->GetA();
    Float_t x = mat->GetRho() * mat->GetLength();

    Float_t I = CalcI(Z) * 1e-6; // MeV

    Float_t K = 0.307075; // MeV g^-1 cm^2
    Float_t j = 0.200;

    Float_t E = std::sqrt(M * M + p * p);
    Float_t beta = p / E;
    Float_t betaSq = beta * beta;
    Float_t gamma = E / M;
    Float_t gammaSq = gamma * gamma;

    Float_t ksi = (K / 2.)*(Z / A)*(x / betaSq); // MeV

    //   myf hwp = 28.816 * std::sqrt(rho*Z/A) * 1e-6 ; // MeV
    //   myf dc = std::log(hwp/I) + std::log(beta*gamma) - 0.5;
    //   dc *= 2;
    Float_t dc = 0.;

    Float_t eloss = ksi * (std::log(2 * M * betaSq * gammaSq / I) + std::log(ksi / I) + j - betaSq - dc);

    return eloss * 1e-3; //GeV
}
