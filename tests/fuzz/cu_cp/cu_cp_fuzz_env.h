// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// Shared CU-CP fuzzing environment.
///
/// Builds a real CU-CP with stub AMF, CU-UP and DU peers attached, and brings UEs up on it, so that
/// a harness only has to describe what it injects and where. The stubs hold no per-UE state: the
/// identifiers and PDCP state belong to the UE object, which is created and released per input so
/// that a crash reproduces from its input file alone.
///
/// See tests/fuzz/README.md for build instructions and run commands.

#pragma once

#include "tests/test_doubles/e1ap/e1ap_cu_cp_test_messages.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include "tests/test_doubles/security/security_test_keys.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/mutexed_mpmc_queue.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "ocudu/asn1/ngap/common.h"
#include "ocudu/asn1/ngap/ngap.h"
#include "ocudu/asn1/ngap/ngap_ies.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg.h"
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
#include "ocudu/pdcp/pdcp_config.h"
#include "ocudu/pdcp/pdcp_factory.h"
#include "ocudu/pdcp/pdcp_tx.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/security/security.h"
#include "ocudu/support/async/async_no_op_task.h"
#include "ocudu/support/executors/inline_task_executor.h"
#include "ocudu/support/executors/task_worker.h"
#include "ocudu/support/timers.h"
#include "ocudu/xnap/gateways/xnc_connection_gateway.h"
#include <cstdint>
#include <memory>
#include <string_view>

namespace ocudu::ocucp::fuzz {

/// UE state reached before a harness injects its payload.
enum class ue_state : uint8_t {
  /// UE created by an Initial UL RRC Message Transfer, awaiting RRCSetupComplete.
  awaiting_setup_complete = 0,
  /// RRCSetupComplete handled. SRB1 is up, AS security is not active.
  connected = 1,
  /// AS security activated on SRB1, UE capabilities transferred and SRB2 created.
  secured = 2,
};

// ---------------------------------------------------------------------------
// fuzz_amf - replaces the SCTP N2 gateway
// ---------------------------------------------------------------------------

/// Thread-safe N2 connection client used instead of a real SCTP gateway.
class fuzz_amf : public n2_connection_client
{
  using pdu_queue = concurrent_queue<ngap_message,
                                     concurrent_queue_policy::locking_mpmc,
                                     concurrent_queue_wait_policy::condition_variable>;

public:
  fuzz_amf() : rx_pdus(512), pending_auto_tx(16) {}

  std::unique_ptr<ngap_message_notifier>
  handle_cu_cp_connection_request(std::unique_ptr<ngap_rx_message_notifier> notifier) override
  {
    rx_pdu_notifier = std::move(notifier);
    return std::make_unique<tx_notifier>(*this);
  }

  /// Deliver a decoded NGAP message to the CU-CP (AMF to CU-CP direction).
  void push_tx_pdu(const ngap_message& msg)
  {
    if (rx_pdu_notifier) {
      rx_pdu_notifier->on_new_message(msg);
    }
  }

  /// Pre-queue a response the AMF sends in reply to the next CU-CP message.
  void enqueue_auto_response(const ngap_message& msg) { pending_auto_tx.push_blocking(msg); }

  /// Pop the next PDU sent by the CU-CP to the AMF. Returns false if empty.
  bool try_pop_rx_pdu(ngap_message& pdu) { return rx_pdus.try_pop(pdu); }

private:
  class tx_notifier : public ngap_message_notifier
  {
  public:
    explicit tx_notifier(fuzz_amf& parent_) : parent(parent_) {}
    ~tx_notifier() override { parent.rx_pdu_notifier.reset(); }

