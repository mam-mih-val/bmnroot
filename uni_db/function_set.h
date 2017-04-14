//============================================================================
// Name        : function_set.h
// Author      : Konstantin Gertsenberger (gertsen@jinr.ru)
// Description : set of common C++ functions
// Version     : 1.01
//============================================================================

#ifndef FUNCTION_SET_H
#define FUNCTION_SET_H

// C++ includes
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <ctime>
//#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

/* declarations */

/* OS FUNCTIONS */
// execute system command in shell (bash)
int system_command_linux(string aCommand, string& result);
// change tilda symbol on HOME in linux path
string replace_home_symbol_linux(string path);
// change $VMCWORKDIR symbol in linux path
string replace_vmc_path_linux(string path);
// get processor core count on this machine
int get_linux_processor_count();
enum BATCH_SYSTEM_NAME {SGE_BATCH_SYSTEM, TORQUE_BATCH_SYSTEM, SLURM_BATCH_SYSTEM};
// get maximum available processor count on Sun Grid Engine system
int get_batch_processor_count(BATCH_SYSTEM_NAME batch_system, string queue_name = "");

/* GLOBAL APPLICATION FUNCTIONS */
// get application name in linux
string get_app_name_linux();
// get aplication directory (path without file name) in linux
string get_app_dir_linux();

/* NUMBER FUNCTIONS */
// check bit in 'variable' at 'position'
#define CHECK_BIT(variable,position) ((variable) & (1ULL<<(position)))

/* STRING FUNCTIONS */
// convert integer number to string
string convert_int_to_string(int number);
// convert integer (hexadecimal value) to string with hexadecimal presentation without "0x"
string int_to_hex_string(int int_number);
// convert string with hexadecimal presentation without "0x" to integer
int hex_string_to_int(string hex_string);
// is string an integer number?
bool is_string_number(const string& s);
// extract first positive number or last positive number in string (result number has string type)
string find_first_number(string const & str); string find_first_double_number(string const & str);
string find_last_number(string const & str); string find_last_double_number(string const & str);
// convert array of chars to the new lowercase array
char* convert_pchar_to_lowercase_new(char* input_char_array);
// replace string 's' by string 'd' in text
void replace_string_in_text(string &text, string s, string d);
// return string without leading and trailing spaces and tabs
string trim(const string& str, const string& whitespace = " \t\r");
// return string changing whitespaces and tabs by single whitespace
string reduce(const string& str, const string& fill = " ", const string& whitespace = " \t\r");

/*   DIR & FILE FUNCTIONS   */
// check directory exists: 0 - not exists, 1 - exists, -2 - cannot access
int check_directory_exist(const char* path);
// check and create directory if not exists: 0 - not existed before, 1 - existed, -1 - errno error, -2 - cannot access
int create_directory(const char* path);
// get file name without extension from a path
string get_file_name(string path);
// get file name with extension from path
string get_file_name_with_ext(string path);

/*  TIME FUNCTIONS  */
// get current date as string
string get_current_date();


#ifndef ONLY_DECLARATIONS


/*              */
/*              */
/* OS FUNCTIONS */
/*              */
/*              */

// execute system command in shell (bash)
int system_command_linux(string aCommand, string& result)
{
    FILE* f;
    if (!(f = popen(aCommand.c_str(), "r")))
    {
    	cout<<"system_command_linux error: popen failed"<<endl;
        return 1;
    }

    const int BUFSIZE = 4096;
    char buf[BUFSIZE];
    if (fgets(buf,BUFSIZE,f) != NULL)
    	result = buf;

    pclose(f);
    return 0;
}

// change tilda symbol on HOME in linux path
string replace_home_symbol_linux(string path)
{
    size_t np = path.find_first_of('~');
    if (np != string::npos)
    {
        bool isNewPath = false;
        char* pPath = getenv("HOME");
        if (pPath == NULL)
        {
            pPath = new char[2];
            pPath[0] = '/';
            pPath[1] = '\0';
            isNewPath = true;
        }

        do
        {
        	path.erase(np, 1);
        	path.insert(np, pPath);
            np = path.find_first_of('~');
        } while (np != string::npos);

        if (isNewPath)
            delete[] pPath;
    }

    return path;
}

// change $VMCWORKDIR symbol in linux path
string replace_vmc_path_linux(string path)
{
    char* pPath = getenv("VMCWORKDIR");
    bool isNewPath = false;
    if (pPath == NULL)
    {
        pPath = new char[2];
        pPath[0] = '/';
        pPath[1] = '\0';
        isNewPath = true;
    }

    size_t found;
    while ((found = path.find("$VMCWORKDIR")) != string::npos)
    {
        path.replace(found, 11, pPath);
    }

    if (isNewPath)
        delete[] pPath;

    return path;
}

