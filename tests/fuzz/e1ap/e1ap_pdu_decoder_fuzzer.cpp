// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/e1ap/common/e1ap_asn1_packer.h"
#include <cstddef>
#include <cstdint>

using namespace ocudu;

namespace {

class gateway_sink final : public sctp_network_gateway_data_handler
{
public:
  void handle_pdu(const byte_buffer&) override {}
};

class message_sink final : public e1ap_message_handler
{
public:
  void handle_message(const e1ap_message&) override {}
};

} // namespace

extern "C" int LLVMFuzzerInitialize(int*, char***)
{
  ocudulog::fetch_basic_logger("E1AP-ASN1-PCK").set_level(ocudulog::basic_levels::none);
  ocudulog::fetch_basic_logger("ASN1").set_level(ocudulog::basic_levels::none);
  return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  if (size > 64U * 1024U) {
    return 0;
  }

  byte_buffer      input{byte_buffer::fallback_allocation_tag{}, span<const uint8_t>(data, size)};
  gateway_sink     gateway;
  message_sink     messages;
  e1ap_asn1_packer packer{gateway, messages};
  packer.handle_packed_pdu(input);
  return 0;
}
