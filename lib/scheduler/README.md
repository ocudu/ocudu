# MAC Scheduler

The MAC scheduler allocates radio resources — time, frequency, and control channels — across all active cells in a DU. It runs once per slot per cell and produces a `sched_result` that the MAC layer uses to generate DL assignments and UL grants.

## Component Hierarchy

```
scheduler_impl
├── sched_config_manager          # Validates and distributes cell/UE config
├── cell_scheduler  [per cell]    # All resources specific to one cell
│   ├── cell_metrics_handler      # Per-slot metric aggregation for this cell
│   ├── cell_resource_allocator   # Circular-buffer resource grid (16 slots)
│   ├── ssb_scheduler             # SSB bursts
│   ├── pdcch_resource_allocator  # PDCCH/DCI allocation
│   ├── si_scheduler              # SIB1 and SI messages
│   ├── csi_rs_scheduler          # CSI-RS
│   ├── ra_scheduler              # Random access response (RAR)
│   ├── prach_scheduler           # PRACH occasions
│   ├── pucch_allocator           # PUCCH resources (HARQ-ACK, SR, CSI)
│   ├── uci_allocator             # UCI scheduling (PUCCH or PUSCH mux)
│   ├── srs_allocator             # Sounding Reference Signal scheduling
│   ├── paging_scheduler          # Paging messages
│   └── ue_cell_scheduler         # UE grants for this cell (via ue_scheduler)
└── ue_scheduler  [per cell group]  # Shared UE state across a CA group
    ├── ue_repository               # All UE contexts (ue, ue_cell objects)
    ├── ue_event_manager            # Deferred UE config/removal events
    ├── uci_scheduler               # SR and CSI scheduling
    ├── srs_scheduler               # Periodic SRS management
    ├── ue_fallback_scheduler       # SRB0 / contention-resolution grants
    ├── inter_slice_scheduler       # Prioritizes slices for each slot
    └── intra_slice_scheduler       # PDSCH/PUSCH allocation within a slice
        ├── grant_params_selector   # MCS, PRBs, HARQ selection
        └── ue_cell_grid_allocator  # Writes grants into the resource grid
```

A `ue_scheduler` is shared across all cells in a cell group (Carrier Aggregation). Each `cell_scheduler` holds a `ue_cell_scheduler` handle, which is an RAII view into the shared `ue_scheduler`.

## Slot Processing Flow

`scheduler_impl::slot_indication(sl_tx, cell_index)` is the entry point. It calls `cell_scheduler::run_slot()`, which executes in this order:

1. **Reset resource grid** — clear stale allocations for this slot.
2. **SSB** — schedule Synchronization Signal Blocks.
3. **CSI-RS** — schedule CSI-RS if due.
4. **SI** — schedule SIB1 and SI-message windows.
5. **PRACH** — schedule PRACH occasions.
6. **RA** — schedule random access (RA) grants (e.g. RAR and Msg3) for detected RACH preambles.
7. **Paging** — schedule paging PDCCH and PDSCH.
8. **UE scheduling** (`ue_cell_scheduler::run_slot`):
   1. Process pending UE events, such as CRC and UCI indications or UE reconfigurations (`ue_event_manager`).
   2. Advance UE state machines (DRX, timing advance, HARQ timers).
   3. Schedule SR and CSI PUCCH opportunities (`uci_scheduler`).
   4. Schedule periodic and aperiodic SRS (`srs_scheduler`).
   5. Schedule SRB0 / fallback grants (`ue_fallback_scheduler`).
   6. Prioritize slices for this slot (`inter_slice_scheduler::slot_indication`).
   7. For each slice in priority order:
      - Schedule PDSCH retransmissions, then new transmissions.
      - Schedule PUSCH retransmissions, then new transmissions.
   8. Post-process allocations (finalize PUCCH/UCI state).

## Resource Grid

`cell_resource_allocator` is a circular ring buffer over `cell_slot_resource_allocator` entries — one per slot. Each entry contains:

