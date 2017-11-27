// ----------------------------------------------------------------------
//                    ElogDbAttachment header file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

/** elog_classes/ElogDbAttachment.h 
 ** Class for the table: attachment_ 
 **/ 

#ifndef ELOGDBATTACHMENT_H 
#define ELOGDBATTACHMENT_H 1 

#include "TString.h"
#include "TDatime.h"

#include "UniDbConnection.h"

class ElogDbAttachment
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
	UniDbConnection* connectionUniDb;

	/// record id
	int i_record_id;
	/// attachment number
	int i_attachment_number;
	/// file name
	TString str_file_name;
	/// file data
	unsigned char* blob_file_data;
	/// size of file data
	Long_t sz_file_data;

	//Constructor
	ElogDbAttachment(UniDbConnection* connUniDb, int record_id, int attachment_number, TString file_name, unsigned char* file_data, Long_t size_file_data);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~ElogDbAttachment(); // Destructor

	// static class functions
	/// add new attachment to the database
	static ElogDbAttachment* CreateAttachment(int record_id, int attachment_number, TString file_name, unsigned char* file_data, Long_t size_file_data);
	/// get attachment from the database
	static ElogDbAttachment* GetAttachment(int record_id, int attachment_number);
	/// check attachment exists in the database
	static bool CheckAttachmentExists(int record_id, int attachment_number);
	/// delete attachment from the database
	static int DeleteAttachment(int record_id, int attachment_number);
	/// print all attachments
	static int PrintAll();

	// Getters
	/// get record id of the current attachment
	int GetRecordId() {return i_record_id;}
	/// get attachment number of the current attachment
	int GetAttachmentNumber() {return i_attachment_number;}
	/// get file name of the current attachment
	TString GetFileName() {return str_file_name;}
	/// get file data of the current attachment
	unsigned char* GetFileData() {unsigned char* tmp_file_data = new unsigned char[sz_file_data]; memcpy(tmp_file_data, blob_file_data, sz_file_data); return tmp_file_data;}
	/// get size of file data of the current attachment
	Long_t GetFileDataSize() {return sz_file_data;}

	// Setters
	/// set record id of the current attachment
	int SetRecordId(int record_id);
	/// set attachment number of the current attachment
	int SetAttachmentNumber(int attachment_number);
	/// set file name of the current attachment
	int SetFileName(TString file_name);
	/// set file data of the current attachment
	int SetFileData(unsigned char* file_data, Long_t size_file_data);

	/// print information about current attachment
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(ElogDbAttachment,1);
};

#endif
