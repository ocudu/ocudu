# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


resource "gitlab_tag_protection" "protected_tags" {
  for_each = var.protected_tags

  project             = var.ci_project_id
  tag                 = each.key
  create_access_level = each.value.create_access_level
}
