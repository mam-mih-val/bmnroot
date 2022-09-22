#include "TMath.h"
#include "TSystem.h"
#include "BmnNdetRaw2Digit.h"

void BmnNdetRaw2Digit::print()
{
  printf("BmnNdetRaw2Digit : \n");
}

BmnNdetRaw2Digit::BmnNdetRaw2Digit() : WfmProcessor()
{
  fPeriodId = 0;
  fRunId = 0;
}

BmnNdetRaw2Digit::BmnNdetRaw2Digit(int period, int run, TString mappingFile, TString CalibrationFile) : WfmProcessor()
{
  fPeriodId = period;
  fRunId = run;
  fmappingFileName = mappingFile;
  ParseConfig(fmappingFileName);
  fcalibrationFileName = CalibrationFile;
  ParseCalibration(fcalibrationFileName);
}

void BmnNdetRaw2Digit::ParseConfig(TString mappingFile)
{

  namespace po = boost::program_options;

  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/input/";

  typedef std::vector<std::string> vect_string_t;
  float version;
  std::string comment;
  std::vector<std::string> tqdc_serials;
  std::vector<std::string> configuration;

  // Setup options.
  po::options_description desc("Options");
  desc.add_options()
    ("VERSION.id", po::value<float>(&version), "version identificator")
    ("COMMENT.str", po::value<std::string>(&comment), "comment")
    ("TQDCSERIALS.serial", po::value<vect_string_t>(&tqdc_serials)->multitoken(), "tqdc serials")
    ("CONFIGURATION.config", po::value<vect_string_t>(&configuration)->multitoken(), "configuration")
    ;

  // Load config file.
  po::variables_map vm;
  std::ifstream config_file((path + mappingFile).Data(), std::ifstream::in);
  if (!config_file.is_open())
  {
    printf("BmnNdetRaw2Digit : Loading Config from file: %s - file open error!\n", mappingFile.Data());
    return;
  }
  printf("BmnNdetRaw2Digit : Loading Config from file: %s\n", mappingFile.Data());
  po::store(po::parse_config_file(config_file, desc), vm);
  config_file.close();
  po::notify(vm);

  fSerials.clear();
  for (auto it : tqdc_serials)
    fSerials.push_back(std::stoul(it, nullptr, 16));
  std::sort(fSerials.begin(), fSerials.end());

  std::string tqdc_ser;
  int tqdc_chan;
  int cell_id;
  int layer_id;
  short x_position;
  short y_position;
  short z_position;

  std::set<short> UniqueX;
  std::set<short> UniqueY;
  std::set<short> UniqueZ;
  // First pass for unique.
  for (auto it : configuration)
  {
    istringstream ss(it);
    ss >> tqdc_ser >> tqdc_chan >> cell_id >> layer_id >> x_position >> y_position >> z_position;
    assert(tqdc_chan < 16);
    UniqueX.insert(x_position);
    UniqueY.insert(y_position);
    UniqueZ.insert(z_position);
  }
  std::copy(UniqueX.begin(), UniqueX.end(), std::back_inserter(fUniqueX));
  std::copy(UniqueY.begin(), UniqueY.end(), std::back_inserter(fUniqueY));
  std::copy(UniqueZ.begin(), UniqueZ.end(), std::back_inserter(fUniqueZ));

  fChannelVect.clear();
  fChannelVect.resize(fSerials.size() * CHANNELS_PER_BOARD);
  // Second pass for mapping.
  for (auto it : configuration)
  {
    istringstream ss(it);
    ss >> tqdc_ser >> tqdc_chan >> cell_id >> layer_id >> x_position >> y_position >> z_position;
    int board_index, xIdx, yIdx, zIdx = -1;
    auto iter = find(fSerials.begin(), fSerials.end(), std::stoul(tqdc_ser, nullptr, 16));
    if (iter != fSerials.end())
      board_index = std::distance(fSerials.begin(), iter);
    else
      printf("BmnNdetRaw2Digit : unknown adc serial\n");

    xIdx = std::distance(UniqueX.begin(), UniqueX.find(x_position));
    yIdx = std::distance(UniqueY.begin(), UniqueY.find(y_position));
    zIdx = std::distance(UniqueZ.begin(), UniqueZ.find(z_position));

    unsigned int flat_channel = (unsigned int)GetFlatChannelFromAdcChannel(std::stoul(tqdc_ser, nullptr, 16), tqdc_chan);
    unsigned int unique_address = BmnNdetAddress::GetAddress(cell_id, layer_id, xIdx, yIdx, zIdx);
    fChannelVect.at(flat_channel) = unique_address;
  }
  //std::LOG(debug) << "COMMENT.str: " << comment << std::endl;
}

