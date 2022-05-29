/** @file BmnSsdSetup.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 13.12.2018
 **/


#ifndef BMNSSDSETUP_H
#define BMNSSDSETUP_H 1

#include <map>
#include "BmnSsdElement.h"
#include "BmnSsdSensor.h"

class TGeoManager;
class BmnSsdDigitize;
class BmnSsdDigitizeParameters;
class BmnSsdModule;
class BmnSsdStation;


/** @class BmnSsdSetup
 ** @brief Class representing the top level of the SSD setup
 ** @version 1.0
 **
 ** The BmnSsdSetup is a singleton class and represents the
 ** interface to the BMN geometry and its elements.
 **/
class BmnSsdSetup : public BmnSsdElement
{

  public:

    /** Destructor **/
    virtual ~BmnSsdSetup() { };


    /** Assign a sensor to an address
     ** @param  address  Unique element address
     ** @param  node     Physical node of sensor
     ** @return  Pointer to sensor object
     **
     ** The sensor object is taken from the sensor list and gets the
     ** geometric node assigned. If it is not present in the list,
     ** the default sensor is instantiated (see DefaultSensor()).
     **/
    BmnSsdSensor* AssignSensor(Int_t address,
                               TGeoPhysicalNode* node = nullptr);


    /** @brief Instantiate the default sensor class for a given node
     ** @param address  Unique element address
     ** @param node Sensor node in the ROOT geometry
     ** @value Pointer to BmnSsdSensor object
     **
     ** Instantiates a default sensor object and adds it to the sensor list.
     ** The default sensor class and parameters are hard-coded here.
     ** The default is BmnSsdSensorDssdStereo, with 58 mum pitch and
     ** stereo angles 0 degrees for front side and 7.5 degrees for back side.
     ** The active size in y is 0.24 cm smaller than the geometric
     ** extension. The number of strips is adjusted to the geometric extension
     ** in x minus 0.24 cm inactive area.
     **/
    BmnSsdSensor* DefaultSensor(Int_t address, TGeoPhysicalNode* node);


    /** Get the digitiser task **/
    BmnSsdDigitize* GetDigitizer() const { return fDigitizer; }


    /** Get digitisation parameters **/
    BmnSsdDigitizeParameters* GetDigiParameters() const {
      return fSettings;
    }


    /** Get an SSD element by address
     ** @param address  Unique element address (see BmnSsdAddress)
     ** @param level    Element level (ESsdElementLevel)
     ** @return Pointer to SSD element
     **/
    BmnSsdElement* GetElement(Int_t address, Int_t level);


    /** Get the name of an element level
     ** @param level    Element level (ESsdElementLevel)
     ** @return  Name of element level (station, ladder, etc.)
     **/
    const char* GetLevelName(Int_t level);


    /** @brief Get a module from the module array.
     ** @param  index  Index of module in the vector
     ** @return  Pointer to module
     **
     ** For convenient loops over all modules.
     ** Note that the index of the module is meaningless.
     **/
    BmnSsdModule* GetModule(Int_t index) { return fModuleVector.at(index); }


    /** Get number of modules in setup **/
    Int_t GetNofModules() const { return fModules.size(); }


    /** Get number of sensors in setup **/
    Int_t GetNofSensors() const { return fSensors.size(); }


    /** Get number of stations **/
    Int_t GetNofStations() const { return fStations.size(); }


    /** Get a station
     ** @param stationId  Station number
     ** @value Pointer to station object. NULL if not present.
     **/
    BmnSsdStation* GetStation(Int_t stationId) {
      if ( fStations.find(stationId) == fStations.end() ) return NULL;
      return fStations[stationId];
    }


    /** Get station number from address
     ** @param address  Unique detector address
     ** @value Station number
     **/
    Int_t GetStationNumber(Int_t address);


    /** @brief Initialise the setup
     ** @param sensorParameterFile  Name of file with sensor parameters
     ** @param geometryFile         Name of file with SSD geometry
     ** @return  kTRUE if successfully initialised
     **
     ** The setup will be initialised from the SSD geometry, either
     ** taken from the TGeoManager or, if specified, read from a
     ** geometry file.
     ** In addition to the geometry, internal parameters of the sensors
     ** have to be specified. This can be done through the parameter file,
     ** which should contain for each sensor to be defined a line with
     ** sensor_name sensor_type parameters
     ** The number of parameters depends on the sensor type.
     ** If no parameter file is specified, the default sensor type will be
     ** used (see method DefautSensor()).
     **/
    Bool_t Init(const char* geometryFile = nullptr,
                const char* sensorParameterFile = nullptr);


