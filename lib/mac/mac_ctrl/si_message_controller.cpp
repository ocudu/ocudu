// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "si_message_controller.h"
#include "../mac_dl/mac_dl_configurator.h"
#include "../mac_dl/mac_scheduler_cell_configurator.h"
#include "../mac_dl/segmented_sib_list.h"
#include "ocudu/adt/lockfree_triple_buffer.h"
#include "ocudu/asn1/rrc_nr/bcch_dl_sch_msg.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <algorithm>
#include <functional>

using namespace ocudu;

/// Converts an ASN.1 SIB type into its RAN representation, or sib_invalid if it has no representation.
static sib_type to_sib_type(const asn1::rrc_nr::sib_type_info_s& sib_info)
{
  using type_opts = asn1::rrc_nr::sib_type_info_s::type_opts;
  switch (sib_info.type.value) {
    case type_opts::sib_type2:
      return sib_type::sib2;
    case type_opts::sib_type3:
      return sib_type::sib3;
    case type_opts::sib_type4:
      return sib_type::sib4;
    case type_opts::sib_type5:
      return sib_type::sib5;
    case type_opts::sib_type6:
      return sib_type::sib6;
    case type_opts::sib_type7:
      return sib_type::sib7;
    case type_opts::sib_type8:
      return sib_type::sib8;
    default:
      return sib_type::sib_invalid;
  }
}

/// SIBs carried by an SI message listed in the SIB1 schedulingInfoList.
static sib_type_set to_sib_type_set(const asn1::rrc_nr::sched_info_s& sched_info)
{
  sib_type_set sib_set;
  for (const asn1::rrc_nr::sib_type_info_s& sib_info : sched_info.sib_map_info) {
    const sib_type sib = to_sib_type(sib_info);
    if (sib != sib_type::sib_invalid) {
      sib_set.add(sib);
    }
  }
  return sib_set;
}

/// Makes room in a SIB1 for the HyperSFN that the HyperSFN-aware encoder keeps up to date.
static void ensure_hypersfn_present(asn1::rrc_nr::sib1_s& sib1_msg)
{
  sib1_msg.non_crit_ext_present                                         = true;
  sib1_msg.non_crit_ext.non_crit_ext_present                            = true;
  sib1_msg.non_crit_ext.non_crit_ext.non_crit_ext_present               = true;
  sib1_msg.non_crit_ext.non_crit_ext.non_crit_ext.hyper_sfn_r17_present = true;
}

/// \brief Repacks a SIB1 payload so that its schedulingInfoList lists the SI messages that an SI epoch broadcasts.
///
/// An SI message carrying a warning is only listed while the warning is on air, and after the SI messages that are
/// always broadcast, so that their SI windows stay in place. As per TS 38.331, 5.2.2.2.2, listing it does not require
/// an SI change notification: the etwsAndCmasIndication short message makes the UE re-acquire SIB1.
/// \return The repacked payload, or an empty buffer if the reference payload could not be processed.
static byte_buffer
repack_sib1_si_sched_info(const byte_buffer& sib1, span<const sib_type_set> on_air, bool with_hypersfn)
{
  asn1::rrc_nr::bcch_dl_sch_msg_s msg;
  {
    asn1::cbit_ref bref{sib1};
    if (msg.unpack(bref) != asn1::OCUDUASN_SUCCESS or
        msg.msg.type().value != asn1::rrc_nr::bcch_dl_sch_msg_type_c::types_opts::c1 or
        msg.msg.c1().type().value != asn1::rrc_nr::bcch_dl_sch_msg_type_c::c1_c_::types_opts::sib_type1) {
      return {};
    }
  }

  asn1::rrc_nr::sib1_s& sib1_msg        = msg.msg.c1().sib_type1();
  auto&                 sched_info_list = sib1_msg.si_sched_info.sched_info_list;

  // Note: PWS SIBs are release 15 SIBs, and an SI message cannot mix release 17 and non-release 17 SIBs. Hence, a PWS
  // SI message is always listed in schedulingInfoList, and never in schedulingInfoList2.
  for (auto* it = sched_info_list.begin(); it != sched_info_list.end();) {
    const sib_type_set sib_set = to_sib_type_set(*it);
    const bool listed = not sib_set.is_pws() or std::find(on_air.begin(), on_air.end(), sib_set) != on_air.end();
    it                = listed ? it + 1 : sched_info_list.erase(it);
  }
  std::stable_partition(sched_info_list.begin(), sched_info_list.end(), [](const auto& sched_info) {
    return not to_sib_type_set(sched_info).is_pws();
  });

  // si-WindowLength comes with the schedulingInfoList, which must hold at least one entry.
  sib1_msg.si_sched_info_present = sched_info_list.size() != 0;

  if (with_hypersfn) {
    ensure_hypersfn_present(sib1_msg);
  }

  byte_buffer   repacked;
  asn1::bit_ref bref{repacked};
  if (msg.pack(bref) != asn1::OCUDUASN_SUCCESS) {
    return {};
  }
  return repacked;
}

