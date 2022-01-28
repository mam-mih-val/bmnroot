/*************************************************************************************
 *
 *         Class BmnFHCalGeoPar
 *
 ************************************************************************************/

#ifndef BMNGEOFHCALPAR_H
#define BMNGEOFHCALPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnFHCalGeoPar : public FairParGenericSet {
public:
    TObjArray* fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
    TObjArray* fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

    BmnFHCalGeoPar(const char* name = "BmnFHCalGeoPar", const char* title = "FHCal Geometry Parameters", const char* context = "FHCalDefaultContext");
    ~BmnFHCalGeoPar(void);
    void clear(void);
    void putParams(FairParamList*);
    Bool_t getParams(FairParamList*);
    TObjArray* GetGeoSensitiveNodes() { return fGeoSensNodes; }
    TObjArray* GetGeoPassiveNodes() { return fGeoPassNodes; }

    ClassDef(BmnFHCalGeoPar, 0)
};

#endif
