////////////////////////////////////////////
//    BmnEventMonitor.h
//    BM@N Event Monitoring Task declaration
//    Konstantin Gertsenberger
//    Created: Oct. 11 2016
////////////////////////////////////////////

#ifndef BMNEVENTMONITOR_H
#define BMNEVENTMONITOR_H

#ifndef __MAKECINT__
  #include <emon/EventIterator.h>
  #include <owl/timer.h>
  #include <cmdl/cmdargs.h>
  #include <ipc/core.h>
#endif  // __MAKECINT__

#include <TString.h>

#include <vector>

class BmnEventMonitor
{
  public:
    /** Default constructor **/
    BmnEventMonitor();

    /** Standard constructor
    *@param partition_name  partition to work in
    *@param sampling_type   sampling address - sampler type
    *@param sampling_names  sampling address - sampler names
    *@param verbosity       verbosity-level: 0 - print nothing (default), 1 - print event number and event size? 2 - print event number, event size and event data
    **/
    BmnEventMonitor(TString partition_name, TString sampling_type, TString sampling_names, Int_t verbose = 0);

    /** Destructor **/
    virtual ~BmnEventMonitor() {}

    /** Set verbosity level. For this task and all of the subtasks. **/
    void SetVerbose(Int_t verbose) {iVerbose = verbose;}
    void SetPartitionName(TString partition_name) {strPartitionName = partition_name;}
    void SetSamplingType(TString sampling_type) {strSamplingType = sampling_type;}
    void SetSamplingName(TString sampling_name) {strSamplingName = sampling_name;}

    /** RunMonitor **/
    int Exec();

  protected:
    TString strPartitionName;
    TString strSamplingType;
    TString strSamplingName;
    // Verbosity level
    Int_t   iVerbose;

    // Operational parameters
    // use event dispersion
    Bool_t	dispersion;
    // delay in ms before another connection attempt (default 10000)
    Int_t	wait;
    // number of events to retrieve (default 10000), -1 means work forever
    Int_t	events;

    // maximum number of events in the buffer (default 1000)), minimum value is 1
    Int_t	buffer_size;
    // number of repetitions for the test (default 1), minimum value is 1
    Int_t	repetitions;
    // 1 if monitor should retrieve events asynchronous (default 0)
    Int_t	async;
    // timeout for the nextEvent call in synchronous mode (default 10000 ms)
    Int_t	timeout;

    ClassDef(BmnEventMonitor,1);
};

#endif
