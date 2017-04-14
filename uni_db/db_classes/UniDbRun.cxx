// ----------------------------------------------------------------------
//                    UniDbRun cxx file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

#include "TSystem.h"
#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "UniDbRun.h"
#include "UniDbRunGeometry.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniDbRun::UniDbRun(UniDbConnection* connUniDb, int period_number, int run_number, TString file_path, TString beam_particle, TString* target_particle, double* energy, TDatime start_datetime, TDatime* end_datetime, int* event_count, double* field_voltage, double* file_size, int* geometry_id)
{
	connectionUniDb = connUniDb;

	i_period_number = period_number;
	i_run_number = run_number;
	str_file_path = file_path;
	str_beam_particle = beam_particle;
	str_target_particle = target_particle;
	d_energy = energy;
	dt_start_datetime = start_datetime;
	dt_end_datetime = end_datetime;
	i_event_count = event_count;
	d_field_voltage = field_voltage;
	d_file_size = file_size;
	i_geometry_id = geometry_id;
}

// -----   Destructor   -------------------------------------------------
UniDbRun::~UniDbRun()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (str_target_particle)
		delete str_target_particle;
	if (d_energy)
		delete d_energy;
	if (dt_end_datetime)
		delete dt_end_datetime;
	if (i_event_count)
		delete i_event_count;
	if (d_field_voltage)
		delete d_field_voltage;
	if (d_file_size)
		delete d_file_size;
	if (i_geometry_id)
		delete i_geometry_id;
}

// -----   Creating new run in the database  ---------------------------
UniDbRun* UniDbRun::CreateRun(int period_number, int run_number, TString file_path, TString beam_particle, TString* target_particle, double* energy, TDatime start_datetime, TDatime* end_datetime, int* event_count, double* field_voltage, double* file_size, int* geometry_id)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into run_(period_number, run_number, file_path, beam_particle, target_particle, energy, start_datetime, end_datetime, event_count, field_voltage, file_size, geometry_id) "
		"values ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, period_number);
	stmt->SetInt(1, run_number);
	stmt->SetString(2, file_path);
	stmt->SetString(3, beam_particle);
	if (target_particle == NULL)
		stmt->SetNull(4);
	else
		stmt->SetString(4, *target_particle);
	if (energy == NULL)
		stmt->SetNull(5);
	else
		stmt->SetDouble(5, *energy);
	stmt->SetDatime(6, start_datetime);
	if (end_datetime == NULL)
		stmt->SetNull(7);
	else
		stmt->SetDatime(7, *end_datetime);
	if (event_count == NULL)
		stmt->SetNull(8);
	else
		stmt->SetInt(8, *event_count);
	if (field_voltage == NULL)
		stmt->SetNull(9);
	else
		stmt->SetDouble(9, *field_voltage);
	if (file_size == NULL)
		stmt->SetNull(10);
	else
		stmt->SetDouble(10, *file_size);
	if (geometry_id == NULL)
		stmt->SetNull(11);
	else
		stmt->SetInt(11, *geometry_id);

	// inserting new run to the Database
	if (!stmt->Process())
	{
		cout<<"Error: inserting new run to the Database has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	int tmp_period_number;
	tmp_period_number = period_number;
	int tmp_run_number;
	tmp_run_number = run_number;
	TString tmp_file_path;
	tmp_file_path = file_path;
	TString tmp_beam_particle;
	tmp_beam_particle = beam_particle;
	TString* tmp_target_particle;
	if (target_particle == NULL) tmp_target_particle = NULL;
	else
		tmp_target_particle = new TString(*target_particle);
	double* tmp_energy;
	if (energy == NULL) tmp_energy = NULL;
	else
		tmp_energy = new double(*energy);
	TDatime tmp_start_datetime;
	tmp_start_datetime = start_datetime;
	TDatime* tmp_end_datetime;
	if (end_datetime == NULL) tmp_end_datetime = NULL;
	else
		tmp_end_datetime = new TDatime(*end_datetime);
	int* tmp_event_count;
	if (event_count == NULL) tmp_event_count = NULL;
	else
		tmp_event_count = new int(*event_count);
	double* tmp_field_voltage;
	if (field_voltage == NULL) tmp_field_voltage = NULL;
	else
		tmp_field_voltage = new double(*field_voltage);
	double* tmp_file_size;
	if (file_size == NULL) tmp_file_size = NULL;
	else
		tmp_file_size = new double(*file_size);
	int* tmp_geometry_id;
	if (geometry_id == NULL) tmp_geometry_id = NULL;
	else
		tmp_geometry_id = new int(*geometry_id);

	return new UniDbRun(connUniDb, tmp_period_number, tmp_run_number, tmp_file_path, tmp_beam_particle, tmp_target_particle, tmp_energy, tmp_start_datetime, tmp_end_datetime, tmp_event_count, tmp_field_voltage, tmp_file_size, tmp_geometry_id);
}

