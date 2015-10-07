// ----------------------------------------------------------------------
//                    UniDbSessionDetector cxx file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "UniDbSessionDetector.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniDbSessionDetector::UniDbSessionDetector(UniDbConnection* connUniDb, int session_number, TString detector_name, int* map_id)
{
	connectionUniDb = connUniDb;

	i_session_number = session_number;
	str_detector_name = detector_name;
	i_map_id = map_id;
}

// -----   Destructor   -------------------------------------------------
UniDbSessionDetector::~UniDbSessionDetector()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (i_map_id)
		delete i_map_id;
}

// -----   Creating new record in class table ---------------------------
UniDbSessionDetector* UniDbSessionDetector::CreateSessionDetector(int session_number, TString detector_name, int* map_id)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into session_detector(session_number, detector_name, map_id) "
		"values ($1, $2, $3)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, session_number);
	stmt->SetString(1, detector_name);
	if (map_id == NULL)
		stmt->SetNull(2);
	else
		stmt->SetInt(2, *map_id);

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
	TString tmp_detector_name;
	tmp_detector_name = detector_name;
	int* tmp_map_id;
	if (map_id == NULL) tmp_map_id = NULL;
	else
		tmp_map_id = new int(*map_id);

	return new UniDbSessionDetector(connUniDb, tmp_session_number, tmp_detector_name, tmp_map_id);
}

// -----   Get table record from database ---------------------------
UniDbSessionDetector* UniDbSessionDetector::GetSessionDetector(int session_number, TString detector_name)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select session_number, detector_name, map_id "
		"from session_detector "
		"where session_number = %d and lower(detector_name) = lower('%s')", session_number, detector_name.Data());
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
	TString tmp_detector_name;
	tmp_detector_name = stmt->GetString(1);
	int* tmp_map_id;
	if (stmt->IsNull(2)) tmp_map_id = NULL;
	else
		tmp_map_id = new int(stmt->GetInt(2));

	delete stmt;

	return new UniDbSessionDetector(connUniDb, tmp_session_number, tmp_detector_name, tmp_map_id);
}

// -----   Delete record from class table ---------------------------
int UniDbSessionDetector::DeleteSessionDetector(int session_number, TString detector_name)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from session_detector "
		"where session_number = $1 and lower(detector_name) = lower($2)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, session_number);
	stmt->SetString(1, detector_name);

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
int UniDbSessionDetector::PrintAll()
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select session_number, detector_name, map_id "
		"from session_detector");
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
	cout<<"Table 'session_detector'"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<". session_number: ";
		cout<<(stmt->GetInt(0));
		cout<<". detector_name: ";
		cout<<(stmt->GetString(1));
		cout<<". map_id: ";
		if (stmt->IsNull(2)) cout<<"NULL";
		else
			cout<<stmt->GetInt(2);
		cout<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int UniDbSessionDetector::SetSessionNumber(int session_number)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update session_detector "
		"set session_number = $1 "
		"where session_number = $2 and detector_name = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, session_number);
	stmt->SetInt(1, i_session_number);
	stmt->SetString(2, str_detector_name);

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

int UniDbSessionDetector::SetDetectorName(TString detector_name)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update session_detector "
		"set detector_name = $1 "
		"where session_number = $2 and detector_name = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, detector_name);
	stmt->SetInt(1, i_session_number);
	stmt->SetString(2, str_detector_name);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_detector_name = detector_name;

	delete stmt;
	return 0;
}

int UniDbSessionDetector::SetMapId(int* map_id)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update session_detector "
		"set map_id = $1 "
		"where session_number = $2 and detector_name = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (map_id == NULL)
		stmt->SetNull(0);
	else
		stmt->SetInt(0, *map_id);
	stmt->SetInt(1, i_session_number);
	stmt->SetString(2, str_detector_name);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (i_map_id)
		delete i_map_id;
	if (map_id == NULL) i_map_id = NULL;
	else
		i_map_id = new int(*map_id);

	delete stmt;
	return 0;
}

// -----   Print current record ---------------------------------------
void UniDbSessionDetector::Print()
{
	cout<<"Table 'session_detector'";
	cout<<". session_number: "<<i_session_number<<". detector_name: "<<str_detector_name<<". map_id: "<<(i_map_id == NULL? "NULL": TString::Format("%d", *i_map_id))<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(UniDbSessionDetector);
