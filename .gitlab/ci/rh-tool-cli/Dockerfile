# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

ARG BUILDKIT_CLI_VERSION=0.31.2
ARG PREFLIGHT_VERSION=1.19.1
ARG COSIGN_VERSION=v2.6.4
ARG REGCLIENT_VERSION=v0.11.5
ARG SYFT_VERSION=v1.48.0
ARG GRYPE_VERSION=v0.115.0

FROM moby/buildkit:v${BUILDKIT_CLI_VERSION} AS buildkit
FROM docker:29.6.1 AS docker-cli
FROM ghcr.io/sigstore/cosign/cosign:${COSIGN_VERSION} AS cosign
FROM ghcr.io/regclient/regctl:${REGCLIENT_VERSION} AS regclient
FROM anchore/syft:${SYFT_VERSION} AS syft
FROM anchore/grype:${GRYPE_VERSION} AS grype

FROM registry.access.redhat.com/ubi9/ubi-minimal

ARG BUILDKIT_CLI_VERSION
ARG PREFLIGHT_VERSION

RUN microdnf install -y ca-certificates git jq && \
    microdnf clean all

# BuildKit client + daemon (daemonless script needs buildkitd + runc OCI worker)
COPY --from=buildkit /usr/bin/buildctl /usr/local/bin/buildctl
COPY --from=buildkit /usr/bin/buildkitd /usr/local/bin/buildkitd
COPY --from=buildkit /usr/bin/buildkit-runc /usr/local/bin/runc

# Docker CLI
COPY --from=docker-cli /usr/local/bin/docker /usr/local/bin/docker

# Signing, attestation, and CVE scanning tools
COPY --from=cosign   /ko-app/cosign /usr/local/bin/cosign
COPY --from=regclient /regctl       /usr/local/bin/regctl
COPY --from=syft     /syft          /usr/local/bin/syft
COPY --from=grype    /grype         /usr/local/bin/grype

# buildctl-daemonless wrapper (spawns ephemeral buildkitd)
COPY buildctl-daemonless.sh /usr/local/bin/buildctl-daemonless.sh
RUN chmod +x /usr/local/bin/buildctl-daemonless.sh

# preflight — arch-aware download
RUN ARCH=$(uname -m) && \
    case "$ARCH" in \
        x86_64)  PFLT_ARCH=amd64 ;; \
        aarch64) PFLT_ARCH=arm64 ;; \
        *) echo "Unsupported arch: $ARCH" && exit 1 ;; \
    esac && \
    curl -fsSL \
      "https://github.com/redhat-openshift-ecosystem/openshift-preflight/releases/download/${PREFLIGHT_VERSION}/preflight-linux-${PFLT_ARCH}" \
      -o /usr/local/bin/preflight && \
    chmod +x /usr/local/bin/preflight

RUN mkdir -p /root/.docker
