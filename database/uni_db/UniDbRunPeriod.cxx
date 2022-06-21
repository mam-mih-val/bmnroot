// ----------------------------------------------------------------------
//                    UniDbRunPeriod cxx file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "UniDbRunPeriod.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniDbRunPeriod::UniDbRunPeriod(UniConnection* db_connect, int period_number, TDatime start_datetime, TDatime* end_datetime)
{
    connectionUniDb = db_connect;

	i_period_number = period_number;
	ts_start_datetime = start_datetime;
	ts_end_datetime = end_datetime;
}

// -----   Destructor   -------------------------------------------------
UniDbRunPeriod::~UniDbRunPeriod()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (ts_end_datetime)
		delete ts_end_datetime;
}

// -----   Creating new run period in the database  ---------------------------
UniDbRunPeriod* UniDbRunPeriod::CreateRunPeriod(int period_number, TDatime start_datetime, TDatime* end_datetime)
{
	UniConnection* connDb = UniConnection::Open();
	if (connDb == nullptr) return nullptr;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into run_period(period_number, start_datetime, end_datetime) "
		"values ($1, $2, $3)");
	TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, period_number);
	stmt->SetDatime(1, start_datetime);
	if (end_datetime == nullptr)
		stmt->SetNull(2);
	else
		stmt->SetDatime(2, *end_datetime);

	// inserting new run period to the Database
	if (!stmt->Process())
	{
		cout<<"ERROR: inserting new run period to the Database has been failed"<<endl;
		delete stmt;
		delete connDb;
		return nullptr;
	}

	delete stmt;

	int tmp_period_number;
	tmp_period_number = period_number;
	TDatime tmp_start_datetime;
	tmp_start_datetime = start_datetime;
	TDatime* tmp_end_datetime;
	if (end_datetime == nullptr) tmp_end_datetime = nullptr;
	else
		tmp_end_datetime = new TDatime(*end_datetime);

	return new UniDbRunPeriod(connDb, tmp_period_number, tmp_start_datetime, tmp_end_datetime);
}

// -----  Get run period from the database  ---------------------------
UniDbRunPeriod* UniDbRunPeriod::GetRunPeriod(int period_number)
{
	UniConnection* connDb = UniConnection::Open();
	if (connDb == nullptr) return nullptr;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select period_number, start_datetime, end_datetime "
		"from run_period "
		"where period_number = %d", period_number);
	TSQLStatement* stmt = db_server->Statement(sql);

	// get run period from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting run period from the database has been failed"<<endl;

		delete stmt;
		delete connDb;
		return nullptr;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: run period was not found in the database"<<endl;

		delete stmt;
		delete connDb;
		return nullptr;
	}

	int tmp_period_number;
	tmp_period_number = stmt->GetInt(0);
	TDatime tmp_start_datetime;
	tmp_start_datetime = stmt->GetDatime(1);
	TDatime* tmp_end_datetime;
	if (stmt->IsNull(2)) tmp_end_datetime = nullptr;
	else
		tmp_end_datetime = new TDatime(stmt->GetDatime(2));

	delete stmt;

	return new UniDbRunPeriod(connDb, tmp_period_number, tmp_start_datetime, tmp_end_datetime);
}

// -----  Check run period exists in the database  ---------------------------
int UniDbRunPeriod::CheckRunPeriodExists(int period_number)
{
	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from run_period "
		"where period_number = %d", period_number);
	TSQLStatement* stmt = db_server->Statement(sql);

	// get run period from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting run period from the database has been failed"<<endl;

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

// -----  Delete run period from the database  ---------------------------
int UniDbRunPeriod::DeleteRunPeriod(int period_number)
{
	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from run_period "
		"where period_number = $1");
	TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, period_number);

	// delete run period from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting run period from the dataBase has been failed"<<endl;

		delete stmt;
		delete connDb;
        return -2;
	}

	delete stmt;
	delete connDb;
	return 0;
}

// -----  Print all 'run periods'  ---------------------------------
int UniDbRunPeriod::PrintAll()
{
	UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr) return -1;

	TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select period_number, start_datetime, end_datetime "
		"from run_period");
	TSQLStatement* stmt = db_server->Statement(sql);

	// get all 'run periods' from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting all 'run periods' from the dataBase has been failed"<<endl;

		delete stmt;
		delete connDb;
        return -2;
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
		if (stmt->IsNull(2)) cout<<"nullptr";
		else
			cout<<stmt->GetDatime(2).AsSQLString();
		cout<<"."<<endl;
	}

	delete stmt;
	delete connDb;

	return 0;
}


