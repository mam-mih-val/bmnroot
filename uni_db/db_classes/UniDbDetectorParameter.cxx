// ----------------------------------------------------------------------
//                    UniDbDetectorParameter cxx file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"

#include "UniDbDetectorParameter.h"

/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniDbDetectorParameter::UniDbDetectorParameter(UniDbConnection* connUniDb, int value_id, TString detector_name, int parameter_id, int start_run, int end_run, int* dc_serial, int* channel, unsigned char* parameter_value, Long_t size_parameter_value)
{
	connectionUniDb = connUniDb;

	i_value_id = value_id;
	str_detector_name = detector_name;
	i_parameter_id = parameter_id;
	i_start_run = start_run;
	i_end_run = end_run;
	i_dc_serial = dc_serial;
	i_channel = channel;
	blob_parameter_value = parameter_value;
	sz_parameter_value = size_parameter_value;
}

// -----   Destructor   -------------------------------------------------
UniDbDetectorParameter::~UniDbDetectorParameter()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (i_dc_serial)
		delete i_dc_serial;
	if (i_channel)
		delete i_channel;
	if (blob_parameter_value)
		delete [] blob_parameter_value;
}

// -----   Creating new record in class table ---------------------------
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, int parameter_id, int start_run, int end_run, int* dc_serial, int* channel, unsigned char* parameter_value, Long_t size_parameter_value)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into detector_parameter(detector_name, parameter_id, start_run, end_run, dc_serial, channel, parameter_value) "
		"values ($1, $2, $3, $4, $5, $6, $7)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, detector_name);
	stmt->SetInt(1, parameter_id);
	stmt->SetInt(2, start_run);
	stmt->SetInt(3, end_run);
	if (dc_serial == NULL)
		stmt->SetNull(4);
	else
		stmt->SetInt(4, *dc_serial);
	if (channel == NULL)
		stmt->SetNull(5);
	else
		stmt->SetInt(5, *channel);
	stmt->SetLargeObject(6, parameter_value, size_parameter_value, 0x4000000);

	// inserting new record to DB
	if (!stmt->Process())
	{
		cout<<"Error: inserting new record to DB has been failed"<<endl;
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
			cout<<"Error: no last ID in DB!"<<endl;
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
		cout<<"Error: getting last ID has been failed!"<<endl;
		delete stmt_last;
		return 0x00;
	}

	int tmp_value_id;
	tmp_value_id = value_id;
	TString tmp_detector_name;
	tmp_detector_name = detector_name;
	int tmp_parameter_id;
	tmp_parameter_id = parameter_id;
	int tmp_start_run;
	tmp_start_run = start_run;
	int tmp_end_run;
	tmp_end_run = end_run;
	int* tmp_dc_serial;
	if (dc_serial == NULL) tmp_dc_serial = NULL;
	else
		tmp_dc_serial = new int(*dc_serial);
	int* tmp_channel;
	if (channel == NULL) tmp_channel = NULL;
	else
		tmp_channel = new int(*channel);
	unsigned char* tmp_parameter_value;
	Long_t tmp_sz_parameter_value = size_parameter_value;
	tmp_parameter_value = new unsigned char[tmp_sz_parameter_value];
	memcpy(tmp_parameter_value, parameter_value, tmp_sz_parameter_value);

	return new UniDbDetectorParameter(connUniDb, tmp_value_id, tmp_detector_name, tmp_parameter_id, tmp_start_run, tmp_end_run, tmp_dc_serial, tmp_channel, tmp_parameter_value, tmp_sz_parameter_value);
}

