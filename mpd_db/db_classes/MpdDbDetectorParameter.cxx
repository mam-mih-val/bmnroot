// ----------------------------------------------------------------------
//                    MpdDbDetectorParameter cxx file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "MpdDbDetectorParameter.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
MpdDbDetectorParameter::MpdDbDetectorParameter(MpdDbConnection* connUniDb, int run_number, TString detector_name, int parameter_id, unsigned char* parameter_value, Long_t size_parameter_value)
{
	connectionUniDb = connUniDb;

	i_run_number = run_number;
	str_detector_name = detector_name;
	i_parameter_id = parameter_id;
	blob_parameter_value = parameter_value;
	sz_parameter_value = size_parameter_value;
}

// -----   Destructor   -------------------------------------------------
MpdDbDetectorParameter::~MpdDbDetectorParameter()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (blob_parameter_value)
		delete [] blob_parameter_value;
}

// -----   Creating new record in class table ---------------------------
MpdDbDetectorParameter* MpdDbDetectorParameter::CreateDetectorParameter(int run_number, TString detector_name, int parameter_id, unsigned char* parameter_value, Long_t size_parameter_value)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into detector_parameter(run_number, detector_name, parameter_id, parameter_value) "
		"values ($1, $2, $3, $4)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, run_number);
	stmt->SetString(1, detector_name);
	stmt->SetInt(2, parameter_id);
	stmt->SetLargeObject(3, parameter_value, size_parameter_value, 0x4000000);

	// inserting new record to DB
	if (!stmt->Process())
	{
		cout<<"Error: inserting new record to DB has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	int tmp_run_number;
	tmp_run_number = run_number;
	TString tmp_detector_name;
	tmp_detector_name = detector_name;
	int tmp_parameter_id;
	tmp_parameter_id = parameter_id;
	unsigned char* tmp_parameter_value;
	Long_t tmp_sz_parameter_value = size_parameter_value;
	tmp_parameter_value = new unsigned char[tmp_sz_parameter_value];
	memcpy(tmp_parameter_value, parameter_value, tmp_sz_parameter_value);

	return new MpdDbDetectorParameter(connUniDb, tmp_run_number, tmp_detector_name, tmp_parameter_id, tmp_parameter_value, tmp_sz_parameter_value);
}

// -----   Get table record from database ---------------------------
MpdDbDetectorParameter* MpdDbDetectorParameter::GetDetectorParameter(int run_number, TString detector_name, int parameter_id)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select run_number, detector_name, parameter_id, parameter_value "
		"from detector_parameter "
		"where run_number = %d and lower(detector_name) = lower('%s') and parameter_id = %d", run_number, detector_name.Data(), parameter_id);
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

	int tmp_run_number;
	tmp_run_number = stmt->GetInt(0);
	TString tmp_detector_name;
	tmp_detector_name = stmt->GetString(1);
	int tmp_parameter_id;
	tmp_parameter_id = stmt->GetInt(2);
	unsigned char* tmp_parameter_value;
	tmp_parameter_value = NULL;
	Long_t tmp_sz_parameter_value = 0;
	stmt->GetLargeObject(3, (void*&)tmp_parameter_value, tmp_sz_parameter_value);

	delete stmt;

	return new MpdDbDetectorParameter(connUniDb, tmp_run_number, tmp_detector_name, tmp_parameter_id, tmp_parameter_value, tmp_sz_parameter_value);
}

