// ----------------------------------------------------------------------
//                    UniDbSessionDcParameter cxx file 
//                      Generated 22-10-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "UniDbSessionDcParameter.h"

#include <iostream>
using namespace std;

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniDbSessionDcParameter::UniDbSessionDcParameter(UniDbConnection* connUniDb, int session_number, TString detector_name, int parameter_id, int dc_serial, int channel, unsigned char* parameter_value, Long_t size_parameter_value)
{
	connectionUniDb = connUniDb;

	i_session_number = session_number;
	str_detector_name = detector_name;
	i_parameter_id = parameter_id;
	i_dc_serial = dc_serial;
	i_channel = channel;
	blob_parameter_value = parameter_value;
	sz_parameter_value = size_parameter_value;
}

// -----   Destructor   -------------------------------------------------
UniDbSessionDcParameter::~UniDbSessionDcParameter()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (blob_parameter_value)
		delete [] blob_parameter_value;
}

// -----   Creating new record in class table ---------------------------
UniDbSessionDcParameter* UniDbSessionDcParameter::CreateSessionDcParameter(int session_number, TString detector_name, int parameter_id, int dc_serial, int channel, unsigned char* parameter_value, Long_t size_parameter_value)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into session_dc_parameter(session_number, detector_name, parameter_id, dc_serial, channel, parameter_value) "
		"values ($1, $2, $3, $4, $5, $6)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, session_number);
	stmt->SetString(1, detector_name);
	stmt->SetInt(2, parameter_id);
	stmt->SetInt(3, dc_serial);
	stmt->SetInt(4, channel);
	stmt->SetLargeObject(5, parameter_value, size_parameter_value, 0x4000000);

	// inserting new record to DB
	if (!stmt->Process())
	{
		cout<<"Error: inserting new record to DB has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	int tmp_session_number;
	tmp_session_number = session_number;
	TString tmp_detector_name;
	tmp_detector_name = detector_name;
	int tmp_parameter_id;
	tmp_parameter_id = parameter_id;
	int tmp_dc_serial;
	tmp_dc_serial = dc_serial;
	int tmp_channel;
	tmp_channel = channel;
	unsigned char* tmp_parameter_value;
	Long_t tmp_sz_parameter_value = size_parameter_value;
	tmp_parameter_value = new unsigned char[tmp_sz_parameter_value];
	memcpy(tmp_parameter_value, parameter_value, tmp_sz_parameter_value);

	return new UniDbSessionDcParameter(connUniDb, tmp_session_number, tmp_detector_name, tmp_parameter_id, tmp_dc_serial, tmp_channel, tmp_parameter_value, tmp_sz_parameter_value);
}

// -----   Get table record from database ---------------------------
UniDbSessionDcParameter* UniDbSessionDcParameter::GetSessionDcParameter(int session_number, TString detector_name, int parameter_id, int dc_serial, int channel)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select session_number, detector_name, parameter_id, dc_serial, channel, parameter_value "
		"from session_dc_parameter "
		"where session_number = %d and lower(detector_name) = lower('%s') and parameter_id = %d and dc_serial = %d and channel = %d", session_number, detector_name.Data(), parameter_id, dc_serial, channel);
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

	int tmp_session_number;
	tmp_session_number = stmt->GetInt(0);
	TString tmp_detector_name;
	tmp_detector_name = stmt->GetString(1);
	int tmp_parameter_id;
	tmp_parameter_id = stmt->GetInt(2);
	int tmp_dc_serial;
	tmp_dc_serial = stmt->GetInt(3);
	int tmp_channel;
	tmp_channel = stmt->GetInt(4);
	unsigned char* tmp_parameter_value;
	tmp_parameter_value = NULL;
	Long_t tmp_sz_parameter_value = 0;
	stmt->GetLargeObject(5, (void*&)tmp_parameter_value, tmp_sz_parameter_value);

	delete stmt;

	return new UniDbSessionDcParameter(connUniDb, tmp_session_number, tmp_detector_name, tmp_parameter_id, tmp_dc_serial, tmp_channel, tmp_parameter_value, tmp_sz_parameter_value);
}