    /** @brief Initialisation status
     ** @return kTRUE if setup is initialised
     **/
    Bool_t IsInit() const { return fIsInitialised; }


    /** Static instance of BmnSsdSetup **/
    static BmnSsdSetup* Instance();


    /** Print list of sensors with parameters **/
    void ListModules() const;


    /** Print list of sensors with parameters **/
    void ListSensors() const {
      for (auto it = fSensors.begin(); it != fSensors.end(); it++)
        LOG(info) << it->second->ToString();
    }


    /** @brief Modify the strip pitch for all sensors
     ** @param pitch  New strip pitch [cm]
     ** @value Number of modified sensors
     **
     ** This method will have effect only for sensor of type DSSD.
     ** The strip pitch of all sensors is modified to the specified
     ** value. The number of strips in the sensor is re-calculated.
     **/
    Int_t ModifyStripPitch(Double_t pitch);


    /** @brief Set the default sensor parameters
     ** @param dInact  Size of inactive boarder (guard ring) [cm]
     ** @param pitch   Strip pitch [cm]
     ** @param stereoF Strip stereo angle front side [degrees]
     ** @param stereoB Strip stereo angle back side [degrees]
     **
     ** These parameters correspond to the default sensor type DssdStereo.
     ** They will be applied if no sensor parameter file is specified.
     **/
    void SetDefaultSensorParameters(Double_t dInact, Double_t pitch,
                                    Double_t stereoF, Double_t stereoB);


    /** @brief Set the digitiser task
     ** @param digitizer  Pointer to SSD digitiser task
     **
     ** In the digitisation run, the digitiser task is available from
     ** the setup instance. In other runs, this will be NULL.
     **/
    void SetDigitizer(BmnSsdDigitize* digitizer) {
      fDigitizer = digitizer;
    }


    /** @brief Set the digitiser parameters
     ** @param settings  Pointer to digitiser parameters
     **
     ** The parameters are registered by the digitiser task during digitisation.
     ** They have to be set by a reconstruction task during reconstruction.
     **/
    void SetDigiParameters(BmnSsdDigitizeParameters* settings) {
      fSettings = settings;
    }


    /** @brief Set parameters for all modules
     ** @param dynRange          Dynamic range [e]
     ** @param threshold         Charge threshold [e]
     ** @param nAdc              Number of ADC channels
     ** @param timeResolution    Time resolution [ns]
     ** @param deadTime          Single-channel dead time [ns]
     ** @param noise             Noise RMS [e]
     ** @param zeroNoiseRate     Zero-threshold noise rate [1/ns]
     ** @param fracDeadChannels  Fraction of dead channels
     ** @value Number of modules the parameters of which were set
     **
     ** This method will set the parameters of all modules in the setup
     ** to the values specified by the arguments.
     **/
    Int_t SetModuleParameters(Double_t dynRange, Double_t threshold,
                              Int_t nAdc, Double_t timeResolution,
                              Double_t deadTime, Double_t noise,
                              Double_t zeroNoiseRate,
                              Double_t fracDeadChannels);


    /** @brief Read module parameters from file
     ** @param fileName  Name of file with sensor conditions
     ** @value Number of modules the parameters of which were set
     **
     ** The file with the parameters is read and the parameters
     ** for the module are set accordingly. Each module in the setup must
     ** show up in the file. The format is a text file containing for each
     ** module a line with
     ** module_name dynRange threshold nAdc tResol tDead noise zeroNoise fracDead
     ** For the definition of the parameters, see SetModuleParameters above.
     ** Empty lines or lines starting with '#' (comments) are ignored.
     **/
    Int_t SetModuleParameters(const char* fileName);


    /** @brief Set conditions for all sensors (same values for all)
     ** @param vDep         Full depletion voltage [V]
     ** @param vBias        Bias voltage [V]
     ** @param temperature  Temperature [K]
     ** @param cCoupling    Coupling capacitance [pF]
     ** @param cInterstrip  Inter-strip capacitance [pF]
     ** @value Number if sensors the conditions were set to.
     **
     ** The values are taken from the member fSettings, which has to be
     ** initialised before.
     **/
    Int_t SetSensorConditions();


