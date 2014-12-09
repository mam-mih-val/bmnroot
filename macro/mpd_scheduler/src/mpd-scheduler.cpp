//============================================================================
// Name        : mpd-scheduler.cpp
// Author      : Konstantin Gertsenberger
// Copyright   : JINR
// Description : Scheduler for MPDRoot
//============================================================================

// ROOT includes for DB work
#include "TSQLServer.h"
#include "TSQLResult.h"
#include "TSQLRow.h"

// C++ includes
#include "sys/wait.h"
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "libxml/tree.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <sstream>
#include <algorithm>

using namespace std;

enum enumSchedulerName {Torque};
enumSchedulerName scheduler_name;

const int MAX_BUFFER = 255;
char buffer[MAX_BUFFER];

// files to process with 'start event', 'event count' and 'parallel_mode' parameters
struct structFilePar{
	string strFileIn;
	string strFileOut;
	int* start_event;
	int* count_event;
	string strParallelMode;
	int iParallelMode;
	bool isMerge;
};

// structure for parameters to transfer for the threads
struct structThreadPar{
	string sConfig;
	string macro_name;

	string inFile;
	string outFile;
	int* start_event;
	int* count_event;
	int parallel_mode;
	bool isMerge;

	string add_args;
	string logs;

	sem_t sem;
	pthread_mutex_t mut;
	int counter;
};

// structure for parameters to transfer for the subthreads
struct structSubThreadPar{
	structThreadPar* thread_parameter;
	int start_event;
	int count_event;
	string outFile;
	int counter2;
};

/*void UnionResulFiles(string[] source_files, string result_file, bool isFullMerge)
{
	    //addition result files for "cbmsim" Tree
	    TChain chainUnion("cbmsim");
	    TString sResultFile;
	    int i=0, beg=0, countFiles=0;

	    while (pcFileList[i] != '\0'){
	        if ((pcFileList[i] == ' ') && (beg != i)){
	            pcFileList[i] = '\0';

	            if (countFiles == 0)
	                sResultFile = &pcFileList[beg];
	            else
	                chainUnion.Add(&pcFileList[beg]);

	            pcFileList[i] = ' ';
	            beg = i+1;
	            countFiles++;
	        }

	        i++;
	    }
	    if (beg != i){
	        if (countFiles == 0)
	            sResultFile = &pcFileList[beg];
	        else
	            chainUnion.Add(&pcFileList[beg]);

	        countFiles++;
	    }

	    //merge|write result files
	    if (countFiles > 1){
	        if (mode == 1) chainUnion.Merge(sResultFile);
	        else{
	            TFile fChain(sResultFile, "RECREATE");
	            chainUnion.Write();
	            fChain.Close();
	        }

	        Int_t events = chainUnion.GetEntries();
	        if (mode == 1) cout<<"The Chain witn "<<events<<" event(s) was merged to file \""<<sResultFile<<"\" from following files:"<<endl;
	        else cout<<"The Chain witn "<<events<<" event(s) was written to file \""<<sResultFile<<"\" to point following files:"<<endl;

	        TObjArray *fileElements = chainUnion.GetListOfFiles();
	        TIter next(fileElements);
	        TChainElement *chEl=0;
	        while (( chEl=(TChainElement*)next() )) {
	            char* pc = chEl->GetTitle();
	            cout<<pc<<endl;
	        }
	    }

	    //test reading result file
	    TChain chainRead("cbmsim");
	    chainRead.Add(sResultFile);

	    Int_t events = chainRead.GetEntries();
	    cout<<"The count of events in test reading is equal "<<events<<endl;
	}
}*/

int syscommand(string aCommand, string& result)
{
    FILE* f;
    if (!(f = popen( aCommand.c_str(), "r" )))
    {
    	cout << "Can not open file" << endl;
        return 1;
    }

    const int BUFSIZE = 4096;
    char buf[ BUFSIZE ];
    if (fgets(buf,BUFSIZE,f)!=NULL)
    	result = buf;

    pclose(f);
    return 0;
}

string getBundleName()
{
    pid_t procpid = getpid();
    stringstream toCom;
    toCom << "cat /proc/" << procpid << "/comm";
    string fRes="";
    syscommand(toCom.str(),fRes);
    size_t last_pos = fRes.find_last_not_of(" \n\r\t") + 1;
    if (last_pos != string::npos) {
        fRes.erase(last_pos);
    }
    return fRes;
}

string getBundlePath()
{
    pid_t procpid = getpid();
    string appName = getBundleName();
    stringstream command;
    command <<  "readlink /proc/" << procpid << "/exe | sed \"s/\\(\\/" << appName << "\\)$//\"";
    string fRes;
    syscommand(command.str(),fRes);
    return fRes;
}

