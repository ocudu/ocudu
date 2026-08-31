// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pxsch_bler_test_channel_emulator.h"
#include "pxsch_bler_test_factories.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/phy/support/resource_grid.h"
#include "ocudu/phy/support/support_factories.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_decoder_result.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_processor_result_notifier.h"
#include "ocudu/phy/upper/rx_buffer_pool.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"
#include "ocudu/ran/precoding/precoding_codebooks.h"
#include "ocudu/ran/pusch/pusch_mcs.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"
#include "ocudu/ran/sch/sch_dmrs_power.h"
#include "ocudu/ran/sch/sch_mcs.h"
#include "ocudu/ran/sch/sch_segmentation.h"
#include "ocudu/ran/sch/tbs_calculator.h"
#include "ocudu/support/executors/task_worker_pool.h"
#include <condition_variable>
#include <getopt.h>
#include <mutex>
#include <random>
#include <sstream>
#include <thread>

using namespace ocudu;

static constexpr subcarrier_spacing scs                         = subcarrier_spacing::kHz30;
static constexpr rnti_t             rnti                        = to_rnti(0x1234);
static constexpr unsigned           bwp_start_rb                = 0;
static constexpr unsigned           nof_ofdm_symbols            = 14;
static const symbol_slot_mask       dmrs_mask                   = {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0};
static constexpr unsigned           nof_ldpc_iterations         = 10;
static constexpr dmrs_config_type   dmrs                        = dmrs_config_type::type1;
static constexpr unsigned           nof_cdm_groups_without_data = 2;
static constexpr cyclic_prefix      cy_prefix                   = cyclic_prefix::NORMAL;
static constexpr unsigned           n_id                        = 0;
static constexpr unsigned           scrambling_id               = 0;
static constexpr bool               n_scid                      = false;
static constexpr bool               use_early_stop              = true;
static unsigned                     max_nof_threads             = std::min(8U, std::thread::hardware_concurrency());
static bool                         show_stats                  = true;
static unsigned                     nof_slots                   = 1000;
static std::string                  channel_delay_profile       = "single-tap";
static std::string                  channel_fading_distribution = "uniform-phase";
static float                        sinr_dB                     = 60.0F;
static float                        cfo_Hz                      = 0.0F;
static unsigned                     nof_corrupted_re_per_ofdm_symbol = 0;
static unsigned                     nof_rx_ports                     = 2;
static unsigned                     nof_layers                       = 1;
static unsigned                     bwp_size_rb                      = 273;
static pusch_mcs_table              mcs_table                        = pusch_mcs_table::qam64;
static sch_mcs_index                mcs_index                        = 20;
static bool                         enable_dc_position               = false;
static std::string                  pxsch_type                       = "auto";
static std::string                  eal_arguments                    = "pxsch_bler_test";
static std::string                  rep_rv_sequence_str              = "0";
static ocudulog::basic_levels       log_level                        = ocudulog::basic_levels::warning;

