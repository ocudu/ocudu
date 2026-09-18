// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// AFL++/libFuzzer harness injecting RRC messages through a complete CU-CP.
///
/// A real CU-CP is built with stub AMF, CU-UP and DU peers attached, and the fuzz input travels the
/// full production receive path:
///
///   fuzz input -> F1AP UL RRC Message Transfer -> CU-CP UE lookup -> PDCP -> RRC ASN.1 UPER decode
///               -> RRC UE state machine -> procedures -> DL message generation
///
/// The F1AP wrapper and the PDCP PDU are scaffolding built by the harness; only the RRC message
/// inside is mutated. Fuzzing the F1AP wrapper itself belongs in tests/fuzz/f1ap.
///
/// A UE is created and released for every input, so a crash reproduces from its input file alone.
/// The UE can be driven up to and including AS security activation: the DU side runs PDCP TX
/// entities keyed with the same AS keys the CU-CP derives, so a mutated payload carries a MAC-I the
/// CU-CP accepts instead of being dropped on integrity failure.
///
/// See tests/fuzz/README.md for the input layout, build instructions and run commands.

#include "tests/fuzz/cu_cp/cu_cp_fuzz_env.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/rb_id.h"
#include <cstddef>
#include <cstdint>
#include <mutex>

using namespace ocudu;
using namespace ocucp;
using namespace ocucp::fuzz;

namespace {

// ---------------------------------------------------------------------------
// Input encoding
// ---------------------------------------------------------------------------

/// Decoded control byte prefixed to every input.
struct input_header {
  bool     is_dcch;
  srb_id_t srb_id;
  ue_state state;
};

/// Decode the control byte. See tests/fuzz/README.md for the bit layout.
input_header decode_header(uint8_t byte)
{
  // State 3 is unused; fold it onto the secured state so that no input is silently dropped.
  const uint8_t state_bits = (byte >> 2U) & 0b0000'0011U;
  return input_header{.is_dcch = (byte & 0b0000'0001U) != 0,
                      .srb_id  = (byte & 0b0000'0010U) != 0 ? srb_id_t::srb2 : srb_id_t::srb1,
                      .state   = static_cast<ue_state>(state_bits == 3 ? 2 : state_bits)};
}

fuzz_state*    g_state = nullptr;
std::once_flag g_init_flag;

void ensure_state()
{
  std::call_once(g_init_flag, []() {
    g_state = new fuzz_state();

    // Bring one UE all the way up before fuzzing starts. A broken security bring-up is otherwise
    // invisible: every payload would be dropped by PDCP on integrity failure and the harness would
    // report healthy throughput while covering nothing past RRC Setup.
    fuzz_ue probe{*g_state};
    probe.drive_to(ue_state::secured);
    if (!probe.is_secured()) {
      std::abort();
    }
  });
}

} // namespace

// ---------------------------------------------------------------------------
// LLVMFuzzer interface
// ---------------------------------------------------------------------------

/// Route logs to /dev/null at debug level before any forking occurs.
///
/// The level stays at debug so that the log formatting code runs: it walks the decoded,
/// attacker-controlled ASN.1 structures, which makes it part of the surface under test.
extern "C" int LLVMFuzzerInitialize(int* /*argc*/, char*** /*argv*/)
{
  if (ocudulog::sink* null_sink = ocudulog::create_file_sink("/dev/null"); null_sink != nullptr) {
    ocudulog::set_default_sink(*null_sink);
  }
  for (const char* name : {"NGAP", "CU-CP", "RRC", "PDCP", "SEC", "F1AP", "E1AP", "XNAP", "ALL"}) {
    ocudulog::fetch_basic_logger(name).set_level(ocudulog::basic_levels::debug);
  }
  // NOTE: do NOT call ensure_state() here. The state, and its task_worker thread, must be created
  // after AFL++ forks so that each child has its own thread.
  return 0;
}

/// Entry point called by AFL++ / libFuzzer for each mutated input.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  // The first byte selects the channel and the UE state; the rest is the RRC PDU.
  if (size < 2) {
    return 0;
  }
  ensure_state();

  const input_header hdr = decode_header(data[0]);
  fuzz_ue            ue{*g_state};
  if (!ue.is_valid()) {
    return 0;
  }
  ue.drive_to(hdr.state);

  const span<const uint8_t> payload(data + 1, size - 1);
  if (hdr.is_dcch) {
    ue.inject_dcch(hdr.srb_id, fuzz_ue::make_buffer(payload));
  } else {
    ue.inject_ccch(payload);
  }

  return 0;
}
