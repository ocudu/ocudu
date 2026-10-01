// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../../../lib/phy/upper/uplink_processor_impl.h"
#include "../../support/task_executor_test_doubles.h"
#include "../support/prach_buffer_test_doubles.h"
#include "../support/resource_grid_test_doubles.h"
#include "channel_processors/prach_detector_test_doubles.h"
#include "channel_processors/pucch/pucch_processor_test_doubles.h"
#include "channel_processors/pusch/pusch_processor_test_doubles.h"
#include "phy_tap_test_doubles.h"
#include "rx_buffer_pool_test_doubles.h"
#include "signal_processors/srs/srs_estimator_test_doubles.h"
#include "upper_phy_rx_results_notifier_test_doubles.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

class UplinkProcessorFixture : public ::testing::Test
{
public:
  UplinkProcessorFixture() :
    pucch_executor(max_nof_tasks_per_test_case),
    pusch_executor(max_nof_tasks_per_test_case),
    srs_executor(max_nof_tasks_per_test_case),
    prach_executor(max_nof_tasks_per_test_case),
    grid_reader_spy(max_nof_layers, max_nof_symbols, max_nof_prb),
    grid_writer_spy(max_nof_layers, max_nof_symbols, max_nof_prb)
  {
  }

  void SetUp() override
  {
    auto prach      = std::make_unique<prach_detector_spy>();
    auto pusch_proc = std::make_unique<pusch_processor_spy>();
    auto pucch_proc = std::make_unique<pucch_processor_spy>();
    auto srs        = std::make_unique<srs_estimator_spy>();
    auto grid       = std::make_unique<resource_grid_spy>(grid_reader_spy, grid_writer_spy);
    auto tap        = std::make_unique<phy_tap_spy>();

    ASSERT_NE(prach, nullptr);
    ASSERT_NE(pusch_proc, nullptr);
    ASSERT_NE(pucch_proc, nullptr);
    ASSERT_NE(srs, nullptr);
    ASSERT_NE(grid, nullptr);
    ASSERT_NE(tap, nullptr);

    prach_spy = prach.get();
    pusch_spy = pusch_proc.get();
    grid_spy  = grid.get();
    tap_spy   = tap.get();
    pucch_spy = pucch_proc.get();
    srs_spy   = srs.get();

    uplink_processor_impl::task_executor_collection executors = {.pucch_executor = pucch_executor,
                                                                 .pusch_executor = pusch_executor,
                                                                 .srs_executor   = srs_executor,
                                                                 .prach_executor = prach_executor};

    ul_processor = std::make_unique<uplink_processor_impl>(std::move(prach),
                                                           std::move(pusch_proc),
                                                           std::move(pucch_proc),
                                                           std::move(srs),
                                                           std::move(grid),
                                                           std::move(tap),
                                                           executors,
                                                           buffer_pool_spy,
                                                           results_notifier,
                                                           max_nof_prb,
                                                           max_nof_layers);

    buffer_pool_spy.clear();
  }

protected:
  static constexpr unsigned   max_nof_tasks_per_test_case = 5;
  static constexpr unsigned   max_nof_prb                 = 15;
  static constexpr unsigned   max_nof_layers              = 1;
  static constexpr unsigned   max_nof_symbols             = 14;
  static constexpr unsigned   pucch_f0_start_symb         = 0;
  static constexpr unsigned   pucch_f0_nof_symb           = 1;
  static constexpr unsigned   pusch_csi2_size             = 3;
  static constexpr unsigned   slot_end_symbol_index       = MAX_NSYMB_PER_SLOT - 1;
  static constexpr slot_point slot                        = {0, 9};

  const uplink_pdu_slot_repository::pusch_pdu pusch_pdu = {
      .tb_size = units::bytes(8),
      .pdu     = {.harq_id       = to_harq_id(0),
                  .slot          = slot,
                  .rnti          = to_rnti(8323),
                  .bwp_size_rb   = 25,
                  .bwp_start_rb  = 0,
                  .cp            = cyclic_prefix::NORMAL,
                  .mcs_descr     = {modulation_scheme::PI_2_BPSK, 0.1},
                  .codeword      = {{0, ldpc_base_graph_type::BG2, true}},
                  .uci           = {1, 20, uci_part2_size_description(pusch_csi2_size), 1, 20, 6.25, 6.25},
                  .n_id          = 935,
                  .nof_tx_layers = 1,
                  .rx_ports      = {0, 1, 2, 3},
                  .dmrs_symbol_mask =
                      {false, false, true, false, false, false, false, false, false, false, false, true, false, false},
                  .dmrs               = pusch_processor::dmrs_configuration{.dmrs                        = dmrs_config_type::type1,
                                                                            .scrambling_id               = 0,
                                                                            .n_scid                      = false,
                                                                            .nof_cdm_groups_without_data = 2},
                  .freq_alloc         = rb_allocation::make_type1(15, 1),
                  .start_symbol_index = 0,
                  .nof_symbols        = max_nof_symbols,
                  .tbs_lbrm           = units::bytes(ldpc::MAX_CODEBLOCK_SIZE / 8),
                  .dc_position        = std::nullopt}};

  const uplink_pdu_slot_repository::srs_pdu srs_pdu = {
      .context = {.slot                                             = slot,
                  .rnti                                             = to_rnti(0x5000),
                  .is_normalized_channel_iq_matrix_report_requested = false,
                  .is_positioning_report_requested                  = false},
      .config  = {.slot     = slot,
                  .resource = {.nof_antenna_ports   = srs_resource_configuration::one_two_four_enum::one,
                               .nof_symbols         = srs_nof_symbols::n1,
                               .start_symbol        = 5,
                               .configuration_index = 0,
                               .sequence_id         = 0,
                               .bandwidth_index     = 0,
                               .comb_size           = tx_comb_size::n2,
                               .comb_offset         = 0,
                               .cyclic_shift        = 0,
                               .freq_position       = 0,
                               .freq_shift          = 0,
                               .freq_hopping        = 0,
                               .hopping             = srs_group_or_sequence_hopping::neither,
                               .periodicity         = std::nullopt}}};

