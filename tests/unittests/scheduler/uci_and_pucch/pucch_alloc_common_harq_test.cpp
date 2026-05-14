// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "uci_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

////////////    Structs with expected parameters and PUCCH sched INPUT     ////////////

static constexpr unsigned NOF_RBS    = 52;
static constexpr unsigned default_k1 = 4U;

namespace pucch_harq_common_test {

// Parameters to be passed to test for PUCCH output assessment.
struct pucch_alloc_common_harq_test_params {
  struct {
    unsigned pucch_res_common;
    unsigned n_cces;
  } input;
  struct {
    pucch_format      format;
    prb_interval      prbs;
    unsigned          second_hop_prb;
    ofdm_symbol_range symbols;
    uint8_t           initial_cyclic_shift;
    uint8_t           time_domain_occ;
    unsigned          pri;
  } output;
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
                                 params.input.pucch_res_common <= 2
                                     ? std::variant<pucch_f1_params, pucch_f0_params>{pucch_f0_params{}}
                                     : std::variant<pucch_f1_params, pucch_f0_params>{pucch_f1_params{}}},
        .pucch_res_common = params.input.pucch_res_common,
        .n_cces           = params.input.n_cces}),
    expected_info(
        test_helpers::make_common_pucch_info(&t_bench.cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params,
                                             t_bench.cell_cfg.params.pci,
                                             params.output.format,
                                             params.output.prbs,
                                             params.output.second_hop_prb,
                                             params.output.symbols,
                                             params.output.initial_cyclic_shift,
                                             params.output.time_domain_occ))
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
  ASSERT_EQ(params.output.pri, pucch_res_indicator.value());

  ASSERT_FALSE(t_bench.res_grid[t_bench.k0 + default_k1].result.ul.pucchs.empty());
  ASSERT_TRUE(find_pucch_pdu(t_bench.res_grid[t_bench.k0 + default_k1].result.ul.pucchs,
                             [&expected = expected_info](const auto& pdu) { return pucch_info_match(expected, pdu); }));
}

