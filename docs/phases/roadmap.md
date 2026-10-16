# Roadmap - QuartzDE (WhiteQuartz)

> **Convención de Estados:**
> - `[x]` **Verificado**: Tests unitarios en verde + test de integración con cableado real + evidencia en VM pegada en `status.md` + build limpio.
> - `[~]` **Implementado sin verificar en VM**: Código implementado y testeado con mocks o en local, pendiente de validación en sesión real de VM.
> - `[ ]` **Pendiente**: Brecha técnica confirmada o tarea sin implementar.

---

## Fase 1: Arquitectura Base, Workspace y Script de Despliegue para VM
- [x] Configuración de Monorepo (Cargo Workspace + CMake)
- [x] Script `scripts/build_and_install.sh` con protecciones para compilación local e instalación en VM
- [x] `crates/libquartz`: Contratos IPC, D-Bus y tipos base
- [x] `crates/quartz-session`: Gestor de inicio de sesión Wayland
- [x] `crates/quartz-quicklookd`: Demonio de previsualización en Rust
- [x] `crates/quartz-spotlight`: Demonio y motor de búsqueda en Rust
- [x] `src/style/`: Motor de estilo Aqua/Sonoma (`QuartzStyle` y métricas)
- [x] `src/compositor/`: Decoración y efectos Stage Manager
- [x] `src/settings/`: Reemplazo de Ajustes del Sistema
- [x] `src/shell/`: Menubar y Dock
- [x] Tests unitarios y de integración

## Fase 2: Fidelidad Estricta Aqua y Mapeo a Teclado Clásico de Windows
- [x] Centralización de atajos PC/Windows en `crates/libquartz/src/shortcuts.rs` (`Ctrl`, `Alt`, `Super`)
- [x] Ciclo de vida centrado en aplicaciones/documentos en `crates/quartz-session` (retención tras `Ctrl+W` hasta `Ctrl+Q`)
- [x] Shaders de superelipse $G^2$ ($n=4.5$), sombras dinámicas y Vibrancy en `src/compositor/`
- [x] Semáforo avanzado en `src/compositor/quartzdecoration`: símbolos en hover (`×`, `-`, flechas), punto de cambios no guardados y menú *Window Tiling* (500 ms)
- [x] Hojas modales (*Sheets*) y barras de desplazamiento efímeras superpuestas en `src/style/`
- [x] Dock con física gaussiana, rebote parabólico y punto indicador de 4pt en `src/shell/dock/`
- [x] MenuBar con introspección dinámica de menús mediante tecla `Alt` en `src/shell/menubar/`
- [x] Finder multimodal con 4 modos (`Ctrl+1` a `Ctrl+4`) e inspector de metadatos en `src/finder/`
- [x] Suite de pruebas unitarias ampliada (12 tests en total: 8 en Rust, 4 en C++/CTest)

## Fase 3: Protocolos Wayland (`LayerShellQt`), Temas, Wallpaper e Indexación Real
- [x] **Bloque 1: Unificación IPC y Persistencia de Configuración XDG**
  - [x] Persistencia atómica en `$XDG_CONFIG_HOME/quartz/config.json` en `libquartz::config`
  - [x] Soporte para temas locales (`theme_name`, `icon_theme`) inspirados en `themes/Orchis-kde` y `themes/Colloid-icon-theme`
  - [x] Unificación del contrato D-Bus `org.quartz.Session` (ciclo de vida de apps y energía/sesión)
  - [x] Implementación y exposición del servicio D-Bus reactivo `org.quartz.Config` en `crates/quartz-session`
  - [x] Conexión reactiva en `quartz-settings` mediante `SettingsConfigController` y controles en QML
  - [x] Suite de tests ampliada (19 tests: 14 en Rust, 5 en C++/CTest)
- [x] **Bloque 2: Wayland Layer-Shell y UI de Servicios**
  - [x] Integración de `LayerShellQt` en `quartz-shell` (MenuBar con zona exclusiva de 24px y Dock anclado inferior)
  - [x] Superficies QML en capa `LayerOverlay` para `Spotlight` y `QuickLook`
  - [x] Separación de `MenuBarWindow.qml`, `DockWindow.qml`, `SpotlightOverlay.qml` y `QuickLookModal.qml`
  - [x] Controladores D-Bus `SpotlightController` y `QuickLookController` con señales reactivas
  - [x] Suite de tests ampliada (20 tests: 14 en Rust, 6 en C++/CTest)
