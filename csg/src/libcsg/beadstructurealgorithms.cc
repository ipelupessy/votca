/*
 * Copyright 2009-2020 The VOTCA Development Team (http://www.votca.org)
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

// VOTCA includes
#include <votca/tools/graphalgorithm.h>

// Local VOTCA includes
#include "votca/csg/beadstructurealgorithms.h"

using namespace std;
using namespace votca::tools;

namespace votca {
namespace csg {

vector<BeadStructure> breakIntoStructures(BeadStructure &beadstructure) {
  vector<BeadStructure> structures;
  if (beadstructure.isSingleStructure()) {
    structures.push_back(beadstructure);
  } else {

    vector<Graph> sub_graphs =
        decoupleIsolatedSubGraphs(beadstructure.getGraph());
    for (Graph &sub_graph : sub_graphs) {
      vector<Edge> sub_graph_edges = sub_graph.getEdges();
      vector<Index> sub_graph_vertices = sub_graph.getVertices();
      structures.push_back(
          beadstructure.getSubStructure(sub_graph_vertices, sub_graph_edges));
    }
  }
  return structures;
}

}  // namespace csg
}  // namespace votca