namespace {

const char* to_string(pusch_mcs_table table)
{
  switch (table) {
    case pusch_mcs_table::qam64:
      return "qam64";
    case pusch_mcs_table::qam256:
      return "qam256";
    case pusch_mcs_table::qam64LowSe:
      return "qam64LowSe";
  }
  return "invalid";
}

#if defined(HWACC_PDSCH_ENABLED) && defined(HWACC_PUSCH_ENABLED)
// Separates EAL and non-EAL arguments.
// The function assumes that 'eal_arg' flags the start of the EAL arguments and that no more non-EAL arguments follow.
static std::string capture_eal_args(int* argc, char*** argv)
{
  // Searchs for the EAL args (if any), flagged by 'eal_args', while removing all the rest (except argv[0]).
  bool        eal_found = false;
  char**      mod_argv  = *argv;
  std::string eal_argv  = {mod_argv[0]};
  int         opt_ind   = *argc;
  for (int j = 1; j < opt_ind; ++j) {
    // Search for the 'eal_args' flag (if any).
    if (!eal_found) {
      if (strcmp(mod_argv[j], "eal_args") == 0) {
        // 'eal_args' flag found.
        eal_found = true;
        // Remove all main app arguments starting from that point, while copying them to the EAL argument string.
        mod_argv[j] = NULL;
        for (int k = j + 1; k < opt_ind; ++k) {
          eal_argv += " ";
          eal_argv += mod_argv[k];
          mod_argv[k] = NULL;
        }
        *argc = j;
      }
    }
  }
  *argv = mod_argv;

  return eal_argv;
}
#endif // HWACC_PDSCH_ENABLED && HWACC_PUSCH_ENABLED

std::optional<pusch_mcs_table> to_mcs_table(const char* str)
{
  for (unsigned table_idx = 0; table_idx != 3; ++table_idx) {
    pusch_mcs_table mcs_table = static_cast<pusch_mcs_table>(table_idx);
    if (strcmp(str, to_string(mcs_table)) == 0) {
      return mcs_table;
    }
  }

  return std::nullopt;
}

/// Parses a comma-separated list of unsigned integers (no spaces) into a vector.
static std::vector<unsigned> parse_rep_rv_sequence(std::string_view csv)
{
  std::vector<unsigned> result;
  std::stringstream     ss{std::string{csv}};
  std::string           token;
  while (std::getline(ss, token, ',')) {
    if (token.empty()) {
      continue;
    }
    result.push_back(static_cast<unsigned>(std::strtoul(token.c_str(), nullptr, 10)));
  }
  return result;
}

class pxsch_bler_test
{
public:
  pxsch_bler_test()
  {
    ocudulog::init();
    rep_rv_sequence = parse_rep_rv_sequence(rep_rv_sequence_str);
    setup();
  }

  void run() { loop(); }

  ~pxsch_bler_test() { teardown(); }

private:
  static std::shared_ptr<resource_grid_factory> create_grid_factory()
  {
    std::shared_ptr<channel_precoder_factory> precod_factory = create_channel_precoder_factory("auto");
    report_fatal_error_if_not(precod_factory, "Failed to create channel precoding factory.");

    return create_resource_grid_factory();
  }

  class pdsch_processor_notifier_adaptor : public pdsch_processor_notifier
  {
  public:
    void on_finish_processing() override { completed = true; }

    void wait_for_completion()
    {
      while (!completed.load()) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
      }
    }

  private:
    std::atomic<bool> completed = {false};
  };

  /// Implements a PUSCH decoder notifier adaptor for synchronizing the decoder results.
  class pusch_processor_notifier_adaptor : public pusch_processor_result_notifier
  {
  public:
    void on_uci(const pusch_processor_result_control& uci_) override
    {
      std::unique_lock<std::mutex> lock(mutex);
      uci = uci_;
    }

    void on_sch(const pusch_processor_result_data& sch_) override
    {
      std::unique_lock<std::mutex> lock(mutex);
      completed = true;
      sch       = sch_;
      cvar.notify_all();
    }

    const pusch_processor_result_data& wait_for_completion()
    {
      std::unique_lock<std::mutex> lock(mutex);
      while (!completed) {
        cvar.wait(lock);
      }

      return sch;
    }

    bool                           completed = false;
    pusch_processor_result_control uci;
    pusch_processor_result_data    sch;
    std::mutex                     mutex;
    std::condition_variable        cvar;
  };

