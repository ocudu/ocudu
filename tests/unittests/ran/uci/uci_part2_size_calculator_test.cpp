// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/uci/uci_part2_size_calculator.h"
#include <gtest/gtest.h>

using namespace ocudu;

// Fixed size.
TEST(uci_part2_size_calculator, fix_size)
{
  static constexpr unsigned csi_part1_size          = 4;
  static constexpr unsigned expected_csi_part2_size = 1;

  // Create description.
  uci_part2_size_description description(expected_csi_part2_size);

  // Generate random payload with all ones.
  uci_payload_type csi_part1 = ~uci_payload_type(csi_part1_size);

  units::bits csi_part2_size = uci_part2_get_size(csi_part1, description);

  ASSERT_EQ(csi_part2_size.value(), expected_csi_part2_size);
}

// Test two ports typical CSI report following TS38.212 Tables 6.3.2.1.2-3 and 6.3.2.1.2-4.
//
// CSI Part 1 consists of:
// - CRI: not present
// - RI: 1 bit
// - Wideband CQI for first TB: 4 bit
// - Subband differential CQI for first TB: not present
//
// CSI Part 2 consists of:
// - Wideband CQI for the second TB: not present
// - Layer Indicator: not present
// - PMI wideband information fields X1: not present
// - PMI wideband information fields X2:
//     - 1 layer: 2 bits
//     - 2 layer: 1 bits
TEST(uci_part2_size_calculator, basic_two_ports)
{
  static constexpr unsigned csi_part1_size = 5;

  // Create description.
  uci_part2_size_description         description = {};
  uci_part2_size_description::entry& entry       = description.entries.emplace_back();

  // Setup RI parameter.
  uci_part2_size_description::parameter& parameter = entry.parameters.emplace_back();
  parameter.width                                  = 1;
  parameter.offset                                 = 0;

  // Push map values of CSI Part 2 sizes in function of the RI.
  entry.map.push_back(2);
  entry.map.push_back(1);

  // Make sure the description is consistent with the CSI Part 1 size.
  ASSERT_TRUE(description.is_valid(csi_part1_size));

  // Generate random payload with all ones.
  uci_payload_type csi_part1 = ~uci_payload_type(csi_part1_size);

  // Test for RI=1 layer.
  {
    // Force RI to 1 layer.
    csi_part1.set(0, false);

    units::bits csi_part2_size = uci_part2_get_size(csi_part1, description);

    ASSERT_EQ(csi_part2_size, units::bits(2));
  }

  // Test for RI=2 layer.
  {
    // Force RI to 2 layer.
    csi_part1.set(0, true);

    units::bits csi_part2_size = uci_part2_get_size(csi_part1, description);

    ASSERT_EQ(csi_part2_size, units::bits(1));
  }
}

// Test four ports typical CSI report following TS38.212 Tables 6.3.2.1.2-3 and 6.3.2.1.2-4.
//
// CSI Part 1 consists of:
// - CRI: not present
// - RI: 2 bit
// - Wideband CQI for first TB: 4 bit
// - Subband differential CQI for first TB: not present
//
// CSI Part 2 consists of:
// - Wideband CQI for the second TB: not present
// - Layer Indicator: not present
// - PMI wideband information fields X1:
//     - 1 layer: 2 bits
//     - 2 layer: 3 bits
//     - 3 and 4 layer: 2 bits
// - PMI wideband information fields X2:
//     - 1 layer: 2 bits
//     - 2 layer: 1 bits
//     - 3 and 4 layer: 1 bit
TEST(uci_part2_size_calculator, basic_four_ports)
{
  static constexpr unsigned csi_part1_size = 5;

  // Create description.
  uci_part2_size_description         description = {};
  uci_part2_size_description::entry& entry       = description.entries.emplace_back();

  // Setup RI parameter.
  uci_part2_size_description::parameter& parameter = entry.parameters.emplace_back();
  parameter.width                                  = 2;
  parameter.offset                                 = 0;

  // Push map values of CSI Part 2 sizes in function of the RI.
  entry.map.push_back(4);
  entry.map.push_back(4);
  entry.map.push_back(3);
  entry.map.push_back(3);

  // Make sure the description is consistent with the CSI Part 1 size.
  ASSERT_TRUE(description.is_valid(csi_part1_size));

  // Generate random payload with all ones.
  uci_payload_type csi_part1 = ~uci_payload_type(csi_part1_size);

  // Test for RI=1 layer.
  {
    csi_part1.set(0, false);
    csi_part1.set(1, false);

    units::bits csi_part2_size = uci_part2_get_size(csi_part1, description);

    ASSERT_EQ(csi_part2_size, units::bits(4));
  }

  // Test for RI=2 layer.
  {
    csi_part1.set(0, false);
    csi_part1.set(1, true);

    units::bits csi_part2_size = uci_part2_get_size(csi_part1, description);

    ASSERT_EQ(csi_part2_size, units::bits(4));
  }

  // Test for RI=3 layer.
  {
    csi_part1.set(0, true);
    csi_part1.set(1, false);

    units::bits csi_part2_size = uci_part2_get_size(csi_part1, description);

    ASSERT_EQ(csi_part2_size, units::bits(3));
  }

  // Test for RI=4 layer.
  {
    csi_part1.set(0, true);
    csi_part1.set(1, true);

    units::bits csi_part2_size = uci_part2_get_size(csi_part1, description);

    ASSERT_EQ(csi_part2_size, units::bits(3));
  }
}

