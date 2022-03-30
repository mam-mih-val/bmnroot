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

const int kPdgLambda = 10000000;
const int kPdgCharge = 10000;
const int kPdgMass   = 10;

Int_t GetIonCharge(int pdgCode) { return (pdgCode % kPdgLambda) / kPdgCharge; }

void run_analysistree_qa(std::string filelist, bool is_single_file = false);

const std::string sim_event_header = "SimEventHeader";
const std::string rec_event_header = "RecEventHeader";
const std::string sim_particles    = "SimParticles";
const std::string rec_tracks       = "GlobalTracks";
const std::string sts_tracks       = "StsTracks";
const std::string tof400_hits         = "Tof400Hits";
const std::string tof700_hits         = "Tof700Hits";
const std::string trd_tracks       = "TrdTracks";
const std::string rich_rings       = "RichRings";

void run_analysistree_qa(std::string filelist, bool is_single_file)
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
  VertexTracksQA(*task, rec_tracks, new Cuts("Primary", {EqualsCut({sim_particles + ".mother_id"}, -1)}));
  VertexTracksQA(*task, sts_tracks);
  VertexTracksQA(*task, sts_tracks, new Cuts("Primary", {EqualsCut({sim_particles + ".mother_id"}, -1)}));
  SimParticlesQA(*task);
  SimParticlesQA(*task, new Cuts("Primary", {EqualsCut({sim_particles + ".mother_id"}, -1)}));
  SimEventHeaderQA(*task);
  RecEventHeaderQA(*task);
  TofHitsQA(*task);
  TrackingQA(*task);

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
  AddTrackQA(&task, branch, cuts);
  if (!sim_particles.empty()) { AddTracksMatchQA(&task, branch, sim_particles, cuts); }

  Variable chi2_over_ndf("chi2_ndf", {{branch, "chi2"}, {branch, "ndf"}},
                         [](std::vector<double>& var) { return var.at(0) / var.at(1); });

  task.AddH1({"DCA_{x}, cm", {branch, "dcax"}, {QA::gNbins, -5, 5}}, cuts);
  task.AddH1({"DCA_{y}, cm", {branch, "dcay"}, {QA::gNbins, -5, 5}}, cuts);
  task.AddH1({"DCA_{z}, cm", {branch, "dcaz"}, {QA::gNbins, -5, 5}}, cuts);
  task.AddH1({"z_{extr}, cm", {branch, "z_first"}, {QA::gNbins, -10, 190}}, cuts);

  task.AddH1({"NDF", {branch, "ndf"}, {30, 0, 30}}, cuts);
  task.AddH1({"N_{hits}", {branch, "n_hits"}, {30, 0, 30}}, cuts);
  task.AddH1({"#chi^{2}_{vertex}", {branch, "vtx_chi2"}, {500, 0, 100}}, cuts);
  task.AddH1({"#chi^{2}/NDF", chi2_over_ndf, {QA::gNbins, 0, 100}}, cuts);
  task.AddH2({"DCA_{x}, cm", {branch, "dcax"}, {QA::gNbins, -10, 10}},
             {"DCA_{y}, cm", {branch, "dcay"}, {QA::gNbins, -10, 10}}, cuts);

  task.AddH2({"q/p first [GeV/c]", {branch, "qp_first"}, {QA::gNbins, -5, 5}},
             {"q/p last [GeV/c]", {branch, "qp_last"}, {QA::gNbins, -5, 5}}, cuts);

  task.AddH2({"t_{x} first", {branch, "tx_first"}, {QA::gNbins, -1, 1}},
             {"t_{x} last", {branch, "tx_last"}, {QA::gNbins, -1, 1}}, cuts);

  task.AddH2({"t_{y} first", {branch, "ty_first"}, {QA::gNbins, -1, 1}},
             {"t_{y} last", {branch, "ty_last"}, {QA::gNbins, -1, 1}}, cuts);
}

