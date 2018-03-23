// -------------------------------------------------------------------------
// -----                        MpdBoxSet source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "MpdBoxSet.h"

// -----   Standard constructor   ------------------------------------------
MpdBoxSet::MpdBoxSet(MpdBoxSetDraw* drawer, const char* name, const char* t)
  : TEveBoxSet(name, t), fDraw(drawer)
{
}

ClassImp(MpdBoxSet)
