// -------------------------------------------------------------------------
// -----                      UniGeoConverter cxx file               -----
// -----                  Created 04/08/15  by K. Gertsenberger        -----
// -------------------------------------------------------------------------
#include "UniGeoConverter.h"

#include <TSQLStatement.h>
#include "TGeoShape.h"
#include "TGeoBBox.h"
#include "TGeoTube.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TGeoCone.h"
#include "TGeoTrd1.h"
#include "TGeoTrd2.h"
#include "TGeoArb8.h"
#include "TGeoEltu.h"
#include "TGeoPara.h"
#include "TGeoTorus.h"
#include "TGeoManager.h"
#include "TKey.h"
#include "TFile.h"

#include <iostream>
using namespace std;

// 0 - exit with error if it doesn't exist in DB, 1 - create in DB, 2 - skip this component
#define Create_New_Component 1

// -----   Constructor with DB connection   -------------------------------
UniGeoConverter::UniGeoConverter()
{
}
// -------------------------------------------------------------------------

// hierarchical writing geometry data to database
int UniGeoConverter::RecursiveNodeChanging(TSQLServer* db_server, TGeoNode* node, int parent_node_id)
{
    for(int i = 0; i < node->GetNdaughters(); i++)
    {
        TGeoNode* child = node->GetDaughter(i);
        TGeoVolume* curVolume = child->GetVolume();
        if (parent_node_id == -1)
            cout<<"Geometry for detector "<<curVolume->GetName()<<" is writing..."<<endl;

        // INSERT MEDIUM INFORMATION

        // check media existence in DataBase
        TGeoMedium* pMedium = child->GetMedium();
        TString medium_name = pMedium->GetName();
        medium_name.ToLower();
        //cout<<"Medium name: "<<medium_name<<endl;

        TString strStatement = TString::Format("SELECT media_name "
                                               "FROM geometry_media "
                                               "WHERE lower(media_name) = \"%s\"", medium_name.Data());
        TSQLStatement* stmt_select = uni_db->Statement(strStatement);

        // process media selection
        TSQLStatement* stmt_insert;
        if (stmt_select->Process())
        {
            // store result of statement in buffer
            stmt_select->StoreResult();

            // if there is no this medium, should add it to DB
            if (!stmt_select->NextResultRow())
            {
                TGeoMaterial* pMaterial = pMedium->GetMaterial();
                // adding new media to DB
                strStatement = TString::Format("INSERT INTO geometry_media(media_name, density, radiation_length, interaction_length, sensitivity_flag, "
                                               "field_flag, field_max, ang_deviation_max, step_max, energy_loss_max, b_crossing_precision, step_min, media_desc) "
                                               "VALUES (\"%s\", %f, %f, %f, %d, %d, %f, %f, %f ,%f, %f, %f, null)",
                                               pMedium->GetName(), pMaterial->GetDensity(), pMaterial->GetRadLen(), pMaterial->GetIntLen(),
                                               (int)pMedium->GetParam(0), (int)pMedium->GetParam(1), pMedium->GetParam(2), pMedium->GetParam(3),
                                               pMedium->GetParam(4), pMedium->GetParam(5), pMedium->GetParam(6), pMedium->GetParam(7));
                stmt_insert = uni_db->Statement(strStatement);

                // process inserting the new medium
                if (!stmt_insert->Process())
                {
                    cout<<"Error: insertion of new medium has been failed!"<<endl;

                    delete stmt_insert;
                    return -20;
                }

                delete stmt_insert;

                // adding medium component to DB
                if (pMaterial->IsMixture())
                {
                    stmt_insert = uni_db->Statement("INSERT INTO media_element(media_name, atomic_weight, atomic_number, relative_weight, Natoms) "
                                                    "VALUES (?, ?, ?, ?, ?)");

                    TGeoMixture* pMixture = (TGeoMixture*) pMaterial;
                    for (int j = 0; j < pMaterial->GetNelements(); j++)
                    {

                        if (stmt_insert->NextIteration())
                        {
                            stmt_insert->SetString(0, pMedium->GetName());
                            stmt_insert->SetDouble(1, (pMixture->GetAmixt())[j]);
                            stmt_insert->SetDouble(2, (pMixture->GetZmixt())[j]);
                            stmt_insert->SetDouble(3, (pMixture->GetWmixt())[j]);
                        }
                    }

                    // process inserting medium component
                    if (!stmt_insert->Process())
                    {
                        cout<<"Error: insertion of medium components (for mixture) has been failed!"<<endl;

                        delete stmt_insert;
                        return -21;
                    }

                    delete stmt_insert;
                }
                else
                {
                    strStatement = TString::Format("INSERT INTO media_element(media_name, atomic_weight, atomic_number, relative_weight, Natoms) "
                                                   "VALUES (\"%s\", %f, %f, null, null)", pMedium->GetName(), pMaterial->GetA(), pMaterial->GetZ());
                    stmt_insert = uni_db->Statement(strStatement);

                    // process inserting medium component
                    if (!stmt_insert->Process())
                    {
                        cout<<"ERROR: inserting medium component has been failed"<<endl;

                        delete stmt_insert;
                        return -22;
                    }

                    delete stmt_insert;
                }
            }// end: if there is no this medium, should add it to DB
            else
            {
                if (pMedium->GetName() != stmt_select->GetString(0))
                    pMedium->SetName(stmt_select->GetString(0));

                delete stmt_select;
            }
        }// end: process media selection
        else
        {
            cout<<"ERROR: selecting geometry_media has been failed"<<endl;

            delete stmt_select;
            return -10;
        }

        // DEFINE VOLUME SHAPE
        dbShapeName curShape;
        TGeoShape* pShape = curVolume->GetShape();
        TString strShapeClass = pShape->ClassName();

        if (pShape->TestShapeBit(TGeoShape::kGeoPgon))         curShape = dbGeoPgon;
        else if (pShape->TestShapeBit(TGeoShape::kGeoPcon))    curShape = dbGeoPcon;
        else if (pShape->TestShapeBit(TGeoShape::kGeoTubeSeg)) curShape = dbGeoTubeSeg;
        else if (pShape->TestShapeBit(TGeoShape::kGeoTube))    curShape = dbGeoTube;
        else if (pShape->TestShapeBit(TGeoShape::kGeoConeSeg)) curShape = dbGeoConeSeg;
        else if (pShape->TestShapeBit(TGeoShape::kGeoCone))    curShape = dbGeoCone;
        else if (pShape->TestShapeBit(TGeoShape::kGeoSph))     curShape = dbGeoSphere;
        else if (pShape->TestShapeBit(TGeoShape::kGeoTorus))   curShape = dbGeoTorus;
        else if (pShape->TestShapeBit(TGeoShape::kGeoTrd1))    curShape = dbGeoTrd1;
        else if (pShape->TestShapeBit(TGeoShape::kGeoTrd2))    curShape = dbGeoTrd2;
        else if (pShape->TestShapeBit(TGeoShape::kGeoTrap))    curShape = dbGeoTrap;
        else if (pShape->TestShapeBit(TGeoShape::kGeoPara))    curShape = dbGeoPara;
        else if (pShape->TestShapeBit(TGeoShape::kGeoComb))    curShape = dbGeoComb;
        else if (pShape->TestShapeBit(TGeoShape::kGeoArb8))    curShape = dbGeoArb8;
        else if (pShape->TestShapeBit(TGeoShape::kGeoEltu))    curShape = dbGeoEltu;
        else if (pShape->TestShapeBit(TGeoShape::kGeoXtru))    curShape = dbGeoXtru;
        else if (pShape->TestShapeBit(TGeoShape::kGeoBox))
        {
            if (strShapeClass == "TGeoShapeAssembly")
                curShape = dbGeoShapeAssembly;
            else
                curShape = dbGeoBox;
        }
        else
        {
            cout<<"Error: geometric shape has not been defined: "<<strShapeClass<<endl;
            return -1;
        }

        /*dbShapeName curClassShape;
        if (strShapeClass == "TGeoBBox")            curClassShape = dbGeoBox;
        else if (strShapeClass == "TGeoPgon")       curClassShape = dbGeoPgon;
        else if (strShapeClass == "TGeoPcon")       curClassShape = dbGeoPcon;
        else if (strShapeClass == "TGeoSphere")     curClassShape = dbGeoSpere;
        else if (strShapeClass == "TGeoTube")       curClassShape = dbGeoTube;
        else if (strShapeClass == "TGeoTubeSeg")    curClassShape = dbGeoTubeSeg;
        else if (strShapeClass == "TGeoTorus")      curClassShape = dbGeoTorus;
        else if (strShapeClass == "TGeoCone")       curClassShape = dbGeoCone;
        else if (strShapeClass == "TGeoConeSeg")    curClassShape = dbGeoConeSeg;
        else if (strShapeClass == "TGeoTrd1")       curClassShape = dbGeoTrd1;
        else if (strShapeClass == "TGeoTrd2")       curClassShape = dbGeoTrd2;
        else if (strShapeClass == "TGeoTrap")       curClassShape = dbGeoTrap;
        else if (strShapeClass == "TGeoCompositeShape") curClassShape = dbGeoComb;
        else if (strShapeClass == "TGeoPara")       curClassShape = dbGeoPara;
        else if (strShapeClass == "TGeoArb8")       curClassShape = dbGeoArb8;
        else if (strShapeClass == "TGeoEltu")       curClassShape = dbGeoEltu;
        else if (strShapeClass == "TGeoXtru")       curClassShape = dbGeoXtru;
        else if (strShapeClass == "TGeoShapeAssembly")  curClassShape = dbGeoShapeAssembly;
        else
        {
            cout<<"Error: geometric shape has not been defined: "<<strShapeClass<<endl;
            pShape->Dump();
            return -1;
        }

        if (curShape != curClassShape)
        {
            cout<<"Error: geometric shape has not been defined correctly: "<<strShapeClass<<endl;
            return -1;
        }*/

        // INSERT NODE INFORMATION

        // if there is main detector (parent_id == -1)
        if (parent_node_id == -1)
        {
            // search for the corresponding detector in DB (component_name)
            TString component_name = curVolume->GetName();
            component_name.ToLower();
            strStatement = TString::Format("SELECT component_name "
                                           "FROM component "
                                           "WHERE lower(component_name) = \"%s\"", component_name.Data());
            stmt_select = uni_db->Statement(strStatement);

            // process selection
            if (stmt_select->Process())
            {
                // store result of statement in buffer
                stmt_select->StoreResult();

                // if there is no this component in DB
                if (!stmt_select->NextResultRow())
                {
                    switch (Create_New_Component)
                    {
                        // exit with error
                        case 0:
                        {
                            cout<<"ERROR: there is no such component in the database: "<<curVolume->GetName()<<endl;

                            delete stmt_select;
                            return -2;
                        }
                        // skip this component
                        case 2:
                        {
                            delete stmt_select;
                            continue;
                        }
                        // create new component in DB
                        case 1:
                        {
                            // adding new component to DB
                            strStatement = TString::Format("INSERT INTO component(component_name, manufacturer_name, responsible_person, description) "
                                                           "VALUES (\"%s\", null, null, null)",
                                                           curVolume->GetName());
                            stmt_insert = uni_db->Statement(strStatement);

                        // process inserting the new medium
                        if (!stmt_insert->Process())
                        {
                            cout<<"Error: insertion of new component in DB has been failed!"<<endl;

                            delete stmt_insert;
                            return -23;
                        }

                        delete stmt_insert;
                        }
                    }// switch (Create_New_Component)
                }
                // if the component exist
                else
                {
                    if (curVolume->GetName() != stmt_select->GetString(0))
                        curVolume->SetName(stmt_select->GetString(0));

                    delete stmt_select;
                }
            }
            else
            {
                cout<<"ERROR: selecting of the component has been failed"<<endl;

                delete stmt_select;
                return -11;
            }

            strStatement = TString::Format("INSERT INTO geometry_node(node_name, parent_node_id, media_name, shape_index) "
                                            "VALUES (\"%s\", null, \"%s\", %d)", child->GetName(), pMedium->GetName(), (int)curShape);
            stmt_insert = uni_db->Statement(strStatement);

            // process inserting geometry node
            if (!stmt_insert->Process())
            {
                cout<<"ERROR: inserting main geometry node has been failed"<<endl;

                delete stmt_insert;
                return -24;
            }

            delete stmt_insert;
        }
        else
        {
            strStatement = TString::Format("INSERT INTO geometry_node(node_name, parent_node_id, media_name, shape_index) "
                                           "VALUES (\"%s\", %d, \"%s\", %d)", child->GetName(), parent_node_id, pMedium->GetName(), (int)curShape);
            stmt_insert = uni_db->Statement(strStatement);

            // process inserting geometry node
            if (!stmt_insert->Process())
            {
                cout<<"ERROR: inserting child geometry node has been failed"<<endl;

                delete stmt_insert;
                return -25;
            }

            delete stmt_insert;
        }

        // storing last inserted node ID
        int last_id = parent_node_id;
        TSQLStatement* stmt_last = db_server->Statement("SELECT LAST_INSERT_ID()");
        // process getting last id
        if (stmt_last->Process())
        {
            // store result of statement in buffer
            stmt_last->StoreResult();

            // if there is no last id then exit with error
            if (!stmt_last->NextResultRow())
            {
                cout<<"ERROR: there is no last ID in the database"<<endl;

                delete stmt_last;
                return -3;
            }
            else
            {
                last_id = stmt_last->GetInt(0);

                delete stmt_last;
            }
        }
        else
        {
            cout<<"ERROR: getting the last ID has been failed!"<<endl;

            delete stmt_last;
            return -4;
        }

        // STORING SHAPE PARAMETERS
        stmt_insert = uni_db->Statement("INSERT INTO geometry_parameter(node_id, parameter_type, parameter_value) "
                                        "VALUES (?, ?, ?)");

        switch (curShape)
        {
            case dbGeoBox:
            {
                TGeoBBox* pBox = (TGeoBBox*) pShape;
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterCenterX);
                    stmt_insert->SetDouble(2, pBox->GetOrigin()[0]);
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterCenterY);
                    stmt_insert->SetDouble(2, pBox->GetOrigin()[1]);
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterCenterZ);
                    stmt_insert->SetDouble(2, pBox->GetOrigin()[2]);
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDXhalf);
                    stmt_insert->SetDouble(2, pBox->GetDX());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDYhalf);
                    stmt_insert->SetDouble(2, pBox->GetDY());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDZhalf);
                    stmt_insert->SetDouble(2, pBox->GetDZ());
                }

                break;
            }// case dbGeoBox
            case dbGeoTube:
            {
                TGeoTube* pTube = (TGeoTube*) pShape;
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmin);
                    stmt_insert->SetDouble(2, pTube->GetRmin());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmax);
                    stmt_insert->SetDouble(2, pTube->GetRmax());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDZhalf);
                    stmt_insert->SetDouble(2, pTube->GetDz());
                }

                break;
            }// case dbGeoTube
            case dbGeoTubeSeg:
            {
                TGeoTubeSeg* pTubeSeg = (TGeoTubeSeg*) pShape;
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmin);
                    stmt_insert->SetDouble(2, pTubeSeg->GetRmin());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmax);
                    stmt_insert->SetDouble(2, pTubeSeg->GetRmax());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDZhalf);
                    stmt_insert->SetDouble(2, pTubeSeg->GetDz());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterPhi1);
                    stmt_insert->SetDouble(2, pTubeSeg->GetPhi1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterPhi2);
                    stmt_insert->SetDouble(2, pTubeSeg->GetPhi2());
                }

                break;
            }// case dbGeoTubeSeg
            case dbGeoPgon:
            {
                TGeoPgon* pPgon = (TGeoPgon*) pShape;
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterPhi1);
                    stmt_insert->SetDouble(2, pPgon->GetPhi1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDphi);
                    stmt_insert->SetDouble(2, pPgon->GetDphi());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterNedges);
                    stmt_insert->SetDouble(2, pPgon->GetNedges());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterNz);
                    stmt_insert->SetDouble(2, pPgon->GetNz());
                }
                for (int j = 0; j < pPgon->GetNz(); j++)
                {
                    if (stmt_insert->NextIteration())
                    {
                        stmt_insert->SetInt(0, last_id);
                        stmt_insert->SetInt(1, 100+3*j);
                        stmt_insert->SetDouble(2, pPgon->GetZ()[j]);
                    }
                    if (stmt_insert->NextIteration())
                    {
                        stmt_insert->SetInt(0, last_id);
                        stmt_insert->SetInt(1, 101+3*j);
                        stmt_insert->SetDouble(2, pPgon->GetRmin()[j]);
                    }
                    if (stmt_insert->NextIteration())
                    {
                        stmt_insert->SetInt(0, last_id);
                        stmt_insert->SetInt(1, 102+3*j);
                        stmt_insert->SetDouble(2, pPgon->GetRmax()[j]);
                    }
                }

                break;
            }// case dbGeoPgon
            case dbGeoPcon:
            {
                TGeoPcon* pPcon = (TGeoPcon*) pShape;
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterPhi1);
                    stmt_insert->SetDouble(2, pPcon->GetPhi1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDphi);
                    stmt_insert->SetDouble(2, pPcon->GetDphi());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterNz);
                    stmt_insert->SetDouble(2, pPcon->GetNz());
                }
                for (int j = 0; j < pPcon->GetNz(); j++)
                {
                    if (stmt_insert->NextIteration())
                    {
                        stmt_insert->SetInt(0, last_id);
                        stmt_insert->SetInt(1, 100+3*j);
                        stmt_insert->SetDouble(2, pPcon->GetZ()[j]);
                    }
                    if (stmt_insert->NextIteration())
                    {
                        stmt_insert->SetInt(0, last_id);
                        stmt_insert->SetInt(1, 101+3*j);
                        stmt_insert->SetDouble(2, pPcon->GetRmin()[j]);
                    }
                    if (stmt_insert->NextIteration())
                    {
                        stmt_insert->SetInt(0, last_id);
                        stmt_insert->SetInt(1, 102+3*j);
                        stmt_insert->SetDouble(2, pPcon->GetRmax()[j]);
                    }
                }

                break;
            }// case dbGeoPcon
            case dbGeoSphere:
            {
                TGeoSphere* pSphere = (TGeoSphere*) pShape;
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmin);
                    stmt_insert->SetDouble(2, pSphere->GetRmin());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmax);
                    stmt_insert->SetDouble(2, pSphere->GetRmax());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterTheta1);
                    stmt_insert->SetDouble(2, pSphere->GetTheta1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterTheta2);
                    stmt_insert->SetDouble(2, pSphere->GetTheta2());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterPhi1);
                    stmt_insert->SetDouble(2, pSphere->GetPhi1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterPhi2);
                    stmt_insert->SetDouble(2, pSphere->GetPhi2());
                }

                break;
            }// case dbGeoSphere
            case dbGeoCone:
            {
                TGeoCone* pCone = (TGeoCone*) pShape;
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmin);
                    stmt_insert->SetDouble(2, pCone->GetRmin1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmax);
                    stmt_insert->SetDouble(2, pCone->GetRmax1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmin2);
                    stmt_insert->SetDouble(2, pCone->GetRmin2());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmax2);
                    stmt_insert->SetDouble(2, pCone->GetRmax2());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDZhalf);
                    stmt_insert->SetDouble(2, pCone->GetDz());
                }

                break;
            }// case dbGeoCone
            case dbGeoConeSeg:
            {
                TGeoConeSeg* pConeSeg = (TGeoConeSeg*) pShape;
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmin);
                    stmt_insert->SetDouble(2, pConeSeg->GetRmin1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmax);
                    stmt_insert->SetDouble(2, pConeSeg->GetRmax1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmin2);
                    stmt_insert->SetDouble(2, pConeSeg->GetRmin2());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmax2);
                    stmt_insert->SetDouble(2, pConeSeg->GetRmax2());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDZhalf);
                    stmt_insert->SetDouble(2, pConeSeg->GetDz());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterPhi1);
                    stmt_insert->SetDouble(2, pConeSeg->GetPhi1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterPhi2);
                    stmt_insert->SetDouble(2, pConeSeg->GetPhi2());
                }

                break;
            }// case dbGeoConeSeg
            case dbGeoTrd1:
            {
                TGeoTrd1* pTrd1 = (TGeoTrd1*) pShape;
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDXhalf);
                    stmt_insert->SetDouble(2, pTrd1->GetDx1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDXhalf2);
                    stmt_insert->SetDouble(2, pTrd1->GetDx2());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDYhalf);
                    stmt_insert->SetDouble(2, pTrd1->GetDy());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDZhalf);
                    stmt_insert->SetDouble(2, pTrd1->GetDz());
                }

                break;
            }// case dbGeoTrd1
            case dbGeoTrd2:
            {
                TGeoTrd2* pTrd2 = (TGeoTrd2*) pShape;
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDXhalf);
                    stmt_insert->SetDouble(2, pTrd2->GetDx1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDXhalf2);
                    stmt_insert->SetDouble(2, pTrd2->GetDx2());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDYhalf);
                    stmt_insert->SetDouble(2, pTrd2->GetDy1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDYhalf2);
                    stmt_insert->SetDouble(2, pTrd2->GetDy2());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDZhalf);
                    stmt_insert->SetDouble(2, pTrd2->GetDz());
                }

                break;
            }// case dbGeoTrd2
            case dbGeoArb8:
            {
                TGeoArb8* pArb8 = (TGeoArb8*) pShape;
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDZhalf);
                    stmt_insert->SetDouble(2, pArb8->GetDz());
                }
                Double_t* vert = pArb8->GetVertices();
                for (int j = 0; j < 8; j++)
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, 100+3*j);
                    stmt_insert->SetDouble(2, vert[3*j]);
                }

                break;
            }// case dbGeoArb8
            case dbGeoEltu:
            {
                TGeoEltu* pEltu = (TGeoEltu*) pShape;
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDXhalf);
                    stmt_insert->SetDouble(2, pEltu->GetA());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDYhalf);
                    stmt_insert->SetDouble(2, pEltu->GetB());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDZhalf);
                    stmt_insert->SetDouble(2, pEltu->GetDz());
                }

                break;
            }// case dbGeoEltu
            case dbGeoPara:
            {
                TGeoPara* pPara = (TGeoPara*) pShape;
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDXhalf);
                    stmt_insert->SetDouble(2, pPara->GetX());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDYhalf);
                    stmt_insert->SetDouble(2, pPara->GetY());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDZhalf);
                    stmt_insert->SetDouble(2, pPara->GetZ());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterAlpha1);
                    stmt_insert->SetDouble(2, pPara->GetAlpha());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterTheta1);
                    stmt_insert->SetDouble(2, pPara->GetTheta());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterPhi1);
                    stmt_insert->SetDouble(2, pPara->GetPhi());
                }

                break;
            }// case dbGeoPara
            case dbGeoTorus:
            {
                TGeoTorus* pTorus = (TGeoTorus*) pShape;
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterR);
                    stmt_insert->SetDouble(2, pTorus->GetR());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmin);
                    stmt_insert->SetDouble(2, pTorus->GetRmin());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterRmax);
                    stmt_insert->SetDouble(2, pTorus->GetRmax());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterPhi1);
                    stmt_insert->SetDouble(2, pTorus->GetPhi1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDphi);
                    stmt_insert->SetDouble(2, pTorus->GetDphi());
                }

                break;
            }// case dbGeoTorus
            case dbGeoTrap:
            {
                TGeoTrap* pTrap = (TGeoTrap*) pShape;
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterDZhalf);
                    stmt_insert->SetDouble(2, pTrap->GetDz());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterTheta1);
                    stmt_insert->SetDouble(2, pTrap->GetTheta());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterPhi1);
                    stmt_insert->SetDouble(2, pTrap->GetPhi());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterH1);
                    stmt_insert->SetDouble(2, pTrap->GetH1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterBl1);
                    stmt_insert->SetDouble(2, pTrap->GetBl1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterTl1);
                    stmt_insert->SetDouble(2, pTrap->GetTl1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterAlpha1);
                    stmt_insert->SetDouble(2, pTrap->GetAlpha1());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterH2);
                    stmt_insert->SetDouble(2, pTrap->GetH2());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterBl2);
                    stmt_insert->SetDouble(2, pTrap->GetBl2());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterTl2);
                    stmt_insert->SetDouble(2, pTrap->GetTl2());
                }
                if (stmt_insert->NextIteration())
                {
                    stmt_insert->SetInt(0, last_id);
                    stmt_insert->SetInt(1, dbParameterAlpha2);
                    stmt_insert->SetDouble(2, pTrap->GetAlpha2());
                }

                break;
            }// case dbGeoTrap
        }// switch (curShape)

        // process inserting geometry parameters
        if (!stmt_insert->Process())
        {
            cout<<"ERROR: inserting geometry parameters has been failed!"<<endl;

            delete stmt_insert;
            return -26;
        }

        delete stmt_insert;

        // recursive child nodes
        if (child->GetNdaughters() != 0)
            RecursiveNodeChanging(uni_db, child, last_id);
    }// for(int i = 0; i < node->GetNdaughters(); i++)

    return 0;
}

