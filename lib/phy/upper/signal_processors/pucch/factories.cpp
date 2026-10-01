// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/upper/signal_processors/pucch/factories.h"
#include "dmrs_pucch_estimator_format2.h"
#include "dmrs_pucch_estimator_formats3_4.h"
#include "dmrs_pucch_estimator_impl.h"
#include "ocudu/adt/format.h"

using namespace ocudu;

namespace {

class dmrs_pucch_estimator_sw_factory : public dmrs_pucch_estimator_factory
{
public:
  dmrs_pucch_estimator_sw_factory(std::shared_ptr<pseudo_random_generator_factory>&      prg_factory_,
                                  std::shared_ptr<low_papr_sequence_collection_factory>& lpc_factory_,
                                  std::shared_ptr<low_papr_sequence_generator_factory>&  lpg_factory_,
                                  std::shared_ptr<port_channel_estimator_factory>&       ch_estimator_factory_) :
    prg_factory(std::move(prg_factory_)),
    lpc_factory(std::move(lpc_factory_)),
    lpg_factory(std::move(lpg_factory_)),
    ch_estimator_factory(std::move(ch_estimator_factory_))
  {
    ocudu_assert(prg_factory, "Invalid sequence generator factory.");
    ocudu_assert(lpc_factory, "Invalid sequence collection factory.");
    ocudu_assert(lpg_factory, "Invalid sequence generator factory.");
    ocudu_assert(ch_estimator_factory, "Invalid channel estimator factory.");
  }

  std::unique_ptr<dmrs_pucch_estimator> create() override
  {
    std::unique_ptr<dmrs_pucch_estimator_format2> estimator_format2 = std::make_unique<dmrs_pucch_estimator_format2>(
        prg_factory->create(),
        ch_estimator_factory->create(port_channel_estimator_fd_smoothing_strategy::filter,
                                     port_channel_estimator_td_interpolation_strategy::average,
                                     true));

    std::unique_ptr<dmrs_pucch_estimator_formats3_4> estimator_formats3_4 =
        std::make_unique<dmrs_pucch_estimator_formats3_4>(
            prg_factory->create(),
            lpg_factory->create(),
            ch_estimator_factory->create(port_channel_estimator_fd_smoothing_strategy::filter,
                                         port_channel_estimator_td_interpolation_strategy::average,
                                         /*compensate_cfo =*/false));

    return std::make_unique<dmrs_pucch_estimator_impl>(std::move(estimator_format2), std::move(estimator_formats3_4));
  }

private:
  std::shared_ptr<pseudo_random_generator_factory>      prg_factory;
  std::shared_ptr<low_papr_sequence_collection_factory> lpc_factory;
  std::shared_ptr<low_papr_sequence_generator_factory>  lpg_factory;
  std::shared_ptr<port_channel_estimator_factory>       ch_estimator_factory;
};

} // namespace

std::shared_ptr<dmrs_pucch_estimator_factory>
ocudu::create_dmrs_pucch_estimator_factory_sw(std::shared_ptr<pseudo_random_generator_factory>      prg_factory,
                                              std::shared_ptr<low_papr_sequence_collection_factory> lpc_factory,
                                              std::shared_ptr<low_papr_sequence_generator_factory>  lpg_factory,
                                              std::shared_ptr<port_channel_estimator_factory> ch_estimator_factory)
{
  return std::make_shared<dmrs_pucch_estimator_sw_factory>(prg_factory, lpc_factory, lpg_factory, ch_estimator_factory);
}
