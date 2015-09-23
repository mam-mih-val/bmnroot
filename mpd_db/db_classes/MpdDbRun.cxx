// ----------------------------------------------------------------------
//                    MpdDbRun cxx file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "MpdDbRun.h"
#include "MpdDbRunGeometry.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
MpdDbRun::MpdDbRun(MpdDbConnection* connUniDb, int run_number, int* session_number, TString file_path, TString beam_particle, TString* target_particle, double* energy, TDatime start_datetime, TDatime* end_datetime, int* event_count, int* field_current, double* file_size, int* geometry_id)
{
	connectionUniDb = connUniDb;

	i_run_number = run_number;
	i_session_number = session_number;
	str_file_path = file_path;
	str_beam_particle = beam_particle;
	str_target_particle = target_particle;
	d_energy = energy;
	dt_start_datetime = start_datetime;
	dt_end_datetime = end_datetime;
	i_event_count = event_count;
	i_field_current = field_current;
	d_file_size = file_size;
	i_geometry_id = geometry_id;
}

// -----   Destructor   -------------------------------------------------
MpdDbRun::~MpdDbRun()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (i_session_number)
		delete i_session_number;
	if (str_target_particle)
		delete str_target_particle;
	if (d_energy)
		delete d_energy;
	if (dt_end_datetime)
		delete dt_end_datetime;
	if (i_event_count)
		delete i_event_count;
	if (i_field_current)
		delete i_field_current;
	if (d_file_size)
		delete d_file_size;
	if (i_geometry_id)
		delete i_geometry_id;
}

// -----   Creating new record in class table ---------------------------
MpdDbRun* MpdDbRun::CreateRun(int run_number, int* session_number, TString file_path, TString beam_particle, TString* target_particle, double* energy, TDatime start_datetime, TDatime* end_datetime, int* event_count, int* field_current, double* file_size, int* geometry_id)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into run_(run_number, session_number, file_path, beam_particle, target_particle, energy, start_datetime, end_datetime, event_count, field_current, file_size, geometry_id) "
		"values ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, run_number);
	if (session_number == NULL)
		stmt->SetNull(1);
	else
		stmt->SetInt(1, *session_number);
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
	if (field_current == NULL)
		stmt->SetNull(9);
	else
		stmt->SetInt(9, *field_current);
	if (file_size == NULL)
		stmt->SetNull(10);
	else
		stmt->SetDouble(10, *file_size);
	if (geometry_id == NULL)
		stmt->SetNull(11);
	else
		stmt->SetInt(11, *geometry_id);

	// inserting new record to DB
	if (!stmt->Process())
	{
		cout<<"Error: inserting new record to DB has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	int tmp_run_number;
	tmp_run_number = run_number;
	int* tmp_session_number;
	if (session_number == NULL) tmp_session_number = NULL;
	else
		tmp_session_number = new int(*session_number);
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
	int* tmp_field_current;
	if (field_current == NULL) tmp_field_current = NULL;
	else
		tmp_field_current = new int(*field_current);
	double* tmp_file_size;
	if (file_size == NULL) tmp_file_size = NULL;
	else
		tmp_file_size = new double(*file_size);
	int* tmp_geometry_id;
	if (geometry_id == NULL) tmp_geometry_id = NULL;
	else
		tmp_geometry_id = new int(*geometry_id);

	return new MpdDbRun(connUniDb, tmp_run_number, tmp_session_number, tmp_file_path, tmp_beam_particle, tmp_target_particle, tmp_energy, tmp_start_datetime, tmp_end_datetime, tmp_event_count, tmp_field_current, tmp_file_size, tmp_geometry_id);
}

// -----   Get table record from database ---------------------------
MpdDbRun* MpdDbRun::GetRun(int run_number)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select run_number, session_number, file_path, beam_particle, target_particle, energy, start_datetime, end_datetime, event_count, field_current, file_size, geometry_id "
		"from run_ "
		"where run_number = %d", run_number);
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get table record from DB
	if (!stmt->Process())
	{
		cout<<"Error: getting record from DB has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"Error: table record wasn't found"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	int tmp_run_number;
	tmp_run_number = stmt->GetInt(0);
	int* tmp_session_number;
	if (stmt->IsNull(1)) tmp_session_number = NULL;
	else
		tmp_session_number = new int(stmt->GetInt(1));
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
	int* tmp_field_current;
	if (stmt->IsNull(9)) tmp_field_current = NULL;
	else
		tmp_field_current = new int(stmt->GetInt(9));
	double* tmp_file_size;
	if (stmt->IsNull(10)) tmp_file_size = NULL;
	else
		tmp_file_size = new double(stmt->GetDouble(10));
	int* tmp_geometry_id;
	if (stmt->IsNull(11)) tmp_geometry_id = NULL;
	else
		tmp_geometry_id = new int(stmt->GetInt(11));

	delete stmt;

	return new MpdDbRun(connUniDb, tmp_run_number, tmp_session_number, tmp_file_path, tmp_beam_particle, tmp_target_particle, tmp_energy, tmp_start_datetime, tmp_end_datetime, tmp_event_count, tmp_field_current, tmp_file_size, tmp_geometry_id);
}

