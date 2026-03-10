// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// AFL++/libFuzzer harness for the OFH VLAN Ethernet frame decoder.
///
/// Targets VLAN frame header parsing including MAC address extraction and
/// Ethertype field validation.
///
/// Build / Run: see ofh_uplane_decoder_fuzzer.cpp for the pattern; replace
/// target name with ofh_vlan_frame_decoder_fuzzer and corpus path with
/// tests/fuzz/ofh/corpus/vlan.

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ofh/ethernet/ethernet_factories.h"
#include <cstddef>
#include <cstdint>
#include <memory>

using namespace ocudu;
using namespace ether;

namespace {

/// Returns a static decoder instance (the factory call allocates once).
vlan_frame_decoder& get_decoder()
{
  static std::unique_ptr<vlan_frame_decoder> dec = create_vlan_frame_decoder(ocudulog::fetch_basic_logger("FUZZ"), 0);
  return *dec;
}

} // namespace

extern "C" int LLVMFuzzerInitialize(int* /*argc*/, char*** /*argv*/)
{
  ocudulog::fetch_basic_logger("FUZZ").set_level(ocudulog::basic_levels::none);
  return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  span<const uint8_t> packet(data, size);
  vlan_frame_params   params{};
  (void)get_decoder().decode(packet, params);
  return 0;
}