// get processor core count on this machine
int get_linux_processor_count()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

// get maximum available processor count of the batch system for the given (default) queue
int get_batch_processor_count(BATCH_SYSTEM_NAME batch_system, string queue_name)
{
    int proc_count = 0;

    const int MAX_BUFFER = 255;
    char buffer[MAX_BUFFER];
    string command_line, data, strDiff;

    //define shell command to get string with processor count for the queue
    if (batch_system == SGE_BATCH_SYSTEM)
    {
    	if (queue_name == "")
    		queue_name = "all.q";
    	command_line = string("export SGE_SINGLE_LINE=1; qconf -sq ") + queue_name + string(" | grep slots");
    }
    if (batch_system == TORQUE_BATCH_SYSTEM)
    {
    	if (queue_name == "")
        	queue_name = "batch";
        command_line = string("qstat -f -Q ") + queue_name + string(" | grep max_running");
    }
    if (batch_system == SLURM_BATCH_SYSTEM)
    {
    	if (queue_name == "")
    		queue_name = "interactive";
    	command_line = string("scontrol show partition ") + queue_name + string(" | grep -o 'TotalCPUs=[0-9]*'");
    }

    // run shell command to define batch processor count
    FILE* stream = popen(command_line.c_str(), "r");
    while (fgets(buffer, MAX_BUFFER, stream) != NULL)
    	data.append(buffer);
    pclose(stream);

    // parse result string to extract processor count
    size_t found = data.find("="), found2 = string::npos;
    if (batch_system == SGE_BATCH_SYSTEM)
    {
    	while (found != string::npos)
        {
    		found2 = data.find("]", found);
        	strDiff = data.substr(found+1, found2 - found - 1);
        	proc_count += atoi(strDiff.c_str());
        	strDiff.clear();

        	found = data.find("=", found2);
        }
    }
    if (batch_system == TORQUE_BATCH_SYSTEM)
    {
    	if (found != string::npos)
        {
    		strDiff = data.substr(found+1, data.length() - found - 1);
    		proc_count = atoi(strDiff.c_str());
        }
    }
    if (batch_system == SLURM_BATCH_SYSTEM)
    {
    	if (found != string::npos)
        {
        	strDiff = data.substr(found+1, data.length() - found - 1);
        	proc_count = atoi(strDiff.c_str());
        }
    }

	data.clear();
    //cout<<"Batch processor count: "<<proc_count<<endl;
    return proc_count;
}


/*                              */
/*				*/
/* GLOBAL APPLICATION FUNCTIONS */
/*				*/
/*                              */

// get application name in linux
string get_app_name_linux()
{
    pid_t procpid = getpid();
    stringstream toCom;
    toCom << "cat /proc/" << procpid << "/comm";
    string fRes="";
    system_command_linux(toCom.str(), fRes);
    size_t last_pos = fRes.find_last_not_of(" \n\r\t") + 1;
    if (last_pos != string::npos)
        fRes.erase(last_pos);

    return fRes;
}

// get aplication directory (path without file name) in linux
string get_app_dir_linux()
{
    pid_t procpid = getpid();
    string appName = get_app_name_linux();
    stringstream command;
    command <<  "readlink /proc/" << procpid << "/exe | sed \"s/\\(\\/" << appName << "\\)$//\"";
    string fRes;
    system_command_linux(command.str(),fRes);

    // remove '\n' from end of the string and add final '/'
    fRes = fRes.erase(fRes.length()-1, 1);
    fRes.push_back('/');

    return fRes;
}


/*                  */
/*                  */
/* NUMBER FUNCTIONS */
/*                  */
/*                  */



/*                  */
/*                  */
/* STRING FUNCTIONS */
/*                  */
/*                  */

// convert integer number to string
string convert_int_to_string(int number)
{
    stringstream ss;
    ss<<number;
    return ss.str();
}

// convert integer (hexadecimal value) to string with hexadecimal presentation without "0x"
string int_to_hex_string(int int_number)
{
    stringstream stream;
    stream<<std::hex<<int_number;
    return stream.str();
}

// convert string with hexadecimal presentation without "0x" to integer
int hex_string_to_int(string hex_string)
{
    int x;
    stringstream stream;
    stream<<std::hex<<hex_string;
    stream>>x;
    return x;
}