/// Encoder for a static BCCH-DL-SCH SIB1 payload.
class si_message_controller::sib1_static_encoder final : public bcch_dl_sch_msg_encoder
{
public:
  sib1_static_encoder(const byte_buffer& buffer) : len(buffer.length()), current_payload(MAX_BCCH_DL_SCH_PDU_SIZE, 0)
  {
    copy_segments(buffer, current_payload);
  }

  expected<span<const uint8_t>, units::bytes> encode(slot_point_extended /*sl_tx*/,
                                                     const sib_information& si_info) override
  {
    const unsigned tbs = si_info.pdsch_cfg.codewords[0].tb_size_bytes.value();
    if (OCUDU_LIKELY(tbs >= len.value())) {
      return span<const uint8_t>(current_payload.data(), tbs);
    }
    return make_unexpected(len);
  }

private:
  units::bytes         len;
  std::vector<uint8_t> current_payload;
};

/// BCCH-DL-SCH SIB1 buffer that gets hyperSFN auto-updated, when eDRX is enabled.
class si_message_controller::sib1_hypersfn_encoder final : public bcch_dl_sch_msg_encoder
{
public:
  sib1_hypersfn_encoder(const byte_buffer& buffer) : current_payload(MAX_BCCH_DL_SCH_PDU_SIZE, 0)
  {
    // Unpack initial SIB1.
    {
      asn1::cbit_ref bref{buffer};
      auto           err = current_unpacked.unpack(bref);
      report_fatal_error_if_not(err == asn1::OCUDUASN_SUCCESS, "Failed to unpack SIB1 for HyperSFN-aware encoder.");
      report_fatal_error_if_not(current_unpacked.msg.type().value ==
                                        asn1::rrc_nr::bcch_dl_sch_msg_type_c::types_opts::c1 and
                                    current_unpacked.msg.c1().type().value ==
                                        asn1::rrc_nr::bcch_dl_sch_msg_type_c::c1_c_::types_opts::sib_type1,
                                "SIB1 message is not of expected type for HyperSFN-aware encoder.");
    }

    // Ensure HyperSFN is encoded in SIB1 and its value matches the member current_hyper_sfn.
    ensure_hypersfn_present(current_unpacked.msg.c1().sib_type1());
    build_bcch_dl_sch_payload(0);
  }

  expected<span<const uint8_t>, units::bytes> encode(slot_point_extended sl_tx, const sib_information& si_info) override
  {
    const unsigned tbs = si_info.pdsch_cfg.codewords[0].tb_size_bytes.value();
    if (OCUDU_UNLIKELY(tbs < current_len.value())) {
      return make_unexpected(current_len);
    }

    const uint32_t new_hyper_sfn = sl_tx.hyper_sfn();
    if (current_hyper_sfn != new_hyper_sfn) {
      // HyperSFN has changed, re-encode payload.
      build_bcch_dl_sch_payload(new_hyper_sfn);
    }
    return span<const uint8_t>(current_payload.data(), tbs);
  }

private:
  void build_bcch_dl_sch_payload(uint32_t hyper_sfn)
  {
    // Update HyperSFN.
    current_hyper_sfn = hyper_sfn;

    // Update HyperSFN in unpacked SIB1.
    current_unpacked.msg.c1().sib_type1().non_crit_ext.non_crit_ext.non_crit_ext.hyper_sfn_r17.from_number(hyper_sfn);

    // Re-encode.
    byte_buffer   buf;
    asn1::bit_ref bref{buf};
    auto          ret = current_unpacked.pack(bref);
    ocudu_assert(ret == asn1::OCUDUASN_SUCCESS, "Failed to pack SIB1 with updated HyperSFN.");
    size_t n = copy_segments(buf, current_payload);
    ocudu_assert(n <= current_payload.size(), "Encoded SIB1 payload exceeds maximum size.");
    current_len = units::bytes{static_cast<unsigned>(n)};
  }

