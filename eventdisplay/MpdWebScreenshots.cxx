// -------------------------------------------------------------------------
// -----                    MpdWebScreenshots source file             -----
// -------------------------------------------------------------------------
#include "MpdWebScreenshots.h"

#include "TEveManager.h"
#include "TGLViewer.h"
#include <TThread.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>

#include <iostream>
using namespace std;

// -----   Default constructor (private)  ----------------------------------
MpdWebScreenshots::MpdWebScreenshots()
  : FairTask("MpdWebScreenshots", 0)
{}

// -----   Destructor   ----------------------------------------------------
MpdWebScreenshots::~MpdWebScreenshots()
{}

// -----   Standard constructor   ------------------------------------------
MpdWebScreenshots::MpdWebScreenshots(const char* name, char* output_dir, bool isWebServer, Int_t iVerbose)
  : FairTask(name, iVerbose)
{
    outputDir = output_dir;
    outputDir += "/";

    isWebStarted = false;
    isWeb = isWebServer;
    if (isWebServer)
        return;

    if (strlen(outputDir) > 0)
    {
        struct stat st = {0};
        if (stat(outputDir, &st) == -1) mkdir(outputDir, 0700);

        cout<<"Images of the event display will be saved in directory: "<<outputDir<<endl;
    }
}

// -------------------------------------------------------------------------
InitStatus MpdWebScreenshots::Init()
{
    if (fVerbose > 1) cout<<"MpdWebScreenshots::Init()"<<endl;

    if (!IsActive())
        return kERROR;

    fMan = MpdEventManager::Instance();

    // start web server if required
    if (isWeb && (!isWebStarted))
    {
        // Set daemon to FALSE
        daemon = 0;

        www_thread_par* pPar = new www_thread_par();
        pPar->web_port = web_port;
        pPar->outputDir = outputDir;
        pPar->daemon = daemon;

        TThread* threadWebServ = new TThread((TThread::VoidFunc_t)&start_server, (void*)pPar, TThread::kNormalPriority);
        threadWebServ->Run();
        isWebStarted = true;
    }

    return kSUCCESS;
}

// -------------------------------------------------------------------------
void MpdWebScreenshots::Exec(Option_t* option)
{
    // redraw event display to capture current event
    gEve->Redraw3D();
    gSystem->ProcessEvents();

    TString fileName = outputDir;
    if (isMultiFiles == 1)
        fileName += "event_" + TString::Format("%d", fMan->GetCurrentEvent());
    else
        fileName += "event";

    if ((iFormatFiles == 0) || (iFormatFiles == 2))
    {
        fileName += ".png";
        gEve->GetDefaultGLViewer()->SavePicture(fileName.Data());
    }

    if (iFormatFiles > 0)
    {
        fileName += ".jpg";
        gEve->GetDefaultGLViewer()->SavePicture(fileName.Data());
    }

    return;
}

// -------------------------------------------------------------------------
void MpdWebScreenshots::SetParContainers()
{}

// -------------------------------------------------------------------------
void MpdWebScreenshots::Finish()
{}

// -------------------------------------------------------------------------
int MpdWebScreenshots::daemonize()
{
	pid_t pid;
	// already a daemon
	if (getppid() == 1)
		return -1;

	// Fork off the parent process
	pid = fork();
	if (pid < 0)
		return -2;

	// If we got a good PID, then we can exit the parent process.
	if (pid > 0)
		return 0;

	// At this point we are executing as the child process

	// Change the file mode mask
	umask(0);

	// Create a new SID for the child process
	if (setsid() < 0)
		return -3;

	// Change the current working directory. This prevents the current directory from being locked; hence not being able to remove it.
	if ((chdir("/")) < 0)
		return -4;

	return 0;
}

int MpdWebScreenshots::sendString(const char *message, int socket)
{
	return send(socket, message, strlen(message), 0);
}

