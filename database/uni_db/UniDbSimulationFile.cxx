// ----------------------------------------------------------------------
//                    UniDbSimulationFile cxx file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"
#include "TObjArray.h"

#include "UniDbSimulationFile.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniDbSimulationFile::UniDbSimulationFile(UniConnection* db_connect, int file_id, TString file_path, TString generator_name, TString beam_particle, TString* target_particle, double* energy, TString centrality, int* event_count, TString* file_desc, int* file_size, TString* file_md5)
{
    connectionUniDb = db_connect;

	i_file_id = file_id;
	str_file_path = file_path;
	str_generator_name = generator_name;
	str_beam_particle = beam_particle;
	str_target_particle = target_particle;
	d_energy = energy;
	str_centrality = centrality;
	i_event_count = event_count;
	str_file_desc = file_desc;
	i_file_size = file_size;
	chr_file_md5 = file_md5;
}

// -----   Destructor   -------------------------------------------------
UniDbSimulationFile::~UniDbSimulationFile()
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
	if (i_file_size)
		delete i_file_size;
	if (chr_file_md5)
		delete chr_file_md5;
}

// -----   Creating new simulation file in the database  ---------------------------
UniDbSimulationFile* UniDbSimulationFile::CreateSimulationFile(TString file_path, TString generator_name, TString beam_particle, TString* target_particle, double* energy, TString centrality, int* event_count, TString* file_desc, int* file_size, TString* file_md5)
{
	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into simulation_file(file_path, generator_name, beam_particle, target_particle, energy, centrality, event_count, file_desc, file_size, file_md5) "
		"values ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10)");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, file_path);
	stmt->SetString(1, generator_name);
	stmt->SetString(2, beam_particle);
	if (target_particle == nullptr)
		stmt->SetNull(3);
	else
		stmt->SetString(3, *target_particle);
	if (energy == nullptr)
		stmt->SetNull(4);
	else
		stmt->SetDouble(4, *energy);
	stmt->SetString(5, centrality);
	if (event_count == nullptr)
		stmt->SetNull(6);
	else
		stmt->SetInt(6, *event_count);
	if (file_desc == nullptr)
		stmt->SetNull(7);
	else
		stmt->SetString(7, *file_desc);
	if (file_size == nullptr)
		stmt->SetNull(8);
	else
		stmt->SetInt(8, *file_size);
	if (file_md5 == nullptr)
		stmt->SetNull(9);
	else
		stmt->SetString(9, *file_md5);

	// inserting new simulation file to the Database
	if (!stmt->Process())
	{
		cout<<"ERROR: inserting new simulation file to the Database has been failed"<<endl;
		delete stmt;
        delete connDb;
        return nullptr;
	}

	delete stmt;

	// getting last inserted ID
	int file_id;
	TSQLStatement* stmt_last = db_server->Statement("SELECT currval(pg_get_serial_sequence('simulation_file','file_id'))");

	// process getting last id
	if (stmt_last->Process())
	{
		// store result of statement in buffer
		stmt_last->StoreResult();

		// if there is no last id then exit with error
		if (!stmt_last->NextResultRow())
		{
			cout<<"ERROR: no last ID in DB!"<<endl;
			delete stmt_last;
            return nullptr;
		}
		else
		{
			file_id = stmt_last->GetInt(0);
			delete stmt_last;
		}
	}
	else
	{
		cout<<"ERROR: getting last ID has been failed!"<<endl;
		delete stmt_last;
        return nullptr;
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
	if (target_particle == nullptr) tmp_target_particle = nullptr;
	else
		tmp_target_particle = new TString(*target_particle);
	double* tmp_energy;
	if (energy == nullptr) tmp_energy = nullptr;
	else
		tmp_energy = new double(*energy);
	TString tmp_centrality;
	tmp_centrality = centrality;
	int* tmp_event_count;
	if (event_count == nullptr) tmp_event_count = nullptr;
	else
		tmp_event_count = new int(*event_count);
	TString* tmp_file_desc;
	if (file_desc == nullptr) tmp_file_desc = nullptr;
	else
		tmp_file_desc = new TString(*file_desc);
	int* tmp_file_size;
	if (file_size == nullptr) tmp_file_size = nullptr;
	else
		tmp_file_size = new int(*file_size);
	TString* tmp_file_md5;
	if (file_md5 == nullptr) tmp_file_md5 = nullptr;
	else
		tmp_file_md5 = new TString(*file_md5);

    return new UniDbSimulationFile(connDb, tmp_file_id, tmp_file_path, tmp_generator_name, tmp_beam_particle, tmp_target_particle, tmp_energy, tmp_centrality, tmp_event_count, tmp_file_desc, tmp_file_size, tmp_file_md5);
}

