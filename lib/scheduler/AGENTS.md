# Configuration refactor

The configuration code of the DU and the scheduler changes step by step in a long refactor. This page tells you how to
write new code before the refactor is complete. Issue: <https://gitlab.com/ocudu/ocudu/-/work_items/213>.

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

### Use `serving_cell_config` only for RRC

`serving_cell_config` becomes an RRC-only structure. `calculate_cell_group_config_diff` reads it to fill the ASN.1
`CellGroupConfig`. It is the only path to the RRC message now. Thus a new RRC field still goes in it.

- Do not read `serving_cell_config` in scheduler code. Read `ran_cell_config`, the cell structures or `ue_bwp_config`.
- Add a new UE parameter of the scheduler to `ue_bwp_config`.
- If the UE also needs that parameter over RRC, write the same value to `serving_cell_config` in the DU. Write the two
  copies in the same function. Then the two values cannot become different. The copies are temporary.
- Do not add a field to `serving_cell_config` if only the scheduler reads the field.

`pucch_resource_manager::alloc_resources` shows the pattern. It writes the SR offset to `ue_pucch_config` for the
scheduler, and to `serv_cell_cfg` for RRC.

One exception stays. Some code reads `pucch_cfg.has_value()` to find a UE configuration that is not complete, for
example `uci_scheduler_impl` and `is_cfg_dedicated_complete`. Do not change these tests to a test of `ue_cfg()`, because
the two structures do not agree. See Open points.

`ue_cell_config` holds `serv_cell_cfg` and `bwps` at the same time. This is temporary. One later change removes
`serv_cell_cfg`. Do not remove the fields of `serving_cell_config` one at a time.

### Put each parameter at one level

| Level | Structures                                  | Content                                                    |
| ----- | ------------------------------------------- | ---------------------------------------------------------- |
| Input | `ran_cell_config`, `bwp_builder_params`     | The parameters that a user or a test sets.                 |
| Cell  | `cell_configuration`, `cell_bwp_res_config` | The values that the scheduler calculates from the input.   |
| UE    | `ue_bwp_config`                             | The values that are different between the UEs of one cell. |

- Put a parameter at the input level if a user or a test sets it. The input level is a superset of the CLI parameters,
  thus a test-only parameter is permitted.
- Calculate a value one time at the cell level. Do not calculate the same value again for each UE.
- Put a value in `ue_bwp_config` only if the value is different between the UEs of one cell.
- If a group of UEs shares a value, put the values in a list at the cell level. Then put the ID of the list element in
  the UE structure.

### Keep the UE structures small

The scheduler reads the UE structures for each UE and each slot. Small UE structures keep the cell data in the cache.

- Put an ID or an index in the UE structure. Do not put a copy of the data.
- Do not put a value in the UE structure if all the UEs of the cell have the same value.

### Do not copy the RRC structure

- Write a structure that holds only the configurations that OCUDU supports.
- Do not add a field because the RRC message has that field.
- Do not add an assertion to protect a field. Change the type. Then an incorrect value is not possible.

### Do not build a UE configuration to read a cell value

Some functions build a full UE configuration and then read one field from it. Do not write this type of code. Read the
value from `ran_cell_config` or from the cell structures.

### Write the RRM code in the scheduler

All the RRM code moves to `lib/scheduler/rrm`, because the RRM code depends on the scheduler implementation.

- Put new RRM code in `lib/scheduler/rrm`.
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
- `cell_pucch_res_config` (`include/ocudu/scheduler/config/cell_bwp_res_config.h`) holds the lists of the common and the
  dedicated PUCCH resources of the cell.
- `ue_pucch_config` (`include/ocudu/scheduler/config/ue_bwp_config.h`) holds only IDs and offsets. It holds no copy of a
  resource.
- `pucch_res_id_t` (`include/ocudu/ran/pucch/pucch_mapping.h`) holds two IDs. The scheduler uses `cell_res_id`. The DU
  uses `ue_res_id` only for the ASN.1 message.

## Target

- `serving_cell_config` does not exist. The DU fills the ASN.1 messages from `ran_cell_config` and `ue_bwp_config`.
- `ue_cell_config` holds only the UE BWP configurations.
- All the RRM code is in `lib/scheduler/rrm`. The scheduler allocates the UE resources. The DU asks the scheduler for
  the data that RRC sends to the UE.
- `serving_cell_config_validator` does not exist, because the types do not permit an incorrect value.
  `scheduler_ue_config_validator` stays, because the request message still needs a check.

## Open points

Ask the user before you make one of these decisions:

- The way to show that a UE configuration is not complete. The UE sends no capabilities in fallback mode. Thus the RRM
  cannot allocate all the resources yet. `is_cfg_dedicated_complete` reads `pucch_cfg.has_value()` now, and the DU
  removes that field to show the incomplete state. `ue_bwp_config` keeps its values at the same time, thus the two
  structures do not agree.
- The future of `cell_config_builder_params` and `cell_config_builder_params_extended`.
- The correct place for the UE capability code (`ue_capability_manager`).
- The correct level for `tag_id`, `pdsch_serv_cell_cfg`, `pusch_serv_cell_cfg` and `csi_meas_cfg`.
- The rule that selects `lib/scheduler/config` or `include/ocudu/scheduler/config` for a new structure.
