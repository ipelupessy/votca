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
#include <iostream>

// Third party includes
#include <boost/program_options.hpp>

// VOTCA includes
#include <votca/tools/property.h>
#include <votca/tools/tokenizer.h>

// Local VOTCA includes
#include "votca/csg/version.h"

using namespace std;
using namespace votca::csg;
using namespace votca::tools;

void help_text() {
  votca::csg::HelpTextHeader("csg_property");
  cout << "Helper program called by inverse scripts to parse xml file.\n\n";
}

int main(int argc, char **argv) {
  string filter, file, path, print;
  bool short_output = false;
  bool with_path = false;

  // lets read in some program options
  namespace po = boost::program_options;

  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce this help message")(
      "path", po::value<string>(&path)->default_value(""),
      "path to part of the xml file to print")(
      "filter", po::value<string>(&filter)->default_value(""),
      "list option values that match given criteria")(
      "print", po::value<string>(&print)->default_value("."),
      "specifies which children or root to print")(
      "file", po::value<string>(&file), "xml file to parse")(
      "short", "short version of output")("with-path",
                                          "include path of node in output");

  // now read in the command line
  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  } catch (po::error &err) {
    cout << "error parsing command line: " << err.what() << endl;
    return -1;
  }
  // does the user want help?
  if (vm.count("help")) {
    help_text();
    cout << desc << endl;
    return 0;
  }
  // file specified
  if (!vm.count("file")) {
    cout << "please specify file\n";
    cout << desc << endl;
    return -1;
  }
  if (vm.count("short")) {
    short_output = true;
  }
  if (vm.count("with-path")) {
    with_path = true;
  }

  try {
    Property p;
    p.LoadFromXML(file);

    for (Property *prop : p.Select(path)) {
      if (filter != "") {
        Tokenizer tokenizer(filter, "=");
        Tokenizer::iterator tok;
        tok = tokenizer.begin();
        if (tok == tokenizer.end()) {
          throw std::invalid_argument("error, specified invalid filter");
        }

        string field = *tok;
        ++tok;
        if (tok == tokenizer.end()) {
          throw std::invalid_argument("error, specified invalid filter");
        }

        string value = *tok;
        if (!wildcmp(value, prop->get(field).value())) {
          continue;
        }
      }

      std::vector<Property *> printvalues;

      // Select returns empty on "." thus we have to handle that case explicitly
      if (print == ".") {
        printvalues.push_back(&(prop->get(print)));
      } else {
        printvalues = prop->Select(print);
      }

      for (Property *p2 : printvalues) {

        if (!short_output && with_path) {
          cout << p2->path() << ".";
        }
        if (!short_output) {
          cout << p2->name() << " = ";
        }
        cout << p2->value();
        cout << endl;
      }
    }
  } catch (std::exception &error) {
    cerr << "Warning from parsing xml file '" << file << "':\n"
         << error.what() << endl;
    return -1;
  }
  return 0;
}
