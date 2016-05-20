#include "RawDataParser.h"
#include "function_set.h"

#include <cerrno>
#include <iostream>
using namespace std;

RawDataParser::RawDataParser()
{
    device_serial[0x1A8D] = 1;
    device_serial[0x2950] = 2;
    device_serial[0x3043] = 3;
    device_serial[0x304E] = 4;
    device_serial[0x4514] = 5;
    device_serial[0x45DF] = 6;

    device_serial1[0x3F8B] = 1;
    device_serial1[0x2950] = 2;
    device_serial1[0x4514] = 3;
    device_serial1[0x4504] = 4;
    device_serial1[0x3043] = 5;
    device_serial1[0x304E] = 6;

    device_serial2[0x47CB] = 1;
    device_serial2[0x2A79] = 2;
    device_serial2[0x3F1D] = 3;
    device_serial2[0x4513] = 4;
    device_serial2[0x3F97] = 5;
    device_serial2[0x2FFF] = 6;
}

RawDataParser::~RawDataParser()
{
}

//buffer - unsigned int array from file with HRB Raw Data Format 1.0
//size - size of 'buffer' variable in words (unsigned int)
//fEventReadData - vector with EventData objects
int RawDataParser::ParseHRB1Buffer(unsigned int* buffer, long size, vector<EventData*>* fEventData)
{
    unsigned char byte_hh;//, byte_hl, byte_lh, byte_ll;
    unsigned int int_h, int_l;
    long cur_word = 0, i = 0;
    //unsigned int max_diff = 0,  = 0;
    cout<<"enter of parsing file"<<endl;
    //sleep(1);
    int max_event_diff = 0, ind = 0;
    while (cur_word < size)
    {
        ind++;
        // M-Link HEADER - frame type
        unsigned int x = buffer[cur_word++];

        // check sync word: 0x2A50
        int_h = x >> 16;
        if (int_h != 0x2A50)
        {
            fputs("sync word error, should be 0x2A50", stderr);
            return -1;
        }

        // check frame type: 0x5354 - stream data
        int_l = x & 0xFFFF;
        if (int_l != 0x5354)
        {
            fputs("frame type error, should be 0x5354", stderr);
            return -2;
        }

        // M-Link HEADER - frame info
        x = buffer[cur_word++];

        unsigned int frame_length = x >> 16;
        unsigned int frame_number = x & 0xFFFF;
        //cout<<"frame_length: "<<frame_length<<endl;
        //cout<<"frame_number: "<<frame_number<<endl;

        // skip destination and source address
        cur_word++;

        // M-Stream Header - data type and flags
        x = buffer[cur_word++];
        byte_hh = *((unsigned char*)&x + 3);
        if (byte_hh != 0xBC)
        {
            fputs("data type error, should be 0xBC", stderr);
            return -3;
        }
        unsigned int fragment_length = x & 0xFFFF;
        //cout<<"fragment_length: "<<fragment_length<<endl;

        // skip fragment ID and offset
        cur_word++;

        // M-Stream Payload - device serial (define plane number starting with 1)
        x = buffer[cur_word++];
        unsigned int plane_number = device_serial.find(x & 0xFFFF)->second;
        //cout<<"plane_number: "<<plane_number<<endl;

        // M-Stream Payload - event (0:23 bits) and channel number
        x = buffer[cur_word++];
        unsigned int event_number = x & 0xFFFFFF;
        //cout<<"event_number: "<<event_number<<endl;
        //if (event_number > max_event)
        //    max_event = event_number;
        //if ((max_event - event_number) > max_diff)
        //    max_diff = max_event - event_number;
        /*
        vector<EventData*>::iterator it = fEventReadData->begin();
        EventData* pCurEvent = NULL;
        for (i = 0; i < fEventReadData->size(); i++, it++)
        {
            pCurEvent = (*fEventReadData)[i];

            if (pCurEvent->uiEventNumber == event_number)
                break;

            if (pCurEvent->uiEventNumber > event_number)
            {
                pCurEvent = new EventData();
                pCurEvent->uiEventNumber = event_number;

                fEventReadData->insert(it, pCurEvent);
                //cout<<"event was inner added: "<<i<<" event_number="<<event_number<<endl;
                break;
            }
        }
        if (i == fEventReadData->size())
        {
            pCurEvent = new EventData();
            pCurEvent->uiEventNumber = event_number;

            fEventReadData->push_back(pCurEvent);
            //cout<<"event was outer added: "<<i<<" event_number="<<event_number<<endl;
        }
        */

        // M-Stream Payload - timestamp (high and low)
        x = buffer[cur_word++];
        ULong64_t event_timestamp = x + (((ULong64_t)buffer[cur_word++]) << 32);
        //ULong64_t event_timestamp = (((ULong64_t)x) << 32) + buffer[cur_word++];

        vector<EventData*>::iterator it = fEventData->end();
        EventData* pCurEvent = NULL;
        int cnt = 0;
        for (i = fEventData->size() - 1; i >= 0; i--, it--, cnt++)
        {
            pCurEvent = (*fEventData)[i];

            if (pCurEvent->event_timestamp < event_timestamp)
            {
                pCurEvent = new EventData();
                pCurEvent->event_timestamp = event_timestamp;

                //if (i == (fEventReadData->size()-1))
                //    fEventReadData->push_back(pCurEvent);
                //else
                fEventData->insert(it, pCurEvent);

                break;
            }

            if (pCurEvent->event_timestamp == event_timestamp)
                break;
        }// for
        if (i < 0)
        {
            pCurEvent = new EventData();
            pCurEvent->event_timestamp = event_timestamp;

            if (fEventData->size() == 0)
                fEventData->push_back(pCurEvent);
            else
            {
                if (*it != *(fEventData->begin()))
                {
                    cout<<"DATA ERROR: it != fEventReadData->begin()"<<endl;
                    sleep(3);
                }

                fEventData->insert(it, pCurEvent);
            }
        }
        if (cnt > max_event_diff)
            max_event_diff = cnt;

        // define vector for digits by plane_number
        vector<BmnMwpcDigit*>* pDigitsPlane = &pCurEvent->MWPC1Planes[plane_number-1];

        // M-Stream data - searching for "1" bits - working only with 32-bit words ratio, can be generalized
        if (((fragment_length / 16) * 16) != fragment_length)
        {
            fputs("fragment length isn't aligned with 4 32-bit word", stderr);
            return -4;
        }
        int time_bin = 1;   // starting from 1
        for (unsigned int ui = 16; ui < fragment_length; ui += 16, time_bin++)
        {
            // 96 bit for every wire and 32 bit is empty (zero)
            for (int j = 0; j < 3; j++)
            {
                x = buffer[cur_word++];

                int pos = sizeof(unsigned int)*8 - 1;
                while ((pos--) >= 0)
                {
                    if (CHECK_BIT(x, pos))
                    {
                        // starting number from 0
                        int active_wire = j*32 + pos;

                        //cout<<"event timestamp: "<<event_timestamp<<"; plane: "<<plane_number<<"; time: "<<time_bin<<"; wire: "<<active_wire<<endl;
                        //sleep(1);

                        BmnMwpcDigit* pDigit = new BmnMwpcDigit(plane_number, active_wire, time_bin, -1);

                        pDigitsPlane->push_back(pDigit);
                    }
                }
            }
            cur_word++;
        }// for detector's wires

        // skip M-Link CRC
        cur_word++;

        //cout<<endl<<endl;
    }// while not end of file

    cout<<"exit of parsing file"<<" iterations count: "<<ind<<endl;
    //cout<<"event count: "<<fEventReadData->size()<<endl;
    //cout<<"max_event_diff: "<<max_event_diff<<endl;
    //sleep(3);
}

