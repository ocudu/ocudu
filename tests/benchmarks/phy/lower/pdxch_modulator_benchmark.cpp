// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdxch_baseband_modulator.h"
#include "pdxch_processor_modulator_notifier.h"
#include "ocudu/adt/format.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_pool.h"
#include "ocudu/phy/lower/amplitude_controller/amplitude_controller.h"
#include "ocudu/phy/lower/amplitude_controller/amplitude_controller_factories.h"
#include "ocudu/phy/lower/modulation/modulation_factories.h"
#include "ocudu/phy/lower/modulation/ofdm_modulator.h"
#include "ocudu/phy/lower/sampling_rate.h"
#include "ocudu/phy/support/resource_grid.h"
#include "ocudu/phy/support/resource_grid_writer.h"
#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/phy/support/support_factories.h"
#include "ocudu/ran/antenna_topology.h"
#include "ocudu/ran/beamforming/beam_weights_codebook.h"
#include "ocudu/ran/beamforming/beam_weights_codebook_generator.h"
#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/support/benchmark_utils.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/executors/task_worker_pool.h"
#include "ocudu/support/synchronization/sync_event.h"
#include <cmath>
#include <getopt.h>
#include <optional>
#include <random>
#include <sstream>

using namespace ocudu;

static std::mt19937                    rgen;
static std::normal_distribution<float> dist(0.0F, M_SQRT1_2);
static unsigned                        nof_repetitions = 1000;
static unsigned                        nof_threads     = std::min(8U, std::thread::hardware_concurrency());
static subcarrier_spacing              scs             = subcarrier_spacing::kHz30;
static std::string                     dft_type        = "auto";
static std::vector<sampling_rate>      rates           = {sampling_rate::from_MHz(122.88),
                                                          sampling_rate::from_MHz(245.76),
                                                          sampling_rate::from_MHz(491.52)};
static std::vector<antenna_topology>   topologies      = {
    antenna_topology::one_port,
    antenna_topology::two_port,
    antenna_topology::four_ports,
    antenna_topology::eight_ports,
};

static std::optional<antenna_topology> parse_topology(const std::string& name)
{
  if (name == "one_port") {
    return antenna_topology::one_port;
  }
  if (name == "two_port") {
    return antenna_topology::two_port;
  }
  if (name == "four_ports") {
    return antenna_topology::four_ports;
  }
  if (name == "eight_ports") {
    return antenna_topology::eight_ports;
  }
  return std::nullopt;
}

static std::optional<sampling_rate> parse_sampling_rate(const std::string& value)
{
  char*  end      = nullptr;
  double rate_MHz = std::strtod(value.c_str(), &end);
  if (value.empty() || (*end != '\0') || !std::isnormal(rate_MHz) || (rate_MHz < 0.0) || (rate_MHz >= 1e3)) {
    return std::nullopt;
  }
  return sampling_rate::from_MHz(rate_MHz);
}

class modulator_completion_notifier : public pdxch_processor_modulator_notifier
{
public:
  void on_modulation_completion(pdxch_processor_baseband::slot_result result, resource_grid_context) override
  {
    // Return the buffer to the pool before unblocking the benchmark, as the pool may be destroyed right after.
    result.buffer.reset();
    sync_token.reset();
  }

  void set_token(scoped_sync_token sync_token_) { sync_token.swap(sync_token_); }

private:
  scoped_sync_token sync_token;
};

static void fill_resource_grid(resource_grid& grid, unsigned nof_ports, unsigned nof_symbols)
{
  for (unsigned port = 0; port != nof_ports; ++port) {
    for (unsigned sym = 0; sym != nof_symbols; ++sym) {
      span<cbf16_t> symbol_view = grid.get_writer().get_view(port, sym);
      std::generate(symbol_view.begin(), symbol_view.end(), [&]() { return to_cbf16(cf_t(dist(rgen), dist(rgen))); });
    }
  }
}

class test_grid_pool : public shared_resource_grid::pool_interface
{
public:
  explicit test_grid_pool(std::unique_ptr<resource_grid> grid_in) :
    ref_count_{0}, grid_ptr(grid_in.get()), grid_owned(std::move(grid_in))
  {
  }

  resource_grid& get() override { return *grid_ptr; }
  void           notify_release_scope() override {}

  std::atomic<unsigned> ref_count_;

private:
  resource_grid*                 grid_ptr = nullptr;
  std::unique_ptr<resource_grid> grid_owned;
};

