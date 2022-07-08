#include <stdio.h>
// Auxillary
#include <boost/exception/all.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <zmq.h>
// FairSoft
#include <TSystem.h>
#include <THttpServer.h>
#include <TBufferFile.h>
#include <TCanvas.h>
#include <TH2F.h>
// BmnRoot
#include "BmnDataReceiver.h"
#include "BmnProfRawTools.h"

namespace pt = boost::property_tree;

#define PAD_WIDTH  1920
#define PAD_HEIGHT 1080
const size_t ASIC_channel = 32; // for the beam profilometer
//	size_t ASIC_channel = 64; // for the beam tracker
const string DataHeader = "DAT";
const string DataTrailer = "SPILLEND";

uint32_t temp_cntr = 0;
const size_t SpillCount = 3;

vector<boost::circular_buffer<vector<float> > > cb;
vector<boost::circular_buffer<size_t> > cb_event_count;

vector<TH1*> hVec;
vector<vector<int>> adc1_word_char;
vector<vector<vector<int>>> adc1_ch;
vector<vector<float>> adc1_ch_mean; // mean by spill

vector<vector<bitset < ASIC_channel >> > adc1, adc2;
vector<bitset < ASIC_channel >> adc1_word, adc2_word;
//vector<uint32_t> trigger_pside, trigger_nside;
uint32_t spill_cntr = 0;
size_t NChars = 0;
const int NBins = 200;
const float thr = 50;
const int NCols = 4; // canvas columns

struct ProfiMap {
    char ChannelName;
    char LayerType;
    int StripMap[ASIC_channel];
};
vector<ProfiMap> channel_maps;

int ReadMap(string name) {
    try {
        pt::ptree conf;
        pt::read_json(name, conf);
        pt::ptree pads = conf.get_child("modules");
        for (auto v = pads.begin(); v != pads.end(); v++) {
            cout << (*v).second.get_optional<Int_t>("nAsicChannels") << endl;
            pt::ptree maps = (*v).second.get_child("channelMapping");
            for (auto m = maps.begin(); m != maps.end(); m++) {
                channel_maps.push_back(
                        ProfiMap{
                    .ChannelName = (*m).second.get<char>("adcChName"),
                    .LayerType = (*m).second.get<char>("layerType")
                }
                );
                pt::ptree strips = (*m).second.get_child("stripsMapping");
                for (auto stripNode = strips.begin(); stripNode != strips.end(); stripNode++) {
                    auto it = (*stripNode).second.begin();
                    //                    cout << it->second.get_value<int>() << " : " <<
                    //                            (++it)->second.get_value<int>()<< endl;
                    int stripId = (it)->second.get_value<int>();
                    int chanlId = (++it)->second.get_value<int>();
                    channel_maps.back().StripMap[chanlId] = stripId;
                }

            }
        }
    } catch (boost::exception &e) {
        cerr << boost::diagnostic_information(e);
        cout << "Unable to parse the channel map!\n" << endl;
        return -1;
    }
    return 0;
}

static vector<float> MeanVector(vector<vector<int>> &vec) {
    vector<float> mean_vec(ASIC_channel, 0);
    for (size_t i = 0; i < vec.size(); i++) {
        for (size_t j = 0; j < vec[i].size(); j++) {
            if (vec[i].size() != ASIC_channel) continue;
            mean_vec[j] += vec[i][j];
        }
    }
    for (size_t i = 0; i < mean_vec.size(); i++) {
        mean_vec[i] /= static_cast<int> (vec.size());
    }
    return mean_vec;
}

int ProcessBuffer(uint32_t *word, size_t len) {
    uint32_t holdb_temp = 0;
    uint32_t holdb = 0;
    string str(reinterpret_cast<char*> (word), 6);
    //    printf("str %s\n", str.c_str());
    for (uint32_t i = 0; i < len; i++) {
        uint32_t data = word[i];
        //                                                printf("data %08X  i = %05u\n",data, i);
        // Check is it data or a trigger:
        if (BmnProfRawTools::data_or_trig(data)) {
            holdb = BmnProfRawTools::holdb_cntr(data); //get holdb counter
            if (holdb_temp == holdb) {
                //                printf("data  %08X\n", data);
                // Divide ADC0 and ADC1 data
                if (!BmnProfRawTools::adc_num(data)) { // 1st ADC
                    adc1_word.push_back(bitset<32>(data));
                } else if (BmnProfRawTools::adc_num(data)) { // 2nd ADC
                    adc2_word.push_back(bitset<32>(data));
                }
            } else {
                if (adc1_word.size() == 6 * ASIC_channel) adc1.push_back(adc1_word);
                if (adc2_word.size() == 6 * ASIC_channel) adc2.push_back(adc2_word);
                //                printf("adc1_word  %lu\n", adc1_word.size());
                //                printf("adc1  %lu\n", adc1.size());
                adc1_word.clear();
                adc2_word.clear();
                // Divide ADC0 and ADC1 data
                if (!BmnProfRawTools::adc_num(data)) { // 1st ADC
                    adc1_word.push_back(bitset<32>(data));
                } else { // 2nd ADC
                    adc2_word.push_back(bitset<32>(data));
                }
                holdb_temp = holdb;
            }
        } else {
            //            spill_cntr++;
            //            if (BmnProfRawTools::trig_psd(data)) {
            //                trigger_pside.push_back(data);
            //            } else if (BmnProfRawTools::trig_nsd(data)) {
            //                trigger_nside.push_back(data);
            //            }
        }
    }
    return 0;
}

