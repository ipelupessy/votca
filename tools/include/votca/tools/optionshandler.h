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

#ifndef VOTCA_TOOLS_OPTIONSHANDLER_H
#define VOTCA_TOOLS_OPTIONSHANDLER_H

// Local VOTCA includes
#include "property.h"

namespace votca {
namespace tools {

class OptionsHandler {
 public:
  OptionsHandler(std::string defaults_path) : defaults_path_(defaults_path) {}

  /**
   * \brief Load the default options and merge them with the user input
   *
   * Defaults are overwritten with user input
   */
  Property ProcessUserInput(const Property &user_input,
                            const std::string &calcname) const;

  /**
   * \brief Resolve links and return all the options of a calculator
   *
   */
  Property CalculatorOptions(const std::string &calcname) const;

  /**
   * \brief Allows to bypass the choice evaluation for a specific calculator
   *
   */
  void setAdditionalChoices(std::vector<std::string> choices) {
    additional_choices_ = choices;
  }

 private:
  /**
   * \brief Resolves "link" attribute in the Property by filling in defaults.
   * Already existing tags are not overwritten
   */
  void ResolveLinks(tools::Property &prop) const;

  /**
   * \brief Loads default options stored in defaults_path_
   */
  Property LoadDefaults(const std::string &calculatorname) const;

  /**
   * \brief Checks if all keywords in user_input (apart from sections named
   * "unchecked") have corresponding keys in defaults
   */
  void CheckUserInput(const Property &user_input,
                      const Property &defaults) const;

  /**
   * \brief Checks that all options with default="REQUIRED" are filled in
   */
  void CheckRequired(const Property &options) const;

  /**
   * \brief Removes tags which have no value and default="OPTIONAL"
   */
  void RemoveOptional(Property &options) const;

  void InjectDefaultsAsValues(Property &options) const;

  void CheckChoices(const Property &options) const;

  void CleanAttributes(Property &options,
                       const std::vector<std::string> &attributes) const;

  void OverwriteDefaultsWithUserInput(const Property &p,
                                      Property &defaults) const;
  // Copy the defaults into the value
  void RecursivelyCheckOptions(const Property &p) const;
  bool IsValidOption(const Property &prop,
                     const std::vector<std::string> &choices) const;
  static std::vector<std::string> GetPropertyChoices(const Property &p);

  std::string defaults_path_;

  std::vector<std::string> additional_choices_{};
  std::vector<std::string> reserved_keywords_{"OPTIONAL", "REQUIRED"};
};

}  // namespace tools
}  // namespace votca

#endif  // VOTCA_TOOLS_OPTIONSHANDLER_H