//buffer - unsigned int array from file with HRB Raw Data Format 2.0
//size - size of 'buffer' variable in words (unsigned int)
//fEventData - vector with EventData objects
int RawDataParser::ParseHRB2Buffer(unsigned int* buffer, long size, vector<EventData*>* fEventData)
{
    unsigned char byte_hh;  //, byte_hl, byte_lh, byte_ll;
    unsigned int int_h, int_l;
    long cur_word = 0, i = 0;
    //unsigned int max_diff = 0,  = 0;
    cout<<"enter of parsing file"<<endl;
    //sleep(1);
    int max_event_diff = 0, ind = 0;
    while (cur_word < size)
    {
        ind++;
        // M-Link HEADER - frame type
        unsigned int x = buffer[cur_word++];

        // check sync word: 0x2A50
        int_h = x >> 16;
        if (int_h != 0x2A50)
        {
            cout<<"sync word error, should be 0x2A50, but it's equal to "<<int_h<<endl;
            return -1;
        }

        // check frame type: 0x2A50 - stream data 2.0
        int_l = x & 0xFFFF;
        if (int_l != 0x2A50)
        {
            fputs("frame type error, should be 0x2A50", stderr);
            return -2;
        }

        // M-Link HEADER - frame info
        x = buffer[cur_word++];

        unsigned int frame_length = x >> 16;
        unsigned int frame_number = x & 0xFFFF;
        //cout<<"frame_length: "<<frame_length<<endl;
        //cout<<"frame_number: "<<frame_number<<endl;

        // skip destination and source address
        cur_word++;

        int MWPC_number = -1;
        unsigned int plane_number = 0;
        // M-Stream Subtype 0 - device serial id (define plane number starting with 1)
        x = buffer[cur_word++];
        int_l = x & 0xFFFF;
        //cout<<"device_serial: 0x"<<int_to_hex_string(int_l)<<endl;
        map<int,int>::iterator it_device = device_serial1.find(int_l);
        if (it_device != device_serial1.end())
        {
            MWPC_number = 1;
            plane_number = it_device->second;
        }
        else
        {
            MWPC_number = 2;
            it_device = device_serial2.find(int_l);
            if (it_device == device_serial2.end())
            {
                cout<<"error: device serial wasn't found in the map, exiting of parsing this file"<<endl;
                return -3;
            }
            plane_number = it_device->second;
        }
        //cout<<"MWPC number: "<<MWPC_number<<". plane_number: "<<plane_number<<endl;

        // M-Stream Subtype 0 - device_id and fragment_length
        x = buffer[cur_word++];
        unsigned int fragment_length = x & 0xFFFF;
        //cout<<"fragment_length: "<<fragment_length<<endl;
        unsigned int device_id = x >> 24;
        //cout<<"device_id: 0x"<<int_to_hex_string(device_id)<<endl;

        // M-Stream Subtype 0 - event (0:23 bits)
        x = buffer[cur_word++];
        unsigned int event_number = x & 0xFFFFFF;
        //cout<<"event_number: "<<event_number<<endl;

        // M-Stream Subtype 0 - trigger timestamps: nanoseconds (32-bit word) and then seconds (32-bit word), e.i. low the high timestamp
        x = buffer[cur_word++];
        //unsigned int timestamp_seconds = x;
        //x = buffer[cur_word++];
        //unsigned int timestamp_nanoseconds = x;
        ULong64_t event_timestamp = x + (((ULong64_t)buffer[cur_word++]) << 32);
        //ULong64_t event_timestamp = (((ULong64_t)x) << 32) + buffer[cur_word++];

        vector<EventData*>::iterator it = fEventData->end();
        EventData* pCurEvent = NULL;
        int cnt = 0;
        for (i = fEventData->size() - 1; i >= 0; i--, it--, cnt++)
        {
            pCurEvent = (*fEventData)[i];

            if (pCurEvent->event_timestamp < event_timestamp)
            {
                pCurEvent = new EventData();
                pCurEvent->event_timestamp = event_timestamp;

                //if (i == (fEventData->size()-1))
                //    fEventData->push_back(pCurEvent);
                //else
                fEventData->insert(it, pCurEvent);

                break;
            }

            if (pCurEvent->event_timestamp == event_timestamp)
                break;
        }// for
        if (i < 0)
        {
            pCurEvent = new EventData();
            pCurEvent->event_timestamp = event_timestamp;

            if (fEventData->size() == 0)
                fEventData->push_back(pCurEvent);
            else
            {
                if (*it != *(fEventData->begin()))
                {
                    cout<<"DATA ERROR: it != fEventData->begin()"<<endl;
                    sleep(3);
                }

                fEventData->insert(it, pCurEvent);
            }
        }
        if (cnt > max_event_diff)
            max_event_diff = cnt;

        // define vector for digits by MWPC and plane number
        vector<BmnMwpcDigit*>* pDigitsPlane;
        if (MWPC_number == 1)
            pDigitsPlane = &pCurEvent->MWPC1Planes[plane_number-1];
        else
            pDigitsPlane = &pCurEvent->MWPC2Planes[plane_number-1];

        // M-Stream data - searching for "1" bits - working only with 32-bit words ratio, can be generalized
        unsigned int start_data = 12;
        if ((fragment_length - start_data) % 16 != 0)
        {
            cout<<"data length isn't aligned with 4 32-bit word"<<endl;
            return -4;
        }
        int time_bin = 1;   // starting from 1
        int pos_bits = sizeof(unsigned int)*8 - 1, pos;
        for (unsigned int ui = start_data; ui < fragment_length; ui += 16, time_bin++)
        {
            // 96 bit for every wire and 32 bit is empty (zero)
            for (int j = 0; j < 3; j++)
            {
                x = buffer[cur_word++];
                if (x == 0)
                    continue;

                pos = pos_bits;
                while ((pos--) >= 0)
                {
                    if (CHECK_BIT(x, pos))
                    {
                        // starting number from 0
                        int active_wire = j*32 + pos;

                        //cout<<"time stamp: "<<event_timestamp<<", bin: "<<time_bin<<"; MWPC: "<<MWPC_number<<"; plane: "<<plane_number<<"; wire: "<<active_wire<<endl;
                        //sleep(1);

                        BmnMwpcDigit* pDigit = new BmnMwpcDigit(plane_number, active_wire, time_bin, -1);

                        pDigitsPlane->push_back(pDigit);
                    }
                }
            }
            cur_word++;
        }// for detector's wires

        // skip M-Link CRC
        //cur_word++;

        //cout<<endl<<endl;
    }// while not end of file

    cout<<"exit of parsing file"<<" iterations count: "<<ind<<endl;
    cout<<"event count: "<<fEventData->size()<<endl;
    //cout<<"max_event_diff: "<<max_event_diff<<endl;
    //sleep(1);

    return 0;
}

