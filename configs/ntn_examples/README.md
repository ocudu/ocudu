# NTN Quickstart Guide

This document provides a minimal end-to-end workflow to run an NTN (Non-Terrestrial Network) scenario using the SRS gNB with an NTN configuration, and an Amarisoft UE with its built-in NTN channel emulator.

---

## Requirements

The script uses the following Python modules:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install numpy skyfield pyyaml
```

---

## 1. Start Open5gs core.

```bash
cd ./ocudu/docker
docker compose up 5gc
```

---

## 2. Generate NTN Scenario

First, generate the NTN configuration files from a TLE (Two-Line Element) file.

Example command:

```bash
python generate_ntn_configs.py --tle=./tle_example_leo.txt --feeder-link-enabled=true --ephemeris-info-format=orbital --use-state-vector=false
```

This will produce the required NTN configuration files (`sat.yml`, `ntn_du.yml`, `ntn_cu.yml`, `ue-position.cfg` and `gw-position.cfg`) based on the satellite orbit defined in the TLE file.

### Command-line options

| Option | Description |
| --- | --- |
| `--tle` | Path to the TLE file describing the satellite orbit. |
| `--pass-start-time` | UTC pass start time (`YYYY-MM-DDTHH:MM:SS`); defaults to the current time. |
| `--pass-start-offset` | Offset [seconds] added to the pass start time (default 0). |
| `--min-sat-elevation` | Minimum satellite elevation [degrees] used to find the pass (default 20). |
| `--feeder-link-enabled` | Enable feeder-link Doppler compensation (transparent payload). Adds `gateway_location` per satellite and `feeder_link` to the cell. |
| `--fl-dl-freq-hz` / `--fl-ul-freq-hz` | Feeder-link DL/UL centre frequencies [Hz]. |
| `--ephemeris-info-format` | `ecef` (state vector) or `orbital` (orbital elements). |
| `--use-state-vector` | Whether the gNB broadcasts ephemeris as ECEF state vectors. |
| `--enable-sat-switch-with-resync` | Add a second (target) satellite and a `sat_switch_with_resync` block for handover. |
| `--ssb-time-offset-sf` | SSB time offset [subframes] for the satellite switch (default 0). |
| `--add-example-ncells` | Add two example neighbor cells to the DU config (`ntn_du.yml`) and the CU-CP config (`ntn_cu.yml`). |
| `--ta-report` | Set `ta_report` in the cell NTN config, so that UEs report their timing advance at random access, establishment, resume and handover. |
| `--ta-report-offset-threshold` | Add `ta_report_offset_threshold` [ms] so that UEs also report on timing advance variation (`0.5` or an integer from 1 to 15). |
| `--ta-report-sr-enabled` | Set `ta_report_sr_enabled`, letting a triggered report raise an SR. Requires `--ta-report-offset-threshold`. |
| `--gnb-id` | gNB ID of this CU-CP, used to build the internal serving `nr_cell_id` in `ntn_cu.yml` (default `411`; accepts `0x` hex). |
| `--gnb-id-bit-length` | gNB ID bit length; the NR Cell Identity is 36 bits (default `22`). |

### Generated NTN configuration format

Satellites are defined once in `sat.yml` (a single source of truth, top-level `ntn.satellites` list) and referenced by `satellite_idx` from the DU config (`ntn_du.yml`) and the CU-CP config (`ntn_cu.yml`). Load `sat.yml` alongside the other configs (e.g. `-c sat.yml`).

`sat.yml`:

```yaml
ntn:
  satellites:                    # global list of satellites, referenced by satellite_idx
  - satellite_idx: 0             # serving satellite
    epoch_timestamp: '...'       # reference epoch of the broadcast ephemeris (pass AOS)
    ephemeris_info_ecef:         # or ephemeris_orbital, per --ephemeris-info-format
      pos_x: ...
      pos_y: ...
      pos_z: ...
      vel_x: ...
      vel_y: ...
      vel_z: ...
    gateway_location:            # present only with --feeder-link-enabled
      latitude: ...
      longitude: ...
      altitude: ...
  - satellite_idx: 1             # switch target; present only with --enable-sat-switch-with-resync
    ...
