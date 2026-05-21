// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/pdcch/aggregation_level.h"
#include "ocudu/ran/pdcch/dci_format.h"
#include "ocudu/ran/pdcch/dci_packing.h"
#include <variant>

namespace ocudu {

struct cce_position {
  /// CCE start index. Values: (0..135).
  unsigned ncce;
  /// Aggregation level of the DCI.
  aggregation_level aggr_lvl;
};

/// Defines which fields are stored in the DCI payload, based on the chosen DCI format and RNTI type.
enum class dci_dl_rnti_config_type : uint8_t { si_f1_0, ra_f1_0, c_rnti_f1_0, tc_rnti_f1_0, p_rnti_f1_0, c_rnti_f1_1 };

/// Retrieve DCI format from DCI DL payload format.
inline dci_dl_format get_dci_format(dci_dl_rnti_config_type rnti_dci_type)
{
  return rnti_dci_type == dci_dl_rnti_config_type::c_rnti_f1_1 ? dci_dl_format::f1_1 : dci_dl_format::f1_0;
}

inline const char* dci_dl_rnti_config_rnti_type(dci_dl_rnti_config_type type)
{
  static constexpr std::array<const char*, 6> rnti_types = {
      "si-rnti", "ra-rnti", "c-rnti", "tc-rnti", "p-rnti", "c-rnti"};
  return (unsigned)type < rnti_types.size() ? rnti_types[(unsigned)type] : "invalid";
}

inline const char* dci_dl_rnti_config_format(dci_dl_rnti_config_type type)
{
  return dci_format_to_string(get_dci_format(type));
}

/// \brief Describes an unpacked DL DCI message.
/// \remark See FAPI DCI PDU and ORAN WG8 DL-DCI Configuration.
struct dci_dl_info {
  using payload_type = std::variant<dci_1_0_si_rnti_configuration,
                                    dci_1_0_ra_rnti_configuration,
                                    dci_1_0_c_rnti_configuration,
                                    dci_1_0_tc_rnti_configuration,
                                    dci_1_0_p_rnti_configuration,
                                    dci_1_1_configuration>;

  dci_dl_rnti_config_type type() const { return static_cast<dci_dl_rnti_config_type>(payload_.index()); }

  const dci_1_0_si_rnti_configuration& as_si_rnti_f1_0() const
  {
    return std::get<dci_1_0_si_rnti_configuration>(payload_);
  }
  dci_1_0_si_rnti_configuration& as_si_rnti_f1_0() { return std::get<dci_1_0_si_rnti_configuration>(payload_); }
  dci_1_0_si_rnti_configuration& set_si_rnti_f1_0() { return payload_.emplace<dci_1_0_si_rnti_configuration>(); }

  const dci_1_0_ra_rnti_configuration& as_ra_rnti_f1_0() const
  {
    return std::get<dci_1_0_ra_rnti_configuration>(payload_);
  }
  dci_1_0_ra_rnti_configuration& as_ra_rnti_f1_0() { return std::get<dci_1_0_ra_rnti_configuration>(payload_); }
  dci_1_0_ra_rnti_configuration& set_ra_rnti_f1_0() { return payload_.emplace<dci_1_0_ra_rnti_configuration>(); }

  const dci_1_0_c_rnti_configuration& as_c_rnti_f1_0() const
  {
    return std::get<dci_1_0_c_rnti_configuration>(payload_);
  }
  dci_1_0_c_rnti_configuration& as_c_rnti_f1_0() { return std::get<dci_1_0_c_rnti_configuration>(payload_); }
  dci_1_0_c_rnti_configuration& set_c_rnti_f1_0() { return payload_.emplace<dci_1_0_c_rnti_configuration>(); }

  const dci_1_0_tc_rnti_configuration& as_tc_rnti_f1_0() const
  {
    return std::get<dci_1_0_tc_rnti_configuration>(payload_);
  }
  dci_1_0_tc_rnti_configuration& as_tc_rnti_f1_0() { return std::get<dci_1_0_tc_rnti_configuration>(payload_); }
  dci_1_0_tc_rnti_configuration& set_tc_rnti_f1_0() { return payload_.emplace<dci_1_0_tc_rnti_configuration>(); }

  const dci_1_0_p_rnti_configuration& as_p_rnti_f1_0() const
  {
    return std::get<dci_1_0_p_rnti_configuration>(payload_);
  }
  dci_1_0_p_rnti_configuration& as_p_rnti_f1_0() { return std::get<dci_1_0_p_rnti_configuration>(payload_); }
  dci_1_0_p_rnti_configuration& set_p_rnti_f1_0() { return payload_.emplace<dci_1_0_p_rnti_configuration>(); }

  const dci_1_1_configuration& as_c_rnti_f1_1() const { return std::get<dci_1_1_configuration>(payload_); }
  dci_1_1_configuration&       as_c_rnti_f1_1() { return std::get<dci_1_1_configuration>(payload_); }
  dci_1_1_configuration&       set_c_rnti_f1_1() { return payload_.emplace<dci_1_1_configuration>(); }

private:
  payload_type payload_;
};

/// Defines which fields are stored in the DCI payload, based on the chosen DCI format and RNTI type.
enum class dci_ul_rnti_config_type : uint8_t { tc_rnti_f0_0, c_rnti_f0_0, c_rnti_f0_1 };

inline const char* dci_ul_rnti_config_rnti_type(dci_ul_rnti_config_type type)
{
  std::array<const char*, 3> rnti_types = {"tc-rnti", "c-rnti", "c-rnti"};
  return (unsigned)type < rnti_types.size() ? rnti_types[(unsigned)type] : "invalid";
}

/// Retrieve DCI format from DCI UL payload format.
inline dci_ul_format get_dci_format(dci_ul_rnti_config_type rnti_dci_type)
{
  return rnti_dci_type == dci_ul_rnti_config_type::c_rnti_f0_1 ? dci_ul_format::f0_1 : dci_ul_format::f0_0;
}

inline const char* dci_ul_rnti_config_format(dci_ul_rnti_config_type type)
{
  return dci_format_to_string(get_dci_format(type));
}

struct dci_ul_info {
  dci_ul_rnti_config_type type;
  union {
    dci_0_0_c_rnti_configuration  c_rnti_f0_0;
    dci_0_0_tc_rnti_configuration tc_rnti_f0_0;
    dci_0_1_configuration         c_rnti_f0_1;
  };

  dci_ul_info() : type(dci_ul_rnti_config_type::c_rnti_f0_0) { new (&c_rnti_f0_0) dci_0_0_c_rnti_configuration(); }
};

} // namespace ocudu
