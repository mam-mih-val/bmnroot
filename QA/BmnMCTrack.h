/**
 * \file BmnMCTrack.h
 * \brief Monte-Carlo track.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
#ifndef BMNMCTRACK_H_
#define BMNMCTRACK_H_

#include "BmnDetectorList.h"
#include "BmnMCPoint.h"

#include <cassert>
#include <map>
#include <vector>
#include <set>

using namespace std;

/**
 * \class BmnMCTrack
 * \brief Monte-Carlo track.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
class BmnMCTrack
{
public:
   /**
    * \brief Constructor.
    */
   BmnMCTrack() {
      // Initialize all maps
      fPoints[kSILICON];
      fPoints[kSSD];
      fPoints[kGEM];
      fPoints[kTOF1];
      fPoints[kDCH];
      fPoints[kTOF];
      fPoints[kCSC];
//      fStationPoints[kGEM];
//      fStationPoints[kTOF1];
//      fStationPoints[kDCH1];
//      fStationPoints[kDCH2];
//      fStationPoints[kTOF];
//      fStationIds[kGEM];
//      fStationIds[kTOF1];
//      fStationIds[kDCH1];
//      fStationIds[kDCH2];
//      fStationIds[kTOF];
//      fMaxConsecutivePoints[kGEM] = -1;
   }

   /**
    * \brief Destructor.
    */
   virtual ~BmnMCTrack() {};

   /**
    * \brief Add point to track.
    * \param[in] detId Detector identificator.
    * \param[in] point MC point to be added.
    */
   void AddPoint(DetectorId detId, const BmnMCPoint& point) {
      fPoints[detId].push_back(point);
//      fStationPoints[detId][point.GetStationId()].push_back(point);
//      fStationIds[detId].insert(point.GetStationId());

   }

   /**
    * \brief Return vector of MC point for specified detector id.
    * \param[in] detId Detector identificator.
    * \return Array of MC points.
    */
   const vector<BmnMCPoint>& GetPoints(DetectorId detId) const {
      return fPoints.find(detId)->second;
   }

   /**
    * \brief Return MC point for specified detector id and point index.
    * \param[in] detId Detector identificator.
    * \param[in] index Index of MC point.
    * \return MC point.
    */
   const BmnMCPoint& GetPoint(DetectorId detId, Int_t index) const {
      assert(GetNofPoints(detId) != 0);
      return fPoints.find(detId)->second[index];
   }

   /**
    * \brief Return number of MC points for specified detector id.
    * \param[in] detId Detector identificator.
    * \return Number of MC points.
    */
   UInt_t GetNofPoints(DetectorId detId) const {
      return fPoints.find(detId)->second.size();
   }

   /**
    * \brief Return number of MC points in different stations for specified detector id.
    * \param[in] detId Detector identificator.
    * \return Number of MC points.
    */
//   UInt_t GetNofPointsInDifferentStations(DetectorId detId) const {
//      return fStationPoints.find(detId)->second.size();
//   }

   /**
    * \brief Return number of consecutive MC points for specified detector id. Currently works only for STS.
    * \param[in] detId Detector identificator.
    * \return Number of MC points.
    */
//   Int_t GetNofConsecutivePoints(DetectorId detId) const {
//	  //assert(detId == kSTS);
////      return fMaxConsecutivePoints.find(detId)->second;
//	   return (detId == kGEM) ? fMaxConsecutivePoints.find(detId)->second : -1;
//   }

   /**
    * \brief Calculates number of consecutive MC points for specified detector id. Currently works only for STS.
    * \param[in] detId Detector identificator.
    * \return Number of MC points.
    */
//   void CalculateNofConsecutivePoints() {
//	  fMaxConsecutivePoints[kGEM] = MaxConsecutiveNumbers(fStationIds.find(kGEM)->second);
//   }

   /**
    * \brief Return MC point for specified detector id and point index.
    *
    * Before accessing points with this function one has to check
    * the number of points on this station!
    *
    * \param[in] detId Detector identificator.
    * \param[in] stationId Station id.
    * \param[in] index Index of MC point.
    * \return MC point.
    */
//   const BmnMCPoint& GetPointAtStation(
//         DetectorId detId,
//         Int_t stationId,
//         Int_t index) const {
//      assert(GetNofPointsAtStation(detId, stationId) != 0);
//      return fStationPoints.find(detId)->second.find(stationId)->second[index];
//   }

   /**
    * \brief Return number of MC points for specified detector ID and station ID.
    * \param[in] detId Detector identificator.
    *  \param[in] stationId Station id.
    * \return Number of MC points.
    */
//   UInt_t GetNofPointsAtStation(
//         DetectorId detId,
//         Int_t stationId) const {
//      if (fStationPoints.find(detId)->second.count(stationId) > 0) {
//         return fStationPoints.find(detId)->second.find(stationId)->second.size();
//      } else return 0;
//   }

private:
   // map<detector id, vector of MC points>
   map<Int_t, vector<BmnMCPoint> > fPoints;

   // map<detector id, map<station id, vector of MC points> >
 //  map<Int_t, map<Int_t, vector<BmnMCPoint> > > fStationPoints;

   // Temporary set to store unique station indices for fast access
   // map<detector id, set<station index>>
  // map<Int_t, set<Int_t> > fStationIds;

   // temporary storage for maximum number of consecutive MC points
   // map<detector id, number of MC points>
  // map<Int_t, Int_t> fMaxConsecutivePoints;

private:

   Int_t MaxConsecutiveNumbers(
         const set<Int_t>& numbers) const {
      if (numbers.size() == 0) return 0;
      if (numbers.size() == 1) return 1;

      vector<Int_t> a(numbers.begin(), numbers.end());

      int maxCnt = 0;
      int cnt = 1;
      for (Int_t i = 0; i < a.size() - 1; i++) {
         if (a[i] == (a[i + 1] - 1)) {
            cnt++;
         } else {
            maxCnt = std::max(cnt, maxCnt);
            cnt = 1;
         }
      }
      maxCnt = std::max(cnt, maxCnt);
      return maxCnt;
   }

   string PointsToString(
         DetectorId detId,
         const string& detName) const {
      stringstream ss;
//      ss << detName << " np=" << GetNofPoints(detId) << " npds=" << GetNofPointsInDifferentStations(detId)
//    		  << " ncp=" << GetNofConsecutivePoints(detId) << " points=(";
//      for (Int_t i = 0; i < GetNofPoints(detId); i++) {
//         ss << ":" << GetPoint(detId, i).GetRefId() << ":" << GetPoint(detId, i).GetStationId() << ",";
//         //ss << GetPoint(detId, i);
//      }
//      ss << ") ";
      return ss.str();
   }

public:

   /**
    * \brief Returns string representation of the class.
    * \return Class representation as string.
    */
   virtual string ToString() const {
      stringstream ss;
      ss << "MCTrack: ";
      ss << PointsToString(kGEM, "STS") << "|";
      ss << PointsToString(kTOF1, "TOF1") << "|";
      ss << PointsToString(kDCH, "DCH1") << "|";
      ss << PointsToString(kTOF, "TOF") << "|";
      ss << endl;
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const BmnMCTrack& track) {
      strm << track.ToString();
      return strm;
   }
};

#endif /* BmnMCTRACK_H_ */
