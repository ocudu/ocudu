# Configuration refactor

The configuration code of the DU and the scheduler changes step by step in a long refactor. This page tells you how to
write new code before the refactor is complete.

This page holds all the context that you need. Issue [#213](https://gitlab.com/ocudu/ocudu/-/work_items/213) holds the
history of the refactor, but you do not need to read it.

Apply this page to the scheduler configuration code, and to the DU code that builds it.

## Why

The scheduler holds the UE configuration in `serving_cell_config`, a copy of the RRC `ServingCellConfig`. RRC gives a
large set of options, because it is not specific to one implementation. OCUDU supports a small part of that set. The
result is:

- Long functions that fill fields that OCUDU never reads.
- Assertions in many files, because the type permits values that the scheduler cannot use.
- Long chains of `std::optional`.
- Large UE structures. The scheduler reads them for each UE and each slot.

The refactor puts each parameter at one level. Each level has a structure that holds only valid data.

## Rules for new code

### Do not add to `serving_cell_config`

`serving_cell_config` is a copy of the RRC structure. The refactor moves one area at a time out of it. The PUCCH is
moved. The DU now builds the PUCCH-Config from the RRM structures (see below).

- Do not read `serving_cell_config` in scheduler code.
- Do not add a field to `serving_cell_config` for a parameter that only the scheduler reads. Add the parameter to the
  RRM structures or to `ue_bwp_config`. A new RRC field in an area that is not moved yet is the exception (see below).
- For an area that is not moved yet, the DU still reads `serving_cell_config` to fill the ASN.1 `CellGroupConfig`
  (`calculate_cell_group_config_diff`). A change to that area stays in `serving_cell_config` until the area moves.

`ue_cell_config` holds `serv_cell_cfg`, `bwps` and `rrm` at the same time. This is temporary. One later change removes
`serv_cell_cfg` and moves `rrm` out of `ue_cell_config`. Do not remove the fields of `serving_cell_config` one at a
time.

### Give a UE all its dedicated configuration of a cell, or none

A UE has a full dedicated configuration in a cell, or no dedicated configuration. There is no partial state.

- If the RRM cannot allocate all the resources of a cell, the DU removes the cell from `cell_group.cells`. The scheduler
  gets an empty `sched_ue_cell_config::ded`. The UE uses only the common configuration of the cell, for example to get
  an RRC Reject.
- To show that the resources of a cell are not allocated or are released, remove the cell entry from `cell_group.cells`.
  `serving_cell_config` is a member of the cell entry, thus it is removed with the entry. Do not keep the entry and
  reset a field of `serving_cell_config`, for example `csi_meas_cfg`. The fields of `serving_cell_config` are RRC data,
  and the refactor removes that structure at the end.
- To find a UE without dedicated configuration, read `ue_cell_configuration::has_dedicated_cfg()`. Do not use a field of
  `serving_cell_config` as a flag for this state. For example, do not read `csi_meas_cfg.has_value()` to know if the UE
  has a dedicated configuration. A field of `serving_cell_config` can be empty for other reasons.

### Put each parameter at one level

| Level | Structures                                  | Content                                                    |
| ----- | ------------------------------------------- | ---------------------------------------------------------- |
| Input | `ran_cell_config`, `bwp_builder_params`     | The parameters that a user or a test sets.                 |
| RRM   | `cell_rrm_config`, `ue_rrm_config`          | What the RRM generates and allocates, for a cell or a UE.  |
| Cell  | `cell_configuration`, `cell_bwp_res_config` | The scheduler copy of the cell data and the cell RRM data. |
| UE    | `ue_bwp_config`                             | The scheduler copy of the UE RRM data.                     |

- Put a parameter at the input level if a user or a test sets it. The input level is a superset of the CLI parameters,
  thus a test-only parameter is permitted.
- Read the input level only when the cell is created. Put each value that the RRM derives from it in `cell_rrm_config`.
  Do not read `ran_cell_config` again for the same value.
- Calculate a value one time at the cell level. Do not calculate the same value again for each UE.
- Put a value in `ue_rrm_config` only if the value is different between the UEs of one cell.
- If a group of UEs shares a value, put the values in a list in `cell_rrm_config`. Then put the ID of the list element
  in `ue_rrm_config`.

### Put what the RRM generates in the RRM structures

The RRM structures hold two things: the data that the DU needs to fill the ASN.1 messages, and the resources that the
scheduler needs.

- The RRM fills `cell_rrm_config` one time for each cell. The DU keeps a copy in the cell repository, and passes it to
  the scheduler in the cell request.
- The RRM fills `ue_rrm_config` for each UE. The DU keeps it in the UE repository, and passes it to the scheduler in the
  UE request.
- The scheduler does not use the RRM structures in the slot path. It copies what it needs into its own structures
  (`cell_bwp_res_config`, `ue_bwp_config`). It chooses the layout of those structures for the cache.
- The DU fills the ASN.1 messages from the RRM structures. The conversion is in the DU. The RRM structures do not hold
  RRC data.
- Do not make an RRM structure with the same shape as an RRC structure.
- An RRM structure holds the values and IDs that the RRM allocates. The conversion to RRC form is the work of the DU.
- A type that the DU and the scheduler both use goes in `include/ocudu/scheduler/rrm`, for example
  `cell_pucch_resources`.

### Keep the UE structures small

The scheduler reads the UE structures for each UE and each slot. Small UE structures keep the cell data in the cache.

- If many UEs share the same data, put an ID or an index in the UE structure in place of a copy of that data. The ID is
  the index of an element in a list of the cell structure, for example the resource set configuration ID in
  `cell_pucch_resources`.
- Do not put a value in the UE structure if all the UEs of the cell have the same value.

### Do not copy the RRC structure

- Write a structure that holds only the configurations that OCUDU supports.
- Do not add a field because the RRC message has that field.
- If possible, make the type unable to represent an invalid state. Do not rely on an assertion to protect a field. For
  example, use an enum with the supported values, not an integer with an assertion on its range.
- If only some combinations of values are valid, try to put the combination in one type. For example, use a
  `std::variant` with one alternative for each valid combination. If the type needs many alternatives, it is probably
  not a good representation. Then use a simpler type and validate the values when you create the cell or the UE.

### Do not build a UE configuration to read a cell value

Some functions build a full UE configuration and then read one field from it. Do not write this type of code. Read the
value from `ran_cell_config` or from the cell structures.

### Write the RRM code in the scheduler

All the RRM code moves to the scheduler, because the RRM code depends on the scheduler implementation. The public
headers are in `include/ocudu/scheduler/rrm`. The implementation is in `lib/scheduler/rrm`.

- Put new RRM code in `include/ocudu/scheduler/rrm` and `lib/scheduler/rrm`.
- Do not add new RRM code to `lib/du/du_high/du_manager/ran_resource_management`.

### Keep the BWP parameters separate

OCUDU supports one BWP now. More than one BWP is necessary for RedCap.

- Keep the cell parameters and the BWP parameters in different structures.
- Index a new BWP structure with `bwp_id_t`.

### Tests

- Change `ran_cell_config` and `bwp_builder_params` in a test.
- Do not change a calculated structure in a test, because a manual change makes the configuration inconsistent.

The test helpers are not refactored yet. Do not use the current test code as an example.

## Example: PUCCH

The PUCCH structures show the correct split. They are not perfect, but use them as the example for new code.

### Structures

| Structure                       | Level | Used by             | Content                                       |
| ------------------------------- | ----- | ------------------- | --------------------------------------------- |
| `pucch_resource_builder_params` | Input | RRM (cell creation) | The number of resources, the format values.   |
| `cell_pucch_resources`          | RRM   | RRM, DU, scheduler  | The dedicated resources and the ID tables.    |
| `cell_pucch_rrm_config`         | RRM   | RRM, DU             | `cell_pucch_resources`, the common values.    |
| `ue_pucch_rrm_config`           | RRM   | RRM, DU, scheduler  | The IDs and the offsets of one UE.            |
| `cell_pucch_res_config`         | Cell  | Scheduler           | The common resources, `cell_pucch_resources`. |
| `ue_pucch_config`               | UE    | Scheduler           | The scheduler copy of `ue_pucch_rrm_config`.  |

- `pucch_resource_builder_params` is in `include/ocudu/scheduler/config/pucch_resource_builder_params.h`.
- `cell_pucch_resources` is in `include/ocudu/scheduler/rrm/cell_pucch_resources.h`. The ID tables go from the IDs that
  the RRM allocates (resource set configuration, SR, CSI) to the resources. It holds no RRC data.
- `cell_pucch_rrm_config` is in `include/ocudu/scheduler/rrm/cell_rrm_config.h`. It is the PUCCH member of
  `cell_rrm_config`, the structure that holds all the cell resources that the RRM allocates.
- `ue_pucch_rrm_config` is in `include/ocudu/scheduler/rrm/ue_rrm_config.h`. It holds only IDs and offsets. It holds no
  copy of any resource.
- `cell_pucch_res_config` is in `include/ocudu/scheduler/config/cell_bwp_res_config.h`. `ue_pucch_config` is in
  `ue_bwp_config.h`.
- `pucch_res_id_t` (`include/ocudu/ran/pucch/pucch_mapping.h`) holds a single index over the common and the dedicated
  PUCCH resources of the cell. The UE resource ID of the ASN.1 message is the position of the resource in the UE's PUCCH
  resource list.

### Why each level has an RRM structure and a scheduler structure

`cell_pucch_rrm_config` and `cell_pucch_res_config` both hold `cell_pucch_resources`. `ue_pucch_rrm_config` and
`ue_pucch_config` hold the same UE data. This is on purpose, for the cell and for the UE.

- The RRM structures are what the DU sees. The DU translates them to ASN.1. They hold only what the RRM allocates.
- The scheduler structures are what the scheduler reads for each slot. The scheduler also needs data that the RRM does
  not allocate, for example the common PUCCH resources.
- The two uses need different shapes. Thus the scheduler copies the RRM data into its own structures, and it can change
  their layout for the cache without a change to the DU.

Today the copies have the same content for the dedicated resources and the UE values. This is acceptable. Do not merge
them.

### Generation flow

1. The input: `ran_cell_config` holds `pucch_resource_builder_params`.
2. At the creation of a cell, the RRM calls `make_cell_rrm_config`. It calls `generate_cell_pucch_res_list` one time,
   and fills `cell_pucch_rrm_config`, with `cell_pucch_resources` inside it.
3. The DU keeps the `cell_rrm_config` in the cell repository. It passes a copy to the scheduler in the cell request.
4. The scheduler calls `make_cell_bwp_res_config`. This function adds the common resources and fills
   `cell_pucch_res_config`.
5. For each UE, the RRM allocates the IDs and the offsets, and fills `ue_pucch_rrm_config` (in `ue_rrm_config`). It
   reads `cell_pucch_resources` and the cell parameters (`cell_params`). It uses the cell parameters for the maximum
   PUCCH payload and for the CSI configuration.
6. The DU keeps `ue_rrm_config` in the UE repository. It passes it to the scheduler in the UE request. The scheduler
   copies it to `ue_pucch_config`.
7. The DU builds the ASN.1 `PUCCH-Config` of the UE from `cell_pucch_resources` and `ue_pucch_rrm_config`
   (`make_ue_pucch_res_layout` and `calculate_pucch_config_diff`). It does not read `serving_cell_config`.

## Target

- `serving_cell_config` does not exist. The DU fills the ASN.1 messages from `cell_rrm_config` and `ue_rrm_config`.
- `ran_cell_config` is an input to the creation of a cell. After that, the scheduler reads only its own cell and UE
  structures, and the DU reads only the RRM structures.
- `ue_cell_config` holds only the UE BWP configurations. `ue_rrm_config` is a separate structure. The DU keeps it, and
  passes it to the scheduler in the UE request.
- All the RRM code is in `include/ocudu/scheduler/rrm` and `lib/scheduler/rrm`. The RRM allocates the UE resources, and
  the DU asks the RRM for the data that RRC sends to the UE. The RRM code is in the scheduler directory, but the RRM is
  not the scheduler.
- `serving_cell_config_validator` does not exist, because the types do not permit an incorrect value.
  `scheduler_ue_config_validator` stays, because the request message still needs a check.

## Open points

Ask the user before you make one of these decisions:

- The future of `cell_config_builder_params` and `cell_config_builder_params_extended`.
- The correct place for the UE capability code (`ue_capability_manager`).
- The correct level for `tag_id`, `pdsch_serv_cell_cfg`, `pusch_serv_cell_cfg` and `csi_meas_cfg`. For `csi_meas_cfg`,
  the RRM structure must not hold an RRC-shaped `csi_meas_config`. The CSI report that the scheduler and the DU need
  must be a smaller structure.
- The place of `pucch_resource_generator.cpp`. It is in `lib/scheduler/config` because the PUCCH guardband code calls it
  to find the PRBs of the PUCCH. It moves to `lib/scheduler/rrm` when the guardband code does not need it.
- The rule that selects `lib/scheduler/config` or `include/ocudu/scheduler/config` for a new structure.
