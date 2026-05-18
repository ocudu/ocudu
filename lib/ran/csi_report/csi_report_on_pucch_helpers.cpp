// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "csi_report_on_puxch_helpers.h"
#include "ocudu/ran/csi_report/csi_report_config_helpers.h"
#include "ocudu/ran/csi_report/csi_report_configuration.h"
#include "ocudu/ran/csi_report/csi_report_data.h"
#include "ocudu/support/error_handling.h"

using namespace ocudu;

static units::bits get_csi_report_pucch_size_cri_ri_li_pmi_cqi(const csi_report_configuration& config,
                                                               csi_report_data::ri_type        ri)
{
  // Counts number of bits following the order in TS38.212 Table 6.3.1.1.2-7.
  unsigned count = 0;

  // Gets RI, LI, CQI and CRI field sizes.
  ri_li_cqi_cri_sizes sizes =
      get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, ri, config.nof_csi_rs_resources);

  // CRI.
  count += sizes.cri;

  // RI.
  count += sizes.ri;

  // LI.
  if (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
    count += sizes.li;
  }

  // Ignore padding.

  // PMI.
  if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
    count += csi_report_get_size_pmi(config.pmi_codebook, ri);
  }

  // Wideband CQI.
  if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
    // For the first TB.
    count += sizes.wideband_cqi_first_tb;
    // For the second TB.
    count += sizes.wideband_cqi_second_tb;
  }

  return units::bits{count};
}

static csi_report_data csi_report_unpack_pucch_cri_ri_li_pmi_cqi(const csi_report_packed&        packed,
                                                                 const csi_report_configuration& config)
{
  csi_report_data data{.valid = true};

  // Validate input size.
  csi_report_size csi_report_size = get_csi_report_pucch_size(config);
  ocudu_assert(csi_report_size.part1_size <= csi_report_max_size,
               "The report size (i.e., {}) exceeds the maximum report size (i.e., {}).",
               csi_report_size.part1_size,
               csi_report_max_size);

  // Verify the CSI payload size.
  ocudu_assert(packed.size() == csi_report_size.part1_size.value(),
               "The number of packed bits (i.e., {}) is not equal to the CSI report size (i.e., {}).",
               units::bits(packed.size()),
               csi_report_size.part1_size);

  // Extract field sizes. Given that the PMI field sizes depend on the RI,
  // use a placeholder for the RI (one layer) and get its correct size after extracting the RI.
  ri_li_cqi_cri_sizes sizes =
      get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, 1U, config.nof_csi_rs_resources);

  // Unpacks bits following the order in TS38.212 Table 6.3.1.1.2-7.
  unsigned count = 0;

  // Extract CRI.
  unsigned cri = 0;
  if (sizes.cri > 0) {
    cri = packed.extract(count, sizes.cri);
  }
  data.cri.emplace(cri);
  count += sizes.cri;

  // Extract RI.
  csi_report_data::ri_type ri = csi_report_unpack_ri(packed.slice(count, count + sizes.ri), config.ri_restriction);
  data.ri.emplace(ri.value());
  count += sizes.ri;

  // Extract LI.
  if (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
    // Get the quantity sizes with the correct RI.
    ri_li_cqi_cri_sizes sizes_ri =
        get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, ri, config.nof_csi_rs_resources);

    unsigned li = 0;
    if (sizes_ri.li != 0) {
      li = packed.extract(count, sizes_ri.li);
    }

    data.li.emplace(li);
    count += sizes_ri.li;
  }

  // Skip padding bits.
  units::bits csi_report_size_ri = get_csi_report_pucch_size_cri_ri_li_pmi_cqi(config, ri);
  ocudu_assert(
      csi_report_size.part1_size >= csi_report_size_ri,
      "The report size (i.e., {}) must be equal to or greater than the report size without padding (i.e., {}).",
      csi_report_size.part1_size,
      csi_report_size_ri);
  count += csi_report_size.part1_size.value() - csi_report_size_ri.value();

  // Extract PMI.
  if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
    unsigned pmi_size = csi_report_get_size_pmi(config.pmi_codebook, ri);

    if (pmi_size != 0) {
      data.pmi.emplace(csi_report_unpack_pmi(packed.slice(count, count + pmi_size), config.pmi_codebook, ri));
      count += pmi_size;
    }
  }

  // Extract wideband CQI.
  if ((config.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_cqi) ||
      (config.quantities == csi_report_quantities::cri_ri_li_pmi_cqi)) {
    // Get the wideband CQI sizes for the unpacked RI (second TB only present for RI > 4).
    ri_li_cqi_cri_sizes sizes_ri =
        get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, ri, config.nof_csi_rs_resources);

    // For the first TB.
    data.first_tb_wideband_cqi.emplace(
        csi_report_unpack_wideband_cqi(packed.slice(count, count + sizes_ri.wideband_cqi_first_tb)));
    count += sizes_ri.wideband_cqi_first_tb;

    // For the second TB.
    if (sizes_ri.wideband_cqi_second_tb != 0) {
      data.second_tb_wideband_cqi.emplace(
          csi_report_unpack_wideband_cqi(packed.slice(count, count + sizes_ri.wideband_cqi_second_tb)));
      count += sizes_ri.wideband_cqi_second_tb;
    }
  }

  ocudu_assert(count == csi_report_size.part1_size.value(),
               "The number of read bits (i.e., {}) is not equal to the CSI report size (i.e., {})",
               units::bits(count),
               csi_report_size.part1_size);

  return data;
}

