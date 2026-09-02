// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/resource_allocation/resource_allocation_frequency.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST(resource_allocation_frequency_test, ra_frequency_type1)
{
  for (unsigned N_bwp_size : {24, 48, 96}) {
    for (unsigned start = 0; start != N_bwp_size - 1; ++start) {
      for (unsigned length = 1, length_end = N_bwp_size - start + 1; length != length_end; ++length) {
        ra_frequency_type1_configuration config;
        config.N_bwp_size = N_bwp_size;
        config.start_vrb  = start;
        config.length_vrb = length;

        unsigned riv = ra_frequency_type1_get_riv(config);

        unsigned expected;
        if ((length - 1) <= (N_bwp_size / 2)) {
          expected = N_bwp_size * (length - 1) + start;
        } else {
          expected = N_bwp_size * (N_bwp_size - length + 1) + (N_bwp_size - 1 - start);
        }

        ASSERT_EQ(riv, expected);
      }
    }
  }
}

TEST(resource_allocation_frequency_test, ra_frequency_type1_special)
{
  for (unsigned N_bwp_initial : {24, 48, 96}) {
    for (unsigned N_bwp_active : {25, 52, 104, 272}) {
      // Determine K.
      unsigned K = 8;
      while (K > 1) {
        if (K <= N_bwp_active / N_bwp_initial) {
          break;
        }
        K /= 2;
      }

      for (unsigned start = 0; start < (N_bwp_initial - 1) * K; start += K) {
        for (unsigned length = K; length < N_bwp_initial * K - start; length += K) {
          ra_frequency_type1_special_configuration config;
          config.N_bwp_initial = N_bwp_initial;
          config.N_bwp_active  = N_bwp_active;
          config.start_vrb     = start;
          config.length_vrb    = length;

          unsigned riv = ra_frequency_type1_special_get_riv(config);

          unsigned start_prime  = start / K;
          unsigned length_prime = length / K;

          unsigned expected;
          if ((length_prime - 1) <= (N_bwp_initial / 2)) {
            expected = N_bwp_initial * (length_prime - 1) + start_prime;
          } else {
            expected = N_bwp_initial * (N_bwp_initial - length_prime + 1) + (N_bwp_initial - 1 - start_prime);
          }

          ASSERT_EQ(riv, expected);
        }
      }
    }
  }
}
