#!/usr/bin/env bash
# ==============================================================================
# QuartzDE - Script de Verificación de Humo en VM (Sesión Wayland Viva)
# ==============================================================================
# Este script se ejecuta DENTRO de una sesión iniciada de QuartzDE en la VM.
# Valida los 8 puntos críticos de conectividad D-Bus, procesos y servicios.
# Devuelve código 0 si todos los puntos críticos pasan, código 1 si alguno falla.
# ==============================================================================

set -u

# Paleta de colores ANSI
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

PASS_COUNT=0
FAIL_COUNT=0
WARN_COUNT=0

log_header() {
    echo -e "\n${CYAN}=== [CHECK $1] $2 ===${NC}"
}

log_pass() {
    echo -e "  ${GREEN}[OK]${NC} $1"
    PASS_COUNT=$((PASS_COUNT + 1))
}

log_fail() {
    echo -e "  ${RED}[FALLO]${NC} $1"
    FAIL_COUNT=$((FAIL_COUNT + 1))
}

log_warn() {
    echo -e "  ${YELLOW}[AVISO]${NC} $1"
    WARN_COUNT=$((WARN_COUNT + 1))
}

echo -e "${CYAN}======================================================${NC}"
echo -e "${CYAN}        QuartzDE - Test de Humo en Sesión VM          ${NC}"
echo -e "${CYAN}======================================================${NC}"
echo "Fecha y hora: $(date -Iseconds)"
echo "Usuario actual: $(whoami)"
echo ""

# ------------------------------------------------------------------------------
# 1. Variables de entorno Wayland / XWayland y Systemd
# ------------------------------------------------------------------------------
log_header "1" "Variables de entorno de Sesión Wayland"
if [[ -n "${WAYLAND_DISPLAY:-}" ]]; then
    log_pass "WAYLAND_DISPLAY configurado: ${WAYLAND_DISPLAY}"
else
    log_fail "WAYLAND_DISPLAY no está definido en el entorno actual."
fi

if [[ -n "${DISPLAY:-}" ]]; then
    log_pass "DISPLAY (XWayland) configurado: ${DISPLAY}"
else
    log_warn "DISPLAY no está definido (XWayland podría estar inactivo)."
fi

SYSTEMD_WAYLAND=$(systemctl --user show-environment 2>/dev/null | grep "^WAYLAND_DISPLAY=" || true)
if [[ -n "${SYSTEMD_WAYLAND}" ]]; then
    log_pass "systemctl --user tiene WAYLAND_DISPLAY importado: ${SYSTEMD_WAYLAND}"
else
    log_warn "WAYLAND_DISPLAY no está importado en systemctl --user (los portales podrían fallar)."
fi

# ------------------------------------------------------------------------------
# 2. Procesos clave de QuartzDE en ejecución
# ------------------------------------------------------------------------------
log_header "2" "Procesos clave de QuartzDE"
for proc in "quartz-shell" "quartz-spotlight" "quartz-quicklookd"; do
    PIDS=$(pgrep -d ',' -f "${proc}" || true)
    if [[ -n "${PIDS}" ]]; then
        log_pass "Proceso ${proc} en ejecución (PID: ${PIDS})"
    else
        log_fail "Proceso ${proc} NO está en ejecución."
    fi
done

# ------------------------------------------------------------------------------
# 3. Servicios D-Bus registrados en el bus de sesión
# ------------------------------------------------------------------------------
log_header "3" "Servicios D-Bus en el bus de sesión"
USER_BUS_SERVICES=$(busctl --user list --no-legend 2>/dev/null | awk '{print $1}' || true)

check_dbus_service() {
    local srv="$1"
    local required="$2"
    if echo "${USER_BUS_SERVICES}" | grep -qx "${srv}"; then
        log_pass "Servicio D-Bus '${srv}' activo."
    else
        if [[ "${required}" == "true" ]]; then
            log_fail "Servicio D-Bus '${srv}' NO está registrado en el bus de sesión."
        else
            log_warn "Servicio D-Bus '${srv}' no presente aún (depende de fase posterior)."
        fi
    fi
}

check_dbus_service "org.quartz.Session" "true"
check_dbus_service "org.quartz.Config" "true"
check_dbus_service "org.freedesktop.Notifications" "true"
check_dbus_service "org.kde.StatusNotifierWatcher" "true"
check_dbus_service "org.quartz.Compositor" "false"