// -----   Get table record from database ---------------------------
UniDbDetectorParameter* UniDbDetectorParameter::GetDetectorParameter(int value_id)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select value_id, detector_name, parameter_id, start_run, end_run, dc_serial, channel, parameter_value "
		"from detector_parameter "
		"where value_id = %d", value_id);
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

	int tmp_value_id;
	tmp_value_id = stmt->GetInt(0);
	TString tmp_detector_name;
	tmp_detector_name = stmt->GetString(1);
	int tmp_parameter_id;
	tmp_parameter_id = stmt->GetInt(2);
	int tmp_start_run;
	tmp_start_run = stmt->GetInt(3);
	int tmp_end_run;
	tmp_end_run = stmt->GetInt(4);
	int* tmp_dc_serial;
	if (stmt->IsNull(5)) tmp_dc_serial = NULL;
	else
		tmp_dc_serial = new int(stmt->GetInt(5));
	int* tmp_channel;
	if (stmt->IsNull(6)) tmp_channel = NULL;
	else
		tmp_channel = new int(stmt->GetInt(6));
	unsigned char* tmp_parameter_value;
	tmp_parameter_value = NULL;
	Long_t tmp_sz_parameter_value = 0;
	stmt->GetLargeObject(7, (void*&)tmp_parameter_value, tmp_sz_parameter_value);

	delete stmt;

	return new UniDbDetectorParameter(connUniDb, tmp_value_id, tmp_detector_name, tmp_parameter_id, tmp_start_run, tmp_end_run, tmp_dc_serial, tmp_channel, tmp_parameter_value, tmp_sz_parameter_value);
}

// -----   Delete record from class table ---------------------------
int UniDbDetectorParameter::DeleteDetectorParameter(int value_id)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"delete from detector_parameter "
		"where value_id = $1");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, value_id);

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
int UniDbDetectorParameter::PrintAll()
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select value_id, detector_name, parameter_id, start_run, end_run, dc_serial, channel, parameter_value "
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
	cout<<"Table 'detector_parameter':"<<endl;
	while (stmt->NextResultRow())
	{
		cout<<"value_id: ";
		cout<<(stmt->GetInt(0));
		cout<<", detector_name: ";
		cout<<(stmt->GetString(1));
		cout<<", parameter_id: ";
		cout<<(stmt->GetInt(2));
		cout<<", start_run: ";
		cout<<(stmt->GetInt(3));
		cout<<", end_run: ";
		cout<<(stmt->GetInt(4));
		cout<<", dc_serial: ";
		if (stmt->IsNull(5)) cout<<"NULL";
		else
			cout<<stmt->GetInt(5);
		cout<<", channel: ";
		if (stmt->IsNull(6)) cout<<"NULL";
		else
			cout<<stmt->GetInt(6);
		cout<<", parameter_value: ";
		unsigned char* tmp_parameter_value = NULL;
		Long_t tmp_sz_parameter_value=0;
		stmt->GetLargeObject(7, (void*&)tmp_parameter_value, tmp_sz_parameter_value);
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
		cout<<"Connection object is null"<<endl;
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

int UniDbDetectorParameter::SetParameterId(int parameter_id)
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
		"where value_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetInt(0, parameter_id);
	stmt->SetInt(1, i_value_id);

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

int UniDbDetectorParameter::SetStartRun(int start_run)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
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

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_start_run = start_run;

	delete stmt;
	return 0;
}

int UniDbDetectorParameter::SetEndRun(int end_run)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
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

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	i_end_run = end_run;

	delete stmt;
	return 0;
}

int UniDbDetectorParameter::SetDcSerial(int* dc_serial)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_parameter "
		"set dc_serial = $1 "
		"where value_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (dc_serial == NULL)
		stmt->SetNull(0);
	else
		stmt->SetInt(0, *dc_serial);
	stmt->SetInt(1, i_value_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (i_dc_serial)
		delete i_dc_serial;
	if (dc_serial == NULL) i_dc_serial = NULL;
	else
		i_dc_serial = new int(*dc_serial);

	delete stmt;
	return 0;
}

