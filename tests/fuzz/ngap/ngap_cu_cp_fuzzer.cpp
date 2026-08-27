// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// AFL++/libFuzzer harness exercising the full CU-CP NGAP receive path.
///
/// Unlike the ASN.1-only harness (ngap_pdu_decoder_fuzzer), this harness spins
/// up a real CU-CP instance and injects decoded NGAP messages directly into the
/// NGAP message dispatcher.  This exercises the complete receive stack:
///
///   fuzz input -> ASN.1 PER decode -> NGAP validators -> procedure dispatcher
///               -> procedure state-machine -> response generation
///
/// See tests/fuzz/README.md for build instructions, run commands, and the
/// test double architecture (fuzz_amf, fuzz_xnc_gateway).

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/mutexed_mpmc_queue.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/ngap/common.h"
#include "ocudu/asn1/ngap/ngap.h"
#include "ocudu/asn1/ngap/ngap_ies.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/cu_cp/cu_cp.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"
#include "ocudu/cu_cp/cu_cp_factory.h"
#include "ocudu/ngap/gateways/n2_connection_client.h"
#include "ocudu/ngap/ngap.h"
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

using namespace ocudu;
using namespace ocucp;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

/// Build an NGSetupResponse that matches the test PLMN used in cu_cp_configuration_helpers.
static ngap_message make_ng_setup_response()
{
  ngap_message msg{};
  msg.pdu.set_successful_outcome();
  msg.pdu.successful_outcome().load_info_obj(ASN1_NGAP_ID_NG_SETUP);

  auto& res = msg.pdu.successful_outcome().value.ng_setup_resp();
  res->amf_name.from_string("fuzz-amf0");

  asn1::ngap::served_guami_item_s guami_item;
  // test PLMN: 00 f1 10 (MCC=001, MNC=01)
  guami_item.guami.plmn_id = {0x00u, 0xf1u, 0x10u};
  guami_item.guami.amf_region_id.from_number(2);
  guami_item.guami.amf_set_id.from_number(1);
  guami_item.guami.amf_pointer.from_number(0);
  res->served_guami_list.push_back(guami_item);
  res->relative_amf_capacity = 255;

  asn1::ngap::plmn_support_item_s plmn_item{};
  plmn_item.plmn_id = {0x00u, 0xf1u, 0x10u};
  asn1::ngap::slice_support_item_s slice_item{};
  slice_item.s_nssai.sst.from_number(1);
  plmn_item.slice_support_list.push_back(slice_item);
  res->plmn_support_list.push_back(plmn_item);

  return msg;
}

// ---------------------------------------------------------------------------
// fuzz_amf – replaces the SCTP N2 gateway
// ---------------------------------------------------------------------------

/// Thread-safe N2 connection client used instead of a real SCTP gateway.
///
/// Calling push_tx_pdu() delivers a decoded ngap_message to the CU-CP as if
/// it arrived from the AMF over N2.  Responses sent by the CU-CP are queued
/// and can be drained with try_pop_rx_pdu().
class fuzz_amf : public n2_connection_client
{
  using pdu_queue = concurrent_queue<ngap_message,
                                     concurrent_queue_policy::locking_mpmc,
                                     concurrent_queue_wait_policy::condition_variable>;

public:
  fuzz_amf() : rx_pdus(512), pending_auto_tx(16) {}

  /// Called by the CU-CP at startup to obtain the AMF-side Tx/Rx notifiers.
  std::unique_ptr<ngap_message_notifier>
  handle_cu_cp_connection_request(std::unique_ptr<ngap_rx_message_notifier> notifier) override
  {
    rx_pdu_notifier = std::move(notifier);
    return std::make_unique<tx_notifier>(*this);
  }

  /// Deliver a decoded NGAP message to the CU-CP (AMF → CU-CP direction).
  void push_tx_pdu(const ngap_message& msg)
  {
    if (rx_pdu_notifier) {
      rx_pdu_notifier->on_new_message(msg);
    }
  }