  asn1::rrc_nr::bcch_dl_sch_msg_s current_unpacked;
  std::vector<uint8_t>            current_payload;
  units::bytes                    current_len{0};
  uint32_t                        current_hyper_sfn = 0;
};

/// Encoder for a static (non-PWS) SI-message. Static SI-messages are never segmented -- only PWS (SIB6/7/8) content
/// segments across multiple SI-message windows (see \c pws_si_msg_encoder).
class si_message_controller::static_si_msg_encoder final : public bcch_dl_sch_msg_encoder
{
public:
  explicit static_si_msg_encoder(const byte_buffer& si_msg) :
    segment{make_linear_bcch_dl_sch_buffer(si_msg), units::bytes{static_cast<unsigned>(si_msg.length())}}
  {
  }

  expected<span<const uint8_t>, units::bytes> encode(slot_point_extended /*sl_tx*/,
                                                     const sib_information& si_info) override
  {
    const unsigned tbs = si_info.pdsch_cfg.codewords[0].tb_size_bytes.value();
    if (OCUDU_UNLIKELY(tbs < segment.len.value())) {
      return make_unexpected(segment.len);
    }
    return span<const uint8_t>(segment.buffer->data(), tbs);
  }

private:
  bcch_segment segment;
};

/// Encoder of the content of a PWS broadcast. See class doc in the header.
class si_message_controller::pws_si_msg_encoder final : public bcch_dl_sch_msg_encoder
{
public:
  explicit pws_si_msg_encoder(span<const byte_buffer> content)
  {
    for (const byte_buffer& segment : content) {
      const units::bytes seg_len{static_cast<unsigned>(segment.length())};
      segments.append_segment(bcch_segment{make_linear_bcch_dl_sch_buffer(segment), seg_len});
      // The scheduler sizes the PDSCH grant for this SI-message off the largest segment, since segments may not all
      // be the same length (e.g. a shorter final segment).
      max_seg_len = std::max(max_seg_len, seg_len);
    }
  }

  /// Length, in bytes, of the largest segment of the warning message.
  units::bytes largest_segment_len() const { return max_seg_len; }

  /// Number of segments composing the warning message.
  unsigned nof_segments() const { return segments.get_nof_segments(); }

  expected<span<const uint8_t>, units::bytes> encode(slot_point_extended /*sl_tx*/,
                                                     const sib_information& si_info) override
  {
    if (force_segment_zero) {
      force_segment_zero = false;
    } else if (segments.get_nof_segments() > 1 and !si_info.is_repetition and (si_info.nof_txs > 0)) {
      // SIB6 is never segmented; advancing only applies to segmented SIB7/8 content.
      segments.advance_current_segment();
    }

    const bcch_segment& seg = segments.get_current_segment();
    const unsigned      tbs = si_info.pdsch_cfg.codewords[0].tb_size_bytes.value();
    if (OCUDU_UNLIKELY(tbs < seg.len.value())) {
      return make_unexpected(seg.len);
    }
    return span<const uint8_t>(seg.buffer->data(), tbs);
  }

private:
  // Segments of the warning message. Set at construction, and only read from the real-time path afterwards.
  segmented_sib_list<bcch_segment> segments;
  units::bytes                     max_seg_len{0};

  // Whether the next call must serve segment 0 unconditionally, ignoring is_repetition/nof_txs.
  bool force_segment_zero = true;
};

/// Repeat/count sequence of the PWS broadcasts of one SI message. See class doc in the header.
class si_message_controller::pws_broadcast_sequence
{
public:
  /// \param on_broadcast Invoked once per broadcast occurrence, including the first one.
  pws_broadcast_sequence(sib_type_set sib_set_, timer_factory timers_, std::function<void()> on_broadcast_) :
    sib_set(sib_set_), timers(timers_), on_broadcast(std::move(on_broadcast_))
  {
  }

  /// Encoder of the warning content currently being broadcast, or nullptr if no warning was ever pushed.
  std::shared_ptr<pws_si_msg_encoder> encoder() const { return current_encoder; }

  /// \brief Properties of the warning currently being broadcast, as the SI epoch states them.
  /// \remark Only valid once a warning was pushed.
  pws_broadcasting_si_message broadcasting_si_message() const
  {
    return pws_broadcasting_si_message{sib_set, nof_segments_per_broadcast, current_encoder->largest_segment_len()};
  }

