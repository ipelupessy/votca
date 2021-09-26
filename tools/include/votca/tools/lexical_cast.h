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

#ifndef VOTCA_TOOLS_LEXICAL_CAST_H
#define VOTCA_TOOLS_LEXICAL_CAST_H

// Standard includes
#include <stdexcept>
#include <string>

// Third party includes
#include <boost/lexical_cast.hpp>

namespace votca {
namespace tools {

/**
 * Wrapper for boost::lexical_cast with improved error messages
 * @param arg variable to convert
 * @param error additional error text
 * @return converted value
 */
template <typename Target, typename Source>
inline Target lexical_cast(const Source &arg, const std::string &error) {
  try {
    return boost::lexical_cast<Target, Source>(arg);
  } catch (std::exception &) {
    throw std::runtime_error("invalid type: " + error);
  }
}

}  // namespace tools
}  // namespace votca

#endif  // VOTCA_TOOLS_LEXICAL_CAST_H
