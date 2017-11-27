// ----------------------------------------------------------------------
//                    ElogDbBeam cxx file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "ElogDbBeam.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
ElogDbBeam::ElogDbBeam(UniDbConnection* connUniDb, TString beam)
{
	connectionUniDb = connUniDb;

	str_beam = beam;
}

// -----   Destructor   -------------------------------------------------
ElogDbBeam::~ElogDbBeam()
{
	if (connectionUniDb)
		delete connectionUniDb;
}

// -----   Creating new beam in the database  ---------------------------
ElogDbBeam* ElogDbBeam::CreateBeam(TString beam)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into beam_(beam) "
		"values ($1)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, beam);

	// inserting new beam to the Database
	if (!stmt->Process())
	{
		cout<<"Error: inserting new beam to the Database has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	TString tmp_beam;
	tmp_beam = beam;

	return new ElogDbBeam(connUniDb, tmp_beam);
}

// -----  Get beam from the database  ---------------------------
ElogDbBeam* ElogDbBeam::GetBeam(TString beam)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select beam "
		"from beam_ "
		"where lower(beam) = lower('%s')", beam.Data());
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get beam from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting beam from the database has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"Error: beam wasn't found in the database"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	TString tmp_beam;
	tmp_beam = stmt->GetString(0);

	delete stmt;

	return new ElogDbBeam(connUniDb, tmp_beam);
}

// -----  Check beam exists in the database  ---------------------------
bool ElogDbBeam::CheckBeamExists(TString beam)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from beam_ "
		"where lower(beam) = lower('%s')", beam.Data());
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get beam from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting beam from the database has been failed"<<endl;

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

// -----  Delete beam from the database  ---------------------------
int ElogDbBeam::DeleteBeam(TString beam)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from beam_ "
		"where lower(beam) = lower($1)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, beam);

	// delete beam from the dataBase
	if (!stmt->Process())
	{
		cout<<"Error: deleting beam from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	delete stmt;
	delete connUniDb;
	return 0;
}

// -----  Print all 'beams'  ---------------------------------
int ElogDbBeam::PrintAll()
{
	UniDbConnection* connUniDb = UniDbConnection::Open(ELOG_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select beam "
		"from beam_");
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get all 'beams' from the database
	if (!stmt->Process())
	{
		cout<<"Error: getting all 'beams' from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
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
	delete connUniDb;

	return 0;
}


// Setters functions
int ElogDbBeam::SetBeam(TString beam)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update beam_ "
		"set beam = $1 "
		"where beam = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, beam);
	stmt->SetString(1, str_beam);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"Error: updating information about beam has been failed"<<endl;

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
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// -------------------------------------------------------------------
ClassImp(ElogDbBeam);