void TrackingQA(QA::Task& task){
  task.AddH2({"q/p STS [GeV/c]", {sts_tracks, "qp_first"}, {QA::gNbins, -5, 5}},
             {"q/p Global [GeV/c]", {rec_tracks, "qp_first"}, {QA::gNbins, -5, 5}});

  task.AddH2({"q/p STS [GeV/c]", {sts_tracks, "qp_last"}, {QA::gNbins, -5, 5}},
             {"q/p Global [GeV/c]", {rec_tracks, "qp_last"}, {QA::gNbins, -5, 5}});

  task.AddH2({"t_{x} STS", {sts_tracks, "tx_first"}, {QA::gNbins, -1, 1}},
             {"t_{x} Global", {rec_tracks, "tx_first"}, {QA::gNbins, -1, 1}});

  task.AddH2({"t_{x} STS", {sts_tracks, "tx_last"}, {QA::gNbins, -1, 1}},
             {"t_{x} Global", {rec_tracks, "tx_last"}, {QA::gNbins, -1, 1}});

  task.AddH2({"t_{y} STS", {sts_tracks, "ty_first"}, {QA::gNbins, -1, 1}},
             {"t_{y} Global", {rec_tracks, "ty_first"}, {QA::gNbins, -1, 1}});

  task.AddH2({"t_{y} STS", {sts_tracks, "ty_last"}, {QA::gNbins, -1, 1}},
             {"t_{y} Global", {rec_tracks, "ty_last"}, {QA::gNbins, -1, 1}});

  Variable sim_particles_qp("sim_particles_qp", {{sim_particles, "p"}, {sim_particles, "pid"}},
                         [](std::vector<double>& var) {
                              auto particle = TDatabasePDG::Instance()->GetParticle(var.at(1));
                              double charge=1.0;
                              if( particle )
                                charge = particle->Charge() / 3.0;
                              else
                                charge = GetIonCharge(var.at(1));
                              return charge / var.at(0);
                            });

  Variable sim_particles_tx("sim_particles_tx", {{sim_particles, "px"}, {sim_particles, "pz"}, {sim_particles, "pid"}},
                         [](std::vector<double>& var) {
                              auto particle = TDatabasePDG::Instance()->GetParticle(var.at(2));
                              double charge=1.0;
                              if( particle )
                                charge = particle->Charge() > 0 ? +1.0 : -1.0;
                              return var.at(0) / var.at(1);
                            });

  Variable sim_particles_ty("sim_particles_ty", {{sim_particles, "py"}, {sim_particles, "pz"}, {sim_particles, "pid"}},
                         [](std::vector<double>& var) {
                              auto particle = TDatabasePDG::Instance()->GetParticle(var.at(2));
                              double charge=1.0;
                              if( particle )
                                charge = particle->Charge() > 0 ? +1.0 : -1.0;
                              return var.at(0) / var.at(1);
                            });

  task.AddH2({"q/p STS [GeV/c]", {sts_tracks, "qp_first"}, {QA::gNbins, -5, 5}},
             {"q/p GEN [GeV/c]", sim_particles_qp, {QA::gNbins, -5, 5}});

  task.AddH2({"q/p STS [GeV/c]", {sts_tracks, "qp_last"}, {QA::gNbins, -5, 5}},
             {"q/p GEN [GeV/c]", sim_particles_qp, {QA::gNbins, -5, 5}});

  task.AddH2({"t_{x} STS", {sts_tracks, "tx_first"}, {QA::gNbins, -1, 1}},
             {"t_{x} GEN", sim_particles_tx, {QA::gNbins, -1, 1}});

  task.AddH2({"t_{x} STS", {sts_tracks, "tx_last"}, {QA::gNbins, -1, 1}},
             {"t_{x} GEN", sim_particles_tx, {QA::gNbins, -1, 1}});

  task.AddH2({"t_{y} STS", {sts_tracks, "ty_first"}, {QA::gNbins, -1, 1}},
             {"t_{y} GEN", sim_particles_ty, {QA::gNbins, -1, 1}});

  task.AddH2({"t_{y} STS", {sts_tracks, "ty_last"}, {QA::gNbins, -1, 1}},
             {"t_{y} GEN", sim_particles_ty, {QA::gNbins, -1, 1}});

  task.AddH2({"q/p Global [GeV/c]", {rec_tracks, "qp_first"}, {QA::gNbins, -5, 5}},
             {"q/p GEN [GeV/c]", sim_particles_qp, {QA::gNbins, -5, 5}});

  task.AddH2({"q/p Global [GeV/c]", {rec_tracks, "qp_last"}, {QA::gNbins, -5, 5}},
             {"q/p GEN [GeV/c]", sim_particles_qp, {QA::gNbins, -5, 5}});

  task.AddH2({"t_{x} Global", {rec_tracks, "tx_first"}, {QA::gNbins, -1, 1}},
             {"t_{x} GEN", sim_particles_tx, {QA::gNbins, -1, 1}});

  task.AddH2({"t_{x} Global", {rec_tracks, "tx_last"}, {QA::gNbins, -1, 1}},
             {"t_{x} GEN", sim_particles_tx, {QA::gNbins, -1, 1}});

  task.AddH2({"t_{y} Global", {rec_tracks, "ty_first"}, {QA::gNbins, -1, 1}},
             {"t_{y} GEN", sim_particles_ty, {QA::gNbins, -1, 1}});

  task.AddH2({"t_{y} Global", {rec_tracks, "ty_last"}, {QA::gNbins, -1, 1}},
             {"t_{y} GEN", sim_particles_ty, {QA::gNbins, -1, 1}});
}

