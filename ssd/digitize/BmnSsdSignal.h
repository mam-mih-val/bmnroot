/** @file BmnSsdSignal.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 12.12.2018
 **/

#ifndef BMNSSDSIGNAL_H
#define BMNSSDSIGNAL_H 1

#include "TObject.h"
#include "BmnMatch.h"


/** @class BmnSsdSignal
 ** @brief Data class for an analog signal in the SSD
 ** @since 12.12.2018
 ** @version 1.0
 **
 ** Simple data class used in the digitisation process of the SSD. It describes
 ** an analog charge signal produced in the SSD sensors and arriving at the
 ** readout. It contains time and charge information (the latter through the
 ** total weight member of the BmnMatch member), and references to the MCPoints
 ** having caused the charge.
 ** In the most general case, a signal can be produced by more than one
 ** MCPoint; that is why the MC reference is of type BmnMatch and not BmnLink.
 **/
class BmnSsdSignal : public TObject {

	public:

		/** Default constructor
		 ** @param time    Signal time [ns]
		 ** @param charge  Analog charge [e]
		 ** @param index   Index of BmnSsdPoint
		 ** @param entry   Entry in input TTree
		 ** @param file    Number of input file
		 **/
		BmnSsdSignal(Double_t time = 0., Double_t charge = 0.,
				         Int_t index = 0, Int_t entry = -1, Int_t file = -1);


		/** Destructor **/
		virtual ~BmnSsdSignal();


		/** Add a link to MCPoint to the match member
		 ** @param charge  Analog charge [e]
		 ** @param index   Index of BmnSsdPoint
		 ** @param entry   Entry in input TTree
		 ** @param file    Number of input file
		 **/
		void AddLink(Double_t charge, Int_t index,
				         Int_t entry = -1, Int_t file = -1) {
			fMatch.AddLink(charge, index, entry, file);
		}


		/** Charge
		 ** @return Signal analog charge [e]
		 **/
		Double_t GetCharge() const { return fMatch.GetTotalWeight(); }


		/** Match object
		 ** @return const. reference to match object
		 **/
		const BmnMatch& GetMatch() const { return fMatch; }


		/** Time
		 ** @return Signal time [ns]
		 **/
		Double_t GetTime() const { return fTime; }


    /** Set time
     ** @param time  New signal time [ns]
     **/
    void SetTime(Double_t time) { fTime = time; }


		/** Less operator
		 ** @param Reference to BmnSignal object to compare to
		 ** @return true if signal time is less than time of comparison signal
		 **/
		bool operator < (const BmnSsdSignal& otherSignal) const {
			return ( fTime < otherSignal.GetTime() );
		}


		/** Comparator for pointer objects
		 ** Needed to store objects by pointer in sorted containers
		 ** Sorting criterion is the signal time.
		 **/
		struct Before {
				bool operator () (BmnSsdSignal* signal1, BmnSsdSignal* signal2) {
					return ( signal1->GetTime() < signal2->GetTime() );
				}
		};

	private:

		Double_t fTime;    ///< Signal time [ns]
		BmnMatch fMatch;   ///< Match object (total weight = charge)

		ClassDef(BmnSsdSignal, 1);
};

#endif /* BMNSSDSIGNAL_H */
