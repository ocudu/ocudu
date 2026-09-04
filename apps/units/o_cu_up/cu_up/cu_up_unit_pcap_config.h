// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <string>

namespace ocudu {

/// Configuration of packet capture functionalities.
struct cu_up_unit_pcap_config {
  struct {
    std::string filename;
    bool        enabled = false;
  } ngu;
  struct {
    std::string filename;
    bool        enabled = false;
  } f1u;
  struct {
    std::string filename;
    bool        enabled = false;
  } e1ap;

  /// When using the gNB app, there is no point in instantiating
  /// E1 pcaps twice. This function force disables them in the CU-UP.
  /// TODO: revisit
  void disable_e1_pcaps() { e1ap.enabled = false; }
};

} // namespace ocudu
