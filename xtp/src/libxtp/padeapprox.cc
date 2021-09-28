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

#include <fstream>
#include <math.h>
#include <votca/tools/property.h>
#include <votca/xtp/logger.h>
#include <votca/xtp/orbitals.h>
#include <votca/xtp/padeapprox.h>

namespace votca {
namespace xtp {

void PadeApprox::clear() {

  _value.clear();

  _grid.clear();

  _coeff.clear();

  _rejected_points = 0;
}

void PadeApprox::addPoint(std::complex<double> frequency,
                          std::complex<double> value) {

  this->_grid.push_back(frequency);
  this->_value.push_back(value);
  this->_coeff.push_back(RecursivePolynom(_grid.size() - 1, _grid.size()));

  if ((_coeff.at(_coeff.size() - 1) != _coeff.at(_coeff.size() - 1)) ||
      abs(_coeff.at(_coeff.size() - 1)) < 1e-6) {
    // std::cout
    //<< "reject point, unvalid coeficient at w=" << frequency
    //<< std::endl <<"Value="<<value<<std::endl<<std::endl;
    _coeff.pop_back();
    _grid.pop_back();
    _value.pop_back();
    for (Index i = 0; i < _num_points; i++) {
      _temp_container_g[_grid.size() - 1][i] = 0;
    }
    _rejected_points++;
  }
}

std::complex<double> PadeApprox::RecursivePolynom(Index indx, Index degree) {

  if (degree == 1) {
    return _value.at(indx);
  } else if (real(this->_temp_container_g[indx][degree - 1]) != 0) {
    return this->_temp_container_g[indx][degree - 1];
  } else {
    std::complex<double> temp = RecursivePolynom(indx, degree - 1);
    std::complex<double> u = RecursivePolynom(degree - 2, degree - 1) - temp;
    std::complex<double> l = temp * (_grid.at(indx) - _grid.at(degree - 2));
    std::complex<double> result = u / l;
    if (result == result) {
      _temp_container_g[indx][degree - 1] = result;
    }
    return result;
  }
}

std::complex<double> PadeApprox::RecursiveA(std::complex<double> frequency,
                                            Index index) {

  if (Index(_temp_container_A.size()) > index) {
    return _temp_container_A[index];
  } else {
    std::complex<double> A =
        ((frequency - _grid.at(index - 2)) * _coeff.at(index - 1) *
             RecursiveA(frequency, index - 2) +
         RecursiveA(frequency, index - 1));
    _temp_container_A.push_back(A);
    return A;
  }
}

std::complex<double> PadeApprox::RecursiveB(std::complex<double> frequency,
                                            Index index) {

  if ( Index(_temp_container_B.size()) > index) {
    return _temp_container_B[index];
  } else {
    std::complex<double> B =
        ((frequency - _grid.at(index - 2)) * _coeff.at(index - 1) *
             RecursiveB(frequency, index - 2) +
         RecursiveB(frequency, index - 1));
    _temp_container_B.push_back(B);
    return B;
  }
}

void PadeApprox::initialize(Index num_points) {
  this->_temp_container_g.resize(num_points);
  for (Index i = 0; i < num_points; i++) {
    _temp_container_g[i].resize(num_points);
  }
  this->_num_points = num_points;
}

std::complex<double> PadeApprox::evaluatePoint(std::complex<double> frequency) {
  _temp_container_A.clear();
  _temp_container_B.clear();
  _temp_container_A.push_back(std::complex<double>(0, 0));
  _temp_container_A.push_back(_coeff.at(0));
  _temp_container_B.push_back(std::complex<double>(1, 0));
  _temp_container_B.push_back(std::complex<double>(1, 0));
  std::complex<double> B = RecursiveB(frequency, _grid.size());
  std::complex<double> A = RecursiveA(frequency, _grid.size());

  return A / B;
}
}  // namespace xtp
}  // namespace votca
