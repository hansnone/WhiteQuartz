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

## Fase 3: Integración de Subsistema y Funcionalidad Real - Bloque 4
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-16
- **Resultado de Compilación:** Compilación limpia en `target/release/` y `build/` en modo `--build-only`.
- **Resultado de Tests:** 22/22 tests aprobados (100%).
  - Rust Workspace (`cargo test --release --workspace`): 4 suites / 16 tests aprobados.
  - C++ Test Suite (`ctest`): 6 tests aprobados (`tst_quartzstyle`, `tst_settings_model`, `tst_stagemanager_math`, `tst_finder_modes`, `tst_settingsconfig`, `tst_shell_controllers`).
- **Hitos completados:**
  - **Superficie de fondo de pantalla Wayland (`WallpaperWindow.qml`)**:
    - Instanciada como superficie de capa `LayerBackground` con `exclusiveZone = -1` y anclajes en los cuatro bordes (`AnchorTop | AnchorBottom | AnchorLeft | AnchorRight`), garantizando renderizado subyacente permanente.
    - Gradiente Aqua Sonoma de reserva y componente `Image` con `PreserveAspectCrop` y transición suave de opacidad.
  - **Indexador XDG real y motor Spotlight (`quartz-spotlight`)**:
    - Parser nativo de archivos `.desktop` estándar recorriendo `/usr/share/applications`, `/usr/local/share/applications` y `$HOME/.local/share/applications`.
    - Filtrado de aplicaciones con `NoDisplay=true`, limpieza de especificadores de campo exec (`%u`, `%F`, etc.) y extracción de nombre, categoría e icono temático.
    - Búsqueda en disco recursiva en el `$HOME` del usuario (hasta 3 niveles de profundidad, excluyendo rutas ocultas y directorios de compilación pesados).
    - Evaluador aritmético con detección automática de operaciones matemáticas elementales.
    - Contrato D-Bus de Spotlight expandido a tupla de 5 elementos `(title, subtitle, category, icon_name, action_payload)` y activación nativa de apps y archivos con `QDesktopServices`.
  - **Previsualización multimedia en QuickLook (`quartz-quicklookd`)**:
    - Extracción y formateo de texto plano hasta 64KB (protección contra desbordamiento o cuellos de botella D-Bus IPC).
    - Detección precisa de formatos de imagen (`png`, `jpg`, `jpeg`, `webp`, `svg`, `bmp`) y metadatos de archivo (tamaño, tipo MIME, fecha de modificación).
    - `QuickLookModal.qml` actualizado con visualizador fluido de imágenes escaladas y visor con scroll para archivos de código y texto.
  - **Despliegue automatizado de temas y entorno Wayland**:
    - Integración en `scripts/build_and_install.sh` (`--vm-install`) para desplegar automáticamente cursores e iconos desde `themes/Colloid-icon-theme` y esquemas de color desde `themes/Orchis-kde`.
    - Configuración en `crates/quartz-session` de variables de entorno de cursor (`XCURSOR_THEME="Colloid-cursors"`, `XCURSOR_SIZE="24"`) y tema de plataforma Qt (`QT_QPA_PLATFORMTHEME="kde"`).

## Fase 4: Integración Real en el Compositor KWin
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-16
- **Resultado de Compilación:** Compilación limpia de bibliotecas y plugins en `target/release/` y `build/` en modo `--build-only`.
- **Resultado de Tests:** 22/22 tests aprobados (100%).
  - Rust Workspace (`cargo test --release --workspace`): 4 suites / 16 tests aprobados.
  - C++ Test Suite (`ctest`): 6 tests aprobados (`tst_quartzstyle`, `tst_settings_model`, `tst_stagemanager_math`, `tst_finder_modes`, `tst_settingsconfig`, `tst_shell_controllers`).
- **Hitos completados:**
  - **Plugin de Decoración de Ventanas (`KDecoration3::Decoration`)**:
    - Desarrollado `QuartzDecorationPlugin` implementando `KDecoration3::Decoration` y registrado mediante macro `K_PLUGIN_CLASS_WITH_JSON(QuartzDecorationPlugin, "decoration.json")`.
    - Integración de semáforo Aqua completo con espaciado de 8px, radio de 12px y margen de 14px, con glifos interactivos en hover (`x`, `-`, `+`) y punto de cambios pendientes.
    - Soporte de Window Tiling menu (temporizador hover de 500ms en botón de zoom).
    - Cálculo dinámico de sombras `KDecoration3::DecorationShadow` sincronizadas con el estado de foco de la ventana (`blurRadius = 32.0` activa vs `14.0` inactiva).
  - **Plugin de Efectos de Composición KWin (`KWin::OffscreenEffect`)**:
    - Desarrollado `StageManagerKWinPlugin` implementando `KWin::OffscreenEffect` y registrado mediante `KWIN_EFFECT_FACTORY(StageManagerKWinPlugin, "metadata.json")`.
    - Carga e inyección del shader superelíptico `squircle.frag` mediante `KWin::ShaderManager` y `compositor.qrc` (`:/effects/quartz/shaders/squircle.frag`).
    - Enlace de uniforms de superelipse en GPU: `uWindowSize`, `uCornerRadius = 11.0`, `uPower = 4.5` ($G^2$), y color de borde perimetral subpíxel.
    - Aplicación en tiempo real de escala, rotación 3D de 14° en el eje Y y traslación hacia la tira lateral de Stage Manager sobre `WindowPaintData`.
  - **Contrato D-Bus de Compositor**:
    - Registro reactivo del servicio `/org/quartz/Compositor/StageManager` bajo la interfaz `org.quartz.Compositor.StageManager` en el bus de sesión, enlazando directamente con `StageManagerProxy` en Rust.

