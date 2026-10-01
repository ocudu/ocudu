// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Mathematical utility functions.

#pragma once

#include "ocudu/support/ocudu_assert.h"
#include <cmath>

namespace ocudu {

/// \brief Implements an exponential moving average accumulator, with simple moving average for faster initial
/// convergence. This accumulator is useful for tracking the average of a series of values, with a bias towards the most
/// recent values. The accumulator uses a simple moving average until a certain number of samples have been accumulated,
/// and then switches to exponential moving average.
template <typename T>
class exp_average_fast_start
{
public:
  exp_average_fast_start(T alpha_val_) : exp_average_fast_start(alpha_val_, 1.0 / alpha_val_) {}
  exp_average_fast_start(T alpha_val_, unsigned start_size) : alpha_value(alpha_val_), start_count_size(start_size)
  {
    ocudu_assert(alpha_value > 0 and alpha_value < 1, "Alpha must be higher than 0 and less than 1");
    ocudu_assert(start_size > 0, "Start size must be greater than 0");
  }

  /// Push new sample.
  void push(T sample)
  {
    // Simple moving average until start_count_size is reached. This speeds up the convergence of the average value.
    if (count < start_count_size) {
      avg += (sample - avg) / (count + 1);
      ++count;
    }
    // Exponential moving average after start_count_size is reached.
    else {
      avg = (1 - alpha_value) * avg + alpha_value * sample;
    }
  }

  /// Push zeros \c n times.
  void push_zeros(unsigned n)
  {
    if (count < start_count_size) {
      const unsigned rem = std::min(start_count_size - count, n);
      for (unsigned i = 0; i != rem; ++i) {
        avg -= avg / (count + 1);
        ++count;
      }
      n -= rem;
    }

    if (n > 0) {
      avg = std::pow(1 - alpha_value, n) * avg;
    }
  }

  void reset()
  {
    count = 0;
    avg   = 0;
  }

  /// Get the current average value.
  T average() const { return avg; }

  void set_alpha(T alpha_val)
  {
    ocudu_assert(alpha_value > 0 and alpha_value < 1, "Alpha must be higher than 0 and less than 1");
    alpha_value = alpha_val;
  }

  /// Get the forgetting coefficient being used.
  T get_alpha() const { return alpha_value; }

  /// Whether the averager is in fast start stage or exponential average stage.
  bool is_exp_average_mode() const { return count >= start_count_size; }

private:
  T        avg = 0;
  T        alpha_value;
  unsigned count = 0;
  unsigned start_count_size;
};

} // namespace ocudu
