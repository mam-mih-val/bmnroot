/**
 ** \file BmnSsdHit.h
 ** \author Volker Friese <v.friese@gsi.de>
 ** \author D. Baranov
 ** \since 20.12.2018
 ** \brief Data class for a reconstructed hit in the SSD
 **/

#ifndef BMNSDSHIT_H
#define BMNSDSHIT_H 1

#include "BmnPixelHit.h"

class TVector3;


/** @class BmnSsdHit
 ** @brief  data class for a reconstructed 3-d hit in the SSD
 **
 ** A hit in the SSD is a position measurement constructed from two clusters
 ** on the front and back side of the sensors, respectively, which have
 ** a geometric intersection. In addition to the base class, it provides
 ** indices of the contributing clusters and the measurement time.
 **/
class BmnSsdHit : public BmnPixelHit
{

public:

		/** Default constructor **/
    BmnSsdHit();


    /** Constructor with all parameters
     ** @param address  Unique detector address (see BmnSsdAddress)
     ** @param pos      Hit coordinate vector [cm]
     ** @param dpos     Hit coordinate error vector [cm]
     ** @param dxy      x-y covariance [cm**2]
     ** @param frontClusterId  Index of front-side cluster
     ** @param backClusterId   Index of back-side cluster
     ** @param time            Hit time [ns]
     ** @param timeError       Hit time error [ns]
     ** @param du       Coordinate error across front-side strips [cm]
     ** @param dv       Coordinate error across back-side strips [cm]
     **/
    BmnSsdHit(Int_t address, const TVector3& pos, const TVector3& dpos,
    		      Double_t dxy, Int_t frontClusterId, Int_t backClusterId,
     		      Double_t time = 0., Double_t timeError = 0.,
     		      Double_t du = 0., Double_t dv = 0.);


    /** Destructor **/
    virtual ~BmnSsdHit();


    /** Index of cluster at the back side
     ** @value  Back-side cluster index
     **/
    Int_t GetBackClusterId() const { return fBackClusterId; }


    /** @brief Error of coordinate across front-side strips
     ** @value Coordinate error [cm]
     **
     ** Note that this error is defined only in the
     ** local coordinate system of the sensor.
     **/
    Double_t GetDu() const { return fDu; }


    /** @brief Error of coordinate across front-side strips
     ** @value Coordinate error [cm]
     **
     ** Note that this error is defined only in the
     ** local coordinate system of the sensor.
     **/
    Double_t GetDv() const { return fDv; }


    /** Index of cluster at the front side
     ** @value  Front-side cluster index
     **/
    Int_t GetFrontClusterId() const { return fFrontClusterId; }


    /** Info to string **/
	virtual std::string ToString() const;


private:

    Int_t fFrontClusterId; ///< Cluster index front side
    Int_t fBackClusterId;  ///< Cluster index back side
    Double_t fDu;          ///< Error of coordinate across front-side strips [cm]
    Double_t fDv;          ///< Error of coordinate across back-side strips [cm]

    ClassDef(BmnSsdHit, 7);
};

#endif
