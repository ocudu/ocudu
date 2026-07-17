// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "fapi_to_mac_indications_fastpath_translator.h"
#include "ocudu/fapi/p7/messages/rach_indication.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;

namespace {
class mac_cell_rach_handler_dummy : public mac_cell_rach_handler
{
  mac_rach_indication indication;

public:
  void handle_rach_indication(const mac_rach_indication& rach_ind) override { indication = rach_ind; }

  std::optional<rnti_t>
  handle_msga_ccch_sdu(rnti_t ra_rnti, uint8_t rapid, slot_point sl_rx, const ue_con_res_id_t& con_res_id) override
  {
    return std::nullopt;
  }

  std::optional<ue_con_res_id_t> resolve_msga_con_res_id(rnti_t tc_rnti) override { return std::nullopt; }

  const mac_rach_indication& get_indication() const { return indication; }
};
} // namespace

class mac_rach_indication_fixture : public testing::TestWithParam<float>
{
  mac_cell_rach_handler_dummy                 rach_handler;
  fapi_to_mac_indications_fastpath_translator translator;

protected:
  mac_rach_indication_fixture() : translator(0, ocudulog::fetch_basic_logger("FAPI"))
  {
    translator.set_cell_rach_handler(rach_handler);
  }

  void test_pdu()
  {
    const fapi::rach_indication& msg = build_message();
    translator.on_rach_indication(msg);
    check_pdu();
  }

  void test_power_values(float value)
  {
    rssi  = std::clamp(value, -140.F, 30.F);
    power = std::clamp(value, -140.F, 30.F);
    test_pdu();
  }

private:
  unsigned slot            = 0;
  unsigned sfn             = 1;
  float    rssi            = 14.F;
  unsigned slot_index      = 3;
  unsigned start_symbol    = 4;
  unsigned freq_index      = 5;
  float    snr             = 12.F;
  float    power           = 13.F;
  unsigned time_advance_ns = 8;

  fapi::rach_indication build_message()
  {
    fapi::rach_indication fapi_msg;
    auto                  scs = subcarrier_spacing::kHz30;
    fapi_msg.slot             = slot_point(scs, sfn, slot);

    fapi_msg.pdu.avg_rssi     = (rssi + 140) * 1000;
    fapi_msg.pdu.symbol_index = start_symbol;
    fapi_msg.pdu.slot_index   = slot_index;
    fapi_msg.pdu.ra_index     = freq_index;
    fapi_msg.pdu.preambles.emplace_back();
    fapi::rach_indication_pdu_preamble& preamble = fapi_msg.pdu.preambles.back();
    preamble.preamble_snr                        = (snr + 64) * 2;
    preamble.preamble_pwr                        = (power + 140) * 1000;
    preamble.timing_advance_offset               = phy_time_unit::from_seconds(time_advance_ns * 1e-9);

    return fapi_msg;
  }

  void check_pdu()
  {
    const mac_rach_indication&                msg = rach_handler.get_indication();
    const mac_rach_indication::rach_occasion& occ = msg.occasions.front();
    EXPECT_EQ(start_symbol, occ.start_symbol);
    EXPECT_EQ(slot_index, occ.slot_index);
    EXPECT_EQ(freq_index, occ.frequency_index);
    EXPECT_FLOAT_EQ(rssi, occ.rssi_dBFS.value());

    const mac_rach_indication::rach_preamble& pream = occ.preambles.front();
    EXPECT_FLOAT_EQ(power, pream.pwr_dBFS.value());
    EXPECT_EQ(time_advance_ns, std::round(pream.time_advance.to_seconds() * 1e9));
  }
};

TEST_P(mac_rach_indication_fixture, PowerRelatedValuesWorks)
{
  test_power_values(GetParam());
}

INSTANTIATE_TEST_SUITE_P(PowerValues, mac_rach_indication_fixture, testing::Values(-140.F, -20.F, 0.F, 30.F, 63.F));

TEST_F(mac_rach_indication_fixture, CorrectMessageConvertsCorrectly)
{
  test_pdu();
}
