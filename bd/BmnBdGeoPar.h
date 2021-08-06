#ifndef BMNBDGEOPAR_H
#define BMNBDGEOPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnBdGeoPar : public FairParGenericSet
{
  public:
    BmnBdGeoPar(const char* name = "BmnBdGeoPar", const char* title = "BD Geometry Parameters",
                const char* context = "BdDefaultContext");
    ~BmnBdGeoPar() {}

    void clear();
    void putParams(FairParamList*);
    Bool_t getParams(FairParamList*);
    TObjArray      *GetGeoSensitiveNodes() {return fGeoSensNodes;}
    TObjArray      *GetGeoPassiveNodes() {return fGeoPassNodes;}

  private:
    TObjArray      *fGeoSensNodes; /** List of FairGeoNodes for sensitive  volumes */
    TObjArray      *fGeoPassNodes; /** List of FairGeoNodes for passive  volumes */

    ClassDef(BmnBdGeoPar, 1)
};

#endif /* !BMNBDGEOPAR_H */
