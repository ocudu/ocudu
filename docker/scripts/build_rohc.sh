#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set -e # stop executing after error

main() {
    # Check number of args
    if [ $# -lt 1 ] || [ $# -gt 3 ]; then
        echo >&2 "Illegal number of parameters"
        echo >&2 "Run like this: \"./build_rohc.sh <rohc_version> [<arch> [<ncores>]]\" where arch is any gcc/clang compatible -march and ncores could be any number or empty for all"
        exit 1
    fi

    local rohc_version=$1
    local arch="${2:-native}"
    local ncores="${3:-$(nproc)}"
    local rohc_name="rohc"
    local rohc_series="${rohc_version%.*}.x"
    local rohc_archive="${rohc_name}-${rohc_version}.tar.xz"

    cd /tmp
    curl "https://rohc-lib.org/download/rohc-${rohc_series}/${rohc_version}/${rohc_archive}" -o "${rohc_archive}"
    tar -xf "${rohc_archive}"

    pushd "${rohc_name}-${rohc_version}"
    export PATH="/bin:${PATH}"
    ./autogen.sh
    ./configure --prefix=/opt/rohc
    make all -j"${ncores}"
    make install
    ldconfig
    popd
    
    rm -Rf /tmp/rohc-"${rohc_version}"*

}

main "$@"
