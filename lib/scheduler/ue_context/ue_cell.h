// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../cell/cell_harq_manager.h"
#include "../config/ue_configuration.h"
#include "../support/pucch_power_controller.h"
#include "../support/pusch_power_controller.h"
#include "ue_channel_state_manager.h"
#include "ue_drx_controller.h"
#include "ue_fsm_states.h"
#include "ue_link_adaptation_controller.h"
#include "ocudu/scheduler/config/scheduler_expert_config.h"
#include "ocudu/scheduler/scheduler_feedback_handler.h"

namespace ocudu {

struct ul_crc_pdu_indication;
struct pdsch_config_params;
struct pusch_config_params;

/// State shared across UE carriers.
struct ue_shared_context {
  /// DRX controller.
  ue_drx_controller& drx_ctrl;
};

/// State of the UE PCell.
struct ue_pcell_state {
  /// RRC configuration progress state.
  /// \note When in fallback mode, only the search spaces and the configuration of cellConfigCommon are used.
  ue_config_state config_st = ue_config_state::config_applied;
  /// Contention resolution state.
  ue_conres_state conres_st = ue_conres_state::conres_completed;
  /// MSG3 rx-slot, set for RACH-created UEs (conres_st == pending_conres_ce).
  slot_point msg3_rx_slot;
  /// Set to true when the UE is expecting a CFRA.
  bool cfra_pending = false;
};

struct ue_cell_components {
  /// State relative to the PCell of the UE, if applicable.
  ue_pcell_state*                pcell_state          = nullptr;
  ue_channel_state_manager*      channel_state        = nullptr;
  ue_link_adaptation_controller* ue_mcs_calculator    = nullptr;
  pusch_power_controller*        pusch_pwr_controller = nullptr;
  pucch_power_controller*        pucch_pwr_controller = nullptr;
};

/// \brief Context respective to a UE serving cell.
class ue_cell
{
public:
  ue_cell(du_ue_index_t                ue_index_,
          rnti_t                       crnti_val,
          const ue_cell_configuration& ue_cell_cfg_,
          cell_harq_manager&           cell_harq_pool,
          ue_shared_context            shared_ctx,
          const ue_cell_components&    components,
          ocudulog::basic_logger&      logger_);

  const du_ue_index_t   ue_index;
  const du_cell_index_t cell_index;

  unique_ue_harq_entity harqs;

  rnti_t rnti() const { return crnti_; }

  bwp_id_t                active_bwp_id() const { return to_bwp_id(0); }
  const sched_bwp_config& active_bwp() const { return cfg().bwp(active_bwp_id()); }

  /// \brief Determines whether the UE cell is currently active.
  bool is_active() const { return active; }

  /// Whether the UE is in fallback mode.
  bool is_in_fallback_mode() const
  {
    return components.pcell_state != nullptr and
           is_in_fallback(components.pcell_state->config_st, components.pcell_state->conres_st);
  }

  const ue_cell_configuration& cfg() const { return *ue_cfg; }

  /// \brief Deactivates cell.
  void deactivate();

  void handle_reconfiguration_request(const ue_cell_configuration& ue_cell_cfg);

  bool is_pdcch_enabled(slot_point dl_slot) const
  {
    return active and cfg().is_dl_enabled(dl_slot) and shared_ctx.drx_ctrl.is_pdcch_enabled();
  }
  bool is_pdsch_enabled(slot_point pdcch_slot, slot_point pdsch_slot) const
  {
    // Verify that the DL is activated for the chosen slots (e.g. they are not UL slots in TDD or there is no measGap).
    return is_pdcch_enabled(pdcch_slot) and (pdcch_slot == pdsch_slot or cfg().is_dl_enabled(pdsch_slot)) and
           // Verify only one PDSCH exists for the same RNTI in the same slot, and that the PDSCHs are in the same order
           // as PDCCHs. [TS 38.214, 5.1] "For any HARQ process ID(s) in a given scheduled cell, the UE is not expected
           // to receive a PDSCH that overlaps in time with another PDSCH". [TS 38.214, 5.1] "For any two HARQ process
           // IDs in a given scheduled cell, if the UE is scheduled to start receiving a first PDSCH starting in symbol
           // j by a PDCCH ending in symbol i, the UE is not expected to be scheduled to receive a PDSCH starting
           // earlier than the end of the first PDSCH with a PDCCH that ends later than symbol i.".
           (not harqs.last_pdsch_slot().valid() or harqs.last_pdsch_slot() < pdsch_slot) and
           // Verify that CQI > 0, meaning that the UE is not out-of-reach.
           channel_state_manager().get_wideband_cqi() != csi_report_wideband_cqi_type{0};
  }
  bool is_pusch_enabled(slot_point pdcch_slot, slot_point pusch_slot) const
  {
    // Verify that the DL is activated for the chosen PDCCH slot and UL is activated for the chosen PUSCH slot
    // (e.g the chosen slots fall in DL and UL slots of a TDD pattern respective, and there is no measGap).
    return is_pdcch_enabled(pdcch_slot) and cfg().is_ul_enabled(pusch_slot) and
           // Verify that the order of PUSCHs for the same UE matches the order of PDCCHs and that there is at most one
           // PUSCH per slot. [TS 38.214, 6.1] "For any HARQ process ID(s) in a given scheduled cell, the UE is not
           // expected to transmit a PUSCH that overlaps in time with another PUSCH". [TS 38.214, 6.1] "For any two HARQ
           // process IDs in a given scheduled cell, if the UE is scheduled to start a first PUSCH transmission starting
           // in symbol j by a PDCCH ending in symbol i, the UE is not expected to be scheduled to transmit a PUSCH
           // starting earlier than the end of the first PUSCH by a PDCCH that ends later than symbol i".
           (not harqs.last_pusch_slot().valid() or harqs.last_pusch_slot() < pusch_slot);
  }

