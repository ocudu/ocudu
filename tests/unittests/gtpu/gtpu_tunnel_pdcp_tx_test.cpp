// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/gtpu/gtpu_pdu.h"
#include "lib/gtpu/gtpu_tunnel_pdcp_tx_impl.h"
#include "tests/unittests/gtpu/gtpu_test_shared.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/io/sockets.h"
#include <gtest/gtest.h>
#include <sys/socket.h>

using namespace ocudu;

namespace {

class gtpu_tunnel_tx_upper_dummy : public gtpu_tunnel_common_tx_upper_layer_notifier
{
public:
  void on_new_pdu(byte_buffer gpdu, const ::sockaddr_storage& dest_addr) final
  {
    tx_ul_gpdus.push_back(gpdu);
    last_dest_addr = dest_addr;
  }

  void clear()
  {
    tx_ul_gpdus.clear();
    last_dest_addr = {};
  }

  std::vector<byte_buffer> tx_ul_gpdus;
  ::sockaddr_storage       last_dest_addr = {};
};

/// Fixture class for GTP-U tunnel PDCP Tx tests.
class gtpu_tunnel_pdcp_tx_test : public ::testing::Test
{
public:
  gtpu_tunnel_pdcp_tx_test() :
    logger(ocudulog::fetch_basic_logger("TEST", false)), gtpu_logger(ocudulog::fetch_basic_logger("GTPU", false))
  {
  }

protected:
  void SetUp() override
  {
    // init test's logger.
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);

    // init GTP-U logger.
    gtpu_logger.set_level(ocudulog::basic_levels::debug);
    gtpu_logger.set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test.
    tx_upper.clear();
    ocudulog::flush();
  }

  /// \brief Helper to advance the timers.
  /// \param nof_tick Number of ticks to advance timers.
  void tick_all(uint32_t nof_ticks)
  {
    for (uint32_t i = 0; i < nof_ticks; i++) {
      timers_manager.tick();
      worker.run_pending_tasks();
    }
  }

  // Test logger.
  ocudulog::basic_logger& logger;

  // GTP-U logger.
  ocudulog::basic_logger& gtpu_logger;

  // Timers.
  manual_task_worker worker{64};
  timer_manager      timers_manager;
  timer_factory      timers{timers_manager, worker};

  // GTP-U tunnel Tx entity.
  std::unique_ptr<gtpu_tunnel_pdcp_tx_impl> tx;

  // Surrounding tester.
  gtpu_tunnel_tx_upper_dummy tx_upper = {};

  null_dlt_pcap dummy_pcap;
};

/// \brief Test correct creation of Tx entity.
TEST_F(gtpu_tunnel_pdcp_tx_test, entity_creation)
{
  // create Tx entity.
  gtpu_tunnel_pdcp_config::gtpu_tunnel_pdcp_tx_config tx_cfg = {};
  tx_cfg.lif                                                 = gtpu_logical_interface::xnu;
  tx_cfg.peer_addr                                           = "127.0.0.1";
  tx_cfg.peer_teid                                           = gtpu_teid_t{0x1};
  tx_cfg.pdcp_sn_len                                         = pdcp_sn_size::size12bits;

  tx = std::make_unique<gtpu_tunnel_pdcp_tx_impl>(cu_up_ue_index_t::MIN_CU_UP_UE_INDEX, tx_cfg, dummy_pcap, tx_upper);

  ASSERT_NE(tx, nullptr);
}