// -----   Delete record from class table ---------------------------
int MpdDbDetectorParameter::DeleteDetectorParameter(int run_number, TString detector_name, int parameter_id)
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from detector_parameter "
		"where run_number = $1 and lower(detector_name) = lower($2) and parameter_id = $3");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, run_number);
	stmt->SetString(1, detector_name);
	stmt->SetInt(2, parameter_id);

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
int MpdDbDetectorParameter::PrintAll()
{
	MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select run_number, detector_name, parameter_id, parameter_value "
		"from detector_parameter");
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
	cout<<"Table 'detector_parameter'"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<". run_number: ";
		cout<<(stmt->GetInt(0));
		cout<<". detector_name: ";
		cout<<(stmt->GetString(1));
		cout<<". parameter_id: ";
		cout<<(stmt->GetInt(2));
		cout<<". parameter_value: ";
		unsigned char* tmp_parameter_value = NULL;
		Long_t tmp_sz_parameter_value=0;
		stmt->GetLargeObject(3, (void*&)tmp_parameter_value, tmp_sz_parameter_value);
		cout<<(void*)tmp_parameter_value<<", binary size: "<<tmp_sz_parameter_value;
		cout<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int MpdDbDetectorParameter::SetRunNumber(int run_number)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_parameter "
		"set run_number = $1 "
		"where run_number = $2 and detector_name = $3 and parameter_id = $4");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, run_number);
	stmt->SetInt(1, i_run_number);
	stmt->SetString(2, str_detector_name);
	stmt->SetInt(3, i_parameter_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_run_number = run_number;

	delete stmt;
	return 0;
}

int MpdDbDetectorParameter::SetDetectorName(TString detector_name)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_parameter "
		"set detector_name = $1 "
		"where run_number = $2 and detector_name = $3 and parameter_id = $4");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, detector_name);
	stmt->SetInt(1, i_run_number);
	stmt->SetString(2, str_detector_name);
	stmt->SetInt(3, i_parameter_id);

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

int MpdDbDetectorParameter::SetParameterId(int parameter_id)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_parameter "
		"set parameter_id = $1 "
		"where run_number = $2 and detector_name = $3 and parameter_id = $4");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, parameter_id);
	stmt->SetInt(1, i_run_number);
	stmt->SetString(2, str_detector_name);
	stmt->SetInt(3, i_parameter_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_parameter_id = parameter_id;

	delete stmt;
	return 0;
}

int MpdDbDetectorParameter::SetParameterValue(unsigned char* parameter_value, Long_t size_parameter_value)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_parameter "
		"set parameter_value = $1 "
		"where run_number = $2 and detector_name = $3 and parameter_id = $4");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetLargeObject(0, parameter_value, size_parameter_value, 0x4000000);
	stmt->SetInt(1, i_run_number);
	stmt->SetString(2, str_detector_name);
	stmt->SetInt(3, i_parameter_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (blob_parameter_value)
		delete [] blob_parameter_value;
	sz_parameter_value = size_parameter_value;
	blob_parameter_value = new unsigned char[sz_parameter_value];
	memcpy(blob_parameter_value, parameter_value, sz_parameter_value);

	delete stmt;
	return 0;
}

// -----   Print current record ---------------------------------------
void MpdDbDetectorParameter::Print()
{
	cout<<"Table 'detector_parameter'";
	cout<<". run_number: "<<i_run_number<<". detector_name: "<<str_detector_name<<". parameter_id: "<<i_parameter_id<<". parameter_value: "<<(void*)blob_parameter_value<<", binary size: "<<sz_parameter_value<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// get detector parameter by run number, detector name and parameter name
MpdDbDetectorParameter* MpdDbDetectorParameter::GetDetectorParameter(int run_number, TString detector_name, TString parameter_name)
{
    MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00) return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
        "select run_number, detector_name, p.parameter_id, parameter_value "
        "from detector_parameter dp join parameter_ p on dp.parameter_id = p.parameter_id "
        "where run_number = %d and lower(detector_name) = lower('%s') and lower(parameter_name) = lower('%s')", run_number, detector_name.Data(), parameter_name.Data());
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

    int tmp_run_number;
    tmp_run_number = stmt->GetInt(0);
    TString tmp_detector_name;
    tmp_detector_name = stmt->GetString(1);
    int tmp_parameter_id;
    tmp_parameter_id = stmt->GetInt(2);
    unsigned char* tmp_parameter_value = NULL;
    Long_t tmp_sz_parameter_value = 0;
    stmt->GetLargeObject(3, (void*&)tmp_parameter_value, tmp_sz_parameter_value);

    delete stmt;

    return new MpdDbDetectorParameter(connUniDb, tmp_run_number, tmp_detector_name, tmp_parameter_id, tmp_parameter_value, tmp_sz_parameter_value);
}

