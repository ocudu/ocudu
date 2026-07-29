# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

# Shared helpers for docker/scripts/install_*dependencies.sh.

# Return "name=version" if PKG_VERSIONS contains an entry for the given package name,
# otherwise return the bare name. PKG_VERSIONS is a space-separated list of "name=version" pairs.
_pkg_ver() {
    local name="$1"
    local pair
    for pair in $PKG_VERSIONS; do
        case "$pair" in
            "${name}="*) echo "${pair}"; return ;;
        esac
    done
    echo "$name"
}

# Return "name==version" if PKG_VERSIONS contains an entry for the given pip package name,
# otherwise return the bare name. Uses pip's == version pin syntax.
_pip_ver() {
    local name="$1"
    local pair
    for pair in $PKG_VERSIONS; do
        case "$pair" in
            "${name}="*) echo "${name}==${pair#*=}"; return ;;
        esac
    done
    echo "$name"
}

install_ubuntu_pkgs() {
    if ((${#@} == 0)); then
        return 0
    fi

    local -a versioned_pkgs=()
    local pkg
    for pkg in "$@"; do
        versioned_pkgs+=("$(_pkg_ver "$pkg")")
    done

    apt-get update
    apt-get install -y --no-install-recommends "${versioned_pkgs[@]}"
    apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/*
}

install_arch_pkgs() {
    if ((${#@} == 0)); then
        return 0
    fi

    local -a versioned_pkgs=()
    local pkg
    for pkg in "$@"; do
        versioned_pkgs+=("$(_pkg_ver "$pkg")")
    done

    pacman -Syu --noconfirm "${versioned_pkgs[@]}"
    pacman -Scc --noconfirm
}

# Optional --enablerepo for microdnf installs (RHEL host CRB on subscribed builds).
# UBI repos are already enabled by default on ubi-minimal; see RHEL docs
# "Adding software in a minimal UBI container".
UBI10_MICRODNF_REPO_ARGS=()

# Prefer dnf when present; otherwise microdnf with RH weak-deps disable (+ optional CRB).
_set_dnf() {
    DNF=$([[ -x /usr/bin/dnf ]] && echo dnf || echo "microdnf --setopt install_weak_deps=0 ${UBI10_MICRODNF_REPO_ARGS[*]}")
}
_set_dnf

install_rhel_pkgs() {
    if ((${#@} == 0)); then
        return 0
    fi

    local -a versioned_pkgs=()
    local pkg
    for pkg in "$@"; do
        versioned_pkgs+=("$(_pkg_ver "$pkg")")
    done

    # shellcheck disable=SC2086
    ${DNF} -y install "${versioned_pkgs[@]}"
    # shellcheck disable=SC2086
    ${DNF} clean all
}

_enable_centos_repos() {
    if ! rpm -q epel-release >/dev/null 2>&1; then
        # shellcheck disable=SC2086
        ${DNF} install -y dnf-plugins-core epel-release
        # shellcheck disable=SC2086
        ${DNF} config-manager --set-enabled crb >/dev/null 2>&1 || crb enable >/dev/null 2>&1 || true
    fi
}

# True on RHEL/UBI 10 base images (registry.redhat.io/ubi10, ubi10-minimal, etc.).
is_ubi10() {
    [[ "${PLATFORM_ID:-}" == "platform:el10" ]] \
        || { [[ "${ID:-}" == "rhel" ]] && [[ "${VERSION_ID%%.*}" == "10" ]]; }
}

# Print all configured dnf/microdnf repository ids (one per line), including disabled.
_ubi10_repo_ids() {
    # shellcheck disable=SC2086
    ${DNF} repolist --all 2>/dev/null | awk 'NR > 1 { print $1 }'
}

# RHEL CodeReady Builder id when the host subscription exposes it (ubi-minimal
# pattern: microdnf install --enablerepo=codeready-builder-for-rhel-*-rpms ...).
_ubi10_rhel_crb_repo() {
    local arch
    arch=$(uname -m)
    local id="codeready-builder-for-rhel-10-${arch}-rpms"
    if _ubi10_repo_ids | grep -qx "$id"; then
        printf '%s\n' "$id"
        return 0
    fi
    return 1
}

# Prefer entitled RHEL CRB; else a UBI CRB id (full ubi10 / dnf config-manager).
_ubi10_pick_crb_repo() {
    local arch
    arch=$(uname -m)
    local -a candidates=(
        "codeready-builder-for-rhel-10-${arch}-rpms"
        "codeready-builder-for-ubi-10-${arch}-rpms"
        "ubi-10-codeready-builder-rpms"
    )
    local repos id
    repos=$(_ubi10_repo_ids)
    for id in "${candidates[@]}"; do
        if printf '%s\n' "$repos" | grep -qx "$id"; then
            printf '%s\n' "$id"
            return 0
        fi
    done
    return 1
}

# Enable CRB (and install EPEL when missing) for UBI10/RHEL 10 package installs.
# On microdnf, folds RHEL CRB into DNF via --enablerepo; on dnf, uses config-manager.
_enable_ubi10_repos() {
    local crb=""

    if [[ "${DNF%% *}" == microdnf ]]; then
        # UBI BaseOS/AppStream/CRB are enabled by default on ubi-minimal. Only
        # pass --enablerepo for a different host repo (RHEL CRB) when present.
        UBI10_MICRODNF_REPO_ARGS=()
        if crb=$(_ubi10_rhel_crb_repo); then
            UBI10_MICRODNF_REPO_ARGS=(--enablerepo="${crb}")
        fi
        _set_dnf
        # microdnf cannot install from an HTTPS URL; use rpm for the EPEL release RPM.
        if ! rpm -q epel-release >/dev/null 2>&1; then
            rpm -ivh "https://dl.fedoraproject.org/pub/epel/epel-release-latest-10.noarch.rpm" \
                2>/dev/null || true
        fi
    else
        # shellcheck disable=SC2086
        ${DNF} -y install dnf-plugins-core

        # libfdt-devel and other -devel packages live in CRB. Subscribed builds
        # expose RHEL 10 CRB; plain UBI images use a UBI CRB repo instead.
        if crb=$(_ubi10_pick_crb_repo); then
            # shellcheck disable=SC2086
            ${DNF} config-manager --enable "${crb}" 2>/dev/null || true
        fi

        if ! rpm -q epel-release >/dev/null 2>&1; then
            # shellcheck disable=SC2086
            ${DNF} -y install "https://dl.fedoraproject.org/pub/epel/epel-release-latest-10.noarch.rpm" 2>/dev/null || true
        fi
    fi
}

# Refresh installed RPMs so base-image CVEs are picked up when newer packages
# are available in enabled repos. Callers must source /etc/os-release so ${ID}
# is set.
update_rpm_pkgs() {
    # shellcheck disable=SC2086
    ${DNF} -y update
}

# Install packages via dnf (or microdnf on UBI minimal) on Fedora, CentOS Stream, and UBI10.
# Optional leading --no-repos skips CentOS/UBI10 repo enablement (base repos only).
install_rpm_pkgs() {
    local enable_repos=1
    if [[ "${1:-}" == "--no-repos" ]]; then
        enable_repos=0
        shift
    fi

    if ((${#@} == 0)); then
        return 0
    fi

    local -a versioned_pkgs=()
    local pkg
    for pkg in "$@"; do
        versioned_pkgs+=("$(_pkg_ver "$pkg")")
    done

    if ((enable_repos)); then
        case "$ID" in
            centos)
                _enable_centos_repos
                ;;
            rhel)
                _enable_ubi10_repos
                ;;
            fedora)
                ;;
            *)
                echo >&2 "OS $ID not supported by install_rpm_pkgs"
                exit 1
                ;;
        esac
    else
        # Avoid carrying CRB enablerepo into --no-repos installs on ubi-minimal.
        UBI10_MICRODNF_REPO_ARGS=()
        _set_dnf
    fi

    # shellcheck disable=SC2086
    ${DNF} -y install "${versioned_pkgs[@]}"
    # shellcheck disable=SC2086
    ${DNF} clean all
}

# Install pip packages. Optional leading "--flag" arguments are passed to pip3.
# Without extra flags, falls back to --break-system-packages on failure (Debian/Ubuntu).
install_pip_pkgs() {
    local -a pip_extra_args=()
    local -a pkgs=()

    for arg in "$@"; do
        case "$arg" in
            --*) pip_extra_args+=("$arg") ;;
            *) pkgs+=("$arg") ;;
        esac
    done

    if ((${#pkgs[@]} == 0)); then
        return 0
    fi

    local -a versioned_pip_pkgs=()
    local pkg
    for pkg in "${pkgs[@]}"; do
        versioned_pip_pkgs+=("$(_pip_ver "$pkg")")
    done

    if ((${#pip_extra_args[@]})); then
        pip3 install "${pip_extra_args[@]}" "${versioned_pip_pkgs[@]}"
    else
        pip3 install "${versioned_pip_pkgs[@]}" || pip3 install --break-system-packages "${versioned_pip_pkgs[@]}"
    fi
}