// event process in working thread
void* SubThreadProcessFile(void* thread_parameter)
{
	// initialization of variables
	structSubThreadPar* thread_param = (structSubThreadPar*) thread_parameter;
	structThreadPar* thread_par = thread_param->thread_parameter;

	string sConfig = thread_par->sConfig;
	string macro_name = thread_par->macro_name;

	string inFile = thread_par->inFile;

	string add_args = thread_par->add_args;
	string logs = thread_par->logs;

	int start_event = thread_param->start_event;
	int count_event = thread_param->count_event;
	string outFile = thread_param->outFile;

	int counter = thread_par->counter;
	int counter2 = thread_param->counter2;

	// generate task string
	int lenSUB = 100+macro_name.length()+inFile.length()+outFile.length()+sConfig.length()+add_args.length();
	char* pcSUB = new char[lenSUB];

	sprintf (pcSUB, "%sroot -b -q \"%s(", sConfig.c_str(), macro_name.c_str());
	bool isFirst = true;
	if (inFile != "")
	{
		sprintf(pcSUB, "%s\\\"%s\\\"", pcSUB, inFile.c_str());
		isFirst = false;
	}

	if (!isFirst) sprintf(pcSUB, "%s, ", pcSUB);
	else isFirst = false;
	sprintf(pcSUB, "%s\\\"%s\\\"", pcSUB, outFile.c_str());

	if (!isFirst) sprintf(pcSUB, "%s, ", pcSUB);
	else isFirst = false;
	sprintf(pcSUB, "%s%d", pcSUB, start_event);

	if (!isFirst) sprintf(pcSUB, "%s, ", pcSUB);
	else isFirst = false;
	sprintf(pcSUB, "%s%d", pcSUB, count_event);

	if (add_args != ""){
		if (!isFirst) sprintf(pcSUB, "%s, ", pcSUB);
		else isFirst = false;
		sprintf(pcSUB, "%s\\\"%s\\\"", pcSUB, add_args.c_str());
	}
	if (logs != "")
		sprintf (pcSUB, "%s)\" > %s_%d_%d 2>&1", pcSUB, logs.c_str(), counter, counter2);
	else
		//sprintf (pcSUB, "%s)\" > /dev/null 2>&1", pcSUB);
		sprintf (pcSUB, "%s)\"", pcSUB);

	// write and execute temporary bash file
	ofstream myfile;
	char temp_file[255], run_temp[255];
	long int t = time(NULL);
	sprintf(temp_file, "temp_%d_%d_%d.sh", counter, counter2, (int)t); // generate output name for temporary bash file
	myfile.open (temp_file);
	myfile<<pcSUB;
	myfile.close();
	sprintf(run_temp, "bash %s", temp_file);

	pthread_mutex_lock(&thread_par->mut);
	cout<<"Subtask is running:"<<endl<<"input - "<<inFile<<endl<<"output - "<<outFile<<endl<<"start event - "<<start_event<<endl<<"event count - "<<count_event<<endl<<endl;
	pthread_mutex_unlock(&thread_par->mut);

	// run task in thread
	system(run_temp);

	delete [] pcSUB;

	//delete temporary bash file
	sprintf(run_temp, "rm %s", temp_file);
	system(run_temp);

	sem_post(&thread_par->sem);

	pthread_exit(NULL);

	return NULL;
}

// event process in working thread
void* ThreadProcessFile(void* thread_parameter)
{
	// initialization of variables
	structThreadPar* thread_par = (structThreadPar*) thread_parameter;

	string sConfig = thread_par->sConfig;
	string macro_name = thread_par->macro_name;

	string inFile = thread_par->inFile;
	string outFile = thread_par->outFile;
	int* start_event = thread_par->start_event;
	int* count_event = thread_par->count_event;
	int parallel_mode = thread_par->parallel_mode;

	string add_args = thread_par->add_args;
	string logs = thread_par->logs;

	int counter = thread_par->counter;

	// if parallel mode > 1 - parallel event processing for one input file
	if (parallel_mode > 1)
	{
		pthread_t* threads = new pthread_t[parallel_mode];
		int real_thread_count = 0;

		string sUnion = outFile;
		int len = sUnion.length();
		int start = *start_event;
		int count = *count_event;
		int rc;
		for (int i=0; i < parallel_mode; i++)
		{
			int event_per_thread = (count+i)/parallel_mode;

		    if (event_per_thread != 0)
		    {
		    	string sOutFile = outFile;

		        // generate output file name
		        char add_comma[9];
		        sprintf(add_comma, "%d", real_thread_count);
		        sOutFile.insert(len, add_comma);

		        structSubThreadPar* par = new structSubThreadPar();
		        par->thread_parameter = thread_par;
		        par->start_event = start;
		        par->count_event = event_per_thread;
		        par->outFile = sOutFile;
		        par->counter2 = i+1;

		        sem_wait(&thread_par->sem);

		        rc = pthread_create(&threads[real_thread_count], NULL, SubThreadProcessFile, (void*)par);
		        if (rc){
		        	printf("ERROR; return code from sub pthread_create() is %d\n", rc);
		            exit(-1);
		        }

		        sUnion += " " + sOutFile;

		        real_thread_count++;
		    }//if (event_per_thread != 0){

		    start = start + event_per_thread;
		}//for

		// waiting for finishing of the child threads
		//cout<<"Waiting for "<<real_thread_count<<" subtask(s) to finish..."<<endl;
		void **thread_return = NULL;
		for (int i=0; i < real_thread_count; i++)
		{
			pthread_join(threads[i], thread_return);

		    if (thread_return != NULL)
		    	cout<<"Thread " << i << " failed" << endl;
		}

		// merge result files or write to single TChain object
		if (thread_par->isMerge)
		{
			string exe_dir = getBundlePath();
			exe_dir = exe_dir.erase(exe_dir.length()-1, 1) + "/union.C";
			char* pcSUB = new char[44+sUnion.length()+sConfig.length()+exe_dir.length()+logs.length()];
		    sprintf (pcSUB, "%sroot -b -q \"%s(\\\"%s\\\", %d)\"", sConfig.c_str(), exe_dir.c_str(),
		    		sUnion.c_str(), thread_par->isMerge);

		    if (logs != "")
		    	sprintf (pcSUB, "%s >> %s 2>&1", pcSUB, logs.c_str());

		    // run union.C macro in bash
		    ofstream myfile;
		    char temp_file[255], run_temp[255];
		    long int t = time(NULL);
		    sprintf(temp_file, "temp_union_%d.sh", (int)t);
		    myfile.open (temp_file);
		    myfile << pcSUB;
		    myfile.close();
		    sprintf(run_temp, "bash %s", temp_file);

		    cout<<endl<<"Merging the result of subtasks..."<<endl<<endl;
		    system(run_temp);

		    delete [] pcSUB;

		    // delete temporary bash file
		    sprintf(run_temp, "rm %s", temp_file);
		    system(run_temp);

			//UnionROOTFiles();
		}//if - merge result files
	}//if (parallel_mode > 1)
	else{// parallel_mode = 1
		// generate task string
		int lenSUB = 100+macro_name.length()+inFile.length()+outFile.length()+sConfig.length()+add_args.length();
		char* pcSUB = new char[lenSUB];

		sprintf (pcSUB, "%sroot -b -q \"%s(", sConfig.c_str(), macro_name.c_str());
		bool isFirst = true;
		if (inFile != "")
		{
			sprintf(pcSUB, "%s\\\"%s\\\"", pcSUB, inFile.c_str());
			isFirst = false;
		}
		if (outFile != ""){
			if (!isFirst) sprintf(pcSUB, "%s, ", pcSUB);
			else isFirst = false;
			sprintf(pcSUB, "%s\\\"%s\\\"", pcSUB, outFile.c_str());
		}
		if (start_event != NULL)
		{
			if (!isFirst) sprintf(pcSUB, "%s, ", pcSUB);
			else isFirst = false;
			sprintf(pcSUB, "%s%d", pcSUB, *start_event);
		}
		if (count_event != NULL)
		{
			if (!isFirst) sprintf(pcSUB, "%s, ", pcSUB);
			else isFirst = false;
			sprintf(pcSUB, "%s%d", pcSUB, *count_event);
		}
		if (add_args != ""){
			if (!isFirst) sprintf(pcSUB, "%s, ", pcSUB);
			else isFirst = false;
			sprintf(pcSUB, "%s\\\"%s\\\"", pcSUB, add_args.c_str());
		}
		if (logs != ""){
			//if (counter == 0)
			//	sprintf (pcSUB, "%s)\" >> %s 2>&1", pcSUB, logs.c_str());
			//else
				sprintf (pcSUB, "%s)\" > %s_%d 2>&1", pcSUB, logs.c_str(), counter);
		}
		else
			//sprintf (pcSUB, "%s)\" > /dev/null 2>&1", pcSUB);
			sprintf (pcSUB, "%s)\"", pcSUB);

		// write and execute temporary bash file
		ofstream myfile;
		char temp_file[255], run_temp[255];
		long int t = time(NULL);
		sprintf(temp_file, "temp_%d_%d.sh", counter, (int)t); // generate output name for temporary bash file
		myfile.open (temp_file);
		myfile<<pcSUB;
		myfile.close();
		sprintf(run_temp, "bash %s", temp_file);

		pthread_mutex_lock(&thread_par->mut);
		cout<<"Task is running:"<<endl<<"input - "<<inFile<<endl<<"output - "<<outFile<<endl<<endl;
		pthread_mutex_unlock(&thread_par->mut);

		// run task in thread
		system(run_temp);

		delete [] pcSUB;

		//delete temporary bash file
		sprintf(run_temp, "rm %s", temp_file);
		system(run_temp);

		sem_post(&thread_par->sem);
	}//else

	pthread_exit(NULL);

	return NULL;
}

