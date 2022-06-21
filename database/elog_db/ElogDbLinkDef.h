// $Id: ElogDbLinkDef.h,v
#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class ElogDbPerson+;
#pragma link C++ class ElogDbType+;
#pragma link C++ class ElogDbTrigger+;
#pragma link C++ class ElogDbBeam+;
#pragma link C++ class ElogDbTarget+;
#pragma link C++ class ElogDbRecord+;
#pragma link C++ class ElogDbAttachment+;

#pragma link C++ class ElogConnection+;
#pragma link C++ class ElogSearchCondition+;

// the database enums
#pragma link C++ enum ElogColumns;
#ifndef enumConditions_link
#define enumConditions_link 1
#pragma link C++ enum enumConditions;
#endif

#endif

