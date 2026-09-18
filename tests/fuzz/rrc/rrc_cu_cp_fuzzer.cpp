// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// AFL++/libFuzzer harness injecting RRC messages through a complete CU-CP.
///
/// A real CU-CP is built with a stub AMF and a mock DU attached, and the fuzz input travels the full
/// production receive path:
///
///   fuzz input -> F1AP UL RRC Message Transfer -> CU-CP UE lookup -> PDCP -> RRC ASN.1 UPER decode
///               -> RRC UE state machine -> procedures -> DL message generation
///
/// The F1AP wrapper and the PDCP header are scaffolding built by the harness; only the RRC container
/// is mutated. Fuzzing the F1AP wrapper itself belongs in tests/fuzz/f1ap.
///
/// A UE is created and released for every input, so a crash reproduces from its input file alone.
///
/// The UE is driven no further than RRC connected, which is the window an unauthenticated attacker
/// reaches: everything up to and including RRCSetupComplete is processed before AS security is
/// activated. Past security activation PDCP verifies a MAC-I over the RRC PDU, and the mock DU sends
/// a zero MAC, so every mutated payload would be dropped on integrity failure and the UE released
/// before RRC ever sees it. Covering that window needs a PDCP TX entity on the mock DU side, keyed
/// with the AS keys the harness already knows. Until then, tests/fuzz/rrc/rrc_ue_fuzzer reaches the
/// post-security RRC handlers by injecting above PDCP.
///
/// See tests/fuzz/README.md for the input layout, build instructions and run commands.

#include "tests/fuzz/cu_cp/cu_cp_fuzz_env.h"
#include "tests/test_doubles/e1ap/e1ap_cu_cp_test_messages.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/mutexed_mpmc_queue.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "ocudu/asn1/ngap/common.h"
#include "ocudu/asn1/ngap/ngap.h"
#include "ocudu/asn1/ngap/ngap_ies.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/cu_cp/cu_cp.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"
#include "ocudu/cu_cp/cu_cp_factory.h"
#include "ocudu/e1ap/common/e1ap_message.h"
#include "ocudu/e1ap/cu_cp/cu_cp_e1_handler.h"
#include "ocudu/f1ap/cu_cp/cu_cp_f1c_handler.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ngap/gateways/n2_connection_client.h"
#include "ocudu/ngap/ngap_message.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/pcap/dlt_pcap.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/support/async/async_no_op_task.h"
#include "ocudu/support/executors/task_worker.h"
#include "ocudu/support/timers.h"
#include "ocudu/xnap/gateways/xnc_connection_gateway.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>

using namespace ocudu;
using namespace ocucp;
using namespace ocucp::fuzz;