// create boolean detector parameter
MpdDbDetectorParameter* MpdDbDetectorParameter::CreateDetectorParameter(int run_number, TString detector_name, TString parameter_name, bool parameter_value)
{
    MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00) return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    // get parameter object from 'parameter_' table
    TString sql = TString::Format(
        "select parameter_id, parameter_name, parameter_type "
        "from parameter_ "
        "where lower(parameter_name) = lower('%s')", parameter_name.Data());
    TSQLStatement* stmt = uni_db->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"Error: getting record with parameter from 'parameter_' table has been failed"<<endl;

        delete stmt;
        delete connUniDb;
        return 0x00;
    }

    stmt->StoreResult();

    // extract row with parameter
    if (!stmt->NextResultRow())
    {
        cout<<"Error: the parameter with name '"<<parameter_name<<"' wasn't found"<<endl;

        delete stmt;
        delete connUniDb;
        return 0x00;
    }

    int parameter_id = stmt->GetInt(0);
    int parameter_type = stmt->GetInt(2);

    delete stmt;

    // if parameter - not boolean type
    if (parameter_type != BoolType)
    {
        cout<<"Error: the parameter with name '"<<parameter_name<<"' isn't boolean type"<<endl;

        delete connUniDb;
        return 0x00;
    }

    bool* p_parameter_value = new bool[1];
    p_parameter_value[0] = parameter_value;
    Long_t size_parameter_value = sizeof(bool);

    sql = TString::Format(
        "insert into detector_parameter(run_number, detector_name, parameter_id, parameter_value) "
        "values ($1, $2, $3, $4)");
    stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, run_number);
    stmt->SetString(1, detector_name);
    stmt->SetInt(2, parameter_id);
    stmt->SetLargeObject(3, p_parameter_value, size_parameter_value);
    //cout<<p_parameter_value<<" "<<p_parameter_value[0]<<" "<<size_parameter_value<<endl;

    // inserting new record to DB
    if (!stmt->Process())
    {
        cout<<"Error: inserting new parameter value to DB has been failed"<<endl;
        delete stmt;
        delete connUniDb;
        delete [] p_parameter_value;
        return 0x00;
    }

    delete stmt;

    return new MpdDbDetectorParameter(connUniDb, run_number, detector_name, parameter_id, (unsigned char*)p_parameter_value, size_parameter_value);
}

bool MpdDbDetectorParameter::CreateDetectorParameters(int start_run_number, int end_run_number, TString detector_name, TString parameter_name, bool parameter_value)
{
    if (end_run_number < start_run_number)
    {
        cout<<"Error: end run number should be greater than start number"<<endl;
        return false;
    }

    bool is_errors = false;
    for (int i = start_run_number; i <= end_run_number; i++)
    {
        MpdDbDetectorParameter* pDetectorParameter = MpdDbDetectorParameter::CreateDetectorParameter(i, detector_name, parameter_name, parameter_value);

        if (pDetectorParameter)
            delete pDetectorParameter;
        else
            is_errors = true;
    }

    return (!is_errors);
}

// get boolean value of detector parameter (for current run, detector and parameter)
bool MpdDbDetectorParameter::GetBool()
{
    if (!connectionUniDb)
    {
        cout<<"Critical Error: Connection object is null"<<endl;
        return false;
    }

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    // get parameter object from 'parameter_' table
    TString sql = TString::Format(
        "select parameter_name, parameter_type "
        "from parameter_ "
        "where parameter_id = %d", i_parameter_id);
    TSQLStatement* stmt = uni_db->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"Critical Error: getting record with parameter from 'parameter_' table has been failed"<<endl;
        delete stmt;
        return false;
    }

    stmt->StoreResult();

    // extract row with parameter
    if (!stmt->NextResultRow())
    {
        cout<<"Critical Error: the parameter with id '"<<i_parameter_id<<"' wasn't found"<<endl;
        delete stmt;
        return false;
    }

    TString parameter_name = stmt->GetString(0);
    int parameter_type = stmt->GetInt(1);
    delete stmt;

    // if parameter - not boolean type
    if (parameter_type != BoolType)
    {
        cout<<"Critical Error: the parameter with name '"<<parameter_name<<"' isn't boolean type"<<endl;
        return false;
    }

    return *((bool*)blob_parameter_value);
}

