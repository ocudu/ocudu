// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdsch_processor_flexible_impl.h"
#include "pdsch_processor_helpers.h"
#include "pdsch_processor_validator_impl.h"
#include "ocudu/adt/scope_exit.h"
#include "ocudu/instrumentation/traces/du_traces.h"
#include "ocudu/ran/sch/sch_segmentation.h"
#include "ocudu/support/rtsan.h"
#include "ocudu/support/tracing/event_tracing.h"

using namespace ocudu;

/// \brief Looks at the output of the validator and, if unsuccessful, fills \c msg with the error message.
///
/// This is used to call the validator inside the process methods only if asserts are active.
[[maybe_unused]] static bool handle_validation(std::string& msg, const error_type<std::string>& err)
{
  bool is_success = err.has_value();
  if (!is_success) {
    msg = err.error();
  }
  return is_success;
}

void pdsch_processor_flexible_impl::process(resource_grid_writer&                                           grid_,
                                            pdsch_processor_notifier&                                       notifier_,
                                            static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data_,
                                            const pdsch_processor::pdu_t&                                   pdu_)
{
  // Makes sure the PDU is valid.
  [[maybe_unused]] std::string msg;
  ocudu_assert(handle_validation(msg, pdsch_processor_validator_impl().is_valid(pdu_)), "{}", msg);

  // Initialize transmission and save inputs.
  initialize_new_transmission(grid_, notifier_, std::move(data_), pdu_);

  if (!async_proc) {
    trace_point process_pdsch_tp = l1_dl_tracer.now();

    // Synchronous CB processing.
    sync_pdsch_cb_processing();

    l1_dl_tracer << trace_event("sync_pdsch_cb_processing", process_pdsch_tp);
  } else {
    // Set the number of asynchronous tasks. It counts as CB processing and DM-RS generation.
    async_task_counter = 2;

    // Add PT-RS to the asynchronous tasks.
    if (config.ptrs) {
      ++async_task_counter;

      // Process PT-RS concurrently.
      auto ptrs_task = [this]() noexcept OCUDU_RTSAN_NONBLOCKING {
        auto execute_on_exit = make_scope_exit([this]() {
          // Decrement asynchronous task counter.
          if (async_task_counter.fetch_sub(1) == 1) {
            // Notify end of the processing.
            notifier->on_finish_processing();
          }
        });

        auto ptrs_generator = ptrs_generator_pool->get();
        if (!ptrs_generator) {
          logger.error("Failed to retrieve PT-RS generator (async).");
          return;
        }

        pdsch_process_ptrs(*grid, *ptrs_generator, config);
      };

      bool success_ptrs = executor.defer(ptrs_task);
      if (!success_ptrs) {
        ptrs_task();
      }
    }

    // Process DM-RS concurrently.
    auto dmrs_task = [this]() noexcept OCUDU_RTSAN_NONBLOCKING {
      auto execute_on_exit = make_scope_exit([this]() {
        // Decrement asynchronous task counter.
        if (async_task_counter.fetch_sub(1) == 1) {
          // Notify end of the processing.
          notifier->on_finish_processing();
        }
      });

      auto dmrs_generator = dmrs_generator_pool->get();
      if (!dmrs_generator) {
        logger.error("Failed to retrieve DM-RS generator (async).");
        return;
      }

      pdsch_process_dmrs(*grid, *dmrs_generator, config);
    };

    bool success_dmrs = executor.defer(dmrs_task);
    if (!success_dmrs) {
      dmrs_task();
    }

    // Fork codeblock processing tasks.
    for (unsigned i_cw = 0; i_cw != nof_codewords; ++i_cw) {
      fork_codeword_processing(i_cw);
    }
  }
}

