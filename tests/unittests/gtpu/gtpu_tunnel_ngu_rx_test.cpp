// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/gtpu/gtpu_pdu.h"
#include "lib/gtpu/gtpu_tunnel_ngu_rx_impl.h"
#include "lib/gtpu/gtpu_tunnel_ngu_tx_impl.h"
#include "ocudu/support/bit_encoding.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/rate_limiting/token_bucket.h"
#include "ocudu/support/rate_limiting/token_bucket_config.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>
#include <sys/socket.h>

using namespace ocudu;

static ocudu::log_sink_spy& test_spy = []() -> ocudu::log_sink_spy& {
  if (!ocudulog::install_custom_sink(
          ocudu::log_sink_spy::name(),
          std::unique_ptr<ocudu::log_sink_spy>(new ocudu::log_sink_spy(ocudulog::get_default_log_formatter())))) {
    report_fatal_error("Unable to create logger spy");
  }
  auto* spy = static_cast<ocudu::log_sink_spy*>(ocudulog::find_sink(ocudu::log_sink_spy::name()));
  if (spy == nullptr) {
    report_fatal_error("Unable to create logger spy");
  }

  ocudulog::fetch_basic_logger("GTPU", *spy, true);
  return *spy;
}();

class gtpu_pdu_generator
{
  class gtpu_tunnel_tx_upper_dummy : public gtpu_tunnel_common_tx_upper_layer_notifier
  {
    void on_new_pdu(byte_buffer buf, const ::sockaddr_storage& dest_addr) final { parent.gen_pdu = std::move(buf); }
    gtpu_pdu_generator& parent;

  public:
    gtpu_tunnel_tx_upper_dummy(gtpu_pdu_generator& parent_) : parent(parent_) {}
  };

public:
  gtpu_pdu_generator(gtpu_teid_t teid) : tx_upper_dummy(*this)
  {
    gtpu_tunnel_ngu_config::gtpu_tunnel_ngu_tx_config cfg = {};
    cfg.peer_teid                                         = teid;
    cfg.peer_addr                                         = "127.0.0.1";

    tx = std::make_unique<gtpu_tunnel_ngu_tx_impl>(
        cu_up_ue_index_t::MIN_CU_UP_UE_INDEX, cfg, dummy_pcap, tx_upper_dummy);
  }

  byte_buffer create_gtpu_pdu(byte_buffer buf, gtpu_teid_t teid, qos_flow_id_t flow_id, std::optional<uint16_t> sn)
  {
    gtpu_header hdr         = {};
    hdr.flags.version       = GTPU_FLAGS_VERSION_V1;
    hdr.flags.protocol_type = GTPU_FLAGS_GTP_PROTOCOL;
    hdr.flags.seq_number    = sn.has_value();
    hdr.flags.ext_hdr       = true;
    hdr.message_type        = GTPU_MSG_DATA_PDU;
    hdr.length              = buf.length() + 4 + 4;
    hdr.teid                = teid;
    hdr.next_ext_hdr_type   = gtpu_extension_header_type::pdu_session_container;

    // Put PDU session container
    byte_buffer ext_buf;
    bit_encoder encoder{ext_buf};
    encoder.pack(0, 4);                      // PDU type
    encoder.pack(0, 4);                      // unused options
    encoder.pack(0, 1);                      // Paging Policy Presence
    encoder.pack(0, 1);                      // Reflective QoS Indicator
    encoder.pack(to_underlying(flow_id), 6); // QFI

    gtpu_extension_header ext;
    ext.extension_header_type = gtpu_extension_header_type::pdu_session_container;
    ext.container             = ext_buf;

    hdr.ext_list.push_back(ext);

    if (sn.has_value()) {
      hdr.flags.seq_number = true;
      hdr.seq_number       = sn.value();
    }

    bool write_ok = gtpu_write_header(buf, hdr, gtpu_logger);

    if (!write_ok) {
      gtpu_logger.log_error("Dropped SDU, error writing GTP-U header. teid={}", hdr.teid);
      return {};
    }

    return buf;
  }

private:
  null_dlt_pcap                            dummy_pcap = {};
  gtpu_tunnel_tx_upper_dummy               tx_upper_dummy;
  std::unique_ptr<gtpu_tunnel_ngu_tx_impl> tx;
  byte_buffer                              gen_pdu;
  gtpu_tunnel_logger                       gtpu_logger{"GTPU", {{}, gtpu_teid_t{1}, "DL"}};

public:
};

