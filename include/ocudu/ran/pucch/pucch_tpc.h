// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

/// \brief UE Transmit Power Control (TPC) command configuration for PUCCH.
///
/// The identifiers are inspired by TS38.331 Section as 6.3.2 (see field \e PUCCH-TPC-CommandConfig).
struct pucch_tpc_command_config {
  /// An index determining the position of the first bit of TPC command (applicable to the SpCell) inside the DCI format
  /// 2-2 payload. Values {1..15}.
  /// The field is mandatory present if the PUCCH-TPC-CommandConfig is provided in the PDCCH-Config for the SpCell.
  /// Otherwise, the field is absent.
  std::optional<uint8_t> tpc_index_pcell;
  /// An index determining the position of the first bit of TPC command (applicable to the PUCCH SCell) inside the DCI
  /// format 2-2 payload. Values {1..15}.
  /// The field is mandatory present if the PUCCH-TPC-CommandConfig is provided in the PDCCH-Config for the PUCCH-SCell.
  /// The field is optionally present, need R, if the UE is configured with a PUCCH SCell in this cell group and if the
  /// PUCCH-TPC-CommandConfig is provided in the PDCCH-Config for the SpCell.
  std::optional<uint8_t> tpc_index_pucch_scell;

  bool operator==(const pucch_tpc_command_config& rhs) const
  {
    return std::tie(tpc_index_pcell, tpc_index_pucch_scell) == std::tie(rhs.tpc_index_pcell, rhs.tpc_index_pucch_scell);
  }

  bool operator!=(const pucch_tpc_command_config& rhs) const { return !(rhs == *this); }
};

} // namespace ocudu
