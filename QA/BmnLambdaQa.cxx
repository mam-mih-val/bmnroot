/**
 * \file BmnLambdaQa.cxx
 * \brief FairTask for MC simulated lambda reconstruction performance calculation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \author Alexander Lytaev <sas-lyt@ya.ru> - modifications for BMN experiment
 * \date 2018 July
 */

#include "BmnLambdaQa.h"
#include "BmnLambdaQaReport.h"
#include "BmnHistManager.h"
//#include "CbmStsAddress.h"
#include <TChain.h>
#include <TString.h>
#include "BmnGemTrack.h"
#include <TClonesArray.h>
#include "BmnSimulationReport.h"
#include "CbmMCTrack.h"
#include "BmnParticlePair.h"
#include "BmnSiliconPoint.h"
#include "BmnGlobalTrack.h"

#include <boost/assign/list_of.hpp>
#include <fstream>
#include <cmath>
#include <sstream>

using std::cout;
using std::vector;
using std::map;
using std::binary_search;
using boost::assign::list_of;
using namespace TMath;

Int_t BmnLambdaQa::fCurrentEvent = 0;

BmnLambdaQa::BmnLambdaQa() :
fHM(NULL),
fOutputDir("./"),
fMCTracks(NULL),
fNOfBins(50),
fLambdaPRangeMin(0.),
fLambdaPRangeMax(4.),
fLambdaEtaRangeMin(0.),
fLambdaEtaRangeMax(5.),
fLambdaThetaRangeMin(0.),
fLambdaThetaRangeMax(45.),
fLambdaThetaRRangeMin(0.),
fLambdaThetaRRangeMax(2.5),
fProtonsPRangeMin(0.),
fProtonsPRangeMax(4.),
fProtonsEtaRangeMin(0.),
fProtonsEtaRangeMax(5.),
fProtonsThetaRangeMin(0.),
fProtonsThetaRangeMax(45.),
fProtonsThetaRRangeMin(0.),
fProtonsThetaRRangeMax(2.5),
fMesonsPRangeMin(0.),
fMesonsPRangeMax(2.),
fMesonsEtaRangeMin(0.),
fMesonsEtaRangeMax(4.),
fMesonsThetaRangeMin(0.),
fMesonsThetaRangeMax(45.),
fMesonsThetaRRangeMin(0.),
fMesonsThetaRRangeMax(2.5),
fInvMassMin(1.07),
fInvMassMax(1.22),
fUseMCFile(kFALSE),
fUseRecoFile(kFALSE) {
    // Initialize cut arrays ...
    Double_t val = 0.;
    for (Int_t i = 0; i < 2; i++) {
        val = (i == 0) ? -DBL_MAX : DBL_MAX;
        for (Int_t j = 0; j < 2; j++) {
            val = (j == 0) ? -DBL_MAX : DBL_MAX;
            fMom[i][j] = val;
            fEta[i][j] = val;
            fDCA[i][j] = val;
            fDCA12[j] = val;
            fPath[j] = val;
        }
    }
}

BmnLambdaQa::~BmnLambdaQa() {
    if (fHM)
        delete fHM;
}

InitStatus BmnLambdaQa::Init() {

    fHM = new BmnHistManager();

    ReadDataBranches();
    fNLambdas = 0;
    fNReconstructable = 0;

    fNOfParticlePairs = 0;
    fNOfParticlePairsWithMatchedLambda = 0;

    fNOfParticlePairsMC = 0;
    fNOfLambdasParticlePairsMC = 0;
    fNOfParticlePairsMCAll = 0;

    CreateH1("numberOfLambdas_LambdaQa", "", "", 1, 0, 1.);
    CreateH1("numberOfReconstructableLambdas_LambdaQa", "", "", 1, 0, 1.);
    CreateNumberOfReconstructableLambdaHistograms();
    CreateNumberOfLambdaHistograms();
    CreateLambdaRecEfficiencyHistograms();
    CreateTwoDimensionalRecEfficiencyHistograms();
    CreateNumberOLambdaDecayProtonsHistograms();
    CreateNumberOfLambdaDecayMesonsHistograms();
    CreateRecProtonsRecEfficiencyHistograms();
    CreateRecMesonsRecEfficiencyHistograms();
    CreateTwoDimensionalRecProtonsRecEfficiencyHistograms();
    CreateTwoDimensionalRecMesonsRecEfficiencyHistograms();
    CreateH1("numberOfMCReconstructedParticlePairsQA", "", "", 1, 0, 1.);
    CreateH1("numberOfMCReconstructedLambdasQA", "", "", 1, 0, 1.);
    CreateH1("numberOfMCReconstructedAllParticlePairsQA", "", "", 1, 0, 1.);
    CreateTwoDimensionalReconstructedParticlePairsFromMCDataHistograms();
    CreateReconstructedParticlePairsFromMCDataHistograms();
    CreateTwoDimensionalReconstructedParticlePairsFromMCDataWOCutsHistograms();
    CreateReconstructedParticlePairsFromMCDataWOCutsHistograms();
    CreateH1("numberOfReconstructedParticlePairWithMatchedLambdaQA", "", "", 1, 0, 1.);
    CreateH1("numberOfNotReconstructedLambdasQA", "", "", 1, 0, 1.);
    CreateNumberOfReconstrcutedLambdaHistograms();
    CreateReconstructionEfficiencyHistograms();
    CreateTwoDimensionalRealRecEfficiencyHistograms();
    CreateNumberOfReconstrcutedRecProtonHistograms();
    CreateTwoDimensionalNumberOfReconstructedProtonsHistograms();
    CreateRecProtonsReconstructionEfficiencyHistograms();
    CreateNumberOfReconstrcutedRecMesonsHistograms();
    CreateTwoDimensionalNumberOfReconstructedMesonsHistograms();
    CreateRecMesonsReconstructionEfficiencyHistograms();
    CreateNumberOfNotReconstructedLambdaHistograms();
    CreateH1("numberOfReconstructedParticlePairsQA", "", "", 1, 0, 1.);
    CreateTwoDimensionalReconstructedParticlePairsHistograms();
    CreateReconstructedParticlePairsHistograms();
    CreateTwoDimensionalReconstructedParticlePairsWOCutsHistograms();
    CreateReconstructedParticlePairsWOCutsHistograms();

    return kSUCCESS;
}

