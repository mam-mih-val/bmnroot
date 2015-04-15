//============================================================================
// Name        : mpd-scheduler.cpp
// Author      : Konstantin Gertsenberger
// Copyright   : JINR
// Description : Scheduler for MPDRoot
//============================================================================

// own headers
#include "function_set.h"

// ROOT includes for DB work
#include "TSQLServer.h"
#include "TSQLResult.h"
#include "TSQLRow.h"

// C++ includes
#include <pthread.h>
#include <semaphore.h>
#include "libxml/tree.h"
#include <fstream>
#include <algorithm>

enum enumSchedulerName {Torque};
enumSchedulerName scheduler_name;

const int MAX_BUFFER = 255;
char buffer[MAX_BUFFER];

// files to process with 'start event', 'event count' and 'parallel_mode' parameters
struct structFilePar
{
	string strFileIn;
	string strFileOut;
	int* start_event;
	int* count_event;
	string strParallelMode;
	int iParallelMode;
	int iMerge;
};

// structure for parameters to transfer for the threads
struct structThreadPar
{
	string sConfig;
	string macro_name;

	string inFile;
	string outFile;
	int* start_event;
	int* count_event;
	int parallel_mode;
	int iMerge;

	string add_args;
	string logs;

	sem_t* sem;
	pthread_mutex_t* mut;
	int counter;
};

// structure for parameters to transfer for the subthreads
struct structSubThreadPar
{
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
	        if (mode == 1) cout<<"mpd-scheduler$ The Chain witn "<<events<<" event(s) was merged to file \""<<sResultFile<<"\" from following files:"<<endl;
	        else cout<<"mpd-scheduler$ The Chain witn "<<events<<" event(s) was written to file \""<<sResultFile<<"\" to point following files:"<<endl;

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
	    cout<<"mpd-scheduler$ The count of events in test reading is equal "<<events<<endl;
	}
}*/

// clear vector parameters
void clearVector(vector<structFilePar*>* vecFiles){
	for (unsigned int i = 0; i < vecFiles->size(); i++){
		structFilePar* filePar = vecFiles->at(i);
		delete filePar;
	}

	vecFiles->clear();
}

// form output file's name from $ variable
string form_file_name(string outName, string inName, string strCounter){
	replace_string_in_text(outName, "${input}", inName);
	replace_string_in_text(outName, "${file_name}", get_file_name(inName));
	replace_string_in_text(outName, "${file_name_with_ext}", get_file_name_with_ext(inName));
	replace_string_in_text(outName, "${counter}", strCounter);

	return outName;
}

// generate output file name with counter (for partial result)
string GenerateOutputFilePath(string path, int counter)
{
	size_t last_point_idx = path.find_last_of(".");

	string add_string = "__";
	add_string += convert_integer_to_string(counter);

	if (string::npos != last_point_idx)
		return path.insert(last_point_idx, add_string);
	else
		return path.append(add_string);
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

	pthread_mutex_lock(thread_par->mut);
	cout<<"mpd-scheduler$ Subtask is running:"<<endl<<"input - "<<inFile<<endl<<"output - "<<outFile<<endl<<"start event - "<<start_event<<endl<<"event count - "<<count_event<<endl<<endl;
	pthread_mutex_unlock(thread_par->mut);

	// run task in thread
	system(run_temp);

	delete [] pcSUB;

	//delete temporary bash file
	sprintf(run_temp, "rm %s", temp_file);
	system(run_temp);

	sem_post(thread_par->sem);

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
		    	sOutFile = GenerateOutputFilePath(sOutFile, real_thread_count);

		        structSubThreadPar* par = new structSubThreadPar();
		        par->thread_parameter = thread_par;
		        par->start_event = start;
		        par->count_event = event_per_thread;
		        par->outFile = sOutFile;
		        par->counter2 = i+1;

		        sem_wait(thread_par->sem);

		        rc = pthread_create(&threads[real_thread_count], NULL, SubThreadProcessFile, (void*)par);
		        if (rc){
		        	printf("mpd-scheduler$ ERROR: return code from sub pthread_create() is %d\n", rc);
		            exit(-1);
		        }

		        sUnion += " " + sOutFile;

		        real_thread_count++;
		    }//if (event_per_thread != 0){

		    start = start + event_per_thread;
		}//for

		// waiting for finishing of the child threads
		//cout<<"mpd-scheduler$ Waiting for "<<real_thread_count<<" subtask(s) to finish..."<<endl;
		void **thread_return = NULL;
		for (int i=0; i < real_thread_count; i++)
		{
			pthread_join(threads[i], thread_return);

		    if (thread_return != NULL)
		    	cout<<"mpd-scheduler$ ERROR: thread " << i << " failed" << endl;
		}

		// merge result files or write to single TChain object
		if (thread_par->iMerge >= 0)
		{
			string exe_dir = get_app_dir_linux();
			string UNIONc_path = exe_dir + "union.C";
			char* pcSUB = new char[44+sUnion.length()+sConfig.length()+UNIONc_path.length()+logs.length()];
		    sprintf (pcSUB, "%sroot -b -q \"%s(\\\"%s\\\", %d)\"", sConfig.c_str(), UNIONc_path.c_str(),
		    		sUnion.c_str(), thread_par->iMerge);

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

		    cout<<endl<<"mpd-scheduler$ Merging the result of subtasks..."<<endl<<endl;
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

		pthread_mutex_lock(thread_par->mut);
		cout<<"mpd-scheduler$ Task is running:"<<endl<<"input - "<<inFile<<endl<<"output - "<<outFile<<endl<<endl;
		pthread_mutex_unlock(thread_par->mut);

		// run task in thread
		system(run_temp);

		delete [] pcSUB;

		//delete temporary bash file
		sprintf(run_temp, "rm %s", temp_file);
		system(run_temp);

		sem_post(thread_par->sem);
	}//else

	pthread_exit(NULL);

	return NULL;
}

