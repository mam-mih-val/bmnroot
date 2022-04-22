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

void VertexTracksQA(QA::Task& task, std::string branch=std::string("GlobalTracks"), Cuts* cuts=nullptr);
void TofHitsQA(QA::Task& task);
void SimParticlesQA(QA::Task& task, Cuts* cuts=nullptr);
void SimEventHeaderQA(QA::Task& task);
void RecEventHeaderQA(QA::Task& task);
void EfficiencyMaps(QA::Task& task);
void TrackingQA(QA::Task& task);
void FHCalModulesQA(QA::Task& task);

const int kPdgLambda = 10000000;
const int kPdgCharge = 10000;
const int kPdgMass   = 10;

Int_t GetIonCharge(int pdgCode) { return (pdgCode % kPdgLambda) / kPdgCharge; }

void run_tracking_qa(std::string filelist, bool is_single_file = false);

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

void run_tracking_qa(std::string filelist, bool is_single_file)
{
  if (is_single_file) {
    std::ofstream fl("fl_temp.txt");
    fl << filelist << "\n";
    fl.close();
    filelist = "fl_temp.txt";
  }

  TaskManager* man = TaskManager::GetInstance();

  auto* task = new QA::Task;
  task->SetOutputFileName("cbm_qa.root");

  VertexTracksQA(*task, rec_tracks);
  VertexTracksQA(*task, rec_tracks, new Cuts("rec_primary", {EqualsCut({sim_particles + ".mother_id"}, -1)}));
  VertexTracksQA(*task, rec_tracks, new Cuts("rec_secondary", {RangeCut({sim_particles + ".mother_id"}, 0., 999.)}));
  VertexTracksQA(*task, rec_tracks, new Cuts("rec_protons", {EqualsCut({sim_particles + ".mother_id"}, -1),
                                             EqualsCut({sim_particles + ".pid"}, 2212)}));
  VertexTracksQA(*task, rec_tracks, new Cuts("rec_pi_neg", {EqualsCut({sim_particles + ".mother_id"}, -1),
                                            EqualsCut({sim_particles + ".pid"}, 211)}));
  VertexTracksQA(*task, rec_tracks, new Cuts("rec_pi_pos", {EqualsCut({sim_particles + ".mother_id"}, -1),
                                            EqualsCut({sim_particles + ".pid"}, -211)}));
  VertexTracksQA(*task, rec_tracks, new Cuts("rec_deuteron", {EqualsCut({sim_particles + ".mother_id"}, -1),
                                             RangeCut({sim_particles + ".pid"}, 1000010020-0.5, 1000010020+0.5)}));

  SimParticlesQA(*task, new Cuts("sim_primary", {EqualsCut({sim_particles + ".mother_id"}, -1)}));
  SimParticlesQA(*task, new Cuts("sim_secondary", {RangeCut({sim_particles + ".mother_id"}, 0., 999.)}));
  SimParticlesQA(*task, new Cuts("sim_protons", {EqualsCut({sim_particles + ".mother_id"}, -1),
                                                 EqualsCut({sim_particles + ".pid"}, 2212)}));
  SimParticlesQA(*task, new Cuts("sim_pi_neg", {EqualsCut({sim_particles + ".mother_id"}, -1),
                                                EqualsCut({sim_particles + ".pid"}, 211)}));
  SimParticlesQA(*task, new Cuts("sim_pi_pos", {EqualsCut({sim_particles + ".mother_id"}, -1),
                                                EqualsCut({sim_particles + ".pid"}, -211)}));
  SimParticlesQA(*task, new Cuts("sim_deuteron", {EqualsCut({sim_particles + ".mother_id"}, -1),
                                                  RangeCut({sim_particles + ".pid"}, 1000010020-0.5, 1000010020+0.5)}));
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

void VertexTracksQA(QA::Task& task, std::string branch, Cuts* cuts)
{
  Variable chi2_over_ndf("chi2_ndf", {{branch, "chi2"}, {branch, "ndf"}},
                         [](std::vector<double>& var) { return var.at(0) / var.at(1); });

  Variable y_lab("y_lab", {{branch, "pz"}, {branch, "p"}, {sim_particles, "mass"}},
                         [](std::vector<double>& var) {
                                                        double E = sqrt( var.at(1)*var.at(1) + var.at(2)*var.at(2) );
                                                        double pz = var.at(0);
                                                        return 0.5 * (
                                                                 log( E + pz ) -
                                                                 log( E - pz ) );
                 });
  Variable momentum_resolution("momentum_resolution", {{branch, "p"}, {sim_particles, "p"}},
                         [](std::vector<double>& var) {
                                 return fabs(var.at(0) - var.at(1)) / var.at(1);
                 });

  task.AddH1({"y_{lab}", y_lab, {100, 0, 4}}, cuts);
  task.AddH1({"DCA_{x}, cm", {branch, "dcax"}, {QA::gNbins, -5, 5}}, cuts);
  task.AddH1({"DCA_{y}, cm", {branch, "dcay"}, {QA::gNbins, -5, 5}}, cuts);
  task.AddH1({"DCA_{z}, cm", {branch, "dcaz"}, {QA::gNbins, -5, 5}}, cuts);
  task.AddH1({"z_{extr}, cm", {branch, "z_first"}, {QA::gNbins, -5, 5}}, cuts);

  task.AddH1({"NDF", {branch, "ndf"}, {30, 0, 30}}, cuts);
  task.AddH1({"N_{hits}", {branch, "n_hits"}, {30, 0, 30}}, cuts);
  task.AddH1({"#chi^{2}/NDF", chi2_over_ndf, {QA::gNbins, 0, 100}}, cuts);
  task.AddH2({"DCA_{x}, cm", {branch, "dcax"}, {QA::gNbins, -10, 10}},
             {"DCA_{y}, cm", {branch, "dcay"}, {QA::gNbins, -10, 10}}, cuts);

  task.AddH2({"y_{lab}", y_lab, {100, 0, 4}},
             {"p_{T} (GeV/c)", {branch, "pT"}, {100, 0, 3}}, cuts);
  task.AddProfile({"p_{sim} (GeV/c)", {sim_particles, "p"}, {250, 0.0, 5.0}}, {"res (%)", momentum_resolution, {}}, cuts);

}
void SimParticlesQA(QA::Task& task, Cuts* cuts=nullptr){
  task.AddH2({"y_{lab}", {sim_particles, "rapidity"}, {100, 0, 4}},
             {"p_{T} (GeV/c)", {sim_particles, "pT"}, {100, 0, 3}}, cuts);
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
