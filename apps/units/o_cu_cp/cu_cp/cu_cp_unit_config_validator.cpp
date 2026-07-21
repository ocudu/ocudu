// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_cp_unit_config_validator.h"
#include "ocudu/adt/format.h"
#include "ocudu/adt/span.h"
#include "ocudu/ran/i_rnti.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/pdcp/pdcp_t_reordering.h"
#include "ocudu/rlc/rlc_config.h"
#include <map>
#include <set>
#include <sstream>
#include <vector>

using namespace ocudu;

/// Validates field presence and physical range for event-triggered measurement parameters.
/// Used by both event_triggered and cond_trigger report types.
/// Returns true on success, prints a diagnostic and returns false on error.
static bool validate_event_trigger_params(const cu_cp_unit_report_config& cfg)
{
  if (!cfg.event_triggered_report_type.has_value()) {
    fmt::print("report_cfg_id={}: event_triggered_report_type must be set for event-triggered/cond_trigger report\n",
               cfg.report_cfg_id);
    return false;
  }

  const ocucp::rrc_event_id::event_id_t ev = *cfg.event_triggered_report_type;

  // D/T distance- and time-based events are only valid for cond_trigger report type.
  if (ev == ocucp::rrc_event_id::event_id_t::d1 || ev == ocucp::rrc_event_id::event_id_t::t1 ||
      ev == ocucp::rrc_event_id::event_id_t::d2) {
    if (cfg.report_type != "cond_trigger") {
      fmt::print("report_cfg_id={}: event '{}' is only valid for report_type=cond_trigger\n",
                 cfg.report_cfg_id,
                 to_string(ev));
      return false;
    }
    if (ev == ocucp::rrc_event_id::event_id_t::d1 || ev == ocucp::rrc_event_id::event_id_t::d2) {
      // Common D1/D2: distance thresholds, hysteresis, and time-to-trigger are mandatory.
      if (!cfg.distance_thresh_from_ref1_km.has_value() || !cfg.distance_thresh_from_ref2_km.has_value()) {
        fmt::print(
            "report_cfg_id={}: {} event requires distance_thresh_from_ref1_km and distance_thresh_from_ref2_km\n",
            cfg.report_cfg_id,
            to_string(ev));
        return false;
      }
      if (!cfg.hysteresis_location_km.has_value()) {
        fmt::print("report_cfg_id={}: {} event requires hysteresis_location_km\n", cfg.report_cfg_id, to_string(ev));
        return false;
      }
      if (!cfg.time_to_trigger_ms.has_value()) {
        fmt::print("report_cfg_id={}: {} event requires time_to_trigger_ms\n", cfg.report_cfg_id, to_string(ev));
        return false;
      }
      // D1-only: range check (ASN.1 uses 50 m steps, upper bound 65535 -> 3276.75 km) and ref locations.
      if (ev == ocucp::rrc_event_id::event_id_t::d1) {
        if (*cfg.distance_thresh_from_ref1_km > 3276.75 || *cfg.distance_thresh_from_ref2_km > 3276.75) {
          fmt::print("report_cfg_id={}: D1 distance thresholds must be in [0..3276.75] km\n", cfg.report_cfg_id);
          return false;
        }
        if (!cfg.ref_location1.has_value() || !cfg.ref_location2.has_value()) {
          fmt::print("report_cfg_id={}: D1 event requires ref_location1 and ref_location2\n", cfg.report_cfg_id);
          return false;
        }
      }
    } else if (ev == ocucp::rrc_event_id::event_id_t::t1) {
      if (!cfg.t1_thres.has_value()) {
        fmt::print("report_cfg_id={}: T1 event requires t1_thres\n", cfg.report_cfg_id);
        return false;
      }
      if (!cfg.duration.has_value() || cfg.duration->count() < 0.1 || cfg.duration->count() > 600.0) {
        fmt::print("report_cfg_id={}: T1 event requires duration_s in [0.1..600]\n", cfg.report_cfg_id);
        return false;
      }
    }
    return true;
  }

  // A-family events (a1-a6): require meas_trigger_quantity, hysteresis_db, time_to_trigger_ms.
  if (!cfg.meas_trigger_quantity.has_value() || !cfg.hysteresis_db.has_value() || !cfg.time_to_trigger_ms.has_value()) {
    fmt::print("report_cfg_id={}: meas_trigger_quantity, hysteresis_db, and time_to_trigger_ms are required\n",
               cfg.report_cfg_id);
    return false;
  }

  // Hysteresis range: [0..15] dB (ASN.1 encodes as value × 2, so [0..30] in 0.5 dB steps).
  if (*cfg.hysteresis_db > 15) {
    fmt::print("report_cfg_id={}: hysteresis_db={} out of range [0..15] dB\n", cfg.report_cfg_id, *cfg.hysteresis_db);
    return false;
  }

  const std::string& qty = *cfg.meas_trigger_quantity;

  if (ev == ocucp::rrc_event_id::event_id_t::a3 || ev == ocucp::rrc_event_id::event_id_t::a6) {
    if (!cfg.meas_trigger_quantity_offset_db.has_value()) {
      fmt::print("report_cfg_id={}: A3/A6 event requires meas_trigger_quantity_offset_db\n", cfg.report_cfg_id);
      return false;
    }
    // Offset range: [-15..+15] dB (ASN.1 encodes as value × 2, giving [-30..+30] in 0.5 dB steps).
    int offset = *cfg.meas_trigger_quantity_offset_db;
    if (offset < -15 || offset > 15) {
      fmt::print("report_cfg_id={}: meas_trigger_quantity_offset_db={} out of range [-15..15] dB\n",
                 cfg.report_cfg_id,
                 offset);
      return false;
    }
  } else {
    // A1, A2, A4, A5: absolute threshold required.
    if (!cfg.meas_trigger_quantity_threshold_db.has_value()) {
      fmt::print("report_cfg_id={}: A1/A2/A4/A5 event requires meas_trigger_quantity_threshold_db\n",
                 cfg.report_cfg_id);
      return false;
    }
    if (ev == ocucp::rrc_event_id::event_id_t::a5 && !cfg.meas_trigger_quantity_threshold_2_db.has_value()) {
      fmt::print("report_cfg_id={}: A5 event requires meas_trigger_quantity_threshold_2_db\n", cfg.report_cfg_id);
      return false;
    }

    // Validate threshold range(s) per measurement quantity.
    auto check_threshold = [&](int val, const char* label) -> bool {
      if (qty == "rsrp") {
        if (val < -156 || val > -31) {
          fmt::print("report_cfg_id={}: RSRP {} = {} dBm out of range [-156..-31]\n", cfg.report_cfg_id, label, val);
          return false;
        }
      } else if (qty == "rsrq") {
        if (val < -43 || val > 20) {
          fmt::print("report_cfg_id={}: RSRQ {} = {} dB out of range [-43..20]\n", cfg.report_cfg_id, label, val);
          return false;
        }
      } else if (qty == "sinr") {
        if (val < -23 || val > 40) {
          fmt::print("report_cfg_id={}: SINR {} = {} dB out of range [-23..40]\n", cfg.report_cfg_id, label, val);
          return false;
        }
      } else {
        fmt::print("report_cfg_id={}: invalid meas_trigger_quantity={}\n", cfg.report_cfg_id, qty);
        return false;
      }
      return true;
    };

    if (!check_threshold(*cfg.meas_trigger_quantity_threshold_db, "threshold1")) {
      return false;
    }
    if (ev == ocucp::rrc_event_id::event_id_t::a5) {
      if (!check_threshold(*cfg.meas_trigger_quantity_threshold_2_db, "threshold2")) {
        return false;
      }
    }
  }

  return true;
}

