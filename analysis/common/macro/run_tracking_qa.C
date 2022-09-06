/* Copyright (C) 2021 Physikalisches Institut, Eberhard Karls Universitaet Tuebingen, Tuebingen
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov [committer], Viktor Klochkov [committer] */

/** @brief run_analysistree_qa
 ** @param filelist    Filefist (text file) of input AnalysisTree files
 ** @param is_single_file  if true, instead of filelist a single ROOT file will be used as input
 **
 ** Macro to run AnalysisTreeQA package (https://github.com/HeavyIonAnalysis/AnalysisTreeQA)
 ** Produces an output ROOT file with specified histograms / TProfiles.
 ** Examples how to add plots could be found here:
 ** https://github.com/HeavyIonAnalysis/AnalysisTreeQA/blob/master/examples/example.cpp
 ** To add event cuts:
 ** task->SetEventCuts(EventCuts);
 ** where EventCuts is AnalysisTree::Cuts* object, for example
 ** GetCbmEventCuts("RecEventHeader") from macro/analysis/common/cuts/cbm_cuts.h
 ** To apply a cut on some branch, for example select only primiry MC-tracks:
 ** task->AddBranchCut(GetCbmMcTracksCuts("SimParticles"));
 ** or apply quality cuts on STS tracks:
 ** task->AddBranchCut(GetCbmTrackCuts("RecTracks"));
 **/

using namespace AnalysisTree;

void VertexTracksQA(QA::Task& task, std::string branch=std::string("GlobalTracks"), Cuts* cuts=nullptr, double y_beam=1.0);
void TofHitsQA(QA::Task& task);
void SimParticlesQA(QA::Task& task, Cuts* cuts=nullptr, double y_beam=1.0);
void SimEventHeaderQA(QA::Task& task);
void RecEventHeaderQA(QA::Task& task);
void EfficiencyMaps(QA::Task& task);
void TrackingQA(QA::Task& task);
void FHCalModulesQA(QA::Task& task);

const int kPdgLambda = 10000000;
const int kPdgCharge = 10000;
const int kPdgMass   = 10;

Int_t GetIonCharge(int pdgCode) { return (pdgCode % kPdgLambda) / kPdgCharge; }

void run_tracking_qa(std::string filelist, std::string output_file="tracking_qa.root", bool is_single_file = false, double y_beam = 1.0);

const std::string sim_event_header = "SimEventHeader";
const std::string rec_event_header = "RecEventHeader";
const std::string sim_particles    = "SimParticles";
const std::string rec_tracks       = "GlobalTracks";
const std::string sts_tracks       = "StsTracks";
const std::string tof400_hits         = "Tof400Hits";
const std::string tof700_hits         = "Tof700Hits";
const std::string trd_tracks       = "TrdTracks";
const std::string rich_rings       = "RichRings";
const std::string fhcal_modules       = "FHCalModules";

