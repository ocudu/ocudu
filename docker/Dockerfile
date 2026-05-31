# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# Build args
################
# OS                    Base image (Docker Hub name), e.g. ubuntu or debian
# OS_VERSION            Image tag, e.g. 24.04 or bookworm (must match OS)
# UHD_VERSION           UHD version number
# DPDK_VERSION          DPDK version number
# ROHC_VERSION          ROHC version number
# MARCH                 gcc/clang compatible arch
# NUM_JOBS              Number or empty for all
# EXTRA_CMAKE_ARGS      Extra flags for OCUDU

ARG OS=ubuntu
ARG OS_VERSION=26.04
ARG UHD_VERSION=4.9.0.1
ARG DPDK_VERSION=25.11.2
ARG ROHC_VERSION=2.3.1
ARG MARCH=native
ARG NUM_JOBS=""
ARG TARGETARCH

########################
# Stage 0: Shared base #
########################
# All build stages except builder-rohc share this base so that UHD/DPDK system
# deps (Boost, libusb, libnuma, libfdt, …) are installed only once
# builder-rohc is kept separate: its deps are known to conflict with the DPDK build.
FROM ${OS}:${OS_VERSION} AS builder-base

COPY docker/scripts/ /src/docker/scripts/

RUN /src/docker/scripts/install_dependencies.sh build && \
    /src/docker/scripts/install_uhd_dependencies.sh build && \
    /src/docker/scripts/install_dpdk_dependencies.sh build && \
    /src/docker/scripts/install_docker_dependencies.sh build

#########################
# Stage 1.1: Build ROHC #
#########################
FROM ${OS}:${OS_VERSION} AS builder-rohc

COPY docker/scripts/ /src/docker/scripts/

RUN /src/docker/scripts/install_rohc_dependencies.sh build

ARG ROHC_VERSION
ARG MARCH
ARG NUM_JOBS

RUN /src/docker/scripts/build_rohc.sh "${ROHC_VERSION}" ${MARCH} ${NUM_JOBS}


########################
# Stage 1.2: Build UHD #
########################
FROM builder-base AS builder-uhd

ARG UHD_VERSION
ARG MARCH
ARG NUM_JOBS
ARG TARGETARCH

RUN UHD_ARCH=$([ "${TARGETARCH}" = "arm64" ] && echo "-mcpu=${MARCH}" || echo "-march=${MARCH}") && \
    /src/docker/scripts/build_uhd.sh "${UHD_VERSION}" ${UHD_ARCH} ${NUM_JOBS}


#########################
# Stage 1.3: Build DPDK #
#########################
FROM builder-base AS builder-dpdk

ARG DPDK_VERSION
ARG MARCH
ARG NUM_JOBS
ARG TARGETARCH

RUN DPDK_PLATFORM=$(case "${MARCH}" in *neoverse-n1*) echo n1sdp ;; *) echo "${MARCH}" ;; esac) && \
    DPDK_ARCH=$([ "${TARGETARCH}" = "arm64" ] && echo "-Dplatform=${DPDK_PLATFORM} -Dmax_numa_nodes=1" || echo "-Dcpu_instruction_set=${MARCH}") && \
    /src/docker/scripts/build_dpdk.sh "${DPDK_VERSION}" "${DPDK_ARCH}" ${NUM_JOBS}


########################
# Stage 2: Build OCUDU #
########################
FROM builder-base AS builder

ARG UHD_VERSION
ARG DPDK_VERSION

# Copy compiled artifacts from the parallel build stages
COPY --from=builder-rohc /opt/rohc /opt/rohc
COPY --from=builder-uhd /opt/uhd/${UHD_VERSION} /opt/uhd/${UHD_VERSION}
COPY --from=builder-dpdk /opt/dpdk/${DPDK_VERSION} /opt/dpdk/${DPDK_VERSION}

ARG MARCH
ARG NUM_JOBS
ARG TARGETARCH
ARG COMPILER=gcc
ARG EXTRA_CMAKE_ARGS=""

ENV ROHC_DIR=/opt/rohc
ENV UHD_DIR=/opt/uhd/${UHD_VERSION}
ENV DPDK_DIR=/opt/dpdk/${DPDK_VERSION}

# Full source added last: source changes only invalidate the compile step below
ADD . /src

RUN if [ -z "$NUM_JOBS" ]; then NUM_JOBS=$(nproc); fi \
    && ARCH_FLAG=$([ "${TARGETARCH}" = "arm64" ] && echo "-DMCPU=${MARCH}" || echo "-DMARCH=${MARCH}") \
    && \
    /src/docker/scripts/builder.sh -c ${COMPILER} -m "-j${NUM_JOBS} ocu odu odu_split_8 odu_split_7_2 gnb gnb_split_8 gnb_split_7_2 ru_emulator" \
    -DBUILD_TESTING=False -DENABLE_UHD=On -DENABLE_DPDK=On ${ARCH_FLAG} -DCMAKE_INSTALL_PREFIX=/opt/ocudu \
    ${EXTRA_CMAKE_ARGS} /src
