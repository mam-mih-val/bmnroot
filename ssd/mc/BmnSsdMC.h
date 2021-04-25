/** @file BmnSsdMC.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 06.12.2018
 **/

#ifndef BMNSSDMC_H
#define BMNSSDMC_H

#include "BmnSsdPoint.h"
#include "BmnSsdSetup.h"
#include "BmnSsdTrackStatus.h"

#include "FairDetector.h"
#include "FairRootManager.h"
#include "FairVolume.h"

#include "TClonesArray.h"
#include "TGeoNode.h"
#include "TGeoCombiTrans.h"

#include <map>


/** @class BmnSsdMC
 ** @brief Class for the MC transport of the BMN-SSD
 ** @author V. Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @version 2.0
 **
 ** The BmnSsdMC defines the behaviour of the SSD system during
 ** transport simulation. It constructs the SSD transport geometry
 ** and creates objects of type BmnSsdPoints.
 **/
class BmnSsdMC : public FairDetector
{
  public:

    /** Constructor
     ** @param active   If set true, ProcessHits will be called and
     **                 BmnSsdPoints will be created.
     ** @param name     Name of detector object
    **/
    BmnSsdMC(Bool_t active = kTRUE, const char* name = "SSDMC");

    /** Destructor **/
    virtual ~BmnSsdMC();


    /** @brief Check whether a volume is sensitive.
     **
     ** @param(name)  Volume name
     ** @value        kTRUE if volume is sensitive, else kFALSE
     **
     ** The decision is based on the volume name (has to contain "Sensor").
     ** Virtual from FairModule.
     **/
    virtual Bool_t CheckIfSensitive(std::string name) {
        return ( TString(name).Contains("Sensor") ? kTRUE : kFALSE );
    }

    /** @brief Construct the SSD geometry in the TGeoManager.
     **
     ** Only ROOT geometries are supported. The method
     ** FairDetector::ConstructRootGeometry() is called.
     ** Virtual from FairModule.
     **/
    virtual void ConstructGeometry();

    /** @brief Action at end of event
     **
     ** Short status log and Reset().
     ** Virtual from FairDetector.
     **/
    virtual void EndOfEvent();

    /**  @brief Initialisation
     **
     ** The output array is created and the map from full node path
     ** to unique address is filled from BmnSsdSetup. Then, the base
     ** class method FairDetector::Initialize() is called.
     ** Virtual from FairDetector.
     **/
    virtual void Initialize();

    /** @brief Get array of BmnSsdPoints
     ** @param iColl  number of point collection
     ** @return Pointer to SsdPoint array. NULL if iColl > 0.
     **
     ** Abstract from FairDetector.
     **/
    virtual TClonesArray* GetCollection(Int_t iColl) const {
        return ( iColl ? NULL : fSsdPoints );
    }

    /** @brief Screen log
     ** Prints current number of SsdPoints in array.
     ** Virtual from TObject.
     **/
    virtual void Print(Option_t* opt = "") const;

    /** @brief Action for a track step in a sensitive node of the SSD
     **
     ** @param vol  Pointer to the active volume
     ** @return kTRUE
     **
     ** The track status is registered when entering or exiting. For all
     ** steps, the energy loss is accumulated. When the track exits the
     ** sensitive node (sensor), a BmnSsdPoint is created (see CreatePoint()),
     ** if the total energy loss in the sensor is non-vanishing (e.g., no
     ** neutral tracks are registered).
     ** Abstract from FairDetector.
     **/
    virtual Bool_t ProcessHits(FairVolume* vol = 0);

    /** @brief Register output array (SsdPoint) to the I/O manager
     **
     ** Abstract from FairDetector.
     **/
    virtual void Register() {
        FairRootManager::Instance()->Register("SsdPoint", GetName(), fSsdPoints, kTRUE);
    }

    /** @brief Create SsdPoints also for neutral particles
     ** @param choice  If kTRUE, SsdPoints are created also for neutrals
     **
     ** By default, SsdPoints are only created if there is non-vanishing
     ** energy loss for the particle in the detector. Neutral particles
     ** do normally not deposit energy, such that no SsdPoints are
     ** created. For some applications however, e.g. for the calculation
     ** of the radiation dose, the neutron flux is required. For such
     ** cases, the creation of SsdPoints for neutrals can be activated
     ** by this method.
     **/
    void ProcessNeutrals(Bool_t choice = kTRUE) {
         fProcessNeutrals = choice;
    }

    /** @brief Clear output array and reset current track status
     **
     ** Abstract from FairDetector.
     **/
    virtual void Reset();

    virtual void ConstructRootGeometry();
    void ExpandSsdNodes(TGeoNode* fN);

  private:

    BmnSsdTrackStatus fStatusIn;   //! Track status at entry of sensor
    BmnSsdTrackStatus fStatusOut;  //! Track status at exit of sensor
    Double_t          fEloss;      //! Accumulated energy loss for current track
    std::map<TString, Int_t> fAddressMap;  ///< Map from full path to unique address
    TClonesArray*     fSsdPoints;  //!  Output array (BmnSsdPoint)
    BmnSsdSetup*      fSetup;      //! Pointer to static instance of BmnSsdSetup
    TGeoCombiTrans*   fCombiTrans; //! Transformation matrix for geometry positioning
    Bool_t fProcessNeutrals;      ///< Create points also for neutral particles

    /** @brief Create a new SsdPoint
     ** Creates a new BmnSsdPoint using the current track status information
     ** and adds it to the output TClonesArray.
     **/
    BmnSsdPoint* CreatePoint();

    /** @brief Set the current track status
     *  Set the current track status (in or out) with parameters obtained
     ** from TVirtualMC (track ID, address, position, momentum, time, length).
     */
    void SetStatus(BmnSsdTrackStatus& status);

    /** @brief Check how the TGeoVolume in file was produced
     *  Check how the TGeoVolume in the geometry file was produced.
     *  The new way is to export the volume with the Export function
     *  of TGeoVolume together with a TGeoMatrix.
     *  To identify a file of new type check for TGeoVolume and a TGeoMatrix
     *  derived class in the file.
     */
    Bool_t IsNewGeometryFile(TString filename);

    /** Copy constructor: usage prevented **/
    BmnSsdMC(const BmnSsdMC&) = delete;
    /** Assignment operator: usage prevented **/
    BmnSsdMC operator=(const BmnSsdMC&) = delete;

    ClassDef(BmnSsdMC,1);
};

#endif /* BMNSSDMC_H */
