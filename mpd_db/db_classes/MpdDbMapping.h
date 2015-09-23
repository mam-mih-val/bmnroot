// ----------------------------------------------------------------------
//                    MpdDbMapping header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/MpdDbMapping.h 
 ** Class for the table: mapping_ 
 **/ 

#ifndef MPDDBMAPPING_H 
#define MPDDBMAPPING_H 1 

#include "TString.h"
#include "TDatime.h"

#include "MpdDbConnection.h"

class MpdDbMapping
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	MpdDbConnection* connectionUniDb;

	int i_map_id;
	int i_map_type;

	//Constructor
	MpdDbMapping(MpdDbConnection* connUniDb, int map_id, int map_type);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~MpdDbMapping(); // Destructor

	// static class functions
	static MpdDbMapping* CreateMapping(int map_type);
	static MpdDbMapping* GetMapping(int map_id);
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

 ClassDef(MpdDbMapping,1);
};

#endif
