// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/expected.h"
#include "fmt/base.h"
#include <cstdint>
#include <string>

namespace ocudu {

/// \brief I-RNTI profiles for the Full-I-RNTI, as specified in TS 38.300 table F-1.
///
/// The profile indicates the width of the Local NG-RAN Node Identifier, which the Local NG-RAN Node Identifier IE of
/// TS 38.423 section 9.2.2.101 gives as 21, 18, 15 and 12 bits.
enum class full_i_rnti_profile : uint8_t { profile_0 = 0b00, profile_1 = 0b01, profile_2 = 0b10, profile_3 = 0b11 };
inline bool from_string(full_i_rnti_profile& profile, const std::string& str)
{
  if (str == "profile0") {
    profile = full_i_rnti_profile::profile_0;
    return true;
  }
  if (str == "profile1") {
    profile = full_i_rnti_profile::profile_1;
    return true;
  }
  if (str == "profile2") {
    profile = full_i_rnti_profile::profile_2;
    return true;
  }
  if (str == "profile3") {
    profile = full_i_rnti_profile::profile_3;
    return true;
  }
  return false;
}

/// \brief I-RNTI profiles for the Short-I-RNTI, as specified in TS 38.300 table F-2.
///
/// The profile indicates the width of the Local NG-RAN Node Identifier, which the Local NG-RAN Node Identifier IE of
/// TS 38.423 section 9.2.2.101 gives as 8 and 6 bits.
enum class short_i_rnti_profile : uint8_t { profile_0 = 0b0, profile_1 = 0b1 };
inline bool from_string(short_i_rnti_profile& profile, const std::string& str)
{
  if (str == "profile0") {
    profile = short_i_rnti_profile::profile_0;
    return true;
  }
  if (str == "profile1") {
    profile = short_i_rnti_profile::profile_1;
    return true;
  }
  return false;
}

/// \brief Common type for Full-I-RNTI (40 Bit) as specified in TS 38.331 section 6.3.2.
///
/// The value is composed as specified in TS 38.300 Annex F, starting from the MSB: 2 bits of I-RNTI profile, the Local
/// NG-RAN Node Identifier of the node that allocated the I-RNTI, and a reference to the UE context stored in that
/// node. The profile gives the width of the node identifier, so a node reading an I-RNTI it did not allocate recovers
/// both fields from the value alone.
///
/// The UE reference should not be derived from the gNB UE ID to avoid the possibility of UE tracking, as specified in
/// TS 33.501 section 6.8.2.1.2.
class full_i_rnti_t
{
public:
  /// Total width of a Full-I-RNTI, and the width of the I-RNTI profile it starts with.
  static constexpr unsigned nof_bits         = 40;
  static constexpr unsigned nof_profile_bits = 2;

  /// Width of the Local NG-RAN Node Identifier carried by the given profile.
  static constexpr unsigned nof_node_id_bits(full_i_rnti_profile profile)
  {
    switch (profile) {
      case full_i_rnti_profile::profile_1:
        return 18;
      case full_i_rnti_profile::profile_2:
        return 15;
      case full_i_rnti_profile::profile_3:
        return 12;
      default:
        return 21;
    }
  }

  /// Width of the UE reference left by the given profile.
  static constexpr unsigned nof_ue_ref_bits(full_i_rnti_profile profile)
  {
    return nof_bits - nof_profile_bits - nof_node_id_bits(profile);
  }

  /// Highest UE reference a Full-I-RNTI of the given profile can carry.
  static constexpr uint32_t max_ue_ref(full_i_rnti_profile profile) { return (1U << nof_ue_ref_bits(profile)) - 1; }

  /// \brief Derive the Local NG-RAN Node Identifier of a node from its gNB ID.
  ///
  /// The identifier is shorter than a gNB ID, so it carries the least significant bits of it, following the NG-RAN
  /// node address index of TS 38.300 Annex C. Nodes whose gNB IDs differ only in the bits above the width of the
  /// profile share an identifier.
  static constexpr uint32_t to_local_node_id(full_i_rnti_profile profile, uint32_t gnb_id)
  {
    return gnb_id & ((1U << nof_node_id_bits(profile)) - 1);
  }

