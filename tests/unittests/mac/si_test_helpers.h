// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/mac/mac_ctrl/si_message_controller.h"
#include "lib/mac/mac_dl/mac_dl_configurator.h"
#include "lib/mac/mac_dl/sib_pdu_assembler.h"
#include "mac_test_helpers.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/asn1/rrc_nr/bcch_dl_sch_msg.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/support/async/async_no_op_task.h"
#include "ocudu/support/executors/manual_task_worker.h"

namespace ocudu {
namespace test_helpers {

inline byte_buffer make_random_pdu(unsigned size = test_rng::uniform_int<unsigned>(10, 200))
{
  return byte_buffer::create(test_rng::vector_of_uniform_ints<uint8_t>(size)).value();
}

inline std::vector<byte_buffer> make_random_segmented_pdu(unsigned segment_size = test_rng::uniform_int<unsigned>(10,
                                                                                                                  200),
                                                          unsigned nof_segments = test_rng::uniform_int<unsigned>(2, 3))
{
  std::vector<byte_buffer> segmented_pdu;
  for (unsigned i_segment = 0; i_segment != nof_segments; ++i_segment) {
    segmented_pdu.emplace_back(make_random_pdu(segment_size));
  }
  return segmented_pdu;
}

/// \brief Packs a BCCH-DL-SCH message carrying a SIB1 that schedules one SI message per given SIB set.
///
/// Only the fields the SI message controller reads back are filled, so that the payload round-trips through the ASN.1
/// unpacking that a si-BroadcastStatus update requires.
inline byte_buffer make_sib1_with_si_sched_info(span<const sib_type> si_msg_sibs)
{
  asn1::rrc_nr::bcch_dl_sch_msg_s msg;
  asn1::rrc_nr::sib1_s&           sib1 = msg.msg.set_c1().set_sib_type1();

  sib1.cell_access_related_info.plmn_id_info_list.resize(1);
  auto& plmn_info = sib1.cell_access_related_info.plmn_id_info_list[0];
  plmn_info.plmn_id_list.resize(1);
  auto& plmn       = plmn_info.plmn_id_list[0];
  plmn.mcc_present = true;
  plmn.mcc         = {0, 0, 1};
  plmn.mnc.resize(2);
  plmn.mnc[0]           = 0;
  plmn.mnc[1]           = 1;
  plmn_info.tac_present = false;
  plmn_info.cell_id.from_number(0x19b0);
  plmn_info.cell_reserved_for_oper.value = asn1::rrc_nr::plmn_id_info_s::cell_reserved_for_oper_opts::not_reserved;

  sib1.si_sched_info_present          = true;
  sib1.si_sched_info.si_win_len.value = asn1::rrc_nr::si_sched_info_s::si_win_len_opts::s20;
  sib1.si_sched_info.sched_info_list.resize(si_msg_sibs.size());
  for (unsigned i = 0, e = si_msg_sibs.size(); i != e; ++i) {
    const sib_type sib        = si_msg_sibs[i];
    auto&          sched_info = sib1.si_sched_info.sched_info_list[i];
    // Mirrors the DU packer: this payload is the reference every SI epoch is derived from, so every entry it lists is
    // one being broadcast.
    sched_info.si_broadcast_status.value = asn1::rrc_nr::sched_info_s::si_broadcast_status_opts::broadcasting;
    sched_info.si_periodicity.value      = asn1::rrc_nr::sched_info_s::si_periodicity_opts::rf16;
    sched_info.sib_map_info.resize(1);
    auto& sib_info = sched_info.sib_map_info[0];
    switch (sib) {
      case sib_type::sib6:
        sib_info.type.value = asn1::rrc_nr::sib_type_info_s::type_opts::sib_type6;
        break;
      case sib_type::sib7:
        sib_info.type.value = asn1::rrc_nr::sib_type_info_s::type_opts::sib_type7;
        break;
      case sib_type::sib8:
        sib_info.type.value = asn1::rrc_nr::sib_type_info_s::type_opts::sib_type8;
        break;
      default:
        sib_info.type.value = asn1::rrc_nr::sib_type_info_s::type_opts::sib_type2;
        break;
    }
  }

  byte_buffer   buf;
  asn1::bit_ref bref{buf};
  report_fatal_error_if_not(msg.pack(bref) == asn1::OCUDUASN_SUCCESS, "Failed to pack the test SIB1");
  return buf;
}

/// Returns the SIB carried by each SI message that a packed BCCH-DL-SCH SIB1 payload lists, in the listed order.
inline std::vector<sib_type> get_listed_sibs(span<const uint8_t> sib1_pdu)
{
  byte_buffer                     sib1_buf = byte_buffer::create(sib1_pdu).value();
  asn1::rrc_nr::bcch_dl_sch_msg_s msg;
  asn1::cbit_ref                  bref{sib1_buf};
  report_fatal_error_if_not(msg.unpack(bref) == asn1::OCUDUASN_SUCCESS, "Failed to unpack the SIB1");

  const asn1::rrc_nr::sib1_s& sib1 = msg.msg.c1().sib_type1();
  std::vector<sib_type>       listed;
  if (not sib1.si_sched_info_present) {
    return listed;
  }
  for (const auto& sched_info : sib1.si_sched_info.sched_info_list) {
    report_fatal_error_if_not(sched_info.sib_map_info.size() == 1, "Test SI messages carry a single SIB");
    listed.push_back(static_cast<sib_type>(sched_info.sib_map_info[0].type.to_number()));
  }
  return listed;
}

inline byte_buffer make_pdu_with_padding(const byte_buffer& payload, units::bytes tbs)
{
  byte_buffer          result = payload.deep_copy().value();
  std::vector<uint8_t> zeros(tbs.value() - result.length(), 0);
  report_fatal_error_if_not(result.append(zeros), "Failed appending zeros");
  return result;
}

inline sib_information
make_sib_pdu(std::optional<unsigned> si_msg_index, si_version_type si_version, units::bytes tbs, sib_type_set sibs = {})
{
  sib_information result{};
  result.si_indicator  = si_msg_index.has_value() ? sib_information::other_si : sib_information::sib1;
  result.si_msg_index  = si_msg_index;
  result.sibs          = sibs;
  result.version       = si_version;
  result.is_repetition = false;
  result.pdsch_cfg.codewords.emplace_back();
  result.pdsch_cfg.codewords[0].tb_size_bytes = tbs;
  return result;
}

/// SIBs carried by each SI message of an SI epoch, in the order the epoch holds them.
inline std::vector<sib_type> get_epoch_sibs(const si_update_command& cmd)
{
  std::vector<sib_type> sibs;
  for (const si_message_scheduling_config& si_msg : cmd.si_sched_cfg.si_messages) {
    sibs.push_back(si_msg.sibs.front());
  }
  return sibs;
}

/// SIBs that the SIB1 of an SI epoch lists in its schedulingInfoList.
inline std::vector<sib_type> get_sib1_listed_sibs(const si_update_command& cmd, slot_point_extended sl_tx)
{
  sib_information si_info = make_sib_pdu(std::nullopt, cmd.version, units::bytes{MAX_BCCH_DL_SCH_PDU_SIZE / 2});
  auto            payload = cmd.sib1->encode(sl_tx, si_info);
  report_fatal_error_if_not(payload.has_value(), "Failed to encode SIB1");
  return get_listed_sibs(payload.value());
}

/// \brief Bench pairing an SI message controller with the SIB assembler it feeds.
///
/// The encoders are owned by the controller, so the assembler can only be exercised through the commands the
/// controller emits. It stands in for the MAC DL cell the controller applies its SI epochs in.
class si_bench : private mac_dl_cell_controller
{
  /// Cell that discards the SI epochs it is given.
  class null_dl_cell final : public mac_dl_cell_controller
  {
    async_task<void> start() override { return launch_no_op_task(); }
    async_task<void> stop() override { return launch_no_op_task(); }
    void             start_broadcast(std::shared_ptr<si_message_extension_handler> ext_handler,
                                     const si_update_command&                      cmd,
                                     std::unique_ptr<pws_broadcast_end_notifier>   pws_end_notifier) override
    {
    }
    void             handle_si_update(const si_update_command& cmd) override {}
    async_task<void> reconfigure(const mac_dl_cell_reconfig_request& request) override { return launch_no_op_task(); }
  };

public:
  explicit si_bench(mac_cell_sys_info_config sys_info_cfg_) :
    sys_info_cfg(std::move(sys_info_cfg_)),
    si_mng(to_du_cell_index(0), sys_info_cfg, timer_factory{timers, task_worker}, task_worker, *this)
  {
  }

