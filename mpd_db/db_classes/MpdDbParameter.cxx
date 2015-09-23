// ----------------------------------------------------------------------
//                    MpdDbParameter cxx file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "MpdDbParameter.h"

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
MpdDbParameter::MpdDbParameter(MpdDbConnection* connUniDb, int parameter_id, TString parameter_name, int parameter_type)
{
	connectionUniDb = connUniDb;

	i_parameter_id = parameter_id;
	str_parameter_name = parameter_name;
	i_parameter_type = parameter_type;
}

// -----   Destructor   -------------------------------------------------
MpdDbParameter::~MpdDbParameter()
{
	if (connectionUniDb)
		delete connectionUniDb;
}

// -----   Creating new record in class table ---------------------------
MpdDbParameter* MpdDbParameter::CreateParameter(TString parameter_name, int parameter_type)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into parameter_(parameter_name, parameter_type) "
		"values ($1, $2)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, parameter_name);
	stmt->SetInt(1, parameter_type);

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
	int parameter_id;
	TSQLStatement* stmt_last = uni_db->Statement("SELECT currval(pg_get_serial_sequence('parameter_','parameter_id'))");

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
			parameter_id = stmt_last->GetInt(0);
			delete stmt_last;
		}
	}
	else
	{
		cout<<"Error: getting last ID has been failed!"<<endl;
		delete stmt_last;
		return 0x00;
	}

	int tmp_parameter_id;
	tmp_parameter_id = parameter_id;
	TString tmp_parameter_name;
	tmp_parameter_name = parameter_name;
	int tmp_parameter_type;
	tmp_parameter_type = parameter_type;

	return new MpdDbParameter(connUniDb, tmp_parameter_id, tmp_parameter_name, tmp_parameter_type);
}

// -----   Get table record from database ---------------------------
MpdDbParameter* MpdDbParameter::GetParameter(int parameter_id)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select parameter_id, parameter_name, parameter_type "
		"from parameter_ "
		"where parameter_id = %d", parameter_id);
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

	int tmp_parameter_id;
	tmp_parameter_id = stmt->GetInt(0);
	TString tmp_parameter_name;
	tmp_parameter_name = stmt->GetString(1);
	int tmp_parameter_type;
	tmp_parameter_type = stmt->GetInt(2);

	delete stmt;

	return new MpdDbParameter(connUniDb, tmp_parameter_id, tmp_parameter_name, tmp_parameter_type);
}

// -----   Get table record from database for unique key--------------
MpdDbParameter* MpdDbParameter::GetParameter(TString parameter_name)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select parameter_id, parameter_name, parameter_type "
		"from parameter_ "
		"where lower(parameter_name) = lower('%s')", parameter_name.Data());
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

	int tmp_parameter_id;
	tmp_parameter_id = stmt->GetInt(0);
	TString tmp_parameter_name;
	tmp_parameter_name = stmt->GetString(1);
	int tmp_parameter_type;
	tmp_parameter_type = stmt->GetInt(2);

	delete stmt;

	return new MpdDbParameter(connUniDb, tmp_parameter_id, tmp_parameter_name, tmp_parameter_type);
}

// -----   Delete record from class table ---------------------------
int MpdDbParameter::DeleteParameter(int parameter_id)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from parameter_ "
		"where parameter_id = $1");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, parameter_id);

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

// -----   Delete table record from database for unique key--------------
int MpdDbParameter::DeleteParameter(TString parameter_name)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from parameter_ "
		"where lower(parameter_name) = lower($1)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, parameter_name);

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
int MpdDbParameter::PrintAll()
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select parameter_id, parameter_name, parameter_type "
		"from parameter_");
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
	cout<<"Table 'parameter_'"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<". parameter_id: ";
		cout<<(stmt->GetInt(0));
		cout<<". parameter_name: ";
		cout<<(stmt->GetString(1));
		cout<<". parameter_type: ";
		cout<<(stmt->GetInt(2));
		cout<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int MpdDbParameter::SetParameterName(TString parameter_name)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update parameter_ "
		"set parameter_name = $1 "
		"where parameter_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, parameter_name);
	stmt->SetInt(1, i_parameter_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_parameter_name = parameter_name;

	delete stmt;
	return 0;
}

int MpdDbParameter::SetParameterType(int parameter_type)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update parameter_ "
		"set parameter_type = $1 "
		"where parameter_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, parameter_type);
	stmt->SetInt(1, i_parameter_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_parameter_type = parameter_type;

	delete stmt;
	return 0;
}

// -----   Print current record ---------------------------------------
void MpdDbParameter::Print()
{
	cout<<"Table 'parameter_'";
	cout<<". parameter_id: "<<i_parameter_id<<". parameter_name: "<<str_parameter_name<<". parameter_type: "<<i_parameter_type<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(MpdDbParameter);