int ReorderBits() {
    vector<bitset < 32 >> temp;
    //    printf("ReorderBits adc1 size %lu\n", adc1.size());
    for (size_t i = 0; i < adc1.size(); i++) {
        gSystem->ProcessEvents();
        //                printf("events processed in the reorder func\n");
        //        printf("ReorderBits adc1[%02lu].size() %lu\n", i, adc1.size());
        for (size_t j = 0; j < adc1[i].size(); j = j + 6) {
            for (size_t k = 0; k < 6; ++k) {
                temp.push_back(adc1[i][j + k]);
            }
            for (size_t iChar = 0; iChar < channel_maps.size(); iChar++)
                adc1_word_char[iChar].push_back(BmnProfRawTools::adc_ch(temp, channel_maps[iChar].ChannelName));
            temp.clear();
        }
        for (size_t iChar = 0; iChar < channel_maps.size(); iChar++)
            adc1_ch[iChar].push_back(move(adc1_word_char[iChar]));
    }
    return 0;
}

int CalcMean() {
    for (size_t iChar = 0; iChar < channel_maps.size(); iChar++) {
        adc1_ch_mean[iChar] = MeanVector(adc1_ch[iChar]);
    }
    return 0;
}

int FillSpill() {
    for (size_t i = 0; i < hVec.size(); i++) {
        hVec[i]->Reset();
    }
    for (size_t iChar = 0; iChar < channel_maps.size(); iChar++) {
        vector<float> cur(ASIC_channel, 0);
        for (size_t iEvent = 0; iEvent < adc1_ch[iChar].size(); iEvent++)
            for (size_t iCh = 0; iCh < ASIC_channel; iCh++) {
                float sig = adc1_ch[iChar][iEvent][iCh] - adc1_ch_mean[iChar][iCh];
                cur[iCh] += sig;
                hVec[NCols * iChar + 3]->Fill(iCh, adc1_ch[iChar][iEvent][iCh]);
                if (
                        ((sig > thr) && channel_maps[iChar].LayerType == 'p') ||
                        ((sig < -thr) && channel_maps[iChar].LayerType == 'n')
                        ) {
                    int strip = channel_maps[iChar].StripMap[iCh];
                    hVec[NCols * iChar]->Fill(strip);
                    hVec[NCols * iChar + 2]->Fill(sig);
                }
            }
        cb[iChar].push_front(move(cur));
        //                printf("adc1_ch[%02lu]size %lu\n", iChar, adc1_ch[iChar].size());
        cb_event_count[iChar].push_front(adc1_ch[iChar].size());
        size_t event_count = 0;
        for (size_t &ne : cb_event_count[iChar])
            event_count += ne;
        //        printf("events[%02lu] %lu\n", iChar, event_count);
        for (size_t iCh = 0; iCh < ASIC_channel; iCh++) {
            float sum = 0;
            for (size_t iSpill = 0; iSpill < cb[iChar].size(); iSpill++) {
                sum += cb[iChar][iSpill][iCh];
            }
            if (event_count) {
                sum /= (float) event_count;
            }
            //            printf("sum %5.2f\n", sum);
            int strip = channel_maps[iChar].StripMap[iCh];
            //            hVec[3* iChar]->SetBinContent(strip, sum);
            hVec[NCols * iChar + 1]->SetBinContent(strip, adc1_ch_mean[iChar][iCh]);
        }
    }
    for (size_t iEvent = 0; iEvent < adc1_ch[0].size(); iEvent++) {
        bitset<ASIC_channel> x = 0;
        bitset<ASIC_channel> y = 0;
        for (size_t iChar = 0; iChar < channel_maps.size(); iChar++) {
            for (size_t iCh = 0; iCh < ASIC_channel; iCh++) {
                float sig = adc1_ch[iChar][iEvent][iCh] - adc1_ch_mean[iChar][iCh];
                if (
                        ((sig > thr) && channel_maps[iChar].LayerType == 'p') ||
                        ((sig < -thr) && channel_maps[iChar].LayerType == 'n')
                        ) {
                    int strip = channel_maps[iChar].StripMap[iCh];
                    if (channel_maps[iChar].LayerType == 'p')
                        x[strip] = true;
                    else
                        y[strip] = true;
                }
            }
        }
        for (size_t iX = 0; iX < ASIC_channel; iX++)
            for (size_t iY = 0; iY < ASIC_channel; iY++)
                if (x[iX] && y[iY])
                    hVec[2 * NCols]->Fill(iX, iY);
    }
    return 0;
}

