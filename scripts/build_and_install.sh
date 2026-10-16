#!/usr/bin/env bash
set -euo pipefail

# ==============================================================================
# QuartzDE (WhiteQuartz) - Script Unificado de Compilación y Despliegue para VM
# ==============================================================================
# NOTA DE SEGURIDAD:
# Por defecto este script SOLO COMPILA (--build-only).
# Bajo ninguna circunstancia instalará nada en el sistema anfitrión.
# La instalación solo se activa con el flag explícito: --vm-install
# ==============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

DEFAULT_PREFIX="/usr/local"
BUILD_DIR="${PROJECT_ROOT}/build"
RUST_TARGET="${PROJECT_ROOT}/target/release"

MODE="build_only"
INSTALL_PREFIX="${DEFAULT_PREFIX}"
RUN_TESTS=1
CLEAN_BUILD=0
PARALLEL_JOBS=""

# Paleta de colores ANSI
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

log_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[OK]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[AVISO]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1" >&2; }

usage() {
    cat <<EOF
Uso: $0 [OPCIONES]

Opciones:
  --build-only        Compila el proyecto sin instalarlo en el sistema (MODO SEGURO - Por defecto).
  --vm-install        Ejecuta la fase de instalación. SOLO USAR DENTRO DE UNA MÁQUINA VIRTUAL.
  --prefix RUTA       Directorio base de instalación (por defecto: ${DEFAULT_PREFIX}).
  -j, --jobs N        Número de hilos de compilación simultáneos (por defecto: auto-calculado por RAM).
  --skip-tests        Omite la ejecución de las suites de pruebas unitarias.
  --clean             Limpia los directorios de compilación previos (target/ y build/).
  -h, --help          Muestra este manual de uso.

Ejemplos:
  1. Compilar de forma segura en tu equipo de desarrollo (sin instalar nada):
     $0 --build-only

  2. Compilar e instalar dentro de una Máquina Virtual limpia:
     $0 --vm-install --prefix /usr

  3. Forzar 1 o 2 hilos para entornos con poca memoria RAM:
     $0 --vm-install --prefix /usr -j 1
EOF
    exit 0
}

# Parseo de argumentos
while [[ $# -gt 0 ]]; do
    case "$1" in
        --build-only)
            MODE="build_only"
            shift
            ;;
        --vm-install)
            MODE="vm_install"
            shift
            ;;
        --prefix)
            INSTALL_PREFIX="$2"
            shift 2
            ;;
        -j|--jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        --skip-tests)
            RUN_TESTS=0
            shift
            ;;
        --clean)
            CLEAN_BUILD=1
            shift
            ;;
        -h|--help)
            usage
            ;;
        *)
            log_error "Argumento no reconocido: $1"
            usage
            ;;
    esac
done

echo -e "${CYAN}======================================================${NC}"
echo -e "${CYAN}        QuartzDE Build & Deployment System           ${NC}"
echo -e "${CYAN}======================================================${NC}"
log_info "Raíz del proyecto:  ${PROJECT_ROOT}"
log_info "Modo de operación:  ${MODE}"
log_info "Prefijo de destino: ${INSTALL_PREFIX}"
echo ""

# Protección de seguridad obligatoria contra instalación en el host
if [[ "${MODE}" == "vm_install" ]]; then
    log_warn "ATENCIÓN: Se ha solicitado el modo de instalación (--vm-install)."
    log_warn "Este modo modificará archivos en el sistema operativo (${INSTALL_PREFIX})."
    log_warn "ESTO DEBE EJECUTARSE EXCLUSIVAMENTE DENTRO DE UNA MÁQUINA VIRTUAL."
    read -p "¿Confirmas que estás dentro de una Máquina Virtual y deseas proceder? [s/N]: " confirm
    if [[ "${confirm}" != "s" && "${confirm}" != "S" ]]; then
        log_warn "Instalación abortada por seguridad. Nada fue modificado."
        exit 0
    fi
fi

# Limpieza previa si fue solicitada
if [[ ${CLEAN_BUILD} -eq 1 ]]; then
    log_info "Limpiando artefactos previos..."
    cargo clean --manifest-path "${PROJECT_ROOT}/Cargo.toml" || true
    rm -rf "${BUILD_DIR}"