static bool validate_mobility_appconfig(gnb_id_t gnb_id, const cu_cp_unit_mobility_config& config)
{
  std::map<unsigned, std::string> report_cfg_ids_to_report_type;
  for (const auto& report_cfg : config.report_configs) {
    // Check that report config ids are unique.
    if (report_cfg_ids_to_report_type.find(report_cfg.report_cfg_id) != report_cfg_ids_to_report_type.end()) {
      fmt::print("Report config ids must be unique\n");
      return false;
    }
    report_cfg_ids_to_report_type.emplace(report_cfg.report_cfg_id, report_cfg.report_type);

    // Check that report configs are valid.
    if (report_cfg.report_type == "event_triggered" || report_cfg.report_type == "cond_trigger") {
      if (!validate_event_trigger_params(report_cfg)) {
        return false;
      }
    }

    // T312 is only valid for event-triggered reports.
    if (report_cfg.t312_ms.has_value() && report_cfg.report_type != "event_triggered") {
      fmt::print("T312 is only valid for event-triggered report configurations.\n");
      return false;
    }
  }

  std::map<nr_cell_identity, std::set<unsigned>> cell_to_report_cfg_id;

  // Check cu_cp_cell_config.
  std::set<nr_cell_identity> ncis;
  for (const auto& cell : config.cells) {
    nr_cell_identity nci = *nr_cell_identity::create(cell.nr_cell_id);
    if (!ncis.emplace(nci).second) {
      fmt::print("Cells must be unique ({:#x} already present)\n", cell.nr_cell_id);
      return false;
    }

    if (cell.ssb_period.has_value() && cell.ssb_offset.has_value() && *cell.ssb_offset >= *cell.ssb_period) {
      fmt::print("ssb_offset must be smaller than ssb_period\n");
      return false;
    }

    if (cell.periodic_report_cfg_id.has_value()) {
      // Try to add report config id to cell_to_report_cfg_id map.
      cell_to_report_cfg_id.emplace(nci, std::set<unsigned>());
      auto& report_cfg_ids = cell_to_report_cfg_id.at(nci);
      if (!report_cfg_ids.emplace(*cell.periodic_report_cfg_id).second) {
        fmt::print("cell={:#x}: report_config_id={} already configured for this cell)\n",
                   cell.nr_cell_id,
                   *cell.periodic_report_cfg_id);
        return false;
      }
    }

    // Check if cell is an external managed cell.
    if (nci.gnb_id(gnb_id.bit_length) != gnb_id) {
      if (!cell.gnb_id_bit_length.has_value() || !cell.pci.has_value() || !cell.plmn_id.has_value() ||
          !cell.tac.has_value() || !cell.band.has_value() || !cell.ssb_arfcn.has_value() || !cell.ssb_scs.has_value() ||
          !cell.ssb_period.has_value() || !cell.ssb_offset.has_value() || !cell.ssb_duration.has_value()) {
        // Collect internal and external cells to help the user diagnose misconfiguration.
        std::vector<std::string> internal_cells;
        std::vector<std::string> external_cells;
        for (const auto& c : config.cells) {
          nr_cell_identity c_nci = *nr_cell_identity::create(c.nr_cell_id);
          if (c_nci.gnb_id(gnb_id.bit_length) == gnb_id) {
            internal_cells.push_back(fmt::format("{:#x}", c.nr_cell_id));
          } else {
            external_cells.push_back(fmt::format("{:#x}", c.nr_cell_id));
          }
        }
        std::string valid_nci_str;
        if (!internal_cells.empty()) {
          valid_nci_str = fmt::format("{}", fmt::join(internal_cells, ", "));
        } else {
          uint64_t nci_base = static_cast<uint64_t>(gnb_id.id) << (36U - gnb_id.bit_length);
          valid_nci_str =
              fmt::format("none configured yet, possible nr_cell_ids: {:#x}, {:#x}", nci_base, nci_base | 1U);
        }
        fmt::print("cell={:#x} is detected as external because its gnb_id part of nr_cell_id differs from this "
                   "CU-CP's gnb_id={:#x}, but some parameters required for external cells are missing.\n"
                   "If this cell is correctly assigned to a different CU-CP - fix the missing parameters:\n"
                   "  - gnb_id_bit_length: {}\n"
                   "  - pci: {}\n"
                   "  - plmn: {}\n"
                   "  - tac: {}\n"
                   "  - band: {}\n"
                   "  - ssb_arfcn: {}\n"
                   "  - ssb_scs: {}\n"
                   "  - ssb_period: {}\n"
                   "  - ssb_offset: {}\n"
                   "  - ssb_duration: {}\n"
                   "If this cell should be within the same CU-CP - fix the nr_cell_id to match gnb_id={:#x} and "
                   "gnb_id_bit_length={}.\n"
                   "  - Internal cells (within this CU-CP): {}\n"
                   "  - External cells (in other CU-CPs): {}\n",
                   cell.nr_cell_id,
                   gnb_id.id,
                   cell.gnb_id_bit_length.has_value() ? fmt::format("{}", *cell.gnb_id_bit_length) : "[MISSING]",
                   cell.pci.has_value() ? fmt::format("{}", *cell.pci) : "[MISSING]",
                   cell.plmn_id.has_value() ? fmt::format("{}", *cell.plmn_id) : "[MISSING]",
                   cell.tac.has_value() ? fmt::format("{}", *cell.tac) : "[MISSING]",
                   cell.band.has_value() ? fmt::format("n{}", static_cast<unsigned>(*cell.band)) : "[MISSING]",
                   cell.ssb_arfcn.has_value() ? fmt::format("{}", *cell.ssb_arfcn) : "[MISSING]",
                   cell.ssb_scs.has_value() ? fmt::format("{}", *cell.ssb_scs) : "[MISSING]",
                   cell.ssb_period.has_value() ? fmt::format("{}", *cell.ssb_period) : "[MISSING]",
                   cell.ssb_offset.has_value() ? fmt::format("{}", *cell.ssb_offset) : "[MISSING]",
                   cell.ssb_duration.has_value() ? fmt::format("{}", *cell.ssb_duration) : "[MISSING]",
                   gnb_id.id,
                   gnb_id.bit_length,
                   valid_nci_str,
                   fmt::join(external_cells, ", "));
        return false;
      }
    } else {
      if (cell.pci.has_value() || cell.band.has_value() || cell.ssb_arfcn.has_value() || cell.ssb_scs.has_value() ||
          cell.ssb_period.has_value() || cell.ssb_offset.has_value() || cell.ssb_duration.has_value()) {
        // Collect internal and external cells to help the user diagnose misconfiguration.
        std::vector<std::string> internal_cells;
        std::vector<std::string> external_cells;
        for (const auto& c : config.cells) {
          nr_cell_identity c_nci = *nr_cell_identity::create(c.nr_cell_id);
          if (c_nci.gnb_id(gnb_id.bit_length) == gnb_id) {
            internal_cells.push_back(fmt::format("{:#x}", c.nr_cell_id));
          } else {
            external_cells.push_back(fmt::format("{:#x}", c.nr_cell_id));
          }
        }
        fmt::print("cell={:#x} is detected as internal because its gnb_id part of nr_cell_id matches this "
                   "CU-CP's gnb_id={:#x}, but some parameters that are only valid for external cells are configured.\n"
                   "If this cell is correctly assigned to this CU-CP - remove the following parameters:\n"
                   "  - pci: {}\n"
                   "  - plmn_id: {}\n"
                   "  - tac: {}\n"
                   "  - band: {}\n"
                   "  - ssb_arfcn: {}\n"
                   "  - ssb_scs: {}\n"
                   "  - ssb_period: {}\n"
                   "  - ssb_offset: {}\n"
                   "  - ssb_duration: {}\n"
                   "If this cell should be within a different CU-CP - fix the nr_cell_id so that it does not match "
                   "this CU-CP gnb_id={:#x} (gnb_id_bit_length={}).\n"
                   "  - Internal cells (within this CU-CP): {}\n"
                   "  - External cells (in other CU-CPs): {}\n",
                   cell.nr_cell_id,
                   gnb_id.id,
                   cell.pci.has_value() ? fmt::format("{} [REMOVE]", *cell.pci) : "not set",
                   cell.plmn_id.has_value() ? fmt::format("{} [REMOVE]", *cell.plmn_id) : "not set",
                   cell.tac.has_value() ? fmt::format("{} [REMOVE]", *cell.tac) : "not set",
                   cell.band.has_value() ? fmt::format("n{} [REMOVE]", static_cast<unsigned>(*cell.band)) : "not set",
                   cell.ssb_arfcn.has_value() ? fmt::format("{} [REMOVE]", *cell.ssb_arfcn) : "not set",
                   cell.ssb_scs.has_value() ? fmt::format("{} [REMOVE]", *cell.ssb_scs) : "not set",
                   cell.ssb_period.has_value() ? fmt::format("{} [REMOVE]", *cell.ssb_period) : "not set",
                   cell.ssb_offset.has_value() ? fmt::format("{} [REMOVE]", *cell.ssb_offset) : "not set",
                   cell.ssb_duration.has_value() ? fmt::format("{} [REMOVE]", *cell.ssb_duration) : "not set",
                   gnb_id.id,
                   gnb_id.bit_length,
                   fmt::join(internal_cells, ", "),
                   fmt::join(external_cells, ", "));
        return false;
      }
    }

    // Check that for neighbor cells managed by this CU-CP no periodic reports are configured.
    for (const auto& ncell : cell.ncells) {
      for (const auto& id : ncell.report_cfg_ids) {
        auto it = report_cfg_ids_to_report_type.find(id);
        if (it == report_cfg_ids_to_report_type.end()) {
          fmt::print("cell={:#x}: Report configuration for neighbor cell={:#x} with id={} does not exist in the report "
                     "configuration list.\n",
                     cell.nr_cell_id,
                     ncell.nr_cell_id,
                     id);
          return false;
        } else if (it->second == "periodical") {
          fmt::print("cell={:#x}: For neighbor cells no periodic reports are allowed\n", cell.nr_cell_id);
          return false;
        }
      }
    }
  }

  // Verify that each configured neighbor cell is present.
  for (const auto& cell : config.cells) {
    for (const auto& ncell : cell.ncells) {
      nr_cell_identity nci = *nr_cell_identity::create(ncell.nr_cell_id);
      if (ncis.find(nci) == ncis.end()) {
        fmt::print("Neighbor cell config for nci={:#x} incomplete. No valid configuration for cell nci={:#x} found.\n",
                   cell.nr_cell_id,
                   ncell.nr_cell_id);
        return false;
      }
    }
  }

  return true;
}