int UniDbDetectorParameter::SetChannel(int* channel)
{
	if (!connectionUniDb)
	{
		cout<<"Connection object is null"<<endl;
		return -1;
	}

	TSQLServer* uni_db = connectionUniDb->GetSQLServer();

	TString sql = TString::Format(
		"update detector_parameter "
		"set channel = $1 "
		"where value_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	if (channel == NULL)
		stmt->SetNull(0);
	else
		stmt->SetInt(0, *channel);
	stmt->SetInt(1, i_value_id);

	// write new value to database
	if (!stmt->Process())
	{
		cout<<"Error: updating the record has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (i_channel)
		delete i_channel;
	if (channel == NULL) i_channel = NULL;
	else
		i_channel = new int(*channel);

	delete stmt;
	return 0;
}

int UniDbDetectorParameter::SetParameterValue(unsigned char* parameter_value, Long_t size_parameter_value)
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
		"where value_id = $2");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetLargeObject(0, parameter_value, size_parameter_value, 0x4000000);
	stmt->SetInt(1, i_value_id);

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
void UniDbDetectorParameter::Print()
{
	cout<<"Table 'detector_parameter'";
	cout<<". value_id: "<<i_value_id<<". detector_name: "<<str_detector_name<<". parameter_id: "<<i_parameter_id<<". start_run: "<<i_start_run<<". end_run: "<<i_end_run<<". dc_serial: "<<(i_dc_serial == NULL? "NULL": TString::Format("%d", *i_dc_serial))<<". channel: "<<(i_channel == NULL? "NULL": TString::Format("%d", *i_channel))<<". parameter_value: "<<(void*)blob_parameter_value<<", binary size: "<<sz_parameter_value<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */

enumParameterType UniDbDetectorParameter::GetParameterTypeByString(string type_name, bool is_array)
{
    if (type_name == "bool")
    {
        if (!is_array)
            return BoolType;
    }
    else
    {
        if (type_name == "int")
        {
            if (is_array)
                return IntArrayType;
            else
                return IntType;
        }
        else
        {
            if (type_name == "double")
            {
                if (is_array)
                    return DoubleArrayType;
                else
                    return DoubleType;
            }
            else
            {
                if (type_name == "IIStructure")
                {
                    if (is_array)
                        return IIArrayType;
                }
            }
        }
    }

    return ErrorType;
}

// get common detector parameter
UniDbDetectorParameter* UniDbDetectorParameter::GetDetectorParameter(TString detector_name, TString parameter_name, int run_number)
{
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
        return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
        "select value_id, detector_name, p.parameter_id, start_run, end_run, parameter_value "
        "from detector_parameter dp join parameter_ p on dp.parameter_id = p.parameter_id "
        "where lower(detector_name) = lower('%s') and lower(parameter_name) = lower('%s') and start_run <= %d and end_run >= %d and dc_serial is null and channel is null",
                detector_name.Data(), parameter_name.Data(), run_number, run_number);
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

    int tmp_value_id;
    tmp_value_id = stmt->GetInt(0);
    TString tmp_detector_name;
    tmp_detector_name = stmt->GetString(1);
    int tmp_parameter_id;
    tmp_parameter_id = stmt->GetInt(2);
    int tmp_start_run;
    tmp_start_run = stmt->GetInt(3);
    int tmp_end_run;
    tmp_end_run = stmt->GetInt(4);
    unsigned char* tmp_parameter_value = NULL;
    Long_t tmp_sz_parameter_value = 0;
    stmt->GetLargeObject(5, (void*&)tmp_parameter_value, tmp_sz_parameter_value);

    delete stmt;

    return new UniDbDetectorParameter(connUniDb, tmp_value_id, tmp_detector_name, tmp_parameter_id, tmp_start_run, tmp_end_run, NULL, NULL, tmp_parameter_value, tmp_sz_parameter_value);
}

// get TDC/ADC parameter value
UniDbDetectorParameter* UniDbDetectorParameter::GetDetectorParameter(TString detector_name, TString parameter_name, int run_number, int dc_serial, int channel)
{
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
        return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
        "select value_id, detector_name, p.parameter_id, start_run, end_run, dc_serial, channel, parameter_value "
        "from detector_parameter dp join parameter_ p on dp.parameter_id = p.parameter_id "
        "where lower(detector_name) = lower('%s') and lower(parameter_name) = lower('%s') and start_run <= %d and end_run >= %d and dc_serial = %d and channel = %d",
                detector_name.Data(), parameter_name.Data(), run_number, run_number, dc_serial, channel);
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

    int tmp_value_id;
    tmp_value_id = stmt->GetInt(0);
    TString tmp_detector_name;
    tmp_detector_name = stmt->GetString(1);
    int tmp_parameter_id;
    tmp_parameter_id = stmt->GetInt(2);
    int tmp_start_run;
    tmp_start_run = stmt->GetInt(3);
    int tmp_end_run;
    tmp_end_run = stmt->GetInt(4);
    int* tmp_dc_serial = new int(stmt->GetInt(5));
    int* tmp_channel = new int(stmt->GetInt(6));
    unsigned char* tmp_parameter_value = NULL;
    Long_t tmp_sz_parameter_value = 0;
    stmt->GetLargeObject(7, (void*&)tmp_parameter_value, tmp_sz_parameter_value);

    delete stmt;

    return new UniDbDetectorParameter(connUniDb, tmp_value_id, tmp_detector_name, tmp_parameter_id, tmp_start_run, tmp_end_run, tmp_dc_serial, tmp_channel, tmp_parameter_value, tmp_sz_parameter_value);
}

