# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

# Single-stage image for fuzz testing with AFL++.
#
# A single stage is used so that the AFL++/clang ASAN runtime libraries are
# always co-located with the compiled fuzz binaries.  Multi-stage builds risk
# the runner stage missing the compiler-rt shared objects that ASAN needs.
#
# Build (from the repository root):
#   docker build -f tests/fuzz/Dockerfile -t ocudu-fuzz .
#
# Run – findings written to ./findings on the host:
#   docker run --rm \
#       -v "$(pwd)/findings:/findings" \
#       ocudu-fuzz
#
# Override defaults:
#   docker run --rm \
#       -v "$(pwd)/findings:/findings" \
#       -e FUZZ_TIMEOUT_EACH=3600 \
#       -e FUZZ_TARGETS="ofh_uplane_decoder_fuzzer ofh_ecpri_decoder_fuzzer" \
#       ocudu-fuzz
#
# GitLab CI – see tests/fuzz/README.md for a full job definition.

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

# ---------------------------------------------------------------------------
# Build fuzz targets
# ---------------------------------------------------------------------------
# Copy the full repository.  The Docker build context must be the repo root:
#   docker build -f tests/fuzz/Dockerfile -t ocudu-fuzz .
COPY . /src

# Configure.  Every optional subsystem that is not required for the OFH layer
# is disabled to keep configure fast and the build minimal.
RUN cmake -S /src -B /build \
        -GNinja \
        -DCMAKE_C_COMPILER=afl-clang-fast \
        -DCMAKE_CXX_COMPILER=afl-clang-fast++ \
        -DCMAKE_BUILD_TYPE=Debug \
        -DENABLE_FUZZTESTS=ON \
        -DENABLE_ASAN=ON \
        -DBUILD_TESTING=OFF \
        -DENABLE_UHD=OFF \
        -DENABLE_ZEROMQ=OFF \
        -DENABLE_FFTW=OFF \
        -DENABLE_MKL=OFF \
        -DENABLE_FFTZ=OFF \
        -DENABLE_ARMPL=OFF \
        -DENABLE_DPDK=OFF \
        -DENABLE_LIBNUMA=OFF \
        -DENABLE_BACKWARD=OFF \
        -DENABLE_WERROR=OFF

# Build OFH and NGAP fuzz targets – no need to build the full project.
RUN ninja -C /build \
        ofh_uplane_decoder_fuzzer \
        ofh_ecpri_decoder_fuzzer \
        ofh_vlan_frame_decoder_fuzzer \
        ngap_pdu_decoder_fuzzer \
        ngap_cu_cp_fuzzer

# Install binaries to a standard PATH location.
RUN cp \
        /build/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer \
        /build/tests/fuzz/ofh/ofh_ecpri_decoder_fuzzer \
        /build/tests/fuzz/ofh/ofh_vlan_frame_decoder_fuzzer \
        /build/tests/fuzz/ngap/ngap_pdu_decoder_fuzzer \
        /build/tests/fuzz/ngap/ngap_cu_cp_fuzzer \
        /usr/local/bin/

# ---------------------------------------------------------------------------
# Seed corpus
# ---------------------------------------------------------------------------
RUN python3 /src/tests/fuzz/ofh/gen_corpus.py
RUN python3 /src/tests/fuzz/ngap/gen_corpus.py
# Corpora land alongside their gen_corpus.py scripts; merge to a stable path.
RUN mkdir -p /corpus && \
    cp -r /src/tests/fuzz/ofh/corpus/* /corpus/ && \
    cp -r /src/tests/fuzz/ngap/corpus/* /corpus/

# ---------------------------------------------------------------------------
# Entrypoint
# ---------------------------------------------------------------------------
COPY tests/fuzz/run_fuzzers.sh /usr/local/bin/run_fuzzers.sh
RUN chmod +x /usr/local/bin/run_fuzzers.sh

# /findings is bind-mounted by the caller so AFL++ output survives container
# exit and can be collected as GitLab CI artifacts.
VOLUME ["/findings"]

ENTRYPOINT ["/usr/local/bin/run_fuzzers.sh"]
