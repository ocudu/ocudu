// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// AFL++/libFuzzer harness exercising the full CU-CP NGAP receive path.
///
/// Unlike the ASN.1-only harness (ngap_pdu_decoder_fuzzer), this harness spins up a real CU-CP and
/// injects decoded NGAP messages into it, so the whole receive stack runs:
///
///   fuzz input -> ASN.1 PER decode -> NGAP validators -> procedure dispatcher
///               -> procedure state-machine -> response generation
///
/// A UE is brought up on the CU-CP before each input, and the message is pointed at it. Most of the
/// NGAP layer is UE-associated and is rejected at the UE lookup without one, which leaves only the
/// handful of non-UE-associated procedures reachable.
///
/// See tests/fuzz/README.md for the input layout, build instructions and run commands.

#include "tests/fuzz/cu_cp/cu_cp_fuzz_env.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/ngap/ngap_message.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <cstddef>
#include <cstdint>
#include <mutex>

using namespace ocudu;
using namespace ocucp;
using namespace ocucp::fuzz;

namespace {

/// Decoded control byte prefixed to every input.
struct input_header {
  ue_state state;
};

/// Decode the control byte. See tests/fuzz/README.md for the bit layout.
input_header decode_header(uint8_t byte)
{
  // State 3 is unused; fold it onto the secured state so that no input is silently dropped.
  const uint8_t state_bits = byte & 0b0000'0011U;
  return input_header{.state = static_cast<ue_state>(state_bits == 3 ? 2 : state_bits)};
}

fuzz_state*    g_state = nullptr;
std::once_flag g_init_flag;

void ensure_state()
{
  std::call_once(g_init_flag, []() {
    g_state = new fuzz_state();

    // Bring one UE all the way up before fuzzing starts, so that a broken bring-up shows as an abort
    // rather than as a harness that only ever reaches the UE lookup.
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
  for (const char* name : {"NGAP", "CU-CP", "RRC", "PDCP", "SEC", "F1AP", "E1AP", "NRPPA", "XNAP", "ALL"}) {
    ocudulog::fetch_basic_logger(name).set_level(ocudulog::basic_levels::debug);
  }
  // NOTE: do NOT call ensure_state() here. The state, and its task_worker thread, must be created
  // after AFL++ forks so that each child has its own thread.
  return 0;
}

/// Entry point called by AFL++ / libFuzzer for each mutated input.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  // The first byte selects the UE state; the rest is the NGAP PDU.
  if (size < 2) {
    return 0;
  }
  ensure_state();

  const input_header hdr = decode_header(data[0]);

  // Inputs that fail to decode are dropped, mirroring ngap_asn1_packer::handle_packed_pdu(): the
  // CU-CP never sees them in production, so injecting them would only exercise unreachable states.
  // The ASN.1 layer itself is covered by ngap_pdu_decoder_fuzzer.
  byte_buffer    buf{byte_buffer::fallback_allocation_tag{}, span<const uint8_t>(data + 1, size - 1)};
  asn1::cbit_ref bref{buf};
  ngap_message   msg{};
  if (msg.pdu.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
    return 0;
  }

  fuzz_ue ue{*g_state};
  if (!ue.is_valid()) {
    return 0;
  }
  ue.drive_to(hdr.state);

  // Point the message at the UE that was just brought up.
  if (std::optional<ran_ue_id_t> ran_ue_id = ue.get_ran_ue_id(); ran_ue_id.has_value()) {
    set_ue_ids(msg.pdu, ran_ue_id.value(), fuzz_ue::get_amf_ue_id());
  }

  g_state->amf.push_tx_pdu(msg);
  g_state->drain();

  return 0;
}
