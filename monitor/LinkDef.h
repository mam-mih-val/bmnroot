#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ class BmnDataReceiver++;
#pragma link C++ class BmnMonitor++;
#pragma link C++ class BmnOnlineDecoder++;
#pragma link C++ class BmnHist+;
#pragma link C++ class PadInfo<TH1F>++;
#pragma link C++ class PadInfo<TH1I>++;
//#pragma link C++ class PadInfo++;
#pragma link C++ class BmnHistGem++;
#pragma link C++ class BmnHistDch++;
#pragma link C++ class BmnHistMwpc++;
#pragma link C++ class BmnHistToF++;
#pragma link C++ class BmnHistToF700++;
#pragma link C++ class BmnHistTrigger++;

#pragma link C++ class vector<TH1F>+;
#pragma link C++ class vector<vector<TH1F> >+;
#pragma link C++ class vector<vector<vector<TH1F> > >+;
#pragma link C++ class vector<TH1F*>+;
#pragma link C++ class vector<vector<TH1F*> >+;
#pragma link C++ class vector<vector<vector<TH1F*> > >+;
#pragma link C++ class vector<int*>+;
#pragma link C++ class vector<vector<int*> >+;
#pragma link C++ class vector<vector<vector<int*> > >+;
#pragma link C++ class vector<TSocket*>+;

#endif



