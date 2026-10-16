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
4. **Compatibilidad Multi-Versión KWin 6 en `OffscreenEffect::drawWindow`:**
   - **Causa:** En KWin 6.0/6.1 (Ubuntu 24.04/26.04 LTS / Debian), el método virtual `KWin::OffscreenEffect::drawWindow` devuelve `void`. En KWin 6.2+ (distribuciones rolling como Arch/CachyOS), la firma fue modificada por KDE para devolver `bool`. Esto causaba el error `conflicting return type specified for 'virtual bool StageManagerKWinPlugin::drawWindow(...)'` al compilar en la VM.
   - **Solución:**
     - Implementada detección de características en tiempo de configuración con `CheckCXXSourceCompiles` en `src/compositor/CMakeLists.txt` enlazando con `KF6::CoreAddons`, `KF6::ConfigCore` y `KWIN_LIBRARY`.
     - Inyectada la macro `KWIN_OFFSCREEN_DRAW_RETURNS_BOOL=1` solo cuando la firma devuelve `bool`.
     - Adaptados `src/compositor/stagemanager_plugin.h` y `src/compositor/stagemanager_plugin.cpp` para compilar condicionalmente la variante `bool` o `void` según la versión instalada de KWin en el sistema de destino.

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

## Fase 6: Paridad Funcional 1:1 en `quartz-finder` y Estabilidad de Configuración
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-16
- **Resultado de Compilación:** Compilación limpia en `target/release/` y `build/` en modo `--build-only`.
- **Resultado de Tests:** 24/24 tests aprobados (100%).
  - Rust Workspace (`cargo test --release --workspace`): 4 suites / 18 tests aprobados (+2 tests de backward compatibility y bad backup).
  - C++ Test Suite (`ctest`): 6 suites / 100% aprobadas (`tst_finder_modes` con 11 tests unitarios completos).
- **Hitos completados:**
  - **Estabilidad de Configuración (Regla 0.2.2 & Hallazgos H2/H3)**:
    - `#[serde(default)]` incorporado en `DockConfig`, `StageManagerConfig` y `QuartzDesktopConfig`.
    - Respaldo automático de JSON inválido/corrupto hacia `config.json.bad` preservando el estado previo en `load_from_xdg`.
    - Incorporación de `wallpaper: String` en `QuartzDesktopConfig` y enlace reactivo en `WallpaperWindow.qml`.
  - **Modelo Asíncrono y Vistas Multimodales (`FinderColumnView`)**:
    - Sustitución de listas síncronas por `QFileSystemModel` reactivo.
    - Barra inferior de ruta (*Path Bar*) de 22px con migas de pan interactivas (`QToolButton`) y contador dinámico de elementos/espacio disponible vía `QStorageInfo`.
    - Carga asíncrona de miniaturas en vista Galería con `QImageReader::setScaledSize` y caché `QPixmapCache` sin bloquear el hilo GUI.
  - **Motor Desacoplado `FileOperations`**:
    - Nombres únicos automáticos (`carpeta sin título`, `carpeta sin título 2`).
    - Duplicación inteligente respetando extensiones dobles (`.tar.gz`, etc.).
    - Lectura/escritura de etiquetas de color XDG (`user.xdg.tags`) con `<sys/xattr.h>` y degradación elegante ante sistemas sin xattr.
    - Soporte estándar de portapapeles con `text/uri-list` y `application/x-kde-cutselection`.
    - Copia recursiva asíncrona previniendo autorecursión y respetando symlinks y fechas/permisos.
    - Deshacer/Rehacer mediante `QUndoStack` (`RenameCommand`, `CreateFolderCommand`).
  - **Multi-Pestaña y Sidebar**:
    - `QTabWidget` con pestañas cerrables (`Ctrl+T`, `Ctrl+W`, `Ctrl+Tab`).
    - Atajos clásicos de Windows/PC completos (`Ctrl+1..4`, `Ctrl+Shift+N`, `Ctrl+C/X/V`, `Ctrl+D`, `F2`, `Delete`, `Ctrl+Z`).
    - Sidebar organizado con Favoritos, Ubicaciones (`QStorageInfo::mountedVolumes` filtrando sistemas pseudo) y Etiquetas de color con `TagCircleDelegate`.

