// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/downlink_processor.h"
#include "ocudu/phy/upper/signal_processors/prs/prs_generator_validator.h"
#include "ocudu/phy/upper/signal_processors/srs/srs_estimator_configuration_validator.h"
#include "ocudu/phy/upper/uplink_pdu_validator.h"
#include "ocudu/phy/upper/uplink_processor.h"

namespace ocudu {

/// Implements the uplink PDU validator for \ref uplink_processor_single_executor_impl.
class uplink_processor_validator_impl : public uplink_pdu_validator
{
public:
  /// Constructs an uplink PDU validator containing the validators for each channel.
  uplink_processor_validator_impl(std::unique_ptr<prach_detector_validator>              prach_,
                                  std::unique_ptr<pucch_pdu_validator>                   pucch_,
                                  std::unique_ptr<pusch_pdu_validator>                   pusch_,
                                  std::unique_ptr<srs_estimator_configuration_validator> srs_) :
    prach(std::move(prach_)), pucch(std::move(pucch_)), pusch(std::move(pusch_)), srs(std::move(srs_))
  {
    ocudu_assert(prach, "Invalid PRACH detector validator.");
    ocudu_assert(pucch, "Invalid PUCCH processor validator.");
    ocudu_assert(pusch, "Invalid PUSCH processor validator.");
    ocudu_assert(srs, "Invalid SRS estimator configuration validator.");
  }

  // See interface for documentation.
  error_type<std::string> is_valid(const prach_detector::configuration& config) const override
  {
    return prach->is_valid(config);
  }
  error_type<std::string> is_valid(const pucch_processor::format0_configuration& config) const override
  {
    return pucch->is_valid(config);
  }
  error_type<std::string> is_valid(const pucch_processor::format1_configuration& config) const override
  {
    return pucch->is_valid(config);
  }
  error_type<std::string> is_valid(const pucch_processor::format2_configuration& config) const override
  {
    return pucch->is_valid(config);
  }
  error_type<std::string> is_valid(const pucch_processor::format3_configuration& config) const override
  {
    return pucch->is_valid(config);
  }
  error_type<std::string> is_valid(const pucch_processor::format4_configuration& config) const override
  {
    return pucch->is_valid(config);
  }
  error_type<std::string> is_valid(const pusch_processor::pdu_t& config) const override
  {
    return pusch->is_valid(config);
  }
  error_type<std::string> is_valid(const srs_estimator_configuration& config) const override
  {
    return srs->is_valid(config);
  }

private:
  std::unique_ptr<prach_detector_validator>              prach;
  std::unique_ptr<pucch_pdu_validator>                   pucch;
  std::unique_ptr<pusch_pdu_validator>                   pusch;
  std::unique_ptr<srs_estimator_configuration_validator> srs;
};

/// Implements the downlink PDU validator for \ref downlink_processor_multi_executor_impl.
class downlink_processor_validator_impl : public downlink_pdu_validator
{
public:
  /// Constructs an downlink PDU validator containing the validators for each channel.
  downlink_processor_validator_impl(std::unique_ptr<ssb_pdu_validator>                  ssb_,
                                    std::unique_ptr<pdcch_pdu_validator>                pdcch_,
                                    std::unique_ptr<pdsch_pdu_validator>                pdsch_,
                                    std::unique_ptr<nzp_csi_rs_configuration_validator> csi_,
                                    std::unique_ptr<prs_generator_validator>            prs_) :
    ssb(std::move(ssb_)), pdcch(std::move(pdcch_)), pdsch(std::move(pdsch_)), csi(std::move(csi_)), prs(std::move(prs_))
  {
    ocudu_assert(ssb, "Invalid SSB processor validator.");
    ocudu_assert(pdcch, "Invalid PDCCH processor validator.");
    ocudu_assert(pdsch, "Invalid PDSCH processor validator.");
    ocudu_assert(csi, "Invalid NZP-CSI-RS processor validator.");
    ocudu_assert(prs, "Invalid PRS generator validator.");
  }

  // See interface for documentation.
  error_type<std::string> is_valid(const ssb_processor::pdu_t& pdu, unsigned cell_bandwith_prbs) const override
  {
    return ssb->is_valid(pdu, cell_bandwith_prbs);
  }
  error_type<std::string> is_valid(const pdcch_processor::pdu_t& pdu) const override { return pdcch->is_valid(pdu); }
  error_type<std::string> is_valid(const pdsch_processor::pdu_t& pdu) const override { return pdsch->is_valid(pdu); }
  error_type<std::string> is_valid(const nzp_csi_rs_generator::config_t& config) const override
  {
    return csi->is_valid(config);
  }
  error_type<std::string> is_valid(const prs_generator_configuration& config) const override
  {
    return prs->is_valid(config);
  }

private:
  std::unique_ptr<ssb_pdu_validator>                  ssb;
  std::unique_ptr<pdcch_pdu_validator>                pdcch;
  std::unique_ptr<pdsch_pdu_validator>                pdsch;
  std::unique_ptr<nzp_csi_rs_configuration_validator> csi;
  std::unique_ptr<prs_generator_validator>            prs;
};

} // namespace ocudu