    [[nodiscard]] bool on_new_message(const ngap_message& msg) override
    {
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

/// Build the UE Context Release Command answering a UE Context Release Request, as the AMF does in
/// TS 38.413 section 8.3.2. Returns nothing for any other message.
inline std::optional<ngap_message> make_release_command(const ngap_message& msg)
{
  if (msg.pdu.type().value != asn1::ngap::ngap_pdu_c::types_opts::init_msg or
      msg.pdu.init_msg().value.type().value !=
          asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ue_context_release_request) {
    return std::nullopt;
  }
  const auto& req = msg.pdu.init_msg().value.ue_context_release_request();

  ngap_message cmd = {};
  cmd.pdu.set_init_msg();
  cmd.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_UE_CONTEXT_RELEASE);
  auto& release_cmd                      = cmd.pdu.init_msg().value.ue_context_release_cmd();
  auto& ue_id_pair                       = release_cmd->ue_ngap_ids.set_ue_ngap_id_pair();
  ue_id_pair.amf_ue_ngap_id              = req->amf_ue_ngap_id;
  ue_id_pair.ran_ue_ngap_id              = req->ran_ue_ngap_id;
  release_cmd->cause.set_radio_network() = asn1::ngap::cause_radio_network_opts::options::radio_conn_with_ue_lost;
  return cmd;
}

// ---------------------------------------------------------------------------
// fuzz_xnc_gateway - no-op Xn-C connection gateway
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
// du_srb_pdcp_tx - DU-side PDCP TX entity for one SRB
// ---------------------------------------------------------------------------

/// K_gNB carried by the Initial Context Setup Request the harness injects.
constexpr std::string_view k_gnb_hex = "fe0d9f168ecba3379d8d9a1830b8230a07f827ac3c35c8a2a8b2c80e1118dd66";

/// Derive the RRC AS security configuration the CU-CP arrives at for this UE.
///
/// The CU-CP runs the same derivation over the K_gNB and the UE security capabilities of the
/// Initial Context Setup Request, using the algorithm preferences configured below, so both sides
/// end up with the same keys and algorithms. A mismatch is not silent: the CU-CP would reject the
/// SecurityModeComplete on integrity failure and the UE bring-up would fail.
inline security::sec_128_as_config make_rrc_128_as_config()
{
  security::security_context sec_ctxt = {};
  sec_ctxt.k                          = test_helpers::make_sec_key(std::string{k_gnb_hex});
  std::fill(sec_ctxt.supported_int_algos.begin(), sec_ctxt.supported_int_algos.end(), true);
  std::fill(sec_ctxt.supported_enc_algos.begin(), sec_ctxt.supported_enc_algos.end(), true);

  sec_ctxt.select_algorithms({security::integrity_algorithm::nia2,
                              security::integrity_algorithm::nia1,
                              security::integrity_algorithm::nia3,
                              security::integrity_algorithm::nia0},
                             {security::ciphering_algorithm::nea0,
                              security::ciphering_algorithm::nea2,
                              security::ciphering_algorithm::nea1,
                              security::ciphering_algorithm::nea3});
  sec_ctxt.generate_as_keys();

  return sec_ctxt.get_128_as_config(security::sec_domain::rrc);
}

/// PDCP TX entity that packs RRC messages the way the UE does, so that the CU-CP's SRB RX entity
/// accepts them once security is active.
///
/// Without this the RRC container would carry a zero MAC-I, which the CU-CP drops on integrity
/// failure before RRC sees it, and the UE would be released.
class du_srb_pdcp_tx : public pdcp_tx_lower_notifier, public pdcp_tx_upper_control_notifier
{
public:
  du_srb_pdcp_tx(srb_id_t srb_id, uint32_t ue_index, timer_factory timers, task_executor& ctrl_exec)
  {
    pdcp_entity_creation_message msg{};
    msg.ue_index = ue_index;
    msg.rb_id    = srb_id;
    msg.config   = pdcp_make_default_srb_config();
    // The CU-CP's SRB entity receives in the uplink direction, so this one transmits in it.
    msg.config.tx.direction    = pdcp_security_direction::uplink;
    msg.tx_lower               = this;
    msg.tx_upper_cn            = this;
    msg.rx_upper_dn            = nullptr;
    msg.rx_upper_cn            = nullptr;
    msg.ue_dl_timer_factory    = timers;
    msg.ue_ul_timer_factory    = timers;
    msg.ue_ctrl_timer_factory  = timers;
    msg.ue_dl_executor         = &inline_executor;
    msg.ue_ul_executor         = &inline_executor;
    msg.ue_ctrl_executor       = &ctrl_exec;
    msg.crypto_executor        = &inline_executor;
    msg.max_nof_crypto_workers = 1;

    entity = create_pdcp_entity(msg);
  }

