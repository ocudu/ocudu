// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "bwp_configuration.h"
#include "sched_pdcch_config.h"
#include "serving_cell_config.h"
#include "ue_bwp_config.h"

namespace ocudu {

/// PDSCH configuration of a BWP.
class sched_pdsch_config
{
public:
  sched_pdsch_config() = default;
  sched_pdsch_config(const pdsch_config_common& cfg_common, const pdsch_config* cfg_ded) :
    cmn(&cfg_common), ded_ptr(cfg_ded)
  {
  }

  const pdsch_config_common& common() const { return *cmn; }
  const pdsch_config*        ded() const { return ded_ptr; }

private:
  const pdsch_config_common* cmn     = nullptr;
  const pdsch_config*        ded_ptr = nullptr;
};

/// Configuration of a Downlink Bandwidth Part (BWP) including the common config and optionally the dedicated config.
class sched_bwp_dl_config
{
public:
  sched_bwp_dl_config() = default;
  sched_bwp_dl_config(const bwp_downlink_common&    common,
                      const bwp_downlink_dedicated* ded,
                      const sched_pdcch_config&     pdcch_cfg_) :
    bwp_dl_common(&common),
    bwp_dl_ded(ded),
    pdcch_cfg(&pdcch_cfg_),
    pdsch_cfg(common.pdsch_common, ded != nullptr and ded->pdsch_cfg.has_value() ? &*ded->pdsch_cfg : nullptr)
  {
  }

  const bwp_configuration&   cfg() const { return bwp_dl_common->generic_params; }
  const bwp_downlink_common& common() const { return *bwp_dl_common; }
  bool                       has_ded_cfg() const { return bwp_dl_ded != nullptr and bwp_dl_ded->pdcch_cfg.has_value(); }

  const sched_pdcch_config& pdcch() const { return *pdcch_cfg; }
  const sched_pdsch_config& pdsch() const { return pdsch_cfg; }

  const radio_link_monitoring_config* rlm() const
  {
    return bwp_dl_ded != nullptr and bwp_dl_ded->rlm_cfg.has_value() ? &*bwp_dl_ded->rlm_cfg : nullptr;
  }

  bool operator==(const sched_bwp_dl_config& other) const
  {
    return *bwp_dl_common == *other.bwp_dl_common and *bwp_dl_ded == *other.bwp_dl_ded and
           *pdcch_cfg == *other.pdcch_cfg;
  }

private:
  const bwp_downlink_common*    bwp_dl_common = nullptr;
  const bwp_downlink_dedicated* bwp_dl_ded    = nullptr;
  const sched_pdcch_config*     pdcch_cfg     = nullptr;
  sched_pdsch_config            pdsch_cfg;
};

/// Configuration of an Uplink Bandwidth Part (BWP) including the common config and optionally the dedicated config.
class sched_bwp_ul_config
{
public:
  sched_bwp_ul_config() = default;
  sched_bwp_ul_config(const bwp_uplink_common&                   common,
                      const bwp_uplink_dedicated*                ded,
                      const std::optional<ue_uplink_bwp_config>& ue_cfg = std::nullopt) :
    bwp_ul_common(&common), bwp_ul_ded(ded), ue_ul_bwp_cfg(ue_cfg)
  {
  }

  const bwp_configuration&    cfg() const { return bwp_ul_common->generic_params; }
  const bwp_uplink_common&    common() const { return *bwp_ul_common; }
  const bwp_uplink_dedicated* ded() const { return bwp_ul_ded; }
  const rach_config_common*   rach_common() const
  {
    return bwp_ul_common->rach_cfg_common.has_value() ? &*bwp_ul_common->rach_cfg_common : nullptr;
  }
  const pusch_config_common* pusch_common() const
  {
    return bwp_ul_common->pusch_cfg_common.has_value() ? &*bwp_ul_common->pusch_cfg_common : nullptr;
  }
  const pusch_config* pusch_ded() const
  {
    return bwp_ul_ded != nullptr and bwp_ul_ded->pusch_cfg.has_value() ? &*bwp_ul_ded->pusch_cfg : nullptr;
  }

  const pucch_config_common* pucch_common() const
  {
    return bwp_ul_common->pucch_cfg_common.has_value() ? &*bwp_ul_common->pucch_cfg_common : nullptr;
  }
  const pucch_config* pucch_ded() const
  {
    return bwp_ul_ded != nullptr and bwp_ul_ded->pucch_cfg.has_value() ? &*bwp_ul_ded->pucch_cfg : nullptr;
  }

  const srs_config* srs() const
  {
    return bwp_ul_ded != nullptr and bwp_ul_ded->srs_cfg.has_value() ? &*bwp_ul_ded->srs_cfg : nullptr;
  }

  const ue_uplink_bwp_config* ue_cfg() const { return ue_ul_bwp_cfg.has_value() ? &*ue_ul_bwp_cfg : nullptr; }

  bool operator==(const sched_bwp_ul_config& other) const
  {
    return *bwp_ul_common == *other.bwp_ul_common and *bwp_ul_ded == *other.bwp_ul_ded and
           ue_ul_bwp_cfg == other.ue_ul_bwp_cfg;
  }

private:
  const bwp_uplink_common*            bwp_ul_common = nullptr;
  const bwp_uplink_dedicated*         bwp_ul_ded    = nullptr;
  std::optional<ue_uplink_bwp_config> ue_ul_bwp_cfg;
};

/// Configuration of a Bandwidth Part (BWP) including the common config and optionally the dedicated config.
class sched_bwp_config
{
public:
  bwp_id_t            id;
  sched_bwp_dl_config dl;
  sched_bwp_ul_config ul;

  bool operator==(const sched_bwp_config& other) const { return id == other.id and dl == other.dl and ul == other.ul; }
};

} // namespace ocudu