// -----  Get simulation file from the database  ---------------------------
UniDbSimulationFile* UniDbSimulationFile::GetSimulationFile(int file_id)
{
	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select file_id, file_path, generator_name, beam_particle, target_particle, energy, centrality, event_count, file_desc, file_size, file_md5 "
		"from simulation_file "
		"where file_id = %d", file_id);
    TSQLStatement* stmt = db_server->Statement(sql);

	// get simulation file from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting simulation file from the database has been failed"<<endl;

		delete stmt;
        delete connDb;
        return nullptr;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: simulation file was not found in the database"<<endl;

		delete stmt;
        delete connDb;
        return nullptr;
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
	if (stmt->IsNull(4)) tmp_target_particle = nullptr;
	else
		tmp_target_particle = new TString(stmt->GetString(4));
	double* tmp_energy;
	if (stmt->IsNull(5)) tmp_energy = nullptr;
	else
		tmp_energy = new double(stmt->GetDouble(5));
	TString tmp_centrality;
	tmp_centrality = stmt->GetString(6);
	int* tmp_event_count;
	if (stmt->IsNull(7)) tmp_event_count = nullptr;
	else
		tmp_event_count = new int(stmt->GetInt(7));
	TString* tmp_file_desc;
	if (stmt->IsNull(8)) tmp_file_desc = nullptr;
	else
		tmp_file_desc = new TString(stmt->GetString(8));
	int* tmp_file_size;
	if (stmt->IsNull(9)) tmp_file_size = nullptr;
	else
		tmp_file_size = new int(stmt->GetInt(9));
	TString* tmp_file_md5;
	if (stmt->IsNull(10)) tmp_file_md5 = nullptr;
	else
		tmp_file_md5 = new TString(stmt->GetString(10));

	delete stmt;

    return new UniDbSimulationFile(connDb, tmp_file_id, tmp_file_path, tmp_generator_name, tmp_beam_particle, tmp_target_particle, tmp_energy, tmp_centrality, tmp_event_count, tmp_file_desc, tmp_file_size, tmp_file_md5);
}

// -----  Get simulation file from the database by unique key  --------------
UniDbSimulationFile* UniDbSimulationFile::GetSimulationFile(TString file_path)
{
	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select file_id, file_path, generator_name, beam_particle, target_particle, energy, centrality, event_count, file_desc, file_size, file_md5 "
		"from simulation_file "
		"where lower(file_path) = lower('%s')", file_path.Data());
    TSQLStatement* stmt = db_server->Statement(sql);

	// get simulation file from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting simulation file from the database has been failed"<<endl;

		delete stmt;
        delete connDb;
        return nullptr;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: simulation file was not found in the database"<<endl;

		delete stmt;
        delete connDb;
        return nullptr;
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
	if (stmt->IsNull(4)) tmp_target_particle = nullptr;
	else
		tmp_target_particle = new TString(stmt->GetString(4));
	double* tmp_energy;
	if (stmt->IsNull(5)) tmp_energy = nullptr;
	else
		tmp_energy = new double(stmt->GetDouble(5));
	TString tmp_centrality;
	tmp_centrality = stmt->GetString(6);
	int* tmp_event_count;
	if (stmt->IsNull(7)) tmp_event_count = nullptr;
	else
		tmp_event_count = new int(stmt->GetInt(7));
	TString* tmp_file_desc;
	if (stmt->IsNull(8)) tmp_file_desc = nullptr;
	else
		tmp_file_desc = new TString(stmt->GetString(8));
	int* tmp_file_size;
	if (stmt->IsNull(9)) tmp_file_size = nullptr;
	else
		tmp_file_size = new int(stmt->GetInt(9));
	TString* tmp_file_md5;
	if (stmt->IsNull(10)) tmp_file_md5 = nullptr;
	else
		tmp_file_md5 = new TString(stmt->GetString(10));

	delete stmt;

    return new UniDbSimulationFile(connDb, tmp_file_id, tmp_file_path, tmp_generator_name, tmp_beam_particle, tmp_target_particle, tmp_energy, tmp_centrality, tmp_event_count, tmp_file_desc, tmp_file_size, tmp_file_md5);
}

