// -------------------------------------------------------------------------
// -----                      UniGeoConverter header file            -----
// -----                  Created 04/08/15  by K. Gertsenberger        -----
// -------------------------------------------------------------------------

/** UniGeoConverter.h
 *@author K.Gertsenberger <gertsen@jinr.ru>
 **
 ** Class for convertion from detector geometry to DB data
 **/

#ifndef UNIGEOCONVERTER_H
#define UNIGEOCONVERTER_H 1

#include "TString.h"
#include "TGeoNode.h"
#include "TSQLServer.h"

#include "UniConnection.h"

enum dbShapeName{dbGeoBox, dbGeoPgon, dbGeoPcon, dbGeoTube, dbGeoTubeSeg, dbGeoSphere, dbGeoTorus, dbGeoCone, dbGeoConeSeg,
                 dbGeoTrd1, dbGeoTrd2, dbGeoTrap, dbGeoComb, dbGeoPara, dbGeoArb8, dbGeoEltu, dbGeoXtru, dbGeoShapeAssembly};

// 100 and next values - dynamic arrays or a set of vertices
enum dbParameterType{dbParameterCenterX, dbParameterCenterY, dbParameterCenterZ, dbParameterDXhalf, dbParameterDYhalf, dbParameterDZhalf,
                     dbParameterRmin, dbParameterRmax, dbParameterPhi1, dbParameterPhi2, dbParameterDphi, dbParameterNedges, dbParameterNz,
                     dbParameterTheta1, dbParameterTheta2, dbParameterRmin2, dbParameterRmax2, dbParameterDXhalf2, dbParameterDYhalf2,
                     dbParameterAlpha1, dbParameterAlpha2, dbParameterR, dbParameterBl1, dbParameterBl2, dbParameterTl1, dbParameterTl2,
                     dbParameterH1, dbParameterH2};


class UniGeoConverter
{
 private:
    UniGeoConverter();

    static int RecursiveNodeChanging(TSQLServer* db_server, TGeoNode* node, int parent_node_id);

 public:
    virtual ~UniGeoConverter(); // Destructor

    // static methods
    static int WriteRootGeoToDB(TString geo_file_path);

  ClassDef(UniGeoConverter,1);
};

#endif
