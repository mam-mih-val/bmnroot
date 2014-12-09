/** BmnTGeoTrackPropagator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2007-2014
 * @version 1.0
 **/

#include "BmnTrackPropagator.h"
#include "BmnDefaultSettings.h"
#include "BmnTrackExtrapolator.h"
#include "BmnGeoNavigator.h"
#include "BmnMaterialEffects.h"
#include "BmnMaterialInfo.h"
#include "BmnMath.h"
#include "BmnMatrixMath.h"

#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

Float_t MAXIMUM_PROPAGATION_STEP_SIZE = 10.;

BmnTrackPropagator::BmnTrackPropagator() {
    fExtrapolator = new BmnTrackExtrapolator();
    fNavigator = new BmnGeoNavigator();
    fMaterial = new BmnMaterialEffects();
}

BmnTrackPropagator::~BmnTrackPropagator() {
}

BmnStatus BmnTrackPropagator::TGeoTrackPropagate(FairTrackParam* par, Float_t zOut, Int_t pdg, vector<Double_t>* F, Float_t* length, TString type) { //FIXME

    if (!IsParCorrect(par)) return kBMNERROR;
    Float_t zIn = par->GetZ();
    Float_t dz = zOut - zIn; //???? 

    if (fabs(dz) < BmnDefaultSettings::MINIMUM_PROPAGATION_DISTANCE) {
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

    Int_t nofSteps = Int_t(abs(dz) / MAXIMUM_PROPAGATION_STEP_SIZE);
    Float_t stepSize;
    if (nofSteps == 0) {
        stepSize = abs(dz);
    } else {
        stepSize = MAXIMUM_PROPAGATION_STEP_SIZE;
    }
    Float_t z = zIn;

//    cout << "Z = " << zIn << " Par q/p = " << par->GetQp() << endl;
    if (length) *length = 0;
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
        for (UInt_t iMat = 0; iMat < inter.size(); iMat++) {
            BmnMaterialInfo mat = inter[iMat];

            // Check if track parameters are correct
            if (!IsParCorrect(par)) return kBMNERROR;
            vector<Double_t>* Fnew = NULL;
            if (F != NULL) Fnew = new vector<Double_t > (25, 0.);
            
            // Extrapolate to the next boundary
            if (type == "field") {                                              //FIXME type of extrapolating has to be depended on a magnetic field presence
                if (fExtrapolator->RK4TrackExtrapolate(par, mat.GetZpos(), Fnew) == kBMNERROR) { //Is it possible to return error from RK4 extrapolator???
                    return kBMNERROR; 
                }
            } else if (type == "line") {
                if (fExtrapolator->LineTrackExtrapolate(par, mat.GetZpos(), Fnew) == kBMNERROR) return kBMNERROR; //Is it possible to return error from LINE extrapolator???
            } else {
                return kBMNERROR;
            }
            
            // Update transport matrix
            if (F != NULL) UpdateF(*F, *Fnew); 
            if (Fnew != NULL) delete Fnew;

            // Add material effects
            fMaterial->Update(par, &mat, pdg, downstream);
//            cout << "Step = " << iStep << " iMat = " << iMat << " Z = " << zIn << " Par q/p = " << par->GetQp() << endl;
            if (length) *length += mat.GetLength();
        }
    }
    
    if (!IsParCorrect(par)) return kBMNERROR;
    else return kBMNSUCCESS;
}

void BmnTrackPropagator::UpdateF(vector<Double_t>& F, const vector<Double_t>& newF) {
    vector<Double_t> A(25);
    Mult25(newF, F, A);
    F.assign(A.begin(), A.end());
}

Bool_t BmnTrackPropagator::IsParCorrect(const FairTrackParam* par) {
    const Float_t maxSlope = 5.;
    const Float_t minSlope = 1e-10;
    const Float_t maxQp = 1000.; // p = 10 MeV

    if (abs(par->GetTx()) > maxSlope || abs(par->GetTy()) > maxSlope || abs(par->GetTx()) < minSlope || abs(par->GetTy()) < minSlope || abs(par->GetQp()) > maxQp) return kFALSE;
    if (isnan(par->GetX()) || isnan(par->GetY()) || isnan(par->GetTx()) || isnan(par->GetTy()) || isnan(par->GetQp())) return kFALSE;

    return kTRUE;
}