static void benchmark_pdxch_modulator(benchmarker&                                  perf_meas,
                                      cyclic_prefix                                 cp,
                                      sampling_rate                                 srate,
                                      antenna_topology                              topology,
                                      task_executor&                                executor,
                                      std::shared_ptr<ofdm_modulator_factory>       ofdm_mod_factory,
                                      std::shared_ptr<amplitude_controller_factory> amp_factory)
{
  // Create completion notifier.
  modulator_completion_notifier notifier;

  // Beamforming codebook from topology.
  beam_weights_codebook codebook = generate_beam_weights_codebook(topology);

  // Maximum bandwidth that fits in the DFT.
  unsigned dft_size = srate.get_dft_size(scs);
  unsigned max_rb   = (dft_size - 1) / NOF_SUBCARRIERS_PER_RB;
  unsigned bw_rb    = std::min(max_rb, 275U);

  // OFDM modulator.
  ofdm_modulator_configuration ofdm_config{
      .numerology     = to_numerology_value(scs),
      .bw_rb          = bw_rb,
      .dft_size       = dft_size,
      .cp             = cp,
      .scale          = 1.0F,
      .center_freq_Hz = 3.5e9,
  };
  std::unique_ptr<ofdm_symbol_modulator> modulator = ofdm_mod_factory->create_ofdm_symbol_modulator(ofdm_config);
  report_fatal_error_if_not(modulator, "Failed to create OFDM symbol modulator.");

  // Amplitude controller.
  std::unique_ptr<amplitude_controller> amp_ctrl = amp_factory->create();
  report_fatal_error_if_not(amp_ctrl, "Failed to create amplitude controller.");

  // Create the actual baseband modulator.
  pdxch_baseband_modulator modulator_obj(scs, cp, srate, executor, *modulator, *amp_ctrl, codebook, notifier);

  // Compute number of samples per slot, assuming that it is the first slot in the subframe.
  static constexpr unsigned i_slot_sf        = 0;
  unsigned                  nof_symbols      = get_nsymb_per_slot(cp);
  unsigned                  nof_samples_slot = 0;
  for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
    nof_samples_slot += modulator->get_symbol_size(i_slot_sf * nof_symbols + i_symbol);
  }

  // Baseband buffer pool.
  unsigned                       nof_ports = get_total_nof_ports(topology);
  baseband_gateway_buffer_pool   buffer_pool(4, nof_ports, nof_samples_slot);
  std::unique_ptr<resource_grid> grid =
      create_resource_grid_factory()->create(nof_ports, nof_symbols, bw_rb * NOF_SUBCARRIERS_PER_RB);
  report_fatal_error_if_not(grid, "Failed to create resource grid.");

  test_grid_pool pool(std::move(grid));

  // Fill grid with fresh random data.
  fill_resource_grid(pool.get(), nof_ports, nof_symbols);

  // Generate case description.
  std::string descr = fmt::format("{}MHz {}RB {}ant {}", srate.to_MHz(), bw_rb, nof_ports, to_string(scs));

  // Create a synchronization event to wait for the modulation to complete.
  sync_event sync_control;

  perf_meas.new_measure(descr, nof_samples_slot, [&]() {
    // Connect the notifier with the synchronization token.
    notifier.set_token(sync_control.get_token());

    // Create resource grid for modulation.
    shared_resource_grid shared_grid(pool, pool.ref_count_);

    // Get buffer from pool.
    baseband_gateway_buffer_ptr buf = buffer_pool.get();
    report_fatal_error_if_not(buf, "Buffer pool exhausted.");

    bool ok = modulator_obj.handle_request(std::move(buf), shared_grid, resource_grid_context{});
    report_fatal_error_if_not(ok, "Modulator failed to handle the request.");

    // Wait for the modulation to complete.
    sync_control.wait();
  });
}

