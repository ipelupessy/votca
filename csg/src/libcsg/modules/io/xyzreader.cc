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

// Standard includes
#include <vector>

// Third party includes
#include <boost/lexical_cast.hpp>

// VOTCA includes
#include <votca/tools/getline.h>

// Local VOTCA includes
#include "votca/csg/xyzreader.h"

namespace votca {
namespace csg {
using namespace boost;
using namespace std;

bool XYZReader::ReadTopology(string file, Topology &top) {
  top.Cleanup();
  file_ = file;
  fl_.open(file);
  if (!fl_.is_open()) {
    throw std::ios_base::failure("Error on open topology file: " + file);
  }

  top.CreateResidue("DUM");

  ReadFrame<true, Topology>(top);

  fl_.close();

  return true;
}

bool XYZReader::Open(const string &file) {
  file_ = file;
  fl_.open(file);
  if (!fl_.is_open()) {
    throw std::ios_base::failure("Error on open trajectory file: " + file);
  }
  line_ = 0;
  return true;
}

void XYZReader::Close() { fl_.close(); }

bool XYZReader::FirstFrame(Topology &top) { return NextFrame(top); }

bool XYZReader::NextFrame(Topology &top) {
  bool success = ReadFrame<false, Topology>(top);
  return success;
}

}  // namespace csg
}  // namespace votca