  /// Apply the AS security configuration, mirroring what the UE does on SecurityModeCommand.
  ///
  /// SRBs are integrity protected but never ciphered by the CU-CP's SRB entities, so ciphering
  /// stays off on this side too.
  void enable_security(const security::sec_128_as_config& sec_cfg)
  {
    entity->get_tx_upper_control_interface().configure_security(
        sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::off);
  }

  /// Pack an RRC message into a PDCP PDU. Returns an empty buffer if the entity produced nothing.
  byte_buffer pack(byte_buffer rrc_pdu)
  {
    packed_pdu.clear();
    entity->get_tx_upper_data_interface().handle_sdu(std::move(rrc_pdu));
    return std::move(packed_pdu);
  }

  // pdcp_tx_lower_notifier
  void on_new_pdu(byte_buffer pdu, bool /*is_retx*/) override { packed_pdu = std::move(pdu); }
  void on_discard_pdu(uint32_t /*pdcp_sn*/) override {}

  // pdcp_tx_upper_control_notifier
  void on_protocol_failure() override {}
  void on_max_count_reached() override {}
  void on_resume_required() override {}

private:
  inline_task_executor         inline_executor;
  std::unique_ptr<pdcp_entity> entity;
  byte_buffer                  packed_pdu;
};

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

  /// RRC AS security configuration matching the one the CU-CP derives for every UE.
  security::sec_128_as_config rrc_sec_cfg = make_rrc_128_as_config();

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

    srb1_pdcp = std::make_unique<du_srb_pdcp_tx>(
        srb_id_t::srb1, to_underlying(du_ue_id), timer_factory{state.timers, *state.exec}, *state.exec);
  }

  ~fuzz_ue()
  {
    release(du_ue_id, cu_ue_id);
    drain_dl_pdus();
  }

  bool is_valid() const { return cu_ue_id.has_value(); }

  /// Whether AS security was activated, which is what lets a mutated payload reach RRC on a secured UE.
  bool is_secured() const { return srb2_pdcp != nullptr; }

  /// Identifiers naming this UE over NGAP, once it has sent its Initial UE Message.
  std::optional<ran_ue_id_t> get_ran_ue_id() const { return ran_ue_id; }
  static amf_ue_id_t         get_amf_ue_id() { return amf_ue_id; }

