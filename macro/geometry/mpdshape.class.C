
/******************************************************************************
*  Version:    09-Jun-2012   (EL)
*****************************************************************************/ 
class FairGeoRotation;

#include <iostream>
#include <string>

class Mpdshape: public TObject {

public:
  Mpdshape();
  Mpdshape(ofstream* file, std::string  volume_name,
	   std::string  mother_volume_name, std::string shape,
	   std::string media, std::string  points,
	   std::string position = "0. 0. 0.", 
	   std::string rotation = "1. 0. 0. 0. 1. 0. 0. 0. 1.", 
	   Int_t segment = 0, Int_t mother_segment = 0); 
  Mpdshape(ofstream* file, std::string  volume_name,
	   std::string  mother_volume_name, std::string shape,
	   std::string media); 
  ~Mpdshape();
  
  void SetFile(ofstream* file) { fFile = file; }
  void SetVolumeName(std::string  name) { fVolumeName = name; }
  void SetMotherVolumeName(std::string  name) { fMotherVolumeName = name; }
  void SetShape(std::string  shape) { fShape = shape; }
  void SetMedia(std::string  media) { fMedia = media; }
  void SetPoints(std::string  points) { fPoints = points; }

  void SetPosition(std::string  position) { fPosition = position; }
  void SetPosition(Double_t x, Double_t y, Double_t z, Int_t precision=6);
  void SetPosition(TVector3 vec);

  void SetRotation(std::string  rotation) { fRotation = rotation; }
  void SetRotation(Double_t p1, Double_t p2, Double_t p3,
		   Double_t p4, Double_t p5, Double_t p6,
		   Double_t p7, Double_t p8, Double_t p9);
  void SetRotation(Double_t z, Double_t y1, Double_t z1);

  void SetPositionRotation(std::string  position, std::string  rotation)
  { fPosition = position; fRotation = rotation; }
  void SetSegment(Int_t segment) { fSegment = segment; }
  void SetMotherSegment(Int_t segment) { fMotherSegment = segment; }

  void DumpToFile();
  void DumpWithIncrement() { DumpToFile(); fSegment++; }
  static std::string itoa(Double_t x);

  std::string GetVolumeName() const { return fVolumeName;}
  Int_t GetSegment() const { return fSegment;}
  void Clear();
  void Fill_TUBE(Double_t length_z, Double_t r_max, Double_t r_min);
  void Fill_TUBS(Double_t zmin, Double_t zmax, Double_t rmin,Double_t rmax, Double_t dr_st, Double_t dr_end);
   void Fill_TRAP(Double_t x, Double_t X, Double_t yW, Double_t zW, Int_t ra = 0);
  // void Fill_TRAP(Double_t x_small, Double_t x_large, Double_t yWidth, Double_t zWidth);
  //void Fill_RECTRAP(Double_t x_small, Double_t x_large,Double_t xx_small, Double_t xx_large,
	//	 Double_t yWidth, Double_t zWidth);	 
  void Fill_PGON( Double_t zmin, Double_t zmax, Double_t rmin, Double_t rmax, Double_t phi1, Int_t nEdges = 12);
  void Fill_BOX(Double_t xWidth, Double_t yWidth, Double_t zWidth);
  
protected:
  ofstream* fFile;
  std::string  fVolumeName;
  std::string  fMotherVolumeName;
  std::string  fShape;
  std::string  fMedia;
  std::string  fPoints;
  std::string  fPosition;
  std::string  fRotation;
  Int_t fSegment;
  Int_t fMotherSegment;
  FairGeoRotation* rot;
    
};

Mpdshape::Mpdshape() {

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  //basiclibs(); 
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  rot = new FairGeoRotation();
}

Mpdshape::Mpdshape(ofstream* file, std::string  volume_name,
		   std::string  mother_volume_name, std::string  shape,
		   std::string  media, std::string  points, std::string  position,
		   std::string  rotation, Int_t segment, Int_t mother_segment) 
  : fFile(file), fVolumeName(volume_name), fMotherVolumeName(mother_volume_name),
    fShape(shape), fMedia(media), fPoints(points), fPosition(position),
    fRotation(rotation), fSegment(segment), fMotherSegment(mother_segment) {

//   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
//   basiclibs(); 
   gSystem->Load("libGeoBase");
//   gSystem->Load("libParBase");
//   gSystem->Load("libBase");
  rot = new FairGeoRotation();
}

Mpdshape::Mpdshape(ofstream* file, std::string  volume_name,
		   std::string  mother_volume_name, std::string  shape,
		   std::string  media):
  fFile(file),  fVolumeName(volume_name), fMotherVolumeName(mother_volume_name),
  fShape(shape), fMedia(media) {

  //   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  //   basiclibs();
  
  gSystem->Load("libGeoBase");
  rot = new FairGeoRotation();
  fSegment = 0;
  fMotherSegment = 0;
}

Mpdshape::~Mpdshape() {}

