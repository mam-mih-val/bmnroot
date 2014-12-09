/**
 * \file BmnTrackingGeometryConstructor.h
 * \brief Tracking geometry constructor.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2008-2014
 **/

#ifndef BMNTRACKINGGEOMETRYCONSTRUCTOR_H_
#define BMNTRACKINGGEOMETRYCONSTRUCTOR_H_

#include "TObject.h"
#include "BmnDetectorSetup.h"
//#include "LitDetectorLayout.h"
//#include "LitMaterialGrid.h"

#include <string>

class TGeoManager;
class CbmTrdGeoHandler;
class TProfile2D;

class BmnTrackingGeometryConstructor
{
public:
   /**
    * \brief Return pointer to singleton object.
    * \return Pointer to singleton object.
    */
   static BmnTrackingGeometryConstructor* Instance();

//   /**
//    * \brief Return MUCH detector layout for parallel MUCH tracking in SIMD format.
//    * \param[out] layout MUCH detector layout.
//    */
//   void GetMuchLayoutVec(lit::parallel::LitDetectorLayoutVec& layout);
//
//   /**
//    * \brief Return MUCH detector layout for parallel MUCH tracking in scalar format.
//    * \param[out] layout MUCH detector layout.
//    */
//   void GetMuchLayoutScal(lit::parallel::LitDetectorLayoutScal& layout);
//
//   /**
//    * \brief Return MUCH detector layout for parallel MUCH tracking.
//    * \param[out] layout MUCH detector layout.
//    */
//   template<class T> void GetMuchLayout(lit::parallel::LitDetectorLayout<T>& layout);
//
//   /**
//    * \brief Return TRD detector layout for TRD parallel tracking in SIMD format.
//    * \param[out] layout TRD detector layout.
//    */
//   void GetTrdLayoutVec(lit::parallel::LitDetectorLayoutVec& layout);
//
//   /**
//    * \brief Return TRD detector layout for TRD parallel tracking in scalar format.
//    * \param[out] layout TRD detector layout.
//    */
//   void GetTrdLayoutScal(lit::parallel::LitDetectorLayoutScal& layout);
//
//   /**
//    * \brief Return TRD detector layout for TRD parallel tracking.
//    * \param[out] layout TRD detector layout.
//    */
//   template<class T> void GetTrdLayout(lit::parallel::LitDetectorLayout<T>& layout);
//
//   void GetRichMaterial(
//         lit::parallel::LitMaterialGrid* material);
//
//   void ConvertTProfile2DToLitMaterialGrid(
//         const TProfile2D* profile,
//         lit::parallel::LitMaterialGrid* grid,
//         Double_t maximumValue = 0);

   /**
    * \brief Return number of stations in TRD.
    * \return Number of stations in TRD.
    */
   Int_t GetNofTrdStations();

   /**
    * \brief Return number of stations in MUCH.
    * \return Number of stations in MUCH.
    */
   Int_t GetNofMuchStations();

   /**
    * \brief Return number of stations in MUCH + TRD.
    * \return Number of stations in MUCH + TRD.
    */
   Int_t GetNofMuchTrdStations();

   /**
    * \brief Return number of stations in MVD.
    * \return Number of stations in MVD.
    */
   Int_t GetNofMvdStations();

   /**
    * \brief Return number of stations in STS.
    * \return Number of stations in STS.
    */
   Int_t GetNofStsStations();

   /**
    * \brief Return number of MUCH absorbers.
    * \return Number of MUCH absorbers.
    */
   Int_t GetNofMuchAbsorbers();

   /**
    *
    */
//   Int_t ConvertTrdToAbsoluteStationNr(
//         Int_t station,
//         Int_t layer);

   /**
    *
    */
   Int_t ConvertMuchToAbsoluteStationNr(
         Int_t station,
         Int_t layer);

private:
   /**
    * \brief Constructor.
    * Constructor is protected since singleton pattern is used.
    * Pointer to object is returned by static Instance() method.
    */
   BmnTrackingGeometryConstructor();

   /**
    * \brief Destructor.
    */
   virtual ~BmnTrackingGeometryConstructor();

   /**
    * \brief Copy constructor.
    */
   BmnTrackingGeometryConstructor(const BmnTrackingGeometryConstructor&);

   /**
    * \brief Assignment operator.
    */
   const BmnTrackingGeometryConstructor& operator=(const BmnTrackingGeometryConstructor&);

   TGeoManager* fGeo; // Pointer to full geometry
   Int_t fNofTrdStations; // Number of TRD stations
   Int_t fNofMuchStations; // Number of MUCH stations
   Int_t fNofMvdStations; // Number of MVD stations
   Int_t fNofStsStations; // Number of STS stations
   Int_t fNofMuchAbsorbers; // Number of MUCH absorbers
   BmnDetectorSetup fDet; //
};

#endif /* BmnTrackingGeometryConstructor_H_ */
