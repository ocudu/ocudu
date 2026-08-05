# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

import time
import argparse
import numpy as np
from datetime import datetime, timedelta, timezone
from dataclasses import dataclass
from typing import Tuple, Union, List, Dict
import skyfield
from skyfield.api import EarthSatellite, load, wgs84, utc
from skyfield.data.spice import inertial_frames
from skyfield.framelib import itrs
from sgp4.api import Satrec, WGS84
import yaml

### YAML helpers ###
class HexInt(int):
    """Integer that serializes to YAML in hexadecimal (e.g. nr_cell_id: 0x19b0)."""
    pass

def _represent_hexint(dumper, data):
    return dumper.represent_scalar('tag:yaml.org,2002:int', hex(data))

yaml.add_representer(HexInt, _represent_hexint)

def make_nr_cell_id(gnb_id, gnb_id_bit_length, cell_id):
    """Build a 36-bit NR Cell Identity: gNB ID in the top bits, cell ID in the low bits."""
    return HexInt((gnb_id << (36 - gnb_id_bit_length)) | cell_id)

### Converters ###
class ReferenceFrameConverter:
    """Converter between ECEF and ECI reference frames.

    This class provides methods to convert state vectors (position and velocity) between
    Earth-Centered Earth-Fixed (ECEF) and Earth-Centered Inertial (ECI) reference frames,
    accounting for Earth's rotation and the resulting Coriolis effects on velocities.
    """

    # Earth's rotation rate in rad/s
    EARTH_ROTATION_RATE = 7.292115146706979e-5

    def __init__(self, epoch_time: datetime):
        """Initialize the coordinate converter.

        Args:
            epoch_time: Reference time point for coordinate conversions
        """
        self.epoch_time = epoch_time

    def ecef_to_eci(self, ecef_pos: Union[np.ndarray, List[float], Tuple[float, float, float]],
                    ecef_vel: Union[np.ndarray, List[float], Tuple[float, float, float]],
                    current_time: datetime) -> Dict[str, np.ndarray]:
        """Convert ECEF coordinates and velocities to ECI frame.

        Args:
            ecef_pos: Position vector in ECEF frame [x, y, z] in meters
            ecef_vel: Velocity vector in ECEF frame [vx, vy, vz] in meters/second
            current_time: Current time point for the conversion

        Returns:
            Dictionary containing:
            - 'position': Position vector in ECI frame [x, y, z] in meters
            - 'velocity': Velocity vector in ECI frame [vx, vy, vz] in meters/second
        """
        # Convert inputs to numpy arrays if needed
        ecef_pos = np.asarray(ecef_pos, dtype=float)
        ecef_vel = np.asarray(ecef_vel, dtype=float)

        # Calculate time difference in seconds
        dt = (current_time - self.epoch_time).total_seconds()

        # Calculate rotation angle
        angle = self.EARTH_ROTATION_RATE * dt

        # Rotation matrix components
        cos_angle = np.cos(angle)
        sin_angle = np.sin(angle)

        # Apply rotation to position
        eci_pos = np.zeros(3)
        eci_pos[0] = ecef_pos[0] * cos_angle - ecef_pos[1] * sin_angle
        eci_pos[1] = ecef_pos[0] * sin_angle + ecef_pos[1] * cos_angle
        eci_pos[2] = ecef_pos[2]

        # Apply rotation to velocity (including Coriolis effect)
        eci_vel = np.zeros(3)
        eci_vel[0] = (ecef_vel[0] * cos_angle - ecef_vel[1] * sin_angle - self.EARTH_ROTATION_RATE * ecef_pos[1] * cos_angle - self.EARTH_ROTATION_RATE * ecef_pos[0] * sin_angle)
        eci_vel[1] = (ecef_vel[0] * sin_angle + ecef_vel[1] * cos_angle + self.EARTH_ROTATION_RATE * ecef_pos[0] * cos_angle - self.EARTH_ROTATION_RATE * ecef_pos[1] * sin_angle)
        eci_vel[2] = ecef_vel[2]

        return {'position': eci_pos, 'velocity': eci_vel}

@dataclass
class OrbitalParams:
    """Orbital parameters for a satellite orbit.

    Attributes:
        semi_major_axis: Semi-major axis in meters
        eccentricity: Orbital eccentricity (dimensionless)
        inclination: Orbital inclination in radians
        longitude: Longitude of ascending node in radians
        periapsis: Argument of periapsis in radians
        mean_anomaly: Mean anomaly in radians
    """
    semi_major_axis: float
    eccentricity: float
    inclination: float
    longitude: float
    periapsis: float
    mean_anomaly: float

