#include "OnlineEventManagerEditor.h"

#include "OnlineEventManager.h"         // for OnlineEventManager
#include "globals.h"
#include "FairRootManager.h"            // for FairRootManager
#include "FairRunAna.h"                 // for FairRunAna

#include "TChain.h"                     // for TChain
#include "TFile.h"                      // for TFile
#include "TGLayout.h"                   // for TGLayoutHints, etc
#include "TGeoManager.h"                // for TGeoManager, gGeoManager
#include "TString.h"                    // for TString
#include "TEveManager.h"                // for TEveManager, gEve
#include "TEveElement.h"                // for TEveElement
#include "TPRegexp.h"

#include "TEvePointSet.h"               // for TEvePointSet

#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TFrame.h"

#define MAXE 12

//map with device identificator corresponding plane number (starting with 1)
map<int, int> device_serial;// = { {0x1A8D, 1}, {0x2950, 2}, {0x3043, 3}, {0x304E, 4}, {0x4514, 5}, {0x45DF, 6} };
map<int, int> device_serial1;// = { {0x3F8B, 1}, {0x2950, 2}, {0x4514, 3}, {0x4504, 4}, {0x3043, 5}, {0x304E, 6} };
map<int, int> device_serial2;// = { {0x47CB, 1}, {0x2A79, 2}, {0x3F1D, 3}, {0x4513, 4}, {0x3F97, 5}, {0x2FFF, 6} };

/*map<int,int> create_map()
{
  map<int,int> m;
  m[0x1A8D] = 1;
  m[0x2950] = 2;
  m[0x3043] = 3;
  m[0x304E] = 4;
  m[0x4514] = 5;
  m[0x45DF] = 6;

  return m;
}*/

map<int,int> create_map1()
{
  map<int,int> m;
  m[0x3F8B] = 1;
  m[0x2950] = 2;
  m[0x4514] = 3;
  m[0x4504] = 4;
  m[0x3043] = 5;
  m[0x304E] = 6;

  return m;
}

map<int,int> create_map2()
{
  map<int,int> m;
  m[0x47CB] = 1;
  m[0x2A79] = 2;
  m[0x3F1D] = 3;
  m[0x4513] = 4;
  m[0x3F97] = 5;
  m[0x2FFF] = 6;

  return m;
}

// convert integer (hexadecimal value) to string with hexadecimal presentation without "0x"
string int_to_hex_string(int int_number)
{
  stringstream stream;
  stream << std::hex << int_number;
  return stream.str();
}

// check bit in 'variable' at 'position'
#define CHECK_BIT(variable,position) ((variable) & (1ULL<<(position)))


