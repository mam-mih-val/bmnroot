// ----------------------------------------------------------------------
//                    UniDbDetectorParameter cxx file
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"
#include "TObjString.h"

#define ONLY_DECLARATIONS
#include "function_set.h"
#include "db_structures.h"
#include "TSystem.h"
#include "UniDbDetectorParameter.h"

#include <fstream>

/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniDbDetectorParameter::UniDbDetectorParameter(UniConnection* connUniDb, int value_id, TString detector_name, int parameter_id, int start_period, int start_run, int end_period, int end_run, int value_key, unsigned char* parameter_value, Long_t size_parameter_value)
{
	connectionUniDb = connUniDb;

	i_value_id = value_id;
	str_detector_name = detector_name;
	i_parameter_id = parameter_id;
	i_start_period = start_period;
	i_start_run = start_run;
	i_end_period = end_period;
	i_end_run = end_run;
	i_value_key = value_key;
	blob_parameter_value = parameter_value;
	sz_parameter_value = size_parameter_value;
}

// -----   Destructor   -------------------------------------------------
UniDbDetectorParameter::~UniDbDetectorParameter()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (blob_parameter_value)
		delete [] blob_parameter_value;
}

// -----   Creating new detector parameter in the database  ---------------------------
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, int parameter_id, int start_period, int start_run, int end_period, int end_run, int value_key, unsigned char* parameter_value, Long_t size_parameter_value)
{
	UniConnection* connUniDb = UniConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into detector_parameter(detector_name, parameter_id, start_period, start_run, end_period, end_run, value_key, parameter_value) "
		"values ($1, $2, $3, $4, $5, $6, $7, $8)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, detector_name);
	stmt->SetInt(1, parameter_id);
	stmt->SetInt(2, start_period);
	stmt->SetInt(3, start_run);
	stmt->SetInt(4, end_period);
	stmt->SetInt(5, end_run);
	stmt->SetInt(6, value_key);
	stmt->SetBinary(7, parameter_value, size_parameter_value, 0x4000000);

	// inserting new detector parameter to the Database
	if (!stmt->Process())
	{
		cout<<"ERROR: inserting new detector parameter to the Database has been failed"<<endl;
		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	delete stmt;

	// getting last inserted ID
	int value_id;
	TSQLStatement* stmt_last = uni_db->Statement("SELECT currval(pg_get_serial_sequence('detector_parameter','value_id'))");

	// process getting last id
	if (stmt_last->Process())
	{
		// store result of statement in buffer
		stmt_last->StoreResult();

		// if there is no last id then exit with error
		if (!stmt_last->NextResultRow())
		{
			cout<<"ERROR: no last ID in DB!"<<endl;
			delete stmt_last;
			return 0x00;
		}
		else
		{
			value_id = stmt_last->GetInt(0);
			delete stmt_last;
		}
	}
	else
	{
		cout<<"ERROR: getting last ID has been failed!"<<endl;
		delete stmt_last;
		return 0x00;
	}

	int tmp_value_id;
	tmp_value_id = value_id;
	TString tmp_detector_name;
	tmp_detector_name = detector_name;
	int tmp_parameter_id;
	tmp_parameter_id = parameter_id;
	int tmp_start_period;
	tmp_start_period = start_period;
	int tmp_start_run;
	tmp_start_run = start_run;
	int tmp_end_period;
	tmp_end_period = end_period;
	int tmp_end_run;
	tmp_end_run = end_run;
	int tmp_value_key;
	tmp_value_key = value_key;
	unsigned char* tmp_parameter_value;
	Long_t tmp_sz_parameter_value = size_parameter_value;
	tmp_parameter_value = new unsigned char[tmp_sz_parameter_value];
	memcpy(tmp_parameter_value, parameter_value, tmp_sz_parameter_value);

	return new UniDbDetectorParameter(connUniDb, tmp_value_id, tmp_detector_name, tmp_parameter_id, tmp_start_period, tmp_start_run, tmp_end_period, tmp_end_run, tmp_value_key, tmp_parameter_value, tmp_sz_parameter_value);
}

// -----  Get detector parameter from the database  ---------------------------
UniDbDetectorParameter* UniDbDetectorParameter::GetDetectorParameter(int value_id)
{
	UniConnection* connUniDb = UniConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select value_id, detector_name, parameter_id, start_period, start_run, end_period, end_run, value_key, parameter_value "
		"from detector_parameter "
		"where value_id = %d", value_id);
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get detector parameter from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting detector parameter from the database has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// extract row
	if (!stmt->NextResultRow())
	{
		cout<<"ERROR: detector parameter was not found in the database"<<endl;

		delete stmt;
		delete connUniDb;
		return 0x00;
	}

	int tmp_value_id;
	tmp_value_id = stmt->GetInt(0);
	TString tmp_detector_name;
	tmp_detector_name = stmt->GetString(1);
	int tmp_parameter_id;
	tmp_parameter_id = stmt->GetInt(2);
	int tmp_start_period;
	tmp_start_period = stmt->GetInt(3);
	int tmp_start_run;
	tmp_start_run = stmt->GetInt(4);
	int tmp_end_period;
	tmp_end_period = stmt->GetInt(5);
	int tmp_end_run;
	tmp_end_run = stmt->GetInt(6);
	int tmp_value_key;
	tmp_value_key = stmt->GetInt(7);
	unsigned char* tmp_parameter_value;
	tmp_parameter_value = NULL;
	Long_t tmp_sz_parameter_value = 0;
	stmt->GetBinary(8, (void*&)tmp_parameter_value, tmp_sz_parameter_value);

	delete stmt;

	return new UniDbDetectorParameter(connUniDb, tmp_value_id, tmp_detector_name, tmp_parameter_id, tmp_start_period, tmp_start_run, tmp_end_period, tmp_end_run, tmp_value_key, tmp_parameter_value, tmp_sz_parameter_value);
}

// -----  Check detector parameter exists in the database  ---------------------------
bool UniDbDetectorParameter::CheckDetectorParameterExists(int value_id)
{
	UniConnection* connUniDb = UniConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select 1 "
		"from detector_parameter "
		"where value_id = %d", value_id);
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get detector parameter from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting detector parameter from the database has been failed"<<endl;

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

// -----  Delete detector parameter from the database  ---------------------------
int UniDbDetectorParameter::DeleteDetectorParameter(int value_id)
{
	UniConnection* connUniDb = UniConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from detector_parameter "
		"where value_id = $1");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, value_id);

	// delete detector parameter from the dataBase
	if (!stmt->Process())
	{
		cout<<"ERROR: deleting detector parameter from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	delete stmt;
	delete connUniDb;
	return 0;
}