  const uplink_pdu_slot_repository::pucch_pdu pucch_f0_pdu = {
      .context = {.slot          = slot,
                  .rnti          = to_rnti(0x4601),
                  .format        = pucch_format::FORMAT_0,
                  .context_f0_f1 = std::nullopt},
      .config  = pucch_processor::format0_configuration{.context              = std::nullopt,
                                                        .slot                 = slot,
                                                        .cp                   = cyclic_prefix::NORMAL,
                                                        .bwp_size_rb          = MAX_NOF_PRBS,
                                                        .bwp_start_rb         = 0,
                                                        .starting_prb         = 0,
                                                        .second_hop_prb       = 270,
                                                        .start_symbol_index   = pucch_f0_start_symb,
                                                        .nof_symbols          = pucch_f0_nof_symb,
                                                        .initial_cyclic_shift = 0,
                                                        .n_id                 = 0,
                                                        .nof_harq_ack         = 1,
                                                        .sr_opportunity       = true,
                                                        .ports                = {0}}};

  // PUCCH Format 1 fixture member.
  const uplink_pdu_slot_repository::pucch_pdu pucch_f1_pdu = {
      .context = {.slot          = slot,
                  .rnti          = to_rnti(0x4602),
                  .format        = pucch_format::FORMAT_1,
                  .context_f0_f1 = std::nullopt},
      .config  = pucch_processor::format1_configuration{.context              = std::nullopt,
                                                        .slot                 = slot,
                                                        .bwp_size_rb          = MAX_NOF_PRBS,
                                                        .bwp_start_rb         = 0,
                                                        .cp                   = cyclic_prefix::NORMAL,
                                                        .starting_prb         = 0,
                                                        .second_hop_prb       = std::nullopt,
                                                        .n_id                 = 0,
                                                        .nof_harq_ack         = 2,
                                                        .ports                = {0},
                                                        .initial_cyclic_shift = 0,
                                                        .nof_symbols          = 4,
                                                        .start_symbol_index   = 2,
                                                        .time_domain_occ      = 0}};

  // PUCCH Format 1 fixture member that can be combined with the previous one.
  const uplink_pdu_slot_repository::pucch_pdu pucch_f1_pdu2 = {
      .context = {.slot          = slot,
                  .rnti          = to_rnti(0x4602),
                  .format        = pucch_format::FORMAT_1,
                  .context_f0_f1 = std::nullopt},
      .config  = pucch_processor::format1_configuration{.context              = std::nullopt,
                                                        .slot                 = slot,
                                                        .bwp_size_rb          = MAX_NOF_PRBS,
                                                        .bwp_start_rb         = 0,
                                                        .cp                   = cyclic_prefix::NORMAL,
                                                        .starting_prb         = 0,
                                                        .second_hop_prb       = std::nullopt,
                                                        .n_id                 = 0,
                                                        .nof_harq_ack         = 2,
                                                        .ports                = {0},
                                                        .initial_cyclic_shift = 2,
                                                        .nof_symbols          = 4,
                                                        .start_symbol_index   = 2,
                                                        .time_domain_occ      = 0}};

  // PUCCH Format 2 fixture member.
  const uplink_pdu_slot_repository::pucch_pdu pucch_f2_pdu = {
      .context = {.slot          = slot,
                  .rnti          = to_rnti(0x4603),
                  .format        = pucch_format::FORMAT_2,
                  .context_f0_f1 = std::nullopt},
      .config  = pucch_processor::format2_configuration{.context            = std::nullopt,
                                                        .slot               = slot,
                                                        .cp                 = cyclic_prefix::NORMAL,
                                                        .ports              = {0},
                                                        .bwp_size_rb        = MAX_NOF_PRBS,
                                                        .bwp_start_rb       = 0,
                                                        .prbs               = prb_interval::start_and_len(0, 2),
                                                        .second_hop_prb     = std::nullopt,
                                                        .start_symbol_index = 0,
                                                        .nof_symbols        = 2,
                                                        .rnti               = static_cast<uint16_t>(to_rnti(0x4603)),
                                                        .n_id               = 0,
                                                        .n_id_0             = 0,
                                                        .nof_harq_ack       = 2,
                                                        .nof_sr             = 0,
                                                        .nof_csi_part1      = 0,
                                                        .csi_part2_size     = uci_part2_size_description(1),
                                                        .max_code_rate      = 0.5}};

  // PUCCH Format 3 fixture member.
  const uplink_pdu_slot_repository::pucch_pdu pucch_f3_pdu = {
      .context = {.slot          = slot,
                  .rnti          = to_rnti(0x4604),
                  .format        = pucch_format::FORMAT_3,
                  .context_f0_f1 = std::nullopt},
      .config  = pucch_processor::format3_configuration{.context            = std::nullopt,
                                                        .slot               = slot,
                                                        .cp                 = cyclic_prefix::NORMAL,
                                                        .ports              = {0},
                                                        .bwp_size_rb        = MAX_NOF_PRBS,
                                                        .bwp_start_rb       = 0,
                                                        .prbs               = prb_interval::start_and_len(0, 2),
                                                        .second_hop_prb     = std::nullopt,
                                                        .start_symbol_index = 0,
                                                        .nof_symbols        = 4,
                                                        .rnti               = static_cast<uint16_t>(to_rnti(0x4604)),
                                                        .n_id_hopping       = 0,
                                                        .n_id_scrambling    = 0,
                                                        .nof_harq_ack       = 2,
                                                        .nof_sr             = 0,
                                                        .nof_csi_part1      = 0,
                                                        .csi_part2_size     = uci_part2_size_description(1),
                                                        .additional_dmrs    = false,
                                                        .pi2_bpsk           = false,
                                                        .max_code_rate      = 0.5}};