// -----   Writing detector geometry in ROOT format to database  -------------------------------
int UniGeoConverter::WriteRootGeoToDB(TString geo_file_path)
{
    if (geo_file_path == "")
    {
        cout<<"ERROR: geometry file path cannot be empty!"<<endl;
        return -1;
    }

    UniConnection* connDb = UniConnection::Open();
    if (connectionUniDb == nullptr)
    {
        cout<<"ERROR: connection to the database cannot be established"<<endl;
        return -2;
    }

    TSQLServer* db_server = connDb->GetSQLServer();

    // get detector geometry from ROOT file
    TFile* geoFile = new TFile(geo_file_path, "READ");
    if (!geoFile->IsOpen())
    {
        cout<<"ERROR: geometry file has not be opened"<<endl;
        return -3;
    }

    // get gGeoManager from file with full geometry
    TList* keyList = geoFile->GetListOfKeys();
    TIter next(keyList);
    TKey* key = (TKey*)next();
    TString className(key->GetClassName());
    if (className.BeginsWith("TGeoManager"))
    {
        //geoMan = dynamic_cast<TGeoManager*> (key->ReadObj() );
        key->ReadObj();
    }
    else
    {
        cout<<"ERROR: TGeoManager is not top element in the given file: "<<geo_file_path<<endl;
        return -4;
    }

    TGeoNode* N = gGeoManager->GetTopNode();
    int db_res = RecursiveNodeChanging(uni_db, N, -1);

    delete connectionUniDb;

    return db_res;
}


// -------------------------------------------------------------------
UniGeoConverter::~UniGeoConverter()
{
}

// -------------------------------------------------------------------
ClassImp(UniGeoConverter)
