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

#define BOOST_TEST_MAIN

#define BOOST_TEST_MODULE beadtriple_test

// Standard includes
#include <string>
#include <tuple>

// Third party includes
#include <boost/test/unit_test.hpp>

// Local VOTCA includes
#include "votca/csg/bead.h"
#include "votca/csg/beadtriple.h"
#include "votca/csg/topology.h"

using namespace std;
using namespace votca::csg;

BOOST_AUTO_TEST_SUITE(beadtriple_test)

BOOST_AUTO_TEST_CASE(test_beadtriple_constructor) {

  Topology top;

  string bead_type_name = "CG";

  string name = "dummy1";
  votca::Index resnr = 0;
  double mass = 1.0;
  double charge = -1.0;

  top.CreateBead(Bead::spherical, name, bead_type_name, resnr, mass, charge);

  name = "dummy2";
  resnr = 0;
  mass = 1.0;
  charge = -1.0;

  top.CreateBead(Bead::spherical, name, bead_type_name, resnr, mass, charge);

  name = "dummy3";
  resnr = 0;
  mass = 1.0;
  charge = -1.0;

  top.CreateBead(Bead::spherical, name, bead_type_name, resnr, mass, charge);

  Eigen::Vector3d dist12(0.1, 0.2, 0.3);
  Eigen::Vector3d dist13(0.2, 0.4, 0.3);
  Eigen::Vector3d dist23(0.1, 0.2, 0.0);

  BeadTriple testtriple(top.getBead(0), top.getBead(1), top.getBead(2), dist12,
                        dist13, dist23);

  double d12ref = 0.3741657;
  double d13ref = 0.5385165;
  double d23ref = 0.2236068;

  double d12 = testtriple.dist12();
  double d13 = testtriple.dist13();
  double d23 = testtriple.dist23();

  BOOST_CHECK_CLOSE(d12, d12ref, 1e-4);
  BOOST_CHECK_CLOSE(d13, d13ref, 1e-4);
  BOOST_CHECK_CLOSE(d23, d23ref, 1e-4);
}

BOOST_AUTO_TEST_SUITE_END()
