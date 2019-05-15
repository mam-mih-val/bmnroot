/** BmnTGeoNavigator.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 **/

#include "BmnGeoNavigator.h"
#include "TMath.h"
#include "FairTrackParam.h"

#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoMaterial.h"
#include "TGeoVolume.h"

#include <iostream>
#include <cmath>

using namespace TMath;

BmnGeoNavigator::BmnGeoNavigator() {
}

BmnGeoNavigator::~BmnGeoNavigator() {
}

BmnStatus BmnGeoNavigator::FindIntersections(const FairTrackParam* par, Float_t zOut, vector<BmnMaterialInfo>& inter) {
    Bool_t downstream = zOut <= par->GetZ();
//     std::cout << "zOut=" << zOut << " Z=" << par->GetZ() << " downstream=" << downstream << std::endl;

    InitTrack(par, downstream);
    if (gGeoManager->IsOutside()) {
        return kBMNERROR;
    }

    BmnMaterialInfo stepInfo;
    Bool_t last = kFALSE;

    cout.precision(7);

    do {
        gGeoManager->PushPoint();
        stepInfo = MakeStep();
        //      std::cout << "stepInfo=" << stepInfo.ToString();
        // Check if outside of the geometry
        if (gGeoManager->IsOutside()) {
            //         std::cout << "Error! BmnTGeoNavigator::FindIntersections: Outside geometry.\n";
            gGeoManager->PopDummy();
            return kBMNERROR;
        }
        // Check for NaN values
        if (IsNaN(gGeoManager->GetCurrentPoint()[0]) ||
                IsNaN(gGeoManager->GetCurrentPoint()[1]) ||
                IsNaN(gGeoManager->GetCurrentPoint()[2])) {
            //         std::cout << "Error! BmnTGeoNavigator::FindIntersections: NaN values.\n";
            gGeoManager->PopDummy();
            return kBMNERROR;
        }
        // Check if we currently at the output position
        Bool_t away = (!downstream) ? stepInfo.GetZpos() >= zOut : stepInfo.GetZpos() <= zOut;
        if (away) { //|| gGeoManager->IsNullStep()){
            gGeoManager->PopPoint();
            Float_t l = CalcLength(zOut);
            stepInfo.SetLength(l);
            stepInfo.SetZpos(zOut);
            last = kTRUE;
        } else {
            gGeoManager->PopDummy();
        }
        inter.push_back(stepInfo);
    } while (!last);
    return kBMNSUCCESS;
}

void BmnGeoNavigator::InitTrack(const FairTrackParam* par, Bool_t downstream) const {
    Double_t nz = 1. / TMath::Sqrt(par->GetTx() * par->GetTx() + par->GetTy() * par->GetTy() + 1);
    
    Double_t nx = par->GetTx() * nz;
    Double_t ny = par->GetTy() * nz;
    
    // Change track direction for upstream
    if (downstream) {
        nx = -nx;
        ny = -ny;
        nz = -nz;
    }   
    gGeoManager->InitTrack(par->GetX(), par->GetY(), par->GetZ(), nx, ny, nz);
}

BmnMaterialInfo BmnGeoNavigator::MakeStep(Float_t step) const {
    // fill current material information and then make a step
    BmnMaterialInfo matInfo;
    TGeoMaterial* mat = gGeoManager->GetCurrentNode()->GetMedium()->GetMaterial();
    matInfo.SetRL(mat->GetRadLen());
    matInfo.SetRho(mat->GetDensity());
    matInfo.SetZ(mat->GetZ());
    matInfo.SetA(mat->GetA());
    matInfo.SetName(gGeoManager->GetCurrentNode()->GetName());

    if (step == 0.) {
        gGeoManager->FindNextBoundaryAndStep(25.);
    } else {
        gGeoManager->SetStep(step);
        gGeoManager->Step(kFALSE);
    }

    matInfo.SetLength(gGeoManager->GetStep());
    matInfo.SetZpos(gGeoManager->GetCurrentPoint()[2]);

    return matInfo;
}

Float_t BmnGeoNavigator::CalcLength(Float_t zOut) const {
    //find intersection point of straight line with plane
    Float_t nx = gGeoManager->GetCurrentDirection()[0];
    Float_t ny = gGeoManager->GetCurrentDirection()[1];
    Float_t nz = gGeoManager->GetCurrentDirection()[2];
    Float_t z = gGeoManager->GetCurrentPoint()[2];

    Float_t t0 = (zOut - z) / nz;

    Float_t dx = nx * t0;
    Float_t dy = ny * t0;
    Float_t dz = (zOut - z); //nz * t0;

    //calculate distance between two points
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}
