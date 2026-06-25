// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "uci_test_utils.h"
#include "ocudu/scheduler/config/pucch_default_resource.h"
#include <gtest/gtest.h>

using namespace ocudu;

////////////    Structs with expected parameters and PUCCH sched INPUT     ////////////

static constexpr unsigned default_k1 = 4U;

namespace pucch_harq_common_test {

// Parameters to be passed to test for PUCCH output assessment.
struct pucch_alloc_common_harq_test_params {
  unsigned pucch_res_common;
  unsigned n_cces;
};

// Dummy function overload of template <typename T> void testing::internal::PrintTo(const T& value, ::std::ostream* os).
// This prevents valgrind from complaining about uninitialized variables.
void PrintTo(const pucch_alloc_common_harq_test_params& value, ::std::ostream* os)
{
  return;
}

} // namespace pucch_harq_common_test

///////   Test allocation of PUCCH common resources    ///////

using namespace pucch_harq_common_test;

class pucch_alloc_common_harq_test : public ::testing::TestWithParam<pucch_alloc_common_harq_test_params>
{
public:
  pucch_alloc_common_harq_test() :
    params{GetParam()},
    t_bench(test_bench_params{
        .pucch_ded_params = {.f0_or_f1_params =
                                 params.pucch_res_common <= 2
                                     ? std::variant<pucch_f1_params, pucch_f0_params>{pucch_f0_params{}}
                                     : std::variant<pucch_f1_params, pucch_f0_params>{pucch_f1_params{}}},
        .pucch_res_common = params.pucch_res_common,
        .n_cces           = params.n_cces}),
    expected_info(test_helpers::make_pucch_info(
        t_bench.cell_cfg,
        t_bench.cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch.get_cmn(get_pucch_default_resource_index(
            params.n_cces,
            t_bench.cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.value().get_nof_cces(),
            0U)),
        {.harq_ack_nof_bits = 1U}))
  {
  }

protected:
  // Parametrized variables.
  const pucch_alloc_common_harq_test_params params;
  test_bench                                t_bench;
  const pucch_info                          expected_info;
};

// Tests the output of the PUCCH allocator (or PUCCH PDU).
TEST_P(pucch_alloc_common_harq_test, test_pucch_output_info)
{
  std::optional<unsigned> pucch_res_indicator = t_bench.pucch_alloc.alloc_common_harq_ack(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, default_k1, t_bench.dci_info);

  ASSERT_TRUE(pucch_res_indicator.has_value());
  ASSERT_EQ(0, pucch_res_indicator.value());

  ASSERT_FALSE(t_bench.res_grid[t_bench.k0 + default_k1].result.ul.pucchs.empty());
  ASSERT_TRUE(find_pucch_pdu(t_bench.res_grid[t_bench.k0 + default_k1].result.ul.pucchs.unsorted(),
                             [&expected = expected_info](const auto& pdu) { return pucch_info_match(expected, pdu); }));
}

INSTANTIATE_TEST_SUITE_P(,
                         pucch_alloc_common_harq_test,
                         testing::Values(pucch_alloc_common_harq_test_params{.pucch_res_common = 0, .n_cces = 1},
                                         pucch_alloc_common_harq_test_params{.pucch_res_common = 2, .n_cces = 1},
                                         pucch_alloc_common_harq_test_params{.pucch_res_common = 2, .n_cces = 8},
                                         pucch_alloc_common_harq_test_params{.pucch_res_common = 10, .n_cces = 0},
                                         pucch_alloc_common_harq_test_params{.pucch_res_common = 10, .n_cces = 8},
                                         pucch_alloc_common_harq_test_params{.pucch_res_common = 11, .n_cces = 0},
                                         pucch_alloc_common_harq_test_params{.pucch_res_common = 11, .n_cces = 8},
                                         pucch_alloc_common_harq_test_params{.pucch_res_common = 15, .n_cces = 0},
                                         pucch_alloc_common_harq_test_params{.pucch_res_common = 15, .n_cces = 8}),
                         [](const ::testing::TestParamInfo<pucch_alloc_common_harq_test::ParamType>& info_) {
                           return fmt::format(
                               "pucch_res_common_{}_n_cces_{}", info_.param.pucch_res_common, info_.param.n_cces);
                         });

///////   Test multiple allocation of common PUCCH resources    ///////

class pucch_alloc_common_harq_multiple_alloc_test : public ::testing::Test
{
public:
  pucch_alloc_common_harq_multiple_alloc_test() : t_bench(test_bench_params{.pucch_res_common = 11, .n_cces = 1}) {}

protected:
  // Parameters that are passed by the routing to run the tests.
  test_bench t_bench;
};

TEST_F(pucch_alloc_common_harq_multiple_alloc_test, test_pucch_out_of_resources)
{
  // For this specific n_cce value (1) and for d_pri = {0,...,7}, we get 8 r_pucch values. This is the maximum number of
  // UEs we can allocate.
  for (uint16_t n_ue = 0; n_ue != 8; ++n_ue) {
    t_bench.add_ue();
    du_ue_index_t                 ue_idx = to_du_ue_index(static_cast<uint16_t>(t_bench.get_main_ue().ue_index) + n_ue);
    const std::optional<unsigned> pucch_res_indicator = t_bench.pucch_alloc.alloc_common_harq_ack(
        t_bench.res_grid, t_bench.get_ue(ue_idx).crnti, t_bench.k0, default_k1, t_bench.dci_info);
    ASSERT_TRUE(pucch_res_indicator.has_value());
  }

  // If we allocate an extra UE, the scheduler is expected to fail.
  t_bench.add_ue();
  du_ue_index_t                 ue_idx = to_du_ue_index(static_cast<uint16_t>(t_bench.get_main_ue().ue_index) + 8);
  const std::optional<unsigned> pucch_res_indicator_1 = t_bench.pucch_alloc.alloc_common_harq_ack(
      t_bench.res_grid, t_bench.get_ue(ue_idx).crnti, t_bench.k0, default_k1, t_bench.dci_info);
  ASSERT_FALSE(pucch_res_indicator_1.has_value());
}

TEST_F(pucch_alloc_common_harq_multiple_alloc_test, test_on_full_grid)
{
  t_bench.pucch_alloc.alloc_common_harq_ack(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, default_k1, t_bench.dci_info);
  ASSERT_FALSE(t_bench.res_grid[t_bench.k0 + default_k1].result.ul.pucchs.empty());

  // Increase the slot to  try the allocation in a different slot.
  t_bench.slot_indication(++t_bench.sl_tx);

  // Fill the entire grid and verify the PUCCH doesn't get allocated.
  t_bench.fill_all_grid(t_bench.sl_tx + default_k1);

  auto pri = t_bench.pucch_alloc.alloc_common_harq_ack(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, default_k1, t_bench.dci_info);
  ASSERT_FALSE(pri.has_value());
  ASSERT_TRUE(t_bench.res_grid[t_bench.k0 + default_k1].result.ul.pucchs.empty());
}