// -----  Check simulation file exists in the database  ---------------------------
int UniDbSimulationFile::CheckSimulationFileExists(int file_id)
{
	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from simulation_file "
		"where file_id = %d", file_id);
    TSQLStatement* stmt = db_server->Statement(sql);

	// get simulation file from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting simulation file from the database has been failed"<<endl;

		delete stmt;
        delete connDb;
        return -2;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		delete stmt;
        delete connDb;
        return 0;
	}

	delete stmt;
    delete connDb;

    return 1;
}

// -----  Check simulation file exists in the database by unique key  --------------
int UniDbSimulationFile::CheckSimulationFileExists(TString file_path)
{
	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from simulation_file "
		"where lower(file_path) = lower('%s')", file_path.Data());
    TSQLStatement* stmt = db_server->Statement(sql);

	// get simulation file from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting simulation file from the database has been failed"<<endl;

		delete stmt;
        delete connDb;
        return -2;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		delete stmt;
        delete connDb;
        return 0;
	}

	delete stmt;
    delete connDb;

    return 1;
}

// -----  Delete simulation file from the database  ---------------------------
int UniDbSimulationFile::DeleteSimulationFile(int file_id)
{
	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from simulation_file "
		"where file_id = $1");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, file_id);

	// delete simulation file from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting simulation file from the dataBase has been failed"<<endl;

		delete stmt;
        delete connDb;
        return -2;
	}

	delete stmt;
    delete connDb;
	return 0;
}

// -----  Delete simulation file from the database by unique key  --------------
int UniDbSimulationFile::DeleteSimulationFile(TString file_path)
{
	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from simulation_file "
		"where lower(file_path) = lower($1)");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, file_path);

	// delete simulation file from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting simulation file from the DataBase has been failed"<<endl;

		delete stmt;
        delete connDb;
        return -2;
	}

	delete stmt;
    delete connDb;
	return 0;
}

// -----  Print all 'simulation files'  ---------------------------------
int UniDbSimulationFile::PrintAll()
{
	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select file_id, file_path, generator_name, beam_particle, target_particle, energy, centrality, event_count, file_desc, file_size, file_md5 "
		"from simulation_file");
    TSQLStatement* stmt = db_server->Statement(sql);

	// get all 'simulation files' from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting all 'simulation files' from the dataBase has been failed"<<endl;

		delete stmt;
        delete connDb;
        return -2;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// print rows
	cout<<"Table 'simulation_file':"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<"file_id: ";
		cout<<(stmt->GetInt(0));
		cout<<", file_path: ";
		cout<<(stmt->GetString(1));
		cout<<", generator_name: ";
		cout<<(stmt->GetString(2));
		cout<<", beam_particle: ";
		cout<<(stmt->GetString(3));
		cout<<", target_particle: ";
		if (stmt->IsNull(4)) cout<<"nullptr";
		else
			cout<<stmt->GetString(4);
		cout<<", energy: ";
		if (stmt->IsNull(5)) cout<<"nullptr";
		else
			cout<<stmt->GetDouble(5);
		cout<<", centrality: ";
		cout<<(stmt->GetString(6));
		cout<<", event_count: ";
		if (stmt->IsNull(7)) cout<<"nullptr";
		else
			cout<<stmt->GetInt(7);
		cout<<", file_desc: ";
		if (stmt->IsNull(8)) cout<<"nullptr";
		else
			cout<<stmt->GetString(8);
		cout<<", file_size: ";
		if (stmt->IsNull(9)) cout<<"nullptr";
		else
			cout<<stmt->GetInt(9);
		cout<<", file_md5: ";
		if (stmt->IsNull(10)) cout<<"nullptr";
		else
			cout<<stmt->GetString(10);
		cout<<"."<<endl;
	}

	delete stmt;
    delete connDb;

	return 0;
}


// Setters functions
int UniDbSimulationFile::SetFilePath(TString file_path)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

    TSQLServer* db_server = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set file_path = $1 "
		"where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, file_path);
	stmt->SetInt(1, i_file_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about simulation file has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_file_path = file_path;

	delete stmt;
	return 0;
}

int UniDbSimulationFile::SetGeneratorName(TString generator_name)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

    TSQLServer* db_server = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set generator_name = $1 "
		"where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, generator_name);
	stmt->SetInt(1, i_file_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about simulation file has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_generator_name = generator_name;

	delete stmt;
	return 0;
}

