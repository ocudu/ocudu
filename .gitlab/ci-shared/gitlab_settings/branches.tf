# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


resource "gitlab_branch_protection" "protected_branches" {
  for_each = var.protected_branches

  project                      = var.ci_project_id
  branch                       = each.key
  allow_force_push             = each.value.allow_force_push
  code_owner_approval_required = each.value.code_owner_approval_required
  allowed_to_merge             = each.value.allowed_to_merge
  allowed_to_push              = each.value.allowed_to_push
}