- [x] **Bloque 3: Completitud funcional en `quartz-finder` y Dock**
  - [x] Conexión de sidebar, historial, doble clic y vista Galería en `quartz-finder`
  - [x] Física continua gaussiana en el Dock capturando coordenadas continuas de cursor
  - [x] Parcheo de rutas CLI y `QProcess::splitCommand` con expansión de tilde (`~`)
  - [x] Suite de tests ampliada (20 tests: 14 en Rust, 6 en C++/CTest con pruebas de física, splitCommand y modos)
- [x] **Bloque 4: Wallpaper, Indexación Real XDG, Multimedia en QuickLook y Despliegue de Temas**
  - [x] Superficie de fondo de pantalla `WallpaperWindow.qml` (`LayerBackground`, `exclusiveZone = -1`)
  - [x] Indexador XDG real (.desktop), búsqueda de archivos y calculadora avanzada en `quartz-spotlight`
  - [x] Previsualización multimedia (imágenes, texto de 64KB, audio/video) en `quartz-quicklookd` y `QuickLookModal.qml`
  - [x] Despliegue automatizado de temas `Colloid` y `Orchis` en `scripts/build_and_install.sh` y variables de entorno de cursor/tema en `startquartz`

## Fase 4: Integración Real en el Compositor KWin (`KDecoration3`, `KWin::Effect` y Shaders GLSL)
- [x] Plugin de Decoración de Ventanas `QuartzDecoration` (`KDecoration3::Decoration` con botones Aqua, sombras dinámicas y semáforo)
- [x] Plugin de Efectos `StageManagerEffect` (`KWin::OffscreenEffect`) y carga de shader superelíptico `squircle.frag`
- [x] Registro del objeto D-Bus `/org/quartz/Compositor/StageManager` en KWin para control reactivo externo

## Fase 5: Gestión de Ventanas Wayland, Menú Global (`DBusMenu`) y TaskManager del Dock (Revisada)
- [ ] Seguimiento de ventanas en producción (F1: nadie alimenta `DockController`/`MenuBarController` fuera de tests)
- [~] Servidor `com.canonical.AppMenu.Registrar` e importador `DBusMenu` en `MenuBarController`
- [~] Stacks del Dock (`Descargas` en cuadrícula emergente) y estado reactivo de papelera

## Fase 6: Paridad Funcional 1:1 en `quartz-finder` y Estabilidad de Configuración (Revisada)
- [x] **0.2.2**: Estabilidad de configuración en `libquartz`: `#[serde(default)]` en structs, respaldo `config.json.bad` y campo `wallpaper`
- [~] **6.0**: Modelo asíncrono en `FinderColumnView` (`QFileSystemModel` + `QSortFilterProxyModel`)
- [~] **6.1**: Barra inferior de ruta (*Path Bar*) interactiva y cálculo de espacio libre (`QStorageInfo`)
- [~] **6.2**: Motor de operaciones de archivos `FileOperations` (copia asíncrona, xattr; pendiente auditoría de conflictos F21.2)
- [~] **6.3**: Pestañas independientes, etiquetas de color (`user.xdg.tags`) y montaje de volúmenes con UDisks2

## Fase 7: Subsistema de Hardware, Control Center, Notificaciones y `quartz-settings` (Revisada)
- [~] **7.1**: Servidor de notificaciones `org.freedesktop.Notifications` 1.2 (F13: saneado débil, sin persistencia, expiración defectuosa)
- [ ] **7.2**: Control Center (F12: consultas vacías en brillo, pulseaudio, bluez, mpris)
- [~] **7.3**: Carga dinámica de paneles en `quartz-settings` (pendiente verificar conexión con backends reales F21.3)

## Fase 8: Atajos Globales, Bloqueo de Pantalla, Login (SDDM) y Gestión de Energía (Revisada)
- [ ] **8.1**: Atajos globales de sesión (F3: `GlobalShortcuts` no enlaza `KGlobalAccel` ni `QAction`)
- [ ] **8.2**: Pantalla de bloqueo Look-and-Feel (F4: KWin lanzado con `--no-lockscreen`, `metadata.json` Plasma 5)
- [~] **8.3**: Tema QML para inicio de sesión en SDDM (`data/sddm/themes/quartz/`)
- [ ] **8.4**: Menú Apple y energía (F14: uso de `systemctl`/`loginctl` directos, `xkill`, sin inhibitors)