/// Validates the given PDCP configuration. Returns true on success, otherwise false.
static bool validate_pdcp_appconfig(five_qi_t five_qi, const cu_cp_unit_pdcp_config& config)
{
  // Check TX.
  if (config.tx.sn_field_length != pdcp_sn_size::size12bits && config.tx.sn_field_length != pdcp_sn_size::size18bits) {
    fmt::print("PDCP TX SN length is neither 12 or 18 bits. {} SN={}\n", five_qi, config.tx.sn_field_length);
    return false;
  }
  if (config.tx.status_report_required) {
    fmt::print("PDCP TX status report required not supported yet. {}\n", five_qi);
    return false;
  }

  // Check RX.
  if (config.rx.sn_field_length != pdcp_sn_size::size12bits && config.rx.sn_field_length != pdcp_sn_size::size18bits) {
    fmt::print("PDCP RX SN length is neither 12 or 18 bits. {} SN={}\n", five_qi, config.rx.sn_field_length);
    return false;
  }

  if (config.rx.t_reordering == pdcp_t_reordering::infinity) {
    fmt::print("PDCP t-Reordering=infinity on DRBs is not advised. It can cause data stalls. {}\n", five_qi);
  }

  if (config.rohc.rohc_type != cu_cp_unit_pdcp_rohc_type::none) {
    if (config.rx.out_of_order_delivery) {
      fmt::print("Conflicting PDCP config: ROHC is ineffective with out-of-order delivery enabled. {}\n", five_qi);
      return false;
    }
  }
  return true;
}