```

`ntn_du.yml`:

```yaml
cell_cfg:
  ta:                            # timing-advance command parameters
    ta_target: 0
    ta_measurement_slot_prohibit_period: ...
    ta_measurement_slot_period: 1000
    ta_cmd_offset_threshold: 1
    ta_outlier_detection_zscore_threshold: 0.0
  ntn:
    satellite_idx: 0             # references the serving satellite above
    cell_specific_koffset: ...   # derived from the maximum propagation delay
    ntn_ul_sync_validity_dur: 5
    distance_threshold: 50000    # location-based measurement distance [meters]
    t_service: '...'             # time the serving cell stops serving (pass LOS)
    ta_report: false             # enable timing-advance reporting in SIB19 (random access and handover)
    ta_report_offset_threshold: 1 # optional; ms of T_TA drift that triggers a UE Timing Advance Report.
                                 # The report resolves whole 15kHz slots (1ms), so values below 1 only add
                                 # duplicate reports carrying a value the gNB already has.
    ta_report_sr_enabled: false  # optional; let a triggered report raise an SR when no UL grant is available.
                                 # Requires the UE to support sr-TriggeredBy-TA-Report-r17.
    reference_location:          # serving cell reference location (= cell center)
      latitude: ...
      longitude: ...
    polarization:                # service-link DL/UL polarization
      dl: lhcp
      ul: rhcp
    use_state_vector: false      # present only if --use-state-vector is given
    feeder_link:                 # present only with --feeder-link-enabled
      enable_doppler_compensation: true
      dl_freq: ...
      ul_freq: ...
    sat_switch_with_resync:      # present only with --enable-sat-switch-with-resync
      satellite_idx: 1           # references the switch-target satellite
      t_service_start: '...'     # time the target satellite starts serving (= t_service)
      ssb_time_offset_sf: 0
      promote_to_serving: true   # promote the target satellite to serving on switch
      promote_neighbors: true    # promote neighbor cells on switch
    ncells:                      # present only with --add-example-ncells
    - pci: 42                    # neighbor cell, references a satellite from the global list
      carrier_freq: 437310       # SSB ARFCN (n256, 2186.55 MHz); matches CU ssb_arfcn
      cell_specific_koffset: 30  # optional per-neighbor koffset
      satellite_idx: 0
    - pci: 43
      carrier_freq: 498030       # SSB ARFCN (n254, 2490.15 MHz); matches CU ssb_arfcn
      satellite_idx: 1           # references the switch target when it exists, otherwise 0
