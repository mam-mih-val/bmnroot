// ----------------------------------------------------------------------
//                    MpdDbDetector cxx file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "MpdDbDetector.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
MpdDbDetector::MpdDbDetector(MpdDbConnection* connUniDb, TString detector_name, TString* manufacturer_name, TString* responsible_person, TString* description)
{
	connectionUniDb = connUniDb;

	str_detector_name = detector_name;
	str_manufacturer_name = manufacturer_name;
	str_responsible_person = responsible_person;
	str_description = description;
}

// -----   Destructor   -------------------------------------------------
MpdDbDetector::~MpdDbDetector()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (str_manufacturer_name)
		delete str_manufacturer_name;
	if (str_responsible_person)
		delete str_responsible_person;
	if (str_description)
		delete str_description;
}

// -----   Creating new record in class table ---------------------------
MpdDbDetector* MpdDbDetector::CreateDetector(TString detector_name, TString* manufacturer_name, TString* responsible_person, TString* description)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into detector_(detector_name, manufacturer_name, responsible_person, description) "
		"values ($1, $2, $3, $4)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, detector_name);
	if (manufacturer_name == NULL)
		stmt->SetNull(1);
	else
		stmt->SetString(1, *manufacturer_name);
	if (responsible_person == NULL)
		stmt->SetNull(2);
	else
		stmt->SetString(2, *responsible_person);
	if (description == NULL)
		stmt->SetNull(3);
	else
		stmt->SetString(3, *description);

	// inserting new record to DB
	if (!stmt->Process())
	{
		cout<<"Error: inserting new record to DB has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	TString tmp_detector_name;
	tmp_detector_name = detector_name;
	TString* tmp_manufacturer_name;
	if (manufacturer_name == NULL) tmp_manufacturer_name = NULL;
	else
		tmp_manufacturer_name = new TString(*manufacturer_name);
	TString* tmp_responsible_person;
	if (responsible_person == NULL) tmp_responsible_person = NULL;
	else
		tmp_responsible_person = new TString(*responsible_person);
	TString* tmp_description;
	if (description == NULL) tmp_description = NULL;
	else
		tmp_description = new TString(*description);

	return new MpdDbDetector(connUniDb, tmp_detector_name, tmp_manufacturer_name, tmp_responsible_person, tmp_description);
}

// -----   Get table record from database ---------------------------
MpdDbDetector* MpdDbDetector::GetDetector(TString detector_name)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select detector_name, manufacturer_name, responsible_person, description "
		"from detector_ "
		"where lower(detector_name) = lower('%s')", detector_name.Data());
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

	TString tmp_detector_name;
	tmp_detector_name = stmt->GetString(0);
	TString* tmp_manufacturer_name;
	if (stmt->IsNull(1)) tmp_manufacturer_name = NULL;
	else
		tmp_manufacturer_name = new TString(stmt->GetString(1));
	TString* tmp_responsible_person;
	if (stmt->IsNull(2)) tmp_responsible_person = NULL;
	else
		tmp_responsible_person = new TString(stmt->GetString(2));
	TString* tmp_description;
	if (stmt->IsNull(3)) tmp_description = NULL;
	else
		tmp_description = new TString(stmt->GetString(3));

	delete stmt;

	return new MpdDbDetector(connUniDb, tmp_detector_name, tmp_manufacturer_name, tmp_responsible_person, tmp_description);
}

// -----   Delete record from class table ---------------------------
int MpdDbDetector::DeleteDetector(TString detector_name)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from detector_ "
		"where lower(detector_name) = lower($1)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, detector_name);

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
int MpdDbDetector::PrintAll()
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select detector_name, manufacturer_name, responsible_person, description "
		"from detector_");
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
	cout<<"Table 'detector_'"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<". detector_name: ";
		cout<<(stmt->GetString(0));
		cout<<". manufacturer_name: ";
		if (stmt->IsNull(1)) cout<<"NULL";
		else
			cout<<stmt->GetString(1);
		cout<<". responsible_person: ";
		if (stmt->IsNull(2)) cout<<"NULL";
		else
			cout<<stmt->GetString(2);
		cout<<". description: ";
		if (stmt->IsNull(3)) cout<<"NULL";
		else
			cout<<stmt->GetString(3);
		cout<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int MpdDbDetector::SetDetectorName(TString detector_name)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
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

int MpdDbDetector::SetManufacturerName(TString* manufacturer_name)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_ "
		"set manufacturer_name = $1 "
		"where detector_name = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (manufacturer_name == NULL)
		stmt->SetNull(0);
	else
		stmt->SetString(0, *manufacturer_name);
	stmt->SetString(1, str_detector_name);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (str_manufacturer_name)
		delete str_manufacturer_name;
	if (manufacturer_name == NULL) str_manufacturer_name = NULL;
	else
		str_manufacturer_name = new TString(*manufacturer_name);

	delete stmt;
	return 0;
}

int MpdDbDetector::SetResponsiblePerson(TString* responsible_person)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_ "
		"set responsible_person = $1 "
		"where detector_name = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (responsible_person == NULL)
		stmt->SetNull(0);
	else
		stmt->SetString(0, *responsible_person);
	stmt->SetString(1, str_detector_name);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (str_responsible_person)
		delete str_responsible_person;
	if (responsible_person == NULL) str_responsible_person = NULL;
	else
		str_responsible_person = new TString(*responsible_person);

	delete stmt;
	return 0;
}

int MpdDbDetector::SetDescription(TString* description)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
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

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

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

// -----   Print current record ---------------------------------------
void MpdDbDetector::Print()
{
	cout<<"Table 'detector_'";
	cout<<". detector_name: "<<str_detector_name<<". manufacturer_name: "<<(str_manufacturer_name == NULL? "NULL": *str_manufacturer_name)<<". responsible_person: "<<(str_responsible_person == NULL? "NULL": *str_responsible_person)<<". description: "<<(str_description == NULL? "NULL": *str_description)<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(MpdDbDetector);
