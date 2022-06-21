#include "../../../uni_db_settings.h"

int copy_table_db2db()
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    // connection to MySQL
    TSQLServer* MySQLServer = TSQLServer::Connect("mysql://nc4.jinr.ru/mpd_grids", MYSQL_DB_USERNAME, MYSQL_DB_PASSWORD);
    if (MySQLServer == nullptr)
    {
        cout<<"Error: connection to MySQLServer wasn't established"<<endl;
        return nullptr;
    }

    // connection to PostgreSQL
    TSQLServer* PgSQLServer = TSQLServer::Connect("pgsql://" + UNI_DB_HOST + "/" + UNI_DB_NAME, UNI_DB_USERNAME, UNI_DB_PASSWORD);
    if (PgSQLServer == nullptr)
    {
        cout<<"Error: connection to PgSQLServer wasn't established"<<endl;
        return nullptr;
    }


    // read from mysql
    TString my_sql = TString::Format(
            "select id, data4mpd_datatype, data4mpd_generator, data4mpd_collision, data4mpd_energy, data4mpd_trigger, data4mpd_path, data4mpd_description "
            "from events");
    TSQLStatement* my_stmt = MySQLServer->Statement(my_sql);

    // write to pgsql
    TString pg_sql = TString::Format(
            "insert into simulation_file(file_path, generator_name, beam_particle, target_particle, energy, centrality, file_desc) "
            "values ($1, $2, $3, $4, $5, $6, $7)");
    TSQLStatement* pg_stmt = PgSQLServer->Statement(pg_sql);


    // get table record from DB
    if (!my_stmt->Process())
    {
            cout<<"Error: getting all records from MySQL DB has been failed"<<endl;

            delete my_stmt;
            delete pg_stmt;
            delete MySQLServer;
            delete PgSQLServer;
            return -1;
    }

    // store result of statement in buffer
    my_stmt->StoreResult();


    // insert to pgSQL
    while (my_stmt->NextResultRow())
    {
        if (pg_stmt->NextIteration())
        {
            // file_path
            if (my_stmt->IsNull(6)) pg_stmt->SetNull(0);
            else pg_stmt->SetString(0, my_stmt->GetString(6));
            // generator_name
            if (my_stmt->IsNull(2)) pg_stmt->SetNull(1);
            else pg_stmt->SetString(1, my_stmt->GetString(2));
            // beam_particle, target_particle
            if (my_stmt->IsNull(3))
            {
                pg_stmt->SetNull(2);
                pg_stmt->SetNull(3);
            }
            else
            {
                TString str_collision = my_stmt->GetString(3);
                str_collision.ToLower();
                int index = 0;
                for (int i = 0; i < 2; i++)
                {
                    if (str_collision[index] == 'a')
                    {
                        pg_stmt->SetString(2+i, "Au");
                        index++; index++;
                    }
                    else
                    {
                        if (str_collision[index] == 'd')
                        {
                            pg_stmt->SetString(2+i, "d");
                            index++;
                        }
                        else
                        {
                            if (str_collision[index] == 'c')
                            {
                                pg_stmt->SetString(2+i, "C");
                                index++;
                            }
                        }
                    }
                }
            }
            // energy
            if (my_stmt->IsNull(4)) pg_stmt->SetNull(4);
            else pg_stmt->SetDouble(4, my_stmt->GetDouble(4));
            // centrality
            if (my_stmt->IsNull(5)) pg_stmt->SetNull(5);
            else pg_stmt->SetString(5, my_stmt->GetString(5));
            // description
            if (my_stmt->IsNull(7)) pg_stmt->SetNull(6);
            else pg_stmt->SetString(6, my_stmt->GetString(7));
        }
    }

    // inserting new records to PgSQL DB
    if (!pg_stmt->Process())
    {
        cout<<"Error: inserting new records to PgSQL DB has been failed"<<endl;
        delete my_stmt;
        delete pg_stmt;
        delete MySQLServer;
        delete PgSQLServer;
        return -2;
    }

    delete my_stmt;
    delete pg_stmt;
    delete MySQLServer;
    delete PgSQLServer;

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

    cout<<"Macro was successfull"<<endl;

    return 0;
}