static bool validate_rlc_um_appconfig(five_qi_t five_qi, const cu_cp_unit_rlc_um_config& config)
{
  // Validate TX.

  rlc_um_sn_size tmp_sn_size;
  if (!from_number(tmp_sn_size, config.tx.sn_field_length)) {
    fmt::print("RLC UM TX SN length is neither 6 or 12 bits. {} sn_size={}\n", five_qi, config.tx.sn_field_length);
    return false;
  }

  if (config.tx.queue_size == 0) {
    fmt::print("RLC TX queue size cannot be 0. {}\n", five_qi);
    return false;
  }

  // Validate RX.

  if (!from_number(tmp_sn_size, config.rx.sn_field_length)) {
    fmt::print("RLC TX queue size cannot be 0. {}\n", five_qi);
    return false;
  }

  rlc_t_reassembly tmp_t_reassembly;
  if (!rlc_t_reassembly_from_int(tmp_t_reassembly, config.rx.t_reassembly)) {
    fmt::print("RLC UM RX t-Reassembly is invalid. {} t_reassembly={}\n", five_qi, config.rx.t_reassembly);
    fmt::print("Valid values are:"
               " ms40, ms45, ms50, ms55, ms60, ms65, ms70,"
               " ms75, ms80, ms85, ms90, ms95, ms100, ms110,"
               " ms120, ms130, ms140, ms150, ms160, ms170,"
               " ms180, ms190, ms200\n");
    return false;
  }
  return true;
}

