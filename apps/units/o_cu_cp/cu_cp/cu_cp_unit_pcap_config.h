// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <string>

namespace ocudu {

/// Configuration of packet capture functionalities.
struct cu_cp_unit_pcap_config {
  struct {
    std::string filename = "/tmp/cu_ngap.pcap";
    bool        enabled  = false;
  } ngap;
  struct {
    std::string filename = "/tmp/cu_xnap.pcap";
    bool        enabled  = false;
  } xnap;
  struct {
    std::string filename = "/tmp/cu_e1ap.pcap";
    bool        enabled  = false;
  } e1ap;
  struct {
    std::string filename = "/tmp/cu_f1ap.pcap";
    bool        enabled  = false;
  } f1ap;
};

} // namespace ocudu
