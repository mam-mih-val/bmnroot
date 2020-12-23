#include <TH2F.h>
#include <TCanvas.h>
#include <TString.h>
#include <Rtypes.h>
#include <TStyle.h>
#include <TPaletteAxis.h>
#include <TList.h>
#include <TGraph.h>
#include <TClonesArray.h>

using namespace std;

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

Double_t FirstDerivative(BmnFieldMap*, Double_t, Double_t, Double_t);
Double_t SecondDerivative(BmnFieldMap*, Double_t, Double_t, Double_t);

Double_t GetX(BmnFieldMap*, Int_t);
Double_t GetY(BmnFieldMap*, Int_t);
Double_t GetZ(BmnFieldMap*, Int_t);

void SmoothByLine(Double_t&, Double_t&, Double_t, Double_t, Double_t, Double_t);

class mapNodes : public TObject {
public:

    mapNodes() {
        nX = -1;
        nY = -1;
        nZ = -1;

        nXs.resize(0);
        nYs.resize(0);
        nZs.resize(0);

        toBeSmoothed.resize(0);
    }

    virtual ~mapNodes() {
        ;
    }

public:
    Int_t nX;
    Int_t nY;
    Int_t nZ;

    vector <Int_t> nXs;
    vector <Int_t> nYs;
    vector <Int_t> nZs;

    vector <Bool_t> toBeSmoothed;

    ClassDef(mapNodes, 1)
};

void smoothMap(TString in = "", TString out = "") {
    gStyle->SetOptStat(0);

    BmnFieldMap* magField = new BmnNewFieldMap(in);
    magField->SetScale(1.);
    magField->Init();

    Int_t node = 0;

    map <Double_t, Int_t> xValueNode; // xValue --> node number
    map <Double_t, Int_t> yValueNode; // yValue --> node number
    map <Double_t, Int_t> zValueNode; // zValue --> node number

    for (Int_t iX = magField->GetXmin(); iX < magField->GetXmax() + 1; iX += magField->GetXstep()) {
        xValueNode[(Double_t) iX] = node;
        node++;
    }

    node = 0;

    for (Int_t iY = magField->GetYmin(); iY < magField->GetYmax() + 1; iY += magField->GetYstep()) {
        yValueNode[(Double_t) iY] = node;

        node++;
    }

    node = 0;

    for (Int_t iZ = magField->GetZmin(); iZ < magField->GetZmax() + 1; iZ += magField->GetZstep()) {
        zValueNode[(Double_t) iZ] = node;
        node++;
    }

    map <Double_t, Double_t> dBdx_fixedYZ[magField->GetNy()][magField->GetNz()];
    for (Int_t iNodeY = 0; iNodeY < magField->GetNy(); iNodeY++)
        for (Int_t iNodeZ = 0; iNodeZ < magField->GetNz(); iNodeZ++)
            dBdx_fixedYZ[iNodeY][iNodeZ].clear();

    map <Double_t, Double_t> d2Bdx2_fixedYZ[magField->GetNy()][magField->GetNz()];
    for (Int_t iNodeY = 0; iNodeY < magField->GetNy(); iNodeY++)
        for (Int_t iNodeZ = 0; iNodeZ < magField->GetNz(); iNodeZ++)
            d2Bdx2_fixedYZ[iNodeY][iNodeZ].clear();

    for (Int_t iY = magField->GetYmin(); iY < magField->GetYmax() + 1; iY += magField->GetYstep()) {
        for (Int_t iZ = magField->GetZmin(); iZ < magField->GetZmax() + 1; iZ += magField->GetZstep()) {

            map <Double_t, Double_t> tmp1;
            map <Double_t, Double_t> tmp2;

            for (Int_t iX = magField->GetXmin(); iX < magField->GetXmax() + 1; iX += magField->GetXstep()) {
                tmp1[(Double_t) iX] = FirstDerivative(magField, iX, iY, iZ);
                tmp2[(Double_t) iX] = SecondDerivative(magField, iX, iY, iZ);
            }
            dBdx_fixedYZ[yValueNode.find(iY)->second][zValueNode.find(iZ)->second] = tmp1;
            d2Bdx2_fixedYZ[yValueNode.find(iY)->second][zValueNode.find(iZ)->second] = tmp2;
        }
    }
    
    TClonesArray* nodesInfo = new TClonesArray("mapNodes");

    for (Int_t iNodeY = 0; iNodeY < magField->GetNy(); iNodeY++)
        for (Int_t iNodeZ = 0; iNodeZ < magField->GetNz(); iNodeZ++) {

            mapNodes* nodeYZ = new ((*nodesInfo)[nodesInfo->GetEntriesFast()]) mapNodes();
            nodeYZ->nY = iNodeY;
            nodeYZ->nZ = iNodeZ;

            // Looking for X, where the second derivative is deviated strongly ...
            map <Double_t, Double_t> der2 = d2Bdx2_fixedYZ[iNodeY][iNodeZ];

            // Calculating abs. ave. value of the second derivative ...
            Double_t ave = 0;
            Int_t usedEntries = 0;

            for (auto it : der2) {
                if (TMath::Abs(it.first) > 110.)
                    continue;

                ave += TMath::Abs(it.second);

                usedEntries++;
            }

            ave /= usedEntries;

            // Comparing current values with the average calculated ...
            for (auto it : der2) {
                if (TMath::Abs(it.first) > 110.)
                    continue;

                nodeYZ->nXs.push_back(xValueNode.find(it.first)->second);

                if (TMath::Abs(it.second) > TMath::Abs(ave))
                    nodeYZ->toBeSmoothed.push_back(kTRUE);
                else
                    nodeYZ->toBeSmoothed.push_back(kFALSE);
            }

            if (nodeYZ->toBeSmoothed.size() != nodeYZ->nXs.size())
                throw;
        }

    TClonesArray* newFieldValues = new TClonesArray("smoothedValues");

    for (Int_t iNodeInfo = 0; iNodeInfo < nodesInfo->GetEntriesFast(); iNodeInfo++) {
        mapNodes* node = (mapNodes*) nodesInfo->UncheckedAt(iNodeInfo);

        // Looking for values to be smoothed ...
        Double_t Y = GetY(magField, node->nY);
        Double_t Z = GetZ(magField, node->nZ);

        vector <Double_t> X;

        for (Int_t iNodeX = 0; iNodeX < node->nXs.size(); iNodeX++) {

            if (!node->toBeSmoothed[iNodeX])
                continue;

            X.push_back(GetX(magField, node->nXs[iNodeX]));
        }

        vector <vector < Double_t>> ranges;

        vector <Double_t> tmp;
        for (auto x0 = X.begin(); x0 < X.end(); x0++) {
            auto x1 = next(x0, 1);

            Double_t X0 = *x0;
            Double_t X1 = *x1;

            if (TMath::Abs(X1 - X0) == magField->GetXstep()) {
                if (tmp.size() != 0)
                    tmp.pop_back();
                tmp.push_back(X0);
                tmp.push_back(X1);
            } else {
                tmp.push_back(X0);
                ranges.push_back(tmp);
                tmp.resize(0);
            }
        }

        for (auto v : ranges) {
            Double_t l = v.front() - magField->GetXstep();
            Double_t r = v.back() + magField->GetXstep();

            Double_t fL = magField->GetBy(l, Y, Z);
            Double_t fR = magField->GetBy(r, Y, Z);

            Double_t k = 0., b = 0.;
            SmoothByLine(k, b, l, fL, r, fR);
            // cout << k << " -- " << b << endl;

            if (v.size() > 1)
                v.pop_back();

            for (auto it : v) {
                smoothedValues* newValue = new ((*newFieldValues)[newFieldValues->GetEntriesFast()]) smoothedValues();
                // cout << it << " From map# " << magField->GetBy(it, Y, Z) << " Smoothed# " << k * it + b << endl;
                newValue->X = it;
                newValue->Y = Y;
                newValue->Z = Z;

                newValue->fieldValue = k * it + b;
            }
        }
    }

    BmnSP41FieldMapCreator* smoothedMap = new BmnSP41FieldMapCreator(in, out);
    smoothedMap->SmoothMap(newFieldValues);

    delete smoothedMap;
    delete magField;
}