## Fase 7: Subsistema de Hardware, Control Center, Notificaciones y `quartz-settings`
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-16
- **Resultado de Compilación:** Compilación limpia en `target/release/` y `build/` en modo `--build-only`.
- **Resultado de Tests:** 24/24 tests aprobados (100%).
  - Rust Workspace (`cargo test --release --workspace`): 4 suites / 18 tests aprobados.
  - C++ Test Suite (`ctest`): 6 suites / 100% aprobadas (`tst_shell_controllers` ampliado a 14 tests unitarios pasando al 100%).
- **Hitos completados:**
  - **Servidor de Notificaciones (`org.freedesktop.Notifications` 1.2)**:
    - Desarrollado `NotificationServer` registrando `/org/freedesktop/Notifications` con capacidades estándar (`body`, `actions`, `persistence`), saneamiento de HTML seguro contra scripts maliciosos (Regla 0.2.7), y despacho de señales `NotificationClosed` y `ActionInvoked`.
    - `NotificationModel`: Gestión reactiva de banners flotantes (`activeBanners`), contador de no leídas e historial completo para Centro de Notificaciones.
    - `NotificationBanner.qml`: Banner emergente translúcido estilo macOS Sonoma en capa `LayerOverlay` con temporizador de cierre automático, cabecera de aplicación y botones de acción interactivos.
    - `NotificationCenter.qml`: Panel lateral deslizante con historial de notificaciones agrupado, botón "Borrar todo" y widgets de Calendario y Reloj.
  - **Centro de Control y OSD (`SystemHardwareController`)**:
    - Integración de volumen (PulseAudio/PipeWire), brillo de pantalla, Wi-Fi (NetworkManager), Bluetooth (Bluez) y batería (UPower) con degradación elegante ante entornos virtualizados (Regla 0.2.5).
    - `ControlCenterPopup.qml`: Cuadrícula 2x2 para conectividad y concentración, deslizadores continuos de brillo y sonido, y control de reproducción multimedia MPRIS2.
    - `OsdWindow.qml`: HUD centrado en pantalla (*Bezel overlay*) para respuesta visual inmediata al cambiar volumen o brillo.
    - Barra de menús (`MenuBarWindow.qml`): Nuevos accesos directos reactivos a Centro de Control y Centro de Notificaciones.
  - **Los 18 Paneles de Ajustes del Sistema (`quartz-settings`)**:
    - Implementación completa de los 18 componentes QML en `src/settings/qml/panes/`: `ProfilePane`, `WifiPane`, `BluetoothPane`, `NetworkPane`, `NotificationsPane`, `SoundPane`, `FocusPane`, `GeneralPane`, `AppearancePane`, `AccessibilityPane`, `ControlCenterPane`, `DockPane`, `DisplaysPane`, `WallpaperPane`, `LockScreenPane`, `UsersPane`, `KeyboardPane`, `TrackpadPane`.
    - `SettingsMainWindow.qml` migrado a carga dinámica modular vía `Loader` según la categoría seleccionada en `SettingsNavigationModel`.
    - Ampliado `SettingsConfigController` con propiedades reactivas para fondo de pantalla, tamaño de Dock y ampliación de iconos.

## Fase 8: Atajos Globales, Bloqueo de Pantalla, Login (SDDM) y Gestión de Energía
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-16
- **Resultado de Compilación:** Compilación limpia en `target/release/` y `build/` en modo `--build-only`.
- **Resultado de Tests:** 25/25 tests aprobados (100%).
  - Rust Workspace (`cargo test --release --workspace`): 4 suites / 18 tests aprobados.
  - C++ Test Suite (`ctest`): 6 suites / 100% aprobadas (`tst_shell_controllers` ampliado a 15 tests unitarios completos).