  std::optional<dl_harq_process_handle> handle_dl_ack_info(slot_point                 uci_slot,
                                                           mac_harq_ack_report_status ack_value,
                                                           unsigned                   harq_bit_idx,
                                                           std::optional<float>       pucch_snr);

  uint8_t get_pdsch_rv(unsigned nof_retxs) const
  {
    return cell_cfg.expert_cfg.ue.pdsch_rv_sequence[nof_retxs % cell_cfg.expert_cfg.ue.pdsch_rv_sequence.size()];
  }
  uint8_t get_pusch_rv(unsigned nof_retxs) const
  {
    return cell_cfg.expert_cfg.ue.pusch_rv_sequence[nof_retxs % cell_cfg.expert_cfg.ue.pusch_rv_sequence.size()];
  }

  /// \brief Handle CRC PDU indication.
  expected<units::bytes> handle_crc_pdu(slot_point pusch_slot, const ul_crc_pdu_indication& crc_pdu);

  /// \brief Handle Sounding Reference Signal (SRS) channel matrix.
  void handle_srs_channel_matrix(const srs_channel_matrix& channel_matrix);

  /// Update UE with the latest CSI report for a given cell.
  void handle_csi_report(const csi_report_data& csi_report);

  sch_mcs_index get_ul_mcs(pusch_mcs_table mcs_table, bool use_transform_precoder) const
  {
    return components.ue_mcs_calculator->calculate_ul_mcs(mcs_table, use_transform_precoder);
  }

  /// \brief Get recommended aggregation level for PDCCH at a given CQI.
  aggregation_level get_aggregation_level(float cqi, const search_space_info& ss_info, bool is_dl) const;

  /// \brief Get list of recommended Search Spaces given the UE current state and channel quality.
  /// \param[in] required_dci_rnti_type Optional parameter to filter Search Spaces by DCI RNTI config type.
  /// \return List of SearchSpace configuration.
  static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP>
  get_active_dl_search_spaces(slot_point                             pdcch_slot,
                              std::optional<dci_dl_rnti_config_type> required_dci_rnti_type = {}) const;
  static_vector<const search_space_info*, MAX_NOF_SEARCH_SPACE_PER_BWP>
  get_active_ul_search_spaces(slot_point                             pdcch_slot,
                              std::optional<dci_ul_rnti_config_type> required_dci_rnti_type = {}) const;

  /// \brief Get UE channel state handler.
  ue_channel_state_manager&       channel_state_manager() { return *components.channel_state; }
  const ue_channel_state_manager& channel_state_manager() const { return *components.channel_state; }

  const ue_link_adaptation_controller& link_adaptation_controller() const { return *components.ue_mcs_calculator; }

  pusch_power_controller&       get_pusch_power_controller() { return *components.pusch_pwr_controller; }
  const pusch_power_controller& get_pusch_power_controller() const { return *components.pusch_pwr_controller; }

  pucch_power_controller&       get_pucch_power_controller() { return *components.pucch_pwr_controller; }
  const pucch_power_controller& get_pucch_power_controller() const { return *components.pucch_pwr_controller; }

  /// \brief Returns an estimated DL rate in bytes per slot based on the given input parameters.
  double get_estimated_dl_rate(const pdsch_config_params& pdsch_cfg, sch_mcs_index mcs, unsigned nof_prbs) const;
  /// \brief Returns an estimated UL rate in bytes per slot based on the given input parameters.
  double get_estimated_ul_rate(const pusch_config_params& pusch_cfg, sch_mcs_index mcs, unsigned nof_prbs) const;

  bool is_pcell() const { return components.pcell_state != nullptr; }

  /// Retrieve the current Pcell state of the UE, if applicable.
  const ue_pcell_state& get_pcell_state() const
  {
    ocudu_assert(components.pcell_state != nullptr, "Invalid access to Pcell state for SCell");
    return *components.pcell_state;
  }

private:
  /// \brief Performs link adaptation procedures such as cancelling HARQs etc.
  void apply_link_adaptation_procedures(const csi_report_data& csi_report);

  rnti_t                            crnti_;
  const cell_configuration&         cell_cfg;
  const ue_cell_configuration*      ue_cfg;
  const scheduler_ue_expert_config& expert_cfg;
  ue_shared_context                 shared_ctx;
  ue_cell_components                components;
  ocudulog::basic_logger&           logger;

  /// \brief Whether cell is currently active.
  bool active = true;
};

} // namespace ocudu
