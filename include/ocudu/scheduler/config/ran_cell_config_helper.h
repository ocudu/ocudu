// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/carrier_configuration.h"
#include "ocudu/ran/configured_grant/cg_configuration.h"
#include "ocudu/ran/pdcch/aggregation_level.h"
#include "ocudu/ran/pdcch/coreset.h"
#include "ocudu/scheduler/config/cell_config_builder_params.h"
#include "ocudu/scheduler/config/ran_cell_config.h"

namespace ocudu::config_helpers {

/// Config struct that extends cell_config_builder_params with parameters that can be derived from the former.
struct cell_config_builder_params_extended : public cell_config_builder_params {
  cell_config_builder_params_extended(const cell_config_builder_params& source = {});

  /// \brief Absolute frequency of the SSB as ARFCN. This is the ARFCN of the \c SS_ref (or SSB central frequency).
  /// \c SS_ref is defined is per TS 38.104, Section 5.4.3.1 and 5.4.3.2.
  std::optional<arfcn_t> ssb_arfcn;
  unsigned               cell_nof_crbs;
  /// \brief Maximum number of DL layers. Derived from \c max_rank if configured, otherwise from the number of DL
  /// antenna ports, and in all cases bounded by the single-codeword layer limit.
  unsigned max_nof_layers;
};

/// Generates a default DL carrier configuration based on the input parameters.
carrier_configuration make_default_dl_carrier_configuration(const cell_config_builder_params_extended& params);

/// Generates a default UL carrier configuration based on the input parameters.
carrier_configuration make_default_ul_carrier_configuration(const cell_config_builder_params_extended& params);

/// Generates a default SSB configuration based on the input parameters.
ssb_configuration make_default_ssb_config(const cell_config_builder_params_extended& params);

/// Generates default RAN cell configuration used by gNB DU. The default configuration should be valid.
ran_cell_config make_default_ran_cell_config(const cell_config_builder_params_extended& params = {});

/// Builds CSI meas config builder parameters from the RAN cell configuration.
csi_helper::csi_meas_config_builder_params make_csi_meas_config_builder_params(const ran_cell_config& cell_cfg);

/// Builds a CORESET configuration from the input parameters and a CORESET ID.
coreset_configuration make_default_coreset_config(const config_helpers::cell_config_builder_params_extended& params,
                                                  coreset_id cs_id = to_coreset_id(1));

/// Builds a Search Space configuration from the input parameters.
search_space_configuration
make_default_common_search_space_config(const config_helpers::cell_config_builder_params_extended& params = {});

/// Compute the maximum number of candidates that can be accommodated in a CORESET for a given aggregation level.
uint8_t compute_max_nof_candidates(aggregation_level aggr_lvl, const coreset_configuration& cs_cfg);

/// \brief Builds the cell-default Configured Grant configuration from the cell-level CG parameters.
/// \remark This function must only be called if the cell has CG enabled (i.e. \c cell_cfg.init_bwp.cg_cfg is set).
cg_configuration make_default_cell_cg_config(const ran_cell_config& cell_cfg);

/// \brief Computes the number of PRBs required per UE by a Configured Grant, derived from the CG grant size or
/// bitrate and the configured MCS.
/// \remark This function must only be called if the cell has CG enabled (i.e. \c cell_cfg.init_bwp.cg_cfg is set).
unsigned compute_nof_cg_prbs_per_ue(const ran_cell_config& cell_cfg, const cg_configuration& cg_cfg);

} // namespace ocudu::config_helpers