/// \brief Test transmission of T-PDUs with no GTP-U SN.
TEST_F(gtpu_tunnel_pdcp_tx_test, tx_tpdus)
{
  // create Tx entity.
  gtpu_tunnel_pdcp_config::gtpu_tunnel_pdcp_tx_config tx_cfg = {};
  tx_cfg.lif                                                 = gtpu_logical_interface::xnu;
  tx_cfg.peer_addr                                           = "127.0.0.1";
  tx_cfg.peer_teid                                           = gtpu_teid_t{0x2};
  tx_cfg.pdcp_sn_len                                         = pdcp_sn_size::size12bits;

  tx = std::make_unique<gtpu_tunnel_pdcp_tx_impl>(cu_up_ue_index_t::MIN_CU_UP_UE_INDEX, tx_cfg, dummy_pcap, tx_upper);
  ASSERT_NE(tx, nullptr);

  for (unsigned i = 0; i < 3; i++) {
    byte_buffer tpdu = byte_buffer::create(tpdu_1).value();
    byte_buffer gpdu;
    switch (i) {
      case 0:
        gpdu = byte_buffer::create(gpdu_tpdu_1_teid_1_pdcp_sn_0).value();
        break;
      case 1:
        gpdu = byte_buffer::create(gpdu_tpdu_1_teid_1_pdcp_sn_1).value();
        break;
      case 2:
        gpdu = byte_buffer::create(gpdu_tpdu_1_teid_1_pdcp_sn_2).value();
        break;
      default:
        break;
    }

    tx->handle_sdu(std::move(tpdu), i);
    ASSERT_EQ(gpdu, tx_upper.tx_ul_gpdus[i]);
    gtpu_teid_t teid_out = {};
    ASSERT_TRUE(gtpu_read_teid(teid_out.value(), tx_upper.tx_ul_gpdus[i], gtpu_logger));
    ASSERT_EQ(teid_out, tx_cfg.peer_teid);
    std::string dest_addr_str;
    ASSERT_TRUE(
        ocudu::sockaddr_to_ip_str(reinterpret_cast<sockaddr*>(&tx_upper.last_dest_addr), dest_addr_str, logger));
    ASSERT_EQ(dest_addr_str, "127.0.0.1");
  }
}

/// \brief Test transmission of T-PDUs with no GTP-U SN is stopped after stop command.
TEST_F(gtpu_tunnel_pdcp_tx_test, tx_stop)
{
  // create Tx entity.
  gtpu_tunnel_pdcp_config::gtpu_tunnel_pdcp_tx_config tx_cfg = {};
  tx_cfg.lif                                                 = gtpu_logical_interface::xnu;
  tx_cfg.peer_addr                                           = "127.0.0.1";
  tx_cfg.peer_teid                                           = gtpu_teid_t{0x2};
  tx_cfg.pdcp_sn_len                                         = pdcp_sn_size::size12bits;

  tx = std::make_unique<gtpu_tunnel_pdcp_tx_impl>(cu_up_ue_index_t::MIN_CU_UP_UE_INDEX, tx_cfg, dummy_pcap, tx_upper);
  ASSERT_NE(tx, nullptr);

  for (unsigned i = 0; i < 3; i++) {
    byte_buffer tpdu = byte_buffer::create(tpdu_1).value();
    byte_buffer gpdu;
    switch (i) {
      case 0:
        gpdu = byte_buffer::create(gpdu_tpdu_1_teid_1_pdcp_sn_0).value();
        break;
      case 1:
        gpdu = byte_buffer::create(gpdu_tpdu_1_teid_1_pdcp_sn_1).value();
        break;
      case 2:
        gpdu = byte_buffer::create(gpdu_tpdu_1_teid_1_pdcp_sn_2).value();
        break;
      default:
        break;
    }

    tx->handle_sdu(std::move(tpdu), i);
    ASSERT_EQ(gpdu, tx_upper.tx_ul_gpdus[i]);
  }
  tx->stop();
  tx_upper.tx_ul_gpdus.clear();

  // No more PDUs should be accepted.
  for (unsigned i = 3; i < 6; i++) {
    byte_buffer tpdu = byte_buffer::create(tpdu_1).value();
    tx->handle_sdu(std::move(tpdu), i);
    ASSERT_TRUE(tx_upper.tx_ul_gpdus.empty());
  }
}

} // namespace

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
