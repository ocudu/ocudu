#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# make sure all commands are echoed
#set -x
set -o pipefail

# check for apps
app1=$(which git)
app2=$(which colordiff)
if [ ! -x "$app1" ] || [ ! -x "$app2" ]; then
  echo "Please install git and colordiff"
  exit 1
fi

# Get diff
diff=$(git diff)

if [ "$diff" ]; then
  # If diff is not empty, print it, save it as a patch and exit with errors
  echo "The following changes were detected:"
  echo "${diff}" | colordiff
  echo "${diff}" >ci.patch
  exit 1
else
  echo "No changes detected."
fi