  /// \brief Handles a new Write-Replace Warning content push, called from the control executor.
  ///
  /// A new warning for this SI message replaces any in-flight one outright, restarting its segment cycle.
  void handle_pws_broadcast(const mac_cell_sys_info_pdu_update& req)
  {
    timer.stop();
    current_encoder            = std::make_shared<pws_si_msg_encoder>(req.si_messages);
    nof_segments_per_broadcast = current_encoder->nof_segments();
    nof_broadcasts_remaining   = req.pws_broadcast->nof_broadcasts_requested;
    repeat_period              = req.pws_broadcast->repeat_period;

    start_one_broadcast();
  }

  /// \brief Broadcasts the given content indefinitely, without ever going back to dormant.
  ///
  /// Used to seed a test_mode-configured PWS SI-message at cell startup, broadcasting the configured ETWS/CMAS
  /// content instead of waiting for a real Write-Replace Warning. No repeat timer is involved -- the scheduler is
  /// asked to broadcast forever, so there is no need to ever re-signal it.
  void activate_forever(span<const byte_buffer> content)
  {
    current_encoder            = std::make_shared<pws_si_msg_encoder>(content);
    nof_segments_per_broadcast = std::nullopt;

    on_broadcast();
  }

private:
  /// Signals the scheduler for one broadcast, and arms the timer for the next one if any remain.
  void start_one_broadcast()
  {
    if (nof_broadcasts_remaining == 0) {
      return;
    }
    --nof_broadcasts_remaining;

    on_broadcast();

    if (nof_broadcasts_remaining == 0) {
      return;
    }

    if (not timer.is_valid()) {
      timer = timers.create_timer();
    }
    timer.set(std::chrono::duration_cast<timer_duration>(repeat_period), [this]() { start_one_broadcast(); });
    timer.run();
  }

  sib_type_set          sib_set;
  timer_factory         timers;
  std::function<void()> on_broadcast;

  std::shared_ptr<pws_si_msg_encoder> current_encoder;
  /// Number of segments of one broadcast. Empty when the content is broadcast indefinitely.
  std::optional<unsigned> nof_segments_per_broadcast;
  /// Number of remaining broadcasts (including the on-going/next one) for the current warning.
  unsigned nof_broadcasts_remaining = 0;
  /// Period between successive broadcasts.
  std::chrono::seconds repeat_period{0};
  /// Timer used to trigger successive broadcasts. Only running while nof_broadcasts_remaining > 0.
  unique_timer timer;
};

/// Hops from the RT path, where the end of a warning broadcast is detected, to the cell control context, where the
/// System Information of the cell is handled.
class si_message_controller::pws_broadcast_end_adapter final : public pws_broadcast_end_notifier
{
public:
  explicit pws_broadcast_end_adapter(si_message_controller& parent_) : parent(parent_) {}

  void on_pws_broadcast_end(si_version_type ended_version) override
  {
    if (not parent.ctrl_exec.defer([this, ended_version]() { parent.handle_pws_broadcast_end(ended_version); })) {
      parent.logger.warning("cell={}: Failed to notify the end of a warning broadcast", parent.cell_index);
    }
  }

private:
  si_message_controller& parent;
};

si_message_controller::si_message_controller(du_cell_index_t                 cell_index_,
                                             const mac_cell_sys_info_config& sys_info,
                                             timer_factory                   timers_,
                                             task_executor&                  ctrl_exec_,
                                             mac_dl_cell_controller&         dl_cell_) :
  logger(ocudulog::fetch_basic_logger("MAC")),
  cell_index(cell_index_),
  timers(timers_),
  ctrl_exec(ctrl_exec_),
  dl_cell(dl_cell_),
  ext_handler(create_si_message_extension_handler(sys_info))
{
  // Set up PWS broadcast sequences, one entry per SI message carrying PWS SIBs.
  const auto& si_sched_messages = sys_info.si_sched_cfg.si_messages;
  for (unsigned i = 0, e = sys_info.si_messages.size(); i != e; ++i) {
    if (i < si_sched_messages.size() and si_sched_messages[i].requires_activation()) {
      pws_sequences.emplace_back(
          si_sched_messages[i].sibs,
          std::make_unique<pws_broadcast_sequence>(
              si_sched_messages[i].sibs, timers, [this, sibs = si_sched_messages[i].sibs]() { push_pws_epoch(sibs); }));
    }
  }

  // Version starts at 0.
  last_cmd.version = 0;
  // A cell that cannot build its SIB1 encoder has nothing to broadcast.
  report_fatal_error_if_not(
      build_command(sys_info), "cell={}: Failed to build the initial System Information", fmt::underlying(cell_index));

  // Start broadcasting the System Information the cell was created with.
  dl_cell.start_broadcast(ext_handler, last_cmd, std::make_unique<pws_broadcast_end_adapter>(*this));

  for (unsigned i = 0, e = sys_info.si_messages.size(); i != e; ++i) {
    if (i >= si_sched_messages.size() or not si_sched_messages[i].test_mode_auto_broadcast) {
      continue;
    }
    // test_mode ETWS/CMAS config was set for this SI-message. Broadcast its (already encoded) content right away,
    // indefinitely, instead of waiting for a real Write-Replace Warning. The sequence pushes the epoch.
    find_pws_sequence(cell_si_sched_cfg, i)->activate_forever(sys_info.si_messages[i]);
  }
}

