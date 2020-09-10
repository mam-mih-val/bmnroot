////////////////////////////////////////////
//    BmnEventPull.h
//    BM@N Event Pull class declaration
//    Creates the Pull Sampler for BM@N experimental data (MPD raw format):
//    read raw dara from file, convert raw data to digits and send it to the event channel
//    Konstantin Gertsenberger
//    Created: Apr. 10 2017
////////////////////////////////////////////

#ifndef BMNEVENTPULL_H
#define BMNEVENTPULL_H

#define _GLIBCXX_USE_CXX11_ABI 0

#ifndef __MAKECINT__
  #include <emon/SelectionCriteria.h>
  #include <emon/EventSampler.h>
  #include <emon/EventChannel.h>
  #include <emon/PullSampling.h>
  #include <emon/PullSamplingFactory.h>
  #include <cmdl/cmdargs.h>
  #include <ipc/core.h>
#endif  // __MAKECINT__

#include "TString.h"

#include <string>
using namespace std;

class BmnEventPull
{
  public:
    /** Default constructor **/
    BmnEventPull();

    /** Standard constructor
    *@param partition_name  partition to work in
    *@param sampling_type   sampling address - sampler type
    *@param sampling_names  sampling address - sampler names
    *@param verbosity       verbosity-level: 0 - print nothing (default), 1 - print event number and event size? 2 - print event number, event size and event data
    **/
    BmnEventPull(TString partition_name, TString sampling_type, TString sampling_name, TString raw_data, int run_period,
                 int max_event = 0, int max_channels = 10, int wait_file_sec = 30, Int_t verbose = 0);

    /** Destructor **/
    virtual ~BmnEventPull();

    /** Set parameters for event pulling. **/
    void SetPartitionName(TString partition_name) {strPartitionName = partition_name;}
    void SetSamplingType(TString sampling_type) {strSamplingType = sampling_type;}
    void SetSamplingName(TString sampling_name) {strSamplingName = sampling_name;}

    void SetRawData(TString raw_data) {strRawData = raw_data;}
    void SetRunPeriod(int run_period) {iRunPeriod = run_period;}
    void SetMaxChannels(int max_channels) {iMaxChannels = max_channels;}
    void SetWaitFileSec(int wait_file_sec) {iWaitFileSec = wait_file_sec;}

    /** Set verbosity level. For this task and all of the subtasks. **/
    void SetVerbose(Int_t verbose) {iVerbose = verbose;}

    /** RunPullSampler - pull model of BM@N event sampler **/
    int Exec();

  protected:
    // partition name to work in
    TString strPartitionName;
    // address type
    TString strSamplingType;
    // sampler name
    TString strSamplingName;

    // directory with BM@N raw data files to be sent to monitor task
    TString strRawData;
    // run period corresponding raw data files
    Int_t   iRunPeriod;

    // maximum number of simultaneous sampling channels (default 100)
    Int_t   iMaxChannels;
    // maximum seconds to wait for a new file (default: 30 sec)
    Int_t   iWaitFileSec;

    // Verbosity level
    Int_t   iVerbose;

    ClassDef(BmnEventPull,1);
};

#endif