# ------------------------------------------------------------------------------
# 4. Puente de ventanas del Compositor (WindowBridge)
# ------------------------------------------------------------------------------
log_header "4" "Puente de Ventanas del Compositor (WindowBridge)"
if busctl --user introspect org.quartz.Compositor /org/quartz/Compositor/WindowBridge &>/dev/null; then
    WIN_JSON=$(busctl --user call org.quartz.Compositor /org/quartz/Compositor/WindowBridge org.quartz.Compositor.WindowBridge ListWindows --json=short 2>/dev/null || true)
    if [[ -n "${WIN_JSON}" ]]; then
        log_pass "ListWindows devolvió respuesta válida: ${WIN_JSON:0:60}..."
    else
        log_fail "ListWindows no devolvió datos."
    fi
else
    log_warn "WindowBridge no disponible en D-Bus (requiere Fase 16)."
fi

# ------------------------------------------------------------------------------
# 5. Servidor de Notificaciones
# ------------------------------------------------------------------------------
log_header "5" "Servidor de Notificaciones (org.freedesktop.Notifications)"
if command -v notify-send &>/dev/null; then
    if notify-send "QuartzDE Smoke Test" "Prueba de notificación emitida desde vm_smoke_test.sh" --app-name="SmokeTest" 2>/dev/null; then
        log_pass "notify-send ejecutado sin errores."
    else
        log_fail "notify-send falló al comunicarse con el servidor de notificaciones."
    fi
else
    log_warn "notify-send no está instalado en el sistema."
fi

# ------------------------------------------------------------------------------
# 6. Atajos Globales (KGlobalAccel)
# ------------------------------------------------------------------------------
log_header "6" "Atajos Globales (KGlobalAccel)"
if command -v qdbus6 &>/dev/null; then
    QDBUS_CMD="qdbus6"
elif command -v qdbus &>/dev/null; then
    QDBUS_CMD="qdbus"
else
    QDBUS_CMD=""
fi

if [[ -n "${QDBUS_CMD}" ]]; then
    COMPONENTS=$(${QDBUS_CMD} org.kde.kglobalaccel /kglobalaccel org.kde.KGlobalAccel.allComponents 2>/dev/null || true)
    if echo "${COMPONENTS}" | grep -q "quartz"; then
        log_pass "Componente 'quartz' registrado en KGlobalAccel."
    else
        log_warn "Componente 'quartz' no encontrado en KGlobalAccel (requiere Fase 17)."
    fi
else
    log_warn "Herramienta qdbus/qdbus6 no instalada; omitiendo comprobación de componentes."
fi

# ------------------------------------------------------------------------------
# 7. Pantalla de Bloqueo (ScreenSaver / kscreenlocker)
# ------------------------------------------------------------------------------
log_header "7" "Pantalla de Bloqueo"
if pgrep -f "kscreenlocker_greet" &>/dev/null; then
    log_warn "kscreenlocker_greet ya estaba en ejecución antes de la prueba."
else
    log_pass "kscreenlocker_greet no está activo en estado normal de sesión."
fi

# ------------------------------------------------------------------------------
# 8. Registro de errores críticos en journalctl de usuario
# ------------------------------------------------------------------------------
log_header "8" "Inspección de fallos en Journalctl de usuario"
if command -v journalctl &>/dev/null; then
    CRITICAL_LOGS=$(journalctl --user -b -p 3..0 --no-pager -n 20 2>/dev/null | grep -iE "segfault|core dump|fatal" || true)
    if [[ -z "${CRITICAL_LOGS}" ]]; then
        log_pass "No se detectaron fallos críticos ni segfaults en el journal de la sesión actual."
    else
        log_warn "Se encontraron entradas críticas en journalctl:\n${CRITICAL_LOGS}"
    fi
else
    log_warn "journalctl no disponible."
fi

echo ""
echo -e "${CYAN}======================================================${NC}"
echo -e "${CYAN}                 Resumen de Humo                      ${NC}"
echo -e "${CYAN}======================================================${NC}"
echo -e "Pruebas Aprobadas:  ${GREEN}${PASS_COUNT}${NC}"
echo -e "Pruebas Fallidas:   ${RED}${FAIL_COUNT}${NC}"
echo -e "Advertencias:       ${YELLOW}${WARN_COUNT}${NC}"

if [[ ${FAIL_COUNT} -eq 0 ]]; then
    echo -e "${GREEN}ESTADO: Satisfactorio. La sesión está operativa.${NC}"
    exit 0
else
    echo -e "${RED}ESTADO: Se detectaron fallos críticos en la sesión.${NC}"
    exit 1
fi