- `sched_result` — the scheduling decisions (PDCCHs, PDSCHs, PUSCHs, etc.)
- A DL and UL `carrier_subslot_resource_grid` per SCS — a Symbol × CRB bitmap used for collision detection and availability checks.

Allocators receive a `cell_slot_resource_allocator&` and write into it directly. The ring buffer provides a lookahead window so allocators can inspect and reserve resources in future slots (e.g., PUSCH scheduled K2 slots ahead of the PDCCH slot).

## RAN Slicing

The scheduler supports multiple RAN slices, each configured with a `slice_rrm_policy_config` (min/max RB limits, S-NSSAI). Two scheduling layers handle slicing:

**Inter-slice scheduler** (`slicing/inter_slice_scheduler.h`) — runs once per slot and produces a priority-ordered queue of `ran_slice_candidate` objects for DL and UL. Priority is computed from slice SLA parameters, recent utilization, and RB limits.

**Intra-slice scheduler** (`ue_scheduling/intra_slice_scheduler.h`) — consumes one slice candidate at a time and allocates PDSCH/PUSCH grants for UEs in that slice. Within a slice, UE ordering is determined by the slice's `scheduler_policy`.

### Scheduling Policies

`scheduler_policy` (`policy/scheduler_policy.h`) is the pluggable per-slice algorithm interface:

- `compute_ue_dl_priorities()` / `compute_ue_ul_priorities()` — assign a `ue_sched_priority` to each UE candidate.
- `save_dl_newtx_grants()` / `save_ul_newtx_grants()` — called after allocation to update internal state.
- `add_ue()` / `rem_ue()` — track UE membership.

Available implementations (in `policy/`) include Round-Robin with QoS weighting and time-domain fair scheduling.

## UE Context

UE state is split into two levels:

**`ue`** (in `ue_context/`) — per-UE, cell-group-wide state:
- Logical channel repository and DL/UL buffer occupancy.
- DRX controller and timing advance manager (shared across cells).
- Link to all serving `ue_cell` objects.

**`ue_cell`** (in `ue_context/`) — per-UE, per-cell state:
- Active BWP configuration and HARQ entities.
- Channel state manager (CQI, RI, PMI from UCI indications).
- Link adaptation controller (translates CQI/SINR to MCS).
- Fallback mode flag (set on repeated HARQ failures; cleared by MAC).

All UEs live in a `ue_repository`.

## Configuration Management

`sched_config_manager` (`config/sched_config_manager.h`) is the single point of entry for all configuration:

- **Cell config** — validates `sched_cell_configuration_request_message` and builds a `cell_configuration` object used throughout the scheduler.
- **UE config** — validates `sched_ue_creation_request_message` / `sched_ue_reconfiguration_message`, builds a thread-safe config snapshot, and emits a `ue_config_update_event` that `ue_event_manager` processes on the next slot boundary.

Configuration changes are never applied mid-slot; they are queued as events and applied at the start of the next `run_slot()` call.

## Directory Layout

```
lib/scheduler/
├── scheduler_impl.{h,cpp}      # mac_scheduler implementation
├── scheduler_factory.cpp       # create_scheduler() factory
├── cell_scheduler.{h,cpp}      # Per-cell orchestrator
├── cell/                       # Resource grid, HARQ entities, PRB tracking
├── common_scheduling/          # SSB, SI, CSI-RS, RA, PRACH, paging schedulers
├── config/                     # Cell/UE configuration management and validation
├── logging/                    # Event logger, result logger, metrics handler
├── pdcch_scheduling/           # PDCCH/DCI allocation
├── policy/                     # Scheduling policy interface and implementations
├── pucch_scheduling/           # PUCCH resource allocation
├── slicing/                    # RAN slice instances and inter-slice scheduler
├── srs/                        # SRS resource allocation
├── support/                    # MCS tables, BWP helpers, DMRS, power control
├── uci_scheduling/             # UCI allocation (PUCCH and PUSCH mux)
├── ue_context/                 # UE and UE-cell state objects
└── ue_scheduling/              # UE grant scheduler, fallback, intra-slice allocator
```