  /// Pre-queue a response that the AMF will send automatically in reply to the
  /// next CU-CP message (used to complete the NG Setup exchange on startup).
  void enqueue_auto_response(const ngap_message& msg) { pending_auto_tx.push_blocking(msg); }

  /// Pop the next PDU sent by the CU-CP to the AMF.  Returns false if empty.
  bool try_pop_rx_pdu(ngap_message& pdu) { return rx_pdus.try_pop(pdu); }

private:
  /// Notifier returned to the CU-CP; called when the CU-CP sends a PDU to AMF.
  class tx_notifier : public ngap_message_notifier
  {
  public:
    explicit tx_notifier(fuzz_amf& parent_) : parent(parent_) {}
    ~tx_notifier() override { parent.rx_pdu_notifier.reset(); }

    [[nodiscard]] bool on_new_message(const ngap_message& msg) override
    {
      // If an auto-response is pending, send it back to the CU-CP.
      ngap_message auto_resp;
      if (parent.pending_auto_tx.try_pop(auto_resp)) {
        parent.push_tx_pdu(auto_resp);
      }
      parent.rx_pdus.push_blocking(msg);
      return true;
    }

  private:
    fuzz_amf& parent;
  };

  std::unique_ptr<ngap_rx_message_notifier> rx_pdu_notifier;
  pdu_queue                                 rx_pdus;
  pdu_queue                                 pending_auto_tx;
};

// ---------------------------------------------------------------------------
// fuzz_xnc_gateway – no-op XN-C connection gateway
// ---------------------------------------------------------------------------

class fuzz_xnc_gateway : public xnc_connection_gateway
{
public:
  async_task<bool> connect_to_peer(std::vector<transport_layer_address> /*peer_addrs*/) override
  {
    return launch_no_op_task(true);
  }
  void                    attach_cu_cp(cu_cp_xnc_handler& /*handler*/) override {}
  void                    stop() override {}
  std::optional<uint16_t> get_listen_port() const override { return std::nullopt; }
};

// ---------------------------------------------------------------------------
// Full CU-CP fuzz state
// ---------------------------------------------------------------------------

struct fuzz_state {
  /// Background thread that executes CU-CP tasks.
  task_worker                    worker{"ngap_fuzz_workr", 1024};
  std::unique_ptr<task_executor> exec{std::make_unique<task_worker_executor>(worker)};

  timer_manager    timers{64};
  fuzz_amf         amf;
  fuzz_xnc_gateway xnc_gw;
  null_dlt_pcap    pcap;

  std::unique_ptr<cu_cp> cu_cp_inst;

  fuzz_state()
  {
    // Build CU-CP configuration (same pattern as cu_cp_test_environment).
    cu_cp_configuration cfg = config_helpers::make_default_cu_cp_config();

    cfg.services.cu_cp_executor = exec.get();
    cfg.services.timers         = &timers;

    // Attach our stub AMF as the only NGAP peer.
    s_nssai_t               nssai{slice_service_type{1}, slice_differentiator{}};
    plmn_item               plmn{plmn_identity::test_value(), {nssai}};
    supported_tracking_area ta{7, {plmn}};
    cfg.ngap.n2_gws.push_back(&amf);
    cfg.ngap.ngaps.push_back(cu_cp_configuration::ngap_config{{ta}});

    // Attach our no-op XN-C gateway.
    cfg.xnap.xnc_gws.push_back(&xnc_gw);

    // Security preferences (NIA2/NEA0 as default).
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

    // Pre-queue the NGSetupResponse so cu_cp->start() can complete the NG
    // setup handshake without a real AMF.
    amf.enqueue_auto_response(make_ng_setup_response());

    // Start the CU-CP.  Blocks until the NG Setup procedure completes.
    if (!cu_cp_inst->start()) {
      // Should not happen with the auto-response above; abort to make
      // initialization failures visible during corpus refinement.
      std::abort();
    }

    // Drain the NGSetupRequest that the CU-CP sent.
    ngap_message dummy;
    while (amf.try_pop_rx_pdu(dummy)) {
    }
    worker.wait_pending_tasks();
  }
};