// -----   Delete record from class table ---------------------------
int UniDbSessionDcParameter::DeleteSessionDcParameter(int session_number, TString detector_name, int parameter_id, int dc_serial, int channel)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from session_dc_parameter "
		"where session_number = $1 and lower(detector_name) = lower($2) and parameter_id = $3 and dc_serial = $4 and channel = $5");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, session_number);
	stmt->SetString(1, detector_name);
	stmt->SetInt(2, parameter_id);
	stmt->SetInt(3, dc_serial);
	stmt->SetInt(4, channel);

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
int UniDbSessionDcParameter::PrintAll()
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select session_number, detector_name, parameter_id, dc_serial, channel, parameter_value "
		"from session_dc_parameter");
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
	cout<<"Table 'session_dc_parameter'"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<". session_number: ";
		cout<<(stmt->GetInt(0));
		cout<<". detector_name: ";
		cout<<(stmt->GetString(1));
		cout<<". parameter_id: ";
		cout<<(stmt->GetInt(2));
		cout<<". dc_serial: ";
		cout<<(stmt->GetInt(3));
		cout<<". channel: ";
		cout<<(stmt->GetInt(4));
		cout<<". parameter_value: ";
		unsigned char* tmp_parameter_value = NULL;
		Long_t tmp_sz_parameter_value=0;
		stmt->GetLargeObject(5, (void*&)tmp_parameter_value, tmp_sz_parameter_value);
		cout<<(void*)tmp_parameter_value<<", binary size: "<<tmp_sz_parameter_value;
		cout<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int UniDbSessionDcParameter::SetSessionNumber(int session_number)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update session_dc_parameter "
		"set session_number = $1 "
		"where session_number = $2 and detector_name = $3 and parameter_id = $4 and dc_serial = $5 and channel = $6");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, session_number);
	stmt->SetInt(1, i_session_number);
	stmt->SetString(2, str_detector_name);
	stmt->SetInt(3, i_parameter_id);
	stmt->SetInt(4, i_dc_serial);
	stmt->SetInt(5, i_channel);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_session_number = session_number;

	delete stmt;
	return 0;
}

int UniDbSessionDcParameter::SetDetectorName(TString detector_name)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update session_dc_parameter "
		"set detector_name = $1 "
		"where session_number = $2 and detector_name = $3 and parameter_id = $4 and dc_serial = $5 and channel = $6");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, detector_name);
	stmt->SetInt(1, i_session_number);
	stmt->SetString(2, str_detector_name);
	stmt->SetInt(3, i_parameter_id);
	stmt->SetInt(4, i_dc_serial);
	stmt->SetInt(5, i_channel);

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

int UniDbSessionDcParameter::SetParameterId(int parameter_id)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update session_dc_parameter "
		"set parameter_id = $1 "
		"where session_number = $2 and detector_name = $3 and parameter_id = $4 and dc_serial = $5 and channel = $6");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, parameter_id);
	stmt->SetInt(1, i_session_number);
	stmt->SetString(2, str_detector_name);
	stmt->SetInt(3, i_parameter_id);
	stmt->SetInt(4, i_dc_serial);
	stmt->SetInt(5, i_channel);

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

int UniDbSessionDcParameter::SetDcSerial(int dc_serial)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update session_dc_parameter "
		"set dc_serial = $1 "
		"where session_number = $2 and detector_name = $3 and parameter_id = $4 and dc_serial = $5 and channel = $6");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, dc_serial);
	stmt->SetInt(1, i_session_number);
	stmt->SetString(2, str_detector_name);
	stmt->SetInt(3, i_parameter_id);
	stmt->SetInt(4, i_dc_serial);
	stmt->SetInt(5, i_channel);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_dc_serial = dc_serial;

	delete stmt;
	return 0;
}

int UniDbSessionDcParameter::SetChannel(int channel)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update session_dc_parameter "
		"set channel = $1 "
		"where session_number = $2 and detector_name = $3 and parameter_id = $4 and dc_serial = $5 and channel = $6");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, channel);
	stmt->SetInt(1, i_session_number);
	stmt->SetString(2, str_detector_name);
	stmt->SetInt(3, i_parameter_id);
	stmt->SetInt(4, i_dc_serial);
	stmt->SetInt(5, i_channel);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_channel = channel;

	delete stmt;
	return 0;
}