// set boolean value to detector parameter
int MpdDbDetectorParameter::SetBool(bool parameter_value)
{
    if (!connectionUniDb)
    {
        cout<<"Connection object is null"<<endl;
        return -1;
    }

    bool* p_parameter_value = new bool[1];
    p_parameter_value[0] = parameter_value;
    Long_t size_parameter_value = sizeof(bool);

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    TString sql = TString::Format(
        "update detector_parameter "
        "set parameter_value = $1 "
        "where run_number = $2 and detector_name = $3 and parameter_id = $4");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetLargeObject(0, p_parameter_value, size_parameter_value);
    stmt->SetInt(1, i_run_number);
    stmt->SetString(2, str_detector_name);
    stmt->SetInt(3, i_parameter_id);

    // write new value to database
    if (!stmt->Process())
    {
        cout<<"Error: updating the detector parameter has been failed"<<endl;

        delete stmt;
        delete [] p_parameter_value;
        return -2;
    }


    if (blob_parameter_value) delete [] blob_parameter_value;
    blob_parameter_value = (unsigned char*)p_parameter_value;
    sz_parameter_value = size_parameter_value;

    delete stmt;
    return 0;
}

// create integer detector parameter
MpdDbDetectorParameter* MpdDbDetectorParameter::CreateDetectorParameter(int run_number, TString detector_name, TString parameter_name, int parameter_value)
{
    MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00) return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    // get parameter object from 'parameter_' table
    TString sql = TString::Format(
        "select parameter_id, parameter_name, parameter_type "
        "from parameter_ "
        "where lower(parameter_name) = lower('%s')", parameter_name.Data());
    TSQLStatement* stmt = uni_db->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"Error: getting record with parameter from 'parameter_' table has been failed"<<endl;

        delete stmt;
        delete connUniDb;
        return 0x00;
    }

    stmt->StoreResult();

    // extract row with parameter
    if (!stmt->NextResultRow())
    {
        cout<<"Error: the parameter with name '"<<parameter_name<<"' wasn't found"<<endl;

        delete stmt;
        delete connUniDb;
        return 0x00;
    }

    int parameter_id = stmt->GetInt(0);
    int parameter_type = stmt->GetInt(2);

    delete stmt;

    // if parameter - not integer type
    if (parameter_type != IntType)
    {
        cout<<"Error: the parameter with name '"<<parameter_name<<"' isn't integer type"<<endl;

        delete connUniDb;
        return 0x00;
    }

    Int_t* p_parameter_value = new Int_t[1];
    p_parameter_value[0] = parameter_value;
    Long_t size_parameter_value = sizeof(Int_t);

    sql = TString::Format(
        "insert into detector_parameter(run_number, detector_name, parameter_id, parameter_value) "
        "values ($1, $2, $3, $4)");
    stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, run_number);
    stmt->SetString(1, detector_name);
    stmt->SetInt(2, parameter_id);
    stmt->SetLargeObject(3, p_parameter_value, size_parameter_value);
    //cout<<p_parameter_value<<" "<<p_parameter_value[0]<<" "<<size_parameter_value<<endl;

    // inserting new record to DB
    if (!stmt->Process())
    {
        cout<<"Error: inserting new parameter value to DB has been failed"<<endl;
        delete stmt;
        delete connUniDb;
        delete [] p_parameter_value;
        return 0x00;
    }

    delete stmt;

    return new MpdDbDetectorParameter(connUniDb, run_number, detector_name, parameter_id, (unsigned char*)p_parameter_value, size_parameter_value);
}

