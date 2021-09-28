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
#include <cstdio>
#include <string>

// Third party includes
#include <boost/format.hpp>

// Local VOTCA includes
#include "votca/csg/pdbwriter.h"

namespace votca {
namespace csg {

using namespace std;

void PDBWriter::Open(string file, bool bAppend) {
  if (bAppend) {
    out_.open(file, std::ios_base::app);
  } else {
    out_.open(file);
  }
}

void PDBWriter::WriteHeader(std::string header) {
  if (header.size() < 10 || header.substr(0, 10) != "HEADER    ") {
    out_ << "HEADER    ";
  }
  out_ << header;
  if (header.back() != '\n') {
    out_ << "\n";
  }
}

void PDBWriter::Close() { out_.close(); }

void PDBWriter::Write(Topology *conf) {

  out_ << boost::format("MODEL     %1$4d\n") % (conf->getStep() + 1)
       << std::flush;
  ;
  WriteContainer<Topology>(*conf);
  out_ << "ENDMDL" << std::endl;
}

void PDBWriter::WriteBox(const Eigen::Matrix3d &box) {
  boost::format boxfrmt("CRYST1%1$9.3f%2$9.3f%3$9.3f%4$7.2f%5$7.2f%6$7.2f\n");
  double a = box.col(0).norm();
  double b = box.col(1).norm();
  double c = box.col(2).norm();
  double alpha =
      180 / tools::conv::Pi * std::acos(box.col(1).dot(box.col(2)) / (b * c));
  double beta =
      180 / tools::conv::Pi * std::acos(box.col(0).dot(box.col(2)) / (a * c));
  double gamma =
      180 / tools::conv::Pi * std::acos(box.col(0).dot(box.col(1)) / (a * b));
  out_ << boxfrmt % a % b % c % alpha % beta % gamma;
}

void PDBWriter::writeSymmetry(const Bead &bead) {
  if (bead.getSymmetry() > 1) {
    Eigen::Vector3d r = 10 * bead.getPos();
    boost::format beadfrmt(
        "HETATM%1$5d %2$4s %3$3s %4$1s%5$4d    %6$8.3f%7$8.3f%8$8.3f\n");
    Eigen::Vector3d ru = 0.1 * bead.getU() + r;

    out_ << beadfrmt % (bead.getId() + 1) % 100000  // atom serial number
                % bead.getName()                    // atom name
                % "REU"                             // residue name
                % " "                               // chain identifier 1 char
                % (bead.getResnr() + 1)             // residue sequence number
                % ru.x() % ru.y() % ru.z();         // we skip the charge

    if (bead.getSymmetry() > 2) {
      Eigen::Vector3d rv = 0.1 * bead.getV() + r;
      out_ << beadfrmt % (bead.getId() + 1) % 100000  // atom serial number
                  % bead.getName()                    // atom name
                  % "REV"                             // residue name
                  % " "                               // chain identifier 1 char
                  % (bead.getResnr() + 1)             // residue sequence number
                  % rv.x() % rv.y() % rv.z();         // we skip the charge
    }
  }
  return;
}

std::string PDBWriter::getResname(Topology &conf, const Bead &bead) {
  if (bead.getResnr() < conf.ResidueCount()) {
    return conf.getResidue(bead.getResnr()).getName();
  } else {
    return "";
  }
}
}  // namespace csg
}  // namespace votca
