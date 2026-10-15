#!/usr/bin/env bash
set -euo pipefail

# ==============================================================================
# Script de aprovisionamiento de dependencias para la Máquina Virtual
# ==============================================================================

echo "Detectando distribución de Linux..."

if [ -f /etc/os-release ]; then
    . /etc/os-release
    DISTRO=$ID
else
    DISTRO="unknown"
fi

echo "Distribución detectada: ${DISTRO}"

case "${DISTRO}" in
    ubuntu|debian|neon|kubuntu)
        echo "Instalando paquetes en Debian/Ubuntu..."
        sudo apt update
        sudo apt install -y \
            build-essential cmake ninja-build git pkg-config \
            qt6-base-dev qt6-declarative-dev qt6-wayland-dev libqt6core6 \
            libkf6kcmutils-dev libkf6solid-dev libkf6networkmanagerqt-dev \
            libkf6bluezqt-dev libkf6pulseaudioqt-dev libkf6baloo-dev libkf6kio-dev \
            libkwin-dev liblayershellqt-dev wayland-protocols \
            libpoppler-qt6-dev libinput-dev curl
        ;;
    fedora)
        echo "Instalando paquetes en Fedora..."
        sudo dnf install -y \
            gcc-c++ cmake ninja-build git pkgconf-pkg-config \
            qt6-qtbase-devel qt6-qtdeclarative-devel qt6-qtwayland-devel \
            kf6-kcmutils-devel kf6-solid-devel kf6-networkmanager-qt-devel \
            kf6-bluez-qt-devel kf6-pulseaudio-qt-devel kf6-baloo-devel kf6-kio-devel \
            kwin-devel layer-shell-qt-devel wayland-protocols-devel \
            poppler-qt6-devel libinput-devel curl
        ;;
    arch|manjaro|endeavouros)
        echo "Instalando paquetes en Arch Linux..."
        sudo pacman -S --needed --noconfirm \
            base-devel cmake ninja git pkgconf \
            qt6-base qt6-declarative qt6-wayland \
            kcmutils solid networkmanager-qt bluez-qt pulseaudio-qt baloo kio \
            kwin layer-shell-qt wayland-protocols poppler-qt6 libinput curl
        ;;
    *)
        echo "Distribución no soportada automáticamente: ${DISTRO}"
        echo "Por favor instala manualmente: CMake, Ninja, Qt6, KDE Frameworks 6, KWin y Rust."
        exit 1
        ;;
esac

# Instalación de Rust si no está presente
if ! command -v cargo &>/dev/null; then
    echo "Instalando toolchain de Rust vía rustup..."
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
    source "$HOME/.cargo/env"
fi

echo "=========================================================="
echo " Dependencias de VM instaladas correctamente.             "
echo " Ya puedes compilar e instalar ejecutando:                "
echo " ./scripts/build_and_install.sh --vm-install --prefix /usr"
echo "=========================================================="