// -----  Print all 'detector parameters'  ---------------------------------
int UniDbDetectorParameter::PrintAll()
{
	UniConnection* connUniDb = UniConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select value_id, detector_name, parameter_id, start_period, start_run, end_period, end_run, value_key, parameter_value "
		"from detector_parameter");
	TSQLStatement* stmt = uni_db->Statement(sql);

	// get all 'detector parameters' from the database
	if (!stmt->Process())
	{
		cout<<"ERROR: getting all 'detector parameters' from the dataBase has been failed"<<endl;

		delete stmt;
		delete connUniDb;
		return -1;
	}

	// store result of statement in buffer
	stmt->StoreResult();

	// print rows
	cout<<"Table 'detector_parameter':"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<"value_id: ";
		cout<<(stmt->GetInt(0));
		cout<<", detector_name: ";
		cout<<(stmt->GetString(1));
		cout<<", parameter_id: ";
		cout<<(stmt->GetInt(2));
		cout<<", start_period: ";
		cout<<(stmt->GetInt(3));
		cout<<", start_run: ";
		cout<<(stmt->GetInt(4));
		cout<<", end_period: ";
		cout<<(stmt->GetInt(5));
		cout<<", end_run: ";
		cout<<(stmt->GetInt(6));
		cout<<", value_key: ";
		cout<<(stmt->GetInt(7));
		cout<<", parameter_value: ";
		unsigned char* tmp_parameter_value = NULL;
		Long_t tmp_sz_parameter_value=0;
		stmt->GetBinary(8, (void*&)tmp_parameter_value, tmp_sz_parameter_value);
		cout<<(void*)tmp_parameter_value<<", binary size: "<<tmp_sz_parameter_value;
		cout<<"."<<endl;
	}

	delete stmt;
	delete connUniDb;

	return 0;
}


// Setters functions
int UniDbDetectorParameter::SetDetectorName(TString detector_name)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_parameter "
		"set detector_name = $1 "
		"where value_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, detector_name);
	stmt->SetInt(1, i_value_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about detector parameter has been failed"<<endl;

		delete stmt;
		return -2;
	}

	str_detector_name = detector_name;

	delete stmt;
	return 0;
}

int UniDbDetectorParameter::SetParameterId(int parameter_id)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_parameter "
		"set parameter_id = $1 "
		"where value_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, parameter_id);
	stmt->SetInt(1, i_value_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about detector parameter has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_parameter_id = parameter_id;

	delete stmt;
	return 0;
}

int UniDbDetectorParameter::SetStartPeriod(int start_period)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_parameter "
		"set start_period = $1 "
		"where value_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, start_period);
	stmt->SetInt(1, i_value_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about detector parameter has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_start_period = start_period;

	delete stmt;
	return 0;
}

int UniDbDetectorParameter::SetStartRun(int start_run)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_parameter "
		"set start_run = $1 "
		"where value_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, start_run);
	stmt->SetInt(1, i_value_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about detector parameter has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_start_run = start_run;

	delete stmt;
	return 0;
}

int UniDbDetectorParameter::SetEndPeriod(int end_period)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_parameter "
		"set end_period = $1 "
		"where value_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, end_period);
	stmt->SetInt(1, i_value_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about detector parameter has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_end_period = end_period;

	delete stmt;
	return 0;
}

int UniDbDetectorParameter::SetEndRun(int end_run)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_parameter "
		"set end_run = $1 "
		"where value_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, end_run);
	stmt->SetInt(1, i_value_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about detector parameter has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_end_run = end_run;

	delete stmt;
	return 0;
}

int UniDbDetectorParameter::SetValueKey(int value_key)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_parameter "
		"set value_key = $1 "
		"where value_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, value_key);
	stmt->SetInt(1, i_value_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about detector parameter has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_value_key = value_key;

	delete stmt;
	return 0;
}