int UniDbSessionDcParameter::SetParameterValue(unsigned char* parameter_value, Long_t size_parameter_value)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update session_dc_parameter "
		"set parameter_value = $1 "
		"where session_number = $2 and detector_name = $3 and parameter_id = $4 and dc_serial = $5 and channel = $6");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetLargeObject(0, parameter_value, size_parameter_value, 0x4000000);
	stmt->SetInt(1, i_session_number);
	stmt->SetString(2, str_detector_name);
	stmt->SetInt(3, i_parameter_id);
	stmt->SetInt(4, i_dc_serial);
	stmt->SetInt(5, i_channel);

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
void UniDbSessionDcParameter::Print()
{
	cout<<"Table 'session_dc_parameter'";
	cout<<". session_number: "<<i_session_number<<". detector_name: "<<str_detector_name<<". parameter_id: "<<i_parameter_id<<". dc_serial: "<<i_dc_serial<<". channel: "<<i_channel<<". parameter_value: "<<(void*)blob_parameter_value<<", binary size: "<<sz_parameter_value<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

// get detector parameter by session number, detector name, parameter name, TDC/ADC serial and channel number
UniDbSessionDcParameter* UniDbSessionDcParameter::GetSessionDcParameter(int session_number, TString detector_name, TString parameter_name, int dc_serial, int channel)
{
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00) return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
        "select session_number, detector_name, p.parameter_id, dc_serial, channel, parameter_value "
        "from session_dc_parameter dp join parameter_ p on dp.parameter_id = p.parameter_id "
        "where session_number = %d and lower(detector_name) = lower('%s') and lower(parameter_name) = lower('%s') and dc_serial = %d and channel = %d", session_number, detector_name.Data(), parameter_name.Data(), dc_serial, channel);
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

    int tmp_session_number;
    tmp_session_number = stmt->GetInt(0);
    TString tmp_detector_name;
    tmp_detector_name = stmt->GetString(1);
    int tmp_parameter_id;
    tmp_parameter_id = stmt->GetInt(2);
    int tmp_dc_serial;
    tmp_dc_serial = stmt->GetInt(3);
    int tmp_channel;
    tmp_channel = stmt->GetInt(4);
    unsigned char* tmp_parameter_value;
    tmp_parameter_value = NULL;
    Long_t tmp_sz_parameter_value = 0;
    stmt->GetLargeObject(5, (void*&)tmp_parameter_value, tmp_sz_parameter_value);

    delete stmt;

    return new UniDbSessionDcParameter(connUniDb, tmp_session_number, tmp_detector_name, tmp_parameter_id, tmp_dc_serial, tmp_channel, tmp_parameter_value, tmp_sz_parameter_value);
}

// common function for creating parameter
UniDbSessionDcParameter* UniDbSessionDcParameter::CreateSessionDcParameter(int session_number, TString detector_name, TString parameter_name, int dc_serial, int channel, unsigned char* p_parameter_value, Long_t size_parameter_value, enumParameterType enum_parameter_type)
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

    // insert new parameter value to the database
    TString sql = TString::Format(
        "insert into session_dc_parameter(session_number, detector_name, parameter_id, dc_serial, channel, parameter_value) "
        "values ($1, $2, $3, $4, $5, $6)");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, session_number);
    stmt->SetString(1, detector_name);
    stmt->SetInt(2, parameter_id);
    stmt->SetInt(3, dc_serial);
    stmt->SetInt(4, channel);
    stmt->SetLargeObject(5, (void*)p_parameter_value, size_parameter_value);
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

    return new UniDbSessionDcParameter(connUniDb, session_number, detector_name, parameter_id, dc_serial, channel, p_parameter_value, size_parameter_value);
}

