// ----------------------------------------------------------------------
//                    UniDbRunGeometry cxx file
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"
#include "TSystem.h"

#include "UniDbRunGeometry.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniDbRunGeometry::UniDbRunGeometry(UniConnection* db_connect, int geometry_id, unsigned char* root_geometry, Long_t size_root_geometry)
{
    connectionUniDb = db_connect;

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
    TString patch_check("$SIMPATH/pgsql_patched");
    gSystem->ExpandPathName(patch_check);
    if (gSystem->AccessPathName(patch_check))
    {
        cout<<"ERROR: TPgSQLStatement (CERN ROOT) was not patched to write binary data into the Unified Database"<<endl;
        return nullptr;
    }

	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into run_geometry(root_geometry) "
		"values ($1)");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetBinary(0, root_geometry, size_root_geometry, 0x4000000);

	// inserting new run geometry to the Database
	if (!stmt->Process())
	{
		cout<<"ERROR: inserting new run geometry to the Database has been failed"<<endl;
		delete stmt;
        delete connDb;
        return nullptr;
	}

	delete stmt;

	// getting last inserted ID
	int geometry_id;
    TSQLStatement* stmt_last = db_server->Statement("SELECT currval(pg_get_serial_sequence('run_geometry','geometry_id'))");

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
			geometry_id = stmt_last->GetInt(0);
			delete stmt_last;
		}
	}
	else
	{
		cout<<"ERROR: getting last ID has been failed!"<<endl;
		delete stmt_last;
        return nullptr;
	}

	int tmp_geometry_id;
	tmp_geometry_id = geometry_id;
	unsigned char* tmp_root_geometry;
	Long_t tmp_sz_root_geometry = size_root_geometry;
	tmp_root_geometry = new unsigned char[tmp_sz_root_geometry];
	memcpy(tmp_root_geometry, root_geometry, tmp_sz_root_geometry);

    return new UniDbRunGeometry(connDb, tmp_geometry_id, tmp_root_geometry, tmp_sz_root_geometry);
}

// -----  Get run geometry from the database  ---------------------------
UniDbRunGeometry* UniDbRunGeometry::GetRunGeometry(int geometry_id)
{
	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select geometry_id, root_geometry "
		"from run_geometry "
		"where geometry_id = %d", geometry_id);
    TSQLStatement* stmt = db_server->Statement(sql);

	// get run geometry from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting run geometry from the database has been failed"<<endl;

		delete stmt;
        delete connDb;
        return nullptr;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: run geometry was not found in the database"<<endl;

		delete stmt;
        delete connDb;
        return nullptr;
	}

	int tmp_geometry_id;
	tmp_geometry_id = stmt->GetInt(0);
	unsigned char* tmp_root_geometry;
	tmp_root_geometry = nullptr;
	Long_t tmp_sz_root_geometry = 0;
	stmt->GetBinary(1, (void*&)tmp_root_geometry, tmp_sz_root_geometry);

	delete stmt;

    return new UniDbRunGeometry(connDb, tmp_geometry_id, tmp_root_geometry, tmp_sz_root_geometry);
}

// -----  Check run geometry exists in the database  ---------------------------
int UniDbRunGeometry::CheckRunGeometryExists(int geometry_id)
{
	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from run_geometry "
		"where geometry_id = %d", geometry_id);
    TSQLStatement* stmt = db_server->Statement(sql);

	// get run geometry from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting run geometry from the database has been failed"<<endl;

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

// -----  Delete run geometry from the database  ---------------------------
int UniDbRunGeometry::DeleteRunGeometry(int geometry_id)
{
	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from run_geometry "
		"where geometry_id = $1");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, geometry_id);

	// delete run geometry from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting run geometry from the dataBase has been failed"<<endl;

		delete stmt;
        delete connDb;
        return -2;
	}

	delete stmt;
    delete connDb;
	return 0;
}