si_message_controller::~si_message_controller() = default;

void si_message_controller::handle_pws_broadcast_end(si_version_type ended_version)
{
  // The cell only goes back to the System Information of the normal operation once the last warning of the epoch ended,
  // so every warning that the epoch carried is over. A warning triggered by a later epoch is still on air.
  for (auto it = active_pws_si_msgs.begin(); it != active_pws_si_msgs.end();) {
    it = it->version <= ended_version ? active_pws_si_msgs.erase(it) : it + 1;
  }
}

si_message_controller::si_update_result
si_message_controller::handle_si_change_request(const std::optional<mac_cell_sys_info_config>&     new_sys_info,
                                                const std::optional<mac_cell_sys_info_pdu_update>& new_si_pdu_info)
{
  si_update_result result;

  if (new_sys_info.has_value()) {
    result.si_updated = push_si_epoch(*new_sys_info);
  }
  if (new_si_pdu_info.has_value()) {
    // Handle SI message updates that do not depend on the SI modification window (e.g. SIB19).
    result.si_pdus_enqueued = handle_si_message_pdu_updates(*new_si_pdu_info);
  }

  return result;
}

bool si_message_controller::push_si_epoch(const mac_cell_sys_info_config& req)
{
  if (not has_si_changed(req)) {
    logger.info("cell={}: Discarding SI update. Cause: The System Information did not change", cell_index);
    return false;
  }

  // Rebuild the encoders that changed and, only if that succeeded, bump the SI epoch.
  if (not build_command(req)) {
    return false;
  }
  last_cmd.version = ++last_version;
  dl_cell.handle_si_update(last_cmd);

  if (not active_pws_si_msgs.empty()) {
    // A warning is on air, so its epoch is the one being broadcast. Derive it again from the System Information that
    // just changed, otherwise it keeps serving a SIB1 that this update has superseded, with its old valueTag. No
    // warning is stamped with this epoch, so none of them is prolonged.
    push_pws_epoch(std::nullopt);
  }

  return true;
}

bool si_message_controller::has_si_changed(const mac_cell_sys_info_config& req) const
{
  // Note: In case the SIB1/SI message does not change, the comparison between the respective byte_buffers should be
  // fast (as they will point to the same memory location).
  if (req.sib1 != last_sib1 or req.sib1_contains_hypersfn != last_hypersfn_enabled) {
    return true;
  }
  if (req.si_messages.size() != last_si_messages.size()) {
    return true;
  }
  for (unsigned i = 0, e = req.si_messages.size(); i != e; ++i) {
    if (find_pws_sequence(req.si_sched_cfg, i) != nullptr) {
      // This SI message is exclusively managed by its PWS encoder, and its content does not flow through this
      // (possibly unrelated) SI reconfiguration.
      continue;
    }
    if (req.si_messages[i] != last_si_messages[i]) {
      return true;
    }
  }
  return req.si_sched_cfg != cell_si_sched_cfg;
}

