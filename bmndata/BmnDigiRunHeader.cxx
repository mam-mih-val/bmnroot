#include "BmnDigiRunHeader.h"

BmnDigiRunHeader::BmnDigiRunHeader() :
 fPeriodId(0),
 fRunId(0)
{}


BmnDigiRunHeader::BmnDigiRunHeader(UInt_t period_id, UInt_t run_id, TTimeStamp run_start_time, TTimeStamp run_end_time) :
 fPeriodId(period_id),
 fRunId(run_id),
 fRunStartTime(run_start_time),
 fRunEndTime(run_end_time)
{
}

BmnDigiRunHeader::~BmnDigiRunHeader() {}


ClassImp(BmnDigiRunHeader)
