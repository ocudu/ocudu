// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/pdcp/pdcp_entity_rx.h"
#include "lib/pdcp/pdcp_interconnect.h"
#include "pdcp_rohc_test_helpers.h"
#include "pdcp_test_vectors.h"
#include "ocudu/pdcp/pdcp_config.h"
#include "ocudu/pdcp/pdcp_entity.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/executors/task_worker_pool.h"
#include "ocudu/support/test_utils.h"
#include "ocudu/support/timers.h"
#include <gtest/gtest.h>
#include <queue>

namespace ocudu {

struct rohc_test_params {
  const char*                name;
  std::optional<rohc_config> config;
};

rohc_test_params cfg_rohc_disabled{.name = "rohc_disabled", .config = std::nullopt};
rohc_test_params cfg_rohc_uncompressed{.name = "rohc_uncompressed", .config = rohc_config{}};
rohc_test_params cfg_rohc_compressed{.name = "rohc_v1_RTP_UDP_IP_v1_UDP_IP_v1_IP",
                                     .config =
                                         rohc_config{.profiles = rohc_profile_config()
                                                                     .set_profile(rohc_profile::profile0x0001, true)
                                                                     .set_profile(rohc_profile::profile0x0002, true)
                                                                     .set_profile(rohc_profile::profile0x0004, true)}};

/// Dummy operator to avoid Valgrind warnings.
inline std::ostream& operator<<(std::ostream& os, const rohc_test_params& params)
{
  return os;
}

/// Helper class to verify the state of the PDCP entity when the order of the PDU's crypto processing
/// is non-deterministic.
inline void assert_pdcp_state(const pdcp_rx_state& st, const pdcp_rx_state& exp_st)
{
  FLUSH_AND_ASSERT_EQ(st.rx_next, exp_st.rx_next);
  FLUSH_AND_ASSERT_EQ(st.rx_reord, exp_st.rx_reord);
  FLUSH_AND_ASSERT_EQ(st.rx_deliv, exp_st.rx_deliv);
}

class mock_pdcp_metrics_notifier : public pdcp_metrics_notifier
{
  void report_metrics(const pdcp_metrics_container& metrics) override { metrics_list.push_back(metrics); }

public:
  std::vector<pdcp_metrics_container> metrics_list;
};

/// Mocking class of the surrounding layers invoked by the PDCP.
class pdcp_rx_test_frame : public pdcp_tx_status_handler,
                           public pdcp_tx_feedback_handler,
                           public pdcp_rx_upper_data_notifier,
                           public pdcp_rx_upper_control_notifier
{
public:
  std::queue<byte_buffer>       sdu_queue;
  uint32_t                      sdu_counter            = 0;
  uint32_t                      integrity_fail_counter = 0;
  uint32_t                      nof_max_count_reached  = 0;
  uint32_t                      nof_protocol_failure   = 0;
  uint32_t                      nof_resume_required    = 0;
  std::queue<byte_buffer_chain> status_report_queue;
  std::queue<byte_buffer_chain> rohc_feedback_received;
  std::queue<byte_buffer>       rohc_feedback_produced;

  /// PDCP TX status handler.
  void on_status_report(byte_buffer_chain status) override { status_report_queue.push(std::move(status)); }

  /// PDCP TX handler for consumption of received ROHC feedback.
  void on_rohc_feedback_received(byte_buffer_chain rohc_feedback) override
  {
    rohc_feedback_received.push(std::move(rohc_feedback));
  }

  /// PDCP TX handler for forwarding of produced ROHC feedback.
  void on_rohc_feedback_produced(byte_buffer rohc_feedback) override
  {
    rohc_feedback_produced.push(std::move(rohc_feedback));
  }

  /// PDCP RX upper layer data notifier
  void on_new_sdu(byte_buffer sdu, bool integrity_verified) override
  {
    sdu_queue.push(std::move(sdu));
    sdu_counter++;
  }

  /// PDCP RX upper layer control notifier
  void on_integrity_failure() override { integrity_fail_counter++; }
  void on_protocol_failure() override { nof_protocol_failure++; }
  void on_max_count_reached() override { nof_max_count_reached++; }
  void on_resume_required() override { nof_resume_required++; }
};

/// Fixture class for PDCP tests
class pdcp_rx_test_helper
{
protected:
  pdcp_rx_test_helper(uint32_t nof_crypto_threads_, task_executor& crypto_exec_, manual_task_worker& ul_worker) :
    nof_crypto_threads(nof_crypto_threads_), worker(ul_worker), crypto_exec(crypto_exec_)
  {
  }