class EphemerisInfoConverter:
    """Converter between ECI state vectors and orbital elements.

    This class provides methods to convert between position and velocity vectors in the
    Earth-Centered Inertial (ECI) frame and classical orbital elements.
    """

    # Earth's gravitational parameter (GM) [m³/s²]
    MU = 3.986004418e14

    @staticmethod
    def rv_to_oe(position: Union[np.ndarray, List[float], Tuple[float, float, float]],
                 velocity: Union[np.ndarray, List[float], Tuple[float, float, float]]) -> OrbitalParams:
        """Convert ECI state vector to orbital elements.

        Args:
            position: ECI position vector [x, y, z] in meters
            velocity: ECI velocity vector [vx, vy, vz] in meters/second

        Returns:
            Orbital parameters object containing the classical orbital elements
        """
        # Convert inputs to numpy arrays if needed
        position = np.asarray(position, dtype=float)
        velocity = np.asarray(velocity, dtype=float)

        # Calculate specific angular momentum
        h = np.cross(position, velocity)

        # Calculate node vector
        n = np.array([-h[1], h[0], 0.0])

        # Calculate eccentricity vector
        v2 = np.dot(velocity, velocity)
        r = np.sqrt(np.dot(position, position))

        ev = np.array([
            (v2 - EphemerisInfoConverter.MU / r) * position[0] - np.dot(position, velocity) * velocity[0],
            (v2 - EphemerisInfoConverter.MU / r) * position[1] - np.dot(position, velocity) * velocity[1],
            (v2 - EphemerisInfoConverter.MU / r) * position[2] - np.dot(position, velocity) * velocity[2]
        ]) / EphemerisInfoConverter.MU

        # Calculate orbital parameters
        # Semi-major axis
        a = -EphemerisInfoConverter.MU / (2.0 * (v2 / 2.0 - EphemerisInfoConverter.MU / r))

        # Eccentricity
        eccentricity = np.sqrt(np.dot(ev, ev))

        # Inclination
        h_mag = np.sqrt(np.dot(h, h))
        inclination = np.arccos(h[2] / h_mag)

        # Longitude of ascending node
        n_mag = np.sqrt(np.dot(n, n))
        longitude = np.arctan2(n[1], n[0])
        if longitude < 0:
            longitude += 2.0 * np.pi

        # Argument of periapsis
        ev_mag = np.sqrt(np.dot(ev, ev))
        periapsis = np.arccos(np.dot(n, ev) / (n_mag * ev_mag))
        if ev[2] < 0:
            periapsis = 2.0 * np.pi - periapsis

        # True anomaly
        true_anomaly = np.arccos(np.dot(ev, position) / (ev_mag * r))
        if np.dot(position, velocity) < 0:
            true_anomaly = 2.0 * np.pi - true_anomaly

        # Eccentric anomaly
        E = 2.0 * np.arctan(np.sqrt((1.0 - eccentricity) / (1.0 + eccentricity)) * np.tan(true_anomaly / 2.0))

        # Mean anomaly
        mean_anomaly = E - eccentricity * np.sin(E)
        if mean_anomaly < 0:
            mean_anomaly += 2.0 * np.pi

        return OrbitalParams(
            semi_major_axis=a,
            eccentricity=eccentricity,
            inclination=inclination,
            longitude=longitude,
            periapsis=periapsis,
            mean_anomaly=mean_anomaly
        )

### Helper Functions ###
def str_to_bool(value):
    if isinstance(value, bool):
        return value
    if value.lower() in {'false', 'f', '0', 'no', 'n'}:
        return False
    elif value.lower() in {'true', 't', '1', 'yes', 'y'}:
        return True
    raise ValueError(f'{value} is not a valid boolean value')

def convert_numpy_types(obj):
    if isinstance(obj, dict):
        return {k: convert_numpy_types(v) for k, v in obj.items()}
    elif isinstance(obj, list):
        return [convert_numpy_types(i) for i in obj]
    elif isinstance(obj, np.generic):
        return obj.item()
    else:
        return obj

def save_ground_position_cfg(filename, loc):
    template = (
        "latitude: {latitude},\n"
        "longitude: {longitude},\n"
        "altitude: {altitude}\n"
    )
    config_content = template.format(latitude=round(loc.latitude.degrees,9), longitude=round(loc.longitude.degrees,9), altitude=int(loc.elevation.m))
    with open(filename, "w") as config_file:
        config_file.write(config_content)

def save_gnb_ntn_config(filename, ntn_config_dict):
    with open(filename, "w") as config_file:
        yaml.dump(convert_numpy_types(ntn_config_dict), config_file, default_flow_style=False, sort_keys=False)

def datetime_to_tle_epoch(dt):
    year = dt.year % 100  # Last two digits of the year
    day_of_year = dt.timetuple().tm_yday  # Day of the year
    fraction = (dt.hour * 3600 + dt.minute * 60 + dt.second) / 86400.0  # Fractional portion of the day

    formatted_fraction = "{:.8f}".format(fraction).lstrip('0').lstrip('.')
    epoch = "{:02d}{:03d}.{}".format(year, day_of_year, formatted_fraction)
    return epoch

def load_tle_from_file(filename):
    try:
        with open(filename, 'r') as file:
            lines = file.readlines()
            lines = [line.strip() for line in lines]
            if (len(lines) == 3):
                return lines[0], lines[1], lines[2]
            if (len(lines) == 2):
                return "No-name", lines[0], lines[1]
            return split_lines
    except FileNotFoundError:
        print(f"File '{filename}' not found.")
        return None,None,None