string IntToStr(int number)
{
   stringstream ss;
   ss << number;
   return ss.str();
}

// clear vector parameters
void clearVector(vector<structFilePar*>* vecFiles){
	for (unsigned int i = 0; i < vecFiles->size(); i++){
		structFilePar* filePar = vecFiles->at(i);
		delete filePar;
	}
}

// get file name without extension from path
string get_file_name(string path){
	// Remove directory if present.
	size_t last_slash_idx = path.find_last_of("/");
	if (string::npos != last_slash_idx)
	{
	    path.erase(0, last_slash_idx + 1);
	}

	// Remove extension if present.
	size_t period_idx = path.rfind('.');
	if (std::string::npos != period_idx)
	{
	    path.erase(period_idx);
	}

	return path;
}

// get file name with extension from path
string get_file_name_with_ext(string path){
	// Remove directory if present.
	size_t last_slash_idx = path.find_last_of("/");
	if (string::npos != last_slash_idx)
	{
	    path.erase(0, last_slash_idx + 1);
	}

	return path;
}

// replace string 's' by string 'd' in text
void replace(string &text, string s, string d)
{
	int start = -1;

	do
	{
		start = text.find(s, start + 1);
	    if(start > -1)
	    	text.replace(start, s.length(), d.c_str());
	}
	while(start > -1);
}

// form output file's name from $ variable
string form_file_name(string outName, string inName, string strCounter){
	replace(outName, "${input}", inName);
	replace(outName, "${file_name}", get_file_name(inName));
	replace(outName, "${file_name_with_ext}", get_file_name_with_ext(inName));
	replace(outName, "${counter}", strCounter);

	return outName;
}

// get maximum available processor count (local - one machine, global - cluster)
// isGlobal: false - one machine (local), true - cluster scheduler (global)
int GetProcessorCount(bool isGlobal)
{
	int proc_count = 0;

	if (isGlobal){
		switch (scheduler_name)
		{
			case Torque:
			{
				// define count of processors for all.q queue
				string data, strDiff;
				FILE* stream = popen("qconf -sq all.q | grep slots", "r");
				while (fgets(buffer, MAX_BUFFER, stream) != NULL)
					data.append(buffer);
				pclose(stream);

				size_t found = data.find("="), found2 = string::npos;
				while (found != string::npos)
				{
					found2 = data.find("]", found);
					strDiff = data.substr(found+1, found2 - found - 1);
					proc_count += atoi(strDiff.c_str());
					strDiff.clear();

					found = data.find("=", found2);
				}

				data.clear();

				break;
			}//case Torque
		}//switch (scheduler_name)
	}//if (isGlobal)
	else
	{
		// define count of machine's cores
		proc_count = sysconf(_SC_NPROCESSORS_ONLN);
	}

	return proc_count;
}

