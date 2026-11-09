#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set -e # stop executing after error

main() {
    # Check number of args
    if [ $# -lt 1 ] || [ $# -gt 3 ]; then
        echo >&2 "Illegal number of parameters"
        echo >&2 "Run like this: \"./build_rohc.sh <rohc_version> [<arch> [<ncores>]]\" where arch is a gcc/clang architecture or CPU target and ncores could be any number or empty for all"
        exit 1
    fi

    local rohc_version=$1
    local arch="${2:-native}"
    local ncores="${3:-$(nproc)}"
    local rohc_series="${rohc_version%.*}.x"
    local rohc_ref="rohc-${rohc_version}"

    # The GitHub repository does not tag every release, those are pinned to the commit that made the release.
    case "${rohc_version}" in
        2.3.1)
            rohc_ref="8d00cde3bc79740f15ed1062269ae046c8558702"
            ;;
    esac

    local rohc_src="/tmp/rohc-${rohc_version}"
    local rohc_archive="/tmp/rohc-${rohc_version}-src.tar"
    rm -Rf "${rohc_src}" "${rohc_archive}"
    mkdir -p "${rohc_src}"

    # Prefer the official release tarball, use the GitHub repository as fallback.
    if ! curl -fL --connect-timeout 30 --retry 2 \
        "https://rohc-lib.org/download/rohc-${rohc_series}/${rohc_version}/rohc-${rohc_version}.tar.xz" \
        -o "${rohc_archive}"; then
        echo "Downloading ROHC ${rohc_version} from rohc-lib.org failed, falling back to GitHub"
        curl -fL --connect-timeout 30 --retry 2 \
            "https://github.com/didier-barvaux/rohc/archive/${rohc_ref}.tar.gz" \
            -o "${rohc_archive}"
    fi
    tar -xf "${rohc_archive}" -C "${rohc_src}" --strip-components=1
    rm -f "${rohc_archive}"

    pushd "${rohc_src}"
    export PATH="/usr/bin:/bin:${PATH}"
    ./autogen.sh
    local arch_flag="-march=${arch}"
    case "${arch}" in
        neoverse-* | cortex-* | native)
            if [[ "$(uname -m)" == "aarch64" ]]; then
                arch_flag="-mcpu=${arch}"
            fi
            ;;
    esac
    CFLAGS="${CFLAGS:--g -O2} ${arch_flag}" ./configure --prefix=/opt/rohc
    make all -j"${ncores}"
    make install
    ldconfig
    popd

    rm -Rf "${rohc_src}"

}

main "$@"
