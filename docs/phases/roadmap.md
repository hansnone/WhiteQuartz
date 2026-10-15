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