template <typename id_type>
static bool validate_rlc_am_appconfig(id_type id, const cu_cp_unit_rlc_am_config& config)
{
  // Validate TX.

  rlc_am_sn_size tmp_sn_size;
  if (!from_number(tmp_sn_size, config.tx.sn_field_length)) {
    fmt::print("RLC AM TX SN length is neither 12 or 18 bits. {} sn_size={}\n", id, config.tx.sn_field_length);
    return false;
  }

  rlc_t_poll_retransmit tmp_t_poll_retransmit;
  if (!rlc_t_poll_retransmit_from_int(tmp_t_poll_retransmit, config.tx.t_poll_retx)) {
    fmt::print("Invalid RLC AM TX t-PollRetransmission. {} t_poll_retx={}\n", id, config.tx.t_poll_retx);
    fmt::print(" Valid values are: ms5, ms10, ms15, ms20, ms25, ms30, ms35,"
               " ms40, ms45, ms50, ms55, ms60, ms65, ms70, ms75, ms80, ms85,"
               " ms90, ms95, ms100, ms105, ms110, ms115, ms120, ms125, ms130,"
               " ms135, ms140, ms145, ms150, ms155, ms160, ms165, ms170, ms175,"
               " ms180, ms185, ms190, ms195, ms200, ms205, ms210, ms215, ms220,"
               " ms225, ms230, ms235, ms240, ms245, ms250, ms300, ms350, ms400,"
               " ms450, ms500, ms800, ms1000, ms2000, ms4000\n");
    return false;
  }

  rlc_max_retx_threshold tmp_max_retx_threshold;
  if (!rlc_max_retx_threshold_from_int(tmp_max_retx_threshold, config.tx.max_retx_thresh)) {
    fmt::print("Invalid RLC AM TX max retx threshold. {} max_retx_threshold={}\n", id, config.tx.max_retx_thresh);
    fmt::print(" Valid values are: t1, t2, t3, t4, t6, t8, t16, t32\n");
    return false;
  }

  rlc_poll_pdu tmp_poll_pdu;
  if (!rlc_poll_pdu_from_int(tmp_poll_pdu, config.tx.poll_pdu)) {
    fmt::print("Invalid RLC AM TX PollPDU. {} poll_pdu={}\n", id, config.tx.poll_pdu);
    fmt::print(" Valid values are:"
               "p4, p8, p16, p32, p64, p128, p256, p512, p1024, p2048,"
               " p4096, p6144, p8192, p12288, p16384,p20480,"
               " p24576, p28672, p32768, p40960, p49152, p57344, p65536\n");
    return false;
  }

  rlc_poll_kilo_bytes tmp_poll_bytes;
  if (!rlc_poll_kilo_bytes_from_int(tmp_poll_bytes, config.tx.poll_byte)) {
    fmt::print("Invalid RLC AM TX PollBytes. {} poll_bytes={}\n", id, config.tx.poll_byte);
    fmt::print(" Valid values are (in KBytes):"
               " kB1, kB2, kB5, kB8, kB10, kB15, kB25, kB50, kB75,"
               " kB100, kB125, kB250, kB375, kB500, kB750, kB1000,"
               " kB1250, kB1500, kB2000, kB3000, kB4000, kB4500,"
               " kB5000, kB5500, kB6000, kB6500, kB7000, kB7500,"
               " mB8, mB9, mB10, mB11, mB12, mB13, mB14, mB15,"
               " mB16, mB17, mB18, mB20, mB25, mB30, mB40, infinity\n");
    return false;
  }

  if (config.tx.queue_size == 0) {
    fmt::print("RLC AM TX queue size cannot be 0. {}\n", id);
    return false;
  }

  // Validate RX.

  if (!from_number(tmp_sn_size, config.rx.sn_field_length)) {
    fmt::print("RLC AM RX SN length is neither 12 or 18 bits. {} sn_size={}\n", id, config.rx.sn_field_length);
    return false;
  }

  rlc_t_reassembly tmp_t_reassembly;
  if (!rlc_t_reassembly_from_int(tmp_t_reassembly, config.rx.t_reassembly)) {
    fmt::print("RLC AM RX t-Reassembly is invalid. {} t_reassembly={}\n", id, config.rx.t_reassembly);
    fmt::print("Valid values are:"
               " ms40, ms45, ms50, ms55, ms60, ms65, ms70,"
               " ms75, ms80, ms85, ms90, ms95, ms100, ms110,"
               " ms120, ms130, ms140, ms150, ms160, ms170,"
               " ms180, ms190, ms200\n");
    return false;
  }

  rlc_t_status_prohibit tmp_t_status_prohibit;
  if (!rlc_t_status_prohibit_from_int(tmp_t_status_prohibit, config.rx.t_status_prohibit)) {
    fmt::print("RLC AM RX t-statusProhibit is invalid. {} t_status_prohibit={}\n", id, config.rx.t_status_prohibit);
    fmt::print("Valid values are:"
               "ms0, ms5, ms10, ms15, ms20, ms25, ms30, ms35,"
               "ms40, ms45, ms50, ms55, ms60, ms65, ms70,"
               "ms75, ms80, ms85, ms90, ms95, ms100, ms105,"
               "ms110, ms115, ms120, ms125, ms130, ms135,"
               "ms140, ms145, ms150, ms155, ms160, ms165,"
               "ms170, ms175, ms180, ms185, ms190, ms195,"
               "ms200, ms205, ms210, ms215, ms220, ms225,"
               "ms230, ms235, ms240, ms245, ms250, ms300,"
               "ms350, ms400, ms450, ms500, ms800, ms1000,"
               "ms1200, ms1600, ms2000, ms2400\n");
    return false;
  }

  if (config.rx.max_sn_per_status >= window_size(config.rx.sn_field_length)) {
    fmt::print("RLC AM RX max_sn_per_status={} exceeds window_size={}. sn_size={}\n",
               config.rx.max_sn_per_status,
               window_size(config.rx.sn_field_length),
               config.rx.sn_field_length);
    return false;
  }

  return true;
}

