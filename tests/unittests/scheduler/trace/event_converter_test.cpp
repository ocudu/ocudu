// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file Tests for the parts of the conversion layer the per-type roundtrip tests cannot cover.
///
/// Every individually serialized type has its own field-exhaustive test under roundtrip/. What is left here is only
/// what that harness structurally cannot check:
///  - Aggregate list wiring: the harness calls each type's own converter directly, never convert_decision_to_fb /
///    convert_fb_to_decision, so it would not notice a list encoded from -- or decoded into -- the wrong sched_result
///    member. Hence make_full_sched_result, which populates every list at once.
///  - The few derived encodings a roundtrip is blind to, because both directions convert rather than copy and so could
///    be wrong the same way. See derived_wire_encodings_are_correct.
///  - convert_cell_cfg_to_fb's ocudu::cell_configuration overload, which has no inverse -- the wire only reconstructs
///    a schedtrace::cell_configuration -- and so cannot be roundtripped at all.

#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip/full_sched_result.h"
#include "tests/test_doubles/scheduler/trace/test_messages.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "ocudu/scheduler/result/sched_result.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace schedtrace;
using namespace schedtrace::roundtrip_test;
using namespace test_helper;

/// Builds a finished buffer with the given decision as root.
static std::vector<uint8_t> decision_to_bytes(const sched_result& result)
{
  flatbuffers::FlatBufferBuilder fbb;
  fbb.Finish(convert_decision_to_fb(fbb, result));
  return {fbb.GetBufferPointer(), fbb.GetBufferPointer() + fbb.GetSize()};
}

/// Converts \p input to a flatbuffer, back to sched_result, then to a flatbuffer again, and checks both buffers are
/// equal. The builder output is deterministic, so byte equality means field equality.
static void check_decision_roundtrip(const sched_result& input)
{
  const std::vector<uint8_t> bytes1 = decision_to_bytes(input);

  sched_result intermediate;
  convert_fb_to_decision(
      intermediate, *flatbuffers::GetRoot<fbs::SlotDecision>(bytes1.data()), make_test_schedtrace_cell_cfg());
  // Check the lists first: byte equality catches a list decoded into the wrong member too, but only as an opaque
  // buffer diff.
  ASSERT_EQ(intermediate.ul.puschs.size(), input.ul.puschs.size());
  ASSERT_EQ(intermediate.ul.pucchs.size(), input.ul.pucchs.size());
  ASSERT_EQ(intermediate.ul.prachs.size(), input.ul.prachs.size());
  ASSERT_EQ(intermediate.ul.srss.size(), input.ul.srss.size());
  ASSERT_EQ(intermediate.dl.dl_pdcchs.size(), input.dl.dl_pdcchs.size());
  ASSERT_EQ(intermediate.dl.ul_pdcchs.size(), input.dl.ul_pdcchs.size());
  ASSERT_EQ(intermediate.dl.bc.sibs.size(), input.dl.bc.sibs.size());
  ASSERT_EQ(intermediate.dl.bc.ssb_info.size(), input.dl.bc.ssb_info.size());
  ASSERT_EQ(intermediate.dl.rar_grants.size(), input.dl.rar_grants.size());
  ASSERT_EQ(intermediate.dl.paging_grants.size(), input.dl.paging_grants.size());
  ASSERT_EQ(intermediate.dl.ue_grants.size(), input.dl.ue_grants.size());
  ASSERT_EQ(intermediate.dl.csi_rs.size(), input.dl.csi_rs.size());

  ASSERT_EQ(bytes1, decision_to_bytes(intermediate));
}

// --- Tests -------------------------------------------------------------------

TEST(event_converter_test, empty_sched_result_roundtrip)
{
  sched_result result;
  result.success = true;
  check_decision_roundtrip(result);
}

TEST(event_converter_test, all_lists_roundtrip)
{
  check_decision_roundtrip(make_full_sched_result());
}