namespace {

// ---------------------------------------------------------------------------
// Input encoding
// ---------------------------------------------------------------------------

/// UE state reached before the fuzzed payload is injected.
enum class ue_state : uint8_t {
  /// UE created by an Initial UL RRC Message Transfer, awaiting RRCSetupComplete.
  awaiting_setup_complete = 0,
  /// RRCSetupComplete handled. SRB1 is up, AS security is not active.
  connected = 1,
};

/// Decoded control byte prefixed to every input.
struct input_header {
  bool     is_dcch;
  ue_state state;
};

/// Decode the control byte. See tests/fuzz/README.md for the bit layout.
input_header decode_header(uint8_t byte)
{
  return input_header{.is_dcch = (byte & 0b0000'0001U) != 0,
                      .state   = static_cast<ue_state>((byte >> 1U) & 0b0000'0001U)};
}

// ---------------------------------------------------------------------------
// fuzz_du - replaces the SCTP F1-C gateway
// ---------------------------------------------------------------------------

/// Thread-safe DU stub attached to the CU-CP's F1-C handler.
///
/// Holds no per-UE state: the harness tracks the UE identifiers it allocates, so that nothing
/// accumulates across inputs.
class fuzz_du
{
  using pdu_queue = concurrent_queue<f1ap_message,
                                     concurrent_queue_policy::locking_mpmc,
                                     concurrent_queue_wait_policy::condition_variable>;

public:
  explicit fuzz_du(cu_cp_f1c_handler& cu_cp_f1c) : dl_pdus(1024)
  {
    tx_pdu_notifier = cu_cp_f1c.handle_new_du_connection(std::make_unique<rx_pdu_notifier>(*this));
  }

  bool connected() const { return tx_pdu_notifier != nullptr; }

  /// Deliver an F1AP message to the CU-CP (DU to CU-CP direction).
  void push_ul_pdu(const f1ap_message& msg)
  {
    if (tx_pdu_notifier) {
      tx_pdu_notifier->on_new_message(msg);
    }
  }

  /// Pop the next PDU sent by the CU-CP to the DU. Returns false if empty.
  bool try_pop_dl_pdu(f1ap_message& msg) { return dl_pdus.try_pop(msg); }

private:
  class rx_pdu_notifier : public f1ap_message_notifier
  {
  public:
    explicit rx_pdu_notifier(fuzz_du& parent_) : parent(parent_) {}

    void on_new_message(const f1ap_message& msg) override { parent.dl_pdus.push_blocking(msg); }

  private:
    fuzz_du& parent;
  };

  std::unique_ptr<f1ap_message_notifier> tx_pdu_notifier;
  pdu_queue                              dl_pdus;
};

// ---------------------------------------------------------------------------
// fuzz_cu_up - replaces the SCTP E1 gateway
// ---------------------------------------------------------------------------

/// CU-UP stub attached to the CU-CP's E1 handler.
///
/// The CU-CP rejects every RRC connection while no CU-UP is connected
/// (cu_cp_controller::request_ue_setup()), so one has to be attached for UEs to get past RRC Setup.
class fuzz_cu_up
{
  using pdu_queue = concurrent_queue<e1ap_message,
                                     concurrent_queue_policy::locking_mpmc,
                                     concurrent_queue_wait_policy::condition_variable>;

public:
  explicit fuzz_cu_up(cu_cp_e1_handler& cu_cp_e1) : rx_pdus(64)
  {
    tx_pdu_notifier = cu_cp_e1.handle_new_cu_up_connection(std::make_unique<rx_pdu_notifier>(*this));
  }

  bool connected() const { return tx_pdu_notifier != nullptr; }

  void push_ul_pdu(const e1ap_message& msg)
  {
    if (tx_pdu_notifier) {
      tx_pdu_notifier->on_new_message(msg);
    }
  }

  bool try_pop_rx_pdu(e1ap_message& msg) { return rx_pdus.try_pop(msg); }

private:
  class rx_pdu_notifier : public e1ap_message_notifier
  {
  public:
    explicit rx_pdu_notifier(fuzz_cu_up& parent_) : parent(parent_) {}

    void on_new_message(const e1ap_message& msg) override { parent.rx_pdus.push_blocking(msg); }

  private:
    fuzz_cu_up& parent;
  };

  std::unique_ptr<e1ap_message_notifier> tx_pdu_notifier;
  pdu_queue                              rx_pdus;
};

// ---------------------------------------------------------------------------
// Persistent harness state
// ---------------------------------------------------------------------------

/// Number of 1ms timer ticks executed after each injection.
///
/// Advancing the clock lets timers armed while handling the message fire. Kept small on purpose:
/// each tick costs throughput, and the long CU-CP guard timers are out of reach at this granularity
/// by design.
constexpr unsigned nof_timer_ticks = 4;

/// CU-CP, AMF stub and mock DU, shared by every input.
///
/// Building a CU-CP and running F1 Setup costs far more than handling a message, so this is done
/// once. The UE is what gets rebuilt per input.
struct fuzz_state {
  task_worker                    worker{"cu_cp_fuzz_wrkr", 1024};
  std::unique_ptr<task_executor> exec{std::make_unique<task_worker_executor>(worker)};

  timer_manager    timers{64};
  fuzz_amf         amf;
  fuzz_xnc_gateway xnc_gw;
  null_dlt_pcap    pcap;

  std::unique_ptr<cu_cp>      cu_cp_inst;
  std::unique_ptr<fuzz_du>    du;
  std::unique_ptr<fuzz_cu_up> cu_up;

  /// Monotonic DU UE F1AP ID, so that a released UE's identifier is never reused.
  uint64_t next_du_ue_id = 0;

  fuzz_state()
  {
    cu_cp_configuration cfg = config_helpers::make_default_cu_cp_config();

    // Keep the UE pool small: the harness holds at most one UE at a time, so a pool this size also
    // makes a UE that is not released show up immediately as a failure to create the next one.
    cfg.admission.max_nof_ues = 8;

    cfg.services.cu_cp_executor = exec.get();
    cfg.services.timers         = &timers;

    // Attach the stub AMF as the only NGAP peer.
    s_nssai_t               nssai{slice_service_type{1}, slice_differentiator{}};
    plmn_item               plmn{plmn_identity::test_value(), {nssai}};
    supported_tracking_area ta{7, {plmn}};
    cfg.ngap.n2_gws.push_back(&amf);
    cfg.ngap.ngaps.push_back(cu_cp_configuration::ngap_config{{ta}});

    cfg.xnap.xnc_gws.push_back(&xnc_gw);

    cfg.security.int_algo_pref_list = {security::integrity_algorithm::nia2,
                                       security::integrity_algorithm::nia1,
                                       security::integrity_algorithm::nia3,
                                       security::integrity_algorithm::nia0};
    cfg.security.enc_algo_pref_list = {security::ciphering_algorithm::nea0,
                                       security::ciphering_algorithm::nea2,
                                       security::ciphering_algorithm::nea1,
                                       security::ciphering_algorithm::nea3};
    cfg.bearers.drb_config          = config_helpers::make_default_cu_cp_qos_config_list();

    cu_cp_inst = create_cu_cp(cfg);

    // Pre-queue the NGSetupResponse so cu_cp->start() can complete the NG setup handshake.
    amf.enqueue_auto_response(generate_ng_setup_response());

    if (!cu_cp_inst->start()) {
      // Should not happen with the auto-response above; abort to make initialization failures
      // visible during corpus refinement.
      std::abort();
    }

    // A CU-UP has to be attached before any UE can be admitted.
    cu_up = std::make_unique<fuzz_cu_up>(cu_cp_inst->get_e1_handler());
    if (!cu_up->connected()) {
      std::abort();
    }
    cu_up->push_ul_pdu(generate_valid_cu_up_e1_setup_request());
    drain();

    du = std::make_unique<fuzz_du>(cu_cp_inst->get_f1c_handler());
    if (!du->connected()) {
      std::abort();
    }

    // Run F1 Setup so that the DU's cell is served and UEs can be created on it.
    du->push_ul_pdu(test_helpers::generate_f1_setup_request());
    drain();

    f1ap_message f1ap_dummy;
    while (du->try_pop_dl_pdu(f1ap_dummy)) {
    }
    ngap_message ngap_dummy;
    while (amf.try_pop_rx_pdu(ngap_dummy)) {
    }
    e1ap_message e1ap_dummy;
    while (cu_up->try_pop_rx_pdu(e1ap_dummy)) {
    }
  }

  /// Run queued CU-CP tasks and advance the clock so that armed timers fire.
  void drain()
  {
    worker.wait_pending_tasks();
    worker.wait_pending_tasks();
    for (unsigned i = 0; i != nof_timer_ticks; ++i) {
      worker.push_task_blocking([this]() { timers.tick(); });
      worker.wait_pending_tasks();
    }
  }
};

/// One UE on the mock DU, created and released around a single input.
class fuzz_ue
{
public:
  explicit fuzz_ue(fuzz_state& state_) : state(state_), du_ue_id(int_to_gnb_du_ue_f1ap_id(state_.next_du_ue_id++))
  {
    // Create the UE with an RRCSetupRequest, as the DU does for a UE arriving on SRB0.
    state.du->push_ul_pdu(test_helpers::generate_init_ul_rrc_message_transfer(
        du_ue_id,
        to_rnti(0x4601),
        plmn_identity::test_value(),
        byte_buffer{},
        test_helpers::pack_ul_ccch_msg(test_helpers::create_rrc_setup_request())));
    state.drain();

    // Pick up the CU-CP UE ID from the DL RRC Message Transfer carrying the RRC Setup.
    cu_ue_id = pop_cu_ue_id(du_ue_id);
  }

  ~fuzz_ue()
  {
    release(du_ue_id, cu_ue_id);
    drain_dl_pdus();
  }

  bool is_valid() const { return cu_ue_id.has_value(); }

  /// Drive the UE into \c target by replaying the canned setup exchange.
  void drive_to(ue_state target)
  {
    if (target == ue_state::awaiting_setup_complete) {
      return;
    }
    inject_dcch(test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_setup_complete()));
  }

  void inject_ccch(span<const uint8_t> pdu)
  {
    // A UL-CCCH message reaches the CU-CP as an Initial UL RRC Message Transfer, which allocates a
    // UE of its own and is routed to an existing one only by the resume identity the payload
    // carries.
    const gnb_du_ue_f1ap_id_t ccch_du_ue_id = int_to_gnb_du_ue_f1ap_id(state.next_du_ue_id++);
    state.du->push_ul_pdu(test_helpers::generate_init_ul_rrc_message_transfer(
        ccch_du_ue_id, to_rnti(0x4602), plmn_identity::test_value(), byte_buffer{}, make_buffer(pdu)));
    state.drain();
    release(ccch_du_ue_id, pop_cu_ue_id(ccch_du_ue_id));
    drain_dl_pdus();
  }

  void inject_dcch(byte_buffer pdu)
  {
    if (!cu_ue_id.has_value()) {
      return;
    }
    // Wrap the RRC message in the PDCP header the CU-CP's SRB1 entity expects. Before security
    // activation no MAC-I is appended, so the payload reaches RRC unmodified.
    state.du->push_ul_pdu(test_helpers::generate_ul_rrc_message_transfer(
        du_ue_id, cu_ue_id.value(), srb_id_t::srb1, next_pdcp_sn++, std::move(pdu)));
    state.drain();
    drain_dl_pdus();
  }

  static byte_buffer make_buffer(span<const uint8_t> bytes)
  {
    auto buf = byte_buffer::create(bytes);
    return buf.has_value() ? std::move(buf.value()) : byte_buffer{};
  }

private:
  /// Release a UE from the DU side, so that UE contexts do not accumulate across inputs.
  void release(gnb_du_ue_f1ap_id_t released_du_ue_id, std::optional<gnb_cu_ue_f1ap_id_t> released_cu_ue_id)
  {
    if (!released_cu_ue_id.has_value()) {
      return;
    }
    state.du->push_ul_pdu(
        test_helpers::generate_ue_context_release_request(released_cu_ue_id.value(), released_du_ue_id));
    state.drain();

    // A UE the AMF knows about is released over NGAP first. Answer from here rather than from the
    // AMF stub's notifier, which runs on the CU-CP worker thread inside the CU-CP's own Tx path.
    ngap_message ngap_pdu;
    while (state.amf.try_pop_rx_pdu(ngap_pdu)) {
      if (std::optional<ngap_message> release_cmd = make_release_command(ngap_pdu); release_cmd.has_value()) {
        state.amf.push_tx_pdu(release_cmd.value());
        state.drain();
      }
    }

    answer_release_command();
  }

  /// Drain DL PDUs until the one carrying the CU-CP UE ID for the given UE is found.
  std::optional<gnb_cu_ue_f1ap_id_t> pop_cu_ue_id(gnb_du_ue_f1ap_id_t wanted_du_ue_id)
  {
    std::optional<gnb_cu_ue_f1ap_id_t> found;
    f1ap_message                       pdu;
    while (state.du->try_pop_dl_pdu(pdu)) {
      if (pdu.pdu.type().value != asn1::f1ap::f1ap_pdu_c::types_opts::init_msg or
          pdu.pdu.init_msg().value.type().value !=
              asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::dl_rrc_msg_transfer) {
        continue;
      }
      const auto& dl_rrc = pdu.pdu.init_msg().value.dl_rrc_msg_transfer();
      if (int_to_gnb_du_ue_f1ap_id(dl_rrc->gnb_du_ue_f1ap_id) == wanted_du_ue_id) {
        found = int_to_gnb_cu_ue_f1ap_id(dl_rrc->gnb_cu_ue_f1ap_id);
      }
    }
    return found;
  }

  /// Answer the F1AP UE Context Release Command, completing the release at the DU.
  void answer_release_command()
  {
    f1ap_message pdu;
    while (state.du->try_pop_dl_pdu(pdu)) {
      if (pdu.pdu.type().value == asn1::f1ap::f1ap_pdu_c::types_opts::init_msg and
          pdu.pdu.init_msg().value.type().value ==
              asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd) {
        state.du->push_ul_pdu(test_helpers::generate_ue_context_release_complete(pdu));
        state.drain();
      }
    }
  }

  void drain_dl_pdus()
  {
    f1ap_message f1ap_pdu;
    while (state.du->try_pop_dl_pdu(f1ap_pdu)) {
    }
    e1ap_message e1ap_pdu;
    while (state.cu_up->try_pop_rx_pdu(e1ap_pdu)) {
    }
    ngap_message ngap_pdu;
    while (state.amf.try_pop_rx_pdu(ngap_pdu)) {
    }
  }

  fuzz_state&                        state;
  gnb_du_ue_f1ap_id_t                du_ue_id;
  std::optional<gnb_cu_ue_f1ap_id_t> cu_ue_id;
  uint32_t                           next_pdcp_sn = 0;
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

  fuzz_ue ue{*g_state};
  if (!ue.is_valid()) {
    return 0;
  }
  ue.drive_to(hdr.state);

  const span<const uint8_t> payload(data + 1, size - 1);
  if (hdr.is_dcch) {
    ue.inject_dcch(fuzz_ue::make_buffer(payload));
  } else {
    ue.inject_ccch(payload);
  }

  return 0;
}