## Fase 9: Dock, Barra de Menús y Escritorio Avanzados (Revisada)
- [ ] **9.0**: Privilegios Wayland de KWin (`X-KDE-Wayland-Interfaces` erróneo en sesión en vez de shell F23)
- [ ] **9.1**: Menú contextual de apps del Dock y minimización real (F1: sin TaskManager real, usa `killall` F14)
- [~] **9.2**: Cambiador de aplicaciones modal (`Alt+Tab`) con orden MRU
- [ ] **9.3**: Esquinas activas (F10: roto en Wayland por sondeo de `QCursor::pos()`, acciones hardcodeadas)
- [ ] **9.4**: Bandeja del sistema (F11: `IsStatusNotifierHostRegistered` falso sin host real, sin soporte de `IconPixmap`)
- [~] **9.5**: Iconos en el escritorio en `WallpaperWindow` con `QFileSystemModel`
- [ ] **9.6**: Capturas de pantalla (F22: dependencia externa de CLI `spectacle` en vez de portales XDG)
- [~] **9.7**: Diálogo de forzar salida (usa `killall` en vez de `SIGTERM`/`SIGKILL` a PID F14)
- [~] **9.8**: Reloj personalizable y menú global

## Fase 10: Launchpad, Mission Control y Spaces (Revisada)
- [~] **10.1**: Launchpad a pantalla completa con catálogo `ListApps()`
- [ ] **10.2**: Mission Control (F8: efecto solo conmuta `spatialMode`, F9: doble disparo D-Bus, sin rejilla en compositor)

## Fase 11: Calidad Visual, Spotlight Avanzado y Robustez (Revisada)
- [~] **11.1**: Desenfoque Gaussiano real (`vibrancy`) con `KF6::WindowSystem`
- [ ] **11.2**: Spotlight avanzado (F21: sin evaluador matemático real, sin cancelación de consultas)
- [~] **11.3**: Quick Look multimedia y visor PDF (`QtPdf`)
- [~] **11.4**: Modo oscuro y color de acento automático sincronizado
- [ ] **11.5**: Robustez integral del compositor (F7: shader aplicado a todas las ventanas, transformaciones sin filtro)

## Fase 12: Aplicaciones del Sistema (Revisada)
- [ ] Integración y tematización de aplicaciones del sistema (F19: lanzadores a apps sin dependencias instaladas en VM)

## Fase 13: Servicios del Sistema (Revisada)
- [ ] Servicios del Sistema reales (F16: Time Machine simulado, F17: llavero inseguro en memoria, F18: impresoras bloqueantes)

---

## Fase 14: Verificación, Honestidad y Fundamentos Correctivos
- [x] **14.1**: Archivar especificación rectora en `docs/requirements/macos-corrective-plan-14-22.md`
- [x] **14.2**: Reiniciar y sincronizar `docs/phases/roadmap.md` bajo estados `[x]`, `[~]`, `[ ]`
- [x] **14.3**: Reestructurar `docs/phases/status.md` incorporando campo `Evidencia:` en cada fase
- [x] **14.4**: Adaptar `.agents/scripts/require-review.py` para exigir `Evidencia:` no vacía para `[x]`
- [x] **14.5**: Validar y actualizar permisos en `.agents/scripts/scope-gate.py`
- [x] **14.6**: Actualizar `scripts/setup_vm_dependencies.sh` con dependencias de compilación y ejecución
- [x] **14.7**: Endurecer CMake en `src/shell/CMakeLists.txt` (`LayerShellQt`, `KF6WindowSystem`, `KF6GlobalAccel` `REQUIRED`)
- [x] **14.8**: Formalizar contratos D-Bus canónicos en `data/dbus/` (`StageManager.xml` y `WindowBridge.xml`)
- [x] **14.9**: Implementar script de prueba de humo en VM `scripts/vm_smoke_test.sh` con 8 comprobaciones
- [x] **14.10**: Verificación integral de compilación (`build_and_install.sh --build-only`) y test suites

