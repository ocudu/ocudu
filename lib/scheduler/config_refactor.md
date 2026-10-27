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
- Do not add a field to `serving_cell_config`. Add a new parameter to the RRM structures or to `ue_bwp_config`.
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
- To show that the resources of a cell are not allocated or are released, remove the cell entry. Do not reset a field of
  `serving_cell_config` to show this state. The fields of `serving_cell_config` are RRC data, and the refactor removes
  that structure. For example, do not reset `csi_meas_cfg`.
- To find a UE without dedicated configuration, read `ue_cell_configuration::has_dedicated_cfg()`. Do not read the
  presence of a `serving_cell_config` field, for example `csi_meas_cfg.has_value()`.

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
- Do not make an RRM structure that has the same shape as an RRC structure. Hold the values and IDs that the RRM
  allocates. The conversion to RRC form is the work of the DU.
- A type that the DU and the scheduler both use goes in `include/ocudu/scheduler/rrm`, for example
  `cell_pucch_resources`.

### Keep the UE structures small

The scheduler reads the UE structures for each UE and each slot. Small UE structures keep the cell data in the cache.

- If many UEs share the same data, put an ID or an index in the UE structure in place of a copy of that data.
- Do not put a value in the UE structure if all the UEs of the cell have the same value.

### Do not copy the RRC structure

- Write a structure that holds only the configurations that OCUDU supports.
- Do not add a field because the RRC message has that field.
- Do not add an assertion to protect a field. Change the type. Then an incorrect value is not possible.

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

### Tell the user

If the user asks for a change that breaks a rule on this page, do the work. Then tell the user which rule the change
breaks.

## Example: PUCCH

The PUCCH structures show the correct split. They are not perfect, but use them as the example for new code.

- `pucch_resource_builder_params` (`include/ocudu/scheduler/config/pucch_resource_builder_params.h`) holds the input
  parameters, for example the number of resources and the format parameters.
- `cell_pucch_resources` (`include/ocudu/scheduler/rrm/cell_pucch_resources.h`) holds the list of the dedicated PUCCH
  resources of the cell. It holds the tables from the IDs that the RRM allocates (resource set configuration, SR, CSI)
  to the resources. `generate_cell_pucch_res_list` fills it one time for each cell.
- `cell_pucch_rrm_config` (`include/ocudu/scheduler/rrm/cell_rrm_config.h`) holds `cell_pucch_resources` and the PUCCH
  values that are common to the UEs of the cell.
- `ue_pucch_rrm_config` (`include/ocudu/scheduler/rrm/ue_rrm_config.h`) holds only IDs and offsets. It holds no copy of
  a resource.
- `cell_pucch_res_config` (`include/ocudu/scheduler/config/cell_bwp_res_config.h`) holds the common PUCCH resources and
  a copy of `cell_pucch_resources`. The scheduler reads them. `ue_pucch_config` (`ue_bwp_config.h`) is the scheduler
  copy of `ue_pucch_rrm_config`.
- The DU builds the PUCCH-Config of a UE from `cell_pucch_resources` and `ue_pucch_rrm_config`
  (`make_ue_pucch_res_layout` and `calculate_pucch_config_diff`). It does not read `serving_cell_config`.
- `pucch_res_id_t` (`include/ocudu/ran/pucch/pucch_mapping.h`) holds a single index over the common and the dedicated
  PUCCH resources of the cell. The UE resource ID of the ASN.1 message is the position of the resource in the UE's PUCCH
  resource list.

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
