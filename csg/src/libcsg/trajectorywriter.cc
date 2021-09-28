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

#ifndef HAVE_NO_CONFIG
#include <votca_csg_config.h>
#endif

// Local VOTCA includes
#include "votca/csg/pdbwriter.h"
#include "votca/csg/trajectorywriter.h"
#include "votca/csg/xyzwriter.h"

// Local private VOTCA includes
#include "modules/io/dlpolytrajectorywriter.h"
#ifdef GMX_DOUBLE
#include "modules/io/gmxtrajectorywriter.h"
#endif
#include "modules/io/growriter.h"
#include "modules/io/lammpsdumpwriter.h"

namespace votca {
namespace csg {

using namespace std;

void TrajectoryWriter::RegisterPlugins() {
  TrjWriterFactory().Register<PDBWriter>("pdb");
  TrjWriterFactory().Register<XYZWriter>("xyz");
  TrjWriterFactory().Register<LAMMPSDumpWriter>("dump");
  TrjWriterFactory().Register<DLPOLYTrajectoryWriter>("dlph");
  TrjWriterFactory().Register<DLPOLYTrajectoryWriter>("dlpc");
#ifdef GMX_DOUBLE
  TrjWriterFactory().Register<GMXTrajectoryWriter>("trr");
  TrjWriterFactory().Register<GMXTrajectoryWriter>("xtc");
#endif
  TrjWriterFactory().Register<GROWriter>("gro");
}
}  // namespace csg
}  // namespace votca
