// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi_adaptor/uci_part2_correspondence_mapper.h"
#include "uci_part2_mapper_functions.h"
#include "ocudu/ran/csi_report/csi_report_configuration.h"

using namespace ocudu;
using namespace fapi_adaptor;

span<const uci_part2_correspondence_information>
uci_part2_correspondence_mapper::map(const csi_report_configuration& csi_report) const
{
  ocudu_assert(csi_report.nof_csi_rs_resources && csi_report.nof_csi_rs_resources <= MAX_NOF_CSI_RESOURCES,
               "Unsupported number of CSI-RS resources");
  ocudu_assert(!csi_report.ri_restriction.empty(), "Empty RI restriction field");
  ocudu_assert(csi_report.ri_restriction.to_uint64() < MAX_NOF_RI_RESTRICTIONS, "Unsupported RI restriction value");

  unsigned index = get_uci_part2_correspondence_index(csi_report.nof_csi_rs_resources,
                                                      to_pmi_codebook_identifier(csi_report.pmi_codebook).value(),
                                                      static_cast<unsigned>(csi_report.ri_restriction.to_uint64()),
                                                      static_cast<unsigned>(csi_report.quantities));

  ocudu_assert(index < correspondence_map.size(),
               "Invalid UCI Part2 correspondence calculated index, index value is '{}' while map size has '{}' entries",
               index,
               correspondence_map.size());

  return correspondence_map[index];
}
