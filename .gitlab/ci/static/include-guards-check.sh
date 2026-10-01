#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# make sure all commands are echoed
#set -x

# This script is an extended and customized version of
# https://github.com/sbeyer/include-guards-check-action

###################################################################################################
### Usage Examples
###################################################################################################
#
# Check whole project for include guards (pattern: {path}) and enable auto correction
#
#   AUTOCORRECTION=1 INPUT_PATH="include/ocudu" INPUT_IGNORE="/bundled/" INPUT_PATTERN="ocudu_{path}" include-guards-check.sh
#   AUTOCORRECTION=1 INPUT_PATH="." INPUT_IGNORE="^include/.*" INPUT_PATTERN="ocudu_{path}" include-guards-check.sh
#
#
#
# Check whole project for "#pragma once" include guards and enable auto correction
#
#   AUTOCORRECTION=1 PRAGMA=1 INPUT_PATH="include/ocudu" INPUT_IGNORE="/bundled/" include-guards-check.sh
#   AUTOCORRECTION=1 PRAGMA=1 INPUT_PATH="." INPUT_IGNORE="^include/.*" include-guards-check.sh
#
#
#
# Same as above, but only check modified files since "origin/dev" branch
#
#   AUTOCORRECTION=1 PRAGMA=1 TARGET_BRANCH=origin/dev INPUT_PATH="include/ocudu" INPUT_IGNORE="/bundled/" include-guards-check.sh
#   AUTOCORRECTION=1 PRAGMA=1 TARGET_BRANCH=origin/dev INPUT_PATH="." INPUT_IGNORE="^include/.*" include-guards-check.sh
#
#

macrofy_allow_lowercase () {
  echo -n "$1" |
    tr -c '[0-9A-Za-z_]' _ |
    sed -e 's/^_*\(.*\)$/\1/'
}

macrofy () {
  macrofy_allow_lowercase "$1" | tr '[a-z]' '[A-Z]'
}

guardify () {
  path="$1"
  file="$(basename "$path")"
  file_ext="$(echo "$file" | sed -e 's|^.*\.||')"
  file_base="$(echo "$file" | sed -e 's/^\(.*\)\.\([^\.]*\)$/\1/')"
  dirs="$(dirname "$path")"
  first_dir="$(echo "$dirs" | cut -d / -f 1)"
  last_dir="$(echo "$dirs" | sed -e 's|^.*/||')"

  echo -n "$INPUT_PATTERN" | sed \
      -e 's/{path}/'"$(macrofy "$path")"'/g' \
      -e 's/{file}/'"$(macrofy "$file")"'/g' \
      -e 's/{file_base}/'"$(macrofy "$file_base")"'/g' \
      -e 's/{file_ext}/'"$(macrofy "$file_ext")"'/g' \
      -e 's/{dirs}/'"$(macrofy "$dirs")"'/g' \
      -e 's/{first_dir}/'"$(macrofy "$first_dir")"'/g' \
      -e 's/{last_dir}/'"$(macrofy "$last_dir")"'/g'
}

guess () {
  path="$1"
  file="$(basename "$path")"
  file_base="$(echo "$file" | sed -e 's/^\(.*\)\.\([^\.]*\)$/\1/')"
  echo -n "$(macrofy "$file_base")"
}

die () {
  echo "::error::$1"
  exit 1
}

has_pragma () {
  header=$1
  awk '
    /^\s*#\s*pragma\s+once\s*$/ { pragma = 1 }
    pragma { exit 1 }' "$header"
  if [ $? -eq 0 ]; then
    return 1
  else
    return 0
  fi
}

has_guard () {
  header=$1
  guard=$2
  awk '
    /^\s*#\s*ifndef\s+'"$guard"'\>/ { ifndef = 1 }
    /^\s*#\s*define\s+'"$guard"'\>/ && ifndef { define = 1 }
    ifndef && define { exit 1 }' "$header"
  if [ $? -eq 0 ]; then
    return 1
  else
    return 0
  fi
}

echo "Checking pattern '$INPUT_PATTERN' ..."

dummy="dir1/dir2/dir3/base.between.ext"
actual="$(guardify dir1/dir2/dir3/base.between.ext)"
expected="$(macrofy_allow_lowercase "$actual")"

