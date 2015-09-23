// ----------------------------------------------------------------------
//                    MpdDbSession cxx file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "MpdDbSession.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
MpdDbSession::MpdDbSession(MpdDbConnection* connUniDb, int session_number, TDatime start_datetime, TDatime* end_datetime)
{
	connectionUniDb = connUniDb;

	i_session_number = session_number;
	dt_start_datetime = start_datetime;
	dt_end_datetime = end_datetime;
}

// -----   Destructor   -------------------------------------------------
MpdDbSession::~MpdDbSession()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (dt_end_datetime)
		delete dt_end_datetime;
}

// -----   Creating new record in class table ---------------------------
MpdDbSession* MpdDbSession::CreateSession(int session_number, TDatime start_datetime, TDatime* end_datetime)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into session_(session_number, start_datetime, end_datetime) "
		"values ($1, $2, $3)");
	TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
	stmt->SetInt(0, session_number);
	stmt->SetDatime(1, start_datetime);
    if (end_datetime == NULL)
        stmt->SetNull(2);
	else
		stmt->SetDatime(2, *end_datetime);

	// inserting new record to DB
	if (!stmt->Process())
	{
		cout<<"Error: inserting new record to DB has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	int tmp_session_number;
	tmp_session_number = session_number;
	TDatime tmp_start_datetime;
	tmp_start_datetime = start_datetime;
	TDatime* tmp_end_datetime;
	if (end_datetime == NULL) tmp_end_datetime = NULL;
	else
		tmp_end_datetime = new TDatime(*end_datetime);

	return new MpdDbSession(connUniDb, tmp_session_number, tmp_start_datetime, tmp_end_datetime);
}

// -----   Get table record from database ---------------------------
MpdDbSession* MpdDbSession::GetSession(int session_number)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select session_number, start_datetime, end_datetime "
		"from session_ "
		"where session_number = %d", session_number);
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

	int tmp_session_number;
	tmp_session_number = stmt->GetInt(0);
	TDatime tmp_start_datetime;
	tmp_start_datetime = stmt->GetDatime(1);
	TDatime* tmp_end_datetime;
	if (stmt->IsNull(2)) tmp_end_datetime = NULL;
	else
		tmp_end_datetime = new TDatime(stmt->GetDatime(2));

	delete stmt;

	return new MpdDbSession(connUniDb, tmp_session_number, tmp_start_datetime, tmp_end_datetime);
}

// -----   Delete record from class table ---------------------------
int MpdDbSession::DeleteSession(int session_number)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from session_ "
		"where session_number = $1");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, session_number);

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
int MpdDbSession::PrintAll()
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select session_number, start_datetime, end_datetime "
		"from session_");
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
	cout<<"Table 'session_'"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<". session_number: ";
		cout<<(stmt->GetInt(0));
		cout<<". start_datetime: ";
		cout<<(stmt->GetDatime(1)).AsSQLString();
		cout<<". end_datetime: ";
		if (stmt->IsNull(2)) cout<<"NULL";
		else
			cout<<stmt->GetDatime(2).AsSQLString();
		cout<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int MpdDbSession::SetSessionNumber(int session_number)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update session_ "
		"set session_number = $1 "
		"where session_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, session_number);
	stmt->SetInt(1, i_session_number);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_session_number = session_number;

	delete stmt;
	return 0;
}

int MpdDbSession::SetStartDatetime(TDatime start_datetime)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update session_ "
		"set start_datetime = $1 "
		"where session_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetDatime(0, start_datetime);
	stmt->SetInt(1, i_session_number);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	dt_start_datetime = start_datetime;

	delete stmt;
	return 0;
}

int MpdDbSession::SetEndDatetime(TDatime* end_datetime)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update session_ "
		"set end_datetime = $1 "
		"where session_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (end_datetime == NULL)
		stmt->SetNull(0);
	else
		stmt->SetDatime(0, *end_datetime);
	stmt->SetInt(1, i_session_number);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

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

// -----   Print current record ---------------------------------------
void MpdDbSession::Print()
{
	cout<<"Table 'session_'";
	cout<<". session_number: "<<i_session_number<<". start_datetime: "<<dt_start_datetime.AsSQLString()<<". end_datetime: "<<(dt_end_datetime == NULL? "NULL": (*dt_end_datetime).AsSQLString())<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(MpdDbSession);
