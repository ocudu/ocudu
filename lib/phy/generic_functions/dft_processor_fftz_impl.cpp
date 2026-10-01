// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "dft_processor_fftz_impl.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

dft_processor_fftz_impl::dft_processor_fftz_impl(const dft_processor_fftz_configuration& fftz_config,
                                                 const configuration&                    dft_config) :
  dir(dft_config.dir), input(dft_config.size), output(dft_config.size)
{
  // FFTZ problem descriptor.
  aoclfftz_prob_desc_f fft_problem = {};
  // FFT dimensions. Since it is a one-dimensional transform, one dimension is enough to describe the FFT.
  aoclfftz_dim_t signal_dimemsions;
  // FFT batch dimensions.
  aoclfftz_dim_t batch_dimensions;

  // Set the FFT input and output.
  fft_problem.in  = reinterpret_cast<float*>(input.data());
  fft_problem.out = reinterpret_cast<float*>(output.data());

  // Set the dimensions and signal stride for a one-dimensional transform.
  signal_dimemsions.n          = static_cast<int32_t>(dft_config.size);
  signal_dimemsions.in_stride  = 1;
  signal_dimemsions.out_stride = 1;
  fft_problem.dim_rank         = 1;
  fft_problem.dims             = &signal_dimemsions;

  // Set the batch dimensions and stride for a single FFT batch.
  batch_dimensions.n          = 1;
  batch_dimensions.in_stride  = signal_dimemsions.n;
  batch_dimensions.out_stride = signal_dimemsions.n;
  fft_problem.vec_rank        = 1;
  fft_problem.vecs            = &batch_dimensions;

  // Set the FFT type as complex.
  fft_problem.flags.fft_type = 0;
  // Set the FFT direction.
  fft_problem.flags.fft_direction = (dir == direction::DIRECT) ? 0 : 1;
  // Set in order storage.
  fft_problem.flags.storage_order = 0;
  // Set out of place FFT, since input and output vectors are different.
  fft_problem.flags.fft_placement = 1; // out-of-place (input/output are different vectors)
  // Set the transpose mode.
  fft_problem.flags.transpose_mode = 0;

  fft_problem.flags.bit_reproducibility = fftz_config.bit_reproducibility ? 1 : 0;

  // Disable multithreading options.
  fft_problem.pthr_fft.num_threads        = 1;
  fft_problem.pthr_fft.dynamic_load_model = 0;

  // Set the optimization level.
  fft_problem.cntrl_params.opt_level = fftz_config.opt_level;
  fft_problem.cntrl_params.opt_off   = 0;

  // Disable logging and stats reporting.
  fft_problem.cntrl_params.logger_mode   = AOCLFFTZ_LOG_NONE;
  fft_problem.cntrl_params.measure_stats = 0;

  // Configure the FFT problem.
  handle = aoclfftz_setup_f(&fft_problem);

  report_error_if_not(handle != nullptr,
                      "FFTZ setup failed for size={} and direction={}.",
                      dft_config.size,
                      dft_processor::direction_to_string(dir));
}

dft_processor_fftz_impl::~dft_processor_fftz_impl()
{
  if (handle != nullptr) {
    aoclfftz_destroy(handle);
    handle = nullptr;
  }
}

span<const cf_t> dft_processor_fftz_impl::run()
{
  // Execute the FFT and place the result in the output buffer defined during setup.
  [[maybe_unused]] aoclfftz_error_type st = aoclfftz_execute(handle);

  ocudu_assert(st == AOCLFFTZ_SUCCESS,
               "FFTZ execution failed with status={}.",
               static_cast<std::underlying_type_t<aoclfftz_error_type>>(st));

  return output;
}
