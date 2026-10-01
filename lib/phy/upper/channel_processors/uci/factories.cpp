// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/upper/channel_processors/uci/factories.h"
#include "uci_decoder_impl.h"

using namespace ocudu;
namespace {

class uci_decoder_factory_generic : public uci_decoder_factory
{
public:
  explicit uci_decoder_factory_generic(std::shared_ptr<short_block_detector_factory> detector_factory_,
                                       std::shared_ptr<polar_factory>                polar_dec_factory_,
                                       std::shared_ptr<crc_calculator_factory>       crc_calc_factory_) :
    detector_factory(std::move(detector_factory_)),
    polar_dec_factory(std::move(polar_dec_factory_)),
    crc_calc_factory(std::move(crc_calc_factory_))
  {
    ocudu_assert(detector_factory, "Invalid detector factory.");
    ocudu_assert(polar_dec_factory, "Invalid polar decoder factory.");
    ocudu_assert(crc_calc_factory, "Invalid CRC calculator factory.");
  }

  std::unique_ptr<uci_decoder> create() override
  {
    return std::make_unique<uci_decoder_impl>(detector_factory->create(),
                                              polar_dec_factory->create_code(),
                                              polar_dec_factory->create_rate_dematcher(),
                                              polar_dec_factory->create_decoder(polar_code::NMAX_LOG),
                                              polar_dec_factory->create_deallocator(),
                                              crc_calc_factory->create(crc_generator_poly::CRC6),
                                              crc_calc_factory->create(crc_generator_poly::CRC11));
  }

private:
  std::shared_ptr<short_block_detector_factory> detector_factory;
  std::shared_ptr<polar_factory>                polar_dec_factory;
  std::shared_ptr<crc_calculator_factory>       crc_calc_factory;
};

} // namespace

std::shared_ptr<uci_decoder_factory>
ocudu::create_uci_decoder_factory_generic(std::shared_ptr<short_block_detector_factory> decoder_factory,
                                          std::shared_ptr<polar_factory>                polar_factory,
                                          std::shared_ptr<crc_calculator_factory>       crc_calc_factory)
{
  return std::make_shared<uci_decoder_factory_generic>(
      std::move(decoder_factory), std::move(polar_factory), std::move(crc_calc_factory));
}
