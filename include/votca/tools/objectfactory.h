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

#ifndef VOTCA_TOOLS_OBJECTFACTORY_H
#define VOTCA_TOOLS_OBJECTFACTORY_H

// Standard includes
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

// Third party includes
#include <boost/lexical_cast.hpp>

namespace votca {
namespace tools {

/**
    \brief template class for object factory

    This class is a template for an object factory. The factory creates an
   instance of an derived class, be giving a key (e.g. a string) for the object
   which identifies it uniquely. This allows the implementation of new features
   (e.g. new file formats, new mapping algorithms) without touching or
   recompiling existing bits of code.

    If you don't understand this, read the book by Alexandresku (Modern C++
   design) everything explained there in detail!
*/
template <typename key_t, typename T>
class ObjectFactory {
 private:
  using creator_t = std::unique_ptr<T> (*)();

 public:
  ObjectFactory() = default;
  virtual ~ObjectFactory() = default;

  /**
   * \brief register an object
   * \param key identifier
   * \param creator create policy
   *
   * This function is called to register an object in the factory. After an
   * object is registered, an instance of it can be created by calling Create
   * specifying the corresponding key.
   */
  void Register(const key_t &key, creator_t creator);

  template <typename obj_t>
  void Register(const key_t &key);

  /**
     Create an instance of the object identified by key.
  */
  virtual std::unique_ptr<T> Create(const key_t &key);
  bool IsRegistered(const key_t &id_) const;

  std::vector<key_t> getKeys() const {
    std::vector<key_t> key;
    key.reserve(objects_.size());
    for (const auto &pair : objects_) {
      key.push_back(pair.first);
    }
    return key;
  }

 private:
  std::map<key_t, creator_t> objects_;
};

template <class parent, class T>
std::unique_ptr<parent> create_policy_new() {
  return std::make_unique<T>();
}

template <typename key_t, typename T>
inline void ObjectFactory<key_t, T>::Register(const key_t &key,
                                              creator_t creator) {
  (void)objects_
      .insert(typename std::map<key_t, creator_t>::value_type(key, creator))
      .second;
}

template <typename key_t, typename T>
template <typename obj_t>
inline void ObjectFactory<key_t, T>::Register(const key_t &key) {
  Register(key, create_policy_new<T, obj_t>);
}

template <typename key_t, typename T>
inline std::unique_ptr<T> ObjectFactory<key_t, T>::Create(const key_t &key) {
  typename std::map<key_t, creator_t>::const_iterator it = objects_.find(key);
  if (it != objects_.end()) {
    return (it->second)();
  } else {
    throw std::runtime_error(
        "factory key " + boost::lexical_cast<std::string>(key) + " not found.");
  }
}

template <typename key_t, typename T>
inline bool ObjectFactory<key_t, T>::IsRegistered(const key_t &id_) const {
  return (objects_.find(id_) != objects_.end());
}

}  // namespace tools
}  // namespace votca

#endif  // VOTCA_TOOLS_OBJECTFACTORY_H