    /** @brief Set conditions for all sensors (same values for all)
     ** @param vDep         Full depletion voltage [V]
     ** @param vBias        Bias voltage [V]
     ** @param temperature  Temperature [K]
     ** @param cCoupling    Coupling capacitance [pF]
     ** @param cInterstrip  Inter-strip capacitance [pF]
     ** @value Number if sensors the conditions were set to.
     **
     ** In addition to the specified values, the magnetic field in the
     ** sensor centre is evaluated and stored.
     **/
    Int_t SetSensorConditions(Double_t vDep, Double_t vBias,
                              Double_t temperature, Double_t cCoupling,
                              Double_t cInterstrip);


    /** @brief Read sensor conditions from file
     ** @param fileName  Name of file with sensor conditions
     ** @value Number of sensors the conditions are set for
     **
     ** The file with the conditions is read and the condition parameters
     ** for the sensor are set accordingly. Each sensor in the setup must
     ** show up in the file. The format is a text file containing for each
     ** sensor a line with
     ** sensor_name vDep vBias temperature cCoupling cInterstrip
     ** Empty lines or lines starting with '#' (comments) are ignored.
     **/
    Int_t SetSensorConditions(const char* fileName);



  private:

    static BmnSsdSetup* fgInstance;    ///< Static instance of this class
    BmnSsdDigitize* fDigitizer;        ///< Pointer to digitiser task
    BmnSsdDigitizeParameters* fSettings;     ///< Pointer to digitiser settings

    Bool_t fIsInitialised;  ///< To protect against multiple initialisation.
    Bool_t fIsOld;          ///< Old setup with stations as top level
    Int_t  fNofSensorsDefault;  ///< Number of sensors created by default

    // --- Default sensor parameters (for type DssdStereo)
    Double_t fSensorDinact;   ///< Size of inactive border [cm]
    Double_t fSensorPitch;    ///< Strip pitch [cm]
    Double_t fSensorStereoF;  // Stereo angle front side [degrees]
    Double_t fSensorStereoB;  // Stereo angle back side [degrees]

    // --- Map of sensors. Key is address.
    std::map<Int_t, BmnSsdSensor*> fSensors;

    // --- Map of modules. Key is address.
    std::map<Int_t, BmnSsdModule*> fModules;

    // --- Vector of modules. For convenient loops.
    std::vector<BmnSsdModule*> fModuleVector;

    // --- Map of stations. Key is station number.
    // --- Stations are a special case needed for reconstruction;
    // --- they are not elements in the setup.
    std::map<Int_t, BmnSsdStation*> fStations;  //!

    /** Default constructor  **/
    BmnSsdSetup();


    /** Create station objects **/
    Int_t CreateStations();


    /** @brief Read the geometry from TGeoManager
     ** @param geoManager  Instance of TGeoManager
     ** @return kTRUE if successfully read; kFALSE else
     **
     ** The ROOT geometry is browsed for elements of the setup,
     ** which are then instantiated and connected to the respective
     ** physical node.
     **/
    Bool_t ReadGeometry(TGeoManager* geoManager);


    /** @brief Read the geometry from a ROOT geometry file
     ** @param fileName  Name of geometry file
     ** @return kTRUE if successfully read; kFALSE else
     **
     ** The ROOT geometry is browsed for elements of the setup,
     ** which are then instantiated and connected to the respective
     ** physical node.
     **/
    Bool_t ReadGeometry(const char* fileName);


    /** @brief Read sensor parameters from file
     ** @param fileName  Parameter file name
     ** @value Number of sensors the type of which was set.
     **
     ** Read the sensor parameters that are not contained in the geometry,
     ** but required for digitisation and reconstruction, like strip pitch,
     ** stereo angle etc. Sensors not contained in the file will be
     ** instantiated with default parameters.
     **/
    Int_t ReadSensorParameters(const char* fileName);


    /** Copy constructor (disabled) **/
    BmnSsdSetup(const BmnSsdSetup&) = delete;


    /** Assignment operator (disabled) **/
    BmnSsdSetup operator=(const BmnSsdSetup&) = delete;

    ClassDef(BmnSsdSetup, 2);

};

#endif /* BMNSSDSETUP_H */
