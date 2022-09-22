#include "TMath.h"
#include "TSystem.h"
#include "BmnFHCalRaw2Digit.h"

void BmnFHCalRaw2Digit::print()
{
  printf("BmnFHCalRaw2Digit : \n");
}

BmnFHCalRaw2Digit::BmnFHCalRaw2Digit() : WfmProcessor()
{
  fPeriodId = 0;
  fRunId = 0;
}

BmnFHCalRaw2Digit::BmnFHCalRaw2Digit(int period, int run, TString mappingFile, TString CalibrationFile) : WfmProcessor()
{
  fPeriodId = period;
  fRunId = run;
  fmappingFileName = mappingFile;
  ParseConfig(fmappingFileName);
  fcalibrationFileName = CalibrationFile;
  ParseCalibration(fcalibrationFileName);
}

void BmnFHCalRaw2Digit::ParseConfig(TString mappingFile)
{

  namespace po = boost::program_options;

  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/input/";

  typedef std::vector<std::string> vect_string_t;
  float version;
  std::string comment;
  std::vector<std::string> adc_serials;
  std::vector<std::string> configuration;

  // Setup options.
  po::options_description desc("Options");
  desc.add_options()
    ("VERSION.id", po::value<float>(&version), "version identificator")
    ("COMMENT.str", po::value<std::string>(&comment), "comment")
    ("ADCSERIALS.serial", po::value<vect_string_t>(&adc_serials)->multitoken(), "adc serials")
    ("CONFIGURATION.config", po::value<vect_string_t>(&configuration)->multitoken(), "configuration")
    ;

  // Load config file.
  po::variables_map vm;
  std::ifstream config_file((path + mappingFile).Data(), std::ifstream::in);
  if (!config_file.is_open())
  {
    printf("BmnFHCalRaw2Digit : Loading Config from file: %s - file open error!\n", mappingFile.Data());
    return;
  }
  printf("BmnFHCalRaw2Digit : Loading Config from file: %s\n", mappingFile.Data());
  po::store(po::parse_config_file(config_file, desc), vm);
  config_file.close();
  po::notify(vm);

  fSerials.clear();
  for (auto it : adc_serials)
    fSerials.push_back(std::stoul(it, nullptr, 16));
  std::sort(fSerials.begin(), fSerials.end());

  std::string adc_ser;
  int adc_chan;
  int module_type;
  int module_id;
  int section_id;
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
    ss >> adc_ser >> adc_chan >> module_type >> module_id >> section_id >> x_position >> y_position >> z_position;
    assert(module_type <= 2);
    assert(section_id <= 10);
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
    ss >> adc_ser >> adc_chan >> module_type >> module_id >> section_id >> x_position >> y_position >> z_position;
    int adc_board_index, xIdx, yIdx, zIdx = -1;
    auto iter = find(fSerials.begin(), fSerials.end(), std::stoul(adc_ser, nullptr, 16));
    if (iter != fSerials.end())
      adc_board_index = std::distance(fSerials.begin(), iter);
    else
      printf("BmnFHCalRaw2Digit : unknown adc serial\n");

    xIdx = std::distance(UniqueX.begin(), UniqueX.find(x_position));
    yIdx = std::distance(UniqueY.begin(), UniqueY.find(y_position));
    zIdx = std::distance(UniqueZ.begin(), UniqueZ.find(z_position));

    unsigned int flat_channel = (unsigned int)GetFlatChannelFromAdcChannel(std::stoul(adc_ser, nullptr, 16), adc_chan);
    unsigned int unique_address = (module_type == 0) ? 0 : BmnFHCalAddress::GetAddress(module_type, module_id, section_id, xIdx, yIdx, zIdx);
    fChannelVect.at(flat_channel) = unique_address;
  }
  //std::LOG(debug) << "COMMENT.str: " << comment << std::endl;
}