// -----  Get run from the database  ---------------------------
UniDbRun* UniDbRun::GetRun(int period_number, int run_number)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select period_number, run_number, file_path, beam_particle, target_particle, energy, start_datetime, end_datetime, event_count, field_voltage, file_size, geometry_id "
		"from run_ "
		"where period_number = %d and run_number = %d", period_number, run_number);
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get run from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting run from the database has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"Error: run wasn't found in the database"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	int tmp_period_number;
	tmp_period_number = stmt->GetInt(0);
	int tmp_run_number;
	tmp_run_number = stmt->GetInt(1);
	TString tmp_file_path;
	tmp_file_path = stmt->GetString(2);
	TString tmp_beam_particle;
	tmp_beam_particle = stmt->GetString(3);
	TString* tmp_target_particle;
	if (stmt->IsNull(4)) tmp_target_particle = NULL;
	else
		tmp_target_particle = new TString(stmt->GetString(4));
	double* tmp_energy;
	if (stmt->IsNull(5)) tmp_energy = NULL;
	else
		tmp_energy = new double(stmt->GetDouble(5));
	TDatime tmp_start_datetime;
	tmp_start_datetime = stmt->GetDatime(6);
	TDatime* tmp_end_datetime;
	if (stmt->IsNull(7)) tmp_end_datetime = NULL;
	else
		tmp_end_datetime = new TDatime(stmt->GetDatime(7));
	int* tmp_event_count;
	if (stmt->IsNull(8)) tmp_event_count = NULL;
	else
		tmp_event_count = new int(stmt->GetInt(8));
	double* tmp_field_voltage;
	if (stmt->IsNull(9)) tmp_field_voltage = NULL;
	else
		tmp_field_voltage = new double(stmt->GetDouble(9));
	double* tmp_file_size;
	if (stmt->IsNull(10)) tmp_file_size = NULL;
	else
		tmp_file_size = new double(stmt->GetDouble(10));
	int* tmp_geometry_id;
	if (stmt->IsNull(11)) tmp_geometry_id = NULL;
	else
		tmp_geometry_id = new int(stmt->GetInt(11));

	delete stmt;

	return new UniDbRun(connUniDb, tmp_period_number, tmp_run_number, tmp_file_path, tmp_beam_particle, tmp_target_particle, tmp_energy, tmp_start_datetime, tmp_end_datetime, tmp_event_count, tmp_field_voltage, tmp_file_size, tmp_geometry_id);
}

