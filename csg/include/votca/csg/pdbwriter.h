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

#ifndef VOTCA_CSG_PDBWRITER_H
#define VOTCA_CSG_PDBWRITER_H

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

class PDBWriter : public TrajectoryWriter {
 public:
  const tools::DistanceUnit distance_unit = tools::DistanceUnit::angstroms;

  void Open(std::string file, bool bAppend = false) override;
  void Close() override;

  void Write(Topology *conf) override;

  template <class T>
  void WriteContainer(T &container);

  void WriteHeader(std::string header);

  void WriteBox(const Eigen::Matrix3d &box);

 private:
  template <class Atom>
  std::string getName(Atom &atom) {
    return atom.getElement();
  }

  std::string getName(const Bead &bead) { return bead.getName(); }

  template <class T, class Atom>
  std::string getResname(T &container, Atom &) {
    return container.getType();
  }
  std::string getResname(Topology &conf, const Bead &bead);

  template <class Atom>
  Index getId(Atom &atom) {
    return atom.getId();
  }

  template <class T, class Atom>
  Index getResId(T &container, Atom &) {
    return container.getId();
  }
  Index getResId(Topology &, const Bead &bead) { return bead.getResnr() + 1; }

  template <class Atom>
  void writeSymmetry(Atom &) {
    return;
  }
  void writeSymmetry(const Bead &bead);

  template <class Atom>
  Eigen::Vector3d getPos(Atom &atom) {
    return atom.getPos() * tools::conv::bohr2ang;
  }

  Eigen::Vector3d getPos(const Bead &bead) {
    return bead.Pos() * tools::conv::nm2ang;
  }

  template <class T>
  T &getIterable(T &container) {
    return container;
  }

  BeadContainer &getIterable(Topology &top) { return top.Beads(); }

  std::ofstream out_;
};

template <class T>
inline void PDBWriter::WriteContainer(T &container) {
  boost::format atomfrmt(
      "ATOM  %1$5d %2$-4s %3$-3s %4$1s%5$4d    %6$8.3f%7$8.3f%8$8.3f\n");

  for (const auto &atom : getIterable(container)) {
    Eigen::Vector3d r = getPos(atom);
    std::string resname = getResname(container, atom);
    std::string atomname = getName(atom);
    if (resname.size() > 3) {
      resname = resname.substr(0, 3);
    }
    if (atomname.size() > 4) {
      atomname = atomname.substr(0, 4);
    }

    out_ << atomfrmt % (getId(atom) % 100000)  // atom serial number, wrapped
                                               // due to pdb limitations
                % atomname % resname % " "     // chain identifier 1 char
                % (getResId(container, atom) % 10000)  // residue sequence
                                                       // number, wrapped due to
                                                       // pdb limitations
                % r.x() % r.y() % r.z();
    // we skip the charge
    writeSymmetry(atom);
  }
  out_ << std::flush;
}
}  // namespace csg
}  // namespace votca

#endif  // VOTCA_CSG_PDBWRITER_H
