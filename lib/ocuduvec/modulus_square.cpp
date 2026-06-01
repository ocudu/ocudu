// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocuduvec/modulus_square.h"
#include "ocudu/ocuduvec/simd.h"

using namespace ocudu;
using namespace ocuduvec;

static void modulus_square_simd(float* result, const cf_t* input, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_CF_SIZE
  for (unsigned simd_end = OCUDU_SIMD_CF_SIZE * (len / OCUDU_SIMD_CF_SIZE); i != simd_end; i += OCUDU_SIMD_CF_SIZE) {
    // Load 2 SIMD words of floats.
    simd_f_t simd_input1 = ocudu_simd_f_loadu(reinterpret_cast<const float*>(input + i));
    simd_f_t simd_input2 = ocudu_simd_f_loadu(reinterpret_cast<const float*>(input + i + OCUDU_SIMD_CF_SIZE / 2));

    // Calculates the squares.
    simd_f_t simd_mul1 = ocudu_simd_f_mul(simd_input1, simd_input1);
    simd_f_t simd_mul2 = ocudu_simd_f_mul(simd_input2, simd_input2);

    // Horizontally add the values in pair, it results in adding the squared real and imaginary parts.
    simd_f_t simd_abs2 = ocudu_simd_f_hadd(simd_mul1, simd_mul2);

    ocudu_simd_f_storeu(result + i, simd_abs2);
  }
#endif // OCUDU_SIMD_CF_SIZE

  for (; i != len; ++i) {
    float real = std::real(input[i]);
    float imag = std::imag(input[i]);

    result[i] = real * real + imag * imag;
  }
}

static void modulus_square_simd(float* result, const cbf16_t* input, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_CF_SIZE
  for (unsigned simd_end = OCUDU_SIMD_CF_SIZE * (len / OCUDU_SIMD_CF_SIZE); i != simd_end; i += OCUDU_SIMD_CF_SIZE) {
    // Load input from 16-bit brain float to single precision floating point.
    simd_cf_t simd_input = ocudu_simd_loadu(input + i);

    // Calculate the modulus squares.
    simd_f_t simd_abs2 = ocudu_simd_cf_norm_sq(simd_input);

    // Store result.
    ocudu_simd_f_storeu(result + i, simd_abs2);
  }
#endif // OCUDU_SIMD_CF_SIZE

  for (; i != len; ++i) {
    cf_t  cf_input = to_cf(input[i]);
    float real     = std::real(cf_input);
    float imag     = std::imag(cf_input);

    result[i] = real * real + imag * imag;
  }
}

static void modulus_square_and_add_simd(float* result, const cf_t* input, const float* offset, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_CF_SIZE
  for (unsigned simd_end = OCUDU_SIMD_CF_SIZE * (len / OCUDU_SIMD_CF_SIZE); i != simd_end; i += OCUDU_SIMD_CF_SIZE) {
    // Load 2 SIMD words of floats.
    simd_f_t simd_input1 = ocudu_simd_f_loadu(reinterpret_cast<const float*>(input + i));
    simd_f_t simd_input2 = ocudu_simd_f_loadu(reinterpret_cast<const float*>(input + i + OCUDU_SIMD_CF_SIZE / 2));

    // Load offset.
    simd_f_t simd_offset = ocudu_simd_f_loadu(&offset[i]);

    // Calculates the squares.
    simd_f_t simd_mul1 = simd_input1 * simd_input1;
    simd_f_t simd_mul2 = simd_input2 * simd_input2;

    // Horizontally add the values in pair, it results in adding the squared real and imaginary parts.
    simd_offset += ocudu_simd_f_hadd(simd_mul1, simd_mul2);

    ocudu_simd_f_storeu(result + i, simd_offset);
  }
#endif // OCUDU_SIMD_CF_SIZE

  for (; i != len; ++i) {
    float real = std::real(input[i]);
    float imag = std::imag(input[i]);

    result[i] = real * real + imag * imag + offset[i];
  }
}

static void modulus_square_and_add_simd(float* result, const cbf16_t* input, const float* offset, std::size_t len)
{
  std::size_t i = 0;

#if OCUDU_SIMD_CF_SIZE
  for (unsigned simd_end = OCUDU_SIMD_CF_SIZE * (len / OCUDU_SIMD_CF_SIZE); i != simd_end; i += OCUDU_SIMD_CF_SIZE) {
    // Load input from 16-bit brain float to single precision floating point.
    simd_cf_t simd_input = ocudu_simd_loadu(input + i);

    // Calculate the modulus squares.
    simd_f_t simd_abs2 = ocudu_simd_cf_norm_sq(simd_input);

    // Load offset.
    simd_f_t simd_offset = ocudu_simd_f_loadu(&offset[i]);

    // Add the modulus squares to the offset.
    simd_offset += simd_abs2;

    ocudu_simd_f_storeu(result + i, simd_offset);
  }
#endif // OCUDU_SIMD_CF_SIZE

  for (; i != len; ++i) {
    cf_t  cf_input = to_cf(input[i]);
    float real     = std::real(cf_input);
    float imag     = std::imag(cf_input);

    result[i] = real * real + imag * imag + offset[i];
  }
}

void ocudu::ocuduvec::modulus_square(span<float> result, span<const cf_t> input)
{
  ocudu_ocuduvec_assert_size(result, input);

  modulus_square_simd(result.data(), input.data(), input.size());
}

void ocudu::ocuduvec::modulus_square(span<float> result, span<const cbf16_t> input)
{
  ocudu_ocuduvec_assert_size(result, input);

  modulus_square_simd(result.data(), input.data(), input.size());
}

void ocudu::ocuduvec::modulus_square_and_add(span<float> result, span<const cf_t> input, span<const float> offset)
{
  ocudu_ocuduvec_assert_size(result, input);
  ocudu_ocuduvec_assert_size(result, offset);

  modulus_square_and_add_simd(result.data(), input.data(), offset.data(), input.size());
}

void ocudu::ocuduvec::modulus_square_and_add(span<float> result, span<const cbf16_t> input, span<const float> offset)
{
  ocudu_ocuduvec_assert_size(result, input);
  ocudu_ocuduvec_assert_size(result, offset);

  modulus_square_and_add_simd(result.data(), input.data(), offset.data(), input.size());
}