  void setup()
  {
    // Prepare executors.
    worker_pool =
        std::make_unique<task_worker_pool<concurrent_queue_policy::locking_mpmc>>("thread", max_nof_threads, 1024);
    executor = std::make_unique<task_worker_pool_executor<concurrent_queue_policy::locking_mpmc>>(*worker_pool);

    // Prepare logging.
    ocudulog::fetch_basic_logger("ALL").set_level(log_level);
    ocudulog::fetch_basic_logger("PHY", true).set_level(log_level);

    // Compute modulation and code scheme.
    sch_mcs_description mcs_descr = pusch_mcs_get_config(mcs_table, mcs_index, false, false);

    // Frequency allocation equal to bandwidth part.
    static prb_interval freq_allocation = {bwp_start_rb, bwp_size_rb};

    // Calculate transport block size.
    tbs_calculator_configuration tbs_config = {};
    tbs_config.mcs_descr                    = mcs_descr;
    tbs_config.n_prb                        = freq_allocation.length();
    tbs_config.nof_layers                   = nof_layers;
    tbs_config.nof_symb_sh                  = nof_ofdm_symbols;
    tbs_config.nof_dmrs_prb = get_nof_re_per_prb(dmrs) * dmrs_mask.count() * nof_cdm_groups_without_data;
    units::bytes tbs        = tbs_calculator_calculate(tbs_config);

    // Select LDPC base graph.
    ldpc_base_graph_type ldpc_base_graph =
        get_ldpc_base_graph(mcs_descr.get_normalised_target_code_rate(), tbs.to_bits());

    // Generate frequency allocation.
    rb_allocation freq_alloc =
        rb_allocation::make_type1(freq_allocation.start(), freq_allocation.length(), std::nullopt);

    // Create PDSCH processor factory.
    std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory =
        create_sw_pdsch_processor_factory(*executor, max_nof_threads + 1, eal_arguments, pxsch_type);
    report_fatal_error_if_not(pdsch_proc_factory, "Failed to create PDSCH processor factory.");

    // Create a PDSCH processor pool factory - creates a PDSCH processor for each retransmission.
    pdsch_proc_factory = create_pdsch_processor_pool(pdsch_proc_factory, rep_rv_sequence.size());
    report_fatal_error_if_not(pdsch_proc_factory, "Failed to create PDSCH processor pool factory.");

    // Create PUSCH processor factory.
    std::shared_ptr<pusch_processor_factory> pusch_proc_factory =
        create_sw_pusch_processor_factory(*executor,
                                          rep_rv_sequence.size() * max_nof_threads + 1,
                                          nof_ldpc_iterations,
                                          use_early_stop,
                                          pxsch_type,
                                          port_channel_estimator_td_interpolation_strategy::average,
                                          channel_equalizer_algorithm_type::zf);
    report_fatal_error_if_not(pusch_proc_factory, "Failed to create PUSCH processor factory.");

    // Create a PUSCH processor pool factory - creates a PUSCH processor for each retransmission.
    pusch_processor_pool_factory_config pusch_proc_pool_config{
        .factory                = pusch_proc_factory,
        .uci_factory            = pusch_proc_factory,
        .nof_regular_processors = static_cast<unsigned>(rep_rv_sequence.size()),
        .nof_uci_processors     = static_cast<unsigned>(rep_rv_sequence.size())};
    pusch_proc_factory = create_pusch_processor_pool(pusch_proc_pool_config);
    report_fatal_error_if_not(pusch_proc_factory, "Failed to create PUSCH processor pool factory.");

    // Create resource grid factory.
    std::shared_ptr<resource_grid_factory> grid_factory = create_grid_factory();
    report_fatal_error_if_not(grid_factory, "Failed to create resource grid factory.");

    // Create PDSCH processor.
    transmitter = pdsch_proc_factory->create();
    report_fatal_error_if_not(transmitter, "Failed to create PDSCH processor.");

    // Create PUSCH processor.
    receiver = pusch_proc_factory->create();
    report_fatal_error_if_not(receiver, "Failed to create PUSCH processor.");

    // Create resource grids.
    for ([[maybe_unused]] unsigned rv : rep_rv_sequence) {
      tx_grids.emplace_back(grid_factory->create(nof_layers, MAX_NSYMB_PER_SLOT, MAX_NOF_SUBCARRIERS));
      rx_grids.emplace_back(grid_factory->create(nof_rx_ports, MAX_NSYMB_PER_SLOT, MAX_NOF_SUBCARRIERS));
    }

    // Calculate number of codeblocks.
    nof_codeblocks = compute_nof_codeblocks(units::bits(tbs), ldpc_base_graph);

    // Prepare receive soft buffer pool.
    rx_buffer_pool_config buffer_pool_config;
    buffer_pool_config.max_codeblock_size   = ldpc::MAX_CODEBLOCK_SIZE;
    buffer_pool_config.nof_buffers          = 1;
    buffer_pool_config.nof_codeblocks       = nof_codeblocks;
    buffer_pool_config.expire_timeout_slots = 1;
    buffer_pool_config.external_soft_bits   = false;

    // Create buffer pool.
    buffer_pool = create_rx_buffer_pool(buffer_pool_config);
    report_error_if_not(buffer_pool, "Failed to create buffer pool.");

    // Prepare PDSCH processor configuration.
    pdsch_config.reserve(rep_rv_sequence.size());
    for (unsigned i_rep = 0, rep_end = rep_rv_sequence.size(); i_rep != rep_end; ++i_rep) {
      unsigned rv = rep_rv_sequence[i_rep];

      pdsch_config.push_back(pdsch_processor::pdu_t{
          .context          = std::nullopt,
          .slot             = slot_point(to_numerology_value(scs), 0),
          .rnti             = rnti,
          .bwp_size_rb      = bwp_size_rb,
          .bwp_start_rb     = bwp_start_rb,
          .cp               = cy_prefix,
          .codewords        = {pdsch_processor::codeword_description{mcs_descr.modulation, rv, ldpc_base_graph}},
          .n_id             = n_id,
          .ref_point        = pdsch_processor::pdu_t::PRB0,
          .dmrs_symbol_mask = dmrs_mask,
          .dmrs             = dmrs,
          .scrambling_id    = scrambling_id,
          .n_scid           = n_scid,
          .nof_cdm_groups_without_data = nof_cdm_groups_without_data,
          .freq_alloc                  = freq_alloc,
          .start_symbol_index          = 0,
          .nof_symbols                 = nof_ofdm_symbols,
          .tbs_lbrm                    = tbs_lbrm_default,
          .reserved                    = {},
          .ratio_pdsch_dmrs_to_sss_dB  = get_sch_to_dmrs_ratio_dB(nof_cdm_groups_without_data),
          .ratio_pdsch_data_to_sss_dB  = 0.0F,
          .precoding                   = precoding_configuration::make_wideband(make_identity(nof_layers))});

      static_vector<uint8_t, MAX_PORTS> rx_ports(nof_rx_ports);
      std::iota(rx_ports.begin(), rx_ports.end(), 0U);

      pusch_processor::dmrs_configuration dmrs_config = {.dmrs                        = dmrs,
                                                         .scrambling_id               = scrambling_id,
                                                         .n_scid                      = n_scid,
                                                         .nof_cdm_groups_without_data = nof_cdm_groups_without_data};

      pusch_processor::codeword_description cw_descr = {rv, ldpc_base_graph, i_rep == 0, i_rep == rep_end - 1};

      // Prepare PUSCH processor configuration.
      pusch_config.push_back(pusch_processor::pdu_t{
          .harq_id            = INVALID_HARQ_ID,
          .slot               = slot_point(to_numerology_value(scs), 0),
          .rnti               = rnti,
          .bwp_size_rb        = bwp_size_rb,
          .bwp_start_rb       = bwp_start_rb,
          .cp                 = cy_prefix,
          .mcs_descr          = mcs_descr,
          .codeword           = cw_descr,
          .uci                = {},
          .n_id               = n_id,
          .nof_tx_layers      = nof_layers,
          .rx_ports           = rx_ports,
          .dmrs_symbol_mask   = dmrs_mask,
          .dmrs               = dmrs_config,
          .freq_alloc         = freq_alloc,
          .start_symbol_index = 0,
          .nof_symbols        = nof_ofdm_symbols,
          .tbs_lbrm           = tbs_lbrm_default,
          .dc_position = enable_dc_position ? std::optional(bwp_size_rb * NOF_SUBCARRIERS_PER_RB / 2) : std::nullopt,
          .n_rapid     = std::nullopt});
    }

    // Resize data to accomodate the transport block.
    tx_data.resize(tbs.value());
    rx_data.resize(tbs.value());

    emulator = std::make_unique<channel_emulator>(channel_delay_profile,
                                                  channel_fading_distribution,
                                                  sinr_dB,
                                                  cfo_Hz,
                                                  nof_corrupted_re_per_ofdm_symbol,
                                                  nof_layers,
                                                  nof_rx_ports,
                                                  MAX_NOF_SUBCARRIERS,
                                                  nof_ofdm_symbols,
                                                  max_nof_threads,
                                                  scs,
                                                  *executor);
  }

