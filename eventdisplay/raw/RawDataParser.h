#ifndef RAWDATAPARSER_H
#define RAWDATAPARSER_H

#include "BmnMwpcDigit.h"

#include <map>

class EventData : public TObject
{
  public:
    ULong64_t event_timestamp;

    vector<BmnMwpcDigit*> MWPC1Planes[6];
    vector<BmnMwpcDigit*> MWPC2Planes[6];

    EventData()
    {
    }

    EventData(const EventData& event_data)
    {
        //uiEventNumber = event_data.uiEventNumber;
        event_timestamp = event_data.event_timestamp;

        for (int i = 0; i < 6; i++)
            MWPC1Planes[i] = event_data.MWPC1Planes[i];

        for (int i = 0; i < 6; i++)
            MWPC2Planes[i] = event_data.MWPC2Planes[i];
    }

    virtual ~EventData() {}

    ClassDef(EventData, 0) //EventData
};

class RawDataParser
{
  public:
    //map with device identificator corresponding plane number (starting with 1)
    map<int, int> device_serial;     // { {0x1A8D, 1}, {0x2950, 2}, {0x3043, 3}, {0x304E, 4}, {0x4514, 5}, {0x45DF, 6} };
    map<int, int> device_serial1;    // { {0x3F8B, 1}, {0x2950, 2}, {0x4514, 3}, {0x4504, 4}, {0x3043, 5}, {0x304E, 6} };
    map<int, int> device_serial2;    // { {0x47CB, 1}, {0x2A79, 2}, {0x3F1D, 3}, {0x4513, 4}, {0x3F97, 5}, {0x2FFF, 6} };

    /** Default constructor **/
    RawDataParser();
    /** Destructor **/
    virtual ~RawDataParser();

    //buffer - unsigned int array from file with HRB Raw Data Format 1.0 (MWPC)
    //size - size of 'buffer' variable in words (unsigned int)
    //fEventData - vector with EventData objects
    int ParseHRB1Buffer(unsigned int* buffer, long size, vector<EventData*>* fEventData);

    //buffer - unsigned int array from file with HRB Raw Data Format 2.0 (MWPC)
    //size - size of 'buffer' variable in words (unsigned int)
    //fEventData - vector with EventData objects
    int ParseHRB2Buffer(unsigned int* buffer, long size, vector<EventData*>* fEventData);

    //buffer - unsigned int array from file with MStreamWaveformDigitizer Format (ZDC + ECAL)
    //size - size of 'buffer' variable in words (unsigned int)
    //fEventData - vector with EventData objects
    int ParseWaveBuffer(unsigned int* buffer, long size, vector<EventData*>* fEventData);

    void GenerateMWPCFileNames(char* raw_file_name_begin, map<int, int>* device_serial, TString* mwpc_file_names);
    void ParseHRBFiles(vector<EventData*>* pEventData, TString* raw_file_names, long* lStart = NULL);
};

#endif // RAWDATAPARSER_H
