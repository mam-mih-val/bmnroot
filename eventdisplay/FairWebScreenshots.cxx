// -------------------------------------------------------------------------
// -----                    FairWebScreenshots source file             -----
// -------------------------------------------------------------------------
#include "FairWebScreenshots.h"

#include "FairEventManager.h"           // for FairEventManager
#include "FairRootManager.h"            // for FairRootManager

#include "TClonesArray.h"               // for TClonesArray
#include "TEveManager.h"                // for TEveManager, gEve
#include "TEvePathMark.h"               // for TEvePathMark
#include "TEveTrack.h"                  // for TEveTrackList, TEveTrack
#include "TEveTrackPropagator.h"        // for TEveTrackPropagator
#include "TEveVector.h"                 // for TEveVector, TEveVectorT
#include "TGeoTrack.h"                  // for TGeoTrack
#include "TMathBase.h"                  // for Max, Min
#include "TObjArray.h"                  // for TObjArray
#include "TParticle.h"                  // for TParticle

#include <string.h>
#include <iostream>

#include <TGLViewer.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <TThread.h>

using std::cout;
using std::endl;

FairWebScreenshots::Status* FairWebScreenshots::Status::obj=NULL;

// -----   Default constructor   -------------------------------------------
FairWebScreenshots::FairWebScreenshots()
  : FairTask("FairWebScreenshots", 0)
{
	Status::getStatus()->log_file = NULL;
	Status::getStatus()->isNeedToStart = true;
}

// -----   Standard constructor   ------------------------------------------
FairWebScreenshots::FairWebScreenshots(const char* name, const char* outputDir, Int_t iVerbose)
  : FairTask(name, iVerbose)
{
	Status::getStatus()->log_file = NULL;
	Status::getStatus()->isNeedToStart = false;

	char buffer[100];
	strcpy(buffer,outputDir);
	int len = strlen(buffer);

	if ((len > 0) && (buffer[len-1] == '/')) buffer[len-1] == '\0';

	Status::getStatus()->outputDir = new TString(buffer);
	if (len > 0)
	{
		char buf2[100];
		strcpy(buf2,"mkdir \"");
		strcat(buf2,buffer);
		strcat(buf2,"\"");
		printf("\n\ncmd:%s\n\n",buf2);
		system(buf2);
	}
}

// -------------------------------------------------------------------------
InitStatus FairWebScreenshots::Init()
{
    if ((Status::getStatus()->isNeedToStart) && (!Status::getStatus()->isStarted))
    {
        initConfiguration();

        TThread* threadWebServ = new TThread((TThread::VoidFunc_t)&start_server,NULL,TThread::kNormalPriority);
        threadWebServ->Run();

        Status::getStatus()->isStarted = true;
    }

    if (fVerbose > 1)
        cout<<"FairWebScreenshots::Init()"<<endl;

    if (!IsActive())
        return kERROR;

    return kSUCCESS;
}

// -------------------------------------------------------------------------
void FairWebScreenshots::Exec(Option_t* option)
{
	if(Status::getStatus()->isNeedToStart)
	{
		TString fileName = *(Status::getStatus()->wwwRoot) + "event" + ".jpg";
		gEve->GetDefaultGLViewer()->SavePicture(fileName.Data());

        fileName = *(Status::getStatus()->wwwRoot) + "event" + ".png";
        gEve->GetDefaultGLViewer()->SavePicture(fileName.Data());
    }
    else
    {
        TString fileName = *(Status::getStatus()->outputDir) + "/event" + ".jpg";
        gEve->GetDefaultGLViewer()->SavePicture(fileName.Data());

		fileName = *(Status::getStatus()->outputDir) + "/event" + ".png";
		gEve->GetDefaultGLViewer()->SavePicture(fileName.Data());
	}

	return;
}

// -----   Destructor   ----------------------------------------------------
FairWebScreenshots::~FairWebScreenshots()
{
}

// -------------------------------------------------------------------------
void FairWebScreenshots::SetParContainers()
{
}

