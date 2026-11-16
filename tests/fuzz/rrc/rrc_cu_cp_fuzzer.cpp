// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
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
/// The F1AP wrapper and the PDCP PDU are scaffolding built by the harness; only the RRC messages
/// inside are mutated. Fuzzing the F1AP wrapper itself belongs in tests/fuzz/f1ap.
///
/// An input is a sequence of steps. A step either injects an RRC message, or has the AMF, the DU or
/// the clock start a CU-CP procedure, so that the RRC messages after it answer a procedure that
/// waits for them: most UL-DCCH messages are only handled while one does.
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
#include <cstdlib>
#include <mutex>
#include <vector>

using namespace ocudu;
using namespace ocucp;
using namespace ocucp::fuzz;

namespace {

fuzz_state*    g_state = nullptr;
std::once_flag g_init_flag;

// ---------------------------------------------------------------------------
// Input encoding
// ---------------------------------------------------------------------------

/// Decode the control byte into the UE state reached before the steps run. See tests/fuzz/README.md for the bit layout.
ue_state decode_header(uint8_t byte)
{
  return static_cast<ue_state>(byte & 0b0000'0011U);
}

/// Step of an input. See tests/fuzz/README.md for what each one does.
enum class step_type : uint8_t {
  ul_ccch,
  ul_dcch_srb1,
  ul_dcch_srb2,
  advance_time,
  initial_context_setup,
  pdu_session_setup,
  pdu_session_modification,
  pdu_session_release,
  dl_nas_transport,
  ue_context_modification,
  amf_ue_context_release,
  du_ue_context_release,
  nof_steps
};

struct step {
  step_type           type;
  span<const uint8_t> payload;
};

/// Upper bound on the steps taken from one input.
///
/// A sequence that drives a procedure to its end is short; the cap stops a mutation that is all
/// empty steps from spending the whole iteration on them.
constexpr unsigned max_nof_steps = 16;

/// Split an input into its steps.
///
/// Each step is a type byte, a length byte and that many bytes of payload. The type is taken modulo
/// the number of step types, so that every byte value names one. A truncated final step ends the
/// sequence, so that a mutation shortening the input drops a step instead of invalidating every step
/// after it.
std::vector<step> split_steps(span<const uint8_t> input)
{
  std::vector<step> steps;
  size_t            offset = 0;
  while (offset + 2 <= input.size() and steps.size() < max_nof_steps) {
    const auto   type = static_cast<step_type>(input[offset] % static_cast<uint8_t>(step_type::nof_steps));
    const size_t len  = input[offset + 1];
    offset += 2;
    if (offset + len > input.size()) {
      break;
    }
    steps.push_back(step{type, input.subspan(offset, len)});
    offset += len;
  }
  return steps;
}

/// Milliseconds the clock advances per unit of an advance_time step payload byte.
///
/// The largest step, 256 units or 4096 ms, then covers the RRC procedure guard timers of a few seconds.
constexpr unsigned ms_per_time_unit = 16;

void run_step(fuzz_ue& ue, const step& s)
{
  switch (s.type) {
    case step_type::ul_ccch:
      if (!s.payload.empty()) {
        ue.inject_ccch(s.payload);
      }
      break;
    case step_type::ul_dcch_srb1:
    case step_type::ul_dcch_srb2:
      if (!s.payload.empty()) {
        ue.inject_dcch(s.type == step_type::ul_dcch_srb2 ? srb_id_t::srb2 : srb_id_t::srb1,
                       fuzz_ue::make_buffer(s.payload));
      }
      break;
    case step_type::advance_time:
      g_state->advance_time(((s.payload.empty() ? 0U : s.payload[0]) + 1U) * ms_per_time_unit);
      break;
    case step_type::initial_context_setup:
      ue.start_initial_context_setup();
      break;
    case step_type::pdu_session_setup:
      ue.start_pdu_session_setup();
      break;
    case step_type::pdu_session_modification:
      ue.modify_pdu_session();
      break;
    case step_type::pdu_session_release:
      ue.release_pdu_session();
      break;
    case step_type::dl_nas_transport:
      ue.send_dl_nas_transport();
      break;
    case step_type::ue_context_modification:
      ue.modify_ue_context();
      break;
    case step_type::amf_ue_context_release:
      ue.release_from_amf();
      break;
    case step_type::du_ue_context_release:
      ue.release_from_du();
      break;
    case step_type::nof_steps:
      break;
  }
}

void ensure_state()
{
  std::call_once(g_init_flag, []() {
    // Started here rather than in LLVMFuzzerInitialize so that the backend thread is created after AFL++ forks.
    ocudulog::init();
    // Construct the byte_buffer pools before registering the atexit handler below, so that they are destroyed after it
    // has drained the log entries holding byte_buffers.
    (void)get_default_byte_buffer_segment_pool();
    (void)get_default_fallback_byte_buffer_segment_pool();
    g_state = new fuzz_state();

    std::atexit([]() {
      g_state->worker.stop();
      // Drain pending log entries while the byte_buffer pool their arguments point into is still alive.
      ocudulog::flush();
    });

    // Bring one UE all the way up before fuzzing starts. A broken security bring-up is otherwise
    // invisible: every payload would be dropped by PDCP on integrity failure and the harness would
    // report healthy throughput while covering nothing past RRC Setup. Likewise, a PDU session setup
    // that stalls would leave the last UE state no different from the secured one.
    fuzz_ue probe{*g_state};
    probe.drive_to(ue_state::awaiting_reconfiguration_complete);
    if (!probe.is_awaiting_reconfiguration_complete()) {
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
  // The first byte selects the UE state; the rest is the sequence of steps.
  if (size < 3) {
    return 0;
  }
  ensure_state();

  const std::vector<step> steps = split_steps(span<const uint8_t>(data + 1, size - 1));
  if (steps.empty()) {
    return 0;
  }

  fuzz_ue ue{*g_state};
  if (!ue.is_valid()) {
    return 0;
  }
  ue.drive_to(decode_header(data[0]));

  for (const step& s : steps) {
    run_step(ue, s);
  }

  return 0;
}
