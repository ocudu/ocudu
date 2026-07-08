// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "bwp_config_res_pool.h"
#include "ocudu/adt/slotted_vector.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/config/scheduler_expert_config.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include <variant>

namespace ocudu {

/// Holds the configuration of a cell.
/// Additionally, this class pre-caches the computation of some const values related to the cell configuration
/// and provide parameter getter helpers.
class cell_configuration
{
public:
  explicit cell_configuration(const scheduler_expert_config&                  expert_cfg,
                              const sched_cell_configuration_request_message& msg,
                              const sched_bwp_config&                         init_bwp);
  cell_configuration(const cell_configuration&) = delete;
  cell_configuration(cell_configuration&&)      = delete;

  /// Expert parameters used to generate the cell configuration.
  const scheduler_expert_config& expert_cfg;

  /// Meta parameters used to generate the cell configuration.
  const ran_cell_config params;

  const du_cell_index_t       cell_index;
  const du_cell_group_index_t cell_group_index;
  /// List of RRM Policy members configured for this cell.
  std::vector<slice_rrm_policy_config> rrm_policy_members;

  /// @name Derived parameters.
  /// Parameters derived from \ref ran_cell_config.
  ///@{

  const unsigned nof_dl_prbs;
  const unsigned nof_ul_prbs;
  const unsigned nof_slots_per_frame;
  /// List of zp-CSI-RS resources.
  const std::vector<zp_csi_rs_resource> zp_csi_rs_list;
  /// List of nzp-CSI-RS resources.
  const std::vector<nzp_csi_rs_resource> nzp_csi_rs_list;
  /// Initial BWP configuration for this cell.
  const sched_bwp_config& init_bwp;
  /// List of dl-DataToUL-ACK values sent to UE in its dedicated configuration.
  span<const uint8_t> dl_data_to_ul_ack;
  /// List of BWP config resources handled by this cell.
  slotted_id_vector<bwp_id_t, bwp_config_res_pool> bwp_res;

  ///@}

  /// Cell-specific K-offset in slots defined by the cell subcarrier spacing.
  unsigned ntn_cs_koffset;

  subcarrier_spacing scs_common() const { return params.dl_cfg_common.init_dl_bwp.generic_params.scs; }

  /// Checks if the cell is configured in TDD mode.
  bool is_tdd() const { return params.tdd_cfg.has_value(); }

  /// Checks if DL is active for all symbols in the given slot.
  bool is_fully_dl_enabled(slot_point sl) const
  {
    if (dl_symbols_per_slot_lst.empty()) {
      // Note: dl_enabled_slot_lst is empty in the FDD case.
      return true;
    }
    if (sl.numerology() != to_numerology_value(params.tdd_cfg->ref_scs)) {
      // Convert slot into equivalent reference SCS.
      sl = set_slot_numerology(sl, to_numerology_value(params.tdd_cfg->ref_scs));
    }
    return dl_symbols_per_slot_lst[sl.to_uint() % dl_symbols_per_slot_lst.size()] ==
           get_nsymb_per_slot(params.dl_cfg_common.init_dl_bwp.generic_params.cp);
  }

  /// Checks if UL is active for all symbols in the given slot.
  bool is_fully_ul_enabled(slot_point sl) const
  {
    if (ul_symbols_per_slot_lst.empty()) {
      // Note: ul_enabled_slot_lst is empty in the FDD case.
      return true;
    }
    if (sl.numerology() != to_numerology_value(params.tdd_cfg->ref_scs)) {
      // Convert slot into equivalent reference SCS.
      sl = set_slot_numerology(sl, to_numerology_value(params.tdd_cfg->ref_scs));
    }
    return ul_symbols_per_slot_lst[sl.to_uint() % ul_symbols_per_slot_lst.size()] ==
           get_nsymb_per_slot(params.ul_cfg_common.init_ul_bwp.generic_params.cp);
  }

  /// Checks if DL is active for at least one symbol in the given slot.
  bool is_dl_enabled(slot_point sl) const
  {
    if (dl_symbols_per_slot_lst.empty()) {
      // Note: dl_enabled_slot_lst is empty in the FDD case.
      return true;
    }
    if (sl.numerology() != to_numerology_value(params.tdd_cfg->ref_scs)) {
      // Convert slot into equivalent reference SCS.
      sl = set_slot_numerology(sl, to_numerology_value(params.tdd_cfg->ref_scs));
    }
    return dl_symbols_per_slot_lst[sl.to_uint() % dl_symbols_per_slot_lst.size()] > 0;
  }

