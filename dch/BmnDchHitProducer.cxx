#include "BmnDchHitProducer.h"
#include "CbmMCTrack.h"

#include "TRandom.h"
#include "TMath.h"
#include "TString.h"

#include "BmnDchPoint.h"
#include "BmnDchHit.h"

using std::cout;

BmnDchHitProducer::BmnDchHitProducer() {
    fInputBranchName = "DCHPoint";
    fOutputHitsBranchName = "BmnDchHit";

    fNActivePlanes = 8;
    fPlaneTypes = new TString[fNActivePlanes]{"va", "vb", "ua", "ub" ,"ya", "yb", "xa", "xb"};
}

BmnDchHitProducer::~BmnDchHitProducer() {
    if(fPlaneTypes) {
        delete [] fPlaneTypes;
        fPlaneTypes = nullptr;
    }
}

InitStatus BmnDchHitProducer::Init() {

    cout << " BmnDchHitProducer::Init() " << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();

    fBmnPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");

    fBmnHitsArray = new TClonesArray(fOutputHitsBranchName, 100);
    ioman->Register(fOutputHitsBranchName, "DCH", fBmnHitsArray, kTRUE);

    return kSUCCESS;
}

void BmnDchHitProducer::Exec(Option_t* opt) {

    fBmnHitsArray->Delete();

    if (!fBmnPointsArray) {
        Error("BmnDchHitProducer::Exec()", " !!! Unknown branch name !!! ");
        return;
    }

    for (Int_t iPoint = 0; iPoint < fBmnPointsArray->GetEntriesFast(); iPoint++) {

        TRandom rand_gen;
        rand_gen.SetSeed(0);

        BmnDchPoint* dchPoint = (BmnDchPoint*) fBmnPointsArray->UncheckedAt(iPoint);

        Double_t x = dchPoint->GetX();
        Double_t y = dchPoint->GetY();
        Double_t z = dchPoint->GetZ();

        Int_t currentPlaneNum = dchPoint->GetPlaneNumber();

        //X,Y GLOBAL shifts need to be set from a file for geomety creation (X,y pos.) and center
        Double_t x_shift = 0.0;
        Double_t y_shift = 0.0;

        //Convert GLOBAL->LOCAL
        Double_t x_loc = -(x - x_shift); // x-plane wires (inverted)
        Double_t y_loc = y - y_shift;    // y-plane wires
        Double_t v_loc = (x_loc + y_loc)/TMath::Sqrt2(); // v-plane wires
        Double_t u_loc = (y_loc - x_loc)/TMath::Sqrt2(); // u-plane wires

        //current position in wire units
        Double_t wire_pos = DBL_MAX; //default

        //distance from the center of DCH to the outermost wire
        Double_t owdist = (fPlaneTypes[currentPlaneNum][1] == 'a') ? 119.0 : 118.5; //119.0 and 118.0 are taken from N.Voytishin's code

        if(fPlaneTypes[currentPlaneNum][0] == 'x') wire_pos = x_loc + owdist;
        if(fPlaneTypes[currentPlaneNum][0] == 'y') wire_pos = y_loc + owdist;
        if(fPlaneTypes[currentPlaneNum][0] == 'v') wire_pos = v_loc + owdist;
        if(fPlaneTypes[currentPlaneNum][0] == 'u') wire_pos = u_loc + owdist;

        if(wire_pos > 2*owdist) continue; //ignore the point outside the wire plane

        //Int_t nearest_wire = (wire_pos - Int_t(wire_pos) < 0.5) ? Int_t(wire_pos) : Int_t(wire_pos) + 1;
        Int_t nearest_wire;
        Bool_t left = false; //pos. relative to the nearest wire (left or right)
        if(wire_pos - Int_t(wire_pos) < 0.5) {
            nearest_wire = Int_t(wire_pos);
            left = false;
        }
        else {
            nearest_wire = Int_t(wire_pos) + 1;
            left = true;
        }

        //distance from the point to the nearest wire
        Double_t wdist = TMath::Abs(wire_pos - nearest_wire);

        //sigma error dependent on the distance to the nearest wire
        Double_t sigm_err = (wdist < 0.02) ? 0.08 : (wdist >= 0.02 && wdist < 0.1) ? 0.06 : (wdist >= 0.1 && wdist < 0.4) ? 0.025 : (wdist >= 0.4 && wdist < 0.41) ? 0.08 : 0.10;

        Double_t dx1 = rand_gen.Gaus(0, sigm_err);
        Double_t dy1 = rand_gen.Gaus(0, sigm_err);
        Double_t dz1 = rand_gen.Gaus(0, 0.0);

        Double_t dx2 = rand_gen.Gaus(0, sigm_err);
        Double_t dy2 = rand_gen.Gaus(0, sigm_err);
        Double_t dz2 = rand_gen.Gaus(0, 0.0);

        //Double_t x_smeared = x + dx;
        //Double_t y_smeared = y + dy;
        //Double_t z_smeared = z + dz;

        //hit-1 with smearing
        Double_t x1_smeared = -x_loc + x_shift + dx1;
        Double_t y1_smeared = y_loc + y_shift + dy1;
        Double_t z1_smeared = z + dz1;

        //hit-2 (symmetrical) with smearing
        Double_t x2_smeared;
        Double_t y2_smeared;
        Double_t z2_smeared = z + dz2;


        if(fPlaneTypes[currentPlaneNum][0] == 'x') {
            if(left) {
                x2_smeared = -(x_loc + 2*wdist) + x_shift + dx2;
                y2_smeared = y_loc + y_shift + dy2;
            }
            else {
                x2_smeared = -(x_loc - 2*wdist) + x_shift + dx2;
                y2_smeared = y_loc + y_shift + dy2;
            }
        }

        if(fPlaneTypes[currentPlaneNum][0] == 'y') {
            if(left) {
                x2_smeared = -x_loc + x_shift + dx2;
                y2_smeared = y_loc + 2*wdist + y_shift + dy2;
            }
            else {
                x2_smeared = -x_loc + x_shift + dx2;
                y2_smeared = y_loc - 2*wdist + y_shift + dy2;
            }
        }

        if(fPlaneTypes[currentPlaneNum][0] == 'v') {
            if(left) {
                x2_smeared = -(((v_loc + 2*wdist) - u_loc)/TMath::Sqrt2()) + x_shift + dx2;
                y2_smeared = (((v_loc + 2*wdist) + u_loc)/TMath::Sqrt2()) + y_shift + dy2;
            }
            else {
                x2_smeared = -(((v_loc - 2*wdist) - u_loc)/TMath::Sqrt2()) + x_shift + dx2;
                y2_smeared = (((v_loc - 2*wdist) + u_loc)/TMath::Sqrt2()) + y_shift + dy2;
            }
        }

        if(fPlaneTypes[currentPlaneNum][0] == 'u') {
            if(left) {
                x2_smeared = -((v_loc  - (u_loc + 2*wdist))/TMath::Sqrt2()) + x_shift + dx2;
                y2_smeared = ((v_loc + (u_loc + 2*wdist))/TMath::Sqrt2()) + y_shift + dy2;
            }
            else {
                x2_smeared = -((v_loc  - (u_loc - 2*wdist))/TMath::Sqrt2()) + x_shift + dx2;
                y2_smeared = ((v_loc + (u_loc - 2*wdist))/TMath::Sqrt2()) + y_shift + dy2;
            }
        }

        /*
        //Output information for debugging
        Bool_t info_out = true;
        if(info_out) {
            cout << "(x:y:z) = ( " << x << " : " << y << " : " << z << " )\n";
            cout << "  currentPlaneNum = " << currentPlaneNum << "\n";
            cout << "  PlaneType = " << fPlaneTypes[currentPlaneNum][0] << "\n";
            cout << "  (dx1:dy1:dz1) = ( " << dx1 << " : " << dy1 << " : " << dz1 << " )\n";
            cout << "  (dx2:dy2:dz2) = ( " << dx2 << " : " << dy2 << " : " << dz2 << " )\n";
            cout << "  smeared1(x:y:z) = ( " << x1_smeared << " : " << y1_smeared << " : " << z1_smeared << " )\n";
            cout << "  smeared2(x:y:z) = ( " << x2_smeared << " : " << y2_smeared << " : " << z2_smeared << " )\n";
            cout << "  wdist = " << wdist << "\n";
            cout << "  wire_pos = " << wire_pos << "\n";
            cout << "  nearest_wire = " << nearest_wire << "\n";
            cout << "  sigm_err = " << sigm_err << "\n";
            cout << "  left = " << left << "\n";
            cout << "\n";
        }
        */

        //adding a new hit with probability 90%
        if(rand_gen.Uniform() <= 0.9) {
            new ((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()])
                   BmnDchHit(0, TVector3(x1_smeared, y1_smeared, z1_smeared), TVector3(sigm_err, sigm_err, 0.0), iPoint);

            new ((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()])
                   BmnDchHit(0, TVector3(x2_smeared, y2_smeared, z2_smeared), TVector3(sigm_err, sigm_err, 0.0), iPoint);
        }

        //delete rand_gen;
    }
}

void BmnDchHitProducer::Finish() {
}

ClassImp(BmnDchHitProducer)
