/*************************************************************************************
*  Author:     Elena Litvinenko (EL)
*  Project:    mpd.jinr.ru
*  Created:    21-Mar-2014
************************************************************************************/ 

#include "mpdshape.class.C"

void fix_rect_width_z0( std::ostringstream &points, Double_t xyhalfwidth, Double_t zfullwidth)
{
  points.str(""); points << setprecision(4);
  points <<  xyhalfwidth << " " << - xyhalfwidth << " " << "0." << endl;
  points <<  xyhalfwidth << " " <<  xyhalfwidth << " " << "0." << endl;
  points << - xyhalfwidth << " " <<  xyhalfwidth << " " << "0." << endl;
  points << - xyhalfwidth << " " << - xyhalfwidth << " " << "0." << endl;
  points <<  xyhalfwidth << " " << - xyhalfwidth << " " << zfullwidth << endl;
  points <<  xyhalfwidth << " " <<  xyhalfwidth << " " << zfullwidth << endl;
  points << - xyhalfwidth << " " <<  xyhalfwidth << " " << zfullwidth << endl;
  points << - xyhalfwidth << " " << - xyhalfwidth << " " << zfullwidth << endl;
}

void fix_rect_width_zcenter( std::ostringstream &points, Double_t xyhalfwidth, Double_t zfullwidth)
{
  points.str(""); points << setprecision(4);
  points <<  xyhalfwidth << " " << - xyhalfwidth << " " << (- zfullwidth /2)<< endl;
  points <<  xyhalfwidth << " " <<  xyhalfwidth << " " << (- zfullwidth /2) << endl;
  points << - xyhalfwidth << " " <<  xyhalfwidth << " " << (- zfullwidth /2) << endl;
  points << - xyhalfwidth << " " << - xyhalfwidth << " " << (- zfullwidth /2) << endl;
  points <<  xyhalfwidth << " " << - xyhalfwidth << " " << zfullwidth/2 << endl;
  points <<  xyhalfwidth << " " <<  xyhalfwidth << " " << zfullwidth/2 << endl;
  points << - xyhalfwidth << " " <<  xyhalfwidth << " " << zfullwidth/2 << endl;
  points << - xyhalfwidth << " " << - xyhalfwidth << " " << zfullwidth/2 << endl;
}

void addnum(char *name)
{
  if (name) {
    int len = strlen(name);
    if (len) {
      name[len]='#'; 
      name[len+1]='1';
      name[len+2]='\0';
    }
  }
}

void calc_module_center_xy (Int_t imodule, Int_t  n_x, Int_t  n_y, 
			    Double_t innerRadius, Double_t  moduleWidth, Double_t moduleLength,
			    Double_t &xm, Double_t &ym, char *module_rotation)
{
  if (imodule<=n_x) {                                           // bottom
    xm = moduleWidth * ( 1.0*imodule - 0.5*(n_x+1));
    ym = -innerRadius-moduleLength/2;
    strcpy(module_rotation,"1. 0. 0. 0. 0. 1. 0. 1.  0.");     // (x,z,y)
  }
  else {                                                       // right
    if (imodule<=(n_x+n_y)) {
      xm = innerRadius+moduleLength/2;
      ym = moduleWidth * ( 1.0*(imodule-n_x) - 0.5*(n_y+1));
      strcpy(module_rotation,"0. 0. 1. 0. 1. 0. 1. 0. 0. ");   // (z,y,x)
    }
    else {
      if (imodule<=(2*n_x+n_y)){                                               // top
	xm = moduleWidth * ( 2.0*n_x+1.0*n_y-1.0*imodule +1.0 - 0.5*(n_x+1));
	ym = innerRadius+moduleLength/2;
	//	strcpy(module_rotation,"1. 0. 0. 0. 1. 0. 0. 0.  1.");
	strcpy(module_rotation,"1. 0. 0. 0. 0. 1. 0. 1.  0.");  // (x,z,y)
      }
      else {                                                                   // left
	xm = -innerRadius-moduleLength/2;
	ym = moduleWidth * ( 2.0*(n_x+n_y)-1.0*imodule +1.0 - 0.5*(n_y+1));
	//	strcpy(module_rotation,"0. 1. 0. 1. 0. 0. 0. 0. 1. ");   // (y,x,z)
      strcpy(module_rotation,"0. 0. 1. 0. 1. 0. 1. 0. 0. ");   // (z,y,x)
      }
    }
  }
}

