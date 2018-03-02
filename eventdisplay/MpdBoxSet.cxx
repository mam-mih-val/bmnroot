/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *              GNU Lesser General Public Licence (LGPL) version 3,             *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----                        MpdBoxSet source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "MpdBoxSet.h"


// -----   Standard constructor   ------------------------------------------
MpdBoxSet::MpdBoxSet(MpdBoxSetDraw* drawer, const char* name, const char* t)
  : TEveBoxSet(name, t), fDraw(drawer)
{}

ClassImp(MpdBoxSet)