def save_tle_to_file(filename, tle_name, tle_line1, tle_line2):
    with open(filename, 'w') as file:
        file.write(f"{tle_name}\n")
        file.write(f"{tle_line1}\n")
        file.write(f"{tle_line2}\n")

def overwrite_tle_epoch(sat_name, line1, line2, epoch_dt, original_epoch_dt=None):
    tle_epoch_time = datetime_to_tle_epoch(epoch_dt)
    line1 = line1[:18] + tle_epoch_time + line1[32:]  # TODO: TLE CRC update?
    if original_epoch_dt is not None:
        # Rotate RAAN to compensate for Earth's rotation since the original epoch,
        # so the satellite's ECEF ground track stays fixed regardless of when the
        # script runs.
        delta_t = (epoch_dt - original_epoch_dt).total_seconds()
        delta_raan_deg = np.degrees(ReferenceFrameConverter.EARTH_ROTATION_RATE) * delta_t
        old_raan = float(line2[17:25])
        new_raan = (old_raan + delta_raan_deg) % 360.0
        line2 = line2[:17] + "{:8.4f}".format(new_raan) + line2[25:]
    return sat_name, line1, line2

def get_lla_at_dt(ts, satellite, timestamp=None):
    if timestamp is not None:
        if isinstance(timestamp, datetime):
            timestamp_ts = ts.from_datetime(timestamp)
        if isinstance(timestamp, skyfield.timelib.Time):
            timestamp_ts = timestamp
    else:
        timestamp_ts = satellite.epoch
    geocentric = satellite.at(timestamp_ts)
    latitude, longitude = wgs84.latlon_of(geocentric)
    position = wgs84.geographic_position_of(geocentric)
    altitude = wgs84.height_of(geocentric)
    return latitude, longitude, altitude

def find_pass_over_ground_station(ts, satellite, subpoint, timestamp_dt, time_window, min_elevation_degrees=0, debug=False):
    t0 = ts.from_datetime(timestamp_dt - time_window)
    t1 = ts.from_datetime(timestamp_dt + time_window)
    t, events = satellite.find_events(subpoint, t0, t1, altitude_degrees=min_elevation_degrees)
    event_names = 'rise above {}°'.format(min_elevation_degrees), 'culminate', 'set below {}°'.format(min_elevation_degrees)
    AOS_ts = None
    TCA_ts = None
    LOS_ts = None

    if (debug):
        print("Example Events above the UE position: ")
    for ti, event in zip(t, events):
        name = event_names[event]
        if AOS_ts is None and event == 0:
            AOS_ts = ti
        if TCA_ts is None and event == 1:
            TCA_ts = ti
        if LOS_ts is None and event == 2:
            LOS_ts = ti
        if (debug):
            print("  ",ti.utc_strftime('%Y %b %d %H:%M:%S'), name)

    # remove second frac part
    AOS_dt = AOS_ts.utc_datetime().replace(microsecond=0)
    TCA_dt = TCA_ts.utc_datetime().replace(microsecond=0)
    LOS_dt = LOS_ts.utc_datetime().replace(microsecond=0)
    return AOS_dt, TCA_dt, LOS_dt

def estimate_pass_duration(ts, satellite, min_elevation_deg):
    geocentric = satellite.at(satellite.epoch)
    latitude, longitude = wgs84.latlon_of(geocentric)
    subpoint = wgs84.latlon(latitude.degrees, longitude.degrees, 1)
    time_window = timedelta(minutes=30)
    AOS_dt, TCA_dt, LOS_dt = find_pass_over_ground_station(ts, satellite, subpoint, satellite.epoch.utc_datetime(), time_window, min_elevation_deg)
    return (LOS_dt - AOS_dt)