INSTANTIATE_TEST_SUITE_P(
    ,
    pucch_alloc_common_harq_test,
    testing::Values(pucch_alloc_common_harq_test_params{.input = {.pucch_res_common = 0, .n_cces = 1},
                                                        .output =
                                                            {
                                                                .format               = pucch_format::FORMAT_0,
                                                                .prbs                 = prb_interval{0, 1},
                                                                .second_hop_prb       = NOF_RBS - 1,
                                                                .symbols              = ofdm_symbol_range{12, 14},
                                                                .initial_cyclic_shift = 0,
                                                                .time_domain_occ      = 0,
                                                                .pri                  = 0,
                                                            }},
                    pucch_alloc_common_harq_test_params{.input  = {.pucch_res_common = 2, .n_cces = 1},
                                                        .output = {.format               = pucch_format::FORMAT_0,
                                                                   .prbs                 = prb_interval{3, 4},
                                                                   .second_hop_prb       = NOF_RBS - 4,
                                                                   .symbols              = ofdm_symbol_range{12, 14},
                                                                   .initial_cyclic_shift = 0,
                                                                   .time_domain_occ      = 0,
                                                                   .pri                  = 0}},
                    pucch_alloc_common_harq_test_params{.input  = {.pucch_res_common = 2, .n_cces = 8},
                                                        .output = {.format               = pucch_format::FORMAT_0,
                                                                   .prbs                 = prb_interval{3, 4},
                                                                   .second_hop_prb       = NOF_RBS - 4,
                                                                   .symbols              = ofdm_symbol_range{12, 14},
                                                                   .initial_cyclic_shift = 8,
                                                                   .time_domain_occ      = 0,
                                                                   .pri                  = 0}},
                    pucch_alloc_common_harq_test_params{.input  = {.pucch_res_common = 10, .n_cces = 0},
                                                        .output = {.format               = pucch_format::FORMAT_1,
                                                                   .prbs                 = prb_interval{4, 5},
                                                                   .second_hop_prb       = NOF_RBS - 5,
                                                                   .symbols              = ofdm_symbol_range{4, 14},
                                                                   .initial_cyclic_shift = 0,
                                                                   .time_domain_occ      = 0,
                                                                   .pri                  = 0}},
                    pucch_alloc_common_harq_test_params{.input  = {.pucch_res_common = 10, .n_cces = 8},
                                                        .output = {.format               = pucch_format::FORMAT_1,
                                                                   .prbs                 = prb_interval{4, 5},
                                                                   .second_hop_prb       = NOF_RBS - 5,
                                                                   .symbols              = ofdm_symbol_range{4, 14},
                                                                   .initial_cyclic_shift = 6,
                                                                   .time_domain_occ      = 0,
                                                                   .pri                  = 0}},
                    pucch_alloc_common_harq_test_params{.input  = {.pucch_res_common = 11, .n_cces = 0},
                                                        .output = {.format               = pucch_format::FORMAT_1,
                                                                   .prbs                 = prb_interval{0, 1},
                                                                   .second_hop_prb       = NOF_RBS - 1,
                                                                   .symbols              = ofdm_symbol_range{0, 14},
                                                                   .initial_cyclic_shift = 0,
                                                                   .time_domain_occ      = 0,
                                                                   .pri                  = 0}},
                    pucch_alloc_common_harq_test_params{.input  = {.pucch_res_common = 11, .n_cces = 8},
                                                        .output = {.format               = pucch_format::FORMAT_1,
                                                                   .prbs                 = prb_interval{1, 2},
                                                                   .second_hop_prb       = NOF_RBS - 2,
                                                                   .symbols              = ofdm_symbol_range{0, 14},
                                                                   .initial_cyclic_shift = 0,
                                                                   .time_domain_occ      = 0,
                                                                   .pri                  = 0}},
                    pucch_alloc_common_harq_test_params{.input  = {.pucch_res_common = 15, .n_cces = 0},
                                                        .output = {.format               = pucch_format::FORMAT_1,
                                                                   .prbs                 = prb_interval{13, 14},
                                                                   .second_hop_prb       = NOF_RBS - 14,
                                                                   .symbols              = ofdm_symbol_range{0, 14},
                                                                   .initial_cyclic_shift = 0,
                                                                   .time_domain_occ      = 0,
                                                                   .pri                  = 0}},
                    pucch_alloc_common_harq_test_params{.input  = {.pucch_res_common = 15, .n_cces = 8},
                                                        .output = {.format               = pucch_format::FORMAT_1,
                                                                   .prbs                 = prb_interval{13, 14},
                                                                   .second_hop_prb       = NOF_RBS - 14,
                                                                   .symbols              = ofdm_symbol_range{0, 14},
                                                                   .initial_cyclic_shift = 6,
                                                                   .time_domain_occ      = 0,
                                                                   .pri                  = 0}}),
    [](const ::testing::TestParamInfo<pucch_alloc_common_harq_test::ParamType>& info_) {
      return fmt::format("pucch_res_common_{}_n_cces_{}", info_.param.input.pucch_res_common, info_.param.input.n_cces);
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

TEST_F(pucch_alloc_common_harq_multiple_alloc_test, test_pucch_double_alloc)
{
  const std::optional<unsigned> pucch_res_indicator = t_bench.pucch_alloc.alloc_common_harq_ack(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, default_k1, t_bench.dci_info);
  ASSERT_TRUE(pucch_res_indicator.has_value());

  // If we allocate the same UE twice, the scheduler is expected to fail, as we don't support PUCCH multiplexing on
  // PUCCH common resources.
  std::optional<unsigned> pucch_res_indicator_1 = t_bench.pucch_alloc.alloc_common_harq_ack(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, default_k1, t_bench.dci_info);
  ASSERT_FALSE(pucch_res_indicator_1.has_value());
}

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
  const std::optional<unsigned> pucch_res_indicator_1 = t_bench.pucch_alloc.alloc_common_harq_ack(
      t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.k0, default_k1, t_bench.dci_info);
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