int main(int argc, char** argv){
	if (argc < 2){
		cout << "mpd-scheduler$ ERROR: first parameter (XML file name) wasn't set" << endl;
        return 1;
	}

    // pointer to XML document
	xmlDocPtr doc = xmlReadFile(argv[1], NULL, 0);
	if(doc == NULL){
		cout<<"mpd-scheduler$ ERROR: open XML file: "<<argv[1]<<endl;
		return 2;
	}

    // get root element
	xmlNodePtr root = xmlDocGetRootElement(doc);

	// temporary variables
	char* pcSUB;
	string data;

	// path to executable's directory ('/' - last char)
	string exe_dir = get_app_dir_linux();

    // cycle for all jobs
	xmlNodePtr cur_node = root;
	while (cur_node)
	{
        // if tag means JOB
        if (cur_node->type == XML_ELEMENT_NODE)
        {
        	if (strcmp((char*)cur_node->name, "job") == 0)
        	{
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
                				cout<<"mpd-scheduler$ WARNING: command line is reassigned"<<endl;

                		    command_line = (char*) xmlGetProp(sub_node, (unsigned char*)"line");
                		    isCommand = true;

                		    sub_node = sub_node->next;
                		    continue;
                		}
                		// MACRO NAME TAG
                        if (strcmp((char*)sub_node->name, "macro") == 0){
                        	if (macro_name != NULL)
                        		cout<<"mpd-scheduler$ WARNING: macro's name is reassigned"<<endl;

                        	macro_name = (char*) xmlGetProp(sub_node, (unsigned char*)"name");

                        	pcStart_event = (char*) xmlGetProp(sub_node, (unsigned char*)"start_event");
                        	pcCount_event = (char*) xmlGetProp(sub_node, (unsigned char*)"count_event");

                        	// check global start parameter
                        	if (pcStart_event != NULL){
                        		start_event = new int;
                        	    *start_event = atoi(pcStart_event);
                        	}
                        	else
                        	{
                        		start_event = NULL;
                        	}
                        	// check global count parameter
                        	if (pcCount_event != NULL)
                        	{
                        		if (!is_string_number(pcCount_event))
                        		{
                        			isError = true;
                        			cout<<"mpd-scheduler$ ERROR: event count must be a number!"<<endl;
                        			break;
                        		}
                        		count_event = new int;
                        	     *count_event = atoi(pcCount_event);
                        	     if (*count_event < 0)
                        	     {
                        	    	 isError = true;
                        	         cout<<"mpd-scheduler$ ERROR: event count must be a positive number or 0 (for all events)!"<<endl;

                        	         //cur_node = cur_node->next;
                        	         //continue;
                        	         break;
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
                        	if (lcStart_event != NULL)
                        	{
                        		lc_start_event = new int;
                        	    *lc_start_event = atoi(lcStart_event);
                        	}
                        	else
                        	{
                        		lc_start_event = NULL;
                        	}
                        	// check count parameter
                        	if (lcCount_event != NULL)
                        	{
                        		if (!is_string_number(lcCount_event))
                        		{
                        			isError = true;
                        		    cout<<"mpd-scheduler$ ERROR: event count must be a number!"<<endl;
                        		    break;
                        		}
                        		lc_count_event = new int;
                        	    *lc_count_event = atoi(lcCount_event);
                        	    if (*lc_count_event < 0)
                        	    {
                        	    	isError = true;
                        	        cout<<"mpd-scheduler$ ERROR: event count must be a positive number or 0 (for all events)!"<<endl;

                        	        //cur_node = cur_node->next;
                        	        //continue;
                        	        break;
                        	    }
                        	}
                        	else{
                        		lc_count_event = NULL;
                        	}

                        	// whether doesn't merge files containing result's parts
                        	merge = (char*) xmlGetProp(sub_node, (unsigned char*)"merge");
                        	int iMerge = 1;	//"true"
                        	if (merge != NULL)
                        	{
                        		char* lower_merge = convert_pchar_to_lowercase_new(merge);
                        		if (strcmp(lower_merge, "false") == 0)
                        			iMerge = -1;
                        		if (strcmp(lower_merge, "chain") == 0)
                        		    iMerge = 0;
                        		if (strcmp(lower_merge, "nodel") == 0)
                        			iMerge = 2;

                        		delete lower_merge;
                        	}
                        	// whether parallelize event processing
                        	parallel_mode = (char*) xmlGetProp(sub_node, (unsigned char*)"parallel_mode");

                        	inDB = (char*) xmlGetProp(sub_node, (unsigned char*)"db_input");

                        	if (inFile != NULL)
                        	{
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
                        		filePar->iMerge = iMerge;

                        	    vecFiles.push_back(filePar);
                        	}
                        	else
                        	{
                        		if (inDB != NULL)
                        		{
                        			// PARSE DATABASE PARAMETERS
                        			string input = inDB;
                        			istringstream ss(input);
                        			string token;

                        			// variables for DB
                        			bool isFirst = true, isGen = false, isEnergy = false, isMinEnergy = false, isMaxEnergy = false, isParts = false, isDesc = false, isType = false;
                        			string server_name, strGen, strParts, strDesc, strType;
                        			double fEnergy, fMaxEnergy;
                        			// parse tokens by comma separated
                        			while(getline(ss, token, ','))
                        			{
                        				if (isFirst)
                        				{
                        					server_name = token;
                        					isFirst = false;

                        					continue;
                        				}

                        				// to lowercase
                        				transform(token.begin(), token.end(),token.begin(), ::tolower);

                        				// generator name parsing
                        				if ((token.length() > 4) && (token.substr(0,4) == "gen="))
                        				{
                        			    	isGen = true;
                        			        strGen = token.substr(4);
                        			    }
                        			    else
                        			    {
                        			    	// energy parsing
                        			    	if ((token.length() > 7) && (token.substr(0,7) == "energy="))
                        			    	{
                        			    		token = token.substr(7);

                        			    		size_t indDash = token.find_first_of('-');
                        			    		if (indDash != string::npos)
                        			    		{
                        			    			stringstream stream;
                        			    		    stream << token.substr(0, indDash);
                        			    		    double dVal;
                        			    		    if (stream >> dVal)
                        			    		    {
                        			    		    	isEnergy = true;
                        			    		        isMinEnergy = true;
                        			    		        fEnergy = dVal;
                        			    		    }
                        			    		    if (token.length() > indDash)
                        			    		    {
                        			    		    	stringstream stream2;
                        			    		        stream2 << token.substr(indDash+1);
                        			    		        if (stream2 >> dVal)
                        			    		        {
                        			    		        	isEnergy = true;
                        			    		            isMaxEnergy = true;
                        			    		            fMaxEnergy = dVal;
                        			    		        }
                        			    		    }
                        			    		}//if (indDash > -1)
                        			    		// if exact energy value
                        			    		else
                        			    		{
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
                        			    	// particles' names in collision parsing
                        			    	else{
                        			        	if ((token.length() > 9) && (token.substr(0,9) == "particle=")){
                        			        		isParts = true;
                        			                strParts = token.substr(9);
                        			            }
                        			            else
                        			            {
                        			            	// search text in description string
                        			            	if ((token.length() > 5) && (token.substr(0,5) == "desc=")){
                        			            		isDesc = true;
                        			            		strDesc = token.substr(5);
                        			            	}
                        			            	else
                        			            	{
                        			            		// type of data parsing
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
                        			TSQLServer* pSQLServer = TSQLServer::Connect(strConnection, "data4mpd", "MixedPhase");
                        			if (pSQLServer == 0x00)
                        			{
                        				cout<<"mpd-scheduler$ ERROR: connection to database wasn't established!"<<endl;
                        				isError = true;

                        				//cur_node = cur_node->next;
                        				//continue;
                        				break;
                        			}

                        			TString sql = "select data4mpd_path "
                        			              "from events";

                        			bool isWhere = false;
                        			// if event generator selection
                        			if (isGen == true)
                        			{
                        				if (isWhere){
                        					sql += TString::Format(" AND data4mpd_generator = '%s'", strGen.data());
                        			    }
                        			    else{
                        			    	isWhere = true;
                        			        sql += TString::Format(" "
                        			                               "where data4mpd_generator = '%s'", strGen.data());
                        			    }
                        			}
                        			// if energy selection
                        			if (isEnergy == true)
                        			{
                        				if (isWhere)
                        					sql += " AND ";
                        				else
                        				{
                        					isWhere = true;
                        				    sql += " "
                        				           "where ";
                        				}

                        				if (isMinEnergy)
                        				{
                        					sql += TString::Format("data4mpd_energy >= %f", fEnergy);
                        				    if (isMaxEnergy)
                        				    	sql += TString::Format(" AND data4mpd_energy <= %f", fMaxEnergy);
                        				}
                        				else
                        				{
                        					if (isMaxEnergy)
                        						sql += TString::Format("data4mpd_energy <= %f", fMaxEnergy);
                        				    else
                        				        sql += TString::Format("data4mpd_energy = %f", fEnergy);
                        				}
                        			}
                        			// if 'particles in collision' selection
                        			if (isParts == true)
                        			{
                        				if (isWhere)
                        			            sql += TString::Format(" AND data4mpd_collision = '%s'", strParts.data());
                        			    else
                        			    {
                        			    	isWhere = true;
                        			        sql += TString::Format(" "
                        			                               "where data4mpd_collision = '%s'", strParts.data());
                        			    }
                        			}
                        			if (isDesc == true)
                        			{
                        				if (isWhere)
                        					sql += TString::Format(" AND data4mpd_description like '%%%s%%'", strDesc.data());
                        			    else
                        			    {
                        			    	isWhere = true;
                        			        sql += TString::Format(" "
                        			                               "where data4mpd_description like '%%%s%%'", strDesc.data());
                        			    }
                        			}
                        			if (isType == true)
                        			{
                        				if (isWhere)
                        					sql += TString::Format(" AND data4mpd_datatype = '%s'", strType.data());
                        			    else
                        			    {
                        			    	isWhere = true;
                        			        sql += TString::Format(" "
                        			                               "where data4mpd_datatype = '%s'", strType.data());
                        			    }
                        			}

                        			TSQLResult* res = pSQLServer->Query(sql);

                        			int nrows = res->GetRowCount();
                        			if (nrows == 0)
                        				cout<<"mpd-scheduler$ WARNING: there are no records for these parameters"<<endl;
                        			else
                        			{
                        				TSQLRow* row;
                        				int counter = 1;
                        			    while ((row = res->Next()) != NULL)
                        			    {
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
                        			    	filePar->iMerge = iMerge;

                        			    	vecFiles.push_back(filePar);

                        			    	counter++;
                        			        delete row;
                        			    }//while (row = res->Next())
                        			}//else nrows > 0

                        			delete res;

                        			if (pSQLServer)
                        				delete pSQLServer;
                        		}
                        		else // if (inDB != NULL)
                        		{
                        			cout<<"mpd-scheduler$ ERROR: there are no files in <file> tag!"<<endl;
                        			isError = true;

                        			//cur_node = cur_node->next;
                        			//continue;
                        			break;
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
                	cout<<"mpd-scheduler$ ERROR: macro's name is required!"<<endl;
                	isError = true;
                }

                if (isError){
                	cout<<"mpd-scheduler$ ERROR: This job will be skipped because of errors above!"<<endl;

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
                		if (filePar->strParallelMode == "0")
                	    {
                			if (isLocal)
                				filePar->iParallelMode = get_linux_processor_count();
                			else
                				filePar->iParallelMode = get_sge_processor_count();

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
                	if (sproc_count != NULL)
                	{
                		if ((sproc_count[0] == '0') && (strlen(sproc_count) == 1))
                		{
                			proc_count = get_sge_processor_count();

                			if (proc_count == 0)
                			{
                				cout<<"mpd-scheduler$ ERROR: cluster's processors aren't set!"<<endl<<"This job will be skipped!!!"<<endl;

                				clearVector(&vecFiles);
                			    cur_node = cur_node->next;
                			    continue;
                			}
                		}
                		else
                			proc_count = atoi(sproc_count);
                	}
                	else
                		proc_count = iParallelCount;

                	if (proc_count == 0)
                	{
                		cout<<"mpd-scheduler$ ERROR: processor count can't be equal 0!"<<endl<<"This job will be skipped!!!"<<endl;

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
                    	//sConfig += " > /dev/null";
                    }

                    // process count is equal count of input files if more
                    if ((iParallelCount > 0) && (proc_count > iParallelCount))
                    	proc_count = iParallelCount;

                    string fileName = "";
                    vector<string> vecParallelOutputs;
                    vector<int> vecUnionMode;
                    string sMacroName = "";
                    if (macro_name != NULL)
                    	sMacroName = macro_name;

                    // if there are some files to process then write info to temporary file for SGE
                    if (iParallelCount > 0)
                    {
                    	//structFilePar* filePar = vecFiles.at(0);
                    	// generate output name for temporary file
                    	//fileName = filePar->strFileIn;
                    	long int t = time(NULL);
						sprintf(buffer, "file_list_%d",(int)t);
                    	fileName = buffer;

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
                    	    	cout<<"mpd-scheduler$ WARNING: parallel_mode is used with 'count_event' parameter, paralell_mode is ignored"<<endl;
                    	    }

                    	    if (parallel_mode > 1)
                    	    {
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
                    	                	sOutFile = GenerateOutputFilePath(sOutFile, counter);

                    	                    fwrite(" \\\"", 3, sizeof(char), pFile);
                    	                    fwrite(sOutFile.c_str(), sOutFile.length(), sizeof(char), pFile);
                    	                    fwrite("\\\"", 2, sizeof(char), pFile);
                    	                }

                    	                fwrite(" ", 1, sizeof(char), pFile);
                    	                string strInt = convert_integer_to_string(start);
                    	                fwrite(strInt.c_str(), strInt.length(), sizeof(char), pFile);
                    	                fwrite(" ", 1, sizeof(char), pFile);
                    	                strInt = convert_integer_to_string(event_per_proc);
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

                    	        if ((counter > 1) && (filePar->iMerge >= 0))
                    	        {
                    	        	vecParallelOutputs.push_back(par);
                    	        	vecUnionMode.push_back(filePar->iMerge);
                    	        }
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
                    	            string strInt = convert_integer_to_string(*cur_start_event);
                    	            fwrite(strInt.c_str(), strInt.length(), sizeof(char), pFile);
                    	            fwrite(" ", 1, sizeof(char), pFile);
                    	            strInt = convert_integer_to_string(*cur_count_event);
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
                    string MPDqsub_path = exe_dir + "mpd.qsub";
                    pcSUB = new char[100+sMacroName.length()+fileName.length()+sConfig.length()+MPDqsub_path.length()];
                    if (isCommand){
                    	sprintf (pcSUB, "qsub -t 1-1 -tc %d -v config=\"%s\",sched_command_line=\"%s\" %s",
                    			proc_count, sConfig.c_str(), command_line, MPDqsub_path.c_str());
                    }
                    else{
                    	sprintf (pcSUB, "qsub -t 1-%d -tc %d -v macro=\"%s\",files=\"%s\",config=\"%s\" %s",
                    			iParallelCount, proc_count, macro_name, fileName.c_str(), sConfig.c_str(), MPDqsub_path.c_str());
                    }

                    // main command
                    //cout<<"COMMAND: "<<pcSUB<<endl;

                    stream = popen(pcSUB, "r");
                    while (fgets(buffer, MAX_BUFFER, stream) != NULL)
                    	data.append(buffer);
                    pclose(stream);

                    delete [] pcSUB;

                    //cout<<"Content of 'data' variable: "<<data<<endl;

                    string ID = data;
                    data.clear();

                    size_t indSymbol = ID.find('.', 15);
                    if (indSymbol == string::npos)
                    {
                    	cout<<"mpd-scheduler$ ERROR: 'qsub' command was encountered with some problems, this job will be skipped"<<endl;

                    	clearVector(&vecFiles);
                    	cur_node = cur_node->next;
                    	continue;
                    }

                    ID = ID.substr(15,indSymbol-15);

                    cout<<"mpd-scheduler$ Job has been started with ID: "<<ID<<". Enter 'qstat' command to check status"<<endl;

                    // merge files if required
                    if (!vecParallelOutputs.empty())
                    {
                    	for (int i = 0; i < (int)vecParallelOutputs.size(); i++)
                    	{
                    		string par = vecParallelOutputs.at(i);
                    		string UNIONqsub_path = exe_dir + "union.qsub";
                    		pcSUB = new char[100+ID.length()+par.length()+sConfig.length()+UNIONqsub_path.length()];
                    	    sprintf (pcSUB, "qsub -hold_jid %s -v \"sParameters=%s\",UnionMode=\"%d\",config=\"%s\" %s",
                    	    		ID.c_str(), par.c_str(), vecUnionMode.at(i), sConfig.c_str(), UNIONqsub_path.c_str());

                    	    stream = popen(pcSUB, "r");
                    	    while (fgets(buffer, MAX_BUFFER, stream) != NULL)
                    	    	data.append(buffer);
                    	    pclose(stream);

                    	    string localID = data;
                    	    int indSymbol = localID.find(' ', 9);
                    	    localID = localID.substr(9,indSymbol-9);
                    	    cout<<"mpd-scheduler$ Task for merging parallel files has ID: "<<localID<<endl;

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
                		if ((sproc_count[0] == '0') && (strlen(sproc_count) == 1)){
                			proc_count = get_linux_processor_count();

                			if (proc_count == 0){
                				cout<<"mpd-scheduler$ ERROR: can't define core number!"<<endl<<"This job will be skipped!!!"<<endl;

                				cur_node = cur_node->next;
                	            continue;
                			}
                		}
                	    else
                	    	proc_count = atoi(sproc_count);
                	}
                	else
                		proc_count = iParallelCount;

                	if (proc_count == 0){
                		cout<<"mpd-scheduler$ ERROR: process count can't be equal 0!"<<endl<<"This job will be skipped!!!"<<endl;

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

                		cout<<"mpd-scheduler$ Task is running:"<<endl<<"command line - "<<command_line<<endl;

                		// run task
                		system(run_temp);

                		delete [] pcSUB;

                		//delete temporary bash file
                		sprintf(run_temp, "rm %s", temp_file);
                		system(run_temp);
                	}
                	// parallelizing macro ROOT on one multicore machine
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
                				cout<<"mpd-scheduler$ WARNING: parallel_mode is used without 'count_event' parameter, paralell_mode is ignored!"<<endl;
                			}
                			else
                				par->parallel_mode = filePar->iParallelMode;
                			par->iMerge = filePar->iMerge;

                			if (add_args != NULL) par->add_args = add_args;
                			else par->add_args = "";
                			if (pcLogs != NULL) par->logs = pcLogs;
                			else par->logs = "";

                			par->sem = &sem;
                			par->mut = &mut;
                			par->counter = i+1;

                			if (par->parallel_mode < 2)
                			{
                				//int value;
                				//sem_getvalue(par->sem, &value);
                				//cout<<"Thread is waiting: "<<i<<" semaphore value: "<<value<<endl;
                				sem_wait(&sem);
                			}

                			// сreate and run threads to parallel processing - function ThreadProcessFile
                			rc = pthread_create(&threads[i], NULL, ThreadProcessFile, (void*)par);
                			if (rc)
                			{
                				printf("mpd-scheduler$ ERROR: return code from pthread_create() is %d\n", rc);
                				exit(-1);
                			}
                		}

                		// waiting for finishing threads
                		void **thread_return = NULL;
                		//cout<<"Waiting for finishing threads "<<vecFiles.size()<<endl;
                		for (unsigned int i=0; i < vecFiles.size(); i++)
                		{
                			pthread_join(threads[i], thread_return);

                		    if (thread_return != NULL)
                		    	cout<<"mpd-scheduler$ ERROR: thread " << i << " failed" << endl;
                		}
                	}//else isCommand

                	sem_destroy(&sem);
                	pthread_mutex_destroy(&mut);

                	cout<<endl<<"mpd-scheduler$ Local job was finished"<<endl;
                }//else local scheduling

                clearVector(&vecFiles);
        	}// JOB tag was processed
        }// if tag means JOB

        cur_node = cur_node->next;
	}// while - cycle for all jobs

    xmlFreeDoc(doc);
}
