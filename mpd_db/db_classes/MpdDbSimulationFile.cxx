// ----------------------------------------------------------------------
//                    MpdDbSimulationFile cxx file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "MpdDbSimulationFile.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
MpdDbSimulationFile::MpdDbSimulationFile(MpdDbConnection* connUniDb, int file_id, TString file_path, TString generator_name, TString beam_particle, TString* target_particle, double* energy, TString centrality, int* event_count, TString* file_desc, double* file_size_kb)
{
	connectionUniDb = connUniDb;

	i_file_id = file_id;
	str_file_path = file_path;
	str_generator_name = generator_name;
	str_beam_particle = beam_particle;
	str_target_particle = target_particle;
	d_energy = energy;
	str_centrality = centrality;
	i_event_count = event_count;
	str_file_desc = file_desc;
	d_file_size_kb = file_size_kb;
}

// -----   Destructor   -------------------------------------------------
MpdDbSimulationFile::~MpdDbSimulationFile()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (str_target_particle)
		delete str_target_particle;
	if (d_energy)
		delete d_energy;
	if (i_event_count)
		delete i_event_count;
	if (str_file_desc)
		delete str_file_desc;
	if (d_file_size_kb)
		delete d_file_size_kb;
}

// -----   Creating new record in class table ---------------------------
MpdDbSimulationFile* MpdDbSimulationFile::CreateSimulationFile(TString file_path, TString generator_name, TString beam_particle, TString* target_particle, double* energy, TString centrality, int* event_count, TString* file_desc, double* file_size_kb)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into simulation_file(file_path, generator_name, beam_particle, target_particle, energy, centrality, event_count, file_desc, file_size_kb) "
		"values ($1, $2, $3, $4, $5, $6, $7, $8, $9)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, file_path);
	stmt->SetString(1, generator_name);
	stmt->SetString(2, beam_particle);
	if (target_particle == NULL)
		stmt->SetNull(3);
	else
		stmt->SetString(3, *target_particle);
	if (energy == NULL)
		stmt->SetNull(4);
	else
		stmt->SetDouble(4, *energy);
	stmt->SetString(5, centrality);
	if (event_count == NULL)
		stmt->SetNull(6);
	else
		stmt->SetInt(6, *event_count);
	if (file_desc == NULL)
		stmt->SetNull(7);
	else
		stmt->SetString(7, *file_desc);
	if (file_size_kb == NULL)
		stmt->SetNull(8);
	else
		stmt->SetDouble(8, *file_size_kb);

	// inserting new record to DB
	if (!stmt->Process())
	{
		cout<<"Error: inserting new record to DB has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	// getting last inserted ID
	int file_id;
	TSQLStatement* stmt_last = uni_db->Statement("SELECT currval(pg_get_serial_sequence('simulation_file','file_id'))");

	// process getting last id
	if (stmt_last->Process())
	{
		// store result of statement in buffer
		stmt_last->StoreResult();

		// if there is no last id then exit with error
		if (!stmt_last->NextResultRow())
		{
			cout<<"Error: no last ID in DB!"<<endl;
			delete stmt_last;
			return 0x00;
		}
		else
		{
			file_id = stmt_last->GetInt(0);
			delete stmt_last;
		}
	}
	else
	{
		cout<<"Error: getting last ID has been failed!"<<endl;
		delete stmt_last;
		return 0x00;
	}

	int tmp_file_id;
	tmp_file_id = file_id;
	TString tmp_file_path;
	tmp_file_path = file_path;
	TString tmp_generator_name;
	tmp_generator_name = generator_name;
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
	TString tmp_centrality;
	tmp_centrality = centrality;
	int* tmp_event_count;
	if (event_count == NULL) tmp_event_count = NULL;
	else
		tmp_event_count = new int(*event_count);
	TString* tmp_file_desc;
	if (file_desc == NULL) tmp_file_desc = NULL;
	else
		tmp_file_desc = new TString(*file_desc);
	double* tmp_file_size_kb;
	if (file_size_kb == NULL) tmp_file_size_kb = NULL;
	else
		tmp_file_size_kb = new double(*file_size_kb);

	return new MpdDbSimulationFile(connUniDb, tmp_file_id, tmp_file_path, tmp_generator_name, tmp_beam_particle, tmp_target_particle, tmp_energy, tmp_centrality, tmp_event_count, tmp_file_desc, tmp_file_size_kb);
}