// make_full_sched_result is where all_lists_roundtrip -- and the schedlog log comparison in trace_to_log_test -- get
// their coverage, so a list quietly dropped from it must fail here rather than silently shrink both tests.
TEST(event_converter_test, full_sched_result_populates_every_serialized_list)
{
  const sched_result result = make_full_sched_result();

  EXPECT_FALSE(result.ul.puschs.empty());
  EXPECT_FALSE(result.ul.pucchs.empty());
  EXPECT_FALSE(result.ul.prachs.empty());
  EXPECT_FALSE(result.ul.srss.empty());
  EXPECT_FALSE(result.dl.dl_pdcchs.empty());
  EXPECT_FALSE(result.dl.ul_pdcchs.empty());
  EXPECT_FALSE(result.dl.bc.sibs.empty());
  EXPECT_FALSE(result.dl.bc.ssb_info.empty());
  EXPECT_FALSE(result.dl.rar_grants.empty());
  EXPECT_FALSE(result.dl.paging_grants.empty());
  EXPECT_FALSE(result.dl.ue_grants.empty());
  EXPECT_FALSE(result.dl.csi_rs.empty());
}

TEST(event_converter_test, intervals_are_encoded_as_start_and_length)
{
  sched_result result;
  result.success = true;
  // Intervals whose start, length and stop are all distinct, so each can only match one reading. This is why the
  // roundtrip test values cannot be reused here: they sweep each field to its corner values, and at a corner the three
  // readings collapse into each other (crb_interval{0, 275} has length == stop).
  ssb_information ssb{};
  ssb.ssb_index = 3;
  ssb.crbs      = crb_interval{4, 24};
  ssb.symbols   = ofdm_symbol_range{2, 6};
  result.dl.bc.ssb_info.push_back(ssb);

  flatbuffers::FlatBufferBuilder fbb;
  fbb.Finish(convert_decision_to_fb(fbb, result));
  const auto& decision = *flatbuffers::GetRoot<fbs::SlotDecision>(fbb.GetBufferPointer());

  // Every interval on the wire is (start, length), never (start, stop) -- both converters would have to agree on the
  // same misreading for a roundtrip to notice, and an external reader would then disagree with the schema. Checked
  // once here, on the one struct carrying both a frequency and a symbol interval.
  ASSERT_NE(decision.ssbs(), nullptr);
  const fbs::Ssb& wire_ssb = *decision.ssbs()->Get(0);
  EXPECT_EQ(wire_ssb.crb_start(), 4U);
  EXPECT_EQ(wire_ssb.crb_length(), 20U);
  EXPECT_EQ(wire_ssb.sym_start(), 2U);
  EXPECT_EQ(wire_ssb.sym_length(), 4U);
}

TEST(event_converter_test, cell_cfg_pucch_resources_roundtrip)
{
  test_helpers::test_sched_config_manager cfg_mng{cell_config_builder_params{}};
  const ocudu::cell_configuration&        cell_cfg = *cfg_mng.add_cell(cfg_mng.get_default_cell_config_request());
  const cell_pucch_res_config&            expected = cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch;
  ASSERT_GT(expected.nof_total_res(), 0U);

  flatbuffers::FlatBufferBuilder fbb;
  fbb.Finish(convert_cell_cfg_to_fb(fbb, cell_cfg));
  const auto& start_event = *flatbuffers::GetRoot<fbs::CellStartEvent>(fbb.GetBufferPointer());

  schedtrace::cell_configuration schedtrace_cfg;
  convert_fb_to_cell_cfg(schedtrace_cfg, start_event);

  // What is checked is the gathering: every resource is present, common ones first, then the dedicated ones. Matching
  // starting_prb per index is enough to pin that order -- the resources' own serialization is covered by
  // roundtrip_test.pucch_resource.
  ASSERT_EQ(schedtrace_cfg.pucch_resources.size(), expected.nof_total_res());
  for (unsigned i = 0; i != expected.common.size(); ++i) {
    EXPECT_EQ(schedtrace_cfg.pucch_resources[i].starting_prb, expected.common[i].starting_prb);
  }
  for (unsigned i = 0; i != expected.dedicated.size(); ++i) {
    const pucch_resource& ded = schedtrace_cfg.pucch_resources[expected.common.size() + i];
    EXPECT_EQ(ded.starting_prb, expected.dedicated[i].starting_prb);
    // Dedicated resources reached the wire with their symbols intact, not default-constructed.
    EXPECT_FALSE(ded.syms.empty());
  }
}
