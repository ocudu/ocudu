// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "../../../../lib/ofh/transmitter/ofh_data_flow_cplane_scheduling_commands.h"
#include "ocudu/adt/span.h"
#include "ocudu/ofh/ofh_controller.h"
#include <vector>

namespace ocudu {
namespace ofh {
namespace testing {

/// Spy Control-Plane scheduling commands data flow.
class data_flow_cplane_scheduling_commands_spy : public data_flow_cplane_scheduling_commands,
                                                 public operation_controller
{
public:
  struct spy_info {
    unsigned          eaxc      = -1;
    data_direction    direction = data_direction::downlink;
    slot_point        slot;
    filter_index_type filter_type;
    beam_identifier   beam_id = beam_identifier::invalid;
  };

  operation_controller& get_operation_controller() override { return *this; }
  void                  start() override {}
  void                  stop() override {}

  void enqueue_section_type_1_message(const data_flow_cplane_type_1_context& context) override
  {
    has_enqueue_section_type_1_message_method_been_called = true;
    info.slot                                             = context.slot;
    info.eaxc                                             = context.eaxc;
    info.direction                                        = context.direction;
    info.filter_type                                      = context.filter_type;
    info.beam_id                                          = context.beam_id;
    section_type_1_calls.push_back(info);
  }

  void enqueue_section_type_3_prach_message(const struct data_flow_cplane_scheduling_prach_context& context) override
  {
    has_enqueue_section_type_3_message_method_been_called = true;
    info.slot                                             = context.slot;
    info.eaxc                                             = context.eaxc;
    info.direction                                        = data_direction::uplink;
    info.filter_type                                      = context.filter_type;
  }

  data_flow_message_encoding_metrics_collector* get_metrics_collector() override { return nullptr; }

  /// Returns true if the method enqueue section type 1 message has been called, otherwise false.
  bool has_enqueue_section_type_1_method_been_called() const
  {
    return has_enqueue_section_type_1_message_method_been_called;
  }

  /// Returns true if the method enqueue section type 3 message has been called, otherwise false.
  bool has_enqueue_section_type_3_method_been_called() const
  {
    return has_enqueue_section_type_3_message_method_been_called;
  }

  /// Returns the configured eAxC.
  spy_info get_spy_info() const { return info; }

  /// Returns the information of every enqueued section type 1 message, in enqueueing order.
  span<const spy_info> get_section_type_1_calls() const { return section_type_1_calls; }

private:
  bool                  has_enqueue_section_type_1_message_method_been_called = false;
  bool                  has_enqueue_section_type_3_message_method_been_called = false;
  spy_info              info;
  std::vector<spy_info> section_type_1_calls;
};

} // namespace testing
} // namespace ofh
} // namespace ocudu
