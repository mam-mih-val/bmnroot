// ----------------------------------------------------------------------
//                    ElogDbAttachment cxx file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "ElogDbAttachment.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
ElogDbAttachment::ElogDbAttachment(UniDbConnection* connUniDb, int record_id, int attachment_number, TString file_name, unsigned char* file_data, Long_t size_file_data)
{
	connectionUniDb = connUniDb;

	i_record_id = record_id;
	i_attachment_number = attachment_number;
	str_file_name = file_name;
	blob_file_data = file_data;
	sz_file_data = size_file_data;
}

// -----   Destructor   -------------------------------------------------
ElogDbAttachment::~ElogDbAttachment()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (blob_file_data)
		delete [] blob_file_data;
}

// -----   Creating new attachment in the database  ---------------------------
ElogDbAttachment* ElogDbAttachment::CreateAttachment(int record_id, int attachment_number, TString file_name, unsigned char* file_data, Long_t size_file_data)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into attachment_(record_id, attachment_number, file_name, file_data) "
		"values ($1, $2, $3, $4)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, record_id);
	stmt->SetInt(1, attachment_number);
	stmt->SetString(2, file_name);
	stmt->SetLargeObject(3, file_data, size_file_data, 0x4000000);

	// inserting new attachment to the Database
	if (!stmt->Process())
	{
		cout<<"Error: inserting new attachment to the Database has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	int tmp_record_id;
	tmp_record_id = record_id;
	int tmp_attachment_number;
	tmp_attachment_number = attachment_number;
	TString tmp_file_name;
	tmp_file_name = file_name;
	unsigned char* tmp_file_data;
	Long_t tmp_sz_file_data = size_file_data;
	tmp_file_data = new unsigned char[tmp_sz_file_data];
	memcpy(tmp_file_data, file_data, tmp_sz_file_data);

	return new ElogDbAttachment(connUniDb, tmp_record_id, tmp_attachment_number, tmp_file_name, tmp_file_data, tmp_sz_file_data);
}

// -----  Get attachment from the database  ---------------------------
ElogDbAttachment* ElogDbAttachment::GetAttachment(int record_id, int attachment_number)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select record_id, attachment_number, file_name, file_data "
		"from attachment_ "
		"where record_id = %d and attachment_number = %d", record_id, attachment_number);
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get attachment from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting attachment from the database has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"Error: attachment wasn't found in the database"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	int tmp_record_id;
	tmp_record_id = stmt->GetInt(0);
	int tmp_attachment_number;
	tmp_attachment_number = stmt->GetInt(1);
	TString tmp_file_name;
	tmp_file_name = stmt->GetString(2);
	unsigned char* tmp_file_data;
	tmp_file_data = NULL;
	Long_t tmp_sz_file_data = 0;
	stmt->GetLargeObject(3, (void*&)tmp_file_data, tmp_sz_file_data);

	delete stmt;

	return new ElogDbAttachment(connUniDb, tmp_record_id, tmp_attachment_number, tmp_file_name, tmp_file_data, tmp_sz_file_data);
}

// -----  Check attachment exists in the database  ---------------------------
bool ElogDbAttachment::CheckAttachmentExists(int record_id, int attachment_number)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(ELOG_DB);
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
		cout<<"Error: getting attachment from the database has been failed"<<endl;

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
	UniDbConnection* connUniDb = UniDbConnection::Open(ELOG_DB);
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
		cout<<"Error: deleting attachment from the dataBase has been failed"<<endl;

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
	UniDbConnection* connUniDb = UniDbConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select record_id, attachment_number, file_name, file_data "
		"from attachment_");
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get all 'attachments' from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting all 'attachments' from the dataBase has been failed"<<endl;

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
		cout<<", file_name: ";
		cout<<(stmt->GetString(2));
		cout<<", file_data: ";
		unsigned char* tmp_file_data = NULL;
		Long_t tmp_sz_file_data=0;
		stmt->GetLargeObject(3, (void*&)tmp_file_data, tmp_sz_file_data);
		cout<<(void*)tmp_file_data<<", binary size: "<<tmp_sz_file_data;
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
		cout<<"Error: updating information about attachment has been failed"<<endl;

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
		cout<<"Error: updating information about attachment has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_attachment_number = attachment_number;

	delete stmt;
	return 0;
}

int ElogDbAttachment::SetFileName(TString file_name)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update attachment_ "
		"set file_name = $1 "
		"where record_id = $2 and attachment_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, file_name);
	stmt->SetInt(1, i_record_id);
	stmt->SetInt(2, i_attachment_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about attachment has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_file_name = file_name;

	delete stmt;
	return 0;
}

int ElogDbAttachment::SetFileData(unsigned char* file_data, Long_t size_file_data)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update attachment_ "
		"set file_data = $1 "
		"where record_id = $2 and attachment_number = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetLargeObject(0, file_data, size_file_data, 0x4000000);
	stmt->SetInt(1, i_record_id);
	stmt->SetInt(2, i_attachment_number);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about attachment has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (blob_file_data)
		delete [] blob_file_data;
	sz_file_data = size_file_data;
	blob_file_data = new unsigned char[sz_file_data];
	memcpy(blob_file_data, file_data, sz_file_data);

	delete stmt;
	return 0;
}

// -----  Print current attachment  ---------------------------------------
void ElogDbAttachment::Print()
{
	cout<<"Table 'attachment_'";
	cout<<". record_id: "<<i_record_id<<". attachment_number: "<<i_attachment_number<<". file_name: "<<str_file_name<<". file_data: "<<(void*)blob_file_data<<", binary size: "<<sz_file_data<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(ElogDbAttachment);
