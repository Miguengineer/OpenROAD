// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, The OpenROAD Authors

#pragma once

#include "db_sta/dbSta.hh"
#include "rsz/Resizer.hh"
#include "utl/Logger.h"

namespace rsz {

class Resizer;

using sta::dbNetwork;
using sta::dbStaState;

// Logic/area resizing driver.
// This is intentionally structured like RepairDesign (component-style helper
// owned by Resizer).
class ResizeDesign : dbStaState
{
 public:
  explicit ResizeDesign(Resizer* resizer);
  ~ResizeDesign() override;

  // Resize the design using the given effort and utilization cap.
  // - effort: heuristic knob (0.0 .. 1.0 typical)
  // - max_utilization: 0.0 .. 1.0 of core area
  void resizeDesign(double effort, double max_utilization, bool verbose);

 protected:
  void init();
  void resetSlews();
  Logger* logger_ = nullptr;
  dbNetwork* db_network_ = nullptr;
  Resizer* resizer_ = nullptr;
  bool initialized_ = false;
  est::EstimateParasitics* estimate_parasitics_ = nullptr;
  double max_utilization_ = 1.0;
  bool verbose_ = false;
  double initial_design_area_ = 0.0;
  const MinMax* min_ = MinMax::min();
  const MinMax* max_ = MinMax::max();
  
  est::ParasiticsSrc parasitics_src_ = est::ParasiticsSrc::none;
};

}  // namespace rsz