int UniDbSimulationFile::SetBeamParticle(TString beam_particle)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

    TSQLServer* db_server = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set beam_particle = $1 "
		"where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, beam_particle);
	stmt->SetInt(1, i_file_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about simulation file has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_beam_particle = beam_particle;

	delete stmt;
	return 0;
}

int UniDbSimulationFile::SetTargetParticle(TString* target_particle)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

    TSQLServer* db_server = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set target_particle = $1 "
		"where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	if (target_particle == nullptr)
		stmt->SetNull(0);
	else
		stmt->SetString(0, *target_particle);
	stmt->SetInt(1, i_file_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about simulation file has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (str_target_particle)
		delete str_target_particle;
	if (target_particle == nullptr) str_target_particle = nullptr;
	else
		str_target_particle = new TString(*target_particle);

	delete stmt;
	return 0;
}

int UniDbSimulationFile::SetEnergy(double* energy)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

    TSQLServer* db_server = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set energy = $1 "
		"where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	if (energy == nullptr)
		stmt->SetNull(0);
	else
		stmt->SetDouble(0, *energy);
	stmt->SetInt(1, i_file_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about simulation file has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (d_energy)
		delete d_energy;
	if (energy == nullptr) d_energy = nullptr;
	else
		d_energy = new double(*energy);

	delete stmt;
	return 0;
}

int UniDbSimulationFile::SetCentrality(TString centrality)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

    TSQLServer* db_server = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set centrality = $1 "
		"where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, centrality);
	stmt->SetInt(1, i_file_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about simulation file has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_centrality = centrality;

	delete stmt;
	return 0;
}

int UniDbSimulationFile::SetEventCount(int* event_count)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

    TSQLServer* db_server = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set event_count = $1 "
		"where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	if (event_count == nullptr)
		stmt->SetNull(0);
	else
		stmt->SetInt(0, *event_count);
	stmt->SetInt(1, i_file_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about simulation file has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (i_event_count)
		delete i_event_count;
	if (event_count == nullptr) i_event_count = nullptr;
	else
		i_event_count = new int(*event_count);

	delete stmt;
	return 0;
}

int UniDbSimulationFile::SetFileDesc(TString* file_desc)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

    TSQLServer* db_server = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set file_desc = $1 "
		"where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	if (file_desc == nullptr)
		stmt->SetNull(0);
	else
		stmt->SetString(0, *file_desc);
	stmt->SetInt(1, i_file_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about simulation file has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (str_file_desc)
		delete str_file_desc;
	if (file_desc == nullptr) str_file_desc = nullptr;
	else
		str_file_desc = new TString(*file_desc);

	delete stmt;
	return 0;
}

int UniDbSimulationFile::SetFileSize(int* file_size)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

    TSQLServer* db_server = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set file_size = $1 "
		"where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	if (file_size == nullptr)
		stmt->SetNull(0);
	else
		stmt->SetInt(0, *file_size);
	stmt->SetInt(1, i_file_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about simulation file has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (i_file_size)
		delete i_file_size;
	if (file_size == nullptr) i_file_size = nullptr;
	else
		i_file_size = new int(*file_size);

	delete stmt;
	return 0;
}

int UniDbSimulationFile::SetFileMd5(TString* file_md5)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

    TSQLServer* db_server = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update simulation_file "
		"set file_md5 = $1 "
		"where file_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	if (file_md5 == nullptr)
		stmt->SetNull(0);
	else
		stmt->SetString(0, *file_md5);
	stmt->SetInt(1, i_file_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about simulation file has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (chr_file_md5)
		delete chr_file_md5;
	if (file_md5 == nullptr) chr_file_md5 = nullptr;
	else
		chr_file_md5 = new TString(*file_md5);

	delete stmt;
	return 0;
}

