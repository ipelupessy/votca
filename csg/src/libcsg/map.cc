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

// Standard includes
#include <numeric>
#include <string>

// VOTCA includes
#include <votca/tools/eigen.h>
#include <votca/tools/tokenizer.h>

// Local VOTCA includes
#include "votca/csg/bead.h"
#include "votca/csg/map.h"
#include "votca/csg/topology.h"

namespace votca {
namespace csg {
class Molecule;
}  // namespace csg
namespace tools {
class Property;
}  // namespace tools
}  // namespace votca

namespace votca {
namespace csg {
using namespace tools;
using namespace std;

/*******************************************************
  Linear map for spherical beads
 *******************************************************/
class Map_Sphere : public BeadMap {
 public:
  Map_Sphere() = default;
  void Apply(const BoundaryCondition &) override;

  virtual void Initialize(const Molecule *in, Bead *out,
                          tools::Property *opts_bead,
                          tools::Property *opts_map) override;

 protected:
  void AddElem(const Bead *in, double weight, double force_weight);

  struct element_t {
    const Bead *in_;
    double weight_;
    double force_weight_;
  };
  std::vector<element_t> matrix_;
};

void Map_Sphere::AddElem(const Bead *in, double weight, double force_weight) {
  element_t el;
  el.in_ = in;
  el.weight_ = weight;
  el.force_weight_ = force_weight;
  matrix_.push_back(el);
}

/*******************************************************
  Linear map for ellipsoidal bead
 *******************************************************/
class Map_Ellipsoid : public Map_Sphere {
 public:
  Map_Ellipsoid() = default;
  void Apply(const BoundaryCondition &) final;
};

void Map::Apply(const BoundaryCondition &bc) {
  for (auto &map_ : maps_) {
    map_->Apply(bc);
  }
}

void Map_Sphere::Initialize(const Molecule *in, Bead *out, Property *opts_bead,
                            Property *opts_map) {

  in_ = in;
  out_ = out;
  opts_map_ = opts_map;
  opts_bead_ = opts_bead;

  vector<double> fweights;

  // get the beads
  string s(opts_bead_->get("beads").value());
  vector<string> beads = Tokenizer(s, " \n\t").ToVector();

  // get vector of weights
  Tokenizer tok_weights(opts_map_->get("weights").value(), " \n\t");
  vector<double> weights = tok_weights.ToVector<double>();

  // check weather weights and # beads matches
  if (beads.size() != weights.size()) {
    throw runtime_error(
        string("number of subbeads in " + opts_bead->get("name").as<string>() +
               " and number of weights in map " +
               opts_map->get("name").as<string>() + " do not match"));
  }

  // normalize the weights
  double norm = 1. / std::accumulate(weights.begin(), weights.end(), 0.);

  transform(weights.begin(), weights.end(), weights.begin(),
            [&norm](double w) { return w * norm; });
  // get the d vector if exists or initialize same as weights
  vector<double> d;
  if (opts_map_->exists("d")) {
    Tokenizer tok_weights2(opts_map_->get("d").value(), " \n\t");
    d = tok_weights2.ToVector<double>();
    // normalize d coefficients
    norm = 1. / std::accumulate(d.begin(), d.end(), 0.);
    transform(d.begin(), d.end(), d.begin(),
              [&norm](double w) { return w * norm; });
  } else {
    // initialize force-weights with weights
    d.resize(weights.size());
    copy(weights.begin(), weights.end(), d.begin());
  }

  // check weather number of d coeffs is correct
  if (beads.size() != d.size()) {
    throw runtime_error(
        string("number of subbeads in " + opts_bead->get("name").as<string>() +
               " and number of d-coefficients in map " +
               opts_map->get("name").as<string>() + " do not match"));
  }

  fweights.resize(weights.size());
  // calculate force weights by d_i/w_i
  for (size_t i = 0; i < weights.size(); ++i) {
    if (weights[i] == 0 && d[i] != 0) {
      throw runtime_error(
          "A d coefficient is nonzero while weights is zero in mapping " +
          opts_map->get("name").as<string>());
    }
    if (weights[i] != 0) {
      fweights[i] = d[i] / weights[i];
    } else {
      fweights[i] = 0;
    }
  }

  for (size_t i = 0; i < beads.size(); ++i) {
    Index iin = in->getBeadByName(beads[i]);
    if (iin < 0) {
      throw std::runtime_error(
          string("mapping error: molecule " + beads[i] + " does not exist"));
    }
    AddElem(in->getBead(iin), weights[i], fweights[i]);
  }
}

void Map_Sphere::Apply(const BoundaryCondition &bc) {

  assert(matrix_.size() > 0 && "Cannot map to sphere there are no beads");

  bool bPos, bVel, bF;
  bPos = bVel = bF = false;
  out_->ClearParentBeads();

  // the following is needed for pbc treatment
  Eigen::Vector3d r0 = Eigen::Vector3d::Zero();
  string name0;
  Index id0 = 0;
  if (matrix_.size() > 0) {
    if (matrix_.front().in_->HasPos()) {
      r0 = matrix_.front().in_->getPos();
      name0 = matrix_.front().in_->getName();
      id0 = matrix_.front().in_->getId();
    }
  }

  double M = 0;
  Eigen::Vector3d cg = Eigen::Vector3d::Zero();
  Eigen::Vector3d f = Eigen::Vector3d::Zero();
  Eigen::Vector3d vel = Eigen::Vector3d::Zero();

  const Bead *bead_max_dist = matrix_.at(0).in_;
  double max_bead_dist = 0;
  if (bead_max_dist->HasPos()) {
    max_bead_dist = bc.BCShortestConnection(r0, bead_max_dist->getPos()).norm();
  }

  for (const auto &iter : matrix_) {
    const Bead *bead = iter.in_;
    out_->AddParentBead(bead->getId());
    M += bead->getMass();
    if (bead->HasPos()) {
      Eigen::Vector3d r = bc.BCShortestConnection(r0, bead->getPos());
      if (r.norm() > max_bead_dist) {
        max_bead_dist = r.norm();
        bead_max_dist = bead;
      }
      cg += iter.weight_ * (r + r0);
      bPos = true;
    }
  }

  /// Safety check, if box is not open check if the bead is larger than the
  /// boundaries
  if (bc.getBoxType() != BoundaryCondition::eBoxtype::typeOpen) {
    double max_dist = 0.5 * bc.getShortestBoxDimension();
    if (max_bead_dist > max_dist) {
      cout << r0 << " " << bead_max_dist->getPos() << endl;
      throw std::runtime_error(
          "coarse-grained bead is bigger than half the box \n "
          "(atoms " +
          name0 + " (id " + boost::lexical_cast<string>(id0 + 1) + ")" + ", " +
          bead_max_dist->getName() + " (id " +
          boost::lexical_cast<string>(bead_max_dist->getId() + 1) + ")" +
          +" , molecule " +
          boost::lexical_cast<string>(bead_max_dist->getMoleculeId() + 1) +
          ")");
    }
  }

  for (const auto &iter : matrix_) {
    const Bead *bead = iter.in_;
    if (bead->HasVel()) {
      vel += iter.weight_ * bead->getVel();
      bVel = true;
    }
    if (bead->HasF()) {
      f += iter.force_weight_ * bead->getF();
      bF = true;
    }
  }
  out_->setMass(M);
  if (bPos) {
    out_->setPos(cg);
  }
  if (bVel) {
    out_->setVel(vel);
  }
  if (bF) {
    out_->setF(f);
  }
}

/// \todo implement this function
void Map_Ellipsoid::Apply(const BoundaryCondition &bc) {

  assert(matrix_.size() > 0 && "Cannot map to ellipsoid there are no beads");

  bool bPos, bVel, bF;
  bPos = bVel = bF = false;

  // the following is needed for pbc treatment
  Eigen::Vector3d r0 = Eigen::Vector3d::Zero();
  string name0;
  Index id0 = 0;
  if (matrix_.size() > 0) {
    if (matrix_.front().in_->HasPos()) {
      r0 = matrix_.front().in_->getPos();
      name0 = matrix_.front().in_->getName();
      id0 = matrix_.front().in_->getId();
    }
  }
  Eigen::Vector3d cg = Eigen::Vector3d::Zero();
  Eigen::Vector3d c = Eigen::Vector3d::Zero();
  Eigen::Vector3d f = Eigen::Vector3d::Zero();
  Eigen::Vector3d vel = Eigen::Vector3d::Zero();

  Index n;
  n = 0;
  out_->ClearParentBeads();

  const Bead *bead_max_dist = matrix_.at(0).in_;
  double max_bead_dist = 0;
  if (bead_max_dist->HasPos()) {
    max_bead_dist = bc.BCShortestConnection(r0, bead_max_dist->getPos()).norm();
  }

  for (const auto &iter : matrix_) {
    const Bead *bead = iter.in_;
    out_->AddParentBead(bead->getId());
    if (bead->HasPos()) {
      Eigen::Vector3d r = bc.BCShortestConnection(r0, bead->getPos());
      if (r.norm() > max_bead_dist) {
        max_bead_dist = r.norm();
        bead_max_dist = bead;
      }
      cg += iter.weight_ * (r + r0);
      bPos = true;
    }
  }

  /// Safety check, if box is not open check if the bead is larger than the
  /// boundaries
  if (bc.getBoxType() != BoundaryCondition::eBoxtype::typeOpen) {
    double max_dist = 0.5 * bc.getShortestBoxDimension();
    if (max_bead_dist > max_dist) {
      cout << r0 << " " << bead_max_dist->getPos() << endl;
      throw std::runtime_error(
          "coarse-grained bead is bigger than half the box \n "
          "(atoms " +
          name0 + " (id " + boost::lexical_cast<string>(id0 + 1) + ")" + ", " +
          bead_max_dist->getName() + " (id " +
          boost::lexical_cast<string>(bead_max_dist->getId() + 1) + ")" +
          +" , molecule " +
          boost::lexical_cast<string>(bead_max_dist->getMoleculeId() + 1) +
          ")");
    }
  }

  for (const auto &iter : matrix_) {
    const Bead *bead = iter.in_;
    if (bead->HasVel() == true) {
      vel += iter.weight_ * bead->getVel();
      bVel = true;
    }
    if (bead->HasF()) {
      /// \todo fix me, right calculation should be F_i = m_cg / sum(w_i) *
      /// sum(w_i/m_i*F_i)
      // f += (*iter). weight_ *  in_->getBeadF((*iter). in_);
      f += iter.force_weight_ * bead->getF();
      bF = true;
    }

    if (iter.weight_ > 0 && bead->HasPos()) {
      c += bead->getPos();
      n++;
    }
  }

  if (bPos) {
    out_->setPos(cg);
  }
  if (bVel) {
    out_->setVel(vel);
  }
  if (bF) {
    out_->setF(f);
  }

  if (!matrix_[0].in_->HasPos()) {
    out_->setU(Eigen::Vector3d::UnitX());
    out_->setV(Eigen::Vector3d::UnitY());
    out_->setW(Eigen::Vector3d::UnitZ());
    return;
  }

  Eigen::Matrix3d m = Eigen::Matrix3d::Zero();
  // calculate the tensor of gyration
  c = c / (double)n;
  for (auto &iter : matrix_) {
    if (iter.weight_ == 0) {
      continue;
    }
    const Bead *bead = iter.in_;
    Eigen::Vector3d v = bead->getPos() - c;
    // v = vec(1, 0.5, 0) * 0.*(drand48()-0.5)
    //    + vec(0.5, -1, 0) * (drand48()-0.5)
    //    + vec(0, 0, 1) * (drand48()-0.5);

    // Normalize the tensor with 1/number_of_atoms_per_bead
    m += v * v.transpose() / (double)matrix_.size();
  }

  Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eig;
  eig.computeDirect(m);

  Eigen::Vector3d u = eig.eigenvectors().col(0);
  Eigen::Vector3d v = matrix_[1].in_->getPos() - matrix_[0].in_->getPos();
  v.normalize();

  out_->setV(v);

  Eigen::Vector3d w = matrix_[2].in_->getPos() - matrix_[0].in_->getPos();
  w.normalize();

  if (v.cross(w).dot(u) < 0) {
    u = -u;
  }
  out_->setU(u);

  // write out w
  w = u.cross(v);
  w.normalize();
  out_->setW(w);
}

Map::Map(Map &&map)
    : in_(map.in_), out_(map.out_), maps_(std::move(map.maps_)) {}

Map &Map::operator=(Map &&map) {
  in_ = map.in_;
  out_ = map.out_;
  maps_ = std::move(map.maps_);
  return *this;
}

BeadMap *Map::CreateBeadMap(const BeadMapType type) {
  if (type == BeadMapType::Spherical) {
    maps_.push_back(std::make_unique<Map_Sphere>());
  } else {
    maps_.push_back(std::make_unique<Map_Ellipsoid>());
  }
  return maps_.back().get();
}

}  // namespace csg
}  // namespace votca
