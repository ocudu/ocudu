// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ru_ofh_config_validator.h"
#include "ocudu/adt/format.h"
#include "ocudu/ofh/compression/compression_params.h"
#include "ocudu/ofh/compression/compression_validator.h"
#include "ocudu/ran/cyclic_prefix.h"

using namespace ocudu;

/// Validates that the given ports are not duplicated. Returns true on success, otherwise false.
template <typename T>
[[gnu::noinline]] static bool validate_duplicated_ports(span<const T> ports)
{
  std::vector<T> temp_ports(ports.begin(), ports.end());
  std::sort(temp_ports.begin(), temp_ports.end());

  return std::unique(temp_ports.begin(), temp_ports.end()) == temp_ports.end();
}

static bool validate_transmission_window(std::chrono::duration<double, std::micro> symbol_duration,
                                         std::chrono::microseconds                 window_start,
                                         std::chrono::microseconds                 window_end)
{
  return ((window_end - window_start) > symbol_duration);
}

static bool validate_scaling_params(
    const std::variant<std::monostate, ru_ofh_scaling_config, ru_ofh_legacy_scaling_config>& scaling_params_)
{
  if (const auto* scaling_params = std::get_if<ru_ofh_scaling_config>(&scaling_params_)) {
    interval<float, true> ref_level_range(-std::numeric_limits<float>::infinity(), 0.0f);
    interval<float, true> backoff_range(0.0f, std::numeric_limits<float>::infinity());
    if (!ref_level_range.contains(scaling_params->ru_reference_level_dBFS)) {
      fmt::print("RU Reference level, i.e., {} dBFS lies outside of the valid range {}\n",
                 scaling_params->ru_reference_level_dBFS,
                 ref_level_range);
      return false;
    }

    if (scaling_params->subcarrier_rms_backoff_dB &&
        !backoff_range.contains(*scaling_params->subcarrier_rms_backoff_dB)) {
      fmt::print("Subcarrier back-off, i.e., {} dB lies outside of the valid range {}\n",
                 *scaling_params->subcarrier_rms_backoff_dB,
                 backoff_range);
      return false;
    }
  } else if (const auto* legacy_scaling_params = std::get_if<ru_ofh_legacy_scaling_config>(&scaling_params_)) {
    interval<float, true> iq_scaling_range(0.0f, std::numeric_limits<float>::infinity());
    if (!iq_scaling_range.contains(legacy_scaling_params->iq_scaling)) {
      fmt::print("IQ scaling, i.e., {} lies outside of the valid range {}\n",
                 legacy_scaling_params->iq_scaling,
                 iq_scaling_range);
      return false;
    }
  } else {
    fmt::print("Missing IQ scaling configuration!\n");
    return false;
  }

  return true;
}

/// Validates the given downlink beamforming configuration. Returns true on success, otherwise false.
static bool validate_beamforming_config(const ru_ofh_beamforming_config& bf_cfg)
{
  if (auto result = ofh::validate_compression_params(ofh::ru_compression_params{
          .type = ofh::to_compression_type(bf_cfg.compression_method), .data_width = bf_cfg.compression_bitwidth});
      !result.has_value()) {
    fmt::println("Beamforming weights {}", result.error());

    return false;
  }

  return true;
}

