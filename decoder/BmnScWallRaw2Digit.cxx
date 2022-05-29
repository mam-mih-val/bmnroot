#include "TMath.h"
#include "TSystem.h"
#include "BmnScWallRaw2Digit.h"

void BmnScWallRaw2Digit::print()
{
  printf("BmnScWallRaw2Digit : \n");
}

BmnScWallRaw2Digit::BmnScWallRaw2Digit() : WfmProcessor()
{
  fPeriodId = 0;
  fRunId = 0;
}

BmnScWallRaw2Digit::BmnScWallRaw2Digit(int period, int run, TString mappingFile, TString CalibrationFile) : WfmProcessor()
{
  fPeriodId = period;
  fRunId = run;
  fmappingFileName = mappingFile;
  ParseConfig(fmappingFileName);
  fcalibrationFileName = CalibrationFile;
  ParseCalibration(fcalibrationFileName);
}

void BmnScWallRaw2Digit::ParseConfig(TString mappingFile)
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
    printf("BmnScWallRaw2Digit : Loading Config from file: %s - file open error!\n", mappingFile.Data());
    return;
  }
  printf("BmnScWallRaw2Digit : Loading Config from file: %s\n", mappingFile.Data());
  po::store(po::parse_config_file(config_file, desc), vm);
  config_file.close();
  po::notify(vm);

  fSerials.clear();
  for (auto it : adc_serials)
    fSerials.push_back(std::stoul(it, nullptr, 16));
  std::sort(fSerials.begin(), fSerials.end());

  std::string adc_ser;
  int adc_chan;
  int cell_id;
  std::string zone;
  short x_position;
  short y_position;
  short size;

  std::set<short> UniqueX;
  std::set<short> UniqueY;
  std::set<short> UniqueSize;
  // First pass for unique.
  for (auto it : configuration)
  {
    istringstream ss(it);
    ss >> adc_ser >> adc_chan >> cell_id >> zone >> x_position >> y_position >> size;
    UniqueX.insert(x_position);
    UniqueY.insert(y_position);
    UniqueSize.insert(size);
  }
  std::copy(UniqueX.begin(), UniqueX.end(), std::back_inserter(fUniqueX));
  std::copy(UniqueY.begin(), UniqueY.end(), std::back_inserter(fUniqueY));
  std::copy(UniqueSize.begin(), UniqueSize.end(), std::back_inserter(fUniqueSize));

  fChannelVect.clear();
  fChannelVect.resize(fSerials.size() * CHANNELS_PER_BOARD);
  // Second pass for mapping.
  for (auto it : configuration)
  {
    istringstream ss(it);
    ss >> adc_ser >> adc_chan >> cell_id >> zone >> x_position >> y_position >> size;
    int adc_board_index, xIdx, yIdx, SizeIdx, ZoneIdx = -1;
    auto iter = find(fSerials.begin(), fSerials.end(), std::stoul(adc_ser, nullptr, 16));
    if (iter != fSerials.end())
      adc_board_index = std::distance(fSerials.begin(), iter);
    else
      printf("BmnScWallRaw2Digit : unknown adc serial\n");

    xIdx = std::distance(UniqueX.begin(), UniqueX.find(x_position));
    yIdx = std::distance(UniqueY.begin(), UniqueY.find(y_position));
    SizeIdx = std::distance(UniqueSize.begin(), UniqueSize.find(size));

    int last_letter = 'V' - 'A' + 1;
    ZoneIdx = (int)(zone[0] - 'A' + 1);
    if (ZoneIdx > last_letter)
      LOG(debug) << "MAX zone letter is " << last_letter << endl;

    unsigned int flat_channel = (unsigned int)GetFlatChannelFromAdcChannel(std::stoul(adc_ser, nullptr, 16), adc_chan);
    unsigned int unique_address = (ZoneIdx > last_letter) ? 0 : BmnScWallAddress::GetAddress(cell_id, xIdx, yIdx, SizeIdx, ZoneIdx);
    fChannelVect.at(flat_channel) = unique_address;
  }
  //std::LOG(debug) << "COMMENT.str: " << comment << std::endl;
}