## Fase 15: Sesión y Arranque Limpio
- [ ] **15.1**: Importación temprana del entorno Wayland (`dbus-update-activation-environment` y systemd)
- [ ] **15.2**: Resolución dinámica del `DISPLAY` XWayland
- [ ] **15.3**: Arranque de servicios acompañantes (agente polkit, portales XDG, `kglobalacceld`, `kded6`)
- [ ] **15.4**: Supervisión de procesos en Rust con reinicio automático de `quartz-shell` y servicios
- [ ] **15.5**: Activación de pantalla de bloqueo (`kscreenlocker`), corrección de `metadata.json` y `kscreenlockerrc`
- [ ] **15.6**: Acciones de energía y sesión mediante `logind` con gestión de inhibitors y cuenta atrás de 60s
- [ ] **15.7**: Verificación y pruebas de tema SDDM en `data/sddm/themes/quartz/`

## Fase 16: Contrato D-Bus del Compositor y Puente de Ventanas
- [ ] **16.1**: Reclamación del servicio `org.quartz.Compositor` y alineación PascalCase de métodos
- [ ] **16.2**: Implementación del puente de ventanas `WindowBridge` dentro del plugin del compositor KWin
- [ ] **16.3**: Implementación de `WindowTracker` en el shell y cableado de señales reales hacia Dock y MenuBar
- [ ] **16.4**: Operaciones de cierre, minimización y salida en DockController usando PIDs y WindowBridge (sin `killall`)

## Fase 17: Atajos Globales (`KF6::GlobalAccel`)
- [ ] **17.1**: Integración de `KGlobalAccel` en `GlobalShortcuts` con acciones QAction vinculadas a `org.quartz.shell`
- [ ] **17.2**: Resolución de conflictos de atajos nativos de KWin en `kglobalshortcutsrc`

## Fase 18: Compositor Robusto y Mission Control Real
- [ ] **18.1**: Robustez de `StageManagerEffect` y `QuartzDecoration` (guardas `nullptr`, contexto GL, modo seguro)
- [ ] **18.2**: Filtrado de `drawWindow` y transformaciones para excluir superficies de shell y decoraciones
- [ ] **18.3**: Rejilla viva de Mission Control y App Exposé en el compositor con hit-testing y animación
- [ ] **18.4**: Reemplazo de controlador local en `MissionControlController` para delegar en el compositor sin doble disparo

## Fase 19: Shell: Layer-Shell, Esquinas Activas y Bandeja del Sistema
- [~] **19.1**: Inicialización de `LayerShellQt` en dos fases en `main.cpp` para todos los overlays (activación temprana, zona exclusiva y anclajes fijos)
- [ ] **19.2**: Esquinas activas mediante ventanas sensor de 2x2 px (`LayerOverlay`) con retardo configurable
- [ ] **19.3**: Implementación de `StatusNotifierHost` y modelo de bandeja del sistema con soporte de `IconPixmap`

## Fase 20: Notificaciones, Control Center, Dock y Energía
- [ ] **20.1**: Servidor de notificaciones con lista blanca HTML, límites de frecuencia y razones de cierre correctas
- [ ] **20.2**: Backends reactivos en Control Center (PulseAudioQt, login1 brillo, NetworkManager, BlueZ, MPRIS2)
- [ ] **20.3**: Lanzamiento de apps en Dock mediante KIO/KService y persistencia de ancladas
- [ ] **20.4**: Menú Apple con "Forzar salida" real y diálogo "Acerca de este Mac" con telemetría del sistema

## Fase 21: Spotlight, Finder, Ajustes y Capturas
- [ ] **21.1**: Spotlight con evaluador matemático real, consultas cancelables y limitación de indexación
- [ ] **21.2**: Auditoría y robustez en Finder (resolución de conflictos al pegar, copia asíncrona segura)
- [ ] **21.3**: Conexión de los 18 paneles de Ajustes a backends de configuración reales
- [ ] **21.4**: Capturas de pantalla mediante portal XDG Desktop con nombrado canónico

## Fase 22: Aplicaciones del Sistema y Servicios
- [ ] **22.1**: Descriptores `.desktop` honestos y perfiles macOS para Konsole, Kate, Okular y Monitor de Actividad
- [ ] **22.2**: Time Machine real integrado con `restic` (inicialización, backup con progreso JSON y retención)
- [ ] **22.3**: Integración con Secret Service (KWallet/libsecret) para credenciales
- [ ] **22.4**: Integración asíncrona de impresoras CUPS o derivación a utilidades nativas