// -------------------------------------------------------------------------
void FairWebScreenshots::Finish()
{
}

void FairWebScreenshots::daemonize()
{
	pid_t pid, sid;

	// already a daemon 
	if (getppid() == 1)
		return;

	// Fork off the parent process 
	pid = fork();
	if (pid < 0)
		exit(EXIT_FAILURE);

	// If we got a good PID, then we can exit the parent process. 
	if (pid > 0)
		exit(EXIT_SUCCESS);

	// At this point we are executing as the child process 

	// Change the file mode mask 
	umask(0);

	// Create a new SID for the child process 
	sid = setsid();
	if (sid < 0)
		exit(EXIT_FAILURE);

	// Change the current working directory.  This prevents the current
	//directory from being locked; hence not being able to remove it. 
	if ((chdir("/")) < 0)
		exit(EXIT_FAILURE);

	return;
}

int FairWebScreenshots::sendString(const char *message, int socket)
{
	int length = strlen(message);

	int bytes_sent = send(socket, message, length, 0);

	return bytes_sent;
}

int FairWebScreenshots::sendBinary(int *byte, int length)
{
	int bytes_sent = send(Status::getStatus()->connecting_socket, byte, length, 0);

	return bytes_sent;
}

void FairWebScreenshots::sendHTML(char *statusCode, char *contentType, char *content, int size, int socket)
{
	sendHeader(statusCode, contentType, size, socket);
	sendString(content, socket);

	return;
}

void FairWebScreenshots::sendHeader(const char *Status_code, char *Content_Type, int TotalSize, int socket)
{
	char *head = (char*)"\r\nHTTP/1.1 ";
	char *content_head = (char*)"\r\nContent-Type: ";
	char *server_head = (char*)"\r\nServer: PT06";
	char *length_head = (char*)"\r\nContent-Length: ";
	char *date_head = (char*)"\r\nDate: ";
	char *newline = (char*)"\r\n";

	time_t rawtime;
	time (&rawtime);

	char contentLength[100];
	sprintf(contentLength, "%i", TotalSize);

	char *message = (char*)malloc((
		strlen(head) +
		strlen(content_head) +
		strlen(server_head) +
		strlen(length_head) +
		strlen(date_head) +
		strlen(newline) +
		strlen(Status_code) +
		strlen(Content_Type) +
		strlen(contentLength) +
		28 +
		sizeof(char)) * 2);

	if (message != NULL)
	{
		strcpy(message, head);
		strcat(message, Status_code);
		strcat(message, content_head);
		strcat(message, Content_Type);
		strcat(message, server_head);
		strcat(message, length_head);
		strcat(message, contentLength);
		strcat(message, date_head);
		strcat(message, (char*)ctime(&rawtime));
		strcat(message, newline);

		sendString(message, socket);

		free(message);
	}
}

void FairWebScreenshots::sendFile(FILE *fp, int file_size)
{
	int current_char = 0;

	do{
		current_char = fgetc(fp);
		sendBinary(&current_char, sizeof(char));
	}
	while(current_char != EOF);
}

int FairWebScreenshots::scan(char *input, char *output, int start, int max)
{
	if (start >= strlen(input))
		return -1;

	int appending_char_count = 0;
	int i = start;
	int count = 0;

	for (; i < strlen(input); i++)
	{
		if ( *(input + i) != '\t' && *(input + i) != ' ' && *(input + i) != '\n' && *(input + i) != '\r')
		{
			if(count < (max-1))
			{
				*(output + appending_char_count) = *(input + i ) ;
				appending_char_count += 1;

				count++;
			}		
		}	
		else
			break;
	}
	*(output + appending_char_count) = '\0';	

	// Find next word start
	i += 1;

	for (; i < strlen(input); i++)
	{
		if ( *(input + i ) != '\t' && *(input + i) != ' ' && *(input + i) != '\n' && *(input + i) != '\r')
			break;
	}

	return i;
}

