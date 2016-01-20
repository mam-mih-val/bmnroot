// -------------------------------------------------------------------------
// -----                 FairWebScreenshots header file                -----
// -----                Created 11/12/15  by K. Smirnov                -----
// ------------------------------------------------------------------------- 

#ifndef FAIRWEBSCREENSHOTS_H
#define FAIRWEBSCREENSHOTS_H

#include "FairTask.h"

#include "TString.h"
#include <TThread.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>

///////////////////////////////////////////////////////////////////////////////
#define BUFFER_SIZE 512
#define MAX_FILE_SIZE 1024*1024
#define MAX_CONNECTIONS 3
///////////////////////////////////////////////////////////////////////////////


class FairWebScreenshots : public FairTask
{
  protected:
    class Status
    {
     public:
        int port;
        int  daemon;
        TString* wwwRoot;
        TString* log_file;
        TString* mime_file;
        TString* outputDir;
        int currentSocket;
        int connecting_socket;
        
        bool isStarted;
        bool isNeedToStart;

        static Status* getStatus()
        {
            if (obj==NULL) obj = new Status();
            return obj;
        }

        friend class FairWebScreenshots;

     private:
        Status() { isStarted=false; }
        static Status* obj;
    };

  public:
    // Default constructor 
    FairWebScreenshots();

    // Standard constructor
    //*@param name        Name of task
    //*@outputDir         Output directory
    //*@param iVerbose    Verbosity level
    FairWebScreenshots(const char* name,const char* outputDir, Int_t iVerbose = 1);

    // Destructor 
    virtual ~FairWebScreenshots();

    // Set verbosity level. For this task and all of the subtasks. 
    void SetVerbose(Int_t iVerbose) { fVerbose = iVerbose; }
    // Executed task 
    virtual void Exec(Option_t* option);
    virtual InitStatus Init();
    virtual void SetParContainers();

    // Action after each event
    virtual void Finish();

  private:
    FairWebScreenshots(const FairWebScreenshots&);
    FairWebScreenshots& operator=(const FairWebScreenshots&);

    static void daemonize();
    static int sendString(const char *message, int socket);
    static int sendBinary(int *byte, int length);
    static void sendHeader(const char *Status_code, char *Content_Type, int TotalSize, int socket);
    static void sendHTML(char *statusCode, char *contentType, char *content, int size, int socket);
    static void sendFile(FILE *fp, int file_size);
    static int scan(char *input, char *output, int start, int max);
    static int checkMime(char *extension, char *mime_type);
    static int getHttpVersion(char *input, char *output);
    static int GetExtension(char *input, char *output, int max);
    static int Content_Lenght(FILE *fp);
    static int handleHttpGET(char *input);
    static int getRequestType(char *input);
    static int receive(int socket);
	//Handles the current connector
	static void handle(int socket);
	static void acceptConnection();
	static void start();
	static void initConfiguration();
	static int start_server(void * ptr);

 ClassDef(FairWebScreenshots,1);
};

#endif
