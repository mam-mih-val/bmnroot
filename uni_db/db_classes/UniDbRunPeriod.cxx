// ----------------------------------------------------------------------
//                    UniDbRunPeriod cxx file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "UniDbRunPeriod.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniDbRunPeriod::UniDbRunPeriod(UniDbConnection* connUniDb, int period_number, TDatime start_datetime, TDatime* end_datetime)
{
	connectionUniDb = connUniDb;

	i_period_number = period_number;
	dt_start_datetime = start_datetime;
	dt_end_datetime = end_datetime;
}

// -----   Destructor   -------------------------------------------------
UniDbRunPeriod::~UniDbRunPeriod()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (dt_end_datetime)
		delete dt_end_datetime;
}

// -----   Creating new run period in the database  ---------------------------
UniDbRunPeriod* UniDbRunPeriod::CreateRunPeriod(int period_number, TDatime start_datetime, TDatime* end_datetime)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into run_period(period_number, start_datetime, end_datetime) "
		"values ($1, $2, $3)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, period_number);
	stmt->SetDatime(1, start_datetime);
	if (end_datetime == NULL)
		stmt->SetNull(2);
	else
		stmt->SetDatime(2, *end_datetime);

	// inserting new run period to the Database
	if (!stmt->Process())
	{
		cout<<"Error: inserting new run period to the Database has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	int tmp_period_number;
	tmp_period_number = period_number;
	TDatime tmp_start_datetime;
	tmp_start_datetime = start_datetime;
	TDatime* tmp_end_datetime;
	if (end_datetime == NULL) tmp_end_datetime = NULL;
	else
		tmp_end_datetime = new TDatime(*end_datetime);

	return new UniDbRunPeriod(connUniDb, tmp_period_number, tmp_start_datetime, tmp_end_datetime);
}

// -----  Get run period from the database  ---------------------------
UniDbRunPeriod* UniDbRunPeriod::GetRunPeriod(int period_number)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select period_number, start_datetime, end_datetime "
		"from run_period "
		"where period_number = %d", period_number);
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get run period from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting run period from the database has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"Error: run period wasn't found in the database"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	int tmp_period_number;
	tmp_period_number = stmt->GetInt(0);
	TDatime tmp_start_datetime;
	tmp_start_datetime = stmt->GetDatime(1);
	TDatime* tmp_end_datetime;
	if (stmt->IsNull(2)) tmp_end_datetime = NULL;
	else
		tmp_end_datetime = new TDatime(stmt->GetDatime(2));

	delete stmt;

	return new UniDbRunPeriod(connUniDb, tmp_period_number, tmp_start_datetime, tmp_end_datetime);
}

// -----  Check run period exists in the database  ---------------------------
bool UniDbRunPeriod::CheckRunPeriodExists(int period_number)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from run_period "
		"where period_number = %d", period_number);
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get run period from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting run period from the database has been failed"<<endl;

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

// -----  Delete run period from the database  ---------------------------
int UniDbRunPeriod::DeleteRunPeriod(int period_number)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from run_period "
		"where period_number = $1");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, period_number);

	// delete run period from the dataBase
	if (!stmt->Process())
	{
		cout<<"Error: deleting run period from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	delete stmt;
	delete connUniDb;
	return 0;
}

// -----  Print all 'run periods'  ---------------------------------
int UniDbRunPeriod::PrintAll()
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select period_number, start_datetime, end_datetime "
		"from run_period");
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get all 'run periods' from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting all 'run periods' from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// print rows
	cout<<"Table 'run_period':"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<"period_number: ";
		cout<<(stmt->GetInt(0));
		cout<<", start_datetime: ";
		cout<<(stmt->GetDatime(1)).AsSQLString();
		cout<<", end_datetime: ";
		if (stmt->IsNull(2)) cout<<"NULL";
		else
			cout<<stmt->GetDatime(2).AsSQLString();
		cout<<"."<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int UniDbRunPeriod::SetPeriodNumber(int period_number)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_period "
		"set period_number = $1 "
		"where period_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, period_number);
	stmt->SetInt(1, i_period_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run period has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_period_number = period_number;

	delete stmt;
	return 0;
}

int UniDbRunPeriod::SetStartDatetime(TDatime start_datetime)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_period "
		"set start_datetime = $1 "
		"where period_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetDatime(0, start_datetime);
	stmt->SetInt(1, i_period_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run period has been failed"<<endl;

		delete stmt;
		return -2;
	}

	dt_start_datetime = start_datetime;

	delete stmt;
	return 0;
}

int UniDbRunPeriod::SetEndDatetime(TDatime* end_datetime)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_period "
		"set end_datetime = $1 "
		"where period_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (end_datetime == NULL)
		stmt->SetNull(0);
	else
		stmt->SetDatime(0, *end_datetime);
	stmt->SetInt(1, i_period_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about run period has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (dt_end_datetime)
		delete dt_end_datetime;
	if (end_datetime == NULL) dt_end_datetime = NULL;
	else
		dt_end_datetime = new TDatime(*end_datetime);

	delete stmt;
	return 0;
}

// -----  Print current run period  ---------------------------------------
void UniDbRunPeriod::Print()
{
	cout<<"Table 'run_period'";
	cout<<". period_number: "<<i_period_number<<". start_datetime: "<<dt_start_datetime.AsSQLString()<<". end_datetime: "<<(dt_end_datetime == NULL? "NULL": (*dt_end_datetime).AsSQLString())<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(UniDbRunPeriod);
