// ----------------------------------------------------------------------
//                    ElogDbTrigger cxx file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "ElogDbTrigger.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
ElogDbTrigger::ElogDbTrigger(UniConnection* connUniDb, int trigger_id, TString trigger_info)
{
	connectionUniDb = connUniDb;

	i_trigger_id = trigger_id;
	str_trigger_info = trigger_info;
}

// -----   Destructor   -------------------------------------------------
ElogDbTrigger::~ElogDbTrigger()
{
	if (connectionUniDb)
		delete connectionUniDb;
}

// -----   Creating new trigger in the database  ---------------------------
ElogDbTrigger* ElogDbTrigger::CreateTrigger(TString trigger_info)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into trigger_(trigger_info) "
		"values ($1)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, trigger_info);

	// inserting new trigger to the Database
	if (!stmt->Process())
	{
		cout<<"ERROR: inserting new trigger to the Database has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	// getting last inserted ID
	int trigger_id;
	TSQLStatement* stmt_last = uni_db->Statement("SELECT currval(pg_get_serial_sequence('trigger_','trigger_id'))");

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
			return 0x00;
		}
		else
		{
			trigger_id = stmt_last->GetInt(0);
			delete stmt_last;
		}
	}
	else
	{
		cout<<"ERROR: getting last ID has been failed!"<<endl;
		delete stmt_last;
		return 0x00;
	}

	int tmp_trigger_id;
	tmp_trigger_id = trigger_id;
	TString tmp_trigger_info;
	tmp_trigger_info = trigger_info;

	return new ElogDbTrigger(connUniDb, tmp_trigger_id, tmp_trigger_info);
}

// -----  Get trigger from the database  ---------------------------
ElogDbTrigger* ElogDbTrigger::GetTrigger(int trigger_id)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select trigger_id, trigger_info "
		"from trigger_ "
		"where trigger_id = %d", trigger_id);
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get trigger from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting trigger from the database has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: trigger was not found in the database"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	int tmp_trigger_id;
	tmp_trigger_id = stmt->GetInt(0);
	TString tmp_trigger_info;
	tmp_trigger_info = stmt->GetString(1);

	delete stmt;

	return new ElogDbTrigger(connUniDb, tmp_trigger_id, tmp_trigger_info);
}

// -----  Get trigger from the database by unique key  --------------
ElogDbTrigger* ElogDbTrigger::GetTrigger(TString trigger_info)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select trigger_id, trigger_info "
		"from trigger_ "
		"where lower(trigger_info) = lower('%s')", trigger_info.Data());
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get trigger from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting trigger from the database has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: trigger was not found in the database"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	int tmp_trigger_id;
	tmp_trigger_id = stmt->GetInt(0);
	TString tmp_trigger_info;
	tmp_trigger_info = stmt->GetString(1);

	delete stmt;

	return new ElogDbTrigger(connUniDb, tmp_trigger_id, tmp_trigger_info);
}

// -----  Check trigger exists in the database  ---------------------------
bool ElogDbTrigger::CheckTriggerExists(int trigger_id)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from trigger_ "
		"where trigger_id = %d", trigger_id);
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get trigger from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting trigger from the database has been failed"<<endl;

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

// -----  Check trigger exists in the database by unique key  --------------
bool ElogDbTrigger::CheckTriggerExists(TString trigger_info)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from trigger_ "
		"where lower(trigger_info) = lower('%s')", trigger_info.Data());
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get trigger from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting trigger from the database has been failed"<<endl;

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

// -----  Delete trigger from the database  ---------------------------
int ElogDbTrigger::DeleteTrigger(int trigger_id)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from trigger_ "
		"where trigger_id = $1");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, trigger_id);

	// delete trigger from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting trigger from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	delete stmt;
	delete connUniDb;
	return 0;
}

// -----  Delete trigger from the database by unique key  --------------
int ElogDbTrigger::DeleteTrigger(TString trigger_info)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from trigger_ "
		"where lower(trigger_info) = lower($1)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, trigger_info);

	// delete trigger from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting trigger from the DataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	delete stmt;
	delete connUniDb;
	return 0;
}

// -----  Print all 'triggers'  ---------------------------------
int ElogDbTrigger::PrintAll()
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select trigger_id, trigger_info "
		"from trigger_");
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get all 'triggers' from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting all 'triggers' from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// print rows
	cout<<"Table 'trigger_':"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<"trigger_id: ";
		cout<<(stmt->GetInt(0));
		cout<<", trigger_info: ";
		cout<<(stmt->GetString(1));
		cout<<"."<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int ElogDbTrigger::SetTriggerInfo(TString trigger_info)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update trigger_ "
		"set trigger_info = $1 "
		"where trigger_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, trigger_info);
	stmt->SetInt(1, i_trigger_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about trigger has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_trigger_info = trigger_info;

	delete stmt;
	return 0;
}

// -----  Print current trigger  ---------------------------------------
void ElogDbTrigger::Print()
{
	cout<<"Table 'trigger_'";
	cout<<". trigger_id: "<<i_trigger_id<<". trigger_info: "<<str_trigger_info<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(ElogDbTrigger);