// -----  Get run from the database by unique key  --------------
UniDbRun* UniDbRun::GetRun(TString file_path)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select period_number, run_number, file_path, beam_particle, target_particle, energy, start_datetime, end_datetime, event_count, field_voltage, file_size, geometry_id "
		"from run_ "
		"where lower(file_path) = lower('%s')", file_path.Data());
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get run from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting run from the database has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"Error: run wasn't found in the database"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	int tmp_period_number;
	tmp_period_number = stmt->GetInt(0);
	int tmp_run_number;
	tmp_run_number = stmt->GetInt(1);
	TString tmp_file_path;
	tmp_file_path = stmt->GetString(2);
	TString tmp_beam_particle;
	tmp_beam_particle = stmt->GetString(3);
	TString* tmp_target_particle;
	if (stmt->IsNull(4)) tmp_target_particle = NULL;
	else
		tmp_target_particle = new TString(stmt->GetString(4));
	double* tmp_energy;
	if (stmt->IsNull(5)) tmp_energy = NULL;
	else
		tmp_energy = new double(stmt->GetDouble(5));
	TDatime tmp_start_datetime;
	tmp_start_datetime = stmt->GetDatime(6);
	TDatime* tmp_end_datetime;
	if (stmt->IsNull(7)) tmp_end_datetime = NULL;
	else
		tmp_end_datetime = new TDatime(stmt->GetDatime(7));
	int* tmp_event_count;
	if (stmt->IsNull(8)) tmp_event_count = NULL;
	else
		tmp_event_count = new int(stmt->GetInt(8));
	double* tmp_field_voltage;
	if (stmt->IsNull(9)) tmp_field_voltage = NULL;
	else
		tmp_field_voltage = new double(stmt->GetDouble(9));
	double* tmp_file_size;
	if (stmt->IsNull(10)) tmp_file_size = NULL;
	else
		tmp_file_size = new double(stmt->GetDouble(10));
	int* tmp_geometry_id;
	if (stmt->IsNull(11)) tmp_geometry_id = NULL;
	else
		tmp_geometry_id = new int(stmt->GetInt(11));

	delete stmt;

	return new UniDbRun(connUniDb, tmp_period_number, tmp_run_number, tmp_file_path, tmp_beam_particle, tmp_target_particle, tmp_energy, tmp_start_datetime, tmp_end_datetime, tmp_event_count, tmp_field_voltage, tmp_file_size, tmp_geometry_id);
}

// -----  Check run exists in the database  ---------------------------
bool UniDbRun::CheckRunExists(int period_number, int run_number)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from run_ "
		"where period_number = %d and run_number = %d", period_number, run_number);
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get run from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting run from the database has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return false;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		delete stmt;
		delete connUniDb;
		return false;
	}

	delete stmt;
	delete connUniDb;

	return true;
}

// -----  Check run exists in the database by unique key  --------------
bool UniDbRun::CheckRunExists(TString file_path)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from run_ "
		"where lower(file_path) = lower('%s')", file_path.Data());
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get run from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting run from the database has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return false;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		delete stmt;
		delete connUniDb;
		return false;
	}

	delete stmt;
	delete connUniDb;

	return true;
}

// -----  Delete run from the database  ---------------------------
int UniDbRun::DeleteRun(int period_number, int run_number)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from run_ "
		"where period_number = $1 and run_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, period_number);
	stmt->SetInt(1, run_number);

	// delete run from the dataBase
	if (!stmt->Process())
	{
		cout<<"Error: deleting run from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	delete stmt;
	delete connUniDb;
	return 0;
}

// -----  Delete run from the database by unique key  --------------
int UniDbRun::DeleteRun(TString file_path)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from run_ "
		"where lower(file_path) = lower($1)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, file_path);

	// delete run from the dataBase
	if (!stmt->Process())
	{
		cout<<"Error: deleting run from the DataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	delete stmt;
	delete connUniDb;
	return 0;
}

// -----  Print all 'runs'  ---------------------------------
int UniDbRun::PrintAll()
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select period_number, run_number, file_path, beam_particle, target_particle, energy, start_datetime, end_datetime, event_count, field_voltage, file_size, geometry_id "
		"from run_");
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get all 'runs' from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting all 'runs' from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// print rows
	cout<<"Table 'run_':"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<"period_number: ";
		cout<<(stmt->GetInt(0));
		cout<<", run_number: ";
		cout<<(stmt->GetInt(1));
		cout<<", file_path: ";
		cout<<(stmt->GetString(2));
		cout<<", beam_particle: ";
		cout<<(stmt->GetString(3));
		cout<<", target_particle: ";
		if (stmt->IsNull(4)) cout<<"NULL";
		else
			cout<<stmt->GetString(4);
		cout<<", energy: ";
		if (stmt->IsNull(5)) cout<<"NULL";
		else
			cout<<stmt->GetDouble(5);
		cout<<", start_datetime: ";
		cout<<(stmt->GetDatime(6)).AsSQLString();
		cout<<", end_datetime: ";
		if (stmt->IsNull(7)) cout<<"NULL";
		else
			cout<<stmt->GetDatime(7).AsSQLString();
		cout<<", event_count: ";
		if (stmt->IsNull(8)) cout<<"NULL";
		else
			cout<<stmt->GetInt(8);
		cout<<", field_voltage: ";
		if (stmt->IsNull(9)) cout<<"NULL";
		else
			cout<<stmt->GetDouble(9);
		cout<<", file_size: ";
		if (stmt->IsNull(10)) cout<<"NULL";
		else
			cout<<stmt->GetDouble(10);
		cout<<", geometry_id: ";
		if (stmt->IsNull(11)) cout<<"NULL";
		else
			cout<<stmt->GetInt(11);
		cout<<"."<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int UniDbRun::SetPeriodNumber(int period_number)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_ "
		"set period_number = $1 "
		"where period_number = $2 and run_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, period_number);
	stmt->SetInt(1, i_period_number);
	stmt->SetInt(2, i_run_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_period_number = period_number;

	delete stmt;
	return 0;
}