// Test that a CSI Part 2 size description is rejected if there is no CSI Part 1 payload to derive the size from.
TEST(uci_part2_size_calculator, empty_part1_payload)
{
  ASSERT_FALSE(uci_part2_size_description(4).is_valid(0));

  // A description without entries does not derive any size from CSI Part 1.
  ASSERT_TRUE(uci_part2_size_description(0).is_valid(0));
}

// Test that a parameter ending exactly at the last CSI Part 1 bit is accepted and read correctly.
//
// The description reads a single parameter placed at the end of a four-bit CSI Part 1, i.e. the parameter occupies the
// last two bits.
TEST(uci_part2_size_calculator, parameter_ends_at_last_part1_bit)
{
  static constexpr unsigned csi_part1_size = 4;
  static constexpr unsigned param_width    = 2;
  static constexpr unsigned param_offset   = csi_part1_size - param_width;

  // Create a description with a single parameter that ends at the last CSI Part 1 bit.
  uci_part2_size_description             description = {};
  uci_part2_size_description::entry&     entry       = description.entries.emplace_back();
  uci_part2_size_description::parameter& parameter   = entry.parameters.emplace_back();
  parameter.offset                                   = param_offset;
  parameter.width                                    = param_width;

  // Map each parameter value onto a distinct CSI Part 2 size. The sizes are arbitrary.
  entry.map.assign({10, 11, 12, 13});

  // The parameter ends at the last CSI Part 1 bit.
  ASSERT_TRUE(description.is_valid(csi_part1_size));

  // Each parameter value must select its CSI Part 2 size.
  uci_payload_type csi_part1(csi_part1_size);

  // Parameter value 0, i.e. bits 0b00.
  csi_part1.set(param_offset, false);
  csi_part1.set(param_offset + 1, false);
  ASSERT_EQ(uci_part2_get_size(csi_part1, description), units::bits(10));

  // Parameter value 1, i.e. bits 0b01.
  csi_part1.set(param_offset, false);
  csi_part1.set(param_offset + 1, true);
  ASSERT_EQ(uci_part2_get_size(csi_part1, description), units::bits(11));

  // Parameter value 2, i.e. bits 0b10.
  csi_part1.set(param_offset, true);
  csi_part1.set(param_offset + 1, false);
  ASSERT_EQ(uci_part2_get_size(csi_part1, description), units::bits(12));

  // Parameter value 3, i.e. bits 0b11.
  csi_part1.set(param_offset, true);
  csi_part1.set(param_offset + 1, true);
  ASSERT_EQ(uci_part2_get_size(csi_part1, description), units::bits(13));
}

// Test that a parameter exceeding the CSI Part 1 size is rejected.
TEST(uci_part2_size_calculator, parameter_exceeds_part1_size)
{
  static constexpr unsigned csi_part1_size = 8;

  // Create a description with a single parameter that reads one bit past the end of the CSI Part 1.
  uci_part2_size_description             description = {};
  uci_part2_size_description::entry&     entry       = description.entries.emplace_back();
  uci_part2_size_description::parameter& parameter   = entry.parameters.emplace_back();
  parameter.offset                                   = 6;
  parameter.width                                    = 3;

  // Set the appropriate map size, so the only reason to reject the Part 2 description is its incorrect size.
  entry.map.resize(1 << parameter.width);

  // The description itself is coherent, i.e. it is accepted for a CSI Part 1 that fits the parameter.
  ASSERT_TRUE(description.is_valid(parameter.offset + parameter.width));

  // It must fail if the Part 2 description is not appropriate for the Part 1 size.
  ASSERT_FALSE(description.is_valid(csi_part1_size));
}