  /// \brief Initializes fixture according to size sequence number size
  /// \param sn_size_ size of the sequence number
  void init(pdcp_sn_size      sn_size_     = pdcp_sn_size::size12bits,
            unsigned          algo_        = 2,
            rohc_test_params  rohc_params  = cfg_rohc_disabled,
            pdcp_rb_type      rb_type_     = pdcp_rb_type::drb,
            pdcp_rlc_mode     rlc_mode_    = pdcp_rlc_mode::am,
            pdcp_t_reordering t_reordering = pdcp_t_reordering::ms10,
            pdcp_max_count    max_count    = {pdcp_rx_default_max_count_notify, pdcp_rx_default_max_count_hard})
  {
    sn_size            = sn_size_;
    algo               = algo_;
    header_compression = rohc_params.config;
    logger.info(
        "Creating PDCP RX ({} bit nia={} nea={}, {})", pdcp_sn_size_to_uint(sn_size), algo, algo, header_compression);

    // Set Rx config
    config.rb_type               = rb_type_;
    config.rlc_mode              = rlc_mode_;
    config.sn_size               = sn_size;
    config.direction             = pdcp_security_direction::downlink;
    config.header_compression    = header_compression;
    config.out_of_order_delivery = false;
    config.t_reordering          = t_reordering;
    config.custom.max_count      = max_count;

    // RB_id and security domain
    rb_id_t rb_id;
    switch (rb_type_) {
      case pdcp_rb_type::srb:
        sec_cfg.domain = security::sec_domain::rrc;
        rb_id          = srb_id_t::srb1;
        break;
      case pdcp_rb_type::drb:
        sec_cfg.domain = security::sec_domain::up;
        rb_id          = drb_id_t::drb1;
        break;
    }

    // Set security keys
    sec_cfg.k_128_int = k_128_int;
    sec_cfg.k_128_enc = k_128_enc;

    // Set encription/integrity algorithms
    sec_cfg.integ_algo  = static_cast<security::integrity_algorithm>(algo);
    sec_cfg.cipher_algo = static_cast<security::ciphering_algorithm>(algo);

    // Create PDCP RX entity
    test_frame = std::make_unique<pdcp_rx_test_frame>();
    metrics_agg =
        std::make_unique<pdcp_metrics_aggregator>(0, rb_id, timer_duration{100}, &metrics_notif, worker, false);
    pdcp_rohc_factory = std::make_unique<rohc::dummy_rohc_factory>();
    pdcp_rx           = std::make_unique<pdcp_entity_rx>(0,
                                               rb_id,
                                               config,
                                               *test_frame,
                                               *test_frame,
                                               timer_factory{timers, worker},
                                               worker,
                                               crypto_exec,
                                               nof_crypto_threads,
                                               *pdcp_rohc_factory,
                                               *metrics_agg);
    pdcp_rx->set_status_handler(test_frame.get());
    pdcp_rx->set_feedback_handler(test_frame.get());

    ocudulog::flush();
  }

  /// \brief Gets test PDU based on the COUNT and SN size
  /// \param count COUNT being tested
  /// \param exp_pdu Expected PDU that is set to the correct test vector
  void get_test_pdu(uint32_t count, byte_buffer& exp_pdu)
  {
    ASSERT_EQ(true, get_pdu_test_vector(sn_size, count, exp_pdu, algo));
  }

