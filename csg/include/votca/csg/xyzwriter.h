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

#ifndef VOTCA_CSG_XYZWRITER_H
#define VOTCA_CSG_XYZWRITER_H

// Standard includes
#include <cstdio>

// VOTCA includes
#include <votca/tools/constants.h>
#include <votca/tools/unitconverter.h>

// Local VOTCA includes
#include "topology.h"
#include "trajectorywriter.h"

namespace votca {
namespace csg {

class XYZWriter : public TrajectoryWriter {
 public:
  const tools::DistanceUnit distance_unit = tools::DistanceUnit::angstroms;

  void Open(std::string file, bool bAppend = false) override;
  void Close() override;

  void Write(Topology *conf) override;

  template <class T>
  void Write(T &container, std::string header);

 private:
  template <class T>
  Index getSize(T &container) {
    return getIterable(container).size();
  }

  template <class Atom>
  std::string getName(Atom &atom) {
    return atom.getElement();
  }

  std::string getName(std::unique_ptr<Bead> &bead) { return bead->getName(); }

  template <class Atom>
  Eigen::Vector3d getPos(Atom &atom) {
    return atom.getPos() * tools::conv::bohr2ang;
  }

  Eigen::Vector3d getPos(std::unique_ptr<Bead> &bead) {
    return bead->Pos() * tools::conv::nm2ang;
  }

  template <class T>
  T &getIterable(T &container) {
    return container;
  }

  BeadContainer &getIterable(Topology &top) { return top.Beads(); }

  std::ofstream out_;
};

template <class T>
inline void XYZWriter::Write(T &container, std::string header) {
  out_ << getSize(container) << "\n";
  out_ << header << "\n";

  boost::format fmter("%1$s%2$10.5f%3$10.5f%4$10.5f\n");

  for (auto &atom : getIterable(container)) {
    Eigen::Vector3d r = getPos(atom);
    // truncate strings if necessary
    std::string atomname = getName(atom);
    if (atomname.size() > 3) {
      atomname = atomname.substr(0, 3);
    }
    while (atomname.size() < 3) {
      atomname = " " + atomname;
    }

    out_ << fmter % atomname % r.x() % r.y() % r.z();
  }
  out_ << std::flush;
}
}  // namespace csg
}  // namespace votca

#endif  // VOTCA_CSG_XYZWRITER_H