int UniDbDetectorParameter::SetParameterValue(unsigned char* parameter_value, Long_t size_parameter_value)
{
	if (!connectionUniDb)
	{
		cout<<"CRITICAL ERROR: Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_parameter "
		"set parameter_value = $1 "
		"where value_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetBinary(0, parameter_value, size_parameter_value, 0x4000000);
	stmt->SetInt(1, i_value_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about detector parameter has been failed"<<endl;

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

// -----  Print current detector parameter  ---------------------------------------
void UniDbDetectorParameter::Print()
{
	cout<<"Table 'detector_parameter'";
	cout<<". value_id: "<<i_value_id<<". detector_name: "<<str_detector_name<<". parameter_id: "<<i_parameter_id<<". start_period: "<<i_start_period<<". start_run: "<<i_start_run<<". end_period: "<<i_end_period<<". end_run: "<<i_end_run<<". value_key: "<<i_value_key<<". parameter_value: "<<(void*)blob_parameter_value<<", binary size: "<<sz_parameter_value<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */


// non-user function for getting parameter value as a binary (char) array
unsigned char* UniDbDetectorParameter::GetUNC(enumValueType enum_parameter_type)
{
    if (enum_parameter_type > -1)
    {
        if (!connectionUniDb)
        {
            cout<<"CRITICAL ERROR: Connection object is null"<<endl;
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
            cout<<"CRITICAL ERROR: getting record with parameter from 'parameter_' table has been failed"<<endl;
            delete stmt;
            return NULL;
        }

        stmt->StoreResult();

        // extract row with parameter
        if (!stmt->NextResultRow())
        {
            cout<<"CRITICAL ERROR: the parameter with id '"<<i_parameter_id<<"' was not found"<<endl;
            delete stmt;
            return NULL;
        }

        TString parameter_name = stmt->GetString(0);
        int parameter_type = stmt->GetInt(1);
        delete stmt;

        if (parameter_type != enum_parameter_type)
        {
            cout<<"CRITICAL ERROR: the parameter with name '"<<parameter_name<<"' is not corresponding the given type: "
                  "Database Type - "<<parameter_type<<", but user type - "<<enum_parameter_type<<endl;
            return NULL;
        }
    }

    return blob_parameter_value;
}

// non-user function for setting parameter value as a binary (char) array
int UniDbDetectorParameter::SetUNC(unsigned char* p_parameter_value, Long_t size_parameter_value)
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
        "where detector_name = $2 and parameter_id = $3 and start_period = $4 and start_run = $5 and end_period = $6 and end_run = $7 and value_key = $8");

    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetBinary(0, (void*)p_parameter_value, size_parameter_value);
    stmt->SetString(1, str_detector_name);
    stmt->SetInt(2, i_parameter_id);
    stmt->SetInt(3, i_start_period);
    stmt->SetInt(4, i_start_run);
    stmt->SetInt(5, i_end_period);
    stmt->SetInt(6, i_end_run);
    stmt->SetInt(7, i_value_key);

    // write new value to database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating the detector parameter has been failed"<<endl;
        delete stmt;
        return -2;
    }

    if (blob_parameter_value) delete [] blob_parameter_value;
    blob_parameter_value = p_parameter_value;
    sz_parameter_value = size_parameter_value;

    delete stmt;
    return 0;
}

// non-user function for writing parameter value (integer value key is optional, default, 0)
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                              unsigned char* p_parameter_value, Long_t size_parameter_value, enumValueType enum_parameter_type, int value_key)
{
    // check if postgresql bug with SetBinary has been patched
    TString patch_check("$SIMPATH/pgsql_patched");
    gSystem->ExpandPathName(patch_check);
    if (gSystem->AccessPathName(patch_check))
    {
        cout<<"ERROR: TPgSQLStatement (CERN ROOT) was not patched to write binary data into the Unified Database"<<endl;
        return 0x00;
    }

    if (((end_period < start_period) or ((end_period == start_period) and (end_run < start_run))) or ((start_period > end_period) or ((start_period == end_period) and (start_run > end_run))))
    {
        cout<<"ERROR: end run should be after or the same as start run"<<endl;
        return 0x00;
    }

    UniConnection* connUniDb = UniConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
        return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    int parameter_id = -1;
    bool res_code = UniDbParameter::CheckAndGetParameterID(uni_db, parameter_name, enum_parameter_type, parameter_id);
    if (!res_code)
    {
        delete connUniDb;
        return 0x00;
    }

    TString sql = TString::Format(
        "insert into detector_parameter(detector_name, parameter_id, start_period, start_run, end_period, end_run, value_key, parameter_value) "
        "values ($1, $2, $3, $4, $5, $6, $7, $8)");
    TSQLStatement* stmt = uni_db->Statement(sql);

    //cout<<"DEBUG: detector_name = "<<detector_name<<". parameter_id = "<<parameter_id<<". start_period = "<<start_period<<". start_run = "<<start_run<<". end_period = "<<end_period<<". end_run = "<<end_run<<". value_key = "<<value_key<<". p_parameter_value[0] = "<<p_parameter_value[0]<<". size_parameter_value = "<<size_parameter_value<<endl;
    stmt->NextIteration();
    stmt->SetString(0, detector_name);
    stmt->SetInt(1, parameter_id);
    stmt->SetInt(2, start_period);
    stmt->SetInt(3, start_run);
    stmt->SetInt(4, end_period);
    stmt->SetInt(5, end_run);
    stmt->SetInt(6, value_key);
    stmt->SetBinary(7, (void*)p_parameter_value, size_parameter_value);

    // inserting new record to DB
    if (!stmt->Process())
    {
        cout<<"ERROR: inserting new parameter value to the database has been failed"<<endl;
        delete stmt;
        delete connUniDb;
        return 0x00;
    }

    delete stmt;

    // getting last inserted node ID
    int last_id = -1;
    //TSQLStatement* stmt_last = uni_db->Statement("SELECT LAST_INSERT_ID()");  //MySQL
    TSQLStatement* stmt_last = uni_db->Statement("SELECT currval(pg_get_serial_sequence('detector_parameter','value_id'))");

    if (stmt_last->Process())
    {
        // store result of statement in buffer
        stmt_last->StoreResult();

        // if there is no last id then exit with error
        if (!stmt_last->NextResultRow())
        {
            cout<<"ERROR: no last ID in the Database for the parameter value has been found!"<<endl;
            delete stmt_last;
            return 0x00;
        }
        else
        {
            last_id = stmt_last->GetInt(0);
            delete stmt_last;
        }
    }
    else
    {
        cout<<"ERROR: getting last ID for new parameter value has been failed!"<<endl;
        delete stmt_last;
        return 0x00;
    }

    //unsigned char* tmp_parameter_value = new unsigned char[size_parameter_value];
    //memcpy(tmp_parameter_value, p_parameter_value, size_parameter_value);
    return new UniDbDetectorParameter(connUniDb, last_id, detector_name, parameter_id, start_period, start_run, end_period, end_run, value_key, p_parameter_value, size_parameter_value);
}

// write detector parameter value presented by an array (integer value key is optional, default, 0)
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                              vector<UniValue*> parameter_value, int value_key)
{
    if (parameter_value.size() < 1)
    {
        cout<<"ERROR: The length of the parameter array should be greater zero"<<endl;
        return NULL;
    }
    enumValueType parameter_type = parameter_value[0]->GetType();

    // calculate total size
    size_t size_parameter_value = 0;
    for (int i = 0; i < parameter_value.size(); i++)
    {
        if (parameter_value[i]->GetType() != parameter_type)
        {
            cout<<"ERROR: the type of the parameters in the array must be the same"<<endl;
            return NULL;
        }
        size_parameter_value += parameter_value[i]->GetStorageSize();
    }

    // write parameters of the vector to the buffer
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    unsigned char* tmp_pointer = p_parameter_value;
    for (int i = 0; i < parameter_value.size(); i++)
    {
        parameter_value[i]->WriteValue(tmp_pointer);
        tmp_pointer += parameter_value[i]->GetStorageSize();
    }

    UniDbDetectorParameter* pDetectorParameter =
            UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                              p_parameter_value, size_parameter_value, parameter_type, value_key);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// write detector parameter value presented by a single value (integer value key is optional, default, 0)
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                              UniValue* parameter_value, int value_key)
{
    if (parameter_value == NULL)
    {
        cout<<"ERROR: the parameter value must not be NULL"<<endl;
        return NULL;
    }
    enumValueType parameter_type = parameter_value->GetType();

    // write the parameter to the buffer
    size_t size_parameter_value = parameter_value->GetStorageSize();
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    parameter_value->WriteValue(p_parameter_value);

    UniDbDetectorParameter* pDetectorParameter =
            UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                               p_parameter_value, size_parameter_value, parameter_type, value_key);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// get detector parameter value (integer value key is optional, default, 0)
