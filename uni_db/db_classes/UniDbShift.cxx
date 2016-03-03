// ----------------------------------------------------------------------
//                    UniDbShift cxx file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "UniDbShift.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniDbShift::UniDbShift(UniDbConnection* connUniDb, int shift_id, int period_number, TDatime start_datetime, TDatime end_datetime, TString fio, TString* responsibility)
{
	connectionUniDb = connUniDb;

	i_shift_id = shift_id;
	i_period_number = period_number;
	dt_start_datetime = start_datetime;
	dt_end_datetime = end_datetime;
	str_fio = fio;
	str_responsibility = responsibility;
}

// -----   Destructor   -------------------------------------------------
UniDbShift::~UniDbShift()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (str_responsibility)
		delete str_responsibility;
}

// -----   Creating new record in class table ---------------------------
UniDbShift* UniDbShift::CreateShift(int period_number, TDatime start_datetime, TDatime end_datetime, TString fio, TString* responsibility)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into shift_(period_number, start_datetime, end_datetime, fio, responsibility) "
		"values ($1, $2, $3, $4, $5)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, period_number);
	stmt->SetDatime(1, start_datetime);
	stmt->SetDatime(2, end_datetime);
	stmt->SetString(3, fio);
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
	int tmp_period_number;
	tmp_period_number = period_number;
	TDatime tmp_start_datetime;
	tmp_start_datetime = start_datetime;
	TDatime tmp_end_datetime;
	tmp_end_datetime = end_datetime;
	TString tmp_fio;
	tmp_fio = fio;
	TString* tmp_responsibility;
	if (responsibility == NULL) tmp_responsibility = NULL;
	else
		tmp_responsibility = new TString(*responsibility);

	return new UniDbShift(connUniDb, tmp_shift_id, tmp_period_number, tmp_start_datetime, tmp_end_datetime, tmp_fio, tmp_responsibility);
}

// -----   Get table record from database ---------------------------
UniDbShift* UniDbShift::GetShift(int shift_id)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select shift_id, period_number, start_datetime, end_datetime, fio, responsibility "
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
	int tmp_period_number;
	tmp_period_number = stmt->GetInt(1);
	TDatime tmp_start_datetime;
	tmp_start_datetime = stmt->GetDatime(2);
	TDatime tmp_end_datetime;
	tmp_end_datetime = stmt->GetDatime(3);
	TString tmp_fio;
	tmp_fio = stmt->GetString(4);
	TString* tmp_responsibility;
	if (stmt->IsNull(5)) tmp_responsibility = NULL;
	else
		tmp_responsibility = new TString(stmt->GetString(5));

	delete stmt;

	return new UniDbShift(connUniDb, tmp_shift_id, tmp_period_number, tmp_start_datetime, tmp_end_datetime, tmp_fio, tmp_responsibility);
}

// -----   Delete record from class table ---------------------------
int UniDbShift::DeleteShift(int shift_id)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
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
int UniDbShift::PrintAll()
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select shift_id, period_number, start_datetime, end_datetime, fio, responsibility "
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
		cout<<". period_number: ";
		cout<<(stmt->GetInt(1));
		cout<<". start_datetime: ";
		cout<<(stmt->GetDatime(2)).AsSQLString();
		cout<<". end_datetime: ";
		cout<<(stmt->GetDatime(3)).AsSQLString();
		cout<<". fio: ";
		cout<<(stmt->GetString(4));
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
int UniDbShift::SetPeriodNumber(int period_number)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update shift_ "
		"set period_number = $1 "
		"where shift_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, period_number);
	stmt->SetInt(1, i_shift_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_period_number = period_number;

	delete stmt;
	return 0;
}

int UniDbShift::SetStartDatetime(TDatime start_datetime)
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

int UniDbShift::SetEndDatetime(TDatime end_datetime)
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

int UniDbShift::SetFio(TString fio)
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

int UniDbShift::SetResponsibility(TString* responsibility)
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
void UniDbShift::Print()
{
	cout<<"Table 'shift_'";
	cout<<". shift_id: "<<i_shift_id<<". period_number: "<<i_period_number<<". start_datetime: "<<dt_start_datetime.AsSQLString()<<". end_datetime: "<<dt_end_datetime.AsSQLString()<<". fio: "<<str_fio<<". responsibility: "<<(str_responsibility == NULL? "NULL": *str_responsibility)<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// -----   Get table record from database ---------------------------
UniDbShift* UniDbShift::GetShift(TDatime shift_datetime)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select shift_id, period_number, start_datetime, end_datetime, fio, responsibility "
		"from shift_ "
		"where '%s' >= start_datetime AND '%s' < end_datetime", shift_datetime.AsSQLString(), shift_datetime.AsSQLString());
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
		cout<<"Error: shift with given time wasn't found"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	int tmp_shift_id;
	tmp_shift_id = stmt->GetInt(0);
	int tmp_period_number;
	tmp_period_number = stmt->GetInt(1);
	TDatime tmp_start_datetime;
	tmp_start_datetime = stmt->GetDatime(2);
	TDatime tmp_end_datetime;
	tmp_end_datetime = stmt->GetDatime(3);
	TString tmp_fio;
	tmp_fio = stmt->GetString(4);
	TString* tmp_responsibility;
	if (stmt->IsNull(5)) tmp_responsibility = NULL;
	else
		tmp_responsibility = new TString(stmt->GetString(5));

	delete stmt;

	return new UniDbShift(connUniDb, tmp_shift_id, tmp_period_number, tmp_start_datetime, tmp_end_datetime, tmp_fio, tmp_responsibility);
}

// -------------------------------------------------------------------
ClassImp(UniDbShift);
