// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/phy/upper/downlink_processor.h"

namespace ocudu {

/// Spy implementation of a downlink processor
class downlink_processor_spy : public downlink_processor_base,
                               public downlink_processor_controller,
                               private unique_downlink_processor::downlink_processor_callback
{
public:
  explicit downlink_processor_spy(unsigned id_) : id(id_) {}

  /// Returns the identifier of downlink processor.
  unsigned get_id() const { return id; }

  /// Returns true if the method has been called, otherwise false.
  bool has_configure_resource_grid_method_been_called() const { return configure_resource_grid_method_called; }
  bool has_process_ssb_method_been_called() const { return process_ssb_method_called; }
  bool has_finish_processing_pdus_method_been_called() const { return finish_processing_pdus_method_called; }

  downlink_processor_controller& get_controller() override { return *this; }
  void                           stop() override
  {
    // Do nothing.
  }

private:
  unique_downlink_processor configure_resource_grid(const resource_grid_context& context,
                                                    shared_resource_grid         grid) override
  {
    configure_resource_grid_method_called = true;
    return unique_downlink_processor(*this);
  }

  unsigned id;
  bool     configure_resource_grid_method_called = false;
  bool     process_ssb_method_called             = false;
  bool     finish_processing_pdus_method_called  = false;

  void process_pdcch(const pdcch_processor::pdu_t& pdu) override {}

  void process_pdsch(static_vector<shared_transport_block, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> data,
                     const pdsch_processor::pdu_t&                                                    pdu) override
  {
  }

  void process_ssb(const ssb_processor::pdu_t& pdu) override { process_ssb_method_called = true; }

  void process_nzp_csi_rs(const nzp_csi_rs_generator::config_t& config) override {}

  void process_prs(const prs_generator_configuration& config) override {}

  void finish_processing_pdus() override { finish_processing_pdus_method_called = true; }
};

} // namespace ocudu