// -----  Print all 'run geometries'  ---------------------------------
int UniDbRunGeometry::PrintAll()
{
	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select geometry_id, root_geometry "
		"from run_geometry");
    TSQLStatement* stmt = db_server->Statement(sql);

	// get all 'run geometrys' from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting all 'run geometrys' from the dataBase has been failed"<<endl;

		delete stmt;
        delete connDb;
        return -2;
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
		unsigned char* tmp_root_geometry = nullptr;
		Long_t tmp_sz_root_geometry=0;
		stmt->GetBinary(1, (void*&)tmp_root_geometry, tmp_sz_root_geometry);
		cout<<(void*)tmp_root_geometry<<", binary size: "<<tmp_sz_root_geometry;
		cout<<"."<<endl;
	}

	delete stmt;
    delete connDb;

	return 0;
}


// Setters functions
int UniDbRunGeometry::SetRootGeometry(unsigned char* root_geometry, Long_t size_root_geometry)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

    TSQLServer* db_server = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_geometry "
		"set root_geometry = $1 "
		"where geometry_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetBinary(0, root_geometry, size_root_geometry, 0x4000000);
	stmt->SetInt(1, i_geometry_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about run geometry has been failed"<<endl;

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
/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */


// -----   Creating new run geometry in the database  ---------------------------
UniDbRunGeometry* UniDbRunGeometry::CreateRunGeometry(int geometry_id, unsigned char* root_geometry, Long_t size_root_geometry)
{
    TString patch_check("$SIMPATH/pgsql_patched");
    gSystem->ExpandPathName(patch_check);
    if (gSystem->AccessPathName(patch_check))
    {
        cout<<"ERROR: TPgSQLStatement (CERN ROOT) was not patched to write binary data into the Unified Database"<<endl;
        return nullptr;
    }

    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "insert into run_geometry(geometry_id, root_geometry) "
        "values ($1,$2)");
    TSQLStatement* stmt = db_server->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, geometry_id);
    stmt->SetBinary(1, root_geometry, size_root_geometry, 0x40000000);

    // inserting new run geometry to the Database
    if (!stmt->Process())
    {
        cout<<"ERROR: inserting new run geometry to the Database has been failed"<<endl;
        delete stmt;
        delete connDb;
        return nullptr;
    }

    delete stmt;

    int tmp_geometry_id;
    tmp_geometry_id = geometry_id;
    unsigned char* tmp_root_geometry;
    Long_t tmp_sz_root_geometry = size_root_geometry;
    tmp_root_geometry = new unsigned char[tmp_sz_root_geometry];
    memcpy(tmp_root_geometry, root_geometry, tmp_sz_root_geometry);

    return new UniDbRunGeometry(connDb, tmp_geometry_id, tmp_root_geometry, tmp_sz_root_geometry);
}

// -----  Get all 'run geometries'  -----------------------------------
TObjArray* UniDbRunGeometry::GetAll()
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
        "select geometry_id, root_geometry "
        "from run_geometry");
    TSQLStatement* stmt = db_server->Statement(sql);

    // get all 'run geometries' from the database
    if (!stmt->Process())
    {
        cout<<"ERROR: getting all 'run geometries' from the dataBase has been failed"<<endl;
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
            cout<<"ERROR: the connection to the Unified Database for the selected run was failed"<<endl;
            return arrayResult;
        }

        int tmp_geometry_id = stmt->GetInt(0);
        unsigned char* tmp_root_geometry = nullptr;
        Long_t tmp_sz_root_geometry = 0;
        stmt->GetBinary(1, (void*&)tmp_root_geometry, tmp_sz_root_geometry);

        arrayResult->Add((TObject*) new UniDbRunGeometry(connRun, tmp_geometry_id, tmp_root_geometry, tmp_sz_root_geometry));
    }

    delete stmt;

    return arrayResult;
}

// -------------------------------------------------------------------
ClassImp(UniDbRunGeometry);