  /// Drive the UE into \c target by replaying the canned setup exchange.
  void drive_to(ue_state target)
  {
    if (target == ue_state::awaiting_setup_complete) {
      return;
    }
    inject_dcch(srb_id_t::srb1, test_helpers::pack_ul_dcch_msg(test_helpers::create_rrc_setup_complete()));
    if (target == ue_state::connected) {
      return;
    }
    activate_security();
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

  void inject_dcch(srb_id_t srb_id, byte_buffer pdu)
  {
    if (!cu_ue_id.has_value()) {
      return;
    }
    du_srb_pdcp_tx* pdcp = srb_id == srb_id_t::srb2 ? srb2_pdcp.get() : srb1_pdcp.get();
    if (pdcp == nullptr) {
      // SRB2 only exists once security is active; an input selecting it earlier has nothing to send on.
      return;
    }

    // The CU-CP's SRB entity expects a PDCP PDU, so the RRC message is packed the way the UE packs
    // it. Once security is active this is what produces the MAC-I the CU-CP verifies.
    byte_buffer pdcp_pdu = pdcp->pack(std::move(pdu));
    if (pdcp_pdu.empty()) {
      return;
    }
    state.du->push_ul_pdu(
        test_helpers::generate_ul_rrc_message_transfer(du_ue_id, cu_ue_id.value(), srb_id, std::move(pdcp_pdu)));
    state.drain();
    // Only the NGAP queue is drained here: the bring-up reads the CU-CP's F1AP answers out of the DU
    // queue, so that one is left alone until the input has been handled.
    drain_ngap_pdus();
  }

  static byte_buffer make_buffer(span<const uint8_t> bytes)
  {
    auto buf = byte_buffer::create(bytes);
    return buf.has_value() ? std::move(buf.value()) : byte_buffer{};
  }

private:
  /// Run the Initial Context Setup exchange, which activates AS security and creates SRB2.
  ///
  /// This mirrors cu_cp_test_environment::setup_ue_security_and_ue_capabilities(), except that the
  /// SecurityModeComplete and the UE capabilities are packed by the DU-side PDCP entity rather than
  /// replayed from a captured PDU.
  void activate_security()
  {
    if (!ran_ue_id.has_value()) {
      return;
    }

    // The AMF answers the Initial UE Message with an Initial Context Setup Request, which carries the
    // K_gNB the DU-side PDCP entities are keyed with.
    state.amf.push_tx_pdu(generate_valid_initial_context_setup_request_message(amf_ue_id, ran_ue_id.value()));
    state.drain();

    // The CU-CP answers with a UE Context Setup Request carrying the SecurityModeCommand.
    std::optional<f1ap_message> ue_ctxt_setup =
        pop_dl_pdu(asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_setup_request);
    if (!ue_ctxt_setup.has_value()) {
      return;
    }
    std::optional<uint8_t> smc_transaction_id = get_transaction_id(
        ue_ctxt_setup.value(), asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::security_mode_cmd);
    if (!smc_transaction_id.has_value()) {
      return;
    }
    state.du->push_ul_pdu(test_helpers::generate_ue_context_setup_response(cu_ue_id.value(), du_ue_id));
    state.drain();

    // From here the CU-CP verifies the MAC-I of everything arriving on SRB1.
    srb1_pdcp->enable_security(state.rrc_sec_cfg);
    inject_dcch(
        srb_id_t::srb1,
        test_helpers::pack_ul_dcch_msg(test_helpers::create_security_mode_complete(smc_transaction_id.value())));

    // A UE Capability Enquiry proves the SecurityModeComplete passed the integrity check: had it
    // failed, the CU-CP would have released the UE instead of asking for capabilities.
    std::optional<f1ap_message> cap_enquiry =
        pop_dl_pdu(asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::dl_rrc_msg_transfer);
    if (!cap_enquiry.has_value()) {
      return;
    }
    std::optional<uint8_t> cap_transaction_id =
        get_transaction_id(cap_enquiry.value(), asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::ue_cap_enquiry);
    if (!cap_transaction_id.has_value()) {
      return;
    }
    inject_dcch(srb_id_t::srb1,
                test_helpers::pack_ul_dcch_msg(test_helpers::create_ue_capability_info(cap_transaction_id.value())));

    // SRB2 is set up by the CU-CP once the UE is registered, and is keyed from the start.
    srb2_pdcp = std::make_unique<du_srb_pdcp_tx>(
        srb_id_t::srb2, to_underlying(du_ue_id), timer_factory{state.timers, *state.exec}, *state.exec);
    srb2_pdcp->enable_security(state.rrc_sec_cfg);
  }

  /// Read the RRC transaction ID out of the DL-DCCH message an F1AP PDU carries, when it is of the
  /// expected type. The CU-CP drops a response that answers a transaction it did not open.
  static std::optional<uint8_t> get_transaction_id(const f1ap_message&                                          pdu,
                                                   asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::options wanted)
  {
    const byte_buffer& rrc_container = test_helpers::get_rrc_container(pdu);
    if (rrc_container.empty()) {
      return std::nullopt;
    }
    const byte_buffer dl_dcch_pdu = test_helpers::extract_dl_dcch_msg(rrc_container);

    asn1::cbit_ref              bref{dl_dcch_pdu};
    asn1::rrc_nr::dl_dcch_msg_s dl_dcch_msg;
    if (dl_dcch_msg.unpack(bref) != asn1::OCUDUASN_SUCCESS or
        dl_dcch_msg.msg.type().value != asn1::rrc_nr::dl_dcch_msg_type_c::types_opts::c1 or
        dl_dcch_msg.msg.c1().type().value != wanted) {
      return std::nullopt;
    }

    switch (wanted) {
      case asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::security_mode_cmd:
        return dl_dcch_msg.msg.c1().security_mode_cmd().rrc_transaction_id;
      case asn1::rrc_nr::dl_dcch_msg_type_c::c1_c_::types_opts::ue_cap_enquiry:
        return dl_dcch_msg.msg.c1().ue_cap_enquiry().rrc_transaction_id;
      default:
        return std::nullopt;
    }
  }

  /// Drain DL PDUs and return the last one of the given initiating-message type, if any.
  std::optional<f1ap_message> pop_dl_pdu(asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::options wanted)
  {
    std::optional<f1ap_message> found;
    f1ap_message                pdu;
    while (state.du->try_pop_dl_pdu(pdu)) {
      if (pdu.pdu.type().value == asn1::f1ap::f1ap_pdu_c::types_opts::init_msg and
          pdu.pdu.init_msg().value.type().value == wanted) {
        found = pdu;
      }
    }
    return found;
  }

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
    drain_ngap_pdus();
  }