def find_real_time_ntn_scenario(ts, tle_fn, min_sat_elevation, pass_start_offset, start_time_str=None):
    # Load TLE data
    sat_name, line1, line2 = load_tle_from_file(tle_fn)

    # Capture the original epoch before overwriting, used as the RAAN rotation reference.
    original_satrec = Satrec.twoline2rv(line1, line2, WGS84)
    original_satellite = EarthSatellite.from_satrec(original_satrec, ts)
    original_epoch_dt = original_satellite.epoch.utc_datetime().replace(microsecond=0, tzinfo=None)

    # Reuse the TLE to generate a pass at current time (overwrite epoch)
    pass_start_dt = datetime.utcnow().replace(microsecond=0)

    if start_time_str is not None:
        pass_start_dt = datetime.strptime(start_time_str, "%Y-%m-%dT%H:%M:%S")

    print("Updated TLE epoch time: ", pass_start_dt)
    sat_name, line1, line2 = overwrite_tle_epoch(sat_name, line1, line2, pass_start_dt, original_epoch_dt)
    # Save the updated TLE
    updated_tle_fn = "tle_updated.txt"
    save_tle_to_file(updated_tle_fn, sat_name, line1, line2)
    print("Saved updated TLE to file: ", updated_tle_fn)

    # Create Satellite object with the updated TLE.
    satrec = Satrec.twoline2rv(line1, line2, WGS84)
    satellite = EarthSatellite.from_satrec(satrec, ts)
    #satellite = EarthSatellite(line1, line2, sat_name, ts)

    geocentric = satellite.at(satellite.epoch)
    sat_latitude, sat_longitude = wgs84.latlon_of(geocentric)
    altitude = wgs84.height_of(geocentric)
    # TODO: need a better check
    if (altitude.km > 30000):
        orbit_type = "GEO"
    else:
        orbit_type = "LEO"

    # Estimate pass duration
    pass_duration_s = 0
    if (orbit_type == "LEO"):
        pass_duration_s = estimate_pass_duration(ts, satellite, min_sat_elevation).seconds

    # Compute the pass TCA timestamp as (pass_start_dt + offset + pass_duration/2)
    pass_tca_dt = pass_start_dt + timedelta(seconds=pass_start_offset) + timedelta(seconds=int(pass_duration_s / 2))
    pass_tca_dt = pass_tca_dt.replace(tzinfo=utc)

    # Compute satellite's LLA coordinates at the expected pass TCA timepoint
    latitude, longitude, altitude = get_lla_at_dt(ts, satellite, pass_tca_dt)
    elevation = skyfield.units.Distance(m=1)
    cell_center_subpoint = wgs84.latlon(latitude.degrees, longitude.degrees, elevation.m)
    
    if orbit_type == "GEO":
        AOS_dt = satellite.epoch.utc_datetime()
        TCA_dt = AOS_dt
        LOS_dt = AOS_dt + timedelta(hours=1)
    else:
        # For LEO find a pass over the UE, between TLE Epoch -0.5h and epoch +0.5h
        time_window = timedelta(minutes=30)
        AOS_dt, TCA_dt, LOS_dt = find_pass_over_ground_station(ts, satellite, cell_center_subpoint, pass_tca_dt, time_window, min_sat_elevation)

    # Calculate NTN link delay at AOS, TCA and LOS
    difference = satellite - cell_center_subpoint

    speed_of_light_km_per_s = 299792.458
    topocentric = difference.at(ts.from_datetime(AOS_dt))
    aos_alt, aos_az, aos_distance = topocentric.altaz()
    aos_propagation_delay_us = int(aos_distance.km / speed_of_light_km_per_s * 1e6)

    topocentric = difference.at(ts.from_datetime(TCA_dt))
    tca_alt, tca_az, tca_distance = topocentric.altaz()
    tca_propagation_delay_us = int(tca_distance.km / speed_of_light_km_per_s * 1e6)

    topocentric = difference.at(ts.from_datetime(LOS_dt))
    los_alt, los_az, los_distance = topocentric.altaz()
    los_propagation_delay_us = int(los_distance.km / speed_of_light_km_per_s * 1e6)

    print("")
    print("Satelite:")
    print("--TLE Epoch datetime: ", satellite.epoch.utc_datetime())
    print('--Satelite position [km]:', geocentric.position.km)
    print('--Satelite velocity [km/s]:', geocentric.velocity.km_per_s)
    print('--Satelite LLA coordinates')
    print('----Latitude [deg]:', latitude.degrees)
    print('----Longitude [deg]:', longitude.degrees)
    print('----Altitude [km]:' , altitude.km)
    print("")
    print("Cell Center Position: ")
    print('--Cell Type:', orbit_type)
    print('--Latitude [deg]:', latitude.degrees)
    print('--Longitude [deg]:', longitude.degrees)
    print('--Elevation [m]:' , elevation.m)
    if orbit_type == "GEO":
        print('--AOS at:', AOS_dt)
        print('----Altitude:',  round(aos_alt.degrees,2))
        print('----Azimuth:', round(aos_az.degrees,2))
        print('----Distance: {:.1f} km'.format(aos_distance.km))
        print('----Propagation Delay: {} us'.format(aos_propagation_delay_us))
    else:
        print('--Estimated Pass Duration [s]:', pass_duration_s)
        print('--AOS at:', AOS_dt)
        print('----Altitude:',  round(aos_alt.degrees,2))
        print('----Azimuth:', round(aos_az.degrees,2))
        print('----Distance: {:.1f} km'.format(aos_distance.km))
        print('----Propagation Delay: {} us'.format(aos_propagation_delay_us))
        print('--TCA at:', TCA_dt)
        print('----Altitude:',  round(tca_alt.degrees,2))
        print('----Azimuth:', round(tca_az.degrees,2))
        print('----Distance: {:.1f} km'.format(tca_distance.km))
        print('----Propagation Delay: {} us'.format(tca_propagation_delay_us))
        print('--LOS at:', LOS_dt)
        print('----Altitude:',  round(los_alt.degrees,2))
        print('----Azimuth:', round(los_az.degrees,2))
        print('----Distance: {:.1f} km'.format(los_distance.km))
        print('----Propagation Delay: {} us'.format(los_propagation_delay_us))
        print("")

    return satellite, cell_center_subpoint, AOS_dt, LOS_dt

