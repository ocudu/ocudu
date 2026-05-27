# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# MR Approval Settings
resource "gitlab_project_level_mr_approvals" "this" {
  project                                        = var.ci_project_id
  disable_overriding_approvers_per_merge_request = var.mr_approvals.disable_overriding_approvers_per_merge_request
  merge_requests_author_approval                 = var.mr_approvals.merge_requests_author_approval
  merge_requests_disable_committers_approval     = var.mr_approvals.merge_requests_disable_committers_approval
  require_reauthentication_to_approve            = var.mr_approvals.require_reauthentication_to_approve
  reset_approvals_on_push                        = var.mr_approvals.reset_approvals_on_push
  selective_code_owner_removals                  = var.mr_approvals.selective_code_owner_removals
}

# MR Approval Rules
resource "gitlab_project_approval_rule" "this" {
  for_each = var.approval_rules

  project                           = var.ci_project_id
  name                              = title(replace(each.key, "_", " "))
  approvals_required                = each.value.approvals_required
  rule_type                         = each.value.rule_type
  applies_to_all_protected_branches = each.value.applies_to_all_protected_branches
}