// create boolean TDC parameter for one channel
UniDbSessionDcParameter* UniDbSessionDcParameter::CreateSessionDcParameter(int session_number, TString detector_name, TString parameter_name, int dc_serial, int channel, bool parameter_value)
{
    Long_t size_parameter_value = sizeof(bool);
    bool* p_parameter_value = new bool[1];
    p_parameter_value[0] = parameter_value;

    UniDbSessionDcParameter* pSessionParameter = UniDbSessionDcParameter::CreateSessionDcParameter(session_number, detector_name, parameter_name, dc_serial, channel, (unsigned char*) p_parameter_value, size_parameter_value, BoolType);

    if (pSessionParameter == 0x00)
        delete [] p_parameter_value;

    return pSessionParameter;
}

// create int TDC parameter for one channel
UniDbSessionDcParameter* UniDbSessionDcParameter::CreateSessionDcParameter(int run_number, TString detector_name, TString parameter_name, int dc_serial, int channel, int parameter_value)
{
    Long_t size_parameter_value = sizeof(Int_t);
    Int_t* p_parameter_value = new Int_t[1];
    p_parameter_value[0] = parameter_value;

    UniDbSessionDcParameter* pSessionParameter = UniDbSessionDcParameter::CreateSessionDcParameter(run_number, detector_name, parameter_name, dc_serial, channel, (unsigned char*) p_parameter_value, size_parameter_value, IntType);

    if (pSessionParameter == 0x00)
        delete [] p_parameter_value;

    return pSessionParameter;
}

// create double detector parameter
UniDbSessionDcParameter* UniDbSessionDcParameter::CreateSessionDcParameter(int run_number, TString detector_name, TString parameter_name, int dc_serial, int channel, double parameter_value)
{
    Long_t size_parameter_value = sizeof(Double_t);
    Double_t* p_parameter_value = new Double_t[1];
    p_parameter_value[0] = parameter_value;

    UniDbSessionDcParameter* pSessionParameter = UniDbSessionDcParameter::CreateSessionDcParameter(run_number, detector_name, parameter_name, dc_serial, channel, (unsigned char*) p_parameter_value, size_parameter_value, DoubleType);

    if (pSessionParameter == 0x00)
        delete [] p_parameter_value;

    return pSessionParameter;
}

// create string detector parameter
UniDbSessionDcParameter* UniDbSessionDcParameter::CreateSessionDcParameter(int run_number, TString detector_name, TString parameter_name, int dc_serial, int channel, TString parameter_value)
{
    Long_t size_parameter_value = parameter_value.Length()+1;
    char* p_parameter_value = new char[size_parameter_value];
    strcpy(p_parameter_value, parameter_value.Data());

    UniDbSessionDcParameter* pSessionParameter = UniDbSessionDcParameter::CreateSessionDcParameter(run_number, detector_name, parameter_name, dc_serial, channel, (unsigned char*) p_parameter_value, size_parameter_value, StringType);

    if (pSessionParameter == 0x00)
        delete [] p_parameter_value;

    return pSessionParameter;
}

// create Integer Array detector parameter
UniDbSessionDcParameter* UniDbSessionDcParameter::CreateSessionDcParameter(int run_number, TString detector_name, TString parameter_name, int dc_serial, int channel, int* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(int);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbSessionDcParameter* pSessionParameter = UniDbSessionDcParameter::CreateSessionDcParameter(run_number, detector_name, parameter_name, dc_serial, channel, p_parameter_value, size_parameter_value, IntArrayType);

    if (pSessionParameter == 0x00)
        delete [] p_parameter_value;

    return pSessionParameter;
}

// create Double Array detector parameter
UniDbSessionDcParameter* UniDbSessionDcParameter::CreateSessionDcParameter(int run_number, TString detector_name, TString parameter_name, int dc_serial, int channel, double* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(double);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbSessionDcParameter* pSessionParameter = UniDbSessionDcParameter::CreateSessionDcParameter(run_number, detector_name, parameter_name, dc_serial, channel, p_parameter_value, size_parameter_value, DoubleArrayType);

    if (pSessionParameter == 0x00)
        delete [] p_parameter_value;

    return pSessionParameter;
}