def compute_ephemeris(ts, satellite, epoch_dt, sample_dt, ephemeris_info_format="ecef"):
    """Compute a satellite's ephemeris block for the gNB NTN config.

    The ephemeris describes the satellite state referenced to epoch_dt, using the satellite's
    actual state at sample_dt. For the serving satellite epoch_dt == sample_dt (state at its
    own epoch). For a phase-lagged switch target, sample_dt precedes epoch_dt so that,
    propagated forward, the satellite reaches the serving satellite's AOS geometry at the
    handover time.

    Note: The ECI and ECEF coincide at epoch_dt, i.e. x,y,z axes in ECEF are aligned with the
    x,y,z axes in ECI at epoch_dt.

    Returns a dict with either "ephemeris_info_ecef" or "ephemeris_orbital".
    """
    sample_ts = ts.from_datetime(sample_dt.replace(tzinfo=utc))
    geocentric = satellite.at(sample_ts)
    sat_position, sat_velocity = geocentric.frame_xyz_and_velocity(itrs)

    # ECEF state vector
    sat_pos_x = sat_position.m[0]
    sat_pos_y = sat_position.m[1]
    sat_pos_z = sat_position.m[2]
    sat_vel_x = sat_velocity.m_per_s[0]
    sat_vel_y = sat_velocity.m_per_s[1]
    sat_vel_z = sat_velocity.m_per_s[2]

    if (ephemeris_info_format == "ecef"):
        return {"ephemeris_info_ecef": {
            "pos_x": sat_pos_x,
            "pos_y": sat_pos_y,
            "pos_z": sat_pos_z,
            "vel_x": sat_vel_x,
            "vel_y": sat_vel_y,
            "vel_z": sat_vel_z
        }}

    # Orbital parameters: convert the ECEF state vector to orbital elements.
    converter = ReferenceFrameConverter(epoch_dt)
    ecef_pos = np.array([sat_pos_x, sat_pos_y, sat_pos_z])
    ecef_vel = np.array([sat_vel_x, sat_vel_y, sat_vel_z])
    eci_rv = converter.ecef_to_eci(ecef_pos, ecef_vel, sample_dt)
    oe = EphemerisInfoConverter.rv_to_oe(eci_rv['position'], eci_rv['velocity'])
    return {"ephemeris_orbital": {
        "semi_major_axis": oe.semi_major_axis,
        "eccentricity": oe.eccentricity,
        "inclination": oe.inclination,
        "longitude": oe.longitude,
        "periapsis": oe.periapsis,
        "mean_anomaly": oe.mean_anomaly
    }}