// -----   Get table record from database ---------------------------
MpdDbSimulationFile* MpdDbSimulationFile::GetSimulationFile(int file_id)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select file_id, file_path, generator_name, beam_particle, target_particle, energy, centrality, event_count, file_desc, file_size_kb "
		"from simulation_file "
		"where file_id = %d", file_id);
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

	int tmp_file_id;
	tmp_file_id = stmt->GetInt(0);
	TString tmp_file_path;
	tmp_file_path = stmt->GetString(1);
	TString tmp_generator_name;
	tmp_generator_name = stmt->GetString(2);
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
	TString tmp_centrality;
	tmp_centrality = stmt->GetString(6);
	int* tmp_event_count;
	if (stmt->IsNull(7)) tmp_event_count = NULL;
	else
		tmp_event_count = new int(stmt->GetInt(7));
	TString* tmp_file_desc;
	if (stmt->IsNull(8)) tmp_file_desc = NULL;
	else
		tmp_file_desc = new TString(stmt->GetString(8));
	double* tmp_file_size_kb;
	if (stmt->IsNull(9)) tmp_file_size_kb = NULL;
	else
		tmp_file_size_kb = new double(stmt->GetDouble(9));

	delete stmt;

	return new MpdDbSimulationFile(connUniDb, tmp_file_id, tmp_file_path, tmp_generator_name, tmp_beam_particle, tmp_target_particle, tmp_energy, tmp_centrality, tmp_event_count, tmp_file_desc, tmp_file_size_kb);
}

// -----   Get table record from database for unique key--------------
MpdDbSimulationFile* MpdDbSimulationFile::GetSimulationFile(TString file_path)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select file_id, file_path, generator_name, beam_particle, target_particle, energy, centrality, event_count, file_desc, file_size_kb "
		"from simulation_file "
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

	int tmp_file_id;
	tmp_file_id = stmt->GetInt(0);
	TString tmp_file_path;
	tmp_file_path = stmt->GetString(1);
	TString tmp_generator_name;
	tmp_generator_name = stmt->GetString(2);
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
	TString tmp_centrality;
	tmp_centrality = stmt->GetString(6);
	int* tmp_event_count;
	if (stmt->IsNull(7)) tmp_event_count = NULL;
	else
		tmp_event_count = new int(stmt->GetInt(7));
	TString* tmp_file_desc;
	if (stmt->IsNull(8)) tmp_file_desc = NULL;
	else
		tmp_file_desc = new TString(stmt->GetString(8));
	double* tmp_file_size_kb;
	if (stmt->IsNull(9)) tmp_file_size_kb = NULL;
	else
		tmp_file_size_kb = new double(stmt->GetDouble(9));

	delete stmt;

	return new MpdDbSimulationFile(connUniDb, tmp_file_id, tmp_file_path, tmp_generator_name, tmp_beam_particle, tmp_target_particle, tmp_energy, tmp_centrality, tmp_event_count, tmp_file_desc, tmp_file_size_kb);
}

// -----   Delete record from class table ---------------------------
int MpdDbSimulationFile::DeleteSimulationFile(int file_id)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from simulation_file "
		"where file_id = $1");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, file_id);

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
int MpdDbSimulationFile::DeleteSimulationFile(TString file_path)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from simulation_file "
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
int MpdDbSimulationFile::PrintAll()
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select file_id, file_path, generator_name, beam_particle, target_particle, energy, centrality, event_count, file_desc, file_size_kb "
		"from simulation_file");
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
	cout<<"Table 'simulation_file'"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<". file_id: ";
		cout<<(stmt->GetInt(0));
		cout<<". file_path: ";
		cout<<(stmt->GetString(1));
		cout<<". generator_name: ";
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
		cout<<". centrality: ";
		cout<<(stmt->GetString(6));
		cout<<". event_count: ";
		if (stmt->IsNull(7)) cout<<"NULL";
		else
			cout<<stmt->GetInt(7);
		cout<<". file_desc: ";
		if (stmt->IsNull(8)) cout<<"NULL";
		else
			cout<<stmt->GetString(8);
		cout<<". file_size_kb: ";
		if (stmt->IsNull(9)) cout<<"NULL";
		else
			cout<<stmt->GetDouble(9);
		cout<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int MpdDbSimulationFile::SetFilePath(TString file_path)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set file_path = $1 "
		"where file_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, file_path);
	stmt->SetInt(1, i_file_id);

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

