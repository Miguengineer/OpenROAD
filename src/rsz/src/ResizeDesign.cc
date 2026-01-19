// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, The OpenROAD Authors

#include "ResizeDesign.hh"

#include "rsz/Resizer.hh"

namespace rsz {

using utl::RSZ;

ResizeDesign::ResizeDesign(Resizer* resizer) : resizer_(resizer)
{
}

ResizeDesign::~ResizeDesign() = default;

void ResizeDesign::init()
{
  logger_ = resizer_->logger_;
  dbStaState::init(resizer_->sta_);
  db_network_ = resizer_->db_network_;
  initialized_ = true;
}

void ResizeDesign::resizeDesign(const double effort,
                               const double max_utilization,
                               const bool verbose)
{
  if (!initialized_) {
    init();
  }

  // Sanity check inputs (Tcl interface already does some validation).
  if (effort < 0.0) {
    logger_->warn(RSZ, 2000, "resize_design: effort {} < 0; clamping to 0.0", effort);
  }
  if (max_utilization < 0.0 || max_utilization > 1.0) {
    logger_->error(RSZ,
                   2001,
                   "resize_design: max_utilization {} must be in [0.0, 1.0]",
                   max_utilization);
  }

  // This component is currently a thin driver that reuses the existing repair
  // flow. The structure matches RepairDesign so the implementation can evolve
  // without adding more responsibilities to Resizer.
  resizer_->initBlock();
  resizer_->setMaxUtilization(max_utilization);

  // Map effort into a conservative buffer gain. Keep defaults safe.
  const double effort_clamped = std::max(0.0, std::min(1.0, effort));
  const double buffer_gain = 1.0 + effort_clamped;  // [1.0, 2.0]

  // Use the computed max wire length. If it can't be computed, allow repair
  // without a wire-length constraint.
  double max_wire_length = 0.0;
  try {
    max_wire_length = resizer_->findMaxWireLength(false /* issue_error */);
  } catch (...) {
    max_wire_length = 0.0;
  }

  // Reuse existing repairDesign pipeline. This is a practical default until
  // a dedicated resize loop is implemented here.
  constexpr double slew_margin = 0.0;
  constexpr double cap_margin = 0.0;
  constexpr bool match_cell_footprint = false;
  resizer_->repairDesign(max_wire_length,
                         slew_margin,
                         cap_margin,
                         buffer_gain,
                         match_cell_footprint,
                         verbose);
}

}  // namespace rsz

