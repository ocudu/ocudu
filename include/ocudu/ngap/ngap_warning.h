// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/tai.h"
#include <array>
#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

namespace ocudu::ocucp {

/// Warning area expressed as a list of NR cell identifiers.
using ngap_nr_cgi_list_for_warning = std::vector<nr_cell_global_id_t>;

/// Warning area expressed as a list of TAIs.
using ngap_tai_list_for_warning = std::vector<tai_t>;

/// Warning area expressed as a list of Emergency Area IDs (3-byte values per TS 38.413).
using ngap_emergency_area_id_list = std::vector<std::array<uint8_t, 3>>;

/// Warning Area List as received in WRITE-REPLACE WARNING REQUEST (TS 38.413 section 9.3.3.83).
/// Absent means the warning applies to all cells served by this NG-RAN node.
using ngap_warning_area_list =
    std::variant<ngap_nr_cgi_list_for_warning, ngap_tai_list_for_warning, ngap_emergency_area_id_list>;

/// Common type for NGAP WRITE-REPLACE WARNING REQUEST (TS 38.413 section 8.9.1.1).
struct ngap_write_replace_warning_request {
  /// Message Identifier (16-bit value, mandatory).
  uint16_t msg_id = 0;
  /// Serial Number (16-bit value, mandatory).
  uint16_t serial_num = 0;
  /// Repetition Period in seconds (mandatory).
  uint32_t repeat_period = 0;
  /// Number of Broadcasts Requested (mandatory).
  uint32_t nof_broadcasts_requested = 0;
  /// Warning Area List (optional; absent = all served cells).
  std::optional<ngap_warning_area_list> warning_area_list;
  /// Warning Type (16-bit value, optional; identifies ETWS primary/secondary vs. CMAS, TS 38.413 section 9.3.1.39).
  std::optional<uint16_t> warning_type;
  /// Data Coding Scheme (1 byte from 8-bit bitstring, optional).
  std::optional<uint8_t> data_coding_scheme;
  /// Warning Message Contents (up to 9600 bytes, optional).
  std::optional<byte_buffer> warning_msg_contents;
  /// Concurrent Warning Message Indication (optional; true = indicated).
  bool concurrent_warning_msg_ind = false;
};

/// Common type for NGAP WRITE-REPLACE WARNING RESPONSE (TS 38.413 section 8.9.1.2).
struct ngap_write_replace_warning_response {
  /// Message Identifier (echoed back from request, 16-bit value).
  uint16_t msg_id = 0;
  /// Serial Number (echoed back from request, 16-bit value).
  uint16_t serial_num = 0;
};

} // namespace ocudu::ocucp
