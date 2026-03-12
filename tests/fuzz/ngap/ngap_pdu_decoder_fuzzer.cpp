// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// AFL++/libFuzzer harness for the NGAP ASN.1 PDU decoder.
///
/// Exercises asn1::ngap::ngap_pdu_c::unpack(), which is the first parsing step
/// for every NGAP message arriving from the AMF over the N2 interface.  This
/// mirrors the exact code path in ngap_asn1_packer::handle_packed_pdu() and
/// covers all three NGAP PDU types (initiatingMessage, successfulOutcome,
/// unsuccessfulOutcome) as well as every IE parser reachable from them.
///
/// Build
/// -----
/// Configure with -DENABLE_FUZZTESTS=ON and compile with afl-clang-fast++:
///
///   CXX=afl-clang-fast++ CC=afl-clang-fast \
///   cmake -DENABLE_FUZZTESTS=ON -DENABLE_ASAN=ON \
///         -DCMAKE_BUILD_TYPE=Debug <src_dir>
///   make ngap_pdu_decoder_fuzzer
///
/// Run
/// ---
///   mkdir -p findings/ngap
///   afl-fuzz -i tests/fuzz/ngap/corpus/ngap -o findings/ngap -- \
///       ./tests/fuzz/ngap/ngap_pdu_decoder_fuzzer @@

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/ngap/ngap.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <cstddef>
#include <cstdint>

using namespace ocudu;

/// Called once before any test inputs to suppress log noise during fuzzing.
extern "C" int LLVMFuzzerInitialize(int* /*argc*/, char*** /*argv*/)
{
  ocudulog::fetch_basic_logger("FUZZ").set_level(ocudulog::basic_levels::none);
  return 0;
}

/// Entry point called by AFL++ / libFuzzer for each mutated input.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  // Wrap the fuzzer input in a byte_buffer.  The fallback-allocation constructor
  // is used so that inputs which exhaust the default pool do not abort the
  // fuzzer process.
  byte_buffer buf{byte_buffer::fallback_allocation_tag{}, span<const uint8_t>(data, size)};

  // Drive the NGAP PER decoder — this mirrors ngap_asn1_packer::handle_packed_pdu().
  asn1::cbit_ref         bref{buf};
  asn1::ngap::ngap_pdu_c pdu;
  (void)pdu.unpack(bref);

  return 0;
}
