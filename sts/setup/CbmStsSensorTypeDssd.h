/** @file CbmStsSensorTypeDssd.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 02.05.2013
 **/

#ifndef CBMSTSSENSORTYPEDSSD_H
#define CBMSTSSENSORTYPEDSSD_H 1


#include "CbmStsSensorType.h"


/** @class CbmStsSensorTypeDssd
 ** @brief Class describing double-sided silicon strip sensors.
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** This class describes the response of double-sided silicon
 ** strip sensors in the STS. Its free parameters are the dimensions
 ** of the active area, the number of strips at the
 ** top (readout) edge and the stereo angles on front and back side.
 **
 ** The active area does not necessarily coincide with the geometric
 ** dimensions of the sensor. It is, however, centred in the latter,
 ** meaning that the width of inactive regions (guard ring) are
 ** the same on the left and on the right side and also the same at
 ** the top and and the bottom.
 **
 ** The stereo angle is defined with respect to the y (vertical) axis.
 ** Readout is performed at the top edge of the sensor. In case of
 ** finite stereo angle, the corner strips not reaching the top edge are
 ** connected horizontally to the corresponding strip in the other corner.
 **
 ** The response to charged particles is modelled by a uniform charge
 ** distribution along the particle trajectory in the active volume,
 ** which is projected to the readout edge, where it is discretised
 ** on the active strips. The charge is then delivered to the corresponding
 ** channel of the readout module (CbmStsModule).
 **
 ** The mapping of strip number and module channel is trivial in the case
 ** of just one sensor per module. In case of several daisy-chained sensors,
 ** the top-edge strip is vertically connected vertically to the corresponding
 ** strip on the bottom edge of the sensor above. This results in an offset
 ** of strip number to channel number which depends on the position of the'
 ** sensor in the daisy chain. The behaviour is implemented in the methods
 ** GetStrip and GetModuleChannel.
 **/



class CbmStsSensorTypeDssd : public CbmStsSensorType
{

  public:

    /** Constructor  **/
    CbmStsSensorTypeDssd();


    /** Destructor  **/
    virtual ~CbmStsSensorTypeDssd() { };


   /** Print parameters **/
    virtual void Print(Option_t* opt = "") const;


    /** Process a point **/
    virtual void ProcessPoint(CbmStsSensorPoint* point,
                              const CbmStsSenzor* sensor) const;



    /** Set the parameters
     ** @param dx,dy,dz          Size in x,y,z [cm]
     ** @param pitchF,pitchB     Strip pitch foint and back side [cm]
     ** @param stereoF,stereoB   Strip stereo angle front and back side [degrees]
     **/
    void SetParameters(Double_t dx, Double_t dy, Double_t dz,
                       Int_t nStripsF, Int_t nStripsB,
                       Double_t stereoF, Double_t stereoB);


  private:

    Double_t fDx;             ///< Dimension of active area in x [cm]
    Double_t fDy;             ///< Dimension of active area in y [cm]
    Double_t fDz;             ///< Thickness in z [cm]
    Int_t    fNofStrips[2];   ///< Number of strips on front/back side
    Double_t fStereo[2];      ///< Stereo angle front/back side [degrees]
    Bool_t   fIsSet;          ///< Flag whether parameters are set


    /** Temporary variables to avoid frequent calculations **/
    Double_t fPitch[2];     //! Strip pitch front/back side [cm]
    Double_t fCosStereo[2]; //! cos of stereo angle front/back side
    Double_t fSinStereo[2]; //! sin if stereo angle front/back side
    Int_t   fStripShift[2]; //! Shift in number of strips from bottom to top


    /** Get the readout channel in the module for a given strip and side
     ** @param strip     Strip number
     ** @param side      Side (0 = front, 1 = back)
     ** @param sensorId  Index of sensor within module
     ** @return  Channel number in module
     **/
    Int_t GetModuleChannel(Int_t strip, Int_t side, Int_t sensorId) const;


    /** Get strip and side from module channel.
     ** @param[in] channel   Channel number in module
     ** @param[in] sensorId  Sensor index in module
     ** @param[out]  strip   Strip number in sensor
     ** @param[out]  side    Sensor side [0 = front, 1 = back]
     **/
    void GetStrip(Int_t channel, Int_t sensorId, Int_t& strip, Int_t& side);


    /** Produce charge on front or back side from a CbmStsPoint
     ** @param side   0 = front, 1 = back side
     **/
    void ProduceCharge(CbmStsSensorPoint* point, Int_t side,
                       const CbmStsSenzor* sensor) const;


    /** Register produced charge in one strip
     ** @param side  0 = front, 1 = back
     ** @param strip strip number
     ** @param charge  charge in strip [e]
     ** @param time    time of registration [ns]
     **/
    void RegisterCharge(const CbmStsSenzor* sensor, Int_t side, Int_t strip,
                        Double_t charge, Double_t time) const;


    /** Test the consistent implementation of GetModuleChannel and
     ** GetStrip. The latter should be the reverse of the former.
     ** @return kTRUE if successful
     **/
    Bool_t SelfTest();



    ClassDef(CbmStsSensorTypeDssd,1);

};


#endif
