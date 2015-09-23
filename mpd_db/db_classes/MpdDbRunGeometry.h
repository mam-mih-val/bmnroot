// ----------------------------------------------------------------------
//                    MpdDbRunGeometry header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/MpdDbRunGeometry.h 
 ** Class for the table: run_geometry 
 **/ 

#ifndef MPDDBRUNGEOMETRY_H 
#define MPDDBRUNGEOMETRY_H 1 

#include "TString.h"
#include "TDatime.h"

#include "MpdDbConnection.h"

class MpdDbRunGeometry
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	MpdDbConnection* connectionUniDb;

	int i_geometry_id;
	unsigned char* blob_root_geometry;
	Long_t sz_root_geometry;

	//Constructor
	MpdDbRunGeometry(MpdDbConnection* connUniDb, int geometry_id, unsigned char* root_geometry, Long_t size_root_geometry);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~MpdDbRunGeometry(); // Destructor

	// static class functions
	static MpdDbRunGeometry* CreateRunGeometry(unsigned char* root_geometry, Long_t size_root_geometry);
	static MpdDbRunGeometry* GetRunGeometry(int geometry_id);
	static int DeleteRunGeometry(int geometry_id);
	static int PrintAll();

	// Getters
	int GetGeometryId() {return i_geometry_id;}
	unsigned char* GetRootGeometry() {unsigned char* tmp_root_geometry = new unsigned char[sz_root_geometry]; memcpy(tmp_root_geometry, blob_root_geometry, sz_root_geometry); return tmp_root_geometry;}
	Long_t GetRootGeometrySize() {return sz_root_geometry;}

	// Setters
	int SetGeometryId(int geometry_id);
	int SetRootGeometry(unsigned char* root_geometry, Long_t size_root_geometry);
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(MpdDbRunGeometry,1);
};

#endif