// get channel count for TDC/ADC parameter value
int UniDbDetectorParameter::GetChannelCount(TString detector_name, TString parameter_name, int run_number, int dc_serial)
{
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
        return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
        "select count(*) "
        "from detector_parameter dp join parameter_ p on dp.parameter_id = p.parameter_id "
        "where lower(detector_name) = lower('%s') and lower(parameter_name) = lower('%s') and start_run <= %d and end_run >= %d and dc_serial = %d", detector_name.Data(), parameter_name.Data(), run_number, run_number, dc_serial);
    TSQLStatement* stmt = uni_db->Statement(sql);

    // get table record from DB
    if (!stmt->Process())
    {
        cout<<"Error: getting channel count fror detector parameter has been failed"<<endl;
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

    int channel_count = stmt->GetInt(0);

    delete stmt;

    return channel_count;
}

// get parameter name of the current detector parameter
TString UniDbDetectorParameter::GetParameterName()
{
    UniDbParameter* pCurParameter = UniDbParameter::GetParameter(i_parameter_id);
    if (pCurParameter == NULL)
    {
        cout<<"Error: parameter with current ID wasn't found"<<endl;
        return "ERROR!!!";
    }

    TString par_name = pCurParameter->GetParameterName();

    delete pCurParameter;

    return par_name;
}

// common function for adding common parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, unsigned char* p_parameter_value, Long_t size_parameter_value, enumParameterType enum_parameter_type)
{
    if (end_run < start_run)
    {
        cout<<"Error: end run number should be greater than start number"<<endl;
        return 0x00;
    }

    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
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
        "insert into detector_parameter(detector_name, parameter_id, start_run, end_run, parameter_value) "
        "values ($1, $2, $3, $4, $5)");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetString(0, detector_name);
    stmt->SetInt(1, parameter_id);
    stmt->SetInt(2, start_run);
    stmt->SetInt(3, end_run);
    stmt->SetLargeObject(4, (void*)p_parameter_value, size_parameter_value);
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
            cout<<"Error: no last ID in DB for new parameter value!"<<endl;
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
        cout<<"Error: getting last ID for new parameter value has been failed!"<<endl;
        delete stmt_last;
        return 0x00;
    }

    return new UniDbDetectorParameter(connUniDb, last_id, detector_name, parameter_id, start_run, end_run, NULL, NULL, p_parameter_value, size_parameter_value);
}

// common function for adding TDC/ADC parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, int dc_serial, int channel, unsigned char* p_parameter_value, Long_t size_parameter_value, enumParameterType enum_parameter_type)
{
    if (end_run < start_run)
    {
        cout<<"Error: end run number should be greater than start number"<<endl;
        return 0x00;
    }

    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
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
        "insert into detector_parameter(detector_name, parameter_id, start_run, end_run, parameter_value) "
        "values ($1, $2, $3, $4, $5)");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetString(0, detector_name);
    stmt->SetInt(1, parameter_id);
    stmt->SetInt(2, start_run);
    stmt->SetInt(3, end_run);
    stmt->SetLargeObject(4, (void*)p_parameter_value, size_parameter_value);
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
            cout<<"Error: no last ID in DB for new parameter value!"<<endl;
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
        cout<<"Error: getting last ID for new parameter value has been failed!"<<endl;
        delete stmt_last;
        return 0x00;
    }

    return new UniDbDetectorParameter(connUniDb, last_id, detector_name, parameter_id, start_run, end_run, new int(dc_serial), new int(channel), p_parameter_value, size_parameter_value);
}