void run_tracking_qa(std::string filelist, std::string output_file, bool is_single_file, double y_beam)
{
  if (is_single_file) {
    std::ofstream fl("fl_temp.txt");
    fl << filelist << "\n";
    fl.close();
    filelist = "fl_temp.txt";
  }

  TaskManager* man = TaskManager::GetInstance();

  auto* task = new QA::Task;
  task->SetOutputFileName(output_file);

  VertexTracksQA(*task, rec_tracks, nullptr, y_beam);
  VertexTracksQA(*task, rec_tracks, new Cuts("primary", {EqualsCut({sim_particles + ".mother_id"}, -1)}), y_beam);
  VertexTracksQA(*task, rec_tracks, new Cuts("secondary", {RangeCut({sim_particles + ".mother_id"}, 0., 999.)}), y_beam);
  SimParticlesQA(*task, new Cuts("primary", {EqualsCut({sim_particles + ".mother_id"}, -1)}), y_beam);
  SimParticlesQA( *task, new Cuts("secondary", {RangeCut({sim_particles + ".mother_id"}, 0., 999.)}), y_beam);
  std::vector pid_codes{2212, 211, -211};
  for (auto pid : pid_codes) {
    auto* particle_cut = new Cuts(std::to_string( pid ), {EqualsCut({sim_particles + ".pid"}, pid)});
    auto* tof400_cut = new Cuts(std::to_string( pid )+"_tof400", {
                                                                     EqualsCut({sim_particles + ".pid"}, pid),
                                                                     EqualsCut({sim_particles + ".mother_id"}, -1),
                                                                     RangeCut({rec_tracks + ".beta400"}, 0., 999.)
                                                                 });
    auto* tof700_cut = new Cuts(std::to_string( pid )+"_tof700", {
                                                                     EqualsCut({sim_particles + ".pid"}, pid),
                                                                     EqualsCut({sim_particles + ".mother_id"}, -1),
                                                                     RangeCut({rec_tracks + ".beta700"}, 0., 999.)
                                                                 });
    auto* not_tof700_cut = new Cuts(std::to_string( pid )+"_!tof700", {
                                                                          EqualsCut({sim_particles + ".pid"}, pid),
                                                                          EqualsCut({sim_particles + ".mother_id"}, -1),
                                                                          RangeCut({rec_tracks + ".beta700"}, -1e9, -0.)
                                                                      });
    auto* tof_cut = new Cuts(std::to_string( pid )+"_tof", {
                                                               EqualsCut({sim_particles + ".pid"}, pid),
                                                               EqualsCut({sim_particles + ".mother_id"}, -1),
                                                               SimpleCut({rec_tracks + ".beta400",
                                                                       rec_tracks + ".beta700"},
                                                                       [](std::vector<double> betas){
                                                                         return betas[0] > 0. || betas[1] > 0.;
                                                                       })
                                                           });
    auto* midrap_cut = new Cuts(std::to_string( pid )+"_midrap", {
                                                                     EqualsCut({sim_particles + ".pid"}, pid),
                                                                     EqualsCut({sim_particles + ".mother_id"}, -1),
                                                                     RangeCut({sim_particles + ".rapidity"}, -0.5 + y_beam, 0.5 + y_beam)
                                                                 });
    auto* midrap_lo_pT_cut = new Cuts(std::to_string( pid )+"_midrap_lo_pT", {
                                                                     EqualsCut({sim_particles + ".pid"}, pid),
                                                                     EqualsCut({sim_particles + ".mother_id"}, -1),
                                                                     RangeCut({sim_particles + ".rapidity"}, -0.5 + y_beam, 0.5 + y_beam),
                                                                     RangeCut({rec_tracks + ".pT"}, 0.0, 0.5)
                                                                 });
    VertexTracksQA(*task, rec_tracks, particle_cut, y_beam);
    VertexTracksQA(*task, rec_tracks, tof400_cut, y_beam);
    VertexTracksQA(*task, rec_tracks, tof700_cut, y_beam);
    VertexTracksQA(*task, rec_tracks, not_tof700_cut, y_beam);
    VertexTracksQA(*task, rec_tracks, tof_cut, y_beam);
    VertexTracksQA(*task, rec_tracks, midrap_cut, y_beam);
    VertexTracksQA(*task, rec_tracks, midrap_lo_pT_cut, y_beam);
    SimParticlesQA(*task, particle_cut, y_beam);
  }
  man->AddTask(task);

  man->Init({filelist}, {"rTree"});
  man->Run(-1);
  man->Finish();

  if (is_single_file) {
    // -----   Finish   -------------------------------------------------------
    std::cout << " Test passed" << std::endl;
    std::cout << " All ok " << std::endl;
  }
}