  /// \brief Gets test PDU based on the COUNT and SN size and algo
  /// \param count COUNT being tested
  /// \param exp_pdu Expected PDU that is set to the correct test vector
  /// \param custom_algo Pick PDU for a specific algorithm (which may differ from current config)
  void get_test_pdu(uint32_t count, byte_buffer& exp_pdu, unsigned custom_algo)
  {
    ASSERT_EQ(true, get_pdu_test_vector(sn_size, count, exp_pdu, custom_algo));
  }

  /// \brief Helper to advance the timers
  /// \param nof_tick Number of ticks to advance timers
  void tick_all(uint32_t nof_ticks)
  {
    for (uint32_t i = 0; i < nof_ticks; i++) {
      timers.tick();
      worker.run_pending_tasks();
    }
  }

  uint32_t SN(uint32_t count) const { return count & (0xffffffffU >> (32U - static_cast<uint8_t>(sn_size))); }
  uint32_t HFN(uint32_t count) const { return (count >> static_cast<uint8_t>(sn_size)); }
  uint32_t COUNT(uint32_t hfn, uint32_t sn) const { return (hfn << static_cast<uint8_t>(sn_size)) | sn; }

  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("TEST", false);

  pdcp_sn_size                        sn_size = {};
  unsigned                            algo    = {};
  std::optional<rohc_config>          header_compression;
  pdcp_rx_config                      config = {};
  timer_manager                       timers;
  std::unique_ptr<pdcp_rx_test_frame> test_frame;

  unsigned crypto_queue_size = 128;

  const uint32_t      nof_crypto_threads;
  manual_task_worker& worker;
  task_executor&      crypto_exec;

  security::sec_128_as_config               sec_cfg;
  std::unique_ptr<pdcp_metrics_aggregator>  metrics_agg;
  std::unique_ptr<rohc::dummy_rohc_factory> pdcp_rohc_factory;
  std::unique_ptr<pdcp_entity_rx>           pdcp_rx;
  mock_pdcp_metrics_notifier                metrics_notif;
  pdcp_rx_lower_interface*                  pdcp_rx_lower = nullptr;
};

/// Fixture class for PDCP tests
class pdcp_rx_test_helper_default_crypto : public pdcp_rx_test_helper
{
public:
  pdcp_rx_test_helper_default_crypto() : pdcp_rx_test_helper(2, crypto_exec, ul_worker) {}

protected:
  void               wait_pending_crypto() { crypto_worker_pool.wait_pending_tasks(); }
  manual_task_worker ul_worker{64};

private:
  unsigned crypto_queue_size = 128;

  task_worker_pool<concurrent_queue_policy::lockfree_mpmc>          crypto_worker_pool{"crypto",
                                                                              nof_crypto_threads,
                                                                              crypto_queue_size};
  task_worker_pool_executor<concurrent_queue_policy::lockfree_mpmc> crypto_exec =
      task_worker_pool_executor<concurrent_queue_policy::lockfree_mpmc>(crypto_worker_pool);
};

class pdcp_rx_test_helper_manual_crypto : public pdcp_rx_test_helper
{
public:
  pdcp_rx_test_helper_manual_crypto() :
    pdcp_rx_test_helper(1, crypto_worker, ul_worker),
    crypto_worker(
        4096,
        true,
        true) // use crypto worker explicit_mode to always have to manually pop tasks, even with matching thread IDs.
  {
  }

protected:
  void               wait_pending_crypto() { crypto_worker.run_pending_tasks(); }
  void               wait_one_crypto_task() { crypto_worker.try_run_next(); }
  manual_task_worker ul_worker{64, true, true};
  const uint32_t     nof_crypto_threads = 1;

private:
  manual_task_worker crypto_worker;
};
} // namespace ocudu