  // PUCCH Format 4 fixture member.
  const uplink_pdu_slot_repository::pucch_pdu pucch_f4_pdu = {
      .context = {.slot          = slot,
                  .rnti          = to_rnti(0x4605),
                  .format        = pucch_format::FORMAT_4,
                  .context_f0_f1 = std::nullopt},
      .config  = pucch_processor::format4_configuration{.context            = std::nullopt,
                                                        .slot               = slot,
                                                        .cp                 = cyclic_prefix::NORMAL,
                                                        .ports              = {0},
                                                        .bwp_size_rb        = MAX_NOF_PRBS,
                                                        .bwp_start_rb       = 0,
                                                        .starting_prb       = 0,
                                                        .second_hop_prb     = std::nullopt,
                                                        .start_symbol_index = 0,
                                                        .nof_symbols        = 4,
                                                        .rnti               = static_cast<uint16_t>(to_rnti(0x4605)),
                                                        .n_id_hopping       = 0,
                                                        .n_id_scrambling    = 0,
                                                        .nof_harq_ack       = 2,
                                                        .nof_sr             = 0,
                                                        .nof_csi_part1      = 0,
                                                        .csi_part2_size     = uci_part2_size_description(1),
                                                        .additional_dmrs    = false,
                                                        .pi2_bpsk           = false,
                                                        .occ_index          = 0,
                                                        .occ_length         = 2,
                                                        .max_code_rate      = 0.5}};

  prach_detector_spy*                     prach_spy = nullptr;
  pusch_processor_spy*                    pusch_spy = nullptr;
  pucch_processor_spy*                    pucch_spy = nullptr;
  srs_estimator_spy*                      srs_spy   = nullptr;
  manual_task_worker_always_enqueue_tasks pucch_executor;
  manual_task_worker_always_enqueue_tasks pusch_executor;
  manual_task_worker_always_enqueue_tasks srs_executor;
  manual_task_worker_always_enqueue_tasks prach_executor;
  rx_buffer_pool_spy                      buffer_pool_spy;
  upper_phy_rx_results_notifier_spy       results_notifier;
  std::unique_ptr<uplink_processor_impl>  ul_processor;

  resource_grid_reader_spy grid_reader_spy;
  resource_grid_writer_spy grid_writer_spy;
  resource_grid_spy*       grid_spy = nullptr;
  phy_tap_spy*             tap_spy;
};

TEST_F(UplinkProcessorFixture, concurrent_prach_stop_workflow)
{
  ul_processor->get_pdu_slot_repository(slot);

  auto prach_buffer_pool = create_spy_prach_buffer_pool();

  // Request PRACH processing.
  ul_processor->get_slot_processor(slot).process_prach(prach_buffer_pool->get(), {});

  // Create asynchronous task - it will block until all tasks are completed.
  std::atomic<bool> stop_thread_started = false;
  std::thread       stop_thread([this, &stop_thread_started]() {
    stop_thread_started = true;
    ul_processor->stop();
  });

  // Wait for thread to start.
  while (!stop_thread_started) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  // Execute tasks.
  ASSERT_TRUE(prach_executor.run_pending_tasks());

  // Check the detector has been called and the result notified.
  ASSERT_TRUE(prach_spy->has_detect_method_been_called());
  ASSERT_TRUE(results_notifier.has_prach_result_been_notified());

  // Synchronize stopping thread.
  stop_thread.join();

  // Assert tap handles were not invoked.
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), 0);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);
}

TEST_F(UplinkProcessorFixture, prach_fail_defer_workflow)
{
  ul_processor->get_pdu_slot_repository(slot);

  // Stop PRACH executor.
  prach_executor.stop();

  auto prach_buffer_pool = create_spy_prach_buffer_pool();

  // Request PRACH processing.
  ul_processor->get_slot_processor(slot).process_prach(prach_buffer_pool->get(), {});

  // Request processor to stop. There is no pending task, so it should not block.
  ul_processor->stop();

  // Check the detector has not been called nor the result notified.
  ASSERT_FALSE(prach_spy->has_detect_method_been_called());
  ASSERT_FALSE(results_notifier.has_prach_result_been_notified());

  // Assert tap handles were not invoked.
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), 0);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);
}

TEST_F(UplinkProcessorFixture, prach_request_after_stop)
{
  ul_processor->get_pdu_slot_repository(slot);

  // Request processor to stop. There is no pending task, so it should not block.
  ul_processor->stop();

  auto prach_buffer_pool = create_spy_prach_buffer_pool();

  // Request PRACH processing.
  ul_processor->get_slot_processor(slot).process_prach(prach_buffer_pool->get(), {});

  // The UL processor must not create an asynchronous task.
  ASSERT_FALSE(prach_executor.run_pending_tasks());

  // Check the detector has not been called nor the result notified.
  ASSERT_FALSE(prach_spy->has_detect_method_been_called());
  ASSERT_FALSE(results_notifier.has_prach_result_been_notified());

  // Assert tap handles were not invoked.
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), 0);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);
}