  void teardown()
  {
    worker_pool->stop();
    ocudulog::flush();
  }

  void print_stats(double completion_percent)
  {
    double crc_bler  = static_cast<double>(crc_error_count) / static_cast<double>(count);
    double data_bler = static_cast<double>(data_error_count) / static_cast<double>(count);
    double mean_iterations =
        static_cast<double>(count_iterations) / static_cast<double>(count * nof_codeblocks * rep_rv_sequence.size());

    fmt::print("\r[{:>5.1f}%] "
               "Iterations={{{:<2} {:<2} {:<3.1f}}}; "
               "Repetitions={{{:<2} {:<2} {:<3.1f}}}; "
               "BLER={:.10f}/{:.10f}; "
               "SINR={{{:+.2f} {:+.2f} {:+.2f}}}; "
               "EVM={{{:.3f} {:.3f} {:.3f}}}; "
               "TA={{{:.2f} {:.2f} {:.2f}}}us; "
               "CFO={{{:.2f} {:.2f} {:.2f}}}Hz; "
               "pxsch={}",
               completion_percent,
               min_iterations,
               max_iterations,
               mean_iterations,
               repetitions_stats.get_min(),
               repetitions_stats.get_max(),
               repetitions_stats.get_mean(),
               crc_bler,
               data_bler,
               sinr_stats.get_min(),
               sinr_stats.get_max(),
               sinr_stats.get_mean(),
               evm_stats.get_min(),
               evm_stats.get_max(),
               evm_stats.get_mean(),
               ta_stats_us.get_min(),
               ta_stats_us.get_max(),
               ta_stats_us.get_mean(),
               cfo_stats_Hz.get_min(),
               cfo_stats_Hz.get_max(),
               cfo_stats_Hz.get_mean(),
               pxsch_type);
  }

