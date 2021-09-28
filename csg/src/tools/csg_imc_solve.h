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

#ifndef VOTCA_CSG_CSG_IMC_SOLVE_H
#define VOTCA_CSG_CSG_IMC_SOLVE_H

// VOTCA includes
#include <votca/tools/application.h>
#include <votca/tools/property.h>

// Local VOTCA includes
#include "votca/csg/version.h"

/**
    \brief Solves linear system for IMCS
 *
 **/

class CG_IMC_solve : public votca::tools::Application {
 public:
  std::string ProgramName() override { return "csg_imc_solve"; }
  void HelpText(std::ostream &out) override {
    out << "Solves the linear system for IMCs";
  }

  void ShowHelpText(std::ostream &out) override {
    std::string name = ProgramName();
    if (VersionString() != "") {
      name = name + ", version " + VersionString();
    }

    votca::csg::HelpTextHeader(name);
    HelpText(out);

    out << "\n\n" << VisibleOptions() << std::endl;
  }

  bool EvaluateOptions() override;
  void Initialize() override;
  void Run() override;
};

#endif  // VOTCA_CSG_CSG_IMC_SOLVE_H