- **Hitos completados:**
  - **Atajos Globales de Sesión (`GlobalShortcuts`)**:
    - Desarrollado `GlobalShortcuts` registrando `/org/quartz/GlobalShortcuts` en D-Bus para disparadores globales: `TriggerSpotlight` (`Ctrl+Space`), `TriggerMissionControl` (`Super+Up`), `TriggerAppExpose` (`Super+Down`), `TriggerLockScreen` (`Super+L`), `TriggerForceQuit` (`Ctrl+Alt+Esc`) y `TriggerScreenshot` (`Ctrl+Shift+3/4/5`).
  - **Paquete Look-and-Feel para Pantalla de Bloqueo (`kscreenlocker`)**:
    - Creado paquete estándar `data/lookandfeel/org.quartz.desktop/` con `metadata.json` y `contents/lockscreen/LockScreen.qml`.
    - Reloj tipográfico grande estilo Sonoma/Sequoia, avatar de usuario con inicial, entrada de contraseña en cápsula y botones inferiores de Reposo, Reinicio y Apagado.
    - Integración en `scripts/build_and_install.sh` para despliegue en `${INSTALL_PREFIX}/share/plasma/look-and-feel/`.
  - **Tema de Inicio de Sesión SDDM (`data/sddm/themes/quartz/`)**:
    - Desarrollados `metadata.desktop`, `theme.conf` y `Main.qml` estilo macOS Sonoma para inicio de sesión en SDDM.
    - Selector de sesiones Wayland, autenticación con PAM/sddm greeter y acciones de energía directas.
    - Integración en `scripts/build_and_install.sh` para despliegue en `${INSTALL_PREFIX}/share/sddm/themes/quartz/`.
  - **Menú Apple y Ciclo de Energía**:
    - `MenuBarWindow.qml` y `MenuBarController::triggerAppleMenuAction` ampliados con "Acerca de este Mac", "Forzar salida...", "Reposo", "Reiniciar...", "Apagar equipo...", "Bloquear pantalla" y "Cerrar sesión...".
    - Delegación segura a `systemctl`, `loginctl` y D-Bus de sesión sin escalada indebida de privilegios (Regla 0.2.8).

## Fase 9: Dock, Barra de Menús y Escritorio Avanzados
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-16
- **Resultado de Compilación:** Compilación limpia en `target/release/` y `build/` en modo `--build-only`.
- **Resultado de Tests:** 26/26 tests aprobados (100%).
  - Rust Workspace (`cargo test --release --workspace`): 4 suites / 18 tests aprobados.
  - C++ Test Suite (`ctest`): 6 suites / 100% aprobadas (`tst_shell_controllers` ampliado a 16 tests unitarios pasando al 100%).
- **Hitos completados:**
  - **Verificación de Privilegios Wayland (Hallazgo H7)**:
    - Incorporado `X-KDE-Wayland-Interfaces=org_kde_plasma_window_management;` en `data/sessions/quartz.desktop` y creado `data/applications/org.quartz.shell.desktop` con despliegue automático en `${INSTALL_PREFIX}/share/applications/`.
  - **Dock Contextual y Ciclo de Vida de Aplicaciones**:
    - Menú contextual de clic derecho en iconos del Dock con submenú "Opciones" ("Mantener en el Dock", "Abrir al iniciar sesión", "Mostrar en el Finder"), "Salir" (vía `org.quartz.Session::quit_application`) y "Forzar salida".
    - Incorporados métodos `quitApp`, `forceQuitApp` y `toggleKeepInDock` en `DockController`.
  - **Cambiador de Aplicaciones (`Alt+Tab`)**:
    - Desarrollado `AppSwitcherOverlay.qml`: HUD modal centrado con acrílico oscuro, iconos de aplicaciones en ejecución en orden MRU y selección interactiva.
  - **Esquinas Activas (*Hot Corners*)**:
    - Desarrollado `HotCornersController` con detección continua en los 4 vértices de la pantalla (TopLeft -> Mission Control, TopRight -> Centro de Notificaciones, BottomLeft -> Spotlight) y retardo de activación de 200ms para evitar falsos positivos.
  - **Bandeja del Sistema (`StatusNotifierItem` / `StatusNotifierWatcher`)**:
    - Desarrollado `StatusNotifierWatcher` registrando `org.kde.StatusNotifierWatcher` en `/StatusNotifierWatcher`, permitiendo el registro y desconexión en caliente de iconos de bandeja de aplicaciones externas.
  - **Iconos de Escritorio y Menú Contextual**:
    - `WallpaperWindow.qml` actualizado con cuadrícula de accesos directos estilo macOS (Macintosh HD, Documentos, Descargas con doble clic) y menú contextual de clic derecho en el escritorio ("Nueva carpeta", "Obtener información", "Cambiar fondo de pantalla...", etc.).
  - **Diálogo "Forzar salida de las aplicaciones" (`Ctrl+Alt+Esc`)**:
    - Desarrollado `ForceQuitModal.qml` con lista interactiva de aplicaciones activas no protegidas y terminación forzosa.