void TofHitsQA(QA::Task& task)
{
  task.AddH1({"TOF hit matching radius (cm)", {tof400_hits, "matching_radius"}, {QA::gNbins, 0, 30}});
  task.AddH1({"TOF hit x-position (cm)", {tof400_hits, "x"}, {QA::gNbins, -200, 200}});
  task.AddH1({"TOF hit y-position (cm)", {tof400_hits, "y"}, {QA::gNbins, -100, 100}});
  task.AddH1({"TOF hit z-position (cm)", {tof400_hits, "z"}, {QA::gNbins, 440, 490}});
  task.AddH1({"TOF hit time (nc)", {tof400_hits, "time"}, {QA::gNbins, 15.0, 30.0}});
  task.AddH1({"TOF hit beta (1/c)", {tof400_hits, "beta"}, {QA::gNbins, -0.2, 1.8}});
  task.AddH1({"TOF hit mass2 (GeV^{2}/c^{4})", {tof400_hits, "mass2"}, {QA::gNbins, -0.5, 4.5}});

  task.AddH2({"TOF hit x-position (cm)", {tof400_hits, "x"}, {QA::gNbins, -200, 200}},
             {"TOF hit y-position (cm)", {tof400_hits, "y"}, {QA::gNbins, -100, 100}});

  task.AddH1({"TOF hit matching radius (cm)", {tof700_hits, "matching_radius"}, {QA::gNbins, 0, 30}});
  task.AddH1({"TOF hit x-position (cm)", {tof700_hits, "x"}, {QA::gNbins, -200, 200}});
  task.AddH1({"TOF hit y-position (cm)", {tof700_hits, "y"}, {QA::gNbins, -100, 100}});
  task.AddH1({"TOF hit z-position (cm)", {tof700_hits, "z"}, {QA::gNbins, 580, 680}});
  task.AddH1({"TOF hit time (nc)", {tof700_hits, "time"}, {QA::gNbins, 0.0, 30.0}});
  task.AddH1({"TOF hit beta (1/c)", {tof700_hits, "beta"}, {QA::gNbins, -0.2, 1.8}});
  task.AddH1({"TOF hit mass2 (GeV^{2}/c^{4})", {tof700_hits, "mass2"}, {QA::gNbins, -0.5, 4.5}});

  task.AddH2({"TOF hit x-position (cm)", {tof700_hits, "x"}, {QA::gNbins, -200, 200}},
             {"TOF hit y-position (cm)", {tof700_hits, "y"}, {QA::gNbins, -100, 100}});

  Variable qp_global("qp_reco", {{rec_tracks, "charge"}, {rec_tracks, "p"}},
                  [](std::vector<double>& qp) { return qp.at(0) * qp.at(1); });

  task.AddH2({"q#timesp, GeV/c", qp_global, {QA::gNbins, -5, 5}},
             {"m^{2}, GeV^{2}/c^{4}", {tof400_hits, "mass2"}, {QA::gNbins, -1.0, 5}});
  task.AddH2({"q#timesp, GeV/c", qp_global, {QA::gNbins, -5, 5}},
             {"m^{2}, GeV^{2}/c^{4}", {tof700_hits, "mass2"}, {QA::gNbins, -1.0, 5}});

  task.AddH2({"q#timesp, GeV/c", qp_global, {QA::gNbins, -5, 5}},
             {"m^{2}, GeV^{2}/c^{4}", {tof400_hits, "beta"}, {QA::gNbins, -0.2, 1.2}});
  task.AddH2({"q#timesp, GeV/c", qp_global, {QA::gNbins, -5, 5}},
             {"m^{2}, GeV^{2}/c^{4}", {tof700_hits, "beta"}, {QA::gNbins, -0.2, 1.2}});

  task.AddH2({"Z_{last} Global", {rec_tracks, "z_last"}, {QA::gNbins, 300, 700}},
             {"Z_{TOF-400} Global", {tof400_hits, "z"}, {QA::gNbins, 300, 700}});
  task.AddH2({"Z_{last} Global", {rec_tracks, "z_last"}, {QA::gNbins, 300, 700}},
             {"Z_{TOF-700} Global", {tof700_hits, "z"}, {QA::gNbins, 300, 700}});

}

