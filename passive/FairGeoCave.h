/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
#ifndef FAIRGEOCAVE_H
#define FAIRGEOCAVE_H

#include "FairGeoSet.h"   // for FairGeoSet
#include "FairGeoMedia.h" // for FairGeoMedia

#include <TString.h>   // for TString
#include <iosfwd>      // for fstream

class FairGeoCave : public FairGeoSet
{
  protected:
    TString name;

  public:
    FairGeoCave();
    ~FairGeoCave() {}
    const char* getModuleName(Int_t) { return name.Data(); }
    Bool_t read(std::fstream&, FairGeoMedia*);
    void addRefNodes();
    void write(std::fstream&);
    void print();

    ClassDef(FairGeoCave, 0);   // Class for the geometry of CAVE
};

#endif /* !FAIRGEOCAVE_H */
