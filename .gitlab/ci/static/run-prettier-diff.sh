#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# make sure all commands are echoed
#set -x
set -o pipefail

# check for apps
if [ -n "$OCUDU_PRETTIER" ] && [ -x "$OCUDU_PRETTIER" ]; then
  prettier=$OCUDU_PRETTIER
elif app1=$(which prettier) && [ -x "$app1" ]; then
  prettier=$app1
else
  echo "Please install prettier or set OCUDU_PRETTIER to a valid executable"
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
  echo "Running Markdown format check between current state and ${target}"
  # Get modified files (added, removed or changed) compared with target branch
  files=$(git diff --name-only --relative --diff-filter=d "${target}" | grep -E "${FILE_EXTENSION_REGEX}" | tr '\n' ' ')
else
  echo "Running Markdown format check on all tracked files"
  # Get all tracked files matching the Markdown extension
  files=$(git ls-files | grep -E "${FILE_EXTENSION_REGEX}" | tr '\n' ' ')
fi

echo "Using prettier version:"
"$prettier" --version

# Use the configuration file given by PRETTIER_FILE. Without it prettier searches for its own, starting in the
# directory of each formatted file, so passing an unset path would only break that search
config_arg=""
if [ -n "${PRETTIER_FILE:-}" ]; then
  config_arg="--config ${PRETTIER_FILE}"
fi

# Run prettier for those files and apply changes
[ "$files" ] && "$prettier" ${config_arg} --write ${files} || echo "No files changed"
