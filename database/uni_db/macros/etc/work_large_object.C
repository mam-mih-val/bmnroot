void work_large_object()
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

    int d = 8332;
    cout<<"Input: "<<d<<endl;
    stmt->NextIteration();
    stmt->SetBinary(0, &d, sizeof(int));
    string ds = "fgh'\\123";
    cout<<"Input: "<<ds<<endl;
    stmt->NextIteration();
    stmt->SetBinary(0, (char*)ds.c_str(), sizeof(char)*9);
    
	if (!stmt->Process())
    {
        cout<<"ERROR: inserting new record to DB has been failed"<<endl;
        delete stmt;
        delete connectionUniDb;
		return -11;
    }
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
        if (size_mem == 4)
        {
            int* val = new int;
            memcpy(val, mem, size_mem);
            cout<<"Binary size: "<<size_mem<<". Value: "<<*val<<endl;
        }
        else
        {
            char* val = new char[size_mem];
            memcpy(val, mem, size_mem);
            cout<<"Binary size: "<<size_mem<<". Value: "<<string(val)<<endl;
        }
    }

    delete stmt;
    delete connectionUniDb;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

    cout<<"Test was successfull"<<endl;
}
