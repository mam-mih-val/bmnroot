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
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// map id
	int i_map_id;
	/// map type
	int i_map_type;

	//Constructor
	UniDbMapping(UniDbConnection* connUniDb, int map_id, int map_type);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbMapping(); // Destructor

	// static class functions
	/// add new mapping to the database
	static UniDbMapping* CreateMapping(int map_type);
	/// get mapping from the database
	static UniDbMapping* GetMapping(int map_id);
	/// delete mapping from the database
	static int DeleteMapping(int map_id);
	/// print all mappings
	static int PrintAll();

	// Getters
	/// get map id of the current mapping
	int GetMapId() {return i_map_id;}
	/// get map type of the current mapping
	int GetMapType() {return i_map_type;}

	// Setters
	/// set map id of the current mapping
	int SetMapId(int map_id);
	/// set map type of the current mapping
	int SetMapType(int map_type);
	/// print information about current mapping
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(UniDbMapping,1);
};

#endif