fi

# 1. Verificación de herramientas básicas
log_info "Comprobando herramientas de compilación..."
command -v cargo >/dev/null 2>&1 || { log_error "Cargo/Rust no está instalado."; exit 1; }
command -v cmake >/dev/null 2>&1 || { log_error "CMake no está instalado."; exit 1; }
command -v ninja >/dev/null 2>&1 || { log_error "Ninja no está instalado."; exit 1; }

# Detección inteligente de memoria y concurrencia para prevenir fallos por OOM Killer (cc1plus killed)
TOTAL_MEM_MB=0
SWAP_TOTAL_MB=0
if [[ -f /proc/meminfo ]]; then
    TOTAL_MEM_KB=$(grep MemTotal /proc/meminfo | awk '{print $2}')
    TOTAL_MEM_MB=$(( TOTAL_MEM_KB / 1024 ))
    SWAP_TOTAL_KB=$(grep SwapTotal /proc/meminfo | awk '{print $2}')
    SWAP_TOTAL_MB=$(( SWAP_TOTAL_KB / 1024 ))
fi

CPU_CORES=$(nproc 2>/dev/null || echo 1)

if [[ -z "${PARALLEL_JOBS}" ]]; then
    if [[ ${TOTAL_MEM_MB} -gt 0 ]]; then
        # Reservar ~2048 MB por hilo de compilación C++20 pesado para evitar que el OOM Killer termine cc1plus
        MEM_JOBS=$(( TOTAL_MEM_MB / 2048 ))
        [[ ${MEM_JOBS} -lt 1 ]] && MEM_JOBS=1
        if [[ ${MEM_JOBS} -lt ${CPU_CORES} ]]; then
            PARALLEL_JOBS=${MEM_JOBS}
        else
            PARALLEL_JOBS=${CPU_CORES}
        fi
    else
        PARALLEL_JOBS=${CPU_CORES}
    fi
    log_info "Concurrencia calculada automáticamente según RAM (${TOTAL_MEM_MB} MB) y CPUs (${CPU_CORES}): ${PARALLEL_JOBS} hilos"
else
    log_info "Concurrencia configurada manualmente: ${PARALLEL_JOBS} hilos"
fi

if [[ ${SWAP_TOTAL_MB} -eq 0 && ${TOTAL_MEM_MB} -lt 4096 ]]; then
    log_warn "ATENCIÓN: Tu sistema tiene menos de 4GB de RAM y no tiene espacio Swap configurado."
    log_warn "Se recomienda encarecidamente activar al menos 2GB-4GB de swap para evitar OOM Killer:"
    log_warn "  sudo fallocate -l 4G /swapfile && sudo chmod 600 /swapfile && sudo mkswap /swapfile && sudo swapon /swapfile"
fi

# 2. Compilación de Crates en Rust
log_info "[1/3] Compilando workspace en Rust (Cargo)..."
cargo build --release --workspace --manifest-path "${PROJECT_ROOT}/Cargo.toml" --jobs "${PARALLEL_JOBS}"
log_success "Workspace de Rust compilado exitosamente."

if [[ ${RUN_TESTS} -eq 1 ]]; then
    log_info "Ejecutando tests de Rust..."
    cargo test --release --workspace --manifest-path "${PROJECT_ROOT}/Cargo.toml"
    log_success "Tests de Rust aprobados."
fi

# 3. Compilación de componentes C++/Qt6 (CMake + Ninja)
log_info "[2/3] Configurando y compilando componentes C++/Qt6 (CMake)..."

# Saneamiento automático de caché foránea (si el directorio build fue copiado de otra máquina o ruta)
if [[ -f "${BUILD_DIR}/CMakeCache.txt" ]]; then
    if ! grep -q "CMAKE_CACHEFILE_DIR:INTERNAL=${BUILD_DIR}" "${BUILD_DIR}/CMakeCache.txt" 2>/dev/null; then
        log_warn "Detectada caché previa de CMake generada en otra ruta o equipo."
        log_warn "Purgando ${BUILD_DIR} para reconfigurar limpiamente..."
        rm -rf "${BUILD_DIR}"
    fi
