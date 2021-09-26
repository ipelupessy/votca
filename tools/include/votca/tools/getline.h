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

#ifndef VOTCA_TOOLS_GETLINE_H
#define VOTCA_TOOLS_GETLINE_H

// Standard includes
#include <algorithm>
#include <iostream>
#include <string>

namespace votca {
namespace tools {

/**
    \brief Wrapper for a getline function

    Removes Windows end-of-line character

  */
inline std::istream& getline(std::istream& is, std::string& str) {
  std::istream& ist = std::getline(is, str, '\n');
  str.erase(remove(str.begin(), str.end(), '\r'), str.end());
  return ist;
}

}  // namespace tools
}  // namespace votca

#endif  // VOTCA_TOOLS_GETLINE_H
