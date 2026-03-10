// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// AFL++/libFuzzer harness for the OFH eCPRI packet decoder.
///
/// Exercises both decoder variants (honour-header and ignore-header payload
/// size modes) on every input so a single corpus covers both code paths.
///
/// Build / Run: see ofh_uplane_decoder_fuzzer.cpp for the pattern; replace
/// target name with ofh_ecpri_decoder_fuzzer and corpus path with
/// tests/fuzz/ofh/corpus/ecpri.

#include "lib/ofh/ecpri/ecpri_packet_decoder_impl.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <cstddef>
#include <cstdint>

using namespace ocudu;
using namespace ecpri;

extern "C" int LLVMFuzzerInitialize(int* /*argc*/, char*** /*argv*/)
{
  ocudulog::fetch_basic_logger("FUZZ").set_level(ocudulog::basic_levels::none);
  return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  // Both decoder variants are stateless; construct them on the stack cheaply.
  packet_decoder_use_header_payload_size    dec_hdr(ocudulog::fetch_basic_logger("FUZZ"), 0);
  packet_decoder_ignore_header_payload_size dec_ign(ocudulog::fetch_basic_logger("FUZZ"), 0);

  span<const uint8_t> packet(data, size);
  packet_parameters   params{};

  (void)dec_hdr.decode(packet, params);
  (void)dec_ign.decode(packet, params);

  return 0;
}
