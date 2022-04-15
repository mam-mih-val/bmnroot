/** @file CbmStsSensorPoint.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 08.05.2013
 **/

#ifndef CBMSTSSENSORPOINT_H
#define CBMSTSSENSORPOINT_H 1


#include "Rtypes.h"

/** @class CbmStsSensorPoint
 ** @brief Container class for a local point in a STS sensor
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** Stores parameters needed for digitisation in the sensor C.S.
 **/


class CbmStsSensorPoint
{

  public:

    /** Default constructor  **/
    CbmStsSensorPoint();


    /** Constructor with parameters
     ** @param x1,y1  Entry coordinates in local C.S. [cm]
     ** @param x2,y2  Exit coordinates in local C.S. [cm]
     ** @param eLoss  Energy deposit [GeV]
     ** @param time   Time [ns]
     **/
    CbmStsSensorPoint(Double_t x1, Double_t y1,
                      Double_t x2, Double_t y2,
                      Double_t eLoss, Double_t time);

    /** Destructor  **/
    virtual ~CbmStsSensorPoint();


    /** Accessors **/
    Double_t GetX1()    const { return fX1; }
    Double_t GetY1()    const { return fY1; }
    Double_t GetX2()    const { return fX2; }
    Double_t GetY2()    const { return fY2; }
    Double_t GetELoss() const { return fELoss; }
    Double_t GetTime()  const { return fTime; }



  private:

    Double_t fX1;      ///< Entry point x [cm]
    Double_t fY1;      ///< Entry point y [cm]
    Double_t fX2;      ///< Exit point x [cm]
    Double_t fY2;      ///< Exit point y [cm]
    Double_t fELoss;   ///< Energy deposit [GeV]
    Double_t fTime;    ///< Time [ns]

    ClassDef(CbmStsSensorPoint,1);

};

#endif /* CBMSTSSENSORPOINT_H */
