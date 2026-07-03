// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_segmenter_buffer.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"
#include "ocudu/phy/upper/channel_processors/pdsch/pdsch_block_processor.h"
#include "ocudu/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include "ocudu/phy/upper/signal_processors/pdsch/dmrs_pdsch_processor.h"
#include "ocudu/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"

namespace ocudu {

/// \brief Implements a flexible PDSCH processor with parameterizable concurrent codeblock processing and memory
/// footprint.
///
/// \remark The number of PDSCH codeblock processor instances contained in \ref block_processor_pool must be equal to or
/// greater than the number of consumers in \ref executor. Otherwise, an assertion is triggered at runtime.
class pdsch_processor_flexible_impl : public pdsch_processor
{
public:
  /// PDSCH block processor pool type.
  using pdsch_block_processor_pool = bounded_unique_object_pool<pdsch_block_processor>;
  /// PDSCH DM-RS generator pool type.
  using pdsch_dmrs_generator_pool = bounded_unique_object_pool<dmrs_pdsch_processor>;
  /// PDSCH PT-RS generator pool type.
  using pdsch_ptrs_generator_pool = bounded_unique_object_pool<ptrs_pdsch_generator>;

  /// \brief Creates a flexible PDSCH processor with all the dependencies.
  /// \param[in] segmenter_                    LDPC segmenter.
  /// \param[in] mapper_                       Grid mapper.
  /// \param[in] block_processor_pool_         Block processor pool.
  /// \param[in] dmrs_generator_pool_          DM-RS for PDSCH generator.
  /// \param[in] ptrs_generator_pool_          PT-RS for PDSCH generator.
  /// \param[in] executor_                     Asynchronous task executor.
  /// \param[in] max_nof_codeblocks_per_batch_ Maximum number of codeblocks per processing batch.
  pdsch_processor_flexible_impl(std::array<std::unique_ptr<ldpc_segmenter_tx>, MAX_NOF_TRANSPORT_BLOCKS> segmenters_,
                                std::unique_ptr<resource_grid_mapper>                                    mapper_,
                                std::shared_ptr<pdsch_block_processor_pool> block_processor_pool_,
                                std::shared_ptr<pdsch_dmrs_generator_pool>  dmrs_generator_pool_,
                                std::shared_ptr<pdsch_ptrs_generator_pool>  ptrs_generator_pool_,
                                task_executor&                              executor_,
                                unsigned                                    max_nof_codeblocks_per_batch_) :
    logger(ocudulog::fetch_basic_logger("PHY")),
    segmenters(std::move(segmenters_)),
    mapper(std::move(mapper_)),
    block_processor_pool(std::move(block_processor_pool_)),
    dmrs_generator_pool(std::move(dmrs_generator_pool_)),
    ptrs_generator_pool(std::move(ptrs_generator_pool_)),
    executor(executor_),
    max_nof_codeblocks_per_batch(max_nof_codeblocks_per_batch_)
  {
    for (unsigned i_segmenter = 0; i_segmenter != MAX_NOF_TRANSPORT_BLOCKS; ++i_segmenter) {
      ocudu_assert(segmenters[i_segmenter], "Invalid LDPC segmenter pointer.");
    }

    // Homogeneous batches of CBs will be processed per thread, unless otherwise specified.
    nof_cb_per_batch = std::max(1U, max_nof_codeblocks_per_batch);

    ocudu_assert(mapper, "Invalid resource grid mapper pointer.");
    ocudu_assert(block_processor_pool, "Invalid CB processor pool pointer.");
    ocudu_assert(dmrs_generator_pool, "Invalid DM-RS pointer.");
    ocudu_assert(ptrs_generator_pool, "Invalid PT-RS pointer.");
  }

