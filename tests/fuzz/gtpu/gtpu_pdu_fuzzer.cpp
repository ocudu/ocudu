// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/gtpu/gtpu_pdu.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <cstddef>
#include <cstdint>
#include <utility>

using namespace ocudu;

extern "C" int LLVMFuzzerInitialize(int*, char***)
{
  ocudulog::fetch_basic_logger("GTPU").set_level(ocudulog::basic_levels::none);
  return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  if (size > 65535U + GTPU_BASE_HEADER_LEN) {
    return 0;
  }

  byte_buffer        raw_pdu{byte_buffer::fallback_allocation_tag{}, span<const uint8_t>(data, size)};
  gtpu_dissected_pdu dissected;
  gtpu_tunnel_logger logger{"GTPU", {gtpu_logical_interface::invalid, {}, gtpu_teid_t{1}, "RX"}};

  if (gtpu_dissect_pdu(dissected, std::move(raw_pdu), logger)) {
    (void)gtpu_extract_msg(std::move(dissected));
  }

  return 0;
}
