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
ElogDbTarget::ElogDbTarget(UniConnection* connUniDb, TString target)
{
	connectionUniDb = connUniDb;

	str_target = target;
}

// -----   Destructor   -------------------------------------------------
ElogDbTarget::~ElogDbTarget()
{
	if (connectionUniDb)
		delete connectionUniDb;
}

// -----   Creating new target in the database  ---------------------------
ElogDbTarget* ElogDbTarget::CreateTarget(TString target)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into target_(target) "
		"values ($1)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, target);

	// inserting new target to the Database
	if (!stmt->Process())
	{
		cout<<"ERROR: inserting new target to the Database has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	TString tmp_target;
	tmp_target = target;

	return new ElogDbTarget(connUniDb, tmp_target);
}

// -----  Get target from the database  ---------------------------
ElogDbTarget* ElogDbTarget::GetTarget(TString target)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select target "
		"from target_ "
		"where lower(target) = lower('%s')", target.Data());
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get target from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting target from the database has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: target was not found in the database"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	TString tmp_target;
	tmp_target = stmt->GetString(0);

	delete stmt;

	return new ElogDbTarget(connUniDb, tmp_target);
}

// -----  Check target exists in the database  ---------------------------
bool ElogDbTarget::CheckTargetExists(TString target)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from target_ "
		"where lower(target) = lower('%s')", target.Data());
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get target from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting target from the database has been failed"<<endl;

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

// -----  Delete target from the database  ---------------------------
int ElogDbTarget::DeleteTarget(TString target)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from target_ "
		"where lower(target) = lower($1)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, target);

	// delete target from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting target from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	delete stmt;
	delete connUniDb;
	return 0;
}

// -----  Print all 'targets'  ---------------------------------
int ElogDbTarget::PrintAll()
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select target "
		"from target_");
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get all 'targets' from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting all 'targets' from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
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
	delete connUniDb;

	return 0;
}


// Setters functions
int ElogDbTarget::SetTarget(TString target)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update target_ "
		"set target = $1 "
		"where target = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

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