def generate_configs(ts, start_dt, stop_dt, satellite, ue_position, gw_position=None, ephemeris_info_format="ecef"):
    # Timestamps
    time_resolution = timedelta(seconds=1)
    start_dt = start_dt.astimezone(timezone.utc).replace(tzinfo=None)
    stop_dt = stop_dt.astimezone(timezone.utc).replace(tzinfo=None)
    timestamps_dt = np.arange(start_dt, stop_dt, time_resolution)
    timestamps_dt = [dt.replace(tzinfo=utc) for dt in timestamps_dt.astype('datetime64[us]').astype(datetime).tolist()]   
    timestamps_ts = ts.from_datetimes(list(timestamps_dt))
    duration_dt = stop_dt - start_dt

    # Satellite position trace
    satellite_position = satellite.at(timestamps_ts)
    sat_position, sat_velocity = satellite_position.frame_xyz_and_velocity(itrs)

    # Satelite - UE location
    difference = satellite - ue_position
    topocentric = difference.at(timestamps_ts)
    altitude, azimuth, distance = topocentric.altaz()
    latitude, longitude, ue_slant_range, latitude_rate, longitude_rate, range_rate = topocentric.frame_latlon_and_rates(ue_position)
    max_ue_slant_range = np.max(ue_slant_range.km)

    # Satelite - Gateway location
    max_gw_slant_range = 0
    if (gw_position is not None):
        difference = satellite - gw_position
        topocentric = difference.at(timestamps_ts)
        altitude, azimuth, distance = topocentric.altaz()
        latitude, longitude, gw_slant_range, latitude_rate, longitude_rate, range_rate = topocentric.frame_latlon_and_rates(ue_position)
        max_gw_slant_range = np.max(gw_slant_range.km)

    ### Compute parameters for gNB NTN config.
    # epochTimestamp - reference epoch of the broadcast ephemeris (pass AOS).
    epoch_timestamp = start_dt

    # cellSpecificKoffset
    speed_of_light_km_per_s = 299792.458
    max_distance = max_ue_slant_range + max_gw_slant_range
    max_rtt = 2 * max_distance / speed_of_light_km_per_s
    max_propagation_delay_ms = int(np.ceil(max_rtt*1e3))
    cell_specific_koffset = max_propagation_delay_ms

    # kmac - TODO

    # ta-Info
    if (gw_position is not None):
        # only placeholders, gnb will fill it properly
        ta_common = 2.0 * max_gw_slant_range / speed_of_light_km_per_s * 1e6 # us
        ta_common_drift = 0.0
        ta_common_drift_variant = 0.0
    else:
        ta_common = 0.0
        ta_common_drift = 0.0
        ta_common_drift_variant = 0.0

    # ntn-PolarizationDL - TODO
    # ntn-PolarizationUL - TODO
    # ta-Report - TODO

    # ephemerisInfo - state of the serving satellite at the broadcast epoch (pass AOS).
    ephemeris = compute_ephemeris(ts, satellite, start_dt, start_dt, ephemeris_info_format)

    return {
        "cell_specific_koffset": cell_specific_koffset,
        "epoch_timestamp": epoch_timestamp.strftime('%Y-%m-%dT%H:%M:%S.%f')[:-3],
        "ephemeris": ephemeris,
    }


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='NTN Config Generator.')
    parser.add_argument("--tle", type=str, default="tle_example_leo.txt", help="TLE data file path.")
    parser.add_argument("--pass-start-time", type=str, default=None, help="Pass UTC start time, format 'Y-m-dTH:M:S' [e.g. 2025-03-14T09:16:00]")
    parser.add_argument("--pass-start-offset", type=int, default=0, help="Pass start offset [s].")
    parser.add_argument("--min-sat-elevation", type=int, default=20, help="Minimal satellite elevetion [degrees].")
    parser.add_argument("--feeder-link-enabled", type=str_to_bool, nargs='?', const=True, help="Whether to enable feeder link compensation.")
    parser.add_argument("--fl-dl-freq-hz", type=float, default=2185e6, help="DL Center Frequency [Hz] for the feeder link.")
    parser.add_argument("--fl-ul-freq-hz", type=float, default=1995e6, help="UL Center Frequency [Hz] for the feeder link.")
    parser.add_argument("--ephemeris-info-format", type=str, default="ecef", help="Format of the ephemeris info in NTN config [ecef, orbital].")
    parser.add_argument("--use-state-vector", type=str_to_bool, nargs='?', const=True, help="Optional parameter for the gnb ntn config. Whether the gnb broadcast EphemerisInfo as ECEF state vectors or ECI Orbital parameters.")
    parser.add_argument("--enable-sat-switch-with-resync", action="store_true", help="Include sat_switch_with_resync section with a phase-lagged target satellite (idx 1).")
    parser.add_argument("--ssb-time-offset-sf", type=int, default=0, help="SSB time offset [subframes] for the sat_switch_with_resync section.")
    parser.add_argument("--add-example-ncells", action="store_true", help="Add two example neighbor cells (ncells) to the cell NTN config.")
    parser.add_argument("--ta-report", action="store_true", help="Broadcast ta-Report in SIB19, so that UEs report their timing advance at random-access, establishment, resume and handover.")
    parser.add_argument("--ta-report-offset-threshold", type=float, default=None, help="Signal TAR-Config with this offsetThresholdTA [ms], so that UEs also report on timing advance variation. Allowed values are 0.5 and the integers 1 to 15.")
    parser.add_argument("--ta-report-sr-enabled", action="store_true", help="Let UEs request an uplink grant for a timing advance report. Requires --ta-report-offset-threshold.")
    parser.add_argument("--gnb-id", type=lambda x: int(x, 0), default=411, help="gNB ID of this CU-CP, used to build the internal serving nr_cell_id (decimal or 0x hex).")
    parser.add_argument("--gnb-id-bit-length", type=int, default=22, help="gNB ID bit length (NR Cell Identity is 36 bits).")
    cfg = parser.parse_args()

    if (cfg.ta_report_offset_threshold is not None and cfg.ta_report_offset_threshold != 0.5
            and cfg.ta_report_offset_threshold not in range(1, 16)):
        parser.error("--ta-report-offset-threshold must be 0.5 or an integer from 1 to 15.")
    if (cfg.ta_report_sr_enabled and cfg.ta_report_offset_threshold is None):
        parser.error("--ta-report-sr-enabled requires --ta-report-offset-threshold.")

    # Find real time NTN scenario.
    ts = load.timescale()
    satellite, cell_center, start_dt, stop_dt = find_real_time_ntn_scenario(ts, cfg.tle, cfg.min_sat_elevation, cfg.pass_start_offset, cfg.pass_start_time)

    ue_location = wgs84.latlon(cell_center.latitude.degrees, cell_center.longitude.degrees, 1)
    print("UE Position: ")
    print('--Latitude [deg]:', ue_location.latitude.degrees)
    print('--Longitude [deg]:', ue_location.longitude.degrees)
    print('--Elevation [m]:' , ue_location.elevation.m)
    print("")

    gw_location = None
    if (cfg.feeder_link_enabled):
        gw_location = wgs84.latlon(cell_center.latitude.degrees, cell_center.longitude.degrees, 1)
        print("Gateway Position: ")
        print('--Latitude [deg]:', gw_location.latitude.degrees)
        print('--Longitude [deg]:', gw_location.longitude.degrees)
        print('--Elevation [m]:' , gw_location.elevation.m)
        print("")

    # Generate NTN configs.
    serving = generate_configs(ts, start_dt, stop_dt, satellite, ue_location, gw_location, cfg.ephemeris_info_format)

    # Normalize pass timestamps to naive UTC (as used inside generate_configs).
    start_naive = start_dt.astimezone(timezone.utc).replace(tzinfo=None)
    stop_naive = stop_dt.astimezone(timezone.utc).replace(tzinfo=None)
    epoch_timestamp = serving["epoch_timestamp"]
    t_service = stop_naive.strftime('%Y-%m-%dT%H:%M:%S.%f')[:-3]

    gateway_location = None
    if (gw_location is not None):
        gateway_location = {
            "latitude": gw_location.latitude.degrees,
            "longitude": gw_location.longitude.degrees,
            "altitude": gw_location.elevation.m
        }

    # Global satellites list: serving satellite (idx 0).
    sat0 = {
        "satellite_idx": 0,
        "epoch_timestamp": epoch_timestamp,
    }
    sat0.update(serving["ephemeris"])
    if (gateway_location is not None):
        sat0["gateway_location"] = dict(gateway_location)
    satellites = [sat0]

    # Serving cell NTN config, referencing the serving satellite by index.
    cell_ntn = {
        "satellite_idx": 0,
        "cell_specific_koffset": serving["cell_specific_koffset"],
        "ntn_ul_sync_validity_dur": 5,
        "distance_threshold": 50000,
        "t_service": t_service,
        "ta_report": cfg.ta_report,
        "reference_location": {
            "latitude": round(ue_location.latitude.degrees, 6),
            "longitude": round(ue_location.longitude.degrees, 6),
        },
        "polarization": {
            "dl": "lhcp",
            "ul": "rhcp",
        },
    }
    if (cfg.ta_report_offset_threshold is not None):
        threshold = cfg.ta_report_offset_threshold
        cell_ntn["ta_report_offset_threshold"] = int(threshold) if threshold.is_integer() else threshold
        cell_ntn["ta_report_sr_enabled"] = cfg.ta_report_sr_enabled
    if (cfg.use_state_vector is not None):
        cell_ntn["use_state_vector"] = cfg.use_state_vector
    if (cfg.feeder_link_enabled):
        cell_ntn["feeder_link"] = {
            "enable_doppler_compensation": cfg.feeder_link_enabled,
            "dl_freq": cfg.fl_dl_freq_hz,
            "ul_freq": cfg.fl_ul_freq_hz
        }

    # Optionally add a satellite switch with resync: a phase-lagged target satellite (idx 1)
    # sharing the serving epoch but propagated back one pass duration, so that forward-
    # propagation puts it at the serving satellite's AOS geometry at the handover (LOS).
    if cfg.enable_sat_switch_with_resync:
        pass_duration = stop_naive - start_naive
        target_sample_dt = start_naive - pass_duration
        target_ephemeris = compute_ephemeris(ts, satellite, start_naive, target_sample_dt, cfg.ephemeris_info_format)

        sat1 = {
            "satellite_idx": 1,
            "epoch_timestamp": epoch_timestamp,
        }
        sat1.update(target_ephemeris)
        if (gateway_location is not None):
            sat1["gateway_location"] = dict(gateway_location)
        satellites.append(sat1)

        cell_ntn["sat_switch_with_resync"] = {
            "satellite_idx": 1,
            "t_service_start": t_service,
            "ssb_time_offset_sf": cfg.ssb_time_offset_sf,
            "promote_to_serving": True,
            "promote_neighbors": True,
        }

    # Optionally add example neighbor cells, each referencing a satellite from the global list.
    # The second ncell references the switch-target satellite (idx 1) when it exists.
    # carrier_freq values are SSB ARFCNs on frequencies different from the serving cell
    # (n256, ssb_arfcn 437090): ncell 0 stays in n256 (437310 -> 2186.55 MHz), ncell 1 is in
    # n254 (498030 -> 2490.15 MHz). They MUST match the corresponding CU-CP neighbor ssb_arfcn
    # (see ntn_cu.yml generation below).
    if cfg.add_example_ncells:
        cell_ntn["ncells"] = [
            {
                "pci": 42,
                "carrier_freq": 437310,
                "cell_specific_koffset": 30,
                "satellite_idx": 0,
            },
            {
                "pci": 43,
                "carrier_freq": 498030,
                "satellite_idx": 1 if cfg.enable_sat_switch_with_resync else 0,
            },
        ]

    # Global satellites list -> separate sat.yml (single source of truth, referenced by
    # satellite_idx from the DU/cell config and the CU-CP config). Load it alongside each
    # (e.g. -c sat.yml).
    sat_cfg = {"ntn": {"satellites": satellites}}

    # DU/cell config: references satellites from sat.yml by satellite_idx.
    ntn_cell_cfg = {
        "cell_cfg": {
            "ta": {
                "ta_target": 0,
                "ta_measurement_slot_prohibit_period": serving["cell_specific_koffset"] + 10,
                "ta_measurement_slot_period": 1000,
                "ta_cmd_offset_threshold": 1,
                "ta_outlier_detection_zscore_threshold": 0.0,
            },
            "ntn": cell_ntn,
        }
    }

    sat_cfg_fn = "sat.yml"
    save_gnb_ntn_config(sat_cfg_fn, sat_cfg)
    print("Saved satellites to file:  ", sat_cfg_fn)

    gnb_ntn_cfg = "ntn_du.yml"
    save_gnb_ntn_config(gnb_ntn_cfg, ntn_cell_cfg)
    print("Saved DU NTN config to file:", gnb_ntn_cfg)

    # Generate a CU-CP NTN config. The serving cell is internal to this CU-CP: its nr_cell_id
    # encodes this gNB id (top gnb_id_bit_length bits of the 36-bit NCI). The neighbor cells
    # are modeled as external cells (in other gNBs), so they carry the full radio parameters
    # the CU-CP needs; example values are used. Each neighbor references a satellite from the
    # global list (idx 1 exists only with sat switch) and its reference_location is offset from
    # the serving cell center so neighbors stay distinct.
    cell_lat = ue_location.latitude.degrees
    cell_lon = ue_location.longitude.degrees
    serving_nr_cell_id = make_nr_cell_id(cfg.gnb_id, cfg.gnb_id_bit_length, 0)

    serving_cell = {
        "nr_cell_id": serving_nr_cell_id,
        "periodic_report_cfg_id": 1,  # periodical report config for the serving cell
    }
    cells = [serving_cell]
    report_configs = [
        {
            "report_cfg_id": 1,
            "report_type": "periodical",
            "report_interval_ms": 1024,
        }
    ]

    if cfg.add_example_ncells:
        ncell_sat_idx = [0, 1 if cfg.enable_sat_switch_with_resync else 0]
        ncell_pci = [42, 43]
        # Neighbor radio config, on frequencies different from the serving cell (n256, ssb_arfcn
        # 437090). Must match the DU-side ncells carrier_freq above: ncell 0 in n256 (2186.55 MHz),
        # ncell 1 in n254 (2490.15 MHz).
        ncell_band = [256, 254]
        ncell_ssb_arfcn = [437310, 498030]
        # The serving cell's ncells list holds only the neighbor relation (nr_cell_id +
        # event-triggered report config). Each external cell is defined as its own top-level cells
        # entry carrying the radio parameters and the NTN config (satellite_idx, reference_location,
        # polarization).
        serving_cell["ncells"] = []
        for i, sat_idx in enumerate(ncell_sat_idx):
            ext_nr_cell_id = make_nr_cell_id(cfg.gnb_id + 1 + i, cfg.gnb_id_bit_length, 0)
            serving_cell["ncells"].append({
                "nr_cell_id": ext_nr_cell_id,
                "report_configs": [2],  # event-triggered (periodical is serving-cell only)
            })
            cells.append({
                "nr_cell_id": ext_nr_cell_id,
                "gnb_id_bit_length": cfg.gnb_id_bit_length,
                "pci": ncell_pci[i],
                "plmn": "00101",
                "tac": 7,
                "band": ncell_band[i],
                "ssb_arfcn": ncell_ssb_arfcn[i],
                "ssb_scs": 15,
                "ssb_period": 10,
                "ssb_offset": 0,
                "ssb_duration": 1,
                "ntn": {
                    "satellite_idx": sat_idx,
                    "reference_location": {
                        "latitude": round(cell_lat + 0.1 * (i + 1), 6),
                        "longitude": round(cell_lon + 0.1 * (i + 1), 6),
                    },
                    "polarization": {
                        "dl": "lhcp",
                        "ul": "rhcp",
                    },
                },
            })
        report_configs.append({
            "report_cfg_id": 2,
            "report_type": "event_triggered",
            "event_triggered_report_type": "a3",
            "meas_trigger_quantity": "rsrp",
            "meas_trigger_quantity_offset_db": 3,
            "hysteresis_db": 0,
            "time_to_trigger_ms": 100,
            "report_interval_ms": 1024,
        })

    mobility = {}
    # ntn_update_period_ms is only relevant when there are NTN neighbor cells.
    if cfg.add_example_ncells:
        mobility["ntn_update_period_ms"] = 1000
    mobility["cells"] = cells
    mobility["report_configs"] = report_configs

    cu_config = {
        "cu_cp": {
            "f1ap": {
                "ref_time_reporting": {
                    "enabled": True,
                    "event_type": "periodic",
                    "periodicity_rf": 128,
                },
            },
            "mobility": mobility,
        }
    }
    cu_ntn_cfg = "ntn_cu.yml"
    save_gnb_ntn_config(cu_ntn_cfg, cu_config)
    print("Saved CU NTN config to file:", cu_ntn_cfg)

    ue_position_cfg_fn = "ue-position.cfg"
    save_ground_position_cfg(ue_position_cfg_fn, ue_location)
    print("Saved UE position to file: ", ue_position_cfg_fn)

    if (gw_location is not None):
        gw_position_cfg_fn = "gw-position.cfg"
        save_ground_position_cfg(gw_position_cfg_fn, gw_location)
        print("Saved GW position to file: ", gw_position_cfg_fn)