int UniDbRun::SetRunNumber(int run_number)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_ "
		"set run_number = $1 "
		"where period_number = $2 and run_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, run_number);
	stmt->SetInt(1, i_period_number);
	stmt->SetInt(2, i_run_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_run_number = run_number;

	delete stmt;
	return 0;
}

int UniDbRun::SetFilePath(TString file_path)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_ "
		"set file_path = $1 "
		"where period_number = $2 and run_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, file_path);
	stmt->SetInt(1, i_period_number);
	stmt->SetInt(2, i_run_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_file_path = file_path;

	delete stmt;
	return 0;
}

int UniDbRun::SetBeamParticle(TString beam_particle)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_ "
		"set beam_particle = $1 "
		"where period_number = $2 and run_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, beam_particle);
	stmt->SetInt(1, i_period_number);
	stmt->SetInt(2, i_run_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_beam_particle = beam_particle;

	delete stmt;
	return 0;
}

int UniDbRun::SetTargetParticle(TString* target_particle)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_ "
		"set target_particle = $1 "
		"where period_number = $2 and run_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (target_particle == NULL)
		stmt->SetNull(0);
	else
		stmt->SetString(0, *target_particle);
	stmt->SetInt(1, i_period_number);
	stmt->SetInt(2, i_run_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (str_target_particle)
		delete str_target_particle;
	if (target_particle == NULL) str_target_particle = NULL;
	else
		str_target_particle = new TString(*target_particle);

	delete stmt;
	return 0;
}

int UniDbRun::SetEnergy(double* energy)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_ "
		"set energy = $1 "
		"where period_number = $2 and run_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (energy == NULL)
		stmt->SetNull(0);
	else
		stmt->SetDouble(0, *energy);
	stmt->SetInt(1, i_period_number);
	stmt->SetInt(2, i_run_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (d_energy)
		delete d_energy;
	if (energy == NULL) d_energy = NULL;
	else
		d_energy = new double(*energy);

	delete stmt;
	return 0;
}

int UniDbRun::SetStartDatetime(TDatime start_datetime)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_ "
		"set start_datetime = $1 "
		"where period_number = $2 and run_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetDatime(0, start_datetime);
	stmt->SetInt(1, i_period_number);
	stmt->SetInt(2, i_run_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run has been failed"<<endl;

		delete stmt;
		return -2;
	}

	dt_start_datetime = start_datetime;

	delete stmt;
	return 0;
}

int UniDbRun::SetEndDatetime(TDatime* end_datetime)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_ "
		"set end_datetime = $1 "
		"where period_number = $2 and run_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (end_datetime == NULL)
		stmt->SetNull(0);
	else
		stmt->SetDatime(0, *end_datetime);
	stmt->SetInt(1, i_period_number);
	stmt->SetInt(2, i_run_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (dt_end_datetime)
		delete dt_end_datetime;
	if (end_datetime == NULL) dt_end_datetime = NULL;
	else
		dt_end_datetime = new TDatime(*end_datetime);

	delete stmt;
	return 0;
}

int UniDbRun::SetEventCount(int* event_count)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_ "
		"set event_count = $1 "
		"where period_number = $2 and run_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (event_count == NULL)
		stmt->SetNull(0);
	else
		stmt->SetInt(0, *event_count);
	stmt->SetInt(1, i_period_number);
	stmt->SetInt(2, i_run_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (i_event_count)
		delete i_event_count;
	if (event_count == NULL) i_event_count = NULL;
	else
		i_event_count = new int(*event_count);

	delete stmt;
	return 0;
}