// -----  Print current simulation file  ---------------------------------------
void UniDbSimulationFile::Print()
{
	cout<<"Table 'simulation_file'";
	cout<<". file_id: "<<i_file_id<<". file_path: "<<str_file_path<<". generator_name: "<<str_generator_name<<". beam_particle: "<<str_beam_particle<<". target_particle: "<<(str_target_particle == nullptr? "nullptr": *str_target_particle)<<". energy: "<<(d_energy == nullptr? "nullptr": TString::Format("%f", *d_energy))<<". centrality: "<<str_centrality<<". event_count: "<<(i_event_count == nullptr? "nullptr": TString::Format("%d", *i_event_count))<<". file_desc: "<<(str_file_desc == nullptr? "nullptr": *str_file_desc)<<". file_size: "<<(i_file_size == nullptr? "nullptr": TString::Format("%d", *i_file_size))<<". file_md5: "<<(chr_file_md5 == nullptr? "nullptr": *chr_file_md5)<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */


TObjArray* UniDbSimulationFile::GetSimulationFiles()
{
    TObjArray* arrayResult = nullptr;

    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr)
    {
        cout<<"ERROR: connection to the Unified Database was failed"<<endl;
        return arrayResult;
    }

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select file_id, file_path, generator_name, beam_particle, target_particle, energy, centrality, event_count, file_desc, file_size, file_md5 "
        "from simulation_file");
    TSQLStatement* stmt = db_server->Statement(sql);
    //cout<<"SQL code: "<<sql<<endl;

    // get record from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting record from the database has been failed"<<endl;

        delete stmt;
        delete connDb;

        return arrayResult;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // extract rows one after another
    arrayResult = new TObjArray();
    arrayResult->SetOwner(kTRUE);
    while (stmt->NextResultRow())
    {
        UniConnection* connRun = UniConnection::Open();
        if (connRun == nullptr)
        {
            cout<<"ERROR: the connection to the Unified Database for the selected simulation file was failed"<<endl;
            return arrayResult;
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
        if (stmt->IsNull(4)) tmp_target_particle = nullptr;
        else
            tmp_target_particle = new TString(stmt->GetString(4));
        double* tmp_energy;
        if (stmt->IsNull(5)) tmp_energy = nullptr;
        else
            tmp_energy = new double(stmt->GetDouble(5));
        TString tmp_centrality;
        tmp_centrality = stmt->GetString(6);
        int* tmp_event_count;
        if (stmt->IsNull(7)) tmp_event_count = nullptr;
        else
            tmp_event_count = new int(stmt->GetInt(7));
        TString* tmp_file_desc;
        if (stmt->IsNull(8)) tmp_file_desc = nullptr;
        else
            tmp_file_desc = new TString(stmt->GetString(8));
        int* tmp_file_size;
        if (stmt->IsNull(9)) tmp_file_size = nullptr;
        else
            tmp_file_size = new int(stmt->GetInt(9));
        TString* tmp_file_md5;
        if (stmt->IsNull(10)) tmp_file_md5 = nullptr;
        else
            tmp_file_md5 = new TString(stmt->GetString(10));

        arrayResult->Add((TObject*) new UniDbSimulationFile(connDb, tmp_file_id, tmp_file_path, tmp_generator_name, tmp_beam_particle, tmp_target_particle, tmp_energy, tmp_centrality, tmp_event_count, tmp_file_desc, tmp_file_size, tmp_file_md5));
    }

    delete stmt;

    return arrayResult;
}