// get integer value of detector parameter (for current run, detector and parameter)
int MpdDbDetectorParameter::GetInt()
{
    if (!connectionUniDb)
    {
        cout<<"Critical Error: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    // get parameter object from 'parameter_' table
    TString sql = TString::Format(
        "select parameter_name, parameter_type "
        "from parameter_ "
        "where parameter_id = %d", i_parameter_id);
    TSQLStatement* stmt = uni_db->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"Critical Error: getting record with parameter from 'parameter_' table has been failed"<<endl;
        delete stmt;
        return -1;
    }

    stmt->StoreResult();

    // extract row with parameter
    if (!stmt->NextResultRow())
    {
        cout<<"Critical Error: the parameter with id '"<<i_parameter_id<<"' wasn't found"<<endl;
        delete stmt;
        return -1;
    }

    TString parameter_name = stmt->GetString(0);
    int parameter_type = stmt->GetInt(1);
    delete stmt;

    // if parameter - not integer type
    if (parameter_type != IntType)
    {
        cout<<"Critical Error: the parameter with name '"<<parameter_name<<"' isn't integer type"<<endl;
        return -1;
    }

    return *((int*)blob_parameter_value);
}

// set integer value to detector parameter
int MpdDbDetectorParameter::SetInt(int parameter_value)
{
    if (!connectionUniDb)
    {
        cout<<"Connection object is null"<<endl;
        return -1;
    }

    Int_t* p_parameter_value = new Int_t[1];
    p_parameter_value[0] = parameter_value;
    Long_t size_parameter_value = sizeof(Int_t);

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    TString sql = TString::Format(
        "update detector_parameter "
        "set parameter_value = $1 "
        "where run_number = $2 and detector_name = $3 and parameter_id = $4");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetLargeObject(0, p_parameter_value, size_parameter_value);
    stmt->SetInt(1, i_run_number);
    stmt->SetString(2, str_detector_name);
    stmt->SetInt(3, i_parameter_id);

    // write new value to database
    if (!stmt->Process())
    {
        cout<<"Error: updating the detector parameter has been failed"<<endl;

        delete stmt;
        delete [] p_parameter_value;
        return -2;
    }


    if (blob_parameter_value) delete [] blob_parameter_value;
    blob_parameter_value = (unsigned char*)p_parameter_value;
    sz_parameter_value = size_parameter_value;

    delete stmt;
    return 0;
}

// create double detector parameter
MpdDbDetectorParameter* MpdDbDetectorParameter::CreateDetectorParameter(int run_number, TString detector_name, TString parameter_name, double parameter_value)
{
    MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00) return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    // get parameter object from 'parameter_' table
    TString sql = TString::Format(
        "select parameter_id, parameter_name, parameter_type "
        "from parameter_ "
        "where lower(parameter_name) = lower('%s')", parameter_name.Data());
    TSQLStatement* stmt = uni_db->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"Error: getting record with parameter from 'parameter_' table has been failed"<<endl;

        delete stmt;
        delete connUniDb;
        return 0x00;
    }

    stmt->StoreResult();

    // extract row with parameter
    if (!stmt->NextResultRow())
    {
        cout<<"Error: the parameter with name '"<<parameter_name<<"' wasn't found"<<endl;

        delete stmt;
        delete connUniDb;
        return 0x00;
    }

    int parameter_id = stmt->GetInt(0);
    int parameter_type = stmt->GetInt(2);

    delete stmt;

    // if parameter - not double type
    if (parameter_type != DoubleType)
    {
        cout<<"Error: the parameter with name '"<<parameter_name<<"' isn't double type"<<endl;

        delete connUniDb;
        return 0x00;
    }

    Double_t* p_parameter_value = new Double_t[1];
    p_parameter_value[0] = parameter_value;
    Long_t size_parameter_value = sizeof(Double_t);

    sql = TString::Format(
        "insert into detector_parameter(run_number, detector_name, parameter_id, parameter_value) "
        "values ($1, $2, $3, $4)");
    stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, run_number);
    stmt->SetString(1, detector_name);
    stmt->SetInt(2, parameter_id);
    stmt->SetLargeObject(3, p_parameter_value, size_parameter_value);
    //cout<<p_parameter_value<<" "<<p_parameter_value[0]<<" "<<size_parameter_value<<endl;

    // inserting new record to DB
    if (!stmt->Process())
    {
        cout<<"Error: inserting new parameter value to DB has been failed"<<endl;
        delete stmt;
        delete connUniDb;
        delete [] p_parameter_value;
        return 0x00;
    }

    delete stmt;

    return new MpdDbDetectorParameter(connUniDb, run_number, detector_name, parameter_id, (unsigned char*)p_parameter_value, size_parameter_value);
}