void create_geom_recoil (const char* filename = "", const char *detector_name="recoil", int version=1)
{
  Double_t moduleWidth  =  40.;        // for module
  Double_t moduleLength = 300.;        // for module

  Double_t zWidthA = 8.0; // 16.0;         // z - FULL-dimension of absorber layer, mm         // for layer
  Double_t zWidthS = 2.0; // 4.0;          // z - FULL-dimension of scintillator layer, mm     // for layer

  Double_t innerRadius  = 200.;         // the hole XY rectangle: "2*innerRadius x 2*innerRadius", mm  // for full detector
  Double_t outerRadius = innerRadius+moduleLength ;  // external XY rectangle, mm                      // for full detector

  Double_t z_min_mother = -innerRadius;  // <= -innerRadius/TMath::Tan(TMath::DegToRad() * 45.0)       // for full detector
  Double_t z_max_mother = 280.0;         // >=  innerRadius/TMath::Tan(TMath::DegToRad() * 40.0)       // for full detector
  Double_t zHalfWidth = (z_max_mother-z_min_mother)/2;                                                     // for full detector
  Double_t initDist = (z_max_mother+z_min_mother)/2; // Distance between center of detector and interacting point (along the beam), mm

  Int_t
    number_of_modules_in_XY_plane = (int)((outerRadius+innerRadius)*4/moduleWidth);     

  Int_t
    number_of_modules_along_Z = zHalfWidth * 2 / moduleWidth;

  ofstream* f;
  if (!strlen(filename)) {
    char filename1[200];
    sprintf(filename1,"%s_modules_%dx%d_v%d\.geo", detector_name, number_of_modules_in_XY_plane,number_of_modules_along_Z, version);   
    f = new ofstream(filename1, ios::out | ios::trunc);
    cout << "filename: " << filename1 << endl;
  }
  else {
    f = new ofstream(filename, ios::out | ios::trunc);
    cout << "filename: " << filename << endl;
  }


  // helper streams
  std::ostringstream points, position, rotation;
  points.setf(ios::showpoint); 
  position.setf(ios::showpoint); position.setf(ios::fixed); position << setprecision(6);
  rotation.setf(ios::showpoint); rotation.setf(ios::fixed); rotation << setprecision(6);

  // detector mother volume:

  char mothername[30]=Form("%s01",detector_name);   //" recoil"

 *f << "//*****************************************************" << endl;
 *f << "// RECOIL detector for BM@ project," << endl;
 *f << "// geometry version 1" << endl;
 *f << "// created 25-03-2014 by E.I.Litvinenko" << endl;
 *f << "//*****************************************************" << endl;


 //   points << "0.0 0.0 " << (-zHalfWidth-5) << endl;
 //   points << innerRadius << " " << (outerRadius+5) << endl;
 //   points << "0.0 0.0 " << (zHalfWidth+5) << endl;
 //  Mpdshape* detmother = new Mpdshape(f, mothername, "cave", "TUBE", "air",    points.str(),position.str());                       

 points << "2" << endl;
 points << "45 360 4" << endl;
 points << (-zHalfWidth-5) << " " << innerRadius << " " << (outerRadius+5) << endl;
 points << (zHalfWidth+5) << " " <<  innerRadius << " " << (outerRadius+5) << endl;
 Mpdshape* detmother = new Mpdshape(f, mothername, "cave", "PGON", "air",    points.str(),position.str());  
                     
 detmother->SetSegment(1);
 detmother->SetPosition(0., 0., initDist);  
 detmother->SetRotation(" 1. 0. 0. 0. 1. 0. 0. 0.  1.");  
 detmother->DumpWithIncrement();

 addnum(mothername);                               // "recoil#1"

 // one module:

 char modulename[30]=Form("%s01m",detector_name);  // "recoil01m"

  position.str(""); rotation.str("");
  fix_rect_width_zcenter(points,moduleWidth/2,moduleLength);
  Mpdshape* detmodule = new Mpdshape(f, modulename, mothername, "BOX", "air",    points.str(),position.str());
  detmodule->SetSegment(1); 

  addnum(modulename);                               // "recoil01m#1"

  Mpdshape *detlayer_abs=0, *detlayer_sci=0;

  Double_t xm,ym;                          // coordinates of center of the module
  Int_t imodule, jmodule;                  // xy-index and z-index of the module

  Int_t
    nlayers =  TMath::Nint(moduleLength/(zWidthA+zWidthS));        // number of Absorber-Scintillator layers 

  Int_t
    n_x = innerRadius*2/moduleWidth,  // top or bottom
    n_y = outerRadius*2/moduleWidth;  // left or right

  char absorber_name[30]     = Form("%s01a",detector_name);      // "recoil01a"
  char scintillator_name[30] = Form("%s01s",detector_name);      // "recoil01s"

  char module_rotation[30] = "1. 0. 0. 0. 1. 0. 0. 0. 1.";

  for (jmodule=1; jmodule<=number_of_modules_along_Z; jmodule++) {
    for (imodule=1; imodule<=number_of_modules_in_XY_plane; imodule++) {

      calc_module_center_xy (imodule, n_x, n_y, innerRadius, moduleWidth, moduleLength, xm, ym, module_rotation); 
      detmodule->SetPosition( xm, ym, moduleWidth*(-0.5*number_of_modules_along_Z+1.0*jmodule-0.5));
      detmodule->SetRotation(module_rotation); // " 1. 0. 0. 0. 1. 0. 0. 0. 1.");
      detmodule->DumpWithIncrement();

      if ((imodule==1)&&(jmodule==1)) {
	for (int ilayer=0;ilayer<nlayers;ilayer++) {
	  if (!ilayer) {
	    position.str("");
	    fix_rect_width_zcenter(points,moduleWidth/2,zWidthA);
	    detlayer_abs = new Mpdshape(f, absorber_name, modulename, "BOX", "lead",    points.str(),position.str());
	    detlayer_abs->SetSegment(1);  

	    position.str("");
	    fix_rect_width_zcenter(points,moduleWidth/2,zWidthS);
	    detlayer_sci = new Mpdshape(f, scintillator_name, modulename, "BOX", "FscScint",    points.str(),position.str());
	    detlayer_sci->SetSegment(1);  
	  }
	  detlayer_abs->SetPosition(0.,0.,(zWidthA+zWidthS)*(1.0*ilayer-0.5*nlayers+0.5));
	  detlayer_abs->SetRotation(" 1. 0. 0. 0. 1. 0. 0. 0. 1.");
	  detlayer_abs->DumpWithIncrement();
	  detlayer_sci->SetPosition(0.,0.,(zWidthA+zWidthS)*(1.0*ilayer-0.5*nlayers+1));
	  detlayer_sci->SetRotation(" 1. 0. 0. 0. 1. 0. 0. 0. 1.");
	  detlayer_sci->DumpWithIncrement();
	}
	cout << " layers: " << nlayers << endl;
      }
    }
  }
  cout << " modules: " << (imodule-1) << endl;
                  
  // close geometry file                                                                                                                                                                        
  f->close(); 

}