bool si_message_controller::build_command(const mac_cell_sys_info_config& req)
{
  // Derive the SIB1 of the normal operation before any state is committed, so that a failure leaves the encoders and
  // the command of the previous epoch untouched.
  std::optional<byte_buffer> epoch_sib1 = make_epoch_sib1(req.sib1, req.sib1_contains_hypersfn, {});
  if (not epoch_sib1.has_value()) {
    logger.error("cell={}: Failed to generate the SIB1 of the normal operation", cell_index);
    return false;
  }
  const bool sib1_encoder_outdated = last_cmd.sib1 == nullptr or *epoch_sib1 != normal_epoch_sib1;

  last_sib1             = req.sib1.copy();
  last_hypersfn_enabled = req.sib1_contains_hypersfn;

  // Check if SI messages have changed.
  cell_si_msgs.resize(req.si_messages.size());
  last_si_messages.resize(req.si_messages.size());
  for (unsigned i = 0, e = req.si_messages.size(); i != e; ++i) {
    if (const pws_broadcast_sequence* pws_seq = find_pws_sequence(req.si_sched_cfg, i)) {
      // The content of this SI message flows through handle_pws_broadcast, not through this (possibly unrelated) SI
      // reconfiguration. Leave it untouched.
      cell_si_msgs[i] = pws_seq->encoder();
      continue;
    }

    if (req.si_messages[i] != last_si_messages[i]) {
      ocudu_assert(req.si_messages[i].size() == 1, "Static SI-messages must not be segmented");
      last_si_messages[i].resize(1);
      last_si_messages[i].front() = req.si_messages[i].front().copy();
      cell_si_msgs[i]             = std::make_shared<static_si_msg_encoder>(req.si_messages[i].front());
    }
  }

  cell_si_sched_cfg = req.si_sched_cfg;

  if (sib1_encoder_outdated) {
    normal_epoch_sib1 = std::move(*epoch_sib1);
    last_cmd.sib1     = make_sib1_encoder(normal_epoch_sib1);
  }
  fill_epoch_si_config(last_cmd, {}, units::bytes{static_cast<unsigned>(normal_epoch_sib1.length())});
  return true;
}

std::optional<byte_buffer> si_message_controller::make_epoch_sib1(const byte_buffer&       cell_sib1,
                                                                  bool                     hypersfn_enabled,
                                                                  span<const sib_type_set> on_air) const
{
  if (pws_sequences.empty()) {
    // A cell with no warning to broadcast has nothing to add to or remove from the SIB1 the DU packed.
    return cell_sib1.copy();
  }
  byte_buffer repacked = repack_sib1_si_sched_info(cell_sib1, on_air, hypersfn_enabled);
  if (repacked.empty()) {
    return std::nullopt;
  }
  return repacked;
}

std::shared_ptr<bcch_dl_sch_msg_encoder> si_message_controller::make_sib1_encoder(const byte_buffer& sib1) const
{
  if (last_hypersfn_enabled) {
    return std::make_shared<sib1_hypersfn_encoder>(sib1);
  }
  // eDRX not enabled, use static buffer.
  return std::make_shared<sib1_static_encoder>(sib1);
}

void si_message_controller::fill_epoch_si_config(si_update_command&       cmd,
                                                 span<const sib_type_set> on_air,
                                                 units::bytes             sib1_len) const
{
  cmd.si_sched_cfg                   = make_si_epoch_config(cell_si_sched_cfg, on_air);
  cmd.si_sched_cfg.sib1_payload_size = sib1_len;

  const span<const si_message_scheduling_config> epoch_si_msgs = cmd.si_sched_cfg.si_messages;
  cmd.si_msgs.resize(epoch_si_msgs.size());
  for (unsigned i = 0, e = epoch_si_msgs.size(); i != e; ++i) {
    cmd.si_msgs[i] = find_si_msg_encoder(epoch_si_msgs[i].sibs);
  }
}

std::shared_ptr<bcch_dl_sch_msg_encoder> si_message_controller::find_si_msg_encoder(sib_type_set sibs) const
{
  const auto& si_msgs = cell_si_sched_cfg.si_messages;
  const auto  it = std::find_if(si_msgs.begin(), si_msgs.end(), [sibs](const auto& cfg) { return cfg.sibs == sibs; });
  if (it == si_msgs.end()) {
    return nullptr;
  }
  const unsigned idx = std::distance(si_msgs.begin(), it);
  return idx < cell_si_msgs.size() ? cell_si_msgs[idx] : nullptr;
}

static_vector<sib_type_set, MAX_PWS_SI_MESSAGES> si_message_controller::on_air_sib_sets() const
{
  static_vector<sib_type_set, MAX_PWS_SI_MESSAGES> on_air;
  for (const pws_broadcasting_si_message& warning : active_pws_si_msgs) {
    on_air.push_back(warning.sib_set);
  }
  return on_air;
}

