// ----------------------------------------------------------------------
//                    UniDbDetector cxx file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "UniDbDetector.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniDbDetector::UniDbDetector(UniConnection* connUniDb, TString detector_name, TString* description)
{
	connectionUniDb = connUniDb;

	str_detector_name = detector_name;
	str_description = description;
}

// -----   Destructor   -------------------------------------------------
UniDbDetector::~UniDbDetector()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (str_description)
		delete str_description;
}

// -----   Creating new detector in the database  ---------------------------
UniDbDetector* UniDbDetector::CreateDetector(TString detector_name, TString* description)
{
	UniConnection* connUniDb = UniConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into detector_(detector_name, description) "
		"values ($1, $2)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, detector_name);
	if (description == NULL)
		stmt->SetNull(1);
	else
		stmt->SetString(1, *description);

	// inserting new detector to the Database
	if (!stmt->Process())
	{
		cout<<"ERROR: inserting new detector to the Database has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	TString tmp_detector_name;
	tmp_detector_name = detector_name;
	TString* tmp_description;
	if (description == NULL) tmp_description = NULL;
	else
		tmp_description = new TString(*description);

	return new UniDbDetector(connUniDb, tmp_detector_name, tmp_description);
}

// -----  Get detector from the database  ---------------------------
UniDbDetector* UniDbDetector::GetDetector(TString detector_name)
{
	UniConnection* connUniDb = UniConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select detector_name, description "
		"from detector_ "
		"where lower(detector_name) = lower('%s')", detector_name.Data());
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get detector from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting detector from the database has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: detector was not found in the database"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	TString tmp_detector_name;
	tmp_detector_name = stmt->GetString(0);
	TString* tmp_description;
	if (stmt->IsNull(1)) tmp_description = NULL;
	else
		tmp_description = new TString(stmt->GetString(1));

	delete stmt;

	return new UniDbDetector(connUniDb, tmp_detector_name, tmp_description);
}

// -----  Check detector exists in the database  ---------------------------
bool UniDbDetector::CheckDetectorExists(TString detector_name)
{
	UniConnection* connUniDb = UniConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from detector_ "
		"where lower(detector_name) = lower('%s')", detector_name.Data());
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get detector from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting detector from the database has been failed"<<endl;

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

// -----  Delete detector from the database  ---------------------------
int UniDbDetector::DeleteDetector(TString detector_name)
{
	UniConnection* connUniDb = UniConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from detector_ "
		"where lower(detector_name) = lower($1)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, detector_name);

	// delete detector from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting detector from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	delete stmt;
	delete connUniDb;
	return 0;
}

// -----  Print all 'detectors'  ---------------------------------
int UniDbDetector::PrintAll()
{
	UniConnection* connUniDb = UniConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select detector_name, description "
		"from detector_");
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get all 'detectors' from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting all 'detectors' from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// print rows
	cout<<"Table 'detector_':"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<"detector_name: ";
		cout<<(stmt->GetString(0));
		cout<<", description: ";
		if (stmt->IsNull(1)) cout<<"NULL";
		else
			cout<<stmt->GetString(1);
		cout<<"."<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int UniDbDetector::SetDetectorName(TString detector_name)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_ "
		"set detector_name = $1 "
		"where detector_name = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, detector_name);
	stmt->SetString(1, str_detector_name);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about detector has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_detector_name = detector_name;

	delete stmt;
	return 0;
}

int UniDbDetector::SetDescription(TString* description)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_ "
		"set description = $1 "
		"where detector_name = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (description == NULL)
		stmt->SetNull(0);
	else
		stmt->SetString(0, *description);
	stmt->SetString(1, str_detector_name);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about detector has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (str_description)
		delete str_description;
	if (description == NULL) str_description = NULL;
	else
		str_description = new TString(*description);

	delete stmt;
	return 0;
}

// -----  Print current detector  ---------------------------------------
void UniDbDetector::Print()
{
	cout<<"Table 'detector_'";
	cout<<". detector_name: "<<str_detector_name<<". description: "<<(str_description == NULL? "NULL": *str_description)<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(UniDbDetector);