  /// Checks if UL is active for at least one symbol in the given slot.
  bool is_ul_enabled(slot_point sl) const
  {
    if (ul_symbols_per_slot_lst.empty()) {
      // Note: ul_enabled_slot_lst is empty in the FDD case.
      return true;
    }
    if (sl.numerology() != to_numerology_value(params.tdd_cfg->ref_scs)) {
      // Convert slot into equivalent reference SCS.
      sl = set_slot_numerology(sl, to_numerology_value(params.tdd_cfg->ref_scs));
    }
    return ul_symbols_per_slot_lst[sl.to_uint() % ul_symbols_per_slot_lst.size()] > 0;
  }

  bool cfra_enabled() const { return params.init_bwp.rach.cfra_enabled; }

  bool paired_spectrum() const { return band_helper::is_paired_spectrum(params.dl_carrier.band); }

  nr_band band() const { return params.dl_carrier.band; }

  /// Returns the number of active DL symbols in the given slot.
  unsigned get_nof_dl_symbol_per_slot(slot_point sl) const
  {
    if (dl_symbols_per_slot_lst.empty()) {
      // Note: dl_enabled_slot_lst is empty in the FDD case.
      return get_nsymb_per_slot(params.dl_cfg_common.init_dl_bwp.generic_params.cp);
    }
    if (sl.numerology() != to_numerology_value(params.tdd_cfg->ref_scs)) {
      // Convert slot into equivalent reference SCS.
      sl = set_slot_numerology(sl, to_numerology_value(params.tdd_cfg->ref_scs));
    }
    return dl_symbols_per_slot_lst[sl.to_uint() % dl_symbols_per_slot_lst.size()];
  }

  /// Returns the number of active UL symbols in the given slot.
  unsigned get_nof_ul_symbol_per_slot(slot_point sl) const
  {
    if (ul_symbols_per_slot_lst.empty()) {
      // Note: ul_enabled_slot_lst is empty in the FDD case.
      return get_nsymb_per_slot(params.ul_cfg_common.init_ul_bwp.generic_params.cp);
    }
    if (sl.numerology() != to_numerology_value(params.tdd_cfg->ref_scs)) {
      // Convert slot into equivalent reference SCS.
      sl = set_slot_numerology(sl, to_numerology_value(params.tdd_cfg->ref_scs));
    }
    return ul_symbols_per_slot_lst[sl.to_uint() % ul_symbols_per_slot_lst.size()];
  }

  /// Returns the coreset configuration for the given coreset ID.
  const coreset_configuration& get_common_coreset(coreset_id cs_id) const
  {
    // The existence of the Coreset (either CommonCoreset or Coreset0) has been verified by the validator.
    return params.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.has_value() and
                   params.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value().get_id() == cs_id
               ? params.dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value()
               : params.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.value();
  }

  /// Checks if the cell is configured with PUCCH Format 0 and Format 2 resources.
  bool is_pucch_f0_and_f2() const
  {
    return std::holds_alternative<pucch_f0_params>(params.init_bwp.pucch.resources.f0_or_f1_params) and
           std::holds_alternative<pucch_f2_params>(params.init_bwp.pucch.resources.f2_or_f3_or_f4_params);
  }

  /// \brief Determines the use of transform precoding according to the parameter \e msg3-transformPrecoder.
  ///
  /// The UE determines the use of the transform precoder in TS 38.214 Section 6.1.3. It uses this parameter when a
  /// PUSCH is scheduled by:
  /// - a RAR UL grant;
  /// - DCI Format 0_0; or
  /// - DCI Format 0_1 and the parameter \e transformPrecoder in \e pusch-Config is not present.
  bool use_msg3_transform_precoder() const
  {
    if (not params.ul_cfg_common.init_ul_bwp.rach_cfg_common) {
      return false;
    }
    return params.ul_cfg_common.init_ul_bwp.rach_cfg_common->msg3_transform_precoder;
  }

private:
  /// Vector circularly indexed by slot with the list of nof active DL/UL symbols per slot.
  std::vector<unsigned> dl_symbols_per_slot_lst;
  std::vector<unsigned> ul_symbols_per_slot_lst;
};

/// \brief List of common configurations for the cells currently added in the scheduler.
///
/// Note: Pointers to cell configurations should remain valid on cell removal/addition.
using cell_common_configuration_list = slotted_id_vector<du_cell_index_t, cell_configuration*>;

} // namespace ocudu