int UniDbRun::SetFieldVoltage(double* field_voltage)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_ "
		"set field_voltage = $1 "
		"where period_number = $2 and run_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (field_voltage == NULL)
		stmt->SetNull(0);
	else
		stmt->SetDouble(0, *field_voltage);
	stmt->SetInt(1, i_period_number);
	stmt->SetInt(2, i_run_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (d_field_voltage)
		delete d_field_voltage;
	if (field_voltage == NULL) d_field_voltage = NULL;
	else
		d_field_voltage = new double(*field_voltage);

	delete stmt;
	return 0;
}

int UniDbRun::SetFileSize(double* file_size)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_ "
		"set file_size = $1 "
		"where period_number = $2 and run_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (file_size == NULL)
		stmt->SetNull(0);
	else
		stmt->SetDouble(0, *file_size);
	stmt->SetInt(1, i_period_number);
	stmt->SetInt(2, i_run_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (d_file_size)
		delete d_file_size;
	if (file_size == NULL) d_file_size = NULL;
	else
		d_file_size = new double(*file_size);

	delete stmt;
	return 0;
}

int UniDbRun::SetGeometryId(int* geometry_id)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_ "
		"set geometry_id = $1 "
		"where period_number = $2 and run_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (geometry_id == NULL)
		stmt->SetNull(0);
	else
		stmt->SetInt(0, *geometry_id);
	stmt->SetInt(1, i_period_number);
	stmt->SetInt(2, i_run_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (i_geometry_id)
		delete i_geometry_id;
	if (geometry_id == NULL) i_geometry_id = NULL;
	else
		i_geometry_id = new int(*geometry_id);

	delete stmt;
	return 0;
}

// -----  Print current run  ---------------------------------------
void UniDbRun::Print()
{
	cout<<"Table 'run_'";
	cout<<". period_number: "<<i_period_number<<". run_number: "<<i_run_number<<". file_path: "<<str_file_path<<". beam_particle: "<<str_beam_particle<<". target_particle: "<<(str_target_particle == NULL? "NULL": *str_target_particle)<<". energy: "<<(d_energy == NULL? "NULL": TString::Format("%f", *d_energy))<<". start_datetime: "<<dt_start_datetime.AsSQLString()<<". end_datetime: "<<(dt_end_datetime == NULL? "NULL": (*dt_end_datetime).AsSQLString())<<". event_count: "<<(i_event_count == NULL? "NULL": TString::Format("%d", *i_event_count))<<". field_voltage: "<<(d_field_voltage == NULL? "NULL": TString::Format("%f", *d_field_voltage))<<". file_size: "<<(d_file_size == NULL? "NULL": TString::Format("%f", *d_file_size))<<". geometry_id: "<<(i_geometry_id == NULL? "NULL": TString::Format("%d", *i_geometry_id))<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// get numbers of runs existing in the Database for a selected range
int UniDbRun::GetRunNumbers(int start_period, int start_run, int end_period, int end_run, UniqueRunNumber*& run_numbers)
{
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
    {
        cout<<"Error: connection to DB was failed"<<endl;
        return -1;
    }

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
        "select period_number, run_number "
        "from run_ "
        "where (not (((%d < period_number) or ((%d = period_number) and (%d < run_number))) or ((%d > period_number) or ((%d = period_number) and (%d > run_number))))) "
        "order by period_number, run_number", end_period, end_period, end_run, start_period, start_period, start_run);
    TSQLStatement* stmt = uni_db->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"Error: getting run numbers from DB has been failed"<<endl;
        delete stmt;
        delete connUniDb;
        return -2;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    vector<int> vecPeriods;
    vector<int> vecRuns;
    while (stmt->NextResultRow())
    {
        vecPeriods.push_back(stmt->GetInt(0));
        vecRuns.push_back(stmt->GetInt(1));
    }

    delete stmt;
    delete connUniDb;

    int run_count = vecPeriods.size();
    run_numbers = new UniqueRunNumber[run_count];
    for (int i = 0; i < run_count; i++)
    {
        run_numbers[i].period_number = vecPeriods[i];
        run_numbers[i].run_number = vecRuns[i];
    }

    return run_count;
}

