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
#ifndef VOTCA_XTP_ECPAOSHELL_H
#define VOTCA_XTP_ECPAOSHELL_H

// VOTCA includes
#include <votca/tools/constants.h>

// Local VOTCA includes
#include "ecpbasisset.h"
#include "eigen.h"
#include "qmatom.h"

namespace votca {
namespace xtp {
class ECPAOShell;
class ECPAOGaussianPrimitive {

 public:
  ECPAOGaussianPrimitive(const ECPGaussianPrimitive& gaussian)
      : _power(gaussian._power),
        _decay(gaussian._decay),
        _contraction(gaussian._contraction) {
    ;
  }

  struct data {
    Index atomid;
    Index l;
    Index lmax;
    Index startindex;
    Index power;
    double decay;
    double contraction;
    double x;
    double y;
    double z;
  };

  ECPAOGaussianPrimitive(const ECPAOGaussianPrimitive::data& d) {
    _decay = d.decay;
    _contraction = d.contraction;
    _power = d.power;
  }

  void WriteData(data& d, const ECPAOShell& shell) const;

  void SetupCptTable(CptTable& table) const;

  Index getPower() const { return _power; }
  double getDecay() const { return _decay; }
  double getContraction() const { return _contraction; }

 private:
  Index _power = 0;
  double _decay = 0.0;
  double _contraction = 0.0;
};

/*
 * shells in a Gaussian-basis expansion
 */

class ECPAOBasis;
class ECPAOShell {
  friend ECPAOBasis;

 public:
  ECPAOShell(const ECPShell& shell, const QMAtom& atom, Index startIndex,
             L Lmax)
      : _L(shell.getL()),
        _startIndex(startIndex),
        _pos(atom.getPos()),
        _atomindex(atom.getId()),
        _Lmax_element(Lmax) {
    ;
  }

  ECPAOShell(const ECPAOGaussianPrimitive::data& d) {
    _L = static_cast<L>(d.l);
    _Lmax_element = static_cast<L>(d.lmax);
    _startIndex = d.startindex;
    _atomindex = d.atomid;
    _pos = Eigen::Vector3d(d.x, d.y, d.z);
    _gaussians.push_back(ECPAOGaussianPrimitive(d));
  }

  Index getNumFunc() const { return NumFuncShell(_L); }
  Index getStartIndex() const { return _startIndex; }
  Index getOffset() const { return OffsetFuncShell(_L); }
  Index getAtomIndex() const { return _atomindex; }

  L getL() const { return _L; }
  L getLmaxElement() const { return _Lmax_element; }
  // Local part is with L=Lmax
  bool isNonLocal() const { return (_L < _Lmax_element); }
  const Eigen::Vector3d& getPos() const { return _pos; }

  Index getSize() const { return _gaussians.size(); }

  // iterator over pairs (decay constant; contraction coefficient)
  using ECPGaussianIterator =
      std::vector<ECPAOGaussianPrimitive>::const_iterator;
  ECPGaussianIterator begin() const { return _gaussians.begin(); }
  ECPGaussianIterator end() const { return _gaussians.end(); }

  // adds a Gaussian
  void addGaussian(const ECPGaussianPrimitive& gaussian) {
    _gaussians.push_back(ECPAOGaussianPrimitive(gaussian));
    return;
  }

  friend std::ostream& operator<<(std::ostream& out, const ECPAOShell& shell);

 private:
  L _L;
  Index _startIndex;
  Eigen::Vector3d _pos;
  Index _atomindex;
  L _Lmax_element;  // Lmax of the Element not the shell

  // vector of pairs of decay constants and contraction coefficients
  std::vector<ECPAOGaussianPrimitive> _gaussians;
};
}  // namespace xtp
}  // namespace votca

#endif  // VOTCA_XTP_ECPAOSHELL_H
