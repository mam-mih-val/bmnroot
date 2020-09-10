/**
 * \file BmnPixelHit.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \author D. Baranov
 * \date 2018
 *
 * Base class for pixel hits used for tracking in BMN.
 * Derives from BmnBaseHit.
 * Additional members are x, y coordinates and x, y, dxy covariances.
 **/
#ifndef BMNPIXELHIT_H_
#define BMNPIXELHIT_H_

#include "BmnBaseHit.h"

#include "TVector3.h"

class BmnPixelHit : public BmnBaseHit
{
public:
	/**
	 * \brief Default constructor.
	 */
	BmnPixelHit();

	/**
	 * \brief Standard constructor.
	 * \param[in] address Detector unique identifier.
	 * \param[in] x X position of the hit [cm].
	 * \param[in] y Y position of the hit [cm].
	 * \param[in] z Z position of the hit [cm].
	 * \param[in] dx X position error of the hit [cm].
	 * \param[in] dy Y position error of the hit [cm].
	 * \param[in] dz Z position error of the hit [cm].
	 * \param[in] dxy XY correlation of the hit.
	 * \param[in] refId Some reference ID.
         * \param[in] time Hit time [ns].
         * \param[in] timeError Error of hit time [ns].
 	 **/
	BmnPixelHit(
			Int_t address,
			Double_t x,
			Double_t y,
			Double_t z,
			Double_t dx,
			Double_t dy,
			Double_t dz,
			Double_t dxy,
			Int_t refId,
 	                Double_t time = -1.,
        	        Double_t timeError = -1.
			);

	/**
	 * \breif Standard constructor.
	 * \param address Detector unique identifier.
	 * \param pos Position of the hit as TVector3 [cm].
	 * \param err Position errors of the hit as TVector3 [cm].
	 * \param dxy XY correlation of the hit.
	 * \param refId Some reference ID.
         * \param[in] time Hit time [ns].
         * \param[in] timeError Error of hit time [ns].
	 **/
	BmnPixelHit(
			Int_t address,
			const TVector3& pos,
			const TVector3& err,
			Double_t dxy,
			Int_t refId,
 	                Double_t time = -1.,
        	        Double_t timeError = -1.
			);

	/**
	 * brief Destructor.
	 */
	virtual ~BmnPixelHit();

	/**
	 * \brief Inherited from BmnBaseHit.
	 **/
	virtual std::string ToString() const;

	/* Accessors */
	Double_t GetX() const { return fX; }
	Double_t GetY() const { return fY; }
	Double_t GetDx() const { return fDx; }
	Double_t GetDy() const { return fDy; }
	Double_t GetDxy() const { return fDxy; }

	/**
	 * \brief Copies hit position to pos.
	 * \param pos Output hit position.
	 */
	void Position(TVector3& pos) const;

	/**
	 * \brief Copies hit position error to pos.
	 * \param pos Output hit position error.
	 */
	void PositionError(TVector3& dpos) const;

	/* Setters */
	void SetX(Double_t x) { fX = x; }
	void SetY(Double_t y) {fY = y;}
	void SetDx(Double_t dx) {fDx = dx;}
	void SetDy(Double_t dy) {fDy = dy;}
	void SetDxy(Double_t dxy) {fDxy = dxy;}

	/**
	 * \brief Sets position of the hit.
	 * \param pos new hit position.
	 **/
	void SetPosition(const TVector3& pos);

	/**
	 * \breif Sets position error of the hit.
	 * \param dpos new hit position error
	 **/
	void SetPositionError(const TVector3& dpos);

private:
	Double_t fX, fY; ///< X, Y positions of hit [cm]
	Double_t fDx, fDy; ///< X, Y errors [cm]
	Double_t fDxy; ///< XY correlation

	ClassDef(BmnPixelHit, 1);
};

#endif /* BMNPIXELHIT_H_ */
