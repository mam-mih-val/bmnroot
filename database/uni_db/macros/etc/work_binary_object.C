void work_binary_object()
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    UniConnection* connDb = UniConnection::Open();
    if (connectionUniDb == nullptr)
    {
        cout<<"ERROR: connection to the database can not be established"<<endl;
        return -1;
    }

    TSQLServer* db_server = connectionUniDb->GetSQLServer();

    cout<<"DB: "<<uni_db->GetDB()<<". Type: "<<uni_db->GetDBMS()<<"."<<endl;

    TSQLStatement* stmt = uni_db->Statement("insert into test(d) "
                                            "values ($1)");

    string file_name("input.txt");
    FILE* input_file = fopen(file_name.c_str(), "rb");
    if (input_file == nullptr) return;
    fseek(input_file, 0L, SEEK_END);
    size_t file_size = ftell(input_file);
    unsigned char* in_array = new unsigned char[file_size];
    fseek(input_file, 0L, SEEK_SET);
    int bytes_read = fread(in_array, sizeof(unsigned char), file_size, input_file);
    if (bytes_read != file_size) return;
    fclose(input_file);

    stmt->NextIteration();
    stmt->SetBinary(0, in_array, file_size);
    
	if (!stmt->Process())
    {
        cout<<"ERROR: inserting new record to DB has been failed"<<endl;
        delete [] in_array;
        delete stmt;
        delete connectionUniDb;
		return -11;
    }
    delete [] in_array;
	delete stmt;
    delete connectionUniDb;

    // print all values
    connDb = UniConnection::Open();
    if (connectionUniDb == nullptr)
    {
        cout<<"ERROR: connection to the database can't be established"<<endl;
        return -2;
    }

    uni_db = connectionUniDb->GetSQLServer();
	
    stmt = uni_db->Statement("select d from test");
    if (!stmt->Process())
    {
        cout<<"ERROR: reading all record from DB has been failed"<<endl;
        delete stmt;
        delete connectionUniDb;
        return -12;
    }

    // store result of statement in buffer
    stmt->StoreResult();

    // if there is no last id then exit with error
    while (stmt->NextResultRow())
    {
        void* mem = 0;
        Long_t size_mem = 0;
        stmt->GetBinary(0, mem, size_mem);

        string file_name_out("output.txt");
        FILE* output_file = fopen(file_name_out.c_str(), "wb");
        int bytes_written = fwrite(mem, sizeof(unsigned char), size_mem, output_file);
        fclose(output_file);
    }

    delete stmt;
    delete connectionUniDb;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

    cout<<"Test was successfull"<<endl;
}
