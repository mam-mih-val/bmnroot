// ----------------------------------------------------------------------
//                    UniDbRunGeometry cxx file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "UniDbRunGeometry.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniDbRunGeometry::UniDbRunGeometry(UniDbConnection* connUniDb, int geometry_id, unsigned char* root_geometry, Long_t size_root_geometry)
{
	connectionUniDb = connUniDb;

	i_geometry_id = geometry_id;
	blob_root_geometry = root_geometry;
	sz_root_geometry = size_root_geometry;
}

// -----   Destructor   -------------------------------------------------
UniDbRunGeometry::~UniDbRunGeometry()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (blob_root_geometry)
		delete [] blob_root_geometry;
}

// -----   Creating new run geometry in the database  ---------------------------
UniDbRunGeometry* UniDbRunGeometry::CreateRunGeometry(unsigned char* root_geometry, Long_t size_root_geometry)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into run_geometry(root_geometry) "
		"values ($1)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetLargeObject(0, root_geometry, size_root_geometry, 0x4000000);

	// inserting new run geometry to the Database
	if (!stmt->Process())
	{
		cout<<"Error: inserting new run geometry to the Database has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	// getting last inserted ID
	int geometry_id;
	TSQLStatement* stmt_last = uni_db->Statement("SELECT currval(pg_get_serial_sequence('run_geometry','geometry_id'))");

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
			geometry_id = stmt_last->GetInt(0);
			delete stmt_last;
		}
	}
	else
	{
		cout<<"Error: getting last ID has been failed!"<<endl;
		delete stmt_last;
		return 0x00;
	}

	int tmp_geometry_id;
	tmp_geometry_id = geometry_id;
	unsigned char* tmp_root_geometry;
	Long_t tmp_sz_root_geometry = size_root_geometry;
	tmp_root_geometry = new unsigned char[tmp_sz_root_geometry];
	memcpy(tmp_root_geometry, root_geometry, tmp_sz_root_geometry);

	return new UniDbRunGeometry(connUniDb, tmp_geometry_id, tmp_root_geometry, tmp_sz_root_geometry);
}

// -----  Get run geometry from the database  ---------------------------
UniDbRunGeometry* UniDbRunGeometry::GetRunGeometry(int geometry_id)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select geometry_id, root_geometry "
		"from run_geometry "
		"where geometry_id = %d", geometry_id);
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get run geometry from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting run geometry from the database has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"Error: run geometry wasn't found in the database"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	int tmp_geometry_id;
	tmp_geometry_id = stmt->GetInt(0);
	unsigned char* tmp_root_geometry;
	tmp_root_geometry = NULL;
	Long_t tmp_sz_root_geometry = 0;
	stmt->GetLargeObject(1, (void*&)tmp_root_geometry, tmp_sz_root_geometry);

	delete stmt;

	return new UniDbRunGeometry(connUniDb, tmp_geometry_id, tmp_root_geometry, tmp_sz_root_geometry);
}

// -----  Check run geometry exists in the database  ---------------------------
bool UniDbRunGeometry::CheckRunGeometryExists(int geometry_id)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from run_geometry "
		"where geometry_id = %d", geometry_id);
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get run geometry from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting run geometry from the database has been failed"<<endl;

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

// -----  Delete run geometry from the database  ---------------------------
int UniDbRunGeometry::DeleteRunGeometry(int geometry_id)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from run_geometry "
		"where geometry_id = $1");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, geometry_id);

	// delete run geometry from the dataBase
	if (!stmt->Process())
	{
		cout<<"Error: deleting run geometry from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	delete stmt;
	delete connUniDb;
	return 0;
}

// -----  Print all 'run geometrys'  ---------------------------------
int UniDbRunGeometry::PrintAll()
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select geometry_id, root_geometry "
		"from run_geometry");
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get all 'run geometrys' from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting all 'run geometrys' from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// print rows
	cout<<"Table 'run_geometry':"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<"geometry_id: ";
		cout<<(stmt->GetInt(0));
		cout<<", root_geometry: ";
		unsigned char* tmp_root_geometry = NULL;
		Long_t tmp_sz_root_geometry=0;
		stmt->GetLargeObject(1, (void*&)tmp_root_geometry, tmp_sz_root_geometry);
		cout<<(void*)tmp_root_geometry<<", binary size: "<<tmp_sz_root_geometry;
		cout<<"."<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int UniDbRunGeometry::SetRootGeometry(unsigned char* root_geometry, Long_t size_root_geometry)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_geometry "
		"set root_geometry = $1 "
		"where geometry_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetLargeObject(0, root_geometry, size_root_geometry, 0x4000000);
	stmt->SetInt(1, i_geometry_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run geometry has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (blob_root_geometry)
		delete [] blob_root_geometry;
	sz_root_geometry = size_root_geometry;
	blob_root_geometry = new unsigned char[sz_root_geometry];
	memcpy(blob_root_geometry, root_geometry, sz_root_geometry);

	delete stmt;
	return 0;
}

// -----  Print current run geometry  ---------------------------------------
void UniDbRunGeometry::Print()
{
	cout<<"Table 'run_geometry'";
	cout<<". geometry_id: "<<i_geometry_id<<". root_geometry: "<<(void*)blob_root_geometry<<", binary size: "<<sz_root_geometry<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(UniDbRunGeometry);