// get numbers of existing runs in the Database
int UniDbRun::GetRunNumbers(UniqueRunNumber*& run_numbers)
{
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
    {
        cout<<"Error: connection to DB was failed"<<endl;
        return -1;
    }

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
        "select period_number, run_number "
        "from run_ "
        "order by period_number, run_number");
    TSQLStatement* stmt = uni_db->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"Error: getting run numbers from DB has been failed"<<endl;
        delete stmt;
        delete connUniDb;
        return -2;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    vector<int> vecPeriods;
    vector<int> vecRuns;
    while (stmt->NextResultRow())
    {
        vecPeriods.push_back(stmt->GetInt(0));
        vecRuns.push_back(stmt->GetInt(1));
    }

    delete stmt;
    delete connUniDb;

    int run_count = vecPeriods.size();
    run_numbers = new UniqueRunNumber[run_count];
    for (int i = 0; i < run_count; i++)
    {
        run_numbers[i].period_number = vecPeriods[i];
        run_numbers[i].run_number = vecRuns[i];
    }

    return run_count;
}

int UniDbRun::SetRootGeometry(int start_period, int start_run, int end_period, int end_run, unsigned char* root_geometry, Long_t size_root_geometry)
{
    if (((end_period < start_period) or ((end_period = start_period) and (end_run < start_run))) or ((start_period > end_period) or ((start_period = end_period) and (start_run > end_run))))
    {
        cout<<"Error: end run should be after or the same as start run"<<endl;
        return -1;
    }

    UniDbRunGeometry* pGeometry = UniDbRunGeometry::CreateRunGeometry(root_geometry, size_root_geometry);
    if (pGeometry == NULL)
    {
        cout<<"Error: creating of geometry was failed"<<endl;
        return -2;
    }

    int geometry_id = pGeometry->GetGeometryId();
    delete pGeometry;

    UniqueRunNumber* pUniqueRuns = NULL;
    int run_count = GetRunNumbers(start_period, start_run, end_period, end_run, pUniqueRuns);
    if (run_count < 0)
        return -10 - run_count;

    for (int i = 0; i < run_count; i++)
    {        
        cout<<"Setting geometry for run_period:run   "<<pUniqueRuns[i].period_number<<":"<<pUniqueRuns[i].run_number<<"..."<<endl;

        UniDbRun* pCurRun = UniDbRun::GetRun(pUniqueRuns[i].period_number, pUniqueRuns[i].run_number);
        if (pCurRun == NULL)
        {
            cout<<"Error: getting of run "<<pUniqueRuns[i].period_number<<":"<<pUniqueRuns[i].run_number<<" (period:number) was failed"<<endl;
            continue;
        }

        pCurRun->SetGeometryId(new int(geometry_id));

        delete pCurRun;
    }

    delete [] pUniqueRuns;

    return 0;
}

int UniDbRun::GetRootGeometry(int period_number, int run_number, unsigned char*& root_geometry, Long_t& size_root_geometry)
{
    UniDbRun* pCurRun = UniDbRun::GetRun(period_number, run_number);
    if (pCurRun == NULL)
    {
        cout<<"Error: getting of run "<<period_number<<":"<<run_number<<" (period:number) was failed"<<endl;
        return -1;
    }

    if (pCurRun->GetGeometryId() == NULL)
    {
        cout<<"Error: no geometry exists for run "<<period_number<<":"<<run_number<<" (period:number)"<<endl;
        return -2;
    }

    int geometry_id = pCurRun->GetGeometryId()[0];
    delete pCurRun;

    UniDbRunGeometry* pGeometry = UniDbRunGeometry::GetRunGeometry(geometry_id);
    if (pGeometry == NULL)
    {
        cout<<"Error: getting of geometry was failed"<<endl;
        return -3;
    }

    size_root_geometry = pGeometry->GetRootGeometrySize();
    root_geometry = new unsigned char[size_root_geometry];
    memcpy(root_geometry, pGeometry->GetRootGeometry(), size_root_geometry);

    delete pGeometry;

    return 0;
}

