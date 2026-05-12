// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ue_capability_summary.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/du/du_cell_config.h"
#include "ocudu/ocudulog/logger.h"

namespace asn1::rrc_nr {

struct ue_nr_cap_s;

} // namespace asn1::rrc_nr

namespace ocudu {

struct scheduler_expert_config;

namespace odu {

/// Helper function to extract a summary of the UE capabilities based on a packed ASN.1 container.
expected<ue_capability_summary, std::string> decode_ue_nr_cap_container(const byte_buffer& ue_cap_container);

/// Entity responsible for decoding and storing the UE RAT capabilities container passed by the CU-CP.
class ue_capability_manager
{
public:
  explicit ue_capability_manager(span<const du_cell_config> cell_cfg_list, ocudulog::basic_logger& logger_);

  /// \brief Decodes and stores the UE capability RAT list container.
  /// \param[in] ue_cap_rat_list packed UE capability RAT list container.
  bool update(const byte_buffer& ue_cap_rat_list);

  /// \brief Stores a previously decoded UE capability summary.
  /// \param[in] summary unpacked UE capability summary.
  void update(const ue_capability_summary& summary);

  /// Retrieve a summary of the UE capabilities.
  const std::optional<ue_capability_summary>& summary() const { return ue_caps; }

private:
  span<const du_cell_config>           base_cell_cfg_list;
  ocudulog::basic_logger&              logger;
  std::optional<ue_capability_summary> ue_caps;
};

} // namespace odu
} // namespace ocudu
