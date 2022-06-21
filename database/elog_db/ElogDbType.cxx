// ----------------------------------------------------------------------
//                    ElogDbType cxx file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "ElogDbType.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
ElogDbType::ElogDbType(ElogConnection* db_connect, int type_id, TString type_text)
{
    connectionDb = db_connect;

	i_type_id = type_id;
	str_type_text = type_text;
}

// -----   Destructor   -------------------------------------------------
ElogDbType::~ElogDbType()
{
	if (connectionDb)
		delete connectionDb;
}

// -----   Creating new type in the database  ---------------------------
ElogDbType* ElogDbType::CreateType(TString type_text)
{
        ElogConnection* connDb = ElogConnection::Open();
	if (connDb == nullptr) return nullptr;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into type_(type_text) "
		"values ($1)");
	TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, type_text);

	// inserting new type to the Database
	if (!stmt->Process())
	{
		cout<<"ERROR: inserting new type to the Database has been failed"<<endl;
		delete stmt;
		delete connDb;
		return nullptr;
	}

	delete stmt;

	// getting last inserted ID
	int type_id;
	TSQLStatement* stmt_last = db_server->Statement("SELECT currval(pg_get_serial_sequence('type_','type_id'))");

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
			type_id = stmt_last->GetInt(0);
			delete stmt_last;
		}
	}
	else
	{
		cout<<"ERROR: getting last ID has been failed!"<<endl;
		delete stmt_last;
		return nullptr;
	}

	int tmp_type_id;
	tmp_type_id = type_id;
	TString tmp_type_text;
	tmp_type_text = type_text;

	return new ElogDbType(connDb, tmp_type_id, tmp_type_text);
}

// -----  Get type from the database  ---------------------------
ElogDbType* ElogDbType::GetType(int type_id)
{
        ElogConnection* connDb = ElogConnection::Open();
	if (connDb == nullptr) return nullptr;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select type_id, type_text "
		"from type_ "
		"where type_id = %d", type_id);
	TSQLStatement* stmt = db_server->Statement(sql);

	// get type from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting type from the database has been failed"<<endl;

		delete stmt;
		delete connDb;
		return nullptr;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: type was not found in the database"<<endl;

		delete stmt;
		delete connDb;
		return nullptr;
	}

	int tmp_type_id;
	tmp_type_id = stmt->GetInt(0);
	TString tmp_type_text;
	tmp_type_text = stmt->GetString(1);

	delete stmt;

	return new ElogDbType(connDb, tmp_type_id, tmp_type_text);
}

// -----  Get type from the database by unique key  --------------
ElogDbType* ElogDbType::GetType(TString type_text)
{
        ElogConnection* connDb = ElogConnection::Open();
	if (connDb == nullptr) return nullptr;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select type_id, type_text "
		"from type_ "
		"where lower(type_text) = lower('%s')", type_text.Data());
	TSQLStatement* stmt = db_server->Statement(sql);

	// get type from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting type from the database has been failed"<<endl;

		delete stmt;
		delete connDb;
		return nullptr;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: type was not found in the database"<<endl;

		delete stmt;
		delete connDb;
		return nullptr;
	}

	int tmp_type_id;
	tmp_type_id = stmt->GetInt(0);
	TString tmp_type_text;
	tmp_type_text = stmt->GetString(1);

	delete stmt;

	return new ElogDbType(connDb, tmp_type_id, tmp_type_text);
}

// -----  Check type exists in the database  ---------------------------
int ElogDbType::CheckTypeExists(int type_id)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from type_ "
		"where type_id = %d", type_id);
	TSQLStatement* stmt = db_server->Statement(sql);

	// get type from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting type from the database has been failed"<<endl;

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

// -----  Check type exists in the database by unique key  --------------
int ElogDbType::CheckTypeExists(TString type_text)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from type_ "
		"where lower(type_text) = lower('%s')", type_text.Data());
	TSQLStatement* stmt = db_server->Statement(sql);

	// get type from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting type from the database has been failed"<<endl;

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

// -----  Delete type from the database  ---------------------------
int ElogDbType::DeleteType(int type_id)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from type_ "
		"where type_id = $1");
	TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, type_id);

	// delete type from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting type from the dataBase has been failed"<<endl;

		delete stmt;
		delete connDb;
        return -2;
	}

	delete stmt;
	delete connDb;
	return 0;
}

// -----  Delete type from the database by unique key  --------------
int ElogDbType::DeleteType(TString type_text)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from type_ "
		"where lower(type_text) = lower($1)");
	TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, type_text);

	// delete type from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting type from the DataBase has been failed"<<endl;

		delete stmt;
		delete connDb;
        return -2;
	}

	delete stmt;
	delete connDb;
	return 0;
}

// -----  Print all 'types'  ---------------------------------
int ElogDbType::PrintAll()
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select type_id, type_text "
		"from type_");
	TSQLStatement* stmt = db_server->Statement(sql);

	// get all 'types' from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting all 'types' from the dataBase has been failed"<<endl;

		delete stmt;
		delete connDb;
        return -2;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// print rows
	cout<<"Table 'type_':"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<"type_id: ";
		cout<<(stmt->GetInt(0));
		cout<<", type_text: ";
		cout<<(stmt->GetString(1));
		cout<<"."<<endl;
	}

	delete stmt;
	delete connDb;

	return 0;
}


// Setters functions
int ElogDbType::SetTypeText(TString type_text)
{
	if (!connectionDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* db_server = connectionDb->GetSQLServer();

	TString sql = TString::Format(
		"update type_ "
		"set type_text = $1 "
		"where type_id = $2");
	TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, type_text);
	stmt->SetInt(1, i_type_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about type has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_type_text = type_text;

	delete stmt;
	return 0;
}

// -----  Print current type  ---------------------------------------
void ElogDbType::Print()
{
	cout<<"Table 'type_'";
	cout<<". type_id: "<<i_type_id<<". type_text: "<<str_type_text<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(ElogDbType);
