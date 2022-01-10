#include "TMath.h"
#include "TSystem.h"
#include "BmnScWallRaw2Digit.h"

void BmnScWallRaw2Digit::print() {
     printf("SCWALLLL\n");
}

BmnScWallRaw2Digit::BmnScWallRaw2Digit(){
  fPeriodId = 0; 
  fRunId = 0;
}

BmnScWallRaw2Digit::BmnScWallRaw2Digit(Int_t period, Int_t run, TString mappingFile, TString CalibrationFile) {
    fPeriodId = period; 
    fRunId = run;
    fmappingFileName = mappingFile;
    ParseConfig(fmappingFileName);
    fcalibrationFileName = CalibrationFile; 
    ParseCalibration(fcalibrationFileName);
}

void BmnScWallRaw2Digit::ParseConfig(TString mappingFile) {

  namespace po = boost::program_options;

  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/input/";

  typedef std::vector< std::string > vect_string_t;
  float version;
  std::string comment;
  std::vector<std::string> adc_serials;
  std::vector<std::string> configuration;

  // Setup options.
  po::options_description desc("Options");
  desc.add_options()
    ("VERSION.id", po::value< float >( &version ), "version identificator" )
    ("COMMENT.str", po::value< std::string >( &comment ), "comment" )
    ("ADCSERIALS.serial", po::value< vect_string_t >( &adc_serials )->multitoken(),
                     "adc serials" )
    ("CONFIGURATION.config", po::value< vect_string_t >( &configuration )->multitoken(),
                     "configuration" )
    ;

  // Load config file.
  po::variables_map vm;
  std::ifstream config_file( (path + mappingFile).Data() , std::ifstream::in );
  if (!config_file.is_open()) {
	  printf("BmnScWallRaw2Digit : Loading Config from file: %s - file open error!\n", mappingFile.Data());
	  return;
  }
  printf("BmnScWallRaw2Digit : Loading Config from file: %s\n", mappingFile.Data());
  po::store( po::parse_config_file( config_file , desc ), vm );
  config_file.close();
  po::notify( vm );

  fScWallSerials.clear();
  for ( auto it : adc_serials )
    fScWallSerials.push_back(std::stoul(it, nullptr, 16));
  std::sort(fScWallSerials.begin(), fScWallSerials.end());

  std::string adc_ser;
  int adc_chan;
  std::string zone;
  int x_position;
  int y_position;
  int size;

  fUniqueX.clear();
  fUniqueY.clear();
  fUniqueSize.clear();
  // First pass for unique.
  for ( auto it : configuration )
  {
    istringstream ss(it);
    ss >> adc_ser >> adc_chan >> zone >> x_position >> y_position >> size;
    fUniqueX.insert(x_position);
    fUniqueY.insert(y_position);
    fUniqueSize.insert(size);
  }

  // Second pass for mapping.
  for ( auto it : configuration )
  {
    istringstream ss(it);
    ss >> adc_ser >> adc_chan >> zone >> x_position >> y_position >> size;

    int adc_board_index, xIdx, yIdx, SizeIdx, ZoneIdx = -1;
    auto iter = find(fScWallSerials.begin(), fScWallSerials.end(), std::stoul(adc_ser, nullptr, 16));
    if (iter != fScWallSerials.end())
      adc_board_index = std::distance(fScWallSerials.begin(), iter);
    else printf("BmnScWallRaw2Digit : unknown adc serial\n");

    xIdx = std::distance(fUniqueX.begin(), fUniqueX.find(x_position));
    yIdx = std::distance(fUniqueY.begin(), fUniqueY.find(y_position));
    SizeIdx = std::distance(fUniqueSize.begin(), fUniqueSize.find(size));

    ZoneIdx = (int)(zone[0] - 'A' + 1);
    if (ZoneIdx > 12) cout << "MAX zone letter is L" << endl;

    UInt_t flat_channel = (UInt_t) GetFlatChannelFromAdcChannel( std::stoul(adc_ser, nullptr, 16), adc_chan );
    UInt_t unique_address = (ZoneIdx > 12)? 0 : BmnScWallAddress::GetAddress( adc_board_index, adc_chan, xIdx, yIdx, SizeIdx, ZoneIdx);
    fChannelMap.insert(std::make_pair(flat_channel, unique_address));
  }
  //std::cout << "COMMENT.str: " << comment << std::endl;

}