  void loop()
  {
    std::mt19937 rgen(0);

    // Iterate different seeds.
    for (unsigned n = 0; n != nof_slots; ++n) {
      // Generate random data.
      std::generate(tx_data.begin(), tx_data.end(), [&rgen]() { return static_cast<uint8_t>(rgen() & 0xff); });

      // Process PDSCH.
      std::vector<pdsch_processor_notifier_adaptor> tx_notifiers(rep_rv_sequence.size());
      for (unsigned i_rep = 0, rep_end = rep_rv_sequence.size(); i_rep != rep_end; ++i_rep) {
        // Select notifier.
        pdsch_processor_notifier_adaptor& tx_notifier = tx_notifiers[i_rep];

        // Process PDSCH
        transmitter->process(
            tx_grids[i_rep]->get_writer(), tx_notifier, {shared_transport_block(tx_data)}, pdsch_config[i_rep]);
      }

      // Wait for PDSCH processing to complete.
      for (pdsch_processor_notifier_adaptor& tx_notifier : tx_notifiers) {
        tx_notifier.wait_for_completion();
      }

      // Run channel emulator for retransmission.
      for (unsigned i_rep = 0, rep_end = rep_rv_sequence.size(); i_rep != rep_end; ++i_rep) {
        emulator->run(rx_grids[i_rep]->get_writer(), tx_grids[i_rep]->get_reader());
      }

      // Process PUSCH.
      std::vector<pusch_processor_notifier_adaptor> rx_notifiers(rep_rv_sequence.size());
      for (unsigned i_rep = 0, rep_end = rep_rv_sequence.size(); i_rep != rep_end; ++i_rep) {
        bool new_data = i_rep == 0;

        // Get a receive buffer.
        unique_rx_buffer buffer = buffer_pool->get_pool().reserve(
            pusch_config[i_rep].slot, trx_buffer_identifier(rnti, 0), nof_codeblocks, new_data);
        report_error_if_not(buffer, "Invalid buffer.");

        // Fork PUSCH reception.
        receiver->process(
            rx_data, std::move(buffer), rx_notifiers[i_rep], rx_grids[i_rep]->get_reader(), pusch_config[i_rep]);
      }

      // Wait for all PUSCH processing to complete.
      std::vector<pusch_processor_result_data> sch_results;
      sch_results.reserve(rep_rv_sequence.size());
      for (pusch_processor_notifier_adaptor& rx_notifier : rx_notifiers) {
        sch_results.push_back(rx_notifier.wait_for_completion());
      }

      // Extract the latest SCH result.
      const auto sch_result_it =
          std::find_if(sch_results.begin(), sch_results.end(), [](const pusch_processor_result_data& data) {
            return data.data.tb_crc_ok;
          });
      repetitions_stats.update(std::min(std::distance(sch_results.begin(), sch_result_it) + 1UL, sch_results.size()));

      // Accumulate counters.
      ++count;
      if (sch_result_it == sch_results.end()) {
        ++crc_error_count;
      }
      if (tx_data != rx_data) {
        ++data_error_count;
      }

      for (const pusch_processor_result_data& sch_result : sch_results) {
        max_iterations = std::max(sch_result.data.ldpc_decoder_stats.get_max(), max_iterations);
        min_iterations = std::min(sch_result.data.ldpc_decoder_stats.get_min(), min_iterations);
        count_iterations += static_cast<uint64_t>(sch_result.data.ldpc_decoder_stats.get_nof_observations() *
                                                  sch_result.data.ldpc_decoder_stats.get_mean());
        if (sch_result.csi.get_total_evm().has_value()) {
          evm_stats.update(sch_result.csi.get_total_evm().value());
        }
        if (sch_result.csi.get_sinr_dB().has_value()) {
          sinr_stats.update(sch_result.csi.get_sinr_dB().value());
        }
        if (sch_result.csi.get_time_alignment().has_value()) {
          ta_stats_us.update(sch_result.csi.get_time_alignment()->to_seconds() * 1e6);
        }
        if (sch_result.csi.get_cfo_Hz().has_value()) {
          cfo_stats_Hz.update(*sch_result.csi.get_cfo_Hz());
        }
      }

      // Increment slots.
      for (unsigned i_rep = 0, rep_end = rep_rv_sequence.size(); i_rep != rep_end; ++i_rep) {
        ++pdsch_config[i_rep].slot;
        ++pusch_config[i_rep].slot;
      }

      // Set following line to 1 for printing partial results.
      if (show_stats && (n % 100 == 0)) {
        print_stats(static_cast<double>(n) / static_cast<double>(nof_slots) * 100.0);
      }
    }

    // Print final results.
    print_stats(100.0);
  }