int main(int argc, char** argv){
	if (argc < 2){
		cout << "first parameter: XML file name wasn't set" << endl;
        return 1;
	}

    // pointer to XML document
	xmlDocPtr doc = xmlReadFile(argv[1], NULL, 0);
	if(doc == NULL){
		cout<<"Error open XML file: "<<argv[1]<<endl;
		return 2;
	}

    // get root element
	xmlNodePtr root = xmlDocGetRootElement(doc);

	// temporary variables
	char* pcSUB;
	string data;

	// variables for DB
	bool isGen = false, isEnergy = false, isMaxEnergy = false, isParts = false, isDesc = false, isType = false;
	string strGen, strParts, strDesc, strType;
	double fEnergy, fMaxEnergy;

    // cycle for all jobs
	xmlNodePtr cur_node = root;
	while (cur_node){
        // if tag means JOB
        if (cur_node->type == XML_ELEMENT_NODE){
        	if (strcmp((char*)cur_node->name, "job") == 0){
        		bool isLocal = true, isCommand = false, isError = false;
        		char* macro_name=NULL, *outFile=NULL, *inFile=NULL, *inDB=NULL, *pcStart_event=NULL, *pcCount_event=NULL,
                    *mode=NULL, *sproc_count=NULL, *pcConfig=NULL, *merge=NULL, *parallel_mode=NULL, *add_args=NULL, *command_line=NULL,
                    *lcStart_event=NULL, *lcCount_event=NULL, *pcLogs=NULL;
        		int* start_event = NULL, *count_event = NULL, *lc_start_event = NULL, *lc_count_event = NULL;
        		vector<structFilePar*> vecFiles;

                xmlNodePtr sub_node = cur_node->children;

                // PARSING TAGS of the JOB
                while (sub_node){
                	if (sub_node->type == XML_ELEMENT_NODE){
                		// COMMAND LINE TAG
                		if (strcmp((char*)sub_node->name, "command") == 0){
                			if (command_line != NULL)
                				cout<<"Warning: command line is reassigned"<<endl;

                		    command_line = (char*) xmlGetProp(sub_node, (unsigned char*)"line");
                		    isCommand = true;

                		    sub_node = sub_node->next;
                		    continue;
                		}
                		// MACRO NAME TAG
                        if (strcmp((char*)sub_node->name, "macro") == 0){
                        	if (macro_name != NULL)
                        		cout<<"Warning: macro's name is reassigned"<<endl;

                        	macro_name = (char*) xmlGetProp(sub_node, (unsigned char*)"name");

                        	pcStart_event = (char*) xmlGetProp(sub_node, (unsigned char*)"start_event");
                        	pcCount_event = (char*) xmlGetProp(sub_node, (unsigned char*)"count_event");

                        	// check global start parameter
                        	if (pcStart_event != NULL){
                        		start_event = new int;
                        	    *start_event = atoi(pcStart_event);
                        	}
                        	else{
                        		start_event = NULL;
                        	}
                        	// check global count parameter
                        	if (pcCount_event != NULL){
                        		count_event = new int;
                        	     *count_event = atoi(pcCount_event);
                        	     if (*count_event <= 0)
                        	     {
                        	    	 isError = true;
                        	         cout<<"Error: event count must be a number greater than zero!"<<endl;

                        	         cur_node = cur_node->next;
                        	         continue;
                        	     }
                        	}
                        	else{
                        		count_event = NULL;
                        	}

                        	add_args = (char*) xmlGetProp(sub_node, (unsigned char*)"add_args");

                            sub_node = sub_node->next;
                            continue;
                        }
                        // FILE TAG
                        if (strcmp((char*)sub_node->name, "file") == 0){
                        	inFile = (char*) xmlGetProp(sub_node, (unsigned char*)"input");
                        	outFile = (char*) xmlGetProp(sub_node, (unsigned char*)"output");

                        	lcStart_event = (char*) xmlGetProp(sub_node, (unsigned char*)"start_event");
                        	lcCount_event = (char*) xmlGetProp(sub_node, (unsigned char*)"count_event");

                        	// check start parameter
                        	if (lcStart_event != NULL){
                        		lc_start_event = new int;
                        	    *lc_start_event = atoi(lcStart_event);
                        	}
                        	else{
                        		lc_start_event = NULL;
                        	}
                        	// check count parameter
                        	if (lcCount_event != NULL){
                        		lc_count_event = new int;
                        	    *lc_count_event = atoi(lcCount_event);
                        	    if (*lc_count_event <= 0)
                        	    {
                        	    	isError = true;
                        	        cout<<"Error: event count must be a number greater than zero!"<<endl;

                        	        cur_node = cur_node->next;
                        	        continue;
                        	    }
                        	}
                        	else{
                        		lc_count_event = NULL;
                        	}

                        	// whether doesn't merge files containing result's parts
                        	merge = (char*) xmlGetProp(sub_node, (unsigned char*)"merge");
                        	bool isMerge = true;
                        	if ((merge != NULL) && (strcmp(merge, "false") == 0))
                        		isMerge = false;
                        	// whether parallelize event processing
                        	parallel_mode = (char*) xmlGetProp(sub_node, (unsigned char*)"parallel_mode");

                        	inDB = (char*) xmlGetProp(sub_node, (unsigned char*)"db_input");

                        	if (inFile != NULL){
                        		structFilePar* filePar = new structFilePar();

                        		// add input file
                        		filePar->strFileIn = inFile;
                        		// add output
                        		if (outFile != NULL) filePar->strFileOut = form_file_name(outFile, filePar->strFileIn, "1");
                        		else filePar->strFileOut = "";
                        		filePar->start_event = lc_start_event;
                        		filePar->count_event = lc_count_event;
                        		if (parallel_mode != NULL) filePar->strParallelMode = parallel_mode;
                        		else filePar->strParallelMode = "";
                        		filePar->isMerge = isMerge;

                        	    vecFiles.push_back(filePar);
                        	}
                        	else{
                        		if (inDB != NULL){
                        			// PARSE DATABASE PARAMETERS
                        			string input = inDB;
                        			istringstream ss(input);
                        			string token;

                        			bool isFirst = true;
                        			string server_name;
                        			while(getline(ss, token, ','))
                        			{
                        				if (isFirst){
                        					server_name = token;
                        					isFirst = false;

                        					continue;
                        				}

                        				transform(token.begin(), token.end(),token.begin(), ::tolower);
                        			    if ((token.length() > 4) && (token.substr(0,4) == "gen=")){
                        			    	isGen = true;
                        			        strGen = token.substr(4);
                        			    }
                        			    else{
                        			    	if ((token.length() > 7) && (token.substr(0,7) == "energy=")){
                        			    		token = token.substr(7);
                        			            int indDash = token.find_first_of('-');
                        			            if (indDash > -1){
                        			            	stringstream stream;
                        			                stream << token.substr(0,indDash);
                        			                double dVal;
                        			                if (stream >> dVal)
                        			                {
                        			                	isEnergy = true;
                        			                    fEnergy = dVal;
                        			                }
                        			                if ((int)token.length() > indDash){
                        			                	stringstream stream2;
                        			                    stream2 << token.substr(indDash+1);
                        			                    if (stream2 >> dVal)
                        			                    {
                        			                    	isMaxEnergy = true;
                        			                        fMaxEnergy = dVal;
                        			                    }
                        			                }
                        			             }//if (indDash > -1)
                        			             else{
                        			             stringstream stream;
                        			             	 stream << token;
                        			                 double dVal;
                        			                 if (stream >> dVal)
                        			                 {
                        			                	 isEnergy = true;
                        			                     fEnergy = dVal;
                        			                 }
                        			             }//else
                        			    	}//if ((token.length() > 7) && (token.substr(0,7) == "energy="))
                        			        else{
                        			        	if ((token.length() > 9) && (token.substr(0,9) == "particle=")){
                        			        		isParts = true;
                        			                strParts = token.substr(9);
                        			            }
                        			            else{
                        			            	if ((token.length() > 5) && (token.substr(0,5) == "desc=")){
                        			            		isDesc = true;
                        			            		strDesc = token.substr(5);
                        			            	}
                        			            	else{
                        			            		if ((token.length() > 5) && (token.substr(0,5) == "type=")){
                        			            			isType = true;
                        			            			strType = token.substr(5);
                        			            		}
                        			            	}//else DESC
                        			            }//else PARTICLE
                        			        }//else ENERGY
                        			    }//else GEN
                        			}//while(getline(ss, token, ','))

                        			//READ PATH FROM DATABASE
                        			TString strConnection = "mysql://" + server_name + "/data4mpd";
                        			TSQLServer* pSQLServer = TSQLServer::Connect(strConnection, "data4mpd", "data4mpd");
                        			if (pSQLServer == 0x00){
                        				cout<<"Error: connection to database wasn't established!"<<endl;
                        				isError = true;

                        				cur_node = cur_node->next;
                        				continue;
                        			}

                        			TString sql = "select data4mpd_path "
                        			              "from events";

                        			bool isWhere = false;
                        			if (isGen == true){
                        				if (isWhere){
                        					sql += TString::Format(" AND data4mpd_generator = '%s'", strGen.data());
                        			    }
                        			    else{
                        			    	isWhere = true;
                        			        sql += TString::Format(" "
                        			                               "where data4mpd_generator = '%s'", strGen.data());
                        			    }
                        			}
                        			if (isEnergy == true){
                        				if (isWhere){
                        					if (isMaxEnergy)
                        						sql += TString::Format(" AND data4mpd_energy >= %f AND data4mpd_energy <= %f", fEnergy, fMaxEnergy);
                        			        else
                        			        	sql += TString::Format(" AND data4mpd_energy = %f", fEnergy);
                        				}
                        			    else{
                        			    	isWhere = true;
                        			        if (isMaxEnergy)
                        			        	sql += TString::Format(" "
                        			                                   "where data4mpd_energy >= %f AND data4mpd_energy <= %f", fEnergy, fMaxEnergy);
                        			        else
                        			        	sql += TString::Format(" "
                        			                                   "where data4mpd_energy = %f", fEnergy);
                        			    }
                        			}
                        			    else{
                        			        if (isMaxEnergy){
                        			            if (isWhere){
                        			                sql += TString::Format(" AND data4mpd_energy <= %f", fMaxEnergy);
                        			            }
                        			            else{
                        			                isWhere = true;
                        			                sql += TString::Format(" "
                        			                                       "where data4mpd_energy <= %f", fMaxEnergy);
                        			            }
                        			        }
                        			    }
                        			    if (isParts == true){
                        			        if (isWhere){
                        			            sql += TString::Format(" AND data4mpd_collision = '%s'", strParts.data());
                        			        }
                        			        else{
                        			            isWhere = true;
                        			            sql += TString::Format(" "
                        			                                   "where data4mpd_collision = '%s'", strParts.data());
                        			        }
                        			    }
                        			    if (isDesc == true){
                        			        if (isWhere){
                        			            sql += TString::Format(" AND data4mpd_description like '%%%s%%'", strDesc.data());
                        			        }
                        			        else{
                        			            isWhere = true;
                        			            sql += TString::Format(" "
                        			                                   "where data4mpd_description like '%%%s%%'", strDesc.data());
                        			        }
                        			    }
                        			    if (isType == true){
                        			        if (isWhere){
                        			            sql += TString::Format(" AND data4mpd_datatype = '%s'", strType.data());
                        			        }
                        			        else{
                        			            isWhere = true;
                        			            sql += TString::Format(" "
                        			                                   "where data4mpd_datatype = '%s'", strType.data());
                        			        }
                        			    }

                        			TSQLResult* res = pSQLServer->Query(sql);

                        			int nrows = res->GetRowCount();
                        			if (nrows == 0){
                        				cout<<"There are no records for these parameters"<<endl;
                        			}
                        			else{
                        				TSQLRow* row;
                        				int counter = 1;
                        			    while ((row = res->Next()) != NULL){
                        			    	//TString path = row->GetField(0);
                        			    	structFilePar* filePar = new structFilePar();
                        			    	// add input file
                        			    	filePar->strFileIn = (char*)row->GetField(0);
                        			    	// add output
                        			    	if (outFile != NULL){
                        			    		char buf_int [9];
                        			    		sprintf (buf_int, "%d", counter);

                        			    		filePar->strFileOut = form_file_name(outFile, filePar->strFileIn, buf_int);
                        			    	}
                        			    	else
                        			    		filePar->strFileOut = "";

                        			    	filePar->start_event = lc_start_event;
                        			    	filePar->count_event = lc_count_event;
                        			    	if (parallel_mode != NULL) filePar->strParallelMode = parallel_mode;
                        			    	else filePar->strParallelMode = "";
                        			    	filePar->isMerge = isMerge;

                        			    	vecFiles.push_back(filePar);

                        			    	counter++;
                        			        delete row;
                        			    }//while (row = res->Next())
                        			}

                        			delete res;

                        			if (pSQLServer)
                        				delete pSQLServer;
                        		}
                        		else{// if (inDB != NULL)
                        			cout<<"Error: there are no files in <file> tag!"<<endl;
                        			isError = true;

                        			cur_node = cur_node->next;
                        			continue;
                        		}
                        	}

                            sub_node = sub_node->next;
                            continue;
                        }
                        // RUN TYPE TAG
                        if (strcmp((char*)sub_node->name, "run") == 0){
                        	mode = (char*) xmlGetProp(sub_node, (unsigned char*)"mode");
                            if ((mode != NULL) && (strcmp(mode, "global") == 0))
                            	isLocal = false;
                            else
                            	isLocal = true;

                            sproc_count = (char*) xmlGetProp(sub_node, (unsigned char*)"count");
                            pcConfig = (char*) xmlGetProp(sub_node, (unsigned char*)"config");
                            pcLogs = (char*) xmlGetProp(sub_node, (unsigned char*)"logs");
							
                            sub_node = sub_node->next;
                            continue;
                        }
                    }

                    sub_node = sub_node->next;
                }//while PARSING TAGS

                // START MAIN EXECUTION AFTER PARSING
                if ((!isCommand) && (macro_name == NULL)){
                	cout<<"Error: macro's name is required!"<<endl;
                	isError = true;
                }

                if (isError){
                	cout<<"This job is skipped!!!"<<endl;

                    clearVector(&vecFiles);
                    cur_node = cur_node->next;
                    continue;
                }

                // define total processor count and fill 'parallel_mode' parameter
                int iParallelCount = 0;
                for (unsigned int i = 0; i < vecFiles.size(); i++)
                {
                	structFilePar* filePar = vecFiles.at(i);
                	if (filePar->strParallelMode != ""){
                		if (filePar->strParallelMode == "*")
                	    {
                			filePar->iParallelMode = GetProcessorCount(!isLocal);
                			if (filePar->iParallelMode < 1)
                				filePar->iParallelMode = 1;
                	    }
                		else
                		{
                				filePar->iParallelMode = atoi(filePar->strParallelMode.c_str());
                				if (filePar->iParallelMode < 1)
                					filePar->iParallelMode = 1;
                		}
                	}
                	else
                		filePar->iParallelMode = 1;

                	iParallelCount += filePar->iParallelMode;
                }
				
                string strDiff;
                FILE *stream = NULL;
                // GLOBAL SCHEDULING
                if (isLocal == false)
                {
                	// define process count for job
                	int proc_count = 1;
                	if (sproc_count != NULL){
                		if ((sproc_count[0] == '*') && (strlen(sproc_count) == 1))
                		{
                			proc_count = GetProcessorCount(true);

                			if (proc_count == 0){
                				cout<<"Error: cluster's processors aren't set!"<<endl<<"This job is skipped!!!"<<endl;

                				clearVector(&vecFiles);
                			    cur_node = cur_node->next;
                			    continue;
                			}
                		}
                		else
                			proc_count = atoi(sproc_count);
                	}

                	if (proc_count == 0){
                		cout<<"Error: process count can't be equal 0!"<<endl<<"This job is skipped!!!"<<endl;

                		clearVector(&vecFiles);
                	    cur_node = cur_node->next;
                	    continue;
                	}

                	// load config file for enviroment
                    string sConfig = "";
                    if (pcConfig != NULL){
                    	sConfig = ". ";
                    	sConfig += pcConfig;
                    	/*if (pcLogs != NULL){
                    		sConfig += " > ";
                    		sConfig += pcLogs;
                    		sConfig += " 2>&1";
                    	}
                    	else*/
                    	sConfig += " > /dev/null";
                    }

                    // process count is equal count of input files if more
                    if ((iParallelCount > 0) && (proc_count > iParallelCount))
                    	proc_count = iParallelCount;

                    string fileName = "";
                    vector<string> vecParallelOutputs;
                    string sMacroName = "";
                    if (macro_name != NULL)
                    	sMacroName = macro_name;

                    // if there are some files to process then write info to to temporary file for SGE
                    if (iParallelCount > 0){
                    	//structFilePar* filePar = vecFiles.at(0);
                    	// generate output name for temporary file
                    	//fileName = filePar->strFileIn;
                    	long int t = time(NULL);
                    	sprintf(buffer,"file_list_%d",(int)t);
                    	fileName = buffer;
                    	// change tilda symbol on HOME
                    	//size_t np = fileName.find_first_of('~');
                    	//if (np != string::npos){
                    	//	char* pPath = getenv("HOME");
                    	//    if (pPath == NULL){
                    	//    	pPath = new char[2];
                    	//        pPath[0] = '/';
                    	//        pPath[1] = '\0';
                    	//    }

                    	//    do{
                    	//    	fileName.erase(np, 1);
                    	//        fileName.insert(np, pPath);
                    	//        np = fileName.find_first_of('~');
                    	//    } while (np != string::npos);
                    	//}

                    	// write parameters to temporary file
                    	FILE* pFile = fopen(fileName.c_str(), "wt");
                    	for (unsigned int i = 0; i < vecFiles.size(); i++){
                    		structFilePar* filePar = vecFiles.at(i);

                          	int* cur_start_event = start_event;
                           	int* cur_count_event = count_event;
                           	if (filePar->start_event != NULL){
                           		cur_start_event = filePar->start_event;
                           	}
                           	if (filePar->count_event != NULL){
                           		cur_count_event = filePar->count_event;
                           	}

                           	int parallel_mode = filePar->iParallelMode;
                    	    if ((parallel_mode > 1) && ((cur_start_event == NULL) || (cur_count_event == NULL))){
                    	    	parallel_mode = 1;
                    	    	cout<<"Warning: parallel_mode is used with 'count_event' parameter, paralell_mode is ignored"<<endl;
                    	    }

                    	    if (parallel_mode > 1){
                    	    	// generate output name for output parts
                    	        string par = filePar->strFileOut;

                    	        int start = *cur_start_event;
                    	        int counter = 1;
                    	        for (int i = 0; i < parallel_mode; i++){
                    	        	string sOutFile = filePar->strFileOut;
                    	            int event_per_proc = (*cur_count_event+i)/proc_count;

                    	            if (event_per_proc != 0){
                    	            	fwrite("\\\"", 2, sizeof(char), pFile);
                    	            	fwrite(filePar->strFileIn.c_str(), filePar->strFileIn.length(), sizeof(char), pFile);
                    	                fwrite("\\\"", 2, sizeof(char), pFile);

                    	                if (sOutFile != ""){
                    	                	sOutFile += "_";
                    	                    string strInt = IntToStr(counter);
                    	                    sOutFile += strInt;

                    	                    fwrite(" \\\"", 3, sizeof(char), pFile);
                    	                    fwrite(sOutFile.c_str(), sOutFile.length(), sizeof(char), pFile);
                    	                    fwrite("\\\"", 2, sizeof(char), pFile);
                    	                }

                    	                fwrite(" ", 1, sizeof(char), pFile);
                    	                string strInt = IntToStr(start);
                    	                fwrite(strInt.c_str(), strInt.length(), sizeof(char), pFile);
                    	                fwrite(" ", 1, sizeof(char), pFile);
                    	                strInt = IntToStr(event_per_proc);
                    	                fwrite(strInt.c_str(), strInt.length(), sizeof(char), pFile);

                    	                if (add_args != NULL){
                    	                	fwrite(" \\\"", 3, sizeof(char), pFile);
                    	                    fwrite(add_args, strlen(add_args), sizeof(char), pFile);
                    	                    fwrite("\\\"", 2, sizeof(char), pFile);
                    	                }

                    	                fwrite("\n", 1, sizeof(char), pFile);

                    	                counter++;

                    	                par += " " + sOutFile;
                    	            }//if (event_per_proc != 0)

                    	            start = start + event_per_proc;
                    	        }//for (int i = 0; i < parallel_mode; i++)

                    	        if ((counter > 1) && (filePar->isMerge))
                    	        	vecParallelOutputs.push_back(par);
                    	    }//if (parallel_mode > 1)
                    	    else{// parallel_mode <= 1
                    	    	fwrite("\\\"", 2, sizeof(char), pFile);
                    	        fwrite(filePar->strFileIn.c_str(), filePar->strFileIn.length(), sizeof(char), pFile);
                    	        fwrite("\\\"", 2, sizeof(char), pFile);

                    	        if (filePar->strFileOut != ""){
                    	        	fwrite(" \\\"", 3, sizeof(char), pFile);
                    	            fwrite(filePar->strFileOut.c_str(), filePar->strFileOut.length(), sizeof(char), pFile);
                    	            fwrite("\\\"", 2, sizeof(char), pFile);
                    	        }

                    	        if (cur_start_event != NULL){
                    	        	fwrite(" ", 1, sizeof(char), pFile);
                    	            string strInt = IntToStr(*cur_start_event);
                    	            fwrite(strInt.c_str(), strInt.length(), sizeof(char), pFile);
                    	            fwrite(" ", 1, sizeof(char), pFile);
                    	            strInt = IntToStr(*cur_count_event);
                    	            fwrite(strInt.c_str(), strInt.length(), sizeof(char), pFile);
                    	        }

                    	        if (add_args != NULL){
                    	        	fwrite(" \\\"", 3, sizeof(char), pFile);
                    	            fwrite(add_args, strlen(add_args), sizeof(char), pFile);
                    	            fwrite("\\\"", 2, sizeof(char), pFile);
                    	        }

                    	        fwrite("\n", 1, sizeof(char), pFile);
                    	    }//else
                    	}//for

                    	fclose (pFile);
                    }

                    // RUN JOB in SGE
                    pcSUB = new char[100+sMacroName.length()+fileName.length()+sConfig.length()];
                    if (isCommand){
                    	sprintf (pcSUB, "qsub -t 1-1 -tc %d -v config=\"%s\",sched_command_line=\"%s\" mpd.qsub",
                    			proc_count, sConfig.c_str(), command_line);
                    }
                    else{
                    	sprintf (pcSUB, "qsub -t 1-%d -tc %d -v macro=\"%s\",files=\"%s\",config=\"%s\" mpd.qsub",
                    			iParallelCount, proc_count, macro_name, fileName.c_str(), sConfig.c_str());
                    }

                    //cout<<"COMMAND: "<<pcSUB<<endl;

                    stream = popen(pcSUB, "r");
                    while (fgets(buffer, MAX_BUFFER, stream) != NULL)
                    	data.append(buffer);
                    pclose(stream);

                    //cout<<"Task string: "<<pcSUB<<endl;

                    delete [] pcSUB;

                    string ID = data;
                    data.clear();

                    int indSymbol = ID.find('.', 15);
                    ID = ID.substr(15,indSymbol-15);

                    cout<<"Job was run with ID: "<<ID<<". Enter 'qstat' command to check status"<<endl;

                    // merge files if required
                    if (!vecParallelOutputs.empty()){
                    	for (int i = 0; i < (int)vecParallelOutputs.size(); i++){
                    		string par = vecParallelOutputs.at(i);
                    		pcSUB = new char[100+ID.length()+par.length()+sConfig.length()];
                    	    sprintf (pcSUB, "qsub -hold_jid %s -v \"sParameters=%s\",config=\"%s\" union.qsub", ID.c_str(), par.c_str(), sConfig.c_str());

                    	    stream = popen(pcSUB, "r");
                    	    while (fgets(buffer, MAX_BUFFER, stream) != NULL)
                    	    	data.append(buffer);
                    	    pclose(stream);

                    	    string localID = data;
                    	    int indSymbol = localID.find(' ', 9);
                    	    localID = localID.substr(9,indSymbol-9);
                    	    cout<<"Task for merging parallel files has ID: "<<localID<<endl;

                    	    data.clear();
                    	    delete [] pcSUB;
                    	}
                    }// if (!vecParallelOutputs.empty())
                }
                // LOCAL SCHEDULING
                else{
                	// define process count for job
                	int proc_count = 1;
                	if (sproc_count != NULL)
                	{
                		if ((sproc_count[0] == '*') && (strlen(sproc_count) == 1)){
                			proc_count = GetProcessorCount(false);

                			if (proc_count == 0){
                				cout<<"Error: can't define core number!"<<endl<<"This job is skipped!!!"<<endl;

                				cur_node = cur_node->next;
                	            continue;
                			}
                		}
                	    else
                	    	proc_count = atoi(sproc_count);
                	}

                	if (proc_count == 0){
                		cout<<"Error: process count can't be equal 0!"<<endl<<"This job is skipped!!!"<<endl;

                		cur_node = cur_node->next;
                		continue;
                	}

                	// generate string for config (enviroment) file
                	string sConfig = ". ";
                	sConfig += pcConfig;
                	if (pcLogs != NULL){
                		sConfig += " > ";
                		sConfig += pcLogs;
                		sConfig += " 2>&1 ; ";
                	}
                	else
                		sConfig += " >/dev/null ; ";

                	// process count is equal count of input files if more
                	if (proc_count > iParallelCount)
                		proc_count = iParallelCount;

                	int rc;
                	pthread_t* threads = new pthread_t[vecFiles.size()];

                	sem_t sem;
                	sem_init(&sem, 0, proc_count);

                	pthread_mutex_t mut;
                	//create mutex attribute variable
                	pthread_mutexattr_t mAttr;
                	// setup recursive mutex for mutex attribute
                	pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_RECURSIVE_NP);
                	// Use the mutex attribute to create the mutex
                	pthread_mutex_init(&mut, &mAttr);
                	// Mutex attribute can be destroy after initializing the mutex variable
                	pthread_mutexattr_destroy(&mAttr);

                	// console command - non-macro ROOT
                	if (isCommand){
                		// generate task string
                		int lenSUB = 100+strlen(command_line)+sConfig.length();
                		char* pcSUB = new char[lenSUB];

                		sprintf (pcSUB, "%s%s", sConfig.c_str(), command_line);
                		if (pcLogs != NULL)
                			sprintf (pcSUB, "%s > %s 2>&1", pcSUB, pcLogs);
                		else
                			sprintf (pcSUB, "%s", pcSUB);

                		// write and execute temporary bash file
                		ofstream myfile;
                		char temp_file[255], run_temp[255];
                		long int t = time(NULL);
                		sprintf(temp_file, "temp_%d.sh", (int)t); // generate output name for temporary bash file
                		myfile.open (temp_file);
                		myfile<<pcSUB;
                		myfile.close();
                		sprintf(run_temp, "bash %s", temp_file);

                		cout<<"Task is running:"<<endl<<"command line - "<<command_line<<endl;

                		// run task
                		system(run_temp);

                		delete [] pcSUB;

                		//delete temporary bash file
                		sprintf(run_temp, "rm %s", temp_file);
                		system(run_temp);
                	}
                	// parallelizing macro ROOT on multicore machine
                	else
                	{
                		for (unsigned int i=0; i < vecFiles.size(); i++)
                		{
                			structThreadPar* par = new structThreadPar();
                			par->sConfig = sConfig;
                			par->macro_name = macro_name;

                			structFilePar* filePar = vecFiles.at(i);
                			par->inFile = filePar->strFileIn;
                			par->outFile = filePar->strFileOut;
                			par->start_event = start_event;
                			par->count_event = count_event;
                			if (filePar->start_event != NULL){
                				par->start_event = filePar->start_event;
                			}
                			if (filePar->count_event != NULL){
                				par->count_event = filePar->count_event;
                			}
                			if ((filePar->iParallelMode > 1) && ((par->start_event == NULL) || (par->count_event == NULL))){
                				par->parallel_mode = 1;
                				cout<<"Warning: parallel_mode is used without 'count_event' parameter, paralell_mode is ignored!"<<endl;
                			}
                			else
                				par->parallel_mode = filePar->iParallelMode;
                			par->isMerge = filePar->isMerge;

                			if (add_args != NULL) par->add_args = add_args;
                			else par->add_args = "";
                			if (pcLogs != NULL) par->logs = pcLogs;
                			else par->logs = "";

                			par->sem = sem;
                			par->mut = mut;
                			par->counter = i+1;

                			if (par->parallel_mode < 2)
                				sem_wait(&sem);

                			// сreate and run threads to parallel processing - function ThreadProcessFile
                			rc = pthread_create(&threads[i], NULL, ThreadProcessFile, (void*)par);
                			if (rc){
                				printf("ERROR; return code from pthread_create() is %d\n", rc);
                				exit(-1);
                			}
                		}

                		// waiting for finishing threads
                		void **thread_return = NULL;
                		for (unsigned int i=0; i < vecFiles.size(); i++)
                		{
                			pthread_join(threads[i], thread_return);

                		    if (thread_return != NULL)
                		    	cout<<"Thread " << i << " failed" << endl;
                		}
                	}//else isCommand

                	sem_destroy(&sem);
                	pthread_mutex_destroy(&mut);

                	cout<<endl<<"Local job was finished"<<endl;
                }//else local scheduling
        	}// JOB tag was processed
        }// if tag means JOB

        cur_node = cur_node->next;
	}// while - cycle for all jobs

    xmlFreeDoc(doc);
}
