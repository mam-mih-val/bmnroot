// ----------------------------------------------------------------------
//                    ElogDbAttachment cxx file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "ElogDbAttachment.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
ElogDbAttachment::ElogDbAttachment(UniConnection* connUniDb, int record_id, int attachment_number, TString file_path)
{
	connectionUniDb = connUniDb;

	i_record_id = record_id;
	i_attachment_number = attachment_number;
	str_file_path = file_path;
}

// -----   Destructor   -------------------------------------------------
ElogDbAttachment::~ElogDbAttachment()
{
	if (connectionUniDb)
		delete connectionUniDb;
}

// -----   Creating new attachment in the database  ---------------------------
ElogDbAttachment* ElogDbAttachment::CreateAttachment(int record_id, int attachment_number, TString file_path)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into attachment_(record_id, attachment_number, file_path) "
		"values ($1, $2, $3)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, record_id);
	stmt->SetInt(1, attachment_number);
	stmt->SetString(2, file_path);

	// inserting new attachment to the Database
	if (!stmt->Process())
	{
		cout<<"ERROR: inserting new attachment to the Database has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	int tmp_record_id;
	tmp_record_id = record_id;
	int tmp_attachment_number;
	tmp_attachment_number = attachment_number;
	TString tmp_file_path;
	tmp_file_path = file_path;

	return new ElogDbAttachment(connUniDb, tmp_record_id, tmp_attachment_number, tmp_file_path);
}

// -----  Get attachment from the database  ---------------------------
ElogDbAttachment* ElogDbAttachment::GetAttachment(int record_id, int attachment_number)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select record_id, attachment_number, file_path "
		"from attachment_ "
		"where record_id = %d and attachment_number = %d", record_id, attachment_number);
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get attachment from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting attachment from the database has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: attachment was not found in the database"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	int tmp_record_id;
	tmp_record_id = stmt->GetInt(0);
	int tmp_attachment_number;
	tmp_attachment_number = stmt->GetInt(1);
	TString tmp_file_path;
	tmp_file_path = stmt->GetString(2);

	delete stmt;

	return new ElogDbAttachment(connUniDb, tmp_record_id, tmp_attachment_number, tmp_file_path);
}

// -----  Get attachment from the database by unique key  --------------
ElogDbAttachment* ElogDbAttachment::GetAttachment(TString file_path)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select record_id, attachment_number, file_path "
		"from attachment_ "
		"where lower(file_path) = lower('%s')", file_path.Data());
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get attachment from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting attachment from the database has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: attachment was not found in the database"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	int tmp_record_id;
	tmp_record_id = stmt->GetInt(0);
	int tmp_attachment_number;
	tmp_attachment_number = stmt->GetInt(1);
	TString tmp_file_path;
	tmp_file_path = stmt->GetString(2);

	delete stmt;

	return new ElogDbAttachment(connUniDb, tmp_record_id, tmp_attachment_number, tmp_file_path);
}

// -----  Check attachment exists in the database  ---------------------------
bool ElogDbAttachment::CheckAttachmentExists(int record_id, int attachment_number)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from attachment_ "
		"where record_id = %d and attachment_number = %d", record_id, attachment_number);
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get attachment from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting attachment from the database has been failed"<<endl;

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

// -----  Check attachment exists in the database by unique key  --------------
bool ElogDbAttachment::CheckAttachmentExists(TString file_path)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from attachment_ "
		"where lower(file_path) = lower('%s')", file_path.Data());
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get attachment from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting attachment from the database has been failed"<<endl;

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

// -----  Delete attachment from the database  ---------------------------
int ElogDbAttachment::DeleteAttachment(int record_id, int attachment_number)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from attachment_ "
		"where record_id = $1 and attachment_number = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, record_id);
	stmt->SetInt(1, attachment_number);

	// delete attachment from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting attachment from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	delete stmt;
	delete connUniDb;
	return 0;
}

// -----  Delete attachment from the database by unique key  --------------
int ElogDbAttachment::DeleteAttachment(TString file_path)
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from attachment_ "
		"where lower(file_path) = lower($1)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, file_path);

	// delete attachment from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting attachment from the DataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	delete stmt;
	delete connUniDb;
	return 0;
}

// -----  Print all 'attachments'  ---------------------------------
int ElogDbAttachment::PrintAll()
{
        UniConnection* connUniDb = UniConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select record_id, attachment_number, file_path "
		"from attachment_");
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get all 'attachments' from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting all 'attachments' from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// print rows
	cout<<"Table 'attachment_':"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<"record_id: ";
		cout<<(stmt->GetInt(0));
		cout<<", attachment_number: ";
		cout<<(stmt->GetInt(1));
		cout<<", file_path: ";
		cout<<(stmt->GetString(2));
		cout<<"."<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int ElogDbAttachment::SetRecordId(int record_id)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update attachment_ "
		"set record_id = $1 "
		"where record_id = $2 and attachment_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, record_id);
	stmt->SetInt(1, i_record_id);
	stmt->SetInt(2, i_attachment_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about attachment has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_record_id = record_id;

	delete stmt;
	return 0;
}

int ElogDbAttachment::SetAttachmentNumber(int attachment_number)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update attachment_ "
		"set attachment_number = $1 "
		"where record_id = $2 and attachment_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, attachment_number);
	stmt->SetInt(1, i_record_id);
	stmt->SetInt(2, i_attachment_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about attachment has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_attachment_number = attachment_number;

	delete stmt;
	return 0;
}

int ElogDbAttachment::SetFilePath(TString file_path)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update attachment_ "
		"set file_path = $1 "
		"where record_id = $2 and attachment_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, file_path);
	stmt->SetInt(1, i_record_id);
	stmt->SetInt(2, i_attachment_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about attachment has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_file_path = file_path;

	delete stmt;
	return 0;
}

// -----  Print current attachment  ---------------------------------------
void ElogDbAttachment::Print()
{
	cout<<"Table 'attachment_'";
	cout<<". record_id: "<<i_record_id<<". attachment_number: "<<i_attachment_number<<". file_path: "<<str_file_path<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(ElogDbAttachment);