// get double value of detector parameter (for current run, detector and parameter)
double MpdDbDetectorParameter::GetDouble()
{
    if (!connectionUniDb)
    {
        cout<<"Critical Error: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    // get parameter object from 'parameter_' table
    TString sql = TString::Format(
        "select parameter_name, parameter_type "
        "from parameter_ "
        "where parameter_id = %d", i_parameter_id);
    TSQLStatement* stmt = uni_db->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"Critical Error: getting record with parameter from 'parameter_' table has been failed"<<endl;
        delete stmt;
        return -1;
    }

    stmt->StoreResult();

    // extract row with parameter
    if (!stmt->NextResultRow())
    {
        cout<<"Critical Error: the parameter with id '"<<i_parameter_id<<"' wasn't found"<<endl;
        delete stmt;
        return -1;
    }

    TString parameter_name = stmt->GetString(0);
    int parameter_type = stmt->GetInt(1);
    delete stmt;

    // if parameter - not double type
    if (parameter_type != DoubleType)
    {
        cout<<"Critical Error: the parameter with name '"<<parameter_name<<"' isn't double type"<<endl;
        return -1;
    }

    return *((double*)blob_parameter_value);
}

// set double value to detector parameter
int MpdDbDetectorParameter::SetDouble(double parameter_value)
{
    if (!connectionUniDb)
    {
        cout<<"Connection object is null"<<endl;
        return -1;
    }

    Double_t* p_parameter_value = new Double_t[1];
    p_parameter_value[0] = parameter_value;
    Long_t size_parameter_value = sizeof(Double_t);

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    TString sql = TString::Format(
        "update detector_parameter "
        "set parameter_value = $1 "
        "where run_number = $2 and detector_name = $3 and parameter_id = $4");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetLargeObject(0, p_parameter_value, size_parameter_value);
    stmt->SetInt(1, i_run_number);
    stmt->SetString(2, str_detector_name);
    stmt->SetInt(3, i_parameter_id);

    // write new value to database
    if (!stmt->Process())
    {
        cout<<"Error: updating the detector parameter has been failed"<<endl;

        delete stmt;
        delete [] p_parameter_value;
        return -2;
    }


    if (blob_parameter_value) delete [] blob_parameter_value;
    blob_parameter_value = (unsigned char*)p_parameter_value;
    sz_parameter_value = size_parameter_value;

    delete stmt;
    return 0;
}

// create string detector parameter
MpdDbDetectorParameter* MpdDbDetectorParameter::CreateDetectorParameter(int run_number, TString detector_name, TString parameter_name, TString parameter_value)
{
    MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00) return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    // get parameter object from 'parameter_' table
    TString sql = TString::Format(
        "select parameter_id, parameter_name, parameter_type "
        "from parameter_ "
        "where lower(parameter_name) = lower('%s')", parameter_name.Data());
    TSQLStatement* stmt = uni_db->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"Error: getting record with parameter from 'parameter_' table has been failed"<<endl;

        delete stmt;
        delete connUniDb;
        return 0x00;
    }

    stmt->StoreResult();

    // extract row with parameter
    if (!stmt->NextResultRow())
    {
        cout<<"Error: the parameter with name '"<<parameter_name<<"' wasn't found"<<endl;

        delete stmt;
        delete connUniDb;
        return 0x00;
    }

    int parameter_id = stmt->GetInt(0);
    int parameter_type = stmt->GetInt(2);

    delete stmt;

    // if parameter - not string type
    if (parameter_type != StringType)
    {
        cout<<"Error: the parameter with name '"<<parameter_name<<"' isn't string type"<<endl;

        delete connUniDb;
        return 0x00;
    }

    Long_t size_parameter_value = parameter_value.Length()+1;
    char* p_parameter_value = new char[size_parameter_value];
    strcpy(p_parameter_value, parameter_value.Data());

    sql = TString::Format(
        "insert into detector_parameter(run_number, detector_name, parameter_id, parameter_value) "
        "values ($1, $2, $3, $4)");
    stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, run_number);
    stmt->SetString(1, detector_name);
    stmt->SetInt(2, parameter_id);
    stmt->SetLargeObject(3, p_parameter_value, size_parameter_value);
    //cout<<p_parameter_value<<" "<<p_parameter_value[0]<<" "<<size_parameter_value<<endl;

    // inserting new record to DB
    if (!stmt->Process())
    {
        cout<<"Error: inserting new parameter value to DB has been failed"<<endl;
        delete stmt;
        delete connUniDb;
        delete [] p_parameter_value;
        return 0x00;
    }

    delete stmt;

    return new MpdDbDetectorParameter(connUniDb, run_number, detector_name, parameter_id, (unsigned char*)p_parameter_value, size_parameter_value);
}

