#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set -e

# Parse multiple paths from the first argument (e.g., "path1 path2 path3")
RAW_INPUT=$1
ADDITIONAL_IGNORE=""

for entry in $RAW_INPUT; do
    CLEAN_PATH=$(echo "$entry" | sed 's|^/||;s|/$||')  # Strip leading/trailing slashes

    if [ -n "$CLEAN_PATH" ]; then
        ADDITIONAL_IGNORE="$ADDITIONAL_IGNORE ! -path */${CLEAN_PATH}/*"
    fi
done

echo "=================="
echo "= Update headers ="
echo "= Ignore flags: $ADDITIONAL_IGNORE ="
echo "=================="

# ============================================================================
# Update headers for CMake/YML/Script files
# ============================================================================
find . -type f \( \
        -name "CMakeLists.txt" \
        -o -name "*.cmake" \
        -o -name "*.yml" \
        -o -name "*.sh" \
        -o -name "*.py" \
        -o -name "*.toml" \
        -o -name "Dockerfile" \
        -o -name "ocudu_performance" \
        -o -name ".gdbinit" \
        -o -name "*.tf" \
        -o -name "*.tfvars" \
        -o -name "*.conf" \
        -o -path "./docs/doxygen/CMakeGenerateTeXMacros.txt" \
        -o -path "./tests/utils/gdb/pretty_printers/pretty_printer_gdb_commands.txt" \
    \) \
    \( -path "./tests/unittests/fapi/*/builders/CMakeLists.txt" -o ! -path "*/build*/*" \) \
    ! -path "*/.tox/*" \
    ! -path "*/docker/open5gs/*" \
    ! -path "*/node_modules*/*" \
    $ADDITIONAL_IGNORE \
    ! -name "FindBackward.cmake" \
    ! -name "sbom.cmake" \
    -print0 | while IFS= read -r -d '' file; do

    # Check header format
    found_header=false
    while IFS= read -r line; do
        if [ -z "$line" ] && [ "$found_header" = false ]; then
            continue  # Ignore empty lines before first comment block
        elif [[ "$line" =~ ^#.*$ ]]; then
            found_header=true
            continue  # Line starts with #, keep reading
        elif [[ -z "$line" ]]; then
            break  # Empty line after header block - format is valid
        else
            echo "$file: Header (or empty line after it) is missing."
            exit 1
        fi
    done <"$file"

    # Replace header
    perl -0777 -pi -e "s/#[^!][\s\S]*?(?=\n.*?=|\n\n)/#

# SPDX-FileCopyrightText: Copyright (C) 2021-$(date +%Y) Software Radio Systems Limited
#
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

done

# ============================================================================
# Update headers for C++/JS source and header files
# ============================================================================
find . -type f \( \
        -name "*.cpp" \
        -o -name "*.h" \
        -o -name "*.h.in" \
        -o -name "*.js" \
        -o -name "*.proto" \
        -o -name "*.dox" \
    \) \
    ! -path "*/external/*" \
    ! -path "*/.docusaurus/*" \
    ! -path "*/node_modules/*" \
    $ADDITIONAL_IGNORE \
    ! -name "rfnoc_test.cc" \
    -exec perl -0777 -pi -e "s{/\*.*?\*/}{/*
 *
 * SPDX-FileCopyrightText: Copyright (C) 2021-$(date +%Y) Software Radio Systems Limited
 *
 * SPDX-License-Identifier: BSD-3-Clause-Open-MPI
 *
 */}s" {} \;

# ============================================================================
# Update headers for MATLAB files
# Note: In MATLAB, the header is the second "%" comment block,
#       as the first block contains the file documentation
# ============================================================================
find . -type f -name "*.m" \
    $ADDITIONAL_IGNORE \
    ! -name "hSkipWeakTimingOffset.m" \
    ! -name "HARQEntity.m" \
    -exec perl -0777 -pi -e "s/((?:%.*\n)+\n)(?:%.*\n)+/\$1%
% SPDX-FileCopyrightText: Copyright (C) 2021-$(date +%Y) Software Radio Systems Limited
%
% SPDX-License-Identifier: BSD-3-Clause-Open-MPI
%
/" {} \;

# ============================================================================
# Update headers for YANG files (// comment style)
# ============================================================================
find . -type f -name "*.yang" \
    ! -path "*/external/*" \
    ! -path "*/node_modules/*" \
    $ADDITIONAL_IGNORE \
    -print0 | while IFS= read -r -d '' file; do

    # Check header format
    found_header=false
    while IFS= read -r line; do
        if [ -z "$line" ] && [ "$found_header" = false ]; then
            continue  # Ignore empty lines before first comment block
        elif [[ "$line" =~ ^//.*$ ]]; then
            found_header=true
            continue  # Line starts with //, keep reading
        elif [[ -z "$line" ]]; then
            break  # Empty line after header block - format is valid
        else
            echo "$file: Header (or empty line after it) is missing."
            exit 1
        fi
    done <"$file"

    # Replace header
    perl -0777 -pi -e "s{//[\s\S]*?(?=\n\n)}{//
// SPDX-FileCopyrightText: Copyright (C) 2021-$(date +%Y) Software Radio Systems Limited
//
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
//}s" "$file"

done

# ============================================================================
# Update headers for XML files (<!-- --> comment style)
# ============================================================================
find . -type f -name "*.xml" \
    -o -name "*.html" \
    ! -path "*/external/*" \
    ! -path "*/node_modules/*" \
    ! -path "*/build/*" \
    $ADDITIONAL_IGNORE \
    -exec perl -0777 -pi -e "s{<!--[\s\S]*?-->}{<!--
SPDX-FileCopyrightText: Copyright (C) 2021-$(date +%Y) Software Radio Systems Limited

SPDX-License-Identifier: BSD-3-Clause-Open-MPI
-->}s" {} \;

# ============================================================================
# Update copyright in LICENSE and other that do not match
# any of the previous patterns.
# ============================================================================
for FILENAME in "LICENSE"
do
    if [ -f "$FILENAME" ]; then
        perl -0777 -pi -e "s/Copyright 2021-202\d Software Radio/Copyright 2021-$(date +%Y) Software Radio/" $FILENAME
    fi
done
