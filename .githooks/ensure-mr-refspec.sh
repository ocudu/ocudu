#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

# Ensures the local clone fetches GitLab's per-MR head refs, so MR top
# commits are reachable locally as refs/remotes/mr/<iid> for `git log`
# and `git bisect`.

set -e

REMOTE="origin"
REFSPEC="+refs/merge-requests/*/head:refs/remotes/mr/*"

git config --get remote.${REMOTE}.url >/dev/null 2>&1 || exit 0

if ! git config --get-all remote.${REMOTE}.fetch 2>/dev/null | grep -qxF "$REFSPEC"; then
    git config --add remote.${REMOTE}.fetch "$REFSPEC"
    echo "githooks: added '${REMOTE}' fetch refspec for MR head refs (refs/remotes/mr/*)"
fi
