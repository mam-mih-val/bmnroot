/** @file CbmStsSetup.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 27.05.2013
 **/


#ifndef CBMSTSSETUP_H
#define CBMSTSSETUP_H 1

#include "CbmStsElement.h"

class TGeoManager;


/** @class CbmStsSetup
 ** @brief Class representing the top level of the STS setup
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** The CbmStsSetup is a singleton class and represents the
 ** interface to the CBM geometry and its elements.
 **/

class CbmStsSetup : public CbmStsElement
{

  public:


    CbmStsSetup();


    virtual ~CbmStsSetup() { };


    static const char* GetLevelName(Int_t level) {
      if ( level < 0 || level >= kStsNofLevels ) return "";
      return (fgkLevelName[level]).Data();
    }


    Bool_t Init(TGeoManager* geo);


    static CbmStsSetup* Instance();



  private:

    static CbmStsSetup* fgInstance;

    static const TString fgkLevelName[kStsNofLevels];


    ClassDef(CbmStsSetup,1);

};

#endif /* CBMSTSSETUP_H */