//buffer - unsigned int array from file with HRB Raw Data Format 1.0
//size - size of 'buffer' variable in words (unsigned int)
//fEventReadData - vector with EventData objects
//fEventDrawData - vector with EventDrawData objects
//semEventData - semaphore to add EventData objects to global vector
int ParseHRB1Buffer(unsigned int* buffer, long size, vector<EventData*>* fEventReadData,
                    vector<EventData*>* fEventDrawData, TSemaphore* semEventData)
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

        vector<EventData*>::iterator it = fEventReadData->end();
        EventData* pCurEvent = NULL;
        int cnt = 0;
        for (i = fEventReadData->size() - 1; i >= 0; i--, it--, cnt++)
        {
            pCurEvent = (*fEventReadData)[i];

            if (pCurEvent->event_timestamp < event_timestamp)
            {
                pCurEvent = new EventData();
                pCurEvent->event_timestamp = event_timestamp;

                //if (i == (fEventReadData->size()-1))
                //    fEventReadData->push_back(pCurEvent);
                //else
                fEventReadData->insert(it, pCurEvent);

                break;
            }

            if (pCurEvent->event_timestamp == event_timestamp)
                break;
        }// for
        if (i < 0)
        {
            pCurEvent = new EventData();
            pCurEvent->event_timestamp = event_timestamp;

            if (fEventReadData->size() == 0)
                fEventReadData->push_back(pCurEvent);
            else
            {
                if (*it != *(fEventReadData->begin()))
                {
                    cout<<"DATA ERROR: it != fEventReadData->begin()"<<endl;
                    sleep(3);
                }

                fEventReadData->insert(it, pCurEvent);
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
        for (unsigned int i = 16; i < fragment_length; i += 16, time_bin++)
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
//fEventReadData - vector with EventData objects
//fEventDrawData - vector with EventDrawData objects
//semEventData - semaphore to add EventData objects to global vector
int ParseHRB2Buffer(unsigned int* buffer, long size, vector<EventData*>* fEventReadData,
                    vector<EventData*>* fEventDrawData, TSemaphore* semEventData)
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

        vector<EventData*>::iterator it = fEventReadData->end();
        EventData* pCurEvent = NULL;
        int cnt = 0;
        for (i = fEventReadData->size() - 1; i >= 0; i--, it--, cnt++)
        {
            pCurEvent = (*fEventReadData)[i];

            if (pCurEvent->event_timestamp < event_timestamp)
            {
                pCurEvent = new EventData();
                pCurEvent->event_timestamp = event_timestamp;

                //if (i == (fEventReadData->size()-1))
                //    fEventReadData->push_back(pCurEvent);
                //else
                fEventReadData->insert(it, pCurEvent);

                break;
            }

            if (pCurEvent->event_timestamp == event_timestamp)
                break;
        }// for
        if (i < 0)
        {
            pCurEvent = new EventData();
            pCurEvent->event_timestamp = event_timestamp;

            if (fEventReadData->size() == 0)
                fEventReadData->push_back(pCurEvent);
            else
            {
                if (*it != *(fEventReadData->begin()))
                {
                    cout<<"DATA ERROR: it != fEventReadData->begin()"<<endl;
                    sleep(3);
                }

                fEventReadData->insert(it, pCurEvent);
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

                        cout<<"time stamp: "<<event_timestamp<<", bin: "<<time_bin<<"; MWPC: "<<MWPC_number<<"; plane: "<<plane_number<<"; wire: "<<active_wire<<endl;
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
    cout<<"event count: "<<fEventReadData->size()<<endl;
    //cout<<"max_event_diff: "<<max_event_diff<<endl;
    sleep(1);

    return 0;
}

void* ReadMWPCFiles(void* ptr)
{
     ThreadParam_ReadFile* thread_par = (ThreadParam_ReadFile*) ptr;
     char* raw_file_name_begin = thread_par->raw_file_name_begin;
     vector<EventData*>* fEventReadData = thread_par->fEventReadData;
     vector<EventData*>* fEventDrawData = thread_par->fEventDrawData;

     // buffer with 4-bytes words
     unsigned int* buffer;
     long curStart = 0, lSize = 0;
     long lStart[12] = { 0 };
     size_t size;
     FILE* pRawFile;
     TString file_name_begin(raw_file_name_begin), raw_file;
     int processed_events = 0;
     while (1)
     {
        //cout<<"before open 1"<<endl;
        //sleep(1);

        for (int mwpc_iter = 1; mwpc_iter < 3; mwpc_iter++)
        {
            map<int,int>::iterator it_planes;
            if (mwpc_iter == 1)
                it_planes = device_serial1.begin();
            else
                it_planes = device_serial2.begin();

            // cycle for six plane (six device identificator)
            for (int i = 1;  i <= 6; i++, it_planes++)
            {
                raw_file = file_name_begin;
                raw_file += int_to_hex_string(it_planes->first);
                raw_file += ".dat";
                pRawFile = fopen(raw_file, "rb");
                if (pRawFile == NULL)
                {
                    //perror("Error opening file: ");
                    fprintf(stderr,"Error opening file (%s): %s\n", strerror(errno), raw_file.Data());
                    return 0;
                }

                // obtain file size:
                fseek(pRawFile, 0, SEEK_END);
                lSize = ftell(pRawFile);

                curStart = lStart[(mwpc_iter-1)*6 + (i-1)];

                //cout<<"read index: "<<i<<"... "<<lStart<<" : "<<lSize<<endl;
                //sleep(2);

                if (lSize <= curStart)
                {
                    fclose(pRawFile);
                    continue;
                }

                //cout<<"reading file tail..."<<endl;

                fseek(pRawFile, curStart, SEEK_SET);

                // allocate memory to contain new tail of file:
                buffer =  new unsigned int[(lSize-curStart)/4];
                if (buffer == NULL)
                {
                    fputs("Memory error", stderr);
                    fclose(pRawFile);

                    return 0;
                }

                // copy the file into the buffer:
                size = fread(buffer, 4, (lSize-curStart)/4, pRawFile);
                if (size != (lSize-curStart)/4)
                {
                    fputs("Reading error", stderr);
                    free(buffer);
                    fclose(pRawFile);

                    return 0;
                }

                // the whole file is now loaded in the memory 'buffer'
                fclose(pRawFile);

                //parse the data
                int result_error = ParseHRB2Buffer(buffer, (lSize-curStart)/4, fEventReadData, fEventDrawData, thread_par->semEventData);

                free(buffer);

                lStart[(mwpc_iter-1)*6 + (i-1)] = lSize;
            }// for cycle for six plane (six device identificator)
        }// for cycle for MWPC detectors

        // copy complete event to another draw vector
        //cout<<"Event count: "<<fEventReadData->size()<<" processed: "<<processed_events<<endl;
        //sleep(2);
        for (int i = processed_events; i < fEventReadData->size(); i++)
        {
            processed_events = fEventReadData->size();
            EventData* pCurEvent = (*fEventReadData)[i];
            EventData* pDrawEvent = new EventData(*pCurEvent);
            fEventDrawData->push_back(pDrawEvent);
            thread_par->semEventData->Post();
        }

        //return 0;
     }// while (1)

     return 0;
}

void GeoDraw(vector<TVector3*>* pPointVector, OnlineEventManager* fEventManager, TEvePointSet** fq)
{
    if (*fq)
    {
        (*fq)->Reset();
        gEve->RemoveElement(*fq, fEventManager->EveRecoPoints);
    }

    Color_t fColor = kRed;
    Style_t fStyle = kFullDotMedium;

    Int_t npoints = pPointVector->size();
    TEvePointSet* q = new TEvePointSet("MWPC points", npoints, TEvePointSelectorConsumer::kTVT_XYZ);

    q->SetOwnIds(kTRUE);
    q->SetMarkerColor(fColor);
    q->SetMarkerSize(1);
    q->SetMarkerStyle(fStyle);

    //q->SetNextPoint(0, 0, 0);

    for (Int_t i = 0; i < npoints; i++)
    {
      TVector3* vec = (*pPointVector)[i];
      cout<<"Point "<<i<<": x="<<vec->X()<<" y="<<vec->Y()<<" z="<<vec->Z()<<endl;
      q->SetNextPoint(vec->X(), vec->Y(), vec->Z());
      q->SetPointId(new TNamed(Form("Point %d", i), ""));
      // bug in ROOT with one point
      if (npoints == 1)
      {
          TVector3* vecAdd = new TVector3(vec->X(), vec->Y(), vec->Z());
          cout<<"Point "<<i<<": x="<<vecAdd->X()<<" y="<<vecAdd->Y()<<" z="<<vecAdd->Z()<<endl;
          q->SetNextPoint(vecAdd->X(), vecAdd->Y(), vecAdd->Z());
          q->SetPointId(new TNamed(Form("Point %d", i+1), ""));
      }
    }

    if (fEventManager->EveRecoPoints == NULL)
    {
        fEventManager->EveRecoPoints = new TEveElementList("MWPC points");
        gEve->AddElement(fEventManager->EveRecoPoints, fEventManager);
        fEventManager->EveRecoPoints->SetRnrState(kTRUE);
    }

    gEve->AddElement(q, fEventManager->EveRecoPoints);

    gEve->Redraw3D(kFALSE);

    *fq = q;

    if (npoints == 1)
        sleep(90);

    return;
}

void* DrawEvent(void* ptr)
{
    ThreadParam_Draw* thread_par = (ThreadParam_Draw*) ptr;
    vector<EventData*>* fEventDrawData = thread_par->fEventDrawData;
    OnlineEventManager* fEventManager = thread_par->fEventManager;
    TSemaphore* semEventData = thread_par->semEventData;

    int i = 0;
    TEvePointSet* fq = NULL;
    while (1)
    {
        semEventData->Wait();

        if (i >= fEventDrawData->size())
        {
            cout<<"Programming Error: index >= size : "<<i<<" >= "<<fEventDrawData->size()<<endl;
            continue;
        }

        //cout<<i<<" "<<fEventData->size()<<endl;

        EventData* curEvent = (*fEventDrawData)[i];
        //if (curEvent->isEventDataFinished)
        //{
            cout<<"Event processing: "<<i<<endl;

            bool isEmptyPlane = false;
            for (int j = 0; j< 6; j++)
            {
                if (curEvent->MWPC1Planes[j].size() == 0)
                    isEmptyPlane = true;
            }

            if (isEmptyPlane)
                cout<<"WARNING!!! digits count 0"<<endl;

            cout<<"Search hits: "<<curEvent->MWPC1Planes[0].size()<<" : "<<curEvent->MWPC1Planes[1].size()<<" : "<<curEvent->MWPC1Planes[2].size()<<" : "
               <<curEvent->MWPC1Planes[3].size()<<" : "<<curEvent->MWPC1Planes[4].size()<<" : "<<curEvent->MWPC1Planes[5].size()<<endl;

            /*for (int z=0; z < curEvent->digitsPlane1.size(); z++)
                curEvent->digitsPlane1[z]->Dump();
            for (int z=0; z < curEvent->digitsPlane2.size(); z++)
                curEvent->digitsPlane2[z]->Dump();
            for (int z=0; z < curEvent->digitsPlane3.size(); z++)
                curEvent->digitsPlane3[z]->Dump();
            for (int z=0; z < curEvent->digitsPlane4.size(); z++)
                curEvent->digitsPlane4[z]->Dump();
            for (int z=0; z < curEvent->digitsPlane5.size(); z++)
                curEvent->digitsPlane5[z]->Dump();
            for (int z=0; z < curEvent->digitsPlane6.size(); z++)
                curEvent->digitsPlane6[z]->Dump();
            */

            vector<TVector3*> event_hits = SearchHits(curEvent->MWPC1Planes[0], curEvent->MWPC1Planes[1], curEvent->MWPC1Planes[2],
                                                      curEvent->MWPC1Planes[3], curEvent->MWPC1Planes[4], curEvent->MWPC1Planes[5]);

            cout<<"Point vector size: "<<event_hits.size()<<endl;

            GeoDraw(&event_hits, fEventManager, &fq);

            cout<<"Event hits were drawn for event "<<i<<endl;
            sleep(3);
            i++;
        //}
    }
}

//______________________________________________________________________________
// OnlineEventManagerEditor
//
// Specialization of TGedEditor for proper update propagation to
// TEveManager.

ClassImp(OnlineEventManagerEditor)

//______________________________________________________________________________
OnlineEventManagerEditor::OnlineEventManagerEditor(const TGWindow* p, Int_t width, Int_t height,
    UInt_t options, Pixel_t back)
  :TGedFrame(p, width, height, options | kVerticalFrame, back),
   fObject(0),
   fManager(OnlineEventManager::Instance()),
   fCurrentEvent(0),
   fCurrentPDG(0),
   fVizPri(0),
   fMinEnergy(0),
   fMaxEnergy(0),
   iCurrentEvent(-1)
{
  Init();

  fEventReadData = new vector<EventData*>();
  fEventDrawData = new vector<EventData*>();
}

void OnlineEventManagerEditor::Init()
{
  // get input file
  FairRootManager* fRootManager=FairRootManager::Instance();
  TChain* chain =fRootManager->GetInChain();
  Int_t Entries= chain->GetEntriesFast();

  // create tab for event visualization
  MakeTitle("OnlineEventManager  Editor");
  TGVerticalFrame*      fInfoFrame= CreateEditorTabSubFrame("Event Info");
  title1 = new TGCompositeFrame(fInfoFrame, 250, 10,
      kVerticalFrame | kLHintsExpandX |
      kFixedWidth    | kOwnBackground);

  // display file name
  TString Infile= "file : ";
//  TFile* file =FairRunAna::Instance()->GetInputFile();
  TFile* file =FairRootManager::Instance()->GetInChain()->GetFile();
  Infile+=file->GetName();
  TGLabel* TFName=new TGLabel(title1, Infile.Data());
  title1->AddFrame(TFName);

  // display Run ID
  UInt_t RunId= FairRunAna::Instance()->getRunId();
  TString run= "Run Id : ";
  run += RunId;
  TGLabel* TRunId=new TGLabel(title1, run.Data());
  title1->AddFrame(TRunId);

  // display event count
  TString nevent= "No of events : ";
  nevent +=Entries ;
  TGLabel* TEvent=new TGLabel(title1, nevent.Data());
  title1->AddFrame(TEvent);

  // count of geometry nodes
  Int_t nodes= gGeoManager->GetNNodes();
  TString NNodes= "No. of Nodes : ";
  NNodes += nodes;
  TGLabel* NoNode=new TGLabel(title1, NNodes.Data());
  title1->AddFrame(NoNode);

  // setting textbox for event number
  TGHorizontalFrame* f = new TGHorizontalFrame(title1);
  TGLabel* l = new TGLabel(f, "Current Event:");
  f->AddFrame(l, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 1, 2, 1, 1));
  fCurrentEvent = new TGNumberEntry(f, 0., 6, -1,
                                    TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative,
                                    TGNumberFormat::kNELLimitMinMax, 0, Entries-1);
  f->AddFrame(fCurrentEvent, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
  fCurrentEvent->Connect("ValueSet(Long_t)","OnlineEventManagerEditor", this, "SelectEvent()");
  title1->AddFrame(f);

  // textbox for time cutting
  TGHorizontalFrame* f2 = new TGHorizontalFrame(title1);
  TGLabel* EventTimeLabel = new TGLabel(f2, "Event Time: ");
  fEventTime = new TGLabel(f2,"");
  f2->AddFrame(EventTimeLabel);
  f2->AddFrame(fEventTime);
  title1->AddFrame(f2);

  // checkbox to display only primary particles in event
  fVizPri = new TGCheckButton(title1, "Primary Only");
  AddFrame(fVizPri, new TGLayoutHints(kLHintsTop, 3, 1, 1, 0));
  fVizPri->Connect("Toggled(Bool_t)", "OnlineEventManagerEditor", this, "DoVizPri()");
  title1->AddFrame(fVizPri);

  // textbox to display only particles with given PDG
  TGHorizontalFrame* f1 = new TGHorizontalFrame(title1);
  TGLabel* L1 = new TGLabel(f1, "Select PDG :");
  f1->AddFrame(L1, new TGLayoutHints(kLHintsLeft|kLHintsCenterY, 1, 2, 1, 1));
  fCurrentPDG = new TGNumberEntry(f1, 0., 12, -1,
                                  TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber,
                                  TGNumberFormat::kNELNoLimits, 0, 1);
  f1->AddFrame(fCurrentPDG, new TGLayoutHints(kLHintsLeft, 1, 1, 1, 1));
  fCurrentPDG->Connect("ValueSet(Long_t)","OnlineEventManagerEditor", this, "SelectPDG()");
  title1->AddFrame(f1);


  // textbox for min energy cutting
  fMinEnergy = new TEveGValuator(title1, "Min Energy:", 90, 0);
  fMinEnergy->SetNELength(5);
  fMinEnergy->SetLabelWidth(80);
  fMinEnergy->Build();
  fMinEnergy->SetLimits(0, MAXE, 2501, TGNumberFormat::kNESRealOne);
  fMinEnergy->SetToolTip("Minimum energy of displayed tracks");
  fMinEnergy->SetValue(0);
  fMinEnergy->Connect("ValueSet(Double_t)", "OnlineEventManagerEditor", this, "MinEnergy()");
  title1->AddFrame(fMinEnergy, new TGLayoutHints(kLHintsTop, 1, 1, 1, 0));
  fManager->SetMinEnergy(0);

  // textbox for max energy cutting
  fMaxEnergy = new TEveGValuator(title1, "Max Energy:", 90, 0);
  fMaxEnergy->SetNELength(5);
  fMaxEnergy->SetLabelWidth(80);
  fMaxEnergy->Build();
  fMaxEnergy->SetLimits(0, MAXE, 2501, TGNumberFormat::kNESRealOne);
  fMaxEnergy->SetToolTip("Maximum energy of displayed tracks");
  fMaxEnergy->SetValue(MAXE);
  fMaxEnergy->Connect("ValueSet(Double_t)", "OnlineEventManagerEditor", this, "MaxEnergy()");
  title1->AddFrame(fMaxEnergy, new TGLayoutHints(kLHintsTop, 1, 1, 1, 0));
  fManager->SetMaxEnergy(MAXE);

  // button: whether show detector geometry or not
  TGCheckButton* fGeometry = new TGCheckButton(title1, "show geometry");
  title1->AddFrame(fGeometry, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5,5,1,1));
  fGeometry->Connect("Toggled(Bool_t)", "OnlineEventManagerEditor", this, "ShowGeometry(Bool_t)");
  fGeometry->SetOn();

  // button for switching from black to white background
  TGCheckButton* fBackground = new TGCheckButton(title1, "light background");
  title1->AddFrame(fBackground, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5,5,1,1));
  fBackground->Connect("Toggled(Bool_t)", "OnlineEventManagerEditor", this, "SwitchBackground(Bool_t)");

  // group for displaying simulation and reconstruction data
  groupData = new TGGroupFrame(title1, "Show MC and reco data");
  groupData->SetTitlePos(TGGroupFrame::kCenter);

  TGHorizontalFrame* framePointsInfo = new TGHorizontalFrame(groupData);
  // button for show|hide MC points
  fShowMCPoints = new TGCheckButton(framePointsInfo, "MC points");
  framePointsInfo->AddFrame(fShowMCPoints, new TGLayoutHints(kLHintsNormal, 0,0,0,0));
  fShowMCPoints->Connect("Toggled(Bool_t)", "OnlineEventManagerEditor", this, "ShowMCPoints(Bool_t)");

  // button for show|hide reconstructed points
  fShowRecoPoints = new TGCheckButton(framePointsInfo, "Reco points");
  framePointsInfo->AddFrame(fShowRecoPoints, new TGLayoutHints(kLHintsRight, 0,0,1,0));
  fShowRecoPoints->Connect("Toggled(Bool_t)", "OnlineEventManagerEditor", this, "ShowRecoPoints(Bool_t)");
  groupData->AddFrame(framePointsInfo, new TGLayoutHints(kLHintsNormal | kLHintsExpandX, 1,1,5,0));

  TGHorizontalFrame* frameTracksInfo = new TGHorizontalFrame(groupData);
  // button for show|hide MC tracks
  fShowMCTracks = new TGCheckButton(frameTracksInfo, "MC tracks");
  frameTracksInfo->AddFrame(fShowMCTracks, new TGLayoutHints(kLHintsNormal, 0,0,0,0));
  fShowMCTracks->Connect("Toggled(Bool_t)", "OnlineEventManagerEditor", this, "ShowMCTracks(Bool_t)");

  // button for show|hide reco tracks
  fShowRecoTracks = new TGCheckButton(frameTracksInfo, "Reco tracks");
  frameTracksInfo->AddFrame(fShowRecoTracks, new TGLayoutHints(kLHintsRight, 0,0,1,0));
  fShowRecoTracks->Connect("Toggled(Bool_t)", "OnlineEventManagerEditor", this, "ShowRecoTracks(Bool_t)");
  groupData->AddFrame(frameTracksInfo, new TGLayoutHints(kLHintsNormal | kLHintsExpandX, 1,1,5,0));

  title1->AddFrame(groupData, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 3,15,1,1));

  // button for update of event visualization
  TGTextButton* fUpdate = new TGTextButton(title1, "Update");
  title1->AddFrame(fUpdate, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 3,15,1,1));
  fUpdate->Connect("Clicked()", "OnlineEventManagerEditor", this, "SelectEvent()");

  // add all frame above to "event info" tab
  fInfoFrame->AddFrame(title1, new TGLayoutHints(kLHintsTop, 0, 0, 2, 0));
}

