#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

# GitLab rebases an MR's source branch onto the target branch before a
# fast-forward merge whenever the target has moved on. That rewrites the
# commits (new committer/date -> new SHA), so refs/remotes/mr/<iid> (the
# pre-rebase source branch head, fetched by ensure-mr-refspec.sh) is left
# pointing at a commit that never actually lands anywhere, and `git log`
# won't decorate the commit that really landed.
#
# This script finds those landed-but-rebased commits by matching patch-id
# (stable across the rebase) and repoints refs/remotes/mr/<iid> at the real
# landed commit, so it decorates exactly like any other MR in `git log`.
#
# The fetch refspec force-updates (+) refs/remotes/mr/*, so a plain
# `git fetch` (with no hook trigger) can silently reset an already-fixed
# ref back to the stale value. To stay correct we keep a permanent record
# of known-good mappings and re-apply them whenever anything under
# refs/remotes/mr has changed since the last run (checked cheaply via a
# snapshot compare) - a checkout/merge with no such change does no work.

set -e

REPO_ROOT="$(git rev-parse --show-toplevel)"
cd "$REPO_ROOT"

TARGET="origin/HEAD"
git rev-parse --verify "$TARGET" >/dev/null 2>&1 || exit 0
TARGET_SHA="$(git rev-parse "$TARGET")"

STATE_DIR="$(git rev-parse --git-dir)/mr-tags"
mkdir -p "$STATE_DIR"
SNAPSHOT="$STATE_DIR/snapshot"           # full refs/remotes/mr state + target sha, last time we ran
LAST_SCANNED="$STATE_DIR/last-scanned"   # last target sha checked for newly landed commits
VERBATIM="$STATE_DIR/verbatim"           # iids that land verbatim (no fix ever needed)
MAPPED="$STATE_DIR/mapped"               # "<iid> <landed_sha>" permanent known-good overrides
CANDIDATES="$STATE_DIR/candidate-cache"  # "<iid> <sha> <patchid>" for still-unresolved refs
BACKFILL_WINDOW=5000

touch "$SNAPSHOT" "$LAST_SCANNED" "$VERBATIM" "$MAPPED" "$CANDIDATES"

mr_state() {
    git for-each-ref --format='%(objectname) %(refname)' refs/remotes/mr
    echo "TARGET $TARGET_SHA"
}

# Nothing under refs/remotes/mr (nor the target branch) has changed since
# our last run: no override could have been clobbered and nothing new
# could have landed, so there is nothing to check.
if [ "$(mr_state)" = "$(cat "$SNAPSHOT")" ]; then
    exit 0
fi

patch_id_of() {
    git show "$1" | git patch-id --stable | cut -d' ' -f1
}

# Re-apply known-good overrides in case a bare `git fetch` reset them.
while read -r iid landed_sha; do
    [ -n "$iid" ] || continue
    current="$(git rev-parse "refs/remotes/mr/$iid" 2>/dev/null || true)"
    [ "$current" != "$landed_sha" ] && git update-ref "refs/remotes/mr/$iid" "$landed_sha"
done < "$MAPPED"

if [ -s "$LAST_SCANNED" ]; then
    range="$(cat "$LAST_SCANNED")..${TARGET_SHA}"
else
    range="${TARGET_SHA}~${BACKFILL_WINDOW}..${TARGET_SHA}"
    echo "githooks: first run - backfilling MR fixups over the last ${BACKFILL_WINDOW} commits on ${TARGET} (older history won't be auto-fixed)"
fi

declare -A landed_pid_to_sha
while read -r sha; do
    pid="$(patch_id_of "$sha")"
    # Merge commits (e.g. "merge dev into feature branch") have no single
    # diff, so patch-id yields nothing - they can't be matched, skip them.
    [ -n "$pid" ] && landed_pid_to_sha["$pid"]="$sha"
done < <(git rev-list "$range" 2>/dev/null)
echo "$TARGET_SHA" > "$LAST_SCANNED"

declare -A verbatim
while read -r iid; do
    [ -n "$iid" ] && verbatim["$iid"]=1
done < "$VERBATIM"

declare -A mapped
while read -r iid _; do
    [ -n "$iid" ] && mapped["$iid"]=1
done < "$MAPPED"

declare -A cached_sha cached_pid
while read -r iid sha pid; do
    [ -n "$iid" ] && cached_sha["$iid"]="$sha" && cached_pid["$iid"]="$pid"
done < "$CANDIDATES"

new_candidates=""
for ref in $(git for-each-ref --format='%(refname)' refs/remotes/mr); do
    iid="${ref##*/}"
    [ -n "${verbatim[$iid]}" ] && continue
    [ -n "${mapped[$iid]}" ] && continue

    sha="$(git rev-parse "$ref")"

    if git merge-base --is-ancestor "$sha" "$TARGET_SHA" 2>/dev/null; then
        echo "$iid" >> "$VERBATIM"
        continue
    fi

    if [ "${cached_sha[$iid]}" = "$sha" ] && [ -n "${cached_pid[$iid]}" ]; then
        pid="${cached_pid[$iid]}"
    else
        pid="$(patch_id_of "$sha")"
    fi

    landed_sha=""
    [ -n "$pid" ] && landed_sha="${landed_pid_to_sha[$pid]}"
    if [ -n "$landed_sha" ]; then
        git update-ref "refs/remotes/mr/${iid}" "$landed_sha"
        echo "$iid $landed_sha" >> "$MAPPED"
    else
        new_candidates="${new_candidates}${iid} ${sha} ${pid}
"
    fi
done

printf '%s' "$new_candidates" > "$CANDIDATES"
mr_state > "$SNAPSHOT"
