// ----------------------------------------------------------------------
//                    ElogDbTarget cxx file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "ElogDbTarget.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
ElogDbTarget::ElogDbTarget(ElogConnection* db_connect, TString target)
{
    connectionDb = db_connect;

	str_target = target;
}

// -----   Destructor   -------------------------------------------------
ElogDbTarget::~ElogDbTarget()
{
	if (connectionDb)
		delete connectionDb;
}

// -----   Creating new target in the database  ---------------------------
ElogDbTarget* ElogDbTarget::CreateTarget(TString target)
{
        ElogConnection* connDb = ElogConnection::Open();
	if (connDb == nullptr) return nullptr;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into target_(target) "
		"values ($1)");
	TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, target);

	// inserting new target to the Database
	if (!stmt->Process())
	{
		cout<<"ERROR: inserting new target to the Database has been failed"<<endl;
		delete stmt;
		delete connDb;
		return nullptr;
	}

	delete stmt;

	TString tmp_target;
	tmp_target = target;

	return new ElogDbTarget(connDb, tmp_target);
}

// -----  Get target from the database  ---------------------------
ElogDbTarget* ElogDbTarget::GetTarget(TString target)
{
        ElogConnection* connDb = ElogConnection::Open();
	if (connDb == nullptr) return nullptr;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select target "
		"from target_ "
		"where lower(target) = lower('%s')", target.Data());
	TSQLStatement* stmt = db_server->Statement(sql);

	// get target from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting target from the database has been failed"<<endl;

		delete stmt;
		delete connDb;
		return nullptr;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: target was not found in the database"<<endl;

		delete stmt;
		delete connDb;
		return nullptr;
	}

	TString tmp_target;
	tmp_target = stmt->GetString(0);

	delete stmt;

	return new ElogDbTarget(connDb, tmp_target);
}

// -----  Check target exists in the database  ---------------------------
int ElogDbTarget::CheckTargetExists(TString target)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from target_ "
		"where lower(target) = lower('%s')", target.Data());
	TSQLStatement* stmt = db_server->Statement(sql);

	// get target from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting target from the database has been failed"<<endl;

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

// -----  Delete target from the database  ---------------------------
int ElogDbTarget::DeleteTarget(TString target)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from target_ "
		"where lower(target) = lower($1)");
	TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, target);

	// delete target from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting target from the dataBase has been failed"<<endl;

		delete stmt;
		delete connDb;
        return -2;
	}

	delete stmt;
	delete connDb;
	return 0;
}

// -----  Print all 'targets'  ---------------------------------
int ElogDbTarget::PrintAll()
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select target "
		"from target_");
	TSQLStatement* stmt = db_server->Statement(sql);

	// get all 'targets' from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting all 'targets' from the dataBase has been failed"<<endl;

		delete stmt;
		delete connDb;
        return -2;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// print rows
	cout<<"Table 'target_':"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<"target: ";
		cout<<(stmt->GetString(0));
		cout<<"."<<endl;
	}

	delete stmt;
	delete connDb;

	return 0;
}


// Setters functions
int ElogDbTarget::SetTarget(TString target)
{
	if (!connectionDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* db_server = connectionDb->GetSQLServer();

	TString sql = TString::Format(
		"update target_ "
		"set target = $1 "
		"where target = $2");
	TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, target);
	stmt->SetString(1, str_target);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about target has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_target = target;

	delete stmt;
	return 0;
}

// -----  Print current target  ---------------------------------------
void ElogDbTarget::Print()
{
	cout<<"Table 'target_'";
	cout<<". target: "<<str_target<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(ElogDbTarget);