void SimParticlesQA(QA::Task& task, Cuts* cuts) {
  AddParticleQA(&task, sim_particles, cuts);
}

void SimEventHeaderQA(QA::Task& task)
{
  task.AddH1({"x_{vertex}^{MC} (cm)", {sim_event_header, "vtx_x"}, {QA::gNbins, -1, 1}});
  task.AddH1({"y_{vertex}^{MC} (cm)", {sim_event_header, "vtx_y"}, {QA::gNbins, -1, 1}});
  task.AddH1({"z_{vertex}^{MC} (cm)", {sim_event_header, "vtx_z"}, {QA::gNbins, -1, 1}});
  task.AddH1({"b (fm)", {sim_event_header, "b"}, {QA::gNbins, 0, 20}});
  task.AddH1({"#Psi_{RP}", {sim_event_header, "psi_RP"}, {QA::gNbins, -3.5, 3.5}});

  task.AddH2({"x_{vertex}^{MC} (cm)", {sim_event_header, "vtx_x"}, {QA::gNbins, -1, 1}},
             {"y_{vertex}^{MC} (cm)", {sim_event_header, "vtx_y"}, {QA::gNbins, -1, 1}});
}

void RecEventHeaderQA(QA::Task& task)
{
  task.AddH1({"x_{vertex} (cm)", {rec_event_header, "vtx_x"}, {QA::gNbins, -1, 1}});
  task.AddH1({"y_{vertex} (cm)", {rec_event_header, "vtx_y"}, {QA::gNbins, -1, 1}});
  task.AddH1({"z_{vertex} (cm)", {rec_event_header, "vtx_z"}, {QA::gNbins, -1, 1}});
  task.AddH1({"#chi^{2}_{vertex fit}", {rec_event_header, "vtx_chi2"}, {QA::gNbins, 0, 5}});

  task.AddH1({"E_{PSD} (GeV)", {rec_event_header, "Epsd"}, {QA::gNbins, 0, 60}});
  task.AddH1({"M_{tracks}", {rec_event_header, "M"}, {100, 0, 100}});
  task.AddH1({"Event ID", {rec_event_header, "evt_id"}, {QA::gNbins, 0, 2000}});

  task.AddH2({"x_{vertex} (cm)", {rec_event_header, "vtx_x"}, {QA::gNbins, -1, 1}},
             {"y_{vertex} (cm)", {rec_event_header, "vtx_y"}, {QA::gNbins, -1, 1}});

  task.AddH2({"x_{vertex} (cm)", {rec_event_header, "vtx_x"}, {QA::gNbins, -1, 1}},
             {"x_{vertex} (cm)", {sim_event_header, "vtx_x"}, {QA::gNbins, -1, 1}});
  task.AddH2({"y_{vertex} (cm)", {rec_event_header, "vtx_y"}, {QA::gNbins, -1, 1}},
             {"y_{vertex} (cm)", {sim_event_header, "vtx_y"}, {QA::gNbins, -1, 1}});
  task.AddH2({"z_{vertex} (cm)", {rec_event_header, "vtx_z"}, {QA::gNbins, -1, 1}},
             {"z_{vertex} (cm)", {sim_event_header, "vtx_z"}, {QA::gNbins, -1, 1}});
}

