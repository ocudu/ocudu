# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# Build args
################
# OS                    Base image name, e.g. ubuntu, debian, or fedora
# OS_VERSION            Image tag, e.g. 24.04, bookworm, or 43 (must match OS)
# UHD_VERSION           UHD version number
# DPDK_VERSION          DPDK version number
# ROHC_VERSION          ROHC version number
# MARCH                 gcc/clang compatible arch
# NUM_JOBS              Number or empty for all
# EXTRA_CMAKE_ARGS      Extra flags for OCUDU
# COMPONENT             Which component to build: gnb | du | cu | cu-cp | cu-up | ru-emulator
#                       Controls which single binary is compiled and installed.

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
ARG COMPONENT=""
ARG ENABLE_UHD=On
ARG ENABLE_DPDK=On

ENV ROHC_DIR=/opt/rohc
ENV UHD_DIR=/opt/uhd/${UHD_VERSION}
ENV DPDK_DIR=/opt/dpdk/${DPDK_VERSION}

# Full source added last: source changes only invalidate the compile step below
ADD . /src

RUN if [ -z "$NUM_JOBS" ]; then NUM_JOBS=$(nproc); fi \
    && ARCH_FLAG=$([ "${TARGETARCH}" = "arm64" ] && echo "-DMCPU=${MARCH}" || echo "-DMARCH=${MARCH}") \
    && mkdir -p /opt/ocudu/bin \
    && if [ -z "${COMPONENT}" ]; then \
         /src/docker/scripts/builder.sh -c ${COMPILER} -m "-j${NUM_JOBS} ocu ocucp ocuup odu odu_low odu_split_8 odu_split_7_2 gnb gnb_split_8 gnb_split_7_2 ru_emulator" \
           -DBUILD_TESTING=False -DENABLE_UHD=${ENABLE_UHD} -DENABLE_DPDK=${ENABLE_DPDK} \
           ${ARCH_FLAG} -DCMAKE_INSTALL_PREFIX=/opt/ocudu ${EXTRA_CMAKE_ARGS} /src \
         && cp /src/build/apps/cu/ocu                     /opt/ocudu/bin/ocu           \
         && cp /src/build/apps/cu_cp/ocucp                /opt/ocudu/bin/ocucp         \
         && cp /src/build/apps/cu_up/ocuup                /opt/ocudu/bin/ocuup         \
         && cp /src/build/apps/du/odu                     /opt/ocudu/bin/odu           \
         && cp /src/build/apps/du_low/odu_low             /opt/ocudu/bin/odu_low       \
         && cp /src/build/apps/du_split_8/odu             /opt/ocudu/bin/odu_split_8   \
         && cp /src/build/apps/du_split_7_2/odu           /opt/ocudu/bin/odu_split_7_2 \
         && cp /src/build/apps/gnb/gnb                    /opt/ocudu/bin/gnb           \
         && cp /src/build/apps/gnb_split_8/gnb            /opt/ocudu/bin/gnb_split_8   \
         && cp /src/build/apps/gnb_split_7_2/gnb          /opt/ocudu/bin/gnb_split_7_2 \
         && cp /src/build/apps/examples/ofh/ru_emulator   /opt/ocudu/bin/ru_emulator; \
       else \
         case "${COMPONENT}" in \
           gnb)         TARGET="gnb";         SRC="apps/gnb/gnb";                  BIN="gnb" ;; \
           du)          TARGET="odu";         SRC="apps/du/odu";                   BIN="odu" ;; \
           cu)          TARGET="ocu";         SRC="apps/cu/ocu";                   BIN="ocu" ;; \
           cu-cp)       TARGET="ocucp";       SRC="apps/cu_cp/ocucp";              BIN="ocucp" ;; \
           cu-up)       TARGET="ocuup";       SRC="apps/cu_up/ocuup";              BIN="ocuup" ;; \
           ru-emulator) TARGET="ru_emulator"; SRC="apps/examples/ofh/ru_emulator"; BIN="ru_emulator" ;; \
           *) echo "Unknown COMPONENT: ${COMPONENT}" && exit 1 ;; \
         esac \
         && /src/docker/scripts/builder.sh -c ${COMPILER} -m "-j${NUM_JOBS} ${TARGET}" \
            -DBUILD_TESTING=False -DENABLE_UHD=${ENABLE_UHD} -DENABLE_DPDK=${ENABLE_DPDK} \
            ${ARCH_FLAG} -DCMAKE_INSTALL_PREFIX=/opt/ocudu ${EXTRA_CMAKE_ARGS} /src \
         && cp /src/build/${SRC} /opt/ocudu/bin/${BIN}; \
       fi \
    && rm -rf /src

