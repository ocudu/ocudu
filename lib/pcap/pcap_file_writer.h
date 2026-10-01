// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/span.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <fstream>
#include <netinet/in.h>
#include <string>

#define PCAP_CONTEXT_HEADER_MAX 256

namespace ocudu {

// End-of-options
const uint32_t EXP_PDU_TAG_END_OF_OPT    = 0;
const uint32_t EXP_PDU_LENGTH_END_OF_OPT = 4;

// The value part should be an ASCII non NULL terminated string
// of the registered dissector used by Wireshark e.g "sip"
// Will be used to call the next dissector.
// NOTE: this is NOT a protocol name; a given protocol may have multiple
// dissectors, if, for example, the protocol headers depend on the
// protocol being used to transport the protocol in question.
const uint16_t EXP_PDU_TAG_DISSECTOR_NAME = ntohs(12);

/// This structure gets written to the start of the file
struct pcap_hdr_t {
  unsigned       magic_number;  ///< magic number
  unsigned short version_major; ///< major version number
  unsigned short version_minor; ///< minor version number
  unsigned       thiszone;      ///< GMT to local correction
  unsigned       sigfigs;       ///< accuracy of timestamps
  unsigned       snaplen;       ///< max length of captured packets, in octets
  unsigned       network;       ///< data link type
};

/// This structure precedes each packet
struct pcaprec_hdr_t {
  unsigned ts_sec;   ///< timestamp seconds
  unsigned ts_usec;  ///< timestamp microseconds
  unsigned incl_len; ///< number of octets of packet saved in file
  unsigned orig_len; ///< actual length of packet
};

/// @brief Base class for PCAP writing to files.
/// The class in *not* thread-safe. Proper protection from multiple threads
/// needs to be implemented by the user of the class.
class pcap_file_writer
{
public:
  pcap_file_writer();
  ~pcap_file_writer();
  pcap_file_writer(const pcap_file_writer& other)            = delete;
  pcap_file_writer& operator=(const pcap_file_writer& other) = delete;
  pcap_file_writer(pcap_file_writer&& other)                 = delete;
  pcap_file_writer& operator=(pcap_file_writer&& other)      = delete;

  bool is_write_enabled() const { return pcap_fstream.is_open(); }

  bool open(uint32_t dlt, const std::string& filename);
  void flush();
  void close();
  void write_pdu_header(uint32_t length, const std::string& dissector);
  void write_exported_pdu_header(const std::string& dissector);
  void write_pdu(ocudu::const_span<uint8_t> pdu);
  void write_pdu(const byte_buffer& pdu);

private:
  ocudulog::basic_logger& logger;
  std::ofstream           pcap_fstream;
  std::string             filename;
  uint32_t                dlt = 0;
  std::array<char, 4>     padding{};
};

} // namespace ocudu
