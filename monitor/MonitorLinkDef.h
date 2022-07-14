// $Id: MonitorLinkDef.h,v
#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ class BmnDataReceiver++;
#pragma link C++ class BmnMonitor++;
#pragma link C++ class BmnOnlineDecoder++;
#pragma link C++ class BmnOnlineReco++;
#pragma link C++ class BmnParts+;
#pragma link C++ class BmnMQSource+;
#pragma link C++ class BmnMQSink+;
//#pragma link C++ class BmnMQRunDevice+;
//#pragma link C++ class BmnMQFileSaver+;
//#pragma link C++ class BmnMQRecoTaskProcessor++;
#pragma link C++ class BmnHistGem++;
#pragma link C++ class BmnHistCsc++;
#pragma link C++ class BmnHistSilicon++;
#pragma link C++ class BmnHistDch++;
#pragma link C++ class BmnHistMwpc++;
#pragma link C++ class BmnHistZDC++;
#pragma link C++ class BmnHistScWall++;
#pragma link C++ class BmnHistECAL++;
#pragma link C++ class BmnHistToF++;
#pragma link C++ class BmnHistToF700++;
#pragma link C++ class BmnHistTrigger++;
#pragma link C++ class BmnHistSrc++;
#pragma link C++ class BmnHistLAND++;
#pragma link C++ class BmnHistTofCal++;

#pragma link C++ class vector<TH1F>+;
#pragma link C++ class vector<vector<TH1F> >+;
#pragma link C++ class vector<vector<vector<TH1F> > >+;
#pragma link C++ class vector<TH1F*>+;
#pragma link C++ class vector<vector<TH1F*> >+;
#pragma link C++ class vector<vector<vector<TH1F*> > >+;
#pragma link C++ class vector<int*>+;
#pragma link C++ class vector<vector<int*> >+;
#pragma link C++ class vector<vector<vector<int*> > >+;
#pragma link C++ class vector<BmnHist*>+;
#pragma link C++ class vector<BmnTrigWaveDigit*>+;

#endif



