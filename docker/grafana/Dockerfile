# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


ARG GF_VERSION=13.0.2
FROM grafana/grafana-oss:${GF_VERSION}

ADD provisioning /etc/grafana/provisioning
ADD dashboards   /usr/share/grafana/public/dashboards

USER root
RUN rm -rf /usr/share/grafana/public/app/plugins/panel/table
ARG LOGO_SRC=https://srs.io/wp-content/uploads/ocudu_color.png
# If LOGO_SRC is a valid path inside the container, then move it to the correct location
# Otherwise, download it from the URL
RUN if [ -f "${LOGO_SRC}" ]; then \
        mv "${LOGO_SRC}" /usr/share/grafana/public/img/logo.png; \
    else \
        wget -O /usr/share/grafana/public/img/logo.png ${LOGO_SRC}; \
    fi
USER grafana
