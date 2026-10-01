// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_integer.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <optional>

namespace ocudu {

/// \brief Parameter "Qhyst" in TS 38.304. Value in dB.
///
/// Specifies the hysteresis value for ranking criteria.
enum class q_hyst_t {
  db0  = 0,
  db1  = 1,
  db2  = 2,
  db3  = 3,
  db4  = 4,
  db5  = 5,
  db6  = 6,
  db8  = 8,
  db10 = 10,
  db12 = 12,
  db14 = 14,
  db16 = 16,
  db18 = 18,
  db20 = 20,
  db22 = 22,
  db24 = 24
};

/// \brief IE "ReselectionThreshold" in TS 38.331.
///
/// Indicates an Rx level threshold for cell reselection. Actual value of threshold = reselection_threshold_t * 2 [dB].
using reselection_threshold_t = bounded_integer<uint8_t, 0, 31>;

/// \brief IE "ReselectionThresholdQ" in TS 38.331.
///
/// Indicates a quality level threshold for cell reselection in dB.
using reselection_threshold_q_t = bounded_integer<uint8_t, 0, 31>;

/// \brief IE "Q-RxLevMin" in TS 38.331.
///
/// Indicates the required minimum received RSRP level in the (NR) cell, for cell selection/reselection.
/// Actual value Qrxlevmin = field value * 2 [dBm].
using q_rx_lev_min_t = bounded_integer<int8_t, -70, -22>;

/// \brief IE "CellReselectionPriority" in TS 38.331.
///
/// Integer part of the absolute priority of the concerned carrier frequency, as used in the cell reselection procedure.
using cell_reselection_priority_t = bounded_integer<uint8_t, 0, 7>;

/// \brief IE "T-ReselectionNR" in TS 38.331.
///
/// Cell reselection timer "TreselectionRAT" for NR and E-UTRA. Value in seconds.
using t_reselection_t = bounded_integer<uint8_t, 0, 7>;

/// \brief IE "Q-OffsetRange" in TS 38.331.
///
/// Indicates a cell, beam or measurement object specific offset to be applied when evaluating candidates for cell
/// reselection or when evaluating conditions for measurement reporting.
enum class q_offset_range_t {
  db_24 = -24,
  db_22 = -22,
  db_20 = -20,
  db_18 = -18,
  db_16 = -16,
  db_14 = -14,
  db_12 = -12,
  db_10 = -10,
  db_8  = -8,
  db_6  = -6,
  db_5  = -5,
  db_4  = -4,
  db_3  = -3,
  db_2  = -2,
  db_1  = -1,
  db0   = 0,
  db1   = 1,
  db2   = 2,
  db3   = 3,
  db4   = 4,
  db5   = 5,
  db6   = 6,
  db8   = 8,
  db10  = 10,
  db12  = 12,
  db14  = 14,
  db16  = 16,
  db18  = 18,
  db20  = 20,
  db22  = 22,
  db24  = 24
};

/// \brief Intra-frequency neighbor cell information for cell reselection. See "IntraFreqNeighCellInfo" in TS 38.331.
struct intra_freq_neigh_cell_info {
  /// Physical cell identifier.
  pci_t pci;
  /// Parameter "Qoffsets,n" in TS 38.304. Specifies the offset between the two cells.
  q_offset_range_t q_offset_cell;
};

/// \brief Inter-frequency carrier information for cell reselection. See "InterFreqCarrierFreqInfo" in TS 38.331.
struct inter_freq_carrier_freq_info {
  /// ARFCN of the carrier frequency.
  uint32_t arfcn;
  /// SSB subcarrier spacing.
  subcarrier_spacing ssb_scs;
  bool               derive_ssb_index_from_cell;
  /// \brief Parameter "Qrxlevmin" in TS 38.304.
  ///
  /// Specifies the minimum required Rx level in the cell in dBm.
  q_rx_lev_min_t q_rx_lev_min;
  /// \brief Parameter "Threshx,HighP" in TS 38.304.
  ///
  /// Specifies the Rx level threshold used by the UE when reselecting towards a higher priority RAT/frequency than the
  /// current serving frequency.
  reselection_threshold_t thresh_x_high_p;
  /// \brief Parameter "Threshx,LowP" in TS 38.304.
  ///
  /// Specifies the Rx level threshold used by the UE when reselecting towards a lower priority RAT/frequency than the
  /// current serving frequency.
  reselection_threshold_t thresh_x_low_p;
  // \brief Parameter "Qoffsetfrequency" in TS 38.304.
  //
  // Frequency specific offset for equal priority NR frequencies.
  q_offset_range_t q_offset_freq = q_offset_range_t::db0;
};

/// \brief IE "EUTRA-AllowedMeasBandwidth" in TS 38.331.
///
/// Indicates the maximum allowed measurement bandwidth on a carrier frequency as defined by the parameter Transmission
/// Bandwidth Configuration "NRB" in TS 36.104. Value in resource blocks.
enum class eutra_allowed_meas_bandwidth_t : uint8_t {
  mbw6   = 6,
  mbw15  = 15,
  mbw25  = 25,
  mbw50  = 50,
  mbw75  = 75,
  mbw100 = 100
};

/// \brief EUTRA carrier frequency information for cell reselection. See "CarrierFreqEUTRA" in TS 38.331.
struct carrier_freq_eutra {
  /// EUTRA ARFCN of the carrier frequency.
  uint32_t earfcn;
  /// Maximum allowed measurement bandwidth.
  eutra_allowed_meas_bandwidth_t allowed_meas_bandwidth;
  /// Indicates whether all the neighbouring cells use Antenna Port 1. When set to true, the UE may assume that at least
  /// two cell-specific antenna ports are used in all neighbouring cells.
  bool presence_antenna_port1;
  // Integer part of the cell reselection priority for this frequency.
  std::optional<cell_reselection_priority_t> cell_reselection_priority;
  /// \brief Parameter "ThreshX,HighP" in TS 38.304.
  ///
  /// Specifies the Rx level threshold used by the UE when reselecting towards a higher priority RAT/frequency than the
  /// current serving frequency.
  reselection_threshold_t thresh_x_high;
  /// \brief Parameter "ThreshX,LowP" in TS 38.304.
  ///
  /// Specifies the Rx level threshold used by the UE when reselecting towards a lower priority RAT/frequency than the
  /// current serving frequency.
  reselection_threshold_t thresh_x_low;
  /// Parameter "Qrxlevmin" in TS 36.304.
  q_rx_lev_min_t q_rx_lev_min;
  /// \brief Parameter "Qqualmin" in TS 36.304.
  ///
  /// Actual value Qqualmin = field value [dB].
  bounded_integer<int8_t, -34, -3> q_qual_min;
  /// The maximum allowed transmission power in dBm on the (uplink) carrier frequency, see TS 36.304.
  bounded_integer<int8_t, -30, 33> p_max_eutra;
};

} // namespace ocudu
