// ----------------------------------------------------------------------
//                    UniDbDetectorParameter cxx file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

#include "TSQLServer.h"
#include "TSQLStatement.h"
#include "TObjString.h"

#define ONLY_DECLARATIONS
#include "../function_set.h"
#include "TSystem.h"
#include "UniDbDetectorParameter.h"

#include <fstream>

/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */
// -----   Constructor with database connection   -----------------------
UniDbDetectorParameter::UniDbDetectorParameter(UniDbConnection* connUniDb, int value_id, TString detector_name, int parameter_id, int start_period, int start_run, int end_period, int end_run, unsigned int* dc_serial, int* channel, unsigned char* parameter_value, Long_t size_parameter_value)
{
	connectionUniDb = connUniDb;

	i_value_id = value_id;
	str_detector_name = detector_name;
	i_parameter_id = parameter_id;
	i_start_period = start_period;
	i_start_run = start_run;
	i_end_period = end_period;
	i_end_run = end_run;
	ui_dc_serial = dc_serial;
	i_channel = channel;
	blob_parameter_value = parameter_value;
	sz_parameter_value = size_parameter_value;
}

// -----   Destructor   -------------------------------------------------
UniDbDetectorParameter::~UniDbDetectorParameter()
{
	if (connectionUniDb)
		delete connectionUniDb;
	if (ui_dc_serial)
		delete ui_dc_serial;
	if (i_channel)
		delete i_channel;
	if (blob_parameter_value)
		delete [] blob_parameter_value;
}

// -----   Creating new detector parameter in the database  ---------------------------
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, int parameter_id, int start_period, int start_run, int end_period, int end_run, unsigned int* dc_serial, int* channel, unsigned char* parameter_value, Long_t size_parameter_value)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"insert into detector_parameter(detector_name, parameter_id, start_period, start_run, end_period, end_run, dc_serial, channel, parameter_value) "
		"values ($1, $2, $3, $4, $5, $6, $7, $8, $9)");
	TSQLStatement* stmt = uni_db->Statement(sql);

	stmt->NextIteration();
	stmt->SetString(0, detector_name);
	stmt->SetInt(1, parameter_id);
	stmt->SetInt(2, start_period);
	stmt->SetInt(3, start_run);
	stmt->SetInt(4, end_period);
	stmt->SetInt(5, end_run);
	if (dc_serial == NULL)
		stmt->SetNull(6);
	else
		stmt->SetUInt(6, *dc_serial);
	if (channel == NULL)
		stmt->SetNull(7);
	else
		stmt->SetInt(7, *channel);
	stmt->SetLargeObject(8, parameter_value, size_parameter_value, 0x4000000);

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
	unsigned int* tmp_dc_serial;
	if (dc_serial == NULL) tmp_dc_serial = NULL;
	else
		tmp_dc_serial = new unsigned int(*dc_serial);
	int* tmp_channel;
	if (channel == NULL) tmp_channel = NULL;
	else
		tmp_channel = new int(*channel);
	unsigned char* tmp_parameter_value;
	Long_t tmp_sz_parameter_value = size_parameter_value;
	tmp_parameter_value = new unsigned char[tmp_sz_parameter_value];
	memcpy(tmp_parameter_value, parameter_value, tmp_sz_parameter_value);

	return new UniDbDetectorParameter(connUniDb, tmp_value_id, tmp_detector_name, tmp_parameter_id, tmp_start_period, tmp_start_run, tmp_end_period, tmp_end_run, tmp_dc_serial, tmp_channel, tmp_parameter_value, tmp_sz_parameter_value);
}

// -----  Get detector parameter from the database  ---------------------------
UniDbDetectorParameter* UniDbDetectorParameter::GetDetectorParameter(int value_id)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select value_id, detector_name, parameter_id, start_period, start_run, end_period, end_run, dc_serial, channel, parameter_value "
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
	unsigned int* tmp_dc_serial;
	if (stmt->IsNull(7)) tmp_dc_serial = NULL;
	else
		tmp_dc_serial = new unsigned int(stmt->GetUInt(7));
	int* tmp_channel;
	if (stmt->IsNull(8)) tmp_channel = NULL;
	else
		tmp_channel = new int(stmt->GetInt(8));
	unsigned char* tmp_parameter_value;
	tmp_parameter_value = NULL;
	Long_t tmp_sz_parameter_value = 0;
	stmt->GetLargeObject(9, (void*&)tmp_parameter_value, tmp_sz_parameter_value);

	delete stmt;

	return new UniDbDetectorParameter(connUniDb, tmp_value_id, tmp_detector_name, tmp_parameter_id, tmp_start_period, tmp_start_run, tmp_end_period, tmp_end_run, tmp_dc_serial, tmp_channel, tmp_parameter_value, tmp_sz_parameter_value);
}

