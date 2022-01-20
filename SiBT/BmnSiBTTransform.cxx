#include "BmnSiBTTransform.h"

#include "Math/RotationX.h"
#include "Math/RotationY.h"
#include "Math/RotationZ.h"

#include "TMath.h"

BmnSiBTTransform::BmnSiBTTransform() {

}

BmnSiBTTransform::~BmnSiBTTransform() {

}

//needed to delete before the real fucking -------------------------------------
void BmnSiBTTransform::PrintTest() {
    for(Int_t iStation = 0; iStation < transform_stack.size(); ++iStation) {
        cout << " iStation[" << iStation << "]:\n";

        for(Int_t iModule = 0; iModule < transform_stack[iStation].size(); ++iModule) {
            cout << "   iModule[" << iModule << "]:\n";

            for(Int_t iTransform = 0; iTransform < transform_stack[iStation][iModule].size(); ++iTransform) {
                cout << "     iTransform[" << iTransform << "]:\n";

                Transform3D transform = transform_stack[iStation][iModule][iTransform];

                Rotation3D rotation = transform.Rotation();
                Translation3D translation = transform.Translation();

                Double_t xx, xy, xz, yx, yy, yz, zx, zy, zz;
                Double_t dx, dy, dz;

                rotation.GetComponents(xx, xy, xz, yx, yy, yz, zx, zy, zz);
                translation.GetComponents(dx, dy, dz);

                cout << "       xx = " << xx << "\n";
                cout << "       xy = " << xy << "\n";
                cout << "       xz = " << xz << "\n";
                cout << "\n";
                cout << "       yx = " << yx << "\n";
                cout << "       yy = " << yy << "\n";
                cout << "       yz = " << yz << "\n";
                cout << "\n";
                cout << "       zx = " << zx << "\n";
                cout << "       zy = " << zy << "\n";
                cout << "       zz = " << zz << "\n";
                cout << "\n";
                cout << "       dx = " << dx << "\n";
                cout << "       dy = " << dy << "\n";
                cout << "       dz = " << dz << "\n";

                Double_t traceR = xx + yy + zz;

                Double_t theta = TMath::ACos((traceR-1)*0.5)*TMath::RadToDeg();

                cout << "theta = " << theta << "\n";

                Double_t cosToX = xx/TMath::Sqrt(xx*xx + yx*yx + zx*zx);
                Double_t angleXdeg = TMath::ACos(cosToX)*TMath::RadToDeg();
                cout << "angleXdeg = " << angleXdeg << "\n";

                Double_t cosToY = yy/TMath::Sqrt(xy*xy + yy*yy + zy*zy);
                Double_t angleYdeg = TMath::ACos(cosToY)*TMath::RadToDeg();
                cout << "angleYdeg = " << angleYdeg << "\n";

                Double_t cosToZ = zz/TMath::Sqrt(xz*xz + yz*yz + zz*zz);
                Double_t angleZdeg = TMath::ACos(cosToZ)*TMath::RadToDeg();
                cout << "angleZdeg = " << angleZdeg << "\n";


                Plane3D::Vector vec1(xx, yx, zx);
                Plane3D::Vector vec2(xy, yy, zy);
                Plane3D::Vector vec3(xz, yz, zz);

                cout << "vec1.Theta() = " << vec1.Theta()*TMath::RadToDeg() << "\n";
                cout << "vec2.Theta() = " << vec2.Theta()*TMath::RadToDeg() << "\n";
                cout << "vec3.Theta() = " << vec3.Theta()*TMath::RadToDeg() << "\n";

                cout << "vec1.Mag2() = " << vec1.Mag2() << "\n";
                cout << "vec2.Mag2() = " << vec2.Mag2() << "\n";
                cout << "vec3.Mag2() = " << vec3.Mag2() << "\n";
            }
        }
    }
}
//------------------------------------------------------------------------------

void BmnSiBTTransform::Reset() {
    transform_stack.clear();
}