void BmnNdetRaw2Digit::ParseCalibration(TString calibrationFile)
{

  namespace po = boost::program_options;

  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/ndet/";

  typedef std::vector<std::string> vect_string_t;
  typedef std::vector<std::complex<float>> vect_complf_t;
  float version;
  std::string comment;
  std::vector<std::string> calibrations;

  // Setup options.
  po::options_description desc("Options");
  desc.add_options()
    ("VERSION.id", po::value<float>(&version), "version identificator")
    ("COMMENT.str", po::value<std::string>(&comment), "comment")
    ("CHECKER.isWriteWfm", po::value<bool>(&fdigiPars.isWriteWfm), "writing waveforms")
    ("PARAMETERS.gateBegin", po::value<int>(&fdigiPars.gateBegin), "digi parameters")
    ("PARAMETERS.gateEnd", po::value<int>(&fdigiPars.gateEnd), "digi parameters")
    ("PARAMETERS.threshold", po::value<float>(&fdigiPars.threshold), "digi parameters")
    ("PARAMETERS.signalType", po::value<int>(&fdigiPars.signalType), "digi parameters")
    ("PARAMETERS.doInvert", po::value<bool>(&fdigiPars.doInvert), "digi parameters")
    ("FITPARAMETERS.isfit", po::value<bool>(&fdigiPars.isfit), "digi parameters")
    ("FITPARAMETERS.harmonic", po::value<vect_complf_t>(&fdigiPars.harmonics)->multitoken(), "fit harmonics")
    ("CALIBRATION.calib", po::value<vect_string_t>(&calibrations)->multitoken(), "calibrations")
    ;

  // Load config file.
  po::variables_map vm;
  std::ifstream calib_file((path + calibrationFile).Data(), std::ifstream::in);
  if (!calib_file.is_open())
  {
    printf("BmnNdetRaw2Digit : Loading Calibration from file: %s - file open error!\n", calibrationFile.Data());
    return;
  }
  printf("BmnNdetRaw2Digit : Loading Calibration from file: %s\n", calibrationFile.Data());
  po::store(po::parse_config_file(calib_file, desc), vm);
  calib_file.close();
  po::notify(vm);

  int cell_id;
  int layer_id;
  float calibration;
  float calibError;
  fuoCalibMap.clear();
  for (auto it : calibrations)
  {
    istringstream ss(it);
    ss >> cell_id >> layer_id >> calibration >> calibError;
    auto key = std::make_pair(cell_id, layer_id);
    fuoCalibMap[key] = std::make_pair(calibration, calibError);
  }

  if(fdigiPars.isfit) {
    int model_order = fdigiPars.harmonics.size() + 1;
    fSignalLength = fdigiPars.gateEnd - fdigiPars.gateBegin + 1;
    fAZik = new std::complex<float> *[model_order];
    for (int i = 0; i < model_order; i++) {
      fAZik[i] = new std::complex<float>[model_order];
      for (int j = 0; j < model_order; j++)
        fAZik[i][j] = {0., 0.};
    }
    PsdSignalFitting::PronyFitter Pfitter;
    Pfitter.Initialize(fdigiPars.harmonics.size(), fdigiPars.harmonics.size(), fdigiPars.gateBegin, fdigiPars.gateEnd);
    Pfitter.SetExternalHarmonics(fdigiPars.harmonics);
    Pfitter.MakeInvHarmoMatrix(fSignalLength, fAZik);
  }
}

