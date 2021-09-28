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

#ifndef VOTCA_CSG_GROWRITER_PRIVATE_H
#define VOTCA_CSG_GROWRITER_PRIVATE_H

// Standard includes
#include <cstdio>

// VOTCA includes
#include <votca/tools/unitconverter.h>

// Local VOTCA includes
#include "votca/csg/topology.h"
#include "votca/csg/trajectorywriter.h"

namespace votca {
namespace csg {

class GROWriter : public TrajectoryWriter {
 public:
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

  void Open(std::string file, bool bAppend = false) override;
  void Close() override;

  void Write(Topology *conf) override;

 private:
  FILE *out_;
};

}  // namespace csg
}  // namespace votca

#endif  // VOTCA_CSG_GROWRITER_PRIVATE_H
