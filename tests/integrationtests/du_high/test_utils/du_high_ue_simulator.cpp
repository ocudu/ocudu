// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_high_ue_simulator.h"
#include "du_high_env_simulator.h"
#include "tests/test_doubles/mac/mac_test_messages.h"
#include "ocudu/du/du_high/du_high_configuration.h"
#include "ocudu/mac/mac_clock_controller.h"
#include "ocudu/pcap/rlc_pcap.h"
#include "ocudu/rlc/rlc_factory.h"
#include "ocudu/rlc/rlc_srb_config_factory.h"

using namespace ocudu;

namespace {

null_rlc_pcap pcap_sink;

struct mac_sdu {
  lcid_t      lcid;
  byte_buffer payload;
};

std::vector<mac_sdu> parse_mac_sdus(span<const uint8_t> pdu_buffer)
{
  std::vector<mac_sdu> sdus;

  size_t offset = 0;
  while (offset < pdu_buffer.size()) {
    const uint8_t       subheader_byte = pdu_buffer[offset++];
    const bool          F_bit          = (subheader_byte & 0x40U) != 0;
    const auto          lcid           = static_cast<lcid_t>(subheader_byte & 0x3fU);
    const lcid_dl_sch_t lcid_sch       = lcid_dl_sch_t{lcid};

    if (lcid_sch == lcid_dl_sch_t::PADDING) {
      break;
    }

    if (not lcid_sch.is_sdu()) {
      // Not an SDU subheader; skip.
      report_fatal_error_if_not(not lcid_sch.is_var_len_ce(), "Variable length CEs not supported");
      report_fatal_error_if_not(offset + lcid_sch.sizeof_ce() <= pdu_buffer.size(),
                                "Invalid MAC PDU: Incomplete MAC subheader for CE");
      offset += lcid_sch.sizeof_ce();
      continue;
    }
    const unsigned L_len = F_bit ? 2 : 1;
    report_fatal_error_if_not(offset + L_len <= pdu_buffer.size(), "Invalid MAC PDU: Incomplete MAC subheader for SDU");

    // Decode L field.
    unsigned sdu_length = 0;
    if (F_bit) {
      // 16-bit L field
      sdu_length = static_cast<unsigned>(pdu_buffer[offset] << 8U) | (static_cast<unsigned>(pdu_buffer[offset + 1]));
      offset += 2;
    } else {
      // 8-bit L field
      sdu_length = static_cast<unsigned>(pdu_buffer[offset++]);
    }

    report_fatal_error_if_not(sdu_length + offset <= pdu_buffer.size(), "Invalid MAC PDU: SDU length exceeds PDU size");

    mac_sdu sdu;
    sdu.lcid                       = lcid;
    span<const uint8_t> sdupayload = pdu_buffer.subspan(offset, sdu_length);
    sdu.payload                    = byte_buffer::create(sdupayload.begin(), sdupayload.end()).value();
    sdus.push_back(std::move(sdu));

    offset += sdu_length;
  }

  return sdus;
}

} // namespace

class du_high_ue_simulator::rlc_bearer_adapter : public rlc_tx_lower_layer_notifier,
                                                 public rlc_tx_upper_layer_control_notifier,
                                                 public rlc_tx_upper_layer_data_notifier,
                                                 public rlc_rx_upper_layer_data_notifier
{
public:
  void on_buffer_state_update(const rlc_buffer_state& bsr) override { last_reported_bo = bsr; }

  void on_protocol_failure() override {}
  void on_max_retx() override {}

  void on_transmitted_sdu(uint32_t max_tx_pdcp_sn, uint32_t desired_buf_size) override {}
  void on_delivered_sdu(uint32_t max_deliv_pdcp_sn) override {}
  void on_retransmitted_sdu(uint32_t max_retx_pdcp_sn) override {}
  void on_delivered_retransmitted_sdu(uint32_t max_deliv_retx_pdcp_sn) override {}

  void on_new_sdu(byte_buffer_chain pdu) override
  {
    last_dl_sdus.push_back(byte_buffer::create(pdu.begin(), pdu.end()).value());
  }

  /// Last reported buffer occupancy for this RLC entity.
  std::optional<rlc_buffer_state> last_reported_bo;

  /// Last DL SDUs received by the UE for this RLC entity.
  std::vector<byte_buffer> last_dl_sdus;
};

