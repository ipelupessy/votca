/*
 * Copyright 2009-2019 The VOTCA Development Team (http://www.votca.org)
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

#ifndef VOTCA_CSG_LAMMPSDUMPREADER_H
#define VOTCA_CSG_LAMMPSDUMPREADER_H

#include "../../../../include/votca/csg/topologyreader.h"
#include "../../../../include/votca/csg/trajectoryreader.h"
#include <fstream>
#include <iostream>
#include <string>
#include <votca/tools/unitconverter.h>

namespace votca {
namespace csg {

/**
    \brief class for reading lammps dump files

    This class provides the TrajectoryReader + Topology reader interface
    for lammps dump files

*/
class LAMMPSDumpReader : public TrajectoryReader, public TopologyReader {
 public:
  /// Assuming units are using 'units real' lammps command
  const tools::DistanceUnit distance_unit = tools::DistanceUnit::angstroms;
  const tools::TimeUnit time_unit = tools::TimeUnit::femtoseconds;
  const tools::MassUnit mass_unit = tools::MassUnit::grams_per_mole;
  const tools::MolarEnergyUnit energy_unit =
      tools::MolarEnergyUnit::kilocalories_per_mole;
  const tools::ChargeUnit charge_unit = tools::ChargeUnit::e;
  const tools::MolarForceUnit force_unit =
      tools::MolarForceUnit::kilocalories_per_mole_angstrom;
  const tools::VelocityUnit velocity_unit =
      tools::VelocityUnit::angstroms_per_femtosecond;

  LAMMPSDumpReader() = default;
  ~LAMMPSDumpReader() override = default;

  /// open a topology file
  bool ReadTopology(std::string file, Topology &top) override;

  /// open a trejectory file
  bool Open(const std::string &file) override;
  /// read in the first frame
  bool FirstFrame(Topology &top) override;
  /// read in the next frame
  bool NextFrame(Topology &top) override;

  void Close() override;

 private:
  void ReadTimestep(Topology &top);
  void ReadBox(Topology &top);
  void ReadNumAtoms(Topology &top);
  void ReadAtoms(Topology &top, std::string itemline);

  std::ifstream fl_;
  std::string fname_;
  bool topology_;
  Index natoms_;
};

}  // namespace csg
}  // namespace votca

#endif  //  VOTCA_CSG_LAMMPSDUMPREADER_H