// -----  Check detector parameter exists in the database  ---------------------------
bool UniDbDetectorParameter::CheckDetectorParameterExists(int value_id)
{
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
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
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
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
	UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
	if (connUniDb == 0x00) return 0x00;

	TSQLServer* uni_db = connUniDb->GetSQLServer();

	TString sql = TString::Format(
		"select value_id, detector_name, parameter_id, start_period, start_run, end_period, end_run, dc_serial, channel, parameter_value "
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
		cout<<", dc_serial: ";
		if (stmt->IsNull(7)) cout<<"NULL";
		else
			cout<<stmt->GetUInt(7);
		cout<<", channel: ";
		if (stmt->IsNull(8)) cout<<"NULL";
		else
			cout<<stmt->GetInt(8);
		cout<<", parameter_value: ";
		unsigned char* tmp_parameter_value = NULL;
		Long_t tmp_sz_parameter_value=0;
		stmt->GetLargeObject(9, (void*&)tmp_parameter_value, tmp_sz_parameter_value);
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
		cout<<"Connection object is null"<<endl;
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
		cout<<"Connection object is null"<<endl;
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

int UniDbDetectorParameter::SetDcSerial(unsigned int* dc_serial)
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
		stmt->SetUInt(0, *dc_serial);
	stmt->SetInt(1, i_value_id);

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about detector parameter has been failed"<<endl;

		delete stmt;
		return -2;
	}

	if (ui_dc_serial)
		delete ui_dc_serial;
	if (dc_serial == NULL) ui_dc_serial = NULL;
	else
		ui_dc_serial = new unsigned int(*dc_serial);

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

	// write new value to the database
	if (!stmt->Process())
	{
		cout<<"ERROR: updating information about detector parameter has been failed"<<endl;

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
	cout<<". value_id: "<<i_value_id<<". detector_name: "<<str_detector_name<<". parameter_id: "<<i_parameter_id<<". start_period: "<<i_start_period<<". start_run: "<<i_start_run<<". end_period: "<<i_end_period<<". end_run: "<<i_end_run<<". dc_serial: "<<(ui_dc_serial == NULL? "NULL": TString::Format("%u", *ui_dc_serial))<<". channel: "<<(i_channel == NULL? "NULL": TString::Format("%d", *i_channel))<<". parameter_value: "<<(void*)blob_parameter_value<<", binary size: "<<sz_parameter_value<<endl;

	return;
}
/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */

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
                else
                {
                    if ((type_name == "void") || (type_name == "binary"))
                    {
                        if (is_array)
                            return BinaryArrayType;
                    }
                }
            }
        }
    }

    return ErrorType;
}

// get common detector parameter
UniDbDetectorParameter* UniDbDetectorParameter::GetDetectorParameter(TString detector_name, TString parameter_name, int period_number, int run_number)
{
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
        return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
        "select value_id, detector_name, p.parameter_id, start_period, start_run, end_period, end_run, parameter_value "
        "from detector_parameter dp join parameter_ p on dp.parameter_id = p.parameter_id "
        "where lower(detector_name) = lower('%s') and lower(parameter_name) = lower('%s') and "
        "(not (((%d < start_period) or ((%d = start_period) and (%d < start_run))) or ((%d > end_period) or ((%d = end_period) and (%d > end_run))))) and "
        "dc_serial is null and channel is null",
                detector_name.Data(), parameter_name.Data(), period_number, period_number, run_number, period_number, period_number, run_number);
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
    stmt->GetLargeObject(7, (void*&)tmp_parameter_value, tmp_sz_parameter_value);

    delete stmt;

    return new UniDbDetectorParameter(connUniDb, tmp_value_id, tmp_detector_name, tmp_parameter_id, tmp_start_period, tmp_start_run, tmp_end_period, tmp_end_run,
                                      NULL, NULL, tmp_parameter_value, tmp_sz_parameter_value);
}

