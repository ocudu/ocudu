#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set -e # stop executing after error

main() {
    # Check number of args
    if [ $# -lt 1 ] || [ $# -gt 3 ]; then
        echo >&2 "Illegal number of parameters"
        echo >&2 "Run like this: \"./build_uhd.sh <uhd_version> [<arch> [<ncores>]]\" where arch is any gcc/clang compatible -march and ncores could be any number or empty for all"
        exit 1
    fi

    local uhd_version=$1
    local arch="${2:--march=native}"
    local ncores="${3:-$(nproc)}"

    cd /tmp
    curl -L "https://github.com/EttusResearch/uhd/archive/refs/tags/v${uhd_version}.tar.gz" | tar xzf -

    local uhd_root="/tmp/uhd-${uhd_version}"

    cd "${uhd_root}/host" && mkdir -p build && cd build
    # CMake 4.0+ rejects cmake_minimum_required(VERSION <3.5) in UHD's bundled lib/rc (CMakeRC.cmake).
    # Harmless on older CMake (unused cache entry).
    cmake \
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
        -DCMAKE_INSTALL_PREFIX=/opt/uhd/"${uhd_version}" \
        -DENABLE_LIBUHD=On \
        -DENABLE_PYTHON_API=Off \
        -DENABLE_EXAMPLES=Off \
        -DENABLE_TESTS=Off \
        -DBoost_NO_BOOST_CMAKE=ON \
        -DCMAKE_CXX_FLAGS="${arch}" ..
    cmake --build . -- -j"${ncores}"
    cmake --install .

    rm -Rf /tmp/uhd*"${uhd_version}"

}

main "$@"