  /// Creates a Full-I-RNTI from an I-RNTI profile, a Local NG-RAN Node Identifier and a UE reference.
  /// \param[in] profile_ I-RNTI profile giving the width of the node identifier.
  /// \param[in] node_id_ Local NG-RAN Node Identifier of the node allocating the I-RNTI.
  /// \param[in] ue_ref_ Reference to the UE context in that node.
  constexpr full_i_rnti_t(full_i_rnti_profile profile_, uint32_t node_id_, uint32_t ue_ref_) :
    profile_val(profile_),
    node_id_val(node_id_ & ((1U << nof_node_id_bits(profile_)) - 1)),
    ue_ref_val(ue_ref_ & max_ue_ref(profile_))
  {
    val = (static_cast<uint64_t>(profile_val) << (nof_bits - nof_profile_bits)) |
          (static_cast<uint64_t>(node_id_val) << nof_ue_ref_bits(profile_)) | ue_ref_val;
  }

  /// Creates a Full-I-RNTI from its integer representation, taking the profile from the leading bits.
  /// \param[in] value Integer representation of the Full-I-RNTI.
  /// \return The created Full-I-RNTI, or an error if the value exceeds 40 bits.
  static expected<full_i_rnti_t> from_uint(uint64_t value)
  {
    if (value > 0xffffffffff) {
      return make_unexpected(default_error_t{});
    }

    const auto profile = static_cast<full_i_rnti_profile>(value >> (nof_bits - nof_profile_bits));
    return full_i_rnti_t{profile,
                         static_cast<uint32_t>(value >> nof_ue_ref_bits(profile)),
                         static_cast<uint32_t>(value & max_ue_ref(profile))};
  }

  /// Returns the Full-I-RNTI value.
  uint64_t value() const { return val; }

  /// Returns the I-RNTI profile the Full-I-RNTI was composed with.
  full_i_rnti_profile profile() const { return profile_val; }

  /// \brief Returns the Local NG-RAN Node Identifier encoded in the Full-I-RNTI, i.e. the identifier of the node that
  /// allocated it.
  uint32_t node_id() const { return node_id_val; }

  /// Returns the UE context reference encoded in the Full-I-RNTI.
  uint32_t ue_ref() const { return ue_ref_val; }

  bool operator==(const full_i_rnti_t& i_rnti) const { return val == i_rnti.val; }
  bool operator!=(const full_i_rnti_t& i_rnti) const { return val != i_rnti.val; }
  bool operator<(const full_i_rnti_t& i_rnti) const { return val < i_rnti.val; }
  bool operator<=(const full_i_rnti_t& i_rnti) const { return val <= i_rnti.val; }
  bool operator>(const full_i_rnti_t& i_rnti) const { return val > i_rnti.val; }
  bool operator>=(const full_i_rnti_t& i_rnti) const { return val >= i_rnti.val; }

private:
  uint64_t            val = 0;
  full_i_rnti_profile profile_val;
  uint32_t            node_id_val = 0;
  uint32_t            ue_ref_val  = 0;
};

/// \brief Common type for Short-I-RNTI (24 Bit) as specified in TS 38.331 section 6.3.2. The width is confirmed by the
/// I-RNTI IE in TS 38.423 section 9.2.3.46, which cross-references the ShortI-RNTI-Value IE of TS 38.331.
///
/// The value is composed as specified in TS 38.300 Annex F, starting from the MSB: 1 bit of I-RNTI profile, the Local
/// NG-RAN Node Identifier of the node that allocated the I-RNTI, and a reference to the UE context stored in that
/// node. The profile gives the width of the node identifier, so a node reading an I-RNTI it did not allocate recovers
/// both fields from the value alone.
///
/// The UE reference should not be derived from the gNB UE ID to avoid the possibility of UE tracking, as specified in
/// TS 33.501 section 6.8.2.1.2.
class short_i_rnti_t
{
public:
  /// Total width of a Short-I-RNTI, and the width of the I-RNTI profile it starts with.
  static constexpr unsigned nof_bits         = 24;
  static constexpr unsigned nof_profile_bits = 1;

  /// Width of the Local NG-RAN Node Identifier carried by the given profile.
  static constexpr unsigned nof_node_id_bits(short_i_rnti_profile profile)
  {
    return profile == short_i_rnti_profile::profile_1 ? 6 : 8;
  }

  /// Width of the UE reference left by the given profile.
  static constexpr unsigned nof_ue_ref_bits(short_i_rnti_profile profile)
  {
    return nof_bits - nof_profile_bits - nof_node_id_bits(profile);
  }

