#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# make sure all commands are echoed
#set -x
set -o pipefail

# check for apps
if [ -n "$OCUDU_MARKDOWNLINT" ] && [ -x "$OCUDU_MARKDOWNLINT" ]; then
  markdownlint=$OCUDU_MARKDOWNLINT
elif app1=$(which markdownlint-cli2) && [ -x "$app1" ]; then
  markdownlint=$app1
else
  echo "Please install markdownlint-cli2 or set OCUDU_MARKDOWNLINT to a valid executable"
  exit 1
fi

app2=$(which git)
if [ ! -x "$app2" ]; then
  echo "Please install git"
  exit 1
fi

FILE_EXTENSION_REGEX='.*\.md$'
target=$1

if [ "$target" ]; then
  echo "Running Markdown lint between current state and ${target}"
  # Get modified files (added, removed or changed) compared with target branch
  files=$(git diff --name-only --relative --diff-filter=d "${target}" | grep -E "${FILE_EXTENSION_REGEX}" | tr '\n' ' ')
else
  echo "Running Markdown lint on all tracked files"
  # Get all tracked files matching the Markdown extension
  files=$(git ls-files | grep -E "${FILE_EXTENSION_REGEX}" | tr '\n' ' ')
fi

if [ -z "$files" ]; then
  echo "No files to lint"
  exit 0
fi

# Unlike prettier, markdownlint does not rewrite the files: it reports the violations it finds and exits non-zero,
# so its exit code must be propagated instead of relying on is-pristine-repo.sh
"$markdownlint" --config .markdownlint.jsonc ${files}