//______________________________________________________________________________
void OnlineEventManagerEditor::MaxEnergy()
{
    fManager->SetMaxEnergy(fMaxEnergy->GetValue());
}
//______________________________________________________________________________
void OnlineEventManagerEditor::MinEnergy()
{
    fManager->SetMinEnergy(fMinEnergy->GetValue());
}

//______________________________________________________________________________
void OnlineEventManagerEditor::DoVizPri()
{
    if (fVizPri->IsOn())
        fManager->SetPriOnly(kTRUE);
    else
        fManager->SetPriOnly(kFALSE);
}
//______________________________________________________________________________
void OnlineEventManagerEditor::SelectPDG()
{
    fManager->SelectPDG(fCurrentPDG->GetIntNumber());
}

//______________________________________________________________________________
void OnlineEventManagerEditor::SetModel(TObject* obj)
{
    fObject = obj;
}

//______________________________________________________________________________
void OnlineEventManagerEditor::SelectEvent()
{
  /*
  int iNewEvent = fCurrentEvent->GetIntNumber();
  // exec event visualization of selected event
  fManager->GotoEvent(iNewEvent);

  if (iCurrentEvent == -1)
  {
      if (fManager->EveMCPoints == NULL)
          fShowMCPoints->SetDisabledAndSelected(kFALSE);
      if (fManager->EveMCTracks == NULL)
          fShowMCTracks->SetDisabledAndSelected(kFALSE);
      if (fManager->EveRecoPoints == NULL)
          fShowRecoPoints->SetDisabledAndSelected(kFALSE);
      if (fManager->EveRecoTracks == NULL)
          fShowRecoTracks->SetDisabledAndSelected(kFALSE);
  }

  if (iCurrentEvent != iNewEvent)
  {
      iCurrentEvent = iNewEvent;

      // display event time
      TString time;
      time.Form("%.2f", FairRootManager::Instance()->GetEventTime());
      time += " ns";
      fEventTime->SetText(time.Data());

      // new min and max energy limits given by event energy range
      fMinEnergy->SetLimits(fManager->GetEvtMinEnergy(), fManager->GetEvtMaxEnergy(), 100);
      fMinEnergy->SetValue(fManager->GetEvtMinEnergy());
      MinEnergy();
      fMaxEnergy->SetLimits(fManager->GetEvtMinEnergy(), fManager->GetEvtMaxEnergy(), 100);
      fMaxEnergy->SetValue(fManager->GetEvtMaxEnergy());
      MaxEnergy();
  }
  */

    semEventData = new TSemaphore(0);

    // map of plane id: number plane - id in data file
    //device_serial = create_map();
    device_serial1 = create_map1();
    device_serial2 = create_map2();

    // run thread for MWPC data file parsing
    RunReadFileThread();

    // draw hit arrays if ready
    RunDrawThread();

  // update tab controls
  //Update();
  // update all scenes
  //fManager->fRPhiView->GetGLViewer()->UpdateScene(kTRUE);
  //fManager->fRhoZView->GetGLViewer()->UpdateScene(kTRUE);
  //fManager->fMulti3DView->GetGLViewer()->UpdateScene(kTRUE);
  //fManager->fMultiRPhiView->GetGLViewer()->UpdateScene(kTRUE);
  //fManager->fMultiRhoZView->GetGLViewer()->UpdateScene(kTRUE);
}