  /// Highest UE reference a Short-I-RNTI of the given profile can carry.
  static constexpr uint32_t max_ue_ref(short_i_rnti_profile profile) { return (1U << nof_ue_ref_bits(profile)) - 1; }

  /// \brief Derive the Local NG-RAN Node Identifier of a node from its gNB ID.
  ///
  /// The identifier is shorter than a gNB ID, so it carries the least significant bits of it, following the NG-RAN
  /// node address index of TS 38.300 Annex C. Nodes whose gNB IDs differ only in the bits above the width of the
  /// profile share an identifier.
  static constexpr uint32_t to_local_node_id(short_i_rnti_profile profile, uint32_t gnb_id)
  {
    return gnb_id & ((1U << nof_node_id_bits(profile)) - 1);
  }

  /// Creates a Short-I-RNTI from an I-RNTI profile, a Local NG-RAN Node Identifier and a UE reference.
  /// \param[in] profile_ I-RNTI profile giving the width of the node identifier.
  /// \param[in] node_id_ Local NG-RAN Node Identifier of the node allocating the I-RNTI.
  /// \param[in] ue_ref_ Reference to the UE context in that node.
  constexpr short_i_rnti_t(short_i_rnti_profile profile_, uint32_t node_id_, uint32_t ue_ref_) :
    profile_val(profile_),
    node_id_val(node_id_ & ((1U << nof_node_id_bits(profile_)) - 1)),
    ue_ref_val(ue_ref_ & max_ue_ref(profile_))
  {
    val = (static_cast<uint32_t>(profile_val) << (nof_bits - nof_profile_bits)) |
          (node_id_val << nof_ue_ref_bits(profile_)) | ue_ref_val;
  }

  /// Creates a Short-I-RNTI from its integer representation, taking the profile from the leading bit.
  /// \param[in] value Integer representation of the Short-I-RNTI.
  /// \return The created Short-I-RNTI, or an error if the value exceeds 24 bits.
  static expected<short_i_rnti_t> from_uint(uint32_t value)
  {
    if (value > 0xffffff) {
      return make_unexpected(default_error_t{});
    }

    const auto profile = static_cast<short_i_rnti_profile>(value >> (nof_bits - nof_profile_bits));
    return short_i_rnti_t{profile, value >> nof_ue_ref_bits(profile), value & max_ue_ref(profile)};
  }

  /// Returns the Short-I-RNTI value.
  uint32_t value() const { return val; }

  /// Returns the I-RNTI profile the Short-I-RNTI was composed with.
  short_i_rnti_profile profile() const { return profile_val; }

  /// \brief Returns the Local NG-RAN Node Identifier encoded in the Short-I-RNTI, i.e. the identifier of the node that
  /// allocated it.
  uint32_t node_id() const { return node_id_val; }

  /// Returns the UE context reference encoded in the Short-I-RNTI.
  uint32_t ue_ref() const { return ue_ref_val; }

  bool operator==(const short_i_rnti_t& i_rnti) const { return val == i_rnti.val; }
  bool operator!=(const short_i_rnti_t& i_rnti) const { return val != i_rnti.val; }
  bool operator<(const short_i_rnti_t& i_rnti) const { return val < i_rnti.val; }
  bool operator<=(const short_i_rnti_t& i_rnti) const { return val <= i_rnti.val; }
  bool operator>(const short_i_rnti_t& i_rnti) const { return val > i_rnti.val; }
  bool operator>=(const short_i_rnti_t& i_rnti) const { return val >= i_rnti.val; }

private:
  uint32_t             val = 0;
  short_i_rnti_profile profile_val;
  uint32_t             node_id_val = 0;
  uint32_t             ue_ref_val  = 0;
};

/// \brief Common type for Full- and Short-I-RNTI pair.
struct i_rntis_t {
  short_i_rnti_t short_i_rnti;
  full_i_rnti_t  full_i_rnti;
};

} // namespace ocudu

// Formatters
namespace fmt {

template <>
struct formatter<ocudu::full_i_rnti_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::full_i_rnti_t i_rnti, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "full-i-rnti={:#x}", i_rnti.value());
  }
};

template <>
struct formatter<ocudu::short_i_rnti_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::short_i_rnti_t i_rnti, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "short-i-rnti={:#x}", i_rnti.value());
  }
};

} // namespace fmt
