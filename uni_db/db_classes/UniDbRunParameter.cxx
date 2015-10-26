// ----------------------------------------------------------------------
//                    UniDbRunParameter cxx file 
//                      Generated 20-10-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "UniDbRunParameter.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniDbRunParameter::UniDbRunParameter(UniDbConnection* connUniDb, int run_number, TString detector_name, int parameter_id, unsigned char* parameter_value, Long_t size_parameter_value)
{
	connectionUniDb = connUniDb;

	i_run_number = run_number;
	str_detector_name = detector_name;
	i_parameter_id = parameter_id;
	blob_parameter_value = parameter_value;
	sz_parameter_value = size_parameter_value;
}

// -----   Destructor   -------------------------------------------------
UniDbRunParameter::~UniDbRunParameter()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (blob_parameter_value)
		delete [] blob_parameter_value;
}

// -----   Creating new record in class table ---------------------------
UniDbRunParameter* UniDbRunParameter::CreateRunParameter(int run_number, TString detector_name, int parameter_id, unsigned char* parameter_value, Long_t size_parameter_value)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into run_parameter(run_number, detector_name, parameter_id, parameter_value) "
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

	return new UniDbRunParameter(connUniDb, tmp_run_number, tmp_detector_name, tmp_parameter_id, tmp_parameter_value, tmp_sz_parameter_value);
}

// -----   Get table record from database ---------------------------
UniDbRunParameter* UniDbRunParameter::GetRunParameter(int run_number, TString detector_name, int parameter_id)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select run_number, detector_name, parameter_id, parameter_value "
		"from run_parameter "
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

	return new UniDbRunParameter(connUniDb, tmp_run_number, tmp_detector_name, tmp_parameter_id, tmp_parameter_value, tmp_sz_parameter_value);
}

// -----   Delete record from class table ---------------------------
int UniDbRunParameter::DeleteRunParameter(int run_number, TString detector_name, int parameter_id)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from run_parameter "
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
int UniDbRunParameter::PrintAll()
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select run_number, detector_name, parameter_id, parameter_value "
		"from run_parameter");
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
	cout<<"Table 'run_parameter'"<<endl;
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
int UniDbRunParameter::SetRunNumber(int run_number)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_parameter "
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

int UniDbRunParameter::SetDetectorName(TString detector_name)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_parameter "
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

int UniDbRunParameter::SetParameterId(int parameter_id)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_parameter "
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

int UniDbRunParameter::SetParameterValue(unsigned char* parameter_value, Long_t size_parameter_value)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update run_parameter "
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
void UniDbRunParameter::Print()
{
	cout<<"Table 'run_parameter'";
	cout<<". run_number: "<<i_run_number<<". detector_name: "<<str_detector_name<<". parameter_id: "<<i_parameter_id<<". parameter_value: "<<(void*)blob_parameter_value<<", binary size: "<<sz_parameter_value<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// get detector parameter by run number, detector name and parameter name
UniDbRunParameter* UniDbRunParameter::GetRunParameter(int run_number, TString detector_name, TString parameter_name)
{
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00) return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
        "select run_number, detector_name, p.parameter_id, parameter_value "
        "from run_parameter dp join parameter_ p on dp.parameter_id = p.parameter_id "
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

    return new UniDbRunParameter(connUniDb, tmp_run_number, tmp_detector_name, tmp_parameter_id, tmp_parameter_value, tmp_sz_parameter_value);
}

