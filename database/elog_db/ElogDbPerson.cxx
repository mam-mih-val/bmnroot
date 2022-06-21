// ----------------------------------------------------------------------
//                    ElogDbPerson cxx file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "ElogDbPerson.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
ElogDbPerson::ElogDbPerson(ElogConnection* db_connect, int person_id, TString person_name, int is_active)
{
    connectionDb = db_connect;

	i_person_id = person_id;
	str_person_name = person_name;
	i_is_active = is_active;
}

// -----   Destructor   -------------------------------------------------
ElogDbPerson::~ElogDbPerson()
{
	if (connectionDb)
		delete connectionDb;
}

// -----   Creating new person in the database  ---------------------------
ElogDbPerson* ElogDbPerson::CreatePerson(TString person_name, int is_active)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into person_(person_name, is_active) "
		"values ($1, $2)");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, person_name);
	stmt->SetInt(1, is_active);

	// inserting new person to the Database
	if (!stmt->Process())
	{
		cout<<"ERROR: inserting new person to the Database has been failed"<<endl;
		delete stmt;
        delete connDb;
        return nullptr;
	}

	delete stmt;

	// getting last inserted ID
	int person_id;
    TSQLStatement* stmt_last = db_server->Statement("SELECT currval(pg_get_serial_sequence('person_','person_id'))");

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
			person_id = stmt_last->GetInt(0);
			delete stmt_last;
		}
	}
	else
	{
		cout<<"ERROR: getting last ID has been failed!"<<endl;
		delete stmt_last;
        return nullptr;
	}

	int tmp_person_id;
	tmp_person_id = person_id;
	TString tmp_person_name;
	tmp_person_name = person_name;
	int tmp_is_active;
	tmp_is_active = is_active;

    return new ElogDbPerson(connDb, tmp_person_id, tmp_person_name, tmp_is_active);
}

// -----  Get person from the database  ---------------------------
ElogDbPerson* ElogDbPerson::GetPerson(int person_id)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select person_id, person_name, is_active "
		"from person_ "
		"where person_id = %d", person_id);
    TSQLStatement* stmt = db_server->Statement(sql);

	// get person from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting person from the database has been failed"<<endl;

		delete stmt;
        delete connDb;
        return nullptr;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: person was not found in the database"<<endl;

		delete stmt;
        delete connDb;
        return nullptr;
	}

	int tmp_person_id;
	tmp_person_id = stmt->GetInt(0);
	TString tmp_person_name;
	tmp_person_name = stmt->GetString(1);
	int tmp_is_active;
	tmp_is_active = stmt->GetInt(2);

	delete stmt;

    return new ElogDbPerson(connDb, tmp_person_id, tmp_person_name, tmp_is_active);
}

// -----  Get person from the database by unique key  --------------
ElogDbPerson* ElogDbPerson::GetPerson(TString person_name)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select person_id, person_name, is_active "
		"from person_ "
		"where lower(person_name) = lower('%s')", person_name.Data());
    TSQLStatement* stmt = db_server->Statement(sql);

	// get person from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting person from the database has been failed"<<endl;

		delete stmt;
        delete connDb;
        return nullptr;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: person was not found in the database"<<endl;

		delete stmt;
        delete connDb;
        return nullptr;
	}

	int tmp_person_id;
	tmp_person_id = stmt->GetInt(0);
	TString tmp_person_name;
	tmp_person_name = stmt->GetString(1);
	int tmp_is_active;
	tmp_is_active = stmt->GetInt(2);

	delete stmt;

    return new ElogDbPerson(connDb, tmp_person_id, tmp_person_name, tmp_is_active);
}

// -----  Check person exists in the database  ---------------------------
int ElogDbPerson::CheckPersonExists(int person_id)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from person_ "
		"where person_id = %d", person_id);
    TSQLStatement* stmt = db_server->Statement(sql);

	// get person from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting person from the database has been failed"<<endl;

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

// -----  Check person exists in the database by unique key  --------------
int ElogDbPerson::CheckPersonExists(TString person_name)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from person_ "
		"where lower(person_name) = lower('%s')", person_name.Data());
    TSQLStatement* stmt = db_server->Statement(sql);

	// get person from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting person from the database has been failed"<<endl;

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

// -----  Delete person from the database  ---------------------------
int ElogDbPerson::DeletePerson(int person_id)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from person_ "
		"where person_id = $1");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, person_id);

	// delete person from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting person from the dataBase has been failed"<<endl;

		delete stmt;
        delete connDb;
        return -2;
	}

	delete stmt;
    delete connDb;
	return 0;
}

// -----  Delete person from the database by unique key  --------------
int ElogDbPerson::DeletePerson(TString person_name)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from person_ "
		"where lower(person_name) = lower($1)");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, person_name);

	// delete person from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting person from the DataBase has been failed"<<endl;

		delete stmt;
        delete connDb;
        return -2;
	}

	delete stmt;
    delete connDb;
	return 0;
}

// -----  Print all 'persons'  ---------------------------------
int ElogDbPerson::PrintAll()
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select person_id, person_name, is_active "
		"from person_");
    TSQLStatement* stmt = db_server->Statement(sql);

	// get all 'persons' from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting all 'persons' from the dataBase has been failed"<<endl;

		delete stmt;
        delete connDb;
        return -2;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// print rows
	cout<<"Table 'person_':"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<"person_id: ";
		cout<<(stmt->GetInt(0));
		cout<<", person_name: ";
		cout<<(stmt->GetString(1));
		cout<<", is_active: ";
		cout<<(stmt->GetInt(2));
		cout<<"."<<endl;
	}

	delete stmt;
    delete connDb;

	return 0;
}


// Setters functions
int ElogDbPerson::SetPersonName(TString person_name)
{
	if (!connectionDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

    TSQLServer* db_server = connectionDb->GetSQLServer();

	TString sql = TString::Format(
		"update person_ "
		"set person_name = $1 "
		"where person_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, person_name);
	stmt->SetInt(1, i_person_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about person has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_person_name = person_name;

	delete stmt;
	return 0;
}

int ElogDbPerson::SetIsActive(int is_active)
{
	if (!connectionDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

    TSQLServer* db_server = connectionDb->GetSQLServer();

	TString sql = TString::Format(
		"update person_ "
		"set is_active = $1 "
		"where person_id = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, is_active);
	stmt->SetInt(1, i_person_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about person has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_is_active = is_active;

	delete stmt;
	return 0;
}

// -----  Print current person  ---------------------------------------
void ElogDbPerson::Print()
{
	cout<<"Table 'person_'";
	cout<<". person_id: "<<i_person_id<<". person_name: "<<str_person_name<<". is_active: "<<i_is_active<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(ElogDbPerson);