Double_t FirstDerivative(BmnFieldMap* map, Double_t x, Double_t Y, Double_t Z) {
    const Double_t h = map->GetXstep();

    if (TMath::Abs(x - map->GetXmin()) < FLT_EPSILON || TMath::Abs(x - map->GetXmax()) < FLT_EPSILON)
        return 0.;

    Double_t x1 = x;
    Double_t y1 = map->GetBy(x1, Y, Z);

    Double_t x0 = x1 - h;
    Double_t y0 = map->GetBy(x0, Y, Z);

    Double_t x2 = x1 + h;
    Double_t y2 = map->GetBy(x2, Y, Z);

    Double_t der = (1. / 2 * h * h) * ((x1 - x2) * y0 - 2 * (2 * x1 - x0 - x2) * y1 + (x1 - x0) * y2);

    return der;
}

Double_t SecondDerivative(BmnFieldMap* map, Double_t x, Double_t Y, Double_t Z) {
    const Double_t h = map->GetXstep();

    if (TMath::Abs(x - map->GetXmin()) < FLT_EPSILON || TMath::Abs(x - map->GetXmax()) < FLT_EPSILON)
        return 0.;

    Double_t x1 = x;
    Double_t y1 = map->GetBy(x1, Y, Z);

    Double_t x0 = x1 - h;
    Double_t y0 = map->GetBy(x0, Y, Z);

    Double_t x2 = x1 + h;
    Double_t y2 = map->GetBy(x2, Y, Z);

    Double_t der = (1. / h * h) * (y0 - 2 * y1 + y2);

    return der;
}

Double_t GetZ(BmnFieldMap* map, Int_t node) {
    return map->GetZmin() + node * map->GetZstep();
}

Double_t GetY(BmnFieldMap* map, Int_t node) {
    return map->GetYmin() + node * map->GetYstep();
}

Double_t GetX(BmnFieldMap* map, Int_t node) {
    return map->GetXmin() + node * map->GetXstep();
}

void SmoothByLine(Double_t& k, Double_t& b, Double_t x0, Double_t y0, Double_t x1, Double_t y1) {
    k = (y1 - y0) / (x1 - x0);

    b = y1 - k * x1;
}