############################
# Stage 2b: Build OCUDU    #
# (lean — no UHD/DPDK)     #
# Used for cu, cu-cp, cu-up #
############################
FROM builder-base AS builder-lean

ARG MARCH
ARG NUM_JOBS
ARG TARGETARCH
ARG COMPILER=gcc
ARG EXTRA_CMAKE_ARGS=""
ARG COMPONENT=""
ARG ENABLE_UHD=Off
ARG ENABLE_DPDK=Off

COPY --from=builder-rohc /opt/rohc /opt/rohc

ENV ROHC_DIR=/opt/rohc

ADD . /src

RUN if [ -z "$NUM_JOBS" ]; then NUM_JOBS=$(nproc); fi \
    && ARCH_FLAG=$([ "${TARGETARCH}" = "arm64" ] && echo "-DMCPU=${MARCH}" || echo "-DMARCH=${MARCH}") \
    && mkdir -p /opt/ocudu/bin \
    && case "${COMPONENT}" in \
         cu)    TARGET="ocu";   SRC="apps/cu/ocu";      BIN="ocu"   ;; \
         cu-cp) TARGET="ocucp"; SRC="apps/cu_cp/ocucp"; BIN="ocucp" ;; \
         cu-up) TARGET="ocuup"; SRC="apps/cu_up/ocuup"; BIN="ocuup" ;; \
         *) echo "builder-lean only supports cu/cu-cp/cu-up (got: '${COMPONENT}')" && exit 1 ;; \
       esac \
    && /src/docker/scripts/builder.sh -c ${COMPILER} -m "-j${NUM_JOBS} ${TARGET}" \
       -DBUILD_TESTING=False -DENABLE_UHD=${ENABLE_UHD} -DENABLE_DPDK=${ENABLE_DPDK} \
       ${ARCH_FLAG} -DCMAKE_INSTALL_PREFIX=/opt/ocudu ${EXTRA_CMAKE_ARGS} /src \
    && cp /src/build/${SRC} /opt/ocudu/bin/${BIN} \
    && rm -rf /src

###################
# Stage 3: Run    #
###################

# Shared runtime base: installs OS packages and sets env vars.
# Does NOT copy any built artifacts — extended by runtime and runtime-lean.
FROM ${OS}:${OS_VERSION} AS runtime-base

ARG UHD_VERSION
ARG DPDK_VERSION

# Copy the install dependencies scripts (top-level dispatchers and per-OS implementations)
ADD docker/scripts/install_*.sh /usr/local/etc/
RUN chmod +x /usr/local/etc/install_*.sh && \
    ln -sf /usr/local/etc/install_dependencies.sh /usr/local/etc/install_ocudu_dependencies.sh
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/rohc/lib/:/opt/rohc/lib64/
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/uhd/${UHD_VERSION}/lib/:/opt/uhd/${UHD_VERSION}/lib64/:/opt/uhd/${UHD_VERSION}/lib/x86_64-linux-gnu/:/opt/uhd/${UHD_VERSION}/lib/aarch64-linux-gnu/
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/dpdk/${DPDK_VERSION}/lib/:/opt/dpdk/${DPDK_VERSION}/lib64/:/opt/dpdk/${DPDK_VERSION}/lib/x86_64-linux-gnu/:/opt/dpdk/${DPDK_VERSION}/lib/aarch64-linux-gnu/
ENV PATH=$PATH:/opt/uhd/${UHD_VERSION}/bin/

# Install base runtime dependencies shared by all targets
RUN /usr/local/etc/install_ocudu_dependencies.sh run && \
    /usr/local/etc/install_rohc_dependencies.sh run && \
    /usr/local/etc/install_docker_dependencies.sh run

# Register DPDK/UHD/ROHC library paths system-wide.
# When the gnb binary has file capabilities (see next RUN), ld.so enters
# secure-execution mode on execve() and STRIPS LD_LIBRARY_PATH. The runtime
# libs must therefore be findable via /etc/ld.so.cache instead.
RUN printf '%s\n' \
    "/opt/rohc/lib" \
    "/opt/uhd/${UHD_VERSION}/lib" \
    "/opt/uhd/${UHD_VERSION}/lib64" \
    "/opt/uhd/${UHD_VERSION}/lib/x86_64-linux-gnu" \
    "/opt/uhd/${UHD_VERSION}/lib/aarch64-linux-gnu" \
    "/opt/dpdk/${DPDK_VERSION}/lib" \
    "/opt/dpdk/${DPDK_VERSION}/lib64" \
    "/opt/dpdk/${DPDK_VERSION}/lib/x86_64-linux-gnu" \
    "/opt/dpdk/${DPDK_VERSION}/lib/aarch64-linux-gnu" \
    > /etc/ld.so.conf.d/ocudu.conf && \
    ldconfig

