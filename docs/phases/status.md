# Estado de Fases - QuartzDE (WhiteQuartz)

## Fase 1: Arquitectura Base, Workspace y Despliegue para VM
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-15
- **Resultado de Compilación:** Aprobado en modo `--build-only` sin alteración del sistema anfitrión.

## Fase 2: Fidelidad Estricta Aqua y Mapeo a Teclado Clásico de Windows
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-15
- **Resultado de Compilación:** Compilación limpia en `target/release/` y `build/` sin instalación en el host.
- **Resultado de Tests:** 15/15 tests aprobados (100%).
  - Rust Workspace (`cargo test --release --workspace`): 4 suites / 11 tests aprobados.
  - C++ Test Suite (`ctest`): 4 tests aprobados.

## Incidencias Resueltas en VM:
1. **Aprovisionamiento en VM Ubuntu 26.04 ("Resolute"):**
   - Habilitado `universe` automáticamente y corregidos nombres oficiales de paquetes (`kwin-dev`, `liblayershellqtinterface-dev`).
2. **Portabilidad de Tests Unitarios (`quartz-quicklookd`):**
   - Eliminada ruta hardcodeada local `/home/pedro/...` en `test_inspect_text_file`.
   - Implementada resolución dinámica universal con `env!("CARGO_MANIFEST_DIR")` + `Cargo.toml`.
   - Añadido test para tolerancia a ficheros inexistentes (`test_inspect_non_existent_file`).
3. **Inicialización de Sesión Wayland y Carga de Escritorio:**
   - **Causa:** Ruta hardcodeada `/usr/local/bin/startquartz` en `quartz.desktop` causaba fallo inmediato en SDDM al instalar con `--prefix /usr`; además `startquartz` no lanzaba `kwin_wayland` ni esperaba al socket Wayland, y `quartz-shell` dependía de rutas QML externas.
   - **Solución:**
     - Parametrización dinámica de `quartz.desktop` con `${INSTALL_PREFIX}/bin/startquartz` durante la instalación en la VM.
     - `startquartz` ahora supervisa `kwin_wayland`, sondea `$XDG_RUNTIME_DIR/wayland-0` de forma asíncrona (hasta 5s) y aborta limpiamente si el compositor finaliza.
     - Embebido `ShellWindow.qml` en el binario `quartz-shell` mediante `resources.qrc` y dimensionado responsive a la pantalla.

## Fase 3: Integración de Subsistema y Funcionalidad Real - Bloque 1
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-15
- **Resultado de Compilación:** Compilación limpia en `target/release/` y `build/` en modo `--build-only`.
- **Resultado de Tests:** 19/19 tests aprobados (100%).
  - Rust Workspace (`cargo test --release --workspace`): 4 suites / 14 tests aprobados.
  - C++ Test Suite (`ctest`): 5 tests aprobados (`tst_quartzstyle`, `tst_settings_model`, `tst_stagemanager_math`, `tst_finder_modes`, `tst_settingsconfig`).
- **Hitos completados:**
  - Persistencia atómica en `$XDG_CONFIG_HOME/quartz/config.json` en `libquartz::config`.
  - Soporte de temas e iconos (`theme_name`, `icon_theme`) para integración con `themes/Orchis-kde` y `themes/Colloid-icon-theme`.
  - Unificación de contratos D-Bus `org.quartz.Session` (ciclo de vida + energía).
  - Servicio D-Bus reactivo `org.quartz.Config` servido en `startquartz`.
  - Integración reactiva en `quartz-settings` con `SettingsConfigController` y controles QML.

## Fase 3: Integración de Subsistema y Funcionalidad Real - Bloque 2
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-15
- **Resultado de Compilación:** Compilación limpia en `target/release/` y `build/` en modo `--build-only`.
- **Resultado de Tests:** 20/20 tests aprobados (100%).
  - Rust Workspace (`cargo test --release --workspace`): 4 suites / 14 tests aprobados.
  - C++ Test Suite (`ctest`): 6 tests aprobados (`tst_quartzstyle`, `tst_settings_model`, `tst_stagemanager_math`, `tst_finder_modes`, `tst_settingsconfig`, `tst_shell_controllers`).
- **Hitos completados:**
  - Integración nativa de `LayerShellQt` en `quartz-shell`, configurando `LayerTop` y zona exclusiva de 24px para la barra de menús (`MenuBarWindow.qml`) y anclaje inferior para el Dock (`DockWindow.qml`).
  - Señales D-Bus en Rust: `SpotlightServer` emite `toggled` y `QuickLookServer` emite `preview_requested` / `preview_closed`.
  - Implementados `SpotlightController` y `QuickLookController` en `quartz-shell`.
  - Superficies flotantes en capa `LayerOverlay`: `SpotlightOverlay.qml` (con búsqueda viva y cálculos) y `QuickLookModal.qml` (con visualizador de texto, imágenes y metadatos).

## Fase 3: Integración de Subsistema y Funcionalidad Real - Bloque 3
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-15
- **Resultado de Compilación:** Compilación limpia en `target/release/` y `build/` en modo `--build-only`.
- **Resultado de Tests:** 20/20 tests aprobados (100%).
  - Rust Workspace (`cargo test --release --workspace`): 4 suites / 14 tests aprobados.
  - C++ Test Suite (`ctest`): 6 tests aprobados (`tst_quartzstyle`, `tst_settings_model`, `tst_stagemanager_math`, `tst_finder_modes`, `tst_settingsconfig`, `tst_shell_controllers`).
- **Hitos completados:**
  - **Navegación e Interacción completa en `quartz-finder`**:
    - Conexión de ubicaciones en barra lateral (`Inicio`, `Escritorio`, `Documentos`, `Descargas`, `Proyectos`) usando rutas reales del sistema mediante `QStandardPaths`.
    - Pila de historial bidireccional (`‹`, `›`) con estados dinámicos `setEnabled` y atajos clásicos de PC (`Alt+Izquierda`, `Alt+Derecha`).
    - Vista Galería (`Ctrl+4`) plenamente operativa: carrusel inferior de miniaturas sincronizado, visor central escalado para imágenes/carpetas/archivos y panel de metadatos inferior.
    - Soporte de doble clic (`fileDoubleClicked`): navegación automática para directorios y apertura nativa mediante `QDesktopServices::openUrl` para ficheros.
    - Argumentos de línea de comandos en `quartz-finder`: soporte para inicialización en rutas arbitrarias (ej. `quartz-finder ~/Descargas`).
  - **Física Gaussiana Continua y Lanzamiento en el Dock**:
    - Captura continua de la coordenada $X$ del cursor en `dockContainer` mediante `HoverHandler`.
    - Escalado reactivo fluido en cada elemento evaluando la campana Gaussiana $S(d) = S_{base} + (S_{max} - S_{base}) \cdot e^{-\frac{d^2}{2\sigma^2}}$ sin bucles de evaluación en QML.
    - Trayectoria física parabólica de rebote vertical $y(t) = -4h \cdot t(t - 1)$ ante solicitudes de atención o lanzamiento de aplicaciones.
    - Parseo robusto de argumentos de línea de comandos en `DockController::launchApp` mediante `QProcess::splitCommand` con resolución y expansión dinámica del prefijo de usuario (`~`).