du_high_ue_simulator::du_high_ue_simulator(const du_high_ue_simulator_config& cfg_,
                                           const odu::du_high_dependencies&   du_hi_deps_,
                                           task_executor&                     test_exec_) :
  cfg(cfg_), test_exec(test_exec_)
{
  // Create SRB entities upfront.
  std::map<srb_id_t, odu::du_srb_config> srb_cfgs = cfg.du_high_cfg.ran.srbs;
  if (srb_cfgs.count(srb_id_t::srb0) == 0) {
    srb_cfgs[srb_id_t::srb0].rlc = make_default_srb0_rlc_config();
  }
  if (srb_cfgs.count(srb_id_t::srb1) == 0) {
    srb_cfgs[srb_id_t::srb1].rlc = make_default_srb_rlc_config();
  }
  if (srb_cfgs.count(srb_id_t::srb2) == 0) {
    srb_cfgs[srb_id_t::srb2].rlc = make_default_srb_rlc_config();
  }

  for (const auto& [srb_id, srb_cfg] : srb_cfgs) {
    bearer_context bc;
    bc.adapter = std::make_unique<rlc_bearer_adapter>();
    {
      rlc_entity_creation_message msg;
      // Reserve max DU-ID for UE RLC entities.
      msg.gnb_du_id         = gnb_du_id_t::max;
      msg.ue_index          = to_du_ue_index(static_cast<unsigned>(cfg.crnti) % MAX_NOF_DU_UES);
      msg.rb_id             = rb_id_t{srb_id};
      msg.config            = srb_cfg.rlc;
      msg.rx_upper_dn       = bc.adapter.get();
      msg.tx_upper_dn       = bc.adapter.get();
      msg.tx_upper_cn       = bc.adapter.get();
      msg.tx_lower_dn       = bc.adapter.get();
      msg.timers            = &du_hi_deps_.timer_ctrl->get_timer_manager();
      msg.pcell_executor    = &test_exec;
      msg.ue_executor       = &test_exec;
      msg.rlc_metrics_notif = nullptr;
      msg.pcap_writer       = &pcap_sink;
      msg.srb_am_rx_pool    = &srb_rx_seg_pool->get_pool_of_type<rlc_rx_am_sdu_info>();
      msg.srb_am_tx_pool    = &srb_tx_seg_pool->get_pool_of_type<rlc_tx_am_sdu_info>();
      auto entity           = create_rlc_entity(msg);
      bc.rlc                = std::move(entity);
    }
    auto ret = bearers.insert(std::make_pair(srb_id_to_lcid(srb_id), std::move(bc)));
    ocudu_assert(ret.second, "Failed to create RLC entity for SRB {}", fmt::underlying(srb_id));
  }
}

du_high_ue_simulator::~du_high_ue_simulator() {}

void du_high_ue_simulator::handle_dl_pdu(const mac_dl_data_result::dl_pdu& pdu)
{
  auto sdus = parse_mac_sdus(pdu.pdu.get_buffer());

  for (const auto& sdu : sdus) {
    if (not is_srb(sdu.lcid)) {
      // For now, just handle SRBs.
      continue;
    }

    auto it = bearers.find(sdu.lcid);
    ocudu_assert(
        it != bearers.end(), "Received DL PDU for LCID {} without existing RLC entity", fmt::underlying(sdu.lcid));
    it->second.rlc->get_rx_lower_layer_interface()->handle_pdu(sdu.payload);
  }
}

std::optional<byte_buffer> du_high_ue_simulator::build_next_ul_mac_pdu()
{
  byte_buffer pdu;
  for (const auto& [lcid, bearer] : bearers) {
    while (bearer.adapter->last_reported_bo.has_value() and bearer.adapter->last_reported_bo->pending_bytes > 0) {
      // The UE RLC entity has data to transmit.
      std::vector<uint8_t> buffer(bearer.adapter->last_reported_bo->pending_bytes);
      size_t               n = bearer.rlc->get_tx_lower_layer_interface()->pull_pdu(buffer);
      if (n == 0) {
        // No PDU was filled.
        break;
      }
      byte_buffer sdu_and_subhr =
          test_helpers::prepend_mac_subheader(lcid, byte_buffer::create(span<uint8_t>(buffer.data(), n)).value());
      *bearer.adapter->last_reported_bo = bearer.rlc->get_tx_lower_layer_interface()->get_buffer_state();
      report_fatal_error_if_not(
          pdu.append(sdu_and_subhr), "Failed to append UL MAC SDU for LCID {} to UL MAC PDU", fmt::underlying(lcid));
    }
  }
  if (pdu.empty()) {
    return std::nullopt;
  }
  return pdu;
}

void du_high_ue_simulator::enqueue_ul_mac_sdu(lcid_t lcid, byte_buffer ul_mac_sdu)
{
  auto it = bearers.find(lcid);
  report_fatal_error_if_not(it != bearers.end(), "No RLC entity for LCID {}", fmt::underlying(lcid));
  it->second.rlc->get_tx_upper_layer_data_interface()->handle_sdu(std::move(ul_mac_sdu), false);
}

std::vector<std::pair<lcid_t, byte_buffer>> du_high_ue_simulator::pop_pending_dl_mac_sdus()
{
  std::vector<std::pair<lcid_t, byte_buffer>> sdus;
  for (auto& [lcid, bearer] : bearers) {
    for (byte_buffer& sdu : bearer.adapter->last_dl_sdus) {
      sdus.emplace_back(lcid, std::move(sdu));
    }
    bearer.adapter->last_dl_sdus.clear();
  }
  return sdus;
}
