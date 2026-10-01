// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ofh/ecpri/ecpri_factories.h"
#include "ecpri_packet_builder_impl.h"
#include "ecpri_packet_decoder_impl.h"

using namespace ocudu;
using namespace ecpri;

std::unique_ptr<packet_builder> ocudu::ecpri::create_ecpri_packet_builder()
{
  return std::make_unique<packet_builder_impl>();
}

std::unique_ptr<packet_decoder>
ocudu::ecpri::create_ecpri_packet_decoder_using_payload_size(ocudulog::basic_logger& logger, unsigned sector)
{
  return std::make_unique<packet_decoder_use_header_payload_size>(logger, sector);
}

std::unique_ptr<packet_decoder>
ocudu::ecpri::create_ecpri_packet_decoder_ignoring_payload_size(ocudulog::basic_logger& logger, unsigned sector)
{
  return std::make_unique<packet_decoder_ignore_header_payload_size>(logger, sector);
}
