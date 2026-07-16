# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


resource "gitlab_pipeline_schedule" "this" {
  for_each       = var.schedules
  project        = var.ci_project_id
  description    = each.value.description
  ref            = each.value.ref
  cron           = each.value.cron
  cron_timezone  = each.value.timezone
  active         = coalesce(try(each.value.active, null), true)
  take_ownership = true

  inputs = [
    for k, v in coalesce(each.value.inputs, {}) : {
      name  = k
      value = v
    }
  ]
}

resource "gitlab_pipeline_schedule_variable" "vars" {
  for_each = {
    for item in flatten([
      for sched_name, s in var.schedules : [
        for k, v in coalesce(s.variables, {}) : {
          key           = "${sched_name}:${k}"
          schedule_name = sched_name
          var_key       = k
          var_value     = v
        }
      ]
    ]) : item.key => item
  }

  project              = var.ci_project_id
  pipeline_schedule_id = tonumber(split(":", gitlab_pipeline_schedule.this[each.value.schedule_name].id)[1])
  key                  = each.value.var_key
  value                = each.value.var_value
}
