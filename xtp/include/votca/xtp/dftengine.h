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
#ifndef VOTCA_XTP_DFTENGINE_H
#define VOTCA_XTP_DFTENGINE_H

// VOTCA includes
#include <votca/tools/property.h>

// Local VOTCA includes
#include "ERIs.h"
#include "convergenceacc.h"
#include "ecpaobasis.h"
#include "logger.h"
#include "staticsite.h"
#include "vxc_grid.h"
#include "vxc_potential.h"

namespace votca {
namespace xtp {
class Orbitals;

/**
 * \brief Electronic ground-state via Density-Functional Theory
 *
 * Evaluates electronic ground state in molecular systems based on
 * density functional theory with Gaussian Orbitals.
 *
 */

class DFTEngine {
 public:
  void Initialize(tools::Property& options);

  void setLogger(Logger* pLog) { _pLog = pLog; }

  void setExternalcharges(
      std::vector<std::unique_ptr<StaticSite> >* externalsites) {
    _externalsites = externalsites;
    _addexternalsites = true;
  }

  bool Evaluate(Orbitals& orb);

  std::string getDFTBasisName() const { return _dftbasis_name; };

 private:
  void Prepare(QMMolecule& mol);

  Vxc_Potential<Vxc_Grid> SetupVxc(const QMMolecule& mol);

  Eigen::MatrixXd OrthogonalizeGuess(const Eigen::MatrixXd& GuessMOs) const;
  void PrintMOs(const Eigen::VectorXd& MOEnergies, Log::Level level);
  void CalcElDipole(const Orbitals& orb) const;

  std::array<Eigen::MatrixXd, 2> CalcERIs_EXX(const Eigen::MatrixXd& MOCoeff,
                                              const Eigen::MatrixXd& Dmat,
                                              double error) const;

  Eigen::MatrixXd CalcERIs(const Eigen::MatrixXd& Dmat, double error) const;

  void ConfigOrbfile(Orbitals& orb);
  void SetupInvariantMatrices();

  Mat_p_Energy SetupH0(const QMMolecule& mol) const;
  Mat_p_Energy IntegrateExternalMultipoles(
      const QMMolecule& mol,
      const std::vector<std::unique_ptr<StaticSite> >& multipoles) const;
  Mat_p_Energy IntegrateExternalDensity(const QMMolecule& mol,
                                        const Orbitals& extdensity) const;

  Eigen::MatrixXd IntegrateExternalField(const QMMolecule& mol) const;

  tools::EigenSystem IndependentElectronGuess(const Mat_p_Energy& H0) const;
  tools::EigenSystem ModelPotentialGuess(
      const Mat_p_Energy& H0, const QMMolecule& mol,
      const Vxc_Potential<Vxc_Grid>& vxcpotential) const;

  Eigen::MatrixXd AtomicGuess(const QMMolecule& mol) const;

  Eigen::MatrixXd RunAtomicDFT_unrestricted(const QMAtom& uniqueAtom) const;

  double NuclearRepulsion(const QMMolecule& mol) const;
  double ExternalRepulsion(
      const QMMolecule& mol,
      const std::vector<std::unique_ptr<StaticSite> >& multipoles) const;
  Eigen::MatrixXd SphericalAverageShells(const Eigen::MatrixXd& dmat,
                                         const AOBasis& dftbasis) const;
  Logger* _pLog;

  // basis sets
  std::string _auxbasis_name;
  std::string _dftbasis_name;
  std::string _ecp_name;
  AOBasis _dftbasis;
  AOBasis _auxbasis;
  ECPAOBasis _ecp;

  bool _with_ecp;

  std::string _four_center_method;  // direct | cache

  // Pre-screening
  bool _with_screening;
  double _screening_eps;

  // numerical integration Vxc
  std::string _grid_name;

  // AO Matrices
  AOOverlap _dftAOoverlap;

  bool _with_guess;
  std::string _initial_guess;

  // Convergence
  Index _numofelectrons = 0;
  Index _max_iter;
  ConvergenceAcc::options _conv_opt;
  // DIIS variables
  ConvergenceAcc _conv_accelerator;
  // Electron repulsion integrals
  ERIs _ERIs;

  // external charges
  std::vector<std::unique_ptr<StaticSite> >* _externalsites;
  bool _addexternalsites = false;

  // exchange and correlation
  double _ScaHFX;
  std::string _xc_functional_name;

  bool _integrate_ext_density = false;
  // integrate external density
  std::string _orbfilename;
  std::string _gridquality;
  std::string _state;

  Eigen::Vector3d _extfield = Eigen::Vector3d::Zero();
  bool _integrate_ext_field = false;
};

}  // namespace xtp
}  // namespace votca

#endif  // VOTCA_XTP_DFTENGINE_H