//buffer - unsigned int array from file with HRB Raw Data Format 2.0
//size - size of 'buffer' variable in words (unsigned int)
//fEventData - vector with EventData objects
int RawDataParser::ParseWaveBuffer(unsigned int* buffer, long size, vector<EventData*>* fEventData)
{
    unsigned char byte_hh;  //, byte_hl, byte_lh, byte_ll;
    unsigned int int_h, int_l;
    long cur_word = 0, i = 0;
    //unsigned int max_diff = 0,  = 0;
    cout<<"enter of parsing file"<<endl;
    //sleep(1);
    int max_event_diff = 0, ind = 0;
    while (cur_word < size)
    {
        ind++;
        // M-Link HEADER - frame type
        unsigned int x = buffer[cur_word++];

        // check sync word: 0x2A50
        int_h = x >> 16;
        if (int_h != 0x2A50)
        {
            cout<<"sync word error, should be 0x2A50, but it's equal to "<<int_h<<endl;
            return -1;
        }

        // check frame type: 0x2A50 - stream data 2.0
        int_l = x & 0xFFFF;
        if (int_l != 0x2A50)
        {
            fputs("frame type error, should be 0x2A50", stderr);
            return -2;
        }

        // M-Link HEADER - frame info
        x = buffer[cur_word++];

        unsigned int frame_length = x >> 16;
        unsigned int frame_number = x & 0xFFFF;
        cout<<"frame_length: "<<frame_length<<endl;
        cout<<"frame_number: "<<frame_number<<endl;

        // M-Stream SUBTYPE 0 Event Header (4 * DWORD)
        // skip destination and source address
        cur_word++;

        // M-Stream Subtype 0 - device serial id (define plane number starting with 1)
        x = buffer[cur_word++];
        int_h = x >> 16;
        int_l = x & 0xFFFF;
        cout<<"device_serial: 0x"<<int_to_hex_string(int_h)<<int_to_hex_string(int_l)<<endl;

        // M-Stream Subtype 0 - device_id and fragment_length
        x = buffer[cur_word++];
        unsigned int fragment_length = x & 0xFFFF;
        cout<<"fragment_length: "<<fragment_length<<endl;
        unsigned int device_id = x >> 24;
        cout<<"device_id: 0x"<<int_to_hex_string(device_id)<<endl;

        // M-Stream Subtype 0 - event (0:23 bits)
        x = buffer[cur_word++];
        unsigned int event_number = x & 0xFFFFFF;
        cout<<"event_number: "<<event_number<<endl;

        // M-Stream Subtype 0 - readout channels 31:0 bit mask
        cur_word++;

        // M-Stream Subtype 0 - readout channels 63:32 bit mask
        cur_word++;




        // M-Stream SUBTYPE 1 Event Header (4 * DWORD)


/*
        // M-Stream Subtype 0 - trigger timestamps: nanoseconds (32-bit word) and then seconds (32-bit word), e.i. low the high timestamp
        x = buffer[cur_word++];
        //unsigned int timestamp_seconds = x;
        //x = buffer[cur_word++];
        //unsigned int timestamp_nanoseconds = x;
        ULong64_t event_timestamp = x + (((ULong64_t)buffer[cur_word++]) << 32);
        //ULong64_t event_timestamp = (((ULong64_t)x) << 32) + buffer[cur_word++];

        vector<EventData*>::iterator it = fEventData->end();
        EventData* pCurEvent = NULL;
        int cnt = 0;
        for (i = fEventData->size() - 1; i >= 0; i--, it--, cnt++)
        {
            pCurEvent = (*fEventData)[i];

            if (pCurEvent->event_timestamp < event_timestamp)
            {
                pCurEvent = new EventData();
                pCurEvent->event_timestamp = event_timestamp;

                //if (i == (fEventData->size()-1))
                //    fEventData->push_back(pCurEvent);
                //else
                fEventData->insert(it, pCurEvent);

                break;
            }

            if (pCurEvent->event_timestamp == event_timestamp)
                break;
        }// for
        if (i < 0)
        {
            pCurEvent = new EventData();
            pCurEvent->event_timestamp = event_timestamp;

            if (fEventData->size() == 0)
                fEventData->push_back(pCurEvent);
            else
            {
                if (*it != *(fEventData->begin()))
                {
                    cout<<"DATA ERROR: it != fEventData->begin()"<<endl;
                    sleep(3);
                }

                fEventData->insert(it, pCurEvent);
            }
        }
        if (cnt > max_event_diff)
            max_event_diff = cnt;

        // define vector for digits by MWPC and plane number
        vector<BmnMwpcDigit*>* pDigitsPlane;
        if (MWPC_number == 1)
            pDigitsPlane = &pCurEvent->MWPC1Planes[plane_number-1];
        else
            pDigitsPlane = &pCurEvent->MWPC2Planes[plane_number-1];

        // M-Stream data - searching for "1" bits - working only with 32-bit words ratio, can be generalized
        unsigned int start_data = 12;
        if ((fragment_length - start_data) % 16 != 0)
        {
            cout<<"data length isn't aligned with 4 32-bit word"<<endl;
            return -4;
        }
        int time_bin = 1;   // starting from 1
        int pos_bits = sizeof(unsigned int)*8 - 1, pos;
        for (unsigned int i = start_data; i < fragment_length; i += 16, time_bin++)
        {
            // 96 bit for every wire and 32 bit is empty (zero)
            for (int j = 0; j < 3; j++)
            {
                x = buffer[cur_word++];
                if (x == 0)
                    continue;

                pos = pos_bits;
                while ((pos--) >= 0)
                {
                    if (CHECK_BIT(x, pos))
                    {
                        // starting number from 0
                        int active_wire = j*32 + pos;

                        //cout<<"time stamp: "<<event_timestamp<<", bin: "<<time_bin<<"; MWPC: "<<MWPC_number<<"; plane: "<<plane_number<<"; wire: "<<active_wire<<endl;
                        //sleep(1);

                        BmnMwpcDigit* pDigit = new BmnMwpcDigit(plane_number, active_wire, time_bin, -1);

                        pDigitsPlane->push_back(pDigit);
                    }
                }
            }
            cur_word++;
        }// for detector's wires
*/
        // skip M-Link CRC
        //cur_word++;

        //cout<<endl<<endl;
    }// while not end of file

    cout<<"exit of parsing file"<<" iterations count: "<<ind<<endl;
    cout<<"event count: "<<fEventData->size()<<endl;
    //cout<<"max_event_diff: "<<max_event_diff<<endl;
    //sleep(1);

    return 0;
}

