// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/mac/mac_dl/mac_cell_pcap_writer.h"
#include "ocudu/mac/mac_cell_result.h"
#include "ocudu/pcap/mac_pcap.h"
#include "ocudu/scheduler/result/sched_result.h"
#include <deque>
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

/// Records the PDUs pushed to the PCAP sink.
class mac_pcap_spy : public mac_pcap
{
public:
  void flush() override {}
  void close() override {}
  bool is_write_enabled() const override { return write_enabled; }
  void push_pdu(const mac_nr_context_info& context, const_span<uint8_t> pdu) override
  {
    contexts.push_back(context);
    payloads.emplace_back(pdu.begin(), pdu.end());
  }
  void push_pdu(const mac_nr_context_info& context, byte_buffer pdu) override
  {
    contexts.push_back(context);
    payloads.emplace_back(pdu.begin(), pdu.end());
  }

  bool                              write_enabled = true;
  std::vector<mac_nr_context_info>  contexts;
  std::vector<std::vector<uint8_t>> payloads;
};

class mac_cell_pcap_writer_test : public ::testing::Test
{
protected:
  static constexpr subcarrier_spacing scs          = subcarrier_spacing::kHz30;
  static constexpr unsigned           nof_dl_syms  = 14;
  static constexpr unsigned           pdu_size     = 8;
  static constexpr du_ue_index_t      ue_index     = to_du_ue_index(3);
  static constexpr harq_id_t          harq_id      = to_harq_id(5);
  static constexpr rnti_t             ue_rnti      = to_rnti(0x4601);
  static constexpr rnti_t             ra_rnti      = to_rnti(0x0002);
  static constexpr unsigned           si_msg_index = 1;
  static constexpr unsigned           nof_si_msgs  = 2;

  mac_cell_pcap_writer_test()
  {
    sched_res.dl.nof_dl_symbols = nof_dl_syms;
    si_messages.resize(nof_si_msgs);
    // An SI grant is matched against the SIBs it carries, so each SI message of the cell must carry a different one.
    si_messages[0].sibs = sib_type_set{sib_type::sib2};
    si_messages[1].sibs = sib_type_set{sib_type::sib3};
  }

  /// Instantiates the writer under test. Must be called after the SI message config is set up.
  void create_writer(bool is_tdd = true)
  {
    writer.emplace(pcap, is_tdd, span<const si_message_scheduling_config>{si_messages});
  }

  /// Appends an SIB1 or SI message grant and its PDU.
  void add_si_grant(sib_information::si_indicator_type si_indicator, unsigned version)
  {
    sib_information& sib = sched_res.dl.bc.sibs.emplace_back();
    sib.si_indicator     = si_indicator;
    sib.version          = version;
    sib.pdsch_cfg.rnti   = rnti_t::SI_RNTI;
    if (si_indicator == sib_information::other_si) {
      sib.si_msg_index = si_msg_index;
      sib.sibs         = si_messages[si_msg_index].sibs;
    }
    data_res.si_pdus.emplace_back(0, make_pdu());
  }

  void add_rar_grant()
  {
    rar_information& rar = sched_res.dl.rar_grants.emplace_back();
    rar.pdsch_cfg.rnti   = ra_rnti;
    data_res.rar_pdus.emplace_back(0, make_pdu());
  }

  void add_paging_grant()
  {
    dl_paging_allocation& pg = sched_res.dl.paging_grants.emplace_back();
    pg.pdsch_cfg.rnti        = rnti_t::P_RNTI;
    data_res.paging_pdus.emplace_back(0, make_pdu());
  }

  void add_ue_grant(bool new_data)
  {
    dl_msg_alloc& ue_grant                               = sched_res.dl.ue_grants.emplace_back();
    ue_grant.pdsch_cfg.rnti                              = ue_rnti;
    ue_grant.pdsch_cfg.harq_id                           = harq_id;
    ue_grant.context.ue_index                            = ue_index;
    ue_grant.pdsch_cfg.codewords.emplace_back().new_data = new_data;
    data_res.ue_pdus.emplace_back(0, make_pdu());
  }

  /// Clears the grants of the last written slot, so that a new slot can be filled in.
  void clear_grants()
  {
    sched_res.dl.bc.sibs.clear();
    sched_res.dl.rar_grants.clear();
    sched_res.dl.paging_grants.clear();
    sched_res.dl.ue_grants.clear();
    data_res.si_pdus.clear();
    data_res.rar_pdus.clear();
    data_res.paging_pdus.clear();
    data_res.ue_pdus.clear();
  }

  shared_transport_block make_pdu()
  {
    // The transport block only holds a view, so the payload has to outlive the writer call.
    auto& payload = pdu_storage.emplace_back(pdu_size, static_cast<uint8_t>(pdu_storage.size()));
    return shared_transport_block{payload};
  }