void BmnScWallRaw2Digit::ParseCalibration(TString calibrationFile)
{

  namespace po = boost::program_options;

  TString dir = getenv("VMCWORKDIR");
  TString path = dir + "/parameters/scwall/";

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
    printf("BmnScWallRaw2Digit : Loading Calibration from file: %s - file open error!\n", calibrationFile.Data());
    return;
  }
  printf("BmnScWallRaw2Digit : Loading Calibration from file: %s\n", calibrationFile.Data());
  po::store(po::parse_config_file(calib_file, desc), vm);
  calib_file.close();
  po::notify(vm);

  int cell_id;
  float calibration;
  float calibError;
  int max_cell_id = 0;
  // First pass for max cell id.
  for (auto it : calibrations)
  {
    istringstream ss(it);
    ss >> cell_id >> calibration >> calibError;
    if (cell_id > max_cell_id) max_cell_id = cell_id;
  }
  fCalibVect.clear();
  fCalibVect.resize(max_cell_id+1);

  // Second pass for calibrations.
  for (auto it : calibrations)
  {
    istringstream ss(it);
    ss >> cell_id >> calibration >> calibError;
    fCalibVect.at(cell_id) = std::make_pair(calibration, calibError);
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

std::pair<float,float> BmnScWallRaw2Digit::GetCalibPairFromAddress(unsigned int address)
{
  auto cell_id = BmnScWallAddress::GetCellId(address);
  return fCalibVect.at(cell_id);
}

int BmnScWallRaw2Digit::GetFlatChannelFromAdcChannel(unsigned int board_serial, unsigned int channel)
{
  auto it = find(fSerials.begin(), fSerials.end(), board_serial);
  if (it != fSerials.end())
  {
    int adc_board_index = std::distance(fSerials.begin(), it);
    return adc_board_index * CHANNELS_PER_BOARD + channel;
  }

  printf("BmnScWallRaw2Digit :: Serial 0x%08x Not found in map %s.\n", board_serial, fmappingFileName.Data());
  return -1;
}

void BmnScWallRaw2Digit::fillEvent(TClonesArray *data, TClonesArray *ScWalldigit)
{

  LOG(debug) << "BmnScWallRaw2Digit::fillEvent" << endl;

  for (int i = 0; i < data->GetEntriesFast(); i++)
  {
    BmnADCDigit *digit = (BmnADCDigit *)data->At(i);
    // check if serial is from ScWall
    // cout<<digit->GetSerial() << " " << digit->GetChannel() << endl;
    if (std::find(fSerials.begin(), fSerials.end(), digit->GetSerial()) == fSerials.end()) {
      LOG(debug) << "BmnScWallRaw2Digit::fillEvent" << std::hex << digit->GetSerial() << " Not found in ";
      for (auto it : fSerials)
        LOG(debug) << "BmnScWallRaw2Digit::fSerials " << std::hex << it << endl;
      continue;
    }
    std::vector<float> wfm((short*) digit->GetUShortValue(), (short*) digit->GetUShortValue()+digit->GetNSamples());
    BmnScWallDigi ThisDigi;
    ThisDigi.reset();
    unsigned int flat_channel = (unsigned int)GetFlatChannelFromAdcChannel(digit->GetSerial(), digit->GetChannel());
    assert(flat_channel < fChannelVect.size());
    ThisDigi.fuAddress = fChannelVect.at(flat_channel);
    if (ThisDigi.fuAddress == 0)
      continue; // not connected lines
    ProcessWfm(wfm, &ThisDigi);
    
    //Apply calibration
    LOG(debug) << "BmnScWallRaw2Digit::ProcessWfm  Calibration" << endl;
    unsigned int cell_id = ThisDigi.GetCellId();
    assert(cell_id < fCalibVect.size());
    if (fdigiPars.signalType == 0)
      ThisDigi.fSignal = (float) ThisDigi.fAmpl * fCalibVect.at(cell_id).first;
    if (fdigiPars.signalType == 1)
      ThisDigi.fSignal = (float) ThisDigi.fIntegral * fCalibVect.at(cell_id).first;
    if (abs(ThisDigi.fSignal) < fdigiPars.threshold)
      continue;

    TClonesArray &ar_ScWall = *ScWalldigit;
    new (ar_ScWall[ScWalldigit->GetEntriesFast()]) BmnScWallDigi(ThisDigi);
  }
}

BmnScWallRaw2Digit::~BmnScWallRaw2Digit()
{
}

ClassImp(BmnScWallRaw2Digit)