  unsigned                    nof_codeblocks;
  uint64_t                    count            = 0;
  uint64_t                    crc_error_count  = 0;
  uint64_t                    data_error_count = 0;
  unsigned                    max_iterations   = std::numeric_limits<unsigned>::min();
  unsigned                    min_iterations   = std::numeric_limits<unsigned>::max();
  uint64_t                    count_iterations = 0;
  sample_statistics<unsigned> repetitions_stats;
  sample_statistics<float>    sinr_stats;
  sample_statistics<float>    evm_stats;
  sample_statistics<float>    ta_stats_us;
  sample_statistics<float>    cfo_stats_Hz;

  std::unique_ptr<pdsch_processor>            transmitter;
  std::unique_ptr<pusch_processor>            receiver;
  std::vector<std::unique_ptr<resource_grid>> tx_grids;
  std::vector<std::unique_ptr<resource_grid>> rx_grids;
  std::unique_ptr<rx_buffer_pool_controller>  buffer_pool;

  std::vector<unsigned> rep_rv_sequence;

  std::vector<pdsch_processor::pdu_t> pdsch_config;
  std::vector<pusch_processor::pdu_t> pusch_config;

  std::vector<uint8_t> tx_data;
  std::vector<uint8_t> rx_data;

  std::unique_ptr<task_worker_pool<concurrent_queue_policy::locking_mpmc>>          worker_pool;
  std::unique_ptr<task_worker_pool_executor<concurrent_queue_policy::locking_mpmc>> executor;

  std::unique_ptr<channel_emulator> emulator;
}; // namespace
} // namespace