// -----   Get table record from database for unique key--------------
MpdDbRun* MpdDbRun::GetRun(TString file_path)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select run_number, session_number, file_path, beam_particle, target_particle, energy, start_datetime, end_datetime, event_count, field_current, file_size, geometry_id "
		"from run_ "
		"where lower(file_path) = lower('%s')", file_path.Data());
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get table record from DB
	if (!stmt->Process())
	{
		cout<<"Error: getting record from DB has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"Error: table record wasn't found"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	int tmp_run_number;
	tmp_run_number = stmt->GetInt(0);
	int* tmp_session_number;
	if (stmt->IsNull(1)) tmp_session_number = NULL;
	else
		tmp_session_number = new int(stmt->GetInt(1));
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
	int* tmp_field_current;
	if (stmt->IsNull(9)) tmp_field_current = NULL;
	else
		tmp_field_current = new int(stmt->GetInt(9));
	double* tmp_file_size;
	if (stmt->IsNull(10)) tmp_file_size = NULL;
	else
		tmp_file_size = new double(stmt->GetDouble(10));
	int* tmp_geometry_id;
	if (stmt->IsNull(11)) tmp_geometry_id = NULL;
	else
		tmp_geometry_id = new int(stmt->GetInt(11));

	delete stmt;

	return new MpdDbRun(connUniDb, tmp_run_number, tmp_session_number, tmp_file_path, tmp_beam_particle, tmp_target_particle, tmp_energy, tmp_start_datetime, tmp_end_datetime, tmp_event_count, tmp_field_current, tmp_file_size, tmp_geometry_id);
}

// -----   Delete record from class table ---------------------------
int MpdDbRun::DeleteRun(int run_number)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from run_ "
		"where run_number = $1");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, run_number);

	// delete table record from DB
	if (!stmt->Process())
	{
		cout<<"Error: deleting record from DB has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	delete stmt;
	delete connUniDb;
	return 0;
}

// -----   Delete table record from database for unique key--------------
int MpdDbRun::DeleteRun(TString file_path)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from run_ "
		"where lower(file_path) = lower($1)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, file_path);

	// delete table record from DB
	if (!stmt->Process())
	{
		cout<<"Error: deleting record from DB has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	delete stmt;
	delete connUniDb;
	return 0;
}

// -----   Print all table records ---------------------------------
int MpdDbRun::PrintAll()
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select run_number, session_number, file_path, beam_particle, target_particle, energy, start_datetime, end_datetime, event_count, field_current, file_size, geometry_id "
		"from run_");
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get table record from DB
	if (!stmt->Process())
	{
		cout<<"Error: getting all records from DB has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// print rows
	cout<<"Table 'run_'"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<". run_number: ";
		cout<<(stmt->GetInt(0));
		cout<<". session_number: ";
		if (stmt->IsNull(1)) cout<<"NULL";
		else
			cout<<stmt->GetInt(1);
		cout<<". file_path: ";
		cout<<(stmt->GetString(2));
		cout<<". beam_particle: ";
		cout<<(stmt->GetString(3));
		cout<<". target_particle: ";
		if (stmt->IsNull(4)) cout<<"NULL";
		else
			cout<<stmt->GetString(4);
		cout<<". energy: ";
		if (stmt->IsNull(5)) cout<<"NULL";
		else
			cout<<stmt->GetDouble(5);
		cout<<". start_datetime: ";
		cout<<(stmt->GetDatime(6)).AsSQLString();
		cout<<". end_datetime: ";
		if (stmt->IsNull(7)) cout<<"NULL";
		else
			cout<<stmt->GetDatime(7).AsSQLString();
		cout<<". event_count: ";
		if (stmt->IsNull(8)) cout<<"NULL";
		else
			cout<<stmt->GetInt(8);
		cout<<". field_current: ";
		if (stmt->IsNull(9)) cout<<"NULL";
		else
			cout<<stmt->GetInt(9);
		cout<<". file_size: ";
		if (stmt->IsNull(10)) cout<<"NULL";
		else
			cout<<stmt->GetDouble(10);
		cout<<". geometry_id: ";
		if (stmt->IsNull(11)) cout<<"NULL";
		else
			cout<<stmt->GetInt(11);
		cout<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int MpdDbRun::SetRunNumber(int run_number)
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
		"where run_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, run_number);
	stmt->SetInt(1, i_run_number);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_run_number = run_number;

	delete stmt;
	return 0;
}

int MpdDbRun::SetSessionNumber(int* session_number)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_ "
		"set session_number = $1 "
		"where run_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (session_number == NULL)
		stmt->SetNull(0);
	else
		stmt->SetInt(0, *session_number);
	stmt->SetInt(1, i_run_number);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (i_session_number)
		delete i_session_number;
	if (session_number == NULL) i_session_number = NULL;
	else
		i_session_number = new int(*session_number);

	delete stmt;
	return 0;
}

