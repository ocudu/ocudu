// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi_adaptor/precoding_matrix_repository.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace fapi_adaptor;

const precoding_beamforming_composite& precoding_matrix_repository::get_precoding(unsigned index) const
{
  ocudu_assert(index < repo.size(), "Invalid precoding matrix index={}, repository size={}", index, repo.size());
  ocudu_assert(repo[index].mimo.get_nof_layers() != 0, "Invalid precoding matrix index={}", index);

  return repo[index];
}
