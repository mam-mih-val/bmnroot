/** Interface to the Event Catalogue (through REST API service) to select events and write new metadata
 *
 * @author  K. Gertsenberger
 * @version 0.1
 * @since   04.11.21
 *
 */
#ifndef EVENTCATALOGUE_H
#define EVENTCATALOGUE_H

#include "TObject.h"

#include <string>
using namespace std;

class EventCatalogue
{
  public:
    // function for adding new event metadata to the Event Catalogue from a JSON file
    // returns a number (>=0) of successfully added metadata records or error_code (<0) in case of failures
    static int WriteEventMetadata(string json_file_path);

  private:
    EventCatalogue(){}
    virtual ~EventCatalogue(){}

  ClassDef(EventCatalogue, 1)
};

#endif /* !EVENTCATALOGUE_H */