  /// Forwards a new System Information config to the controller and applies the resulting command, if any.
  std::optional<si_update_command> update_si(const mac_cell_sys_info_config& req)
  {
    last_si_cmd.reset();
    si_mng.handle_si_change_request(req, std::nullopt);
    return last_si_cmd;
  }

  /// Forwards SI message PDU updates, which includes the push of a warning content, to the controller.
  bool push_si_pdu_updates(const mac_cell_sys_info_pdu_update& req)
  {
    return si_mng.handle_si_change_request(std::nullopt, req).si_pdus_enqueued;
  }

  /// \brief Builds an SI epoch out of a separate SI configuration and applies it as the ETWS/CMAS one.
  ///
  /// Stands in for the warning snapshot that the SI message controller will derive from the baseline one, so that
  /// the assembler can be exercised with two coexisting epochs.
  si_update_command apply_pws_si(const mac_cell_sys_info_config& pws_cfg, si_version_type version)
  {
    pws_si_mng = std::make_unique<si_message_controller>(
        to_du_cell_index(0), pws_cfg, timer_factory{timers, task_worker}, task_worker, null_dl);

    si_update_command cmd = pws_si_mng->last_command();
    cmd.version           = version;
    assembler.handle_pws_si_update(cmd);
    return cmd;
  }