std::pair<float,float> BmnNdetRaw2Digit::GetCalibPairFromAddress(unsigned int address)
{
  auto cell_id = BmnNdetAddress::GetCellId(address);
  auto layer_id = BmnNdetAddress::GetLayerId(address);
  std::pair<int,int> key = std::make_pair(cell_id, layer_id);
  if (fuoCalibMap.find(key) != fuoCalibMap.end())
    return fuoCalibMap.at(key);
  
  return std::make_pair(0.0, 0.0);
}

int BmnNdetRaw2Digit::GetFlatChannelFromAdcChannel(unsigned int board_serial, unsigned int channel)
{
  auto it = find(fSerials.begin(), fSerials.end(), board_serial);
  if (it != fSerials.end())
  {
    int board_index = std::distance(fSerials.begin(), it);
    return board_index * CHANNELS_PER_BOARD + channel;
  }

  printf("BmnNdetRaw2Digit :: Serial 0x%08x Not found in map %s.\n", board_serial, fmappingFileName.Data());
  return -1;
}

void BmnNdetRaw2Digit::fillEvent(TClonesArray *tdc, TClonesArray *adc, TClonesArray *Ndetdigit)
{

  LOG(debug) << "BmnNdetRaw2Digit::fillEvent" << endl;

  for (int iAdc = 0; iAdc < adc->GetEntriesFast(); iAdc++)
  {
    BmnTQDCADCDigit *adcDig = (BmnTQDCADCDigit*) adc->At(iAdc);
    //    Double_t adcTimestamp = adcDig->GetAdcTimestamp() * ADC_CLOCK_TQDC16VS;
    //    Double_t trgTimestamp = adcDig->GetTrigTimestamp() * ADC_CLOCK_TQDC16VS;

    // check if serial is from Ndet
    // cout<<adcDig->GetSerial() << " " << adcDig->GetChannel() << endl;
    if (std::find(fSerials.begin(), fSerials.end(), adcDig->GetSerial()) == fSerials.end()) {
      LOG(debug) << "BmnNdetRaw2Digit::fillEvent" << std::hex << adcDig->GetSerial() << " Not found in ";
      for (auto it : fSerials)
        LOG(debug) << "BmnNdetRaw2Digit::fSerials " << std::hex << it << endl;
      continue;
    }

    std::vector<float> wfm(adcDig->GetShortValue(), adcDig->GetShortValue() + adcDig->GetNSamples());
    BmnNdetDigi ThisDigi;
    ThisDigi.reset();
    unsigned int flat_channel = (unsigned int)GetFlatChannelFromAdcChannel(adcDig->GetSerial(), adcDig->GetChannel());
    assert(flat_channel < fChannelVect.size());
    ThisDigi.fuAddress = fChannelVect.at(flat_channel);
    if (ThisDigi.fuAddress == 0)
      continue; // not connected lines
    ProcessWfm(wfm, &ThisDigi);

    //Apply calibration
    LOG(debug) << "BmnNdetRaw2Digit::ProcessWfm  Calibration" << endl;
    auto calib_pair = GetCalibPairFromAddress(ThisDigi.GetAddress());
    if (fdigiPars.signalType == 0)
      ThisDigi.fSignal = (float) ThisDigi.fAmpl * calib_pair.first;
    if (fdigiPars.signalType == 1)
      ThisDigi.fSignal = (float) ThisDigi.fIntegral * calib_pair.first;
    if (abs(ThisDigi.fSignal) < fdigiPars.threshold)
      continue;

    TClonesArray &ar_Ndet = *Ndetdigit;
    new (ar_Ndet[Ndetdigit->GetEntriesFast()]) BmnNdetDigi(ThisDigi);
  }
}

BmnNdetRaw2Digit::~BmnNdetRaw2Digit()
{
}

ClassImp(BmnNdetRaw2Digit)
