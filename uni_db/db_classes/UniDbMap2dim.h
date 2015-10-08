// ----------------------------------------------------------------------
//                    UniDbMap2dim header file 
//                      Generated 15-09-2015 
// ----------------------------------------------------------------------

/** db_classes/UniDbMap2dim.h 
 ** Class for the table: map_2dim 
 **/ 

#ifndef UNIDBMAP2DIM_H
#define UNIDBMAP2DIM_H 1

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class UniDbMap2dim
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
	/// channel
	int i_channel;
	/// f channel
	int i_f_channel;
	/// channel size
	int i_channel_size;
	/// x
	int i_x;
	/// y
	int i_y;
	/// is connected
	int i_is_connected;

	//Constructor
	UniDbMap2dim(UniDbConnection* connUniDb, int map_id, int map_row, TString serial_hex, int channel, int f_channel, int channel_size, int x, int y, int is_connected);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~UniDbMap2dim(); // Destructor

	// static class functions
	/// add new map 2dim to the database
	static UniDbMap2dim* CreateMap2dim(int map_id, int map_row, TString serial_hex, int channel, int f_channel, int channel_size, int x, int y, int is_connected);
	/// get map 2dim from the database
	static UniDbMap2dim* GetMap2dim(int map_id, int map_row);
	/// delete map 2dim from the database
	static int DeleteMap2dim(int map_id, int map_row);
	/// print all map 2dims
	static int PrintAll();

	// Getters
	/// get map id of the current map 2dim
	int GetMapId() {return i_map_id;}
	/// get map row of the current map 2dim
	int GetMapRow() {return i_map_row;}
	/// get serial hex of the current map 2dim
	TString GetSerialHex() {return str_serial_hex;}
	/// get channel of the current map 2dim
	int GetChannel() {return i_channel;}
	/// get f channel of the current map 2dim
	int GetFChannel() {return i_f_channel;}
	/// get channel size of the current map 2dim
	int GetChannelSize() {return i_channel_size;}
	/// get x of the current map 2dim
	int GetX() {return i_x;}
	/// get y of the current map 2dim
	int GetY() {return i_y;}
	/// get is connected of the current map 2dim
	int GetIsConnected() {return i_is_connected;}

	// Setters
	/// set map id of the current map 2dim
	int SetMapId(int map_id);
	/// set map row of the current map 2dim
	int SetMapRow(int map_row);
	/// set serial hex of the current map 2dim
	int SetSerialHex(TString serial_hex);
	/// set channel of the current map 2dim
	int SetChannel(int channel);
	/// set f channel of the current map 2dim
	int SetFChannel(int f_channel);
	/// set channel size of the current map 2dim
	int SetChannelSize(int channel_size);
	/// set x of the current map 2dim
	int SetX(int x);
	/// set y of the current map 2dim
	int SetY(int y);
	/// set is connected of the current map 2dim
	int SetIsConnected(int is_connected);
	/// print information about current map 2dim
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(UniDbMap2dim,1);
};

#endif
