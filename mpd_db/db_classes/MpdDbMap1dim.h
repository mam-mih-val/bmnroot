// ----------------------------------------------------------------------
//                    MpdDbMap1dim header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/MpdDbMap1dim.h 
 ** Class for the table: map_1dim 
 **/ 

#ifndef MPDDBMAP1DIM_H 
#define MPDDBMAP1DIM_H 1 

#include "TString.h"
#include "TDatime.h"

#include "MpdDbConnection.h"

class MpdDbMap1dim
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	MpdDbConnection* connectionUniDb;

	int i_map_id;
	int i_map_row;
	TString str_serial_hex;
	int i_plane;
	int i_map_group;
	int i_slot;
	int i_channel_low;
	int i_channel_high;

	//Constructor
	MpdDbMap1dim(MpdDbConnection* connUniDb, int map_id, int map_row, TString serial_hex, int plane, int map_group, int slot, int channel_low, int channel_high);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~MpdDbMap1dim(); // Destructor

	// static class functions
	static MpdDbMap1dim* CreateMap1dim(int map_id, int map_row, TString serial_hex, int plane, int map_group, int slot, int channel_low, int channel_high);
	static MpdDbMap1dim* GetMap1dim(int map_id, int map_row);
	static int DeleteMap1dim(int map_id, int map_row);
	static int PrintAll();

	// Getters
	int GetMapId() {return i_map_id;}
	int GetMapRow() {return i_map_row;}
	TString GetSerialHex() {return str_serial_hex;}
	int GetPlane() {return i_plane;}
	int GetMapGroup() {return i_map_group;}
	int GetSlot() {return i_slot;}
	int GetChannelLow() {return i_channel_low;}
	int GetChannelHigh() {return i_channel_high;}

	// Setters
	int SetMapId(int map_id);
	int SetMapRow(int map_row);
	int SetSerialHex(TString serial_hex);
	int SetPlane(int plane);
	int SetMapGroup(int map_group);
	int SetSlot(int slot);
	int SetChannelLow(int channel_low);
	int SetChannelHigh(int channel_high);
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(MpdDbMap1dim,1);
};

#endif
