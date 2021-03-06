/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// $Id: GenLinkDef.h,v 1.2 2006/09/15 16:53:34 friese Exp $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class MpdMCEventHeader+;
#pragma link C++ class MpdLAQGSMGenerator+;
#pragma link C++ class MpdHypYPtGenerator+;
#pragma link C++ class MpdRainGenerator+;
#pragma link C++ class MpdPHSDGenerator+;
#pragma link C++ class Mpd3fdGenerator+;
#pragma link C++ class MpdUrqmdGenerator+;
#pragma link C++ class MpdVHLLEGenerator+;
#pragma link C++ class MpdLibZ+;
#pragma link C++ class MpdGetNumEvents+;
#pragma link C++ class MpdHistoGenerator+;
#pragma link C++ class MpdDCMSMMGenerator+;
#pragma link C++ class MpdUnigenGenerator+;

#pragma link C++ class map<TString, FairIon*>;

#endif