// get TDC/ADC parameter value
UniDbDetectorParameter* UniDbDetectorParameter::GetDetectorParameter(TString detector_name, TString parameter_name, int period_number, int run_number, unsigned int dc_serial, int channel)
{
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
        return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
        "select value_id, detector_name, p.parameter_id, start_period, start_run, end_period, end_run, dc_serial, channel, parameter_value "
        "from detector_parameter dp join parameter_ p on dp.parameter_id = p.parameter_id "
        "where lower(detector_name) = lower('%s') and lower(parameter_name) = lower('%s') and "
        "(not (((%d < start_period) or ((%d = start_period) and (%d < start_run))) or ((%d > end_period) or ((%d = end_period) and (%d > end_run))))) and "
        "dc_serial = %d and channel = %d",
                detector_name.Data(), parameter_name.Data(), period_number, period_number, run_number, period_number, period_number, run_number, dc_serial, channel);
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
    int tmp_start_period;
    tmp_start_period = stmt->GetInt(3);
    int tmp_start_run;
    tmp_start_run = stmt->GetInt(4);
    int tmp_end_period;
    tmp_end_period = stmt->GetInt(5);
    int tmp_end_run;
    tmp_end_run = stmt->GetInt(6);
    unsigned int* tmp_dc_serial = new unsigned int(stmt->GetUInt(7));
    int* tmp_channel = new int(stmt->GetInt(8));
    unsigned char* tmp_parameter_value = NULL;
    Long_t tmp_sz_parameter_value = 0;
    stmt->GetLargeObject(9, (void*&)tmp_parameter_value, tmp_sz_parameter_value);

    delete stmt;

    return new UniDbDetectorParameter(connUniDb, tmp_value_id, tmp_detector_name, tmp_parameter_id, tmp_start_period, tmp_start_run, tmp_end_period, tmp_end_run,
                                      tmp_dc_serial, tmp_channel, tmp_parameter_value, tmp_sz_parameter_value);
}

// ----- Delete common detector parameter value ------------------------
int UniDbDetectorParameter::DeleteDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run)
{
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00) return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
        "delete from detector_parameter "
        "where lower(detector_name) = lower($1) and parameter_id IN (select parameter_id from parameter_ where lower(parameter_name) = lower($2)) and start_period = $3 and start_run = $4 and end_period = $5 and end_run = $6");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetString(0, detector_name);
    stmt->SetString(1, parameter_name);
    stmt->SetInt(2, start_period);
    stmt->SetInt(3, start_run);
    stmt->SetInt(4, end_period);
    stmt->SetInt(5, end_run);

    // delete table record from DB
    if (!stmt->Process())
    {
        cout<<"Error: deleting record from DB has been failed"<<endl;

        delete stmt;
        delete connUniDb;
        return -1;
    }

    if (stmt->GetNumAffectedRows() == 0)
    {
        cout<<"Warning: parameter value was not found for deleting"<<endl;
        delete stmt;
        delete connUniDb;
        return -2;
    }

    delete stmt;
    delete connUniDb;
    return 0;
}

// ----- Delete TDC/ADC parameter value -----------------------------
int UniDbDetectorParameter::DeleteDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run, unsigned int dc_serial, int channel)
{
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00) return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
        "delete from detector_parameter "
        "where lower(detector_name) = lower($1) and parameter_id IN (select parameter_id from parameter_ where lower(parameter_name) = lower($2)) and start_period = $3 and start_run = $4 and end_period = $5 and end_run = $6 and dc_serial = $7 and channel = $8");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetString(0, detector_name);
    stmt->SetString(1, parameter_name);
    stmt->SetInt(2, start_period);
    stmt->SetInt(3, start_run);
    stmt->SetInt(4, end_period);
    stmt->SetInt(5, end_run);
    stmt->SetUInt(6, dc_serial);
    stmt->SetInt(7, channel);

    // delete table record from DB
    if (!stmt->Process())
    {
        cout<<"Error: deleting record from DB has been failed"<<endl;

        delete stmt;
        delete connUniDb;
        return -1;
    }

    if (stmt->GetNumAffectedRows() == 0)
    {
        cout<<"Warning: parameter value was not found for deleting"<<endl;
        delete stmt;
        delete connUniDb;
        return -2;
    }

    delete stmt;
    delete connUniDb;
    return 0;
}