## Fase 5: Gestión de Ventanas Wayland, Menú Global (`DBusMenu`) y TaskManager del Dock
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-16
- **Resultado de Compilación:** Compilación limpia en `target/release/` y `build/` en modo `--build-only`.
- **Resultado de Tests:** 22/22 tests aprobados (100%).
  - Rust Workspace (`cargo test --release --workspace`): 4 suites / 16 tests aprobados.
  - C++ Test Suite (`ctest`): 6 tests aprobados (`tst_quartzstyle`, `tst_settings_model`, `tst_stagemanager_math`, `tst_finder_modes`, `tst_settingsconfig`, `tst_shell_controllers`).
- **Hitos completados:**
  - **Seguimiento de Ventanas Wayland y TaskManager del Dock**:
    - `DockController`: Métodos `onWindowAdded`, `onWindowRemoved` y `onActiveWindowChanged` para rastreo dinámico de ventanas y aplicaciones activas.
    - Sincronización del estado `running = true` y punto indicador luminoso de 4pt para aplicaciones ancladas (`quartz-finder`, `quartz-settings`, `x-terminal-emulator`).
    - Inserción y eliminación reactiva de aplicaciones en ejecución no ancladas antes del separador vertical en la propiedad `apps` de `DockController`.
    - `DockWindow.qml` migrado a la lista reactiva `dockCtrl.apps`.
  - **Servidor `AppMenu.Registrar` e Importador `DBusMenu` en MenuBar**:
    - `AppMenuRegistrar`: Servicio D-Bus `com.canonical.AppMenu.Registrar` en `/com/canonical/AppMenu/Registrar` implementando `RegisterWindow`, `UnregisterWindow`, `GetMenuForWindow`, con señales D-Bus `WindowRegistered` y `WindowUnregistered`.
    - `MenuBarController`: Consulta asíncrona de la jerarquía de menús vía `com.canonical.dbusmenu::GetLayout` y despacho de eventos con `Event(id, "clicked")`. Menús macOS por defecto estructurados ("Archivo", "Edición", "Visualización", "Ventana", "Ayuda") con atajos de teclado estándar de PC (`Ctrl+N`, `Ctrl+O`, `Ctrl+W`, `Ctrl+Z`, `Ctrl+X/C/V/A`, `Ctrl+1..4`, etc.) ante ausencia o desregistro de menú de la app.
    - `MenuBarWindow.qml` actualizado con `Repeater` dinámico sobre `menuBarCtrl.activeAppMenus` desplegando submenús nativos emergentes y disparando acciones al hacer clic.
  - **Stacks del Dock (`Descargas`) y Papelera Dinámica**:
    - `DockController::listStackDirectory`: consulta ordenada de hasta 10 elementos recientes con nombres, rutas, tamaños human-readable e iconos temáticos por tipo MIME.
    - `DockWindow.qml`: popup tipo cuadrícula (*Grid Stack*) estilo macOS Sonoma/Sequoia sobre el icono de "Descargas" con miniaturas interactivas, indicador de ítems, apertura de archivos o carpetas en el Finder y botón footer "Abrir en el Finder".
    - Monitorización en tiempo real de `~/.local/share/Trash/files` mediante `QFileSystemWatcher` en `DockController::updateTrashState`, alternando automáticamente el icono de Papelera entre `user-trash` (vacía) y `user-trash-full` (con elementos).
    - Tooltips flotantes oscuros estilo macOS (*tooltip pills*) sobre los iconos del Dock durante el hover.
  - **Ampliación de la Suite de Tests**:
    - `tst_shell_controllers` ampliado a 12 pruebas unitarias individuales cubriendo `AppMenuRegistrar`, `MenuBarController`, `listStackDirectory`, `updateTrashState` y seguimiento dinámico de ventanas no ancladas y ancladas en el Dock.


