/*************************************************************************************
 *
 *         Class BmnScWallGeoPar
 *         
 ************************************************************************************/

#ifndef BMNGEOSCWALLPAR_H
#define BMNGEOSCWALLPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnScWallGeoPar : public FairParGenericSet {
   public:
    TObjArray *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
    TObjArray *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

    BmnScWallGeoPar(const char *name = "BmnScWallGeoPar", const char *title = "ScWall Geometry Parameters", const char *context = "ScWallDefaultContext");
    ~BmnScWallGeoPar(void);
    void clear(void);
    void putParams(FairParamList *);
    Bool_t getParams(FairParamList *);
    TObjArray *GetGeoSensitiveNodes() { return fGeoSensNodes; }
    TObjArray *GetGeoPassiveNodes() { return fGeoPassNodes; }

    ClassDef(BmnScWallGeoPar, 0)
};

#endif