  mac_pcap_spy                                                 pcap;
  static_vector<si_message_scheduling_config, MAX_SI_MESSAGES> si_messages;
  std::optional<mac_cell_pcap_writer>                          writer;
  sched_result                                                 sched_res;
  mac_dl_data_result                                           data_res;
  std::deque<std::vector<uint8_t>>                             pdu_storage;
  slot_point                                                   sl_tx{scs, 10};
};

} // namespace

TEST_F(mac_cell_pcap_writer_test, when_pcap_write_is_disabled_then_no_pdu_is_written)
{
  pcap.write_enabled = false;
  create_writer();
  add_ue_grant(true);

  writer->write_slot_result(sl_tx, sched_res, data_res);

  ASSERT_TRUE(pcap.contexts.empty());
}

TEST_F(mac_cell_pcap_writer_test, when_slot_has_no_dl_symbols_then_no_pdu_is_written)
{
  create_writer();
  sched_res.dl.nof_dl_symbols = 0;
  add_ue_grant(true);

  writer->write_slot_result(sl_tx, sched_res, data_res);

  ASSERT_TRUE(pcap.contexts.empty());
}

TEST_F(mac_cell_pcap_writer_test, when_ue_grant_is_new_data_then_pdu_is_written_with_c_rnti_context)
{
  create_writer();
  add_ue_grant(true);

  writer->write_slot_result(sl_tx, sched_res, data_res);

  ASSERT_EQ(pcap.contexts.size(), 1);
  const mac_nr_context_info& context = pcap.contexts[0];
  ASSERT_EQ(context.radioType, PCAP_TDD_RADIO);
  ASSERT_EQ(context.direction, PCAP_DIRECTION_DOWNLINK);
  ASSERT_EQ(context.rntiType, PCAP_C_RNTI);
  ASSERT_EQ(context.rnti, to_underlying(ue_rnti));
  ASSERT_EQ(context.ueid, ue_index + 1);
  ASSERT_EQ(context.harqid, harq_id);
  ASSERT_EQ(context.system_frame_number, sl_tx.sfn());
  ASSERT_EQ(context.sub_frame_number, sl_tx.subframe_index());
  ASSERT_EQ(context.length, pdu_size);
  ASSERT_EQ(pcap.payloads[0], pdu_storage[0]);
}

TEST_F(mac_cell_pcap_writer_test, when_ue_grant_is_retx_then_no_pdu_is_written)
{
  create_writer();
  add_ue_grant(false);

  writer->write_slot_result(sl_tx, sched_res, data_res);

  ASSERT_TRUE(pcap.contexts.empty());
}

TEST_F(mac_cell_pcap_writer_test, when_cell_is_fdd_then_radio_type_is_fdd)
{
  create_writer(false);
  add_ue_grant(true);

  writer->write_slot_result(sl_tx, sched_res, data_res);

  ASSERT_EQ(pcap.contexts.size(), 1);
  ASSERT_EQ(pcap.contexts[0].radioType, PCAP_FDD_RADIO);
}

TEST_F(mac_cell_pcap_writer_test, when_rar_is_scheduled_then_pdu_is_written_with_ra_rnti_context)
{
  create_writer();
  add_rar_grant();

  writer->write_slot_result(sl_tx, sched_res, data_res);

  ASSERT_EQ(pcap.contexts.size(), 1);
  ASSERT_EQ(pcap.contexts[0].rntiType, PCAP_RA_RNTI);
  ASSERT_EQ(pcap.contexts[0].rnti, to_underlying(ra_rnti));
  ASSERT_EQ(pcap.payloads[0], pdu_storage[0]);
}

TEST_F(mac_cell_pcap_writer_test, when_paging_is_scheduled_then_pdu_is_written_with_p_rnti_context)
{
  create_writer();
  add_paging_grant();

  writer->write_slot_result(sl_tx, sched_res, data_res);

  ASSERT_EQ(pcap.contexts.size(), 1);
  ASSERT_EQ(pcap.contexts[0].rntiType, PCAP_P_RNTI);
  ASSERT_EQ(pcap.contexts[0].rnti, to_underlying(rnti_t::P_RNTI));
  ASSERT_EQ(pcap.payloads[0], pdu_storage[0]);
}

TEST_F(mac_cell_pcap_writer_test, when_sib1_is_scheduled_then_pdu_is_written_with_si_rnti_context)
{
  create_writer();
  add_si_grant(sib_information::sib1, 0);

  writer->write_slot_result(sl_tx, sched_res, data_res);

  ASSERT_EQ(pcap.contexts.size(), 1);
  ASSERT_EQ(pcap.contexts[0].rntiType, PCAP_SI_RNTI);
  ASSERT_EQ(pcap.contexts[0].rnti, to_underlying(rnti_t::SI_RNTI));
  ASSERT_EQ(pcap.payloads[0], pdu_storage[0]);
}