class gtpu_tunnel_rx_lower_dummy : public gtpu_tunnel_ngu_rx_lower_layer_notifier
{
  void on_new_sdu(byte_buffer sdu, qos_flow_id_t qos_flow_id) final
  {
    rx_sdus.push_back(std::move(sdu));
    rx_qfis.push_back(qos_flow_id);
  }

public:
  void clear()
  {
    rx_sdus.clear();
    rx_qfis.clear();
  }

  std::vector<byte_buffer>   rx_sdus;
  std::vector<qos_flow_id_t> rx_qfis;
};

class gtpu_tunnel_rx_upper_dummy : public gtpu_tunnel_common_rx_upper_layer_interface
{
public:
  void handle_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) final
  {
    last_rx   = std::move(pdu);
    last_addr = src_addr;
  }

  byte_buffer      last_rx;
  sockaddr_storage last_addr = {};
};

/// Fixture base class for GTP-U tunnel NG-U Rx tests
class gtpu_tunnel_ngu_rx_test : public ::testing::Test
{
public:
  gtpu_tunnel_ngu_rx_test() :
    logger(ocudulog::fetch_basic_logger("TEST", false)), gtpu_logger(ocudulog::fetch_basic_logger("GTPU", false))
  {
  }

protected:
  void SetUp() override
  {
    // init test's logger
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);

    // reset log spy
    test_spy.reset_counters();

    // init GTP-U logger
    gtpu_logger.set_level(ocudulog::basic_levels::debug);
    gtpu_logger.set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test
    rx_lower.clear();
    ocudulog::flush();
  }

  void create_gtpu_rx_entity(bool warn_on_drop)
  {
    uint64_t            ue_ambr = 1000000000;
    token_bucket_config ue_ambr_cfg =
        generate_token_bucket_config(ue_ambr, ue_ambr, std::chrono::milliseconds(1), timers);
    ue_ambr_limiter = std::make_unique<token_bucket>(ue_ambr_cfg);

    // create Rx entity
    gtpu_tunnel_ngu_config::gtpu_tunnel_ngu_rx_config rx_cfg = {};
    rx_cfg.local_teid                                        = local_teid;
    rx_cfg.ue_ambr_limiter                                   = ue_ambr_limiter.get();
    rx_cfg.t_reordering                                      = std::chrono::milliseconds{10};
    rx_cfg.warn_on_drop                                      = warn_on_drop;

    rx = std::make_unique<gtpu_tunnel_ngu_rx_impl>(cu_up_ue_index_t::MIN_CU_UP_UE_INDEX, rx_cfg, rx_lower, timers);
  }

  /// \brief Helper to advance the timers
  /// \param nof_tick Number of ticks to advance timers
  void tick_all(uint32_t nof_ticks)
  {
    for (uint32_t i = 0; i < nof_ticks; i++) {
      timers_manager.tick();
      worker.run_pending_tasks();
    }
  }

  gtpu_pdu_generator pdu_generator{gtpu_teid_t{0x1}};

  // Test logger
  ocudulog::basic_logger& logger;

  // GTP-U logger
  ocudulog::basic_logger& gtpu_logger;
  gtpu_tunnel_logger      gtpu_rx_logger{"GTPU", {{}, gtpu_teid_t{1}, "DL"}};

  // Timers
  manual_task_worker worker{64};
  timer_manager      timers_manager;
  timer_factory      timers{timers_manager, worker};

  // GTP-U tunnel Rx entity
  std::unique_ptr<token_bucket>            ue_ambr_limiter;
  std::unique_ptr<gtpu_tunnel_ngu_rx_impl> rx;

  // Surrounding tester
  gtpu_tunnel_rx_lower_dummy rx_lower = {};

  gtpu_teid_t local_teid{0x1};
};

