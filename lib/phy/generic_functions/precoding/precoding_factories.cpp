// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/generic_functions/precoding/precoding_factories.h"
#include "channel_precoder_generic.h"
#include "ocudu/adt/format.h"
#include "ocudu/support/cpu_features.h"

#ifdef __x86_64__
#include "channel_precoder_avx2.h"
#include "channel_precoder_avx512.h"
#endif // __x86_64__
#ifdef __ARM_NEON
#include "channel_precoder_neon.h"
#endif // __ARM_NEON

using namespace ocudu;

namespace {

class channel_precoder_factory_impl : public channel_precoder_factory
{
public:
  explicit channel_precoder_factory_impl(std::string precoder_type_) : precoder_type(std::move(precoder_type_)) {}

  std::unique_ptr<channel_precoder> create() override
  {
#ifdef __x86_64__
    bool supports_avx512 = cpu_supports_feature(cpu_feature::avx512f) && cpu_supports_feature(cpu_feature::avx512bw);
    bool supports_avx2   = cpu_supports_feature(cpu_feature::avx2);
    bool supports_fma    = cpu_supports_feature(cpu_feature::fma);
    if (((precoder_type == "avx512") || (precoder_type == "auto")) && supports_avx512) {
      return std::make_unique<channel_precoder_avx512>();
    }
    if (((precoder_type == "avx2") || (precoder_type == "auto")) && supports_avx2 && supports_fma) {
      return std::make_unique<channel_precoder_avx2>();
    }
#endif // __x86_64__
#ifdef __ARM_NEON
    bool supports_neon = cpu_supports_feature(cpu_feature::neon);
    if (((precoder_type == "neon") || (precoder_type == "auto")) && supports_neon) {
      return std::make_unique<channel_precoder_neon>();
    }
#endif // //__ARM_NEON

    if ((precoder_type == "generic") || (precoder_type == "auto")) {
      return std::make_unique<channel_precoder_generic>();
    }
    // Do not instantiate the channel precoder if the hardware does not support the implementation.
    return nullptr;
  }

private:
  std::string precoder_type;
};

} // namespace

std::shared_ptr<channel_precoder_factory> ocudu::create_channel_precoder_factory(const std::string& precoder_type)
{
  return std::make_shared<channel_precoder_factory_impl>(precoder_type);
}