void VertexTracksQA(QA::Task& task, std::string branch, Cuts* cuts, double y_beam)
{
  Variable chi2_over_ndf("chi2_ndf", {{branch, "chi2"}, {branch, "ndf"}},
                         [](std::vector<double>& var) { return var.at(0) / var.at(1); });

  Variable y_cm("y_cm", {{branch, "pz"}, {branch, "p"}, {sim_particles, "mass"}},
                         [y_beam](std::vector<double>& var) {
                                                        double E = sqrt( var.at(1)*var.at(1) + var.at(2)*var.at(2) );
                                                        double pz = var.at(0);
                                                        return 0.5 * (
                                                                 log( E + pz ) -
                                                                 log( E - pz ) ) - y_beam;
                 });
  Variable momentum_resolution("momentum_resolution", {{branch, "p"}, {sim_particles, "p"}},
                         [](std::vector<double>& var) {
                                 auto res = fabs(var.at(0) - var.at(1)) / var.at(1);
                                 if( res < 100.0 )
                                  return res;
                                 else return 100.;
                 });
  Variable px_resolution("px_resolution", {{branch, "px"}, {sim_particles, "px"}},
                         [](std::vector<double>& var) {
                           auto res = fabs(var.at(0) - var.at(1)) / var.at(1);
                           if( res < 100.0 )
                             return res;
                           else return 100.;
                 });
  Variable py_resolution("py_resolution", {{branch, "py"}, {sim_particles, "py"}},
                         [](std::vector<double>& var) {
                           auto res = fabs(var.at(0) - var.at(1)) / var.at(1);
                           if( res < 100.0 )
                             return res;
                           else return 100.;
                 });
  Variable pz_resolution("pz_resolution", {{branch, "pz"}, {sim_particles, "pz"}},
                         [](std::vector<double>& var) {
                           auto res = fabs(var.at(0) - var.at(1)) / var.at(1);
                           if( res < 100.0 )
                             return res;
                           else return 100.;
                 });

  task.AddH1({"y_{cm}", y_cm, {100, -1, 3}}, cuts);
  task.AddH1({"DCA_{x}, cm", {branch, "dcax"}, {QA::gNbins, -5, 5}}, cuts);
  task.AddH1({"DCA_{y}, cm", {branch, "dcay"}, {QA::gNbins, -5, 5}}, cuts);
  task.AddH1({"DCA_{z}, cm", {branch, "dcaz"}, {QA::gNbins, -5, 5}}, cuts);
  task.AddH1({"z_{extr}, cm", {branch, "z_first"}, {QA::gNbins, -5, 5}}, cuts);

  task.AddH1({"NDF", {branch, "ndf"}, {30, 0, 30}}, cuts);
  task.AddH1({"N_{hits}", {branch, "n_hits"}, {30, 0, 30}}, cuts);
  task.AddH1({"#chi^{2}/NDF", chi2_over_ndf, {QA::gNbins, 0, 100}}, cuts);
  task.AddH2({"DCA_{x}, cm", {branch, "dcax"}, {QA::gNbins, -10, 10}},
             {"DCA_{y}, cm", {branch, "dcay"}, {QA::gNbins, -10, 10}}, cuts);

  task.AddH2({"X at z=450, cm", {branch, "x400"}, {250, -500, 500}},
             {"Y at z=450, cm", {branch, "y400"}, {250, -500, 500}}, cuts);



  task.AddH2({"y_{cm}", y_cm, {50, -1, 3}},
             {"p_{T} (GeV/c)", {branch, "pT"}, {50, 0, 3}}, cuts);

  task.AddProfile({"p_{sim} (GeV/c)", {sim_particles, "p"}, {250, 0.0, 5.0}}, {"res (%)", momentum_resolution, {}}, cuts);
  task.AddProfile({"p_{T} (GeV/c)", {sim_particles, "pT"}, {150, 0.0, 3.0}}, {"res (%)", momentum_resolution, {}}, cuts);
  task.AddProfile({"#eta", {sim_particles, "eta"}, {250, 0.0, 5.0}}, {"res (%)", momentum_resolution, {}}, cuts);

  task.AddProfile({"p_{x} (GeV/c)", {sim_particles, "px"}, {250, 0.0, 5.0}}, {"res (%)", px_resolution, {}}, cuts);
  task.AddProfile({"p_{y} (GeV/c)", {sim_particles, "py"}, {250, 0.0, 5.0}}, {"res (%)", py_resolution, {}}, cuts);
  task.AddProfile({"p_{z} (GeV/c)", {sim_particles, "pz"}, {250, 0.0, 5.0}}, {"res (%)", pz_resolution, {}}, cuts);
}
void SimParticlesQA(QA::Task& task, Cuts* cuts=nullptr, double y_beam){
  Variable y_cm("y_cm", { {sim_particles, "rapidity"} },
                 [y_beam](std::vector<double>& var) {
                   double y_lab = var.at(0);
                   return y_lab - y_beam;
                 });

  task.AddH2({"y_{cm}", y_cm, {50, -1, 3}},
             {"p_{T} (GeV/c)", {sim_particles, "pT"}, {50, 0, 3}}, cuts);
}

int main(int argc, char** argv)
{
  if (argc <= 1) {
    std::cout << "Not enough arguments! Please use:" << std::endl;
    std::cout << "   ./cbm_qa filelist" << std::endl;
    return -1;
  }

  const std::string filelist = argv[1];
  run_tracking_qa(filelist);
  return 0;
}
