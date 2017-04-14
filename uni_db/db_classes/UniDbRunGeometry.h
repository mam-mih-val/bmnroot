// ----------------------------------------------------------------------
//                    UniDbRunGeometry header file 
//                      Generated 05-11-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbRunGeometry.h 
 ** Class for the table: run_geometry 
 **/ 

#ifndef UNIDBRUNGEOMETRY_H 
#define UNIDBRUNGEOMETRY_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class UniDbRunGeometry
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// geometry id
	int i_geometry_id;
	/// root geometry
	unsigned char* blob_root_geometry;
	/// size of root geometry
	Long_t sz_root_geometry;

	//Constructor
	UniDbRunGeometry(UniDbConnection* connUniDb, int geometry_id, unsigned char* root_geometry, Long_t size_root_geometry);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbRunGeometry(); // Destructor

	// static class functions
	/// add new run geometry to the database
	static UniDbRunGeometry* CreateRunGeometry(unsigned char* root_geometry, Long_t size_root_geometry);
	/// get run geometry from the database
	static UniDbRunGeometry* GetRunGeometry(int geometry_id);
	/// check run geometry exists in the database
	static bool CheckRunGeometryExists(int geometry_id);
	/// delete run geometry from the database
	static int DeleteRunGeometry(int geometry_id);
	/// print all run geometrys
	static int PrintAll();

	// Getters
	/// get geometry id of the current run geometry
	int GetGeometryId() {return i_geometry_id;}
	/// get root geometry of the current run geometry
	unsigned char* GetRootGeometry() {unsigned char* tmp_root_geometry = new unsigned char[sz_root_geometry]; memcpy(tmp_root_geometry, blob_root_geometry, sz_root_geometry); return tmp_root_geometry;}
	/// get size of root geometry of the current run geometry
	Long_t GetRootGeometrySize() {return sz_root_geometry;}

	// Setters
	/// set root geometry of the current run geometry
	int SetRootGeometry(unsigned char* root_geometry, Long_t size_root_geometry);
	/// print information about current run geometry
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(UniDbRunGeometry,1);
};

#endif