TEST_F(UplinkProcessorFixture, pusch_normal_workflow)
{
  // Get PDU repository, add PDU and release repository. Keep the grid alive until the end of the test.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  repository->add_pusch_pdu(pusch_pdu);
  shared_resource_grid grid = repository.release();

  unsigned pusch_end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  // Notify reception of the previous reception symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(pusch_end_symbol_index - 1, true);
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), pusch_end_symbol_index);

  // Check that nothing happened.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(buffer_pool_spy.is_locked());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Notify reception of receive symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(pusch_end_symbol_index, true);
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), pusch_end_symbol_index + 1);

  // Check PUSCH processing has been enqueued and the processor was not called.
  ASSERT_TRUE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Execute tasks.
  pusch_executor.run_pending_tasks();

  // Check the processor has been called and the result notified.
  ASSERT_TRUE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());

  // Validate UCI message content (HARQ-ACK, CSI Part 1, CSI Part 2).
  const auto& pusch_uci = results_notifier.get_last_pusch_uci_result();
  ASSERT_TRUE(pusch_uci.has_value()) << "PUSCH UCI result must be present";

  ASSERT_TRUE(pusch_uci->harq_ack.has_value()) << "HARQ-ACK field must be present";
  ASSERT_EQ(pusch_uci->harq_ack->payload.size(), pusch_pdu.pdu.uci.nof_harq_ack)
      << "HARQ-ACK payload bit count must match";

  ASSERT_TRUE(pusch_uci->csi1.has_value()) << "CSI Part 1 field must be present";
  ASSERT_EQ(pusch_uci->csi1->payload.size(), pusch_pdu.pdu.uci.nof_csi_part1)
      << "CSI Part 1 payload bit count must match";

  ASSERT_TRUE(pusch_uci->csi2.has_value()) << "CSI Part 2 field must be present";
  ASSERT_EQ(pusch_uci->csi2->payload.size(), pusch_csi2_size) << "CSI Part 2 payload bit count must match";

  // Validate PHY tap.
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), MAX_NSYMB_PER_SLOT);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);
}

TEST_F(UplinkProcessorFixture, pucch_normal_workflow)
{
  // Get PDU repository, add PDU and release repository. Keep the grid alive until the end of the test.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  repository->add_pucch_pdu(pucch_f0_pdu);
  repository->add_pucch_pdu(pucch_f1_pdu);
  repository->add_pucch_pdu(pucch_f1_pdu2);
  repository->add_pucch_pdu(pucch_f2_pdu);
  repository->add_pucch_pdu(pucch_f3_pdu);
  repository->add_pucch_pdu(pucch_f4_pdu);
  shared_resource_grid grid = repository.release();

  // The PUCCH Format 0 PDU ends at symbol 0 (start=0, length=1).
  // handle_rx_symbol(0) triggers processing since the end symbol is reached.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(slot_end_symbol_index, true);

  // Check PUCCH processing has been enqueued.
  ASSERT_TRUE(pucch_executor.has_pending_tasks());
  ASSERT_FALSE(results_notifier.has_pucch_result_been_notified());

  // Execute tasks.
  pucch_executor.run_pending_tasks();

  // Check the processor has been called and the result notified.
  ASSERT_TRUE(pucch_spy->has_format0_been_called());
  ASSERT_TRUE(pucch_spy->has_format1_been_called());
  ASSERT_TRUE(pucch_spy->has_format2_been_called());
  ASSERT_TRUE(pucch_spy->has_format3_been_called());
  ASSERT_TRUE(pucch_spy->has_format4_been_called());
  ASSERT_TRUE(results_notifier.has_pucch_result_been_notified());
}

TEST_F(UplinkProcessorFixture, srs_normal_workflow)
{
  // Get PDU repository, add PDU and release repository. Keep the grid alive until the end of the test.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  repository->add_srs_pdu(srs_pdu);
  shared_resource_grid grid = repository.release();

  // Notify reception of the SRS OFDM symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(slot_end_symbol_index, true);

  // Check SRS processing has been enqueued and the processor was not called.
  ASSERT_TRUE(srs_executor.has_pending_tasks());
  ASSERT_FALSE(srs_spy->has_estimate_method_been_called());
  ASSERT_FALSE(results_notifier.has_srs_result_been_notified());

  // Execute tasks.
  srs_executor.run_pending_tasks();

  // Check the estimator has been called and the result notified.
  ASSERT_TRUE(srs_spy->has_estimate_method_been_called());
  ASSERT_TRUE(results_notifier.has_srs_result_been_notified());
}

TEST_F(UplinkProcessorFixture, phy_tap_normal_workflow)
{
  // Get PDU repository, add a PDU to avoid a quiet notification, and release repository.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  repository->add_pucch_pdu(pucch_f0_pdu);
  shared_resource_grid grid = repository.release();

  // Initial state: tap has not been called.
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), 0);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);

  // The PUCCH Format 0 PDU ends at symbol 0. Process only that symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(pucch_f0_nof_symb - 1, true);

  // Tap should have been called for symbol 0.
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), 1);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);

  // Execute PUCCH task.
  pucch_executor.run_pending_tasks();
  ASSERT_TRUE(pucch_spy->has_format0_been_called());

  // Continue processing the remaining symbols (1..max_nof_symbols-1).
  for (unsigned sym = 1; sym < max_nof_symbols; ++sym) {
    ul_processor->get_slot_processor(slot).handle_rx_symbol(sym, true);
    ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), sym + 1);
  }

  // No quiet grid shall be reported.
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);
}

TEST_F(UplinkProcessorFixture, phy_tap_quiet_workflow)
{
  // Get PDU repository, do nothing, and release repository.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  shared_resource_grid              grid       = repository.release();

  // Initial state: tap has not been called.
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), 0);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);

  // Feed one symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(slot_end_symbol_index - 1, true);

  // Tap should have been called for symbol 0.
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), 0);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);

  // Notify the last symbol within the slot. Check the processor fed the remaining symbols (1..max_nof_symbols-1).
  ul_processor->get_slot_processor(slot).handle_rx_symbol(slot_end_symbol_index, true);

  // The quiet grid shall be reported once.
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 1);
}