## Fase 10: Launchpad, Mission Control y Spaces
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-16
- **Resultado de Compilación:** Compilación limpia en `target/release/` y `build/` en modo `--build-only`.
- **Resultado de Tests:** 28/28 tests aprobados (100%).
  - Rust Workspace (`cargo test --release --workspace`): 4 suites / 18 tests aprobados.
  - C++ Test Suite (`ctest`): 6 suites / 100% aprobadas (`tst_stagemanager_math` ampliado con tests de Mission Control y App Exposé, `tst_shell_controllers` ampliado a 17 tests unitarios completos).
- **Hitos completados:**
  - **Launchpad a Pantalla Completa (`LaunchpadOverlay.qml` y `LaunchpadController`)**:
    - Desarrollado `LaunchpadController` con escaneo asíncrono de archivos `.desktop`, ordenación alfabética y filtrado reactivo de búsqueda.
    - Cuadrícula paginada de aplicaciones estilo macOS con iconos estilizados, tarjetas translúcidas, animación hover y ejecución segura de programas.
    - Integración de apertura al hacer clic en el icono "Launchpad" del Dock y con atajo global `shortcuts->TriggerLaunchpad()`.
  - **Mission Control y Distribución de Ventanas (`StageManagerEffect`)**:
    - Implementado algoritmo de rejilla sin solapes `calculateMissionControlTransform` y `calculateAppExposeTransform`, respetando márgenes de Spaces superior (120px) y Dock inferior (80px).
    - Verificación matemática en `tst_stagemanager_math` comprobando contención en pantalla e intersección cero entre ventanas.
  - **Spaces y Escritorios Virtuales (`MissionControlController` y `MissionControlOverlay.qml`)**:
    - Desarrollado `MissionControlController` con soporte para creación, eliminación y conmutación reactiva de escritorios virtuales mediante integración con `org.kde.KWin.VirtualDesktopManager`.
    - `MissionControlOverlay.qml`: Barra superior de escritorios virtuales (*Spaces Bar*) con miniaturas interactivas, botón "+", indicador activo con borde acento, y cierre contextual.

## Fase 11: Calidad Visual, Spotlight Avanzado y Robustez
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-16
- **Resultado de Compilación:** Compilación limpia en `target/release/` y `build/` en modo `--build-only`.
- **Resultado de Tests:** 30/30 tests aprobados (100%).
  - Rust Workspace (`cargo test --release --workspace`): 4 suites / 19 tests aprobados (+1 test de fuzzy matching, math evaluator ampliado a expresiones arbitrarias).
  - C++ Test Suite (`ctest`): 6 suites / 100% aprobadas (`tst_shell_controllers` ampliado con comprobación de tipos PDF y multimedia en Quick Look).