void ClearAllVectors() {
    adc1.clear();
    adc2.clear();
    adc1_word.clear();
    adc2_word.clear();
    for (size_t i = 0; i < NChars; i++) {
        adc1_word_char[i].clear();
        adc1_ch[i].clear();
        adc1_ch_mean[i].clear();
    }
}

int Draw(TCanvas *c, vector<TH1*> hv) {
    for (size_t i = 0; i < hv.size(); i++) {
        c->cd(i + 1);
        hv[i]->Draw("colz");
    }
    c->Update();
    c->Modified();
    return 0;
}

int main(int argc, char **argv) {
    printf("Hi!\n");
    string name = string(getenv("VMCWORKDIR")) + "/input/" + "Prof_map_run_8.json";
    ReadMap(name);
    // webserver init
    Int_t _webPort = 10000;
    string TargetBoardId = "board1";
    char* SourceAddr;
    if (argc > 1)
        SourceAddr = argv[1];
    if (argc > 2) {
        string str(argv[2]);
        _webPort = stoi(str);
    }
    if (argc > 3) {
        TargetBoardId = argv[3];
    }
    TString cgiStr = Form("http:%d;noglobal", _webPort);
    THttpServer* fServer = new THttpServer(cgiStr.Data());
    // hist declaration
    TCanvas *c = new TCanvas("Profilometer", "Profilometer", PAD_WIDTH, PAD_HEIGHT);
    fServer->Register("/", c);
    NChars = channel_maps.size();
    c->Divide(NCols, NChars + 1, 0.001, 0.001);
    for (auto & map : channel_maps) {
        TH1 *h = new TH1F(
                Form("h%c_strips", map.LayerType),
                Form("%c side", map.LayerType), ASIC_channel, 0, ASIC_channel);
        TH1 *hMean = new TH1F(
                Form("h%c_strips_mean", map.LayerType),
                Form("%c side mean", map.LayerType), ASIC_channel, 0, ASIC_channel);
        TH1 *hSig = new TH1F(
                Form("h%c_sig", map.LayerType),
                Form("%c side signals", map.LayerType), NBins, 0, 0);
        TH1 *hPed = new TH2F(
                Form("h%c_ped", map.LayerType),
                Form("%c side pedestals", map.LayerType), ASIC_channel, 0, ASIC_channel, NBins, -150, 150);
        hVec.push_back(h);
        hVec.push_back(hMean);
        hVec.push_back(hSig);
        hVec.push_back(hPed);

    }
    TH1 *h2d = new TH2F("h_2D", "Profile 2D",
            ASIC_channel, 0, ASIC_channel,
            ASIC_channel, 0, ASIC_channel);
    hVec.push_back(h2d);
    adc1_word_char.resize(NChars);
    adc1_ch.resize(NChars);
    adc1_ch_mean.resize(NChars);
    cb.resize(NChars, boost::circular_buffer<vector<float> >(SpillCount));
    cb_event_count.resize(NChars, boost::circular_buffer<size_t>(SpillCount));
    for (size_t iChar = 0; iChar < channel_maps.size(); iChar++) {
        //        printf("init cb[%02lu]size %lu\n", iChar, cb[iChar].size());
        //        printf("init cb_event_count[%02lu]size %lu\n", iChar, cb_event_count[iChar].size());
    }

    // data socket init
    void * _ctx = nullptr;
    void * _rawSocket = nullptr;
    _ctx = zmq_ctx_new();
    _rawSocket = zmq_socket(_ctx, ZMQ_SUB);
    if (_rawSocket == NULL) {
        DBGERR("zmq socket")
        return 0;
    }
    if (zmq_connect(_rawSocket, SourceAddr) != 0) {
        DBGERR("zmq connect")
        return -1;
    }
    //    if (zmq_connect(_rawSocket, "tcp://127.0.0.1:5602") != 0) {
    //        DBGERR("zmq connect")
    //        return -1;
    //    }
    if (zmq_setsockopt(_rawSocket, ZMQ_SUBSCRIBE, NULL, 0) == -1) {
        DBGERR("zmq subscribe")
        return 0;
    }
    printf("MonProf listens to %s\n", SourceAddr);

    //    // delete
    //    std::ifstream file;
    //    file.open(argv[3]);
    //    file.seekg(0, std::ios_base::end);
    //    size_t len = file.tellg();
    //
    //    file.seekg(0, std::ios_base::beg);
    //
    //    //  II. Read to one big vector
    //    void *data = malloc(len);
    //    file.read((char*) data, len);
    //    file.close();
    //    ProcessBuffer(static_cast<uint32_t*> (data), len / 4);
    //            ReorderBits();
    //            printf("ReorderBits\n");
    //            CalcMean();
    //            printf("CalcMean\n");
    //            FillSpill();
    //            Draw(c, hVec);
    //    free(data);
    Draw(c, hVec);


    TBufferFile t(TBuffer::kRead);
    t.SetReadMode();
    bool keepWorking = kTRUE;
    bool isReceiving = kTRUE;
    const Int_t MaxStrLen = 100;
    bool isIdFound = kFALSE;
    bool isHeaderFound = kFALSE;
    bool isTrailerFound = kFALSE;
    while (keepWorking) {
        Int_t recv_more = 0;
        isIdFound = kFALSE;

        do {
            gSystem->ProcessEvents();
            //            fServer->ProcessRequests();
            zmq_msg_t msg;
            zmq_msg_init(&msg);
            Int_t frame_size = zmq_msg_recv(&msg, _rawSocket, ZMQ_DONTWAIT); //  ZMQ_DONTWAIT
            //            printf("recv %d\n", frame_size);
            if (frame_size == -1) {
                //                printf("Receive error # %d #%s\n", errno, zmq_strerror(errno));
                switch (errno) {
                    case EAGAIN:
                        //                        printf("EAGAIN\n");
                        usleep(50000);
                        break;
                    case EINTR:
                        printf("EINTR\n");
                        isReceiving = kFALSE;
                        keepWorking = kFALSE;
                        printf("Exit!\n");
                        break;
                    case EFAULT:
                        printf("EFAULT\n");
                        zmq_close(_rawSocket);
                        isReceiving = kFALSE;
                        keepWorking = kFALSE;
                        break;
                    default:
                        break;
                }
            } else {
                if (frame_size < MaxStrLen) {
                    string str(static_cast<char*> (zmq_msg_data(&msg)), zmq_msg_size(&msg));
                    //                    printf("str %s\n", str.c_str());
                    if (isIdFound) {
                        if (str == DataTrailer) {
                            isTrailerFound = kTRUE;
                            //                            printf("trailer\n");
                        }
                        if (str == DataHeader) {
                            isHeaderFound = kTRUE;
                            //                            printf("header\n");
                        }
                    } else {
                        if (str == TargetBoardId) {
                            isIdFound = kTRUE;
                            //                            printf("id\n");
                        }
                    }
                } else {
                    if (isHeaderFound) {
                        ProcessBuffer(static_cast<uint32_t*> (zmq_msg_data(&msg)), zmq_msg_size(&msg) / 4);
                    }
                }
            }
            size_t opt_size = sizeof (recv_more);
            if (zmq_getsockopt(_rawSocket, ZMQ_RCVMORE, &recv_more, &opt_size) == -1) {
                printf("ZMQ socket options error #%s\n", zmq_strerror(errno));
                return -1;
            }
            //            printf("ZMQ rcvmore = %d\n", recv_more);
            zmq_msg_close(&msg);
        } while (recv_more && isReceiving && (!isTrailerFound));
        //        printf("FullReceive\n");
        if (isTrailerFound) {
            ReorderBits();
            printf("ReorderBits\n");
            CalcMean();
            printf("CalcMean\n");
            FillSpill();
            printf("FillSpill\n");
            Draw(c, hVec);
            printf("Draw\n\n");
            ClearAllVectors();
            isHeaderFound = kFALSE;
            isTrailerFound = kFALSE;
            //            return 0;
        }
    }
    for (auto h : hVec)
        delete h;
    delete c;
    delete fServer;

    zmq_close(_rawSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
}