void OnlineEventManagerEditor::RunReadFileThread()
{
    ThreadParam_ReadFile* par_read_file = new ThreadParam_ReadFile();
    par_read_file->fEventReadData = fEventReadData;
    par_read_file->fEventDrawData = fEventDrawData;
    par_read_file->raw_file_name_begin = fManager->file_name_begin;
    par_read_file->semEventData = semEventData;

    TThread* thread_read_file = new TThread(ReadMWPCFiles, (void*)par_read_file);
    thread_read_file->Run();
}

void OnlineEventManagerEditor::RunDrawThread()
{
    ThreadParam_Draw* par_draw = new ThreadParam_Draw();
    par_draw->fEventDrawData = fEventDrawData;
    par_draw->fEventManager = fManager;
    par_draw->semEventData = semEventData;

    TThread* thread_draw = new TThread(DrawEvent, (void*)par_draw);
    thread_draw->Run();
}

//______________________________________________________________________________
void OnlineEventManagerEditor::SwitchBackground(Bool_t is_on)
{
    gEve->GetViewers()->SwitchColorSet();
}

//______________________________________________________________________________
void OnlineEventManagerEditor::ShowGeometry(Bool_t is_show)
{
    gEve->GetGlobalScene()->SetRnrState(is_show);
    fManager->fRPhiGeomScene->SetRnrState(is_show);
    fManager->fRhoZGeomScene->SetRnrState(is_show);

    gEve->Redraw3D();
}

