// ----------------------------------------------------------------------
//                    ElogDbAttachment header file 
//                      Generated 27-11-2017 
// ----------------------------------------------------------------------

/** ElogDbAttachment.h
 ** Class for the table: attachment_ 
 **/ 

#ifndef ELOGDBATTACHMENT_H 
#define ELOGDBATTACHMENT_H 1 

#include "TString.h"
#include "TDatime.h"

#include "ElogConnection.h"

class ElogDbAttachment
{
 private:
	/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	/// connection to the database
    ElogConnection* connectionDb;

	/// record id
	int i_record_id;
	/// attachment number
	int i_attachment_number;
	/// file path
	TString str_file_path;

	//Constructor
    ElogDbAttachment(ElogConnection* connect_db, int record_id, int attachment_number, TString file_path);
	/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 public:
	/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */
	virtual ~ElogDbAttachment(); // Destructor

	// static class functions
	/// add new attachment to the database
	static ElogDbAttachment* CreateAttachment(int record_id, int attachment_number, TString file_path);
	/// get attachment from the database
	static ElogDbAttachment* GetAttachment(int record_id, int attachment_number);
	/// get attachment from the database
	static ElogDbAttachment* GetAttachment(TString file_path);
    /// check attachment exists in the database: 1 - true, 0 - false, <0 - database operation errors
    static int CheckAttachmentExists(int record_id, int attachment_number);
    /// check attachment exists in the database: 1 - true, 0 - false, <0 - database operation errors
    static int CheckAttachmentExists(TString file_path);
	/// delete attachment from the database
	static int DeleteAttachment(int record_id, int attachment_number);
	/// delete attachment from the database
	static int DeleteAttachment(TString file_path);
	/// print all attachments
	static int PrintAll();

	// Getters
	/// get record id of the current attachment
	int GetRecordId() {return i_record_id;}
	/// get attachment number of the current attachment
	int GetAttachmentNumber() {return i_attachment_number;}
	/// get file path of the current attachment
	TString GetFilePath() {return str_file_path;}

	// Setters
	/// set record id of the current attachment
	int SetRecordId(int record_id);
	/// set attachment number of the current attachment
	int SetAttachmentNumber(int attachment_number);
	/// set file path of the current attachment
	int SetFilePath(TString file_path);

	/// print information about current attachment
	void Print();
	/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */

 ClassDef(ElogDbAttachment,1);
};

#endif