if test "$actual" != "$expected"
then
  cat <<EOF
  Oops! We tested the dummy file name
    $dummy
  and the given pattern produced
    $actual
  which contains invalid characters.
  Removing invalid characters produces
    $expected

  Please fix this.
EOF
  die "Pattern contains invalid characters!"
fi

echo "Switching to given path '$INPUT_PATH' ..."
cd "$INPUT_PATH" || die "Cannot change directory"

ret=0
failed=""
nfailed=0

fail () {
  failed="$failed$1 "
  nfailed=$((nfailed+1))
  echo "::error file={$1}::Header guard macro $2 expected in $1 but not found"
  ret=1

  #######################################################################################
  # Only apply auto correction if AUTOCORRECTION is set to a non-empty value
  # Warning: This will change your files! Ensure you can revert all files in INPUT_PATH
  #######################################################################################
  if [ ! -z "${AUTOCORRECTION}" ]   # if AUTOCORRECTION is set to any non-empty value
  then
    if [ ! -z "${PRAGMA}" ]   # if PRAGMA is set to any non-empty value
    then
      # replace guards by pragma once
      guard_guess="$(guess "$1")"
      sed -i -E 's/^\s*#\s*ifndef\s+.*'"$guard_guess"'\S*\s*$/#pragma once/' $1
      sed -i -E '/^\s*#\s*define\s+.*'"$guard_guess"'\S*\s*$/d' $1
      sed -i -E '/^\s*#\s*endif\s*\/\/\s*.*'"$guard_guess"'\S*\s*$/d' $1
    else
      if has_pragma "$1"; then
        # replace pragma once by ifndef/define pair
        sed -i -E  's/^\s*#\s*pragma\s+once\s*$/#ifndef '"$2"'\n#define '"$2"'/' $1
        # add new line if not already
        sed -i -e '$a\' $1
        # append closing endif // $guard
        echo '#endif // '"$2" >> $1
      else
        # update existing guard name
        guard_guess="$(guess "$1")"
        sed -i -E 's/^\s*#\s*ifndef\s+.*'"$guard_guess"'\S*\s*$/#ifndef '"$2"'/' $1
        sed -i -E 's/^\s*#\s*define\s+.*'"$guard_guess"'\S*\s*$/#define '"$2"'/' $1
        sed -i -E 's/^\s*#\s*endif\s*\/\/\s*.*'"$guard_guess"'\S*\s*$/#endif \/\/ '"$2"'/' $1
      fi
    fi
  fi
}

headers=""
if [ -z "$TARGET_BRANCH" ]
then
  headers=$(find . -regex '.+\.\(h\|H\|hh\|hpp\|hxx\)' |
      sed -e 's/^\.\///' |
      grep -e "$INPUT_ONLY" |
      grep -v -e '^.git/' ${INPUT_IGNORE:+-e} ${INPUT_IGNORE:+"$INPUT_IGNORE"})
else
  echo "$TARGET_BRANCH"
  echo "$INPUT_PATH"
  headers=$(git diff --diff-filter=ACMR --name-only --relative "$TARGET_BRANCH" HEAD -- . |
      grep '.\+\.\(h\|H\|hh\|hpp\|hxx\)' |
      sed -e "s/^${INPUT_PATH//\//\\/}\/*\///" |
      sed -e 's/^\.\///' |
      grep -e "$INPUT_ONLY" |
      grep -v -e '^.git/' ${INPUT_IGNORE:+-e} ${INPUT_IGNORE:+"$INPUT_IGNORE"})
fi

for header in $headers
do
  if [ ! -z "${PRAGMA}" ]   # if PRAGMA is set to any non-empty value
  then
    guard="#pragma once"
    echo "Checking $header for $guard"
    has_pragma "$header" ||
      fail "$header" "$guard"
  else
    guard="$(guardify "$header")"
    echo "Checking $header for $guard"
    has_guard "$header" "$guard" ||
      fail "$header" "$guard"
  fi
done

echo "::set-output name=fails::$failed"
echo "::set-output name=nfails::$nfailed"

exit $ret
