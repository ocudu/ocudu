// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/mac/mac_cell_result.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/csi_report/csi_report_data.h"
#include <vector>

namespace ocudu {

/// MAC PDSCH precoding information.
struct mac_pdsch_precoding_info {
  /// \brief CSI-RS report.
  ///
  /// This field is empty for omnidirectional precoding.
  std::optional<precoding_matrix_indicator> report;
};

struct mac_pdcch_precoding_info {};
struct mac_ssb_precoding_info {};
struct mac_csi_rs_precoding_info {};

namespace fapi_adaptor {

/// Precoding matrix mapper codebook offset configuration.
struct precoding_matrix_mapper_codebook_offset_configuration {
  /// Codebook offset for the omnidirectional PDSCH.
  unsigned pdsch_omni_offset;
  /// Codebook offsets for SSB. Each entry represents a layer.
  std::vector<unsigned> ssb_codebook_offsets;
  /// Codebook offsets for PDSCH. Each entry represents a layer.
  std::vector<unsigned> pdsch_codebook_offsets;
  /// Codebook offsets for PDCCH. Each entry represents a layer.
  std::vector<unsigned> pdcch_codebook_offsets;
  /// Codebook offsets for CSI-RS. Each entry represents a layer.
  std::vector<unsigned> csi_rs_codebook_offsets;
};

/// \brief Precoding matrix mapper.
///
/// Maps the given arguments to a precoding matrix index.
class precoding_matrix_mapper
{
public:
  precoding_matrix_mapper(unsigned sector_id_, const precoding_matrix_mapper_codebook_offset_configuration& config);

  /// Maps the given MAC precoding information into a precoding matrix index.
  unsigned map(const mac_pdsch_precoding_info& precoding_info, unsigned nof_layers) const;

  /// Maps the given MAC precoding information into a precoding matrix index.
  unsigned map(const mac_pdcch_precoding_info& precoding_info) const;

  /// Maps the given MAC precoding information into a precoding matrix index.
  unsigned map(const mac_csi_rs_precoding_info& precoding_info) const;

  /// Maps the given MAC precoding information into a precoding matrix index.
  unsigned map(const mac_ssb_precoding_info& precoding_info) const;

private:
  /// Sector identifier;
  const unsigned sector_id;
  /// Logger.
  ocudulog::basic_logger& logger;
  /// Codebook offset for the omnidirectional PDSCH.
  unsigned pdsch_omni_offset;
  /// Codebook offsets for SSB. Each entry represents a layer.
  std::vector<unsigned> ssb_codebook_offsets;
  /// Codebook offsets for PDSCH. Each entry represents a layer.
  std::vector<unsigned> pdsch_codebook_offsets;
  /// Codebook offsets for PDCCH. Each entry represents a layer.
  std::vector<unsigned> pdcch_codebook_offsets;
  /// Codebook offsets for CSI-RS. Each entry represents a layer.
  std::vector<unsigned> csi_rs_codebook_offsets;
};

} // namespace fapi_adaptor
} // namespace ocudu
