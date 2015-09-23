// ----------------------------------------------------------------------
//                    MpdDbMapping cxx file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "MpdDbMapping.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
MpdDbMapping::MpdDbMapping(MpdDbConnection* connUniDb, int map_id, int map_type)
{
	connectionUniDb = connUniDb;

	i_map_id = map_id;
	i_map_type = map_type;
}

// -----   Destructor   -------------------------------------------------
MpdDbMapping::~MpdDbMapping()
{
	if (connectionUniDb)
		delete connectionUniDb;
}

// -----   Creating new record in class table ---------------------------
MpdDbMapping* MpdDbMapping::CreateMapping(int map_type)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into mapping_(map_type) "
		"values ($1)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, map_type);

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
	int map_id;
	TSQLStatement* stmt_last = uni_db->Statement("SELECT currval(pg_get_serial_sequence('mapping_','map_id'))");

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
			map_id = stmt_last->GetInt(0);
			delete stmt_last;
		}
	}
	else
	{
		cout<<"Error: getting last ID has been failed!"<<endl;
		delete stmt_last;
		return 0x00;
	}

	int tmp_map_id;
	tmp_map_id = map_id;
	int tmp_map_type;
	tmp_map_type = map_type;

	return new MpdDbMapping(connUniDb, tmp_map_id, tmp_map_type);
}

// -----   Get table record from database ---------------------------
MpdDbMapping* MpdDbMapping::GetMapping(int map_id)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select map_id, map_type "
		"from mapping_ "
		"where map_id = %d", map_id);
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

	int tmp_map_id;
	tmp_map_id = stmt->GetInt(0);
	int tmp_map_type;
	tmp_map_type = stmt->GetInt(1);

	delete stmt;

	return new MpdDbMapping(connUniDb, tmp_map_id, tmp_map_type);
}

// -----   Delete record from class table ---------------------------
int MpdDbMapping::DeleteMapping(int map_id)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from mapping_ "
		"where map_id = $1");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, map_id);

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
int MpdDbMapping::PrintAll()
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select map_id, map_type "
		"from mapping_");
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
	cout<<"Table 'mapping_'"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<". map_id: ";
		cout<<(stmt->GetInt(0));
		cout<<". map_type: ";
		cout<<(stmt->GetInt(1));
		cout<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int MpdDbMapping::SetMapType(int map_type)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update mapping_ "
		"set map_type = $1 "
		"where map_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, map_type);
	stmt->SetInt(1, i_map_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_map_type = map_type;

	delete stmt;
	return 0;
}

// -----   Print current record ---------------------------------------
void MpdDbMapping::Print()
{
	cout<<"Table 'mapping_'";
	cout<<". map_id: "<<i_map_id<<". map_type: "<<i_map_type<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(MpdDbMapping);
