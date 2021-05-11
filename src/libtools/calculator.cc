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

// Local VOTCA includes
#include "votca/tools/calculator.h"
#include "votca/tools/globals.h"
#include "votca/tools/tokenizer.h"
#include <algorithm>

namespace votca {
namespace tools {

Property Calculator::LoadDefaults(const std::string package) {

  std::string calculator_name = Identify();
  // add default values if specified in VOTCASHARE
  std::string votca_share = GetVotcaShare();

  // load the xml description of the calculator (with defaults and test values)
  std::string xmlFile = votca_share + std::string("/") + package +
                        std::string("/xml/") + calculator_name +
                        std::string(".xml");

  Property defaults_all;
  defaults_all.LoadFromXML(xmlFile);
  return defaults_all.get("options." + calculator_name);
}

void Calculator::UpdateWithUserOptions(Property &default_options,
                                       const Property &user_options) {

  // copy options from the object supplied by the Application
  std::string calculator_name = Identify();
  Property options_id = user_options.get("options." + calculator_name);

  // if a value is given override default values
  OverwriteDefaultsWithUserInput(options_id, default_options);
}

void Calculator::OverwriteDefaultsWithUserInput(const Property &p,
                                                Property &defaults) {

  // Go through everything that is defined in user option
  for (const Property &prop : p) {
    if (prop.HasChildren()) {
      if (defaults.exists(prop.name())) {
        OverwriteDefaultsWithUserInput(prop, defaults.get(prop.name()));
      } else {
        Property &new_prop = defaults.add(prop.name(), "");
        new_prop = prop;
      }
    } else if (defaults.exists(prop.name()) && (prop.value() != "")) {
      defaults.set(prop.name(), prop.value());
    } else if (prop.value() != "") {
      defaults.add(prop.name(), prop.value());
    }
  }
}

std::vector<std::string> Calculator::GetPropertyChoices(const Property &p) {
  if (p.hasAttribute("choices")) {
    std::string att = p.getAttribute<std::string>("choices");
    std::size_t start_bracket = att.find('[');
    if (start_bracket != std::string::npos) {
      std::size_t end_bracket = att.find(']');
      att = att.substr(start_bracket + 1, end_bracket - start_bracket - 1);
    }
    return Tokenizer{att, " ,"}.ToVector();
  } else {
    return {""};
  }
}

void Calculator::RecursivelyCheckOptions(const Property &p) {
  for (const Property &prop : p) {
    if (prop.HasChildren()) {
      RecursivelyCheckOptions(prop);
    } else {
      std::vector<std::string> choices = Calculator::GetPropertyChoices(prop);
      const std::string &head = choices.front();
      if (head != "") {
        if (!Calculator::IsValidOption(prop, choices)) {
          std::ostringstream oss;
          oss << "\nThe input value for \"" << prop.name() << "\"";
          if (choices.size() == 1) {
            oss << " should be a \"" << head << "\"";
          } else {
            oss << " should be one of the following values: ";
            for (const std::string &c : choices) {
              oss << "\"" << c << "\""
                  << " ";
            }
          }
          oss << " But \"" << prop.value()
              << "\" cannot be converted into one.\n";
          std::cout << oss.str();
          throw std::runtime_error(oss.str());
        }
      }
    }
  }
}

bool Calculator::IsValidOption(const Property &prop,
                               const std::vector<std::string> &choices) {
  const std::string &head = choices.front();
  std::ostringstream oss;
  bool is_valid = true;
  if (head == "bool") {
    is_valid = Calculator::IsValidCast<bool>(prop);
  } else if (head == "float") {
    is_valid = Calculator::IsValidCast<double>(prop);
  } else if (head == "float+") {
    is_valid =
        Calculator::IsValidCast<double>(prop) && (prop.as<double>() >= 0.0);
  } else if (head == "int") {
    is_valid = Calculator::IsValidCast<Index>(prop);
  } else if (head == "int+") {
    is_valid = Calculator::IsValidCast<Index>(prop) && (prop.as<Index>() >= 0);
  } else {
    std::string value = prop.as<std::string>();
    std::string att = prop.getAttribute<std::string>("choices");
    bool has_brackets = att.find('[') != std::string::npos;
    if (!has_brackets) {
      // There is a single choice out of multiple default valid choices
      is_valid = std::find(std::cbegin(choices), std::cend(choices), value) !=
                 std::cend(choices);
    } else {
      // there are multiple valid choices
      Tokenizer tok{value, " ,"};
      is_valid =
          std::all_of(tok.begin(), tok.end(), [&](const std::string &word) {
            return std::find(std::cbegin(choices), std::cend(choices), word) !=
                   std::cend(choices);
          });
    }
  }
  return is_valid;
}

void Calculator::InjectDefaultsAsValues(Property &defaults) {
  for (Property &prop : defaults) {
    if (prop.HasChildren()) {
      InjectDefaultsAsValues(prop);
    } else {
      if (prop.hasAttribute("default") && (prop.value() == "")) {
        prop.set(".", prop.getAttribute<std::string>("default"));
      }
    }
  }
}

}  // namespace tools
}  // namespace votca