static void usage(std::string_view prog)
{
  fmt::print(
      "Usage: {} [-C X] [-F X] [-S X] [-N X] [-P X] [-R X] [-M X] [-m X] [-D] [-T X] [-V X] [-j X] [eal_args ...]\n",
      prog);
  fmt::print("\t-C       Channel delay profile: single-tap, TDLA, TDLB or TDLC. [Default {}]\n", channel_delay_profile);
  fmt::print("\t-F       Channel fading distribution: uniform-phase, rayleigh or butler. [Default {}]\n",
             channel_fading_distribution);
  fmt::print("\t-D       Toggle enable DC position. [Default {}]\n", enable_dc_position);
  fmt::print("\t-S       SINR. [Default {}]\n", sinr_dB);
  fmt::print("\t-N       Number of corrupted RE per OFDM symbol. [Default {}]\n", nof_corrupted_re_per_ofdm_symbol);
  fmt::print("\t-P       Number of receive ports. [Default {}]\n", nof_rx_ports);
  fmt::print("\t-L       Number of transmit layers. It must not exceed the number of ports. [Default {}]\n",
             nof_layers);
  fmt::print("\t-B       Number of allocated PRBs (same as BWP size). [Default {}]\n", bwp_size_rb);
  fmt::print("\t-M       MCS table. [{} {} {}][Default {}]\n",
             to_string(pusch_mcs_table::qam64),
             to_string(pusch_mcs_table::qam256),
             to_string(pusch_mcs_table::qam64LowSe),
             to_string(mcs_table));
  fmt::print("\t-m       MCS index. [Default {}]\n", mcs_index);
  fmt::print("\t-R       Number of slots to process. [Default {}]\n", nof_slots);
  fmt::print("\t-T       PxSCH implementation type [auto,acc100][Default {}]\n", pxsch_type);
  fmt::print("\t-V       Retransmission RV/RV sequence (comma-separated, no spaces, e.g. 0,4,2,3) [Default {}]\n",
             rep_rv_sequence_str);
  fmt::print("\t-l       Log level: none, error, warning, info, debug. [Default {}]\n",
             ocudulog::basic_level_to_string(log_level));
  fmt::print("\teal_args EAL arguments\n");
  fmt::print("\t-v       Toggle preliminary stats. [Default {}]\n", show_stats);
  fmt::print("\t-j       Number of threads. [Default {}]\n", max_nof_threads);
  fmt::print("\t-h       Print this message.\n");
}

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "C:F:S:N:P:L:R:B:M:m:V:D:T:vl:j:h")) != -1) {
    switch (opt) {
      case 'C':
        if (optarg != nullptr) {
          channel_delay_profile = std::string(optarg);
        }
        break;
      case 'F':
        if (optarg != nullptr) {
          channel_fading_distribution = std::string(optarg);
        }
        break;
      case 'D':
        enable_dc_position = !enable_dc_position;
        break;
      case 'S':
        sinr_dB = std::strtof(optarg, nullptr);
        break;
      case 'N':
        nof_corrupted_re_per_ofdm_symbol = std::strtol(optarg, nullptr, 10);
        break;
      case 'P':
        nof_rx_ports = std::strtol(optarg, nullptr, 10);
        break;
      case 'L':
        nof_layers = std::strtol(optarg, nullptr, 10);
        break;
      case 'B':
        bwp_size_rb = std::strtol(optarg, nullptr, 10);
        break;
      case 'M':
        if (optarg != nullptr) {
          std::optional<pusch_mcs_table> table = to_mcs_table(optarg);
          if (!table) {
            fmt::print("Invalid MCS table {}.", optarg);
            usage(argv[0]);
            std::exit(-1);
          }
          mcs_table = table.value();
        }
        break;
      case 'm':
        mcs_index = std::strtol(optarg, nullptr, 10);
        break;
      case 'V':
        if (optarg != nullptr) {
          rep_rv_sequence_str = std::string(optarg);
        }
        break;
      case 'R':
        nof_slots = std::strtol(optarg, nullptr, 10);
        break;
      case 'T':
        pxsch_type = std::string(optarg);
        break;
      case 'l':
        if (optarg != nullptr) {
          std::optional<ocudulog::basic_levels> level = ocudulog::str_to_basic_level(optarg);
          if (!level.has_value()) {
            fmt::println("Invalid log level {}.", optarg);
            usage(argv[0]);
            std::exit(-1);
          }
          log_level = level.value();
        }
        break;
      case 'v':
        show_stats = !show_stats;
        break;
      case 'j':
        if (optarg != nullptr) {
          max_nof_threads = std::strtol(optarg, nullptr, 10);
        }
        break;
      case 'h':
      default:
        usage(argv[0]);
        std::exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
#if defined(HWACC_PDSCH_ENABLED) && defined(HWACC_PUSCH_ENABLED)
  // Separate EAL and non-EAL arguments.
  eal_arguments = capture_eal_args(&argc, &argv);
#endif // HWACC_PDSCH_ENABLED && HWACC_PUSCH_ENABLED

  parse_args(argc, argv);

  pxsch_bler_test test;
  test.run();

  return 0;
}
