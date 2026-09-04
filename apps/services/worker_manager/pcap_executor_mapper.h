// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/executors/task_executor.h"

namespace ocudu {

/// Mapper of PCAP executors for the DU.
class du_pcap_executor_mapper
{
public:
  virtual ~du_pcap_executor_mapper() = default;

  /// Retrieves the executor for F1AP PCAP writing.
  virtual task_executor& get_f1ap_executor() = 0;

  /// Retrieves the executor for E2AP PCAP writing.
  virtual task_executor& get_e2ap_executor() = 0;

  /// Retrieves the executor for F1U PCAP writing.
  virtual task_executor& get_f1u_executor() = 0;

  /// Retrieves the executor for MAC PCAP writing.
  virtual task_executor& get_mac_executor() = 0;

  /// Retrieves the executor for RLC PCAP writing.
  virtual task_executor& get_rlc_executor() = 0;
};

/// Mapper of PCAP executors for the CU-CP.
class cu_cp_pcap_executor_mapper
{
public:
  virtual ~cu_cp_pcap_executor_mapper() = default;

  /// Retrieves the executor for F1AP PCAP writing.
  virtual task_executor& get_f1ap_executor() = 0;

  /// Retrieves the executor for NGAP PCAP writing.
  virtual task_executor& get_ngap_executor() = 0;

  /// Retrieves the executor for XNAP PCAP writing.
  virtual task_executor& get_xnap_executor() = 0;

  /// Retrieves the executor for E2AP PCAP writing.
  virtual task_executor& get_e2ap_executor() = 0;

  /// Retrieves the executor for E1AP PCAP writing.
  virtual task_executor& get_e1ap_executor() = 0;
};

/// Mapper of PCAP executors for the CU-UP.
class cu_up_pcap_executor_mapper
{
public:
  virtual ~cu_up_pcap_executor_mapper() = default;

  /// Retrieves the executor for E2AP PCAP writing.
  virtual task_executor& get_e2ap_executor() = 0;

  /// Retrieves the executor for F1U PCAP writing.
  virtual task_executor& get_f1u_executor() = 0;

  /// Retrieves the executor for E1AP PCAP writing.
  virtual task_executor& get_e1ap_executor() = 0;

  /// Retrieves the executor for NG-U PCAP writing.
  virtual task_executor& get_ngu_executor() = 0;
};

} // namespace ocudu
