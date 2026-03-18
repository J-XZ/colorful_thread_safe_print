#!/usr/bin/env bash

set -euo pipefail

log() {
    printf '[install_deps] %s\n' "$*"
}

has_cmd() {
    command -v "$1" >/dev/null 2>&1
}

need_sudo() {
    [[ "${EUID}" -ne 0 ]]
}

run_pkg_install() {
    if need_sudo; then
        sudo "$@"
    else
        "$@"
    fi
}

have_cxx_compiler() {
    has_cmd c++ || has_cmd g++ || has_cmd clang++
}

have_gtest_config() {
    if [[ "$(uname -s)" == "Darwin" ]] && has_cmd brew; then
        if brew list --versions googletest >/dev/null 2>&1; then
            return 0
        fi
    fi

    local search_roots=(
        /opt/homebrew/opt/googletest
        /usr/local/opt/googletest
        /usr/lib
        /usr/lib64
        /usr/local/lib
        /usr/local/lib64
        /usr/lib/x86_64-linux-gnu
        /usr/lib/aarch64-linux-gnu
    )

    for root in "${search_roots[@]}"; do
        if [[ -f "${root}/cmake/GTest/GTestConfig.cmake" ]] || [[ -f "${root}/GTest/GTestConfig.cmake" ]]; then
            return 0
        fi
    done

    return 1
}

install_with_brew() {
    local missing=()

    has_cmd cmake || missing+=(cmake)
    has_cmd ninja || missing+=(ninja)
    have_cxx_compiler || missing+=(llvm)

    if ! have_gtest_config; then
        missing+=(googletest)
    fi

    if ((${#missing[@]} == 0)); then
        log "All required dependencies are already installed."
        return 0
    fi

    log "Installing with Homebrew: ${missing[*]}"
    brew install "${missing[@]}"
}

install_with_apt() {
    local packages=()

    has_cmd cmake || packages+=(cmake)
    has_cmd ninja || packages+=(ninja-build)
    have_cxx_compiler || packages+=(build-essential)
    have_gtest_config || packages+=(libgtest-dev)

    if ((${#packages[@]} == 0)); then
        log "All required dependencies are already installed."
        return 0
    fi

    log "Updating apt package index"
    run_pkg_install apt-get update
    log "Installing with apt: ${packages[*]}"
    run_pkg_install apt-get install -y "${packages[@]}"
}

install_with_dnf() {
    local packages=()

    has_cmd cmake || packages+=(cmake)
    has_cmd ninja || packages+=(ninja-build)
    have_cxx_compiler || packages+=(gcc-c++)
    have_gtest_config || packages+=(gtest-devel)

    if ((${#packages[@]} == 0)); then
        log "All required dependencies are already installed."
        return 0
    fi

    log "Installing with dnf: ${packages[*]}"
    run_pkg_install dnf install -y "${packages[@]}"
}

install_with_yum() {
    local packages=()

    has_cmd cmake || packages+=(cmake)
    has_cmd ninja || packages+=(ninja-build)
    have_cxx_compiler || packages+=(gcc-c++)
    have_gtest_config || packages+=(gtest-devel)

    if ((${#packages[@]} == 0)); then
        log "All required dependencies are already installed."
        return 0
    fi

    log "Installing with yum: ${packages[*]}"
    run_pkg_install yum install -y "${packages[@]}"
}

install_with_pacman() {
    local packages=()

    has_cmd cmake || packages+=(cmake)
    has_cmd ninja || packages+=(ninja)
    have_cxx_compiler || packages+=(gcc)
    have_gtest_config || packages+=(gtest)

    if ((${#packages[@]} == 0)); then
        log "All required dependencies are already installed."
        return 0
    fi

    log "Refreshing pacman package database"
    run_pkg_install pacman -Sy --noconfirm
    log "Installing with pacman: ${packages[*]}"
    run_pkg_install pacman -S --noconfirm "${packages[@]}"
}

install_with_zypper() {
    local packages=()

    has_cmd cmake || packages+=(cmake)
    has_cmd ninja || packages+=(ninja)
    have_cxx_compiler || packages+=(gcc-c++)
    have_gtest_config || packages+=(gtest)

    if ((${#packages[@]} == 0)); then
        log "All required dependencies are already installed."
        return 0
    fi

    log "Installing with zypper: ${packages[*]}"
    run_pkg_install zypper install -y "${packages[@]}"
}

main() {
    case "$(uname -s)" in
        Darwin)
            if ! has_cmd brew; then
                log "Homebrew is required on macOS but was not found."
                log "Install Homebrew first: https://brew.sh/"
                exit 1
            fi
            install_with_brew
            ;;
        Linux)
            if has_cmd apt-get; then
                install_with_apt
            elif has_cmd dnf; then
                install_with_dnf
            elif has_cmd yum; then
                install_with_yum
            elif has_cmd pacman; then
                install_with_pacman
            elif has_cmd zypper; then
                install_with_zypper
            else
                log "Unsupported Linux distribution: no supported package manager found."
                exit 1
            fi
            ;;
        *)
            log "Unsupported operating system: $(uname -s)"
            exit 1
            ;;
    esac

    log "Dependency check complete."
}

main "$@"
