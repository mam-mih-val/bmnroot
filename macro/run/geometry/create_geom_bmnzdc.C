/*************************************************************************************
*  Author:     Elena Litvinenko (EL)
*  Project:    BMN (mpd.jinr.ru/trac/browser/bmnroot; git.jinr.ru/nica/bmnroot)
*  Created:     28-Oct-2015
*  Last update: 05-11-2015
************************************************************************************/ 

#include "mpdshape.class.C"

class Bmn_ZDC_map_element{
public:
  Bmn_ZDC_map_element(){ id=chan=front_chan=size=x=y=-1; used=0;};
  void set(unsigned long v1,unsigned char v2,unsigned char v3,unsigned char v4,unsigned char v5,unsigned char v6,unsigned char v7, float v8, float v9){
    id=v1; chan=v2; front_chan=v3; size=v4; ix=v5; iy=v6; used=v7; x=v8; y=v9;
  }; 
  unsigned long id;
  unsigned char chan;
  unsigned char front_chan;
  unsigned char size;
  unsigned char ix;
  unsigned char iy;
  unsigned char used;
  float x;
  float y;
};

int n_rec=0; 
Bmn_ZDC_map_element zdc_map_element[256]; 
 
void local_BmnZDCRaw2Digit(TString mappingFile) {
    n_rec=0;
    TString dummy;
    ifstream in;

    TString dir = getenv("VMCWORKDIR");
    TString path = dir + "/input/";
    in.open((path + mappingFile).Data());
    in >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy>> dummy >> dummy >> dummy >> dummy;
    while (!in.eof()) {
        int id,chan,front_chan,size,ix,iy,used;
	float x,y;
        in >>std::hex >> id >>std::dec >> chan >> front_chan>>size>>ix>>iy>>x>>y>>used;
        if (!in.good()) break;
        zdc_map_element[n_rec].id=id;
        zdc_map_element[n_rec].chan=chan;
        zdc_map_element[n_rec].front_chan=front_chan;
        zdc_map_element[n_rec].size=size;
        zdc_map_element[n_rec].ix=ix;
        zdc_map_element[n_rec].iy=iy;
        zdc_map_element[n_rec].x=x;
        zdc_map_element[n_rec].y=y;
        zdc_map_element[n_rec].used=used;
	if (size<=2)
	  n_rec++;
    }
    in.close();
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

void fix_rect_xy_width_zcenter( std::ostringstream &points, Double_t xhalfwidth, Double_t yhalfwidth, Double_t zfullwidth)
{
  points.str(""); points << setprecision(4);
  points <<  xhalfwidth << " " << - yhalfwidth << " " << (- zfullwidth /2)<< endl;
  points <<  xhalfwidth << " " <<  yhalfwidth << " " << (- zfullwidth /2) << endl;
  points << - xhalfwidth << " " <<  yhalfwidth << " " << (- zfullwidth /2) << endl;
  points << - xhalfwidth << " " << - yhalfwidth << " " << (- zfullwidth /2) << endl;
  points <<  xhalfwidth << " " << - yhalfwidth << " " << zfullwidth/2 << endl;
  points <<  xhalfwidth << " " <<  yhalfwidth << " " << zfullwidth/2 << endl;
  points << - xhalfwidth << " " <<  yhalfwidth << " " << zfullwidth/2 << endl;
  points << - xhalfwidth << " " << - yhalfwidth << " " << zfullwidth/2 << endl;
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

void create_geom_bmnzdc (const char* filename = "", const char *detector_name="zdc", int version=1)
{
  TString mappingFile="ZDC_map_Mar08.txt";

  local_BmnZDCRaw2Digit(mappingFile);

  Double_t 
    initDist = 9500.,          // Distance between interacting point and left point of detector (along the beam), mm
    x_fullwidth_mother=1650.,  // x size of external box of the detector
    y_fullwidth_mother=1056.,  // y size of external box of the detector
    z_fullwidth_mother=1450.,  // z size of external box of the detector

    z_fullwidth_box=1190.,            // end of WLS
    thickness_box=2,                   //  size suggested by EL
    thickness_WLS=0.4,                 //  size suggested by EL

    xy_fullwidth_big_box  =  150.,      // XY size for small module box (for 36 small central modules)
    xy_fullwidth_small_box = 75.,       // XY size for small module box (for 36 small central modules)

    x_fullwidth_big_plate=xy_fullwidth_big_box-2*thickness_box,      // =146: x size of scintillator plate for big modules
    y_fullwidth_big_plate=x_fullwidth_big_plate-4,                   // =142: y size of scintillator plate for big modules
    x_fullwidth_small_plate=xy_fullwidth_small_box-2*thickness_box,  // =71:  x size of scintillator plate for small modules
    y_fullwidth_small_plate=x_fullwidth_small_plate-4,               // =67:  y size of scintillator plate for small modules

    zWidthS = 5.0,           // z - FULL-size of scintillator layer (scintillator plate), mm    
    zWidthA = 10.0;          // z - FULL-size of absorber layer, mm 

  Double_t 
    moduleWidth = 75;   

  Int_t
    number_of_layers_along_Z = 64;   // for HCAL - 80 scintillator/lead layers 

  Int_t
    nlayers =  number_of_layers_along_Z; // number of Absorber-Scintillator layers 

  // Int_t
  //   number_of_small_modules_in_XY_plane = 36, // 6x6     
  //   number_of_big_modules_in_XY_plane = 68;   // (7x4+2x3)x2   

  ofstream* f;
  if (!strlen(filename)) {
    char filename1[200];
    sprintf(filename1,"%s_%dmodules_%dlayers_v%d\.geo", detector_name, n_rec, number_of_layers_along_Z, version);   
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

  char mothername[30]=Form("%s01",detector_name);   //" zdc"

  *f << "//*****************************************************" << endl;
  *f << "// ZDC detector for BMN project," << endl;
  *f << "// geometry version 1" << endl;
  *f << "// created 19-11-2015 by E.I.Litvinenko" << endl;
  *f << "//*****************************************************" << endl;


  //  fix_rect_xy_width_z0 (points, x_fullwidth_mother/2+5,y_fullwidth_mother/2+5, z_fullwidth_mother+5);
  fix_rect_xy_width_zcenter (points, x_fullwidth_mother/2+1,y_fullwidth_mother/2+1, z_fullwidth_mother+2);
  Mpdshape* detmother = new Mpdshape(f, mothername, "cave", "BOX", "air",    points.str(),position.str());
                     
  detmother->SetSegment(1);
  //  detmother->SetPosition(0., 0., initDist);  
  detmother->SetPosition(0., 0., initDist+z_fullwidth_mother/2);  
  detmother->SetRotation(" 1. 0. 0. 0. 1. 0. 0. 0.  1.");  
  detmother->DumpWithIncrement();

  addnum(mothername);                               // "zdc#1"

  // one module small box:

  char boxnames[30]     = Form("%s01boxs",detector_name);      // "zdc01boxs"

  position.str(""); rotation.str("");
  fix_rect_width_zcenter(points,xy_fullwidth_small_box/2,z_fullwidth_box+thickness_box*2);
  Mpdshape* detmoduleboxs = new Mpdshape(f, boxnames, mothername, "BOX", "steel",    points.str(),position.str());
  detmoduleboxs->SetSegment(1); 

  addnum(boxnames);                               // "zdc01boxs#1"

  // one module big box:

  char boxnameb[30]     = Form("%s01boxb",detector_name);      // "zdc01boxb"

  position.str(""); rotation.str("");
  fix_rect_width_zcenter(points,xy_fullwidth_big_box/2,z_fullwidth_box+thickness_box*2);
  Mpdshape* detmoduleboxb = new Mpdshape(f, boxnameb, mothername, "BOX", "steel",    points.str(),position.str());
  detmoduleboxb->SetSegment(1); 

  addnum(boxnameb);                               // "zdc01boxb#1"

  // one  small module:

  char modulenames[30]=Form("%s01ms",detector_name);  // "zdc01ms"

  position.str(""); rotation.str("");
  fix_rect_width_zcenter(points,xy_fullwidth_small_box/2-thickness_box,z_fullwidth_box);
  Mpdshape* detmodules = new Mpdshape(f, modulenames, boxnames, "BOX", "air",    points.str(),position.str());
  detmodules->SetSegment(1); 
  detmodules->SetPosition(0,0,0); 

  addnum(modulenames);                               // "zdc01ms#1"

  // one  big module:

  char modulenameb[30]=Form("%s01mb",detector_name);  // "zdc01mb"

  position.str(""); rotation.str("");
  fix_rect_width_zcenter(points,xy_fullwidth_big_box/2-thickness_box,z_fullwidth_box);
  Mpdshape* detmoduleb= new Mpdshape(f, modulenameb, boxnameb, "BOX", "air",    points.str(),position.str());
  detmoduleb->SetSegment(1); 
  detmoduleb->SetPosition(0,0,0);

  addnum(modulenameb);                               // "zdc01mb#1"

  // layers

  char absorber_names[30]     = Form("%s01as",detector_name);      // "zdc01as"
  char scintillator_names[30] = Form("%s01ss",detector_name);      // "zdc01ss"
  char absorber_nameb[30]     = Form("%s01ab",detector_name);      // "zdc01ab"
  char scintillator_nameb[30] = Form("%s01sb",detector_name);      // "zdc01sb"

  Mpdshape *detlayer_abss=0, *detlayer_scis=0;
  Mpdshape *detlayer_absb=0, *detlayer_scib=0;

  Double_t xm,ym;                          // coordinates of center of the module
  Int_t imodule, jmodule;                  // xy-index and z-index of the module

  char module_rotation[30] = "1. 0. 0. 0. 1. 0. 0. 0. 1.";

  Bool_t 
    first_small = 1,
    first_large = 1;

  for (imodule=0; imodule<n_rec; imodule++) {

    xm=zdc_map_element[imodule].x;
    ym=zdc_map_element[imodule].y;
    if (zdc_map_element[imodule].size==1) {
      detmoduleboxs->SetPosition(xm,ym,(z_fullwidth_box-z_fullwidth_mother-2)/2);
      detmoduleboxs->DumpWithIncrement();
      if (first_small) {
	detmodules->DumpWithIncrement();
	for (int ilayer=0;ilayer<nlayers;ilayer++) {
	  if (!ilayer) {
	    position.str("");
	    fix_rect_xy_width_zcenter (points,x_fullwidth_small_plate/2,y_fullwidth_small_plate/2,zWidthA);
	    detlayer_abss = new Mpdshape(f, absorber_names, modulenames, "BOX", "lead",    points.str(),position.str());
	    detlayer_abss->SetSegment(1);  

	    position.str("");
	    fix_rect_xy_width_zcenter (points,x_fullwidth_small_plate/2,y_fullwidth_small_plate/2,zWidthS);
	    detlayer_scis = new Mpdshape(f, scintillator_names, modulenames, "BOX", "polystyrene",    points.str(),position.str());
	    detlayer_scis->SetSegment(1);
	  }  

	  detlayer_abss->SetPosition(0.,0.,-z_fullwidth_box/2+zWidthA*0.5+(zWidthA+zWidthS)*ilayer);
	  detlayer_abss->SetRotation(" 1. 0. 0. 0. 1. 0. 0. 0. 1.");
	  detlayer_abss->DumpWithIncrement();
	  detlayer_scis->SetPosition(0.,0.,-z_fullwidth_box/2+zWidthA+zWidthS*0.5+(zWidthA+zWidthS)*ilayer); 
	  detlayer_scis->SetRotation(" 1. 0. 0. 0. 1. 0. 0. 0. 1.");
	  detlayer_scis->DumpWithIncrement();
	
	}
	first_small=0;
      }
    }
    else {
      detmoduleboxb->SetPosition(xm,ym,(z_fullwidth_box-z_fullwidth_mother-2)/2);
      detmoduleboxb->DumpWithIncrement();
      if (first_large) {
	detmoduleb->DumpWithIncrement();
	for (int ilayer=0;ilayer<nlayers;ilayer++) {
	  if (!ilayer) {
	    position.str("");
	    fix_rect_xy_width_zcenter (points,x_fullwidth_big_plate/2,y_fullwidth_big_plate/2,zWidthA);
	    detlayer_absb = new Mpdshape(f, absorber_nameb, modulenameb, "BOX", "lead",    points.str(),position.str());
	    detlayer_absb->SetSegment(1);  

	    position.str("");
	    fix_rect_xy_width_zcenter (points,x_fullwidth_big_plate/2,y_fullwidth_big_plate/2,zWidthS);
	    detlayer_scib = new Mpdshape(f, scintillator_nameb, modulenameb, "BOX", "polystyrene",    points.str(),position.str());
	    detlayer_scib->SetSegment(1); 
	  }  

	  detlayer_absb->SetPosition(0.,0.,-z_fullwidth_box/2+zWidthA*0.5+(zWidthA+zWidthS)*ilayer); 
	  detlayer_absb->SetRotation(" 1. 0. 0. 0. 1. 0. 0. 0. 1.");
	  detlayer_absb->DumpWithIncrement();
	  detlayer_scib->SetPosition(0.,0.,-z_fullwidth_box/2+zWidthA+zWidthS*0.5+(zWidthA+zWidthS)*ilayer); 
	  detlayer_scib->SetRotation(" 1. 0. 0. 0. 1. 0. 0. 0. 1.");
	  detlayer_scib->DumpWithIncrement();
	}
	first_large=0;
      }
    }
    cout << " imodule: " << imodule << " " << xm << " " << ym << endl;
  }
  cout << " layers: " << nlayers << endl;
  cout << " modules: " << imodule << endl;                 
                                                                  

  // close geometry file    
  f->close(); 
}