int UniDbRun::WriteGeometryFile(int start_period, int start_run, int end_period, int end_run, char* geo_file_path)
{
    TString strGeoFilePath(geo_file_path);
    gSystem->ExpandPathName(strGeoFilePath);
    FILE* root_file = fopen(strGeoFilePath.Data(), "rb");
    if (root_file == NULL)
    {
        cout<<"Error: opening root file: "<<strGeoFilePath<<" was failed"<<endl;
        return -1;
    }

    fseek(root_file, 0, SEEK_END);
    long file_size = ftell(root_file);
    rewind(root_file);
    if (file_size <= 0)
    {
        cout<<"Error: getting file size: "<<strGeoFilePath<<" was failed"<<endl;
        fclose(root_file);
        return -2;
    }

    unsigned char* buffer = new unsigned char[file_size];
    if (buffer == NULL)
    {
        cout<<"Error: getting memory from heap was failed"<<endl;
        fclose(root_file);
        return -3;
    }

    size_t bytes_read = fread(buffer, 1, file_size, root_file);
    if (bytes_read != file_size)
    {
        cout<<"Error: reading file: "<<strGeoFilePath<<", got "<<bytes_read<<" bytes of "<<file_size<<endl;
        delete [] buffer;
        fclose(root_file);
        return -4;
    }

    fclose(root_file);

    // set root geometry file's bytes for run range
    int res_code = UniDbRun::SetRootGeometry(start_period, start_run, end_period, end_run, buffer, file_size);
    if (res_code != 0)
    {
        delete [] buffer;
        return -5;
    }

    delete [] buffer;

    return 0;
}

int UniDbRun::ReadGeometryFile(int period_number, int run_number, char* geo_file_path)
{
    // get root geometry file's bytes for selected run
    unsigned char* buffer = NULL;
    Long_t file_size;
    int res_code = UniDbRun::GetRootGeometry(period_number, run_number, buffer, file_size);
    if (res_code != 0)
    {
        return -1;
    }

    FILE* root_file = fopen(geo_file_path, "wb");
    if (root_file == NULL)
    {
        cout<<"Error: creating root file: "<<geo_file_path<<endl;
        return -2;
    }

    size_t bytes_write = fwrite(buffer, 1, file_size, root_file);
    if (bytes_write != file_size)
    {
        cout<<"Error: writing file: "<<geo_file_path<<", put "<<bytes_write<<" bytes of "<<file_size<<endl;
        delete [] buffer;
        fclose(root_file);
        return -3;
    }

    fclose(root_file);

    if (buffer)
        delete [] buffer;

    return 0;
}