/// Fixture class for GTP-U tunnel NG-U Rx tests (different configs)
class gtpu_tunnel_ngu_rx_test_cfg : public gtpu_tunnel_ngu_rx_test, public ::testing::WithParamInterface<bool>
{
public:
  gtpu_tunnel_ngu_rx_test_cfg() {}
};

/// Fixture class for GTP-U tunnel NG-U Rx tests (different configs, different start SN)
class gtpu_tunnel_ngu_rx_test_cfg_sn : public gtpu_tunnel_ngu_rx_test,
                                       public ::testing::WithParamInterface<std::tuple<bool, uint16_t>>
{
public:
  gtpu_tunnel_ngu_rx_test_cfg_sn() {}
};

/// \brief Test correct creation of Rx entity
TEST_P(gtpu_tunnel_ngu_rx_test_cfg, entity_creation)
{
  create_gtpu_rx_entity(false);
  ASSERT_NE(rx, nullptr);

  // No warnings or errors during construction
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

/// \brief Test reception of PDUs with no SN
TEST_P(gtpu_tunnel_ngu_rx_test_cfg_sn, rx_no_sn)
{
  bool warn_on_drop = std::get<bool>(GetParam());
  create_gtpu_rx_entity(warn_on_drop);
  ASSERT_NE(rx, nullptr);

  sockaddr_storage src_addr;

  for (unsigned i = 0; i < 3; i++) {
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x11));
    byte_buffer pdu = pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, {});
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_EQ(rx_lower.rx_qfis[i], qos_flow_id_t::min);
    EXPECT_EQ(rx_lower.rx_sdus[i], sdu);
  }

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

/// \brief Test in-order reception of PDUs
TEST_P(gtpu_tunnel_ngu_rx_test_cfg_sn, rx_in_order)
{
  bool     warn_on_drop = std::get<bool>(GetParam());
  uint16_t start_sn     = std::get<uint16_t>(GetParam());
  create_gtpu_rx_entity(warn_on_drop);
  ASSERT_NE(rx, nullptr);

  sockaddr_storage src_addr;

  for (unsigned i = 0; i < 3; i++) {
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x11));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + i);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_EQ(rx_lower.rx_qfis[i], qos_flow_id_t::min);
    EXPECT_EQ(rx_lower.rx_sdus[i], sdu);
  }

  // Check warnings or errors
  unsigned warnings = warn_on_drop && start_sn != 0 ? 1 : 0;
  EXPECT_EQ(test_spy.get_warning_counter(), warnings);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

/// \brief Test out-of-order reception of PDUs
TEST_P(gtpu_tunnel_ngu_rx_test_cfg_sn, rx_out_of_order)
{
  bool     warn_on_drop = std::get<bool>(GetParam());
  uint16_t start_sn     = std::get<uint16_t>(GetParam());
  create_gtpu_rx_entity(warn_on_drop);
  ASSERT_NE(rx, nullptr);

  sockaddr_storage src_addr;

  { // SN = 0
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x0));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 0);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_EQ(rx_lower.rx_sdus.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis[0], qos_flow_id_t::min);
    EXPECT_EQ(rx_lower.rx_sdus[0], sdu);
  }

  { // SN = 2
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x2));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 2);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_TRUE(rx->is_reordering_timer_running());
    EXPECT_EQ(rx_lower.rx_sdus.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 1); // nothing was received
  }

  { // SN = 4
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x4));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 4);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_EQ(rx_lower.rx_sdus.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 1); // nothing was received
  }

  { // SN = 1
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x1));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 1);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_TRUE(rx->is_reordering_timer_running()); // it should have been restarted.
    EXPECT_EQ(rx_lower.rx_sdus.size(), 3);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 3); // all up to sn=2 was delivered.
  }

  { // SN = 3
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x3));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 3);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_FALSE(rx->is_reordering_timer_running()); // nothing out of order, timer must have been stopped.
    EXPECT_EQ(rx_lower.rx_sdus.size(), 5);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 5); // all was received
  }

  // Check warnings or errors
  unsigned warnings = warn_on_drop && start_sn != 0 ? 1 : 0;
  EXPECT_EQ(test_spy.get_warning_counter(), warnings);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