int MpdDbRun::SetFilePath(TString file_path)
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
		"where run_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, file_path);
	stmt->SetInt(1, i_run_number);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_file_path = file_path;

	delete stmt;
	return 0;
}

int MpdDbRun::SetBeamParticle(TString beam_particle)
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
		"where run_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, beam_particle);
	stmt->SetInt(1, i_run_number);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_beam_particle = beam_particle;

	delete stmt;
	return 0;
}

int MpdDbRun::SetTargetParticle(TString* target_particle)
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
		"where run_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (target_particle == NULL)
		stmt->SetNull(0);
	else
		stmt->SetString(0, *target_particle);
	stmt->SetInt(1, i_run_number);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

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

int MpdDbRun::SetEnergy(double* energy)
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
		"where run_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (energy == NULL)
		stmt->SetNull(0);
	else
		stmt->SetDouble(0, *energy);
	stmt->SetInt(1, i_run_number);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

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

int MpdDbRun::SetStartDatetime(TDatime start_datetime)
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
		"where run_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetDatime(0, start_datetime);
	stmt->SetInt(1, i_run_number);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	dt_start_datetime = start_datetime;

	delete stmt;
	return 0;
}

int MpdDbRun::SetEndDatetime(TDatime* end_datetime)
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
		"where run_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (end_datetime == NULL)
		stmt->SetNull(0);
	else
		stmt->SetDatime(0, *end_datetime);
	stmt->SetInt(1, i_run_number);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

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

int MpdDbRun::SetEventCount(int* event_count)
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
		"where run_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (event_count == NULL)
		stmt->SetNull(0);
	else
		stmt->SetInt(0, *event_count);
	stmt->SetInt(1, i_run_number);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

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

int MpdDbRun::SetFieldCurrent(int* field_current)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_ "
		"set field_current = $1 "
		"where run_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (field_current == NULL)
		stmt->SetNull(0);
	else
		stmt->SetInt(0, *field_current);
	stmt->SetInt(1, i_run_number);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (i_field_current)
		delete i_field_current;
	if (field_current == NULL) i_field_current = NULL;
	else
		i_field_current = new int(*field_current);

	delete stmt;
	return 0;
}

int MpdDbRun::SetFileSize(double* file_size)
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
		"where run_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (file_size == NULL)
		stmt->SetNull(0);
	else
		stmt->SetDouble(0, *file_size);
	stmt->SetInt(1, i_run_number);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

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

int MpdDbRun::SetGeometryId(int* geometry_id)
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
		"where run_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (geometry_id == NULL)
		stmt->SetNull(0);
	else
		stmt->SetInt(0, *geometry_id);
	stmt->SetInt(1, i_run_number);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

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

// -----   Print current record ---------------------------------------
void MpdDbRun::Print()
{
	cout<<"Table 'run_'";
	cout<<". run_number: "<<i_run_number<<". session_number: "<<(i_session_number == NULL? "NULL": TString::Format("%d", *i_session_number))<<". file_path: "<<str_file_path<<". beam_particle: "<<str_beam_particle<<". target_particle: "<<(str_target_particle == NULL? "NULL": *str_target_particle)<<". energy: "<<(d_energy == NULL? "NULL": TString::Format("%f", *d_energy))<<". start_datetime: "<<dt_start_datetime.AsSQLString()<<". end_datetime: "<<(dt_end_datetime == NULL? "NULL": (*dt_end_datetime).AsSQLString())<<". event_count: "<<(i_event_count == NULL? "NULL": TString::Format("%d", *i_event_count))<<". field_current: "<<(i_field_current == NULL? "NULL": TString::Format("%d", *i_field_current))<<". file_size: "<<(d_file_size == NULL? "NULL": TString::Format("%f", *d_file_size))<<". geometry_id: "<<(i_geometry_id == NULL? "NULL": TString::Format("%d", *i_geometry_id))<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

int MpdDbRun::SetRootGeometry(int start_run_number, int end_run_number, unsigned char* root_geometry, Long_t size_root_geometry)
{
    int run_count = end_run_number - start_run_number;
    if (run_count < 0)
    {
        cout<<"Error: end run number should be greater or equal start run_number"<<endl;
        return -1;
    }

    MpdDbRunGeometry* pGeometry = MpdDbRunGeometry::CreateRunGeometry(root_geometry, size_root_geometry);
    if (pGeometry == NULL)
    {
        cout<<"Error: creating of geometry was failed"<<endl;
        return -2;
    }

    int geometry_id = pGeometry->GetGeometryId();
    delete pGeometry;

    for (int i = start_run_number; i <= end_run_number; i++)
    {
        cout<<i<<endl;

        MpdDbRun* pCurRun = MpdDbRun::GetRun(i);
        if (pCurRun == NULL)
        {
            cout<<"Error: getting of run with number "<<i<<" was failed"<<endl;
            continue;
        }

        pCurRun->SetGeometryId(new int(geometry_id));

        delete pCurRun;
    }

    return 0;
}

// -------------------------------------------------------------------
ClassImp(MpdDbRun);
