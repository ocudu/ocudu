// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "coordinates_types.h"
#include "propagators/orbital_propagator_model.h"
#include <chrono>
#include <optional>

namespace ocudu {
namespace ocudu_ntn {

/// Satellite orbit state that lazily converts between ECI, ECEF, and orbital-element representations on demand.
class orbit_ephemeris_info
{
public:
  using time_point = std::chrono::system_clock::time_point;

  /// \brief Construct orbit ephemeris information from a state vector.
  ///
  /// \param propagator   Reference to the orbital propagation model to be used for orbit propagation.
  /// \param ref_time     Reference time when ECEF and ECI frames are aligned.
  /// \param state        State vector (position and velocity) in either ECEF or ECI frame.
  /// \param eci_ref_frame True if the state vector is in ECI frame, false if in ECEF frame.
  /// \param current_time (Optional) Current epoch time associated with the orbit state. If not provided, use ref_time.
  orbit_ephemeris_info(orbital_propagation_model& propagator,
                       time_point                 ref_time,
                       const state_vector&        state,
                       bool                       eci_ref_frame,
                       std::optional<time_point>  current_time = std::nullopt);

  /// \brief Construct orbit ephemeris information from orbital elements.
  ///
  /// \param propagator   Reference to the orbital propagation model to be used for orbit propagation.
  /// \param ref_time     Reference time when ECEF and ECI frames are aligned.
  /// \param elements     Orbital elements in ECI frame.
  /// \param current_time (Optional) Current epoch time associated with the orbit state. If not provided, use ref_time.
  orbit_ephemeris_info(orbital_propagation_model& propagator,
                       time_point                 ref_time,
                       const orbital_elements&    elements,
                       std::optional<time_point>  current_time = std::nullopt);

  /// \brief Set the orbit state using a state vector.
  ///
  /// \param state State vector (position and velocity) in either ECEF or ECI frame.
  /// \param eci_ref_frame True if the state vector is in ECI frame, false if in ECEF frame.
  /// \param epoch_time (Optional) Epoch time associated with the new state. If not provided, uses reference time.
  void set_state(const state_vector& state, bool eci_ref_frame, std::optional<time_point> epoch_time);

  /// \brief Set the orbit state using orbital elements.
  ///
  /// \param elements Orbital elements in ECI frame.
  /// \param epoch_time (Optional) Epoch time associated with the new state. If not provided, uses reference time.
  void set_state(const orbital_elements& elements, std::optional<time_point> epoch_time);

  /// \brief Propagate the orbit state forward by a given duration.
  ///
  /// \param propagation_time Duration to propagate the orbit state.
  /// \param align_ref_frames If true, aligns the ECEF and ECI reference frames after propagation at the new epoch time.
  void propagate(std::chrono::duration<double> propagation_time, bool align_ref_frames = false);

  /// \brief Aligns the ECEF and ECI reference frames by updating the reference time.
  /// This function resets the reference time so that the ECEF and ECI frames are considered aligned at the current
  /// epoch. It updates the internal state vectors to ensure consistency between frames.
  void align_reference_frames();

  /// \brief Get the reference time when ECEF and ECI frames are aligned.
  ///
  /// \return the reference time.
  time_point reference_time() const { return ref_time_val; }

  /// \brief Get the current epoch time associated with the orbit state.
  ///
  /// \return the epoch time.
  time_point epoch_time() const { return epoch_time_val; }

  /// \brief Get the state vector (position and velocity) in the ECEF reference frame.
  ///
  /// If the state is not already in ECEF, it will be converted from ECI or orbital elements.
  /// \return const reference to the ECEF state vector.
  const state_vector& ecef_rv();

  /// \brief Get the state vector (position and velocity) in the ECI reference frame.
  ///
  /// If the state is not already in ECI state vector, it will be converted from orbital elements.
  /// \return const reference to the ECI state vector.
  const state_vector& eci_rv();

  /// \brief Get the orbital elements in the ECI reference frame.
  ///
  /// If the elements are not already available, they will be computed from the ECI state vector.
  /// \return const reference to the orbital elements.
  const orbital_elements& oe();

private:
  /// Reference to the orbital propagation model used for orbit propagation.
  orbital_propagation_model& propagation_model;
  /// Time when ECEF and ECI reference frames are aligned. It is used for conversion between ECI and ECEF ref frames.
  time_point ref_time_val;
  /// Current epoch time associated with the orbit state (may differ from ref_time_ if propagated).
  time_point epoch_time_val;
  /// Current state vector in ECEF reference frame.
  std::optional<state_vector> ecef_rv_val;
  /// Current state vector in ECI reference frame.
  std::optional<state_vector> eci_rv_val;
  /// Current orbital elements in ECI reference frame.
  std::optional<orbital_elements> oe_val;
};

} // namespace ocudu_ntn
} // namespace ocudu