int MpdDbSimulationFile::SetGeneratorName(TString generator_name)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set generator_name = $1 "
		"where file_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, generator_name);
	stmt->SetInt(1, i_file_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_generator_name = generator_name;

	delete stmt;
	return 0;
}

int MpdDbSimulationFile::SetBeamParticle(TString beam_particle)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set beam_particle = $1 "
		"where file_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, beam_particle);
	stmt->SetInt(1, i_file_id);

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

int MpdDbSimulationFile::SetTargetParticle(TString* target_particle)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set target_particle = $1 "
		"where file_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (target_particle == NULL)
		stmt->SetNull(0);
	else
		stmt->SetString(0, *target_particle);
	stmt->SetInt(1, i_file_id);

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

int MpdDbSimulationFile::SetEnergy(double* energy)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set energy = $1 "
		"where file_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (energy == NULL)
		stmt->SetNull(0);
	else
		stmt->SetDouble(0, *energy);
	stmt->SetInt(1, i_file_id);

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

int MpdDbSimulationFile::SetCentrality(TString centrality)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set centrality = $1 "
		"where file_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, centrality);
	stmt->SetInt(1, i_file_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_centrality = centrality;

	delete stmt;
	return 0;
}

int MpdDbSimulationFile::SetEventCount(int* event_count)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set event_count = $1 "
		"where file_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (event_count == NULL)
		stmt->SetNull(0);
	else
		stmt->SetInt(0, *event_count);
	stmt->SetInt(1, i_file_id);

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

int MpdDbSimulationFile::SetFileDesc(TString* file_desc)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set file_desc = $1 "
		"where file_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (file_desc == NULL)
		stmt->SetNull(0);
	else
		stmt->SetString(0, *file_desc);
	stmt->SetInt(1, i_file_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (str_file_desc)
		delete str_file_desc;
	if (file_desc == NULL) str_file_desc = NULL;
	else
		str_file_desc = new TString(*file_desc);

	delete stmt;
	return 0;
}

int MpdDbSimulationFile::SetFileSizeKb(double* file_size_kb)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set file_size_kb = $1 "
		"where file_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (file_size_kb == NULL)
		stmt->SetNull(0);
	else
		stmt->SetDouble(0, *file_size_kb);
	stmt->SetInt(1, i_file_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (d_file_size_kb)
		delete d_file_size_kb;
	if (file_size_kb == NULL) d_file_size_kb = NULL;
	else
		d_file_size_kb = new double(*file_size_kb);

	delete stmt;
	return 0;
}

// -----   Print current record ---------------------------------------
void MpdDbSimulationFile::Print()
{
	cout<<"Table 'simulation_file'";
	cout<<". file_id: "<<i_file_id<<". file_path: "<<str_file_path<<". generator_name: "<<str_generator_name<<". beam_particle: "<<str_beam_particle<<". target_particle: "<<(str_target_particle == NULL? "NULL": *str_target_particle)<<". energy: "<<(d_energy == NULL? "NULL": TString::Format("%f", *d_energy))<<". centrality: "<<str_centrality<<". event_count: "<<(i_event_count == NULL? "NULL": TString::Format("%d", *i_event_count))<<". file_desc: "<<(str_file_desc == NULL? "NULL": *str_file_desc)<<". file_size_kb: "<<(d_file_size_kb == NULL? "NULL": TString::Format("%f", *d_file_size_kb))<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(MpdDbSimulationFile);