//______________________________________________________________________________
void OnlineEventManagerEditor::ShowMCPoints(Bool_t is_show)
{
    /*
    TEveElement::List_t matches;
    TPRegexp* regexp = new TPRegexp("(\\w+)Point\\b");
    Int_t numFound = fManager->FindChildren(matches, *regexp);
    if (numFound > 0)
    {
        for (TEveElement::List_i p = matches.begin(); p != matches.end(); ++p)
            (*p)->SetRnrState(is_show);
    }
    */

    TEveElement* points = fManager->FindChild("MC points");
    if (points == NULL)
    {
        cout<<"There is no information about MC points"<<endl;
        fShowMCPoints->SetOn(kFALSE);
        return;
    }

    points->SetRnrState(is_show);
    gEve->Redraw3D();
}

//______________________________________________________________________________
void OnlineEventManagerEditor::ShowMCTracks(Bool_t is_show)
{
    TEveElement* tracks = fManager->FindChild("MC tracks");
    if (tracks == NULL)
    {
        cout<<"There is no information about MC tracks"<<endl;
        fShowMCTracks->SetOn(kFALSE);
        return;
    }

    tracks->SetRnrState(is_show);
    gEve->Redraw3D();
}

//______________________________________________________________________________
void OnlineEventManagerEditor::ShowRecoPoints(Bool_t is_show)
{
    TEveElement* points = fManager->FindChild("Reco points");
    if (points == NULL)
    {
        cout<<"There is no information about reconstructed points"<<endl;
        fShowRecoPoints->SetOn(kFALSE);
        return;
    }

    points->SetRnrState(is_show);
    gEve->Redraw3D();
}

//______________________________________________________________________________
void OnlineEventManagerEditor::ShowRecoTracks(Bool_t is_show)
{
    TEveElement* tracks = fManager->FindChild("Reco tracks");
    if (tracks == NULL)
    {
        cout<<"There is no information about reconstructed tracks"<<endl;
        fShowRecoTracks->SetOn(kFALSE);
        return;
    }

    tracks->SetRnrState(is_show);
    gEve->Redraw3D();
}
