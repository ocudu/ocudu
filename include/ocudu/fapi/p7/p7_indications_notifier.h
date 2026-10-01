// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace fapi {

struct crc_indication;
struct rach_indication;
struct rx_data_indication;
struct srs_indication;
struct uci_indication;

/// \brief Slot-based, data-specific message notifier interface.
///
/// This interface notifies the reception of slot-based, P7 FAPI indications from the underlying PHY.
class p7_indications_notifier
{
public:
  virtual ~p7_indications_notifier() = default;

  /// \brief Notifies the reception of a RX data indication message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_rx_data_indication(const rx_data_indication& msg) = 0;

  /// \brief Notifies the reception of a CRC indication message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_crc_indication(const crc_indication& msg) = 0;

  /// \brief Notifies the reception of a UCI indication message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_uci_indication(const uci_indication& msg) = 0;

  /// \brief Notifies the reception of a SRS indication message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_srs_indication(const srs_indication& msg) = 0;

  /// \brief Notifies the reception of a RACH indication message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_rach_indication(const rach_indication& msg) = 0;
};

} // namespace fapi
} // namespace ocudu
