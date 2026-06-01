#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# make sure all commands are echoed
#set -x
set -o pipefail

# check for apps
app1=$(which clang-format)
app2=$(which git)
if [ ! -x "$app1" ] || [ ! -x "$app2" ]; then
  echo "Please install clang-format and git"
  exit 1
fi

FILE_EXTENSION_REGEX='.*\.(cpp|cc|c\+\+|cxx|c|cl|h|hh|hpp)$'
target=$1

if [ "$target" ]; then
  echo "Running code format check between current state and ${target}"
  # Get modified files (added, removed or changed) compared with target branch
  files=$(git diff --name-only --relative --diff-filter=d "${target}" | grep -E "${FILE_EXTENSION_REGEX}" | tr '\n' ' ')
else
  echo "Running code format check on all tracked files"
  # Get all tracked files matching source extensions
  files=$(git ls-files | grep -E "${FILE_EXTENSION_REGEX}" | tr '\n' ' ')
fi

echo "Using clang-format version:"
clang-format --version

# Run clang-format for those files and apply changes
[ "$files" ] && clang-format -style=file -i ${files} || echo "No files changed"
