/** @file BmnSsdSensor.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 13.12.2018
 **
 ** This class is to replace the BmnSsdSensor, at which point it will be
 ** renamed to spell correctly.
 **/

#ifndef BMNSSDSENSOR_H
#define BMNSSDSENSOR_H 1


#include <vector>
#include <string>
#include "BmnSsdAddress.h"
#include "BmnSsdCluster.h"
#include "BmnSsdElement.h"
#include "BmnSsdSensorConditions.h"

class TClonesArray;
class TGeoPhysicalNode;
class BmnEvent;
class BmnLink;
class BmnSsdModule;
class BmnSsdPoint;
class BmnSsdSensorPoint;


/** @class BmnSsdSensor
 ** @brief Class representing an instance of a sensor in the BMN-SSD.
 ** @version 2.0
 **
 ** The sensor is the smallest geometric element in the SSD setup.
 ** It is the daughter node of a module, which may contain one sensor
 ** or several daisy-chained ones. The sensor class represents
 ** the physical node through its member fNode, but also performs detector
 ** response simulation through its member fType.
 ** After being instantiated by the initialisation of BmnSsdSetup,
 ** the sensor type must be assigned to the sensor. This is currently done
 ** by the digitiser task.
 ** The sensor class performs the coordinate transformation from the
 ** global system to the sensor system, having the sensor midpoint as origin.
 ** The analog response is then modelled by the pure virtual method
 ** CalulateResponse.
 **/
class BmnSsdSensor : public BmnSsdElement
{

  public:

    /** Constructor
     ** @param address Unique element address
     ** @param node    Pointer to geometry node
     ** @param mother  Pointer to mother element (module)
     **/
    BmnSsdSensor(UInt_t address = 0, TGeoPhysicalNode* node = nullptr,
                 BmnSsdElement* mother = nullptr);


    /** Destructor  **/
    virtual ~BmnSsdSensor() { };


    /** Create a new hit in the output array from two clusters
     ** @param xLocal   hit x coordinate in sensor system [cm]
     ** @param yLocal   hit y coordinate in sensor system [cm]
     ** @param varX     Variance in x [cm^2]
     ** @param varY     Variance in y [cm^2]
     ** @param varXY    Covariance of x and y [cm^2]
     ** @param clusterF pointer to front side cluster
     ** @param clusterB pointer to back side cluster
     ** @param du       Error in u coordinate (across strips front side) [cm]
     ** @param dv       Error in v coordinate (across strips back side) [cm]
     **/
    void CreateHit(Double_t xLocal, Double_t yLocal,
    		       Double_t varX, Double_t varY, Double_t varXY,
    		       BmnSsdCluster* clusterF, BmnSsdCluster* clusterB,
    		       Double_t du = 0., Double_t dv = 0.);


    /** Find hits in sensor
     ** @param clusters  Vector of clusters
     ** @param hitArray  TClonesArray to store the hits in
     ** @param event     Pointer to current event for registering of hits
     ** @param dTime     Max. time difference of clusters in a hit
     ** @return Number of created hits
     **/
    virtual Int_t FindHits(std::vector<BmnSsdCluster*>& clusters,
                           TClonesArray* hitArray, BmnEvent* event,
                           Double_t dTime) = 0;


    /** @brief Get the address from the sensor name (static)
     ** @param name Name of sensor
     ** @value Unique element address
     **/
    static UInt_t GetAddressFromName(TString name);


    /** Sensor conditions
     ** @return Pointer to sensor condition object
     **/
    const BmnSsdSensorConditions* GetConditions() const { return fConditions; }


    /** Current link object
     ** @return Pointer to current link object (to BmnSsdPoint)
     **/
    BmnLink* GetCurrentLink() const { return fCurrentLink; }


    /** Get mother module **/
    BmnSsdModule* GetModule() const;


  	/** Get physical node
  	 ** @return Pointer to TGeoPhysicalNode of sensor
     **/
  	TGeoPhysicalNode* GetNode() const {return fNode;}


  	/** @brief Initialise the sensor, if needed
  	 ** @return kTRUE is successfully initialised
  	 **
  	 ** The implementation depends on the concrete sensor class.
  	 **/
  	virtual Bool_t Init() { return kTRUE; }


    /** Get the sensor Id within the module  **/
    Int_t GetSensorId() const {
      return BmnSsdAddress::GetElementId(fAddress, kSsdSensor); }


    /** Process one MC Point
     ** @param point  Pointer to BmnSsdPoint object
     ** @return  Status variable, depends on sensor type
     **
     ** The point coordinates are converted into the internal coordinate
     ** system. The appropriate analogue response is then calculated
     ** with the pure virtual method CalculateResponse.
     **/
    Int_t ProcessPoint(const BmnSsdPoint* point,
    		               Double_t eventTime = 0., BmnLink* link = NULL);


    /** @brief Set sensor address
     ** @param address SSD element address
     **/
    void SetAddress(Int_t address) {
      fAddress = address;
      fName = BmnSsdElement::ConstructName(address, kSsdSensor);
    }


     /** Set the sensor conditions
      ** @param vFD           Full depletion voltage [V]
      ** @param vBias         Bias voltage [V]
      ** @param temperature   Temperature [K]
      ** @param cCoupling     Coupling capacitance [pF]
      ** @param cInterstrip   Inter-strip capacitance [pF]
      ** @param bX            Magn. field Bx at sensor centre [T]
      ** @param bY            Magn. field By at sensor centre [T]
      ** @param bZ            Magn. field Bz at sensor centre [T]
      **/
     void SetConditions(Double_t vFD, Double_t vBias, Double_t temperature,
                        Double_t cCoupling, Double_t cInterstrip,
                        Double_t bX, Double_t bY, Double_t bZ) {
       fConditions = new BmnSsdSensorConditions(vFD, vBias, temperature,
                                                cCoupling, cInterstrip,
                                                bX, bY, bZ);
     }


    /** @brief Set the physical node
     ** @param node  Pointer to associated TGeoPhysicalNode object
     **/
    void SetNode(TGeoPhysicalNode* node) { fNode = node; }


    /** String output **/
    virtual std::string ToString() const = 0;


  protected:

    BmnSsdSensorConditions*  fConditions;  ///< Operating conditions
    BmnLink* fCurrentLink;  ///< Link to currently processed MCPoint
    TClonesArray* fHits;    ///< Output array for hits. Used in hit finding.
    BmnEvent* fEvent;       //! ///< Pointer to current event


    /** Perform response simulation for one MC Point
     ** @param point   Pointer to BmnSsdSensorPoint with relevant parameters
     ** @return  Status variable, depends on concrete type
     **
     ** Perform the appropriate action for a particle trajectory in the
     ** sensor characterised by the BmnSsdSensorPoint object. This is specific
     ** to the sensor type and has to be implemented in the derived class.
     **/
    virtual Int_t CalculateResponse(BmnSsdSensorPoint* point) = 0;


    /** Prevent usage of copy constructor and assignment operator **/
    BmnSsdSensor(const BmnSsdSensor&) = delete;
    BmnSsdSensor& operator=(const BmnSsdSensor&) = delete;


    ClassDef(BmnSsdSensor,2);

};


#endif
