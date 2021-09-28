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

#ifndef VOTCA_CSG_GMXTRAJECTORYREADER_PRIVATE_H
#define VOTCA_CSG_GMXTRAJECTORYREADER_PRIVATE_H

#ifndef HAVE_NO_CONFIG
#include <votca_csg_config.h>
#endif

// Standard includes
#include <string>

// Third party includes
#include <gromacs/fileio/oenv.h>
#include <gromacs/fileio/trxio.h>
#include <gromacs/trajectory/trajectoryframe.h>

// VOTCA includes
#include <votca/tools/unitconverter.h>

// Local VOTCA includes
#include "votca/csg/trajectoryreader.h"

// this one is needed because of bool is defined in one of the headers included
// by gmx
#undef bool

namespace votca {
namespace csg {

/**
    \brief class for reading gromacs trajectory files

    This class provides the TrajectoryReader interface and encapsulates the
   trajectory reading function of gromacs

*/

class GMXTrajectoryReader : public TrajectoryReader {
 public:
  GMXTrajectoryReader() = default;

  const tools::DistanceUnit distance_unit = tools::DistanceUnit::nanometers;
  const tools::MassUnit mass_unit = tools::MassUnit::atomic_mass_units;
  const tools::TimeUnit time_unit = tools::TimeUnit::picoseconds;
  const tools::ChargeUnit charge_unit = tools::ChargeUnit::e;
  const tools::MolarEnergyUnit energy_unit =
      tools::MolarEnergyUnit::kilojoules_per_mole;
  const tools::VelocityUnit velocity_unit =
      tools::VelocityUnit::nanometers_per_picosecond;
  const tools::MolarForceUnit force_unit =
      tools::MolarForceUnit::kilojoules_per_mole_nanometer;

  /// open a trejectory file
  bool Open(const std::string &file) override;
  /// read in the first frame
  bool FirstFrame(Topology &conf) override;
  /// read in the next frame
  bool NextFrame(Topology &conf) override;

  void Close() override;

 private:
  std::string filename_;

  // gmx status used in read_first_frame and  read_next_frame_;
  t_trxstatus *gmx_status_;
  /// gmx frame
  t_trxframe gmx_frame_;
};

}  // namespace csg
}  // namespace votca

#endif  // VOTCA_CSG_GMXTRAJECTORYREADER_PRIVATE_H