// common function for creating parameter
UniDbRunParameter* UniDbRunParameter::CreateRunParameter(int run_number, TString detector_name, TString parameter_name, unsigned char* p_parameter_value, Long_t size_parameter_value, enumParameterType enum_parameter_type)
{
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00) return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    int parameter_id = -1;
    bool res_code = UniDbParameter::CheckAndGetParameterID(uni_db, parameter_name, enum_parameter_type, parameter_id);
    if (!res_code)
    {
        delete connUniDb;
        return 0x00;
    }

    TString sql = TString::Format(
        "insert into run_parameter(run_number, detector_name, parameter_id, parameter_value) "
        "values ($1, $2, $3, $4)");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, run_number);
    stmt->SetString(1, detector_name);
    stmt->SetInt(2, parameter_id);
    stmt->SetLargeObject(3, (void*)p_parameter_value, size_parameter_value);
    //cout<<p_parameter_value<<" "<<p_parameter_value[0]<<" "<<size_parameter_value<<endl;

    // inserting new record to DB
    if (!stmt->Process())
    {
        cout<<"Error: inserting new parameter value to DB has been failed"<<endl;
        delete stmt;
        delete connUniDb;
        return 0x00;
    }

    delete stmt;

    return new UniDbRunParameter(connUniDb, run_number, detector_name, parameter_id, p_parameter_value, size_parameter_value);
}

// create boolean detector parameter
UniDbRunParameter* UniDbRunParameter::CreateRunParameter(int run_number, TString detector_name, TString parameter_name, bool parameter_value)
{
    Long_t size_parameter_value = sizeof(bool);
    bool* p_parameter_value = new bool[1];
    p_parameter_value[0] = parameter_value;

    UniDbRunParameter* pRunParameter = UniDbRunParameter::CreateRunParameter(run_number, detector_name, parameter_name, (unsigned char*) p_parameter_value, size_parameter_value, BoolType);

    if (pRunParameter == 0x00)
        delete [] p_parameter_value;

    return pRunParameter;
}

// create boolean detector parameter for run range (from start_run_number to end_run_number)
bool UniDbRunParameter::CreateRunParameters(int start_run_number, int end_run_number, TString detector_name, TString parameter_name, bool parameter_value)
{
    if (end_run_number < start_run_number)
    {
        cout<<"Error: end run number should be greater than start number"<<endl;
        return false;
    }

    bool is_errors = false;
    for (int i = start_run_number; i <= end_run_number; i++)
    {
        UniDbRunParameter* pRunParameter = UniDbRunParameter::CreateRunParameter(i, detector_name, parameter_name, parameter_value);

        if (pRunParameter)
            delete pRunParameter;
        else
            is_errors = true;
    }

    return (!is_errors);
}

// create integer detector parameter
UniDbRunParameter* UniDbRunParameter::CreateRunParameter(int run_number, TString detector_name, TString parameter_name, int parameter_value)
{
    Long_t size_parameter_value = sizeof(Int_t);
    Int_t* p_parameter_value = new Int_t[1];
    p_parameter_value[0] = parameter_value;

    UniDbRunParameter* pRunParameter = UniDbRunParameter::CreateRunParameter(run_number, detector_name, parameter_name, (unsigned char*) p_parameter_value, size_parameter_value, IntType);

    if (pRunParameter == 0x00)
        delete [] p_parameter_value;

    return pRunParameter;
}

// create double detector parameter
UniDbRunParameter* UniDbRunParameter::CreateRunParameter(int run_number, TString detector_name, TString parameter_name, double parameter_value)
{
    Long_t size_parameter_value = sizeof(Double_t);
    Double_t* p_parameter_value = new Double_t[1];
    p_parameter_value[0] = parameter_value;

    UniDbRunParameter* pRunParameter = UniDbRunParameter::CreateRunParameter(run_number, detector_name, parameter_name, (unsigned char*) p_parameter_value, size_parameter_value, DoubleType);

    if (pRunParameter == 0x00)
        delete [] p_parameter_value;

    return pRunParameter;
}

// create string detector parameter
UniDbRunParameter* UniDbRunParameter::CreateRunParameter(int run_number, TString detector_name, TString parameter_name, TString parameter_value)
{
    Long_t size_parameter_value = parameter_value.Length()+1;
    char* p_parameter_value = new char[size_parameter_value];
    strcpy(p_parameter_value, parameter_value.Data());

    UniDbRunParameter* pRunParameter = UniDbRunParameter::CreateRunParameter(run_number, detector_name, parameter_name, (unsigned char*) p_parameter_value, size_parameter_value, StringType);

    if (pRunParameter == 0x00)
        delete [] p_parameter_value;

    return pRunParameter;
}