UniDbDetectorParameter* UniDbDetectorParameter::GetDetectorParameter(TString detector_name, TString parameter_name, int period_number, int run_number, int value_key)
{
    UniConnection* connUniDb = UniConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
        return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
        "select value_id, detector_name, p.parameter_id, start_period, start_run, end_period, end_run, parameter_value "
        "from detector_parameter dp join parameter_ p on dp.parameter_id = p.parameter_id "
        "where lower(detector_name) = lower('%s') and lower(parameter_name) = lower('%s') and "
        "(not (((%d < start_period) or ((%d = start_period) and (%d < start_run))) or ((%d > end_period) or ((%d = end_period) and (%d > end_run))))) and "
        "value_key = %d",
        detector_name.Data(), parameter_name.Data(), period_number, period_number, run_number, period_number, period_number, run_number, value_key);
    TSQLStatement* stmt = uni_db->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"ERROR: getting record from the Database has been failed"<<endl;
        delete stmt;
        delete connUniDb;
        return 0x00;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // extract row
    if (!stmt->NextResultRow())
    {
        cout<<"ERROR: parameter value was not found"<<endl;
        delete stmt;
        delete connUniDb;
        return 0x00;
    }

    int tmp_value_id;
    tmp_value_id = stmt->GetInt(0);
    TString tmp_detector_name;
    tmp_detector_name = stmt->GetString(1);
    int tmp_parameter_id;
    tmp_parameter_id = stmt->GetInt(2);
    int tmp_start_period;
    tmp_start_period = stmt->GetInt(3);
    int tmp_start_run;
    tmp_start_run = stmt->GetInt(4);
    int tmp_end_period;
    tmp_end_period = stmt->GetInt(5);
    int tmp_end_run;
    tmp_end_run = stmt->GetInt(6);
    unsigned char* tmp_parameter_value = NULL;
    Long_t tmp_sz_parameter_value = 0;
    stmt->GetBinary(7, (void*&)tmp_parameter_value, tmp_sz_parameter_value);

    delete stmt;

    return new UniDbDetectorParameter(connUniDb, tmp_value_id, tmp_detector_name, tmp_parameter_id, tmp_start_period, tmp_start_run, tmp_end_period, tmp_end_run,
                                         value_key, tmp_parameter_value, tmp_sz_parameter_value);
}

// delete detector parameter value (integer value key is optional, default, 0)
int UniDbDetectorParameter::DeleteDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run, int value_key)
{
    UniConnection* connUniDb = UniConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00) return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
        "delete from detector_parameter "
        "where lower(detector_name) = lower($1) and parameter_id IN (select parameter_id from parameter_ where lower(parameter_name) = lower($2)) and start_period = $3 and start_run = $4 and end_period = $5 and end_run = $6 and value_key = $7");
    //cout<<"UniDbDetectorParameter::DeleteDetectorParameter: SQL code = "<<sql<<endl;
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetString(0, detector_name);
    stmt->SetString(1, parameter_name);
    stmt->SetInt(2, start_period);
    stmt->SetInt(3, start_run);
    stmt->SetInt(4, end_period);
    stmt->SetInt(5, end_run);
    stmt->SetInt(6, value_key);

    // delete table record from DB
    if (!stmt->Process())
    {
        cout<<"ERROR: deleting parameter value from the Database has been failed"<<endl;

        delete stmt;
        delete connUniDb;
        return -1;
    }

    if (stmt->GetNumAffectedRows() == 0)
    {
        cout<<"WARNING: parameter value was not found for deleting"<<endl;
        delete stmt;
        delete connUniDb;
        return -2;
    }

    delete stmt;
    delete connUniDb;
    return 0;
}

// get value of detector parameter presented by an array
int UniDbDetectorParameter::GetValue(vector<UniValue*>& parameter_value)
{
    unsigned char* p_parameter_value = GetUNC();
    if (p_parameter_value == NULL)
        return -1;

    Long_t full_size = sz_parameter_value;
    while (full_size > 0)
    {
        //cout<<"full_size = "<<full_size<<". GetParameterType() = "<<GetParameterType()<<endl;
        UniValue* pParameterValue = CreateParameterValue(GetParameterType());
        pParameterValue->ReadValue(p_parameter_value);
        full_size -= pParameterValue->GetStorageSize();
        p_parameter_value += pParameterValue->GetStorageSize();
        parameter_value.push_back(pParameterValue);
    }

    return 0;
}

// get value of detector parameter presented by a single value
UniValue* UniDbDetectorParameter::GetValue()
{
    unsigned char* p_parameter_value = GetUNC();
    if (p_parameter_value == NULL)
        return NULL;

    UniValue* parameter_value = CreateParameterValue(GetParameterType());
    parameter_value->ReadValue(p_parameter_value);

    return parameter_value;
}

// set value to detector parameter presented by an array
int UniDbDetectorParameter::SetValue(vector<UniValue*> parameter_value)
{
    if (parameter_value.size() < 1)
    {
        cout<<"ERROR: The length of the parameter array should be greater zero"<<endl;
        return -1;
    }

    // calculate full size to create a 'char' buffer
    size_t size_parameter_value = 0;
    for (int i = 0; i < parameter_value.size(); i++)
        size_parameter_value += parameter_value[i]->GetStorageSize();
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];

    // write value by value to the 'char' buffer
    unsigned char* tmp_pointer = p_parameter_value;
    for (int i = 0; i < parameter_value.size(); i++)
    {
        parameter_value[i]->WriteValue(tmp_pointer);
        tmp_pointer += parameter_value[i]->GetStorageSize();
    }

    int res_code = SetUNC(p_parameter_value, size_parameter_value);
    if (res_code != 0)
    {
        delete [] p_parameter_value;
        return res_code;
    }

    return 0;
}

// set value to detector parameter presented by a single value
int UniDbDetectorParameter::SetValue(UniValue* parameter_value)
{
    if (parameter_value == NULL)
    {
        cout<<"ERROR: the parameter value must not be NULL"<<endl;
        return -1;
    }

    // write value to the 'char' buffer
    size_t size_parameter_value = parameter_value->GetStorageSize();
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    parameter_value->WriteValue(p_parameter_value);

    int res_code = SetUNC(p_parameter_value, size_parameter_value);
    if (res_code != 0)
    {
        delete [] p_parameter_value;
        return res_code;
    }

    return 0;
}