void RawDataParser::GenerateMWPCFileNames(char* raw_file_name_begin, map<int, int>* p_device_serial, TString* mwpc_file_names)
{
    map<int,int>::iterator it_planes = p_device_serial->begin();

    // cycle for six plane (six device identificator)
    for (int i = 1;  i <= p_device_serial->size(); i++, it_planes++)
    {
        mwpc_file_names[i-1] = raw_file_name_begin;
        mwpc_file_names[i-1] += int_to_hex_string(it_planes->first);
        mwpc_file_names[i-1] += ".dat";
        //cout<<"MWPC: "<<mwpc_file_names[i-1]<<endl;
    }
}

void RawDataParser::ParseHRBFiles(vector<EventData*>* pEventData, TString* raw_file_names, long* lStart)
{
     // buffer with 4-bytes words
     unsigned int* buffer;
     long lSize, curStart;
     size_t size;
     FILE* pRawFile;

     for (int mwpc_iter = 1; mwpc_iter < 3; mwpc_iter++)
     {
         // cycle for six plane (six device identificator)
         for (int i = 1;  i <= 6; i++)
         {
             pRawFile = fopen(replace_vmc_path_linux(raw_file_names[(mwpc_iter-1)*6 + (i-1)].Data()).data(), "rb");
             if (pRawFile == NULL)
             {
                 fprintf(stderr,"Error opening file (%s): %s\n", strerror(errno), raw_file_names[(mwpc_iter-1)*6 + (i-1)].Data());
                 return;
             }

             // obtain file size:
             fseek(pRawFile, 0, SEEK_END);
             lSize = ftell(pRawFile);

             if (lStart == NULL)
                 curStart = 0;
             else
                 curStart = lStart[(mwpc_iter-1)*6 + (i-1)];

             if (lSize <= curStart)
             {
                 fclose(pRawFile);
                 return;
             }

             fseek(pRawFile, curStart, SEEK_SET);

             // allocate memory containing new tail of file:
             buffer =  new unsigned int[(lSize-curStart) / 4];
             if (buffer == NULL)
             {
                 fputs("Memory error", stderr);
                 fclose(pRawFile);
                 return;
             }

             // copy the file into the buffer:
             size = fread(buffer, 4, (lSize-curStart)/4, pRawFile);
             if (size != (lSize-curStart)/4)
             {
                 fputs("Reading error", stderr);
                 free(buffer);
                 fclose(pRawFile);
                 return;
             }

             // the whole file is now loaded in the memory 'buffer'
             fclose(pRawFile);

             //parse the data
             int result_error = ParseHRB2Buffer(buffer, (lSize-curStart)/4, pEventData);

             free(buffer);

             if (lStart != NULL)
                lStart[(mwpc_iter-1)*6 + (i-1)] = lSize;
         }// for cycle for six plane (six device identificator)
     }// for cycle for MWPC detectors

     return;
}
