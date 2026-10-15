#!/usr/bin/env bash
set -euo pipefail

# ==============================================================================
# Script de aprovisionamiento de dependencias para la Máquina Virtual
# ==============================================================================
# NOTA: Este script instala dependencias del sistema y debe ejecutarse
# EXCLUSIVAMENTE dentro de una Máquina Virtual (VM) de desarrollo/pruebas.
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
        echo "=========================================================="
        echo "Aprovisionando repositorios y dependencias en Debian/Ubuntu"
        echo "=========================================================="

        # 1. Habilitar repositorio universe (requerido para kwin-dev y liblayershellqtinterface-dev)
        echo "[1/5] Comprobando repositorio universe..."
        if command -v add-apt-repository &>/dev/null; then
            sudo add-apt-repository -y universe || true
        else
            echo "Instalando software-properties-common para gestionar repositorios..."
            sudo apt update
            sudo apt install -y software-properties-common
            sudo add-apt-repository -y universe || true
        fi

        echo "Actualizando índices de paquetes (apt update)..."
        sudo apt update

        # 2. Herramientas base de compilación
        echo "[2/5] Instalando herramientas de compilación base..."
        sudo apt install -y \
            build-essential cmake ninja-build git pkg-config curl libinput-dev wayland-protocols

        # 3. SDK de Qt6
        echo "[3/5] Instalando SDK de Qt6..."
        sudo apt install -y \
            qt6-base-dev qt6-declarative-dev qt6-wayland-dev libpoppler-qt6-dev

        # 4. KWin y LayerShellQt (nombres oficiales en Ubuntu/Debian)
        echo "[4/5] Instalando cabeceras de KWin y LayerShellQt..."
        sudo apt install -y \
            kwin-dev liblayershellqtinterface-dev || {
            echo "Aviso: No se pudo instalar kwin-dev o liblayershellqtinterface-dev directamente."
            echo "Comprobando alternativas en repositorios de KDE..."
            sudo apt install -y kwin-wayland-dev || true
        }

        # 5. KDE Frameworks 6 (KF6)
        echo "[5/5] Instalando librerías de KDE Frameworks 6..."
        sudo apt install -y \
            libkf6kcmutils-dev libkf6solid-dev libkf6networkmanagerqt-dev \
            libkf6bluezqt-dev libkf6pulseaudioqt-dev libkf6baloo-dev libkf6kio-dev || {
            echo "Aviso: Algunos paquetes opcionales de KF6 no están disponibles en esta rama. Continuando..."
        }
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
    source "$HOME/.cargo/env" || true
fi

echo ""
echo "=========================================================="
echo " Dependencias de VM aprovisionadas correctamente.         "
echo " Ya puedes compilar e instalar ejecutando:                "
echo " ./scripts/build_and_install.sh --vm-install --prefix /usr"
echo "=========================================================="
