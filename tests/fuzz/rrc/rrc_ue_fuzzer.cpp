// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// AFL++/libFuzzer harness for the RRC UE uplink receive path.
///
/// The harness drives an rrc_ue_impl directly, without a CU-CP, PDCP or F1AP around it:
///
///   fuzz input -> RRC ASN.1 UPER decode -> RRC UE state machine -> procedures -> DL message generation
///
/// Injecting above PDCP is what makes the layer fuzzable. Once security is activated PDCP verifies
/// a MAC-I over the RRC PDU, so a mutated payload is rejected and the UE released before RRC sees
/// it. Above PDCP the integrity_verified flag becomes a fuzzer-controlled input bit, which reaches
/// the TS 38.331 Annex B1 gating branches in rrc_ue_impl::handle_pdu(): a protected message
/// arriving unprotected, and an unprotected message arriving protected.
///
/// The input carries a control byte selecting the logical channel, the SRB, the integrity flag and
/// the UE state to reach before the payload is injected. See tests/fuzz/README.md for the layout,
/// build instructions and run commands.

#include "lib/cu_cp/pdcp/srb_pdcp_ue_context.h"
#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "lib/rrc/ue/rrc_ue_impl.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "tests/test_doubles/security/security_test_keys.h"
#include "tests/unittests/rrc/test_helpers.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/security/security.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/timers.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>

using namespace ocudu;
using namespace ocucp;