/// Gets the CSI report size as a function of the RI.
static units::bits get_csi_report_size_ri(const csi_report_configuration& config, csi_report_data::ri_type ri)
{
  switch (config.quantities) {
    case csi_report_quantities::cri_ri_pmi_cqi:
    case csi_report_quantities::cri_ri_cqi:
    case csi_report_quantities::cri_ri_li_pmi_cqi:
      return get_csi_report_pucch_size_cri_ri_li_pmi_cqi(config, ri);
    case csi_report_quantities::other:
    default:
      return units::bits{0};
  }
}

csi_report_size ocudu::get_csi_report_pucch_size(const csi_report_configuration& config)
{
  ocudu_assert(!config.subband.has_value(), "Subbands CSI reports are not supported on PUCCH.");
  using namespace units::literals;

  unsigned nof_csi_antenna_ports = get_precoding_codebook_antenna_ports(config.pmi_codebook);

  // Case for wideband PMI and CQI, TS38.214 Table 6.3.1.1.2-7.
  if (!config.subband.has_value()) {
    units::bits csi_report_size(0);

    // For each possible RI, find the largest CSI report size.
    for (unsigned ri = 1, ri_end = nof_csi_antenna_ports + 1; ri != ri_end; ++ri) {
      csi_report_size = std::max(csi_report_size, get_csi_report_size_ri(config, ri));
    }

    return {
        .part1_size = csi_report_size, .part2_correspondence = {}, .part2_min_size = 0_bits, .part2_max_size = 0_bits};
  }

  return {};
}

bool ocudu::validate_pucch_csi_payload(const csi_report_packed& packed, const csi_report_configuration& config)
{
  // CSI report configuration is invalid.
  if (!is_valid(config)) {
    return false;
  }

  // The number of packed bits does not match the expected CSI report size.
  if (packed.size() != get_csi_report_pucch_size(config).part1_size.value()) {
    return false;
  }

  ri_li_cqi_cri_sizes sizes =
      get_ri_li_cqi_cri_sizes(config.pmi_codebook, config.ri_restriction, 1U, config.nof_csi_rs_resources);
  unsigned ri_packed = packed.extract(sizes.cri, sizes.ri);

  // The RI is out of bounds given the number of allowed rank values.
  if (ri_packed >= config.ri_restriction.count()) {
    return false;
  }

  return true;
}

csi_report_data ocudu::csi_report_unpack_pucch(const csi_report_packed& packed, const csi_report_configuration& config)
{
  ocudu_assert(!std::holds_alternative<std::monostate>(config.pmi_codebook), "Unsupported PMI codebook type.");

  [[maybe_unused]] bool is_pmi_codebook_one_port = std::holds_alternative<pmi_codebook_one_port>(config.pmi_codebook);
  [[maybe_unused]] unsigned ri_restriction_size  = config.ri_restriction.size();
  [[maybe_unused]] unsigned nof_csi_rs_antenna_ports = get_precoding_codebook_antenna_ports(config.pmi_codebook);
  ocudu_assert(is_pmi_codebook_one_port || (ri_restriction_size >= nof_csi_rs_antenna_ports),
               "The RI restriction set size, i.e., {}, is smaller than the number of CSI-RS ports, i.e., {}.",
               ri_restriction_size,
               nof_csi_rs_antenna_ports);

  ocudu_assert(is_pmi_codebook_one_port ||
                   (config.ri_restriction.find_highest() < static_cast<int>(nof_csi_rs_antenna_ports)),
               "The RI restriction set, i.e., {}, allows higher rank values than the number of CSI-RS ports, i.e., {}.",
               config.ri_restriction,
               nof_csi_rs_antenna_ports);

  // Select unpacking depending on the CSI report quantities.
  switch (config.quantities) {
    case csi_report_quantities::cri_ri_pmi_cqi:
    case csi_report_quantities::cri_ri_cqi:
    case csi_report_quantities::cri_ri_li_pmi_cqi:
      return csi_report_unpack_pucch_cri_ri_li_pmi_cqi(packed, config);
    case csi_report_quantities::other:
    default:
      report_error("Invalid CSI report quantities.");
  }
}