// write a whole file with parameter value (with given detector and parameter names) to the Database from start run to end one
int UniDbDetectorParameter::WriteFile(const char* detector_name, const char* parameter_name, int start_period, int start_run, int end_period, int end_run, const char* file_path)
{
    TString strFilePath(file_path);
    gSystem->ExpandPathName(strFilePath);

    FILE* cur_file = fopen(strFilePath.Data(), "rb");
    if (cur_file == NULL)
    {
        cout<<"ERROR: opening file: "<<strFilePath<<" was failed"<<endl;
        return -1;
    }

    fseek(cur_file, 0, SEEK_END);
    size_t file_size = ftell(cur_file);
    rewind(cur_file);
    if (file_size <= 0)
    {
        cout<<"ERROR: getting file size: "<<strFilePath<<" was failed"<<endl;
        fclose(cur_file);
        return -2;
    }

    unsigned char* buffer = new unsigned char[file_size];
    if (buffer == NULL)
    {
        cout<<"ERROR: getting memory from heap for file was failed"<<endl;
        fclose(cur_file);
        return -3;
    }

    size_t bytes_read = fread(buffer, 1, file_size, cur_file);
    if (bytes_read != file_size)
    {
        cout<<"ERROR: reading file: "<<strFilePath<<", got "<<bytes_read<<" bytes of "<<file_size<<endl;
        delete [] buffer;
        fclose(cur_file);
        return -4;
    }

    fclose(cur_file);

    // create binary parameter with file's bytes
    BinaryValue* pValue = new BinaryValue;
    pValue->size = file_size;
    pValue->value = buffer;
    UniDbDetectorParameter* pParameterValue = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, pValue);
    if (pParameterValue == NULL)
    {
        delete [] buffer;
        return -5;
    }
    delete pValue;

    delete [] buffer;
    delete pParameterValue;

    return 0;
}

// read a whole file with parameter value (with given detector and parameter names) from the Database for the selected run
int UniDbDetectorParameter::ReadFile(const char* detector_name, const char* parameter_name, int period_number, int run_number, const char* file_path)
{
    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::GetDetectorParameter(detector_name, parameter_name, period_number, run_number);
    if (pDetectorParameter == NULL)
        return -1;
    BinaryValue* pValue = (BinaryValue*) pDetectorParameter->GetValue();

    FILE* cur_file = fopen(file_path, "wb");
    if (cur_file == NULL)
    {
        cout<<"ERROR: creating file: "<<file_path<<" has been failed!"<<endl;
        delete pValue;
        delete pDetectorParameter;
        return -2;
    }

    unsigned char* buffer = pValue->value;
    if (buffer == NULL)
    {
        cout<<"ERROR: getting binary parameter array"<<endl;
        delete pValue;
        delete pDetectorParameter;
        fclose(cur_file);
        return -3;
    }
    size_t parameter_size = pValue->size;

    size_t bytes_write = fwrite(buffer, 1, parameter_size, cur_file);
    delete pDetectorParameter;
    if (bytes_write != parameter_size)
    {
        cout<<"ERROR: writing file: "<<file_path<<", put "<<bytes_write<<" bytes of "<<parameter_size<<endl;
        fclose(cur_file);
        delete pValue;
        return -4;
    }

    fclose(cur_file);
    delete[] buffer;
    delete pValue;

    return 0;
}

TObjArray* UniDbDetectorParameter::Search(TObjArray& search_conditions)
{
    TObjArray* arrayResult = NULL;
    search_conditions.SetOwner(kTRUE);

    UniConnection* connUniDb = UniConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
    {
        cout<<"ERROR: the connection to the Unified Database was failed"<<endl;
        return arrayResult;
    }

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
                "select value_id, detector_name, p.parameter_id, start_period, start_run, end_period, end_run, value_key, parameter_value "
                "from detector_parameter dp join parameter_ p on dp.parameter_id = p.parameter_id");

    TString strCondition;
    bool isFirst = true;
    TIter next(&search_conditions);
    UniSearchCondition* curCondition;
    while (curCondition = (UniSearchCondition*) next())
    {
        strCondition = "";

        switch (curCondition->GetColumn())
        {
            case columnDetectorName:    strCondition += "lower(detector_name) "; break;
            case columnParameterName:   strCondition += "lower(parameter_name) "; break;
            case columnStartPeriod:     strCondition += "start_period "; break;
            case columnStartRun:        strCondition += "start_run "; break;
            case columnEndPeriod:       strCondition += "end_period "; break;
            case columnEndRun:          strCondition += "end_run "; break;
            default:
                cout<<"ERROR: the column in the search condition was not defined, the condition is skipped"<<endl;
                continue;
        }

        switch (curCondition->GetCondition())
        {
            case conditionLess:             strCondition += "< "; break;
            case conditionLessOrEqual:      strCondition += "<= "; break;
            case conditionEqual:            strCondition += "= "; break;
            case conditionNotEqual:         strCondition += "<> "; break;
            case conditionGreater:          strCondition += "> "; break;
            case conditionGreaterOrEqual:   strCondition += ">= "; break;
            case conditionLike:             strCondition += "like "; break;
            case conditionNull:             strCondition += "is null "; break;
            case conditionNotNull:          strCondition += "is not null "; break;
            default:
                cout<<"ERROR: the comparison operator in the search condition was not defined, the condition is skipped"<<endl;
                continue;
        }

        switch (curCondition->GetValueType())
        {
            case 0:
                if ((curCondition->GetCondition() != conditionNull) && (curCondition->GetCondition() != conditionNotNull)) continue;
                break;
            case 1: strCondition += Form("%d", curCondition->GetIntValue()); break;
            case 2: strCondition += Form("%u", curCondition->GetUIntValue()); break;
            case 3: strCondition += Form("%f", curCondition->GetDoubleValue()); break;
            case 4: strCondition += Form("lower('%s')", curCondition->GetStringValue().Data()); break;
            case 5: strCondition += Form("'%s'", curCondition->GetDatimeValue().AsSQLString()); break;
            default:
                cout<<"ERROR: the value type in the search condition was not found, the condition is skipped"<<endl;
                continue;
        }

        if (isFirst)
        {
            sql += " where ";
            isFirst = false;
        }
        else
            sql += " and ";

        sql += strCondition;
    }

    TSQLStatement* stmt = uni_db->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"ERROR: getting runs from the Unified Database has been failed"<<endl;
        delete stmt;
        delete connUniDb;

        return arrayResult;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // extract rows one after another
    arrayResult = new TObjArray();
    arrayResult->SetOwner(kTRUE);
    while (stmt->NextResultRow())
    {
        UniConnection* connPar = UniConnection::Open(UNIFIED_DB);
        if (connPar == 0x00)
        {
            cout<<"ERROR: the connection to the Unified Database for the selected run was failed"<<endl;
            return arrayResult;
        }

        int tmp_value_id;
        tmp_value_id = stmt->GetInt(0);
        TString tmp_detector_name;
        tmp_detector_name = stmt->GetString(1);
        int tmp_parameter_id;
        tmp_parameter_id = stmt->GetInt(2);
        int tmp_start_period;
        tmp_start_period = stmt->GetInt(3);
        int tmp_start_run;
        tmp_start_run = stmt->GetInt(4);
        int tmp_end_period;
        tmp_end_period = stmt->GetInt(5);
        int tmp_end_run;
        tmp_end_run = stmt->GetInt(6);
        int tmp_value_key;
        tmp_value_key = stmt->GetInt(7);
        unsigned char* tmp_parameter_value;
        tmp_parameter_value = NULL;
        Long_t tmp_sz_parameter_value = 0;
        stmt->GetBinary(8, (void*&)tmp_parameter_value, tmp_sz_parameter_value);

        arrayResult->Add((TObject*) new UniDbDetectorParameter(connPar, tmp_value_id, tmp_detector_name, tmp_parameter_id, tmp_start_period, tmp_start_run, tmp_end_period, tmp_end_run,
                                                                  tmp_value_key, tmp_parameter_value, tmp_sz_parameter_value));
    }

    delete stmt;

    arrayResult->SetOwner(kTRUE);
    return arrayResult;
}

