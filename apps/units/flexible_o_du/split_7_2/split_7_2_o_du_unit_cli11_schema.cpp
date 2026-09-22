// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split_7_2_o_du_unit_cli11_schema.h"
#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config_cli11_schema.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_cli11_schema.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config_cli11_schema.h"
#include "split_7_2_o_du_unit_config.h"

using namespace ocudu;

void ocudu::configure_cli11_with_split_7_2_o_du_unit_config_schema(CLI::App&                   app,
                                                                   split_7_2_o_du_unit_config& parsed_cfg)
{
  configure_cli11_with_o_du_high_config_schema(app, parsed_cfg.odu_high_cfg);
  configure_cli11_with_du_low_config_schema(app, parsed_cfg.du_low_cfg);
  configure_cli11_with_ru_ofh_config_schema(app, parsed_cfg.ru_cfg);
}

void ocudu::autoderive_split_7_2_o_du_parameters_after_parsing(CLI::App& app, split_7_2_o_du_unit_config& parsed_cfg)
{
  autoderive_o_du_high_parameters_after_parsing(app, parsed_cfg.odu_high_cfg, true);

  // Auto derive DU low parameters. Split 7.2 does not use blocking mode.
  const auto& cell = parsed_cfg.odu_high_cfg.du_high_cfg.config.cells_cfg.front().cell;
  nr_band     band = cell.band ? cell.band.value() : band_helper::get_band_from_dl_arfcn(cell.dl_f_ref_arfcn);
  autoderive_du_low_parameters_after_parsing(app, parsed_cfg.du_low_cfg, band_helper::get_duplex_mode(band));
}