// get string value of detector parameter (for current run, detector and parameter)
TString MpdDbDetectorParameter::GetString()
{
    if (!connectionUniDb)
    {
        cout<<"Critical Error: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    // get parameter object from 'parameter_' table
    TString sql = TString::Format(
        "select parameter_name, parameter_type "
        "from parameter_ "
        "where parameter_id = %d", i_parameter_id);
    TSQLStatement* stmt = uni_db->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"Critical Error: getting record with parameter from 'parameter_' table has been failed"<<endl;
        delete stmt;
        return -1;
    }

    stmt->StoreResult();

    // extract row with parameter
    if (!stmt->NextResultRow())
    {
        cout<<"Critical Error: the parameter with id '"<<i_parameter_id<<"' wasn't found"<<endl;
        delete stmt;
        return -1;
    }

    TString parameter_name = stmt->GetString(0);
    int parameter_type = stmt->GetInt(1);
    delete stmt;

    // if parameter - not string type
    if (parameter_type != StringType)
    {
        cout<<"Critical Error: the parameter with name '"<<parameter_name<<"' isn't string type"<<endl;
        return -1;
    }

    return (char*)blob_parameter_value;
}

// set string value to detector parameter
int MpdDbDetectorParameter::SetString(TString parameter_value)
{
    if (!connectionUniDb)
    {
        cout<<"Connection object is null"<<endl;
        return -1;
    }

    Long_t size_parameter_value = parameter_value.Length()+1;
    char* p_parameter_value = new char[size_parameter_value];
    strcpy(p_parameter_value, parameter_value.Data());

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    TString sql = TString::Format(
        "update detector_parameter "
        "set parameter_value = $1 "
        "where run_number = $2 and detector_name = $3 and parameter_id = $4");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetLargeObject(0, p_parameter_value, size_parameter_value);
    stmt->SetInt(1, i_run_number);
    stmt->SetString(2, str_detector_name);
    stmt->SetInt(3, i_parameter_id);

    // write new value to database
    if (!stmt->Process())
    {
        cout<<"Error: updating the detector parameter has been failed"<<endl;

        delete stmt;
        delete [] p_parameter_value;
        return -2;
    }


    if (blob_parameter_value) delete [] blob_parameter_value;
    blob_parameter_value = (unsigned char*)p_parameter_value;
    sz_parameter_value = size_parameter_value;

    delete stmt;
    return 0;
}