void MpdWebScreenshots::sendHeader(const char* Status_code, char* Content_Type, int TotalSize, int socket)
{
    char* head = (char*)"\r\nHTTP/1.1 ";
    char* content_head = (char*)"\r\nContent-Type: ";
    char* server_head = (char*)"\r\nServer: PT06";
    char* length_head = (char*)"\r\nContent-Length: ";
    char* date_head = (char*)"\r\nDate: ";
    char* newline = (char*)"\r\n";

	time_t rawtime;
	time(&rawtime);

	char contentLength[100];
	sprintf(contentLength, "%i", TotalSize);

    char* message = (char*)malloc(
        (strlen(head) +
         strlen(content_head) +
         strlen(server_head) +
         strlen(length_head) +
         strlen(date_head) +
         strlen(newline) +
         strlen(Status_code) +
         strlen(Content_Type) +
         strlen(contentLength) +
         28 +
         sizeof(char)
         )
         * 2
    );

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

// send file
void MpdWebScreenshots::sendFile(FILE* fp, int connecting_socket)
{
	int current_char = 0;
	do
	{
		current_char = fgetc(fp);
		// send binary data
		int bytes_sent = send(connecting_socket, &current_char, sizeof(char), 0);
	}
	while (current_char != EOF);
}

int MpdWebScreenshots::scan(char* input, char* output, int start, int max)
{
	if (start >= strlen(input))
		return -1;

	int appending_char_count = 0;
	int count = 0;

	int i = start;
	for (; i < strlen(input); i++)
	{
        if (*(input + i) != '\t' && *(input + i) != ' ' && *(input + i) != '\n' && *(input + i) != '\r')
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
        if (*(input + i ) != '\t' && *(input + i) != ' ' && *(input + i) != '\n' && *(input + i) != '\r')
			break;
	}

	return i;
}

int MpdWebScreenshots::checkMime(char* extension, char* mime_type)
{
    char* current_word = (char*) malloc(600);
    char* word_holder = (char*) malloc(600);
    char* line = (char*) malloc(200);
	int startline = 0;

	TString mime_file = "mime.types";
	FILE* mimeFile = fopen(mime_file.Data(), "r");

	free(mime_type);
	mime_type = (char*)malloc(200);

	memset(mime_type,'\0',200);

	while (fgets(line, 200, mimeFile) != NULL)
	{
		if (line[0] != '#')
		{
			startline = scan(line, current_word, 0, 600);
			while (1)
			{
				startline = scan(line, word_holder, startline, 600);
				if (startline != -1)
				{
                    if (strcmp(word_holder, extension) == 0)
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

        memset(line, '\0', 200);
	}

	free(current_word);
	free(word_holder);
	free(line);

	return 0;
}

int MpdWebScreenshots::getHttpVersion(char* input, char* output)
{
    char* filename = (char*) malloc(100);
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

int MpdWebScreenshots::GetExtension(char* input, char* output, int max)
{
	int in_position = 0;
	int appended_position = 0;

	int i = 0, count = 0;
	for (; i < strlen(input); i++)
	{
		if (in_position == 1)
		{
			if (count < max)
			{
				output[appended_position] = input[i];
                appended_position += 1;
				count++;
			}
		}

        if (input[i] == '.')
            in_position = 1;
	}
	output[appended_position+1] = '\0';

	if (strlen(output) > 0)
		return 1;

	return -1;
}

// IF NOT EXISTS - RETURN -1. IF EXISTS - RETURN 1
int MpdWebScreenshots::handleHttpGET(char* input, TString output_dir, int connecting_socket)
{
    char* filename = (char*) malloc(200 * sizeof(char));
    char* path = (char*) malloc(1000 * sizeof(char));
    char* extension = (char*) malloc(10 * sizeof(char));
    char* mime = (char*) malloc(200 * sizeof(char));
    char* httpVersion = (char*) malloc(20 * sizeof(char));

    int contentLength = 0, mimeSupported = 0;

	memset(path, '\0', 1000);
	memset(filename, '\0', 200);
	memset(extension, '\0', 10);
	memset(mime, '\0', 200);
	memset(httpVersion, '\0', 20);

	int fileNameLength = scan(input, filename, 5, 200);

    int i = 0;
	while (filename[i]!='\0' && filename[i]!='?')
		i++;

	if (filename[i] == '?')
		filename[i] = '\0';
	
	if (fileNameLength <= 0)
		return -1;

	if (getHttpVersion(input, httpVersion) == -1)
	{
		sendString("501 Not Implemented\n", connecting_socket);
		return -1;
	}

	if (GetExtension(filename, extension, 10) == -1)
	{
		printf("File extension not existing");
		sendString("400 Bad Request\n", connecting_socket);

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
		sendString("400 Bad Request\n", connecting_socket);

		free(filename);
		free(mime);
		free(path);
		free(extension);
		return -1;
	}

	// Open the requesting file as binary
	strcpy(path, output_dir.Data());
	strcat(path, filename);

	FILE* fp = fopen(path, "rb");
	if (fp == NULL)
	{
		printf("\nUnable to open file %s\n",path);

		sendString("404 Not Found\n", connecting_socket);

		free(filename);
		free(mime);
		free(extension);
		free(path);
		return -1;
	}

	// Calculate Content Length
    fseek(fp, 0, SEEK_END);
    contentLength = ftell(fp);
    rewind(fp);
	if (contentLength < 0)
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
	sendHeader("200 OK", mime, contentLength, connecting_socket);

	sendFile(fp, connecting_socket);

	free(filename);
	free(mime);
	free(extension);
	free(path);
	fclose(fp);

	return 1;
}

// IF NOT VALID REQUEST - RETURN -1. IF VALID REQUEST - RETURN 1. IF GET - RETURN 2. IF HEAD - RETURN 0.
int MpdWebScreenshots::getRequestType(char* input)
{
	int type = -1;
	if (strlen(input) > 0)
		type = 1;

    char* requestType = (char*) malloc(5);
	scan(input, requestType, 0, 5);

    if (type == 1 && strcmp("GET", requestType) == 0)               type = 1;
    else if (type == 1 && strcmp("HEAD", requestType) == 0)         type = 2;
    else if (strlen(input) > 4 && strcmp("POST", requestType) == 0) type = 0;
    else type = -1;

	return type;
}

int MpdWebScreenshots::receive(int connecting_socket, TString output_dir)
{
	char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

	if ((recv(connecting_socket, buffer, BUFFER_SIZE, 0)) == -1)
	{
		printf("Error handling incoming request");
		return -1;
	}

	int request = getRequestType(buffer);
	if (request == 1)				// GET
		handleHttpGET(buffer, output_dir, connecting_socket);
	else
		if (request == 2)			// HEAD
			1; //SendHeader();
		else
			if (request == 0)		// POST
				sendString("501 Not Implemented\n", connecting_socket);
			else					// GARBAGE
				sendString("400 Bad Request\n", connecting_socket);

	return 1;
}

int MpdWebScreenshots::acceptConnection(int currentSocket, TString output_dir)
{
	sockaddr_storage connectorSocket;
	socklen_t addressSize = sizeof(connectorSocket);

	int connecting_socket = accept(currentSocket, (struct sockaddr *)&(connectorSocket), &addressSize);
	if (connecting_socket < 0)
	{
		perror("Accepting sockets");
		return -1;
	}

	// --- Workflow --- //
	// 1. Receive ( recv() ) the GET / HEAD
	// 2. Process the request and see if the file exists
	// 3. Read the file content
	// 4. Send out with correct mine and http 1.1
	if (receive(connecting_socket, output_dir) < 0)
	{
		perror("Receive");
		return -1;
	}

	close(connecting_socket);

	while (-1 != waitpid(-1, NULL, WNOHANG));

    return 0;
}

int MpdWebScreenshots::start(int webPort, TString output_dir)
{
	// Create a socket and assign currentSocket to the descriptor
	int currentSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (currentSocket == -1)
	{
		perror("Create socket");
		return -1;
	}

	// Bind to the currentSocket descriptor and listen to the port in PORT
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(webPort);

	if (bind(currentSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("Bind to port");
		return -2;
	}

	// Start listening for connections and accept no more than MAX_CONNECTIONS in the Quee
	if (listen(currentSocket, MAX_CONNECTIONS) < 0)
	{
		perror("Listen on port");
		return -3;
	}

	while (1)
		acceptConnection(currentSocket, output_dir);
}

int MpdWebScreenshots::start_server(void* ptr)
{
	www_thread_par* pPar = (www_thread_par*)ptr;
	int webPort = pPar->web_port;
	TString output_dir = pPar->outputDir;
	int daemon = pPar->daemon;

	int argc = 0;
	char** argv = NULL;

	TString log_file;
	for (int parameterCount = 1; parameterCount < argc; parameterCount++)
	{
		// If flag -p is used, set port
		if (strcmp(argv[parameterCount], "-p") == 0)
		{
			// Indicate that we want to jump over the next parameter
			parameterCount++;
			printf("Setting port to %i\n", atoi(argv[parameterCount]));
			webPort = atoi(argv[parameterCount]);
		}
		// If flag -d is used, set daemon to TRUE;
		else
		{
			if (strcmp(argv[parameterCount], "-d") == 0)
			{
				printf("Setting daemon = TRUE");
				daemon = 1;
			}
			else
			{
				if (strcmp(argv[parameterCount], "-l") == 0)
				{
					// Indicate that we want to jump over the next parameter
					parameterCount++;
					printf("Setting logfile = %s\n", argv[parameterCount]);
					log_file = (TString) (char*) argv[parameterCount];
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
	printf("Port:\t\t\t%i\n", webPort);
	printf("Server root:\t\t%s\n", output_dir.Data());
	printf("Logfile:\t\t%s\n", log_file.Data());
	printf("daemon:\t\t\t%i\n", daemon);

	if (daemon == 1)
		daemonize();

	start(webPort, output_dir);

	return 0;
}

ClassImp(MpdWebScreenshots)