- **Hitos completados:**
  - **Desenfoque Gaussiano Real (*Vibrancy*) en Capas Shell**:
    - Detección condicional y enlace de `KF6::WindowSystem` en `src/shell/CMakeLists.txt`.
    - Activación de desenfoque translúcido real detrás de ventanas flotantes (`KWindowEffects::enableBlurBehind`) en `MenuBarWindow`, `DockWindow`, `SpotlightOverlay`, `NotificationBanner`, `ControlCenterPopup`, `LaunchpadOverlay` y `MissionControlOverlay`.
  - **Spotlight Avanzado con Evaluador Matemático Completo y Fuzzy Matching**:
    - Motor de expresiones matemáticas implementado con analizador sintáctico descendente recursivo (`Parser`) soportando precedencia de operadores (`+`, `-`, `*`, `/`, `%`), exponenciación (`^`), agrupaciones por paréntesis, funciones trigonométricas/matemáticas (`sqrt`, `abs`, `sin`, `cos`) y constantes (`pi`, `e`).
    - Algoritmo de correspondencia difusa (*fuzzy matching*) para búsqueda no lineal de aplicaciones y clasificación por orden de relevancia.
    - Método D-Bus `list_apps` expuesto en la interfaz `org.quartz.Spotlight`.
  - **Quick Look Multimedia y Documentos PDF**:
    - `QuickLookModal.qml` ampliado con vistas especializadas para PDF (insignia vectorial, metadatos y botón de apertura externa), Audio (control de reproducción y barra de progreso) y Vídeo.
  - **Modo Oscuro Automático y Robustez de Compositor**:
    - Sincronización automática de modo de apariencia con `QGuiApplication::styleHints()->colorScheme()` en `SettingsConfigController`.
    - Guardas estrictas de punteros nulos y comprobación de integridad en plugins de KWin (`StageManagerKWinPlugin` y `QuartzDecorationPlugin`), garantizando estabilidad sin caídas del compositor.

## Fase 12: Aplicaciones del Sistema
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-16
- **Resultado de Compilación:** Compilación limpia en `target/release/` y `build/` en modo `--build-only`.
- **Resultado de Tests:** 31/31 tests aprobados (100%).
  - Rust Workspace (`cargo test --release --workspace`): 4 suites / 19 tests aprobados.
  - C++ Test Suite (`ctest`): 6 suites / 100% aprobadas.
- **Hitos completados:**
  - **Lanzadores y Metadatos de Aplicaciones del Sistema (`data/applications/`)**:
    - Creados los descriptores XDG `.desktop` nativos para las aplicaciones base: Terminal (`quartz-terminal.desktop`), Vista Previa (`quartz-preview.desktop`), TextEdit (`quartz-textedit.desktop`), Monitor de Actividad (`quartz-activitymonitor.desktop`), Finder (`quartz-finder.desktop`) y Ajustes del Sistema (`quartz-settings.desktop`).
  - **Perfil y Esquema de Color para Terminal (`data/terminal/`)**:
    - Desarrollados `Quartz.profile` y `Quartz.colorscheme` estilo macOS Pro (fondo `#1e1e1e`, opacidad 0.95, soporte de desenfoque gaussiano, cursor beam parpadeante y paleta ANSI vibrante).
    - Despliegue automático en `${INSTALL_PREFIX}/share/konsole/` en `scripts/build_and_install.sh`.

## Fase 13: Servicios del Sistema
- **Estado:** COMPLETADA (100%)
- **Fecha:** 2026-10-16
- **Resultado de Compilación:** Compilación limpia en `target/release/` y `build/` en modo `--build-only`.
- **Resultado de Tests:** 31/31 tests aprobados (100%).
  - Rust Workspace (`cargo test --release --workspace`): 4 suites / 19 tests aprobados.
  - C++ Test Suite (`ctest`): 6 suites / 100% aprobadas (`tst_shell_controllers` ampliado con `testQuartzSystemServices` cubriendo Time Machine, Llavero y CUPS).
- **Hitos completados:**
  - **Time Machine (`TimeMachineService`)**:
    - Servicio D-Bus `org.quartz.TimeMachine` en `/org/quartz/TimeMachine` para gestión de copias de seguridad incrementales (compatible con `restic` y snapshots de Btrfs). Métodos `startBackup`, `stopBackup`, `refreshSnapshots`, con emisión reactiva de progreso y timestamps de snapshot.
  - **Llavero y Contraseñas (`KeychainService`)**:
    - Servicio D-Bus `org.quartz.Keychain` en `/org/quartz/Keychain` para almacenamiento, consulta, eliminación y listado seguro de credenciales con aislamiento por servicio y cuenta.
  - **Servicio de Impresoras (`PrintService`)**:
    - Servicio D-Bus `org.quartz.PrintService` en `/org/quartz/PrintService` con consulta y monitorización de colas de impresión de CUPS (`lpstat`), detección de impresora predeterminada, cancelación de trabajos y degradación elegante ("Imprimir en PDF") ante ausencia de hardware.