TEST_F(UplinkProcessorFixture, rx_symbol_bad_order)
{
  // Get PDU repository, add PDU and release repository. Keep the grid alive until the end of the test.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  repository->add_pusch_pdu(pusch_pdu);
  shared_resource_grid grid = repository.release();

  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  // Notify reception of the previous reception symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index - 1, true);

  // Notify reception of the symbol before the previous.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index - 2, true);

  // Check that nothing happened.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(buffer_pool_spy.is_locked());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Notify reception of receive symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), max_nof_symbols);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);

  // Check PUSCH processing has been enqueued and the processor was not called.
  ASSERT_TRUE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Execute tasks.
  pusch_executor.run_pending_tasks();

  // Check the processor has been called and the result notified.
  ASSERT_TRUE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pusch_wrong_slot)
{
  // Get PDU repository, add PDU and release repository. Keep the grid alive until the end of the test.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  repository->add_pusch_pdu(pusch_pdu);
  shared_resource_grid grid = repository.release();

  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  // Notify reception of receive symbol for the wrong slot.
  ul_processor->get_slot_processor(slot - 1).handle_rx_symbol(end_symbol_index, true);
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), 0);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 1);

  // Check PUSCH processing has NOT been enqueued and the processor was not called.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Notify reception of receive symbol for the right slot.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), max_nof_symbols);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 1);

  // Check PUSCH processing has been enqueued and the processor was not called.
  ASSERT_TRUE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Execute tasks.
  pusch_executor.run_pending_tasks();

  // Check the processor has been called and the result notified.
  ASSERT_TRUE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pusch_exceed_tb_size)
{
  // Create PDU with a larger TB than the one the pool contains.
  uplink_pdu_slot_repository::pusch_pdu pusch_pdu_large = pusch_pdu;
  pusch_pdu_large.tb_size                               = units::bytes(2 * max_nof_layers * 156 * max_nof_prb);

  // Get PDU repository, add PDU and release repository. Keep the grid alive until the end of the test.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(pusch_pdu_large.pdu.slot);
  repository->add_pusch_pdu(pusch_pdu_large);
  shared_resource_grid grid = repository.release();

  unsigned end_symbol_index = pusch_pdu_large.pdu.start_symbol_index + pusch_pdu_large.pdu.nof_symbols - 1;

  // Notify reception of receive symbol.
  ul_processor->get_slot_processor(pusch_pdu_large.pdu.slot).handle_rx_symbol(end_symbol_index, true);
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), max_nof_symbols);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);

  // Execute tasks.
  pusch_executor.run_pending_tasks();

  // Check the processor has not been called and the result notified.
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pusch_locked_rx_buffer)
{
  // Get PDU repository, add PDU and release repository. Keep the grid alive until the end of the test.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  repository->add_pusch_pdu(pusch_pdu);
  shared_resource_grid grid = repository.release();

  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  // Lock buffer pool to ensure it fails to retrieve a buffer.
  ASSERT_TRUE(buffer_pool_spy.try_lock());

  // Notify reception of receive symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), max_nof_symbols);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);

  // Check PUSCH processing has been enqueued and the processor was not called.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());
}

TEST_F(UplinkProcessorFixture, pdu_fail_defer_workflow)
{
  // Get PDU repository, add a PDU of each and release repository.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
    repository->add_pucch_pdu(pucch_f0_pdu);
    repository->add_pucch_pdu(pucch_f1_pdu);
    repository->add_pucch_pdu(pucch_f2_pdu);
    repository->add_pucch_pdu(pucch_f3_pdu);
    repository->add_pucch_pdu(pucch_f4_pdu);
    repository->add_srs_pdu(srs_pdu);
  }

  // Ensure executors do not accept more tasks.
  pucch_executor.stop();
  pusch_executor.stop();
  srs_executor.stop();

  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  // Notify reception of receive symbol.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);

  // PHY tap should be called as normal.
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), max_nof_symbols);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);

  // Check PUCCH, PUSCH, and SRS processing was not invoked.
  ASSERT_FALSE(pucch_spy->has_format0_been_called());
  ASSERT_FALSE(pucch_spy->has_format1_been_called());
  ASSERT_FALSE(pucch_spy->has_format2_been_called());
  ASSERT_FALSE(pucch_spy->has_format3_been_called());
  ASSERT_FALSE(pucch_spy->has_format4_been_called());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(srs_spy->has_estimate_method_been_called());

  // Validate notifiers.
  ASSERT_EQ(results_notifier.get_nof_pucch_results(), 5);
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());
  ASSERT_FALSE(results_notifier.has_srs_result_been_notified());
}

TEST_F(UplinkProcessorFixture, discard_slot_before_first_rx)
{
  // Get PDU repository, add PDU and release repository. Keep the grid alive until the end of the test.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  repository->add_pusch_pdu(pusch_pdu);
  repository->add_pucch_pdu(pucch_f0_pdu);
  repository->add_pucch_pdu(pucch_f1_pdu);
  repository->add_pucch_pdu(pucch_f2_pdu);
  repository->add_pucch_pdu(pucch_f3_pdu);
  repository->add_pucch_pdu(pucch_f4_pdu);
  repository->add_srs_pdu(srs_pdu);
  shared_resource_grid grid = repository.release();

  uplink_slot_processor& slot_processor = ul_processor->get_slot_processor(slot);

  // Discard twice from and release grid from different threads.
  std::thread discard_slot_thread  = std::thread([&slot_processor]() {
    slot_processor.discard_slot();
    slot_processor.handle_rx_symbol(max_nof_symbols - 1, true);
  });
  std::thread discard_slot_thread2 = std::thread([&slot_processor]() { slot_processor.discard_slot(); });
  std::thread release_grid_thread  = std::thread([grid_ = std::move(grid)]() {});
  discard_slot_thread.join();
  discard_slot_thread2.join();
  release_grid_thread.join();

  // Assert no processor nor executor was used.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pucch_executor.has_pending_tasks());
  ASSERT_FALSE(srs_executor.has_pending_tasks());
  ASSERT_FALSE(prach_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(pucch_spy->has_format0_been_called());
  ASSERT_FALSE(pucch_spy->has_format1_been_called());
  ASSERT_FALSE(pucch_spy->has_format2_been_called());
  ASSERT_FALSE(pucch_spy->has_format3_been_called());
  ASSERT_FALSE(pucch_spy->has_format4_been_called());
  ASSERT_FALSE(srs_spy->has_estimate_method_been_called());

  // Validate PUSCH results.
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  const auto& pusch_uci = results_notifier.get_last_pusch_uci_result();
  ASSERT_TRUE(pusch_uci.has_value());
  ASSERT_TRUE(pusch_uci->harq_ack.has_value());
  ASSERT_EQ(pusch_uci->harq_ack->payload.size(), pusch_pdu.pdu.uci.nof_harq_ack);
  ASSERT_EQ(pusch_uci->harq_ack->status, uci_status::unknown);
  ASSERT_FALSE(pusch_uci->csi1.has_value());
  ASSERT_FALSE(pusch_uci->csi2.has_value());

  // Validate PUCCH results.
  ASSERT_EQ(results_notifier.get_nof_pucch_results(), 5);

  // Assert tap handles were not invoked.
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), 0);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);

  // Assert the UL processor is available again.
  ASSERT_TRUE(ul_processor->get_pdu_slot_repository(slot + 1).is_valid());
}