  // See interface for documentation.
  void process(resource_grid_writer&                                           grid,
               pdsch_processor_notifier&                                       notifier,
               static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                    pdu) override;

private:
  /// PDSCH processor configuration parameters for each codeword.
  struct pdsch_processor_cw_context {
    /// Pointer to an LDPC segmenter output buffer interface.
    const ldpc_segmenter_buffer* segment_buffer = nullptr;
    /// Transport block data.
    shared_transport_block data;
    /// PDSCH block processor configuration.
    pdsch_block_processor::configuration block_config;
    /// Number of codeblocks that this codeword is divided into.
    unsigned nof_cb;
    /// Number of codeblock batches (asynchronous tasks) spawned for this codeword processing. Set to zero if codeword
    /// processing is synchronous.
    unsigned nof_cb_batches;
    /// Codeblock resource block offset.
    static_vector<unsigned, MAX_NOF_SEGMENTS> re_offset;
    /// Precoding configuration scaled.
    precoding_configuration precoding;
    /// Port identifiers onto which the codeword is mapped in the resource grid.
    span<const uint8_t> ports;
    /// Number of layers the codeword is mapped into.
    unsigned nof_layers;
  };

  /// Configures a new transmission and saves process() parameters for future uses during an asynchronous execution.
  void initialize_new_transmission(resource_grid_writer&                                           grid,
                                   pdsch_processor_notifier&                                       notifier,
                                   static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
                                   const pdu_t&                                                    pdu);

  /// Configures a new codeword processing context.
  void initialize_codeword(unsigned i_cw, shared_transport_block& data, const pdu_t& pdu);

  /// Synchronous CB processing.
  void sync_pdsch_cb_processing();

  /// Creates code block processing batches and starts the asynchronous processing.
  void fork_codeword_processing(unsigned i_cw);

  /// Logger instance.
  ocudulog::basic_logger& logger;
  /// List of PDSCH processor context per codeword in the current transmission.
  std::array<pdsch_processor_cw_context, MAX_NOF_TRANSPORT_BLOCKS> processor_context;
  /// List of pointers to LDPC segmenters, one per codeword.
  std::array<std::unique_ptr<ldpc_segmenter_tx>, MAX_NOF_TRANSPORT_BLOCKS> segmenters;
  /// Resource grid mapper.
  std::unique_ptr<resource_grid_mapper> mapper;
  /// Pool of block processors.
  std::shared_ptr<pdsch_block_processor_pool> block_processor_pool;
  /// DM-RS processor.
  std::shared_ptr<pdsch_dmrs_generator_pool> dmrs_generator_pool;
  /// PT-RS processor.
  std::shared_ptr<pdsch_ptrs_generator_pool> ptrs_generator_pool;
  /// Asynchronous task executor.
  task_executor& executor;

  resource_grid_writer*     grid;
  pdsch_processor_notifier* notifier;
  pdsch_processor::pdu_t    config;

  /// Maximum number of codeblocks per batch.
  unsigned max_nof_codeblocks_per_batch;
  /// Actual number of codeblocks per batch.
  unsigned nof_cb_per_batch;
  /// Indicates whether the current transmission is concurrent (true) or not.
  bool async_proc = false;
  /// Indicates the number of codewords of the current transmission - one or two.
  uint8_t nof_codewords;
  /// PDSCH transmission allocation pattern.
  resource_grid_mapper::allocation_configuration allocation;
  /// PDSCH transmission reserved elements pattern.
  re_pattern_list reserved;
  /// Pending code block batch counter.
  std::atomic<unsigned> cb_task_counter = {0};
  /// Pending asynchronous task counter (DM-RS and CB processing).
  std::atomic<unsigned> async_task_counter = {0};
  /// List of resource grid ports for the current transmission - each codeword gets a view of their corresponding ports
  /// from the list.
  static_vector<uint8_t, precoding_constants::MAX_NOF_PORTS> ports;
  /// Number of resource elements used to map PDSCH on the resource grid - common for all codewords.
  unsigned nof_re_pdsch;
};

} // namespace ocudu
