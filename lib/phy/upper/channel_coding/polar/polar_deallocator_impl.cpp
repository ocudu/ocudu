// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "polar_deallocator_impl.h"

using namespace ocudu;

void polar_deallocator_impl::deallocate(span<uint8_t>       message,
                                        span<const uint8_t> output_decoder,
                                        const polar_code&   code)
{
  const bounded_bitset<polar_code::NMAX>& K_set  = code.get_K_set();
  span<const uint16_t>                    PC_set = code.get_PC_set();

  K_set.for_each(0, K_set.size(), [&PC_set, &message, &output_decoder, iPC = 0, iK = 0](uint16_t i_o) mutable {
    if (i_o == PC_set[iPC]) { // skip
      iPC = iPC + 1;
    } else {
      message[iK] = output_decoder[i_o];
      iK          = iK + 1;
    }
  });
}
