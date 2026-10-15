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

## Fase 3: Integración de Subsistema y Funcionalidad Real
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
- [ ] **Bloque 4: Enlace real con KWin**
  - [ ] Adaptación de `QuartzDecoration` a `KDecoration2::Decoration`
  - [ ] Inyección de shaders (`squircle.frag`) en `KWin::GLShader` y pipeline de composición
