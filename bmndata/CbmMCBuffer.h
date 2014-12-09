/** @file CbmMCBuffer.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 8 February 2012
 **/


#ifndef CBMMCBUFFER_H
#define CBMMCBUFFER_H 1

//#define NDET 20

#include "CbmMCPointBuffer.h"
#include "CbmMvdPoint.h"
#include "CbmStsPoint.h"
#include "CbmTofPoint.h"
#include "CbmPsdPoint.h"


class TClonesArray;

using namespace std;


/** @class CbmMCBuffer
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 8 February 2012
 ** @brief Container class for buffers of CbmMCPoints.
 **
 ** This class manages and provides access to buffers of
 ** type CbmMCPointBuffer - one for each detector.
 **
 ** The buffers are filled with the content of a TClonesArray
 ** by the method Fill. Points can be accessed by the method
 ** GetNextPoint. This will deliver a time-ordered sequence
 ** of MCPoints for the respective detector with absolute time
 ** before the current event time. There should be a loop over
 ** this method until it returns a NULL pointer.
 **
 ** The buffered MCPoints are transient. When used, they will be marked
 ** and removed from the buffer at the end of the input event.
 **/
class CbmMCBuffer
{
 public:

  /**   Destructor  **/
  ~CbmMCBuffer();


  /**   Clear 
   ** Removes all used points from the buffers
   **/
  void Clear();


  /**   Get singleton instance  **/
  static CbmMCBuffer* Instance();


  /**   Fill buffer
   ** @param points     Pointer to TClonesArray with FairMCPoints
   ** @param det        Detector System (e.g. kSTS)
   ** @param eventId    ID of current event
   ** @param eventTime  Start time of event [ns]
   ** @value            Number of copied points
   **
   ** The FairMCPoints in the given TClonesArray are copied to the CbmMCBuffer.
   ** The event start time is added to the MCPoint time, the event ID is set.
   **/
  Int_t Fill(TClonesArray* points, DetectorId det, 
	     Int_t eventId, Double_t eventTime);
  Int_t Fill(TClonesArray* points, Int_t det, 
	     Int_t eventId, Double_t eventTime);


  /**  Current time 
   ** @value Start time of last event read into the buffer [ns]
   **/
  Double_t GetTime() const { return fTime; }


  /**  Current event ID
   ** @value ID of last event read into the buffer
   **/
  Int_t GetEventId() const { return fEventId; }
  

  /**   Current size
   ** @value Current buffer size [MB]
   **/
  Double_t GetSize() const;


  /**   Get time of last data in all buffers
   ** @value Time of last MCPoint [ns]
   */
  Double_t GetMaxTime() const;


  /**   Get time of first data in all buffers
   ** @value Time of first MCPoint [ns]
   */
  Double_t GetMinTime() const;


  /**   Get next MCPoint from buffer
   ** @param det    DetectorId (e.g. kSTS)
   ** @value point  Pointer to MCPoint
   **
   ** Returns a pointer to the next (w.r.t. absolute time) MCPoint
   ** of the respective detector in its buffer.
   ** Points are released up to the start time of the last event read
   ** After that, a NULL pointer is returned.
   ** After retrieval, the MCPoint is removed from the buffer.
   **/
  const FairMCPoint* GetNextPoint(DetectorId det);


  /**   Get number of entries
   ** @value Total number of entries in all buffers
   */
  Int_t GetNofEntries() const;


  /**   Screen output 
   ** Reports the current buffer status.
   ** @param option L for info on all buffers
   **/
  void Print(const char* option = "") const;


  /** Set the end-of-run flag
   ** This will triggetr to deliver all points from the buffer
   ** through GetNextPoint, irrespective of the last event time.
   ** @param flag  kTRUE (default) if end of run
   **/
  void SetEndOfRun(Bool_t flag = kTRUE) { fEndOfRun = flag; }

 private:

  /** MCPoint buffers **/
  CbmMCPointBuffer<CbmMvdPoint>   fMvdBuffer; 
  CbmMCPointBuffer<CbmStsPoint>   fGemBuffer;
  CbmMCPointBuffer<CbmTofPoint>   fTofBuffer; 
  CbmMCPointBuffer<CbmPsdPoint>   fZdcBuffer;


  /** Start time of last event read into the buffer **/
  Double_t fTime;


  /** ID of last event read into the buffer  **/
  Int_t fEventId;


  /** End-of-run flag  **/
  Bool_t fEndOfRun;


  /** Pointer to singleton instance **/
  static CbmMCBuffer* fgInstance;


  /**  Default constructor
   **  Declared private to prevent instantiation.
   **/
  CbmMCBuffer();


  /**  Copy constructor. Defined private to prevent usage. **/
  CbmMCBuffer(const CbmMCBuffer&);


  /**  Assignment operator. Defined private to prevent usage. **/
  CbmMCBuffer& operator=(const CbmMCBuffer&);
};

#endif
