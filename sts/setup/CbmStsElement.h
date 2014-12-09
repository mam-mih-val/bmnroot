/** @file CbmStsElement.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 27.05.2013
 **/

#ifndef CBMSTSELEMENT_H
#define CBMSTSELEMENT_H 1


#include <vector>
#include "TNamed.h"
#include "CbmStsAddress.h"

class TGeoPhysicalNode;

using namespace std;



/** @class CbmStsElement
 ** @brief Class representing an element of the STS setup
 ** @author V.Friese <v.friese@gsi.de>
 ** @version 1.0
 **
 ** A CbmStsElement represents an element in the STS setup
 ** hierarchy (e.g., station, ladder, module,...). It has
 ** a unique address, a level (enum CbmStsElementLevel),
 ** a pointer to a TGeoPhysicalNode and an array of daughter
 ** elements. It is thus an alignable object.
 **/
class CbmStsElement : public TNamed
{

  public:

    /** Default constructor **/
    CbmStsElement();


    /** Standard constructor
     ** @param name     Element name
     ** @param title    Level name
     ** @param level    Element level
     ** @param node     Pointer to physical node in geometry
     **/
    CbmStsElement(const char* name, const char* title,
                  Int_t level, TGeoPhysicalNode* node = NULL);


    /** Destructor **/
    virtual ~CbmStsElement() { };


    /** Add a daughter element
     ** The daughter must be one element level higher than the mother.
     ** @param element  Pointer to element to be added as daughter.
     **/
    virtual void AddDaughter(CbmStsElement* element);


    /** Get unique address
     ** @return Element address
     **/
    UInt_t GetAddress() const { return fAddress; }


    /** Get a daughter element
     ** @param index  Index of daughter element
     **/
    CbmStsElement* GetDaughter(Int_t index) const;


    /** Get the index within the mother element
     ** @return Index of element in mother
     **/
    Int_t GetIndex() const {
      return CbmStsAddress::GetElementId(fAddress, fLevel);
    }


    /** Get the element level
     ** @return Element level (type enum StsElementLevel)
     **/
    StsElementLevel GetLevel() const { return fLevel; }


    /** Get number of daughter elements
     ** @return Number of daughters
     **/
    Int_t GetNofDaughters() const { return fDaughters.size(); }


    /** Initialise daughters from geometry **/
    void InitDaughters();


    /** Print **/
    virtual void Print(Option_t* opt = "") const;





  protected:

    UInt_t fAddress;                       ///< Unique address
    StsElementLevel fLevel;                ///< Level in hierarchy
    TGeoPhysicalNode* fNode;               ///< Pointer to geometry
    vector<CbmStsElement*> fDaughters;     ///< Array of daughters


    /** Set the element level from integer
     ** Protection against being out of range.
     ** @param  level  Element level
     **/
    void SetLevel(Int_t level);


    // --- Prevent usage of copy constructor and assignment operator
    CbmStsElement(const CbmStsElement&);
    CbmStsElement& operator=(const CbmStsElement&);


    ClassDef(CbmStsElement,1);

};

#endif /* CBMSTSELEMENT_H */