void pdsch_processor_flexible_impl::initialize_codeword(unsigned i_cw, shared_transport_block& data, const pdu_t& pdu)
{
  // Get the PDSCH processor context for this codeword.
  pdsch_processor_cw_context& codeword_context = processor_context[i_cw];

  // Set the transport block data.
  codeword_context.data = std::move(data);

  // Get the codeword LDPC base graph.
  ldpc_base_graph_type ldpc_base_graph = config.codewords[i_cw].ldpc_base_graph;

  // Calculate transport block size.
  units::bits tbs = units::bytes(codeword_context.data.get_buffer().size()).to_bits();

  // Calculate number of codeblocks.
  codeword_context.nof_cb = compute_nof_codeblocks(tbs, ldpc_base_graph);

  // Number of tasks, or codeblock batches, that the codeword will spawn.
  codeword_context.nof_cb_batches = divide_ceil(codeword_context.nof_cb, nof_cb_per_batch);

  // If the number of codeblocks per batch is very big (e.g., UINT_MAX), the result of the division is zero - ensure
  // at least one codeblock batch is processed.
  if (codeword_context.nof_cb_batches == 0) {
    codeword_context.nof_cb_batches = 1;
  }

  // If only one codeword is transmitted, the processing is asynchronous if the number of codeblocks for the codeword is
  // larger than the maximum number of codeblocks per batch.
  if (!async_proc) {
    async_proc = (codeword_context.nof_cb > max_nof_codeblocks_per_batch);
  }

  // Extract the modulation scheme.
  modulation_scheme modulation = config.codewords[i_cw].modulation;

  // Number of ports for precoding - in case of two codewords, each is mapped to a different half.
  unsigned nof_ports = ports.size() / nof_codewords;

  // Number of transmission layers from precoding configuration.
  unsigned nof_layers = pdu.precoding.get_nof_layers();

  // The layer mapping per codeword is defined in TS38.211 Table 7.3.1.3-1.
  unsigned base_layers = nof_layers / nof_codewords;

  // Number of layers that each codeword is mapped into.
  std::array<unsigned, MAX_NOF_TRANSPORT_BLOCKS> nof_layers_per_cw = {base_layers, nof_layers - base_layers};

  // Number of layers that this codeword is mapped into.
  unsigned nof_layers_cw      = nof_layers_per_cw[i_cw];
  codeword_context.nof_layers = nof_layers_cw;

  // Get a view of the list of ports where the codeword is being mapped to.
  codeword_context.ports = span<const uint8_t>(ports.data(), ports.size()).subspan(i_cw * nof_ports, nof_ports);

  // Extract the codeword-specific precoding.
  codeword_context.precoding =
      config.precoding.slice(interval<uint8_t>::start_and_len(i_cw * nof_layers_per_cw[0], nof_layers_cw),
                             interval<uint8_t>::start_and_len(i_cw * nof_ports, nof_ports));

  // Apply scaling
  float scaling = convert_dB_to_amplitude(-config.ratio_pdsch_data_to_sss_dB);
  scaling *= modulation_mapper::get_modulation_scaling(modulation);
  codeword_context.precoding *= scaling;

  // Extract redundancy version.
  unsigned rv = config.codewords[i_cw].rv;

  // Calculate rate match buffer size.
  units::bits Nref = ldpc::compute_N_ref(config.tbs_lbrm, codeword_context.nof_cb);

  // Derive block processor configuration.
  codeword_context.block_config = pdsch_block_processor::configuration{.rnti            = config.rnti,
                                                                       .modulation      = modulation,
                                                                       .rv              = rv,
                                                                       .n_id            = config.n_id,
                                                                       .scrambling_id   = config.scrambling_id,
                                                                       .ldpc_base_graph = ldpc_base_graph,
                                                                       .nof_re_pdsch    = nof_re_pdsch,
                                                                       .Nref            = Nref,
                                                                       .nof_layers      = nof_layers_cw};

  // Initialize the segmenter.
  segmenter_config segmenter_cfg = {.transport_block_size = units::bytes(codeword_context.data.get_buffer().size()),
                                    .base_graph           = ldpc_base_graph,
                                    .rv                   = rv,
                                    .mod                  = modulation,
                                    .Nref                 = Nref.value(),
                                    .nof_layers           = nof_layers_cw,
                                    .nof_ch_symbols       = nof_layers_cw * nof_re_pdsch};

  const ldpc_segmenter_buffer& segment_buffer = segmenters[i_cw]->new_transmission(segmenter_cfg);
  codeword_context.segment_buffer             = &segment_buffer;

  // If the processing is asynchronous, calculate the starting RE offset for each codeblock in the codeword.
  if (async_proc) {
    // Add the codeword contribution to the total task counter.
    cb_task_counter += codeword_context.nof_cb_batches;

    // Number of segments that will have a short rate-matched length. In TS38.212 Section 5.4.2.1, these correspond to
    // codeblocks whose length E_r is computed by rounding down - floor. For the remaining codewords, the length is
    // rounded up.
    unsigned nof_short_segments = segment_buffer.get_nof_short_segments();

    // Calculate RE offset for each codeblock.
    codeword_context.re_offset.resize(codeword_context.nof_cb);
    unsigned re_count_sum = 0;
    for (unsigned i_cb = 0; i_cb != codeword_context.nof_cb; ++i_cb) {
      // Calculate RM length in RE.
      unsigned rm_length_re = divide_ceil(nof_re_pdsch, codeword_context.nof_cb);
      if (i_cb < nof_short_segments) {
        rm_length_re = nof_re_pdsch / codeword_context.nof_cb;
      }

      // Set RE offset for the resource mapper.
      codeword_context.re_offset[i_cb] = re_count_sum;

      // Increment RE count.
      re_count_sum += rm_length_re;
    }

    // Make sure the codeword length is consistent with the number of REs for data.
    [[maybe_unused]] units::bits bits_per_symbol(get_bits_per_symbol(config.codewords[i_cw].modulation));
    [[maybe_unused]] units::bits cw_length = segment_buffer.get_cw_length();
    ocudu_assert(re_count_sum * nof_layers_cw * bits_per_symbol == cw_length,
                 "RM length sum (i.e., {}) must be equal to the codeword length (i.e., {}).",
                 units::bits(re_count_sum * nof_layers_cw * bits_per_symbol),
                 cw_length);
  }
}

