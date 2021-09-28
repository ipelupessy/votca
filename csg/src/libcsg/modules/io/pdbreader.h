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

#ifndef VOTCA_CSG_PDBREADER_PRIVATE_H
#define VOTCA_CSG_PDBREADER_PRIVATE_H

// Standard includes
#include <fstream>
#include <iostream>
#include <string>

// VOTCA includes
#include <votca/tools/elements.h>
#include <votca/tools/unitconverter.h>

// Local VOTCA includes
#include "votca/csg/topologyreader.h"
#include "votca/csg/trajectoryreader.h"

namespace votca {
namespace csg {

/**
    brief class for reading pdb files

    This class provides the Trajectory and Topology reader interface
    for pdb files

*/
class PDBReader : public TopologyReader, public TrajectoryReader {
 public:
  const tools::DistanceUnit distance_unit = tools::DistanceUnit::angstroms;

  /// Constuctor
  PDBReader() = default;
  /// Destructor
  ~PDBReader() override = default;
  /// open a topology file
  bool ReadTopology(std::string file, Topology &top) override;
  /// open a trajectory file
  bool Open(const std::string &file) override;
  /// read in the first frame
  bool FirstFrame(Topology &top) override;
  /// read in the next frame
  bool NextFrame(Topology &top) override;
  void Close() override;

 private:
  std::ifstream fl_;
  bool topology_;
};
}  // namespace csg
}  // namespace votca

#endif  // VOTCA_CSG_PDBREADER_PRIVATE_H
