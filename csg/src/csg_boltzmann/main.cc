/*
 * Copyright 2009-2020 The VOTCA Development Team (http://www.votca.org)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// TODO: This code need lots of cleaning up! please do not look at anything in
// here!
//

#include "../../include/votca/csg/csgapplication.h"
#include "analysistool.h"
#include "bondedstatistics.h"
#include "stdanalysis.h"
#include "tabulatedpotential.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <votca/tools/getline.h>
#include <votca/tools/rangeparser.h>
#include <votca/tools/tokenizer.h>

using namespace std;
using namespace votca::csg;

class CsgBoltzmann final : public CsgApplication {
 public:
  string ProgramName() { return "csg_boltzmann"; }
  void HelpText(ostream &out) {
    out << "Performs tasks that are needed for simple boltzmann\n"
           "inversion in an interactive environment.";
  }
  bool DoTrajectory() { return !OptionsMap().count("excl"); }
  bool DoMapping() { return true; }

  void Initialize();
  bool EvaluateOptions();
  void Run();

  void InteractiveMode();
  bool EvaluateTopology(Topology *top, Topology *top_ref);

 protected:
  ExclusionList CreateExclusionList(Topology *top_atomistic,
                                    Molecule &atomistic, Topology *top_cg,
                                    Molecule &cg);
  BondedStatistics bs_;
};
void CsgBoltzmann::Initialize() {
  CsgApplication::Initialize();
  AddProgramOptions("Special options")(
      "excl", boost::program_options::value<string>(),
      "write atomistic exclusion list to file");

  AddObserver(&bs_);
}

bool CsgBoltzmann::EvaluateOptions() {
  CsgApplication::EvaluateOptions();
  if (OptionsMap().count("excl")) {
    CheckRequired("cg", "excl options needs a mapping file");
  }
  return true;
}

bool CsgBoltzmann::EvaluateTopology(Topology *top, Topology *top_ref) {
  if (OptionsMap().count("excl")) {
    if (top_ref->MoleculeCount() > 1) {
      cout << "WARNING: cannot create exclusion list for topology with"
              "multiple molecules, using only first molecule\n";
    }

    cout << "Writing exclusion list for atomistic molecule "
         << top_ref->MoleculeByIndex(0)->getName()
         << " in coarse grained representation "
         << top_ref->MoleculeByIndex(0)->getName() << endl;

    ExclusionList ex = CreateExclusionList(
        top_ref, *top_ref->MoleculeByIndex(0), top, *top->MoleculeByIndex(0));
    std::ofstream fl;
    fl.open(OptionsMap()["excl"].as<string>());
    fl << "# atomistic: " << top_ref->MoleculeByIndex(0)->getName()
       << " cg: " << top_ref->MoleculeByIndex(0)->getName()
       << " cgmap: " << OptionsMap()["cg"].as<string>() << endl;
    fl << ex;
    fl.close();

    return false;
  }
  return true;
}

ExclusionList CsgBoltzmann::CreateExclusionList(Topology *top_atomistic,
                                                Molecule &atomistic,
                                                Topology *top_cg,
                                                Molecule &cg) {
  ExclusionList ex;
  // exclude all with all
  ex.ExcludeList(atomistic.Beads());
  // remove exclusions from inside a mapped bead
  for (votca::Index i = 0; i < cg.BeadCount(); ++i) {
    std::vector<Bead *> excl_list;
    for (votca::Index parent_bead_id : cg.getBead(i)->ParentBeads()) {
      excl_list.push_back(top_atomistic->getBead(parent_bead_id));
    }
    ex.Remove(excl_list);
  }
  // remove exclusion which come from atomistic topology and hence bonds and
  // angles
  for (votca::Index i = 0; i < cg.BeadCount() - 1; ++i) {
    for (votca::Index j = i + 1; j < cg.BeadCount(); ++j) {
      if (top_cg->getExclusions().IsExcluded(cg.getBead(i), cg.getBead(j))) {

        for (votca::Index parent_bead_id_i : cg.getBead(i)->ParentBeads()) {
          for (votca::Index parent_bead_id_j : cg.getBead(j)->ParentBeads()) {
            ex.RemoveExclusion(top_atomistic->getBead(parent_bead_id_i),
                               top_atomistic->getBead(parent_bead_id_j));
          }
        }
      }
    }
  }
  return ex;
}

void CsgBoltzmann::Run() {
  CsgApplication::Run();
  if (OptionsMap().count("excl")) {
    return;
  }
  InteractiveMode();
}

void CsgBoltzmann::InteractiveMode() {
  std::map<std::string, AnalysisTool *> cmds;
  TabulatedPotential tab;
  StdAnalysis std;
  tab.Register(cmds);
  std.Register(cmds);

  string help_text =
      "Interactive mode, expecting commands:\n"
      "help: show this help\n"
      "q: quit\n"
      "list: list all available bonds\n"
      "vals <file> <selection>: write values to file\n"
      "hist <file> <selection>: create histogram\n"
      "tab <file> <selection>: create tabulated potential\n"
      "autocor <file> <selection>: calculate autocorrelation, only one row "
      "allowed in selection!\n"
      "cor <file> <selection>: calculate correlations, first row is correlated "
      "with all other rows";

  cout << help_text << endl;

  while (true) {
    string line;
    cout << "> ";
    votca::tools::getline(cin, line);

    boost::trim(line);

    votca::tools::Tokenizer tok(line, " \t");
    vector<string> args = tok.ToVector();

    if (args.size() == 0) {
      continue;
    }

    string cmd = args.front();
    args.erase(args.begin());

    if (cmd == "q") {
      break;
    }

    std::map<string, AnalysisTool *>::iterator tool;
    if (cmd == "help") {
      if (args.size() == 0) {
        cout << help_text << endl;
        continue;
      }
      cmd = args.front();
      args.erase(args.begin());
      tool = cmds.find(cmd);
      if (tool == cmds.end()) {
        cout << "error, no help item found" << endl;
        continue;
      }
      tool->second->Help(cmd, args);
      cout << endl;
      continue;
    }

    tool = cmds.find(cmd);
    if (tool == cmds.end()) {
      cout << "error, command not found" << endl;
      continue;
    }

    tool->second->Command(bs_, cmd, args);
  }
}

int main(int argc, char **argv) {
  CsgBoltzmann app;
  app.Exec(argc, argv);
}