namespace {

// ---------------------------------------------------------------------------
// Input encoding
// ---------------------------------------------------------------------------

/// UE state reached before the fuzzed payload is injected.
enum class ue_state : uint8_t {
  /// No RRC connection. Only UL-CCCH is meaningful here.
  fresh = 0,
  /// RRCSetupRequest handled, RRCSetup sent, awaiting RRCSetupComplete.
  awaiting_setup_complete = 1,
  /// RRCSetupComplete handled. SRB1 is up, AS security is not active.
  connected = 2,
  /// AS security activated on SRB1 and SRB2 created.
  secured = 3,
};

/// Decoded control byte prefixed to every input.
struct input_header {
  bool     is_dcch;
  bool     integrity_verified;
  srb_id_t srb_id;
  ue_state state;
};

/// Decode the control byte. See tests/fuzz/README.md for the bit layout.
input_header decode_header(uint8_t byte)
{
  return input_header{.is_dcch            = (byte & 0b0000'0001U) != 0,
                      .integrity_verified = (byte & 0b0000'0010U) != 0,
                      .srb_id             = (byte & 0b0000'0100U) != 0 ? srb_id_t::srb2 : srb_id_t::srb1,
                      .state              = static_cast<ue_state>((byte >> 3U) & 0b0000'0011U)};
}

// ---------------------------------------------------------------------------
// Canned messages used to drive the UE into the requested state
// ---------------------------------------------------------------------------
//
// Taken from tests/unittests/rrc/rrc_ue_test_helpers.h. The DCCH vectors there are PDCP PDUs; the
// 2-byte PDCP header and the 4-byte MAC-I are stripped here because the harness injects above PDCP.

/// UL-DCCH RRCSetupComplete, carrying a NAS Registration Request.
constexpr std::array<uint8_t, 125> rrc_setup_complete = {
    0x10, 0xc0, 0x10, 0x00, 0x08, 0x27, 0x27, 0xe0, 0x1c, 0x3f, 0xf1, 0x00, 0xc0, 0x47, 0xe0, 0x04, 0x13, 0x90,
    0x00, 0xbf, 0x20, 0x2f, 0x89, 0x98, 0x00, 0x04, 0x10, 0x00, 0x00, 0x00, 0xf2, 0xe0, 0x4f, 0x07, 0x0f, 0x07,
    0x07, 0x10, 0x05, 0x17, 0xe0, 0x04, 0x13, 0x90, 0x00, 0xbf, 0x20, 0x2f, 0x89, 0x98, 0x00, 0x04, 0x10, 0x00,
    0x00, 0x00, 0xf1, 0x00, 0x10, 0x32, 0xe0, 0x4f, 0x07, 0x0f, 0x07, 0x02, 0xf1, 0xb0, 0x80, 0x10, 0x02, 0x7d,
    0xb0, 0x00, 0x00, 0x00, 0x00, 0x80, 0x10, 0x1b, 0x66, 0x90, 0x00, 0x00, 0x00, 0x00, 0x80, 0x10, 0x00, 0x00,
    0x10, 0x00, 0x00, 0x00, 0x05, 0x20, 0x2f, 0x89, 0x90, 0x00, 0x00, 0x11, 0x70, 0x7f, 0x07, 0x0c, 0x04, 0x01,
    0x98, 0x0b, 0x01, 0x80, 0x10, 0x17, 0x40, 0x00, 0x09, 0x05, 0x30, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00};

/// UL-DCCH SecurityModeComplete.
constexpr std::array<uint8_t, 2> rrc_smc_complete = {0x2a, 0x00};

/// K_gNB the harness installs, so that the security context can be finalized.
constexpr std::string_view k_gnb_hex = "45cbc3f8a81193fd5c5229300d59edf812e998a115ec4e0ce903ba89367e2628";

// ---------------------------------------------------------------------------
// Persistent harness state
// ---------------------------------------------------------------------------

/// State shared by every input: everything that carries no per-UE RRC state.
///
/// The RRC UE itself is rebuilt for every input so that a crash reproduces from its input file
/// alone, rather than depending on the inputs the fuzzer happened to run before it.
struct fuzz_state {
  timer_manager       timers;
  manual_task_worker  ctrl_worker{64};
  cu_cp_configuration cu_cp_cfg;
  ue_manager          ue_mng;

  dummy_rrc_f1ap_pdu_notifier rrc_ue_f1ap_notifier;
  dummy_rrc_ue_ngap_adapter   rrc_ue_ngap_notifier;
  dummy_rrc_ue_cu_cp_adapter  rrc_ue_cu_cp_notifier;
  dummy_rrc_ue_rrc_du_adapter rrc_ue_rrc_du_notifier;

  fuzz_state() : cu_cp_cfg(make_cfg()), ue_mng(make_ue_cfg(cu_cp_cfg), make_ue_deps(timers, ctrl_worker)) {}

private:
  static cu_cp_configuration make_cfg()
  {
    cu_cp_configuration cfg = config_helpers::make_default_cu_cp_config();
    // Keep the UE pool small: the harness holds at most one UE at a time.
    cfg.admission.max_nof_ues = 4;
    return cfg;
  }

  static ue_manager_config make_ue_cfg(const cu_cp_configuration& cfg)
  {
    return ue_manager_config{.gnb_id              = cfg.node.gnb_id,
                             .max_nof_ues         = cfg.admission.max_nof_ues,
                             .drb_config          = cfg.bearers.drb_config,
                             .max_nof_drbs_per_ue = cfg.admission.max_nof_drbs_per_ue,
                             .int_algo_pref_list  = cfg.security.int_algo_pref_list,
                             .enc_algo_pref_list  = cfg.security.enc_algo_pref_list,
                             .enable_rrc_metrics  = cfg.metrics.layers_cfg.enable_rrc_metrics,
                             .ue                  = cfg.ue};
  }

  static ue_manager_dependencies make_ue_deps(timer_manager& timers_, task_executor& exec)
  {
    return ue_manager_dependencies{
        .timers = timers_, .cu_cp_executor = exec, .logger = ocudulog::fetch_basic_logger("CU-CP")};
  }
};

/// One RRC UE, its CU-CP UE entry and its SRB PDCP context, torn down after each input.
class fuzz_ue
{
public:
  explicit fuzz_ue(fuzz_state& state_) : state(state_)
  {
    ue_index = state.ue_mng.add_ue(cu_cp_du_index_t::min);
    if (ue_index == cu_cp_ue_index_t::invalid) {
      return;
    }
    (void)state.ue_mng.set_plmn(ue_index, plmn_identity::test_value());

    rrc_cell_context cell;
    cell.bands.push_back(nr_band::n78);
    cell.plmn_identity_list.push_back(plmn_identity::test_value());
    cell.timers = rrc_timers_t{.t300 = std::chrono::milliseconds{400},
                               .t301 = std::chrono::milliseconds{2000},
                               .t310 = std::chrono::milliseconds{1000},
                               .t311 = std::chrono::milliseconds{1000},
                               .t319 = std::chrono::milliseconds{1000}};

    rrc_ue_cfg_t    rrc_ue_cfg;
    rrc_meas_timing meas_timing;
    meas_timing.freq_and_timing.emplace();
    meas_timing.freq_and_timing.value().carrier_freq            = 535930;
    meas_timing.freq_and_timing.value().ssb_subcarrier_spacing  = subcarrier_spacing::kHz15;
    meas_timing.freq_and_timing.value().ssb_meas_timing_cfg.dur = 5;
    meas_timing.freq_and_timing.value().ssb_meas_timing_cfg.periodicity_and_offset.periodicity =
        rrc_periodicity_and_offset::periodicity_t::sf10;
    meas_timing.freq_and_timing.value().ssb_meas_timing_cfg.periodicity_and_offset.offset = 0;
    rrc_ue_cfg.meas_timings.push_back(meas_timing);
    rrc_ue_cfg.rrc_procedure_guard_time_ms = state.cu_cp_cfg.rrc.rrc_procedure_guard_time_ms;

    byte_buffer du_to_cu_container;
    (void)du_to_cu_container.resize(1);

    pdcp_ctx = std::make_unique<srb_pdcp_ue_context>(
        ue_index, timer_factory{state.timers, state.ctrl_worker}, state.ctrl_worker, 1U);

    rrc_ue = std::make_unique<rrc_ue_impl>(state.rrc_ue_f1ap_notifier,
                                           state.rrc_ue_ngap_notifier,
                                           state.rrc_ue_cu_cp_notifier,
                                           state.rrc_ue_cu_cp_notifier,
                                           state.ue_mng.find_ue(ue_index)->get_rrc_ue_cu_cp_ue_notifier(),
                                           state.rrc_ue_rrc_du_notifier,
                                           *pdcp_ctx,
                                           ue_index,
                                           to_rnti(0x1234),
                                           cell,
                                           rrc_ue_cfg,
                                           du_to_cu_container,
                                           std::optional<rrc_ue_transfer_context>{});

    pdcp_ctx->connect_rrc_ue(rrc_ue->get_ul_pdu_handler(), [](ngap_cause_t) {});
  }

  ~fuzz_ue()
  {
    rrc_ue.reset();
    pdcp_ctx.reset();
    if (ue_index != cu_cp_ue_index_t::invalid) {
      state.ue_mng.remove_ue(ue_index);
    }
    // ue_manager::remove_ue() defers the destruction of the UE task scheduler onto a task loop of
    // its own. Drain until quiescent so that those schedulers are released, rather than piling up
    // across inputs until the fuzzer hits its memory limit.
    run_tasks();
  }

  bool is_valid() const { return rrc_ue != nullptr; }

  /// Drive the UE into \c target by replaying the canned setup exchange.
  void drive_to(ue_state target)
  {
    if (target == ue_state::fresh) {
      return;
    }

    inject_ccch(test_helpers::pack_ul_ccch_msg(test_helpers::create_rrc_setup_request()));
    if (target == ue_state::awaiting_setup_complete) {
      return;
    }

    inject_dcch(srb_id_t::srb1, rrc_setup_complete, false);
    if (target == ue_state::connected) {
      return;
    }

    if (!init_security_context()) {
      return;
    }
    // Asking for the Security Mode Command context activates SRB1 PDCP security and leaves the SMC
    // transaction pending, exactly as the CU-CP does it. Completing it moves the UE past security
    // activation.
    (void)rrc_ue->get_rrc_ue_control_message_handler().get_security_mode_command_context();
    inject_dcch(srb_id_t::srb1, rrc_smc_complete, true);

    srb_creation_message srb2_msg;
    srb2_msg.ue_index = ue_index;
    srb2_msg.srb_id   = srb_id_t::srb2;
    rrc_ue->get_rrc_ue_control_message_handler().create_srb(srb2_msg);
    run_tasks();
  }

  void inject_ccch(span<const uint8_t> pdu)
  {
    auto buf = byte_buffer::create(pdu);
    if (buf.has_value()) {
      inject_ccch(std::move(buf.value()));
    }
  }

  void inject_ccch(byte_buffer pdu)
  {
    rrc_ue->get_ul_pdu_handler().handle_ul_ccch_pdu(std::move(pdu), to_rnti(0x1234));
    run_tasks();
  }

  void inject_dcch(srb_id_t srb_id, span<const uint8_t> pdu, bool integrity_verified)
  {
    auto buf = byte_buffer::create(pdu);
    if (!buf.has_value()) {
      return;
    }
    rrc_ue->get_ul_pdu_handler().handle_ul_dcch_pdu(srb_id, std::move(buf.value()), integrity_verified);
    run_tasks();
  }

  /// Run queued tasks and advance the clock so that timers armed while handling a message fire.
  ///
  /// Kept short on purpose: each tick costs throughput, and the long RRC guard timers are out of
  /// reach at this granularity by design.
  void run_tasks()
  {
    state.ctrl_worker.run_pending_tasks();
    for (unsigned i = 0; i != nof_timer_ticks; ++i) {
      state.timers.tick();
      state.ctrl_worker.run_pending_tasks();
    }
  }

private:
  /// Install an AS security context on the UE, deriving the AS keys from a fixed K_gNB.
  bool init_security_context()
  {
    security::security_context sec_ctxt = {};
    sec_ctxt.k                          = test_helpers::make_sec_key(std::string{k_gnb_hex});
    std::fill(sec_ctxt.supported_int_algos.begin(), sec_ctxt.supported_int_algos.end(), true);
    std::fill(sec_ctxt.supported_enc_algos.begin(), sec_ctxt.supported_enc_algos.end(), true);

    ue_security_manager& sec_mng = state.ue_mng.find_ue(ue_index)->get_security_manager();
    return sec_mng.init_security_context(sec_ctxt) && sec_mng.finalize_security_context();
  }

  static constexpr unsigned nof_timer_ticks = 4;

  fuzz_state&                          state;
  cu_cp_ue_index_t                     ue_index = cu_cp_ue_index_t::invalid;
  std::unique_ptr<srb_pdcp_ue_context> pdcp_ctx;
  std::unique_ptr<rrc_ue_impl>         rrc_ue;
};

fuzz_state*    g_state = nullptr;
std::once_flag g_init_flag;

void ensure_state()
{
  std::call_once(g_init_flag, []() { g_state = new fuzz_state(); });
}

} // namespace

// ---------------------------------------------------------------------------
// LLVMFuzzer interface
// ---------------------------------------------------------------------------

/// Route logs to /dev/null at debug level before any forking occurs.
///
/// The level stays at debug so that the log formatting code runs: log_rrc_message() and the
/// to_json() call in rrc_ue_impl::store_ue_capabilities() walk the decoded, attacker-controlled
/// ASN.1 structures, which makes them part of the surface under test.
extern "C" int LLVMFuzzerInitialize(int* /*argc*/, char*** /*argv*/)
{
  if (ocudulog::sink* null_sink = ocudulog::create_file_sink("/dev/null"); null_sink != nullptr) {
    ocudulog::set_default_sink(*null_sink);
  }
  for (const char* name : {"RRC", "CU-CP", "PDCP", "SEC", "TEST", "ALL"}) {
    ocudulog::fetch_basic_logger(name).set_level(ocudulog::basic_levels::debug);
  }
  return 0;
}

/// Entry point called by AFL++ / libFuzzer for each mutated input.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  // The first byte selects the channel, SRB, integrity flag and UE state; the rest is the RRC PDU.
  if (size < 2) {
    return 0;
  }
  ensure_state();

  const input_header hdr = decode_header(data[0]);

  fuzz_ue ue{*g_state};
  if (!ue.is_valid()) {
    return 0;
  }
  ue.drive_to(hdr.state);

  const span<const uint8_t> payload(data + 1, size - 1);
  if (hdr.is_dcch) {
    ue.inject_dcch(hdr.srb_id, payload, hdr.integrity_verified);
  } else {
    ue.inject_ccch(payload);
  }

  return 0;
}
