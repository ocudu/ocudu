#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set -e # stop executing after error

main() {
    # Check number of args
    if [ $# -lt 0 ] || [ $# -gt 2 ]; then
        echo >&2 "Illegal number of parameters"
        echo >&2 "Run like this: \"./build_rohc.sh [<arch> [<ncores>]]\" where arch is any gcc/clang compatible -march and ncores could be any number or empty for all"
        exit 1
    fi

    local arch="${1:-native}"
    local ncores="${2:-$(nproc)}"
    local rohc_name="rohc"
    local rohc_version="2.3.1"
    local rohc_archive="${rohc_name}-${rohc_version}.tar.xz"
    local rohc_archive_sha256sum="e5c3808518239a6a4673c0c595356d5054b208f32e39015a487a0490d03f9bec"
    local SHA256SUM_CHECKLINE="${rohc_archive_sha256sum}  ${rohc_archive}"

    cd /tmp
    curl "https://rohc-lib.org/download/rohc-2.3.x/${rohc_version}/${rohc_archive}" -o "${rohc_archive}"
    if ! echo "$SHA256SUM_CHECKLINE" | sha256sum -c -; then
        echo "Checksum verification failed for ${rohc_archive}"
        exit 1
    fi
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