RUN cp /src/build/apps/cu/ocu                     /tmp/ocu                     && \
    cp /src/build/apps/du/odu                     /tmp/odu                     && \
    cp /src/build/apps/du_split_8/odu             /tmp/odu_split_8             && \
    cp /src/build/apps/du_split_7_2/odu           /tmp/odu_split_7_2           && \
    cp /src/build/apps/gnb/gnb                    /tmp/gnb                     && \
    cp /src/build/apps/gnb_split_8/gnb            /tmp/gnb_split_8             && \
    cp /src/build/apps/gnb_split_7_2/gnb          /tmp/gnb_split_7_2           && \
    cp /src/build/apps/examples/ofh/ru_emulator   /tmp/ru_emulator             && \
    cd /src/build                                                              && \
    make install                                                               && \
    mv /tmp/ocu                                   /opt/ocudu/bin/ocu           && \
    mv /tmp/odu                                   /opt/ocudu/bin/odu           && \
    mv /tmp/odu_split_8                           /opt/ocudu/bin/odu_split_8   && \
    mv /tmp/odu_split_7_2                         /opt/ocudu/bin/odu_split_7_2 && \
    mv /tmp/gnb                                   /opt/ocudu/bin/gnb           && \
    mv /tmp/gnb_split_8                           /opt/ocudu/bin/gnb_split_8   && \
    mv /tmp/gnb_split_7_2                         /opt/ocudu/bin/gnb_split_7_2 && \
    mv /tmp/ru_emulator                           /opt/ocudu/bin/ru_emulator   && \
    rm -rf /src

################
# Stage 3: Run #
################

FROM ${OS}:${OS_VERSION} AS runtime

ARG UHD_VERSION
ARG DPDK_VERSION

# Copy OCUDU binaries and libraries installed in previous stage(s)
COPY --from=builder /opt/rohc                 /opt/rohc
COPY --from=builder /opt/uhd/${UHD_VERSION}   /opt/uhd/${UHD_VERSION}
COPY --from=builder /opt/dpdk/${DPDK_VERSION} /opt/dpdk/${DPDK_VERSION}
COPY --from=builder /opt/ocudu                /usr/local

# Copy the install dependencies scripts (top-level dispatchers and per-OS implementations)
ADD docker/scripts/install_*.sh /usr/local/etc/
RUN chmod +x /usr/local/etc/install_*.sh && \
    ln -sf /usr/local/etc/install_dependencies.sh /usr/local/etc/install_ocudu_dependencies.sh
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/rohc/lib/
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/uhd/${UHD_VERSION}/lib/:/opt/uhd/${UHD_VERSION}/lib/x86_64-linux-gnu/:/opt/uhd/${UHD_VERSION}/lib/aarch64-linux-gnu/
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/dpdk/${DPDK_VERSION}/lib/:/opt/dpdk/${DPDK_VERSION}/lib/x86_64-linux-gnu/:/opt/dpdk/${DPDK_VERSION}/lib/aarch64-linux-gnu/
ENV PATH=$PATH:/opt/uhd/${UHD_VERSION}/bin/:/opt/dpdk/${DPDK_VERSION}/bin/

# Install ocudu and lib runtime dependencies (curl / NTP helpers: install_docker_dependencies.sh)
RUN /usr/local/etc/install_ocudu_dependencies.sh run && \
    /usr/local/etc/install_rohc_dependencies.sh run && \
    /usr/local/etc/install_uhd_dependencies.sh run && \
    /usr/local/etc/install_dpdk_dependencies.sh run && \
    /usr/local/etc/install_docker_dependencies.sh run

# Register DPDK/UHD/ROHC library paths system-wide.
# When the gnb binary has file capabilities (see next RUN), ld.so enters
# secure-execution mode on execve() and STRIPS LD_LIBRARY_PATH. The runtime
# libs must therefore be findable via /etc/ld.so.cache instead.
RUN printf '%s\n' \
    "/opt/rohc/lib" \
    "/opt/uhd/${UHD_VERSION}/lib" \
    "/opt/uhd/${UHD_VERSION}/lib/x86_64-linux-gnu" \
    "/opt/uhd/${UHD_VERSION}/lib/aarch64-linux-gnu" \
    "/opt/dpdk/${DPDK_VERSION}/lib" \
    "/opt/dpdk/${DPDK_VERSION}/lib/x86_64-linux-gnu" \
    "/opt/dpdk/${DPDK_VERSION}/lib/aarch64-linux-gnu" \
    > /etc/ld.so.conf.d/ocudu.conf && \
    ldconfig

# Bake minimum-privilege Linux capabilities into the binaries that need to
# run as a non-root uid in Kubernetes. Kubernetes' capabilities.add only
# populates the bounding set for non-root processes; for caps to land in
# the effective set after execve(), they must be declared as file caps.
#
#   cap_sys_nice  - SCHED_FIFO on ru_timing / tx-rx threads (DPDK real-time)
#   cap_ipc_lock  - mlock() on DPDK hugepages
#   cap_perfmon   - perf_event_open() RAPL energy reader (kernel >= 5.8, opt-in via metricsService.powercap.enabled)
#
# gnb/odu: DPDK real-time + hugepages + powercap metrics
# ocu: no DPDK/hugepages, no powercap; SYS_NICE+IPC_LOCK for non-root Kubernetes scheduling
RUN setcap cap_sys_nice,cap_ipc_lock,cap_perfmon+ep /usr/local/bin/gnb && \
    setcap cap_sys_nice,cap_ipc_lock,cap_perfmon+ep /usr/local/bin/odu && \
    setcap cap_sys_nice,cap_ipc_lock,cap_perfmon+ep /usr/local/bin/odu_split_8 && \
    setcap cap_sys_nice,cap_ipc_lock,cap_perfmon+ep /usr/local/bin/odu_split_7_2 && \
    setcap cap_sys_nice,cap_ipc_lock+ep             /usr/local/bin/ocu && \
    setcap cap_sys_nice,cap_ipc_lock+ep             /usr/local/bin/ru_emulator && \
    getcap /usr/local/bin/gnb /usr/local/bin/odu /usr/local/bin/odu_split_8 \
           /usr/local/bin/odu_split_7_2 /usr/local/bin/ocu /usr/local/bin/ru_emulator