void EfficiencyMaps(QA::Task& task)
{
  const float y_beam  = 1.62179f;  // TODO from DataHeader
  const float p_mass  = 0.938;
  const float pi_mass = 0.14;

  Variable proton_y("y-y_{beam}", {{rec_tracks, "p"}, {rec_tracks, "pz"}}, [y_beam, p_mass](std::vector<double>& var) {
    const float e = sqrt(p_mass * p_mass + var[0] * var[0]);
    return 0.5 * log((e + var[1]) / (e - var[1]));
  });

  Variable pion_y("y-y_{beam}", {{rec_tracks, "p"}, {rec_tracks, "pz"}}, [y_beam, pi_mass](std::vector<double>& var) {
    const float e = sqrt(pi_mass * pi_mass + var[0] * var[0]);
    return 0.5 * log((e + var[1]) / (e - var[1]));
  });

  Cuts* mc_protons   = new Cuts("McProtons", {EqualsCut({sim_particles + ".pid"}, 2212)});
  Cuts* mc_pions_neg = new Cuts("McPionsNeg", {EqualsCut({sim_particles + ".pid"}, -211)});
  Cuts* mc_pions_pos = new Cuts("McPionsPos", {EqualsCut({sim_particles + ".pid"}, -211)});

  task.AddH2({"#it{y}_{Lab}", {sim_particles, "rapidity"}, {QA::gNbins, -1, 5}},
             {"p_{T}, GeV/c", {sim_particles, "pT"}, {QA::gNbins, 0, 2}}, mc_protons);
  task.AddH2({"#it{y}_{Lab}", proton_y, {QA::gNbins, -1, 5}}, {"p_{T}, GeV/c", {rec_tracks, "pT"}, {QA::gNbins, 0, 2}},
             mc_protons);

  task.AddH2({"#it{y}_{Lab}", {sim_particles, "rapidity"}, {QA::gNbins, -1, 5}},
             {"p_{T}, GeV/c", {sim_particles, "pT"}, {QA::gNbins, 0, 2}}, mc_pions_neg);
  task.AddH2({"#it{y}_{Lab}", pion_y, {QA::gNbins, -1, 5}}, {"p_{T}, GeV/c", {rec_tracks, "pT"}, {QA::gNbins, 0, 2}},
             mc_pions_neg);

  task.AddH2({"#it{y}_{Lab}", {sim_particles, "rapidity"}, {QA::gNbins, -1, 5}},
             {"p_{T}, GeV/c", {sim_particles, "pT"}, {QA::gNbins, 0, 2}}, mc_pions_pos);
  task.AddH2({"#it{y}_{Lab}", pion_y, {QA::gNbins, -1, 5}}, {"p_{T}, GeV/c", {rec_tracks, "pT"}, {QA::gNbins, 0, 2}},
             mc_pions_pos);
}

int main(int argc, char** argv)
{
  if (argc <= 1) {
    std::cout << "Not enough arguments! Please use:" << std::endl;
    std::cout << "   ./cbm_qa filelist" << std::endl;
    return -1;
  }

  const std::string filelist = argv[1];
  run_analysistree_qa(filelist);
  return 0;
}
