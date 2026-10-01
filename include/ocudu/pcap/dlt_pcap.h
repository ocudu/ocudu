// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/span.h"
#include <cstdint>
#include <memory>

namespace ocudu {

class task_executor;

/// \brief Interface class for writing a DLT PCAP to a file.
class dlt_pcap
{
public:
  virtual ~dlt_pcap() = default;

  /// Flush all the pending writes to the PCAP file.
  virtual void flush() = 0;

  /// Close the PCAP file.
  virtual void close() = 0;

  /// Whether writing to the PCAP file is enabled.
  virtual bool is_write_enabled() const = 0;

  /// Push a PDU to be written to the PCAP file.
  virtual void push_pdu(const_span<uint8_t> pdu) = 0;

  /// Push a PDU to be written to the PCAP file.
  virtual void push_pdu(byte_buffer pdu) = 0;
};

/// \brief Creates a layer DLT PCAP sink that writes the incoming PDUs to a pcap file.
std::unique_ptr<dlt_pcap> create_null_dlt_pcap();
std::unique_ptr<dlt_pcap> create_ngap_pcap(const std::string& filename, task_executor& backend_exec);
std::unique_ptr<dlt_pcap> create_xnap_pcap(const std::string& filename, task_executor& backend_exec);
std::unique_ptr<dlt_pcap> create_f1ap_pcap(const std::string& filename, task_executor& backend_exec);
std::unique_ptr<dlt_pcap> create_e1ap_pcap(const std::string& filename, task_executor& backend_exec);
std::unique_ptr<dlt_pcap> create_gtpu_pcap(const std::string& filename, task_executor& backend_exec);
std::unique_ptr<dlt_pcap> create_e2ap_pcap(const std::string& filename, task_executor& backend_exec);

/// Null sink for DLT pcap messages. This is useful in unit tests and when the application disables pcaps.
class null_dlt_pcap : public dlt_pcap
{
public:
  void flush() override {}
  void close() override {}
  bool is_write_enabled() const override { return false; }
  void push_pdu(const_span<uint8_t> pdu) override {}
  void push_pdu(byte_buffer pdu) override {}
};

} // namespace ocudu