void pdsch_processor_flexible_impl::initialize_new_transmission(
    resource_grid_writer&                                           grid_,
    pdsch_processor_notifier&                                       notifier_,
    static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data_,
    const pdsch_processor::pdu_t&                                   pdu)
{
  using namespace units::literals;

  // Save process parameter inputs.
  grid          = &grid_;
  notifier      = &notifier_;
  config        = pdu;
  nof_codewords = data_.size();

  // Ensure the number of ports is valid.
  ocudu_assert(config.precoding.get_nof_ports() % nof_codewords == 0,
               "The number of ports must be divisible by the number of codewords.");

  // Calculate the number of resource elements used to map PDSCH on the grid. Common for all codewords.
  nof_re_pdsch = pdsch_compute_nof_data_re(config);

  // Populate the full list of resource grid ports.
  ports.resize(config.precoding.get_nof_ports());
  std::iota(ports.begin(), ports.end(), 0);

  // If two codewords are transmitted, the processing is asynchronous.
  async_proc = (nof_codewords > 1);

  // First symbol used in this transmission.
  unsigned start_symbol_index = config.start_symbol_index;

  // Calculate the end symbol index (excluded) and assert it does not exceed the slot boundary.
  unsigned end_symbol_index = config.start_symbol_index + config.nof_symbols;

  // PDSCH OFDM symbol mask.
  symbol_slot_mask symbols;
  symbols.fill(start_symbol_index, end_symbol_index);

  // Prepare the allocation pattern for the resource grid mapper.
  allocation.bwp        = {pdu.bwp_start_rb, pdu.bwp_start_rb + pdu.bwp_size_rb};
  allocation.freq_alloc = pdu.freq_alloc;
  allocation.time_alloc = {pdu.start_symbol_index, pdu.start_symbol_index + pdu.nof_symbols};

  // Reserved REs, including DM-RS and CSI-RS.
  reserved = re_pattern_list(config.reserved);

  // Get DM-RS RE pattern.
  re_pattern dmrs_pattern = get_dmrs_pattern(config.dmrs,
                                             config.bwp_start_rb,
                                             config.bwp_size_rb,
                                             config.nof_cdm_groups_without_data,
                                             config.dmrs_symbol_mask);

  // Merge DM-RS RE pattern into the reserved RE patterns.
  reserved.merge(dmrs_pattern);

  // Initialize the processor for each codeword.
  for (unsigned i_cw = 0; i_cw != nof_codewords; ++i_cw) {
    initialize_codeword(i_cw, data_[i_cw], pdu);
  }
}

