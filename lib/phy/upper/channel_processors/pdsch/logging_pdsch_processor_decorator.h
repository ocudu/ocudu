// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/phy/support/precoding_formatters.h"
#include "ocudu/phy/support/support_formatters.h"
#include "ocudu/phy/upper/channel_processors/pdsch/formatters.h"

namespace ocudu {

inline bool is_broadcast_rnti(rnti_t rnti)
{
  return ((rnti < rnti_t::MIN_CRNTI) || (rnti > rnti_t::MAX_CRNTI));
}

class logging_pdsch_processor_decorator : public pdsch_processor, private pdsch_processor_notifier
{
public:
  logging_pdsch_processor_decorator(ocudulog::basic_logger&          logger_,
                                    bool                             enable_logging_broadcast_,
                                    std::unique_ptr<pdsch_processor> processor_) :
    logger(logger_), enable_logging_broadcast(enable_logging_broadcast_), processor(std::move(processor_))
  {
    ocudu_assert(processor, "Invalid processor.");
  }

  void process(resource_grid_writer&                                           grid,
               pdsch_processor_notifier&                                       notifier_,
               static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data_,
               const pdu_t&                                                    pdu_) override
  {
    notifier = &notifier_;
    data     = shared_transport_block(data_.front());
    pdu      = pdu_;

    start = std::chrono::steady_clock::now();
    processor->process(grid, *this, std::move(data_), pdu);
  }

private:
  void on_finish_processing() override
  {
    // Finish time measurement.
    auto end = std::chrono::steady_clock::now();

    // Only print if it is allowed.
    if (enable_logging_broadcast || !is_broadcast_rnti(pdu.rnti)) {
      // Get elapsed time.
      std::chrono::nanoseconds time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

      if (logger.debug.enabled()) {
        // Detailed log information, including a list of all PDU fields.
        logger.debug(pdu.slot.sfn(),
                     pdu.slot.slot_index(),
                     data.get_buffer().data(),
                     data.get_buffer().size(),
                     "PDSCH: {:s} tbs={} {}\n  {:n}",
                     pdu,
                     data.get_buffer().size(),
                     time_ns,
                     pdu);
      } else {
        // Single line log entry.
        logger.info(pdu.slot.sfn(),
                    pdu.slot.slot_index(),
                    data.get_buffer().data(),
                    data.get_buffer().size(),
                    "PDSCH: {:s} tbs={} {}",
                    pdu,
                    data.get_buffer().size(),
                    time_ns);
      }
    }

    // Verify the notifier is valid.
    ocudu_assert(notifier != nullptr, "Detected PDSCH processor notified twice.");

    // Set notifier to invalid pointer before notification.
    pdsch_processor_notifier* notifier_ = notifier;
    notifier                            = nullptr;

    // Release ownership of the TB buffer.
    data.release();

    // Notify original callback. Processor will be available after returning.
    notifier_->on_finish_processing();
  }

  ocudulog::basic_logger&                            logger;
  bool                                               enable_logging_broadcast;
  std::unique_ptr<pdsch_processor>                   processor;
  pdsch_processor_notifier*                          notifier;
  shared_transport_block                             data;
  pdu_t                                              pdu;
  std::chrono::time_point<std::chrono::steady_clock> start;
};

} // namespace ocudu