```

The neighbor `carrier_freq` values are SSB ARFCNs on frequencies different from the serving cell (n256 @ `ssb_arfcn 437090`, 2185 MHz): one neighbor stays in n256, the other is in n254. Each equals the corresponding CU-CP neighbor `ssb_arfcn` (same physical cell), and the `pci` values match too.

When `--enable-sat-switch-with-resync` is set, a second satellite (`satellite_idx: 1`) is added: the same orbit propagated so that it reaches the serving satellite's AOS geometry at the serving satellite's LOS — i.e. it enters the cell exactly as the serving satellite leaves.

### CU-CP configuration format

The CU-CP config `ntn_cu.yml` defines one **serving cell** (internal to this CU-CP). With `--add-example-ncells` it also gets two example **external neighbor cells** (in other gNBs); without the flag the serving cell has no `ncells`. The serving cell's `nr_cell_id` encodes this gNB's id — the top `--gnb-id-bit-length` bits of the 36-bit NR Cell Identity hold `--gnb-id` (default `0x19b`, giving `0x66c000`).

The neighbors use different gNB ids, so they are external. The CU-CP config layout for this (matching the parser) is:

- The **serving cell** entry carries `periodic_report_cfg_id` and lists each neighbor under `ncells` by `nr_cell_id` with an **event-triggered** `report_configs` id (periodical reports are only allowed for the serving cell).
- Each **external cell** is defined as its own top-level `cells` entry carrying the full radio parameters (`gnb_id_bit_length`, `pci`, `plmn`, `tac`, `band`, `ssb_*`) plus the `ntn` block: `satellite_idx` (from `sat.yml`), a `reference_location` (derived from the serving cell center with a per-neighbor offset), and `polarization` (`dl`/`ul`). Radio params or the `ntn` block nested inside `ncells` are rejected by the parser.

Load `sat.yml` alongside it so the `satellite_idx` references resolve.

```yaml
cu_cp:
  f1ap:
    ref_time_reporting:
      enabled: true             # required when any NTN ncell is configured
      event_type: periodic      # must be "periodic" (not "on_demand")
      periodicity_rf: 128        # [1..512]
  mobility:
    ntn_update_period_ms: 1000   # only emitted when NTN neighbors are present
    cells:
    - nr_cell_id: 0x66c000       # serving cell, internal (encodes gnb_id 0x19b, 22-bit)
      periodic_report_cfg_id: 1  # periodical report config for the serving cell
      ncells:
      - nr_cell_id: 0x670000     # reference to the external neighbour cell below
        report_configs: [2]      # event-triggered (periodical is serving-cell only)
      - nr_cell_id: 0x674000
        report_configs: [2]
    - nr_cell_id: 0x670000       # external neighbour cell definition (different gnb_id)
      gnb_id_bit_length: 22
      pci: 42
      plmn: '00101'
      tac: 7
      band: 256                  # n256, matches DU carrier_freq 437310
      ssb_arfcn: 437310
      ssb_scs: 15
      ssb_period: 10
      ssb_offset: 0
      ssb_duration: 1
      ntn:
        satellite_idx: 0         # resolved from sat.yml's ntn.satellites list
        reference_location:      # 2-D (lat/lon only), offset from the serving cell
          latitude: ...
          longitude: ...
        polarization:
          dl: lhcp
          ul: rhcp
    - nr_cell_id: 0x674000       # external neighbour cell (band 254, ssb_arfcn 498030)
      gnb_id_bit_length: 22
      pci: 43
      # ... same external radio params ...
      ntn:
        satellite_idx: 1         # switch target when sat switch is enabled, otherwise 0
        reference_location:
          latitude: ...
          longitude: ...
        polarization:
          dl: lhcp
          ul: rhcp
    report_configs:
    - report_cfg_id: 1           # periodical: serving cell only
      report_type: periodical
      report_interval_ms: 1024
    - report_cfg_id: 2           # event-triggered: neighbor cells
      report_type: event_triggered
      event_triggered_report_type: a3
      meas_trigger_quantity: rsrp
      meas_trigger_quantity_offset_db: 3
      hysteresis_db: 0
      time_to_trigger_ms: 100
      report_interval_ms: 1024
```

---

### Broadcasting several TACs in one cell

TS 38.331 lets an NTN cell broadcast up to 12 TACs per PLMN in `trackingAreaList`. Stack
`multi_tac.yml` on top of `gnb.yml`:

```bash
sudo $GNB_PATH -c ./gnb.yml -c sat.yml -c ntn_du.yml -c ntn_cu.yml -c zmq.yml -c multi_tac.yml
```

```yaml
cell_cfg:
  tac: 7
  additional_tacs: [8, 9]           # cell advertises 7, 8 and 9

cu_cp:
  amf:
    supported_tracking_areas:       # one entry per broadcast TAC, all known to the core
      - tac: 7
        plmn_list: [{ plmn: "00101", tai_slice_support_list: [{ sst: 1 }] }]
      - tac: 8
        plmn_list: [{ plmn: "00101", tai_slice_support_list: [{ sst: 1 }] }]
      - tac: 9
        plmn_list: [{ plmn: "00101", tai_slice_support_list: [{ sst: 1 }] }]
```

A later `-c` file replaces a list instead of appending to it, which is why `tac: 7` is repeated.

---

## 3. Start the SRS gNB

Launch the gNB using the generated NTN configuration together with the standard configuration files:

```bash
export GNB_PATH=/absolute/path/to/binary/srsgnb/build/apps/gnb/gnb  # absolute path to srsgnb binary
sudo $GNB_PATH -c ./gnb.yml -c sat.yml -c ntn_du.yml -c ntn_cu.yml -c zmq.yml
```

---

## 4. Start Amarisoft UE with Built-in NTN Channel Emulator

```bash
export UE_PATH=/absolute/path/to/binary/amarisoft/2025-11-24/lteue-linux-2025-11-24/lteue  # absolute path to amarisoft UE binary
sudo $UE_PATH ./ue-nr-ntn-emu.cfg
```
