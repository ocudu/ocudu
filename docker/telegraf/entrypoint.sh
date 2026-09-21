#!/usr/bin/env bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set -o pipefail

if [ -z "$INFLUXDB3_EXTERNAL_URL" ]; then
  # Stay up (so the container remains "ready") without ever starting telegraf, so a
  # misconfigured/absent InfluxDB destination produces no connection-retry log spam.
  echo "$(date -u '+%Y-%m-%dT%H:%M:%SZ') W! INFLUXDB3_EXTERNAL_URL is empty, not starting telegraf" >&2
  sleep infinity &
else
  telegraf --non-strict-env-handling --config /etc/ocudu/telegraf.conf $TELEGRAF_CLI_EXTRA_ARGS &
fi
child=$!

health_code=0
_term() {
    curl -sf -o /dev/null http://localhost:9273/health
    health_code=$?
    echo "$(date -u '+%Y-%m-%dT%H:%M:%SZ') I! Health check returned code $health_code"
    kill -TERM "$child" 2>/dev/null
}
trap _term SIGTERM SIGINT

wait "$child"

exit $health_code
