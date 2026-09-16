// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ofh/compression/compression_params.h"
#include "ocudu/ofh/compression/compression_validator.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ofh;

TEST(ofh_compression_validator, no_compression_is_accepted)
{
  // Only the minimum is looked at without compression, the rest of what the udIqWidth nibble can carry passes.
  for (unsigned width = MIN_IQ_WIDTH; width <= MAX_IQ_WIDTH; ++width) {
    EXPECT_TRUE(validate_compression_params({compression_type::none, width}).has_value()) << width;
  }
}

TEST(ofh_compression_validator, no_compression_rejects_a_width_the_quantizer_cannot_represent)
{
  // One bit leaves the quantizer with a gain of zero and zero bits shifts by more than the width of the type.
  for (unsigned width : {0U, 1U}) {
    EXPECT_FALSE(validate_compression_params({compression_type::none, width}).has_value()) << width;
  }
}

TEST(ofh_compression_validator, bfp_accepts_the_implemented_widths)
{
  for (unsigned width : {8U, 9U, 12U, 14U, 16U}) {
    EXPECT_TRUE(validate_compression_params({compression_type::BFP, width}).has_value()) << width;
  }
}

TEST(ofh_compression_validator, bfp_rejects_the_other_widths)
{
  for (unsigned width : {1U, 7U, 10U, 11U, 13U, 15U}) {
    EXPECT_FALSE(validate_compression_params({compression_type::BFP, width}).has_value()) << width;
  }
}

TEST(ofh_compression_validator, the_reason_names_what_was_rejected)
{
  // The caller prefixes the setting name and prints this, so the text is part of what the gNB shows the user.
  // error() may only be read once the result is known to hold one, hence the assertion before each check.
  auto bad_method = validate_compression_params({compression_type::mu_law, MAX_IQ_WIDTH});
  ASSERT_FALSE(bad_method.has_value());
  EXPECT_EQ(bad_method.error(), "compression method 'Mu law' is not supported. Valid values are [none,bfp]");

  auto bad_width = validate_compression_params({compression_type::BFP, 11});
  ASSERT_FALSE(bad_width.has_value());
  EXPECT_EQ(bad_width.error(), "BFP compression bit width '11' is not supported. Valid values are [8,9,12,14,16]");

  auto narrow_width = validate_compression_params({compression_type::none, 1});
  ASSERT_FALSE(narrow_width.has_value());
  EXPECT_EQ(narrow_width.error(), "compression bit width '1' is not supported without compression. The minimum is 2");
}

TEST(ofh_compression_validator, the_methods_without_an_implementation_are_rejected)
{
  const std::array<compression_type, 6> unsupported = {compression_type::block_scaling,
                                                       compression_type::mu_law,
                                                       compression_type::modulation,
                                                       compression_type::bfp_selective,
                                                       compression_type::mod_selective,
                                                       compression_type::reserved};

  for (compression_type type : unsupported) {
    EXPECT_FALSE(validate_compression_params({type, MAX_IQ_WIDTH}).has_value()) << to_string(type);
  }
}

TEST(ofh_compression_validator, the_method_names_are_unchanged)
{
  EXPECT_STREQ(to_string(compression_type::none), "none");
  EXPECT_STREQ(to_string(compression_type::BFP), "BFP");
  EXPECT_STREQ(to_string(compression_type::block_scaling), "Block scaling");
  EXPECT_STREQ(to_string(compression_type::mu_law), "Mu law");
  EXPECT_STREQ(to_string(compression_type::modulation), "Modulation");
  EXPECT_STREQ(to_string(compression_type::bfp_selective), "BFP selective");
  EXPECT_STREQ(to_string(compression_type::mod_selective), "Modulation selective");
}
