/** @file CbmStsSensorType.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 02.05.2013
 **/

#ifndef CBMSTSSENSORTYPE_H
#define CBMSTSSENSORTYPE_H 1


#include "TNamed.h"

class CbmStsSenzor;
class CbmStsSensorPoint;


/** @class CbmStsSensorType
 ** @brief Abstract base class for description of the functional
 ** behaviour of sensors in the STS.
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** Concrete classes deriving from CbmStsSensorType describe the
 ** production of charge by a CbmStsSensorPoint (charged particle
 ** traversing the sensor). The must implement the abstract
 ** method ProcessPoint.
 **/



class CbmStsSensorType : public TNamed
{

  public:

    /** Constructor  **/
    CbmStsSensorType() : fTypeId(-1) { };


    /** Destructor  **/
    virtual ~CbmStsSensorType() { };


    /** Type identifier **/
    Int_t GetTypeId() const { return fTypeId; }


    /** Produce charge in the sensor
     **
     ** @param point   Pointer to CbmStsSensorPoint with relevant parameters
     **
     ** Perform the appropriate action for a particle trajectory in the
     ** sensor characterised by the CbmStsSensorPoint object.
     **/
    virtual void ProcessPoint(CbmStsSensorPoint* point,
                              const CbmStsSenzor* sensor) const = 0;


  private:

    Int_t fTypeId;  ///< Unique type identifier



  ClassDef(CbmStsSensorType,1);

};




#endif
