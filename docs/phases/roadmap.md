# Roadmap - QuartzDE (WhiteQuartz)

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

## Fase 4: Integración Real en el Compositor KWin (`KDecoration2`/`KDecoration3`, `KWin::Effect` y Shaders GLSL)
- [x] Plugin de Decoración de Ventanas `QuartzDecoration` (`KDecoration3::Decoration` con botones Aqua, sombras dinámicas y semáforo)
- [x] Plugin de Efectos `StageManagerEffect` (`KWin::OffscreenEffect`) y carga de shader superelíptico `squircle.frag`
- [x] Registro del objeto D-Bus `/org/quartz/Compositor/StageManager` en KWin para control reactivo externo

## Fase 5: Gestión de Ventanas Wayland, Menú Global (`DBusMenu`) y TaskManager del Dock
- [x] Seguimiento de ventanas activas en `DockController` y `MenuBarController` (`KF6::WindowSystem` / puente Wayland)
- [x] Servidor `com.canonical.AppMenu.Registrar` e importador `DBusMenu` en `MenuBarController` con menús dinámicos en QML
- [x] Stacks del Dock (`Descargas` en cuadrícula emergente) y estado reactivo de papelera (`user-trash` vs `user-trash-full`)

## Fase 6: Paridad Funcional 1:1 en `quartz-finder` y Estabilidad de Configuración
- [x] **0.2.2**: Estabilidad de configuración en `libquartz`: `#[serde(default)]` en structs, respaldo `config.json.bad` ante JSON corrupto, campo `wallpaper` y test de migración
- [x] **6.0**: Preparación del modelo asíncrono en `FinderColumnView` (`QFileSystemModel` + `QSortFilterProxyModel`) con vista previa en segundo plano
- [x] **6.1**: Barra inferior de ruta (*Path Bar*) con migas de pan interactivas y cálculo de espacio libre (`QStorageInfo`)
- [x] **6.2**: Motor de operaciones de archivos `FileOperations` (nueva carpeta `Ctrl+Shift+N`, cortar/copiar/pegar `Ctrl+X/C/V`, mover aquí `Ctrl+Alt+V`, duplicar `Ctrl+D`, renombrar `Return/F2`, papelera `Delete/Ctrl+Backspace`, deshacer `Ctrl+Z`, copia asíncrona sin symlinks y preservación de xattr)
- [x] **6.3**: Pestañas independientes (`Ctrl+T`, `Ctrl+W`, `Ctrl+Tab`), etiquetas de color con `user.xdg.tags` y montaje/expulsión de volúmenes con UDisks2/QStorageInfo

## Fase 7: Subsistema de Hardware, Control Center, Notificaciones y `quartz-settings`
- [x] **7.1**: Servidor de notificaciones `org.freedesktop.Notifications` 1.2, banners flotantes en capa LayerOverlay y panel lateral `NotificationCenter.qml`
- [x] **7.2**: Control Center (`ControlCenterPopup.qml`) con audio (PulseAudio/PipeWire), brillo (`login1`), Wi-Fi, Bluetooth, batería y OSD
- [x] **7.3**: Carga dinámica de los 18 paneles en `quartz-settings` con persistencia reactiva en `QuartzDesktopConfig`

## Fase 8: Atajos Globales, Bloqueo de Pantalla, Login (SDDM) y Gestión de Energía
- [x] **8.1**: Registro de atajos globales de sesión con `KF6::GlobalAccel` y atajos de compositor (`DesktopShortcuts`)
- [x] **8.2**: Pantalla de bloqueo Look-and-Feel para `kscreenlocker` (`data/lookandfeel/org.quartz.desktop/`) y D-Bus lock
- [x] **8.3**: Tema QML para inicio de sesión en SDDM (`data/sddm/themes/quartz/`)
- [x] **8.4**: Menú Apple y acciones de energía (reposo, reinicio, apagado con cuenta atrás de 60s y logind inhibitors)

## Fase 9: Dock, Barra de Menús y Escritorio Avanzados
- [x] **9.0**: Verificación de privilegios Wayland de KWin (`X-KDE-Wayland-Interfaces`) para el seguimiento de ventanas
- [x] **9.1**: Menú contextual de apps del Dock, persistencia de `.desktop` anclados y minimización de ventanas
- [x] **9.2**: Cambiador de aplicaciones modal (`Alt+Tab`) con orden MRU
- [x] **9.3**: Esquinas activas con retardo configurable
- [x] **9.4**: Bandeja del sistema (`StatusNotifierItem` / `StatusNotifierWatcher`)
- [x] **9.5**: Iconos en el escritorio en `WallpaperWindow` con `QFileSystemModel`
- [x] **9.6**: Capturas de pantalla (`Ctrl+Shift+3/4/5`) mediante portales XDG o `ScreenShot2`
- [x] **9.7**: Diálogo de forzar salida (`Ctrl+Alt+Esc`) con lista negra de protección y Acerca de este Mac
- [x] **9.8**: Reloj personalizable y soporte de menú global auditado

## Fase 10: Launchpad, Mission Control y Spaces
- [x] **10.1**: Launchpad a pantalla completa con paginación, arrastrar para carpetas y catálogo unificado `ListApps()`
- [x] **10.2**: Mission Control con algoritmo de reparto sin solapes, App Exposé (`Super+Down`) y Spaces virtuales (`VirtualDesktopManager`)

## Fase 11: Calidad Visual, Spotlight Avanzado y Robustez
- [x] **11.1**: Desenfoque Gaussiano real (`vibrancy`) con `KWindowEffects::enableBlurBehind`
- [x] **11.2**: Spotlight con evaluador de expresiones matemáticas reales, fuzzy matching y consultas cancelables
- [x] **11.3**: Quick Look multimedia y visor PDF (`QtPdf`)
- [x] **11.4**: Modo oscuro y color de acento automático sincronizado con el sistema
- [x] **11.5**: Robustez integral del compositor KWin (guardas `nullptr` y modo seguro sin fallos)

## Fase 12: Aplicaciones del Sistema
- [x] Integración y tematización de Terminal (Konsole), Vista Previa (PDF/imagen), Editor de texto (TextEdit) y Monitor de Actividad

## Fase 13: Servicios del Sistema
- [x] Integración de copias de seguridad (Time Machine sobre restic/btrfs), llavero de contraseñas e interfaz de impresoras CUPS

