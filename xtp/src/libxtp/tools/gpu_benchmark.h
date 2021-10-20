/*
 *            Copyright 2009-2021 The VOTCA Development Team
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
#ifndef VOTCA_XTP_GPUBENCHMARK_H
#define VOTCA_XTP_GPUBENCHMARK_H

#include "votca/xtp/qmtool.h"

namespace votca {
namespace xtp {

class GPUBenchmark final : public QMTool {
 public:
  GPUBenchmark() = default;

  ~GPUBenchmark() = default;
  std::string Identify() const { return "GPUBenchmark"; }

 protected:
  void ParseOptions(const tools::Property &user_options);
  bool Run();

 private:
  Index repetitions_;
  std::string outputfile_;
};

}  // namespace xtp
}  // namespace votca

#endif  // VOTCA_XTP_APDFT_H