void pdsch_processor_flexible_impl::sync_pdsch_cb_processing()
{
  // Synchronous PDSCH processing is only allowed when only one codeword is transmitted.
  ocudu_assert(nof_codewords == 1, "Synchronous processing is only allowed for one codeword.");

  // Get codeword processing context and description.
  pdsch_processor_cw_context& codeword_context = processor_context.front();

  // Get actual codeword data.
  shared_transport_block data = codeword_context.data;

  auto execute_on_exit = make_scope_exit([this]() {
    // No more code block tasks pending to execute, it is now safe to discard the TB buffer.
    processor_context.front().data.release();
    // Notify end of the processing.
    notifier->on_finish_processing();
  });

  // Select codeblock processor.
  auto block_processor = block_processor_pool->get();
  if (!block_processor) {
    logger.error("Failed to retrieve PDSCH block processor.");
    return;
  }

  // Get the LDPC segmenter buffer of the codeword.
  const ldpc_segmenter_buffer* segment_buffer = codeword_context.segment_buffer;

  // Configure the new transmission. Codeword index, start CB index and CB batch length are fixed.
  resource_grid_mapper::symbol_buffer& grid_buffer = block_processor->configure_new_transmission(
      data.get_buffer(), 0, codeword_context.block_config, *segment_buffer, 0, codeword_context.nof_cb);

  // Map PDSCH.
  mapper->map(*grid, grid_buffer, allocation, reserved, codeword_context.ports, codeword_context.precoding);

  // Prepare PT-RS configuration and generate.
  if (config.ptrs) {
    auto ptrs_generator = ptrs_generator_pool->get();
    if (!ptrs_generator) {
      logger.error("Failed to retrieve PT-RS generator.");
      return;
    }

    pdsch_process_ptrs(*grid, *ptrs_generator, config);
  }

  // Process DM-RS.
  {
    auto dmrs_generator = dmrs_generator_pool->get();
    if (!dmrs_generator) {
      logger.error("Failed to retrieve DM-RS generator.");
      return;
    }

    pdsch_process_dmrs(*grid, *dmrs_generator, config);
  }
}

void pdsch_processor_flexible_impl::fork_codeword_processing(unsigned i_cw)
{
  // Get the codeword processing context.
  pdsch_processor_cw_context& codeword_ctx = processor_context[i_cw];
  // Get the number of codeblock batches that the codeword is divided into.
  unsigned nof_cb_batches_cw = codeword_ctx.nof_cb_batches;

  // Spawn a task for each codeblock batch.
  for (unsigned i_task = 0; i_task != nof_cb_batches_cw; ++i_task) {
    unsigned i_batch = nof_cb_batches_cw - 1 - i_task;

    // Create asynchronous task for the codeblock batch.
    auto async_task = [this, i_batch, i_cw, &codeword_ctx]() noexcept OCUDU_RTSAN_NONBLOCKING {
      // Start PDSCH codeblock batch tracing.
      trace_point cb_batch_pdsch_tp = l1_dl_tracer.now();

      // Code to execute when returning.
      auto exec_at_exit = make_scope_exit([this, &codeword_ctx]() {
        // Decrement code block batch counter.
        if (cb_task_counter.fetch_sub(1) == 1) {
          // No more code block tasks pending to execute, it is now safe to discard the TB buffer.
          codeword_ctx.data.release();
          // Decrement asynchronous task counter.
          if (async_task_counter.fetch_sub(1) == 1) {
            // Notify end of the processing.
            notifier->on_finish_processing();
          }
        }
      });

      // Select codeblock processor.
      auto block_processor = block_processor_pool->get();
      if (!block_processor) {
        logger.error("Failed to retrieve PDSCH codeblock processor.");
        return;
      }

      // Calculate the first codeblock index within the batch.
      unsigned first_cb_index = i_batch * nof_cb_per_batch;

      // Limit batch size for the last batch.
      unsigned next_cb_batch_length = std::min(codeword_ctx.nof_cb - first_cb_index, nof_cb_per_batch);

      // Get the LDPC segmenter buffer for this codeword.
      const ldpc_segmenter_buffer* segmenter_buffer = codeword_ctx.segment_buffer;

      // Configure new transmission.
      resource_grid_mapper::symbol_buffer& grid_buffer =
          block_processor->configure_new_transmission(codeword_ctx.data.get_buffer(),
                                                      i_cw,
                                                      codeword_ctx.block_config,
                                                      *segmenter_buffer,
                                                      first_cb_index,
                                                      next_cb_batch_length);

      // Map PDSCH.
      mapper->map(*grid,
                  grid_buffer,
                  allocation,
                  reserved,
                  codeword_ctx.ports,
                  codeword_ctx.precoding,
                  codeword_ctx.re_offset[first_cb_index]);

      // Trace PDSCH.
      l1_dl_tracer << trace_event("CB batch", cb_batch_pdsch_tp);
    };

    // Try to execute task asynchronously.
    bool successful = false;
    if (codeword_ctx.nof_cb_batches > 1) {
      successful = executor.defer(async_task);
    }

    // Execute task locally if it was not enqueued.
    if (!successful) {
      async_task();
    }
  }
}
