// @(#)bmnroot/macro/miscellaneous:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2018-05-20

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SRC_EventCounter                                                           //
//                                                                            //
// A useful macro to visualize number of collected events (RUN7)              //
// to be used for physics analysis in form of chart pies as a function of     //
// projectile, energy, target                                                 //
// The  BM@N UniDb used                                                       //
////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <Rtypes.h>
#include <TString.h>
#include <set>
#include <cfloat>
#include <TCanvas.h>
#include <TH2.h>
#include <TPie.h>
#include <TPieSlice.h>
#include <TMath.h>

#include "../run/bmnloadlibs.C"

using namespace std;
using namespace TMath;

void SRC_EventCounter(Int_t run_period = 7) {
    // ----  Load libraries   --------------------------------------------------
    bmnloadlibs(); // load BmnRoot libraries

    // SRC file numbers ...
    const Int_t kStart = 2041;
    const Int_t kFinish = 3589;

    set <Double_t> energies;
    set <TString> targets;
    set <TString> beams;

    for (Int_t iRun = kStart; iRun < kFinish; iRun++) {
        UniRun* pCurrentRun = UniRun::GetRun(run_period, iRun);

        if (pCurrentRun == NULL) {
            delete pCurrentRun;
            continue;
        }

        TString* targStar = pCurrentRun->GetTargetParticle();
        Double_t* eneStar = pCurrentRun->GetEnergy();
        Double_t* apprCurrentStar = pCurrentRun->GetFieldVoltage();

        if (targStar == NULL || eneStar == NULL || apprCurrentStar == NULL)
            continue;

        TString beamPart = pCurrentRun->GetBeamParticle();

        energies.insert(*eneStar);
        targets.insert(*targStar);
        beams.insert(beamPart);
    }

    TCanvas* c = new TCanvas("c", "c", 800, 400);
    c->Divide(energies.size(), beams.size());
    TPie * charts[beams.size()][energies.size()];

    const Int_t nProjectiles = 1; //beams.size();
    TString PROJECTILES[nProjectiles] = {"C"};

    const Int_t nEnergies = 1; //energies.size();
    const Double_t ENERGIES[nEnergies] = {3.17};

    const Int_t nTargs = 5; //targets.size();
    const TString TARGETS[nTargs] = {"H2", "Pb", "SRC Lead 1", "SRC Lead 2", "SRC Lead 3"};

    Long_t nEvs[beams.size()][energies.size()][nTargs]; // [] proj, [] ene, [] nEvs per each target
    for (Int_t iProj = 0; iProj < beams.size(); iProj++)
        for (Int_t iEne = 0; iEne < energies.size(); iEne++)
            for (Int_t iTarg = 0; iTarg < nTargs; iTarg++)
                nEvs[iProj][iEne][iTarg] = 0;

    // Loop over projectiles ...
    for (auto itBeam : beams) {
        for (auto itEne : energies) {
            for (Int_t iRun = kStart; iRun < kFinish; iRun++) {
                UniRun* pCurrentRun = UniRun::GetRun(run_period, iRun);

                if (pCurrentRun == NULL) {
                    delete pCurrentRun;
                    continue;
                }

                TString* targStar = pCurrentRun->GetTargetParticle();
                Double_t* eneStar = pCurrentRun->GetEnergy();
                Double_t* apprCurrentStar = pCurrentRun->GetFieldVoltage();

                if (targStar == NULL || eneStar == NULL || apprCurrentStar == NULL)
                    continue;

                // Get only data to be used for physics ...
                if (*apprCurrentStar < FLT_EPSILON)
                    continue;

                TString proj = pCurrentRun->GetBeamParticle();

                if (proj != itBeam || *eneStar != itEne)
                    continue;

                // Get target ...
                TString targ = *pCurrentRun->GetTargetParticle();

                // Set indices ... 
                Int_t idxBeam = (proj == PROJECTILES[0]) ? 0 : -1;

                Int_t idxEne = (Abs(*eneStar - ENERGIES[0]) < FLT_EPSILON) ? 0 : -1;

                Int_t idxTarg = -1;
                for (Int_t iTarg = 0; iTarg < nTargs; iTarg++)
                    if (targ == TARGETS[iTarg]) {
                        idxTarg = iTarg;
                        break;
                    }

                nEvs[idxBeam][idxEne][idxTarg] += *pCurrentRun->GetEventCount();
            }
        }
    }

    for (Int_t iProj = 0; iProj < beams.size(); iProj++)
        for (Int_t iEne = 0; iEne < energies.size(); iEne++)
            for (Int_t iTarg = 0; iTarg < nTargs; iTarg++)
                cout << iProj << " " << iEne << " " << iTarg << " " << nEvs[iProj][iEne][iTarg] / 1e6 << " MEvs" << endl;

    Int_t iPad = 1;
    for (Int_t iProj = 0; iProj < beams.size(); iProj++)
        for (Int_t iEne = 0; iEne < energies.size(); iEne++) {
            Double_t pieContent[nTargs];
            Int_t colors[nTargs];
            for (Int_t iTarg = 0; iTarg < nTargs; iTarg++) {
                pieContent[iTarg] = nEvs[iProj][iEne][iTarg] / 1e6;
                colors[iTarg] = 2 + iTarg;
            }

            Double_t pieContentSum = 0.;
            for (Int_t iTarg = 0; iTarg < nTargs; iTarg++)
                pieContentSum += pieContent[iTarg];

            charts[iProj][iEne] = new TPie(Form("Projectile %s (T =  %G GeV/n)", PROJECTILES[iProj].Data(), ENERGIES[iEne]),
                    Form("Projectile %s (T =  %G GeV/n)", PROJECTILES[iProj].Data(), ENERGIES[iEne]), nTargs, pieContent, colors);
            c->cd(iPad);
            for (Int_t iTarg = 0; iTarg < nTargs; iTarg++) {
                TPieSlice* slice = charts[iProj][iEne]->GetSlice(iTarg);
                Double_t valSlice = slice->GetValue();
                TString sliceTitle = (valSlice > FLT_EPSILON) ? TARGETS[iTarg] : "";
                if (valSlice < FLT_EPSILON )
                    slice->SetRadiusOffset(DBL_MAX);
                slice->SetTitle(Form("%s", sliceTitle.Data()));
            }

            charts[iProj][iEne]->SetRadius(.25);
            charts[iProj][iEne]->SetLabelsOffset(.06);
            charts[iProj][iEne]->SetAngularOffset(-75.);
            charts[iProj][iEne]->SetTextSize(0.05);
                charts[iProj][iEne]->SetLabelFormat("#splitline{%txt (%perc)}{%val MEvs}");
            if (pieContentSum > FLT_EPSILON) {
                //charts[iProj][iEne]->SetY(.75);
                charts[iProj][iEne]->Draw("3d nol");
            }

            iPad++;
        }
    c->SaveAs("SRC_physEvents_Run7.pdf");
}