Bool_t BmnSiBTTransform::LoadFromXMLFile(TString xml_config_file) {

    Bool_t testVerboseOut = false;

    //clear all previous transformations before loading new
    Reset();

    TDOMParser *parser = new TDOMParser();
    parser->SetValidate(false);

    Int_t parse_status = parser->ParseFile(xml_config_file);
    if(parse_status != 0) {
        std::cerr << "Error: An error occured when parsing the file! (in BmnSiBTTransform)\n";
        return false;
    }

    TXMLNode *stationSet_node = parser->GetXMLDocument()->GetRootNode();

    if( strcmp(stationSet_node->GetNodeName(), "StationSet") != 0 ) {
        std::cerr << "Error: Incorrect name of the root element! (in BmnSiBTStationSet)\n";
        return false;
    }

    TXMLNode *station_node = stationSet_node->GetChildren();

    Int_t station_cnt = 0;
    while(station_node) {
        if( strcmp(station_node->GetNodeName(), "Station") == 0 ) {
            transform_stack.push_back(vector<vector<Transform3D>>());
            TXMLNode *module_node = station_node->GetChildren();

            Int_t module_cnt = 0;
            while(module_node) {
                if( strcmp(module_node->GetNodeName(), "Module") == 0 ) {
                    transform_stack[station_cnt].push_back(vector<Transform3D>());
                    TXMLNode *transform_node = module_node->GetChildren();

                    Int_t transform_cnt = 0;
                    while(transform_node) {
                         if( strcmp(transform_node->GetNodeName(), "Transform") == 0 ) {
                            TString rotationOrder = "ZYX"; //default order
                            Double_t xRotationDeg = 0.0;
                            Double_t yRotationDeg = 0.0;
                            Double_t zRotationDeg = 0.0;
                            Double_t xTranslation = 0.0;
                            Double_t yTranslation = 0.0;
                            Double_t zTranslation = 0.0;

                            //getting information from the current transform
                            if( transform_node->HasAttributes() ) {
                                TList *attrList = transform_node->GetAttributes();
                                TXMLAttr *attr = 0;
                                TIter next(attrList);

                                while( attr = (TXMLAttr*)next() ) {
                                    if( strcmp(attr->GetName(), "rotOrder") == 0 ) {
                                        rotationOrder = attr->GetValue();
                                    }
                                    if( strcmp(attr->GetName(), "xRotationDeg") == 0 ) {
                                        xRotationDeg = atof(attr->GetValue());
                                    }
                                    if( strcmp(attr->GetName(), "yRotationDeg") == 0 ) {
                                        yRotationDeg = atof(attr->GetValue());
                                    }
                                    if( strcmp(attr->GetName(), "zRotationDeg") == 0 ) {
                                        zRotationDeg = atof(attr->GetValue());
                                    }
                                    if( strcmp(attr->GetName(), "xTranslation") == 0 ) {
                                        xTranslation = atof(attr->GetValue());
                                    }
                                    if( strcmp(attr->GetName(), "yTranslation") == 0 ) {
                                        yTranslation = atof(attr->GetValue());
                                    }
                                    if( strcmp(attr->GetName(), "zTranslation") == 0 ) {
                                        zTranslation = atof(attr->GetValue());
                                    }
                                }
                            }

                            if(testVerboseOut) {
                                cout << "current_station = " << station_cnt << "\n";
                                cout << "  current_module = " << module_cnt << "\n";
                                cout << "    current_transform = " << transform_cnt << "\n";
                                cout << "      rotationOrder = " << rotationOrder << "\n";
                                cout << "      xRotationDeg = " << xRotationDeg << "\n";
                                cout << "      yRotationDeg = " << yRotationDeg << "\n";
                                cout << "      zRotationDeg = " << zRotationDeg << "\n";
                                cout << "      xTranslation = " << xTranslation << "\n";
                                cout << "      yTranslation = " << yTranslation << "\n";
                                cout << "      zTranslation = " << zTranslation << "\n";
                                cout << "\n";
                            }

                            RotationX  rotX(xRotationDeg*TMath::DegToRad());
                            RotationY  rotY(yRotationDeg*TMath::DegToRad());
                            RotationZ  rotZ(zRotationDeg*TMath::DegToRad());
                            Rotation3D rotation;
                            //rotation = rotX*rotY*rotZ;

                            for(Int_t iaxis = 0; iaxis < rotationOrder.Sizeof(); ++iaxis) {
                                if(rotationOrder[iaxis] == 'X' || rotationOrder[iaxis] == 'x') rotation *= rotX;
                                if(rotationOrder[iaxis] == 'Y' || rotationOrder[iaxis] == 'y') rotation *= rotY;
                                if(rotationOrder[iaxis] == 'Z' || rotationOrder[iaxis] == 'z') rotation *= rotZ;
                            }


                            Translation3D translation(xTranslation, yTranslation, zTranslation);
                            Transform3D transform(rotation, translation);

                            //adding the current transform into the vector
                            transform_stack[station_cnt][module_cnt].push_back(transform);

                            transform_cnt++;
                         }
                        transform_node = transform_node->GetNextNode();
                    }
                    module_cnt++;
                }
                module_node = module_node->GetNextNode();
            }
            station_cnt++;
        }
        station_node = station_node->GetNextNode();
    }
    delete parser;

    return true;
}

Plane3D::Point BmnSiBTTransform::ApplyTransforms(Plane3D::Point point, Int_t station, Int_t module) {
    Plane3D::Point tpoint = point; //transformed point

    for(auto it = transform_stack.at(station).at(module).cbegin();  it != transform_stack.at(station).at(module).cend(); it++) {
        tpoint = (*it)(tpoint);
    }
    return tpoint;
}

Plane3D::Point BmnSiBTTransform::ApplyInverseTransforms(Plane3D::Point point, Int_t station, Int_t module) {
    Plane3D::Point tpoint = point; //transformed point

    for(auto it = transform_stack.at(station).at(module).crbegin();  it != transform_stack.at(station).at(module).crend(); it++) {
        tpoint = (*it).Inverse()(tpoint);
    }
    return tpoint;
}

//------------------------------------------------------------------------------

ClassImp(BmnSiBTTransform)