void BmnScWallRaw2Digit::ParseCalibration(TString calibrationFile) {

  namespace po = boost::program_options;

  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/scwall/";

  typedef std::vector< std::string > vect_string_t;
  typedef std::vector< std::complex<float> > vect_complf_t;
  float version;
  std::string comment;
  //std::vector<std::string> digi_params;
  std::vector<std::string> calibrations;

  // Setup options.
  po::options_description desc("Options");
  desc.add_options()
    ("VERSION.id", po::value< float >( &version ), "version identificator" )
    ("COMMENT.str", po::value< std::string >( &comment ), "comment" )
    ("PARAMETERS.gateBegin", po::value< int >( &fdigiPars.gateBegin ), "digi parameters" )
    ("PARAMETERS.gateEnd", po::value< int >( &fdigiPars.gateEnd ), "digi parameters" )
    ("PARAMETERS.threshold", po::value< float >( &fdigiPars.threshold ), "digi parameters" )
    ("PARAMETERS.signalType", po::value< int >( &fdigiPars.signalType ), "digi parameters" )
    ("PARAMETERS.doInvert", po::value< bool >( &fdigiPars.doInvert ), "digi parameters" )
    ("FITPARAMETERS.isfit", po::value< bool >( &fdigiPars.isfit ), "digi parameters" )  
    ("FITPARAMETERS.harmonic", po::value< vect_complf_t >( &fdigiPars.harmonics )->multitoken(),
                     "fit harmonics" )                   
    ("CALIBRATION.calib", po::value< vect_string_t >( &calibrations )->multitoken(),
                     "calibrations" )
    ;

  // Load config file.
  po::variables_map vm;
  std::ifstream calib_file( (path + calibrationFile).Data() , std::ifstream::in );
  if (!calib_file.is_open()) {
	  printf("BmnScWallRaw2Digit : Loading Calibration from file: %s - file open error!\n", calibrationFile.Data());
	  return;
  }
  printf("BmnScWallRaw2Digit : Loading Calibration from file: %s\n", calibrationFile.Data());
  po::store( po::parse_config_file( calib_file , desc ), vm );
  calib_file.close();
  po::notify( vm );

  std::string adc_ser;
  int adc_chan;
  float calibration;
  float calibError;

  for ( auto it : calibrations )
  {
    istringstream ss(it);
    ss >> adc_ser >> adc_chan >> calibration >> calibError;
    auto clibs = std::make_pair(calibration, calibError);

    UInt_t flat_channel = (UInt_t) GetFlatChannelFromAdcChannel( std::stoul(adc_ser, nullptr, 16), adc_chan );
    fCalibMap.insert(std::make_pair(flat_channel, clibs));
  }
}


Int_t BmnScWallRaw2Digit::GetFlatChannelFromAdcChannel(UInt_t adc_board_serial, UInt_t adc_ch) {
    auto it = find(fScWallSerials.begin(), fScWallSerials.end(), adc_board_serial);
    if (it != fScWallSerials.end()) {
      int adc_board_index = std::distance(fScWallSerials.begin(), it);
      return adc_board_index*100 + adc_ch;
    }
    return -1;
}





void BmnScWallRaw2Digit::fillEvent(TClonesArray *data, TClonesArray *ScWalldigit) {

	  printf("ScWall fillEvent\n");

    for (int i = 0; i < data->GetEntriesFast(); i++) {
      BmnADCDigit *digit = (BmnADCDigit*) data->At(i);
      // check if serial is from ScWall
      if ( std::find(fScWallSerials.begin(), fScWallSerials.end(), digit->GetSerial()) == fScWallSerials.end() )
        continue; 

      std::vector<float> wfm(digit->GetUShortValue(), digit->GetUShortValue() + digit->GetNSamples());
      BmnScWallDigi ThisDigi;
      map<UInt_t,UInt_t>::iterator it = fChannelMap.find(GetFlatChannelFromAdcChannel(digit->GetSerial(), digit->GetChannel()));
      if(it != fChannelMap.end())
        ThisDigi.fuAddress = it->second;
      else {
        printf("BmnScWallRaw2Digit :: Serial 0x%08x, Channel %0u Not found in map %s.\n", digit->GetSerial(), digit->GetChannel(), fmappingFileName.Data());
        continue;
      }
      if (ThisDigi.fuAddress == 0) continue; // not connected lines
      ProcessWfm(wfm, &ThisDigi);
      if (abs(ThisDigi.fSignal) < fdigiPars.threshold) continue;


      TClonesArray &ar_ScWall = *ScWalldigit;
      new(ar_ScWall[ScWalldigit->GetEntriesFast()]) BmnScWallDigi(ThisDigi);
    }
}

