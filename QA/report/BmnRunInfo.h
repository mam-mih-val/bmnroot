#ifndef BMNRUNINFO_H
#define BMNRUNINFO_H

#include "UniRun.h"

class BmnRunInfo : public TObject {
public:
    BmnRunInfo();
    BmnRunInfo(UniRun* orig);
    virtual ~BmnRunInfo();// Getters
	/// get period number of the current run
	Int_t GetPeriodNumber() {return i_period_number;}
	/// get run number of the current run
	Int_t GetRunNumber() {return i_run_number;}
	/// get file path of the current run
	TString GetFilePath() {return str_file_path;}
	/// get beam particle of the current run
	TString GetBeamParticle() {return str_beam_particle;}
	/// get target particle of the current run
	TString GetTargetParticle() {return str_target_particle;}
	/// get energy of the current run
	Double_t GetEnergy() {return d_energy;}
//	/// get start datetime of the current run
//	TDatime GetStartDatetime() {return dt_start_datetime;}
//	/// get end datetime of the current run
//	TDatime GetEndDatetime() {return dt_end_datetime;}
	/// get event count of the current run
	Int_t GetEventCount() {return i_event_count;}
	/// get field voltage of the current run
	Double_t GetFieldVoltage() {return d_field_voltage;}
	/// get geometry id of the current run
	Int_t GetGeometryId() {return i_geometry_id;}

	// Setters
	/// set period number of the current run
    void SetPeriodNumber(Int_t period_number){
            i_period_number = period_number;
        }
	/// set run number of the current run
    void SetRunNumber(Int_t run_number){
            i_run_number = run_number;
        }
	/// set file path of the current run
    void SetFilePath(TString file_path){
            str_file_path = file_path;
        }
	/// set beam particle of the current run
    void SetBeamParticle(TString beam_particle){
            str_beam_particle = beam_particle;
        }
	/// set target particle of the current run
    void SetTargetParticle(TString target_particle){
            str_target_particle = target_particle;
        }
	/// set energy of the current run
    void SetEnergy(Double_t energy){
            d_energy = energy;
        }
//	/// set start datetime of the current run
//	void SetStartDatetime(TDatime start_datetime){
//            dt_start_datetime = start_datetime;
//        }
//	/// set end datetime of the current run
//	void SetEndDatetime(TDatime end_datetime){
//            dt_end_datetime = end_datetime;
//        }
	/// set event count of the current run
    void SetEventCount(Int_t event_count){
            i_event_count = event_count;
        }
	/// set field voltage of the current run
    void SetFieldVoltage(Double_t field_voltage){
            d_field_voltage = field_voltage;
        }
	/// set geometry id of the current run
    void SetGeometryId(Int_t geometry_id){
            i_geometry_id = geometry_id;
        }
	/// set geometry id of the current run
    void SetTriggerId(Int_t trigger_id){
            i_trigger_id = trigger_id;
        }
private:
	/// period number
	Int_t i_period_number;
	/// run number
	Int_t i_run_number;
	/// file path
	TString str_file_path;
	/// beam particle
	TString str_beam_particle;
	/// target particle
	TString str_target_particle;
	/// energy
	Double_t d_energy;
	/// start datetime
	//TDatime dt_start_datetime;
	/// end datetime
	//TDatime dt_end_datetime;
	/// event count
	Int_t i_event_count;
	/// field voltage
	Double_t d_field_voltage;
	/// geometry id
	Int_t i_geometry_id;
	/// trigger id
	Int_t i_trigger_id;
        
    ClassDef(BmnRunInfo, 1)

};

#endif /* BMNRUNINFO_H */