  /// Drain the NGAP queue, picking up the UE's RAN-UE-NGAP-ID on the way.
  void drain_ngap_pdus()
  {
    ngap_message ngap_pdu;
    while (state.amf.try_pop_rx_pdu(ngap_pdu)) {
      // The Initial UE Message names the UE over NGAP, which the Initial Context Setup Request has to
      // address to reach this UE.
      if (ngap_pdu.pdu.type().value == asn1::ngap::ngap_pdu_c::types_opts::init_msg and
          ngap_pdu.pdu.init_msg().value.type().value ==
              asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::init_ue_msg) {
        ran_ue_id = uint_to_ran_ue_id(ngap_pdu.pdu.init_msg().value.init_ue_msg()->ran_ue_ngap_id);
      }
    }
  }

  fuzz_state&                        state;
  gnb_du_ue_f1ap_id_t                du_ue_id;
  std::optional<gnb_cu_ue_f1ap_id_t> cu_ue_id;
  std::unique_ptr<du_srb_pdcp_tx>    srb1_pdcp;
  std::unique_ptr<du_srb_pdcp_tx>    srb2_pdcp;
  std::optional<ran_ue_id_t>         ran_ue_id;
  /// Any AMF UE ID works: the stub AMF holds no state of its own.
  static constexpr amf_ue_id_t amf_ue_id = uint_to_amf_ue_id(0x1234);
};

// ---------------------------------------------------------------------------
// NGAP UE identifier rewriting
// ---------------------------------------------------------------------------

/// Point an initiating NGAP message at the given UE.
///
/// A mutated message almost always names a UE that does not exist, and the CU-CP drops it at the UE
/// lookup before any procedure runs. Rewriting the identifiers is what lets a fuzzed message reach
/// the UE-associated procedures at all.
///
/// Mirrors the case list of asn1_utils::get_ran_ue_id()/get_amf_ue_id() for initiating messages,
/// which is the direction the AMF sends. Returns whether the message carries identifiers to set.
inline bool set_ue_ids(asn1::ngap::init_msg_s& init_msg, ran_ue_id_t ran_ue_id, amf_ue_id_t amf_ue_id)
{
  using namespace asn1::ngap;
  using init_types      = ngap_elem_procs_o::init_msg_c::types_opts;
  const uint64_t ran_id = to_underlying(ran_ue_id);
  const uint64_t amf_id = to_underlying(amf_ue_id);

  switch (init_msg.value.type()) {
    case init_types::init_context_setup_request:
      init_msg.value.init_context_setup_request()->ran_ue_ngap_id = ran_id;
      init_msg.value.init_context_setup_request()->amf_ue_ngap_id = amf_id;
      return true;
    case init_types::pdu_session_res_setup_request:
      init_msg.value.pdu_session_res_setup_request()->ran_ue_ngap_id = ran_id;
      init_msg.value.pdu_session_res_setup_request()->amf_ue_ngap_id = amf_id;
      return true;
    case init_types::pdu_session_res_modify_request:
      init_msg.value.pdu_session_res_modify_request()->ran_ue_ngap_id = ran_id;
      init_msg.value.pdu_session_res_modify_request()->amf_ue_ngap_id = amf_id;
      return true;
    case init_types::pdu_session_res_release_cmd:
      init_msg.value.pdu_session_res_release_cmd()->ran_ue_ngap_id = ran_id;
      init_msg.value.pdu_session_res_release_cmd()->amf_ue_ngap_id = amf_id;
      return true;
    case init_types::ue_context_mod_request:
      init_msg.value.ue_context_mod_request()->ran_ue_ngap_id = ran_id;
      init_msg.value.ue_context_mod_request()->amf_ue_ngap_id = amf_id;
      return true;
    case init_types::ue_context_release_cmd:
      if (init_msg.value.ue_context_release_cmd()->ue_ngap_ids.type().value ==
          ue_ngap_ids_c::types_opts::ue_ngap_id_pair) {
        init_msg.value.ue_context_release_cmd()->ue_ngap_ids.ue_ngap_id_pair().ran_ue_ngap_id = ran_id;
        init_msg.value.ue_context_release_cmd()->ue_ngap_ids.ue_ngap_id_pair().amf_ue_ngap_id = amf_id;
      } else {
        init_msg.value.ue_context_release_cmd()->ue_ngap_ids.amf_ue_ngap_id() = amf_id;
      }
      return true;
    case init_types::ue_context_resume_request:
      init_msg.value.ue_context_resume_request()->ran_ue_ngap_id = ran_id;
      init_msg.value.ue_context_resume_request()->amf_ue_ngap_id = amf_id;
      return true;
    case init_types::ue_context_suspend_request:
      init_msg.value.ue_context_suspend_request()->ran_ue_ngap_id = ran_id;
      init_msg.value.ue_context_suspend_request()->amf_ue_ngap_id = amf_id;
      return true;
    case init_types::dl_nas_transport:
      init_msg.value.dl_nas_transport()->ran_ue_ngap_id = ran_id;
      init_msg.value.dl_nas_transport()->amf_ue_ngap_id = amf_id;
      return true;
    case init_types::dl_ue_associated_nrppa_transport:
      init_msg.value.dl_ue_associated_nrppa_transport()->ran_ue_ngap_id = ran_id;
      init_msg.value.dl_ue_associated_nrppa_transport()->amf_ue_ngap_id = amf_id;
      return true;
    case init_types::ho_request:
      init_msg.value.ho_request()->amf_ue_ngap_id = amf_id;
      return true;
    case init_types::error_ind:
      if (init_msg.value.error_ind()->ran_ue_ngap_id_present) {
        init_msg.value.error_ind()->ran_ue_ngap_id = ran_id;
      }
      if (init_msg.value.error_ind()->amf_ue_ngap_id_present) {
        init_msg.value.error_ind()->amf_ue_ngap_id = amf_id;
      }
      return true;
    default:
      return false;
  }
}

/// Point an NGAP PDU at the given UE. Only initiating messages carry identifiers this rewrites.
inline bool set_ue_ids(asn1::ngap::ngap_pdu_c& pdu, ran_ue_id_t ran_ue_id, amf_ue_id_t amf_ue_id)
{
  if (pdu.type().value != asn1::ngap::ngap_pdu_c::types_opts::init_msg) {
    return false;
  }
  return set_ue_ids(pdu.init_msg(), ran_ue_id, amf_ue_id);
}

} // namespace ocudu::ocucp::fuzz
