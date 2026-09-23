// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/format.h"
#include "ocudu/ofh/compression/compression_validator.h"
#include "ocudu/ofh/ofh_constants.h"
#include "ocudu/ru/ofh/ru_ofh_configuration.h"

using namespace ocudu;

static bool check_eaxc_id(unsigned eaxc)
{
  bool result = eaxc < ofh::MAX_SUPPORTED_EAXC_ID_VALUE;
  if (!result) {
    fmt::println(
        "Configured eAxC id '{}' is out of range. Valid range is [0-{}]", eaxc, ofh::MAX_SUPPORTED_EAXC_ID_VALUE - 1U);
  }

  return result;
}

static bool check_eaxcs_id(const ofh::sector_configuration& config)
{
  // Check PRACH eAxC.
  for (auto eaxc : config.prach_eaxc) {
    if (!check_eaxc_id(eaxc)) {
      return false;
    }
  }

  // Check uplink eAxCs.
  for (auto eaxc : config.ul_eaxc) {
    if (!check_eaxc_id(eaxc)) {
      return false;
    }
  }

  // Check downlink eAxCs.
  for (auto eaxc : config.dl_eaxc) {
    if (!check_eaxc_id(eaxc)) {
      return false;
    }
  }

  return true;
}

bool ocudu::is_valid_ru_ofh_config(const ru_ofh_configuration& config)
{
  for (const auto& sector : config.sector_configs) {
    if (auto result = ofh::validate_compression_params(sector.ul_compression_params); !result.has_value()) {
      fmt::println("Uplink {}", result.error());

      return false;
    }

    if (auto result = ofh::validate_compression_params(sector.dl_compression_params); !result.has_value()) {
      fmt::println("Downlink {}", result.error());

      return false;
    }

    if (auto result = ofh::validate_compression_params(sector.prach_compression_params); !result.has_value()) {
      fmt::println("PRACH {}", result.error());

      return false;
    }

    if (sector.dl_beamforming.has_value()) {
      if (auto result = ofh::validate_compression_params(sector.dl_beamforming->bfw_compr_params);
          !result.has_value()) {
        fmt::println("Beamforming weights {}", result.error());

        return false;
      }
    }

    if (!check_eaxcs_id(sector)) {
      return false;
    }
  }

  return true;
}