// ---------------------------------------------------------------------------
// Global state – lazy, created inside the (post-fork) child process so that
// the task_worker thread is never inherited across a fork().
//
// The state is deliberately leaked: the ocudulog singleton registers its exit handler before the
// state exists, so it is torn down first and stopping the CU-CP at exit would log through a freed
// logger. The OS reclaims everything when the process ends.
// ---------------------------------------------------------------------------

static fuzz_state*    g_state = nullptr;
static std::once_flag g_init_flag;

/// Number of 1ms timer ticks executed after each input.
///
/// Advancing the clock lets timers armed while handling the input fire. Kept small on purpose: each
/// tick costs throughput, and the long CU-CP guard timers are out of reach at this granularity by
/// design.
constexpr unsigned nof_timer_ticks_per_input = 8;

static void ensure_state()
{
  std::call_once(g_init_flag, []() { g_state = new fuzz_state(); });
}

} // namespace

// ---------------------------------------------------------------------------
// LLVMFuzzer interface
// ---------------------------------------------------------------------------

/// Suppress log noise for all CU-CP subsystems before any forking occurs.
extern "C" int LLVMFuzzerInitialize(int* /*argc*/, char*** /*argv*/)
{
  for (const char* name : {"NGAP", "CU-CP", "RRC", "PDCP", "SEC", "F1AP", "E1AP", "NRPPA", "XNAP", "ALL"}) {
    ocudulog::fetch_basic_logger(name).set_level(ocudulog::basic_levels::debug);
  }
  // NOTE: do NOT call ensure_state() here.  The state (and its task_worker
  // thread) must be created after AFL++ forks so each child has its own thread.
  return 0;
}

/// Entry point called by AFL++ / libFuzzer for each mutated input.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  // Lazy initialisation: safe after AFL++ fork, called once per process with
  // libFuzzer and once per child-process lifecycle with AFL++.
  ensure_state();

  // ------------------------------------------------------------------
  // Step 1 – decode the fuzz input into an ngap_message.
  //
  // Inputs that fail to decode are dropped, mirroring ngap_asn1_packer::handle_packed_pdu(): the
  // CU-CP never sees them in production, so injecting them would only exercise unreachable states.
  // The ASN.1 layer itself is covered by ngap_pdu_decoder_fuzzer.
  // ------------------------------------------------------------------
  byte_buffer    buf{byte_buffer::fallback_allocation_tag{}, span<const uint8_t>(data, size)};
  asn1::cbit_ref bref{buf};
  ngap_message   msg{};
  if (msg.pdu.unpack(bref) != asn1::OCUDUASN_SUCCESS) {
    return 0;
  }

  // ------------------------------------------------------------------
  // Step 2 – inject the message into the CU-CP via the AMF stub.
  // ------------------------------------------------------------------
  g_state->amf.push_tx_pdu(msg);

  // ------------------------------------------------------------------
  // Step 3 – drain the CU-CP task queue so all async procedures run.
  //
  // Coroutine continuations may queue further tasks; two rounds of
  // wait_pending_tasks() covers procedures that span a single await point.
  // ------------------------------------------------------------------
  g_state->worker.wait_pending_tasks();
  g_state->worker.wait_pending_tasks();

  // ------------------------------------------------------------------
  // Step 4 – advance the timer to exercise the expiry paths of the timers armed while handling this
  // input.
  // ------------------------------------------------------------------
  for (unsigned i = 0; i != nof_timer_ticks_per_input; ++i) {
    g_state->worker.push_task_blocking([&]() { g_state->timers.tick(); });
    g_state->worker.wait_pending_tasks();
  }

  // ------------------------------------------------------------------
  // Step 5 – drain CU-CP responses to prevent queue overflow across
  // iterations.
  // ------------------------------------------------------------------
  ngap_message response;
  while (g_state->amf.try_pop_rx_pdu(response)) {
  }

  return 0;
}