TObjArray* UniDbDetectorParameter::Search(UniSearchCondition& search_condition)
{
    TObjArray search_conditions;
    search_conditions.Add((TObject*)&search_condition);

    return Search(search_conditions);
}

// get parameter name of the current detector parameter
TString UniDbDetectorParameter::GetParameterName()
{
    UniDbParameter* pCurParameter = UniDbParameter::GetParameter(i_parameter_id);
    if (pCurParameter == NULL)
    {
        cout<<"ERROR: parameter with current ID was not found"<<endl;
        return "";
    }

    TString par_name = pCurParameter->GetParameterName();

    delete pCurParameter;
    return par_name;
}

// get parameter type of the current detector parameter
enumValueType UniDbDetectorParameter::GetParameterType()
{
    UniDbParameter* pCurParameter = UniDbParameter::GetParameter(i_parameter_id);
    if (pCurParameter == NULL)
    {
        cout<<"ERROR: parameter with current ID was not found"<<endl;
        return UndefinedType;
    }

    enumValueType par_type = (enumValueType) pCurParameter->GetParameterType();

    delete pCurParameter;
    return par_type;
}

// get start period and run of the current detector parameter
void UniDbDetectorParameter::GetStart(int& start_period, int& start_run)
{
    start_period = i_start_period;
    start_run = i_start_run;

    return;
}

// get end period and run of the current detector parameter
void UniDbDetectorParameter::GetEnd(int& end_period, int& end_run)
{
    end_period = i_end_period;
    end_run = i_end_run;

    return;
}

// set start period and run of the current detector parameter
int UniDbDetectorParameter::SetStart(int start_period, int start_run)
{
    if (!connectionUniDb)
    {
        cout<<"ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    TString sql = TString::Format(
        "update detector_parameter "
        "set start_period = $1, start_run = $2"
        "where value_id = $3");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, start_period);
    stmt->SetInt(1, start_run);
    stmt->SetInt(2, i_value_id);

    // write new value to database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating the record has been failed"<<endl;

        delete stmt;
        return -2;
    }

    i_start_period = start_period;
    i_start_run = start_run;

    delete stmt;
    return 0;
}

// set end period and run of the current detector parameter
int UniDbDetectorParameter::SetEnd(int end_period, int end_run)
{
    if (!connectionUniDb)
    {
        cout<<"ERROR: Connection object is null"<<endl;
        return -1;
    }

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    TString sql = TString::Format(
        "update detector_parameter "
        "set end_period = $1, end_run = $2"
        "where value_id = $3");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetInt(0, end_period);
    stmt->SetInt(1, end_run);
    stmt->SetInt(2, i_value_id);

    // write new value to database
    if (!stmt->Process())
    {
        cout<<"ERROR: updating the record has been failed"<<endl;

        delete stmt;
        return -2;
    }

    i_end_period = end_period;
    i_end_run = end_run;

    delete stmt;
    return 0;
}

