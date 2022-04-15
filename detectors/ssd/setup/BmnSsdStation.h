/** @file BmnSsdStation.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 19.12.2018
 **/

#ifndef BMNSSDSTATION_H
#define BMNSSDSTATION_H 1


#include <string>
#include "TGeoPhysicalNode.h"
#include "TNamed.h"

class BmnSsdElement;
class BmnSsdSensor;


/** @class BmnSsdStation
 ** @brief Class representing a station of the SsdSystem.
 ** @author V.Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @version 1.0
 **
 ** The SsdStation class represents one level in the SSD element hierarchy.
 ** It holds station-wide parameters like z position, sensor thickness,
 ** and material budget for use in reconstruction tasks.
 **/
class BmnSsdStation : public TNamed {

	public:

		/** Default constructor **/
		BmnSsdStation();


		/** Standard constructor
		 ** @param name   Station name
		 ** @param title  Station type
		 ** @param node   Pointer to corresponding TGeoPhysicalNode
		 */
		BmnSsdStation(const char* name, const char* title,
						TGeoPhysicalNode* node = NULL);


		/** Destructor **/
		virtual ~BmnSsdStation();


		/** Add a ladder to the station **/
		void AddLadder(BmnSsdElement* ladder);


		/** Radiation length of the main material of the station (silicon)
		 ** @value  Radiation length of silicon [cm]
		 **
		 ** Required from L1/KF. The value for silicon is hard-coded here,
		 ** since it is not likely that the material will change.
		 **/
		Double_t GetRadLength() const { return 9.34953; }


		/** Sensor thickness
   	 ** @value Thickness (extension in z) of first sensor [cm]
   	 **
     ** This return the thickness of the first sensor in the station.
		 ** Needed by L1/KF.
		 ** Note that in general, sensors of different thicknesses can be
		 ** in the station. In this case, a warning message is issued when
		 ** the station properties are initialised.
	   **/
		Double_t GetSensorD() const { return fSensorD; }


		/** Get sensor strip pitch
		 ** @param iSide  Sensor side (0 = front, 1 = back)
		 ** @value Strip pitch [cm]
   	 **
     ** This return the strip pitch of the first sensor in the station.
     ** Valid only for sensors of type DSSD.
		 ** Needed by L1/KF.
		 ** Note that in general, sensors of different pitches can be
		 ** in the station.
		 **/
		Double_t GetSensorPitch(Int_t iSide) const;


		/** Sensor rotation
		 ** @value Rotation of first sensor in station in global C.S. [rad]
		 **
     ** This return the rotation angle in the x-y plane of the first sensor
     ** in the station w.r.t. the global C.S.
		 ** Needed by L1/KF.
		 ** Note that in general, the rotation angle will be different for each
		 ** sensor. In particular, half of them are upside down.
		 **/
		Double_t GetSensorRotation() const { return fSensorRot; }


		/** Get sensor stereo angle
		 ** @param iSide  Sensor side (0 = front, 1 = back)
		 ** @value Stereo angle [degrees]
   	 **
     ** This return the strip stereo angle of the first sensor in the station.
     ** Valid only for sensors of type DSSD.
		 ** Needed by L1/KF.
		 ** Note that in general, sensors of different stereo angles can be
		 ** in the station.
		 **/
		Double_t GetSensorStereoAngle(Int_t iSide) const;


		/** Minimal and maximal x and y coordinates **/
		Double_t GetXmin() const { return fXmin; }
		Double_t GetXmax() const { return fXmax; }
		Double_t GetYmin() const { return fYmin; }
		Double_t GetYmax() const { return fYmax; }


		/** Station z position in global c.s.
		 ** @value  z Position of station in global C.S. [cm]
		 **
		 ** Calculated from the sensor positions (mean between min z and max z)
		 **/
		Double_t GetZ() const { return fZ; }


	 /** @brief Initialise the station parameters **/
		void Init();


		/** Info  **/
		virtual std::string ToString() const;




	private:

		Double_t fZ;            ///< z position of station [cm]
		Double_t fXmin;         ///< minimal x coordinate [cm]
		Double_t fXmax;         ///< maximal x coordinate [cm]
		Double_t fYmin;         ///< minimal y coordinate [cm]
		Double_t fYmax;         ///< maximal y coordinate [cm]
		Double_t fSensorD;      ///< thickness of sensors [cm]
		Double_t fSensorRot;    ///< Rotation of first sensor in global c.s. [rad]
		Int_t    fNofSensors;   ///< Number of sensors in station
		Bool_t   fDiffSensorD;  ///< Flag for different sensor thicknesses
		BmnSsdSensor* fFirstSensor; ///< Pointer to first sensor

	    TGeoPhysicalNode* fNode;               ///< Pointer to geometry
	    std::vector<BmnSsdElement*> fLadders;  ///< Array of ladders



		/** @brief Check properties of sensors (position, thickness)
		 ** The z position of the station is determined as the mean of the minimum
		 ** and maximum z positions of all sensors within the station.
		 ** The active thickness of the first sensor is defined as thickness of the
		 ** entire station. A warning is given out if there are different thicknesses
		 ** among the sensors in the stations.
		 **/
		void CheckSensorProperties();


    // --- Prevent usage of copy constructor and assignment operator
    BmnSsdStation(const BmnSsdStation&) = delete;
    BmnSsdStation& operator=(const BmnSsdStation&) = delete;


    ClassDef(BmnSsdStation, 1);
};

#endif /* BMNSSDSTATION_H */
