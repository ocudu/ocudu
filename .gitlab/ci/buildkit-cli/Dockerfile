# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

ARG UBUNTU_VERSION=24.04
ARG COSIGN_VERSION=v2.6.4
ARG REGCLIENT_VERSION=v0.11.5
ARG SYFT_VERSION=v1.48.0
ARG GRYPE_VERSION=v0.115.0

FROM ubuntu:${UBUNTU_VERSION} AS builder
ARG BUILDKIT_CLI_VERSION=0.31.2

RUN export DEBIAN_FRONTEND=noninteractive && \
    apt update && apt dist-upgrade -y && \
    apt install -y wget nano unzip ca-certificates curl gnupg lsb-release && \
    cd /root && \
    ARCH=`uname -m` && \
    if [ "$ARCH" = "aarch64" ]; then \
        echo "Building arm64 container." && \
        wget "https://awscli.amazonaws.com/awscli-exe-linux-aarch64.zip" && \
        wget https://github.com/moby/buildkit/releases/download/v${BUILDKIT_CLI_VERSION}/buildkit-v${BUILDKIT_CLI_VERSION}.linux-arm64.tar.gz && \
        unzip awscli-exe-linux-aarch64.zip && \
        tar xvf buildkit-v${BUILDKIT_CLI_VERSION}.linux-arm64.tar.gz; \
    else \
        echo "Building amd64 container." && \
        wget "https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip" && \
        wget https://github.com/moby/buildkit/releases/download/v${BUILDKIT_CLI_VERSION}/buildkit-v${BUILDKIT_CLI_VERSION}.linux-amd64.tar.gz && \
        unzip awscli-exe-linux-x86_64.zip && \
        tar xvf buildkit-v${BUILDKIT_CLI_VERSION}.linux-amd64.tar.gz; \
    fi

FROM ghcr.io/sigstore/cosign/cosign:${COSIGN_VERSION} AS cosign
FROM ghcr.io/regclient/regctl:${REGCLIENT_VERSION} AS regclient
FROM anchore/syft:${SYFT_VERSION} AS syft
FROM anchore/grype:${GRYPE_VERSION} AS grype

FROM ubuntu:${UBUNTU_VERSION}

COPY --from=builder    /root/aws       /root/aws
COPY --from=builder    /root/bin/*     /usr/local/bin
COPY --from=cosign     /ko-app/cosign  /usr/local/bin/cosign
COPY --from=regclient  /regctl         /usr/local/bin/regctl
COPY --from=syft       /syft           /usr/local/bin/syft
COPY --from=grype      /grype          /usr/local/bin/grype

COPY buildctl-daemonless.sh /usr/local/bin

RUN export DEBIAN_FRONTEND=noninteractive && \
    apt update && apt dist-upgrade -y && \
    apt install -y ca-certificates curl gnupg lsb-release jq && \
    /root/aws/install && \
    mkdir -p /etc/apt/keyrings /root/.aws /root/.docker && \
    curl -fsSL https://download.docker.com/linux/ubuntu/gpg | gpg --dearmor -o /etc/apt/keyrings/docker.gpg && \
    echo \
        "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
        $(lsb_release -cs) stable" | tee /etc/apt/sources.list.d/docker.list > /dev/null && \
    apt update && \
    apt install -y docker-ce docker-ce-cli containerd.io docker-compose-plugin && \
    apt-get autoremove && apt-get clean && rm -rf /var/lib/apt/lists/*
