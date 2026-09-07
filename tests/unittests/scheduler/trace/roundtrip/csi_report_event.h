// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h" // IWYU pragma: keep
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "ocudu/scheduler/input/uci_inputs.h"

namespace ocudu::schedtrace::roundtrip_test {

/// Alternative of the PMI variant standing for "reported, codebook unknown".
template <>
struct test_values<std::monostate> {
  static std::vector<std::monostate> get() { return {std::monostate{}}; }
};

template <>
struct test_values<pmi_codebook_single_panel_config> {
  static std::vector<pmi_codebook_single_panel_config> get()
  {
    return {pmi_codebook_single_panel_config::two_one, pmi_codebook_single_panel_config::sixteen_one};
  }
};

template <>
struct test_values<pmi_codebook_typeI_mode> {
  static std::vector<pmi_codebook_typeI_mode> get()
  {
    return {pmi_codebook_typeI_mode::one, pmi_codebook_typeI_mode::two};
  }
};

template <>
struct test_values<pmi_codebook_typeII_phase_size> {
  static std::vector<pmi_codebook_typeII_phase_size> get()
  {
    return {pmi_codebook_typeII_phase_size::qpsk, pmi_codebook_typeII_phase_size::psk8};
  }
};

template <>
struct roundtrip_traits<pmi_two_antenna_port> {
  static constexpr auto members = std::make_tuple(field("pmi", &pmi_two_antenna_port::pmi));
};

template <>
struct roundtrip_traits<pmi_codebook_typeI_single_panel> {
  static constexpr auto members = std::make_tuple(field("n1_n2", &pmi_codebook_typeI_single_panel::n1_n2),
                                                  field("mode", &pmi_codebook_typeI_single_panel::mode));
};

template <>
struct roundtrip_traits<pmi_typeI_single_panel> {
  static constexpr auto members = std::make_tuple(field("panel_config", &pmi_typeI_single_panel::panel_config),
                                                  field("i_1_1", &pmi_typeI_single_panel::i_1_1),
                                                  field("i_1_2", &pmi_typeI_single_panel::i_1_2),
                                                  field("i_1_3", &pmi_typeI_single_panel::i_1_3),
                                                  field("i_2", &pmi_typeI_single_panel::i_2));
};

template <>
struct roundtrip_traits<pmi_codebook_typeII> {
  static constexpr auto members =
      std::make_tuple(field("n1_n2", &pmi_codebook_typeII::n1_n2),
                      field("nof_beams", &pmi_codebook_typeII::nof_beams),
                      field("phase_alphabet_size", &pmi_codebook_typeII::phase_alphabet_size),
                      field("subband_amplitude", &pmi_codebook_typeII::subband_amplitude));
};

template <>
struct roundtrip_traits<pmi_typeII::layer_coefficients> {
  static constexpr auto members = std::make_tuple(field("i_1_3", &pmi_typeII::layer_coefficients::i_1_3),
                                                  field("i_1_4", &pmi_typeII::layer_coefficients::i_1_4),
                                                  field("i_2_1", &pmi_typeII::layer_coefficients::i_2_1),
                                                  field("i_2_2", &pmi_typeII::layer_coefficients::i_2_2));
};

template <>
struct roundtrip_traits<pmi_typeII> {
  static constexpr auto members = std::make_tuple(field("config", &pmi_typeII::config),
                                                  field("i_1_1", &pmi_typeII::i_1_1),
                                                  field("i_1_2", &pmi_typeII::i_1_2),
                                                  field("layers", &pmi_typeII::layers));
};

template <>
struct roundtrip_traits<csi_report_data> {
  static constexpr auto members =
      std::make_tuple(field("cri", &csi_report_data::cri),
                      field("rsrp_dBm", &csi_report_data::rsrp_dBm),
                      field("ri", &csi_report_data::ri),
                      field("li", &csi_report_data::li),
                      field("pmi", &csi_report_data::pmi),
                      field("first_tb_wideband_cqi", &csi_report_data::first_tb_wideband_cqi),
                      field("second_tb_wideband_cqi", &csi_report_data::second_tb_wideband_cqi),
                      field("first_tb_subband_diff_cqi", &csi_report_data::first_tb_subband_diff_cqi),
                      field("second_tb_subband_diff_cqi", &csi_report_data::second_tb_subband_diff_cqi),
                      field("valid", &csi_report_data::valid));
};

template <>
struct roundtrip_traits<csi_report_event> {
  static constexpr auto members = std::make_tuple(field("ue_index", &csi_report_event::ue_index),
                                                  field("rnti", &csi_report_event::rnti),
                                                  field("sl_rx", &csi_report_event::sl_rx),
                                                  field("csi", &csi_report_event::csi));

  static csi_report_event roundtrip(const csi_report_event& event)
  {
    return roundtrip_via<fbs::CsiReportEvent>(
        event, convert_csi_report_event_to_fb, [&event](csi_report_event& result, const fbs::CsiReportEvent& fb) {
          convert_fb_to_csi_report_event(result, fb, event.sl_rx.scs());
        });
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
