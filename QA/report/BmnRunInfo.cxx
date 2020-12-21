/* 
 * File:   BmnRunInfo.h
 * Author: ilnur
 *
 * Created on February 10 2018, 17:25
 */

#include "BmnRunInfo.h"

BmnRunInfo::BmnRunInfo() {
    i_period_number = 0;
    i_run_number = 0;
    str_file_path = "";
    str_beam_particle = "";
    str_target_particle = "";
    d_energy = 0;
    i_event_count = 0;
    d_field_voltage = 0;
    i_geometry_id = 0;
    i_trigger_id = 0;
}

BmnRunInfo::BmnRunInfo(UniDbRun* orig) {
    i_period_number = orig->GetPeriodNumber();
    i_run_number = orig->GetRunNumber();
    str_beam_particle = orig->GetBeamParticle();
    str_target_particle = orig->GetTargetParticle() ? *orig->GetTargetParticle() : "";
    d_energy = orig->GetEnergy() ? *orig->GetEnergy() : -1;
    //    dt_start_datetime = orig->GetStartDatetime();
    //    dt_end_datetime = orig->GetEndDatetime() ? *orig->GetEndDatetime() : dt_end_datetime;
    i_event_count = orig->GetEventCount() ? *orig->GetEventCount() : -1;
    d_field_voltage = orig->GetFieldVoltage() ? *orig->GetFieldVoltage() : -1;
    str_file_path = orig->GetFilePath();
    i_geometry_id = orig->GetGeometryId() ? *orig->GetGeometryId() : -1;
    //        i_trigger_id = orig->Get;

}

BmnRunInfo::~BmnRunInfo() {
}

ClassImp(BmnRunInfo);
