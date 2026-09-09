// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/asn1/rrc_nr/rrc_nr.h"
#include "ocudu/rrc/rrc_config.h"
#include "ocudu/rrc/rrc_du_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

namespace {

/// Packs a MeasurementTimingConfiguration as an Xn-C peer advertises it for one of its served cells.
byte_buffer pack_meas_timing_cfg(const std::vector<std::optional<uint32_t>>& carrier_freqs)
{
  asn1::rrc_nr::meas_timing_cfg_s meas_timing_cfg;
  auto& meas_timing_list = meas_timing_cfg.crit_exts.set_c1().set_meas_timing_conf().meas_timing;

  meas_timing_list.resize(carrier_freqs.size());
  for (unsigned i = 0; i != carrier_freqs.size(); ++i) {
    if (!carrier_freqs[i].has_value()) {
      continue;
    }
    auto& meas_timing                                   = meas_timing_list[i];
    meas_timing.freq_and_timing_present                 = true;
    meas_timing.freq_and_timing.carrier_freq            = carrier_freqs[i].value();
    meas_timing.freq_and_timing.ssb_subcarrier_spacing  = asn1::rrc_nr::subcarrier_spacing_opts::khz30;
    meas_timing.freq_and_timing.ssb_meas_timing_cfg.dur = asn1::rrc_nr::ssb_mtc_s::dur_opts::sf1;
    meas_timing.freq_and_timing.ssb_meas_timing_cfg.periodicity_and_offset.set_sf20() = 0;
  }

  byte_buffer   buf;
  asn1::bit_ref bref{buf};
  EXPECT_EQ(meas_timing_cfg.pack(bref), asn1::OCUDUASN_SUCCESS);
  return buf;
}

} // namespace

class rrc_du_ssb_arfcn_test : public ::testing::Test
{
protected:
  std::unique_ptr<rrc_du> rrc = create_rrc_du(rrc_cfg_t{});
};

TEST_F(rrc_du_ssb_arfcn_test, carrier_freq_is_decoded)
{
  const byte_buffer encoded = pack_meas_timing_cfg({632628U});

  const std::optional<arfcn_t> decoded = rrc->get_ssb_arfcn(encoded);
  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(*decoded, 632628U);
}

TEST_F(rrc_du_ssb_arfcn_test, first_entry_carrying_freq_and_timing_is_used)
{
  // The first entry has no frequency and timing information, so the second one must be picked.
  const byte_buffer encoded = pack_meas_timing_cfg({std::nullopt, 620000U});

  const std::optional<arfcn_t> decoded = rrc->get_ssb_arfcn(encoded);
  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(*decoded, 620000U);
}

TEST_F(rrc_du_ssb_arfcn_test, config_without_freq_and_timing_is_rejected)
{
  const byte_buffer encoded = pack_meas_timing_cfg({std::nullopt});

  EXPECT_FALSE(rrc->get_ssb_arfcn(encoded).has_value());
}

TEST_F(rrc_du_ssb_arfcn_test, empty_container_is_rejected)
{
  EXPECT_FALSE(rrc->get_ssb_arfcn(byte_buffer{}).has_value());
}

TEST_F(rrc_du_ssb_arfcn_test, malformed_container_is_rejected)
{
  EXPECT_FALSE(rrc->get_ssb_arfcn(make_byte_buffer("deadbeef").value()).has_value());
}
