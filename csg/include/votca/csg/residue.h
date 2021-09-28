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
#pragma once
#ifndef VOTCA_CSG_RESIDUE_H
#define VOTCA_CSG_RESIDUE_H

// Standard includes
#include <string>

namespace votca {
namespace csg {

/**
    \brief class for a residue

    The Residue class describes a residue. When reading in the atoms, all beads
   belong to a residue. Later on, the molecules can be organized into molecules
   based on their residue.

*/
class Residue {
 public:
  /// get the name of the residue
  const std::string &getName() const;

  /// get the name of the residue
  Index getId() const { return id_; }

 private:
  Index id_;
  std::string name_;

  /// constructor
  Residue(Index id, const std::string &name) : id_(id), name_(name) {}
  friend class Topology;
};

inline const std::string &Residue::getName() const { return name_; }

}  // namespace csg
}  // namespace votca

#endif  // VOTCA_CSG_RESIDUE_H