int FairWebScreenshots::checkMime(char *extension, char *mime_type)
{
	char *current_word = (char*)malloc(600);
	char *word_holder = (char*)malloc(600);
	char *line = (char*)malloc(200);
	int startline = 0;

	FILE *mimeFile = fopen(Status::getStatus()->mime_file->Data(), "r");

	free(mime_type);

	mime_type = (char*)malloc(200);

	memset(mime_type,'\0',200);

	while(fgets(line, 200, mimeFile) != NULL)
	{
		if (line[0] != '#')
		{
			startline = scan(line, current_word, 0, 600);
			while (1)
			{
				startline = scan(line, word_holder, startline, 600);
				if (startline != -1)
				{
					if (strcmp ( word_holder, extension ) == 0)
					{
						memcpy(mime_type, current_word, strlen(current_word));
						free(current_word);
						free(word_holder);
						free(line);
						return 1;	
					}
				}
				else
					break;
			}
		}

		memset (line, '\0', 200);
	}

	free(current_word);
	free(word_holder);
	free(line);

	return 0;
}

int FairWebScreenshots::getHttpVersion(char *input, char *output)
{
	char *filename = (char*)malloc(100);
	int start = scan(input, filename, 4, 100);
	if (start > 0)
	{
		if (scan(input, output, start, 20))
		{
			output[strlen(output)+1] = '\0';

			if (strcmp("HTTP/1.1" , output) == 0)
				return 1;
			else
			{
				if (strcmp("HTTP/1.0", output) == 0)
					return 0;
				else
					return -1;
			}
		}
		else
			return -1;
	}

	return -1;
}

int FairWebScreenshots::GetExtension(char *input, char *output, int max)
{
	int in_position = 0;
	int appended_position = 0;
	int i = 0;
	int count = 0;

	for (; i < strlen(input); i++)
	{		
		if ( in_position == 1 )
		{
			if(count < max)
			{
				output[appended_position] = input[i];
				appended_position +=1;
				count++;
			}
		}

		if ( input[i] == '.' )
			in_position = 1;
	}
	output[appended_position+1] = '\0';

	if (strlen(output) > 0)
		return 1;

	return -1;
}

int FairWebScreenshots::Content_Lenght(FILE *fp)
{
	fseek(fp, 0, SEEK_END);
	int filesize = ftell(fp);
	rewind(fp);

	return filesize;
}

// IF NOT EXISTS - RETURN -1. IF EXISTS - RETURN 1
int FairWebScreenshots::handleHttpGET(char *input)
{
	char *filename = (char*)malloc(200 * sizeof(char));
	char *path = (char*)malloc(1000 * sizeof(char));
	char *extension = (char*)malloc(10 * sizeof(char));
	char *mime = (char*)malloc(200 * sizeof(char));
	char *httpVersion = (char*)malloc(20 * sizeof(char));

	int contentLength = 0;
	int mimeSupported = 0;

	memset(path, '\0', 1000);
	memset(filename, '\0', 200);
	memset(extension, '\0', 10);
	memset(mime, '\0', 200);
	memset(httpVersion, '\0', 20);

	int fileNameLength = scan(input, filename, 5, 200);

	int i=0;
	while (filename[i]!='\0' && filename[i]!='?')
		i++;

	if (filename[i]=='?')
		filename[i]='\0';
	
	if (fileNameLength <= 0)
		return -1;

	if (getHttpVersion(input, httpVersion) == -1)
	{
		sendString("501 Not Implemented\n", Status::getStatus()->connecting_socket);
		return -1;
	}

	if (GetExtension(filename, extension, 10) == -1)
	{
		printf("File extension not existing");

		sendString("400 Bad Request\n", Status::getStatus()->connecting_socket);

		free(filename);
		free(mime);
		free(path);
		free(extension);

		return -1;
	}

	mimeSupported = checkMime(extension, mime);
	if (mimeSupported != 1)
	{
		printf("Mime not supported");

		sendString("400 Bad Request\n", Status::getStatus()->connecting_socket);

		free(filename);
		free(mime);
		free(path);
		free(extension);

		return -1;
	}

	// Open the requesting file as binary
	strcpy(path, Status::getStatus()->wwwRoot->Data());
	strcat(path, filename);

	FILE* fp = fopen(path, "rb");
	if (fp == NULL)
	{
		printf("\nUnable to open file%s\n",path);

		sendString("404 Not Found\n", Status::getStatus()->connecting_socket);

		free(filename);
		free(mime);
		free(extension);
		free(path);

		return -1;
	}

	// Calculate Content Length
	contentLength = Content_Lenght(fp);
	if (contentLength  < 0 )
	{
		printf("File size is zero");

		free(filename);
		free(mime);
		free(extension);
		free(path);

		fclose(fp);

		return -1;
	}

	// Send File Content
	sendHeader("200 OK", mime,contentLength, Status::getStatus()->connecting_socket);

	sendFile(fp, contentLength);

	free(filename);
	free(mime);
	free(extension);
	free(path);

	fclose(fp);

	return 1;
}