static bool validate_rlc_appconfig(five_qi_t five_qi, const cu_cp_unit_rlc_config& config)
{
  // Check mode.
  if (config.mode != rlc_mode::am && config.mode != rlc_mode::um_bidir) {
    fmt::print("RLC mode is neither \"am\" or \"um-bidir\". {} mode={}\n", five_qi, format_as(config.mode));
    return false;
  }

  // Check AM.
  if (config.mode == rlc_mode::am && !validate_rlc_am_appconfig(five_qi, config.am)) {
    fmt::print("RLC AM config is invalid. {}\n", five_qi);
    return false;
  }

  // Check UM.
  if (config.mode == rlc_mode::um_bidir && !validate_rlc_um_appconfig(five_qi, config.um)) {
    fmt::print("RLC UM config is invalid. {}\n", five_qi);
    return false;
  }
  return true;
}

/// Validates the given QoS configuration. Returns true on success, otherwise false.
static bool validate_qos_appconfig(span<const cu_cp_unit_qos_config> config)
{
  for (const auto& qos : config) {
    if (!validate_pdcp_appconfig(qos.five_qi, qos.pdcp)) {
      return false;
    }
    if (!validate_rlc_appconfig(qos.five_qi, qos.rlc)) {
      return false;
    }
  }
  return true;
}

