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
#ifndef VOTCA_XTP_SIGMA_BASE_H
#define VOTCA_XTP_SIGMA_BASE_H

// Local VOTCA includes
#include "eigen.h"

namespace votca {
namespace xtp {

class TCMatrix_gwbse;
class RPA;

class Sigma_base {
 public:
  Sigma_base(TCMatrix_gwbse& Mmn, const RPA& rpa) : _Mmn(Mmn), _rpa(rpa){};

  virtual ~Sigma_base() = default;

  struct options {
    Index homo;
    Index qpmin;
    Index qpmax;
    Index rpamin;
    Index rpamax;
    double eta;
    std::string quadrature_scheme;  // Gaussian-quadrature scheme to use in CDA
    Index order;  // used in numerical integration of CDA Sigma
    double alpha;
  };

  void configure(options opt) {
    _opt = opt;
    _qptotal = opt.qpmax - opt.qpmin + 1;
    _rpatotal = opt.rpamax - opt.rpamin + 1;
  }

  // Calculates full exchange matrix
  Eigen::MatrixXd CalcExchangeMatrix() const;
  // Calculates correlation diagonal
  Eigen::VectorXd CalcCorrelationDiag(const Eigen::VectorXd& frequencies) const;
  // Calculates correlation off-diagonal
  Eigen::MatrixXd CalcCorrelationOffDiag(
      const Eigen::VectorXd& frequencies) const;

  // Sets up the screening parametrisation
  virtual void PrepareScreening() = 0;
  // Calculates Sigma_c diagonal elements
  virtual double CalcCorrelationDiagElementDerivative(
      Index gw_level, double frequency) const = 0;
  virtual double CalcCorrelationDiagElement(Index gw_level,
                                            double frequency) const = 0;
  // Calculates Sigma_c off-diagonal elements
  virtual double CalcCorrelationOffDiagElement(Index gw_level1, Index gw_level2,
                                               double frequency1,
                                               double frequency2) const = 0;

 protected:
  options _opt;
  TCMatrix_gwbse& _Mmn;
  const RPA& _rpa;

  Index _qptotal = 0;
  Index _rpatotal = 0;
};
}  // namespace xtp
}  // namespace votca

#endif  // VOTCA_XTP_SIGMA_BASE_H
