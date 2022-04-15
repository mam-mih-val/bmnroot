/** @file BmnSsdElement.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 07.12.2018
 **/

#ifndef BMNSSDELEMENT_H
#define BMNSSDELEMENT_H 1


#include "TGeoPhysicalNode.h"
#include "TNamed.h"
#include "BmnSsdAddress.h"



/** @class BmnSsdElement
 ** @brief Class representing an element of the SSD setup
 ** @version 1.0
 **
 ** A BmnSsdElement represents an element in the SSD setup
 ** hierarchy (e.g., station, ladder, module,...). It has
 ** a unique address, a level (enum BmnSsdElementLevel),
 ** a pointer to a TGeoPhysicalNode and an array of daughter
 ** elements. It is thus an alignable object.
 **/
class BmnSsdElement : public TNamed
{

  public:

    /** Default constructor **/
    BmnSsdElement();


    /** Standard constructor
     ** @param address  Unique element address
     ** @param level    Element level
     ** @param node     Pointer to geometry node
     ** @param mother   Pointer to mother element
     **/
    BmnSsdElement(Int_t address, Int_t level,
                  TGeoPhysicalNode* node = nullptr,
                  BmnSsdElement* mother = nullptr);


    /** Destructor **/
    virtual ~BmnSsdElement() { };


    /** Construct the element name from the address (static)
     ** @param address Unique element address
     ** @param level   Element level (unit, ladder, etc.)
     **/
    static TString ConstructName(Int_t address, ESsdElementLevel level);


    /** Get unique address
     ** @return Element address
     **/
    Int_t GetAddress() const { return fAddress; }


    /** Get a daughter element
     ** @param index  Index of daughter element
     **/
    BmnSsdElement* GetDaughter(Int_t index) const;


    /** Get the index within the mother element
     ** @return Index of element in mother
     **/
    Int_t GetIndex() const {
      return BmnSsdAddress::GetElementId(fAddress, fLevel);
    }


    /** Get the element level
     ** @return Element level (type enum ESsdElementLevel)
     **/
    ESsdElementLevel GetLevel() const { return fLevel; }


    /** Get the mother element **/
    BmnSsdElement* GetMother() const { return fMother; }


    /** Get number of daughter elements
     ** @return Number of daughters
     **/
    Int_t GetNofDaughters() const { return fDaughters.size(); }


    /** Get number of elements at given level
     ** @param level  Element level (see enum ESsdElementLevel)
     ** @return Number of elements at given level with this
     **         element as ancestor
     */
    Int_t GetNofElements(Int_t level) const;


    TGeoPhysicalNode* GetPnode() const { return fNode; }


    /** Initialise daughters from geometry **/
    virtual void InitDaughters();


    /** Set the mother element
     ** @param Pointer to mother element
     **/
    void SetMother(BmnSsdElement* mother) { fMother = mother; }


    /** Print **/
    virtual void Print(Option_t* opt = "") const;


  protected:

    Int_t fAddress;                        ///< Unique element address
    ESsdElementLevel fLevel;               ///< Level in hierarchy
    TGeoPhysicalNode* fNode;               ///< Pointer to geometry
    std::vector<BmnSsdElement*> fDaughters;     ///< Array of daughters
    BmnSsdElement* fMother;                ///< Mother element


    /** Construct the name of the element **/
    void ConstructName();


    /** Set the element level from integer
     ** Protection against being out of range.
     ** @param  level  Element level
     **/
    void SetLevel(Int_t level);


private:

    // --- Disable copy constructor and assignment operator
    BmnSsdElement(const BmnSsdElement&) = delete;
    BmnSsdElement& operator=(const BmnSsdElement&) = delete;


    ClassDef(BmnSsdElement, 2);
};

#endif /* BMNSSDELEMENT_H */