void BmnScWallRaw2Digit::ProcessWfm(std::vector<float> wfm, BmnScWallDigi* digi)
{
  if(fdigiPars.doInvert) {
    float myconstant{-1.0};
    std::transform(wfm.begin(), wfm.end(), wfm.begin(),
               std::bind1st(std::multiplies<float>(), myconstant));
  }

    //Zero level calculation
    const int n_gates = 3;
    int gate_npoints = (int)floor((fdigiPars.gateBegin-2.)/n_gates);

    Float_t gates_mean[n_gates], gates_rms[n_gates];
    for(Int_t igate = 0; igate < n_gates; igate++)
  	  MeanRMScalc(wfm, gates_mean+igate, gates_rms+igate, igate*gate_npoints, (igate+1)*gate_npoints);

    int best_gate = 0;
    for(Int_t igate = 0; igate < n_gates; igate++)
	  if(gates_rms[igate] < gates_rms[best_gate]) best_gate = igate;
    digi->fZL = gates_mean[best_gate];

    //MAX and Integral calculation
    float integral_in_gate = 0.0;
    float MAX_in_gate = wfm.at(fdigiPars.gateBegin);
    int time_MAX_in_gate = fdigiPars.gateBegin;

    for(int sample_curr = fdigiPars.gateBegin; sample_curr <= fdigiPars.gateEnd; sample_curr++) {
      float val_sample = wfm.at(sample_curr);
      integral_in_gate += val_sample-digi->fZL;
      if(val_sample > MAX_in_gate) {
        MAX_in_gate = val_sample;
        time_MAX_in_gate = sample_curr;
      }
    }
    MAX_in_gate -= digi->fZL;
  
    digi->fAmpl = MAX_in_gate;
    digi->fTimeMax = time_MAX_in_gate;
    digi->fIntegral = integral_in_gate;

    UInt_t adc_ser = fScWallSerials.at(BmnScWallAddress::GetADCidx(digi->fuAddress));
    UInt_t adc_chan = (BmnScWallAddress::GetADCch(digi->fuAddress));
    UInt_t flat_channel = (UInt_t) GetFlatChannelFromAdcChannel( adc_ser, adc_chan );

    float calibr = 0.0;
    std::map<UInt_t, std::pair<float,float>>::iterator it = fCalibMap.find(flat_channel);
    if(it != fCalibMap.end())
      calibr = it->second.first;
    else
      printf("BmnScWallRaw2Digit :: Serial 0x%08x, Channel %0u Not found in calibration file %s.\n", adc_ser, adc_chan, fcalibrationFileName.Data());
    
    if (fdigiPars.signalType == 0) 
      digi->fSignal = MAX_in_gate * calibr;
    if (fdigiPars.signalType == 1)
      digi->fSignal = integral_in_gate * calibr;

}

void BmnScWallRaw2Digit::MeanRMScalc(std::vector<float> wfm, float* Mean, float* RMS, int begin, int end, int step)
{
    begin = (begin < 0)? 0 : begin;

    if(begin > end){float swap=end;end=begin;begin=swap;};
    step = TMath::Abs(step);

    *Mean = *RMS = 0.; int Delta = 0;
    for(int n=begin; n<=end; n += step){ *Mean += wfm[n]; Delta++;}
    *Mean /= (float)Delta;

    for(int n=begin; n<=end; n += step) *RMS += (wfm[n] - *Mean) * (wfm[n] - *Mean);
    *RMS = TMath::Sqrt( *RMS/((float)Delta) );

    //printf("AMPL %.2f, RMS %.2f\n",*Mean,*RMS);
}

BmnScWallRaw2Digit::~BmnScWallRaw2Digit()
{
}

ClassImp(BmnScWallRaw2Digit)

