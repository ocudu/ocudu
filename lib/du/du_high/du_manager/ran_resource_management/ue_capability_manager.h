// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_drx_resource_manager.h"
#include "du_ue_resource_config.h"
#include "ue_capability_summary.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/du/du_cell_config.h"
#include "ocudu/du/du_high/du_test_mode_config.h"
#include "ocudu/ocudulog/logger.h"

namespace asn1::rrc_nr {

struct ue_nr_cap_s;

} // namespace asn1::rrc_nr

namespace ocudu {

struct scheduler_expert_config;

namespace odu {

/// Helper function to extract a summary of the UE capabilities based on a packed ASN.1 container.
expected<ue_capability_summary, std::string> decode_ue_nr_cap_container(const byte_buffer& ue_cap_container);

/// Helper function to convert advanced UE NR capabilities.
void decode_advanced_ue_nr_caps(ue_capability_summary& ue_capability, const asn1::rrc_nr::ue_nr_cap_s& ue_cap);

/// Helper function to convert advanced UE NR capabilities.
void decode_advanced_ue_nr_ntn_caps(ue_capability_summary& ue_capability, const asn1::rrc_nr::ue_nr_cap_s& ue_cap);

/// Entity responsible for handling the UE RAT capabilities container, passed by the CU-CP, and updating the UE
/// configuration in the DU accordingly.
class ue_capability_manager
{
public:
  explicit ue_capability_manager(span<const du_cell_config> cell_cfg_list,
                                 du_drx_resource_manager&   drx_mng_,
                                 ocudulog::basic_logger&    logger_,
                                 const du_test_mode_config& test_mode_);

  /// \brief Called on creation of a new UE.
  ///
  /// This method is used to set the right default values for several of the UE resources that depend on the UE
  /// capabilities.
  void handle_ue_creation(du_ue_resource_config& ue_res_cfg);

  /// \brief Called on each UE capability RAT list container received from the CU-CP, to update the UE DU-specific
  /// resources.
  /// \param[in/out] ue_res_cfg current UE resource configuration, which will be updated inplace, depending on the UE
  /// capabilities received.
  /// \param[in] ue_cap_rat_list packed UE capability RAT list container.
  void update(du_ue_resource_config& ue_res_cfg, const byte_buffer& ue_cap_rat_list);

  /// \brief Stores a previously decoded UE capability RAT list container and updates the UE DU-specific resources.
  /// \param[in/out] ue_res_cfg current UE resource configuration, which will be updated inplace, depending on the UE
  /// capabilities received.
  /// \param[in] summary unpacked UE capability RAT list container.
  void update(du_ue_resource_config& ue_res_cfg, const ue_capability_summary& summary);

  /// \brief Called on UE removal, to release its resources.
  void release(du_ue_resource_config& ue_res_cfg);

  /// Retrieve a summary of the UE capabilities.
  const std::optional<ue_capability_summary>& summary() const { return ue_caps; }

private:
  // Helper function to decode a packed UE capability RAT list. Returns true, if the packed container was successfully
  // decoded and processed.
  bool decode_ue_capability_list(const byte_buffer& ue_cap_rat_list);

  void update_impl(du_ue_resource_config& ue_res_cfg);

  pusch_mcs_table select_pusch_mcs_table(du_cell_index_t cell_idx) const;

  /// Selects the PUSCH transmission codebook subset.
  tx_scheme_codebook_subset select_tx_codebook_subset(du_cell_index_t cell_idx) const;
  /// Selects the SRS transmission number of ports.
  unsigned select_srs_nof_ports(du_cell_index_t cell_idx) const;
  /// Selects the PUSCH maximum number of layers.
  unsigned select_pusch_max_rank(du_cell_index_t cell_idx) const;
  /// Selects the maximum number of UL HARQ processes.
  unsigned select_max_ul_nof_harqs(du_cell_index_t cell_idx) const;
  /// Selects the UL HARQ Process Number field size.
  unsigned select_ul_dci_harq_num_field_size(du_cell_index_t cell_idx) const;
  /// Selects the UL HARQ Mode B.
  harq_ul_mode_mask select_ul_harq_mode(du_cell_index_t cell_idx) const;

  // Updates the DRX config of the UE.
  void update_drx(du_ue_resource_config& ue_res_cfg);

  span<const du_cell_config> base_cell_cfg_list;
  // Allocator of DRX and measGap resources for the cellGroup.
  du_drx_resource_manager&   drx_res_mng;
  ocudulog::basic_logger&    logger;
  const du_test_mode_config& test_cfg;

  // Flag that tells whether this is the first time that update is called.
  std::optional<ue_capability_summary> ue_caps;
};

} // namespace odu
} // namespace ocudu
