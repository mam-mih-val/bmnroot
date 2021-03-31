/*************************************************************************************
 *
 *         Class BmnHodoGeoPar
 *         
 ************************************************************************************/

#ifndef BMNGEOHODOPAR_H
#define BMNGEOHODOPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnHodoGeoPar : public FairParGenericSet {
   public:
    TObjArray *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
    TObjArray *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

    BmnHodoGeoPar(const char *name = "BmnHodoGeoPar", const char *title = "Hodo Geometry Parameters", const char *context = "HodoDefaultContext");
    ~BmnHodoGeoPar(void);
    void clear(void);
    void putParams(FairParamList *);
    Bool_t getParams(FairParamList *);
    TObjArray *GetGeoSensitiveNodes() { return fGeoSensNodes; }
    TObjArray *GetGeoPassiveNodes() { return fGeoPassNodes; }

    ClassDef(BmnHodoGeoPar, 0)
};

#endif