// IF NOT VALID REQUEST - RETURN -1. IF VALID REQUEST - RETURN 1. IF GET - RETURN 2. IF HEAD - RETURN 0.
int FairWebScreenshots::getRequestType(char *input)
{
	int type = -1;
	if (strlen(input) > 0)
		type = 1;

	char *requestType = (char *)malloc(5);
	scan(input, requestType, 0, 5);

	if (type == 1 && strcmp("GET", requestType) == 0)
		type = 1;
	else
		if (type == 1 && strcmp("HEAD", requestType) == 0)
			type = 2;
		else
			if (strlen(input) > 4 && strcmp("POST", requestType) == 0)
				type = 0;
			else
				type = -1;

	return type;
}

int FairWebScreenshots::receive(int socket)
{
	int msgLen = 0;
	char buffer[BUFFER_SIZE];

	memset(buffer,'\0', BUFFER_SIZE);

	if ((msgLen = recv(socket, buffer, BUFFER_SIZE, 0)) == -1)
	{
		printf("Error handling incoming request");
		return -1;
	}

	int request = getRequestType(buffer);
	if (request == 1)				// GET
		handleHttpGET(buffer);
	else
		if (request == 2)			// HEAD
			1;//SendHeader();
		else
			if (request == 0)		// POST
				sendString("501 Not Implemented\n", Status::getStatus()->connecting_socket);
			else					// GARBAGE
				sendString("400 Bad Request\n", Status::getStatus()->connecting_socket);

	return 1;
}

void FairWebScreenshots::handle(int socket)
{
	// --- Workflow --- //
	// 1. Receive ( recv() ) the GET / HEAD
	// 2. Process the request and see if the file exists
	// 3. Read the file content
	// 4. Send out with correct mine and http 1.1

	if (receive((int)socket) < 0)
	{
		perror("Receive");
		exit(-1);
	}
}

void FairWebScreenshots::acceptConnection()
{
	sockaddr_storage connectorSocket;

	socklen_t addressSize = sizeof(connectorSocket);

	Status::getStatus()->connecting_socket = accept(Status::getStatus()->currentSocket, (struct sockaddr *)&(connectorSocket), &addressSize);
	if ( Status::getStatus()->connecting_socket < 0 )
	{
		perror("Accepting sockets");
		exit(-1);
	}

	handle(Status::getStatus()->connecting_socket);

	close(Status::getStatus()->connecting_socket);

	while (-1 != waitpid(-1, NULL, WNOHANG));
}

