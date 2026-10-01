#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

# Manual utility (not a hook): wipes every refs/remotes/mr/<iid> ref
# (however it got created), the cached fixup-rebased-mrs.sh state, and the
# MR fetch refspec, then rebuilds everything from scratch. Useful if the
# local state ever looks wrong and you just want a clean slate.

set -e

REPO_ROOT="$(git rev-parse --show-toplevel)"
cd "$REPO_ROOT"

echo "Deleting all refs/remotes/mr/* refs..."
git for-each-ref --format='delete %(refname)' refs/remotes/mr | git update-ref --stdin

echo "Removing cached fixup-rebased-mrs.sh state..."
rm -rf "$(git rev-parse --git-dir)/mr-tags"

echo "Removing the MR fetch refspec..."
git config --unset-all remote.origin.fetch merge-requests 2>/dev/null || true

echo "Re-adding the fetch refspec and re-fetching..."
"$REPO_ROOT/.githooks/ensure-mr-refspec.sh"
git fetch origin

echo "Backfilling MR fixups from scratch (this can take ~20-30s)..."
"$REPO_ROOT/.githooks/fixup-rebased-mrs.sh"

echo "Done."
