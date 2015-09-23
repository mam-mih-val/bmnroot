// ----------------------------------------------------------------------
//                    MpdDbMap2dim header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/MpdDbMap2dim.h 
 ** Class for the table: map_2dim 
 **/ 

#ifndef MPDDBMAP2DIM_H 
#define MPDDBMAP2DIM_H 1 

#include "TString.h"
#include "TDatime.h"

#include "MpdDbConnection.h"

class MpdDbMap2dim
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	MpdDbConnection* connectionUniDb;

	int i_map_id;
	int i_map_row;
	TString str_serial_hex;
	int i_channel;
	int i_f_channel;
	int i_channel_size;
	int i_x;
	int i_y;
	int i_is_connected;

	//Constructor
	MpdDbMap2dim(MpdDbConnection* connUniDb, int map_id, int map_row, TString serial_hex, int channel, int f_channel, int channel_size, int x, int y, int is_connected);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~MpdDbMap2dim(); // Destructor

	// static class functions
	static MpdDbMap2dim* CreateMap2dim(int map_id, int map_row, TString serial_hex, int channel, int f_channel, int channel_size, int x, int y, int is_connected);
	static MpdDbMap2dim* GetMap2dim(int map_id, int map_row);
	static int DeleteMap2dim(int map_id, int map_row);
	static int PrintAll();

	// Getters
	int GetMapId() {return i_map_id;}
	int GetMapRow() {return i_map_row;}
	TString GetSerialHex() {return str_serial_hex;}
	int GetChannel() {return i_channel;}
	int GetFChannel() {return i_f_channel;}
	int GetChannelSize() {return i_channel_size;}
	int GetX() {return i_x;}
	int GetY() {return i_y;}
	int GetIsConnected() {return i_is_connected;}

	// Setters
	int SetMapId(int map_id);
	int SetMapRow(int map_row);
	int SetSerialHex(TString serial_hex);
	int SetChannel(int channel);
	int SetFChannel(int f_channel);
	int SetChannelSize(int channel_size);
	int SetX(int x);
	int SetY(int y);
	int SetIsConnected(int is_connected);
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(MpdDbMap2dim,1);
};

#endif