TEST_F(UplinkProcessorFixture, discard_slot_before_first_rx_no_pdu)
{
  // Get PDU repository, add PDU and release repository.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  shared_resource_grid              grid       = repository.release();

  // Discard twice from and release grid from different threads.
  std::thread discard_slot_thread  = std::thread([this]() { ul_processor->get_slot_processor(slot).discard_slot(); });
  std::thread discard_slot_thread2 = std::thread([this]() { ul_processor->get_slot_processor(slot).discard_slot(); });
  std::thread release_grid_thread  = std::thread([grid_ = std::move(grid)]() {});
  discard_slot_thread.join();
  discard_slot_thread2.join();
  release_grid_thread.join();

  // Assert the UL processor is available again.
  ASSERT_TRUE(ul_processor->get_pdu_slot_repository(slot + 1).is_valid());
}

TEST_F(UplinkProcessorFixture, invalid_first_symbol)
{
  // Get PDU repository, add PDU and release repository. Keep the grid alive until the end of the test.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
    repository->add_pucch_pdu(pucch_f0_pdu);
    repository->add_pucch_pdu(pucch_f1_pdu);
    repository->add_pucch_pdu(pucch_f2_pdu);
    repository->add_pucch_pdu(pucch_f3_pdu);
    repository->add_pucch_pdu(pucch_f4_pdu);
    repository->add_srs_pdu(srs_pdu);
  }

  // Notify an invalid symbol.
  uplink_slot_processor& slot_processor = ul_processor->get_slot_processor(slot);
  slot_processor.handle_rx_symbol(0, false);

  // Assert PUSCH PDU was discarded.
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  const auto& pusch_uci = results_notifier.get_last_pusch_uci_result();
  ASSERT_TRUE(pusch_uci.has_value());
  ASSERT_TRUE(pusch_uci->harq_ack.has_value());
  ASSERT_EQ(pusch_uci->harq_ack->payload.size(), pusch_pdu.pdu.uci.nof_harq_ack);
  ASSERT_EQ(pusch_uci->harq_ack->status, uci_status::unknown);
  ASSERT_FALSE(pusch_uci->csi1.has_value());
  ASSERT_FALSE(pusch_uci->csi2.has_value());

  // The uplink processor shall be available again.
  ASSERT_TRUE(ul_processor->get_pdu_slot_repository(slot + 1).is_valid());
}

TEST_F(UplinkProcessorFixture, invalid_middle_symbol_with_pusch)
{
  // Get PDU repository, add PDU and release repository. Keep the grid alive until the end of the test.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  // Notify some valid symbols.
  unsigned               nof_valid_symbols = 3;
  uplink_slot_processor& slot_processor    = ul_processor->get_slot_processor(slot);
  for (unsigned i_symbol = 0; i_symbol != nof_valid_symbols; ++i_symbol) {
    slot_processor.handle_rx_symbol(i_symbol, true);
  }

  // Notify an invalid symbol.
  slot_processor.handle_rx_symbol(nof_valid_symbols, false);

  // Assert PUSCH PDU was discarded.
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  const auto& pusch_uci = results_notifier.get_last_pusch_uci_result();
  ASSERT_TRUE(pusch_uci.has_value());
  ASSERT_TRUE(pusch_uci->harq_ack.has_value());
  ASSERT_EQ(pusch_uci->harq_ack->payload.size(), pusch_pdu.pdu.uci.nof_harq_ack);
  ASSERT_EQ(pusch_uci->harq_ack->status, uci_status::unknown);
  ASSERT_FALSE(pusch_uci->csi1.has_value());
  ASSERT_FALSE(pusch_uci->csi2.has_value());

  // Clear notifier, complete symbol reception for the entire slot. No new notification should be reported.
  results_notifier.clear();
  for (unsigned i_symbol = nof_valid_symbols; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
    slot_processor.handle_rx_symbol(i_symbol, true);
  }
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), nof_valid_symbols);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);

  // The uplink processor shall be available again.
  ASSERT_TRUE(ul_processor->get_pdu_slot_repository(slot + 1).is_valid());
}

TEST_F(UplinkProcessorFixture, invalid_middle_symbol_with_pucch_f0)
{
  // Get PDU repository, add PDU and release repository. Keep the grid alive until the end of the test.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pucch_pdu(pucch_f0_pdu);
  }

  // Notify some valid symbols.
  unsigned               nof_valid_symbols = slot_end_symbol_index - 1;
  uplink_slot_processor& slot_processor    = ul_processor->get_slot_processor(slot);
  for (unsigned i_symbol = 0; i_symbol != nof_valid_symbols; ++i_symbol) {
    slot_processor.handle_rx_symbol(i_symbol, true);
  }

  // Process PUCCH requests that fall within the valid OFDM symbols.
  ASSERT_TRUE(pucch_executor.run_pending_tasks());

  // Notify an invalid symbol.
  slot_processor.handle_rx_symbol(nof_valid_symbols, false);

  // Clear notifier, complete symbol reception for the entire slot. No new notification should be reported.
  results_notifier.clear();
  for (unsigned i_symbol = nof_valid_symbols; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
    slot_processor.handle_rx_symbol(i_symbol, true);
  }
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), nof_valid_symbols);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);

  // The uplink processor shall be available again.
  ASSERT_TRUE(ul_processor->get_pdu_slot_repository(slot + 1).is_valid());
}

