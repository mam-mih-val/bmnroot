/** @file BmnSsdTrackStatus.h
 **
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 07.12.2018
 **/


#ifndef BMNSSDTRACKSTATUS_H
#define BMNSSDTRACKSTATUS_H


/** @class BmnSsdTrackStatus
 ** @brief Stores status of track during transport.
 ** Auxiliary for BmnSsd.
 ** @version 1.0
 **/
class BmnSsdTrackStatus {

	public:

		/** Constructor  **/
		BmnSsdTrackStatus() : fAddress(0), fTrackId(0), fPid(0),
		                      fX(0.), fY(0.), fZ(0.),
		                      fPx(0.), fPy(0.), fPz(0.),
		                      fTime(0.), fLength(0.), fFlag(kTRUE) {		}


		/** Destructor  **/
		~BmnSsdTrackStatus() { };


		/** Reset all parameters to zero **/
		void Reset() {
			fAddress = 0;
			fTrackId = 0;
			fPid     = 0;
			fX       = 0.;
			fY       = 0.;
			fZ       = 0.;
			fPx      = 0.;
			fPy      = 0.;
			fPz      = 0.;
			fTime    = 0.;
			fLength  = 0.;
			fFlag    = kTRUE;
		}


		Int_t    fAddress;    ///< Unique address
		Int_t    fTrackId;    ///< MCTrack index
		Int_t    fPid;        ///< MCTrack PID [PDG code]
		Double_t fX;          ///< x position [cm]
		Double_t fY;          ///< x position [cm]
		Double_t fZ;          ///< x position [cm]
		Double_t fPx;         ///< Momentum x component [GeV]
		Double_t fPy;         ///< Momentum x component [GeV]
		Double_t fPz;         ///< Momentum x component [GeV]
		Double_t fTime;       ///< Time since track creation [ns]
		Double_t fLength;     ///< Length since track creation [cm]
		Bool_t   fFlag;       ///< Status flag. TRUE if normal entry/exit, else FALSE

};




#endif /* BMNSSDTRACKSTATUS_H */
