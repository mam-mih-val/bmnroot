#include "EventCatalogue.h"
#include "event_db_settings.h"

#include <curl/curl.h>
#include <json.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

using json = nlohmann::ordered_json;

static size_t writeFunction(void* contents, size_t size, size_t nmemb, string* data)
{
    size_t realsize = size * nmemb;
    try
    {
        data->append((char*)contents, realsize);
    }
    catch (std::bad_alloc& e)
    {
      //handle memory problem
      return 0;
    }

    return realsize;
}

// function for adding new event metadata to the Event Catalogue from a JSON file
// returns a number (>=0) of successfully added metadata records or error_code (<0) in case of failures
int EventCatalogue::WriteEventMetadata(string json_file_path)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if (!curl)
    {
        cout<<"ERROR: CURL initialisation was failed"<<endl;
        return -1;
    }

    ifstream json_file(json_file_path);
    ostringstream sstr;
    sstr<<json_file.rdbuf();
    string json_string = sstr.str();
    //cout<<"json_string = "<<json_string<<endl;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Accept: */*");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    string strServiceUrl = EVENT_API_HOST + string("/") + EVENT_API_NAME;
    curl_easy_setopt(curl, CURLOPT_URL, strServiceUrl.c_str());

    string strAccount = EVENT_API_USERNAME + string(":") + EVENT_API_PASSWORD;
    curl_easy_setopt(curl, CURLOPT_USERPWD, strAccount.c_str());

    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_string.c_str());

    string response_string;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

    // write new event metadata to the Event Catalogue
    int res = curl_easy_perform(curl);
    if (res)
    {
        cout<<"ERROR: sending command to '"<<strServiceUrl<<"' failed with code = "<<res<<endl;
        return -2;
    }

    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    //double elapsed;
    //curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &elapsed);
    //char* url;
    //curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &url);
    //cout<<"CURLINFO_TOTAL_TIME = "<<elapsed<<endl;
    if (response_code == 200)
        //cout<<"Event metadata were written from JSON file '"<<json_path<<"' to the Event Catalogue"<<endl<<endl;
        cout<<"Request was successfully finished: "<<response_string<<endl;
    else
    {
        cout<<"ERROR: sending request to '"<<strServiceUrl<<"' was failed with response: "<<response_string<<" (code = "<<response_code<<")"<<endl;
        return -3;
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    curl_global_cleanup();

    return 0;
}

ClassImp(EventCatalogue)
