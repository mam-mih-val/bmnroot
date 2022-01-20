#ifndef BMNSiBTTRANSFORM_H
#define	BMNSiBTTRANSFORM_H

#include "TROOT.h"
#include "Math/Plane3D.h"
#include "Math/Transform3D.h"

#include "TDOMParser.h"
#include "TXMLNode.h"
#include "TXMLAttr.h"
#include "TList.h"

#include <vector>
#include <iostream>

using namespace ROOT::Math;
using namespace std;

class BmnSiBTTransform {

public:
    //Constructor
    BmnSiBTTransform();

    //Destructor
    virtual ~BmnSiBTTransform();

    void Reset();
    Bool_t LoadFromXMLFile(TString xml_config_file);

    void PrintTest();

    Plane3D::Point ApplyTransforms(Plane3D::Point point, Int_t station, Int_t module);
    //void ApplyTransforms(Plane3D::Point &point, Int_t station, Int_t module);

    Plane3D::Point ApplyInverseTransforms(Plane3D::Point point, Int_t station, Int_t module);

private:

    vector<vector<vector<Transform3D>>> transform_stack;

    ClassDef(BmnSiBTTransform, 1);
};

#endif	/* BMNSiBTTRANSFORM_H */