// create Int+Int Array detector parameter
MpdDbDetectorParameter* MpdDbDetectorParameter::CreateDetectorParameter(int run_number, TString detector_name, TString parameter_name, IIStructure* parameter_value, int element_count)
{
    MpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00) return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    // get parameter object from 'parameter_' table
    TString sql = TString::Format(
        "select parameter_id, parameter_name, parameter_type "
        "from parameter_ "
        "where lower(parameter_name) = lower('%s')", parameter_name.Data());
    TSQLStatement* stmt = uni_db->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"Error: getting record with parameter from 'parameter_' table has been failed"<<endl;

        delete stmt;
        delete connUniDb;
        return 0x00;
    }

    stmt->StoreResult();

    // extract row with parameter
    if (!stmt->NextResultRow())
    {
        cout<<"Error: the parameter with name '"<<parameter_name<<"' wasn't found"<<endl;

        delete stmt;
        delete connUniDb;
        return 0x00;
    }

    int parameter_id = stmt->GetInt(0);
    int parameter_type = stmt->GetInt(2);

    delete stmt;

    // if parameter - not IIArray type
    if (parameter_type != IIArrayType)
    {
        cout<<"Error: the parameter with name '"<<parameter_name<<"' isn't IntIntArray type"<<endl;

        delete connUniDb;
        return 0x00;
    }

    Long_t size_parameter_value = element_count * sizeof(IIStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    sql = TString::Format(
        "insert into detector_parameter(run_number, detector_name, parameter_id, parameter_value) "
        "values ($1, $2, $3, $4)");
    stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, run_number);
    stmt->SetString(1, detector_name);
    stmt->SetInt(2, parameter_id);
    stmt->SetLargeObject(3, p_parameter_value, size_parameter_value);
    //cout<<p_parameter_value<<" "<<p_parameter_value[0]<<" "<<size_parameter_value<<endl;

    // inserting new record to DB
    if (!stmt->Process())
    {
        cout<<"Error: inserting new parameter value to DB has been failed"<<endl;
        delete stmt;
        delete connUniDb;
        delete [] p_parameter_value;
        return 0x00;
    }

    delete stmt;

    return new MpdDbDetectorParameter(connUniDb, run_number, detector_name, parameter_id, p_parameter_value, size_parameter_value);
}

// get Int+Int array for detector parameter (for current run, detector and parameter)
int MpdDbDetectorParameter::GetIIArray(IIStructure*& parameter_value, int& element_count)
{
    if (!connectionUniDb)
    {
        cout<<"Critical Error: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    // get parameter object from 'parameter_' table
    TString sql = TString::Format(
        "select parameter_name, parameter_type "
        "from parameter_ "
        "where parameter_id = %d", i_parameter_id);
    TSQLStatement* stmt = uni_db->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"Critical Error: getting record with parameter from 'parameter_' table has been failed"<<endl;
        delete stmt;
        return -2;
    }

    stmt->StoreResult();

    // extract row with parameter
    if (!stmt->NextResultRow())
    {
        cout<<"Critical Error: the parameter with id '"<<i_parameter_id<<"' wasn't found"<<endl;
        delete stmt;
        return -3;
    }

    TString parameter_name = stmt->GetString(0);
    int parameter_type = stmt->GetInt(1);
    delete stmt;

    // if parameter - not IIArray type
    if (parameter_type != IIArrayType)
    {
        cout<<"Critical Error: the parameter with name '"<<parameter_name<<"' isn't IntIntArray type"<<endl;
        return -4;
    }

    element_count = sz_parameter_value / sizeof(IIStructure);
    parameter_value = new IIStructure[element_count];
    memcpy(parameter_value, blob_parameter_value, sz_parameter_value);

    return 0;
}

// set Int+Int array for detector parameter
int MpdDbDetectorParameter::SetIIArray(IIStructure* parameter_value, int element_count)
{
    if (!connectionUniDb)
    {
        cout<<"Connection object is null"<<endl;
        return -1;
    }

    Long_t size_parameter_value = element_count * sizeof(IIStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    TString sql = TString::Format(
        "update detector_parameter "
        "set parameter_value = $1 "
        "where run_number = $2 and detector_name = $3 and parameter_id = $4");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetLargeObject(0, p_parameter_value, size_parameter_value);
    stmt->SetInt(1, i_run_number);
    stmt->SetString(2, str_detector_name);
    stmt->SetInt(3, i_parameter_id);

    // write new value to database
    if (!stmt->Process())
    {
        cout<<"Error: updating the detector parameter has been failed"<<endl;

        delete stmt;
        delete [] p_parameter_value;
        return -2;
    }


    if (blob_parameter_value) delete [] blob_parameter_value;
    blob_parameter_value = (unsigned char*)p_parameter_value;
    sz_parameter_value = size_parameter_value;

    delete stmt;
    return 0;
}

// -------------------------------------------------------------------
ClassImp(MpdDbDetectorParameter);