// get channel count for TDC/ADC parameter value
int UniDbDetectorParameter::GetChannelCount(TString detector_name, TString parameter_name, int period_number, int run_number, unsigned int dc_serial)
{
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
        return 0x00;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

    TString sql = TString::Format(
        "select count(*) "
        "from detector_parameter dp join parameter_ p on dp.parameter_id = p.parameter_id "
        "where lower(detector_name) = lower('%s') and lower(parameter_name) = lower('%s') and "
        "(not (((%d < start_period) or ((%d = start_period) and (%d < start_run))) or ((%d > end_period) or ((%d = end_period) and (%d > end_run))))) and "
        "dc_serial = %u", detector_name.Data(), parameter_name.Data(), period_number, period_number, run_number, period_number, period_number, run_number, dc_serial);
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
        cout<<"ERROR: parameter with current ID was not found"<<endl;
        return "";
    }

    TString par_name = pCurParameter->GetParameterName();

    delete pCurParameter;
    return par_name;
}

// get parameter type of the current detector parameter
enumParameterType UniDbDetectorParameter::GetParameterType()
{
    UniDbParameter* pCurParameter = UniDbParameter::GetParameter(i_parameter_id);
    if (pCurParameter == NULL)
    {
        cout<<"ERROR: parameter with current ID was not found"<<endl;
        return ErrorType;
    }

    enumParameterType par_type = (enumParameterType) pCurParameter->GetParameterType();

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
        cout<<"Connection object is null"<<endl;
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
        cout<<"Error: updating the record has been failed"<<endl;

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
        cout<<"Connection object is null"<<endl;
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
        cout<<"Error: updating the record has been failed"<<endl;

        delete stmt;
        return -2;
    }

    i_end_period = end_period;
    i_end_run = end_run;

    delete stmt;
    return 0;
}

// write ROOT file with binary detector parameter (with given detector and parameter names) to the database from start run_number to end run_number
int UniDbDetectorParameter::WriteRootFile(int start_period, int start_run, int end_period, int end_run, const char* detector_name, const char* parameter_name, const char* root_file_path)
{
    TString strRootFilePath(root_file_path);
    gSystem->ExpandPathName(strRootFilePath);
    FILE* root_file = fopen(strRootFilePath.Data(), "rb");
    if (root_file == NULL)
    {
        cout<<"Error: opening root file: "<<strRootFilePath<<" was failed"<<endl;
        return -1;
    }

    fseek(root_file, 0, SEEK_END);
    size_t file_size = ftell(root_file);
    rewind(root_file);
    if (file_size <= 0)
    {
        cout<<"Error: getting file size: "<<strRootFilePath<<" was failed"<<endl;
        fclose(root_file);
        return -2;
    }

    unsigned char* buffer = new unsigned char[file_size];
    if (buffer == NULL)
    {
        cout<<"Error: getting memory from heap was failed"<<endl;
        fclose(root_file);
        return -3;
    }

    size_t bytes_read = fread(buffer, 1, file_size, root_file);
    if (bytes_read != file_size)
    {
        cout<<"Error: reading file: "<<strRootFilePath<<", got "<<bytes_read<<" bytes of "<<file_size<<endl;
        delete [] buffer;
        fclose(root_file);
        return -4;
    }

    fclose(root_file);

    // set root file's bytes for run range
    UniDbDetectorParameter* pParameterValue = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, buffer, file_size);
    if (pParameterValue == NULL)
    {
        delete [] buffer;
        return -5;
    }

    delete [] buffer;
    delete pParameterValue;

    return 0;
}

// read ROOT file with binary detector parameter (with given detector and parameter names) from the database for a selected run number
int UniDbDetectorParameter::ReadRootFile(int period_number, int run_number, const char* detector_name, const char* parameter_name, const char* root_file_path)
{
    // get root file's bytes for selected run
    unsigned char* buffer = NULL;
    size_t file_size;
    UniDbDetectorParameter* pParameterValue = UniDbDetectorParameter::GetDetectorParameter(detector_name, parameter_name, period_number, run_number);
    if (pParameterValue == NULL)
        return -1;

    FILE* root_file = fopen(root_file_path, "wb");
    if (root_file == NULL)
    {
        cout<<"Error: creating root file: "<<root_file_path<<endl;
        delete pParameterValue;
        return -2;
    }

    pParameterValue->GetBinaryArray(buffer, file_size);
    size_t bytes_write = fwrite(buffer, 1, file_size, root_file);
    delete pParameterValue;
    if (bytes_write != file_size)
    {
        cout<<"Error: writing file: "<<root_file_path<<", put "<<bytes_write<<" bytes of "<<file_size<<endl;
        delete [] buffer;
        fclose(root_file);
        return -3;
    }

    fclose(root_file);

    if (buffer)
        delete [] buffer;

    return 0;
}