// create Integer Array detector parameter
UniDbRunParameter* UniDbRunParameter::CreateRunParameter(int run_number, TString detector_name, TString parameter_name, int* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(int);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbRunParameter* pRunParameter = UniDbRunParameter::CreateRunParameter(run_number, detector_name, parameter_name, p_parameter_value, size_parameter_value, IntArrayType);

    if (pRunParameter == 0x00)
        delete [] p_parameter_value;

    return pRunParameter;
}

// create Double Array detector parameter
UniDbRunParameter* UniDbRunParameter::CreateRunParameter(int run_number, TString detector_name, TString parameter_name, double* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(double);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbRunParameter* pRunParameter = UniDbRunParameter::CreateRunParameter(run_number, detector_name, parameter_name, p_parameter_value, size_parameter_value, DoubleArrayType);

    if (pRunParameter == 0x00)
        delete [] p_parameter_value;

    return pRunParameter;
}

// create Int+Int Array detector parameter
UniDbRunParameter* UniDbRunParameter::CreateRunParameter(int run_number, TString detector_name, TString parameter_name, IIStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(IIStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbRunParameter* pRunParameter = UniDbRunParameter::CreateRunParameter(run_number, detector_name, parameter_name, p_parameter_value, size_parameter_value, IIArrayType);

    if (pRunParameter == 0x00)
        delete [] p_parameter_value;

    return pRunParameter;
}

// common function for getting parameter
unsigned char* UniDbRunParameter::GetUNC(enumParameterType enum_parameter_type)
{
    if (!connectionUniDb)
    {
        cout<<"Critical Error: Connection object is null"<<endl;
        return NULL;
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
        return NULL;
    }

    stmt->StoreResult();

    // extract row with parameter
    if (!stmt->NextResultRow())
    {
        cout<<"Critical Error: the parameter with id '"<<i_parameter_id<<"' wasn't found"<<endl;
        delete stmt;
        return NULL;
    }

    TString parameter_name = stmt->GetString(0);
    int parameter_type = stmt->GetInt(1);
    delete stmt;

    if (parameter_type != enum_parameter_type)
    {
        cout<<"Critical Error: the parameter with name '"<<parameter_name<<"' isn't as getting type"<<endl;
        return NULL;
    }

    return blob_parameter_value;
}

// get boolean value of detector parameter (for current run, detector and parameter)
bool UniDbRunParameter::GetBool()
{
    return *((bool*) GetUNC(BoolType));
}

// get integer value of detector parameter (for current run, detector and parameter)
int UniDbRunParameter::GetInt()
{
    return *((int*) GetUNC(IntType));
}

// get double value of detector parameter (for current run, detector and parameter)
double UniDbRunParameter::GetDouble()
{
    return *((double*) GetUNC(DoubleType));
}

// get string value of detector parameter (for current run, detector and parameter)
TString UniDbRunParameter::GetString()
{
    return (char*) GetUNC(StringType);
}

// get Integer array for detector parameter (for current run, detector and parameter)
int UniDbRunParameter::GetIntArray(int*& parameter_value, int& element_count)
{
    unsigned char* p_parameter_value = GetUNC(IntArrayType);
    if (p_parameter_value == NULL)
        return - 1;

    element_count = sz_parameter_value / sizeof(int);
    parameter_value = new int[element_count];
    memcpy(parameter_value, p_parameter_value, sz_parameter_value);

    return 0;
}

// get Double array for detector parameter (for current run, detector and parameter)
int UniDbRunParameter::GetDoubleArray(double*& parameter_value, int& element_count)
{
    unsigned char* p_parameter_value = GetUNC(DoubleArrayType);
    if (p_parameter_value == NULL)
        return - 1;

    element_count = sz_parameter_value / sizeof(double);
    parameter_value = new double[element_count];
    memcpy(parameter_value, p_parameter_value, sz_parameter_value);

    return 0;
}

// get Int+Int array for detector parameter (for current run, detector and parameter)
int UniDbRunParameter::GetIIArray(IIStructure*& parameter_value, int& element_count)
{
    unsigned char* p_parameter_value = GetUNC(IIArrayType);
    if (p_parameter_value == NULL)
        return - 1;

    element_count = sz_parameter_value / sizeof(IIStructure);
    parameter_value = new IIStructure[element_count];
    memcpy(parameter_value, p_parameter_value, sz_parameter_value);

    return 0;
}

// common function for setting parameter
int UniDbRunParameter::SetUNC(unsigned char* p_parameter_value, Long_t size_parameter_value)
{
    if (!connectionUniDb)
    {
        cout<<"Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    TString sql = TString::Format(
        "update run_parameter "
        "set parameter_value = $1 "
        "where run_number = $2 and detector_name = $3 and parameter_id = $4");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetLargeObject(0, (void*)p_parameter_value, size_parameter_value);
    stmt->SetInt(1, i_run_number);
    stmt->SetString(2, str_detector_name);
    stmt->SetInt(3, i_parameter_id);

    // write new value to database
    if (!stmt->Process())
    {
        cout<<"Error: updating the detector parameter has been failed"<<endl;
        delete stmt;
        return -2;
    }

    if (blob_parameter_value) delete [] blob_parameter_value;
    blob_parameter_value = p_parameter_value;
    sz_parameter_value = size_parameter_value;

    delete stmt;
    return 0;
}

// set boolean value to detector parameter
int UniDbRunParameter::SetBool(bool parameter_value)
{
    Long_t size_parameter_value = sizeof(bool);
    bool* p_parameter_value = new bool[1];
    p_parameter_value[0] = parameter_value;

    int res_code = SetUNC((unsigned char*)p_parameter_value, size_parameter_value);
    if (res_code != 0)
    {
        delete [] p_parameter_value;
        return res_code;
    }

    return 0;
}

// set integer value to detector parameter
int UniDbRunParameter::SetInt(int parameter_value)
{
    Long_t size_parameter_value = sizeof(Int_t);
    Int_t* p_parameter_value = new Int_t[1];
    p_parameter_value[0] = parameter_value;

    int res_code = SetUNC((unsigned char*)p_parameter_value, size_parameter_value);
    if (res_code != 0)
    {
        delete [] p_parameter_value;
        return res_code;
    }

    return 0;
}

// set double value to detector parameter
int UniDbRunParameter::SetDouble(double parameter_value)
{
    Long_t size_parameter_value = sizeof(Double_t);
    Double_t* p_parameter_value = new Double_t[1];
    p_parameter_value[0] = parameter_value;

    int res_code = SetUNC((unsigned char*)p_parameter_value, size_parameter_value);
    if (res_code != 0)
    {
        delete [] p_parameter_value;
        return res_code;
    }

    return 0;
}

// set string value to detector parameter
int UniDbRunParameter::SetString(TString parameter_value)
{
    Long_t size_parameter_value = parameter_value.Length()+1;
    char* p_parameter_value = new char[size_parameter_value];
    strcpy(p_parameter_value, parameter_value.Data());

    int res_code = SetUNC((unsigned char*)p_parameter_value, size_parameter_value);
    if (res_code != 0)
    {
        delete [] p_parameter_value;
        return res_code;
    }

    return 0;
}

// set Integer array for detector parameter
int UniDbRunParameter::SetIntArray(int* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(int);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    int res_code = SetUNC(p_parameter_value, size_parameter_value);
    if (res_code != 0)
    {
        delete [] p_parameter_value;
        return res_code;
    }

    return 0;
}

// set Double array for detector parameter
int UniDbRunParameter::SetDoubleArray(double* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(double);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    int res_code = SetUNC(p_parameter_value, size_parameter_value);
    if (res_code != 0)
    {
        delete [] p_parameter_value;
        return res_code;
    }

    return 0;
}

// set Int+Int array for detector parameter
int UniDbRunParameter::SetIIArray(IIStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(IIStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    int res_code = SetUNC(p_parameter_value, size_parameter_value);
    if (res_code != 0)
    {
        delete [] p_parameter_value;
        return res_code;
    }

    return 0;
}

// -------------------------------------------------------------------
ClassImp(UniDbRunParameter);
