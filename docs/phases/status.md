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