void BmnFHCalRaw2Digit::ParseCalibration(TString calibrationFile)
{

  namespace po = boost::program_options;

  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/fhcal/";

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
    printf("BmnFHCalRaw2Digit : Loading Calibration from file: %s - file open error!\n", calibrationFile.Data());
    return;
  }
  printf("BmnFHCalRaw2Digit : Loading Calibration from file: %s\n", calibrationFile.Data());
  po::store(po::parse_config_file(calib_file, desc), vm);
  calib_file.close();
  po::notify(vm);

  int mod_id;
  int sec_id;
  float calibration;
  float calibError;
  int max_mod_id = 0;
  // First pass for max mod id.
  for (auto it : calibrations)
  {
    istringstream ss(it);
    ss >> mod_id >> sec_id >> calibration >> calibError;
    if (mod_id > max_mod_id) max_mod_id = mod_id;
  }
  fCalibVect.clear();
  fCalibVect.resize(GetFlatIndex(max_mod_id, 10) + 1);

  // Second pass for calibrations.
  for (auto it : calibrations)
  {
    istringstream ss(it);
    ss >> mod_id >> sec_id >> calibration >> calibError;
    fCalibVect.at(GetFlatIndex(mod_id, sec_id)) = std::make_pair(calibration, calibError);
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

std::pair<float,float> BmnFHCalRaw2Digit::GetCalibPairFromAddress(unsigned int address)
{
  auto mod_id = BmnFHCalAddress::GetModuleId(address);
  auto sec_id = BmnFHCalAddress::GetSectionId(address);
  return fCalibVect.at(GetFlatIndex(mod_id, sec_id));
}

int BmnFHCalRaw2Digit::GetFlatChannelFromAdcChannel(unsigned int board_serial, unsigned int channel)
{
  auto it = find(fSerials.begin(), fSerials.end(), board_serial);
  if (it != fSerials.end())
  {
    int board_index = std::distance(fSerials.begin(), it);
    return board_index * CHANNELS_PER_BOARD + channel;
  }

  printf("BmnFHCalRaw2Digit :: Serial 0x%08x Not found in map %s.\n", board_serial, fmappingFileName.Data());
  return -1;
}

int BmnFHCalRaw2Digit::GetFlatIndex(int mod_id, int sec_id)
{
  return mod_id*10 + sec_id - 1;
}

void BmnFHCalRaw2Digit::fillEvent(TClonesArray *data, TClonesArray *FHCaldigit)
{
  LOG(debug) << "BmnFHCalRaw2Digit::fillEvent" << endl;

  for (int i = 0; i < data->GetEntriesFast(); i++)
  {
    BmnADCDigit *digit = (BmnADCDigit *)data->At(i);
    // check if serial is from FHCal
    // cout<<digit->GetSerial() << " " << digit->GetChannel() << endl;
    if (std::find(fSerials.begin(), fSerials.end(), digit->GetSerial()) == fSerials.end()) {
      LOG(debug) << "BmnFHCalRaw2Digit::fillEvent" << std::hex << digit->GetSerial() << " Not found in ";
      for (auto it : fSerials)
        LOG(debug) << "BmnFHCalRaw2Digit::fSerials " << std::hex << it << endl;
      continue;
    }
    
    std::vector<float> wfm((short*) digit->GetUShortValue(), (short*) digit->GetUShortValue()+digit->GetNSamples());
    BmnFHCalDigi ThisDigi;
    ThisDigi.reset();
    unsigned int flat_channel = (unsigned int)GetFlatChannelFromAdcChannel(digit->GetSerial(), digit->GetChannel());
    assert(flat_channel < fChannelVect.size());
    ThisDigi.fuAddress = fChannelVect.at(flat_channel);
    if (ThisDigi.fuAddress == 0)
      continue; // not connected lines
    ProcessWfm(wfm, &ThisDigi);

    //Apply calibration
    LOG(debug) << "BmnFHCalRaw2Digit::ProcessWfm  Calibration" << endl;
    int mod_id = ThisDigi.GetModuleId();
    int sec_id = ThisDigi.GetSectionId();
    int flat_index = GetFlatIndex(mod_id, sec_id);
    assert(flat_index < fCalibVect.size());
    if (fdigiPars.signalType == 0)
      ThisDigi.fSignal = (float) ThisDigi.fAmpl * fCalibVect.at(flat_index).first;
    if (fdigiPars.signalType == 1)
      ThisDigi.fSignal = (float) ThisDigi.fIntegral * fCalibVect.at(flat_index).first;
    if (abs(ThisDigi.fSignal) < fdigiPars.threshold)
      continue;

    TClonesArray &ar_FHCal = *FHCaldigit;
    new (ar_FHCal[FHCaldigit->GetEntriesFast()]) BmnFHCalDigi(ThisDigi);
  }
}

BmnFHCalRaw2Digit::~BmnFHCalRaw2Digit()
{
}

ClassImp(BmnFHCalRaw2Digit)
