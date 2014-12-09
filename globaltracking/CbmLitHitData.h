/**
 * \file CbmLitHitData.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \since 2008-2014
 * \brief Class for accessing the hits in the track reconstruction.
 **/

#ifndef CBMLITHITDATA_H_
#define CBMLITHITDATA_H_

//#include "CbmLitTypes.h"

#include <vector>
#include <set>
#include <map>
#include <string>
#include <utility>

using namespace std;

class CbmLitHit;

class CbmLitHitData
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitHitData();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitHitData();

   /**
    * \brief Set number of stations.
    */
   void SetNofStations(Int_t nofStations);

   /**
    * \brief Add hit.
    * \param[in] hit Pointer to hit to be added.
    */
   void AddHit(
      CbmLitHit* hit);

   /**
    * \brief Return hit pointer.
    * \param[in] station Index of the station.
    * \param[in] hitId Hit index in array of hits for the specified station.
    * \return Hit pointer.
    */
   const CbmLitHit* GetHit(
      Int_t station,
      Int_t hitId) const;

   /**
    * \brief Return array of hits.
    * \param[in] station Index of station.
    * \return Array of hits.
    */
   const HitPtrVector& GetHits(
      Int_t station);

   /**
    * \brief Return number of hits for the specified station index.
    * \param[in] station Index of station.
    * \return Number of hits for the specified station index.
    */
   Int_t GetNofHits(
      Int_t station) const;

   /**
    * \breif Return maximum hit error in [cm] and coordinate name ("X", "Y", "U").
    * \param[in] station Index of station.
    * \return Pair of hit error and and coordinate name.
    */
   litfloat GetMaxErrX(
      Int_t station) const;

   /**
    * \breif Return maximum hit error in [cm] and coordinate name ("X", "Y", "U").
    * \param[in] station Index of station.
    * \return Pair of hit error and and coordinate name.
    */
   litfloat GetMaxErrY(
      Int_t station) const;

//   /**
//    * \brief Return Z positions of hits.
//    * \param[in] station Index of station.
//    * \return Z positions of hits
//    */
//   const vector<litfloat>& GetZPos(
//      Int_t station) const;

   /**
    * \brief Return bin numbers for Z positions of hits.
    * \param[in] station Index of station.
    * \return Bin numbers.
    */
   const vector<Int_t>& GetZPosBins(
      Int_t station) const;

   /**
    * \brief Return Z positions of hit.
    * \param[in] station Index of station.
    * \param[in] bin Bin number.
    * \return Z positions of hits
    */
   litfloat GetZPosByBin(
      Int_t station,
      Int_t bin) const;

   /**
    * \brief Return bin number for hit Z position.
    * \param[in] station Index of station.
    * \param[in] zPos Z position.
    * \return Bin number.
    */
   Int_t GetBinByZPos(
      Int_t station,
      litfloat zPos) const;

   /**
    * \brief Return minimum Z position of hits.
    * \param[in] station Index of station.
    * \return minimum Z position of hits
    */
   litfloat GetMinZPos(
      Int_t station) const;

   /**
    * \brief Clear array of hits
    */
   void Clear();

   /**
    * \brief Must be called after all hits are added.
    */
   void Arrange();

   /**
    * \brief Return string representation of this class.
    * \return String representation of the class.
    */
   virtual string ToString() const;

private:
   static const litfloat EPSILON;
   vector<HitPtrVector> fHits; // Array of hits for each station
   vector<litfloat> fMaxErrX; // Array of maximum X position errors for each station
   vector<litfloat> fMaxErrY; // Array of maximum Y position errors for each station
   Int_t fNofStations; // Number of stations
   vector<set<litfloat> > fZPosSet; // Set of Z positions of hits in each station
                                    // Temporarily used for Z different Z positions calculation
   //vector<vector<litfloat> > fZPos; // Array of Z positions of hits in each station
   vector<vector<Int_t> > fZPosBins; // Array of Z positions bin number of hits in each station
};

#endif /*CBMLITHITDATA_H_*/