/// \brief Test out-of-order reception of PDUs
/// When there are two holes and they gets filled out-of-order
/// t-Reordering is stopped correctly
TEST_P(gtpu_tunnel_ngu_rx_test_cfg_sn, rx_out_of_order_two_holes)
{
  bool     warn_on_drop = std::get<bool>(GetParam());
  uint16_t start_sn     = std::get<uint16_t>(GetParam());
  create_gtpu_rx_entity(warn_on_drop);
  ASSERT_NE(rx, nullptr);

  sockaddr_storage src_addr;

  { // SN = 0
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x0));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 0);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_EQ(rx_lower.rx_sdus.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis[0], qos_flow_id_t::min);
    EXPECT_EQ(rx_lower.rx_sdus[0], sdu);
  }

  { // SN = 2
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x2));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 2);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_TRUE(rx->is_reordering_timer_running());
    EXPECT_EQ(rx_lower.rx_sdus.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 1); // nothing was received
  }

  { // SN = 4
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x4));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 4);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_EQ(rx_lower.rx_sdus.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 1); // nothing was received
  }

  { // SN = 3
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x3));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 3);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_TRUE(rx->is_reordering_timer_running()); // one hole left, timer still running
    EXPECT_EQ(rx_lower.rx_sdus.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 1); // nothing was received
  }

  { // SN = 1
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x1));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 1);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_FALSE(rx->is_reordering_timer_running()); // it should have been stopped
    EXPECT_EQ(rx_lower.rx_sdus.size(), 5);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 5); // all PDUs were delivered.
  }

  // Check warnings or errors
  unsigned warnings = warn_on_drop && start_sn != 0 ? 1 : 0;
  EXPECT_EQ(test_spy.get_warning_counter(), warnings);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

/// \brief Test t-Reordering expiration
TEST_P(gtpu_tunnel_ngu_rx_test_cfg_sn, rx_t_reordering_expiration)
{
  bool     warn_on_drop = std::get<bool>(GetParam());
  uint16_t start_sn     = std::get<uint16_t>(GetParam());
  create_gtpu_rx_entity(warn_on_drop);
  ASSERT_NE(rx, nullptr);

  sockaddr_storage src_addr;

  { // SN = 0
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x0));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 0);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_EQ(rx_lower.rx_sdus.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis[0], qos_flow_id_t::min);
    EXPECT_EQ(rx_lower.rx_sdus[0], sdu);
  }

  { // SN = 2
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x2));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 2);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_EQ(rx_lower.rx_sdus.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 1); // nothing was received
  }

  { // SN = 4
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x4));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 4);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_EQ(rx_lower.rx_sdus.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 1); // nothing was received
  }

  tick_all(10);

  EXPECT_EQ(rx_lower.rx_sdus.size(), 2);
  EXPECT_EQ(rx_lower.rx_qfis.size(), 2); // reordering timer was triggered and 0 and 2 were received.

  tick_all(10);

  EXPECT_EQ(rx_lower.rx_sdus.size(), 3);
  EXPECT_EQ(rx_lower.rx_qfis.size(), 3); // reordering timer was triggered and 0, 2 and 4 were received.

  { // SN = 1
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x1));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 1);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_EQ(rx_lower.rx_sdus.size(), 4);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 4); // all was received
  }

  // Check warnings or errors
  unsigned warnings = 1 + (warn_on_drop ? 2 : 0) + (warn_on_drop && start_sn != 0 ? 1 : 0);
  EXPECT_EQ(test_spy.get_warning_counter(), warnings);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

