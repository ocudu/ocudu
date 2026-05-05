// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "orbit_ephemeris_info.h"
#include "converters/ephemeris_info_converter.h"
#include "converters/reference_frame_converter.h"

using namespace ocudu;
using namespace ocudu_ntn;

orbit_ephemeris_info::orbit_ephemeris_info(orbital_propagation_model&                           propagator,
                                           time_point                                           ref_time,
                                           const state_vector&                                  state,
                                           bool                                                 eci_ref_frame,
                                           std::optional<std::chrono::system_clock::time_point> epoch_time) :
  propagation_model(propagator), ref_time_val(ref_time), epoch_time_val(epoch_time.value_or(ref_time))
{
  reference_frame_converter converter(ref_time_val);
  if (eci_ref_frame) {
    eci_rv_val  = state;
    ecef_rv_val = converter.eci_to_ecef(state, epoch_time_val);
  } else {
    ecef_rv_val = state;
    eci_rv_val  = converter.ecef_to_eci(state, epoch_time_val);
  }
}

orbit_ephemeris_info::orbit_ephemeris_info(orbital_propagation_model&                           propagator,
                                           time_point                                           ref_time,
                                           const orbital_elements&                              elements,
                                           std::optional<std::chrono::system_clock::time_point> epoch_time) :
  propagation_model(propagator), ref_time_val(ref_time), epoch_time_val(epoch_time.value_or(ref_time)), oe_val(elements)
{
}

void orbit_ephemeris_info::set_state(const state_vector&       state,
                                     bool                      eci_ref_frame,
                                     std::optional<time_point> epoch_time)
{
  epoch_time_val = epoch_time.value_or(ref_time_val);
  reference_frame_converter converter(ref_time_val);
  if (eci_ref_frame) {
    eci_rv_val  = state;
    ecef_rv_val = converter.eci_to_ecef(state, epoch_time_val);
  } else {
    ecef_rv_val = state;
    eci_rv_val  = converter.ecef_to_eci(state, epoch_time_val);
  }
  // Clear orbital elements, it will be recomputed if requested.
  oe_val = std::nullopt;
}

void orbit_ephemeris_info::set_state(const orbital_elements& elements, std::optional<time_point> epoch_time)
{
  epoch_time_val = epoch_time.value_or(ref_time_val);
  oe_val         = elements;
  // Clear ECEF and ECI state vectors, they will be recomputed if requested.
  ecef_rv_val = std::nullopt;
  eci_rv_val  = std::nullopt;
}

const state_vector& orbit_ephemeris_info::ecef_rv()
{
  if (!ecef_rv_val.has_value() && oe_val.has_value()) {
    eci_rv_val = ephemeris_info_converter::orbital_to_eci(*oe_val);
    reference_frame_converter converter(ref_time_val);
    ecef_rv_val = converter.eci_to_ecef(*eci_rv_val);
  }
  return *ecef_rv_val;
}

const state_vector& orbit_ephemeris_info::eci_rv()
{
  if (!eci_rv_val.has_value() && oe_val.has_value()) {
    eci_rv_val = ephemeris_info_converter::orbital_to_eci(*oe_val);
  }
  return *eci_rv_val;
}

const orbital_elements& orbit_ephemeris_info::oe()
{
  if (!oe_val.has_value() && eci_rv_val.has_value()) {
    oe_val = ephemeris_info_converter::eci_to_orbital(*eci_rv_val);
  }
  return *oe_val;
}

void orbit_ephemeris_info::propagate(std::chrono::duration<double> propagation_time, bool align_ref_frames)
{
  propagation_model.propagate(*this, propagation_time);

  if (align_ref_frames) {
    align_reference_frames();
  }
}

void orbit_ephemeris_info::align_reference_frames()
{
  if (ref_time_val == epoch_time_val) {
    return;
  }
  // First convert back to ECEF reference frame.
  reference_frame_converter to_ecef_converter(ref_time_val);
  ecef_rv_val = to_ecef_converter.eci_to_ecef(*eci_rv_val, epoch_time_val);

  // Update reference time.
  ref_time_val = epoch_time_val;

  // Now convert again to ECI that is aligned with ECEF at epoch time.
  reference_frame_converter to_eci_converter(ref_time_val);
  eci_rv_val = to_eci_converter.ecef_to_eci(*ecef_rv_val);

  // Clear orbital elements, it will be recomputed if requested.
  oe_val = std::nullopt;
}
