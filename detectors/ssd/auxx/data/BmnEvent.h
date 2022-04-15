/** @file BmnEvent.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 13.12.2018
 **/


#ifndef BMNEVENT_H
#define BMNEVENT_H 1

#include <map>
#include <string>
#include <vector>
#include <TObject.h>
#include "BmnDetectorList.h" //#include "CbmDefs.h"
#include "BmnMatch.h"
#include "BmnVertex.h"


/** @class BmnEvent
 ** @brief Class characterising one event by a collection of links (indices)
 ** to data objects,
 ** @version 1.0
 **
 **/
class BmnEvent: public TObject {

	public:

		/** Default constructor **/
		BmnEvent() : BmnEvent(-1, 0., 0.) {};

		/** Constructor with event number and time
		 ** @param[in] number    Event number
		 ** @param[in] startTime Event start time [ns]
		 ** @param[in] endTime   Event start time [ns]
		 **/
		BmnEvent(Int_t number, Double_t startTime = 0., Double_t endTime = 0.) :
			TObject(), fNumber(number), fTimeStart(startTime), fTimeEnd(endTime),
			fNofData(0), fVertex(), fMatch(NULL), fIndexMap() {
		}

		/** Destructor **/
		virtual ~BmnEvent() {
			if ( fMatch ) delete fMatch;
		}


		/** Add a data object to the index map
         ** @param DataType  Type of data (for values see BmnDetectorList.h)
		 ** @param Index     Index of the data object in its TClonesArray
		 */
		void AddData(DataType type, UInt_t index);


		/** Add an SSD track to the event
		 ** @param Index of SSD track in its TClonesArray
		 **/
		void AddSsdTrack(UInt_t index) { AddData(kSsdTrack, index); }


		/** Get the index of a data object in its TClonesArray
         ** @param DataType  Type of data (for values see BmnDetectorList.h)
		 ** @param iData     Running number of data object in event
		 ** @value Index of data object in its TClonesArray
		 **/
		UInt_t GetIndex(DataType type, UInt_t iData);


		/** Get match object
		 ** @value Pointer to match object
		 **/
		BmnMatch* GetMatch() const { return fMatch; }


		/** Get total number of data (of all types) in the event **/
		Int_t GetNofData() const { return fNofData; }


		/** Get number of data objects of a given type in this event
         ** @param DataType  Type of data (for values see BmnDetectorList.h)
		 ** @value Number of objects of type DataType in the event.
		 ** -1 is data type is not registered.
		 **/
		Int_t GetNofData(DataType type) const;


		/** Get number of SSD tracks
		 ** @value Number of SSD tracks in the event. -1 if not registered.
		 **/
		Int_t GetNofSsdTracks() const { return GetNofData(kSsdTrack); }


		/** Get event number
		 ** @value Event number
		 **/
		Int_t GetNumber() const { return fNumber; }


		/** Get SSD track index
		 ** @param iTrack  Running number of SSD track in the event
		 ** @value index   Index of SSD track in TClonesArray
		 **/
		Int_t GetSsdTrackIndex(Int_t iTrack) {
			return GetIndex(kSsdTrack, iTrack);
		}


		/** Get event end time
		 ** @value End time of event [ns]
		 **/
		Double_t GetEndTime() const { return fTimeEnd; }


		/** Get event start time
		 ** @value Start time of event [ns]
		 **/
		Double_t GetStartTime() const { return fTimeStart; }


		/** Set end time
		 ** @param endTime  End time of event [ns]
		 **/
		void SetEndTime(Double_t endTime) { fTimeEnd = endTime; }


		/** Set a match object
		 ** @param match  Pointer to a match object
		 **/
		void SetMatch(BmnMatch* match) { fMatch = match; }


		/** Set start time
		 ** @param endTime  Start time of event [ns]
		 **/
		void SetStartTime(Double_t startTime) { fTimeStart = startTime; }


		/** Set the SSD track index array
		 ** @brief Sets the index array for SSD tracks.
		 ** Old content will be overwritten.
		 ** @param indexVector  Vector with indices of SSD tracks
	     **/
		void SetSsdTracks(std::vector<UInt_t>& indexVector) {
			fNofData -= fIndexMap[kSsdTrack].size();
			fIndexMap[kSsdTrack] = indexVector;
			fNofData += fIndexMap[kSsdTrack].size();
		}


	  /** Set the event vertex variables
	   *@param x         x coordinate [cm]
	   *@param y         y coordinate [cm]
	   *@param z         z coordinate [cm]
	   *@param chi2      chi square of vertex fit
	   *@param ndf       Number of degrees of freedom of vertex fit
	   *@param nTracks   Number of tracks used for vertex fit
	   *@param covMat    Covariance Matrix (symmetric, 3x3)
	   **/
	  void SetVertex(Double_t x, Double_t y, Double_t z, Double_t chi2,
			 Int_t ndf, Int_t nTracks, const TMatrixFSym& covMat);


		/** String output **/
		std::string ToString() const;


		/** Get event vertex
		 ** @value Pointer to vertex object
		 **/
		BmnVertex* GetVertex() { return &fVertex; }




	private:

		/** Event meta data **/
		Int_t fNumber;   ///< Event number
		Double_t fTimeStart;  ///< Event start time [ns]
		Double_t fTimeEnd;    ///< Event end time [ns]
		Int_t fNofData;  ///< Number of data objects in the event
		BmnVertex fVertex;  ///< Primary Vertex
		BmnMatch* fMatch; ///< Match object to MCEvent

		/** Arrays of indices to data types **/
		std::map<DataType, std::vector<UInt_t>> fIndexMap;

        BmnEvent(const BmnEvent&) = delete;
        BmnEvent& operator=(const BmnEvent&) = delete;


		ClassDef(BmnEvent, 2);
};

#endif /* BMNEVENT_H_ */