/// \brief Test t-Reordering expiration
/// When there are two holes and the second one gets filled before
/// t-Reordering expires, timer is not restarted.
TEST_P(gtpu_tunnel_ngu_rx_test_cfg_sn, rx_t_reordering_two_holes)
{
  bool     warn_on_drop = std::get<bool>(GetParam());
  uint16_t start_sn     = std::get<uint16_t>(GetParam());
  create_gtpu_rx_entity(warn_on_drop);
  ASSERT_NE(rx, nullptr);

  sockaddr_storage src_addr;

  { // SN = 0
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x0));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 0);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_EQ(rx_lower.rx_sdus.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis[0], qos_flow_id_t::min);
    EXPECT_EQ(rx_lower.rx_sdus[0], sdu);
  }

  { // SN = 2
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x2));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 2);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_EQ(rx_lower.rx_sdus.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 1); // nothing was received
  }

  { // SN = 4
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x4));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 4);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_EQ(rx_lower.rx_sdus.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 1); // nothing was received
  }

  { // SN = 3
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x3));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 3);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_EQ(rx_lower.rx_sdus.size(), 1);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 1); // nothing was received
  }

  tick_all(10);

  EXPECT_EQ(rx_lower.rx_sdus.size(), 4);
  EXPECT_EQ(rx_lower.rx_qfis.size(), 4); // reordering timer was triggered and 0, 2, 3, 4 were received.
  EXPECT_FALSE(rx->is_reordering_timer_running());

  { // SN = 1
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x1));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 1);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_EQ(rx_lower.rx_sdus.size(), 5);
    EXPECT_EQ(rx_lower.rx_qfis.size(), 5); // all was received
  }

  // Check warnings or errors
  unsigned warnings = (warn_on_drop ? 2 : 1) + (warn_on_drop && start_sn != 0 ? 1 : 0);
  EXPECT_EQ(test_spy.get_warning_counter(), warnings);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

/// \brief Test in-order reception of PDUs
TEST_P(gtpu_tunnel_ngu_rx_test_cfg_sn, rx_stop)
{
  bool     warn_on_drop = std::get<bool>(GetParam());
  uint16_t start_sn     = std::get<uint16_t>(GetParam());
  create_gtpu_rx_entity(warn_on_drop);
  ASSERT_NE(rx, nullptr);

  sockaddr_storage src_addr;

  for (unsigned i = 0; i < 3; i++) {
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x11));
    byte_buffer pdu =
        pdu_generator.create_gtpu_pdu(sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + i);
    gtpu_tunnel_base_rx* rx_base = rx.get();
    if (i != 1) {
      rx_base->handle_pdu(std::move(pdu), src_addr);
    }
  }

  EXPECT_EQ(rx_lower.rx_sdus.size(), 1);
  EXPECT_TRUE(rx->is_reordering_timer_running());

  // Stop RX interface
  rx->stop();
  rx_lower.rx_sdus.clear();
  rx_lower.rx_qfis.clear();

  // Timers should have been stopped
  EXPECT_FALSE(rx->is_reordering_timer_running());

  // No more PDUs should flow
  {
    byte_buffer sdu;
    EXPECT_TRUE(sdu.append(0x11));
    byte_buffer pdu = pdu_generator.create_gtpu_pdu(
        sdu.deep_copy().value(), local_teid, qos_flow_id_t::min, start_sn + 1); // push missing pdu
    gtpu_tunnel_base_rx* rx_base = rx.get();
    rx_base->handle_pdu(std::move(pdu), src_addr);

    EXPECT_TRUE(rx_lower.rx_qfis.empty());
    EXPECT_TRUE(rx_lower.rx_sdus.empty());
  }

  // Check warnings or errors
  unsigned warnings = warn_on_drop && start_sn != 0 ? 1 : 0;
  EXPECT_EQ(test_spy.get_warning_counter(), warnings);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

///////////////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each supported SN size //
///////////////////////////////////////////////////////////////////
std::string cfg_test_param_info_to_string(const ::testing::TestParamInfo<bool>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "warn_on_drop_{}", info.param);
  return fmt::to_string(buffer);
}

std::string cfg_sn_test_param_info_to_string(const ::testing::TestParamInfo<std::tuple<bool, uint16_t>>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer),
                 "warn_on_drop_{}_start_sn_{}",
                 std::get<bool>(info.param),
                 std::get<uint16_t>(info.param));
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(ngu_rx_cfg,
                         gtpu_tunnel_ngu_rx_test_cfg,
                         ::testing::Values(false, true),
                         cfg_test_param_info_to_string);

INSTANTIATE_TEST_SUITE_P(ngu_rx_cfg_sn,
                         gtpu_tunnel_ngu_rx_test_cfg_sn,
                         ::testing::Combine(::testing::Values(false, true),
                                            ::testing::Values(0, 1, 17000, 33000, 65535)),
                         cfg_sn_test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