/// Validates the given Open Fronthaul Radio Unit application configuration. Returns true on success, otherwise
/// false.
static bool validate_ru_ofh_unit_config(span<const ru_ofh_unit_cell_config>       ofh_cells,
                                        span<const ru_ofh_cell_validation_config> cell_config)
{
  for (unsigned i = 0, e = ofh_cells.size(); i != e; ++i) {
    const ru_ofh_unit_cell_config&       ofh_cell = ofh_cells[i];
    const ru_ofh_cell_validation_config& cell_cfg = cell_config[i];

    if (auto result = ofh::validate_compression_params(
            ofh::ru_compression_params{.type       = ofh::to_compression_type(ofh_cell.cell.compression_method_ul),
                                       .data_width = ofh_cell.cell.compression_bitwidth_ul});
        !result.has_value()) {
      fmt::println("Uplink {}", result.error());

      return false;
    }

    if (auto result = ofh::validate_compression_params(
            ofh::ru_compression_params{.type       = ofh::to_compression_type(ofh_cell.cell.compression_method_dl),
                                       .data_width = ofh_cell.cell.compression_bitwidth_dl});
        !result.has_value()) {
      fmt::println("Downlink {}", result.error());

      return false;
    }

    if (auto result = ofh::validate_compression_params(
            ofh::ru_compression_params{.type       = ofh::to_compression_type(ofh_cell.cell.compression_method_prach),
                                       .data_width = ofh_cell.cell.compression_bitwidth_prach});
        !result.has_value()) {
      fmt::println("PRACH {}", result.error());

      return false;
    }

    if (ofh_cell.cell.dl_beamforming.has_value() && !validate_beamforming_config(*ofh_cell.cell.dl_beamforming)) {
      return false;
    }

    const std::chrono::duration<double, std::micro> symbol_duration(
        (1e3 / (get_nsymb_per_slot(cyclic_prefix::NORMAL) * get_nof_slots_per_subframe(cell_cfg.scs))));

    if (!validate_transmission_window(symbol_duration, ofh_cell.cell.T1a_min_cp_dl, ofh_cell.cell.T1a_max_cp_dl)) {
      fmt::print("Transmission timing window length for DL Control-Plane must be bigger than the symbol duration "
                 "({:.2f}us).\n",
                 symbol_duration.count());
      return false;
    }

    if (!validate_transmission_window(symbol_duration, ofh_cell.cell.T1a_min_cp_ul, ofh_cell.cell.T1a_max_cp_ul)) {
      fmt::print("Transmission timing window length for UL Control-Plane must be bigger than the symbol duration "
                 "({:.2f}us).\n",
                 symbol_duration.count());
      return false;
    }

    if (!validate_transmission_window(symbol_duration, ofh_cell.cell.T1a_min_up, ofh_cell.cell.T1a_max_up)) {
      fmt::print(
          "Transmission timing window length for DL User-Plane must be bigger than the symbol duration ({:.2f}us).\n",
          symbol_duration.count());
      return false;
    }

    if (cell_cfg.nof_antennas_dl != ofh_cell.ru_dl_port_id.size()) {
      fmt::print("RU number of downlink ports={} must match the number of transmission antennas={}\n",
                 ofh_cell.ru_dl_port_id.size(),
                 cell_cfg.nof_antennas_dl);

      return false;
    }

    if (cell_cfg.nof_antennas_ul > ofh_cell.ru_ul_port_id.size()) {
      fmt::print("RU number of uplink ports={} must be equal or greater than the number of reception antennas={}\n",
                 ofh_cell.ru_ul_port_id.size(),
                 cell_cfg.nof_antennas_ul);

      return false;
    }

    if (cell_cfg.nof_antennas_ul > ofh_cell.ru_prach_port_id.size()) {
      fmt::print("RU number of PRACH ports={} must be equal or greater than the number of reception antennas={}\n",
                 ofh_cell.ru_prach_port_id.size(),
                 cell_cfg.nof_antennas_ul);

      return false;
    }

    if (cell_cfg.nof_prach_ports > ofh_cell.ru_prach_port_id.size()) {
      fmt::print("RU number of PRACH ports={} must be equal or greater than the cell number of PRACH ports={}\n",
                 ofh_cell.ru_prach_port_id.size(),
                 cell_cfg.nof_prach_ports);

      return false;
    }

    if (!validate_duplicated_ports<unsigned>(ofh_cell.ru_dl_port_id)) {
      fmt::print("Detected duplicated downlink port identifiers\n");

      return false;
    }

    if (!validate_duplicated_ports<unsigned>(ofh_cell.ru_ul_port_id)) {
      fmt::print("Detected duplicated uplink port identifiers\n");

      return false;
    }

    if (!validate_duplicated_ports<unsigned>(ofh_cell.ru_prach_port_id)) {
      fmt::print("Detected duplicated uplink PRACH port identifiers\n");

      return false;
    }

    if (ofh_cell.ru_dl_port_id.size() > ofh::MAX_NOF_SUPPORTED_EAXC) {
      fmt::println("RU number of downlink ports={} is bigger than supported={}",
                   ofh_cell.ru_dl_port_id.size(),
                   ofh::MAX_NOF_SUPPORTED_EAXC);

      return false;
    }

    if (ofh_cell.ru_ul_port_id.size() > ofh::MAX_NOF_SUPPORTED_EAXC) {
      fmt::println("RU number of uplink ports={} is bigger than supported={}",
                   ofh_cell.ru_ul_port_id.size(),
                   ofh::MAX_NOF_SUPPORTED_EAXC);

      return false;
    }

    if (ofh_cell.ru_prach_port_id.size() > ofh::MAX_NOF_SUPPORTED_EAXC) {
      fmt::println("RU number of PRACH ports={} is bigger than supported={}",
                   ofh_cell.ru_prach_port_id.size(),
                   ofh::MAX_NOF_SUPPORTED_EAXC);

      return false;
    }

    if (!validate_scaling_params(ofh_cell.cell.iq_scaling_config)) {
      return false;
    }
  }

  return true;
}

bool ocudu::validate_ru_ofh_config(const ru_ofh_unit_config&                 config,
                                   span<const ru_ofh_cell_validation_config> cell_config)
{
  if (config.cells.size() != cell_config.size()) {
    fmt::print("Number of cells in the DU={} don't match the number of cells in the RU={}\n",
               cell_config.size(),
               config.cells.size());

    return false;
  }

  // Configuring more cells for expert execution than the number of cells is an error.
  if (config.expert_execution_cfg.cell_affinities.size() != cell_config.size()) {
    fmt::print("Using different number of cells for Open Fronthaul expert execution '{}' than the number of defined "
               "cells '{}'\n",
               config.expert_execution_cfg.cell_affinities.size(),
               cell_config.size());

    return false;
  }

  // Configuring more entries for tx-rx affinities than the number of cells is an error.
  if (config.expert_execution_cfg.txrx_affinities.size() > cell_config.size()) {
    fmt::print("Using more txrx cells for Open Fronthaul expert execution '{}' than the number of defined cells '{}'\n",
               config.expert_execution_cfg.txrx_affinities.size(),
               cell_config.size());
    return false;
  }

  if (!validate_ru_ofh_unit_config(config.cells, cell_config)) {
    return false;
  }

  return true;
}