TEST_F(mac_cell_pcap_writer_test, when_sib1_version_does_not_change_then_pdu_is_written_only_once)
{
  create_writer();
  for (unsigned i = 0; i != 3; ++i) {
    clear_grants();
    add_si_grant(sib_information::sib1, 0);
    writer->write_slot_result(sl_tx, sched_res, data_res);
  }

  ASSERT_EQ(pcap.contexts.size(), 1);
}

TEST_F(mac_cell_pcap_writer_test, when_sib1_version_changes_then_pdu_is_written_again)
{
  create_writer();
  for (unsigned version = 0; version != 3; ++version) {
    clear_grants();
    add_si_grant(sib_information::sib1, version);
    writer->write_slot_result(sl_tx, sched_res, data_res);
  }

  ASSERT_EQ(pcap.contexts.size(), 3);
}

TEST_F(mac_cell_pcap_writer_test, when_si_message_does_not_require_activation_then_repeated_version_is_deduped)
{
  si_messages[si_msg_index].sibs = sib_type_set{sib_type::sib3};
  create_writer();
  for (unsigned i = 0; i != 3; ++i) {
    clear_grants();
    add_si_grant(sib_information::other_si, 0);
    writer->write_slot_result(sl_tx, sched_res, data_res);
  }

  ASSERT_EQ(pcap.contexts.size(), 1);
}

TEST_F(mac_cell_pcap_writer_test, when_si_message_requires_activation_then_repeated_version_is_written)
{
  si_messages[si_msg_index].sibs = sib_type_set{sib_type::sib7};
  create_writer();
  for (unsigned i = 0; i != 3; ++i) {
    clear_grants();
    add_si_grant(sib_information::other_si, 0);
    writer->write_slot_result(sl_tx, sched_res, data_res);
  }

  ASSERT_EQ(pcap.contexts.size(), 3);
}

TEST_F(mac_cell_pcap_writer_test, when_a_warning_is_scheduled_then_sib1_stays_deduped)
{
  si_messages[si_msg_index].sibs = sib_type_set{sib_type::sib7};
  create_writer();
  add_si_grant(sib_information::sib1, 0);
  writer->write_slot_result(sl_tx, sched_res, data_res);
  ASSERT_EQ(pcap.contexts.size(), 1);

  // The warning bypasses the version-based dedup, which must leave the dedup state of SIB1 alone. Its version differs
  // from the one of SIB1, so tracking it against the SIB1 state would make every SIB1 occasion look like a new one.
  for (unsigned i = 0; i != 2; ++i) {
    clear_grants();
    add_si_grant(sib_information::other_si, 5);
    add_si_grant(sib_information::sib1, 0);
    writer->write_slot_result(sl_tx, sched_res, data_res);
  }

  ASSERT_EQ(pcap.contexts.size(), 3) << "An unchanged SIB1 must stay deduped while a warning is on air";
}

TEST_F(mac_cell_pcap_writer_test, when_sib1_and_si_message_are_scheduled_then_dedup_state_is_independent)
{
  create_writer();
  add_si_grant(sib_information::sib1, 0);
  add_si_grant(sib_information::other_si, 0);
  writer->write_slot_result(sl_tx, sched_res, data_res);
  ASSERT_EQ(pcap.contexts.size(), 2);

  // Only the SI message content changed, so SIB1 must stay deduped.
  clear_grants();
  add_si_grant(sib_information::sib1, 0);
  add_si_grant(sib_information::other_si, 1);
  writer->write_slot_result(sl_tx, sched_res, data_res);

  ASSERT_EQ(pcap.contexts.size(), 3);
  ASSERT_EQ(pcap.payloads[2], pdu_storage[3]);
}

TEST_F(mac_cell_pcap_writer_test, when_all_grant_types_are_scheduled_then_all_pdus_are_written)
{
  create_writer();
  add_si_grant(sib_information::sib1, 0);
  add_rar_grant();
  add_paging_grant();
  add_ue_grant(true);

  writer->write_slot_result(sl_tx, sched_res, data_res);

  ASSERT_EQ(pcap.contexts.size(), 4);
  ASSERT_EQ(pcap.contexts[0].rntiType, PCAP_SI_RNTI);
  ASSERT_EQ(pcap.contexts[1].rntiType, PCAP_RA_RNTI);
  ASSERT_EQ(pcap.contexts[2].rntiType, PCAP_P_RNTI);
  ASSERT_EQ(pcap.contexts[3].rntiType, PCAP_C_RNTI);
}