TEST_F(UplinkProcessorFixture, discard_invalid_symbol_twice)
{
  // The PUCCH PDU ends at the first symbol of the slot and the PUSCH PDU ends later. Two invalid OFDM symbols must fit
  // in between.
  static constexpr unsigned pucch_end_symbol_index = 0;
  const unsigned            pusch_end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;
  ASSERT_GT(pusch_end_symbol_index, pucch_end_symbol_index + 2);

  // Get PDU repository, add PDUs and release repository. Keep the grid alive until the end of the test.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  repository->add_pucch_pdu(pucch_f0_pdu);
  repository->add_pucch_pdu(pucch_f1_pdu);
  repository->add_pucch_pdu(pucch_f2_pdu);
  repository->add_pucch_pdu(pucch_f3_pdu);
  repository->add_pucch_pdu(pucch_f4_pdu);
  repository->add_pusch_pdu(pusch_pdu);
  shared_resource_grid grid = repository.release();

  // Notify the PUCCH last OFDM symbol as valid. It enqueues the PUCCH processing task.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(pucch_end_symbol_index, true);

  // The PUCCH task is left enqueued: the PUCCH PDU remains in execution and keeps the processor out of idle.
  ASSERT_TRUE(pucch_executor.has_pending_tasks());
  ASSERT_FALSE(results_notifier.has_pucch_result_been_notified());

  // Notify an invalid OFDM symbol. It discards the PUSCH PDU, which has not been processed yet.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(pucch_end_symbol_index + 1, false);
  EXPECT_EQ(results_notifier.get_nof_pusch_data_results(), 1);
  EXPECT_EQ(results_notifier.get_nof_pucch_results(), 4);

  // Notify a second invalid OFDM symbol for the same slot. The PUSCH PDU has already been discarded, so it must not be
  // discarded again.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(pucch_end_symbol_index + 2, false);
  EXPECT_EQ(results_notifier.get_nof_pusch_data_results(), 1);
  EXPECT_EQ(results_notifier.get_nof_pucch_results(), 4);

  // Advance to the last OFDM symbol as valid. The all PDUs have already been discarded, so it must not be discarded
  // again.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(slot_end_symbol_index, true);
  EXPECT_EQ(results_notifier.get_nof_pusch_data_results(), 1);
  EXPECT_EQ(results_notifier.get_nof_pucch_results(), 4);

  // Validate PUSCH results.
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  const auto& pusch_uci = results_notifier.get_last_pusch_uci_result();
  ASSERT_TRUE(pusch_uci.has_value());
  ASSERT_TRUE(pusch_uci->harq_ack.has_value());
  ASSERT_EQ(pusch_uci->harq_ack->payload.size(), pusch_pdu.pdu.uci.nof_harq_ack);
  ASSERT_EQ(pusch_uci->harq_ack->status, uci_status::unknown);
  ASSERT_FALSE(pusch_uci->csi1.has_value());
  ASSERT_FALSE(pusch_uci->csi2.has_value());

  // Run the pending PUCCH task and then the first PUCCH will be notified.
  ASSERT_TRUE(pucch_executor.run_pending_tasks());
  EXPECT_EQ(results_notifier.get_nof_pucch_results(), 5);
  ASSERT_TRUE(pucch_spy->has_format0_been_called());

  // Assert no processor nor executor was used.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(srs_executor.has_pending_tasks());
  ASSERT_FALSE(prach_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(pucch_spy->has_format1_been_called());
  ASSERT_FALSE(pucch_spy->has_format2_been_called());
  ASSERT_FALSE(pucch_spy->has_format3_been_called());
  ASSERT_FALSE(pucch_spy->has_format4_been_called());
  ASSERT_FALSE(srs_spy->has_estimate_method_been_called());

  // Assert tap handles were not invoked.
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), 1);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);
}

TEST_F(UplinkProcessorFixture, pusch_execute_after_stop)
{
  // Get PDU repository, add PDU, release repository and ignore the grid.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  // Get processor before stopping. Otherwise, the processor will not be available.
  uplink_slot_processor& processor = ul_processor->get_slot_processor(slot);

  // Stop processor.
  ul_processor->stop();

  // Notify reception of receive symbol.
  processor.handle_rx_symbol(end_symbol_index, true);

  // Check PUSCH processing has been enqueued and the processor was not called.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());

  // Assert tap handles were not invoked.
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), max_nof_symbols);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);
}

TEST_F(UplinkProcessorFixture, reserve_slot_twice_without_request)
{
  // Get PDU repository, add PDU and release repository. Keep the grid alive until the end of the test.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  shared_resource_grid              grid       = repository.release();

  // Get the repository for the second time - it shall return an invalid repository as long as the grid is alive.
  repository = ul_processor->get_pdu_slot_repository(slot);
  ASSERT_FALSE(repository.is_valid());

  // Release grid
  grid.release();

  // Get the repository for the second time - it shall return a valid repository.
  repository = ul_processor->get_pdu_slot_repository(slot);
  ASSERT_TRUE(repository.is_valid());

  // Assert tap handles were not invoked.
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), 0);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);
}

TEST_F(UplinkProcessorFixture, reserve_slot_twice_with_request)
{
  // Get PDU repository, add PDU and release repository. Keep the grid alive until the end of the test.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  repository->add_pusch_pdu(pusch_pdu);
  shared_resource_grid grid = repository.release();

  // Get the repository for the second time - it shall return an invalid repository as long as the grid is alive.
  repository = ul_processor->get_pdu_slot_repository(slot);
  ASSERT_FALSE(repository.is_valid());

  // Release grid
  grid.release();

  // Get the repository for the second time - it shall return a valid repository.
  repository = ul_processor->get_pdu_slot_repository(slot);
  ASSERT_TRUE(repository.is_valid());

  // Assert tap handles were not invoked.
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), 0);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);
}

