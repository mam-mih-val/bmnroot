#ifndef __BMN_TOF_HIT_GEOUTILS_H
#define __BMN_TOF_HIT_GEOUTILS_H 1

#include <TVector3.h>
#include <TList.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TClonesArray.h>

#include <map>
#include <iostream>

#define TOF2_MAX_CHAMBERS 59
#define TOF2_MAX_STRIPS_IN_CHAMBER 32


class LRectangle	// convex quadrangle
{ 
    bool		IsInvalid;

  public:
    enum Side_t { kUpper=0,  kLower=1, kInvalid= -1 };

    Int_t       volumeUID;
    TVector3    A, B, C, D, center, perp;  // [cm]

    LRectangle() : IsInvalid(true), volumeUID(kInvalid){};
    LRectangle(Int_t uid, const TVector3& a, const TVector3& b, const TVector3& c, const TVector3& d, bool check = false);

    void        GetInteriorAngle(int vertexIndex) const {;} // FIXME
    TVector3    GetCenter() const{ return (A+B+C+D) * 0.25;}
    bool        isInvalid() const{ return IsInvalid;}

    Double_t    DistanceFromPointToLineSegment(const TVector3* pos, const TVector3& P1,const TVector3& P2)const;
    Double_t    DistanceFromPointToLine(const TVector3* pos, const TVector3& P1,const TVector3& P2)const;
    Double_t    MinDistanceToEdge(const TVector3* pos, Side_t& side) const;

    void        Print(std::ostream &out, const TVector3 &point, const char* comment = nullptr)const;
    void        Dump(const char* comment, std::ostream& out = std::cout) const;

    void        Shift(const TVector3& shift){ A +=shift; B +=shift; C +=shift; D +=shift; };

    inline void InitCenterPerp()
    {
        center = (A+B+C+D) * 0.25;
        perp = (B-A).Cross(D-A).Unit();
    }

    inline void CheckInValid()
    {
        IsInvalid = false;

        // Convex Polygon Definition: A polygon that has all interior angles less than 180°
        ;
        // Sum of Interior Angles, sum = 180*(n-2) degree, where n is the number of sides
        // A square has 4 sides, 	so interior angles sum = 360°
        ;
        // Rectangle check - all angles == 90 degree
        TVector3 ab = A-B, bc = B-C, cd = C-D, da = D-A;
        if( ab.Dot(bc) != 0. || bc.Dot(cd) != 0. || cd.Dot(da) != 0. || da.Dot(ab) != 0.)
        {
            std::cerr<<"\n ---> ERROR: invalid Rectangle."; Dump("", std::cerr);
            IsInvalid = true;
        }
    }
};

class LStrip : public LRectangle
{
  public:
    Int_t       sectorID, boxID, detectorID, stripID;
    Int_t       neighboring[2]; // dim same as  Side_t enum

    LStrip();
    LStrip(Int_t uid, Int_t sector, Int_t box, Int_t detector, Int_t strip);

    void        SetIDs(Int_t uid, Int_t sector, Int_t box, Int_t  detector, Int_t  strip){ volumeUID = uid; sectorID = sector; boxID = box; detectorID = detector; stripID = strip;}

    inline bool IsSameDetector(const LStrip& strip) const { return ( sectorID == strip.sectorID && boxID == strip.boxID && detectorID == strip.detectorID); }
    inline bool operator==(const LStrip& rhs) { return ( sectorID == rhs.sectorID && boxID == rhs.boxID && detectorID == rhs.detectorID && stripID == rhs.stripID); }
    inline bool operator!=(const LStrip& rhs) { return !((*this) == rhs); }

    void        Dump(const char* comment = nullptr, std::ostream& out = std::cout) const;
    Double_t    Distance(Side_t side, const LStrip& strip);
};

class BmnTofGeoUtils
{
    typedef std::map<Int_t, LStrip>     MStripType; // pair<detectorUID, Strip parameters>
    typedef MStripType::const_iterator  MStripCIT;
    typedef MStripType::iterator        MStripIT;

    Int_t fVerbose;
    Bool_t useMCdata;
    Int_t nchambers;
    Double_t halfxwidth[TOF2_MAX_CHAMBERS];
    Double_t halfywidth[TOF2_MAX_CHAMBERS];
    Int_t chtype[TOF2_MAX_CHAMBERS];
    Int_t nstrips[TOF2_MAX_CHAMBERS];
    Double_t zchamb[TOF2_MAX_CHAMBERS];
    Double_t xcens[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    Double_t ycens[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    Double_t xmins[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    Double_t xmaxs[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    Double_t ymins[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
    Double_t ymaxs[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];

    MStripType      mStrips; //!  indexing strips by detectorUID

  public:
    BmnTofGeoUtils(Bool_t useMC = true);

    void            FindNeighborStrips(TH1D* h1 = nullptr, TH2D* h2 = nullptr, bool doTest = false);
    void            ParseTGeoManager(bool useMCinput, TH2D* h1 = nullptr, bool forced = false);
    void            ParseStripsGeometry(const char *geomFile);
    void            SetVerbosity(Int_t ver) {fVerbose = ver;}
    const LStrip*   FindStrip(Int_t UID);
    const LStrip*   FindStrip(Int_t UID, TVector3& p);
    int             readGeom(const char *);
};

#endif