TObjArray* UniDbRun::Search(const TObjArray& search_conditions)
{
    TObjArray* arrayResult = NULL;

    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
    {
        cout<<"Error: connection to DB was failed"<<endl;
        return arrayResult;
    }

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
                "select run_number, period_number, file_path, beam_particle, target_particle, energy, start_datetime, end_datetime, event_count, field_voltage, file_size, geometry_id "
                "from run_");

    TString strCondition;
    bool isFirst = true;
    TIter next(&search_conditions);
    UniDbSearchCondition* curCondition;
    while (curCondition = (UniDbSearchCondition*) next())
    {
        strCondition = "";

        switch (curCondition->GetColumn())
        {
            case columnRunNumber:       strCondition += "run_number "; break;
            case columnPeriodNumber:    strCondition += "period_number "; break;
            case columnFilePath:        strCondition += "lower(file_path) "; break;
            case columnBeamParticle:    strCondition += "lower(beam_particle) "; break;
            case columnTargetParticle:  strCondition += "lower(target_particle) "; break;
            case columnEnergy:          strCondition += "energy "; break;
            case columnStartDatetime:   strCondition += "start_datetime "; break;
            case columnEndDateTime:     strCondition += "end_datetime "; break;
            case columnEventCount:      strCondition += "event_count "; break;
            case columnFieldVoltage:    strCondition += "field_voltage "; break;
            case columnFileSize:        strCondition += "file_size "; break;
            default:
                cout<<"Error: column in the search condition wasn't defined, condition is skipped"<<endl;
                continue;
        }

        switch (curCondition->GetCondition())
        {
            case conditionLess:             strCondition += "< "; break;
            case conditionLessOrEqual:      strCondition += "<= "; break;
            case conditionEqual:            strCondition += "= "; break;
            case conditionNotEqual:         strCondition += "<> "; break;
            case conditionGreater:          strCondition += "> "; break;
            case conditionGreaterOrEqual:   strCondition += ">= "; break;
            case conditionLike:             strCondition += "like "; break;
            case conditionNull:             strCondition += "is null "; break;
            default:
                cout<<"Error: comparison operator in the search condition wasn't defined, condition is skipped"<<endl;
                continue;
        }

        switch (curCondition->GetValueType())
        {
            case 0: if (curCondition->GetCondition() != conditionNull) continue; break;
            case 1: strCondition += Form("%d", curCondition->GetIntValue()); break;
            case 2: strCondition += Form("%u", curCondition->GetUIntValue()); break;
            case 3: strCondition += Form("%f", curCondition->GetDoubleValue()); break;
            case 4: strCondition += Form("lower('%s')", curCondition->GetStringValue().Data()); break;
            case 5: strCondition += Form("'%s'", curCondition->GetDatimeValue().AsSQLString()); break;
            default:
                cout<<"Error: value type in the search condition wasn't found, condition is skipped"<<endl;
                continue;
        }

        if (isFirst)
        {
            sql += " where ";
            isFirst = false;
        }
        else
            sql += " and ";

        sql += strCondition;
    }
    sql += " order by period_number,run_number";

    TSQLStatement* stmt = uni_db->Statement(sql);
    //cout<<"SQL code: "<<sql<<endl;

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"Error: getting runs from DB has been failed"<<endl;
        delete stmt;
        delete connUniDb;

        return arrayResult;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // extract rows one after another
    arrayResult = new TObjArray();
    while (stmt->NextResultRow())
    {
        UniDbConnection* connRun = UniDbConnection::Open(UNIFIED_DB);
        if (connRun == 0x00)
        {
            cout<<"Error: connection to DB for single run was failed"<<endl;
            return arrayResult;
        }

        int tmp_run_number;
        tmp_run_number = stmt->GetInt(0);
        int tmp_period_number;
        tmp_period_number = stmt->GetInt(1);
        TString tmp_file_path;
        tmp_file_path = stmt->GetString(2);
        TString tmp_beam_particle;
        tmp_beam_particle = stmt->GetString(3);
        TString* tmp_target_particle;
        if (stmt->IsNull(4)) tmp_target_particle = NULL;
        else
            tmp_target_particle = new TString(stmt->GetString(4));
        double* tmp_energy;
        if (stmt->IsNull(5)) tmp_energy = NULL;
        else
            tmp_energy = new double(stmt->GetDouble(5));
        TDatime tmp_start_datetime;
        tmp_start_datetime = stmt->GetDatime(6);
        TDatime* tmp_end_datetime;
        if (stmt->IsNull(7)) tmp_end_datetime = NULL;
        else
            tmp_end_datetime = new TDatime(stmt->GetDatime(7));
        int* tmp_event_count;
        if (stmt->IsNull(8)) tmp_event_count = NULL;
        else
            tmp_event_count = new int(stmt->GetInt(8));
        double* tmp_field_voltage;
        if (stmt->IsNull(9)) tmp_field_voltage = NULL;
        else
            tmp_field_voltage = new double(stmt->GetDouble(9));
        double* tmp_file_size;
        if (stmt->IsNull(10)) tmp_file_size = NULL;
        else
            tmp_file_size = new double(stmt->GetDouble(10));
        int* tmp_geometry_id;
        if (stmt->IsNull(11)) tmp_geometry_id = NULL;
        else
            tmp_geometry_id = new int(stmt->GetInt(11));

        arrayResult->Add((TObject*) new UniDbRun(connRun, tmp_run_number, tmp_period_number, tmp_file_path, tmp_beam_particle, tmp_target_particle, tmp_energy, tmp_start_datetime, tmp_end_datetime, tmp_event_count, tmp_field_voltage, tmp_file_size, tmp_geometry_id));
    }

    delete stmt;

    return arrayResult;
}

TObjArray* UniDbRun::Search(const UniDbSearchCondition& search_condition)
{
    TObjArray search_conditions;
    search_conditions.Add((TObject*)&search_condition);

    return Search(search_conditions);
}

// -------------------------------------------------------------------
ClassImp(UniDbRun);