/// Validates the given AMF configuration. Returns true on success, otherwise false.
static bool validate_amf_appconfig(const cu_cp_unit_amf_config&                   amf_config,
                                   const std::vector<cu_cp_unit_amf_config_item>& extra_amfs)
{
  std::vector<std::string> plmns;

  std::vector<cu_cp_unit_amf_config_item> amfs;

  amfs.push_back(amf_config.amf);

  amfs.insert(amfs.end(), extra_amfs.begin(), extra_amfs.end());

  for (const auto& config : amfs) {
    // check for non-empty AMF address list
    if (config.ip_addrs.empty()) {
      fmt::print("AMF address list cannot be empty\n");
      return false;
    }

    for (const auto& ip_addr : config.ip_addrs) {
      // check for non-empty AMF address
      if (ip_addr.empty()) {
        fmt::print("AMF IP address cannot be empty\n");
        return false;
      }
    }

    if (!config.bind_interface.empty() && config.bind_interface != "auto" && config.bind_addrs.size() > 1) {
      fmt::print("Network device to bind for N2 interface should not be set if more than one bind_addrs is specified "
                 "for SCTP multihoming\n");
      return false;
    }

    // check supported tracking areas
    if (config.supported_tas.size() > 1) {
      for (unsigned outer_ta_idx = 0; outer_ta_idx < config.supported_tas.size(); outer_ta_idx++) {
        std::vector<std::string> outer_plmns;
        for (const auto& plmn_item : config.supported_tas[outer_ta_idx].plmn_list) {
          outer_plmns.push_back(plmn_item.plmn_id);
        }

        for (unsigned inner_ta_idx = outer_ta_idx + 1; inner_ta_idx < config.supported_tas.size(); inner_ta_idx++) {
          if (config.supported_tas[outer_ta_idx].tac == config.supported_tas[inner_ta_idx].tac) {
            for (const auto& plmn_item : config.supported_tas[inner_ta_idx].plmn_list) {
              if (std::find(outer_plmns.begin(), outer_plmns.end(), plmn_item.plmn_id) != outer_plmns.end()) {
                fmt::print("Supported tracking areas of a AMF must be unique\n");
                return false;
              }
            }
          }
        }
      }
    }

    // One AMF may serve a PLMN across several tracking areas; only a PLMN shared with another AMF is an error.
    std::vector<std::string> amf_plmns;
    for (const auto& ta : config.supported_tas) {
      for (const auto& plmn_item : ta.plmn_list) {
        if (plmn_item.tai_slice_support_list.empty()) {
          fmt::print("TAI slice support list for PLMN={} and TAC={} is empty\n", plmn_item.plmn_id, ta.tac);
          return false;
        }

        if (std::find(amf_plmns.begin(), amf_plmns.end(), plmn_item.plmn_id) == amf_plmns.end()) {
          amf_plmns.push_back(plmn_item.plmn_id);
        }
      }
    }

    for (const auto& plmn : amf_plmns) {
      if (std::find(plmns.begin(), plmns.end(), plmn) != plmns.end()) {
        fmt::print("PLMN={} is already supported by another AMF\n", plmn);
        return false;
      }
    }
    plmns.insert(plmns.end(), amf_plmns.begin(), amf_plmns.end());
  }

  return true;
}

