/** @file CbmDaqBuffer.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 17 July 2012
 **/

#ifndef CBMDAQBUFFER_H
#define CBMDAQBUFFER_H 1

#include <map>
#include <vector>

#include "CbmDetectorList.h"
#include "CbmDigi.h"

using namespace std;



/** @class CbmDaqBuffer
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 20 July 2012
 ** @brief Singleton buffer class for CBM raw data
 **
 ** This class manages and provides access to buffers of
 ** type CbmMCPointBuffer - one for each detector.
 **
 ** The CbmDaqBuffer stores and sorts (w.r.t. time) CBM raw data
 ** (currently: CbmDigi) transiently.
 ** Data can be send to the buffer by the method InsertData.
 ** They can be retrieved by GetNextData, which delivers a
 ** time-ordered sequence of raw data objects for each detector.
 **
 ** The buffer handles objects only by pointer, i.e. the data have
 ** to be instantiated by the sending task (digitiser) and
 ** deleted by the receiving class (CbmDaq).
 **/
class CbmDaqBuffer
{
  public:

    /**   Destructor  **/
     ~CbmDaqBuffer();


    /** Time of first data for all detectors
     ** @return time of first data [ns]
     **/
    Double_t GetFirstTime() const;


    /** Time of first data for a given detector
     ** @param iDet  detector type (e.g. kSTS)
     ** @return time of first data [ns]
     */
    Double_t GetFirstTime(Int_t iDet) const {
      if ( iDet < kREF || iDet >= kNOFDETS ) return -1.;
      if ( ! GetSize(iDet) ) return -1.;
      return (fData[iDet].begin())->second->GetTime();
    }


    /** Time of last data for all detectors
     ** @return time of last data [ns]
     **/
    Double_t GetLastTime() const;


    /** Time of last data for a given detector
     ** @param iDet  detector type (e.g. kSTS)
     ** @return time of last data [ns]
     */
    Double_t GetLastTime(Int_t iDet) const {
      if ( iDet < kREF || iDet >= kNOFDETS ) return -1.;
      if ( ! GetSize(iDet) ) return -1.;
      return (--fData[iDet].end())->second->GetTime();
    }

    /** Pointer to next raw data object for a given detector
     ** up to given time
     ** @param iDet  detector type (e.g. kSTS)
     ** @param time  maximal time [ns]
     ** @return pointer to raw data object
     **/
    CbmDigi* GetNextData(Int_t iDet, Double_t time);


    /** Current buffer size
     ** @return number of objects in buffer
     */
    Int_t GetSize() const;


    /** Current buffer size for given detector
     ** @param  det  Detector system (e.g. kSTS)
     ** @return number of objects in buffer
     */
    Int_t GetSize(Int_t det) const;


    /** Insert data into the buffer
     ** @param digi  pointer to data object to be inserted
     **/
    void InsertData(CbmDigi* digi);


    /**   Access to singleton instance
     ** @return pointer to instance
     **/
    static CbmDaqBuffer* Instance();


    /**   Print buffer status  **/
    void PrintStatus() const;


  private:


    /** Buffer management **/
    multimap<Double_t, CbmDigi*> fData[kNOFDETS];


    /** Pointer to singleton instance **/
    static CbmDaqBuffer* fgInstance;


    /**  Default constructor
     **  Declared private to prevent instantiation.
     **/
    CbmDaqBuffer();


    /**  Copy constructor. Defined private to prevent usage. **/
    CbmDaqBuffer(const CbmDaqBuffer&);


    /**  Assignment operator. Defined private to prevent usage. **/
    CbmDaqBuffer& operator=(const CbmDaqBuffer&);



};

#endif /* CBMDAQBUFFER_H */
