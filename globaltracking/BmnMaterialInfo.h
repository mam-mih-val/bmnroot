/** BmnMaterialInfo.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * @since 2008-2014
 * @version 1.0
 **
 ** Data class to store information about detector material.
 **/

#ifndef BMNMATERIALINFO_H_
#define BMNMATERIALINFO_H_

#include <string>
#include <sstream>

using std::string;

class BmnMaterialInfo
{
public:
   /* Constructor */
   BmnMaterialInfo():
      fLength(0.),
      fRL(0.),
      fRho(0.),
      fZ(0.),
      fA(0.),
      fZpos(0.),
      fName(""){}

   // This constructor never used!
//   /* Constructor with assignment
//    * @param length Length of the material [cm]
//    * @param rl Radiation length [cm]
//    * @param rho Density [g/cm^3]
//    * @param Z
//    * @param A
//    * @param zpos Z position of the material
//    */
//   BmnMaterialInfo(
//      Float_t length,
//      Float_t rl,
//      Float_t rho,
//      Float_t Z,
//      Float_t A,
//      Float_t zpos,
//      string name):
//      fLength(length),
//      fRL(rl),
//      fRho(rho),
//      fZ(Z),
//      fA(A),
//      fZpos(zpos),
//      fName(name) {}

   /* Destructor */
   virtual ~BmnMaterialInfo() {};

   /*@return Length of the material */
   Float_t GetLength() const { return fLength;}

   /*@return Radiation length */
   Float_t GetRL() const { return fRL;}

   /*@return Density */
   Float_t GetRho() const { return fRho;}

   /*@return Atomic number */
   Float_t GetZ() const { return fZ;}

   /*@return  Atomic mass */
   Float_t GetA() const { return fA;}

   /*@return Z position of the material */
   Float_t GetZpos() const { return fZpos;}

   const string& GetName() const { return fName;}

   /* Sets length of the material */
   void SetLength(Float_t length) {fLength = length;}

   /* Sets radiation length of the material */
   void SetRL(Float_t rl) {fRL = rl;}

   /* Sets density */
   void SetRho(Float_t rho) {fRho = rho;}

   /* Sets atomic number */
   void SetZ(Float_t Z) {fZ = Z;}

   /* Sets atomic mass */
   void SetA(Float_t A) {fA = A;}

   /* Sets Z position of the material */
   void SetZpos(Float_t zpos) {fZpos = zpos;}

   void SetName(const string& name) {fName = name;}

   /* @return String representation of the class */
   virtual std::string ToString() const {
      std::stringstream ss;
      ss << "MaterialInfo: length=" << fLength << " rl=" << fRL
         << " rho=" << fRho << " Z=" << fZ << " A=" << fA << " zpos=" << fZpos
         << " name=" << fName << std::endl;
      return ss.str();
   }

private:
   Float_t fLength; // Length of the material [cm]
   Float_t fRL; // Radiation length [cm]
   Float_t fRho; // Density [g/cm^3]
   Float_t fZ; // Atomic number
   Float_t fA; // Atomic mass
   Float_t fZpos; // Z position of the material
   string fName; // Name of material
};

#endif /*BMNMATERIALINFO_H_*/
