/** @file BmnDaqBuffer.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 19.12.2018
 **/

#ifndef BMNDAQBUFFER_H
#define BMNDAQBUFFER_H 1

#include <map>
#include <string>
#include <vector>

#include "BmnDigi.h"

/** @class BmnDaqBuffer
 ** @date 19.12.2018
 ** @brief Singleton buffer class for BMN raw data
 **
 ** This class manages and provides access to buffers of
 ** type BmnMCPointBuffer - one for each detector.
 **
 ** The BmnDaqBuffer stores and sorts (w.r.t. time) BMN raw data
 ** (currently: BmnDigi) transiently.
 ** Data can be send to the buffer by the method InsertData.
 ** They can be retrieved by GetNextData, which delivers a
 ** time-ordered sequence of raw data objects for each detector.
 **
 ** The buffer handles objects only by pointer, i.e. the data have
 ** to be instantiated by the sending task (digitiser) and
 ** deleted by the receiving class (BmnDaq).
 **/
class BmnDaqBuffer
{
  public:

    /**   Destructor  **/
     ~BmnDaqBuffer();


    /** Time of first data for all detectors
     ** @return time of first data [ns]
     **/
    Double_t GetFirstTime() const;


    /** Time of first data for a given detector
     ** @param iDet  detector type (e.g. kSSD)
     ** @return time of first data [ns]
     */
    Double_t GetFirstTime(Int_t iDet) const;


    /** Time of last data for all detectors
     ** @return time of last data [ns]
     **/
    Double_t GetLastTime() const;


    /** Time of last data for a given detector
     ** @param iDet  detector type (e.g. kSSD)
     ** @return time of last data [ns]
     */
    Double_t GetLastTime(Int_t iDet) const;


    /** @brief Pointer to next raw data object for a given detector
     ** @param iDet  detector type (DetectorId)
     ** @return pointer to raw data object
     **/
    BmnDigi* GetNextData(Int_t iDet);


    /** Pointer to next raw data object for a given detector
     ** up to given time
     ** @param iDet  detector type (e.g. kSSD)
     ** @param time  maximal time [ns]
     ** @return pointer to raw data object
     **
     ** If the argument time is negative, no time limit is set.
     **/
    BmnDigi* GetNextData(Int_t iDet, Double_t time);


    /** Current buffer size
     ** @return number of objects in buffer
     */
    Int_t GetSize() const;


    /** Current buffer size for given detector
     ** @param  det  Detector system (e.g. kSSD)
     ** @return number of objects in buffer
     */
    Int_t GetSize(Int_t det) const;


    /** Insert data into the buffer
     ** @param digi  pointer to data object to be inserted
     **/
    void InsertData(BmnDigi* digi);


    /**   Access to singleton instance
     ** @return pointer to instance
     **/
    static BmnDaqBuffer* Instance();


    /**   Print buffer status  **/
    void PrintStatus() const;


    /**   Status string  **/
    std::string ToString() const;


  private:


    /** Buffer management **/
    std::multimap<Double_t, BmnDigi*> fData[kNOFDETS];


    /** Pointer to singleton instance **/
    static BmnDaqBuffer* fgInstance;


    /**  Default constructor
     **  Declared private to prevent instantiation.
     **/
    BmnDaqBuffer();


    /**  Copy constructor. Defined private to prevent usage. **/
    BmnDaqBuffer(const BmnDaqBuffer&) = delete;


    /**  Assignment operator. Defined private to prevent usage. **/
    BmnDaqBuffer& operator=(const BmnDaqBuffer&) = delete;
};

#endif /* BMNDAQBUFFER_H */
