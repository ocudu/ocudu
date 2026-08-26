# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

# Build-environment image for fuzz testing with AFL++.
# Contains only system dependencies; fuzz targets are built at job time from
# the checked-out source so the image does not need to be rebuilt on every
# code change.
#
# Build (from the repository root):
#   docker build -f tests/fuzz/Dockerfile -t ocudu-fuzz-deps .
#
# Bump FUZZ_IMAGE_VERSION in .gitlab-ci.yml whenever this file changes.
# GitLab CI – see tests/fuzz/README.md for the full job definition.

ARG UBUNTU_VERSION=26.04
FROM ubuntu:${UBUNTU_VERSION}

ENV DEBIAN_FRONTEND=noninteractive

# ---------------------------------------------------------------------------
# System dependencies
# ---------------------------------------------------------------------------
# afl++            fuzzer toolchain: afl-clang-fast++, afl-fuzz, afl-cmin,
#                  afl-tmin, afl-whatsup, afl-plot
# clang            C/C++ compiler front-end (AFL++ LLVM mode)
# llvm / llvm-dev  LLVM back-end and llvm-symbolizer for ASAN stack traces
# cmake / ninja-build  build system
# python3          corpus generation script (gen_corpus.py)
# libmbedtls-dev   required: find_package(MbedTLS REQUIRED) in lib/security
# libsctp-dev      required: find_package(SCTP REQUIRED) in lib/gateways
# libyaml-cpp-dev  required: find_package(YAMLCPP REQUIRED) in top-level CMake
# ---------------------------------------------------------------------------
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        afl++ \
        clang \
        llvm \
        llvm-dev \
        cmake \
        ninja-build \
        python3 \
        libmbedtls-dev \
        libsctp-dev \
        libyaml-cpp-dev \
    && rm -rf /var/lib/apt/lists/*