// Setters functions
int UniDbRunPeriod::SetPeriodNumber(int period_number)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* db_server = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_period "
		"set period_number = $1 "
		"where period_number = $2");
	TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, period_number);
	stmt->SetInt(1, i_period_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about run period has been failed"<<endl;

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
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* db_server = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_period "
		"set start_datetime = $1 "
		"where period_number = $2");
	TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetDatime(0, start_datetime);
	stmt->SetInt(1, i_period_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about run period has been failed"<<endl;

		delete stmt;
		return -2;
	}

	ts_start_datetime = start_datetime;

	delete stmt;
	return 0;
}

int UniDbRunPeriod::SetEndDatetime(TDatime* end_datetime)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* db_server = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_period "
		"set end_datetime = $1 "
		"where period_number = $2");
	TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	if (end_datetime == nullptr)
		stmt->SetNull(0);
	else
		stmt->SetDatime(0, *end_datetime);
	stmt->SetInt(1, i_period_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about run period has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (ts_end_datetime)
		delete ts_end_datetime;
	if (end_datetime == nullptr) ts_end_datetime = nullptr;
	else
		ts_end_datetime = new TDatime(*end_datetime);

	delete stmt;
	return 0;
}

// -----  Print current run period  ---------------------------------------
void UniDbRunPeriod::Print()
{
	cout<<"Table 'run_period'";
	cout<<". period_number: "<<i_period_number<<". start_datetime: "<<ts_start_datetime.AsSQLString()<<". end_datetime: "<<(ts_end_datetime == nullptr? "nullptr": (*ts_end_datetime).AsSQLString())<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */

// get numbers of runs existing in the Database for a selected period
int UniDbRunPeriod::GetRunNumbers(int period_number, UniqueRunNumber*& run_numbers)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr)
    {
        cout<<"ERROR: connection to the database was failed"<<endl;
        return -1;
    }

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select run_number "
        "from run_ "
        "where period_number = %d "
        "order by run_number", period_number);
    TSQLStatement* stmt = db_server->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"ERROR: getting run numbers from the database failed"<<endl;
        delete stmt;
        delete connDb;
        return -2;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    vector<int> vecRuns;
    while (stmt->NextResultRow())
        vecRuns.push_back(stmt->GetInt(0));

    delete stmt;
    delete connDb;

    int run_count = vecRuns.size();
    run_numbers = new UniqueRunNumber[run_count];
    for (int i = 0; i < run_count; i++)
    {
        run_numbers[i].period_number = period_number;
        run_numbers[i].run_number = vecRuns[i];
    }

    return run_count;
}

// get first run number for a selected period
int UniDbRunPeriod::GetFirstRunNumber(int period_number)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr)
    {
        cout<<"ERROR: connection to the database was failed"<<endl;
        return -1;
    }

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select min(run_number) "
        "from run_ "
        "where period_number = %d ", period_number);
    TSQLStatement* stmt = db_server->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"ERROR: getting run numbers from the database failed"<<endl;
        delete stmt;
        delete connDb;
        return -2;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    int min_number = -3; // -3: no runs in the period
    if (stmt->NextResultRow())
        min_number = stmt->GetInt(0);

    delete stmt;
    delete connDb;

    return min_number;
}

// get last run number for a selected period
int UniDbRunPeriod::GetLastRunNumber(int period_number)
{
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr)
    {
        cout<<"ERROR: connection to the database was failed"<<endl;
        return -1;
    }

    TSQLServer* db_server = connDb->GetSQLServer();

    TString sql = TString::Format(
        "select max(run_number) "
        "from run_ "
        "where period_number = %d ", period_number);
    TSQLStatement* stmt = db_server->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"ERROR: getting run numbers from the database failed"<<endl;
        delete stmt;
        delete connDb;
        return -2;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    int max_number = -3; // -3: no runs in the period
    if (stmt->NextResultRow())
        max_number = stmt->GetInt(0);

    delete stmt;
    delete connDb;

    return max_number;
}

// -------------------------------------------------------------------
ClassImp(UniDbRunPeriod);
