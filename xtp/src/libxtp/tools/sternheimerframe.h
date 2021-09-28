/*
 *            Copyright 2009-2020 The VOTCA Development Team
 *                       (http://www.votca.org)
 *
 *      Licensed under the Apache License, Version 2.0 (the "License")
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#pragma once
#ifndef VOTCA_XTP_STERNHEIMERFRAME_PRIVATE_H
#define VOTCA_XTP_STERNHEIMERFRAME_PRIVATE_H

// Standard includes
#include <cstdio>

// Local VOTCA includes
#include "votca/xtp/logger.h"
#include "votca/xtp/qmtool.h"
#include "votca/xtp/sternheimer.h"
#include <votca/xtp/orbitals.h>

namespace votca {
namespace xtp {

class SternheimerFrame : public QMTool {

 public:
  SternheimerFrame() = default;

  ~SternheimerFrame() override = default;

  std::string Identify() override { return "sternheimer"; }

 protected:
  void ParseOptions(const tools::Property& options) final;
  bool Run() final;

 private:
  std::string _guess_file;
  bool _do_guess;

  std::string _orbfile;
  std::string _xml_output;  // .xml output
  std::string _guess_orbA;
  std::string _guess_orbB;

  Logger _log;

  Sternheimer::options_sternheimer _options;
};
}  // namespace xtp
}  // namespace votca
#endif