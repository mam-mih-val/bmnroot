// ----------------------------------------------------------------------
//                    UniDbMapping header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbMapping.h 
 ** Class for the table: mapping_ 
 **/ 

#ifndef UNIDBMAPPING_H
#define UNIDBMAPPING_H 1

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class UniDbMapping
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	UniDbConnection* connectionUniDb;

	int i_map_id;
	int i_map_type;

	//Constructor
	UniDbMapping(UniDbConnection* connUniDb, int map_id, int map_type);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbMapping(); // Destructor

	// static class functions
	static UniDbMapping* CreateMapping(int map_type);
	static UniDbMapping* GetMapping(int map_id);
	static int DeleteMapping(int map_id);
	static int PrintAll();

	// Getters
	int GetMapId() {return i_map_id;}
	int GetMapType() {return i_map_type;}

	// Setters
	int SetMapId(int map_id);
	int SetMapType(int map_type);
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(UniDbMapping,1);
};

#endif
