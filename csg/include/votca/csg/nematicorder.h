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

#ifndef VOTCA_CSG_NEMATICORDER_H
#define VOTCA_CSG_NEMATICORDER_H

// VOTCA includes
#include <votca/tools/eigen.h>

// Local VOTCA includes
#include "topology.h"

namespace votca {
namespace csg {

class NematicOrder {
 public:
  NematicOrder() = default;
  ~NematicOrder() = default;

  void Process(Topology &top, const std::string &filter = "*");

  Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> &NematicU() {
    return nemat_u_;
  }
  Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> &NematicV() {
    return nemat_v_;
  }
  Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> &NematicW() {
    return nemat_w_;
  }

 private:
  Eigen::Matrix3d mu_, mv_, mw_;
  Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> nemat_u_, nemat_v_, nemat_w_;
};

}  // namespace csg
}  // namespace votca

#endif /*  VOTCA_CSG_NEMATICORDER_H */
