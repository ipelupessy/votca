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
#include <memory>
#ifndef VOTCA_XTP_JOBAPPLICATION_H
#define VOTCA_XTP_JOBAPPLICATION_H

// Local VOTCA includes
#include "jobcalculator.h"
#include "progressobserver.h"
#include "statesaver.h"
#include "topology.h"
#include "xtpapplication.h"

namespace votca {
namespace xtp {

class JobApplication : public XtpApplication {
 public:
  JobApplication();
  ~JobApplication() override = default;
  void Initialize() override;
  bool EvaluateOptions() override;
  void Run() override;

  void BeginEvaluate(Index nThreads, Index ompthread,
                     ProgObserver<std::vector<Job> > &jobs);
  bool EvaluateFrame(Topology &top);
  void SetCalculator(std::unique_ptr<JobCalculator> &&calculator);

 protected:
  bool _generate_input = false;
  bool _run = false;
  bool _import = false;
  std::unique_ptr<JobCalculator> _calculator;
};

}  // namespace xtp
}  // namespace votca

#endif  // VOTCA_XTP_JOBAPPLICATION_H