// create Int+Int Array detector parameter
UniDbSessionDcParameter* UniDbSessionDcParameter::CreateSessionDcParameter(int run_number, TString detector_name, TString parameter_name, int dc_serial, int channel, IIStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(IIStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbSessionDcParameter* pSessionParameter = UniDbSessionDcParameter::CreateSessionDcParameter(run_number, detector_name, parameter_name, dc_serial, channel, p_parameter_value, size_parameter_value, IIArrayType);

    if (pSessionParameter == 0x00)
        delete [] p_parameter_value;

    return pSessionParameter;
}

// common function for getting parameter
unsigned char* UniDbSessionDcParameter::GetUNC(enumParameterType enum_parameter_type)
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

// get parameter value (bool) for one channel (for current run, detector, TDC/ADC and channel)
bool UniDbSessionDcParameter::GetBool()
{
    return *((bool*) GetUNC(BoolType));
}

// get parameter value (int) for one channel (for current run, detector, TDC/ADC and channel)
int UniDbSessionDcParameter::GetInt()
{
    return *((int*) GetUNC(IntType));
}

// get parameter value (double) for one channel (for current run, detector, TDC/ADC and channel)
double UniDbSessionDcParameter::GetDouble()
{
    return *((double*) GetUNC(DoubleType));
}

// get parameter value (string) for one channel (for current run, detector, TDC/ADC and channel)
TString UniDbSessionDcParameter::GetString()
{
    return (char*) GetUNC(StringType);
}

// get parameter value (Integer Array) for one channel (for current run, detector, TDC/ADC and channel)
int UniDbSessionDcParameter::GetIntArray(int*& parameter_value, int& element_count)
{
    unsigned char* p_parameter_value = GetUNC(IntArrayType);
    if (p_parameter_value == NULL)
        return - 1;

    element_count = sz_parameter_value / sizeof(int);
    parameter_value = new int[element_count];
    memcpy(parameter_value, p_parameter_value, sz_parameter_value);

    return 0;
}

// get parameter value (Double Array) for one channel (for current run, detector, TDC/ADC and channel)
int UniDbSessionDcParameter::GetDoubleArray(double*& parameter_value, int& element_count)
{
    unsigned char* p_parameter_value = GetUNC(DoubleArrayType);
    if (p_parameter_value == NULL)
        return - 1;

    element_count = sz_parameter_value / sizeof(double);
    parameter_value = new double[element_count];
    memcpy(parameter_value, p_parameter_value, sz_parameter_value);

    return 0;
}

// get parameter value (Int+Int Array) for one channel (for current run, detector, TDC/ADC and channel)
int UniDbSessionDcParameter::GetIIArray(IIStructure*& parameter_value, int& element_count)
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
int UniDbSessionDcParameter::SetUNC(unsigned char* p_parameter_value, Long_t size_parameter_value)
{
    if (!connectionUniDb)
    {
        cout<<"Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    TString sql = TString::Format(
        "update session_dc_parameter "
        "set parameter_value = $1 "
        "where session_number = $2 and detector_name = $3 and parameter_id = $4 and dc_serial = $5 and channel = $6");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetLargeObject(0, (void*)p_parameter_value, size_parameter_value);
    stmt->SetInt(1, i_session_number);
    stmt->SetString(2, str_detector_name);
    stmt->SetInt(3, i_parameter_id);
    stmt->SetInt(4, i_dc_serial);
    stmt->SetInt(5, i_channel);

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

// set boolean parameter value for one channel TDC/ADC
int UniDbSessionDcParameter::SetBool(bool parameter_value)
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

// set integer parameter value for one channel TDC/ADC
int UniDbSessionDcParameter::SetInt(int parameter_value)
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

// set double parameter value for one channel TDC/ADC
int UniDbSessionDcParameter::SetDouble(double parameter_value)
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

// set string parameter value for one channel TDC/ADC
int UniDbSessionDcParameter::SetString(TString parameter_value)
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

// set parameter value (Integer Array) for one channel TDC/ADC
int UniDbSessionDcParameter::SetIntArray(int* parameter_value, int element_count)
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

// set parameter value (Double Array) for one channel TDC/ADC
int UniDbSessionDcParameter::SetDoubleArray(double* parameter_value, int element_count)
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

// set Int+Int parameter value for one channel TDC/ADC
int UniDbSessionDcParameter::SetIIArray(IIStructure* parameter_value, int element_count)
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
ClassImp(UniDbSessionDcParameter);