TEST_F(UplinkProcessorFixture, reserve_slot_twice_pending_exec)
{
  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;

  // Get PDU repository, add PDU and release repository. Keep the grid alive until the end of the test.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  repository->add_pusch_pdu(pusch_pdu);
  shared_resource_grid grid = repository.release();

  // Notify reception of receive symbol and release grid.
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), max_nof_symbols);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);
  grid.release();

  // Assert execution expectations.
  ASSERT_TRUE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());

  // Get the repository for the second time - it shall return an invalid repository.
  repository = ul_processor->get_pdu_slot_repository(slot);
  ASSERT_FALSE(repository.is_valid());

  ASSERT_TRUE(pusch_executor.run_pending_tasks());
  ASSERT_TRUE(pusch_spy->has_process_method_been_called());

  repository = ul_processor->get_pdu_slot_repository(slot);
  ASSERT_TRUE(repository.is_valid());
}

TEST_F(UplinkProcessorFixture, reserve_slot_twice_no_request_grid_alive)
{
  // Get the repository for the first time and keep the grid alive.
  shared_resource_grid grid;
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    ASSERT_TRUE(repository.is_valid());

    grid = repository.release();
    ASSERT_FALSE(repository.is_valid());
    ASSERT_TRUE(grid.is_valid());
  }

  // Get the repository for the second time - it shall return an invalid repository.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  ASSERT_FALSE(repository.is_valid());

  // Notify reception of a receiving symbol - an asynchronous task is not expected (because there are no request).
  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), 0);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 1);
  ASSERT_FALSE(pusch_executor.has_pending_tasks());

  // The tap should have been called.
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 1);

  // Release the resource grid.
  grid.release();

  // The Ul processor should be available.
  repository = ul_processor->get_pdu_slot_repository(slot);
  ASSERT_TRUE(repository.is_valid());
}

TEST_F(UplinkProcessorFixture, stop_while_no_pending_task)
{
  // Direct stop.
  ul_processor->stop();

  // Assert results.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), 0);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);
}

TEST_F(UplinkProcessorFixture, stop_while_accepting_tasks)
{
  // Add PDU to the repository.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);

  // Create asynchronous stop task.
  std::thread stop_thread([this]() { ul_processor->stop(); });

  // Create asynchronous repository release thread.
  std::thread repository_release_thread(
      [local_repository = std::move(repository)]() mutable { local_repository.release(); });

  // The stopping thread will block until the repository is released.
  stop_thread.join();
  repository_release_thread.join();
}

TEST_F(UplinkProcessorFixture, stop_while_pending_task)
{
  // Get PDU repository, add PDU and release repository. Keep the grid alive until the end of the test.
  unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
  repository->add_pusch_pdu(pusch_pdu);
  shared_resource_grid grid = repository.release();

  // Notify reception of receive symbol - an asynchronous task is expected.
  unsigned end_symbol_index = pusch_pdu.pdu.start_symbol_index + pusch_pdu.pdu.nof_symbols - 1;
  ul_processor->get_slot_processor(slot).handle_rx_symbol(end_symbol_index, true);
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), max_nof_symbols);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);
  ASSERT_TRUE(pusch_executor.has_pending_tasks());

  // Create asynchronous task - it will block until all tasks are completed.
  std::thread stop_thread([this]() { ul_processor->stop(); });

  // Execute PUSCH asynchronous task.
  pusch_executor.try_run_next();

  // Assert execution expectations.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_TRUE(pusch_spy->has_process_method_been_called());
  ASSERT_TRUE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_TRUE(results_notifier.has_pusch_uci_result_been_notified());

  // Release grid.
  grid.release();

  // Synchronize stopping thread.
  stop_thread.join();
}

TEST_F(UplinkProcessorFixture, stop_while_grid_alive)
{
  // Get PDU repository, release repository and keep the grid.
  shared_resource_grid grid;
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    grid                                         = repository.release();
  }
  ASSERT_TRUE(grid);

  // Stop processor asynchronously.
  std::thread stop_thread([this]() { ul_processor->stop(); });

  // Thread containing the resource grid in its scope.
  std::thread grid_scope([grid_ = std::move(grid)]() {});

  // The stop thread will block until the resource grid is released.
  stop_thread.join();
  grid_scope.join();
}

TEST_F(UplinkProcessorFixture, pusch_discard_after_stop)
{
  // Add PDU to the repository.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  uplink_slot_processor& slot_processor = ul_processor->get_slot_processor(slot);

  // Stop.
  ul_processor->stop();

  // Discard.
  slot_processor.discard_slot();

  // Assert results.
  ASSERT_FALSE(pusch_executor.has_pending_tasks());
  ASSERT_FALSE(pusch_spy->has_process_method_been_called());
  ASSERT_FALSE(results_notifier.has_pusch_data_result_been_notified());
  ASSERT_FALSE(results_notifier.has_pusch_uci_result_been_notified());

  // Assert tap handles were not invoked.
  ASSERT_EQ(tap_spy->get_handle_ul_symbol_count(), 0);
  ASSERT_EQ(tap_spy->get_handle_quiet_grid_count(), 0);
}

TEST_F(UplinkProcessorFixture, simultaneous_pusch_discard_and_stop)
{
  // Add PDU to the repository.
  {
    unique_uplink_pdu_slot_repository repository = ul_processor->get_pdu_slot_repository(slot);
    repository->add_pusch_pdu(pusch_pdu);
  }

  uplink_slot_processor& slot_processor = ul_processor->get_slot_processor(slot);

  // Stop.
  std::thread stop_thread([this]() { ul_processor->stop(); });

  // Discard.
  std::thread discard_thread([&slot_processor]() { slot_processor.discard_slot(); });

  stop_thread.join();
  discard_thread.join();
}

} // namespace