/// Validates the XnAP configuration:
/// - checks that no peer address is listed under more than one peer
/// - checks that no bind address is listed under more than one gateway.
/// Uses string-level comparison: equivalent representations (IPv4 vs IPv4-mapped IPv6) or hostnames resolving to the
/// same address are not detected here — those errors will surface at SCTP bind/connect time.
static bool validate_xnap_appconfig(const cu_cp_unit_xnap_config& config)
{
  std::set<std::string> seen_peer_addrs;
  for (const auto& gw : config.gateways) {
    for (const auto& peer : gw.connections) {
      for (const auto& addr : peer.peer_addrs) {
        if (addr.empty()) {
          fmt::print("XnAP peer address cannot be empty\n");
          return false;
        }
        if (!seen_peer_addrs.insert(addr).second) {
          fmt::print("XnAP peer address {} is listed under more than one peer\n", addr);
          return false;
        }
      }
    }
  }

  std::set<std::string> seen_bind_addrs;
  for (const auto& gw : config.gateways) {
    for (const auto& addr : gw.bind_addrs) {
      if (addr.empty()) {
        fmt::print("XnAP bind address cannot be empty\n");
        return false;
      }
      if (!seen_bind_addrs.insert(addr).second) {
        fmt::print("XnAP bind address {} is listed under more than one gateway\n", addr);
        return false;
      }
    }
  }

  return true;
}

/// Validates the given CU-CP configuration. Returns true on success, otherwise false.
/// Validates the NTN configuration of the cells. Returns true on success, otherwise false.
static bool validate_ntn_appconfig(const cu_cp_unit_config& config)
{
  bool has_ntn_cells = false;
  for (const auto& cell : config.mobility_config.cells) {
    if (!cell.ntn_cfg.has_value()) {
      continue;
    }
    has_ntn_cells = true;
    // Satellite must be a global reference or an inline definition.
    const auto& sat_ref = cell.ntn_cfg->sat_ref;
    if (!sat_ref.satellite_idx && !(sat_ref.epoch_timestamp && sat_ref.ephemeris_info)) {
      fmt::print("cell={:#x} ntn: either satellite_idx or inline ephemeris definition "
                 "(epoch_timestamp and ephemeris_info) must be provided\n",
                 cell.nr_cell_id);
      return false;
    }
  }

  // The NTN neighbour info epoch time is derived from the DU reference time reports, so periodic reference time
  // reporting must be enabled when NTN cells are configured.
  if (has_ntn_cells && (!config.f1ap_config.ref_time_reporting_enabled ||
                        config.f1ap_config.ref_time_reporting_event_type != "periodic")) {
    fmt::print("NTN cells are configured, but periodic reference time reporting is disabled. Enable it via "
               "cu_cp.f1ap.ref_time_reporting.\n");
    return false;
  }

  return true;
}

/// Validates the logical cell configuration. Returns true on success, false otherwise.
static bool validate_cells_appconfig(const gnb_id_t gnb_id, span<const cu_cp_unit_logical_cell_config> cells)
{
  // The NCI is 36 bits: the gNB Id occupies the upper bit_length bits, the sector ID the rest.
  const unsigned     nof_sector_id_bits = 36U - gnb_id.bit_length;
  std::set<unsigned> used_sector_ids;
  for (const cu_cp_unit_logical_cell_config& cell : cells) {
    if (cell.sector_id >= (1U << nof_sector_id_bits)) {
      fmt::print("Cell sector ID {} does not fit in the {} bits left by a gNB Id of {} bits\n",
                 cell.sector_id,
                 nof_sector_id_bits,
                 gnb_id.bit_length);
      return false;
    }
    if (!used_sector_ids.insert(cell.sector_id).second) {
      fmt::print("Duplicated cell sector ID {}\n", cell.sector_id);
      return false;
    }
  }
  return true;
}

static bool validate_cu_cp_appconfig(const gnb_id_t gnb_id, const cu_cp_unit_config& config)
{
  // validate AMF config
  if (!validate_amf_appconfig(config.amf_config, config.extra_amfs)) {
    return false;
  }

  // validate mobility config
  if (!validate_mobility_appconfig(gnb_id, config.mobility_config)) {
    return false;
  }

  // validate NTN neighbor cell config
  if (!validate_ntn_appconfig(config)) {
    return false;
  }

  if (!validate_qos_appconfig(config.qos_cfg)) {
    return false;
  }

  if (!validate_xnap_appconfig(config.xnap_config)) {
    return false;
  }

  full_i_rnti_profile  full_profile  = full_i_rnti_profile::profile_0;
  short_i_rnti_profile short_profile = short_i_rnti_profile::profile_0;
  if (!from_string(full_profile, config.full_i_rnti_profile)) {
    fmt::print("Invalid full_i_rnti_profile={}.\n", config.full_i_rnti_profile);
    return false;
  }
  if (!from_string(short_profile, config.short_i_rnti_profile)) {
    fmt::print("Invalid short_i_rnti_profile={}.\n", config.short_i_rnti_profile);
    return false;
  }

  if (!validate_cells_appconfig(gnb_id, config.cells_cfg)) {
    return false;
  }

  return true;
}

bool ocudu::validate_cu_cp_unit_config(const cu_cp_unit_config& config)
{
  return validate_cu_cp_appconfig(config.gnb_id, config);
}
