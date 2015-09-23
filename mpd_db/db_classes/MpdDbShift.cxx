// ----------------------------------------------------------------------
//                    MpdDbShift cxx file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "MpdDbShift.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
MpdDbShift::MpdDbShift(MpdDbConnection* connUniDb, int shift_id, int session_number, TString fio, TDatime start_datetime, TDatime end_datetime, TString* responsibility)
{
	connectionUniDb = connUniDb;

	i_shift_id = shift_id;
	i_session_number = session_number;
	str_fio = fio;
	dt_start_datetime = start_datetime;
	dt_end_datetime = end_datetime;
	str_responsibility = responsibility;
}

// -----   Destructor   -------------------------------------------------
MpdDbShift::~MpdDbShift()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (str_responsibility)
		delete str_responsibility;
}

// -----   Creating new record in class table ---------------------------
MpdDbShift* MpdDbShift::CreateShift(int session_number, TString fio, TDatime start_datetime, TDatime end_datetime, TString* responsibility)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into shift_(session_number, fio, start_datetime, end_datetime, responsibility) "
		"values ($1, $2, $3, $4, $5)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, session_number);
	stmt->SetString(1, fio);
	stmt->SetDatime(2, start_datetime);
	stmt->SetDatime(3, end_datetime);
	if (responsibility == NULL)
		stmt->SetNull(4);
	else
		stmt->SetString(4, *responsibility);

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
	int shift_id;
	TSQLStatement* stmt_last = uni_db->Statement("SELECT currval(pg_get_serial_sequence('shift_','shift_id'))");

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
			shift_id = stmt_last->GetInt(0);
			delete stmt_last;
		}
	}
	else
	{
		cout<<"Error: getting last ID has been failed!"<<endl;
		delete stmt_last;
		return 0x00;
	}

	int tmp_shift_id;
	tmp_shift_id = shift_id;
	int tmp_session_number;
	tmp_session_number = session_number;
	TString tmp_fio;
	tmp_fio = fio;
	TDatime tmp_start_datetime;
	tmp_start_datetime = start_datetime;
	TDatime tmp_end_datetime;
	tmp_end_datetime = end_datetime;
	TString* tmp_responsibility;
	if (responsibility == NULL) tmp_responsibility = NULL;
	else
		tmp_responsibility = new TString(*responsibility);

	return new MpdDbShift(connUniDb, tmp_shift_id, tmp_session_number, tmp_fio, tmp_start_datetime, tmp_end_datetime, tmp_responsibility);
}

// -----   Get table record from database ---------------------------
MpdDbShift* MpdDbShift::GetShift(int shift_id)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select shift_id, session_number, fio, start_datetime, end_datetime, responsibility "
		"from shift_ "
		"where shift_id = %d", shift_id);
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

	int tmp_shift_id;
	tmp_shift_id = stmt->GetInt(0);
	int tmp_session_number;
	tmp_session_number = stmt->GetInt(1);
	TString tmp_fio;
	tmp_fio = stmt->GetString(2);
	TDatime tmp_start_datetime;
	tmp_start_datetime = stmt->GetDatime(3);
	TDatime tmp_end_datetime;
	tmp_end_datetime = stmt->GetDatime(4);
	TString* tmp_responsibility;
	if (stmt->IsNull(5)) tmp_responsibility = NULL;
	else
		tmp_responsibility = new TString(stmt->GetString(5));

	delete stmt;

	return new MpdDbShift(connUniDb, tmp_shift_id, tmp_session_number, tmp_fio, tmp_start_datetime, tmp_end_datetime, tmp_responsibility);
}

// -----   Delete record from class table ---------------------------
int MpdDbShift::DeleteShift(int shift_id)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from shift_ "
		"where shift_id = $1");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, shift_id);

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
int MpdDbShift::PrintAll()
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select shift_id, session_number, fio, start_datetime, end_datetime, responsibility "
		"from shift_");
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
	cout<<"Table 'shift_'"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<". shift_id: ";
		cout<<(stmt->GetInt(0));
		cout<<". session_number: ";
		cout<<(stmt->GetInt(1));
		cout<<". fio: ";
		cout<<(stmt->GetString(2));
		cout<<". start_datetime: ";
		cout<<(stmt->GetDatime(3)).AsSQLString();
		cout<<". end_datetime: ";
		cout<<(stmt->GetDatime(4)).AsSQLString();
		cout<<". responsibility: ";
		if (stmt->IsNull(5)) cout<<"NULL";
		else
			cout<<stmt->GetString(5);
		cout<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int MpdDbShift::SetSessionNumber(int session_number)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update shift_ "
		"set session_number = $1 "
		"where shift_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, session_number);
	stmt->SetInt(1, i_shift_id);

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

int MpdDbShift::SetFio(TString fio)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update shift_ "
		"set fio = $1 "
		"where shift_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, fio);
	stmt->SetInt(1, i_shift_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_fio = fio;

	delete stmt;
	return 0;
}

int MpdDbShift::SetStartDatetime(TDatime start_datetime)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update shift_ "
		"set start_datetime = $1 "
		"where shift_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetDatime(0, start_datetime);
	stmt->SetInt(1, i_shift_id);

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

int MpdDbShift::SetEndDatetime(TDatime end_datetime)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update shift_ "
		"set end_datetime = $1 "
		"where shift_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetDatime(0, end_datetime);
	stmt->SetInt(1, i_shift_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	dt_end_datetime = end_datetime;

	delete stmt;
	return 0;
}

int MpdDbShift::SetResponsibility(TString* responsibility)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update shift_ "
		"set responsibility = $1 "
		"where shift_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (responsibility == NULL)
		stmt->SetNull(0);
	else
		stmt->SetString(0, *responsibility);
	stmt->SetInt(1, i_shift_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (str_responsibility)
		delete str_responsibility;
	if (responsibility == NULL) str_responsibility = NULL;
	else
		str_responsibility = new TString(*responsibility);

	delete stmt;
	return 0;
}

// -----   Print current record ---------------------------------------
void MpdDbShift::Print()
{
	cout<<"Table 'shift_'";
	cout<<". shift_id: "<<i_shift_id<<". session_number: "<<i_session_number<<". fio: "<<str_fio<<". start_datetime: "<<dt_start_datetime.AsSQLString()<<". end_datetime: "<<dt_end_datetime.AsSQLString()<<". responsibility: "<<(str_responsibility == NULL? "NULL": *str_responsibility)<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(MpdDbShift);