// common function for adding common parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run, unsigned char* p_parameter_value, Long_t size_parameter_value, enumParameterType enum_parameter_type)
{
    if (((end_period < start_period) or ((end_period = start_period) and (end_run < start_run))) or ((start_period > end_period) or ((start_period = end_period) and (start_run > end_run))))
    {
        cout<<"Error: end run should be after or the same as start run"<<endl;
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
        "insert into detector_parameter(detector_name, parameter_id, start_period, start_run, end_period, end_run, parameter_value) "
        "values ($1, $2, $3, $4, $5, $6, $7)");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetString(0, detector_name);
    stmt->SetInt(1, parameter_id);
    stmt->SetInt(2, start_period);
    stmt->SetInt(3, start_run);
    stmt->SetInt(4, end_period);
    stmt->SetInt(5, end_run);
    stmt->SetLargeObject(6, (void*)p_parameter_value, size_parameter_value);
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

    return new UniDbDetectorParameter(connUniDb, last_id, detector_name, parameter_id, start_period, start_run, end_period, end_run, NULL, NULL, p_parameter_value, size_parameter_value);
}

// common function for adding TDC/ADC parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int dc_serial, int channel, unsigned char* p_parameter_value, Long_t size_parameter_value, enumParameterType enum_parameter_type)
{
    if (((end_period < start_period) or ((end_period = start_period) and (end_run < start_run))) or ((start_period > end_period) or ((start_period = end_period) and (start_run > end_run))))
    {
        cout<<"Error: end run should be after or the same as start run"<<endl;
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

    TString sql = TString::Format("insert into detector_parameter(detector_name, parameter_id, start_period, start_run, end_period, end_run, dc_serial, channel, parameter_value) "
                                  "values ($1, $2, $3, $4, $5, $6, $7, $8, $9)");
    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetString(0, detector_name);
    stmt->SetInt(1, parameter_id);
    stmt->SetInt(2, start_period);
    stmt->SetInt(3, start_run);
    stmt->SetInt(4, end_period);
    stmt->SetInt(5, end_run);
    stmt->SetUInt(6, dc_serial);
    stmt->SetInt(7, channel);
    stmt->SetLargeObject(8, (void*)p_parameter_value, size_parameter_value);
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

    return new UniDbDetectorParameter(connUniDb, last_id, detector_name, parameter_id, start_period, start_run, end_period, end_run, new unsigned int(dc_serial), new int(channel), p_parameter_value, size_parameter_value);
}

// common function for getting parameter value
unsigned char* UniDbDetectorParameter::GetUNC(enumParameterType enum_parameter_type, bool isCheckType)
{
    if (isCheckType)
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
        "where detector_name = $2 and parameter_id = $3 and start_period = $4 and start_run = $5 and end_period = $6 and end_run = $7");
    if (ui_dc_serial != NULL)
        sql += " and dc_serial = $6 and channel = $7";

    TSQLStatement* stmt = uni_db->Statement(sql);

    stmt->NextIteration();
    stmt->SetLargeObject(0, (void*)p_parameter_value, size_parameter_value);
    stmt->SetString(1, str_detector_name);
    stmt->SetInt(2, i_parameter_id);
    stmt->SetInt(3, i_start_period);
    stmt->SetInt(4, i_start_run);
    stmt->SetInt(5, i_end_period);
    stmt->SetInt(6, i_end_run);
    if (ui_dc_serial != NULL)
    {
        stmt->SetUInt(7, *ui_dc_serial);
        stmt->SetInt(8, *i_channel);
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
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        bool parameter_value)
{
    Long_t size_parameter_value = sizeof(bool);
    bool* p_parameter_value = new bool[1];
    p_parameter_value[0] = parameter_value;

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, BoolType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create bool TDC/ADC parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int dc_serial, int channel, bool parameter_value)
{
    Long_t size_parameter_value = sizeof(bool);
    bool* p_parameter_value = new bool[1];
    p_parameter_value[0] = parameter_value;

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, dc_serial, channel,
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
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run, int parameter_value)
{
    Long_t size_parameter_value = sizeof(int);
    int* p_parameter_value = new int[1];
    p_parameter_value[0] = parameter_value;

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, IntType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create integer TDC/ADC parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int dc_serial, int channel, int parameter_value)
{
    Long_t size_parameter_value = sizeof(int);
    int* p_parameter_value = new int[1];
    p_parameter_value[0] = parameter_value;

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, dc_serial, channel,
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
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        double parameter_value)
{
    Long_t size_parameter_value = sizeof(double);
    double* p_parameter_value = new double[1];
    p_parameter_value[0] = parameter_value;

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, DoubleType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create double TDC/ADC parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int dc_serial, int channel, double parameter_value)
{
    Long_t size_parameter_value = sizeof(double);
    double* p_parameter_value = new double[1];
    p_parameter_value[0] = parameter_value;

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, dc_serial, channel,
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
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        TString parameter_value)
{
    Long_t size_parameter_value = parameter_value.Length()+1;
    char* p_parameter_value = new char[size_parameter_value];
    strcpy(p_parameter_value, parameter_value.Data());

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, StringType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create string TDC/ADC parameter value
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int dc_serial, int channel, TString parameter_value)
{
    Long_t size_parameter_value = parameter_value.Length()+1;
    char* p_parameter_value = new char[size_parameter_value];
    strcpy(p_parameter_value, parameter_value.Data());

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, dc_serial, channel,
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
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        int* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(int);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, IntArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create TDC/ADC parameter value as integer array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int dc_serial, int channel, int* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(int);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, dc_serial, channel,
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
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        double* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(double);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, DoubleArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create TDC/ADC parameter value as double array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int dc_serial, int channel, double* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(double);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, dc_serial, channel,
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

// create detector parameter value as binary array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned char* parameter_value, int byte_count)
{
    unsigned char* p_parameter_value = new unsigned char[byte_count];
    memcpy(p_parameter_value, parameter_value, byte_count);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                                                                 p_parameter_value, byte_count, BinaryArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create TDC/ADC parameter value as binary array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int dc_serial, int channel, unsigned char* parameter_value, int byte_count)
{
    unsigned char* p_parameter_value = new unsigned char[byte_count];
    memcpy(p_parameter_value, parameter_value, byte_count);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, dc_serial, channel,
                                                                                                 p_parameter_value, byte_count, BinaryArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// get value of detector parameter as binary array
int UniDbDetectorParameter::GetBinaryArray(unsigned char*& parameter_value, size_t& byte_count)
{
    unsigned char* p_parameter_value = GetUNC(BinaryArrayType);
    if (p_parameter_value == NULL)
        return - 1;

    byte_count = sz_parameter_value;
    parameter_value = new unsigned char[byte_count];
    memcpy(parameter_value, p_parameter_value, sz_parameter_value);

    return 0;
}

// set value to detector parameter as binary array
int UniDbDetectorParameter::SetBinaryArray(unsigned char* parameter_value, size_t byte_count)
{
    unsigned char* p_parameter_value = new unsigned char[byte_count];
    memcpy(p_parameter_value, parameter_value, byte_count);

    int res_code = SetUNC(p_parameter_value, byte_count);
    if (res_code != 0)
    {
        delete [] p_parameter_value;
        return res_code;
    }

    return 0;
}

// create detector parameter value as Int+Int array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        IIStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(IIStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, IIArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create TDC/ADC parameter value as Int+Int array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int dc_serial, int channel, IIStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(IIStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, dc_serial, channel,
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

// create detector parameter value as unsigned integer array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(unsigned int);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                                                                 p_parameter_value, size_parameter_value, UIntArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create TDC/ADC parameter value as unsigned integer array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int dc_serial, int channel, unsigned int* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(unsigned int);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, dc_serial, channel,
                                                                                                 p_parameter_value, size_parameter_value, UIntArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// get value of detector parameter as unsigned integer array
int UniDbDetectorParameter::GetUIntArray(unsigned int*& parameter_value, int& element_count)
{
    unsigned char* p_parameter_value = GetUNC(UIntArrayType);
    if (p_parameter_value == NULL)
        return - 1;

    element_count = sz_parameter_value / sizeof(unsigned int);
    parameter_value = new unsigned int[element_count];
    memcpy(parameter_value, p_parameter_value, sz_parameter_value);

    return 0;
}

// set value to detector parameter as unsigned integer array
int UniDbDetectorParameter::SetUIntArray(unsigned int* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(unsigned int);
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

// create detector parameter value as DCH mapping Array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        DchMapStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(DchMapStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, DchMapArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create TDC/ADC parameter value as DCH mapping Array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int dc_serial, int channel, DchMapStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(DchMapStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, dc_serial, channel,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, DchMapArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// get value of detector parameter as DCH mapping Array
int UniDbDetectorParameter::GetDchMapArray(DchMapStructure*& parameter_value, int& element_count)
{
    unsigned char* p_parameter_value = GetUNC(DchMapArrayType);
    if (p_parameter_value == NULL)
        return - 1;

    element_count = sz_parameter_value / sizeof(DchMapStructure);
    parameter_value = new DchMapStructure[element_count];
    memcpy(parameter_value, p_parameter_value, sz_parameter_value);

    return 0;
}

// set value to detector parameter as DCH mapping Array
int UniDbDetectorParameter::SetDchMapArray(DchMapStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(DchMapStructure);
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

// create detector parameter value as GEM mapping Array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        GemMapStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(GemMapStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, GemMapArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create TDC/ADC parameter value as GEM mapping Array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int dc_serial, int channel, GemMapStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(GemMapStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, dc_serial, channel,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, GemMapArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// get value of detector parameter as GEM mapping Array
int UniDbDetectorParameter::GetGemMapArray(GemMapStructure*& parameter_value, int& element_count)
{
    unsigned char* p_parameter_value = GetUNC(GemMapArrayType);
    if (p_parameter_value == NULL)
        return - 1;

    element_count = sz_parameter_value / sizeof(GemMapStructure);
    parameter_value = new GemMapStructure[element_count];
    memcpy(parameter_value, p_parameter_value, sz_parameter_value);

    return 0;
}

// set value to detector parameter as GEM mapping Array
int UniDbDetectorParameter::SetGemMapArray(GemMapStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(GemMapStructure);
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

// create detector parameter value as GEM pedestal map Array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        GemPedestalStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(GemPedestalStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, GemPedestalArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create TDC/ADC parameter value as GEM pedestal map Array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int dc_serial, int channel, GemPedestalStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(GemPedestalStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, dc_serial, channel,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, GemPedestalArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// get value of detector parameter as GEM pedestal map Array
int UniDbDetectorParameter::GetGemPedestalArray(GemPedestalStructure*& parameter_value, int& element_count)
{
    unsigned char* p_parameter_value = GetUNC(GemPedestalArrayType);
    if (p_parameter_value == NULL)
        return - 1;

    element_count = sz_parameter_value / sizeof(GemPedestalStructure);
    parameter_value = new GemPedestalStructure[element_count];
    memcpy(parameter_value, p_parameter_value, sz_parameter_value);

    return 0;
}

// set value to detector parameter as GEM pedestal map Array
int UniDbDetectorParameter::SetGemPedestalArray(GemPedestalStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(GemPedestalStructure);
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

// create detector parameter value as Trigger mapping Array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        TriggerMapStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(TriggerMapStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, TriggerMapArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create TDC/ADC parameter value as Trigger mapping Array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int dc_serial, int channel, TriggerMapStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(TriggerMapStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, dc_serial, channel,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, TriggerMapArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// get value of detector parameter as Trigger mapping Array
int UniDbDetectorParameter::GetTriggerMapArray(TriggerMapStructure*& parameter_value, int& element_count)
{
    unsigned char* p_parameter_value = GetUNC(TriggerMapArrayType);
    if (p_parameter_value == NULL)
        return - 1;

    element_count = sz_parameter_value / sizeof(TriggerMapStructure);
    parameter_value = new TriggerMapStructure[element_count];
    memcpy(parameter_value, p_parameter_value, sz_parameter_value);

    return 0;
}

// set value to detector parameter as Trigger mapping Array
int UniDbDetectorParameter::SetTriggerMapArray(TriggerMapStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(TriggerMapStructure);
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

// create detector parameter value as Lorents Shift Array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        LorentzShiftStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(LorentzShiftStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, LorentzShiftArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// create TDC/ADC parameter value as Lorents Shift Array
UniDbDetectorParameter* UniDbDetectorParameter::CreateDetectorParameter(TString detector_name, TString parameter_name, int start_period, int start_run, int end_period, int end_run,
                                                                        unsigned int dc_serial, int channel, LorentzShiftStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(LorentzShiftStructure);
    unsigned char* p_parameter_value = new unsigned char[size_parameter_value];
    memcpy(p_parameter_value, parameter_value, size_parameter_value);

    UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter(detector_name, parameter_name, start_period, start_run, end_period, end_run, dc_serial, channel,
                                                                                                 (unsigned char*)p_parameter_value, size_parameter_value, LorentzShiftArrayType);
    if (pDetectorParameter == 0x00)
        delete [] p_parameter_value;

    return pDetectorParameter;
}

// get value of detector parameter as Lorents Shift Array
int UniDbDetectorParameter::GetLorentzShiftArray(LorentzShiftStructure*& parameter_value, int& element_count)
{
    unsigned char* p_parameter_value = GetUNC(LorentzShiftArrayType);
    if (p_parameter_value == NULL)
        return - 1;

    element_count = sz_parameter_value / sizeof(LorentzShiftStructure);
    parameter_value = new LorentzShiftStructure[element_count];
    memcpy(parameter_value, p_parameter_value, sz_parameter_value);

    return 0;
}

// set value to detector parameter as Lorents Shift Array
int UniDbDetectorParameter::SetLorentzShiftArray(LorentzShiftStructure* parameter_value, int element_count)
{
    Long_t size_parameter_value = element_count * sizeof(LorentzShiftStructure);
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
                "select value_id, detector_name, p.parameter_id, start_period, start_run, end_period, end_run, dc_serial, channel, parameter_value "
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
            case columnStartPeriod:     strCondition += "start_period "; break;
            case columnStartRun:        strCondition += "start_run "; break;
            case columnEndPeriod:       strCondition += "end_period "; break;
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
            case 2: strCondition += Form("%u", curCondition->GetUIntValue()); break;
            case 3: strCondition += Form("%f", curCondition->GetDoubleValue()); break;
            case 4: strCondition += Form("lower('%s')", curCondition->GetStringValue().Data()); break;
            case 5: strCondition += Form("'%s'", curCondition->GetDatimeValue().AsSQLString()); break;
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
    arrayResult->SetOwner(kTRUE);
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
        int tmp_start_period;
        tmp_start_period = stmt->GetInt(3);
        int tmp_start_run;
        tmp_start_run = stmt->GetInt(4);
        int tmp_end_period;
        tmp_end_period = stmt->GetInt(5);
        int tmp_end_run;
        tmp_end_run = stmt->GetInt(6);
        unsigned int* tmp_dc_serial;
        if (stmt->IsNull(7)) tmp_dc_serial = NULL;
        else
            tmp_dc_serial = new unsigned int(stmt->GetUInt(7));
        int* tmp_channel;
        if (stmt->IsNull(8)) tmp_channel = NULL;
        else
            tmp_channel = new int(stmt->GetInt(8));
        unsigned char* tmp_parameter_value;
        tmp_parameter_value = NULL;
        Long_t tmp_sz_parameter_value = 0;
        stmt->GetLargeObject(9, (void*&)tmp_parameter_value, tmp_sz_parameter_value);

        arrayResult->Add((TObject*) new UniDbDetectorParameter(connPar, tmp_value_id, tmp_detector_name, tmp_parameter_id, tmp_start_period, tmp_start_run, tmp_end_period, tmp_end_run,
                                                               tmp_dc_serial, tmp_channel, tmp_parameter_value, tmp_sz_parameter_value));
    }

    delete stmt;

    arrayResult->SetOwner(kTRUE);
    return arrayResult;
}

TObjArray* UniDbDetectorParameter::Search(const UniDbSearchCondition& search_condition)
{
    TObjArray search_conditions;
    search_conditions.Add((TObject*)&search_condition);

    return Search(search_conditions);
}


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
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
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
            cout<<"Error: the detector with name '"<<detector_name<<"' wasn't found"<<endl;
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
            cout<<"Error: the parameter with name '"<<parameter_name<<"' wasn't found"<<endl;
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
}

// -------------------------------------------------------------------
ClassImp(UniDbDetectorParameter);