// common function for getting parameter value
unsigned char* UniDbDetectorParameter::GetUNC(enumParameterType enum_parameter_type)
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
        cout<<"Critical Error: the parameter with name '"<<parameter_name<<"' isn't corresponding the given type"<<endl;
        return NULL;
    }

    return blob_parameter_value;
}

// common function for setting parameter
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
        "where detector_name = $2 and parameter_id = $3 and start_run = $4 and end_run = $5");
    if (i_dc_serial != NULL)
        sql += " and dc_serial = $6 and channel = $7";

    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetLargeObject(0, (void*)p_parameter_value, size_parameter_value);
    stmt->SetString(1, str_detector_name);
    stmt->SetInt(2, i_parameter_id);
    stmt->SetInt(3, i_start_run);
    stmt->SetInt(4, i_end_run);
    if (i_dc_serial != NULL)
    {
        stmt->SetInt(5, *i_dc_serial);
        stmt->SetInt(6, *i_channel);
    }

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

// create bool detector parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, bool parameter_value)
{
    Long_t size_parameter_value = sizeof(bool);
    bool* p_parameter_value = new bool[1];
    p_parameter_value[0] = parameter_value;

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, BoolType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create bool TDC/ADC parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, int dc_serial, int channel, bool parameter_value)
{
    Long_t size_parameter_value = sizeof(bool);
    bool* p_parameter_value = new bool[1];
    p_parameter_value[0] = parameter_value;

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run, dc_serial, channel,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, BoolType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// get boolean value of detector parameter
bool UniDbDetectorParameter::GetBool()
{
    return *((bool*) GetUNC(BoolType));
}

// set boolean value to detector parameter
int UniDbDetectorParameter::SetBool(bool parameter_value)
{
    Long_t size_parameter_value = sizeof(bool);
    bool* p_parameter_value = new bool[1];
    p_parameter_value[0] = parameter_value;

    int res_code = SetUNC((unsigned char*) p_parameter_value, size_parameter_value);
    if (res_code != 0)
    {
        delete [] p_parameter_value;
        return res_code;
    }

    return 0;
}

// create integer detector parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, int parameter_value)
{
    Long_t size_parameter_value = sizeof(int);
    int* p_parameter_value = new int[1];
    p_parameter_value[0] = parameter_value;

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, IntType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create integer TDC/ADC parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, int dc_serial, int channel, int parameter_value)
{
    Long_t size_parameter_value = sizeof(int);
    int* p_parameter_value = new int[1];
    p_parameter_value[0] = parameter_value;

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run, dc_serial, channel,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, IntType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// get integer value of detector parameter
int UniDbDetectorParameter::GetInt()
{
    return *((int*) GetUNC(IntType));
}

// set integer value to detector parameter
int UniDbDetectorParameter::SetInt(int parameter_value)
{
    Long_t size_parameter_value = sizeof(int);
    int* p_parameter_value = new int[1];
    p_parameter_value[0] = parameter_value;

    int res_code = SetUNC((unsigned char*) p_parameter_value, size_parameter_value);
    if (res_code != 0)
    {
        delete [] p_parameter_value;
        return res_code;
    }

    return 0;
}

// create double detector parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, double parameter_value)
{
    Long_t size_parameter_value = sizeof(double);
    double* p_parameter_value = new double[1];
    p_parameter_value[0] = parameter_value;

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, DoubleType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create double TDC/ADC parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, int dc_serial, int channel, double parameter_value)
{
    Long_t size_parameter_value = sizeof(double);
    double* p_parameter_value = new double[1];
    p_parameter_value[0] = parameter_value;

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run, dc_serial, channel,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, DoubleType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// get double value of detector parameter
double UniDbDetectorParameter::GetDouble()
{
    return *((double*) GetUNC(DoubleType));
}

// set double value to detector parameter
int UniDbDetectorParameter::SetDouble(double parameter_value)
{
    Long_t size_parameter_value = sizeof(double);
    double* p_parameter_value = new double[1];
    p_parameter_value[0] = parameter_value;

    int res_code = SetUNC((unsigned char*) p_parameter_value, size_parameter_value);
    if (res_code != 0)
    {
        delete [] p_parameter_value;
        return res_code;
    }

    return 0;
}

// create string detector parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, TString parameter_value)
{
    Long_t size_parameter_value = parameter_value.Length()+1;
    char* p_parameter_value = new char[size_parameter_value];
    strcpy(p_parameter_value, parameter_value.Data());

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, StringType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create string TDC/ADC parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, int dc_serial, int channel, TString parameter_value)
{
    Long_t size_parameter_value = parameter_value.Length()+1;
    char* p_parameter_value = new char[size_parameter_value];
    strcpy(p_parameter_value, parameter_value.Data());

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run, dc_serial, channel,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, StringType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// get string value of detector parameter
TString UniDbDetectorParameter::GetString()
{
    return (char*)GetUNC(StringType);
}

// set string value to detector parameter
int UniDbDetectorParameter::SetString(TString parameter_value)
{
    Long_t size_parameter_value = parameter_value.Length()+1;
    char* p_parameter_value = new char[size_parameter_value];
    strcpy(p_parameter_value, parameter_value.Data());

    int res_code = SetUNC((unsigned char*) p_parameter_value, size_parameter_value);
    if (res_code != 0)
    {
        delete [] p_parameter_value;
        return res_code;
    }

    return 0;
}

// create detector parameter value as integer array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, int* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(int);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, IntArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create TDC/ADC parameter value as integer array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, int dc_serial, int channel, int* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(int);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run, dc_serial, channel,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, IntArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// get value of detector parameter as integer array
int UniDbDetectorParameter::GetIntArray(int*& parameter_value, int& element_count)
{
    unsigned char* p_parameter_value = GetUNC(IntArrayType);
    if (p_parameter_value == NULL)
        return - 1;

    element_count = sz_parameter_value / sizeof(int);
    parameter_value = new int[element_count];
    memcpy(parameter_value, p_parameter_value, sz_parameter_value);

    return 0;
}

// set value to detector parameter as integer array
int UniDbDetectorParameter::SetIntArray(int* parameter_value, int element_count)
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

// create detector parameter value as double array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, double* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(double);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, DoubleArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create TDC/ADC parameter value as double array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, int dc_serial, int channel, double* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(double);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run, dc_serial, channel,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, DoubleArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// get value of detector parameter as double array
int UniDbDetectorParameter::GetDoubleArray(double*& parameter_value, int& element_count)
{
    unsigned char* p_parameter_value = GetUNC(DoubleArrayType);
    if (p_parameter_value == NULL)
        return - 1;

    element_count = sz_parameter_value / sizeof(double);
    parameter_value = new double[element_count];
    memcpy(parameter_value, p_parameter_value, sz_parameter_value);

    return 0;
}

// set value to detector parameter as double array
int UniDbDetectorParameter::SetDoubleArray(double* parameter_value, int element_count)
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

// create detector parameter value as Int+Int array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, IIStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(IIStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, IIArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create TDC/ADC parameter value as Int+Int array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_run, int end_run, int dc_serial, int channel, IIStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(IIStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_run, end_run, dc_serial, channel,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, IIArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// get value of detector parameter as Int+Int array
int UniDbDetectorParameter::GetIIArray(IIStructure*& parameter_value, int& element_count)
{
    unsigned char* p_parameter_value = GetUNC(IIArrayType);
    if (p_parameter_value == NULL)
        return - 1;

    element_count = sz_parameter_value / sizeof(IIStructure);
    parameter_value = new IIStructure[element_count];
    memcpy(parameter_value, p_parameter_value, sz_parameter_value);

    return 0;
}

// set value to detector parameter as Int+Int array
int UniDbDetectorParameter::SetIIArray(IIStructure* parameter_value, int element_count)
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

TObjArray* UniDbDetectorParameter::Search(const TObjArray& search_conditions)
{
    TObjArray* arrayResult = NULL;

    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
    {
        cout<<"Error: connection to DB was failed"<<endl;
        return arrayResult;
    }

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
                "select value_id, detector_name, p.parameter_id, start_run, end_run, dc_serial, channel, parameter_value "
                "from detector_parameter dp join parameter_ p on dp.parameter_id = p.parameter_id");

    TString strCondition;
    bool isFirst = true;
    TIter next(&search_conditions);
    UniDbSearchCondition* curCondition;
    while (curCondition = (UniDbSearchCondition*) next())
    {
        strCondition = "";

        switch (curCondition->GetColumn())
        {
            case columnDetectorName:    strCondition += "lower(detector_name) "; break;
            case columnParameterName:   strCondition += "lower(parameter_name) "; break;
            case columnStartRun:        strCondition += "start_run "; break;
            case columnEndRun:          strCondition += "end_run "; break;
            case columnDCSerial:        strCondition += "dc_serial "; break;
            case columnChannel:         strCondition += "channel "; break;
            default:
                cout<<"Error: column in the search condition wasn't defined, condition is skipped"<<endl;
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
            default:
                cout<<"Error: comparison operator in the search condition wasn't defined, condition is skipped"<<endl;
                continue;
        }

        switch (curCondition->GetValueType())
        {
            case 0: if (curCondition->GetCondition() != conditionNull) continue; break;
            case 1: strCondition += Form("%d", curCondition->GetIntValue()); break;
            case 2: strCondition += Form("%f", curCondition->GetDoubleValue()); break;
            case 3: strCondition += Form("lower('%s')", curCondition->GetStringValue().Data()); break;
            case 4: strCondition += Form("'%s'", curCondition->GetDatimeValue().AsSQLString()); break;
            default:
                cout<<"Error: value type in the search condition wasn't found, condition is skipped"<<endl;
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
        cout<<"Error: getting runs from DB has been failed"<<endl;
        delete stmt;
        delete connUniDb;

        return arrayResult;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // extract rows one after another
    arrayResult = new TObjArray();
    while (stmt->NextResultRow())
    {
        UniDbConnection* connPar = UniDbConnection::Open(UNIFIED_DB);
        if (connPar == 0x00)
        {
            cout<<"Error: connection to DB for single run was failed"<<endl;
            return arrayResult;
        }

        int tmp_value_id;
        tmp_value_id = stmt->GetInt(0);
        TString tmp_detector_name;
        tmp_detector_name = stmt->GetString(1);
        int tmp_parameter_id;
        tmp_parameter_id = stmt->GetInt(2);
        int tmp_start_run;
        tmp_start_run = stmt->GetInt(3);
        int tmp_end_run;
        tmp_end_run = stmt->GetInt(4);
        int* tmp_dc_serial;
        if (stmt->IsNull(5)) tmp_dc_serial = NULL;
        else
            tmp_dc_serial = new int(stmt->GetInt(5));
        int* tmp_channel;
        if (stmt->IsNull(6)) tmp_channel = NULL;
        else
            tmp_channel = new int(stmt->GetInt(6));
        unsigned char* tmp_parameter_value;
        tmp_parameter_value = NULL;
        Long_t tmp_sz_parameter_value = 0;
        stmt->GetLargeObject(7, (void*&)tmp_parameter_value, tmp_sz_parameter_value);

        arrayResult->Add((TObject*) new UniDbDetectorParameter(connPar, tmp_value_id, tmp_detector_name, tmp_parameter_id, tmp_start_run, tmp_end_run, tmp_dc_serial, tmp_channel, tmp_parameter_value, tmp_sz_parameter_value));
    }

    delete stmt;

    return arrayResult;
}

TObjArray* UniDbDetectorParameter::Search(const UniDbSearchCondition& search_condition)
{
    TObjArray search_conditions;
    search_conditions.Add((TObject*)&search_condition);

    return Search(search_conditions);
}

// -------------------------------------------------------------------
ClassImp(UniDbDetectorParameter);