void FairWebScreenshots::start()
{
	// Create a socket and assign currentSocket to the descriptor
	Status::getStatus()->currentSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (Status::getStatus()->currentSocket == -1)
	{
		perror("Create socket");
		exit(-1);
	}

	// Bind to the currentSocket descriptor and listen to the port in PORT
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(Status::getStatus()->port);

	if (bind(Status::getStatus()->currentSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("Bind to port");
		exit(-1);
	}

	// Start listening for connections and accept no more than MAX_CONNECTIONS in the Quee
	if (listen(Status::getStatus()->currentSocket, MAX_CONNECTIONS) < 0)
	{
		perror("Listen on port");
		exit(-1);
	}

	while (1)
		acceptConnection();
}

void FairWebScreenshots::initConfiguration()
{
    char* currentLine = (char*)malloc(100);
	char* cwwwRoot = (char*)malloc(100);
	char* cconfFile = (char*)malloc(100);
	char* clog_file = (char*)malloc(100);
	char* cmime_file = (char*)malloc(600);

	// Setting default values
	cconfFile = (char*)"httpd.conf";
	clog_file = (char*)".log";
	Status::getStatus()->log_file = new TString(clog_file);
	strcpy(cmime_file, "mime.types");
	Status::getStatus()->mime_file = new TString(cmime_file);
	// Set  daemon to FALSE
	Status::getStatus()-> daemon = 0;

	FILE* filePointer = fopen(cconfFile, "r");
	// Ensure that the configuration file is open
	if (filePointer == NULL)
	{
		fprintf(stderr, "Can't open configuration file!\n");
		exit(1);
	}

	// Get server root directory from configuration file
	if (fscanf(filePointer, "%s %s", currentLine, cwwwRoot) != 2)
	{
		fprintf(stderr, "Error in configuration file on line 1!\n");
		exit(1);
	}
	
	//add '/' if it needs
	int len = strlen(cwwwRoot);
	if ((len > 0) && (cwwwRoot[len-1] != '/'))
		strcat(cwwwRoot,"/");
	
	Status::getStatus()->wwwRoot = new TString(cwwwRoot);
	
	// Get default port from configuration file
	if (fscanf(filePointer, "%s %i", currentLine, &(Status::getStatus()->port)) != 2)
	{
		fprintf(stderr, "Error in configuration file on line 2!\n");
		exit(1);
	}

	fclose(filePointer);
	free(currentLine);
}

int FairWebScreenshots::start_server(void * ptr)
{
	int argc = 0;
	char** argv = NULL;

	for (int parameterCount = 1; parameterCount < argc; parameterCount++)
	{
		// If flag -p is used, set port
		if (strcmp(argv[parameterCount], "-p") == 0)
		{
			// Indicate that we want to jump over the next parameter
			parameterCount++;
			printf("Setting port to %i\n", atoi(argv[parameterCount]));
			Status::getStatus()->port = atoi(argv[parameterCount]);
		}
		// If flag -d is used, set daemon to TRUE;
		else
		{
			if (strcmp(argv[parameterCount], "-d") == 0)
			{
				printf("Setting daemon = TRUE");
				Status::getStatus()-> daemon = 1;
			}
			else
			{
				if (strcmp(argv[parameterCount], "-l") == 0)
				{
					// Indicate that we want to jump over the next parameter
					parameterCount++;
					printf("Setting logfile = %s\n", argv[parameterCount]);
					Status::getStatus()->log_file = new TString((char*)argv[parameterCount]);
				}
				else
				{
					printf("Usage: %s [-p port] [-d] [-l logfile]\n", argv[0]);
					printf("\t\t-p port\t\tWhich port to listen to.\n");
					printf("\t\t-d\t\tEnables  daemon mode.\n");
					printf("\t\t-l logfile\tWhich file to store the log to.\n");
					return -1;
				}
			}
		}
	}

	printf("Settings:\n");
	printf("Port:\t\t\t%i\n", Status::getStatus()->port);
	printf("Server root:\t\t%s\n", Status::getStatus()->wwwRoot->Data());
	printf("Logfile:\t\t%s\n", Status::getStatus()->log_file->Data());
	printf(" daemon:\t\t\t%i\n", Status::getStatus()-> daemon);

	if (Status::getStatus()-> daemon == 1)
		daemonize();

	start();

	return 0;
}

ClassImp(FairWebScreenshots)