# Stage 3a: runtime-lean — cu, cu-cp, cu-up (no UHD/DPDK build or runtime libs)
FROM runtime-base AS runtime-lean

ARG COMPONENT=""

COPY --from=builder-lean /opt/rohc  /opt/rohc
COPY --from=builder-lean /opt/ocudu /usr/local

# Bake minimum-privilege Linux capabilities into the binaries that need to
# run as a non-root uid in Kubernetes. Kubernetes' capabilities.add only
# populates the bounding set for non-root processes; for caps to land in
# the effective set after execve(), they must be declared as file caps.
#
#   cap_sys_nice  - SCHED_FIFO on ru_timing / tx-rx threads (DPDK real-time)
#   cap_ipc_lock  - mlock() on DPDK hugepages
RUN case "${COMPONENT}" in \
      cu)    setcap cap_sys_nice,cap_ipc_lock+ep /usr/local/bin/ocu   && getcap /usr/local/bin/ocu ;; \
      cu-cp) setcap cap_sys_nice,cap_ipc_lock+ep /usr/local/bin/ocucp && getcap /usr/local/bin/ocucp ;; \
      cu-up) setcap cap_sys_nice,cap_ipc_lock+ep /usr/local/bin/ocuup && getcap /usr/local/bin/ocuup ;; \
      *) echo "runtime-lean only supports cu/cu-cp/cu-up (got: '${COMPONENT}')" && exit 1 ;; \
    esac

# Stage 3b: runtime — gnb, du, ru-emulator, and legacy all-binary build.
# This is the last stage, so docker-compose (no --target) lands here.
FROM runtime-base AS runtime

ARG UHD_VERSION
ARG DPDK_VERSION
ARG COMPONENT=""
ARG ENABLE_UHD=On
ARG ENABLE_DPDK=On

# Copy OCUDU binaries and libraries installed in previous stage(s)
COPY --from=builder /opt/rohc                 /opt/rohc
COPY --from=builder /opt/uhd/${UHD_VERSION}   /opt/uhd/${UHD_VERSION}
COPY --from=builder /opt/dpdk/${DPDK_VERSION} /opt/dpdk/${DPDK_VERSION}
COPY --from=builder /opt/ocudu                /usr/local

# Install UHD/DPDK runtime dependencies after copying binaries — uhd_images_downloader
# must be on PATH before install_uhd_dependencies.sh run can execute.
# Only install deps for drivers that were actually built.
RUN if [ "${ENABLE_UHD}" = "On" ]; then /usr/local/etc/install_uhd_dependencies.sh run; fi && \
    if [ "${ENABLE_DPDK}" = "On" ]; then /usr/local/etc/install_dpdk_dependencies.sh run; fi

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
# ocu/ocucp/ocuup/ru_emulator: SYS_NICE+IPC_LOCK for non-root Kubernetes scheduling
# When COMPONENT is empty (legacy all-binary build), set caps on all binaries that exist.
RUN case "${COMPONENT}" in \
      gnb|du) \
        BIN=$([ "${COMPONENT}" = "gnb" ] && echo "gnb" || echo "odu") && \
        setcap cap_sys_nice,cap_ipc_lock,cap_perfmon+ep /usr/local/bin/${BIN} && \
        getcap /usr/local/bin/${BIN} ;; \
      ru-emulator) setcap cap_sys_nice,cap_ipc_lock+ep /usr/local/bin/ru_emulator && getcap /usr/local/bin/ru_emulator ;; \
      *) \
        for bin in gnb odu; do \
            test -f /usr/local/bin/${bin} && setcap cap_sys_nice,cap_ipc_lock,cap_perfmon+ep /usr/local/bin/${bin} || true; \
        done && \
        for bin in ocu ocucp ocuup ru_emulator; do \
            test -f /usr/local/bin/${bin} && setcap cap_sys_nice,cap_ipc_lock+ep /usr/local/bin/${bin} || true; \
        done && \
        for bin in gnb odu ocu ocucp ocuup ru_emulator; do \
            test -f /usr/local/bin/${bin} && getcap /usr/local/bin/${bin} || true; \
        done ;; \
    esac