int main(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:T:C:D:r:t:h")) != -1) {
    switch (opt) {
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'T':
        nof_threads = std::strtol(optarg, nullptr, 10);
        break;
      case 'C': {
        std::string val(optarg);
        if (val == "15") {
          scs = subcarrier_spacing::kHz15;
        } else if (val == "30") {
          scs = subcarrier_spacing::kHz30;
        } else if (val == "60") {
          scs = subcarrier_spacing::kHz60;
        } else if (val == "120") {
          scs = subcarrier_spacing::kHz120;
        } else if (val == "240") {
          scs = subcarrier_spacing::kHz240;
        } else {
          report_fatal_error("Invalid subcarrier spacing: {}", val);
        }
        break;
      }
      case 'D':
        dft_type = optarg;
        break;
      case 'r': {
        rates.clear();
        std::stringstream ss(optarg);
        std::string       value;
        while (std::getline(ss, value, ',')) {
          std::optional<sampling_rate> srate = parse_sampling_rate(value);
          if (!srate.has_value()) {
            report_fatal_error("Invalid sampling rate: {}", value);
          }
          rates.push_back(srate.value());
        }
        break;
      }
      case 't': {
        topologies.clear();
        std::stringstream ss(optarg);
        std::string       name;
        while (std::getline(ss, name, ',')) {
          std::optional<antenna_topology> ant_topology = parse_topology(name);
          if (!ant_topology.has_value()) {
            report_fatal_error("Unknown antenna topology: {}", name);
          }
          topologies.push_back(ant_topology.value());
        }
        break;
      }
      case 'h':
      default:
        fmt::print(
            "Usage: {} [-R repetitions] [-T threads] [-C <scs>] [-D <dft_type>] [-r <rates>] [-t <topologies>] [-h]\n",
            argv[0]);
        fmt::print("  -C <scs>         : subcarrier spacing (15, 30, 60, 120, 240), default 30\n");
        fmt::print("  -D <dft_type>    : DFT library (fftw_slow, fftz, generic), default auto\n");
        fmt::print("  -r <rates>       : sampling rates in MHz, comma-separated (default 122.88,245.76,491.52)\n");
        fmt::print("  -t <topologies>  : antenna topologies, comma-separated (default "
                   "one_port,two_port,four_ports,eight_ports)\n");
        return 0;
    }
  }

  ocudulog::init();

  // Worker pool and executor.
  using task_pool_type = task_worker_pool<concurrent_queue_policy::lockfree_mpmc>;
  auto worker_pool     = std::make_unique<task_pool_type>("mod_pool", nof_threads, 2048, std::chrono::microseconds(1));
  report_fatal_error_if_not(worker_pool, "Failed to create worker pool.");
  std::unique_ptr<task_executor> executor = make_task_worker_pool_executor_ptr(*worker_pool);

  // Select the DFT factory. Pick the selected one by "dft_type", if none is specified ("auto"), use the factory picked
  // by default.
  std::shared_ptr<dft_processor_factory> dft_factory;
  if (dft_type == "fftw") {
    dft_factory = create_dft_processor_factory_fftw_fast();
  } else if (dft_type == "fftw_slow") {
    dft_factory = create_dft_processor_factory_fftw_slow();
  } else if (dft_type == "fftz") {
    dft_factory = create_dft_processor_factory_fftz();
  } else if (dft_type == "generic") {
    dft_factory = create_dft_processor_factory_generic();
  } else if (dft_type == "auto") {
    dft_factory = create_dft_processor_factory();
  } else {
    report_fatal_error("Unkown DFT factory type: {}", dft_type);
  }
  report_fatal_error_if_not(dft_factory, "Failed to create DFT factory ({})", dft_type);

  ofdm_factory_generic_configuration      ofdm_cfg         = {.dft_factory = dft_factory};
  std::shared_ptr<ofdm_modulator_factory> ofdm_mod_factory = create_ofdm_modulator_factory_generic(ofdm_cfg);
  report_fatal_error_if_not(ofdm_mod_factory, "Failed to create OFDM modulator factory.");

  // Create a pool factory of OFDM modulators.
  ofdm_mod_factory = create_ofdm_modulator_pool_factory(std::move(ofdm_mod_factory), nof_threads);
  report_fatal_error_if_not(ofdm_mod_factory, "Failed to create OFDM modulator pool factory.");

  std::shared_ptr<amplitude_controller_factory> amp_factory = create_amplitude_controller_scaling_factory(0.0F);
  report_fatal_error_if_not(amp_factory, "Failed to create amplitude controller factory.");

  benchmarker perf_meas("pdxch_modulator", nof_repetitions);

  for (const auto topology : topologies) {
    for (const sampling_rate rate : rates) {
      benchmark_pdxch_modulator(
          perf_meas, cyclic_prefix::NORMAL, rate, topology, *executor, ofdm_mod_factory, amp_factory);
    }
  }

  perf_meas.print_percentiles_time("microseconds", 1e-3);
  perf_meas.print_percentiles_throughput("samples", 1);

  worker_pool->stop();
  return 0;
}
