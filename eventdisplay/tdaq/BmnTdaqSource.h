////////////////////////////////////////////
//    BmnTdaqSource.h
//    BM@N TDAQ Source declaration
//    Konstantin Gertsenberger
//    Created: Apr. 18 2017
////////////////////////////////////////////

#ifndef BMNTDAQSOURCE_H
#define BMNTDAQSOURCE_H

#define _GLIBCXX_USE_CXX11_ABI 0

#ifndef __MAKECINT__
  #include <emon/EventIterator.h>
  #include <owl/timer.h>
  #include <cmdl/cmdargs.h>
  #include <ipc/core.h>
#endif  // __MAKECINT__

#include "FairOnlineSource.h"

#include <TString.h>

class BmnTdaqSource : public FairOnlineSource
{
  public:
    /** Default constructor **/
    BmnTdaqSource();
    /** Standard constructor
    *@param partition_name  partition to work in
    *@param sampling_type   sampling address - sampler type
    *@param sampling_names  sampling address - sampler names
    *@param max_events      maximum event to receive from Event Sampler
    *@param verbosity       verbosity-level: 0 - print nothing (default), 1 - print event number and event size? 2 - print event number, event size and event data
    **/
    BmnTdaqSource(TString partition_name, TString sampling_type, TString sampling_names, Int_t verbose = 0);
    BmnTdaqSource(const BmnTdaqSource& source) = delete;
    /** Destructor **/
    virtual ~BmnTdaqSource();

    virtual Bool_t Init();
    virtual Int_t ReadEvent(UInt_t = 0);
    virtual void Close();

    /** Set parameters for event receiving. **/
    void SetPartitionName(TString partition_name) {strPartitionName = partition_name;}
    void SetSamplingType(TString sampling_type) {strSamplingType = sampling_type;}
    void SetSamplingName(TString sampling_name) {strSamplingName = sampling_name;}
    /** Set verbosity level. For this task and all of the subtasks. **/
    virtual void SetVerbose(Int_t verbose) {iVerbose = verbose;}

    /** Get parameters for event receiving. **/
    TString GetPartitionName() const {return strPartitionName;}
    TString GetSamplingType() const {return strSamplingType;}
    TString GetSamplingName() const {return strSamplingName;}
    /** Get verbosity level. **/
    Int_t GetVerbose() const {return iVerbose;}

  private:
    // partition name to work in
    TString strPartitionName;
    // address type
    TString strSamplingType;
    // sampler name
    TString strSamplingName;

    // use event dispersion
    Bool_t isDispersion;
    // delay in ms before another connection attempt (default 10000)
    Int_t	iWait;
    // number of repetitions for the test (default 1), minimum value is 1
    Int_t	iRepetitions;
    // maximum number of events in the buffer (default 1000)), minimum value is 1
    Int_t	iBufferSize;
    // 1 if monitor should retrieve events asynchronous (default 0)
    Int_t	iAsync;
    // timeout for the nextEvent call in synchronous mode (default 10000 ms)
    Int_t	iTimeout;

    // Verbosity level
    Int_t   iVerbose;

    // current event number (event count received from TDAQ)
    int     iEventNumber;

    // Event Header in TClonesArray
    TClonesArray* fEventHeader;     //!
    // array for gem digits to transfer it to the next tasks
    TClonesArray* fGemDigits;       //!
    // array for tof-400 digits to transfer it to the next tasks
    TClonesArray* fTof1Digits;      //!
    // array for T0 to transfer it to the next tasks
    TClonesArray* fT0Digits;        //!

    BmnTdaqSource& operator=(const BmnTdaqSource&) = delete;
    
    ClassDef(BmnTdaqSource, 0)
};

#endif