void Mpdshape::DumpToFile() {
  *fFile << "//*************************************************" << endl;
  if (fSegment > 0) {
    *fFile << fVolumeName << "#" << fSegment << endl;;
  } else {
    *fFile << fVolumeName << endl;
  }
  if (fMotherSegment > 0) {
    *fFile << fMotherVolumeName << "#" << fMotherSegment << endl;
  } else {
    *fFile << fMotherVolumeName << endl;
  }

  if (fSegment <= 1) {
    *fFile << fShape << endl << fMedia << endl << fPoints << endl;
  }
  *fFile << fPosition << endl << fRotation << endl;
}

std::string Mpdshape::itoa(Double_t x) {
  std::ostringstream o;
  if (!(o << x)) return "ERROR";
  return o.str();
} 

void Mpdshape::SetPosition(TVector3 vec) {
  ostringstream o;
  o.precision(6);
  o.setf(ios::showpoint); 
  o.setf(ios::fixed);
  o << vec.x() << " " << vec.y() << " " << vec.z();
  fPosition.append(o.str().c_str());
}

void Mpdshape::SetPosition(Double_t x, Double_t y, Double_t z, Int_t precision) {
  ostringstream o;
  o.precision(precision);
  o.setf(ios::showpoint); 
  o.setf(ios::fixed);
  o << x << " " << y << " " << z;
  fPosition = o.str();
}

void Mpdshape::SetRotation(Double_t p1, Double_t p2, Double_t p3,
		      Double_t p4, Double_t p5, Double_t p6,
		      Double_t p7, Double_t p8, Double_t p9) {
	
  std::ostringstream o;
  o.setf(ios::showpoint); 
  o.setf(ios::fixed);
  o.precision(10);
  o << p1 << " " << p2 << " " << p3 << " "
    << p4 << " " << p5 << " " << p6 << " "
    << p7 << " " << p8 << " " << p9;
  fRotation = o.str();
}

void Mpdshape::SetRotation(Double_t z, Double_t y1, Double_t z1) {
  // z = phi
  // y1 = theta
  rot->setEulerAngles(z, y1, z1);
  std::ostringstream o;
  o.setf(ios::showpoint); 
  o.setf(ios::fixed);
  o << setprecision(10);
  for (Int_t i = 0; i < 9; i++) {
    o << rot->operator()(i) << " ";
  }
  fRotation = o.str();
}

std::string itoa(Double_t x) {
  std::ostringstream o;
  if (!(o << x)) return "ERROR";
  return o.str();
} 
 //______________________________________________________________
void Mpdshape::Clear() {
  fVolumeName.clear();
  fMotherVolumeName.clear();
  fShape.clear();
  fMedia.clear();
  fSegment = 0;
  fMotherSegment = 0;
  fPoints.clear(); fPosition.clear(); fRotation.clear();
}

//______________________________________________________________
void Mpdshape::Fill_TUBE(Double_t length_z, Double_t r_in, Double_t r_out) {
  //
  fPoints.clear();
  std::ostringstream o;
  o.setf(ios::showpoint); 
  o.setf(ios::fixed);
  o.precision(10);

  o << 0.0 << " " << 0.0 << " " << -length_z << endl
   <<  r_in << " " << r_out << endl
   << 0.0 << " " << 0.0 << " " << length_z;
  
   fPoints = o.str();
}
//______________________________________________________________
void Mpdshape::Fill_TUBS(Double_t zmin,Double_t zmax, Double_t rmin, Double_t rmax, Double_t st, Double_t end) {
  //
  fPoints.clear();
  std::ostringstream o;
  o.setf(ios::showpoint); 
  o.setf(ios::fixed);
  o.precision(10);

  o << 0.0 << " " << 0.0 << " " << zmin << endl
   <<  rmin << " " << rmax << endl
   << 0.0 << " " << 0.0 << " " << zmax << endl
   << st<< " " << end ;
   
   fPoints = o.str();
}

//_______________________________________________________________
void Mpdshape::Fill_PGON( Double_t zmin, Double_t zmax, Double_t rmin, Double_t rmax, Double_t phi1, Int_t nEdges) {
  fPoints.clear();
  std::ostringstream o;
  o.setf(ios::showpoint); 
  o.setf(ios::fixed);
  o.precision(10);
  o << "2 " << endl
    << phi1 << " 360 " << nEdges << endl
    << zmin << " " << rmin << " " << rmax << endl
    << zmax << " " << rmin << " " << rmax;
   fPoints = o.str();
   
}

