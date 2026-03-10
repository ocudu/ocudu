// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// AFL++/libFuzzer harness for the OFH U-Plane message decoder.
///
/// Targets the static-compression U-Plane decoder, exercising packet header
/// parsing, section decoding, and the peek helpers.  A no-op IQ decompressor
/// is used so the fuzzer focuses on the parsing layer rather than the
/// compression arithmetic.
///
/// Build
/// -----
/// Configure with -DENABLE_FUZZTESTS=ON and compile with afl-clang-fast++:
///
///   CXX=afl-clang-fast++ cmake -DENABLE_FUZZTESTS=ON \
///       -DENABLE_ASAN=ON -DCMAKE_BUILD_TYPE=Debug <src_dir>
///   make ofh_uplane_decoder_fuzzer
///
/// Run
/// ---
///   mkdir findings
///   afl-fuzz -i tests/fuzz/ofh/corpus/uplane -o findings -- \
///       ./tests/fuzz/ofh/ofh_uplane_decoder_fuzzer @@

#include "lib/ofh/serdes/ofh_uplane_message_decoder_static_compression_impl.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ofh/compression/iq_decompressor.h"
#include "ocudu/ofh/serdes/ofh_uplane_message_decoder_properties.h"
#include "ocudu/ran/cyclic_prefix.h"
#include <cstddef>
#include <cstdint>

using namespace ocudu;
using namespace ofh;

namespace {

/// No-op IQ decompressor - keeps the fuzzer focused on packet parsing.
class null_iq_decompressor : public iq_decompressor
{
public:
  void decompress(span<cbf16_t>, span<const uint8_t>, const ru_compression_params&) override {}
};

/// Returns a static decoder instance configured for no-compression / 16-bit samples.
uplane_message_decoder_static_compression_impl& get_decoder()
{
  static uplane_message_decoder_static_compression_impl dec(ocudulog::fetch_basic_logger("FUZZ"),
                                                            subcarrier_spacing::kHz30,
                                                            get_nsymb_per_slot(cyclic_prefix::NORMAL),
                                                            273,
                                                            0,
                                                            std::make_unique<null_iq_decompressor>(),
                                                            {compression_type::none, 16});
  return dec;
}

} // namespace

/// Called once before any test inputs to suppress log noise during fuzzing.
extern "C" int LLVMFuzzerInitialize(int* /*argc*/, char*** /*argv*/)
{
  ocudulog::fetch_basic_logger("FUZZ").set_level(ocudulog::basic_levels::none);
  return 0;
}

/// Entry point called by AFL++ / libFuzzer for each mutated input.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  span<const uint8_t> packet(data, size);

  // Exercise the packet peek helpers (lightweight, no decompressor involvement).
  (void)uplane_peeker::peek_filter_index(packet);
  (void)uplane_peeker::peek_slot_symbol_point(
      packet, get_nsymb_per_slot(cyclic_prefix::NORMAL), subcarrier_spacing::kHz30);

  // Exercise the full decoder.
  uplane_message_decoder_results results;
  (void)get_decoder().decode(results, packet);

  return 0;
}