bool si_message_controller::handle_si_message_pdu_updates(const mac_cell_sys_info_pdu_update& req)
{
  if (req.pws_broadcast.has_value()) {
    return handle_pws_broadcast(req);
  }
  return ext_handler != nullptr and ext_handler->enqueue_si_pdu_updates(req);
}

si_message_controller::pws_broadcast_sequence*
si_message_controller::find_pws_sequence(const si_scheduling_config& si_sched_cfg, unsigned si_msg_idx) const
{
  if (si_msg_idx >= si_sched_cfg.si_messages.size()) {
    return nullptr;
  }
  return find_pws_sequence(si_sched_cfg.si_messages[si_msg_idx].sibs);
}

si_message_controller::pws_broadcast_sequence* si_message_controller::find_pws_sequence(sib_type_set sib_set) const
{
  auto it = std::find_if(
      pws_sequences.begin(), pws_sequences.end(), [sib_set](const auto& entry) { return entry.first == sib_set; });
  return it != pws_sequences.end() ? it->second.get() : nullptr;
}

bool si_message_controller::handle_pws_broadcast(const mac_cell_sys_info_pdu_update& req)
{
  pws_broadcast_sequence* pws_seq = find_pws_sequence(cell_si_sched_cfg, req.si_msg_idx);
  if (pws_seq == nullptr) {
    // The SI message carries no PWS SIB, so no PWS broadcast state was allocated for it.
    return false;
  }
  // The new content is broadcast from the epochs that carry its encoder, and the SI message starts being listed as
  // broadcasting in SIB1 for as long as the warning is on air. A replacement warning updates the properties of the one
  // it supersedes. The epoch itself is pushed by the sequence, once per broadcast.
  pws_seq->handle_pws_broadcast(req);

  return true;
}

void si_message_controller::push_pws_epoch(std::optional<sib_type_set> pws_sib_set)
{
  const si_version_type new_version = ++last_version;

  if (pws_sib_set.has_value()) {
    // One more broadcast of this warning is starting. Refresh its encoder and the properties the epoch states for it,
    // before deriving the epoch.
    const auto&    si_msgs    = cell_si_sched_cfg.si_messages;
    const unsigned si_msg_idx = std::distance(
        si_msgs.begin(),
        std::find_if(si_msgs.begin(), si_msgs.end(), [&](const auto& cfg) { return cfg.sibs == *pws_sib_set; }));
    ocudu_assert(si_msg_idx < si_msgs.size(), "Broadcasting a warning of an SI message that is not scheduled");

    pws_broadcast_sequence* pws_seq = find_pws_sequence(*pws_sib_set);
    cell_si_msgs[si_msg_idx]        = pws_seq->encoder();

    // Stamping it with the version of the epoch it triggers is what tells the scheduler to start one more broadcast of
    // this warning, and of this warning alone.
    pws_broadcasting_si_message active_si_msg = pws_seq->broadcasting_si_message();
    active_si_msg.version                     = new_version;

    auto entry = std::find_if(active_pws_si_msgs.begin(), active_pws_si_msgs.end(), [&](const auto& warning) {
      return warning.sib_set == active_si_msg.sib_set;
    });
    if (entry != active_pws_si_msgs.end()) {
      // The SI message for this SIB set was already broadcasting a warning. Update its properties to the new ones.
      *entry = active_si_msg;
    } else {
      // The SI message for this SIB set was not being broadcast. Activate it.
      active_pws_si_msgs.push_back(active_si_msg);
    }
  }

  const static_vector<sib_type_set, MAX_PWS_SI_MESSAGES> on_air = on_air_sib_sets();
  const std::optional<byte_buffer> pws_sib1 = make_epoch_sib1(last_sib1, last_hypersfn_enabled, on_air);
  if (not pws_sib1.has_value()) {
    logger.error("cell={}: Failed to generate the SIB1 of a warning broadcast", cell_index);
    return;
  }

  si_update_command cmd;
  cmd.version = new_version;
  cmd.sib1    = make_sib1_encoder(*pws_sib1);
  fill_epoch_si_config(cmd, on_air, units::bytes{static_cast<unsigned>(pws_sib1->length())});
  cmd.active_pws_si_messages = active_pws_si_msgs;

  // Forward SI update command to DL MAC and scheduler.
  dl_cell.handle_si_update(cmd);
}
