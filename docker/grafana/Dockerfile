# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


ARG GF_VERSION=13.2.1
FROM grafana/grafana:${GF_VERSION}-distroless-slim AS grafana

FROM --platform=${BUILDPLATFORM} alpine:3.23 AS customizer
ARG LOGO_SRC=https://srs.io/wp-content/uploads/ocudu_color.png
COPY . /context
COPY --from=grafana / /rootfs
# If LOGO_SRC is a valid path inside the container, then move it to the correct location
# Otherwise, download it from the URL
RUN rm -rf /rootfs/usr/share/grafana/public/app/plugins/panel/table && \
    if [ -f "/context/${LOGO_SRC}" ]; then \
        mv "/context/${LOGO_SRC}" /rootfs/usr/share/grafana/public/img/logo.png; \
    elif [ -f "${LOGO_SRC}" ]; then \
        mv "${LOGO_SRC}" /rootfs/usr/share/grafana/public/img/logo.png; \
    else \
        wget -q -O /rootfs/usr/share/grafana/public/img/logo.png "${LOGO_SRC}"; \
    fi
COPY provisioning /rootfs/etc/grafana/provisioning
COPY dashboards /rootfs/usr/share/grafana/public/dashboards

FROM scratch
LABEL maintainer="Grafana Labs <hello@grafana.com>"
LABEL org.opencontainers.image.source="https://github.com/grafana/grafana"
ENV PATH="/usr/share/grafana/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin" \
    SSL_CERT_FILE="/etc/ssl/certs/ca-certificates.crt" \
    GF_PATHS_CONFIG="/etc/grafana/grafana.ini" \
    GF_PATHS_DATA="/var/lib/grafana" \
    GF_PATHS_HOME="/usr/share/grafana" \
    GF_PATHS_LOGS="/var/log/grafana" \
    GF_PATHS_PLUGINS="/var/lib/grafana/plugins" \
    GF_PATHS_PROVISIONING="/etc/grafana/provisioning"
COPY --from=customizer /rootfs /
EXPOSE 3000
WORKDIR /usr/share/grafana
USER 472
ENTRYPOINT ["/usr/share/grafana/bin/grafana", "server", "--homepath=/usr/share/grafana", "--config=/etc/grafana/grafana.ini", "--packaging=docker"]
CMD ["cfg:default.log.mode=console"]