// is string an integer number?
bool is_string_number(const string& s)
{
    string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

string find_first_number(string const & str)
{
	size_t const n = str.find_first_of("0123456789");
	if (n != string::npos)
	{
		size_t const m = str.find_first_not_of("0123456789", n);
	    return str.substr(n, m != string::npos ? m-n : m);
	}

	return string();
}

string find_last_number(string const & str)
{
	size_t const n = str.find_last_of("0123456789");
	if (n != string::npos)
	{
		string temp = str.substr(0, n+1);
		size_t const m = temp.find_last_not_of("0123456789");
		if (m != string::npos) return temp.substr(m+1, n-m);
		return temp;
	}

	return string();
}

string find_first_double_number(string const & str)
{
	size_t const n = str.find_first_of("0123456789");
	if (n != string::npos)
	{
		size_t const m = str.find_first_not_of("0123456789,.", n);
	    return str.substr(n, m != string::npos ? m-n : m);
	}

	return string();
}

string find_last_double_number(string const & str)
{
	size_t const n = str.find_last_of("0123456789");
	if (n != string::npos)
	{
		string temp = str.substr(0, n+1);
		size_t const m = temp.find_last_not_of("0123456789,.");
		if (m != string::npos) return temp.substr(m+1, n-m);
		return temp;
	}

	return string();
}

// convert array of chars to the new lowercase array
char* convert_pchar_to_lowercase_new(char* input_char_array)
{
    if (input_char_array == NULL)
        return NULL;

    const int length = strlen(input_char_array);	// get the length of the text
    char* lower = new char[length + 1];		// allocate 'length' bytes + 1 (for null terminator) and cast to char*
    lower[length] = 0;						// set the last byte to a null terminator

    // copy all character bytes to the new buffer using tolower
    for( int i = 0; i < length; i++ )
        lower[i] = tolower(input_char_array[i]);


    return lower;
}

// replace string 's' by string 'd' in text
void replace_string_in_text(string &text, string old_substring, string new_substring)
{
    int start = -1;
    do
    {
        start = text.find(old_substring, start + 1);
        if (start > -1)
            text.replace(start, old_substring.length(), new_substring.c_str());
    }
    while (start > -1);
}

// return string without leading and trailing spaces and tabs
string trim(const string& str, const string& whitespace)
{
    size_t strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return ""; // no content

    size_t strEnd = str.find_last_not_of(whitespace);
    size_t strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

// return string changing whitespaces and tabs by single whitespace
string reduce(const string& str, const string& fill, const string& whitespace)
{
    // trim first
    string result = trim(str, whitespace);

    // replace sub ranges
    size_t beginSpace = result.find_first_of(whitespace);
    while (beginSpace != string::npos)
    {
        size_t endSpace = result.find_first_not_of(whitespace, beginSpace);
        size_t range = endSpace - beginSpace;

        result.replace(beginSpace, range, fill);

        size_t newStart = beginSpace + fill.length();
        beginSpace = result.find_first_of(whitespace, newStart);
    }

    return result;
}


/*                          */
/*                          */
/*   DIR & FILE FUNCTIONS   */
/*                          */
/*                          */

// check directory exists: 0 - not exists, 1 - exists, -2 - cannot access
int check_directory_exist(const char* path)
{
    struct stat info;

    if (stat(path, &info) != 0)
        return -2;
    else if (info.st_mode & S_IFDIR)
        return 1;
    else
        return 0;
}

// check and create directory if not exists: 0 - not existed before, 1 - existed, -1 - errno error, -2 - cannot access
int create_directory(const char* path)
{
    struct stat info;

    if (stat(path, &info) != 0)
        return -2;
    else if (info.st_mode & S_IFDIR)
        return 1;
    else
    {
        int status = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        return status;
    }
}

// get file name without extension from a path
string get_file_name(string path)
{
    // Remove directory if present.
    size_t last_slash_idx = path.find_last_of("/");
    if (string::npos != last_slash_idx)
        path.erase(0, last_slash_idx + 1);

    // Remove extension if present.
    size_t period_idx = path.rfind('.');
    if (string::npos != period_idx)
        path.erase(period_idx);

    return path;
}

// get file name with extension from path
string get_file_name_with_ext(string path)
{
    // Remove directory if present.
    size_t last_slash_idx = path.find_last_of("/");
    if (string::npos != last_slash_idx)
        path.erase(0, last_slash_idx + 1);

    return path;
}


/*                  */
/*                  */
/*  TIME FUNCTIONS  */
/*                  */
/*                  */

// get current date as string
string get_current_date()
{
    time_t rawtime;
    time(&rawtime);

    struct tm* timeinfo;
    timeinfo = localtime(&rawtime);

    char buffer[80];
    strftime(buffer, 80, "%d-%m-%Y", timeinfo);
    string str(buffer);

    return str;
}

#endif /* ONLY_DECLARATIONS */
#endif /* FUNCTION_SET_H */