TObjArray* UniDbSimulationFile::Search(TObjArray& search_conditions)
{
    TObjArray* arrayResult = nullptr;
    search_conditions.SetOwner(kTRUE);

    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr)
    {
        cout<<"ERROR: connection to the Unified Database was failed"<<endl;
        return arrayResult;
    }

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select file_id, file_path, generator_name, beam_particle, target_particle, energy, centrality, event_count, file_desc, file_size, file_md5 "
        "from simulation_file");

    TString strCondition;
    bool isFirst = true;
    TIter next(&search_conditions);
    UniSearchCondition* curCondition;
    while (curCondition = (UniSearchCondition*) next())
    {
        strCondition = "";

        switch (curCondition->GetColumn())
        {
            case UniSearchCondition::columnFilePath:        strCondition += "lower(file_path) "; break;
            case UniSearchCondition::columnGeneratorName:   strCondition += "lower(generator_name) "; break;
            case UniSearchCondition::columnBeamParticle:    strCondition += "lower(beam_particle) "; break;
            case UniSearchCondition::columnTargetParticle:  strCondition += "lower(target_particle) "; break;
            case UniSearchCondition::columnEnergy:          strCondition += "energy "; break;
            case UniSearchCondition::columnCentrality:      strCondition += "lower(centrality) "; break;
            case UniSearchCondition::columnEventCount:      strCondition += "event_count "; break;
            case UniSearchCondition::columnFileDesc:        strCondition += "lower(file_desc) "; break;
            case UniSearchCondition::columnFileSize:        strCondition += "file_size "; break;
            default:
                cout<<"ERROR: the column in the search condition was not defined, the condition is skipped"<<endl;
                continue;
        }

        switch (curCondition->GetCondition())
        {
            case UniSearchCondition::conditionLess:             strCondition += "< "; break;
            case UniSearchCondition::conditionLessOrEqual:      strCondition += "<= "; break;
            case UniSearchCondition::conditionEqual:            strCondition += "= "; break;
            case UniSearchCondition::conditionNotEqual:         strCondition += "<> "; break;
            case UniSearchCondition::conditionGreater:          strCondition += "> "; break;
            case UniSearchCondition::conditionGreaterOrEqual:   strCondition += ">= "; break;
            case UniSearchCondition::conditionLike:             strCondition += "like "; break;
            case UniSearchCondition::conditionNull:             strCondition += "is null "; break;
            case UniSearchCondition::conditionNotNull:          strCondition += "is not null "; break;
            default:
                cout<<"ERROR: the comparison operator in the search condition was not defined, the condition is skipped"<<endl;
                continue;
        }

        switch (curCondition->GetValueType())
        {
            case 0:
                if ((curCondition->GetCondition() != UniSearchCondition::conditionNull) &&
                    (curCondition->GetCondition() != UniSearchCondition::conditionNotNull)) continue;
                break;
            case 1: strCondition += Form("%d", curCondition->GetIntValue()); break;
            case 2: strCondition += Form("%u", curCondition->GetUIntValue()); break;
            case 3: strCondition += Form("%f", curCondition->GetDoubleValue()); break;
            case 4: strCondition += Form("lower('%s')", curCondition->GetStringValue().Data()); break;
            case 5: strCondition += Form("'%s'", curCondition->GetDatimeValue().AsSQLString()); break;
            default:
                cout<<"ERROR: the value type in the search condition was not found, the condition is skipped"<<endl;
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
    sql += " order by generator_name";

    TSQLStatement* stmt = db_server->Statement(sql);
    //cout<<"SQL code: "<<sql<<endl;

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"ERROR: getting simulation files from the Unified Database has been failed"<<endl;
        delete stmt;
        delete connDb;

        return arrayResult;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // extract rows one after another
    arrayResult = new TObjArray();
    arrayResult->SetOwner(kTRUE);
    while (stmt->NextResultRow())
    {
        UniConnection* connRun = UniConnection::Open();
        if (connRun == nullptr)
        {
            cout<<"ERROR: the connection to the Unified Database for the selected simulation file was failed"<<endl;
            return arrayResult;
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
        if (stmt->IsNull(4)) tmp_target_particle = nullptr;
        else
            tmp_target_particle = new TString(stmt->GetString(4));
        double* tmp_energy;
        if (stmt->IsNull(5)) tmp_energy = nullptr;
        else
            tmp_energy = new double(stmt->GetDouble(5));
        TString tmp_centrality;
        tmp_centrality = stmt->GetString(6);
        int* tmp_event_count;
        if (stmt->IsNull(7)) tmp_event_count = nullptr;
        else
            tmp_event_count = new int(stmt->GetInt(7));
        TString* tmp_file_desc;
        if (stmt->IsNull(8)) tmp_file_desc = nullptr;
        else
            tmp_file_desc = new TString(stmt->GetString(8));
        int* tmp_file_size;
        if (stmt->IsNull(9)) tmp_file_size = nullptr;
        else
            tmp_file_size = new int(stmt->GetInt(9));
        TString* tmp_file_md5;
        if (stmt->IsNull(10)) tmp_file_md5 = nullptr;
        else
            tmp_file_md5 = new TString(stmt->GetString(10));

        arrayResult->Add((TObject*) new UniDbSimulationFile(connDb, tmp_file_id, tmp_file_path, tmp_generator_name, tmp_beam_particle, tmp_target_particle, tmp_energy, tmp_centrality, tmp_event_count, tmp_file_desc, tmp_file_size, tmp_file_md5));
    }

    delete stmt;

    return arrayResult;
}

TObjArray* UniDbSimulationFile::Search(UniSearchCondition& search_condition)
{
    TObjArray search_conditions;
    search_conditions.Add((TObject*)&search_condition);

    return Search(search_conditions);
}

// -------------------------------------------------------------------
ClassImp(UniDbSimulationFile);
