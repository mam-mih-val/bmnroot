// ----------------------------------------------------------------------
//                    ElogDbBeam cxx file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "ElogDbBeam.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
ElogDbBeam::ElogDbBeam(ElogConnection* connect_db, TString beam)
{
    connectionDb = connect_db;

	str_beam = beam;
}

// -----   Destructor   -------------------------------------------------
ElogDbBeam::~ElogDbBeam()
{
	if (connectionDb)
		delete connectionDb;
}

// -----   Creating new beam in the database  ---------------------------
ElogDbBeam* ElogDbBeam::CreateBeam(TString beam)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into beam_(beam) "
		"values ($1)");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, beam);

	// inserting new beam to the Database
	if (!stmt->Process())
	{
		cout<<"ERROR: inserting new beam to the Database has been failed"<<endl;
		delete stmt;
        delete connDb;
        return nullptr;
	}

	delete stmt;

	TString tmp_beam;
	tmp_beam = beam;

    return new ElogDbBeam(connDb, tmp_beam);
}

// -----  Get beam from the database  ---------------------------
ElogDbBeam* ElogDbBeam::GetBeam(TString beam)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return nullptr;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select beam "
		"from beam_ "
		"where lower(beam) = lower('%s')", beam.Data());
    TSQLStatement* stmt = db_server->Statement(sql);

	// get beam from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting beam from the database has been failed"<<endl;

		delete stmt;
        delete connDb;
        return nullptr;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: beam was not found in the database"<<endl;

		delete stmt;
        delete connDb;
        return nullptr;
	}

	TString tmp_beam;
	tmp_beam = stmt->GetString(0);

	delete stmt;

    return new ElogDbBeam(connDb, tmp_beam);
}

// -----  Check beam exists in the database  ---------------------------
int ElogDbBeam::CheckBeamExists(TString beam)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from beam_ "
		"where lower(beam) = lower('%s')", beam.Data());
    TSQLStatement* stmt = db_server->Statement(sql);

	// get beam from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting beam from the database has been failed"<<endl;

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

// -----  Delete beam from the database  ---------------------------
int ElogDbBeam::DeleteBeam(TString beam)
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from beam_ "
		"where lower(beam) = lower($1)");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, beam);

	// delete beam from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting beam from the dataBase has been failed"<<endl;

		delete stmt;
        delete connDb;
        return -2;
	}

	delete stmt;
    delete connDb;
	return 0;
}

// -----  Print all 'beams'  ---------------------------------
int ElogDbBeam::PrintAll()
{
    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;

    TSQLServer* db_server = connDb->GetSQLServer();

	TString sql = TString::Format(
		"select beam "
		"from beam_");
    TSQLStatement* stmt = db_server->Statement(sql);

	// get all 'beams' from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting all 'beams' from the dataBase has been failed"<<endl;

		delete stmt;
        delete connDb;
        return -2;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// print rows
	cout<<"Table 'beam_':"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<"beam: ";
		cout<<(stmt->GetString(0));
		cout<<"."<<endl;
	}

	delete stmt;
    delete connDb;

	return 0;
}


// Setters functions
int ElogDbBeam::SetBeam(TString beam)
{
	if (!connectionDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

    TSQLServer* db_server = connectionDb->GetSQLServer();

	TString sql = TString::Format(
		"update beam_ "
		"set beam = $1 "
		"where beam = $2");
    TSQLStatement* stmt = db_server->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, beam);
	stmt->SetString(1, str_beam);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about beam has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_beam = beam;

	delete stmt;
	return 0;
}

// -----  Print current beam  ---------------------------------------
void ElogDbBeam::Print()
{
	cout<<"Table 'beam_'";
	cout<<". beam: "<<str_beam<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(ElogDbBeam);