fi

cmake -B "${BUILD_DIR}" -S "${PROJECT_ROOT}" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}"

ninja -C "${BUILD_DIR}" -j "${PARALLEL_JOBS}"
log_success "Componentes C++/Qt6 compilados exitosamente."

if [[ ${RUN_TESTS} -eq 1 ]]; then
    log_info "Ejecutando tests de C++ (ctest)..."
    ctest --test-dir "${BUILD_DIR}" --output-on-failure
    log_success "Tests de C++ aprobados."
fi

# 4. Fase de instalación protegida (SOLO --vm-install)
if [[ "${MODE}" == "vm_install" ]]; then
    log_info "[3/3] Instalando en la Máquina Virtual (${INSTALL_PREFIX})..."

    # Instalación de componentes C++/Qt mediante CMake
    if [[ -w "${INSTALL_PREFIX}" ]]; then
        ninja -C "${BUILD_DIR}" -j "${PARALLEL_JOBS}" install
    else
        log_info "Se requieren privilegios sudo para escribir en ${INSTALL_PREFIX}..."
        sudo ninja -C "${BUILD_DIR}" -j "${PARALLEL_JOBS}" install
    fi

    # Instalación de binarios compilados en Rust
    BIN_DIR="${INSTALL_PREFIX}/bin"
    log_info "Instalando binarios Rust en ${BIN_DIR}..."
    INSTALL_CMD="install -m 755"
    if [[ ! -w "${BIN_DIR}" ]]; then
        INSTALL_CMD="sudo ${INSTALL_CMD}"
    fi

    ${INSTALL_CMD} "${RUST_TARGET}/startquartz" "${BIN_DIR}/"
    ${INSTALL_CMD} "${RUST_TARGET}/quartz-quicklookd" "${BIN_DIR}/"
    ${INSTALL_CMD} "${RUST_TARGET}/quartz-spotlight" "${BIN_DIR}/"

    # Instalación de descriptores de sesión Wayland
    SESSION_DIR="${INSTALL_PREFIX}/share/wayland-sessions"
    log_info "Instalando sesión Wayland en ${SESSION_DIR}..."
    if [[ -d "${PROJECT_ROOT}/data/sessions" ]]; then
        SUDO_PREFIX=""
        [[ ! -w "${SESSION_DIR}" ]] && SUDO_PREFIX="sudo"
        ${SUDO_PREFIX} mkdir -p "${SESSION_DIR}"
        TMP_DESKTOP=$(mktemp)
        sed -e "s|^Exec=.*|Exec=${BIN_DIR}/startquartz|" \
            -e "s|^TryExec=.*|TryExec=${BIN_DIR}/startquartz|" \
            "${PROJECT_ROOT}/data/sessions/quartz.desktop" > "${TMP_DESKTOP}"
        ${SUDO_PREFIX} install -m 644 "${TMP_DESKTOP}" "${SESSION_DIR}/quartz.desktop"
        rm -f "${TMP_DESKTOP}"
    fi

    # Instalación de lanzadores y descriptores de aplicaciones privilegiadas
    APPS_DIR="${INSTALL_PREFIX}/share/applications"
    if [[ -d "${PROJECT_ROOT}/data/applications" ]]; then
        SUDO_PREFIX=""
        [[ ! -w "${APPS_DIR}" ]] && SUDO_PREFIX="sudo"
        ${SUDO_PREFIX} mkdir -p "${APPS_DIR}"
        ${SUDO_PREFIX} cp -r "${PROJECT_ROOT}/data/applications/"* "${APPS_DIR}/"
    fi

    # Instalación de perfiles y esquemas de color de Terminal (Konsole)
    KONSOLE_DIR="${INSTALL_PREFIX}/share/konsole"
    if [[ -d "${PROJECT_ROOT}/data/terminal" ]]; then
        SUDO_PREFIX=""
        [[ ! -w "${KONSOLE_DIR}" ]] && SUDO_PREFIX="sudo"
        ${SUDO_PREFIX} mkdir -p "${KONSOLE_DIR}"
        ${SUDO_PREFIX} cp -r "${PROJECT_ROOT}/data/terminal/"* "${KONSOLE_DIR}/"
    fi

    # Instalación de temas visuales (Colloid y Orchis)
    ICONS_DIR="${INSTALL_PREFIX}/share/icons"
    COLOR_SCHEMES_DIR="${INSTALL_PREFIX}/share/color-schemes"
    SUDO_PREFIX=""
    [[ ! -w "${INSTALL_PREFIX}" ]] && SUDO_PREFIX="sudo"

    if [[ -d "${PROJECT_ROOT}/themes/Colloid-icon-theme" ]]; then
        log_info "Instalando cursores e iconos Colloid en ${ICONS_DIR}..."
        ${SUDO_PREFIX} mkdir -p "${ICONS_DIR}"
        if [[ -d "${PROJECT_ROOT}/themes/Colloid-icon-theme/cursors/dist" ]]; then
            ${SUDO_PREFIX} cp -r "${PROJECT_ROOT}/themes/Colloid-icon-theme/cursors/dist" "${ICONS_DIR}/Colloid-cursors"
        fi
        if [[ -f "${PROJECT_ROOT}/themes/Colloid-icon-theme/install.sh" ]]; then
            ${SUDO_PREFIX} bash "${PROJECT_ROOT}/themes/Colloid-icon-theme/install.sh" -d "${ICONS_DIR}" 2>/dev/null || true
        fi
    fi

    if [[ -d "${PROJECT_ROOT}/themes/Orchis-kde/color-schemes" ]]; then
        log_info "Instalando esquemas de color Orchis en ${COLOR_SCHEMES_DIR}..."
        ${SUDO_PREFIX} mkdir -p "${COLOR_SCHEMES_DIR}"
        ${SUDO_PREFIX} cp -r "${PROJECT_ROOT}/themes/Orchis-kde/color-schemes/"* "${COLOR_SCHEMES_DIR}/"
    fi

    # Instalación de paquete Look-and-Feel (kscreenlocker) y tema SDDM
    LOOKANDFEEL_DIR="${INSTALL_PREFIX}/share/plasma/look-and-feel"
    SDDM_THEMES_DIR="${INSTALL_PREFIX}/share/sddm/themes"

    if [[ -d "${PROJECT_ROOT}/data/lookandfeel/org.quartz.desktop" ]]; then
        log_info "Instalando paquete Look-and-Feel de bloqueo en ${LOOKANDFEEL_DIR}..."
        ${SUDO_PREFIX} mkdir -p "${LOOKANDFEEL_DIR}"
        ${SUDO_PREFIX} cp -r "${PROJECT_ROOT}/data/lookandfeel/org.quartz.desktop" "${LOOKANDFEEL_DIR}/"
    fi

    if [[ -d "${PROJECT_ROOT}/data/sddm/themes/quartz" ]]; then
        log_info "Instalando tema de inicio de sesión SDDM en ${SDDM_THEMES_DIR}..."
        ${SUDO_PREFIX} mkdir -p "${SDDM_THEMES_DIR}"
        ${SUDO_PREFIX} cp -r "${PROJECT_ROOT}/data/sddm/themes/quartz" "${SDDM_THEMES_DIR}/"
    fi

    echo ""
    echo -e "${GREEN}======================================================${NC}"
    echo -e "${GREEN}      ¡Instalación completada con éxito en la VM!    ${NC}"
    echo -e "${GREEN}======================================================${NC}"
    echo -e "Para iniciar QuartzDE en la máquina virtual:"
    echo -e "1. Selecciona 'QuartzDE' en la pantalla de inicio de sesión (SDDM/GDM)."
    echo -e "2. O ejecuta manualmente desde TTY: ${BIN_DIR}/startquartz"
    echo ""
else
    echo ""
    echo -e "${GREEN}======================================================${NC}"
    echo -e "${GREEN}      Compilación completada exitosamente (--build-only) ${NC}"
    echo -e "${GREEN}======================================================${NC}"
    log_success "Aislamiento preservado: Ningún archivo fue instalado en tu máquina."
    echo -e "Los artefactos compilados se encuentran en:"
    echo -e "  - Rust: ${RUST_TARGET}/"
    echo -e "  - C++:  ${BUILD_DIR}/"
    echo ""
fi
