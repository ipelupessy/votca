/*
 * Copyright 2009-2021 The VOTCA Development Team (http://www.votca.org)
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

// Standard includes
#include <fstream>
#include <memory>

// VOTCA includes
#include <votca/tools/tokenizer.h>

// Local VOTCA includes
#include "votca/csg/cgengine.h"
#include "votca/csg/version.h"

namespace votca {
namespace csg {

using namespace std;

namespace po = boost::program_options;

CGEngine::CGEngine() = default;

/**
    \todo melts with different molecules
*/
std::unique_ptr<TopologyMap> CGEngine::CreateCGTopology(const Topology &in,
                                                        Topology &out) {
  const MoleculeContainer &mols = in.Molecules();
  auto m = std::make_unique<TopologyMap>(&in, &out);
  for (const auto &mol : mols) {
    if (IsIgnored(mol.getName())) {
      continue;
    }
    CGMoleculeDef *def = getMoleculeDef(mol.getName());
    if (!def) {
      cout << "--------------------------------------\n"
           << "WARNING: unknown molecule \"" << mol.getName() << "\" with id "
           << mol.getId() << " in topology" << endl
           << "molecule will not be mapped to CG representation\n"
           << "Check weather a mapping file for all molecule exists, was "
              "specified in --cg "
           << "separated by ; and the ident tag in xml-file matches the "
              "molecule name\n"
           << "--------------------------------------\n";
      continue;
    }
    Molecule *mcg = def->CreateMolecule(out);
    Map map = def->CreateMap(mol, *mcg);
    m->AddMoleculeMap(std::move(map));
  }
  out.RebuildExclusions();
  return m;
}

void CGEngine::LoadMoleculeType(const string &filename) {
  tools::Tokenizer tok(filename, ";");

  for (auto &word : tok) {
    auto def = std::make_unique<CGMoleculeDef>();
    string file = word;
    boost::trim(file);
    def->Load(file);
    molecule_defs_[def->getIdent()] = std::move(def);
  }
}

}  // namespace csg
}  // namespace votca