//______________________________________________________________
//______________________________________________________________
void Mpdshape::Fill_TRAP(Double_t x_small, Double_t x_large,
			 Double_t yWidth, Double_t zWidth,
			 Int_t right_angled) {
  //
  fPoints.clear();
  std::ostringstream o;
  o.setf(ios::showpoint); 
  o.setf(ios::fixed);
  o.precision(10);

  if (right_angled == 0) {

  // trapezoid
           // _____________
           // \           / ^     
           //  \         /  | y  /z   
           //   \       /
           //    \_____/
           //       x

  o << x_small << " " << -yWidth << " " << -zWidth << endl
   << x_large << " " << yWidth << " " << -zWidth << endl
   << -x_large << " " << yWidth << " " << -zWidth << endl
   << -x_small << " " << -yWidth << " " << -zWidth << endl

   << x_small << " " << -yWidth << " " << zWidth << endl
   << x_large << " " << yWidth << " " << zWidth << endl
   << -x_large << " " << yWidth << " " << zWidth << endl
   << -x_small << " " << -yWidth << " " << zWidth;
  }
  else {

  // right-angled trapezoid
           // _____________
           // \           / ^     
           // \          /  | y  /z   
           // \         /
           // \________/
           //       x

  o << x_small << " " << -yWidth << " " << -zWidth << endl
   << x_large << " " << yWidth << " " << -zWidth << endl
   << -x_small << " " << yWidth << " " << -zWidth << endl
   << -x_small << " " << -yWidth << " " << -zWidth << endl

   << x_small << " " << -yWidth << " " << zWidth << endl
   << x_large << " " << yWidth << " " << zWidth << endl
   << -x_small << " " << yWidth << " " << zWidth << endl
   << -x_small << " " << -yWidth << " " << zWidth;
  }

   fPoints = o.str();
}


//______________________________________________________________
void Mpdshape::Fill_BOX(Double_t xWidth, Double_t yWidth, Double_t zWidth) {

  // box
  fPoints.clear();
  std::ostringstream o;
  o.setf(ios::showpoint); 
  o.setf(ios::fixed);
  o.precision(6);

  o << xWidth << " " << -yWidth << " " << -zWidth << endl
    << xWidth << " " << yWidth << " " << -zWidth << endl
    << -xWidth << " " << yWidth << " " << -zWidth << endl
    << -xWidth << " " << -yWidth << " " << -zWidth << endl
    << xWidth << " " << -yWidth << " " << zWidth << endl
    << xWidth << " " << yWidth << " " << zWidth << endl
    << -xWidth << " " << yWidth << " " << zWidth << endl
    << -xWidth << " " << -yWidth << " " << zWidth;
   fPoints = o.str();
}


//=============================================================
//=============================================================
//=============================================================

TVector3 LineCrossesCircle(TVector3 CircleCenter, Double_t CircleRadius,
			   TVector3 LineStart, TVector3 LineEnd) {

  TVector3 LineVec = LineEnd - LineStart;
  TVector3 VecToLine = LineStart - CircleCenter;
  Double_t a, b, c;
  Double_t sqrtterm, res1, res2, res;
  a = LineVec.X()*LineVec.X() + LineVec.Y()*LineVec.Y();
  b = 2 * ( VecToLine.X()*LineVec.X() + VecToLine.Y()*LineVec.Y() );
  c = ( VecToLine.X()*VecToLine.X() + VecToLine.Y()*VecToLine.Y()) -
    CircleRadius*CircleRadius;
  sqrtterm = b*b - 4*a*c;
  if(sqrtterm < 0) return LineStart;
  sqrtterm = TMath::Sqrt(sqrtterm);
  res1 = ( -b - sqrtterm ) / (2 * a);
  res2 = ( -b + sqrtterm ) / (2 * a);
  if(res1 >= 0 && res1 <= 1) {
    res = res1;
  } else {
    res = res2;
  }
  TVector3 result(LineStart.X() + res * (LineEnd.X() - LineStart.X()),
		  LineStart.Y() + res * (LineEnd.Y() - LineStart.Y()), 0);            
  return result;
} 

Double_t LineLength(Double_t alpha, TVector3 CircleCenter,
		    Double_t CircleRadius, TVector3 LineStart) {                   

  Double_t yend = CircleRadius*1.1;
  Double_t xend = yend*TMath::Tan(TMath::DegToRad()*alpha);
  TVector3 LineEnd(xend, yend, 0);
  TVector3 rend = LineCrossesCircle(CircleCenter, CircleRadius, LineStart, LineEnd);
  if (rend != LineStart) {
    TVector3 llen = rend - LineStart;
    return llen.Mag();
  }
  return 0;
} 

TVector3 StereoPosition(Double_t alpha, TVector3 CircleCenter,
			Double_t CircleRadius, TVector3 LineStart) {

  Double_t yend = CircleRadius*1.1;
  Double_t xend = yend*TMath::Tan(TMath::DegToRad()*alpha);
  TVector3 LineEnd(xend, yend, 0);
  TVector3 rend = LineCrossesCircle(CircleCenter, CircleRadius, LineStart, LineEnd);
  if (rend != LineStart) {
    TVector3 res = rend + LineStart;
    return res*0.5;
  }
  return LineStart;
} 

TVector3 StereoEnd(Double_t alpha, TVector3 CircleCenter,
		   Double_t CircleRadius, TVector3 LineStart) {

  Double_t yend = CircleRadius*1.1;
  Double_t xend = yend*TMath::Tan(TMath::DegToRad()*alpha);
  TVector3 LineEnd(xend, yend, 0);
  TVector3 rend = LineCrossesCircle(CircleCenter, CircleRadius, LineStart, LineEnd);
  return rend;
}
 
