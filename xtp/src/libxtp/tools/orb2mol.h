
#pragma once
#ifndef VOTCA_XTP_ORB2MOL_H
#define VOTCA_XTP_ORB2MOL_H

// VOTCA includes
#include <votca/tools/constants.h>

// Local VOTCA includes
#include "votca/xtp/logger.h"
#include "votca/xtp/orbreorder.h"
#include "votca/xtp/qmtool.h"
#include <fstream>
#include <votca/xtp/orbitals.h>

namespace votca {
namespace xtp {

class Orb2Mol final : public QMTool {
 public:
  Orb2Mol() = default;

  ~Orb2Mol() = default;

  std::string Identify() { return "orb2mol"; }

 protected:
  void ParseOptions(const tools::Property& user_options);
  bool Run();

 private:
  // clang-format off
  std::string _moldenfile;
  std::string _orbfile;
  std::string _xyzfile;
  std::string _basisset_name;
  std::string _aux_basisset_name;
  Logger _log;
};

}  // namespace xtp
}  // namespace votca

#endif // VOTCA_XTP_ORB2MOL_H
