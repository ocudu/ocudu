#!/bin/sh

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#
# Returns the number of CPU cores effectively available to this job.
# Works for host/VM, Docker executor and Kubernetes executor (cgroup v1/v2).
#

set -eu

first_existing_file() {
  for path in "$@"; do
    if [ -r "${path}" ]; then
      echo "${path}"
      return 0
    fi
  done
  return 1
}

resolve_cgroup_v2_path() {
  # Returns the cgroup v2 relative path for the current process.
  # Example line: 0::/kubepods.slice/kubepods-burstable.slice/...
  if [ ! -r /proc/self/cgroup ]; then
    return 1
  fi
  awk -F: '$1 == "0" {print $3; exit 0}' /proc/self/cgroup
}

resolve_cgroup_v1_path() {
  controller="$1"
  if [ ! -r /proc/self/cgroup ]; then
    return 1
  fi
  awk -F: -v ctrl="${controller}" '
    $2 ~ "(^|,)"ctrl"(,|$)" {print $3; exit 0}
  ' /proc/self/cgroup
}

read_cgroup_v2_value() {
  file_name="$1"
  rel_path="$(resolve_cgroup_v2_path || true)"
  [ -n "${rel_path}" ] || return 1
  file="/sys/fs/cgroup${rel_path}/${file_name}"
  [ -r "${file}" ] || return 1
  cat "${file}"
}

read_cgroup_v1_value() {
  controller="$1"
  file_name="$2"
  rel_path="$(resolve_cgroup_v1_path "${controller}" || true)"
  [ -n "${rel_path}" ] || return 1
  file="/sys/fs/cgroup/${controller}${rel_path}/${file_name}"
  [ -r "${file}" ] || return 1
  cat "${file}"
}

count_cpuset_cpus() {
  cpuset="$1"
  [ -n "${cpuset}" ] || return 1

  old_ifs="${IFS}"
  IFS=","
  set -- ${cpuset}
  IFS="${old_ifs}"

  total=0
  for part in "$@"; do
    case "${part}" in
      *-*)
        start="${part%-*}"
        end="${part#*-}"
        case "${start}${end}" in
          ''|*[!0-9]*)
            continue
            ;;
        esac
        if [ "${end}" -ge "${start}" ]; then
          total=$((total + end - start + 1))
        fi
        ;;
      *)
        case "${part}" in
          ''|*[!0-9]*)
            continue
            ;;
        esac
        total=$((total + 1))
        ;;
    esac
  done

  [ "${total}" -gt 0 ] || return 1
  echo "${total}"
}

read_cpuset_limit() {
  # Prefer cgroup v2 path for this process, fall back to global files.
  if cpuset_value="$(read_cgroup_v2_value cpuset.cpus.effective 2>/dev/null)"; then
    cpuset_value="$(printf '%s' "${cpuset_value}" | tr -d ' \n\t')"
    count_cpuset_cpus "${cpuset_value}"
    return 0
  fi
  if cpuset_value="$(read_cgroup_v2_value cpuset.cpus 2>/dev/null)"; then
    cpuset_value="$(printf '%s' "${cpuset_value}" | tr -d ' \n\t')"
    count_cpuset_cpus "${cpuset_value}"
    return 0
  fi

  if cpuset_value="$(read_cgroup_v1_value cpuset cpuset.cpus 2>/dev/null)"; then
    cpuset_value="$(printf '%s' "${cpuset_value}" | tr -d ' \n\t')"
    count_cpuset_cpus "${cpuset_value}"
    return 0
  fi

  cpuset_file="$(first_existing_file \
    /sys/fs/cgroup/cpuset.cpus.effective \
    /sys/fs/cgroup/cpuset.cpus \
    /sys/fs/cgroup/cpuset/cpuset.cpus \
  )" || return 1

  cpuset_value="$(tr -d ' \n\t' < "${cpuset_file}")"
  count_cpuset_cpus "${cpuset_value}"
}

ceil_div() {
  num="$1"
  den="$2"
  [ "${den}" -gt 0 ] || return 1
  echo $(((num + den - 1) / den))
}

read_quota_limit() {
  # cgroup v2
  if cpu_max="$(read_cgroup_v2_value cpu.max 2>/dev/null)"; then
    set -- ${cpu_max}
    quota="$1"
    period="$2"
    if [ "${quota}" != "max" ] && [ -n "${quota}" ] && [ -n "${period}" ]; then
      case "${quota}${period}" in
        *[!0-9]*)
          :
          ;;
        *)
          if [ "${quota}" -gt 0 ] && [ "${period}" -gt 0 ]; then
            ceil_div "${quota}" "${period}"
            return 0
          fi
          ;;
      esac
    fi
  fi

  # cgroup v1
  if quota="$(read_cgroup_v1_value cpu cpu.cfs_quota_us 2>/dev/null)" \
    && period="$(read_cgroup_v1_value cpu cpu.cfs_period_us 2>/dev/null)"; then
    case "${quota}${period}" in
      *[!0-9-]*)
        return 1
        ;;
    esac
    if [ "${quota}" -gt 0 ] && [ "${period}" -gt 0 ]; then
      ceil_div "${quota}" "${period}"
      return 0
    fi
  fi

  return 1
}

read_host_cores() {
  cores="$(getconf _NPROCESSORS_ONLN 2>/dev/null || true)"
  if [ -z "${cores}" ] || [ "${cores}" -le 0 ] 2>/dev/null; then
    cores="$(nproc 2>/dev/null || true)"
  fi
  if [ -z "${cores}" ] || [ "${cores}" -le 0 ] 2>/dev/null; then
    cores=1
  fi
  echo "${cores}"
}

min_positive() {
  current="$1"
  candidate="$2"
  [ -n "${candidate}" ] || { echo "${current}"; return 0; }
  case "${candidate}" in
    *[!0-9]*)
      echo "${current}"
      return 0
      ;;
  esac
  if [ "${candidate}" -le 0 ]; then
    echo "${current}"
    return 0
  fi
  if [ "${current}" -le 0 ] || [ "${candidate}" -lt "${current}" ]; then
    echo "${candidate}"
  else
    echo "${current}"
  fi
}

host_cores="$(read_host_cores)"
effective_cores="${host_cores}"

if cpuset_cores="$(read_cpuset_limit 2>/dev/null)"; then
  effective_cores="$(min_positive "${effective_cores}" "${cpuset_cores}")"
fi

if quota_cores="$(read_quota_limit 2>/dev/null)"; then
  effective_cores="$(min_positive "${effective_cores}" "${quota_cores}")"
fi

if [ "${effective_cores}" -le 0 ] 2>/dev/null; then
  effective_cores=1
fi

echo "${effective_cores}"