/*
/// parse detector parameter's values, function returns row count added to the database
int UniDbDetectorParameter::ParseTxt(TString text_file, TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run, bool isSerialChannel)
{
    ifstream txtFile;
    txtFile.open(text_file, ios::in);
    if (!txtFile.is_open())
    {
        cout<<"Error: opening TXT file '"<<text_file<<"' was failed"<<endl;
        return -1;
    }

    // open connection to database
    UniConnection* connUniDb = UniConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
        return -3;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    string cur_line;
    while (getline(txtFile, cur_line))
    {
        //remove duplicates of whitespaces and tabs
        TString reduce_line = reduce(cur_line);

        if (reduce_line[0] == '#')
            continue;

        //stringstream ss;
        //ss.str(reduce_line);

        // get detector object from 'detector_' table
        TString sql = TString::Format(
                    "select detector_name "
                    "from detector_ "
                    "where lower(detector_name) = lower('%s'')", detector_name.Data());
        TSQLStatement* stmt = uni_db->Statement(sql);

        // get table record from DB
        if (!stmt->Process())
        {
            cout<<"Error: getting record with detector from 'detector_' table has been failed"<<endl;
            delete stmt;
            return -4;
        }
        stmt->StoreResult();

        // check detector with the given name is exist
        if (!stmt->NextResultRow())
        {
            cout<<"Error: the detector with name '"<<detector_name<<"' was not found"<<endl;
            delete stmt;
            return -5;
        }

        detector_name = stmt->GetString(0);

        delete stmt;

        // get parameter object from 'parameter_' table
        sql = TString::Format(
                    "select parameter_id, parameter_name, parameter_type "
                    "from parameter_ "
                    "where lower(parameter_name) = lower('%s')", parameter_name.Data());
        stmt = uni_db->Statement(sql);

        // get table record from DB
        if (!stmt->Process())
        {
            cout<<"Error: getting record with parameter from 'parameter_' table has been failed"<<endl;
            delete stmt;
            return -6;
        }

        stmt->StoreResult();

        // extract row with parameter
        if (!stmt->NextResultRow())
        {
            cout<<"Error: the parameter with name '"<<parameter_name<<"' was not found"<<endl;
            delete stmt;
            return -7;
        }

        //int parameter_id = stmt->GetInt(0);
        parameter_name = stmt->GetString(1);
        int parameter_type = stmt->GetInt(2);

        delete stmt;

        TObjArray* tokens = reduce_line.Tokenize("\t ");
        TObjString* item;
        int value_count = tokens->GetEntriesFast();

        UniDbDetectorParameter* pDetectorParameter = NULL;
        // choose parsing method for parameter's value according it's type
        int serial_number = -1, channel_number = -1, cur_num_item = 0;
        switch (parameter_type)
        {
            case BoolType:
                {
                    int count_required = 1;
                    if (isSerialChannel) count_required++, count_required++;

                    if (value_count != count_required)
                    {
                        cout<<"Warning: the count of parameters is not equal 1 or 3 ([serial] [channel] value)"<<endl<<"The current line will be skipped!"<<endl;
                        tokens->Delete();
                        continue;
                    }
                    if (isSerialChannel)
                    {
                        // get serial number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        serial_number = item->GetString().Atoi();
                        // get channel number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        channel_number = item->GetString().Atoi();
                    }
                    item = (TObjString*) tokens->At(cur_num_item++);
                    tokens->Delete();

                    bool value = (bool) item->GetString().Atoi();
                    if (isSerialChannel)
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, serial_number, channel_number, value);
                    else
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, value);

                    break;
                }
            case IntType:
                {
                    int count_required = 1;
                    if (isSerialChannel) count_required++, count_required++;

                    if (value_count != count_required)
                    {
                        cout<<"Warning: the count of parameters is not equal 1 or 3 ([serial] [channel] value)"<<endl<<"The current line will be skipped!"<<endl;
                        tokens->Delete();
                        continue;
                    }
                    if (isSerialChannel)
                    {
                        // get serial number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        serial_number = item->GetString().Atoi();
                        // get channel number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        channel_number = item->GetString().Atoi();
                    }
                    item = (TObjString*) tokens->At(cur_num_item++);
                    tokens->Delete();

                    int value = item->GetString().Atoi();
                    if (value == 0)
                    {
                        if (!item->GetString().IsDigit())
                        {
                            cout<<"Error: the parameter value is not integer: '"<<item<<"'"<<endl;
                            continue;
                        }
                    }

                    if (isSerialChannel)
                       pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, serial_number, channel_number, value);
                    else
                       pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, value);

                    break;
                }
            case DoubleType:
                {
                    int count_required = 1;
                    if (isSerialChannel) count_required++, count_required++;

                    if (value_count != count_required)
                    {
                        cout<<"Warning: the count of parameters is not equal 1 or 3 ([serial] [channel] value)"<<endl<<"The current line will be skipped!"<<endl;
                        tokens->Delete();
                        continue;
                    }
                    if (isSerialChannel)
                    {
                        // get serial number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        serial_number = item->GetString().Atoi();
                        // get channel number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        channel_number = item->GetString().Atoi();
                    }
                    item = (TObjString*) tokens->At(cur_num_item++);
                    tokens->Delete();

                    double value = item->GetString().Atof();
                    if (isSerialChannel)
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, serial_number, channel_number, value);
                    else
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, value);

                    break;
                }
            case IIArrayType:
                {
                    int count_required = 2, size_arr = value_count/2;
                    if (isSerialChannel)
                    {
                        count_required++, count_required++;
                        size_arr--;
                    }

                    if ((value_count < count_required) || ((value_count % 2) != 0))
                    {
                        cout<<"Warning: the count of parameters should be greater 2 or 4 ([serial] [channel] values@2) and should be even"<<endl<<"The current line will be skipped!"<<endl;
                        tokens->Delete();
                        continue;
                    }
                    if (isSerialChannel)
                    {
                        // get serial number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        serial_number = item->GetString().Atoi();
                        // get channel number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        channel_number = item->GetString().Atoi();
                    }

                    int iter = 0, num = count_required-2;
                    IIStructure* pValues = new IIStructure[size_arr];
                    for (; iter < size_arr; iter++)
                    {
                        pValues[iter].int_1 = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                        pValues[iter].int_2 = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                    }
                    tokens->Delete();

                    if (isSerialChannel)
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, serial_number, channel_number, pValues, size_arr);
                    else
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, pValues, size_arr);

                    delete [] pValues;
                    break;
                }
            case IntArrayType:
                {
                    int count_required = 1, size_arr = value_count;
                    if (isSerialChannel)
                    {
                        count_required++, count_required++;
                        size_arr--, size_arr--;
                    }

                    if (value_count < count_required)
                    {
                        cout<<"Warning: the count of parameters should be greater 1 or 3 ([serial] [channel] value1 value2...)"<<endl<<"The current line will be skipped!"<<endl;
                        tokens->Delete();
                        continue;
                    }
                    if (isSerialChannel)
                    {
                        // get serial number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        serial_number = item->GetString().Atoi();
                        // get channel number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        channel_number = item->GetString().Atoi();
                    }

                    int iter = 0;
                    int* pValues = new int[size_arr];
                    for (int num = count_required-1; num < value_count; num++,iter++)
                        pValues[iter] = ((TObjString*) tokens->At(num))->GetString().Atoi();
                    tokens->Delete();

                    if (isSerialChannel)
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, serial_number, channel_number, pValues, size_arr);
                    else
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, pValues, size_arr);

                    delete [] pValues;
                    break;
                }
            case DoubleArrayType:
                {
                    int count_required = 1, size_arr = value_count;
                    if (isSerialChannel)
                    {
                        count_required++, count_required++;
                        size_arr--, size_arr--;
                    }

                    if (value_count < count_required)
                    {
                        cout<<"Warning: the count of parameters should be greater 1 or 3 ([serial] [channel] value1 value2...)"<<endl<<"The current line will be skipped!"<<endl;
                        tokens->Delete();
                        continue;
                    }
                    if (isSerialChannel)
                    {
                        // get serial number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        serial_number = item->GetString().Atoi();
                        // get channel number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        channel_number = item->GetString().Atoi();
                    }

                    int iter = 0;
                    double* pValues = new double[size_arr];
                    for (int num = count_required-1; num < value_count; num++, iter++)
                        pValues[iter] = ((TObjString*) tokens->At(num))->GetString().Atof();
                    tokens->Delete();

                    if (isSerialChannel)
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, serial_number, channel_number, pValues, size_arr);
                    else
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, pValues, size_arr);

                    delete [] pValues;
                    break;
                }
            case UIntArrayType:
                {
                    int count_required = 1, size_arr = value_count;
                    if (isSerialChannel)
                    {
                        count_required++, count_required++;
                        size_arr--, size_arr--;
                    }

                    if (value_count < count_required)
                    {
                        cout<<"Warning: the count of parameters should be greater 1 or 3 ([serial] [channel] value1 value2...)"<<endl<<"The current line will be skipped!"<<endl;
                        tokens->Delete();
                        continue;
                    }
                    if (isSerialChannel)
                    {
                        // get serial number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        serial_number = item->GetString().Atoi();
                        // get channel number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        channel_number = item->GetString().Atoi();
                    }

                    int iter = 0;
                    unsigned int* pValues = new unsigned int[size_arr];
                    for (int num = count_required-1; num < value_count; num++, iter++)
                        pValues[iter] = (unsigned int) ((TObjString*) tokens->At(num))->GetString().Atoll();
                    tokens->Delete();

                    if (isSerialChannel)
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, serial_number, channel_number, pValues, size_arr);
                    else
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, pValues, size_arr);

                    delete [] pValues;
                    break;
                }
            case DchMapArrayType:
                {
                    int count_required = 6, size_arr = value_count/6;
                    if (isSerialChannel)
                        count_required++, count_required++;

                    if ((value_count < count_required) || (((value_count-count_required+6) % 6) != 0))
                    {
                        cout<<"Warning: the count of parameters should be greater 6 or 8 ([serial] [channel] values@6) and must be divisible by 6"<<endl<<"The current line will be skipped!"<<endl;
                        tokens->Delete();
                        continue;
                    }
                    if (isSerialChannel)
                    {
                        // get serial number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        serial_number = item->GetString().Atoi();
                        // get channel number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        channel_number = item->GetString().Atoi();
                    }

                    int iter = 0, num = count_required-6;
                    DchMapStructure* pValues = new DchMapStructure[size_arr];
                    for (; iter < size_arr; iter++)
                    {
                        pValues[iter].plane = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                        pValues[iter].group = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                        pValues[iter].crate = (unsigned int) ((TObjString*) tokens->At(num++))->GetString().Atoll();
                        pValues[iter].slot = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                        pValues[iter].channel_low = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                        pValues[iter].channel_high = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                    }
                    tokens->Delete();

                    if (isSerialChannel)
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, serial_number, channel_number, pValues, size_arr);
                    else
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, pValues, size_arr);

                    delete [] pValues;
                    break;
                }
            case GemMapArrayType:
                {
                    int count_required = 6, size_arr = value_count/6;
                    if (isSerialChannel)
                        count_required++, count_required++;

                    if ((value_count < count_required) || (((value_count-count_required+6) % 6) != 0))
                    {
                        cout<<"Warning: the count of parameters should be greater 6 or 8 ([serial] [channel] values@6) and must be divisible by 6"<<endl<<"The current line will be skipped!"<<endl;
                        tokens->Delete();
                        continue;
                    }
                    if (isSerialChannel)
                    {
                        // get serial number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        serial_number = item->GetString().Atoi();
                        // get channel number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        channel_number = item->GetString().Atoi();
                    }

                    int iter = 0, num = count_required-6;
                    GemMapStructure* pValues = new GemMapStructure[size_arr];
                    for (; iter < size_arr; iter++)
                    {
                        pValues[iter].serial = (unsigned int) ((TObjString*) tokens->At(num++))->GetString().Atoll();
                        pValues[iter].id = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                        pValues[iter].station = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                        pValues[iter].channel_low = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                        pValues[iter].channel_high = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                        pValues[iter].hotZone = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                    }
                    tokens->Delete();

                    if (isSerialChannel)
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, serial_number, channel_number, pValues, size_arr);
                    else
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, pValues, size_arr);

                    delete [] pValues;
                    break;
                }
            case GemPedestalArrayType:
                {
                    int count_required = 4, size_arr = value_count/4;
                    if (isSerialChannel)
                        count_required++, count_required++;

                    if ((value_count < count_required) || (((value_count-count_required+4) % 4) != 0))
                    {
                        cout<<"Warning: the count of parameters should be greater 4 or 6 ([serial] [channel] values@6) and must be divisible by 4"<<endl<<"The current line will be skipped!"<<endl;
                        tokens->Delete();
                        continue;
                    }
                    if (isSerialChannel)
                    {
                        // get serial number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        serial_number = item->GetString().Atoi();
                        // get channel number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        channel_number = item->GetString().Atoi();
                    }

                    int iter = 0, num = count_required-4;
                    GemPedestalStructure* pValues = new GemPedestalStructure[size_arr];
                    for (; iter < size_arr; iter++)
                    {
                        pValues[iter].serial = (unsigned int) ((TObjString*) tokens->At(num++))->GetString().Atoll();
                        pValues[iter].channel = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                        pValues[iter].pedestal = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                        pValues[iter].noise = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                    }
                    tokens->Delete();

                    if (isSerialChannel)
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, serial_number, channel_number, pValues, size_arr);
                    else
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, pValues, size_arr);

                    delete [] pValues;
                    break;
                }
            case TriggerMapArrayType:
                {
                    int count_required = 3, size_arr = value_count/3;
                    if (isSerialChannel)
                        count_required++, count_required++;

                    if ((value_count < count_required) || (((value_count-count_required+3) % 3) != 0))
                    {
                        cout<<"Warning: the count of parameters should be greater 3 or 5 ([serial] [channel] values@3) and must be divisible by 3"<<endl<<"The current line will be skipped!"<<endl;
                        tokens->Delete();
                        continue;
                    }
                    if (isSerialChannel)
                    {
                        // get serial number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        serial_number = item->GetString().Atoi();
                        // get channel number
                        item = (TObjString*) tokens->At(cur_num_item++);
                        channel_number = item->GetString().Atoi();
                    }

                    int iter = 0, num = count_required-3;
                    TriggerMapStructure* pValues = new TriggerMapStructure[size_arr];
                    for (; iter < size_arr; iter++)
                    {
                        pValues[iter].serial = (unsigned int) ((TObjString*) tokens->At(num++))->GetString().Atoll();
                        pValues[iter].slot = (unsigned int) ((TObjString*) tokens->At(num++))->GetString().Atoll();
                        pValues[iter].channel = ((TObjString*) tokens->At(num++))->GetString().Atoi();
                    }
                    tokens->Delete();

                    if (isSerialChannel)
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, serial_number, channel_number, pValues, size_arr);
                    else
                        pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name,start_period, start_run, end_period, end_run, pValues, size_arr);

                    delete [] pValues;
                    break;
                }
            default:
                {
                    cout<<"Error: the type of parameter ("<<parameter_name<<") is not supported for parsing (txt) now"<<endl;
                    continue;
                }
        }// switch (parameter_type)

        // clean memory
        if (pDetectorParameter)
            delete pDetectorParameter;
    }

    txtFile.close();
    delete connUniDb;

    return 0;
}*/

// -------------------------------------------------------------------
ClassImp(UniDbDetectorParameter);
