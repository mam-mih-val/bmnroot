// ----------------------------------------------------------------------
//                    UniDbMap1dim header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbMap1dim.h 
 ** Class for the table: map_1dim 
 **/ 

#ifndef UNIDBMAP1DIM_H
#define UNIDBMAP1DIM_H 1

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class UniDbMap1dim
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// map id
	int i_map_id;
	/// map row
	int i_map_row;
	/// serial hex
	TString str_serial_hex;
	/// plane
	int i_plane;
	/// map group
	int i_map_group;
	/// slot
	int i_slot;
	/// channel low
	int i_channel_low;
	/// channel high
	int i_channel_high;

	//Constructor
	UniDbMap1dim(UniDbConnection* connUniDb, int map_id, int map_row, TString serial_hex, int plane, int map_group, int slot, int channel_low, int channel_high);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbMap1dim(); // Destructor

	// static class functions
	/// add new map 1dim to the database
	static UniDbMap1dim* CreateMap1dim(int map_id, int map_row, TString serial_hex, int plane, int map_group, int slot, int channel_low, int channel_high);
	/// get map 1dim from the database
	static UniDbMap1dim* GetMap1dim(int map_id, int map_row);
	/// delete map 1dim from the database
	static int DeleteMap1dim(int map_id, int map_row);
	/// print all map 1dims
	static int PrintAll();

	// Getters
	/// get map id of the current map 1dim
	int GetMapId() {return i_map_id;}
	/// get map row of the current map 1dim
	int GetMapRow() {return i_map_row;}
	/// get serial hex of the current map 1dim
	TString GetSerialHex() {return str_serial_hex;}
	/// get plane of the current map 1dim
	int GetPlane() {return i_plane;}
	/// get map group of the current map 1dim
	int GetMapGroup() {return i_map_group;}
	/// get slot of the current map 1dim
	int GetSlot() {return i_slot;}
	/// get channel low of the current map 1dim
	int GetChannelLow() {return i_channel_low;}
	/// get channel high of the current map 1dim
	int GetChannelHigh() {return i_channel_high;}

	// Setters
	/// set map id of the current map 1dim
	int SetMapId(int map_id);
	/// set map row of the current map 1dim
	int SetMapRow(int map_row);
	/// set serial hex of the current map 1dim
	int SetSerialHex(TString serial_hex);
	/// set plane of the current map 1dim
	int SetPlane(int plane);
	/// set map group of the current map 1dim
	int SetMapGroup(int map_group);
	/// set slot of the current map 1dim
	int SetSlot(int slot);
	/// set channel low of the current map 1dim
	int SetChannelLow(int channel_low);
	/// set channel high of the current map 1dim
	int SetChannelHigh(int channel_high);
	/// print information about current map 1dim
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(UniDbMap1dim,1);
};

#endif
