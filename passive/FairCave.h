/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *              GNU Lesser General Public Licence (LGPL) version 3,             *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
#ifndef CAVE_H
#define CAVE_H

#include "FairModule.h"   // for FairModule

class FairCave : public FairModule
{
  public:
    FairCave(const char* name, const char* Title = "Exp Cave");
    FairCave();
    virtual ~FairCave();
    virtual void ConstructGeometry();

    virtual FairModule* CloneModule() const;

  private:
    FairCave(const FairCave& rhs);
    Double_t world[3];

    ClassDef(FairCave, 1);
};

#endif   // CAVE_H