  /// \brief Properties of the SI message the last generated ETWS/CMAS epoch lists as broadcasting.
  /// \remark The epoch must list exactly one.
  pws_broadcasting_si_message only_broadcasting_si_message() const
  {
    report_fatal_error_if_not(last_pws_cmd.has_value() and last_pws_cmd->active_pws_si_messages.size() == 1,
                              "Expected exactly one SI message broadcasting a warning");
    return last_pws_cmd->active_pws_si_messages[0];
  }

  /// \brief Serves one SIB1 grant stamped with a given SI epoch, as the cell does every SIB1 occasion.
  ///
  /// Going back to the epoch of the normal operation is how the MAC learns that a warning is over.
  void serve_sib1_grant(si_version_type version)
  {
    assembler.encode_si_pdu(current_slot,
                            make_sib_pdu(std::nullopt, version, units::bytes{MAX_BCCH_DL_SCH_PDU_SIZE / 2}));

    // The end of a warning broadcast is notified in the cell control context.
    task_worker.run_pending_tasks();
  }

  /// Advances the timers by the given number of milliseconds, running any dispatched task.
  void tick(unsigned nof_ticks)
  {
    for (unsigned t = 0; t != nof_ticks; ++t) {
      timers.tick();
      task_worker.run_pending_tasks();
    }
  }

  manual_task_worker          task_worker{128};
  timer_manager               timers;
  dummy_mac_scheduler_adapter sched;

  mac_cell_sys_info_config sys_info_cfg;
  sib_pdu_assembler        assembler;

  // Last SI epoch of the normal operation the controller pushed.
  std::optional<si_update_command> last_si_cmd;

  // Last ETWS/CMAS epoch the controller pushed, and how many it pushed in total.
  std::optional<si_update_command> last_pws_cmd;
  unsigned                         nof_pws_epochs = 0;

  slot_point_extended current_slot{subcarrier_spacing::kHz30, 0};

  // Declared last, given that its constructor already applies SI epochs in this bench.
  si_message_controller si_mng;

  // Stands in for the SI message controller-side production of the ETWS/CMAS epoch.
  null_dl_cell                           null_dl;
  std::unique_ptr<si_message_controller> pws_si_mng;

private:
  async_task<void> start() override { return launch_no_op_task(); }

  async_task<void> stop() override { return launch_no_op_task(); }

  void start_broadcast(std::shared_ptr<si_message_extension_handler> ext_handler_,
                       const si_update_command&                      cmd,
                       std::unique_ptr<pws_broadcast_end_notifier>   pws_end_notifier) override
  {
    assembler.start_broadcast(std::move(ext_handler_), cmd, std::move(pws_end_notifier));
  }

  void handle_si_update(const si_update_command& cmd) override
  {
    if (not cmd.active_pws_si_messages.empty()) {
      handle_pws_si_update(cmd);
      return;
    }
    assembler.handle_si_update(cmd);
    last_si_cmd = cmd;
  }

  void handle_pws_si_update(const si_update_command& cmd)
  {
    assembler.handle_pws_si_update(cmd);
    sched.handle_pws_si_change_indication(pws_si_scheduling_update_request{
        to_du_cell_index(0), cmd.version, cmd.si_sched_cfg, cmd.active_pws_si_messages});
    last_pws_cmd = cmd;
    ++nof_pws_epochs;
  }

  async_task<void> reconfigure(const mac_dl_cell_reconfig_request& request) override { return launch_no_op_task(); }
};

} // namespace test_helpers
} // namespace ocudu