void BmnLambdaQa::Exec(Option_t* opt) {
    if (fCurrentEvent % 100 == 0)
        cout << "Event# " << fCurrentEvent << endl;
    fCurrentEvent++;

    vector <Int_t> indicesOfLambdaProtons;
    vector <Int_t> indicesOfLambdaMesons;

    /* array: 0-th index is mother lambda's Id, 1-st is proton's Id, 2-nd is negative pi-meson's Id*/
    vector <array< int, 3 >> indicesOfLamda;

    for (Int_t iTrack = 0; iTrack < fMCTracks->GetEntriesFast(); iTrack++) {
        CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(iTrack);

        if (NULL == track) {
            cout << "Monte-Carlo simulation branch error" << endl;
            return;
        }

        Double_t P = ((CbmMCTrack*) fMCTracks->At(iTrack))->GetP();
        Double_t P_x = ((CbmMCTrack*) fMCTracks->At(iTrack))->GetPx();
        Double_t P_y = ((CbmMCTrack*) fMCTracks->At(iTrack))->GetPy();
        Double_t P_z = ((CbmMCTrack*) fMCTracks->At(iTrack))->GetPz();
        Double_t pRapidity = 0.5 * Log((P + P_z) / (P - P_z));
        Double_t rapidity = ((CbmMCTrack*) fMCTracks->At(iTrack))->GetRapidity();

        if (3122 == track->GetPdgCode()) {
            fNLambdas++;

            fHM->H1("numberOfLambdas_LambdaQa")->Fill(1);

            fHM->H1("simNLambda_P_sim")->Fill(P);
            fHM->H1("simNLambda_eta_sim")->Fill(pRapidity);
            fHM->H1("simNLambda_theta_sim")->Fill(TMath::ACos(P_z / P) * TMath::RadToDeg());
            fHM->H1("simNLambda_theta_r_sim")->Fill(rapidity);
            fHM->H1("simNLambda_P_x_sim")->Fill(P_x);
            fHM->H1("simNLambda_P_y_sim")->Fill(P_y);
            fHM->H1("simNLambda_P_z_sim")->Fill(P_z);
            fHM->H2("simNLambda_eta_P_sim")->Fill(P, pRapidity);
        }

        if (2212 == track->GetPdgCode()) {
            int motherId = track->GetMotherId();

            if (motherId < 0)
                continue;

            if (3122 == ((CbmMCTrack*) fMCTracks->At(motherId))->GetPdgCode()) {
                indicesOfLambdaProtons.push_back(iTrack);

                fHM->H1("NProtons_P_sim")->Fill(P);
                fHM->H1("NProtons_eta_sim")->Fill(pRapidity);
                fHM->H1("NProtons_theta_sim")->Fill(TMath::ACos(P_z / P) * TMath::RadToDeg());
                fHM->H1("NProtons_theta_r_sim")->Fill(rapidity);
                fHM->H2("simNRecProtons_eta_P_sim")->Fill(pRapidity, P);

                Double_t nSiliconHitsOnTrackP = 0;

                for (Int_t hit = 0; hit < fSiliconPoints->GetEntriesFast(); hit++) {
                    if (((BmnSiliconPoint*) fSiliconPoints->At(hit))->GetTrackID() == iTrack)
                        nSiliconHitsOnTrackP++;
                }

                if (track->GetNPoints(kGEM) + nSiliconHitsOnTrackP >= 4) {
                    fHM->H1("NRecProtons_P_sim")->Fill(P);
                    fHM->H1("NRecProtons_eta_sim")->Fill(pRapidity);
                    fHM->H1("NRecProtons_theta_sim")->Fill(TMath::ACos(P_z / P) * TMath::RadToDeg());
                    fHM->H1("NRecProtons_theta_r_sim")->Fill(rapidity);
                    fHM->H2("simNReconstructableRecProtons_eta_P_sim")->Fill(pRapidity, P);
                }
            }
        }

        if (-211 == track->GetPdgCode()) {
            int motherIdMeson = track->GetMotherId();

            if (motherIdMeson < 0) {
                continue;
            }

            if (3122 == ((CbmMCTrack*) fMCTracks->At(motherIdMeson))->GetPdgCode()) {
                indicesOfLambdaMesons.push_back(iTrack);

                fHM->H1("NMesons_P_sim")->Fill(P);
                fHM->H1("NMesons_eta_sim")->Fill(pRapidity);
                fHM->H1("NMesons_theta_sim")->Fill(TMath::ACos(P_z / P) * TMath::RadToDeg());
                fHM->H1("NMesons_theta_r_sim")->Fill(rapidity);
                fHM->H2("simNRecMesons_eta_P_sim")->Fill(pRapidity, P);

                Double_t nSiliconHitsOnTrackM = 0;

                for (Int_t hit = 0; hit < fSiliconPoints->GetEntriesFast(); hit++) {
                    if (((BmnSiliconPoint*) fSiliconPoints->At(hit))->GetTrackID() == iTrack)
                        nSiliconHitsOnTrackM++;
                }

                if (track->GetNPoints(kGEM) + nSiliconHitsOnTrackM >= 4) {
                    fHM->H1("NRecMesons_P_sim")->Fill(P);
                    fHM->H1("NRecMesons_eta_sim")->Fill(pRapidity);
                    fHM->H1("NRecMesons_theta_sim")->Fill(TMath::ACos(P_z / P) * TMath::RadToDeg());
                    fHM->H1("NRecMesons_theta_r_sim")->Fill(rapidity);
                    fHM->H2("simNReconstructableRecMesons_eta_P_sim")->Fill(pRapidity, P);
                }
            }
        }
    }

    for (Int_t cnt1 = 0; cnt1 < indicesOfLambdaProtons.size(); cnt1++) {
        for (Int_t cnt2 = 0; cnt2 < indicesOfLambdaMesons.size(); cnt2++) {
            if (((CbmMCTrack*) fMCTracks->At(indicesOfLambdaProtons[cnt1]))->GetMotherId() == ((CbmMCTrack*) fMCTracks->At(indicesOfLambdaMesons[cnt2]))->GetMotherId()) {
                array<int, 3> tmp = {((CbmMCTrack*) fMCTracks->At(indicesOfLambdaProtons[cnt1]))->GetMotherId(), indicesOfLambdaProtons[cnt1], indicesOfLambdaMesons[cnt2]};
                indicesOfLamda.push_back(tmp);
            }
        }
    }

    for (Int_t cnt = 0; cnt < indicesOfLamda.size(); cnt++) {
        Int_t nSiliconHitsOnTrackP = 0;
        Int_t nSiliconHitsOnTrackM = 0;
        Int_t nSSDHitsOnTrackP = 0;
        Int_t nSSDHitsOnTrackM = 0;
        Int_t nGEMHitsOnTrackP = 0;
        Int_t nGEMHitsOnTrackM = 0;

        for (Int_t hit = 0; hit < fSiliconPoints->GetEntriesFast(); hit++) {
            Int_t trId = ((BmnSiliconPoint*) fSiliconPoints->At(hit))->GetTrackID();
            if (trId == indicesOfLamda[cnt][1]) nSiliconHitsOnTrackP++;
            else if (trId == indicesOfLamda[cnt][2]) nSiliconHitsOnTrackM++;
        }

        nGEMHitsOnTrackP = ((CbmMCTrack*) fMCTracks->At(indicesOfLamda[cnt][1]))->GetNPoints(kGEM);
        nGEMHitsOnTrackM = ((CbmMCTrack*) fMCTracks->At(indicesOfLamda[cnt][2]))->GetNPoints(kGEM);

        if (nGEMHitsOnTrackP + nSiliconHitsOnTrackP + nSSDHitsOnTrackP >= 4 && nGEMHitsOnTrackM + nSiliconHitsOnTrackM + nSSDHitsOnTrackM >= 4) {
            fNReconstructable++;

            Double_t P = ((CbmMCTrack*) fMCTracks->At(indicesOfLamda[cnt][0]))->GetP();
            Double_t P_x = ((CbmMCTrack*) fMCTracks->At(indicesOfLamda[cnt][0]))->GetPx();
            Double_t P_y = ((CbmMCTrack*) fMCTracks->At(indicesOfLamda[cnt][0]))->GetPy();
            Double_t P_z = ((CbmMCTrack*) fMCTracks->At(indicesOfLamda[cnt][0]))->GetPz();
            Double_t pRapidity = 0.5 * Log((P + P_z) / (P - P_z));
            Double_t rapidity = ((CbmMCTrack*) fMCTracks->At(indicesOfLamda[cnt][0]))->GetRapidity();

            fHM->H1("numberOfReconstructableLambdas_LambdaQa")->Fill(1);

            fHM->H1("simNReconstructableLambda_P_sim")->Fill(P);
            fHM->H1("simNReconstructableLambda_eta_sim")->Fill(pRapidity);
            fHM->H1("simNReconstructableLambda_theta_r_sim")->Fill(rapidity);
            fHM->H1("simNReconstructableLambda_theta_sim")->Fill(TMath::ACos(P_z / P) * TMath::RadToDeg());
            fHM->H1("simNReconstructableLambda_P_x_sim")->Fill(P_x);
            fHM->H1("simNReconstructableLambda_P_y_sim")->Fill(P_y);
            fHM->H1("simNReconstructableLambda_P_z_sim")->Fill(P_z);
            fHM->H2("simNReconstructableLambda_eta_P_sim")->Fill(P, pRapidity);
        }
    }

    if (fParticlePair_MC)
        for (Int_t iPair = 0; iPair < fParticlePair_MC->GetEntriesFast(); iPair++) {
            BmnParticlePair* pair = (BmnParticlePair*) fParticlePair_MC->UncheckedAt(iPair);
            Double_t momPart1 = pair->GetMomPart1();
            Double_t momPart2 = pair->GetMomPart2();
            Double_t etaPart1 = pair->GetEtaPart1();
            Double_t etaPart2 = pair->GetEtaPart2();
            Double_t dca1 = pair->GetDCA1();
            Double_t dca2 = pair->GetDCA2();
            Double_t dca12 = pair->GetDCA12();
            Double_t path = pair->GetPath();

            // Apply cuts ...
            if (momPart1 < fMom[0][0] || momPart1 > fMom[0][1] || momPart2 < fMom[1][0] || momPart2 > fMom[1][1])
                continue;

            if (etaPart1 < fEta[0][0] || etaPart1 > fEta[0][1] || etaPart2 < fEta[1][0] || etaPart2 > fEta[1][1])
                continue;

            // Geom. cuts applied ...
            if (dca1 < fDCA[0][0] || dca1 > fDCA[0][1] || dca2 < fDCA[1][0] || dca2 > fDCA[1][1])
                continue;

            if (dca12 < fDCA12[0] || dca12 > fDCA12[1])
                continue;

            if (path < fPath[0] || path > fPath[1])
                continue;

            fNOfParticlePairsMC++;
            fHM->H1("numberOfMCReconstructedAllParticlePairsQA")->Fill(1);
            fHM->H1("NPairsRecoFromMCInvMass")->Fill(pair->GetInvMass());

            fHM->H2("NPairsRecoFromMCInvMassMomProton")->Fill(momPart1, pair->GetInvMass());
            fHM->H2("NPairsRecoFromMCInvMassMomMeson")->Fill(momPart2, pair->GetInvMass());
            fHM->H2("NPairsRecoFromMCInvMassEtaProton")->Fill(etaPart1, pair->GetInvMass());
            fHM->H2("NPairsRecoFromMCInvMassEtaMeson")->Fill(etaPart2, pair->GetInvMass());
            fHM->H2("NPairsRecoFromMCInvMassDCA1")->Fill(dca1, pair->GetInvMass());
            fHM->H2("NPairsRecoFromMCInvMassDCA2")->Fill(dca2, pair->GetInvMass());
            fHM->H2("NPairsRecoFromMCInvMassDCA12X")->Fill(dca12, pair->GetInvMass());
            fHM->H2("NPairsRecoFromMCInvMassPath")->Fill(path, pair->GetInvMass());

            CbmMCTrack* posMCTrack = (CbmMCTrack*) fMCTracks->At(pair->GetMCTrackIdPart1());
            CbmMCTrack* negMCTrack = (CbmMCTrack*) fMCTracks->At(pair->GetMCTrackIdPart2());
            if (!negMCTrack || !posMCTrack) continue;
            if ((posMCTrack->GetMotherId() != negMCTrack->GetMotherId()) || (posMCTrack->GetMotherId() < 0)) {
                continue;
            }
            CbmMCTrack* lambda = (CbmMCTrack*) fMCTracks->At(posMCTrack->GetMotherId());
            if (lambda->GetPdgCode() != 3122) {
                continue;
            }
            fNOfLambdasParticlePairsMC++;
            fHM->H1("numberOfMCReconstructedLambdasQA")->Fill(1);
        }

    if (fParticlePair_MC)
        for (Int_t iPair = 0; iPair < fParticlePair_MC->GetEntriesFast(); iPair++) {
            fNOfParticlePairsMCAll++;
            fHM->H1("numberOfMCReconstructedParticlePairsQA")->Fill(1);

            BmnParticlePair* pair = (BmnParticlePair*) fParticlePair_MC->At(iPair);

            fHM->H1("NPairsRecoFromMCWOCutsInvMass")->Fill(pair->GetInvMass());

            Double_t momPart1 = pair->GetMomPart1();
            Double_t momPart2 = pair->GetMomPart2();
            Double_t etaPart1 = pair->GetEtaPart1();
            Double_t etaPart2 = pair->GetEtaPart2();
            Double_t dca1 = pair->GetDCA1();
            Double_t dca2 = pair->GetDCA2();
            Double_t dca12x = pair->GetDCA12();
            Double_t dca12y = pair->GetDCA12();
            Double_t path = pair->GetPath();

            fHM->H2("NPairsRecoFromMCWOCutsInvMassMomProton")->Fill(momPart1, pair->GetInvMass());
            fHM->H2("NPairsRecoFromMCWOCutsInvMassMomMeson")->Fill(momPart2, pair->GetInvMass());
            fHM->H2("NPairsRecoFromMCWOCutsInvMassEtaProton")->Fill(etaPart1, pair->GetInvMass());
            fHM->H2("NPairsRecoFromMCWOCutsInvMassEtaMeson")->Fill(etaPart2, pair->GetInvMass());

            fHM->H2("NPairsRecoFromMCWOCutsInvMassDCA1")->Fill(dca1, pair->GetInvMass());
            fHM->H2("NPairsRecoFromMCWOCutsInvMassDCA2")->Fill(dca2, pair->GetInvMass());
            fHM->H2("NPairsRecoFromMCWOCutsInvMassDCA12X")->Fill(dca12x, pair->GetInvMass());
            fHM->H2("NPairsRecoFromMCWOCutsInvMassDCA12Y")->Fill(dca12y, pair->GetInvMass());
            fHM->H2("NPairsRecoFromMCWOCutsInvMassDCA12XsubY")->Fill(abs(dca12x - dca12y), pair->GetInvMass());
            fHM->H2("NPairsRecoFromMCWOCutsInvMassPath")->Fill(path, pair->GetInvMass());

            CbmMCTrack* posMCTrack = (CbmMCTrack*) fMCTracks->At(pair->GetMCTrackIdPart1());
            CbmMCTrack* negMCTrack = (CbmMCTrack*) fMCTracks->At(pair->GetMCTrackIdPart2());
            if (!negMCTrack || !posMCTrack) continue;
            if ((posMCTrack->GetMotherId() != negMCTrack->GetMotherId()) || (posMCTrack->GetMotherId() < 0)) {
                continue;
            }
            CbmMCTrack* lambda = (CbmMCTrack*) fMCTracks->At(posMCTrack->GetMotherId());
            if (lambda->GetPdgCode() != 3122) {
                continue;
            }
        }

    /// RECO
    if (fParticlePair_RECO)
        for (Int_t iPair = 0; iPair < fParticlePair_RECO->GetEntriesFast(); iPair++) {
            BmnParticlePair* pair = (BmnParticlePair*) fParticlePair_RECO->UncheckedAt(iPair);

            CbmMCTrack* posPartTr = (CbmMCTrack*) fMCTracks->At(pair->GetMCTrackIdPart1());
            if (!posPartTr) {
                cout << "proton track obtaining error" << endl;
                return;
            }

            CbmMCTrack* negPartTr = (CbmMCTrack*) fMCTracks->At(pair->GetMCTrackIdPart2());
            if (!negPartTr) {
                cout << "meson track obtaining error" << endl;
                return;
            }

            if (-1 == posPartTr->GetMotherId() || -1 == negPartTr->GetMotherId() || negPartTr->GetMotherId() != posPartTr->GetMotherId())
                continue;

            CbmMCTrack* primaryPartTr = (CbmMCTrack*) fMCTracks->At(posPartTr->GetMotherId());
            if (!primaryPartTr) {
                cout << "lambda track obtaining error" << endl;
                return;
            }

            if (primaryPartTr->GetPdgCode() != 3122)
                continue;

            fNOfParticlePairsWithMatchedLambda++;
            fHM->H1("numberOfReconstructedParticlePairWithMatchedLambdaQA")->Fill(1);

            // Double_t momLambdaXMC = primaryPartTr->GetPx();
            // Double_t momLambdaYMC = primaryPartTr->GetPy();
            Double_t momLambdaZMC = primaryPartTr->GetPz();
            Double_t momLambdaMC = primaryPartTr->GetP();
            Double_t thetaMC = TMath::ACos(momLambdaZMC / momLambdaMC) * TMath::RadToDeg();
            Double_t pRapidityLMC = 0.5 * Log((momLambdaMC + momLambdaZMC) / (momLambdaMC - momLambdaZMC));
            Double_t thetaR = primaryPartTr->GetRapidity();

            fHM->H1("recNLambda_P_rec")->Fill(momLambdaMC);
            fHM->H1("recNLambda_theta_rec")->Fill(thetaMC);
            fHM->H1("recNLambda_eta_rec")->Fill(pRapidityLMC);
            fHM->H1("recNLambda_theta_r_rec")->Fill(thetaR);
            fHM->H2("recNLambda_eta_P_sim")->Fill(pRapidityLMC, momLambdaMC);

            Double_t momProtMC = posPartTr->GetP();
            Double_t momProtZMC = posPartTr->GetPz();
            Double_t pRapidityPMC = 0.5 * Log((momProtMC + momProtZMC) / (momProtMC - momProtZMC));
            Double_t thetaRProt = posPartTr->GetRapidity();

            fHM->H1("recNRecProtons_P_rec")->Fill(momProtMC);
            fHM->H1("recNRecProtons_theta_rec")->Fill(TMath::ACos(momProtZMC / momProtMC) * TMath::RadToDeg());
            fHM->H1("recNRecProtons_eta_rec")->Fill(pRapidityPMC);
            fHM->H1("recNRecProtons_theta_r_rec")->Fill(thetaRProt);
            fHM->H2("recNRecProtons_eta_P_sim")->Fill(pair->GetEtaPart1(), momProtMC);

            Double_t momMesMC = negPartTr->GetP();
            Double_t momMesZMC = negPartTr->GetPz();
            Double_t pRapidityMMC = 0.5 * Log((momMesMC + momMesZMC) / (momMesMC - momMesZMC));
            Double_t thetaRMes = negPartTr->GetRapidity();

            fHM->H1("recNRecMesons_P_rec")->Fill(momMesMC);
            fHM->H1("recNRecMesons_theta_rec")->Fill(TMath::ACos(momMesZMC / momMesMC) * TMath::RadToDeg());
            fHM->H1("recNRecMesons_eta_rec")->Fill(pRapidityMMC);
            fHM->H1("recNRecMesons_theta_r_rec")->Fill(thetaRMes);
            fHM->H2("recNRecMesons_eta_P_sim")->Fill(pair->GetEtaPart1(), momMesMC);
        }

    if (fParticlePair_RECO)
        for (Int_t iPair = 0; iPair < fParticlePair_RECO->GetEntriesFast(); iPair++) {
            BmnParticlePair* pair = (BmnParticlePair*) fParticlePair_RECO->UncheckedAt(iPair);
            Double_t momPart1 = pair->GetMomPart1();
            Double_t momPart2 = pair->GetMomPart2();
            Double_t etaPart1 = pair->GetEtaPart1();
            Double_t etaPart2 = pair->GetEtaPart2();
            Double_t dca1 = pair->GetDCA1();
            Double_t dca2 = pair->GetDCA2();
            Double_t dca12x = pair->GetDCA12();
            Double_t dca12y = pair->GetDCA12();
            Double_t path = pair->GetPath();

            // Apply cuts ...
            if (momPart1 < fMom[0][0] || momPart1 > fMom[0][1] || momPart2 < fMom[1][0] || momPart2 > fMom[1][1])
                continue;

            if (etaPart1 < fEta[0][0] || etaPart1 > fEta[0][1] || etaPart2 < fEta[1][0] || etaPart2 > fEta[1][1])
                continue;

            // Geom. cuts applied ...
            if (dca1 < fDCA[0][0] || dca1 > fDCA[0][1] || dca2 < fDCA[1][0] || dca2 > fDCA[1][1])
                continue;

            if (dca12x < fDCA12[0] || dca12x > fDCA12[1])
                continue;

            if (path < fPath[0] || path > fPath[1])
                continue;

            fNOfParticlePairs++;
            fHM->H1("numberOfReconstructedParticlePairsQA")->Fill(1);
            fHM->H1("NPairsRecoInvMass")->Fill(pair->GetInvMass());

            fHM->H2("NPairsRecoInvMassMomProton")->Fill(momPart1, pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassMomMeson")->Fill(momPart2, pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassEtaProton")->Fill(etaPart1, pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassEtaMeson")->Fill(etaPart2, pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassDCA1")->Fill(dca1, pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassDCA2")->Fill(dca2, pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassDCA12X")->Fill(dca12x, pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassDCA12Y")->Fill(dca12y, pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassDCA12XsubY")->Fill(abs(dca12x - dca12y), pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassPath")->Fill(path, pair->GetInvMass());
        }

    if (fParticlePair_RECO)
        for (Int_t iPair = 0; iPair < fParticlePair_RECO->GetEntriesFast(); iPair++) {
            BmnParticlePair* pair = (BmnParticlePair*) fParticlePair_RECO->At(iPair);

            fHM->H1("NPairsRecoInvMassWOCuts")->Fill(pair->GetInvMass());

            Double_t momPart1 = pair->GetMomPart1();
            Double_t momPart2 = pair->GetMomPart2();
            Double_t etaPart1 = pair->GetEtaPart1();
            Double_t etaPart2 = pair->GetEtaPart2();
            Double_t dca1 = pair->GetDCA1();
            Double_t dca2 = pair->GetDCA2();
            Double_t dca12x = pair->GetDCA12();
            Double_t dca12y = pair->GetDCA12();
            Double_t path = pair->GetPath();

            fHM->H2("NPairsRecoInvMassMomProtonWOCuts")->Fill(momPart1, pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassMomMesonWOCuts")->Fill(momPart2, pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassEtaProtonWOCuts")->Fill(etaPart1, pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassEtaMesonWOCuts")->Fill(etaPart2, pair->GetInvMass());

            fHM->H2("NPairsRecoInvMassDCA1WOCuts")->Fill(dca1, pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassDCA2WOCuts")->Fill(dca2, pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassDCA12XWOCuts")->Fill(dca12x, pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassDCA12YWOCuts")->Fill(dca12y, pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassDCA12XsubYWOCuts")->Fill(abs(dca12x - dca12y), pair->GetInvMass());
            fHM->H2("NPairsRecoInvMassPathWOCuts")->Fill(path, pair->GetInvMass());
        }
}

void BmnLambdaQa::Finish() {
    fHM->WriteToFile();

    vector <TClonesArray*> isPairBranches;
    isPairBranches.push_back(fParticlePair_MC);
    isPairBranches.push_back(fParticlePair_RECO);

    BmnParticlePairsInfo* pairInfo = new BmnParticlePairsInfo();
    pairInfo->setMomPart1Min(fMom[0][0]);
    pairInfo->setMomPart1Max(fMom[0][1]);
    pairInfo->setMomPart2Min(fMom[1][0]);
    pairInfo->setMomPart2Max(fMom[1][1]);

    pairInfo->setEtaPart1Min(fEta[0][0]);
    pairInfo->setEtaPart1Max(fEta[0][1]);
    pairInfo->setEtaPart2Min(fEta[1][0]);
    pairInfo->setEtaPart2Max(fEta[1][1]);

    pairInfo->setDCAPart1Min(fDCA[0][0]);
    pairInfo->setDCAPart1Max(fDCA[0][1]);
    pairInfo->setDCAPart2Min(fDCA[1][0]);
    pairInfo->setDCAPart2Max(fDCA[1][1]);

    pairInfo->setDCA12Min(fDCA12[0]);
    pairInfo->setDCA12Max(fDCA12[1]);
    pairInfo->setPathMin(fPath[0]);
    pairInfo->setPathMax(fPath[1]);
    BmnSimulationReport* report = new BmnLambdaQaReport(fUseMCFile, fUseRecoFile, pairInfo, isPairBranches);
    report->Create(fHM, fOutputDir);
    delete report;
    delete pairInfo;
}

void BmnLambdaQa::ReadDataBranches() {
    FairRootManager* ioman = FairRootManager::Instance();

    if (!ioman)
        Fatal("Init", "BmnRootManager is not instantiated");

    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
    fGlobalTracks = (TClonesArray*) ioman->GetObject("BmnGlobalTrack");
    fSiliconPoints = (TClonesArray*) ioman->GetObject("SiliconPoint");

    fParticlePair_MC = (TClonesArray*) ioman->GetObject("ParticlePair_MC");
    fParticlePair_RECO = (TClonesArray*) ioman->GetObject("ParticlePair_RECO");

    if (fMCTracks)
        fUseMCFile = kTRUE;

    if (fGlobalTracks)
        fUseRecoFile = kTRUE;
}

void BmnLambdaQa::CreateH1(const string& name, const string& xTitle, const string& yTitle, Int_t nofBins, Double_t minBin, Double_t maxBin) {
    TH1F* h = new TH1F(name.c_str(), string(";" + xTitle + ";" + yTitle).c_str(), nofBins, minBin, maxBin);
    fHM->Add(name, h);
}

void BmnLambdaQa::CreateH2(
        const string& name,
        const string& xTitle,
        const string& yTitle,
        const string& zTitle,
        Int_t nofBinsX,
        Double_t minBinX,
        Double_t maxBinX,
        Int_t nofBinsY,
        Double_t minBinY,
        Double_t maxBinY) {
    TH2F* h = new TH2F(name.c_str(), (";" + xTitle + ";" + yTitle + ";" + zTitle).c_str(), nofBinsX, minBinX, maxBinX, nofBinsY, minBinY, maxBinY);
    fHM->Add(name, h);
}

void BmnLambdaQa::CreateNumberOfReconstructableLambdaHistograms() {
    CreateH1("simNReconstructableLambda_P_sim", "P_{sim}, GeV/c", "number of reconstructable #Lambda^{0}", fNOfBins, fLambdaPRangeMin, fLambdaPRangeMax);
    CreateH1("simNReconstructableLambda_eta_sim", "#eta_{sim}", "number of reconstructable #Lambda^{0}", fNOfBins, fLambdaEtaRangeMin, fLambdaEtaRangeMax);
    CreateH1("simNReconstructableLambda_theta_sim", "#theta_{sim}, degrees", "number of reconstructable #Lambda^{0}", fNOfBins, fLambdaThetaRangeMin, fLambdaThetaRangeMax);
    CreateH1("simNReconstructableLambda_theta_r_sim", "Y^{r}_{sim}", "number of reconstructable #Lambda^{0}", fNOfBins, fLambdaThetaRRangeMin, fLambdaThetaRRangeMax);
    CreateH1("simNReconstructableLambda_P_x_sim", "P^{x}_{sim}, GeV/c", "number of reconstructable #Lambda^{0}", fNOfBins, -2, 2);
    CreateH1("simNReconstructableLambda_P_y_sim", "P^{y}_{sim}, GeV/c", "number of reconstructable #Lambda^{0}", fNOfBins, -2, 2);
    CreateH1("simNReconstructableLambda_P_z_sim", "P^{z}_{sim}, GeV/c", "number of reconstructable #Lambda^{0}", fNOfBins, -1, 4);
}

void BmnLambdaQa::CreateNumberOfLambdaHistograms() {
    CreateH1("simNLambda_P_sim", "P_{sim}, GeV/c", "number of #Lambda^{0}", fNOfBins, fLambdaPRangeMin, fLambdaPRangeMax);
    CreateH1("simNLambda_eta_sim", "#eta_{sim}", "number of #Lambda^{0}", fNOfBins, fLambdaEtaRangeMin, fLambdaEtaRangeMax);
    CreateH1("simNLambda_theta_sim", "#theta_{sim}, degrees", "number of #Lambda^{0}", fNOfBins, fLambdaThetaRangeMin, fLambdaThetaRangeMax);
    CreateH1("simNLambda_theta_r_sim", "Y^{r}_{sim}", "number of #Lambda^{0}", fNOfBins, fLambdaThetaRRangeMin, fLambdaThetaRRangeMax);
    CreateH1("simNLambda_P_x_sim", "P^{x}_{sim}, GeV/c", "number of #Lambda^{0}", fNOfBins, -2, 2);
    CreateH1("simNLambda_P_y_sim", "P^{y}_{sim}, GeV/c", "number of #Lambda^{0}", fNOfBins, -2, 2);
    CreateH1("simNLambda_P_z_sim", "P^{z}_{sim}, GeV/c", "number of #Lambda^{0}", fNOfBins, -1, 4);
}

void BmnLambdaQa::CreateLambdaRecEfficiencyHistograms() {
    CreateH1("simLambdaRecEff_P_sim", "P_{sim}, GeV/c", "geometrical efficiency, %", fNOfBins, fLambdaPRangeMin, fLambdaPRangeMax);
    CreateH1("simLambdaRecEff_eta_sim", "#eta_{sim}", "geometrical efficiency, % efficiency, %", fNOfBins, fLambdaEtaRangeMin, fLambdaEtaRangeMax);
    CreateH1("simLambdaRecEff_theta_sim", "#theta_{sim}, degrees", "geometrical efficiency, %", fNOfBins, fLambdaThetaRangeMin, fLambdaThetaRangeMax);
    CreateH1("simLambdaRecEff_theta_r_sim", "Y^{r}_{sim}", "geometrical efficiency, %", fNOfBins, fLambdaThetaRRangeMin, fLambdaThetaRRangeMax);
    CreateH1("simLambdaRecEff_P_x_sim", "P^{x}_{sim}, GeV/c", "geometrical efficiency, %", fNOfBins, -2, 2);
    CreateH1("simLambdaRecEff_P_y_sim", "P^{y}_{sim}, GeV/c", "geometrical efficiency, %", fNOfBins, -2, 2);
    CreateH1("simLambdaRecEff_P_z_sim", "P^{z}_{sim}, GeV/c", "geometrical efficiency, %", fNOfBins, -1, 4);
}

void BmnLambdaQa::CreateTwoDimensionalRecEfficiencyHistograms() {
    CreateH2("simNReconstructableLambda_eta_P_sim", "P_{sim}, GeV/c", "#eta_{sim}", "number of #Lambda^{0}", fNOfBins, fLambdaPRangeMin, fLambdaPRangeMax, fNOfBins, fLambdaEtaRangeMin, fLambdaEtaRangeMax);
    CreateH2("simNLambda_eta_P_sim", "P_{sim}, GeV/c", "#eta_{sim}", "number of #Lambda^{0}", fNOfBins, fLambdaPRangeMin, fLambdaPRangeMax, fNOfBins, fLambdaEtaRangeMin, fLambdaEtaRangeMax);
    CreateH2("simEffRecLambda_eta_P_sim", "P_{sim}, GeV/c", "#eta_{sim}", "geometrical efficiency, %", fNOfBins, fLambdaPRangeMin, fLambdaPRangeMax, fNOfBins, fLambdaEtaRangeMin, fLambdaEtaRangeMax);
}

void BmnLambdaQa::CreateNumberOLambdaDecayProtonsHistograms() {
    CreateH1("NProtons_P_sim", "P_{sim}, GeV/c", "number of lambda decay p", fNOfBins, fProtonsPRangeMin, fProtonsPRangeMax);
    CreateH1("NProtons_eta_sim", "#eta_{sim}", "number of lambda decay p", fNOfBins, fProtonsEtaRangeMin, fProtonsEtaRangeMax);
    CreateH1("NProtons_theta_sim", "#theta_{sim}, degrees", "number of lambda decay p", fNOfBins, fProtonsThetaRangeMin, fProtonsThetaRangeMax);
    CreateH1("NProtons_theta_r_sim", "Y^{r}_{sim}", "number of lambda decay p", fNOfBins, fProtonsThetaRRangeMin, fProtonsThetaRRangeMax);
    CreateH1("NRecProtons_P_sim", "P_{sim}, GeV/c", "number of reconstructable lambda decay p", fNOfBins, fProtonsPRangeMin, fProtonsPRangeMax);
    CreateH1("NRecProtons_eta_sim", "#eta_{sim}", "number of reconstructable lambda decay p", fNOfBins, fProtonsEtaRangeMin, fProtonsEtaRangeMax);
    CreateH1("NRecProtons_theta_sim", "#theta_{sim}, degrees", "number of reconstructable lambda decay p", fNOfBins, fProtonsThetaRangeMin, fProtonsThetaRangeMax);
    CreateH1("NRecProtons_theta_r_sim", "Y^{r}_{sim}, degrees", "number of reconstructable lambda decay p", fNOfBins, fProtonsThetaRRangeMin, fProtonsThetaRRangeMax);
}

void BmnLambdaQa::CreateRecProtonsRecEfficiencyHistograms() {
    CreateH1("simRecProtonsRecEff_P_sim", "P_{sim}, GeV/c", "geometrical efficiency, %", fNOfBins, fProtonsPRangeMin, fProtonsPRangeMax);
    CreateH1("simRecProtonsRecEff_eta_sim", "#eta_{sim}", "geometrical efficiency, %", fNOfBins, fProtonsEtaRangeMin, fProtonsEtaRangeMax);
    CreateH1("simRecProtonsRecEff_theta_sim", "#theta_{sim}, degrees", "geometrical efficiency, %", fNOfBins, fProtonsThetaRangeMin, fProtonsThetaRangeMax);
    CreateH1("simRecProtonsRecEff_theta_r_sim", "Y^{r}_{sim}, degrees", "geometrical efficiency, %", fNOfBins, fProtonsThetaRRangeMin, fProtonsThetaRRangeMax);
}

void BmnLambdaQa::CreateTwoDimensionalRecProtonsRecEfficiencyHistograms() {
    CreateH2("simNRecProtons_eta_P_sim", "P_{sim}, GeV/c", "#eta_{sim}", "number of lambda decay p", fNOfBins, fProtonsEtaRangeMin, fProtonsEtaRangeMax, fNOfBins, fProtonsPRangeMin, fProtonsPRangeMax);
    CreateH2("simNReconstructableRecProtons_eta_P_sim", "P_{sim}, GeV/c", "#eta_{sim}", "number of lambda decay p", fNOfBins, fProtonsEtaRangeMin, fProtonsEtaRangeMax, fNOfBins, fProtonsPRangeMin, fProtonsPRangeMax);
    CreateH2("simEffRecRecProtons_eta_P_sim", "P_{sim}, GeV/c", "#eta_{sim}", "reconstruction efficiency, %", fNOfBins, fProtonsEtaRangeMin, fProtonsEtaRangeMax, fNOfBins, fProtonsPRangeMin, fProtonsPRangeMax);
}

void BmnLambdaQa::CreateNumberOfLambdaDecayMesonsHistograms() {
    CreateH1("NMesons_P_sim", "P_{sim}, GeV/c", "number of lambda decay #pi^{-}", fNOfBins, fMesonsPRangeMin, fMesonsPRangeMax);
    CreateH1("NMesons_eta_sim", "#eta_{sim}", "number of lambda decay #pi^{-}", fNOfBins, fMesonsEtaRangeMin, fMesonsEtaRangeMax);
    CreateH1("NMesons_theta_sim", "#theta_{sim}, degrees", "number of lambda decay #pi^{-}", fNOfBins, fMesonsThetaRangeMin, fMesonsThetaRangeMax);
    CreateH1("NMesons_theta_r_sim", "Y^{r}_{sim}, degrees", "number of lambda decay #pi^{-}", fNOfBins, fMesonsThetaRRangeMin, fMesonsThetaRRangeMax);
    CreateH1("NRecMesons_P_sim", "P_{sim}, GeV/c", "number of reconstructable lambda decay #pi^{-}", fNOfBins, fMesonsPRangeMin, fMesonsPRangeMax);
    CreateH1("NRecMesons_eta_sim", "#eta_{sim}", "number of reconstructable lambda decay #pi^{-}", fNOfBins, fMesonsEtaRangeMin, fMesonsEtaRangeMax);
    CreateH1("NRecMesons_theta_sim", "#theta_{sim}, degrees", "number of reconstructable lambda decay #pi^{-}", fNOfBins, fMesonsThetaRangeMin, fMesonsThetaRangeMax);
    CreateH1("NRecMesons_theta_r_sim", "Y^{r}_{sim}, degrees", "number of reconstructable lambda decay #pi^{-}", fNOfBins, fMesonsThetaRRangeMin, fMesonsThetaRRangeMax);
}

void BmnLambdaQa::CreateRecMesonsRecEfficiencyHistograms() {
    CreateH1("simRecMesonsRecEff_P_sim", "P_{sim}, GeV/c", "geometrical efficiency, %", fNOfBins, fMesonsPRangeMin, fMesonsPRangeMax);
    CreateH1("simRecMesonsRecEff_eta_sim", "#eta_{sim}", "geometrical efficiency, %", fNOfBins, fMesonsEtaRangeMin, fMesonsEtaRangeMax);
    CreateH1("simRecMesonsRecEff_theta_sim", "#theta_{sim}, degrees", "geometrical efficiency, %", fNOfBins, fMesonsThetaRangeMin, fMesonsThetaRangeMax);
    CreateH1("simRecMesonsRecEff_theta_r_sim", "Y^{r}_{sim}, degrees", "geometrical efficiency, %", fNOfBins, fMesonsThetaRRangeMin, fMesonsThetaRRangeMax);
}

void BmnLambdaQa::CreateTwoDimensionalRecMesonsRecEfficiencyHistograms() {
    CreateH2("simNRecMesons_eta_P_sim", "P_{sim}, GeV/c", "#eta_{sim}", "number of lambda decay #pi^{-}", fNOfBins, fMesonsEtaRangeMin, fMesonsEtaRangeMax, fNOfBins, fMesonsPRangeMin, fMesonsPRangeMax);
    CreateH2("simNReconstructableRecMesons_eta_P_sim", "P_{sim}, GeV/c", "#eta_{sim}", "number of lambda decay #pi^{-}", fNOfBins, fMesonsEtaRangeMin, fMesonsEtaRangeMax, fNOfBins, fMesonsPRangeMin, fMesonsPRangeMax);
    CreateH2("simEffRecRecMesons_eta_P_sim", "P_{sim}, GeV/c", "#eta_{sim}", "geometrical efficiency, %", fNOfBins, fMesonsEtaRangeMin, fMesonsEtaRangeMax, fNOfBins, fMesonsPRangeMin, fMesonsPRangeMax);
}

void BmnLambdaQa::CreateNumberOfReconstrcutedLambdaHistograms() {
    CreateH1("recNLambda_P_rec", "P_{sim}, GeV/c", "number of #Lambda^{0} reconstructed", fNOfBins, fLambdaPRangeMin, fLambdaPRangeMax);
    CreateH1("recNLambda_theta_rec", "#theta_{sim}, degrees", "number of #Lambda^{0} reconstructed", fNOfBins, fLambdaThetaRangeMin, fLambdaThetaRangeMax);
    CreateH1("recNLambda_eta_rec", "#eta_{sim}", "number of #Lambda^{0} reconstructed", fNOfBins, fLambdaEtaRangeMin, fLambdaEtaRangeMax);
    CreateH1("recNLambda_theta_r_rec", "Y^{r}_{sim}", "number of #Lambda^{0} reconstructed", fNOfBins, fLambdaThetaRRangeMin, fLambdaThetaRRangeMax);
}

void BmnLambdaQa::CreateReconstructionEfficiencyHistograms() {
    CreateH1("recEffLambda_P_rec", "P_{sim}, GeV/c", "reconstruction efficiency, %", fNOfBins, fLambdaPRangeMin, fLambdaPRangeMax);
    CreateH1("recEffLambda_theta_rec", "#theta_{sim}, degrees", "reconstruction efficiency, %", fNOfBins, fLambdaThetaRangeMin, fLambdaThetaRangeMax);
    CreateH1("recEffLambda_eta_rec", "#eta_{sim}", "reconstruction efficiency, %", fNOfBins, fLambdaEtaRangeMin, fLambdaEtaRangeMax);
    CreateH1("recEffLambda_theta_r_rec", "Y^{r}_{sim}", "reconstruction efficiency, %", fNOfBins, fLambdaThetaRRangeMin, fLambdaThetaRRangeMax);
}

void BmnLambdaQa::CreateTwoDimensionalRealRecEfficiencyHistograms() {
    CreateH2("recNLambda_eta_P_sim", "P_{sim}, GeV/c", "#eta_{sim}", "number of reconstructed #Lambda^{0}", fNOfBins, fLambdaEtaRangeMin, fLambdaEtaRangeMax, fNOfBins, fLambdaPRangeMin, fLambdaPRangeMax);
    CreateH2("recEffRecLambda_eta_P_sim", "P_{sim}, GeV/c", "#eta_{sim}", "efficiency, %", fNOfBins, fLambdaEtaRangeMin, fLambdaEtaRangeMax, fNOfBins, fLambdaPRangeMin, fLambdaPRangeMax);
}

void BmnLambdaQa::CreateNumberOfReconstrcutedRecProtonHistograms() {
    CreateH1("recNRecProtons_P_rec", "P_{sim}, GeV/c", "number of lambda decay p reconstructed", fNOfBins, fProtonsPRangeMin, fProtonsPRangeMax);
    CreateH1("recNRecProtons_theta_rec", "#theta_{sim}, degrees", "number of lambda decay p reconstructed", fNOfBins, fProtonsThetaRangeMin, fProtonsThetaRangeMax);
    CreateH1("recNRecProtons_eta_rec", "#eta_{sim}", "number of lambda decay p reconstructed", fNOfBins, fProtonsEtaRangeMin, fProtonsEtaRangeMax);
    CreateH1("recNRecProtons_theta_r_rec", "Y^{r}_{sim}", "number of lambda decay p reconstructed", fNOfBins, fProtonsThetaRRangeMin, fProtonsThetaRRangeMax);
}

void BmnLambdaQa::CreateRecProtonsReconstructionEfficiencyHistograms() {
    CreateH1("recEffRecProtons_P_rec", "P_{sim}, GeV/c", "reconstruction efficiency, %", fNOfBins, fProtonsPRangeMin, fProtonsPRangeMax);
    CreateH1("recEffRecProtons_theta_rec", "#theta_{sim}, degrees", "reconstruction efficiency, %", fNOfBins, fProtonsThetaRangeMin, fProtonsThetaRangeMax);
    CreateH1("recEffRecProtons_eta_rec", "#eta_{sim}", "reconstruction efficiency, %", fNOfBins, fProtonsEtaRangeMin, fProtonsEtaRangeMax);
    CreateH1("recEffRecProtons_theta_r_rec", "Y^{r}_{sim}", "reconstruction efficiency, %", fNOfBins, fProtonsThetaRRangeMin, fProtonsThetaRRangeMax);
}

void BmnLambdaQa::CreateTwoDimensionalNumberOfReconstructedProtonsHistograms() {
    CreateH2("recNRecProtons_eta_P_sim", "P_{sim}, GeV/c", "#eta_{sim}", "number of lambda decay p", fNOfBins, fProtonsEtaRangeMin, fProtonsEtaRangeMax, fNOfBins, fProtonsPRangeMin, fProtonsPRangeMax);
    CreateH2("recEffRecProtons_eta_P_sim", "P_{sim}, GeV/c", "#eta_{sim}", "efficiency, %", fNOfBins, fProtonsEtaRangeMin, fProtonsEtaRangeMax, fNOfBins, fProtonsPRangeMin, fProtonsPRangeMax);
}

void BmnLambdaQa::CreateNumberOfReconstrcutedRecMesonsHistograms() {
    CreateH1("recNRecMesons_P_rec", "P_{sim}, GeV/c", "number of lambda decay p reconstructed", fNOfBins, fMesonsPRangeMin, fMesonsPRangeMax);
    CreateH1("recNRecMesons_theta_rec", "#theta_{sim}, degrees", "number of lambda decay p reconstructed", fNOfBins, fMesonsThetaRangeMin, fMesonsThetaRangeMax);
    CreateH1("recNRecMesons_eta_rec", "#eta_{sim}", "number of lambda decay p reconstructed", fNOfBins, fMesonsEtaRangeMin, fMesonsEtaRangeMax);
    CreateH1("recNRecMesons_theta_r_rec", "Y^{r}_{sim}", "number of lambda decay p reconstructed", fNOfBins, fMesonsThetaRRangeMin, fMesonsThetaRRangeMax);
}

void BmnLambdaQa::CreateRecMesonsReconstructionEfficiencyHistograms() {
    CreateH1("recEffRecMesons_P_rec", "P_{sim}, GeV/c", "reconstruction efficiency, %", fNOfBins, fMesonsPRangeMin, fMesonsPRangeMax);
    CreateH1("recEffRecMesons_theta_rec", "#theta_{sim}, degrees", "reconstruction efficiency, %", fNOfBins, fMesonsThetaRangeMin, fMesonsThetaRangeMax);
    CreateH1("recEffRecMesons_eta_rec", "#eta_{sim}", "reconstruction efficiency, %", fNOfBins, fMesonsEtaRangeMin, fMesonsEtaRangeMax);
    CreateH1("recEffRecMesons_theta_r_rec", "Y^{r}_{sim}", "reconstruction efficiency, %", fNOfBins, fMesonsThetaRRangeMin, fMesonsThetaRRangeMax);
}

void BmnLambdaQa::CreateTwoDimensionalNumberOfReconstructedMesonsHistograms() {
    CreateH2("recNRecMesons_eta_P_sim", "P_{sim}, GeV/c", "#eta_{sim}", "number of lambda decay p", fNOfBins, fMesonsEtaRangeMin, fMesonsEtaRangeMax, fNOfBins, fMesonsPRangeMin, fMesonsPRangeMax);
    CreateH2("recEffRecMesons_eta_P_sim", "P_{sim}, GeV/c", "#eta_{sim}", "efficiency, %", fNOfBins, fMesonsEtaRangeMin, fMesonsEtaRangeMax, fNOfBins, fMesonsPRangeMin, fMesonsPRangeMax);
}

void BmnLambdaQa::CreateReconstructedParticlePairsFromMCDataHistograms() {
    CreateH1("NPairsRecoFromMCInvMass", "m_{inv}, GeV/c^2", "number of particle pairs", fNOfBins, 1.07, 1.22);
}

void BmnLambdaQa::CreateTwoDimensionalReconstructedParticlePairsFromMCDataHistograms() {
    CreateH2("NPairsRecoFromMCInvMassMomProton", "P^{p}_{sim}, GeV/c", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fProtonsPRangeMin, fProtonsPRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCInvMassEtaProton", "#eta^{p}_{sim}", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fProtonsEtaRangeMin, fProtonsEtaRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCInvMassDCA1", "DCA^{p}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA1RangeMin, fDCA1RangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCInvMassMomMeson", "P^{#pi^{-}}_{sim}, GeV/c", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fMesonsPRangeMin, fMesonsPRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCInvMassEtaMeson", "#eta^{#pi^{-}}_{sim}", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fMesonsEtaRangeMin, fMesonsEtaRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCInvMassDCA2", "DCA^{#pi^{-}}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA2RangeMin, fDCA2RangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCInvMassDCA12X", "DCA12_{X}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA12XRangeMin, fDCA12XRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCInvMassDCA12Y", "DCA12_{Y}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA12YRangeMin, fDCA12YRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCInvMassDCA12XsubY", "|DCA12_{X} - DCA12_{Y}|, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA12XRangeMin, fDCA12XRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCInvMassPath", "path, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fPathRangeMin, fPathRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
}

void BmnLambdaQa::CreateReconstructedParticlePairsFromMCDataWOCutsHistograms() {
    CreateH1("NPairsRecoFromMCWOCutsInvMass", "m_{inv}, GeV/c^2", "number of particle pairs", fNOfBins, 1.07, 1.22);
}

void BmnLambdaQa::CreateTwoDimensionalReconstructedParticlePairsFromMCDataWOCutsHistograms() {
    CreateH2("NPairsRecoFromMCWOCutsInvMassMomProton", "P^{p}_{sim}, GeV/c", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fProtonsPRangeMin, fProtonsPRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCWOCutsInvMassEtaProton", "#eta^{p}_{sim}", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fProtonsEtaRangeMin, fProtonsEtaRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCWOCutsInvMassDCA1", "DCA^{p}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA1RangeMin, fDCA1RangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCWOCutsInvMassMomMeson", "P^{#pi^{-}}_{sim}, GeV/c", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fMesonsPRangeMin, fMesonsPRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCWOCutsInvMassEtaMeson", "#eta^{#pi^{-}}_{sim}", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fMesonsEtaRangeMin, fMesonsEtaRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCWOCutsInvMassDCA2", "DCA^{#pi^{-}}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA2RangeMin, fDCA2RangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCWOCutsInvMassDCA12X", "DCA12_{X}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA12XRangeMin, fDCA12XRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCWOCutsInvMassDCA12Y", "DCA12_{Y}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA12YRangeMin, fDCA12YRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCWOCutsInvMassDCA12XsubY", "|DCA12_{X} - DCA12_{Y}|, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA12XRangeMin, fDCA12XRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoFromMCWOCutsInvMassPath", "path, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fPathRangeMin, fPathRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
}

void BmnLambdaQa::CreateNumberOfNotReconstructedLambdaHistograms() {
    CreateH1("recNNotRecLambda_P_rec", "P_{sim}, GeV/c", "number of #Lambda^{0} not reconstructed", fNOfBins, fLambdaPRangeMin, fLambdaPRangeMax);
    CreateH1("recNNotRecLambda_theta_rec", "#theta_{sim}, degrees", "number of #Lambda^{0} not reconstructed", fNOfBins, fLambdaThetaRangeMin, fLambdaThetaRangeMax);
    CreateH1("recNNotRecLambda_eta_rec", "#eta_{sim}", "number of #Lambda^{0} not reconstructed", fNOfBins, fLambdaEtaRangeMin, fLambdaEtaRangeMax);
    CreateH1("recNNotRecLambda_theta_r_rec", "Y^{r}_{sim}", "number of #Lambda^{0} not reconstructed", fNOfBins, fLambdaThetaRRangeMin, fLambdaThetaRRangeMax);
}

void BmnLambdaQa::CreateReconstructedParticlePairsHistograms() {
    CreateH1("NPairsRecoInvMass", "m_{inv}, GeV/c^2", "number of particle pairs", fNOfBins, fInvMassMin, fInvMassMax);
}

void BmnLambdaQa::CreateTwoDimensionalReconstructedParticlePairsHistograms() {
    CreateH2("NPairsRecoInvMassMomProton", "P^{p}_{reco}, GeV/c", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fProtonsPRangeMin, fProtonsPRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassEtaProton", "#eta^{p}_{reco}", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fProtonsEtaRangeMin, fProtonsEtaRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassDCA1", "DCA^{p}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA1RangeMin, fDCA1RangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassMomMeson", "P^{#pi^{-}}_{reco}, GeV/c", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fMesonsPRangeMin, fMesonsPRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassEtaMeson", "#eta^{#pi^{-}}_{reco}", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fMesonsEtaRangeMin, fMesonsEtaRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassDCA2", "DCA^{#pi^{-}}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA2RangeMin, fDCA2RangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassDCA12X", "DCA12_{X}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA12XRangeMin, fDCA12XRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassDCA12Y", "DCA12_{Y}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA12YRangeMin, fDCA12YRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassDCA12XsubY", "|DCA12_{X} - DCA12_{Y}|, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA12XRangeMin, fDCA12XRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassPath", "path, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fPathRangeMin, fPathRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
}

void BmnLambdaQa::CreateReconstructedParticlePairsWOCutsHistograms() {
    CreateH1("NPairsRecoInvMassWOCuts", "m_{inv}, GeV/c^2", "number of particle pairs", fNOfBins, fInvMassMin, fInvMassMax);
}

void BmnLambdaQa::CreateTwoDimensionalReconstructedParticlePairsWOCutsHistograms() {
    CreateH2("NPairsRecoInvMassMomProtonWOCuts", "P^{p}_{reco}, GeV/c", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fProtonsPRangeMin, fProtonsPRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassEtaProtonWOCuts", "#eta^{p}_{reco}", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fProtonsEtaRangeMin, fProtonsEtaRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassDCA1WOCuts", "DCA^{p}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA1RangeMin, fDCA1RangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassMomMesonWOCuts", "P^{#pi^{-}}_{reco}, GeV/c", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fMesonsPRangeMin, fMesonsPRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassEtaMesonWOCuts", "#eta^{#pi^{-}}_{reco}", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fMesonsEtaRangeMin, fMesonsEtaRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassDCA2WOCuts", "DCA^{#pi^{-}}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA2RangeMin, fDCA2RangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassDCA12XWOCuts", "DCA12_{X}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA12XRangeMin, fDCA12XRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassDCA12YWOCuts", "DCA12_{Y}, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA12YRangeMin, fDCA12YRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassDCA12XsubYWOCuts", "|DCA12_{X} - DCA12_{Y}|, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fDCA12XRangeMin, fDCA12XRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
    CreateH2("NPairsRecoInvMassPathWOCuts", "path, cm", "m_{inv}, GeV/c^{2}", "number of particle pairs", fNOfBins, fPathRangeMin, fPathRangeMax, fNOfBins, fInvMassMin, fInvMassMax);
}

ClassImp(BmnLambdaQa);
